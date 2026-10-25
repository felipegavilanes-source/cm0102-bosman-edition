/* Host-only mock of the original League::create_schedule callback interface.
 *
 * Compiles the SAME season_bra_calendar.c the game uses and intercepts
 * game malloc and round_set_date/match_slot callbacks. Checks that all 38
 * date records are complete and that the original 2002 class is preserved.
 * This is NOT a full original-game smoke test or a fixture-creation test.
 *
 * cc -D_POSIX_C_SOURCE=200809L -std=c11 -Wall -Wextra -Werror -pedantic \
 *   -Isrc/recomp_rt -Isrc/recomp_rt/win \
 *   tests/seasons/brazil_2026_runtime_template_test.c -o /tmp/br-calendar-runtime
 * /tmp/br-calendar-runtime
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rt.h"

Cpu cpu;
Fpu fpu;
uint8_t *g_mem;
int rt_unwinding;

static const uint32_t OBJ = 0x20000u;
static const uint32_t STACK = 0x18000u;
static const uint32_t COUNT = 0x21000u;
static const uint32_t NAME = 0x21010u;
static const uint32_t KIND = 0x21020u;
static uint32_t next_alloc = 0x30000u;
static const char *test_file;
static int season_on;
static int fallback_a, fallback_b;
static int allocations, frees, dates_seen, slots_seen;
static int recorded_day[38], recorded_month[38], recorded_year[38];
static int visited_dates[38], visited_slots[38];

int season_game_changes(void) { return season_on; }
const char *season_change_file(uint32_t obj)
{
    assert(obj == OBJ);
    return test_file;
}
uint32_t rt_callback(uint32_t target, int nargs, const uint32_t *args)
{
    switch (target) {
    case 0x00944e46u: /* original game malloc */
        assert(nargs == 1);
        assert(args[0] == 38u * 0x41u);
        ++allocations;
        next_alloc += 0x1000u;
        assert(next_alloc + args[0] < 0x80000u);
        return next_alloc;
    case 0x009452cau: /* original game free */
        assert(nargs == 1);
        ++frees;
        return 0;
    case 0x0068a160u: { /* original league round_set_date */
        assert(nargs == 9);
        uint32_t table = args[0];
        int round = (int)args[1];
        assert(table == next_alloc && round >= 0 && round < 38);
        assert(!visited_dates[round]);
        visited_dates[round] = 1;
        assert(args[4] == 0 && args[6] == 2 && args[7] == 2026 && args[8] == 0);
        int day = (int)args[2], month = (int)args[3];
        assert(day >= 1 && day <= 31 && month >= 0 && month < 12);
        recorded_day[round] = day;
        recorded_month[round] = month;
        recorded_year[round] = (int)args[7];
        ++dates_seen;
        uint32_t entry = table + (uint32_t)round * 0x41u;
        wr16(g_mem, entry + 0, (uint16_t)(1 + round));
        wr16(g_mem, entry + 2, 0);
        wr32(g_mem, entry + 0x3d, args[7]);
        return 0;
    }
    case 0x0068a1c0u: { /* original league round_set_match_slot */
        assert(nargs == 7);
        assert(args[0] == next_alloc && args[1] < 38 && args[2] == 0);
        assert(args[3] == 0xffffffffu && args[4] == 0xffffffffu &&
               args[5] == 0xffffffffu && args[6] == 0);
        int round = (int)args[1];
        assert(!visited_slots[round]);
        visited_slots[round] = 1;
        ++slots_seen;
        return 0;
    }
    default:
        fprintf(stderr, "unexpected original callback: %#x\n", target);
        abort();
    }
}

/* Compile the exact same wrapped in-engine implementation as the game. */
#include "../../src/recomp_rt/win/season_bra_calendar.c"

void o_00429b50(void)
{
    ++fallback_a;
    /* Simulate the original ret 0x10 with a sentinel. */
    rt_ret_stdcall(0xabcdef01u, 4);
}
void o_0042c140(void)
{
    ++fallback_b;
    rt_ret_stdcall(0xabcdef02u, 4);
}

