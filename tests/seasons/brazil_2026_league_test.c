/* Pure-C tests of modern 2026 Brazilian league shape and fixtures.
 * cc -std=c99 -Wall -Wextra -Werror -pedantic -O2 \
 *   tests/seasons/brazil_2026_league_test.c -o /tmp/br2026league && /tmp/br2026league
 */
#include <assert.h>
#include <string.h>

#include "../../src/recomp_rt/win/brazil_2026_league.h"

static void test_profiles(void)
{
    Brazil2026LeagueProfile p;
    memset(&p, 0, sizeof p);
    assert(!brazil_2026_league_profile(NULL, 2026, &p));
    assert(!brazil_2026_league_profile("bra_nat_first", 2026, NULL));
    assert(!brazil_2026_league_profile("eng_prm", 2026, &p));
    assert(!brazil_2026_league_profile("bra_nat_first", 2002, &p));
    assert(!brazil_2026_league_profile("bra_nat_first", 2025, &p));

    assert(brazil_2026_league_profile("bra_nat_first", 2026, &p));
    assert(p.clubs == 20);
    assert(p.meetings_per_pair == 2);
    assert(p.rounds == 38 && p.fixtures == 380);
    assert(p.title_playoff_rounds == 0); /* absolutely no Serie A title KO */
    assert(p.direct_promotions == 0 && p.promotion_playoff_winners == 0);
    assert(p.relegations == 4);

    assert(brazil_2026_league_profile("bra_nat_second", 2026, &p));
    assert(p.clubs == 20 && p.meetings_per_pair == 2);
    assert(p.rounds == 38 && p.fixtures == 380);
    assert(p.title_playoff_rounds == 0);
    assert(p.direct_promotions == 2 && p.promotion_playoff_winners == 2);
    assert(p.relegations == 4);

    assert(brazil_2026_league_profile("bra_nat_first", 2027, &p));
    assert(p.rounds == 38 && p.title_playoff_rounds == 0);
}

static void test_round_robin(void)
{
    Brazil2026Fixture fixtures[BRAZIL_2026_FIXTURES], saved[2];
    assert(brazil_2026_round_robin(NULL, BRAZIL_2026_FIXTURES) == 0);
    assert(brazil_2026_round_robin(fixtures, BRAZIL_2026_FIXTURES - 1) == 0);
    assert(brazil_2026_round_robin(fixtures, BRAZIL_2026_FIXTURES) == 380);
    assert(brazil_2026_fixtures_valid(fixtures, BRAZIL_2026_FIXTURES));
    assert(fixtures[0].round == 0 && fixtures[0].home == 0 && fixtures[0].away == 19);
    assert(fixtures[19 * 10].round == 19 &&
           fixtures[19 * 10].home == 19 && fixtures[19 * 10].away == 0);

    saved[0] = fixtures[0];
    saved[1] = fixtures[1];
    fixtures[0].away = fixtures[0].home; /* self-match */
    assert(!brazil_2026_fixtures_valid(fixtures, 380));
    fixtures[0] = saved[0];

    fixtures[0] = fixtures[1]; /* duplicate match */
    assert(!brazil_2026_fixtures_valid(fixtures, 380));
    fixtures[0] = saved[0];

    fixtures[0].round = 1; /* two games in the same round */
    assert(!brazil_2026_fixtures_valid(fixtures, 380));
    fixtures[0] = saved[0];

    fixtures[0].home = saved[1].home; /* wrong pair */
    assert(!brazil_2026_fixtures_valid(fixtures, 380));
    fixtures[0] = saved[0];

    assert(!brazil_2026_fixtures_valid(fixtures, 379));
    assert(!brazil_2026_fixtures_valid(NULL, 380));
    assert(brazil_2026_fixtures_valid(fixtures, 380));
}

int main(void)
{
    test_profiles();
    test_round_robin();
    return 0;
}
