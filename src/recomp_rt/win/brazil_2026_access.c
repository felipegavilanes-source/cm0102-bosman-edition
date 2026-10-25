/* Experimental 2026 Série B access stage using the English Championship's
 * EXISTING semi-final round as the game object (Cup_Stage).
 *
 * The four entrants are [6th, 3rd, 5th, 4th], yielding 3rd v 6th and
 * 4th v 5th. Only ONE two-legged round is installed: THERE IS NO FINAL.
 * First leg 21 Nov 2026, second leg 28 Nov 2026.
 *
 * Safety: gated on RECOMP_BRAZIL_2026_ACCESS_EXPERIMENT=1 AND a verified
 * 380-fixture game manager audit. The only stable persistent marker is
 * an original Cup_Stage type 1 with EXACTLY 1 round and 4 entrants. The
 * original save/load mechanism handles Cup_Stage; this branch has NOT been
 * smoke-tested with saves. The English match-result handler promotes the
 * stage's winners (code 1) via status 0 and marks eliminated clubs as
 * status 0xfe. The original Cup::resolve_tie honors flag 0x1000 to make
 * the second team advance on a level two-leg aggregate. We enable this
 * ONLY for the two 2026 Brazilian promotion ties, where the better-ranked
 * side is the second of each seeded pair. Not yet tested in a live game.
 * DO NOT enable this flag for important saves or release this feature.
 *
 * Historical Brazilian games and other competitions run original code.
 */
#include "win.h"
#include "season.h"
#include "season_names.h"
#include "season_change.h"
#include "brazil_2026_access.h"
#include "brazil_2026_access_gate.h"
#include "brazil_2026_fixture_audit.h"
#include "brazil_2026_calendar.h"
#include "brazil_2026_promotion.h"
#include "brazil_2026_promotion_round.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define F_ENG_ROUNDS 0x00571980u
#define F_CUP_STAGE 0x00522e00u
#define F_SET_UP_ROUND 0x0051c800u
#define F_DRAW_DATE 0x00521e60u
#define F_MATCH_DATE 0x00521eb0u
#define F_MALLOC 0x00944e46u
#define F_FREE 0x009452cau
#define F_NEW 0x00944cf1u
#define F_ENGLISH_RESULT 0x00576c50u
#define GAME_DATE 0x00ae2c90u
#define COMP_STAGES 0xcu
#define COMP_STAGE_COUNT 0x2cu
#define COMP_STAGE_NOW 0x30u
#define COMP_MEETINGS 0x3cu
#define COMP_CLUBS 0x3eu
#define COMP_YEAR 0x40u
#define COMP_RECORD 4u
#define COMP_TABLE 0xb1u
#define COMP_DATES 0xa9u
#define UP 0xbeu
#define UP_PLAYOFF 0xbfu
#define DOWN 0xc1u
#define ROW_SIZE 0x3bu
#define STAGE_VTABLE 0x00969468u /* original Cup_Stage vtable */
#define NO_WEEKDAY 0xffffffffu

static int readable(uint32_t a) { return a >= 0x10000u && a < 0xfff00000u; }
static int enabled(void)
{
    const char *e = getenv("RECOMP_BRAZIL_2026_ACCESS_EXPERIMENT");
    return e && !strcmp(e, "1") && season_game_changes();
}
static uint32_t call_cdecl(uint32_t fn, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t res = rt_callback(fn, n, args);
    if (!rt_unwinding) cpu = saved;
    return res;
}
static uint32_t call_this(uint32_t fn, uint32_t self, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t res = rt_callback(fn, n, args);
    if (!rt_unwinding) cpu = saved;
    return res;
}
static uint32_t game_alloc(uint32_t count)
{
    uint32_t args[1] = {count};
    uint32_t ptr = call_cdecl(F_MALLOC, 1, args);
    if (!rt_unwinding && readable(ptr)) memset(g_mem + ptr, 0, count);
    return rt_unwinding ? 0 : ptr;
}
static void game_free(uint32_t ptr)
{
    if (!readable(ptr) || rt_unwinding) return;
    uint32_t args[1] = {ptr};
    call_cdecl(F_FREE, 1, args);
}

static BrazilAccessState snapshot(uint32_t league)
{
    BrazilAccessState s = {0};
    s.edition = (int16_t)rd16(g_mem, league + COMP_YEAR);
    s.clubs = (int16_t)rd16(g_mem, league + COMP_CLUBS);
    s.meetings = (int16_t)rd16(g_mem, league + COMP_MEETINGS);
    s.dates = (int16_t)rd16(g_mem, league + COMP_DATES);
    s.stages = (int32_t)rd32(g_mem, league + COMP_STAGE_COUNT);
    s.current = (int32_t)rd32(g_mem, league + COMP_STAGE_NOW);
    s.direct = (int8_t)g_mem[league + UP];
    s.playoff = (int8_t)g_mem[league + UP_PLAYOFF];
    s.relegations = (int8_t)g_mem[league + DOWN];
    return s;
}

