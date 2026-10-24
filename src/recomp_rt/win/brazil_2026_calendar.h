/* Brazilian competition calendar guard for modern seasons.
 * Pure C99: no game memory, no side effects, no existing cup rewritten.
 *
 * National league dates coexist with state championships, Copa do Brasil,
 * Libertadores and Sul-Americana. Their competition windows OVERLAP by
 * design; the same CLUB must never play two matches on the same day.
 * A 3-calendar-day gap between matches is the scheduling default, not
 * a claim about the official CBF minimum rest measured in hours.
 *
 * 2026 source dates:
 * CBF: calendar announced 2025-10-01, national league/Copa do Brasil
 *   regulations published 2025-12-15 and 2026-02-06.
 * CONMEBOL: 2026 dates announced 2025-07-28.
 * 2027 source: CBF calendar published in October 2026. Continental and
 * state 2027 dates are intentionally NOT invented; existing game dates
 * must remain authoritative until the corresponding calendars are known.
 */
#ifndef RECOMP_BRAZIL_2026_CALENDAR_H
#define RECOMP_BRAZIL_2026_CALENDAR_H

enum BrazilCalendarKind {
    BRA_CAL_STATE = 1,
    BRA_CAL_SERIE_A = 2,
    BRA_CAL_SERIE_B = 3,
    BRA_CAL_SERIE_B_ACCESS = 4,
    BRA_CAL_COPA_BRASIL = 5,
    BRA_CAL_LIBERTADORES = 6,
    BRA_CAL_SUDAMERICANA = 7
};

typedef struct BrazilCalendarWindow {
    int first_day;           /* 1-based day of calendar year */
    int last_day;
    int maximum_matchdays;   /* 0 when not established by this policy */
    int fixed_day_one;       /* optional: a mandatory specific matchday */
    int fixed_day_two;
    int blackout_first;      /* 0 if no authoritative protected break */
    int blackout_last;
} BrazilCalendarWindow;

typedef struct BrazilCalendarEvent {
    int year;                /* real calendar year */
    int day;                 /* 1-based day of year */
    int competition;         /* enum BrazilCalendarKind */
    int home_club;           /* nonnegative global club ID, not division rank */
    int away_club;
} BrazilCalendarEvent;

static inline int brazil_cal_leap(int year)
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static inline int brazil_cal_days_in_year(int year)
{
    return brazil_cal_leap(year) ? 366 : 365;
}

static inline int brazil_cal_day(int year, int month, int day)
{
    static const int months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 2001 || year > 2099 || month < 1 || month > 12) return 0;
    int limit = months[month - 1] + (month == 2 && brazil_cal_leap(year));
    if (day < 1 || day > limit) return 0;
    int ordinal = day;
    for (int m = 1; m < month; ++m)
        ordinal += months[m - 1] + (m == 2 && brazil_cal_leap(year));
    return ordinal;
}

/* Gregorian continuous day count to compare fixtures across year rollover. */
static inline int brazil_cal_serial(int year, int day)
{
    if (year < 2001 || year > 2099 || day < 1 || day > brazil_cal_days_in_year(year))
        return -1;
    int previous = year - 1;
    return 365 * previous + previous / 4 - previous / 100 + previous / 400 + day;
}

/* Known authoritative windows only. Unknown 2027 state/continental schedules
 * return 0, so callers MUST keep those legacy competitions unchanged. */
static inline int brazil_cal_window(int year, int kind, BrazilCalendarWindow *out)
{
    if (!out || (year != 2026 && year != 2027)) return 0;
    BrazilCalendarWindow w = {0};
    if (year == 2026) {
        switch (kind) {
        case BRA_CAL_STATE:
            w.first_day = brazil_cal_day(year, 1, 11);
            w.last_day = brazil_cal_day(year, 3, 8);
            w.maximum_matchdays = 11;
            break;
        case BRA_CAL_SERIE_A:
            w.first_day = brazil_cal_day(year, 1, 28);
            w.last_day = brazil_cal_day(year, 12, 2);
            w.blackout_first = brazil_cal_day(year, 6, 11);
            w.blackout_last = brazil_cal_day(year, 7, 19);
            break;
        case BRA_CAL_SERIE_B:
            w.first_day = brazil_cal_day(year, 3, 20);
            w.last_day = brazil_cal_day(year, 11, 14);
            /* Unlike Serie A, Serie B plays DURING the men's World Cup. */
            break;
        case BRA_CAL_SERIE_B_ACCESS:
            w.first_day = brazil_cal_day(year, 11, 21);
            w.last_day = brazil_cal_day(year, 11, 28);
            w.fixed_day_one = w.first_day;
            w.fixed_day_two = w.last_day;
            break;
        case BRA_CAL_COPA_BRASIL:
            w.first_day = brazil_cal_day(year, 2, 18);
            w.last_day = brazil_cal_day(year, 12, 6);
            break;
        case BRA_CAL_LIBERTADORES:
            w.first_day = brazil_cal_day(year, 2, 4);
            w.last_day = brazil_cal_day(year, 11, 28);
            break;
        case BRA_CAL_SUDAMERICANA:
            w.first_day = brazil_cal_day(year, 3, 4);
            w.last_day = brazil_cal_day(year, 11, 21);
            break;
        default:
            return 0;
        }
    } else {
        switch (kind) {
        case BRA_CAL_SERIE_A:
            w.first_day = brazil_cal_day(year, 1, 27);
            w.last_day = brazil_cal_day(year, 12, 1);
            break;
        case BRA_CAL_SERIE_B:
            w.first_day = brazil_cal_day(year, 3, 20);
            w.last_day = brazil_cal_day(year, 11, 27);
            break;
        case BRA_CAL_COPA_BRASIL:
            w.first_day = brazil_cal_day(year, 1, 6);
            w.last_day = brazil_cal_day(year, 12, 5);
            break;
        default:
            return 0; /* no made-up 2027 CONMEBOL or state windows */
        }
        /* CBF 2027: women's World Cup pauses Serie A, B, Copa do Brasil. */
        w.blackout_first = brazil_cal_day(year, 6, 24);
        w.blackout_last = brazil_cal_day(year, 7, 25);
    }
    *out = w;
    return 1;
}

