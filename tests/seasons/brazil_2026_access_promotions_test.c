/* The original promotion chain moves every League row with status 0/5
 * up, and every row with status 3 down. Require exactly four of each.
 */
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../../src/recomp_rt/win/brazil_2026_access_promotions.h"

static void table(uint8_t s[20])
{
    memset(s, 0xfe, 20);
    s[0] = 5; /* champion */
    s[1] = 0; /* second: direct */
    for (int i = 16; i < 20; ++i) s[i] = 3;
}

int main(void)
{
    uint8_t s[20];
    table(s);
    s[2] = 0; /* third wins against sixth */
    s[3] = 0; /* fourth wins against fifth */
    assert(brazil_2026_access_promotions_valid(s, 20));
    assert(!brazil_2026_access_promotions_valid(s, 19));
    assert(!brazil_2026_access_promotions_valid(0, 20));

    /* Both opponents of the SAME tie must never be promoted together,
     * even if there are exactly four up overall. */
    table(s);
    s[2] = 0; s[5] = 0; /* 3rd AND 6th: impossible */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    table(s);
    s[3] = 0; s[4] = 0; /* 4th AND 5th: impossible */
    assert(!brazil_2026_access_promotions_valid(s, 20));

    table(s);
    s[5] = 0; /* sixth upsets third: still legal on better aggregate */
    s[4] = 0; /* fifth upsets fourth */
    assert(brazil_2026_access_promotions_valid(s, 20));

    table(s);
    s[2] = 0; s[4] = 0; /* one winner each tie */
    assert(brazil_2026_access_promotions_valid(s, 20));
    s[5] = 0; /* impossible third play-off winner */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    s[5] = 0xfe;
    s[4] = 1; /* semifinal not decided */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    s[4] = 0;

    s[19] = 0xfe; /* only three relegated */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    s[19] = 3;
    s[6] = 3; /* wrong middle-table relegation */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    s[6] = 0xfe;
    s[7] = 0; /* extra unrelated promotion */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    s[7] = 0xfe;
    s[1] = 1; /* second should be automatic */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    s[1] = 0;
    s[0] = 0xfe; /* champion never missing */
    assert(!brazil_2026_access_promotions_valid(s, 20));
    return 0;
}
