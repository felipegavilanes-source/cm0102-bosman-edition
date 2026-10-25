/* Experimental Brazilian national league 38-date *runtime* schedule template.
 *
 * Original 3.9.68:
 *   BraNatFirst::create_schedule 0x429b50, and
 *   BraNatSecond::create_schedule 0x42c140
 * allocate exactly 27 * 0x41-byte League round entries (0x6db), then use
 * League::round_set_date 0x68a160 and round_set_match_slot 0x68a1c0.
 *
 * Modern Brazil 2026 requires 38 dates, 20 teams and 2 meetings per pair.
 * Reuse the engine's date records and existing fixture allocator, not a
 * separate competition. Both wrapped functions pass through to the original
 * on every other season, stage and team count.
 *
 * IMPORTANT: strictly OFF by default. Test only with
 * RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT=1 on an isolated save/game install.
 * Existing state cups, Copa do Brasil, Libertadores and Sudamericana are NOT
 * changed, but actual in-game fixture conflict resolution is NOT verified.
 * Candidate dates are not the official club-by-club CBF 2026 fixtures.
 *
 * Do not promote this to a release until full-game fixture, cup conflict,
 * save/reload and multi-season tests have passed.
 */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "brazil_2026_calendar.h"
#include "brazil_2026_schedule.h"
#include "brazil_2026_runtime_gate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define F_MALLOC 0x00944e46u  /* same game malloc as original builders */
#define F_FREE 0x009452cau
#define F_GROUP_DATE 0x0068a160u   /* cdecl, 9 arguments */
#define F_GROUP_SLOT 0x0068a1c0u   /* cdecl, 7 arguments */

#define COMP_YEAR 0x40u
#define COMP_CLUBS 0x3eu
#define COMP_MEETINGS 0x3cu
#define COMP_MATCHES 0xa7u
#define ROUND_BYTES 0x41u
#define NEW_ROUNDS 38
#define NO_WEEKDAY 0xffffffffu

static int readable(uint32_t a)
{
    return a >= 0x10000u && a < 0xfff00000u;
}

static int enabled(void)
{
    const char *v = getenv("RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT");
    return v && !strcmp(v, "1") && season_game_changes();
}

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static void game_free(uint32_t pointer)
{
    if (rt_unwinding || !readable(pointer)) return;
    const uint32_t args[1] = {pointer};
    call_cdecl(F_FREE, 1, args);
}

/* Convert ordinal day (1-based) into the month index (0..11) and calendar
 * day expected by League::round_set_date. No host localtime or TZ calls. */
static int day_and_month(int year, int ordinal, int *day, int *month)
{
    if (!day || !month || ordinal < 1 ||
        ordinal > brazil_cal_days_in_year(year)) return 0;
    for (int m = 1; m <= 12; ++m) {
        int begin = brazil_cal_day(year, m, 1);
        int end = m == 12 ? brazil_cal_days_in_year(year) + 1
                          : brazil_cal_day(year, m + 1, 1);
        if (ordinal >= begin && ordinal < end) {
            *day = ordinal - begin + 1;
            *month = m - 1;
            return 1;
        }
    }
    return 0;
}

/* Build the 38 x 0x41 in-game round records using the same two original
 * engine routines as the 2002 Brazilian and English Premier schedulers.
 * Fail without publishing the pointer or changing league-object fields.
 */
static uint32_t template_38(int edition, int kind)
{
    int anchors[NEW_ROUNDS];
    if (!brazil_2026_candidate_round_dates(edition, kind, anchors))
        return 0;

    const uint32_t bytes = (uint32_t)NEW_ROUNDS * ROUND_BYTES;
    uint32_t alloc_args[1] = {bytes};
    uint32_t table = call_cdecl(F_MALLOC, 1, alloc_args);
    if (rt_unwinding || !readable(table)) return 0;
    memset(g_mem + table, 0, bytes);

    for (int round = 0; round < NEW_ROUNDS && !rt_unwinding; ++round) {
        int day = 0, month = 0;
        if (!day_and_month(edition, anchors[round], &day, &month)) {
            game_free(table);
            return 0;
        }
        uint32_t d[9] = {
            table, (uint32_t)round, (uint32_t)day, (uint32_t)month,
            0u, NO_WEEKDAY, 2u, (uint32_t)edition, 0u
        };
        call_cdecl(F_GROUP_DATE, 9, d);
        if (rt_unwinding) break;

        /* One match slot per match date; the existing League::create_fixtures
         * owns the actual ten club pairings, including the return matches. */
        uint32_t slot[7] = {
            table, (uint32_t)round, 0u, NO_WEEKDAY,
            NO_WEEKDAY, NO_WEEKDAY, 0u
        };
        call_cdecl(F_GROUP_SLOT, 7, slot);
    }
    if (rt_unwinding) return 0;
    return table;
}

static int use_modern(uint32_t self, int stage, int kind)
{
    if (!enabled() || !readable(self)) return 0;
    const char *file = season_change_file(self);
    const char *target = kind == BRA_CAL_SERIE_A ? "bra_nat_first" : "bra_nat_second";
    if (!file || strcmp(file, target)) return 0;
    return brazil_2026_runtime_schedule_allowed(
        file, (int16_t)rd16(g_mem, self + COMP_YEAR),
        (int16_t)rd16(g_mem, self + COMP_CLUBS),
        (int16_t)rd16(g_mem, self + COMP_MEETINGS), stage);
}

/* Both historical Brazilian methods are thiscall (stage, &round_count,
 * &name, &type), ret 0x10. stage==-1 is the league stage, not the knockout.
 * Output layout verified from original instructions at 0x429b67..0x429b95
 * and 0x42c157..0x42c185. Return to original on any failed guard.
 */
static int modern_schedule(uint32_t self, int kind)
{
    const int stage = (int8_t)rt_arg(0);
    if (!use_modern(self, stage, kind)) return 0;
    const int edition = (int16_t)rd16(g_mem, self + COMP_YEAR);
    uint32_t count_ptr = rt_arg(1), name_ptr = rt_arg(2), type_ptr = rt_arg(3);
    if (!readable(count_ptr) || !readable(name_ptr) || !readable(type_ptr))
        return 0;

    uint32_t table = template_38(edition, kind);
    if (rt_unwinding) return 1;
    if (!table) return 0;

    /* Commit live game fields only after all 38 records exist. */
    wr16(g_mem, count_ptr, NEW_ROUNDS);
    wr16(g_mem, name_ptr, 0u);
    wr32(g_mem, type_ptr, 1u);
    wr16(g_mem, self + COMP_MEETINGS, 2u);
    wr16(g_mem, self + COMP_MATCHES, 0xffffu);
    fprintf(stderr, "recomp: Brazil %s %d: experimental 38-date league table built; "
                    "other cups not modified; fixture collisions not yet validated\n",
            kind == BRA_CAL_SERIE_A ? "Serie A" : "Serie B", edition);
    rt_ret_stdcall(table, 4);
    return 1;
}

void o_00429b50(void);
void o_0042c140(void);

/* Brazilian National First Division::create_schedule */
RT_WRAP(00429b50, brazil_serie_a_schedule)
{
    if (modern_schedule(cpu.ecx, BRA_CAL_SERIE_A)) return;
    if (!rt_unwinding) o_00429b50();
}

/* Brazilian National Second Division::create_schedule */
RT_WRAP(0042c140, brazil_serie_b_schedule)
{
    if (modern_schedule(cpu.ecx, BRA_CAL_SERIE_B)) return;
    if (!rt_unwinding) o_0042c140();
}