/* Mark the original one-stage Brazilian class as a four-team promotion
 * playoff only when all 380 national fixtures were accepted by the
 * original fixture manager. No Cup_Stage is created at this point. */
void brazil_2026_access_after_fixtures(uint32_t league)
{
    if (!enabled() || !readable(league) ||
        !brazil_fixture_audit_ready(league)) return;
    const char *file = season_change_file(league);
    if (!file || strcmp(file, "bra_nat_second")) return;
    BrazilAccessState s = snapshot(league);
    if (s.edition != 2026 || s.clubs != 20 || s.meetings != 2 ||
        s.dates != 38 || s.stages != 1 || s.current != -1 ||
        s.relegations != 4 || !readable(rd32(g_mem, league + COMP_STAGES)))
        return;
    /* Preserve the original stage array, already allocated for one stage
     * by BraNatSecond::BraNatSecond. Set the direct and playoff zones for
     * the game's normal sort/status update after each result. */
    g_mem[league + UP] = 2;
    g_mem[league + UP_PLAYOFF] = 4;
    fprintf(stderr, "recomp: Serie B 2026: experimental two-direct and two-tie playoff zones configured\n");
}

/* A stage loaded from disk is recognized by its original Cup_Stage
 * structure (the ctor at 0x522e00 writes +0x3c=round count,
 * +0x3e=club count, +0x4=competition record, vtable 0x969468).
 * The old Brazilian championship stage had 8 participants, and will NOT
 * pass this saved-stage identity gate. */
static int own_stage(uint32_t league)
{
    uint32_t array = rd32(g_mem, league + COMP_STAGES);
    if (!readable(array)) return 0;
    uint32_t stage = rd32(g_mem, array);
    if (!readable(stage)) return 0;
    return rd32(g_mem, stage) == STAGE_VTABLE &&
           (int16_t)rd16(g_mem, stage + 0x3c) == 1 &&
           (int16_t)rd16(g_mem, stage + 0x3e) == 4 &&
           rd32(g_mem, stage + 4) == rd32(g_mem, league + COMP_RECORD);
}

static int set_first_and_second_leg(uint32_t round_table, int edition,
                                    const int ordered_ranks[4])
{
    int now_year = (int16_t)rd16(g_mem, GAME_DATE + 2);
    int now_day = (int16_t)rd16(g_mem, GAME_DATE);
    const int leg1 = brazil_cal_day(edition, 11, 21);
    const int draw = brazil_cal_day(edition, 11, 19);
    if (now_year != edition || now_day + 1 > draw ||
        leg1 <= 0 || !readable(round_table)) return 0;

    /* The original 2002 binary's tie resolver honors bit 0x1000 before
     * away goals and penalties, advancing the SECOND side on a tied
     * aggregate. We seed the 3rd and 4th placed clubs second in their
     * respective pairs. Reject unexpected English round formats to avoid
     * silently applying the wrong tiebreak to a modified executable. */
    uint16_t cbf_flags = 0;
    if (!brazil_2026_access_cbf_flags(
            rd16(g_mem, round_table + 0x0du),
            (int)g_mem[round_table + 0x21u],
            ordered_ranks, &cbf_flags))
        return 0;

    /* draw: Nov 19; two-legged match: Nov 21, Nov 28.
     * English semi-final round has fixed pairs (0x200), two legs at +0x21
     * and leg separation at +0x22; change only latter to 7 days. */
    uint32_t d[7] = {round_table, 0, 19, 10, 0, NO_WEEKDAY, (uint32_t)edition};
    uint32_t m[9] = {round_table, 0, 21, 10, 0, NO_WEEKDAY, 1, (uint32_t)edition, 0};
    call_cdecl(F_DRAW_DATE, 7, d);
    if (rt_unwinding) return 0;
    call_cdecl(F_MATCH_DATE, 9, m);
    if (rt_unwinding) return 0;
    g_mem[round_table + 0x21] = 2;
    g_mem[round_table + 0x22] = 7;
    wr16(g_mem, round_table + 0x0du, cbf_flags);
    return 1;
}

/* Builds the one-round stage in the SAME stage array the original owns.
 * Fail closed before writing +0x30: the save loader can read stages[0]
 * only once current stage >= 0, exactly as season_playoffs.c. */
