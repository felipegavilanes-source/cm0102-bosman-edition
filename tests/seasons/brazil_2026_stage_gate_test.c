/* C99 deterministic safety checks for the runtime gate that removes
 * the historical eight-team title playoff only after the real league
 * has 38 dates / 2 meetings. This does not mutate game state.
 */
#include <assert.h>
#include "../../src/recomp_rt/win/brazil_2026_stage_gate.h"

int main(void)
{
    Brazil2026StageState state = {2026, 20, 2, 38, -1, 1, -1};
    assert(brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.matches_per_club = 38;
    assert(brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_second", &state));
    assert(!brazil_2026_serie_a_can_remove_final("eng_prm", &state));
    assert(!brazil_2026_serie_a_can_remove_final(0, &state));
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", 0));

    state.edition = 2002;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.edition = 2026;
    state.meetings = 1;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.meetings = 2;
    state.date_slots = 27;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.date_slots = 38;
    state.matches_per_club = 19;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.matches_per_club = 38;
    state.stages = 2;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.stages = 1;
    state.current_stage = 0;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    state.current_stage = -1;
    state.clubs = 19;
    assert(!brazil_2026_serie_a_can_remove_final("bra_nat_first", &state));
    return 0;
}
