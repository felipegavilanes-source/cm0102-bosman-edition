/* Host-only mock for the *same* experimental Serie B 2026 one-round
 * Cup_Stage adapter compiled in the game. No original .EXE is executed.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rt.h"
#include "../../src/recomp_rt/win/brazil_2026_promotion_round.h"

Cpu cpu;
Fpu fpu;
uint8_t *g_mem;
int rt_unwinding;
static const uint32_t LEAGUE = 0x20000u;
static const uint32_t ARR = 0x22000u;
static const uint32_t REC = 0x23000u;
static const uint32_t ROWS = 0x24000u;
static const uint32_t STACK = 0x1a000u;
static uint32_t clubs[20];
static const char *current_file = "bra_nat_second";
static int season_on = 1, fixture_ok = 1;
static int fallback_stage, fallback_result, english_result;
static int cup_stages, rounds_created, rounds_set_up;
static int draw_called, match_called;
static int allocated, freed;
static uint32_t fixture_year;
static uint16_t mock_english_flags = 0x204u;

int season_game_changes(void) { return season_on; }
const char *season_change_file(uint32_t obj)
{
    assert(obj == LEAGUE);
    return current_file;
}
int brazil_fixture_audit_ready(uint32_t league)
{
    return league == LEAGUE && fixture_ok;
}
void rt_jump(uint32_t target)
{
    assert(target == 0x00576c50u);
    ++english_result;
    /* Simulate the known 0x576c50 English result-handler effect. The real
     * EXE was disassembled: code 1 -> status 0, code 2 -> status 0xfe.
     * This cannot replace a true save/load and gameplay smoke test. */
    uint32_t played = rt_arg(0);
    int code = (int8_t)rt_arg(1);
    for (int i = 0; i < 20; ++i)
        if (clubs[i] == played) {
            if (code == 1)
                g_mem[ROWS + 0x3bu * (uint32_t)i + 0x37u] = 0;
            else if (code == 2)
                g_mem[ROWS + 0x3bu * (uint32_t)i + 0x37u] = 0xfe;
            return;
        }
    assert(!"Unknown club in English result handler mock");
}
uint32_t rt_callback(uint32_t fn, int nargs, const uint32_t *args)
{
    switch (fn) {
    case 0x00944e46u: /* game malloc */
        assert(nargs == 1);
        ++allocated;
        return 0x30000u + (uint32_t)allocated * 0x1000u;
    case 0x009452cau:
        assert(nargs == 1);
        ++freed;
        return 0;
    case 0x00944cf1u:
        assert(nargs == 1 && args[0] == 0xb2);
        return 0x39000u;
    case 0x00571980u: {
        assert(nargs == 4 && args[0] == 0 && args[3] == 0);
        ++rounds_created;
        wr16(g_mem, args[1], 2);
        wr16(g_mem, args[2], 0);
        uint32_t table = 0x37000u;
        memset(g_mem + table, 0, 0xd0);
        wr16(g_mem, table + 0xd, mock_english_flags); /* English 0x0204 */
        g_mem[table + 0x21] = 2; /* two legs */
        g_mem[table + 0x22] = 4; /* default England gap */
        return table;
    }
    case 0x00521e60u:
        assert(nargs == 7 && args[0] == 0x37000u);
        assert(args[1] == 0 && args[2] == 19 && args[3] == 10);
        assert(args[4] == 0 && args[5] == 0xffffffffu && args[6] == 2026);
        ++draw_called;
        return 0;
    case 0x00521eb0u:
        assert(nargs == 9 && args[0] == 0x37000u);
        assert(args[1] == 0 && args[2] == 21 && args[3] == 10);
        assert(args[4] == 0 && args[5] == 0xffffffffu);
        assert(args[6] == 1 && args[7] == 2026 && args[8] == 0);
        ++match_called;
        return 0;
    case 0x00522e00u: {
        assert(nargs == 15);
        assert(args[0] == LEAGUE && args[1] == 4 && args[3] == 1);
        assert(args[4] == REC && args[5] == 0x37000u);
        assert(args[6] == 2026);
        uint32_t list = args[2];
        int rank[4] = {5, 2, 4, 3};
        for (int i = 0; i < 4; ++i)
            assert(rd32(g_mem, list + 4u * (uint32_t)i) == clubs[rank[i]]);
        assert(g_mem[0x37000u + 0x21u] == 2);
        assert(g_mem[0x37000u + 0x22u] == 7);
        assert(rd16(g_mem, 0x37000u + 0x0du) == 0x1204u);
        assert(brazil_2026_access_level_winner(rd16(g_mem, 0x37000u + 0x0du)) == 2);
        uint32_t stage = cpu.ecx;
        assert(stage == 0x39000u);
        wr32(g_mem, stage, 0x00969468u);
        wr32(g_mem, stage + 4, REC);
        wr16(g_mem, stage + 0x3c, 1);
        wr16(g_mem, stage + 0x3e, 4);
        ++cup_stages;
        return stage;
    }
    case 0x0051c800u:
        assert(nargs == 1 && args[0] == 0 && cpu.ecx == 0x39000u);
        ++rounds_set_up;
        return 0;
    default:
        fprintf(stderr, "unexpected original callback: %08x\n", fn);
        abort();
    }
}