static int build_access_stage(uint32_t league)
{
    const char *file = season_change_file(league);
    if (!file || strcmp(file, "bra_nat_second") ||
        !enabled() || !readable(league)) return 0;
    BrazilAccessState s = snapshot(league);
    if (!brazil_2026_access_gate(file, &s, 1)) return 0;
    uint32_t array = rd32(g_mem, league + COMP_STAGES);
    uint32_t table_rows = rd32(g_mem, league + COMP_TABLE);
    if (!readable(array) || !readable(table_rows) ||
        !readable(rd32(g_mem, league + COMP_RECORD))) return 0;
    if (rd32(g_mem, array) != 0) return 0;

    /* Only use the real rank order once 38 rounds have completed.
     * Original row markers must be 5,0 then four 1 (playoff places).
     */
    uint32_t clubs[4];
    int order[4];
    if (!brazil_2026_promotion_english_slots(order, 4)) return 0;
    unsigned char *mem = g_mem + table_rows;
    if (mem[0 * ROW_SIZE + 0x37] != 5 ||
        mem[1 * ROW_SIZE + 0x37] != 0) return 0;
    for (int i = 2; i <= 5; ++i)
        if (mem[i * ROW_SIZE + 0x37] != 1) return 0;
    for (int i = 0; i < 4; ++i) {
        clubs[i] = rd32(g_mem, table_rows + (uint32_t)order[i] * ROW_SIZE);
        if (!readable(clubs[i])) return 0;
    }

    /* Use the exact English stage-0 semi-final round layout; stage 1
     * (the English final) is intentionally omitted. */
    uint32_t output = game_alloc(8);
    if (!output) return 0;
    uint32_t a[4] = {0, output, output + 4, 0};
    uint32_t rounds_table = call_this(F_ENG_ROUNDS, league, 4, a);
    int rounds = (int16_t)rd16(g_mem, output);
    game_free(output);
    if (rt_unwinding) return 1;
    if (!readable(rounds_table) || rounds != 2) {
        game_free(rounds_table);
        return 0;
    }
    if (!set_first_and_second_leg(rounds_table, s.edition, order)) {
        game_free(rounds_table);
        return 0;
    }

    uint32_t list = game_alloc(16);
    if (!list) { game_free(rounds_table); return 0; }
    for (int i = 0; i < 4; ++i)
        wr32(g_mem, list + (uint32_t)i * 4u, clubs[i]);
    uint32_t bytes[1] = {0xb2};
    uint32_t stage = call_cdecl(F_NEW, 1, bytes);
    if (rt_unwinding) return 1; /* never resume an unwinding game frame */
    if (!readable(stage)) {
        game_free(list);
        game_free(rounds_table);
        return 0; /* original stage remains available as fallback */
    }
    uint32_t args[15] = {
        league, 4, list, 1, rd32(g_mem, league + COMP_RECORD),
        rounds_table, (uint32_t)s.edition, 0, 1, 0xa0, 0x14, 0, 0, 0, 0
    };
    call_this(F_CUP_STAGE, stage, 15, args);
    if (rt_unwinding) return 1;
    wr32(g_mem, array, stage);
    game_free(list);
    game_free(rounds_table);
    uint32_t first[1] = {0};
    if (!rt_unwinding) call_this(F_SET_UP_ROUND, stage, 1, first);
    if (rt_unwinding) return 1;
    wr32(g_mem, league + COMP_STAGE_NOW, 0);
    fprintf(stderr, "recomp: Serie B 2026: ONE-round promotion stage drawn; "
                    "two two-legged ties 21/28 Nov, NO FINAL "
                    "(experimental; higher-placed club advances on level aggregate)\n");
    return 1;
}

/* Original BraNatSecond::advance_stage (thiscall, no arguments).
 * In historical games it creates its own eight-team title playoff. */
void o_0042cdd0(void);
RT_WRAP(0042cdd0, brazil_serie_b_access_advance)
{
    if (build_access_stage(cpu.ecx)) return;
    if (!rt_unwinding) o_0042cdd0();
}

/* English First Division's existing result handler knows the base
 * League table status logic: code 1 means promoted (status 0), code 2
 * eliminated (status 0xfe). With only one stage round, a correctly
 * selected winner of EACH tie should be promoted rather than sending
 * just one finalist up. The single round uses the game's 0x1000
 * "second side wins level aggregate" flag (Cup::resolve_tie 0x51f675),
 * with the higher-ranked entrant seeded second in each fixed pair.
 * This remains a diagnostic opt-in until end-to-end gameplay proves it.
 *
 * The identity check of the loaded original Cup_Stage makes the result
 * routing resilient to save/load without separate C global state.
 */
void o_0042d040(void);
RT_WRAP(0042d040, brazil_serie_b_access_result)
{
    uint32_t league = cpu.ecx;
    if (enabled() && readable(league)) {
        const char *file = season_change_file(league);
        BrazilAccessState s = snapshot(league);
        int stage_id = (int8_t)rt_arg(2);
        if (brazil_2026_access_result_gate(file, &s, stage_id) && own_stage(league)) {
            rt_jump(F_ENGLISH_RESULT);
            return;
        }
    }
    o_0042d040();
}
