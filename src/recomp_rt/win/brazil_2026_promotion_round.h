/* Original CM 01/02 3.9.68 two-legged Cup_Stage tie-break configuration.
 *
 * English First's semi-final generator writes 0x0204 to round table +0x0d
 * (0x572af3). Cup_Stage copies that WORD into its tie fixture+0x38
 * (0x51b741). Cup::resolve_tie (0x51f2f0), at 0x51f64c..0x51f696,
 * examines a level aggregate score after BOTH legs and tests:
 *   0x0800 => first participant qualifies;
 *   0x1000 => second participant qualifies;
 * before checking away goals or penalty kicks.
 *
 * The 2026 Brazilian promotion ties use English seeded entry order:
 *   [6th, 3rd], [5th, 4th].
 * In EACH case the second participant finished higher and must win if
 * the two-leg aggregate is level, irrespective of away goals.
 *
 * Require the precisely reverse-engineered English 0x0204 flags (fixed
 * pairs, 2-leg routine). This deliberately fails closed on unknown future
 * game builds, one-legged rounds or a previously forced outcome.
 */
#ifndef RECOMP_BRAZIL_2026_PROMOTION_ROUND_H
#define RECOMP_BRAZIL_2026_PROMOTION_ROUND_H
#include <stdint.h>

enum {
    BR_ACCESS_ENGLISH_SEMIS = 0x0204,
    BR_ACCESS_FIRST_WINS_DRAW = 0x0800,
    BR_ACCESS_SECOND_WINS_DRAW = 0x1000,
    BR_ACCESS_CBF_SEMIS = BR_ACCESS_ENGLISH_SEMIS | BR_ACCESS_SECOND_WINS_DRAW
};

/* Returns 1 and an adapted round word, or 0 without modifying out.
 * The second participant must be higher-ranked in both fixed pairings.
 */
static inline int brazil_2026_access_cbf_flags(uint16_t original,
                                               int legs,
                                               const int seed_rank[4],
                                               uint16_t *out)
{
    if (!out || !seed_rank || legs != 2 ||
        original != BR_ACCESS_ENGLISH_SEMIS ||
        seed_rank[0] != 5 || seed_rank[1] != 2 ||
        seed_rank[2] != 4 || seed_rank[3] != 3)
        return 0;
    *out = (uint16_t)BR_ACCESS_CBF_SEMIS;
    return 1;
}

/* Pure model of the directly inspected original aggregate-tie branch,
 * useful for a regression test, not a substitute for running the EXE.
 * 1 = first participant, 2 = second, 0 = game applies ordinary rules.
 */
static inline int brazil_2026_access_level_winner(uint16_t flags)
{
    if (flags & BR_ACCESS_FIRST_WINS_DRAW) return 1;
    if (flags & BR_ACCESS_SECOND_WINS_DRAW) return 2;
    return 0;
}
#endif