#include "../../src/recomp_rt/win/brazil_2026_access.c"

void o_0042cdd0(void) { ++fallback_stage; }
void o_0042d040(void)
{
    ++fallback_result;
    rt_ret_stdcall(0, 6);
}

static void reset_game(int year)
{
    memset(g_mem, 0, 0xb10000u);
    cpu = (Cpu){0};
    cpu.ecx = LEAGUE;
    cpu.esp = STACK;
    wr32(g_mem, STACK, 0x12345);
    wr32(g_mem, STACK + 4, 0x50000u);
    wr32(g_mem, STACK + 8, 1);
    wr32(g_mem, STACK + 12, 0);
    wr16(g_mem, LEAGUE + COMP_YEAR, (uint16_t)year);
    wr16(g_mem, LEAGUE + COMP_CLUBS, 20);
    wr16(g_mem, LEAGUE + COMP_MEETINGS, 2);
    wr16(g_mem, LEAGUE + COMP_DATES, 38);
    wr32(g_mem, LEAGUE + COMP_STAGE_COUNT, 1);
    wr32(g_mem, LEAGUE + COMP_STAGE_NOW, 0xffffffffu);
    wr32(g_mem, LEAGUE + COMP_STAGES, ARR);
    wr32(g_mem, LEAGUE + COMP_TABLE, ROWS);
    wr32(g_mem, LEAGUE + COMP_RECORD, REC);
    g_mem[LEAGUE + UP] = 4;
    g_mem[LEAGUE + UP_PLAYOFF] = 0;
    g_mem[LEAGUE + DOWN] = 4;
    wr16(g_mem, GAME_DATE, (uint16_t)(brazil_cal_day(year, 11, 14) - 1));
    wr16(g_mem, GAME_DATE + 2, (uint16_t)year);
    for (int i = 0; i < 20; ++i) {
        clubs[i] = 0x50000u + (uint32_t)i * 0x245u;
        wr32(g_mem, ROWS + ROW_SIZE * (uint32_t)i, clubs[i]);
        g_mem[ROWS + ROW_SIZE * (uint32_t)i + 0x37u] =
            i == 0 ? 5 : i == 1 ? 0 : i <= 5 ? 1 : 0xfe;
    }
    current_file = "bra_nat_second";
    season_on = 1;
    fixture_ok = 1;
    fallback_stage = fallback_result = english_result = 0;
    cup_stages = rounds_created = rounds_set_up = 0;
    draw_called = match_called = allocated = freed = 0;
    fixture_year = (uint32_t)year;
    mock_english_flags = 0x204u;
}

