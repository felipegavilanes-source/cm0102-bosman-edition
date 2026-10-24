# Brazilian Serie A and Serie B — 2026 implementation checklist

Status: design and audit only. No runtime changes have been applied in this branch yet.

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
