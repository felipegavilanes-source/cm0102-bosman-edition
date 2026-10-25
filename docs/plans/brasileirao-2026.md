# Brazilian Serie A and Serie B — 2026 implementation checklist

Status: standalone pure-C promotion rules, home/away pairing, and fail-closed original-league status adapter added; GitHub Actions standalone tests PASS. Live match-engine wiring and executable season tests remain NOT DONE.

## Goal
New game starts in calendar year 2026 with the 2026 Brazilian league structure while keeping original match engine and a separately playable stable v0.5 installation.

## Competition requirements
- Serie A: 20 clubs, double round robin, 38 matches each, champion by table, bottom four relegated.
- Serie B: 20 clubs, double round robin, 38 matches each, bottom four relegated.
- Serie B automatic promotion: positions 1 and 2.
- Serie B promotion ties: 3rd vs 6th and 4th vs 5th, each home-and-away; better-placed club hosts second leg. **No final.** When aggregate scores are level, higher regular-season placement wins; no penalty shootout.
- Dates: use 2026 CBF schedule and ensure fixtures and promotions resolve before rollover. Avoid hardcoding 2026 dates for subsequent seasons without an explicit policy.

## Existing code to inspect and modify
- `docs/research/rules-0.6.0/bra.tsv`: season rule data; ensure 2026 rows and source references are correct.
- `src/recomp_rt/win/season_sizes.c`: division sizes, exchange of clubs, 20-team fixture generation.
- `src/recomp_rt/win/season_playoffs.c`: existing four-team playoff creates a final and promotes **one** winner; must NOT be reused unchanged for Serie B 2026.
- `src/recomp_rt/win/season_league_table.c`: generated from tools/seasons/encode_leagues.py; never edit by hand.
- `src/recomp_rt/win/season_change.c`: 2026 starting year, rollover, save/load invariants.
- Season selection and calendar modules: inspect and use existing 0.6.0 architecture rather than patching 0.5 binaries.

## Required implementation sequence
1. Verify current fork/upstream 0.6.0 code and source assets, feature flags, Brazilian competition IDs and CBF 2026 participants.
2. Add explicit Brazilian Serie B 2026 playoff shape (two independent two-leg ties, two promotions, no final) and its correct aggregate tie-break. Keep original behavior for every other competition.
3. Ensure 20-team, double-round-robin Serie A and Serie B fixtures, four relegations each, and balanced promotion/relegation including Serie C.
4. Start new games in 2026 with matching club membership and date, then simulate through rollover to 2027.
5. Add deterministic unit/integration tests: fixtures (380 per league), table positions, tie-breaks, playoff pairings, two winners, four up/down, saves before and during playoffs, reload, and multi-season stability.
6. Run CI and actual game smoke tests before making any release claim. Do not merge to main or distribute an executable until passing.

## Safety
- Isolated branch: feature/brasileirao-2026.
- Preserve existing game engine, original database modes and stable release.
- No claim of working gameplay until a built executable has been tested.
- Modern rosters are a separate data task; season/rule changes alone do not update player squads.

## Sources
- https://www.cbf.com.br/futebol-brasileiro/noticias/undefined/vitoria-x-flamengo/cbf-divulga-documentos-tecnicos-do-brasileirao-da-serie-b-de-2026
- https://stcbfsiteprdimgbrs.blob.core.windows.net/img-site/cdn/REC_Brasileiro_Serie_B_2026_85d55b9f72.pdf
- https://stcbfsiteprdimgbrs.blob.core.windows.net/img-site/cdn/Tabela_Basica_Brasileiro_Serie_B_2026_efe6ba77b5.pdf

## Progress 2026-10-24
- Added `src/recomp_rt/win/brazil_2026_promotion.h`: zero-based pairings 3rd/6th and 4th/5th; aggregate scoring; higher-ranked side advances on level aggregate.
- Added `tests/seasons/brazil_2026_promotion_test.c`: standalone compile-and-run assertions; not yet executed by CI.
- Existing `season_playoffs.c` explicitly excludes two-winner playoffs; a dedicated runtime stage and promotion handling remain necessary.

## Progress — second implementation step (24 October 2026)
- `brazil_2026_promotion_venue` assigns the lower-ranked team home in leg 1 and the higher-ranked team home in leg 2.
- `brazil_2026_promotion_apply` is a fail-closed adapter for the game's League table-row status byte: status 0 / 5 are promoted, 1 is a play-off club and 3 is relegated. It accepts only a 20-club table, two valid winners (3rd-or-6th and 4th-or-5th), two direct promotion slots, and four correctly marked playoff rows. It changes only the two winners to promoted.
- Expanded standalone tests verify aggregate goals, level-aggregate higher-ranking tiebreak, venues, 4 up / 4 down and rejection without partial mutations.
- Added `.github/workflows/brasileirao-2026.yml` that executes Python source-rule tests and compiles/runs the C unit tests. **PASS**: https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40095902227
- This is **not** a game release; the adapter does not receive actual fixture results from the game yet.

