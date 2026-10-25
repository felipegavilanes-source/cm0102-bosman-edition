/* Standalone coexistence tests: states, A/B, Copa do Brasil,
 * Libertadores, Sudamericana, World Cup breaks and 2027 rollover.
 * cc -std=c99 -Wall -Wextra -Werror -pedantic -O2 \
 *    tests/seasons/brazil_2026_calendar_test.c -o /tmp/br-calendar &&
 *    /tmp/br-calendar
 */
#include <assert.h>
#include "../../src/recomp_rt/win/brazil_2026_calendar.h"

#define DAY(Y,M,D) brazil_cal_day((Y), (M), (D))

static void test_roundtrip_dates(void)
{
    for (int year = 2026; year <= 2028; ++year) {
        for (int ordinal = 1; ordinal <= brazil_cal_days_in_year(year); ++ordinal) {
            int day = -1, month = -1;
            assert(brazil_cal_split_day(year, ordinal, &day, &month));
            assert(month >= 0 && month < 12);
            assert(day >= 1 && day <= 31);
            assert(brazil_cal_day(year, month + 1, day) == ordinal);
        }
    }
    int day = 9, month = 8;
    assert(!brazil_cal_split_day(2026, 0, &day, &month));
    assert(!brazil_cal_split_day(2026, 366, &day, &month));
    assert(!brazil_cal_split_day(1900, 1, &day, &month));
    assert(!brazil_cal_split_day(2026, 1, 0, &month));
    assert(!brazil_cal_split_day(2026, 1, &day, 0));
    assert(day == 9 && month == 8);
}

