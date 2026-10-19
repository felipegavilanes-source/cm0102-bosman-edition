### Slice 28, Nations League and Euro qualifying paths (Opus), after slice 27
Waits for: slice 27 on main (spec "Blocked by: 27").

Research:
- `int-continental.tsv` rows `UEFA Nations League` and `CONCACAF Nations League` (`comp_file` `new`, UEFA editions 2018-19, 2020-21, 2022-23, 2024-25 and 2026-27, CONCACAF qualifying 2018/19, then 2019-20, 2022-23, 2023-24, 2024-25 and 2026-27), `European Championship qualifying` from 2018/19, columns `clubs`, `up`, `down`, `playoffs`, `format`, `notes`. `int-continental.md` ("Nations Leagues", "Late editions": the Euro 2020 play-offs in March 2020). `int-world.tsv` UEFA qualifying rows of the 2022 cycle (play-off paths with Nations League group winners).
- `2026-10-16-sweep-international.md` 3 (key ids, `create_other_int_comps` 0x830ac0), 5 (Euro qualifying: pool 0x57f76f, hosts 0x57f6fa, play-offs `create_playoff` 0x57ef50, fixed pairs 0x57f33f), 11 (what a Nations League needs), 12.

Steps:
1. **UEFA Nations League:** a new competition (shared module) with leagues A to D, groups of 3 or 4, promotion and relegation between editions (a tier for each nation, kept across editions and in the save), finals of four in June, and from 2024-25 League A quarter-finals and promotion and relegation play-offs. Reuse the group draw by pots and the group stages of `euro_champ_qual` (0x57dab0, 0x57ed10). Decide whether it counts as a tournament in `national_teams::is_tournament` 0x76d940, `get_squad_limits` 0x76d430 and the FIFA ranking weights 0x58cf70 (sweep 11), and cite the reason.
2. **Euro qualifying with Nations League paths:** Euro 2020 (4 paths), Euro 2024 (3 paths), Euro 2028 (12 groups, two host places, play-offs in March 2028). `create_playoff` 0x57ef50 builds only two-leg ties from runners-up: write a new play-off builder for single-match paths.
3. **World Cup 2022 UEFA play-off paths** with the Nations League group winners (3 paths, single matches). Inference: these need the Nations League table, so they are here. The 2026 paths are slice 29's.
4. **CONCACAF Nations League:** a new competition with its qualifying of 2018/19, leagues A to C, and its finals.
5. Decisions to keep: Q169, Q277, Q278, Q288. The Nations League is not a club tier, so Q331 does not apply (`int-continental.md` Pass 2).
6. **Stop and report to Max:** the CONCACAF Gold Cup and the Finalissima are `new` rows of `int-continental.tsv` that no slice of the spec names. Recommended answer: build both in this slice (the CONCACAF Nations League qualifies teams for the Gold Cup).

Test seam: rules report `--year 2018 --database "<October 2018>" --last 2020` (Nations League and the Euro 2020 play-offs in March 2020), `--year 2020` (November 2020), 2022 (October 2022), 2024 (October 2024, quarter-finals), each across the next season with changing rules on. Checks: each nation is in its league, promotion and relegation between editions hold after a save and a load, the play-off paths take the right group winners. Same-save checks (Q342) and repeatability.

Shares: `euro_champ_qual` with slice 27, the WC Europe section with slices 27 and 29, the new competitions module, the season part (Nations League tiers) with slice 16. Merge order: after 27, before 29.
