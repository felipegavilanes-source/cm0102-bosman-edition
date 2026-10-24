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

The current source-level calendar/planner is **not wired into the live original-engine fixture scheduler**. Never describe the 0.5.0 program or this experimental branch as already having modern Brazilian championships.

### 24 October 2026 — starting month correction
The January Brazilian start is already an observed part of the original game. The project **does not** require inventing a new global January launch. Keep existing nation-sensitive start date logic unchanged. Test Brazil-only 2025/26 (real 2026 season) and multiple-league selection to ensure the pre-existing start is preserved, including January state games. Source evidence: `docs/research/2026-10-16-sweep-season-end.md` lines 68–79, and `season_screen.c` currently exposes a separate static July preview label that is not the game's selection logic.