static void test_date_and_competition_windows(void)
{
    BrazilCalendarWindow w;
    assert(DAY(2026, 2, 29) == 0);
    assert(DAY(2027, 2, 29) == 0);
    assert(DAY(2028, 2, 29) == 60);
    assert(DAY(2026, 3, 1) == 60);
    assert(DAY(2026, 12, 31) == 365);
    assert(brazil_cal_serial(2027, 1) - brazil_cal_serial(2026, 365) == 1);

    assert(brazil_cal_window(2026, BRA_CAL_STATE, &w));
    assert(w.maximum_matchdays == 11);
    assert(w.first_day == DAY(2026, 1, 11) && w.last_day == DAY(2026, 3, 8));
    assert(brazil_cal_allowed(2026, BRA_CAL_STATE, DAY(2026, 2, 14)));
    assert(!brazil_cal_allowed(2026, BRA_CAL_STATE, DAY(2026, 3, 9)));

    assert(brazil_cal_window(2026, BRA_CAL_SERIE_A, &w));
    assert(w.first_day == DAY(2026, 1, 28));
    assert(w.last_day == DAY(2026, 12, 2));
    assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_A, DAY(2026, 1, 28)));
    assert(!brazil_cal_allowed(2026, BRA_CAL_SERIE_A, DAY(2026, 6, 11)));
    assert(!brazil_cal_allowed(2026, BRA_CAL_SERIE_A, DAY(2026, 7, 19)));
    assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_A, DAY(2026, 7, 22)));
    assert(!brazil_cal_allowed(2026, BRA_CAL_SERIE_A, DAY(2026, 12, 3)));

    assert(brazil_cal_window(2026, BRA_CAL_SERIE_B, &w));
    assert(w.first_day == DAY(2026, 3, 20));
    assert(w.last_day == DAY(2026, 11, 14));
    /* Serie B DOES NOT stop for the 2026 men's World Cup. */
    assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_B, DAY(2026, 6, 20)));
    assert(!brazil_cal_allowed(2026, BRA_CAL_SERIE_B, DAY(2026, 11, 21)));

    assert(brazil_cal_window(2026, BRA_CAL_SERIE_B_ACCESS, &w));
    assert(w.fixed_day_one == DAY(2026, 11, 21));
    assert(w.fixed_day_two == DAY(2026, 11, 28));
    assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_B_ACCESS, w.fixed_day_one));
    assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_B_ACCESS, w.fixed_day_two));
    assert(!brazil_cal_allowed(2026, BRA_CAL_SERIE_B_ACCESS, DAY(2026, 11, 27)));

    assert(brazil_cal_allowed(2026, BRA_CAL_COPA_BRASIL, DAY(2026, 2, 18)));
    assert(brazil_cal_allowed(2026, BRA_CAL_COPA_BRASIL, DAY(2026, 12, 6)));
    assert(brazil_cal_allowed(2026, BRA_CAL_LIBERTADORES, DAY(2026, 4, 8)));
    assert(brazil_cal_allowed(2026, BRA_CAL_LIBERTADORES, DAY(2026, 11, 28)));
    assert(brazil_cal_allowed(2026, BRA_CAL_SUDAMERICANA, DAY(2026, 7, 22)));
    assert(brazil_cal_allowed(2026, BRA_CAL_SUDAMERICANA, DAY(2026, 11, 21)));
    assert(!brazil_cal_allowed(2026, BRA_CAL_SUDAMERICANA, DAY(2026, 11, 28)));

    /* 2027 female WC: A, B, domestic cup stop for June 24-July 25. */
    assert(brazil_cal_window(2027, BRA_CAL_SERIE_A, &w));
    assert(w.first_day == DAY(2027, 1, 27));
    assert(w.last_day == DAY(2027, 12, 1));
    assert(!brazil_cal_allowed(2027, BRA_CAL_SERIE_A, DAY(2027, 7, 2)));
    assert(!brazil_cal_allowed(2027, BRA_CAL_SERIE_B, DAY(2027, 7, 2)));
    assert(!brazil_cal_allowed(2027, BRA_CAL_COPA_BRASIL, DAY(2027, 7, 2)));
    assert(brazil_cal_allowed(2027, BRA_CAL_SERIE_A, DAY(2027, 7, 26)));
    assert(brazil_cal_allowed(2027, BRA_CAL_SERIE_B, DAY(2027, 11, 27)));
    assert(brazil_cal_allowed(2027, BRA_CAL_COPA_BRASIL, DAY(2027, 12, 5)));
    /* Do not invent detailed 2027 state/continental dates. */
    assert(!brazil_cal_window(2027, BRA_CAL_STATE, &w));
    assert(!brazil_cal_window(2027, BRA_CAL_LIBERTADORES, &w));
    assert(!brazil_cal_window(2027, BRA_CAL_SUDAMERICANA, &w));
    /* Known-existing continental/state fixtures in 2027 are preserved,
     * even while official new fixture templates are not coded here. */
    assert(brazil_cal_existing_event_allowed(2027, BRA_CAL_STATE,
                                             DAY(2027, 2, 10)));
    assert(brazil_cal_existing_event_allowed(2027, BRA_CAL_LIBERTADORES,
                                             DAY(2027, 4, 14)));
    assert(brazil_cal_existing_event_allowed(2027, BRA_CAL_SUDAMERICANA,
                                             DAY(2027, 8, 19)));
    assert(!brazil_cal_allowed(2027, BRA_CAL_LIBERTADORES,
                               DAY(2027, 4, 14))); /* no new games scheduled */
    assert(!brazil_cal_existing_event_allowed(2027, BRA_CAL_SERIE_A,
                                              DAY(2027, 7, 2)));

    assert(!brazil_cal_window(2002, BRA_CAL_SERIE_A, &w));
    assert(!brazil_cal_window(2026, BRA_CAL_SERIE_A, 0));
}

