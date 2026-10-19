### Slice 29, 48-team World Cup (Opus), after slice 27
Waits for: slice 27 on main (spec "Blocked by: 27"). The UEFA play-off paths of 2026 also need slice 28.

Research:
- `int-world.tsv` rows of 2023/24 to 2027/28: `world_cup` (2026: 48 teams, 12 groups of 4, round of 32, 104 matches, three hosts), the six sections (2026 places: UEFA 16, CONMEBOL 6 + 1, CONCACAF 3 + 3 hosts + 2, CAF 9 + 1, AFC 8 + 1, OFC 1 + 1), the inter-confederation play-off tournament (six teams in Mexico in March 2026, two brackets of three, single matches), and the 2030 qualifying rows of 2026/27 and 2027/28. Columns `clubs`, `format`, `notes`. `int-world.md` "Change points".
- `2026-10-16-sweep-international.md` 5 (World Cup and sections), 6 (host rows: two hosts per pair), 11 (what a 48-team World Cup needs), 12.

Steps:
1. **A new World Cup class or shape** for 2026: the present sizes are immediates (32 slots 0x92bffa, 8 groups 0x92ea9d, group size 0x92d0fe, 8 stages 0x92c063, last group stage 0x92d52c, 16-team knockout 0x92d7a5), its kick-off table is a jump table per group (0x92ccd0), its knockout pairs groups A to H (0x92d770), and `set_predicted_positions` makes 32 fixed calls (0x92f680). The sections call `world_cup::add_qualified_team` 0x92e740 directly, not through the vtable, by the key id 0x9cf79c: a new class on the same key must offer the same call. The eight best thirds can use the third-place ranking of `copa_america::create_knockout_stage` 0x5e18a0.
2. **Three hosts:** a host row holds pairs of two (+6/+0xa, +0xe/+0x12, +0x16/+0x1a), and `find_hosts` 0x5fa730 returns two. Add a third host field (sweep 11).
3. **Qualifiers of 2026:** the places and formats of each section from 2023/24 (stage counts of WC Europe 0x918348..0x91836f, Africa first round sizes 0x910133..0x910147, Asia stages 0x911cf6, CONCACAF entrants 0x9163f4, Oceania exactly 10 at 0x91fa95), the CONCACAF hosts exempt, and the six-team play-off tournament (new code).
4. **After 2025/26:** the 2030 qualifying rows apply (Q248), with 48 teams. After the table the rules stay.
5. Decisions to keep: Q169, Q248, Q288 (Russia stays out of the 2026 qualifying, int-world), Q278.
6. Stop and report any row the encoder cannot read, and any 2030 format that the table gives only as an announcement.

Test seam: rules report `--year 2023 --database "<October 2023>" --last 2026` and `--year 2025 --database "<December 2025>" --last 2027`, with changing rules on across 2025/26 to 2026/27. Checks: 48 teams in 12 groups in the report, the three hosts in the host news, the play-off tournament with six teams, the qualifiers end with the right count per confederation. Same-save checks (Q342) and repeatability.

Shares: the World Cup and its sections with slices 27 and 28, the host table with slices 26 and 27. Merge order: after 27 and 28. Then run the full jump to 2027/28 once.
