# Brief for the 0.6.0 research agents

Every research agent of slices 1 to 10 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md` gets this brief. Read the spec, `GLOSSARY.md` and `docs/research/2026-10-16-season-change-points.md` first.

## Rules for every agent

1. Research only. Write only the files that your task names. Do not change other files. Do not commit.
2. Never name another modder, their patch or their tool in any file or reply. Name a database by its month and year only (for example "October 2003"), never by the extra word in its folder name. Never write the name of a forum poster.
3. Never copy code, pseudocode or data files of the original into the repo. Addresses, names and plain facts are allowed.
4. If you cannot find a fact, write `unknown`. Do not guess. Mark an inference as an inference.
5. Do not run `rm` with a wildcard or a variable path.
6. Final reply: at most 300 words, with the paths of the files that you wrote.

## Rules for the rules table (slices 4 to 10)

- **Seasons:** one row per league or competition per season, from 2001/02 to 2027/28 (27 seasons). For a calendar-year league, the 2001/02 row holds the year 2002, and so on. The column `real_season` holds the real label ("2002" or "2001/02").
- **Scope:** every competition that the original runs for your nations: the source files in `docs/research/function-map/index.md` with your nation prefix (for example `eng_*`, `sco_*`). Add the real competitions of the same tiers that the original lacks (for example Conference North, 3. Liga, Serie C groups), with `comp_file` = `new`. A season in which a competition does not run gets a row with `format` = `not held`.
- **Sources:** each fact carries the URL of a page that you read today. Wikipedia season pages are a good first source. WebFetch passes a page through a summary model that can get numbers wrong: ask it to quote the sentence or the table row, or read the raw page with `curl -s -A "Mozilla/5.0"` and grep it.
- **Rows:** write a tab-separated file with this header and one row per competition per season:
  `id season real_season nation comp_file comp tier name clubs up down playoffs split format subs_used bench foreign work_permit registration windows notes sources`
  - `id` is `<part>-<n>`. `name` is the name of the competition in that season. `up`/`down` are direct places by the rules of that season, not the clubs that went up or down in the end. Withdrawals, expulsions and licence refusals go in `notes`. A season stopped early (2019/20, 2020/21) keeps its rule places, with what happened in `notes`, because a game plays every season to the end (defaults set on 2026-10-16, for Max to confirm). `playoffs` says who plays off for what. `split` describes a championship or relegation split. `format` describes a cup (rounds, legs, replays, extra time, penalties, entrants).
  - Nation-wide rules (foreign players, work permits, squad registration, transfer windows) go in one row per season with `comp` = `nation rules`. Substitutes and bench size go in each competition's row.
  - `sources` lists `field=URL` pairs separated by ` ; `. Use `all=URL` when one page gives every fact of the row. Every filled fact field must be covered.
  - Use `n/a` for a field that does not apply. Use `unknown` for a fact that you could not find.
- **Club moves:** clubs per division of each season database are in `/Users/max/Projects/cm0102/research-0.6.0/db-divisions/` (`<database>.divisions.tsv` and `<database>.clubs.tsv`). The database of each season is the starting table in the spec. For each season start, compare the real club count of each division with the database count. Where they differ, or the division is new, list every real club of that division in that season in `<part>-clubs.tsv`:
  `season division club db_club_id db_club db_division source`
  Leave `db_club_id` empty when the database holds no such club. Take a club for a short division only from that nation's clubs that have no division in the database (Q244).
- **Missing clubs:** list each division that still lacks clubs after that step in your notes file. Max decides them. Never add club records.
- **Second source:** a second Opus agent checks every row. It writes `<part>.check.tsv` with the header `id field verdict second_url note`, where `verdict` is `agree`, `disagree` or `no second source`. Its second source must come from another publisher than the first source of that fact (for example RSSSF, worldfootball.net, the league's own site or a national newspaper). It also writes `<part>.check.md` with the disagreements. It does not change the first agent's files.

- **Reconcile:** a third Opus agent takes each `disagree` of the check file. It reads a third publisher, or the primary record (full table, match report or rule text) when no third publisher exists. It decides `first`, `second` or `conflict` (sources stay split, or a question of reading). It accepts a `found` value only if its page states it. It writes the settled and accepted values into `<part>.tsv` with their URLs, fixes other fields that the change makes inconsistent (also in rows that copy it), applies club-match corrections to `<part>-clubs.tsv`, and keeps row order and ids. It writes `<part>.reconcile.tsv` (`id field first_value second_value third_url decision note`) and a "Reconcile" section at the end of `<part>.md` with the counts and every conflict in full.

## Rules for the code sweeps (slices 1 and 2)

- Use the REA tools on `/Users/max/Projects/cm0102/clean-3968/cm0102.exe` (load them with ToolSearch, call `open_binary` first). Read the assembly before you state a signature: Ghidra pseudocode drops the thiscall ECX.
- The function map docs give the file and name of each function. A row with model `opus` is a checked name. A `haiku` or `slot copy` row is a first-pass guess: read the code before you rely on it (`2026-10-16-research-name-check.md`). Do not run `write_docs.py`: the function map sweeps own it. Write the proven names as JSON lines (fields: address, name, file, convention, purpose, evidence, confidence, model) to `/Users/max/Projects/cm0102/research-0.6.0/sweeps/<sweep>.jsonl`, and a doc in the repo.
- Name each fact's evidence by address. Put first what the build slices 11 to 29 need.

## Files

All rules files go to `docs/research/rules-0.6.0/`. Parts:

| Slice | Part | Scope |
|---|---|---|
| 1 | sweep-rulebooks | `rb_*` files |
| 1 | sweep-rules | `*_rules` files, transfer rules, the end-of-season step, the year constants |
| 2 | sweep-europe | UEFA and other club competitions, seeding, `world_club_*` |
| 2 | sweep-international | `world_cup`, `wc_*`, `euro_champ*`, `fifa_confed`, `goldcup`, nations cups, `olympics`, `host_country`, `rb_international` |
| 3 | databases | the season database table |
| 4 | eng; sco; wal-nir-irl | England; Scotland; Wales, Northern Ireland, Republic of Ireland |
| 5 | ita; spa; fra; por | Italy; Spain; France; Portugal |
| 6 | ger; hol-bel; den-swe; nor-fin | Germany; Holland and Belgium; Denmark and Sweden; Norway and Finland |
| 7 | rus-tur; gre-pol-cro | Russia and Turkey; Greece, Poland and Croatia |
| 8 | bra; arg-usa; aus-jap-kor | Brazil (with the state championships); Argentina and the United States; Australia, Japan and South Korea |
| 9 | uefa; club-world | UEFA competitions; the other confederations' club competitions and FIFA club competitions |
| 10 | int-world; int-continental | World Cup, qualifiers, Confederations Cup, Olympics; continental championships, Nations Leagues and their qualifiers |

Each rules part writes `<part>.tsv`, `<part>-clubs.tsv` (where club moves apply) and `<part>.md` (method, sources, missing clubs, unknowns). The lead session writes one doc per slice that links its parts, and the list for Max.

## Pass 2 (2026-10-17): Max's decisions

Read the section "Research decisions (Q268 to Q331)" in the spec first. Keep row ids. Run `tools/research/rules_lint.py` at the end, and leave your part with no more problems than before. Each part's `.md` gets a section "Pass 2" with every change.

**Decision agent (one per part):**
1. **No new tiers (Q331) and no new nations (Q288).** A nation's lowest tier is the lowest tier of its competition files in the original. Delete the rows, and the club rows, of every competition below that tier, and of every competition that needs a nation a game cannot select. Keep the real competitions of the existing tiers. When a real tier ends, that tier's rows from then on say `not held`.
2. **Conflicts (Q293, Q295 and Q297 to Q327).** Settle every conflict and every open point for Max in the part's `.md` by what happened in real life, read in the light of Q272 to Q276 and Q278. Give the URL.
3. **Club moves (Q279, Q280, Q290).** For each season with a database, go down every tier from the top to the nation's lowest tier. Where the real club count differs from the database count, list every real club of that tier in that season. The pool is clubs with no division, clubs in lower or spare divisions of that nation, and clubs of database divisions whose real tier has ended. A club moved up leaves a gap below it: fill that tier too.
4. **Reserve and B teams (Q289).** List each real reserve or B team that a tier needs and the season's database lacks. The build adds them as new club records.
5. **Part rules.** Brazil: replace the three composite championships with the real competitions of those regions (Q283). UEFA: two legs in 2019/20 and 2020/21 (Q294), and the names Champions Path and League Path in every season (Q296). Scotland: the pyramid play-off takes two clubs with no division (Q284). A late edition plays in its normal slot (Q277).

**Second-source agent (one per part, after the decision agent):** take every check line with verdict `no second source` whose field still holds a value (not `unknown`, not an inference). Find a page of another publisher that states the value and read it today. Write `<part>.check2.tsv` (`id field verdict second_url note`). For a `disagree`, read a third source and settle it as in the Reconcile rule, and write the result into `<part>.tsv`.
