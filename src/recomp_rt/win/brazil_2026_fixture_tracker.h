/* Exact directed-pair ledger for a modern 20-club Brazilian league.
 *
 * Every club must play each other club once at home and once away.
 * 20 * 19 = 380 directed fixtures, 38 fixtures per club (19 home/19 away).
 * This is a pure, fixed-size data structure; no game memory is touched.
 *
 * Runtime adapter logs ADD_FIXTURE callback *attempts*, not verified
 * insertions in the game's fixture manager. Do not infer a successful
 * save/load or collision-free 2026 season from this ledger alone.
 */
#ifndef RECOMP_BRAZIL_2026_FIXTURE_TRACKER_H
#define RECOMP_BRAZIL_2026_FIXTURE_TRACKER_H
#include <stdint.h>
#include <string.h>

enum { BRA_FIX_CLUBS = 20, BRA_FIX_EXPECTED = 380 };

typedef struct BrazilFixtureTracker {
    uint32_t club[BRA_FIX_CLUBS];   /* actual game club record pointers */
    unsigned char seen[BRA_FIX_CLUBS][BRA_FIX_CLUBS];
    int home[BRA_FIX_CLUBS], away[BRA_FIX_CLUBS];
    int count, bad;
} BrazilFixtureTracker;

static inline int brazil_fixture_tracker_begin(BrazilFixtureTracker *t,
                                                const uint32_t club[20])
{
    if (!t || !club) return 0;
    for (int i = 0; i < BRA_FIX_CLUBS; ++i) {
        if (club[i] == 0) return 0;
        for (int j = 0; j < i; ++j)
            if (club[i] == club[j]) return 0;
    }
    memset(t, 0, sizeof *t);
    memcpy(t->club, club, sizeof t->club);
    return 1;
}

/* Add one observed call to the original fixture manager. Fail closed on
 * unknown clubs, duplicate fixtures, or a self-match. A failed add poisons
 * the ledger so a false "complete" result is impossible.
 */
static inline int brazil_fixture_tracker_add(BrazilFixtureTracker *t,
                                              uint32_t home, uint32_t away)
{
    if (!t) return 0;
    int h = -1, a = -1;
    for (int i = 0; i < BRA_FIX_CLUBS; ++i) {
        if (t->club[i] == home) h = i;
        if (t->club[i] == away) a = i;
    }
    if (t->bad || h < 0 || a < 0 || h == a ||
        t->seen[h][a] || t->count >= BRA_FIX_EXPECTED) {
        t->bad = 1;
        return 0;
    }
    t->seen[h][a] = 1;
    ++t->home[h];
    ++t->away[a];
    ++t->count;
    return 1;
}

static inline int brazil_fixture_tracker_complete(const BrazilFixtureTracker *t)
{
    if (!t || t->bad || t->count != BRA_FIX_EXPECTED) return 0;
    for (int i = 0; i < BRA_FIX_CLUBS; ++i) {
        if (t->home[i] != 19 || t->away[i] != 19) return 0;
        for (int j = 0; j < BRA_FIX_CLUBS; ++j)
            if (i != j && t->seen[i][j] != 1) return 0;
    }
    return 1;
}
#endif
