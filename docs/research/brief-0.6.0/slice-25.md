### Slice 25, UEFA league phase, 2024/25 on (Opus), after slice 24
Waits for: slice 24 on main (spec "Blocked by: 24").

Research:
- `uefa.tsv` rows of 2024/25 to 2027/28 (uefa-185 on), columns `clubs`, `format` (league phase of 36: CL and EL 8 matches against 2 clubs of each of 4 pots, Conference League 6 matches against 1 club of each of 6 pots, places 1 to 8 to the round of 16, 9 to 24 to a two-leg knockout play-off), the access list text with the European Performance Spots (EPS), `notes` (seeding: pots by club coefficient), and `uefa.md` (U10, the 2027/28 unknowns: the entry rounds read from a PDF are an inference).
- `2026-10-16-sweep-europe.md` 2.3 point 5 (group stages are fixed groups of four, `League_Stage` 0x68b100 with the draws 0x585290 and 0x585ab0), 3.3 (points: `add_nation_points` 0x9058b0, the group phase bonus), `2026-10-16-sweep-season-end.md` 2.1 (`League::check_complete` 0x689ad0: complete when every row has played its matches, row +0x11 against +0xa7).

Steps:
1. **League phase stage:** one table of 36 with 8 (or 6) matches per club. Inference to prove first: a `League_Stage` of 36 with +0xa7 set to 8 completes by `check_complete`, while its fixtures come from a new draw by pots. Then the knockout play-off round (9 to 24) and the round of 16.
2. **All three competitions** in one table per season, from slice 24's objects.
3. **European Performance Spots:** two more Champions League places for the two nations with the best coefficient of the season before, from the points of that season (seeding entry +0xf points and +0x11 clubs entered, sweep 3.1).
4. **2025/26 to 2027/28:** the rows apply, and after 2027/28 the rules stay (Q248).
5. **Stop and report to Max:** Russia in 2027/28. The access list PDF gives Russia places, UEFA decided the suspension only to 2026/27 (`uefa.md`, "Conflicts for Max" 1). Recommended answer: keep Russia suspended, because no return is announced (Q248).

Test seam: rules report `--year 2024 --database "<October 2024>" --last 2025` and `--year 2025` (December 2025) `--last 2027`, a season change run across 2023/24 to 2024/25 with changing rules on, a check that each league phase table holds 36 clubs with 8 (or 6) matches each and that the knockout play-off has the clubs of places 9 to 24. Same-save checks (Q342) and repeatability.

Shares: as slice 24. Merge order: after 24. This is the last UEFA slice, so run the full jump to 2027/28 here as well.
