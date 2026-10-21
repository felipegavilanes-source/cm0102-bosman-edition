# Worldwide competition modernization (2026/27)

This fork prioritizes competition rules and calendars **without changing the CM 01/02 user interface or match engine**.

The upstream Bosman Edition already includes significant work on season-dependent leagues, cup stages, promotion, European cups, new competitions, and national-team tournaments. The goal here is to inventory and verify those rules before adding more hooks to the original game.

## Run the audit

From repository root, using Python 3 and no third-party packages:

~~~bash
python3 tools/world_competitions/report.py --season 2026/27 --previous 2025/26 \
  --out docs/world-competitions/2026-27-status.md \
  --json-out build/world-competitions-2026-27.json
python3 -m unittest discover -s tests/world_competitions -p 'test_*.py'
~~~

The report reads the 22-column TSV rules researched in \`docs/research/rules-0.6.0/\`. It does **not** edit those tables or generate game patches. Statuses describe the research, not verified in-game behavior.

- **inactive:** a table row explicitly says a tournament was not held / cancelled in this edition.
- **discontinued_or_paused:** changed to inactive compared with the preceding season; not necessarily permanently abolished.
- **newly_listed:** new in the research comparison, not necessarily newly founded in real life.
- **missing_from_research:** listed in the earlier year but not the target year; never assume extinct.
- **implementation_review:** a researched active competition uses \`comp_file=new\`; that alone does not prove the game already creates its object.
- **fields_to_verify:** unknown or inferred counts or format.

The report includes domestic, continental and worldwide competitions and is designed to avoid false claims based on missing data or assumptions.

## Safe implementation sequence

1. Use this report to select individual competition-format gaps.
2. Check current upstream runtime implementation, including \`season_comps.c\`, \`season_kind_cup_breaks.c\`, \`season_uefa*.c\` and \`season_intl*.c\`.
3. Confirm the latest official rules and how teams qualify.
4. Add a small code or table change, unit tests, and at least one complete simulated season. Verify reloading saved games.
5. Preserve the original behavior when season changes are disabled. Do not edit visuals or match engine.

No copyrighted game binaries or database files should be committed.
