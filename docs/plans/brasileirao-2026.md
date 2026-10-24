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
