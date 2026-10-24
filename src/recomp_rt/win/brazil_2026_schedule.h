/* Standalone, fail-closed schedule planner for Brazilian A/B 2026.
 *
 * Produces candidate calendar dates for 380 league fixtures without
 * modifying existing state, Copa do Brasil or CONMEBOL fixtures.
 *
 * This is deliberately NOT attached to the original engine: 2002 Brazilian
 * create_schedule has 27 template dates and a knockout stage. The live
 * engine must first be adapted to hold 38 dates and to save/load them.
 *
 * Candidate dates are a simulated calendar, NOT the CBF's exact round table.
 * Match pairs are from brazil_2026_league.h, not official 2026 fixtures.
 */
#ifndef RECOMP_BRAZIL_2026_SCHEDULE_H
#define RECOMP_BRAZIL_2026_SCHEDULE_H

#include "brazil_2026_calendar.h"
#include "brazil_2026_league.h"
#include <stdlib.h>
#include <string.h>

/* Produces strictly increasing 2026 candidate dates.
 * Serie A: 18 dates before the June/July World Cup gap, then
 * 20 dates beginning 22 July; Serie B: 38 between March and November
 * without a World Cup break.
 * Simulated distribution inside official overall windows, not official
 * date-by-date or club-by-club fixtures.
 */
static inline int brazil_2026_candidate_round_dates(int year, int kind,
                                                    int out[BRAZIL_2026_ROUNDS])
{
    if (!out || year != 2026 ||
        (kind != BRA_CAL_SERIE_A && kind != BRA_CAL_SERIE_B))
        return 0;
    int dates[BRAZIL_2026_ROUNDS];
    if (kind == BRA_CAL_SERIE_A) {
        const int before_first = brazil_cal_day(year, 1, 28);
        const int before_last = brazil_cal_day(year, 5, 31);
        const int after_first = brazil_cal_day(year, 7, 22);
        const int after_last = brazil_cal_day(year, 12, 2);
        for (int i = 0; i < 18; ++i)
            dates[i] = before_first + (before_last - before_first) * i / 17;
        for (int i = 0; i < 20; ++i)
            dates[18 + i] = after_first + (after_last - after_first) * i / 19;
    } else {
        const int first = brazil_cal_day(year, 3, 20);
        const int last = brazil_cal_day(year, 11, 14);
        for (int i = 0; i < BRAZIL_2026_ROUNDS; ++i)
            dates[i] = first + (last - first) * i / (BRAZIL_2026_ROUNDS - 1);
    }
    for (int i = 0; i < BRAZIL_2026_ROUNDS; ++i)
        if (!brazil_cal_allowed(year, kind, dates[i]) ||
            (i && dates[i] <= dates[i - 1]))
            return 0;
    memcpy(out, dates, sizeof dates);
    return BRAZIL_2026_ROUNDS;
}

/* Generate a proposed fixture-day list. cup_events[] is an existing, immutable
 * collection of state, national-cup and continental matches (or fixed
 * league fixtures). This module NEVER edits the source array.
 *
 * round_dates[] holds 38 increasing preferred days (generated above or
 * supplied by an official table). Each team's games may move by at most
 * flex_days, respecting other club matches.
 *
 * All 380 entries are written to out[] atomically only if the proposal
 * passes every fixture and clash check. On failure out[] is unchanged.
 * There is NO calendar mutation, save-file operation or engine hook.
 */
static inline int brazil_2026_plan_league(const Brazil2026Fixture *fixtures,
                                           int fixture_count, int year, int kind,
                                           const int round_dates[BRAZIL_2026_ROUNDS],
                                           const BrazilCalendarEvent *cup_events,
                                           int cup_count, int min_gap, int flex_days,
                                           BrazilCalendarEvent *out, int out_capacity)
{
    if (!fixtures || !out || !round_dates ||
        fixture_count != BRAZIL_2026_FIXTURES ||
        out_capacity < BRAZIL_2026_FIXTURES ||
        year != 2026 || (kind != BRA_CAL_SERIE_A && kind != BRA_CAL_SERIE_B) ||
        cup_count < 0 || cup_count > 5000 || (cup_count && !cup_events) ||
        flex_days < 0 || flex_days > 7 || min_gap < 1 || min_gap > 10 ||
        !brazil_2026_fixtures_valid(fixtures, fixture_count))
        return 0;

    for (int i = 0; i < BRAZIL_2026_ROUNDS; ++i)
        if (!brazil_cal_allowed(year, kind, round_dates[i]) ||
            (i && round_dates[i] <= round_dates[i - 1]))
            return 0;
    if (!brazil_cal_validate(cup_events, cup_count, min_gap, NULL))
        return 0;

    const int capacity = cup_count + BRAZIL_2026_FIXTURES;
    BrazilCalendarEvent *used =
        (BrazilCalendarEvent *)malloc((size_t)capacity * sizeof *used);
    if (!used) return 0;
    if (cup_count)
        memcpy(used, cup_events, (size_t)cup_count * sizeof *used);

    int count = cup_count;
    int ok = 1;
    for (int i = 0; i < BRAZIL_2026_FIXTURES && ok; ++i) {
        const Brazil2026Fixture *fixture = &fixtures[i];
        int anchor = round_dates[fixture->round];
        int first = anchor - flex_days;
        int last = anchor + flex_days;
        if (first < 1) first = 1;
        if (last > brazil_cal_days_in_year(year))
            last = brazil_cal_days_in_year(year);
        int date = 0;
        if (!brazil_cal_find_league_day(year, kind, fixture->home, fixture->away,
                                        anchor, first, last, used, count,
                                        min_gap, &date)) {
            ok = 0;
            break;
        }
        used[count].year = year;
        used[count].day = date;
        used[count].competition = kind;
        used[count].home_club = fixture->home;
        used[count].away_club = fixture->away;
        ++count;
    }
    if (ok)
        ok = brazil_cal_validate(used, count, min_gap, NULL);
    if (ok)
        memcpy(out, used + cup_count,
               sizeof *out * BRAZIL_2026_FIXTURES);
    free(used);
    return ok ? BRAZIL_2026_FIXTURES : 0;
}

#endif
