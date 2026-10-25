/* Host-only test of the SAME in-engine fixture audit code the game calls.
 * Mock fixture-manager insertion callbacks and Brazilian League game memory.
 *
 * This proves correct audit-counting and stale-state rejection, but does
 * not prove the original 3.9.68 fixture engine creates/inserts 380 games.
 */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "rt.h"

Cpu cpu;
Fpu fpu;
uint8_t *g_mem;
int rt_unwinding;

static const uint32_t OBJ = 0x20000u;
static const uint32_t REC = 0x23000u;
static const uint32_t TABLE = 0x24000u;
static const uint32_t FIXTURE = 0x26000u;
static uint32_t club[20];
static int season_on = 1;
static const char *game_file = "bra_nat_first";

int season_game_changes(void) { return season_on; }
const char *season_change_file(uint32_t obj)
{
    assert(obj == OBJ);
    return game_file;
}

/* Include the runtime implementation to link without the full game. */
#include "../../src/recomp_rt/win/brazil_2026_fixture_audit.c"

static void setup(int year, int meetings, int rounds)
{
    memset(g_mem, 0, 0x80000u);
    wr16(g_mem, OBJ + COMP_YEAR, (uint16_t)year);
    wr16(g_mem, OBJ + COMP_CLUBS, 20);
    wr16(g_mem, OBJ + COMP_MEETINGS, (uint16_t)meetings);
    wr16(g_mem, OBJ + COMP_DATES, (uint16_t)rounds);
    wr32(g_mem, OBJ + COMP_RECORD, REC);
    wr32(g_mem, OBJ + COMP_TABLE, TABLE);
    for (int i = 0; i < 20; ++i) {
        club[i] = 0x50000u + (uint32_t)i * 0x245u;
        wr32(g_mem, TABLE + (uint32_t)i * ROW_SIZE, club[i]);
    }
    wr32(g_mem, FIXTURE + FX_RECORD, REC);
}

static void emit(int h, int a)
{
    wr32(g_mem, FIXTURE + FX_HOME, club[h]);
    wr32(g_mem, FIXTURE + FX_AWAY, club[a]);
    brazil_fixture_audit_observe(FIXTURE);
}

static void test_full(int year)
{
    setup(year, 2, 38);
    game_file = "bra_nat_first";
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ)); /* never before END */
    for (int home = 0; home < 20; ++home)
        for (int away = 0; away < 20; ++away)
            if (home != away)
                emit(home, away);

    /* A cup event in the middle of league creation must not count toward
     * the observed 380 fixtures or overwrite the league's records. */
    wr32(g_mem, FIXTURE + FX_RECORD, REC + 8u);
    emit(0, 1);
    wr32(g_mem, FIXTURE + FX_RECORD, REC);
    brazil_fixture_audit_end(OBJ);
    assert(brazil_fixture_audit_ready(OBJ));

    /* Any following call, even one that does not qualify, invalidates the
     * result and prevents it leaking into the next season/save. */
    game_file = "eng_prm";
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ));
}

static void test_invalid(void)
{
    game_file = "bra_nat_second";
    setup(2026, 2, 38);
    brazil_fixture_audit_begin(OBJ);
    for (int h = 0; h < 20; ++h)
        for (int a = 0; a < 20; ++a)
            if (h != a && !(h == 19 && a == 18)) emit(h, a);
    brazil_fixture_audit_end(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ)); /* only 379 matches */

    setup(2026, 2, 38);
    brazil_fixture_audit_begin(OBJ);
    emit(0, 1); emit(0, 1);
    brazil_fixture_audit_end(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ));

    setup(2026, 1, 38);
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ)); /* 1 meeting */
    setup(2026, 2, 27);
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ)); /* 27-date legacy */
    setup(2002, 2, 38);
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ)); /* historical season */

    setup(2026, 2, 38);
    game_file = "bra_nat_first";
    unsetenv("RECOMP_BRAZIL_2026_FIXTURE_AUDIT");
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ));
    assert(setenv("RECOMP_BRAZIL_2026_FIXTURE_AUDIT", "1", 1) == 0);
    season_on = 0;
    brazil_fixture_audit_begin(OBJ);
    assert(!brazil_fixture_audit_ready(OBJ));
    season_on = 1;
}

int main(void)
{
    g_mem = (uint8_t *)calloc(1, 0x80000u);
    assert(g_mem);
    assert(setenv("RECOMP_BRAZIL_2026_FIXTURE_AUDIT", "1", 1) == 0);
    test_full(2026);
    test_full(2027);
    test_invalid();
    free(g_mem);
    return 0;
}
