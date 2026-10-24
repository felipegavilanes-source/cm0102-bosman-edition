/* Brazilian Serie B 2026 promotion play-offs: pure rules, no game-memory access.
 * This is a preparation module. It is NOT wired into the runtime yet.
 * CBF 2026: two direct promotions, 3rd v 6th and 4th v 5th, two legs,
 * with the better league-placed club advancing on equal aggregate score.
 * Ranks are zero-based, so 2=third and 5=sixth.
 */
#ifndef BRAZIL_2026_PROMOTION_H
#define BRAZIL_2026_PROMOTION_H

typedef struct Brazil2026Tie {
    int higher_rank;
    int lower_rank;
} Brazil2026Tie;

/* Returns 1 on success; caller must supply at least two slots. */
static inline int brazil_2026_promotion_pairs(Brazil2026Tie *out, int slots)
{
    if (!out || slots < 2) return 0;
    out[0].higher_rank = 2;
    out[0].lower_rank = 5;
    out[1].higher_rank = 3;
    out[1].lower_rank = 4;
    return 1;
}

/* Leg 1 is hosted by lower_rank; leg 2 by higher_rank.
 * Each score is home then away. Return the zero-based winning league rank,
 * or -1 for invalid ranks or negative goals.
 */
static inline int brazil_2026_promotion_winner(Brazil2026Tie tie,
                                                int leg1_home, int leg1_away,
                                                int leg2_home, int leg2_away)
{
    if (tie.higher_rank < 0 || tie.lower_rank <= tie.higher_rank ||
        leg1_home < 0 || leg1_away < 0 || leg2_home < 0 || leg2_away < 0)
        return -1;
    const int higher_goals = leg1_away + leg2_home;
    const int lower_goals = leg1_home + leg2_away;
    return higher_goals >= lower_goals ? tie.higher_rank : tie.lower_rank;
}

#endif
