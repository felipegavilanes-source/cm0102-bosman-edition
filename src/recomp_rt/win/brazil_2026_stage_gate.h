/* Fail-closed rule gate for the *real* Brazilian Serie A's old title playoff.
 *
 * The 2002 Brazilian class normally has a single round robin (19 fixtures
 * after resizing to 20 clubs) followed by a top-eight playoff. Merely
 * clearing the stage count would end the season after 19 rounds.
 *
 * Only allow the stage count to be cleared if a runtime inspection already
 * confirms a fully built, two-meeting, 38-date league with no active stage.
 * No memory access in this helper, so unit tests can check it independently.
 */
#ifndef RECOMP_BRAZIL_2026_STAGE_GATE_H
#define RECOMP_BRAZIL_2026_STAGE_GATE_H

#include <string.h>

typedef struct Brazil2026StageState {
    int edition;
    int clubs;
    int meetings;
    int date_slots;
    int matches_per_club;
    int stages;
    int current_stage;
} Brazil2026StageState;

/* The matches-per-club field can legitimately be -1 until check_complete
 * computes it. For stage removal require an explicit 38, not -1. */
static inline int brazil_2026_serie_a_can_remove_final(const char *file,
                                                       const Brazil2026StageState *s)
{
    return file && s && !strcmp(file, "bra_nat_first") &&
           s->edition >= 2026 &&
           s->clubs == 20 && s->meetings == 2 &&
           s->date_slots == 38 && s->matches_per_club == 38 &&
           s->stages == 1 && s->current_stage == -1;
}

#endif
