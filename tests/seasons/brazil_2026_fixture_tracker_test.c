/* Tests the exact 380-directed-pair invariant without a game binary. */
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../../src/recomp_rt/win/brazil_2026_fixture_tracker.h"

int main(void)
{
    uint32_t club[20];
    for (int i = 0; i < 20; ++i) club[i] = 0x10000u + 0x245u * (uint32_t)i;
    BrazilFixtureTracker t;
    assert(!brazil_fixture_tracker_begin(0, club));
    assert(!brazil_fixture_tracker_begin(&t, 0));
    club[10] = club[0];
    assert(!brazil_fixture_tracker_begin(&t, club));
    club[10] = 0x10000u + 0x245u * 10u;
    assert(brazil_fixture_tracker_begin(&t, club));
    assert(!brazil_fixture_tracker_complete(&t));

    for (int home = 0; home < 20; ++home) {
        for (int away = 0; away < 20; ++away) {
            if (home == away) continue;
            assert(brazil_fixture_tracker_add(&t, club[home], club[away]));
        }
    }
    assert(t.count == 380);
    assert(brazil_fixture_tracker_complete(&t));
    for (int i = 0; i < 20; ++i)
        assert(t.home[i] == 19 && t.away[i] == 19);
    /* Attempt number 381 poisons the ledger. */
    assert(!brazil_fixture_tracker_add(&t, club[0], club[1]));
    assert(!brazil_fixture_tracker_complete(&t));
    assert(t.bad == 1);

    /* Missing return fixture: count 379 cannot pass. */
    assert(brazil_fixture_tracker_begin(&t, club));
    for (int home = 0; home < 20; ++home)
        for (int away = 0; away < 20; ++away)
            if (home != away && !(home == 19 && away == 18))
                assert(brazil_fixture_tracker_add(&t, club[home], club[away]));
    assert(t.count == 379);
    assert(!brazil_fixture_tracker_complete(&t));

    assert(brazil_fixture_tracker_begin(&t, club));
    assert(!brazil_fixture_tracker_add(&t, club[0], club[0]));
    assert(!brazil_fixture_tracker_complete(&t));
    assert(brazil_fixture_tracker_begin(&t, club));
    assert(!brazil_fixture_tracker_add(&t, club[0], 0xdeadbeefu));
    assert(!brazil_fixture_tracker_complete(&t));
    assert(brazil_fixture_tracker_begin(&t, club));
    assert(brazil_fixture_tracker_add(&t, club[0], club[1]));
    assert(!brazil_fixture_tracker_add(&t, club[0], club[1]));
    assert(!brazil_fixture_tracker_complete(&t));

    assert(brazil_fixture_tracker_begin(&t, club));
    assert(!brazil_fixture_tracker_add(0, club[0], club[1]));
    return 0;
}