### Critical engine limitation found
The underlying 2002 Brazilian competition classes `BraNatFirst` and `BraNatSecond` are different from the generic `plain_league` covered by `season_playoffs.c`. Per `docs/research/2026-10-18-league-sizes.md`, their original engine assumes one meeting per pair and a historical top-eight knockout, instead of 20 teams with 38 league games. The existing season sizes/rules metadata alone does not remove that original format. Their `set_up_rules` entrypoints are 0x42a5a0 and 0x42cb70 (season_size_wraps.c); do not forcibly change the stage counts or match-count fields without a fixture builder and game smoke tests.

### Next runtime implementation, in safe order
1. Reverse-engineer both Brazilian fixture builders and stage builders. Design a 20-club, 38-match calendar and ensure the original top-eight knockout is disabled **only in 2026+ season games**, not in the historical 2002 game.
2. Build a save/load-safe Brazilian Série B cup stage containing exactly two independent two-leg ties, no final, with an on-disk stage object. Connect completed tie outcomes to `brazil_2026_promotion_apply` and the promotion/relegation chain.
3. Verify 380 regular-season fixtures in both divisions, the two extra home-and-away ties, table status, four up/down, and the change into 2027 after loading saved games during the tie.
4. Only after actual compiled-game tests and multi-season simulation pass should the feature be enabled as a normal playable option. Until then, this PR stays a draft.

