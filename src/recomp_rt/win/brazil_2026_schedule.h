/* Standalone, fail-closed schedule planner for Brazilian A/B 2026.
 *
 * Produces candidate calendar dates for 380 league fixtures without
 * modifying existing state, Copa do Brasil or CONMEBOL fixtures.
 *
 * A gated, untested live template now uses these candidate dates in
 * season_bra_calendar.c. The real game must still pass save/load and fixture
 * collision tests before this feature is enabled by default.
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

/* Produce exactly 38 monotonically increasing *candidate* round dates.
 * These dates are not the CBF's final fixture ordering or club pairings.
 * 2026 Série A: play before/after the men's World Cup, Série B continues.
 * 2027 both divisions pause around the women's World Cup (CBF calendar).
 * The published competition start/end windows remain authoritative.
 */
static inline int brazil_2026_candidate_round_dates(int year, int kind,
                                                    int out[BRAZIL_2026_ROUNDS])
{
    if (!out || (year != 2026 && year != 2027) ||
        (kind != BRA_CAL_SERIE_A && kind != BRA_CAL_SERIE_B))
        return 0;
    BrazilCalendarWindow window;
    if (!brazil_cal_window(year, kind, &window)) return 0;
    int dates[BRAZIL_2026_ROUNDS] = {0};
    if (window.blackout_first) {
        /* Keep 18 rounds before the break for Serie A. Serie B of 2027
         * starts later and has 17 dates before the same break. */
        const int before = kind == BRA_CAL_SERIE_B ? 17 : 18;
        const int after = BRAZIL_2026_ROUNDS - before;
        const int before_first = window.first_day;
        /* In 2026 the test baseline stops before June, and resumes with
         * the published 22 July Serie A round. 2027 uses the full
         * protected World Cup break boundaries. */
        const int before_last = year == 2026
            ? brazil_cal_day(year, 5, 31) : window.blackout_first - 1;
        const int after_first = year == 2026
            ? brazil_cal_day(year, 7, 22) : window.blackout_last + 1;
        const int after_last = window.last_day;
        if (before < 2 || after < 2 ||
            before_last <= before_first || after_last <= after_first)
            return 0;
        for (int i = 0; i < before; ++i)
            dates[i] = before_first + (before_last - before_first) * i / (before - 1);
        for (int i = 0; i < after; ++i)
            dates[before + i] =
                after_first + (after_last - after_first) * i / (after - 1);
    } else {
        for (int i = 0; i < BRAZIL_2026_ROUNDS; ++i)
            dates[i] = window.first_day +
                (window.last_day - window.first_day) * i /
                (BRAZIL_2026_ROUNDS - 1);
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
        (year != 2026 && year != 2027) ||
        (kind != BRA_CAL_SERIE_A && kind != BRA_CAL_SERIE_B) ||
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