static inline int brazil_cal_allowed(int year, int kind, int day)
{
    BrazilCalendarWindow w;
    if (!brazil_cal_window(year, kind, &w) ||
        day < w.first_day || day > w.last_day) return 0;
    if (w.fixed_day_one && day != w.fixed_day_one && day != w.fixed_day_two)
        return 0;
    if (w.blackout_first && day >= w.blackout_first && day <= w.blackout_last)
        return 0;
    return 1;
}

/* For already-existing 2027 state/continental fixtures, the exact dates
 * are not established by this adapter. Accept their saved calendar days as
 * immutable inputs to the clash checker, rather than deleting/rejecting them.
 * A NEW 2027 continental/state fixture cannot be created by this module.
 */
static inline int brazil_cal_existing_event_allowed(int year, int kind, int day)
{
    BrazilCalendarWindow w;
    if (brazil_cal_window(year, kind, &w))
        return brazil_cal_allowed(year, kind, day);
    return year == 2027 && day >= 1 && day <= brazil_cal_days_in_year(year) &&
           (kind == BRA_CAL_STATE || kind == BRA_CAL_LIBERTADORES ||
            kind == BRA_CAL_SUDAMERICANA);
}

static inline int brazil_cal_shared_club(const BrazilCalendarEvent *a,
                                        const BrazilCalendarEvent *b)
{
    return a->home_club == b->home_club || a->home_club == b->away_club ||
           a->away_club == b->home_club || a->away_club == b->away_club;
}

/* min_gap = 3 means games at least 3 *calendar days* apart.
 * Unknown or invalid dates/competition windows fail closed.
 */
static inline int brazil_cal_validate(const BrazilCalendarEvent *events,
                                      int count, int min_gap, int *bad_index)
{
    if (bad_index) *bad_index = -1;
    if (count < 0 || count > 10000 || min_gap < 1 || min_gap > 10 ||
        (count && !events)) return 0;
    for (int i = 0; i < count; ++i) {
        const BrazilCalendarEvent *a = &events[i];
        if (a->home_club < 0 || a->away_club < 0 ||
            a->home_club == a->away_club ||
            !brazil_cal_existing_event_allowed(a->year, a->competition, a->day)) {
            if (bad_index) *bad_index = i;
            return 0;
        }
        int day_a = brazil_cal_serial(a->year, a->day);
        for (int j = 0; j < i; ++j) {
            const BrazilCalendarEvent *b = &events[j];
            if (!brazil_cal_shared_club(a, b)) continue;
            int distance = day_a - brazil_cal_serial(b->year, b->day);
            if (distance < 0) distance = -distance;
            if (distance < min_gap) {
                if (bad_index) *bad_index = i;
                return 0;
            }
        }
    }
    return 1;
}

/* Reserve a new national-league match, without modifying the existing cup,
 * state and continental fixtures. The preferred date is tried first; if
 * occupied, prefer later alternatives, then earlier, within [first,last].
 * No calendar is modified here; caller receives the selected day.
 * Do not use for compulsory-date Serie B playoffs or existing cup rounds.
 */
static inline int brazil_cal_find_league_day(int year, int kind, int home_club,
                                             int away_club, int preferred,
                                             int first, int last,
                                             const BrazilCalendarEvent *existing,
                                             int existing_count, int min_gap,
                                             int *result_day)
{
    if (!result_day || (kind != BRA_CAL_SERIE_A && kind != BRA_CAL_SERIE_B) ||
        home_club < 0 || away_club < 0 || home_club == away_club ||
        existing_count < 0 || existing_count > 10000 ||
        (existing_count && !existing) || min_gap < 1 || min_gap > 10 ||
        preferred < 1 || preferred > brazil_cal_days_in_year(year) ||
        first < 1 || last > brazil_cal_days_in_year(year) ||
        first > last || preferred < first || preferred > last)
        return 0;
    /* Never ignore a corrupt fixture input or a wrong real calendar year. */
    for (int i = 0; i < existing_count; ++i) {
        if (existing[i].home_club < 0 || existing[i].away_club < 0 ||
            existing[i].home_club == existing[i].away_club ||
            !brazil_cal_existing_event_allowed(existing[i].year,
                                                existing[i].competition,
                                                existing[i].day))
            return 0;
    }
    for (int offset = 0; offset <= last - first; ++offset) {
        int candidate[2] = {preferred + offset, preferred - offset};
        for (int c = 0; c < (offset ? 2 : 1); ++c) {
            int day = candidate[c];
            if (day < first || day > last || !brazil_cal_allowed(year, kind, day))
                continue;
            BrazilCalendarEvent target = {year, day, kind, home_club, away_club};
            int fits = 1;
            for (int i = 0; i < existing_count; ++i) {
                if (!brazil_cal_shared_club(&target, &existing[i])) continue;
                int distance = brazil_cal_serial(year, day) -
                               brazil_cal_serial(existing[i].year, existing[i].day);
                if (distance < 0) distance = -distance;
                if (distance < min_gap) {
                    fits = 0;
                    break;
                }
            }
            if (fits) {
                *result_day = day;
                return 1;
            }
        }
    }
    return 0;
}

#endif
