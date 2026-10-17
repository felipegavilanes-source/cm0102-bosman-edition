# 0.6.0 research: decisions for Max

Date 2026-10-16. Research slices 1 to 10 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md` are done. This file holds every point that needs your decision. Max answered on 2026-10-16: questions 1 to 63 are Q268 to Q330 in the spec, and Q331 is the rule of no new tiers.

## Pass 2 (2026-10-17)

Max answered sections 1 to 8 below. His answers are Q268 to Q331 in the spec. Pass 2 applied them to every part, and a second-source pass and a retry looked for a second publisher for every single-source fact. Each part's `.md` lists every change.

### What changed

- No new tiers (Q331): Conference North and South, the Regionalliga from 2008/09, Serie D, Segunda Federación, the Portuguese fourth tier, the Dutch third tier, the Belgian fourth tier, the Welsh second tier, the Northern Irish and Irish third tiers, J3, K League 2, the Australian Championship and Série D left the table.
- No new nations (Q288): the Leagues Cup, Campeones Cup, CONCACAF League, AFC Cup and OFC Professional League left the table.
- Club moves now go down every tier of each nation (Q280). Clubs come from lower and spare divisions (Q279) and from database divisions whose tier ended (Q290).
- Late tournaments play in their planned slot (Q277): Euro 2020 in summer 2020, the 2020 Copa América in 2020, the 2021 Africa Cup of Nations in January 2021, the 2023 Africa Cup of Nations and the 2023 Asian Cup in June and July 2023.
- Brazil (Q283): the three composite codes now run the Cearense (northeast), Paraense (north) and Mato-Grossense (centre-west).

### How sure the table is now

- 7,074 rows in 20 parts.
- Of the filled facts with a check, 2,277 still have only one publisher (before pass 2: about 6,300). 950 more are marked inferences. 6,691 fields are `unknown` because no page gives them.
- 3 filled fields cite no page (`tools/research/rules_lint.py`).
- Sources that block automatic reads (fff.fr, transfermarkt, worldfootball.net, the Irish and Northern Irish league sites) and the web search limit of 200 per agent turn are the main causes of the rest.

### Open questions (pass 2)

1. **Brazil:** the agent also added the other 12 state championships of those regions as new competitions (rows bra-757 to bra-1080). Keep them, or keep only the three that take the codes?
2. **CONCACAF Champions Cup from 2024:** three places came through the Leagues Cup, which is out. Give them to the next MLS clubs in the league table?
3. **Suruga Bank Championship (2008 to 2019):** J.League Cup winner against the Copa Sudamericana winner, only nations a game can select. Add it?
4. **Real clubs missing from a season's database** (not reserve teams): about 200 across all parts, listed in each part's `.md`. Add them as new club records, as for reserve teams (Q289), or fill those places with clubs that have no division?
5. **Academy sides** in the EFL Trophy (from 2016/17) and the National League Cup (from 2024/25): add them as new club records?
6. **Play-offs against a tier that left the game** (for example Northern Ireland, and the 3. Liga against the Regionalliga): skip the play-off and use the table places, or play it against a club with no division, as for Scotland (Q284)?
7. **Source splits that no third source settles.** The rows keep the first value. Keep these values?

| Part | Row | Question | Value kept |
|---|---|---|---|
| sco | 111 | League Cup 2014/15 bench: 5 by the rule at the start, 7 named in the final | 5 |
| ita | 237, 254 | Third-tier super cup name 2014/15 and 2015/16 | Supercoppa di Lega Pro |
| hol-bel | 12 | Belgian Cup 2001/02 quarter-finals: one leg with replay, or one leg with extra time and penalties | one leg with replay (proposal: extra time and penalties) |
| fra | 33, 51 | CFA 2 relegations 2004/05 and 2006/07: 22 or 23 | 23 |
| rus-tur | 156 | Russian squad 2021/22: 25 or 28 | 25 |
| gre-pol-cro | 22 | Greek Cup 2022/23: 85, 84 or 93 planned | 85 |
| aus-jap-kor | 74 | Australia Cup 2024/25: 759 or 758 entrants | 759 |
| club-world | 351, 355 | Club competition substitutes and bench of the late-2020 matches | the 2019/20 rules |

### What went wrong in pass 2

- The usage limit stopped 20 agents at about 02:50 on 2026-10-17. Each was resumed at 07:16 with its own context and checked its earlier changes first.
- The web search limit of 200 per agent turn stopped several second-source agents. A retry pass took their lines again.
- Agents left backup and temporary folders outside the repo: `~/.uefa-pass2.6BjE`, `~/Library/Caches/ger-pass2.6nzhO0`, the `*-pass2.*` folders in `/Users/max/Projects/cm0102/research-0.6.0/`, and many `mktemp` folders in the system temp folder. They can be deleted when you agree.

## 1. Season database table (slice 3)

Approve the table in `2026-10-16-season-database-table.md`. It is the same as the starting table in the spec. No forum post reports a fault in any season database. The topics are locked and hold only the uploader's posts. The agent read all 733 posts of the support thread too. The check of September 2011 ODB found no more duplicate players than in the databases before and after it.

Three points:

1. October 2003 and September 2005 are not Update Team databases. Each topic names one person as the maker. The credit text of Q147 ("the Update Team for every database") is wrong for these two seasons. Choose the credit text. The brief does not let us name the makers.
2. 2002/03 uses 3.9.68 by Q233. The rule of Q263 finds no database for that season.
3. The Autumn 2013 topic gives no month.

Recognition (Q264): the size pair of `club.dat` and `staff.dat` is unique across the 49 databases. Confirm with the SHA-256 of both files. Leave `index.dat` out, because the editor rewrites it on each save. An edited known database then gets a Custom database tile.

## 2. Defaults to confirm

1. `up` and `down` hold the places by the rules of the season, not the clubs that went up or down in the end. Withdrawals, expulsions and licence refusals are in `notes`.
2. A season stopped early (2019/20, 2020/21) keeps its rule places, because a game plays every season to the end. Seasons that were suspended and then finished with relegation removed (J1 and J2 2020, Süper Lig and TFF 1. Lig 2019/20) and Argentina 2021 (relegation suspended before the start) keep 0, because 0 was the rule of that season.
3. A calendar-year league: the 2001/02 row holds 2002. Australia Cup and the Australian Championship break this rule: edition Y is in the Y/Y+1 row, because their main rounds run at the start of the A-League season.
4. A tournament or cup edition goes in the season of its first match. Some seasons therefore hold two Recopa or Sudamericana editions and some hold none. The Supercopa Internacional match of September 2026 is filed as the 2025 edition.
5. Where a league cut its places in a season after a club left (Conference North 2013/14, League Two and National League 2019/20), the row keeps the value at the season start.

## 3. Decisions that change the build

1. **Where club moves take clubs from (Q244).** In most nations the real clubs of a short division are in the database, but in a lower or spare division ("A Lower Division", "A Premier Division", the JFL, the Korean Amateur League). They are not clubs with "no division". Under Q244 as written, those divisions stay short in nearly every season. Should a club move take clubs from the lower and spare divisions of the same nation?
2. **Cascade.** A club that moves up leaves its old division short. France shows it most: every season database has 18 clubs in Ligue 1, also in the 20-club seasons, so the moves run down through tiers 2 to 5. Should the club moves fill every tier down to the lowest division that the original runs?
3. **Rule places against next season's size.** If the game applies rule places in a stopped season, it does not reach the real club counts of the next season (Regionalliga 2020/21, J1 2021 with 20 clubs, Danish 2nd Division 2020/21 with 28). Slice 18 must make the counts and the places agree. Choose: the next season takes its size from the game's own results, or from the rules table with a one-off transition.
4. **Brazilian composite championships.** `bra_reg_north`, `bra_reg_northeast` and `bra_reg_central` are not real competitions. The original makes each one from clubs of several small states. Keep them, drop them, or replace them with real competitions (Copa do Nordeste has a data record, 272, but no code)?
5. **Competitions the original lacks.** Choose which come in:
   - Scotland: the pyramid play-off with the Highland and Lowland League champions, and the guest clubs of the League Cup and Challenge Cup (B teams, Welsh, Northern Irish and Irish clubs).
   - Ireland: the A Championship (2008 to 2011, mostly reserve sides), the FAI National League (from 2026), the Setanta Sports Cup, and the President of Ireland's Cup (from 2014) in place of the old President's Cup.
   - Italy: Serie D as a playable tier.
   - The United States and Argentina: the US second tier, the Argentine third tier, the SuperLiga and the Canadian Championship.
   - Reserve and B teams, which the older databases do not hold (French CFA, Danish Superliga reserves, Spanish and Portuguese B teams, J3 U-23 sides).
6. **Divisions without a real home.** The database clubs of the Italian C2/C (2011/12 to 2013/14), the Croatian South division (from 2006/07) and the Portuguese Third Division (2013/14 to 2018/19) have no real division. From 2021/22, should `por_second_b` play the fourth tier, or should Liga 3 take its code? The Belgian agent proposes a mapping of the 2016 reform onto the database's Third A/B and Fourth A to D.
7. **UEFA.**
   - Russia: the 2027/28 access list gives places, but UEFA has decided the suspension only for 2026/27. Should Russia get places in 2027/28?
   - The single-match ties of 2019/20 and 2020/21: should the game copy them?
   - The rows count a title holder with its nation up to 2006/07 and apart from it from 2007/08. Should the game use one rule for all seasons?
   - The names "Champions Path" and "League Path" come from the regulations of 2015/16. Before that the season pages say "Champions route" and "League route" (and "non-champions path" in 2009/10 and 2010/11). Keep the later names for every season?
8. **Single matches in the data.** Bury is in League One in the October 2019 database, but Bury was expelled before 2019/20. Should a 2019/20 start remove Bury?
9. **The 2001 Copa América** runs from 11 to 29 July 2001. It is already in progress when a 2001/02 game starts. No file of the original runs the Copa América (inference from the file names), so it is new code.
10. **Unofficial matches.** The 2002 Norwegian super cup and the 2009 German Supercup were played without official status. Should the game play them?

## 4. Code findings that change the plan

These come from the sweeps (`2026-10-16-sweep-rulebooks.md`, `-sweep-season-end.md`, `-sweep-europe.md`, `-sweep-international.md`). No decision is needed now, but the build slices depend on them.

- The `rb_*` files are the discipline bodies, not rule books. Club counts, promotion and relegation places and substitutes are fields of each competition (+0x3e, +0xbe to +0xc1, +0x49, +0x4a). The sweep lists 1,431 patch sites. The Premier League sets no bench values of its own, so a change for it needs new code.
- The original finds every competition by name at load (0x60efd0). A renamed competition must get its new name after that match. A new data record never runs without new code.
- The real 2001/02 Italian fixtures and the World Cup qualifier fixtures are fixed in code with absolute dates. A later start year leaves them in 2001 and 2002.
- The season change needs match results. The rules report jump of Q265 must add a quick result for each fixture.
- The Oceania World Cup qualifying needs exactly 10 nations. Australia's move to Asia breaks it without a code change.
- The 2002 European Championship qualifying list leaves out Belgium and Holland. The reason is unknown.
- Hosts come from a 57-row table that ends between 2009 and 2028. Later hosts are random.
- Every year cycle (Confederations Cup, Africa Cup of Nations, Copa América) is a base year in code, so the 2001 cycles drift.
- The Inter-American Cup ended in 1998, but the original still runs it. The Recopa has a data record but no code.

## 5. Conflicts between sources

Each conflict is in full in the "Reconcile" section of the part's `.md` in `rules-0.6.0/`. The row keeps the first value until you decide.

| Part | Row | Question |
|---|---|---|
| eng | several | National League 2020/21 places: 3 (league maximum) or 2 (RSSSF, counts Dover's withdrawal) |
| eng | several | North and South 2019/20 = 2 and 2011/12 = 3 rest on one page each |
| eng | several | Football League matchday squad size 2009/10 to 2011/12: the sources stay split |
| wal-nir-irl | 161 | Bangor 2007/08: 3rd or 4th, and promoted by application |
| wal-nir-irl | 406 | Welsh League Division One 2017/18: 2 or 3 down |
| wal-nir-irl | 456 | Cymru South 2019/20: three sources, three answers |
| wal-nir-irl | 488 | Irish Cup 2020/21: 32 clubs in the draw or 26 that played |
| ger | 82, 83 | Regionalliga 2007/08: does the drop from tier 3 to tier 4 count as relegation? |
| ger | 164 | Regionalliga Südwest 2013/14 `down`: 2 or 3, proposal 1 |
| ita | 1 | Non-EU rule 2001/02: no limit, or 5 registered and 3 in the match list |
| ita | Lega Pro | Five-substitute trial from 2017/18: did it run in 2018/19 and 2019/20? |
| por | 39 | Segunda Divisão 2005/06 `down`: 13 by the rule at the start, 14 after a mid-season rule |
| por | 41 | Terceira Divisão 2005/06 `down` per series |
| hol-bel | 256 | Belgian First Division B 2020/21 `down`: proposal 1 |
| hol-bel | 313 | Dutch summer window 2025 opens on 11, 16 or 23 June: proposal 23 June |
| den-swe | 89 | Danish Cup 2015/16: 108 or 107 clubs |
| den-swe | 274 | Swedish third tier name in 2020: Ettan or Division 1 |
| nor-fin | 2, 4 | Norwegian bench 2002: 7 or 8 |
| rus-tur | 196 | TFF 1. Lig 2007/08 name |
| rus-tur | 301 | Süper Lig 2004/05 foreign limit: 6 or 5+1+2 |
| gre-pol-cro | 66 | Greek second tier 2012/13 `down` |
| gre-pol-cro | 160 | Greece 2025/26: 6 non-EU over 23, or 7 non-EU per squad |
| gre-pol-cro | 361 | Croatian second tier 2010/11 `down`: 4 or 1 |
| aus-jap-kor | 343 | K League 1 2020 `down`: 1, or 2 with Sangju's move fixed in advance |
| bra | clubs | Comercial FC 2002 and 2003 match (recommended: keep) and Toledo EC 2020 match |
| spa | 190 | Tercera Federación 2024/25: 325 clubs that played or 324 in the rules |

## 6. Missing clubs (Q244)

The table counts real clubs with no record in that season's database, after the club moves. Each part's `.md` lists them by season and division. No club record was added.

| Part | Clubs not in the database | Main cause |
|---|---|---|
| eng, sco, nor-fin | 0 | |
| ita | 2 | Trapani 2020/21, Rimini 2025/26 |
| spa | 9 | Segunda Federación 2025/26, Ourense CF |
| arg-usa | 45 | New MLS clubs; every database keeps the 12 MLS clubs of 2001 |
| ger | 51 | Regionalliga 2009/10 to 2016/17 |
| gre-pol-cro | 49 | B teams |
| hol-bel | 31 | |
| rus-tur | 37 | |
| aus-jap-kor | 39 | J3 and K League 2 clubs, U-23 sides |
| den-swe | 72 | 23 Superliga reserve teams |
| wal-nir-irl | 194 | Welsh second tier, Northern Irish third tier |
| fra | 222 | CFA reserve teams 2001/02 to 2012/13 |
| bra | 258 | Série D and the large early Série C |
| por | 355 | Third and fourth tiers |

## 7. How sure the table is

- 7,560 rows in 20 parts. The check agents wrote 39,473 field verdicts: 23,262 agree, 429 disagree, 513 found, and 15,269 with no second source.
- Of the 15,269, about 7,600 are `unknown` (no page gave the fact) and about 900 are inferences. About 6,300 are filled facts that only one publisher states. Most are substitutes (1,548), names (1,186) and bench sizes (703).
- Every disagreement went to a third source. The reconcile agents wrote 375 decisions for the second value, 81 for the first and 32 conflicts. These counts also hold found values and rows that copy a change. The conflicts are in section 5.
- 183 filled fields still cite no page that states them (`tools/research/rules_lint.py`). Most are the name and format of the three Brazilian composite championships, which no real page describes.

Your goal is two sources per fact. Should a further pass look for a second publisher for the 6,300 single-source facts, part by part?

## 8. What went wrong in the research

- One source-fill agent sent about 115 page requests, mostly to Wikipedia, with your email address in the browser header. It removed the address when it noticed.
- Four agents ran `rm` against brief rule 5, each on its own temporary files. No project file was lost.
- Some agents left small files in `/tmp` and the system temp folder.
- The committed doc `2026-10-16-update-team-databases.md` (lines 71 and 85) gives the October 2003 and September 2005 databases by their folder names, which name their makers. This breaks the rule on outside names. The folder names in `test-fixtures/databases` cause it.
