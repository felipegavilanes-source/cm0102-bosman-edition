/* Modern Brazilian national leagues, modeled on the English Premier League's
 * 20-club double round robin. Standalone rule and fixture validation helpers.
 *
 * Not a replacement for BraNatFirst/BraNatSecond::create_schedule or their
 * historical knockout builders: those need separately reviewed runtime hooks.
 * In particular, these round indices are NOT the actual CBF 2026 match dates.
 * Keeping the module free of g_mem allows deterministic native tests.
 */
#ifndef RECOMP_BRAZIL_2026_LEAGUE_H
#define RECOMP_BRAZIL_2026_LEAGUE_H

#include <string.h>

enum {
    BRAZIL_2026_CLUBS = 20,
    BRAZIL_2026_MEETINGS = 2,
    BRAZIL_2026_ROUNDS = 38,
    BRAZIL_2026_MATCHES_PER_ROUND = 10,
    BRAZIL_2026_FIXTURES = 380
};

typedef struct Brazil2026LeagueProfile {
    int clubs;
    int meetings_per_pair;
    int rounds;
    int fixtures;
    int title_playoff_rounds;
    int direct_promotions;
    int promotion_playoff_winners;
    int relegations;
} Brazil2026LeagueProfile;

typedef struct Brazil2026Fixture {
    int round;     /* zero-based: 0 through 37 */
    int home;      /* zero-based club slot: 0 through 19 */
    int away;
} Brazil2026Fixture;

/* The season data calls calendar year 2026 "2025/26". The caller must supply
 * the REAL calendar year, not the database's season-key year (2025).
 * Future seasons retain the most recent known regulation until superseded.
 * Never apply this to the base game's historical 2001/02 season.
 */
static inline int brazil_2026_league_profile(const char *file, int real_year,
                                             Brazil2026LeagueProfile *out)
{
    if (!file || !out || real_year < 2026) return 0;
    if (strcmp(file, "bra_nat_first") && strcmp(file, "bra_nat_second"))
        return 0;

    Brazil2026LeagueProfile p;
    p.clubs = BRAZIL_2026_CLUBS;
    p.meetings_per_pair = BRAZIL_2026_MEETINGS;
    p.rounds = BRAZIL_2026_ROUNDS;
    p.fixtures = BRAZIL_2026_FIXTURES;
    p.title_playoff_rounds = 0; /* crucial: Serie A champion by table */
    p.direct_promotions = 0;
    p.promotion_playoff_winners = 0;
    p.relegations = 4;
    if (!strcmp(file, "bra_nat_second")) {
        p.direct_promotions = 2;
        p.promotion_playoff_winners = 2; /* independent 3v6 and 4v5 ties */
    }
    *out = p;
    return 1;
}

/* Deterministic circle-method reference fixture sequence, like a 20-team
 * Premier League. Each pair plays once in either direction in the first
 * 19 rounds and with the reversed venue in the next 19 rounds.
 * IMPORTANT: a game integration should normally use its league scheduler;
 * this is a fixture topology reference and validator, not official CBF dates.
 * Returns 380, or 0 without modifying output if the buffer is invalid.
 */
static inline int brazil_2026_round_robin(Brazil2026Fixture *out, int capacity)
{
    if (!out || capacity < BRAZIL_2026_FIXTURES) return 0;
    int seats[BRAZIL_2026_CLUBS];
    for (int i = 0; i < BRAZIL_2026_CLUBS; ++i) seats[i] = i;
    for (int round = 0; round < BRAZIL_2026_ROUNDS / 2; ++round) {
        for (int pair = 0; pair < BRAZIL_2026_MATCHES_PER_ROUND; ++pair) {
            const int a = seats[pair];
            const int b = seats[BRAZIL_2026_CLUBS - pair - 1];
            const int first_home = ((round + pair) & 1) ? b : a;
            const int first_away = ((round + pair) & 1) ? a : b;
            const int first_i = round * BRAZIL_2026_MATCHES_PER_ROUND + pair;
            const int return_i = (round + 19) * BRAZIL_2026_MATCHES_PER_ROUND + pair;
            out[first_i].round = round;
            out[first_i].home = first_home;
            out[first_i].away = first_away;
            out[return_i].round = round + 19;
            out[return_i].home = first_away;
            out[return_i].away = first_home;
        }
        /* Hold the first club fixed and rotate the remaining 19 seats. */
        const int last = seats[BRAZIL_2026_CLUBS - 1];
        for (int i = BRAZIL_2026_CLUBS - 1; i > 1; --i)
            seats[i] = seats[i - 1];
        seats[1] = last;
    }
    return BRAZIL_2026_FIXTURES;
}

/* Fail-closed check of a realized league fixture list before any change to
 * a live game: 380 fixtures, 10 distinct matches per round, exactly one
 * home game and one away game against each opponent, 19 home/19 away.
 */
static inline int brazil_2026_fixtures_valid(const Brazil2026Fixture *fixtures, int count)
{
    if (!fixtures || count != BRAZIL_2026_FIXTURES) return 0;
    int encounters[BRAZIL_2026_CLUBS][BRAZIL_2026_CLUBS] = {{0}};
    int round_clubs[BRAZIL_2026_ROUNDS][BRAZIL_2026_CLUBS] = {{0}};
    int home_games[BRAZIL_2026_CLUBS] = {0};
    int away_games[BRAZIL_2026_CLUBS] = {0};

    for (int i = 0; i < count; ++i) {
        const Brazil2026Fixture *f = &fixtures[i];
        if (f->round < 0 || f->round >= BRAZIL_2026_ROUNDS ||
            f->home < 0 || f->home >= BRAZIL_2026_CLUBS ||
            f->away < 0 || f->away >= BRAZIL_2026_CLUBS ||
            f->home == f->away)
            return 0;
        if (++encounters[f->home][f->away] != 1 ||
            ++round_clubs[f->round][f->home] != 1 ||
            ++round_clubs[f->round][f->away] != 1)
            return 0;
        ++home_games[f->home];
        ++away_games[f->away];
    }

    for (int club = 0; club < BRAZIL_2026_CLUBS; ++club) {
        if (home_games[club] != 19 || away_games[club] != 19) return 0;
        for (int other = 0; other < BRAZIL_2026_CLUBS; ++other)
            if (other != club && encounters[club][other] != 1) return 0;
    }
    for (int round = 0; round < BRAZIL_2026_ROUNDS; ++round)
        for (int club = 0; club < BRAZIL_2026_CLUBS; ++club)
            if (round_clubs[round][club] != 1) return 0;
    return 1;
}

#endif
