/* The live template must never replace historical or non-Brazilian games. */
#include <assert.h>
#include "../../src/recomp_rt/win/brazil_2026_runtime_gate.h"

int main(void)
{
    assert(brazil_2026_runtime_schedule_allowed("bra_nat_first", 2026, 20, 1, -1));
    assert(brazil_2026_runtime_schedule_allowed("bra_nat_second", 2026, 20, 1, -1));
    assert(brazil_2026_runtime_schedule_allowed("bra_nat_first", 2026, 20, 2, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2002, 20, 1, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2025, 20, 1, -1));
    assert(brazil_2026_runtime_schedule_allowed("bra_nat_first", 2027, 20, 1, -1));
    assert(brazil_2026_runtime_schedule_allowed("bra_nat_second", 2027, 20, 1, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2028, 20, 1, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2026, 28, 1, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2026, 20, 3, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2026, 20, 1, 0));
    assert(!brazil_2026_runtime_schedule_allowed("bra_nat_first", 2026, 20, 1, 1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_cup", 2026, 20, 1, -1));
    assert(!brazil_2026_runtime_schedule_allowed("bra_reg_sp", 2026, 20, 1, -1));
    assert(!brazil_2026_runtime_schedule_allowed("eng_prm", 2026, 20, 2, -1));
    assert(!brazil_2026_runtime_schedule_allowed(0, 2026, 20, 2, -1));
    return 0;
}