### England second-tier analogue (24 October 2026)
The historical English First Division (today's Championship) is the relevant existing game-engine mechanism, **not** England's third-tier League One. The reverse-engineering documentation `docs/research/2026-10-18-playoffs-splits-calendar.md` identifies these existing entrypoints:
- `EngFirst::create_playoff_stage` 0x573080: 4 participants seeded [6th, 3rd, 5th, 4th], with two-leg semi-finals.
- `EngFirst` round table 0x571980: two rounds including a **final**; the final must be omitted for Brazil.
- `Cup_Stage` ctor 0x522e00 and setup 0x51c800: potential reusable stage and match setup components, with their original save/load format.
- The English result handler 0x576c50 marks only the **final winner** promoted; Brazil must instead mark **both semi-final winners** promoted and both losers as non-promoted.
- The English semi-final tie-break cannot simply be used: CBF 2026 requires the higher regular-season position to advance on equal aggregate score, without penalties.
- Legacy Brazil league classes `BraNatFirst` / `BraNatSecond` use separate calendar/fixture builders. Reusing the English stage is **not** a substitute for fixing Brazil's 38 regular-season matches.

This fork now has `brazil_2026_promotion_english_slots`, a small zero-based position adapter that produces the English stage seeding [5,2,4,3]. The C unit tests assert the layout. It performs **no unsafe runtime hook**.

Verified external rule descriptions: https://www.cbf.com.br/futebol-brasileiro/noticias/undefined/vitoria-x-flamengo/cbf-divulga-documentos-tecnicos-do-brasileirao-da-serie-b-de-2026 ; https://ge.globo.com/pe/futebol/brasileirao-serie-b/lista/2026/02/06/serie-b-cbf-divulga-tabela-e-datas-dos-novos-playoffs-de-acesso.ghtml

Further engine integration requires locating a one-round `Cup_Stage` configuration that does not auto-advance to a final, verifying the event callback contract, and running actual save/load and full-season simulation before enabling it.


### Premier League reference for Série A: tested source milestone (24 October 2026)
The historical game engine's **English Premier Division (EngPrm)** is a better structural comparator for Série A than the English First Division (EngFirst), which uses promotion playoffs. EngPrm's table builder allocates 20 rows and its fixture schedule uses 38 dates (original game addresses 0x574a2b and 0x57369a, documented in docs/research/2026-10-18-league-sizes.md). Do **not** reuse English relegation counts (England relegates 3; Brazil 2026 relegates 4) or August-to-May dates.

**Committed and tested:**
- \`src/recomp_rt/win/brazil_2026_league.h\`: pure-C modern profile for \`bra_nat_first\` and \`bra_nat_second\`, selected by **real calendar year >= 2026**. Both leagues have 20 clubs, two meetings per pair, 38 rounds, 380 scheduled league fixtures, and \`title_playoff_rounds = 0\`. Série A has four relegations and *no title playoffs*. Série B has two automatic + two playoff promotions and four relegations.
- The profile carefully distinguishes actual 2026 from the data key \`2025/26\` (season-key year 2025). A caller must supply the **real edition year**, not the year from the key.
- A deterministic 20-team 38-round circle-method reference fixture sequence; every pair meets home and away exactly once. This is a topology model, **not** the official CBF 2026 fixture date/order.
- \`brazil_2026_fixtures_valid\` rejects missing fixtures, repeats, self-matches, duplicate rounds for a club, and wrong home/away totals.
- \`tests/seasons/brazil_2026_league_test.c\` asserts 20/38/380, no championship knockout, exact home/away coverage, future 2027 profile, and rejected invalid fixture samples. Registered with CTest along with the existing promotion and TSV tests.
- Source CI also syntax-checks \`season_sizes.c\`, without an original EXE. CI PASS: https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40100618494

**Read-only live audit instrumentation:** In \`season_sizes.c\`, the existing BraNatFirst/Second \`set_up_rules\` wrapper logs club count, meetings per pair (+0x3c), calendar date slots (+0xa9), matches per club (+0xa7), stage count (+0x2c), stage index (+0x30), promotion zones (+0xbe/+0xbf), and relegations (+0xc1) only when \`RECOMP_BRAZIL_2026_AUDIT=1\`. This performs **zero writes**, so it does not risk corrupting save games. On a future runnable experimental build, capture the game's log with this option to determine where the 2002 title knockout remains active.

**Important:** The above does not yet disable knockout stages in the game's running Brazilian class. The original classes' schedule still has the historic 27-date limit and a top-eight final. Removing a stage count alone, or blindly forcing +0x3c to 2, may end a season early or overrun a 27-date template. The next safe game-engine step must fix the Brazilian schedule/date allocator, stage lifecycle and save/load together, behind strict season-only guards. Do not advertise the 0.6.0 fork as a playable modern Brasileirão until an original-EXE build and 2026→2027 multi-season smoke test pass.


## Brazilian integrated competition calendar — 2026 and 2027 (24 October 2026)

**Correction (user-tested Brazilian start, 24 October 2026):** The game's initial date is **not universally July**. The existing original game selects a start date from the first selected league-table entry (`setup_pick_start_date` 0x830520) and `create_game` starts approximately four weeks earlier (0x828480). For a Brazil-selected game, the user observes a **January start** already. The old warning that a Brazil-specific January-start feature was *necessarily* missing was wrong and is withdrawn. Research `docs/research/2026-10-16-sweep-season-end.md` §3.1 also documents that calendar-year league entries use **year + 1**, while the data row describing real Brazil 2026 is labelled `2025/26`. **Follow-up test, not a prerequisite rewrite:** verify the exact playable first day of a Brazil-only `2025/26` selected-season new game, Brazilian state fixtures, Série A opening rounds and the treatment of mixed Brazil + European league selections. Note that the "Choose Season" preview UI currently has a fixed `Starts July %d` label at `season_screen.c:659`, which can be misleading about the actual start of a Brazil-only game. Avoid changing the underlying nation-sensitive start-date picker without a reproducible failing test.

### Official season-wide dates
- **State leagues:** 11 Jan–8 March 2026, maximum 11 national match dates per state championship. A's season starts while states are still playing, so state/A league **windows can overlap**, though one club cannot be double-booked on the same date.
- **Serie A:** 28 Jan–2 Dec, 38 rounds, World Cup men's interruption 11 June–19 July; 19th round starts 22 July. Standings decide champion, no title playoff, four relegated.
- **Serie B:** from 20 March, round 38 on 14 Nov. Continues playing during men's 2026 World Cup. After league table closes: **21 and 28 Nov**, the two direct home/away promotion playoff ties. Winners of each are promoted without a final; higher regular-season ranking advances when aggregate is tied. Four bottom clubs relegated.
- **Copa do Brasil:** from 18 Feb to single-match final 6 Dec; 126 participants in 2026, the 20 Serie A clubs enter at stage 5. Keep existing cup fixtures and qualification entries, do not replace with league rounds.
- **Libertadores:** qualification begins 4 Feb, groups in April/May, final 28 Nov; respect club-specific match dates. Preserve qualification/entrants and ranking from the continental code.
- **Sul-Americana:** qualification begins 4 March, groups in April/May, July knockout playoff, final 21 Nov; respect club-specific match dates.
- **2027:** CBF published the next calendar in Oct 2026: Serie A 27 Jan–1 Dec, Serie B 20 March–27 Nov, Copa do Brasil 6 Jan–5 Dec, with a protected break **24 June–25 July 2027** for these three competitions during the Women's World Cup. Exact state and CONMEBOL 2027 windows are intentionally NOT inferred. Their already existing game fixtures must not be silently dropped.

Sources: https://www.cbf.com.br/a-cbf/noticias/informes-cbf/a/cbf-anuncia-novo-calendario-do-futebol-profissional-masculino ;
https://www.cbf.com.br/futebol-brasileiro/noticias/undefined/portuguesa-x-parnahyba/cbf-divulga-documentos-tecnicos-do-brasileirao-da-serie-b-de-2026 ;
https://www.cbf.com.br/futebol-brasileiro/noticias/undefined/ponte-preta-x-ituano/cbf-divulga-tabela-basica-plano-geral-de-acoes-e-regulamento-especifico-da-copa-do-brasil-2026 ;
https://ge.globo.com/rs/futebol/noticia/2025/07/28/conmebol-divulga-calendario-de-2026-final-da-libertadores-sera-em-28-de-novembro.ghtml ;
https://www.cbf.com.br/futebol-brasileiro/noticias/noticias/campeonato-brasileiro/cbf-oficializa-calendario-do-futebol-profissional-masculino-de-2027

### Code added in this experimental branch
- src/recomp_rt/win/brazil_2026_calendar.h: explicit, source-grounded 2026 competition windows and 2027 A/B/Cup pause, immutable already-booked state/national-cup/continental fixtures, no same-club conflict, configurable calendar-day gap (3 days is a simulation preference, NOT a CBF legal minimum). Unknown 2027 state/continental dates are accepted only as *existing* fixtures, not invented as new ones.
- src/recomp_rt/win/brazil_2026_schedule.h: generate 38 **candidate** round anchors per 2026 division; schedule 380 league fixtures around already-fixed state, Copa do Brasil, Libertadores and Sudamericana fixtures, moving only league dates by at most an explicit flex period. No changes are committed to the caller's output if the proposed schedule fails validation. Dates within the year are SIMULATED, not exact official CBF matchday dates; no claim of accurate actual 2026 club pairings.
- tests/seasons/brazil_2026_calendar_test.c and brazil_2026_schedule_test.c: World Cup exception for Serie B, 2027 protected pause, overlapping competition windows, same-club clashes, immutable cup events, 380 matches and fail-without-partial-write.
- tests/seasons/test_brazil_2026_rules.py additionally guards 2026 Copa do Brasil (126 clubs; Serie A enters fifth phase) and existing researched state championship rows.
- Registered standalone tests via CMake/CTest and in .github/workflows/brasileirao-2026.yml.
- **Standalone CI pass:** https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40103306546
- tools/recomp/brazil_2026_legacy_audit.py: a read-only PE32 inspection script. On the locally available 3.9.68 original executable it confirmed BOTH Brazilian league classes set **1 meeting** per club pair and have a fixed **27-date** schedule template. It also confirmed the English Premier template's 38 date slots and the two different Brazil-specific knockout builders. It reads bytes only, does not execute nor distribute the proprietary game binary.

### Engine integration remains a blocker, not a completed feature
The league class vtables of the original 3.9.68 executable were checked against the actual PE:
- Brazil A: rules 0x42a5a0, schedule 0x429b50, advance_stage 0x42a950, result 0x42abc0.
- Brazil B: rules 0x42cb70, schedule 0x42c140, advance_stage 0x42cdd0, result 0x42d040.
- English Premier: rules 0x574b70, schedule 0x573660 (38 slots), advance_stage 0x5a8f60 (empty).
- English First: rules 0x572d90, schedule 0x571980, advance_stage 0x572ff0, result 0x576c50 (four-club two-leg semis plus final).

**Next safe runtime milestones**: (1) verify Brazil-only and mixed-nation new-game start dates with the existing nation-specific picker; (2) implement and validate a Brazil 38-date League calendar allocation and 20-club fixtures in the original 3.9.68 structure, retaining state/Cup/CONMEBOL dates and rescheduling league only when club conflicts; (3) replace old Serie A top-eight title stage with a no-playoff championship; (4) replace old Serie B top-eight stage with a two-tie/two-promotion Cup_Stage, respecting match results and save/load, no final; (5) verify CBF continental qualifications and Copa do Brasil stage 5 entry; (6) play the original executable through 2026–2027 and reload saves during national/continental cups and playoffs; (7) soak 5–10 subsequent seasons with multiple active nations.

The standalone conflict-aware planner is **not wired to the game's existing cup fixture store**. A separate, disabled-by-default experimental hook can now generate a 38-date in-engine league template. Neither 0.5.0 nor this branch is a validated playable modern Brazilian championship.

### 24 October 2026 — starting month correction
The January Brazilian start is already an observed part of the original game. The project **does not** require inventing a new global January launch. Keep existing nation-sensitive start date logic unchanged. Test Brazil-only 2025/26 (real 2026 season) and multiple-league selection to ensure the pre-existing start is preserved, including January state games. Source evidence: `docs/research/2026-10-16-sweep-season-end.md` lines 68–79, and `season_screen.c` currently exposes a separate static July preview label that is not the game's selection logic.


### Engine guard and actual PE scheduling evidence — 24 Oct 2026
The locally supplied 3.9.68 `cm0102.exe` was inspected *without execution* using PE32/x86 disassembly. The two original Brazilian schedulers are not just generic 27-round counters: they allocate **27 x 65 = 1755 bytes** for date template entries at `0x429b88` / `0x42c178` (push 0x6db) and then write 27 as their round count at `0x429b8d` / `0x42c17d`. The original's `EngPrm` scheduler allocates **38 x 65 = 2470 bytes** (push 0x9a6 at 0x573695) and writes 38 at 0x57369a. The Brazilian schedulers then initialize their 27 records with many explicit Date construction and weekday-template calls. **DO NOT** change only the count, only the meetings-per-pair flag or only the allocation: all three and the populated dates must agree, else the game can read uninitialized dates or exceed allocated memory.

To support the no-playoff part safely:
- Added `src/recomp_rt/win/brazil_2026_stage_gate.h`: requires file=bra_nat_first, edition >=2026, exactly 20 teams, 2 meetings, **38 valid date slots**, 38 expected matches (or -1 while the engine defers the computation), 1 historical title stage not yet started, and current stage=-1.
- In the original `season_sizes_rules` runtime hook, added `brazil_2026_remove_title_stage_if_ready`: enabled ONLY with `RECOMP_BRAZIL_2026_STAGE_EXPERIMENT=1`, default OFF. If and only if all the guard conditions are true, changes only `COMP_STAGE_COUNT` from 1 to 0, suppressing the *old title knockout*; never touches Serie B, English classes, continental cups, or the 2002 mode.
- Added `tests/seasons/brazil_2026_stage_gate_test.c` and CTest / GitHub Actions coverage. The experimental 38-date engine template exists in `season_bra_calendar.c`, but has not been verified in real game runs. By default, **this hook does not alter playable games**. Its primary safety property is refusing to modify the original 27-date Brazilian competition.
- Fixed the Choose Season tile's misleading generic `Starts July %d` preview to `Start date follows chosen leagues` in `season_screen.c`. The real nation-sensitive setup mechanism was not changed.
- Do NOT merge the PR or enable experimental stage variable in normal gameplay until 38-date scheduler integration, game recompilation and 2026–27 save/load soak tests have passed.


### 25 October 2026 — First opt-in 38-date in-engine path (UNRELEASED)
- Confirmed actual PE32 3.9.68 class method addresses and four-argument ABI: \`BraNatFirst::create_schedule\` 0x429b50, \`BraNatSecond::create_schedule\` 0x42c140, stage -1, &rounds, &name, &type. Both original functions allocate 27 x 0x41 bytes.
- Added \`src/recomp_rt/win/season_bra_calendar.c\` with \`RT_WRAP\` for both methods. **Only** when \`RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT=1\`, season mode is active, file matches Série A/B, competition edition is 2026, 20 clubs, one/two meetings and stage=-1: allocates 38 x 0x41 bytes using the original game allocator, then calls the original engine's \`0x68a160\` date setter and \`0x68a1c0\` match-slot setter for all 38 dates. It publishes output count 38 and sets meetings per pair to 2 / expected games per club to -1 after building the full template. Every other case delegates to the original game method.
- Uses 38 strictly increasing **candidate** 2026 dates from \`brazil_2026_schedule.h\`, not a claimed verbatim official fixture list. This uses the 2026 CBF A/B overall windows and Serie A World Cup pause; no game-object cup scheduling is modified.
- Added \`brazil_2026_runtime_gate.h\` and unit tests to reject 2002 games, non-Brazilian and state/cup competitions, wrong club counts, wrong stage index, and other editions. The 2027 rollover is **NOT** included in this experimental hook yet.
- Connected the previous \`RECOMP_BRAZIL_2026_STAGE_EXPERIMENT=1\` guarded Série A title-stage removal to the **after** side of \`League::create_fixtures\` in \`season_uefa_league.c\`, where the 38-date state can be inspected (rather than only during the original \`set_up_rules\` before date generation). Requires BOTH environment options to experiment with the full 2026 Serie A flow. It still rejects a 27-date or 19-games-per-club league.
- \`brazil_cal_split_day\` converts 1-based day-of-year back to the original engine's 0-based month / 1-based day; exhaustive roundtrips for 2026, 2027 and leap-year 2028 are in the standalone calendar tests.
- Syntax and standalone C/Python CI **PASS**: https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40185814551
- **Major missing evidence**: no rebuilt game executable was launched. The current 38-date template has one generic match slot per round; not yet proved the game generates all 380 fixtures per competition or preserves save/load correctly. The standalone cup collision planner is not connected to the actual game fixture store; overlapping state, Copa do Brasil and continental fixtures still require full integration. User should **NOT enable either experimental environment option** in a normal save. Série B's actual two-tie playoff stage/results and year 2027 progression are not yet implemented.


## 25 October 2026 — Verified callback-based 380-match audit and 2027 rollover support

### Experimental runtime progression
- \`brazil_2026_schedule.h\` now generates candidate 38-round regular-season dates for both Brazilian leagues in **2026 and 2027**. It follows the CBF published 2027 pause from June 24 to July 25 for A and B, while the 2026 Série B continues during the men's World Cup. Its atomic national-league planner can use existing 2027 CONMEBOL fixture dates as immutable inputs, without inventing their detailed calendar.
- \`brazil_2026_runtime_gate.h\` allows the 2026/27 League schedule wrappers, excluding all editions earlier than 2026 and after 2027. This is still **OFF by default** behind \`RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT=1\`.
- \`tests/seasons/brazil_2026_runtime_template_test.c\` compiles the **actual runtime wrapper** with mocked original game callbacks. It checks 38 allocated dates, all 38 date/slot callback arguments, month conversion, second meetings, and original-engine fallback for the 2002, non-Brazilian, wrong-stage and disabled modes, for **both** 2026 and 2027.
- The previous source-only \`brazil_2026_round_robin\` invariant (380 fixtures) now has a path to observe the **actual original game fixture-manager callback**, not just an unrelated fixture generator.

### New audit of actual League::create_fixtures callbacks
- \`brazil_2026_fixture_tracker.h\`: pure, fixed-size C ledger tracking each actual league club pointer. Completeness requires exactly 380 directed pairings: every distinct home-to-away pairing appears **once**, each club has 19 home and 19 away games, no duplicates or unrecognized clubs.
- \`brazil_2026_fixture_audit.c/.h\`: opt-in, read-only game-memory observer. \`League::create_fixtures\` (existing wrap in \`season_uefa_league.c\`) calls \`brazil_fixture_audit_begin\` before the original and \`brazil_fixture_audit_end\` after. The already-existing \`FIXTURE_MANAGER::add_fixture\` wrapper (in \`season_kind_cup_breaks.c\`) calls \`brazil_fixture_audit_observe\`. It counts only events whose original fixture->record matches the Brazilian League's record; state championships, Copa do Brasil, Libertadores and Sul-Americana are excluded from the national *380 games* count, and their original insertion calls still occur.
- The audit is enabled by \`RECOMP_BRAZIL_2026_FIXTURE_AUDIT=1\`, or implicitly when the separate Série A final-removal experiment is requested. Historical editions, 27-date calendars, other leagues, and non-Brazilian cups are rejected. Each separate fixture-creation call clears prior audit results, so a loaded/restarted season cannot reuse a stale "380 ready".
- **Safer old Série A title-stage guard:** earlier code could clear the old championship knockout when the object's declared number of dates/meetings was 38, even if the game had not actually attempted all matches. Now its after-create-fixures integration only calls \`brazil_2026_remove_title_stage_if_ready\` if the **same immediate create_fixtures call** observed 380 distinct directed fixture-manager adds. The check remains **OFF by default** behind \`RECOMP_BRAZIL_2026_STAGE_EXPERIMENT=1\`; a failed/incomplete ledger preserves the historical knockout stage.
- Standalone unit tests \`tests/seasons/brazil_2026_fixture_tracker_test.c\` and \`tests/seasons/brazil_2026_fixture_audit_test.c\` assert all 380 directed pairings, reject 379, duplicate/self/unknown club match attempts, wrong season, 27-date/1-meeting legacy, old-stale results, ignore other competitions, and verify 2026/2027. The audit unit tests compile the same observer implementation as the in-engine C file with mock memory and callbacks.
- **Latest CI PASS:** https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40188257459

### Limits still requiring game execution
The audit sees calls **before** the original \`FIXTURE_MANAGER::add_fixture\` commits each game. 380 observed callback *attempts* does **not** independently prove all 380 fixtures were inserted, kept across save/load, and scheduled collision-free. The actual game must be rebuilt with the player's original game data and tested with a Brazil-only 2025/26 selection (actual Brazilian edition 2026) through 2027, including Cup and continental fixtures, four promotions/relegations and persistent Série B playoffs. There is still NO in-game 2026 Série B two-tie stage/result integration. Keep this PR draft and the released 0.5.0 unaffected. The opt-in flags must NOT be enabled on valuable saves.


## 25 October 2026 — First experimental engine stage for Série B promotion (draft only)

**Critical difference: two independent ties instead of an English play-off final.** The original 3.9.68 \`BraNatSecond::advance_stage\` at virtual address 0x42cdd0 creates a one-stage, eight-team Brazilian championship knockout. The original game's \`Cup_Stage\` class (vtable 0x969468) persists its round and club counts in the saved game at stage offsets +0x3c and +0x3e. The \`EngFirst::create_round_table\` at 0x571980 can generate an English semi-final round for 3rd/6th and 4th/5th; only its first round is reused. The original English result handler (0x576c50) marks a winner with status 0 and an eliminated club 0xfe.

### Code now committed
- \`src/recomp_rt/win/brazil_2026_access.c\` and \`brazil_2026_access.h\`: gated **only for calendar year 2026**, not other editions or nations. After the original fixture manager accepts exactly 380 uniquely directed matches, \`brazil_2026_access_after_fixtures\` sets two direct promotion places plus four play-off places. When the regular-season table is complete, the \`RT_WRAP(0042cdd0)\` wrapper builds a \`Cup_Stage\` with one round and **four clubs**, seeded \`[6th, 3rd, 5th, 4th]\`, using the original English semifinal round specification but omitting its final entirely. The two-leg match template uses 21 and 28 November 2026. Original historical methods remain the fallback.
- The original \`BraNatSecond::result\` (0x42d040) is wrapped; callbacks to a confirmed live or saved 1-round/4-club Cup_Stage are passed to the original English promotion result handler (0x576c50); regular league results and old 8-team championship stages stay with the original Brazilian handler. **This only wires result events, not validation of aggregate-score winners.**
- A new \`brazil_2026_access_gate.h\` checks edition 2026, 20 clubs, two meetings, 38 dates, exactly one stage not yet started, 2 direct + 4 play-off places and 4 relegations. The result path additionally verifies stage-now=0 and confirms the saved Cup_Stage's original vtable 0x969468, exactly one round, four entrants, and matching competition record.
- \`brazil_2026_fixture_audit.c\` now activates for the access experiment, and the existing \`FIXTURE_MANAGER::add_fixture\` observer runs **AFTER** the original manager returns success (EAX=1), rather than counting every attempt. The original 3.9.68 code at 0x5aab9b returns EAX=1 on acceptance, while 0x5aa964 returns EAX=0 on rejection. The ledger's complete result is still not proof that a save/reload keeps the fixtures.
- \`tests/seasons/brazil_2026_access_gate_test.c\` and \`brazil_2026_access_stage_test.c\` test the PURE gate and compile the actual in-engine stage module with mocked original callbacks. Assertions cover one round/four clubs, fixed pair order, exactly two legs with 7-day spacing, result routing, rejected 2002/2027/Serie A/missing 380-fixture audit, and historical fallback.
- **Latest isolated GitHub Actions CI PASS:** https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40208183415

### Still NOT proven
1. **The Brazilian tiebreak rule is missing in the executable.** CBF 2026: on equal aggregate score, the better regular-season finisher must advance. The imported English tie resolution MAY apply penalties or away goals and produce the wrong winner. The pure \`brazil_2026_promotion_winner\` helper handles the rule, but isn't wired to the original Cup tie resolver.
2. **Actual match results and persistence:** the standalone mock verifies the expected Cup_Stage constructor arguments and saved-stage marker, but not that the game creates both real ties, calls its result handlers twice, advances both winners, and preserves fixtures/results after saving and reloading.
3. **Actual cup conflicts:** 21 Nov Sul-Americana final and 28 Nov Libertadores final overlap Série B playoff days; the stage's fixed match dates need a real-club-specific clash check. Neither its dates nor the other cups have been validated in a playable run.
4. The 2027 playoff regulations and its original Cup_Stage behavior have not been verified. This access experiment is intentionally restricted to 2026 (the 2027 regular-season 38-date template is a separate protected experiment).
5. This feature is **OFF BY DEFAULT**, only exposed through \`RECOMP_BRAZIL_2026_ACCESS_EXPERIMENT=1\`. It also requires the independent \`RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT=1\` 38-date builder to have run, producing an accepted 380-fixture audit. No one should enable either experimental flag in a valuable save, merge this PR into v0.5, or call it released.


## 25 October 2026 — CBF aggregate tie-break implemented using original Cup flag (current state)

**Important: this supersedes the earlier comment that the aggregate tiebreak was absent from the executable.** Local read-only disassembly of the attached original 3.9.68 \`cm0102.exe\` shows an existing mechanism that can select the second team of a tie when the two-legged aggregate is level:

1. EngFirst::create_round_table 0x571980 writes original first-round flags \`0x0204\` to \`table+0x0d\` (instruction at 0x572af3). Those flags mean fixed-pair English two-leg semifinal without a seeded aggregate tie-break.
2. The original Cup stage copies the round flags into each fixture's rule word (\`0x51b741\`, \`0x51b745\`); the original \`Cup::resolve_tie\` 0x51f2f0 reads the fixture rule word at +0x38 (\`0x51f30f\`).
3. When the aggregate score after two legs is equal, this resolver first checks \`0x0800\` (first entrant qualifies) and then \`0x1000\` (second entrant qualifies), at \`0x51f652\` / \`0x51f675\`, before the away-goals and penalty branches. The second entrant's qualifying marker is set at \`0x51f680\` and the resolver returns winning entrant 2 at \`0x51f687\`.
4. Our two independently fixed pairings are \`[6th,3rd]\` and \`[5th,4th]\`. Therefore the **second entrant is higher ranked in both pairs**. We now update only the Brazilian stage's first-round flags from \`0x0204\` to \`0x1204\`, preserving fixed pairs/two legs while applying \`0x1000\`. The same original Cup mechanism owns both the winner choice and the original serializable Cup_Stage format. This is still a **code-level** inference requiring gameplay confirmation.
5. New header \`src/recomp_rt/win/brazil_2026_promotion_round.h\` validates the exact English base flags, two legs and rank order; any other format fails without modifying the round. In \`brazil_2026_access.c\`, the CBF flag is written only after successful date setup. Historical Brazil/England and all 2027 play-offs are unaffected.
6. Source tests \`tests/seasons/brazil_2026_promotion_round_test.c\` check 0x0204 -> 0x1204, forbid wrong seed order/one-leg/other flags, and exercise level aggregates (including a case where the 6th-placed club scores more away goals). The existing mocked original stage test was expanded to verify the real wrapper builds precisely one 4-team stage with the CBF flags, routes **four semifinal outcome callbacks** to the known English handler's success statuses, yields two automatic + two playoff promotions, and recognizes a copied original Cup_Stage at a different simulated post-load address.
7. Additional pure check \`brazil_2026_access_promotions.h\` verifies exactly **four promoted** and **four relegated** rows, including exactly two successful play-off clubs from 3rd–6th. A *read-only* hook in the existing \`League::promote_and_relegate\` wrapper logs this status audit before the original inter-division movement, with no effect on the game even if the check fails. Unit tests reject incomplete ties, unexpected extra promotions and missing relegations.
8. The read-only tool \`tools/recomp/brazil_2026_cup_tiebreak_audit.py\` can check eight exact binary signatures on a user's own local original 3.9.68 EXE (it does not run, alter, copy or include the executable). Eight opcodes were manually verified with GNU objdump and Python against the original EXE. No proprietary binary was added to the repo.
9. **GitHub Actions test suite passed:** https://github.com/felipegavilanes-source/cm0102-bosman-edition/actions/runs/40217000696

### Remaining release blockers — not optional
- Full Windows/executable compile and actual 2026 new-game run from January, including all 380 Série A and B league fixtures, complete home/away season, CBF better-ranked-on-draw tie result, BOTH winners promoted, and season transition to 2027.
- True **save/reload** while a two-leg tie is in progress and after it is complete: the mock copies an in-memory Cup_Stage but does **not** exercise the game's serializing save and load code. Verify persistence of the chosen winner and promotion statuses.
- Actual fixture collisions for a club participating in state championship, Copa do Brasil, Libertadores, Sudamericana and Série A/B; especially CONMEBOL finals on 21 and 28 November, which coincide with Série B access tie dates. Current in-engine date template doesn't use the source-only cross-cup conflict planner.
- Proper Serie B promotion/relegation balance in the original chain when Brazil and European leagues are selected together; multiple subsequent 2027+ seasons and real game memory/saves.
- The current Série B access stage is **OFF by default** behind \`RECOMP_BRAZIL_2026_ACCESS_EXPERIMENT=1\` and the national league 38-date builder behind \`RECOMP_BRAZIL_2026_RUNTIME_EXPERIMENT=1\`. Do **not** enable these experimental options on normal saved games or merge them into v0.5.
