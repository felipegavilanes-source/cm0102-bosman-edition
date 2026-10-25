/* Pure rule gate protects only real Brazilian Serie B in 2026. */
#include <assert.h>
#include "../../src/recomp_rt/win/brazil_2026_access_gate.h"

int main(void)
{
    BrazilAccessState s = {2026, 20, 2, 38, 1, -1, 2, 4, 4};
    assert(brazil_2026_access_gate("bra_nat_second", &s, 1));
    assert(!brazil_2026_access_gate("bra_nat_second", &s, 0));
    assert(!brazil_2026_access_gate("bra_nat_first", &s, 1));
    assert(!brazil_2026_access_gate("eng_first", &s, 1));
    assert(!brazil_2026_access_gate(NULL, &s, 1));
    assert(!brazil_2026_access_gate("bra_nat_second", NULL, 1));
    s.edition=2027; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.edition=2026;
    s.meetings=1; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.meetings=2;
    s.dates=27; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.dates=38;
    s.clubs=8; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.clubs=20;
    s.stages=2; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.stages=1;
    s.current=0; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.current=-1;
    s.direct=4; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.direct=2;
    s.playoff=0; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.playoff=4;
    s.relegations=3; assert(!brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.relegations=4;
    assert(brazil_2026_access_gate("bra_nat_second", &s, 1));
    s.current=0;
    assert(brazil_2026_access_result_gate("bra_nat_second", &s, 0));
    assert(!brazil_2026_access_result_gate("bra_nat_second", &s, -1));
    assert(!brazil_2026_access_result_gate("bra_nat_second", &s, 1));
    s.current=-1;
    assert(!brazil_2026_access_result_gate("bra_nat_second", &s, 0));
    return 0;
}
