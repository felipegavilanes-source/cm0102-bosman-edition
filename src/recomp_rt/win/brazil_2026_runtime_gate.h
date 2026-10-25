/* Guard for an experimental 38-date Brazilian national league scheduler.
 *
 * This predicate does not read or modify game memory. Callers must also
 * require an explicit experimental option and season_game_changes().
 * The existing historical seasons continue using their original builders.
 */
#ifndef RECOMP_BRAZIL_2026_RUNTIME_GATE_H
#define RECOMP_BRAZIL_2026_RUNTIME_GATE_H
#include <string.h>

static inline int brazil_2026_runtime_schedule_allowed(const char *file,
                                                        int edition, int clubs,
                                                        int meetings,
                                                        int stage)
{
    return file &&
           (!strcmp(file, "bra_nat_first") || !strcmp(file, "bra_nat_second")) &&
           edition == 2026 && clubs == 20 &&
           (meetings == 1 || meetings == 2) && stage == -1;
}

#endif