static void clear_state(const char *file, int edition, int clubs, int meetings, int stage)
{
    memset(g_mem, 0, 0x80000);
    cpu = (Cpu){0};
    cpu.ecx = OBJ;
    cpu.esp = STACK;
    wr32(g_mem, STACK, 0x77777777u); /* fake return address */
    wr32(g_mem, STACK + 4, (uint32_t)stage);
    wr32(g_mem, STACK + 8, COUNT);
    wr32(g_mem, STACK + 12, NAME);
    wr32(g_mem, STACK + 16, KIND);
    wr16(g_mem, OBJ + COMP_YEAR, (uint16_t)edition);
    wr16(g_mem, OBJ + COMP_CLUBS, (uint16_t)clubs);
    wr16(g_mem, OBJ + COMP_MEETINGS, (uint16_t)meetings);
    wr16(g_mem, OBJ + COMP_MATCHES, 19u);
    wr16(g_mem, COUNT, 0x1234u);
    wr16(g_mem, NAME, 0x2345u);
    wr32(g_mem, KIND, 0x3456789au);
    test_file = file;
    season_on = 1;
    fallback_a = fallback_b = allocations = frees = dates_seen = slots_seen = 0;
    memset(visited_dates, 0, sizeof visited_dates);
    memset(visited_slots, 0, sizeof visited_slots);
}

static void test_modern(int serie_b)
{
    clear_state(serie_b ? "bra_nat_second" : "bra_nat_first", 2026, 20, 1, -1);
    if (serie_b) f_0042c140(); else f_00429b50();
    assert(fallback_a == 0 && fallback_b == 0);
    assert(allocations == 1 && frees == 0);
    assert(dates_seen == 38 && slots_seen == 38);
    assert(cpu.eax == next_alloc && cpu.esp == STACK + 20u);
    assert(rd16(g_mem, COUNT) == 38 && rd16(g_mem, NAME) == 0);
    assert(rd32(g_mem, KIND) == 1u);
    assert(rd16(g_mem, OBJ + COMP_MEETINGS) == 2);
    assert(rd16(g_mem, OBJ + COMP_MATCHES) == 0xffffu);
    int anchors[38];
    assert(brazil_2026_candidate_round_dates(
        2026, serie_b ? BRA_CAL_SERIE_B : BRA_CAL_SERIE_A, anchors) == 38);
    for (int i = 0; i < 38; ++i) {
        assert(visited_dates[i] && visited_slots[i]);
        assert(recorded_year[i] == 2026);
        assert(brazil_cal_day(2026, recorded_month[i] + 1, recorded_day[i]) == anchors[i]);
        assert(rd32(g_mem, next_alloc + (uint32_t)i * 0x41u + 0x3du) == 2026u);
    }
}

static void test_historical_fallback(void)
{
    clear_state("bra_nat_first", 2002, 20, 1, -1);
    f_00429b50();
    assert(fallback_a == 1 && allocations == 0 && cpu.eax == 0xabcdef01u);
    assert(rd16(g_mem, COUNT) == 0x1234u);
    assert(rd16(g_mem, OBJ + COMP_MEETINGS) == 1u);
    clear_state("bra_nat_first", 2026, 28, 1, -1);
    f_00429b50();
    assert(fallback_a == 1 && allocations == 0);
    clear_state("bra_nat_first", 2026, 20, 1, 0);
    f_00429b50();
    assert(fallback_a == 1 && allocations == 0);
    clear_state("bra_nat_first", 2026, 20, 1, -1);
    season_on = 0;
    f_00429b50();
    assert(fallback_a == 1 && allocations == 0);
    clear_state("bra_nat_second", 2002, 20, 1, -1);
    f_0042c140();
    assert(fallback_b == 1 && allocations == 0);
    clear_state("bra_cup", 2026, 20, 1, -1);
    f_00429b50();
    assert(fallback_a == 1 && allocations == 0);
}

int main(void)
{
    g_mem = (uint8_t *)calloc(1, 0x80000u);
    assert(g_mem);
    assert(setenv("RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT", "1", 1) == 0);
    test_modern(0);
    test_modern(1);
    test_historical_fallback();
    unsetenv("RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT");
    clear_state("bra_nat_first", 2026, 20, 1, -1);
    f_00429b50();
    assert(fallback_a == 1 && allocations == 0);
    free(g_mem);
    return 0;
}
