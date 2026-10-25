/* Original 3.9.68 Cup_Stage tie-flag rule, restricted to Brazilian
 * 2026 Série B. See disassembly: 0x572af3, 0x51b741 and 0x51f64c..0x51f696.
 */
#include <assert.h>
#include <stdint.h>
#include "../../src/recomp_rt/win/brazil_2026_promotion_round.h"
#include "../../src/recomp_rt/win/brazil_2026_promotion.h"

int main(void)
{
    int ranks[4] = {5, 2, 4, 3}; /* second of each pair is higher-ranked */
    uint16_t flags = 0xffff;
    assert(!brazil_2026_access_cbf_flags(0x204, 2, ranks, 0));
    assert(!brazil_2026_access_cbf_flags(0x204, 2, 0, &flags));
    assert(!brazil_2026_access_cbf_flags(0x204, 1, ranks, &flags));
    assert(!brazil_2026_access_cbf_flags(0x200, 2, ranks, &flags));
    assert(!brazil_2026_access_cbf_flags(0x224, 2, ranks, &flags));
    assert(!brazil_2026_access_cbf_flags(0x1204, 2, ranks, &flags));
    assert(flags == 0xffff);

    ranks[0] = 2; /* higher placed team must be seeded SECOND */
    assert(!brazil_2026_access_cbf_flags(0x204, 2, ranks, &flags));
    ranks[0] = 5;
    ranks[3] = 4;
    assert(!brazil_2026_access_cbf_flags(0x204, 2, ranks, &flags));
    ranks[3] = 3;
    assert(brazil_2026_access_cbf_flags(0x204, 2, ranks, &flags));
    assert(flags == 0x1204u);
    assert((flags & 0x204u) == 0x204u);
    assert((flags & 0x0800u) == 0);
    assert((flags & 0x1000u) != 0);
    assert(brazil_2026_access_level_winner(flags) == 2);
    assert(brazil_2026_access_level_winner(0x204u) == 0);
    assert(brazil_2026_access_level_winner(0x0a04u) == 1);

    Brazil2026Tie ties[2];
    assert(brazil_2026_promotion_pairs(ties, 2));
    /* Aggregate tied: 3rd/4th advances regardless of away goals. */
    assert(brazil_2026_promotion_winner(ties[0], 2, 0, 0, 2) == 2);
    assert(brazil_2026_promotion_winner(ties[1], 1, 1, 1, 1) == 3);
    /* Lower-placed side still advances on a strictly better aggregate. */
    assert(brazil_2026_promotion_winner(ties[0], 3, 0, 0, 1) == 5);
    assert(brazil_2026_promotion_winner(ties[1], 2, 1, 2, 0) == 4);
    return 0;
}
