/* Standalone test of atomic 380-match Brazilian schedule proposals.
 * No proprietary game binary or real 2026 CBF fixture order is required.
 */
#include <assert.h>
#include <string.h>
#include "../../src/recomp_rt/win/brazil_2026_schedule.h"

#define DAY(M,D) brazil_cal_day(2026, (M), (D))

static void test_anchor_dates(void)
{
    int a[38], b[38];
    assert(brazil_2026_candidate_round_dates(2026, BRA_CAL_SERIE_A, a) == 38);
    assert(a[0] == DAY(1, 28));
    assert(a[17] == DAY(5, 31));
    assert(a[18] == DAY(7, 22));
    assert(a[37] == DAY(12, 2));
    for (int i = 0; i < 38; ++i)
        assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_A, a[i]));
    assert(brazil_2026_candidate_round_dates(2026, BRA_CAL_SERIE_B, b) == 38);
    assert(b[0] == DAY(3, 20));
    assert(b[37] == DAY(11, 14));
    assert(brazil_cal_allowed(2026, BRA_CAL_SERIE_B, DAY(6, 20)));
    assert(!brazil_2026_candidate_round_dates(2027, BRA_CAL_SERIE_B, b));
    assert(!brazil_2026_candidate_round_dates(2026, BRA_CAL_STATE, b));
    assert(!brazil_2026_candidate_round_dates(2026, BRA_CAL_SERIE_A, NULL));
}

static void test_schedule(int kind)
{
    Brazil2026Fixture fixtures[380];
    BrazilCalendarEvent planned[380];
    BrazilCalendarEvent all[384];
    int dates[38], bad = -1;
    assert(brazil_2026_round_robin(fixtures, 380) == 380);
    assert(brazil_2026_candidate_round_dates(2026, kind, dates) == 38);

    /* Global club IDs 0-19 are the league participants in this test.
     * 100+ are clubs taking part only in outside cup fixtures. */
    const BrazilCalendarEvent locked[] = {
        {2026, DAY(1, 25), BRA_CAL_STATE, 0, 100},
        {2026, DAY(4, 8), BRA_CAL_COPA_BRASIL, 0, 101},
        {2026, DAY(5, 6), BRA_CAL_LIBERTADORES, 1, 102},
        {2026, DAY(9, 9), BRA_CAL_SUDAMERICANA, 2, 103}
    };
    /* Serie B has not started when the January state fixture occurs,
     * but the source fixture must nevertheless remain immutable. */
    const int nlocked = (int)(sizeof locked / sizeof locked[0]);
    assert(brazil_cal_validate(locked, nlocked, 3, &bad));
    assert(brazil_2026_plan_league(fixtures, 380, 2026, kind, dates,
                                   locked, nlocked, 3, 3, planned, 380) == 380);
    for (int i = 0; i < nlocked; ++i)
        all[i] = locked[i];
    for (int i = 0; i < 380; ++i)
        all[nlocked + i] = planned[i];
    assert(brazil_cal_validate(all, nlocked + 380, 3, &bad));
    for (int i = 0; i < 380; ++i) {
        assert(planned[i].year == 2026);
        assert(planned[i].competition == kind);
        assert(planned[i].home_club == fixtures[i].home);
        assert(planned[i].away_club == fixtures[i].away);
        assert(brazil_cal_allowed(2026, kind, planned[i].day));
    }
    for (int i = 0; i < nlocked; ++i) {
        assert(locked[i].year == all[i].year);
        assert(locked[i].day == all[i].day);
    }
}

static void test_fail_closed_without_output_changes(void)
{
    Brazil2026Fixture fixtures[380];
    BrazilCalendarEvent proposal[380];
    int anchors[38];
    assert(brazil_2026_round_robin(fixtures, 380) == 380);
    assert(brazil_2026_candidate_round_dates(2026, BRA_CAL_SERIE_A, anchors) == 38);
    memset(proposal, 0x53, sizeof proposal);
    BrazilCalendarEvent first = proposal[0], last = proposal[379];

    /* Club 0 cannot play on any of Jan 25..31 with a three-day gap:
     * protected fixtures 27 Jan and 30 Jan are three days apart. */
    const BrazilCalendarEvent unavailable[] = {
        {2026, DAY(1, 27), BRA_CAL_STATE, 0, 100},
        {2026, DAY(1, 30), BRA_CAL_STATE, 0, 101}
    };
    assert(brazil_cal_validate(unavailable, 2, 3, NULL));
    assert(brazil_2026_plan_league(fixtures, 380, 2026,
                                   BRA_CAL_SERIE_A, anchors,
                                   unavailable, 2, 3, 3, proposal, 380) == 0);
    assert(memcmp(&proposal[0], &first, sizeof first) == 0);
    assert(memcmp(&proposal[379], &last, sizeof last) == 0);

    /* No fixture array may be partially mutated on malformed input. */
    assert(brazil_2026_plan_league(fixtures, 379, 2026, BRA_CAL_SERIE_A,
                                   anchors, NULL, 0, 3, 3, proposal, 380) == 0);
    assert(memcmp(&proposal[0], &first, sizeof first) == 0);
    anchors[18] = DAY(6, 20); /* attempted Serie A World Cup match */
    assert(brazil_2026_plan_league(fixtures, 380, 2026, BRA_CAL_SERIE_A,
                                   anchors, NULL, 0, 3, 3, proposal, 380) == 0);
    assert(memcmp(&proposal[0], &first, sizeof first) == 0);
}

int main(void)
{
    test_anchor_dates();
    test_schedule(BRA_CAL_SERIE_A);
    test_schedule(BRA_CAL_SERIE_B);
    test_fail_closed_without_output_changes();
    return 0;
}
