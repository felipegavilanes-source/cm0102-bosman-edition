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

/* Reports the home and away ranks for a particular leg (0 = first leg,
 * 1 = second leg). The lower-placed club hosts the first leg. */
static inline int brazil_2026_promotion_venue(Brazil2026Tie tie, int leg,
                                               int *home_rank, int *away_rank)
{
    if (!home_rank || !away_rank || tie.higher_rank < 0 ||
        tie.lower_rank <= tie.higher_rank || (leg != 0 && leg != 1))
        return 0;
    *home_rank = leg == 0 ? tie.lower_rank : tie.higher_rank;
    *away_rank = leg == 0 ? tie.higher_rank : tie.lower_rank;
    return 1;
}

/* Integrate two winners with the original League table-row status byte:
 * 0=promoted, 1=play-off place, 5=champion (the direct places are untouched).
 * Only apply AFTER both ties are complete and the game's regular-season
 * status table has been populated. The caller remains responsible for save/
 * load persistence of fixtures and for the league's season-change chain.
 * This function fails without changing any status if the expected 20-club
 * table or play-off zone is absent. Rank indexes are zero-based.
 */
static inline int brazil_2026_promotion_apply(unsigned char *status, int count,
                                              int first_winner, int second_winner)
{
    if (!status || count != 20 || (first_winner != 2 && first_winner != 5) ||
        (second_winner != 3 && second_winner != 4))
        return 0;
    for (int i = 2; i <= 5; ++i)
        if (status[i] != 1) return 0;
    if (status[0] != 5 && status[0] != 0) return 0;
    if (status[1] != 0) return 0;
    status[first_winner] = 0;
    status[second_winner] = 0;
    return 1;
}

/* Adapter for the four club slots used by the legacy English First Division
 * semi-final builder (0x573080): [6th, 3rd, 5th, 4th].
 * Its first round pairs adjacent slots into exactly the two Brazilian ties.
 * The English final and the English tie-break MUST NOT be reused for Brazil.
 * This only provides ordering; no runtime patch or stage is installed here.
 */
static inline int brazil_2026_promotion_english_slots(int out[4], int slots)
{
    Brazil2026Tie ties[2];
    if (!out || slots < 4 || !brazil_2026_promotion_pairs(ties, 2)) return 0;
    out[0] = ties[0].lower_rank;
    out[1] = ties[0].higher_rank;
    out[2] = ties[1].lower_rank;
    out[3] = ties[1].higher_rank;
    return 1;
}

#endif