static void test_overlap_and_reschedule(void)
{
    const BrazilCalendarEvent mixed[] = {
        /* A and state competition seasons overlap, but same club plays
         * three calendar days apart: valid. */
        {2026, DAY(2026, 1, 25), BRA_CAL_STATE, 11, 10},
        {2026, DAY(2026, 1, 28), BRA_CAL_SERIE_A, 11, 12},
        {2026, DAY(2026, 2, 1), BRA_CAL_STATE, 11, 15},
        /* Libertadores midweek and Serie A the following weekend. */
        {2026, DAY(2026, 4, 8), BRA_CAL_LIBERTADORES, 11, 200},
        {2026, DAY(2026, 4, 11), BRA_CAL_SERIE_A, 11, 13},
        /* Copa do Brasil midweek and Serie B on a separate date. */
        {2026, DAY(2026, 4, 8), BRA_CAL_COPA_BRASIL, 35, 36},
        {2026, DAY(2026, 4, 11), BRA_CAL_SERIE_B, 35, 37},
        /* Different clubs may play on the same date in separate cups. */
        {2026, DAY(2026, 4, 8), BRA_CAL_SUDAMERICANA, 41, 42},
        /* Serie B remains active during the World Cup. */
        {2026, DAY(2026, 6, 20), BRA_CAL_SERIE_B, 35, 38},
        /* Copa do Brasil final follows Serie A's final round. */
        {2026, DAY(2026, 12, 2), BRA_CAL_SERIE_A, 11, 9},
        {2026, DAY(2026, 12, 6), BRA_CAL_COPA_BRASIL, 11, 35}
    };
    int bad = -2;
    assert(brazil_cal_validate(mixed, (int)(sizeof mixed / sizeof mixed[0]), 3, &bad));
    assert(bad == -1);

    const BrazilCalendarEvent same_club_same_day[] = {
        {2026, DAY(2026, 4, 8), BRA_CAL_LIBERTADORES, 10, 500},
        {2026, DAY(2026, 4, 8), BRA_CAL_COPA_BRASIL, 10, 400}
    };
    assert(!brazil_cal_validate(same_club_same_day, 2, 3, &bad));
    assert(bad == 1);
    const BrazilCalendarEvent too_close[] = {
        {2026, DAY(2026, 4, 8), BRA_CAL_LIBERTADORES, 10, 500},
        {2026, DAY(2026, 4, 10), BRA_CAL_SERIE_A, 10, 400}
    };
    assert(!brazil_cal_validate(too_close, 2, 3, &bad));
    assert(bad == 1);

    const BrazilCalendarEvent conflict_for_a[] = {
        {2026, DAY(2026, 4, 8), BRA_CAL_LIBERTADORES, 10, 500}
    };
    int chosen = -1;
    assert(brazil_cal_find_league_day(2026, BRA_CAL_SERIE_A, 10, 400,
                                      DAY(2026, 4, 8), DAY(2026, 4, 6),
                                      DAY(2026, 4, 15), conflict_for_a, 1, 3, &chosen));
    assert(chosen == DAY(2026, 4, 11));
    /* The cup fixture remains fixed; the league date is moved. */
    assert(conflict_for_a[0].day == DAY(2026, 4, 8));
    assert(!brazil_cal_find_league_day(2026, BRA_CAL_SERIE_A, 10, 400,
                                       DAY(2026, 4, 8), DAY(2026, 4, 8),
                                       DAY(2026, 4, 9), conflict_for_a, 1, 3, &chosen));
    assert(!brazil_cal_find_league_day(2026, BRA_CAL_LIBERTADORES, 10, 400,
                                       DAY(2026, 4, 8), DAY(2026, 4, 7),
                                       DAY(2026, 4, 10), conflict_for_a, 1, 3, &chosen));

    /* The same clubs play Serie B and their historic Cup fixtures. */
    const BrazilCalendarEvent playoff_conflict[] = {
        {2026, DAY(2026, 11, 21), BRA_CAL_SERIE_B_ACCESS, 44, 45},
        {2026, DAY(2026, 11, 28), BRA_CAL_SERIE_B_ACCESS, 45, 44}
    };
    assert(brazil_cal_validate(playoff_conflict, 2, 3, &bad));
    assert(!brazil_cal_find_league_day(2026, BRA_CAL_SERIE_B, 44, 45,
                                       DAY(2026, 11, 21), DAY(2026, 11, 21),
                                       DAY(2026, 11, 28), playoff_conflict, 2, 3, &chosen));

    /* The 2027 continental fixture is immutable, but it still blocks a
     * same-team national league game on that date. */
    const BrazilCalendarEvent conmebol_2027[] = {
        {2027, DAY(2027, 8, 19), BRA_CAL_SUDAMERICANA, 51, 55}
    };
    assert(brazil_cal_validate(conmebol_2027, 1, 3, &bad));
    assert(brazil_cal_find_league_day(2027, BRA_CAL_SERIE_A, 51, 60,
                                      DAY(2027, 8, 19), DAY(2027, 8, 17),
                                      DAY(2027, 8, 24), conmebol_2027, 1, 3, &chosen));
    assert(chosen == DAY(2027, 8, 22));

    /* Refuse fixture dates that fall in the Serie A World Cup break. */
    assert(!brazil_cal_find_league_day(2026, BRA_CAL_SERIE_A, 9, 10,
                                       DAY(2026, 6, 20), DAY(2026, 6, 20),
                                       DAY(2026, 6, 25), 0, 0, 3, &chosen));
}

int main(void)
{
    test_roundtrip_dates();
    test_date_and_competition_windows();
    test_overlap_and_reschedule();
    return 0;
}
