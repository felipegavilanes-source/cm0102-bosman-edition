/* Observes the ORIGINAL engine's fixture-add calls for a modern Brazil
 * league while League::create_fixtures runs. Off by default. The audit
 * becomes active when either:
 *   RECOMP_BRAZIL_2026_FIXTURE_AUDIT=1
 *   RECOMP_BRAZIL_2026_STAGE_EXPERIMENT=1 (extra promotion/title guard)
 *
 * It does not intercept match results, change fixtures, or write to game
 * memory. It checks home/away pairs only AFTER the original fixture manager
 * returns success. A successful insertion is still not proof of safe
 * save/load, or that no other competition may later move the match. Use a real-game smoke
 * test for that stronger claim.
 */
#include "win.h"
#include "season.h"
#include "season_names.h"
#include "season_change.h"
#include "brazil_2026_fixture_audit.h"
#include "brazil_2026_fixture_tracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_YEAR 0x40u
#define COMP_CLUBS 0x3eu
#define COMP_MEETINGS 0x3cu
#define COMP_DATES 0xa9u
#define COMP_RECORD 4u
#define COMP_TABLE 0xb1u
#define ROW_SIZE 0x3bu
#define FX_RECORD 0x14u
#define FX_HOME 0x1cu
#define FX_AWAY 0x20u

typedef struct BrazilLiveAudit {
    BrazilFixtureTracker tracker;
    uint32_t league, record;
    int active;
} BrazilLiveAudit;

static BrazilLiveAudit current;
static uint32_t last_league;
static int last_ok, last_seen;

static int readable(uint32_t addr)
{
    return addr >= 0x10000u && addr < 0xfff00000u;
}
static int flag(const char *name)
{
    const char *value = getenv(name);
    return value && strcmp(value, "1") == 0;
}
static int audit_on(void)
{
    return season_game_changes() &&
        (flag("RECOMP_BRAZIL_2026_FIXTURE_AUDIT") ||
         flag("RECOMP_BRAZIL_2026_STAGE_EXPERIMENT"));
}

void brazil_fixture_audit_begin(uint32_t league)
{
    /* Nested create_fixtures calls from other cups must not clear an active
     * national-league ledger. Their record pointers do not match anyway. */
    if (current.active) return;
    /* An object address can be reused after loading a save or starting
     * another game. Never trust a result from an earlier fixture pass. */
    last_league = 0;
    last_ok = 0;
    last_seen = 0;
    if (!audit_on() || !readable(league)) return;
    const char *file = season_change_file(league);
    if (!file ||
        (strcmp(file, "bra_nat_first") && strcmp(file, "bra_nat_second")))
        return;
    int edition = (int16_t)rd16(g_mem, league + COMP_YEAR);
    int count = (int16_t)rd16(g_mem, league + COMP_CLUBS);
    int meetings = (int16_t)rd16(g_mem, league + COMP_MEETINGS);
    int rounds = (int16_t)rd16(g_mem, league + COMP_DATES);
    if ((edition != 2026 && edition != 2027) ||
        count != BRA_FIX_CLUBS || meetings != 2 || rounds != 38)
        return;
    uint32_t row_table = rd32(g_mem, league + COMP_TABLE);
    uint32_t record = rd32(g_mem, league + COMP_RECORD);
    if (!readable(row_table) || !readable(record)) return;
    uint32_t clubs[BRA_FIX_CLUBS];
    for (int i = 0; i < BRA_FIX_CLUBS; ++i)
        clubs[i] = rd32(g_mem, row_table + (uint32_t)i * ROW_SIZE);
    BrazilFixtureTracker tracker;
    if (!brazil_fixture_tracker_begin(&tracker, clubs)) return;

    current.tracker = tracker;
    current.league = league;
    current.record = record;
    current.active = 1;
    last_league = league;
    last_seen = 0;
    last_ok = 0;
}

void brazil_fixture_audit_observe(uint32_t fixture)
{
    if (!current.active || !readable(fixture)) return;
    if (rd32(g_mem, fixture + FX_RECORD) != current.record) return;
    uint32_t home = rd32(g_mem, fixture + FX_HOME);
    uint32_t away = rd32(g_mem, fixture + FX_AWAY);
    brazil_fixture_tracker_add(&current.tracker, home, away);
}

void brazil_fixture_audit_end(uint32_t league)
{
    if (!current.active || current.league != league) return;
    last_league = league;
    last_seen = current.tracker.count;
    last_ok = brazil_fixture_tracker_complete(&current.tracker);
    if (flag("RECOMP_BRAZIL_2026_FIXTURE_AUDIT") ||
        flag("RECOMP_BRAZIL_2026_STAGE_EXPERIMENT")) {
        const char *file = season_change_file(league);
        fprintf(stderr,
                "recomp: Brazil fixture audit %s year=%d inserted=%d/380 "
                "duplicates_or_invalid=%d exact_home_away=%s "
                "(accepted fixture-manager insertions; save/load unverified)\n",
                file ? file : "?", (int16_t)rd16(g_mem, league + COMP_YEAR),
                last_seen, current.tracker.bad, last_ok ? "YES" : "NO");
    }
    current.active = 0;
}

int brazil_fixture_audit_ready(uint32_t league)
{
    return audit_on() && !current.active && league && last_league == league && last_ok;
}
