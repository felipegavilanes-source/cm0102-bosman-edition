/* Isolated game-rule adapter tests; no proprietary executable required.
 * cc -std=c99 -Wall -Wextra -Werror -pedantic -o /tmp/br2026 \
 *   tests/seasons/brazil_2026_promotion_test.c && /tmp/br2026
 */
#include <assert.h>
#include <string.h>
#include "../../src/recomp_rt/win/brazil_2026_promotion.h"

static void test_pairing_and_venue(void)
{
    Brazil2026Tie ties[2];
    int home = -1, away = -1;
    assert(!brazil_2026_promotion_pairs(0, 2));
    assert(!brazil_2026_promotion_pairs(ties, 1));
    assert(brazil_2026_promotion_pairs(ties, 2));
    assert(ties[0].higher_rank == 2 && ties[0].lower_rank == 5);
    assert(ties[1].higher_rank == 3 && ties[1].lower_rank == 4);
    /* Matches the England 0x573080 stage ordering: 6th, 3rd, 5th, 4th. */
    int english_slots[4] = {-1, -1, -1, -1};
    assert(!brazil_2026_promotion_english_slots(0, 4));
    assert(!brazil_2026_promotion_english_slots(english_slots, 3));
    assert(brazil_2026_promotion_english_slots(english_slots, 4));
    assert(english_slots[0] == 5 && english_slots[1] == 2);
    assert(english_slots[2] == 4 && english_slots[3] == 3);
    assert(brazil_2026_promotion_venue(ties[0], 0, &home, &away));
    assert(home == 5 && away == 2); /* 6th hosts 3rd */
    assert(brazil_2026_promotion_venue(ties[0], 1, &home, &away));
    assert(home == 2 && away == 5); /* 3rd hosts 6th */
    assert(brazil_2026_promotion_venue(ties[1], 0, &home, &away));
    assert(home == 4 && away == 3);
    assert(brazil_2026_promotion_venue(ties[1], 1, &home, &away));
    assert(home == 3 && away == 4);
    assert(!brazil_2026_promotion_venue(ties[0], 2, &home, &away));
    assert(!brazil_2026_promotion_venue(ties[0], 0, 0, &away));
}

static void test_results(void)
{
    Brazil2026Tie ties[2];
    assert(brazil_2026_promotion_pairs(ties, 2));
    /* Equal aggregate (1-1): higher placed advances, not penalties. */
    assert(brazil_2026_promotion_winner(ties[0], 1, 0, 1, 0) == 2);
    /* Lower placed wins on aggregate: 2-1. */
    assert(brazil_2026_promotion_winner(ties[1], 2, 0, 1, 0) == 4);
    /* Higher placed wins aggregate: 3-0. */
    assert(brazil_2026_promotion_winner(ties[0], 0, 1, 2, 0) == 2);
    /* Away goals do not trump better-placed team on level aggregate. */
    assert(brazil_2026_promotion_winner(ties[0], 1, 0, 2, 1) == 2);
    assert(brazil_2026_promotion_winner(ties[0], -1, 0, 0, 0) == -1);
}

static void sample_status(unsigned char status[20])
{
    memset(status, 0xfe, 20);
    status[0] = 5; /* champion */
    status[1] = 0; /* runner-up */
    for (int i = 2; i <= 5; ++i) status[i] = 1; /* playoff */
    for (int i = 16; i < 20; ++i) status[i] = 3; /* relegated */
}

static void test_game_status_adapter(void)
{
    unsigned char status[20], saved[20];
    sample_status(status);
    assert(brazil_2026_promotion_apply(status, 20, 2, 4));
    int up = 0, down = 0;
    for (int i = 0; i < 20; ++i) {
        up += status[i] == 0 || status[i] == 5;
        down += status[i] == 3;
    }
    assert(up == 4 && down == 4);
    assert(status[2] == 0 && status[3] == 1);
    assert(status[4] == 0 && status[5] == 1);

    sample_status(status);
    memcpy(saved, status, 20);
    assert(!brazil_2026_promotion_apply(status, 20, 4, 3)); /* wrong tie winner */
    assert(!brazil_2026_promotion_apply(status, 19, 2, 3)); /* wrong table size */
    assert(!memcmp(status, saved, 20));
    status[3] = 0; /* no longer a playoff zone */
    memcpy(saved, status, 20);
    assert(!brazil_2026_promotion_apply(status, 20, 2, 4));
    assert(!memcmp(status, saved, 20));
    assert(!brazil_2026_promotion_apply(0, 20, 2, 3));
}

int main(void)
{
    test_pairing_and_venue();
    test_results();
    test_game_status_adapter();
    return 0;
}