static void test_stage(void)
{
    reset_game(2026);
    assert(setenv("RECOMP_BRAZIL_2026_ACCESS_EXPERIMENT", "1", 1) == 0);
    brazil_2026_access_after_fixtures(LEAGUE);
    assert(g_mem[LEAGUE + UP] == 2 && g_mem[LEAGUE + UP_PLAYOFF] == 4);
    f_0042cdd0();
    assert(fallback_stage == 0);
    assert(cup_stages == 1 && rounds_created == 1 && rounds_set_up == 1);
    assert(draw_called == 1 && match_called == 1);
    assert(rd32(g_mem, ARR) == 0x39000u);
    assert(rd32(g_mem, LEAGUE + COMP_STAGE_NOW) == 0);
    assert(rd32(g_mem, LEAGUE + COMP_STAGE_COUNT) == 1);
    assert(own_stage(LEAGUE));

    /* Four result calls from the single two-tie stage: ranks 3 and 5
     * (0-based 2 and 4) win, ranks 4 and 6 (3,5) lose. The English
     * 0x576c50 winner marking must yield 2 automatic + 2 tie winners. */
    wr32(g_mem, STACK + 4, clubs[2]);
    wr32(g_mem, STACK + 8, 1);
    f_0042d040();
    wr32(g_mem, STACK + 4, clubs[5]);
    wr32(g_mem, STACK + 8, 2);
    f_0042d040();
    wr32(g_mem, STACK + 4, clubs[4]);
    wr32(g_mem, STACK + 8, 1);
    f_0042d040();
    wr32(g_mem, STACK + 4, clubs[3]);
    wr32(g_mem, STACK + 8, 2);
    f_0042d040();
    assert(english_result == 4 && fallback_result == 0);
    int total_up = 0;
    for (int i = 0; i < 20; ++i) {
        unsigned char v = g_mem[ROWS + ROW_SIZE * (uint32_t)i + 0x37u];
        total_up += (v == 0 || v == 5);
    }
    assert(total_up == 4);
    /* The inter-division chain sees four relegated rows at 17-20.
     * Confirm the four-up/four-down checker against the actual original
     * League table's noncontiguous status bytes. */
    for (int i = 16; i < 20; ++i)
        g_mem[ROWS + (uint32_t)i * ROW_SIZE + 0x37u] = 3;
    uint8_t status[20];
    for (int i = 0; i < 20; ++i)
        status[i] = g_mem[ROWS + (uint32_t)i * ROW_SIZE + 0x37u];
    assert(brazil_2026_access_promotions_valid(status, 20));
    brazil_2026_access_audit_promotion(LEAGUE); /* read-only */
    assert(g_mem[ROWS + 2 * ROW_SIZE + 0x37u] == 0);
    assert(g_mem[ROWS + 3 * ROW_SIZE + 0x37u] == 0xfe);
    assert(g_mem[ROWS + 4 * ROW_SIZE + 0x37u] == 0);
    assert(g_mem[ROWS + 5 * ROW_SIZE + 0x37u] == 0xfe);

    /* A saved and reloaded Cup_Stage at a new memory address must still
     * route result messages without any process-global stage pointer. */
    memcpy(g_mem + 0x3a000u, g_mem + 0x39000u, 0xb2u);
    wr32(g_mem, ARR, 0x3a000u);
    assert(own_stage(LEAGUE));
    wr32(g_mem, STACK + 4, clubs[2]);
    wr32(g_mem, STACK + 8, 1);
    f_0042d040();
    assert(english_result == 5);

    /* The stage has a saved Cup_Stage vtable and 1x4 identity.
     * Clearing any of those saved fields rejects promotion results. */
    wr16(g_mem, 0x3a000u + 0x3e, 8);
    f_0042d040();
    assert(english_result == 5 && fallback_result == 1);
    wr16(g_mem, 0x3a000u + 0x3e, 4);
    cpu.esp = STACK;
    wr32(g_mem, STACK + 12, 0xffffffffu); /* regular league result */
    f_0042d040();
    assert(english_result == 5 && fallback_result == 2);
}

static void test_reject(void)
{
    reset_game(2002);
    brazil_2026_access_after_fixtures(LEAGUE);
    assert(g_mem[LEAGUE + UP_PLAYOFF] == 0);
    f_0042cdd0();
    assert(fallback_stage == 1 && cup_stages == 0);
    reset_game(2027); /* no invented 2027 access regulations */
    brazil_2026_access_after_fixtures(LEAGUE);
    assert(g_mem[LEAGUE + UP_PLAYOFF] == 0);
    f_0042cdd0();
    assert(fallback_stage == 1);
    reset_game(2026);
    fixture_ok = 0; /* only 379 fixtures accepted */
    brazil_2026_access_after_fixtures(LEAGUE);
    f_0042cdd0();
    assert(fallback_stage == 1 && cup_stages == 0);
    reset_game(2026);
    current_file = "bra_nat_first"; /* NEVER change Serie A */
    brazil_2026_access_after_fixtures(LEAGUE);
    f_0042cdd0();
    assert(fallback_stage == 1 && cup_stages == 0);
    reset_game(2026);
    mock_english_flags = 0x200u; /* modified engine lacks expected flags */
    brazil_2026_access_after_fixtures(LEAGUE);
    f_0042cdd0();
    assert(fallback_stage == 1 && cup_stages == 0);
    assert(rd32(g_mem, LEAGUE + COMP_STAGE_NOW) == 0xffffffffu);
    reset_game(2026);
    unsetenv("RECOMP_BRAZIL_2026_ACCESS_EXPERIMENT");
    brazil_2026_access_after_fixtures(LEAGUE);
    f_0042cdd0();
    assert(fallback_stage == 1 && cup_stages == 0);
}

int main(void)
{
    g_mem = (uint8_t *)calloc(1, 0xb10000u);
    assert(g_mem);
    test_stage();
    test_reject();
    free(g_mem);
    return 0;
}
