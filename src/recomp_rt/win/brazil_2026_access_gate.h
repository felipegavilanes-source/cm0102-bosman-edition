/* Experimental one-round/two-tie Serie B access-stage safety gate.
 * In 2026 the 1st and 2nd clubs go up directly and each independent
 * play-off tie has a separate winning promotion. No final is permitted.
 *
 * This is intentionally limited to 2026 until a 2027 Série B regulation
 * and game smoke tests are completed. No persistent state is held here.
 */
#ifndef RECOMP_BRAZIL_2026_ACCESS_GATE_H
#define RECOMP_BRAZIL_2026_ACCESS_GATE_H
#include <string.h>

typedef struct BrazilAccessState {
    int edition, clubs, meetings, dates, stages, current;
    int direct, playoff, relegations;
} BrazilAccessState;

static inline int brazil_2026_access_gate(const char *file,
                                          const BrazilAccessState *s,
                                          int correct_fixtures)
{
    return file && s && !strcmp(file, "bra_nat_second") &&
           s->edition == 2026 && s->clubs == 20 &&
           s->meetings == 2 && s->dates == 38 &&
           s->stages == 1 && s->current == -1 &&
           s->direct == 2 && s->playoff == 4 &&
           s->relegations == 4 && correct_fixtures;
}

/* A result callback after stage creation also needs to reject the old
 * 8-club Brazilian championship format and non-Brazilian leagues. */
static inline int brazil_2026_access_result_gate(const char *file,
                                                 const BrazilAccessState *s,
                                                 int result_stage)
{
    return file && s && !strcmp(file, "bra_nat_second") &&
           s->edition == 2026 && s->clubs == 20 &&
           s->meetings == 2 && s->dates == 38 &&
           s->stages == 1 && s->current == 0 &&
           s->direct == 2 && s->playoff == 4 &&
           s->relegations == 4 && result_stage == 0;
}
#endif
