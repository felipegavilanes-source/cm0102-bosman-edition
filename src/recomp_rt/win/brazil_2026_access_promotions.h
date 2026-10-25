/* Fail-closed check at the point the original League promotion chain runs.
 * The in-engine 2026 Serie B one-round Cup_Stage must have produced TWO
 * winners at ranks 3-6 in addition to the first two automatic promotions.
 * The bottom four must be relegated. The original engine moves rows with
 * status 0/5 up and status 3 down.
 *
 * No game memory access here. This checker observes final table status;
 * it never edits the source table or alters season progression.
 */
#ifndef RECOMP_BRAZIL_2026_ACCESS_PROMOTIONS_H
#define RECOMP_BRAZIL_2026_ACCESS_PROMOTIONS_H
#include <stdint.h>

static inline int brazil_2026_access_promotions_valid(const uint8_t *status,
                                                      int count)
{
    if (!status || count != 20) return 0;
    if (status[0] != 5 && status[0] != 0) return 0;
    if (status[1] != 0) return 0;

    int up = 0, down = 0, playoff_winners = 0;
    for (int i = 0; i < count; ++i) {
        if (status[i] == 0 || status[i] == 5) ++up;
        if (status[i] == 3) ++down;
        if (i >= 2 && i <= 5) {
            if (status[i] == 0) ++playoff_winners;
            else if (status[i] != 0xfe) return 0;
        }
        /* The relegated teams are precisely ranks 17-20. */
        if (i >= 16 && status[i] != 3) return 0;
        if (i < 16 && status[i] == 3) return 0;
    }
    return up == 4 && down == 4 && playoff_winners == 2;
}
#endif
