/* Standalone rule tests: cc -std=c99 -Wall -Wextra -Werror -o /tmp/br2026 tests/seasons/brazil_2026_promotion_test.c && /tmp/br2026 */
#include <assert.h>
#include "../../src/recomp_rt/win/brazil_2026_promotion.h"

int main(void)
{
    Brazil2026Tie ties[2];
    assert(!brazil_2026_promotion_pairs(0, 2));
    assert(!brazil_2026_promotion_pairs(ties, 1));
    assert(brazil_2026_promotion_pairs(ties, 2));
    assert(ties[0].higher_rank == 2 && ties[0].lower_rank == 5);
    assert(ties[1].higher_rank == 3 && ties[1].lower_rank == 4);
    /* Equal aggregate: higher placed advances (1-0, 0-1). */
    assert(brazil_2026_promotion_winner(ties[0], 1, 0, 1, 0) == 2);
    /* Lower placed advances with more aggregate goals. */
    assert(brazil_2026_promotion_winner(ties[1], 2, 0, 1, 0) == 4);
    /* Higher placed wins aggregate. */
    assert(brazil_2026_promotion_winner(ties[0], 0, 1, 2, 0) == 2);
    /* Invalid scores rejected. */
    assert(brazil_2026_promotion_winner(ties[0], -1, 0, 0, 0) == -1);
    return 0;
}
