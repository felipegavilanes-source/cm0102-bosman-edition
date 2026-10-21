# World competitions modernization — initial technical audit

Scope: update national, continental and international competitions worldwide while keeping the CM 01/02 interface and match engine unchanged.

## Verified repository findings

- The README states that the 3.9.68 executable is recompiled instruction-by-instruction and that the original match and transfer decisions are retained.
- The roadmap lists season-specific rules, league sizes, European cups and international competitions for 0.6.0; this does **not** establish that all are implemented.
- `tools/seasons/make_db_table.py` generates `src/recomp_rt/win/seasons_table.c` from `docs/research/rules-0.6.0/databases-signatures.tsv`. This recognizes database versions using club/staff sizes and SHA-256; it is **not** a competition rule editor.
- `tools/seasons/make_dates_table.py` generates `season_dates_table.c` from date-shift TSV files. It references competition-history rows, but does **not** by itself create competitions.
- `CMakeLists.txt` defines season database, date and international-competition checks. Their underlying implementation and runtime behavior require further inspection.

## Required next investigation

1. Locate actual competition scheduling, qualification, promotion/relegation and tournament-stage code and data structures.
2. Distinguish database-defined competition metadata from executable-defined rules.
3. Inventory current tournaments by federation, then compare with a documented target season and official regulations.
4. Select a narrow proof of concept (one tournament format) and write regression tests for fixtures, qualification, calendar and saves.
5. Do not change UI, match engine or default main branch; avoid claiming an implementation before a reproducible build and season test.

## Constraints and acceptance criteria

- Preserve existing game behavior when the modernization feature is disabled.
- All changes confined to feature/world-competitions.
- Document compatibility with saves and upstream updates.
- No bundled original proprietary executable or database files.
- Run builds and simulated season tests before calling a rules change complete.

Status: audit notes only; no competition rules modified.
