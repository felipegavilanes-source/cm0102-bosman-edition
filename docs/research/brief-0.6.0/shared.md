# Brief for the 0.6.0 build sessions

Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (Decisions, with the research decisions Q268 to Q339). Read it first, then `GLOSSARY.md` (use its words) and `docs/HANDOVER.md`. For the REA tools and their traps, read `docs/research/agent-brief.md`. Build only your own slice. Function map names (`docs/research/function-map/`) are hints: read a function's code before you rely on it, and cite its address.

## Shared facts
- Repo `/Users/max/Projects/cm0102/cm0102-mac`, GitHub `maxevans60/cm0102-bosman-edition`. You work in your own worktree on your own branch. **First run `git merge main`**: a worktree can be cut from an older commit. Commit on your branch. Do not push. Do not merge into `main`: the lead merges after review.
- The original: `/Users/max/Projects/cm0102/clean-3968/cm0102.exe` (3.9.68). Game files: `/Users/max/Projects/cm0102/clean-3968`. Never use the fan pack in `/Users/max/Downloads/Game`.
- Build: `cmake -S . -B build -DCM0102_EXE=/Users/max/Projects/cm0102/clean-3968/cm0102.exe -DRECOMP_PYTHON=/Users/max/.venvs/cm0102/bin/python`, then `cmake --build build -j 6 --target cm0102_recomp`. Game code changes go through the checked mechanisms (`RT_WRAP`, `tools/recomp/callbacks.py`, `tools/recomp/callsites.py`, `INSNS` entries as in `tools/recomp/widescreen.py`): the build must stop if an address no longer matches.
- Builds to compare with (do not rebuild them):
  - main: `/Users/max/Projects/cm0102/cm0102-mac/build/cm0102_recomp`
  - 0.5.0: `/Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp` (tag v0.5.0). The base game rule compares against this one.
- Scripted runs: `RECOMP_SCRIPT` steps with wait steps (`src/recomp_rt/win/waits.c`). `RECOMP_SCREEN_LOG=1` prints each screen change. Never write to Max's save folder: use `RECOMP_USER_DIR=<new folder from mktemp -d>`. `tests/fixtures/fixtures.py play` loads a saved test game and can dump screens.
- Saved test games in `/Users/max/Projects/cm0102/test-fixtures/`: `start.sav` (Arsenal 14.7.01), `prematch.sav` (16.7.01, Basel friendly 20.7.01), `yeovil.sav` (20.8.02, a copy of Max's save; never touch his own save).
- Update Team databases: `/Users/max/Projects/cm0102/test-fixtures/databases/<name>/` (49 unpacked folders) and `.../databases/zips/` (the original zips). Content signatures of all 49: `docs/research/rules-0.6.0/databases-signatures.tsv` (sizes and SHA-256 of `index.dat`, `club.dat`, `staff.dat`). Season to database: `docs/research/2026-10-16-season-database-table.md`. A database already loads as a custom data set: copy its folder into `Databases` of the user folder and set `database = folder:<name>` (`dbfolders.c`, `tests/smoke/custom_database_run.py`).
- Rules table: `docs/research/rules-0.6.0/<part>.tsv` (20 parts, one row per league or competition per season, columns in the first line), club moves in `<part>-clubs.tsv`, academy sides in `eng-academy.tsv`. The `.check*`, `.reconcile` and `.md` files are the research evidence.
- Code sweeps: `docs/research/2026-10-16-sweep-season-end.md` (end-of-season step, year constants, transfer rules), `2026-10-16-sweep-rulebooks.md`, `2026-10-16-sweep-europe.md`, `2026-10-16-sweep-international.md`, and `2026-10-16-season-change-points.md`.
- Options: `src/recomp_rt/win/options.c` and `options.h` (one row per option), test `ctest -R options` in `build/`. The `add-option` skill (`.claude/skills/add-option/`) gives the steps.

## Same-save checks (every slice that touches game code)
Use the `same-save-check` skill (`.claude/skills/same-save-check/SKILL.md`). With `G=/Users/max/Projects/cm0102/clean-3968` and `REF=/Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp`:
- **Q342 (Max, 2026-10-17):** from slice 13 on, the Setup Game text moves memory, so the base game is compared with the main build after slice 13, not with 0.5.0. Every slice adds its runs with the test switch `RECOMP_SEASON_CODE=off` (it turns the slice 11 year sites and the later season code off): with the season at 2001/02 and `changing_rules` off, the saves must be equal.
- **Base game against 0.5.0 (slices 11, 12 and 14 only):** `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --ref-build $REF`, the same with `--save prematch.sav`, and `--same-save` without `--save` (a new game, `changing_rules = off`). Every save must equal the 0.5.0 save byte for byte.
- **Yeovil:** the same with `--save yeovil.sav --days 7 --ref-build $REF`. If `--save` does not take a save outside the start list, add that to `smoke_run.py` in its own small block and say so.
- **Repeatability of a moved season:** a new game in 2010/11 (September 2010 ODB) twice with the same steps gives equal saves.
- A new switch adds itself to the `off` run of `SAME_SAVE_RUNS` and a pair `<name>-on` and `<name>-off` to `SAME_SAVE_RUNS` and `MATCH_RUNS`.
- Rule (Q152): no feature may change the game's decisions while the player makes the same clicks. If a design would change the base game's decisions under the same steps, stop and report.

## Slices in this batch

## Slice files

Read this file, then only the file of your slice. Slices 15b to 29 also read `common-15b-to-29.md`. In a slice file, "above" and "below" mean this file and `common-15b-to-29.md`. Files are in `docs/research/brief-0.6.0/`.
- `slice-11.md`: Slice 11, season core (Opus). One-way door: it fixes the save format.
- `slice-14.md`: Slice 14, season databases in the download (Sonnet)
- `slice-11b.md`: Slice 11b, right ages and dates in every season (Opus), after slice 11
- `slice-11c.md`: Slice 11c, competition history between 2001 and the start season (Opus), research now, code after slice 11b is on main
- `slice-12.md`: Slice 12, rules table reader and rules report (Opus), after slice 11 is on main
- `slice-13.md`: Slice 13, Choose Season screen (Opus), after slices 11 and 14 are on main
- `slice-15.md`: Slice 15, club moves and names (Opus), after slice 12
- `slice-16.md`: Slice 16, season change and news (Opus), after slice 12
- `slices-17-18-21-22.md`: Slices 17, 18, 21 and 22 (Opus), after slice 16 is on main
- `common-15b-to-29.md`: Common to slices 15b to 29
- `slice-15b.md`: Slice 15b, real club records for missing top and second tier clubs (Opus), after slice 15
- `slice-19.md`: Slice 19, play-offs, splits and calendar changes in existing divisions (Opus), after slice 18
- `slice-20.md`: Slice 20, new divisions (Opus), after slice 18
- `slice-23.md`: Slice 23, UEFA 2009/10 to 2020/21 (Opus), after slice 22
- `slice-24.md`: Slice 24, UEFA 2021/22 to 2023/24 (Opus), after slice 23
- `slice-25.md`: Slice 25, UEFA league phase, 2024/25 on (Opus), after slice 24
- `slice-26.md`: Slice 26, FIFA and other club competitions (Opus), after slice 16
- `slice-27.md`: Slice 27, international competitions in the original (Opus), after slice 16
- `slice-28.md`: Slice 28, Nations League and Euro qualifying paths (Opus), after slice 27
- `slice-29.md`: Slice 29, 48-team World Cup (Opus), after slice 27

## Files that two slices share
- `options.c`, `options.h`, the `ctest` options cases: slices 11 and 13. Keep each change in its own small block.
- `SAME_SAVE_RUNS` and `MATCH_RUNS` in `smoke_run.py`: slices 11 and 13.
- `database.c`, `dbfolders.c`: slices 13 and 14. Slice 14 adds new files and only small hooks in these.
- `tools/release/make_release.sh` and `CMakeLists.txt`: slices 12 and 14.
- `season_rule_kinds` in `season_change.c`: slices 16 to 29, one line each. At a merge conflict keep both lines.
- `tests/seasons/rules_report_expected/*.tsv`: every slice from 15 on. After a merge, run the check with `--update-expected` on the merged build, not a text merge of the files.
- `tests/seasons/rules_report_check.py` (a `--changing-rules` flag, new kinds of difference): slices 12, 15, 16 and every slice that adds a flag. Keep each change in its own small block.
- The new competitions module (`season_comps.c` and `.h`, suggested names) and its design doc: slices 20, 21, 24, 26, 28 and 29. The first of them to start owns it (the lead names it), the others add in small blocks.
- `season_clubs.c`, `tools/seasons/make_clubs.py` (`TARGETS`) and `Seasons/clubs.tsv`: slices 15, 15b, 18 and 20.
- `season_names.c` and `tools/seasons/make_names.py`: slices 15, 16, 20, 23, 24, 26 and 28.
- `season_report.c` (club lines): slices 12, 15, 15b and 20.
- `tools/seasons/encode_rules.py`: slices 17 and 19, and slices 22 to 25 if they encode the access list there.
- The UEFA seeding code (0x903360 to 0x905f00) and the Europe array (0x8313bb, 0x8313bd): slices 22, 23, 24 and 25. Merge them in that order.
- The host table (`HostCountry` 0x5f9970, 0x5fab70): slices 26, 27 and 29. Slice 27 owns the rows.
- Australia's confederation (nation +0x71 and the fixed Oceania lists): slices 26 and 27. Slice 27 owns the change.
- `euro_champ_qual` and the WC Europe section: slices 27, 28 and 29.
- The season part (`season.c`, `season_change_part_*`): slice 16, slice 27 (nation names), slice 28 (Nations League tiers) and every slice that saves new state. Add fields after those of the last version and raise the version, one slice at a time in merge order.
- The staff reserve estimate (`staff_reserve_estimate`, Select All fix): slice 15b.

## Speed
- Run test games in parallel. If a parallel run fails on timing, rerun it alone before you call it a bug (`uptime`: above about 20 runs fail).
- While you build, run only the test your change touches. Run the full set once at the end (the `same-save-check` skill lists it).
- Send long decompiles to a subagent: REA returns each listing twice.

## Rules
- Never name another modder, their patch or tool in any committed file, comment, commit message or test name. Some source zip and folder names hold a person's name: they never go in the download or the repo.
- Never commit the original's code or data, the databases, saves, screenshots or the logo.
- No hidden value may appear on a screen or feed a shown number.
- Every option is a line in the settings file, read through the existing reader.
- Do not run `rm` with a wildcard or on a path in a variable. Make new folders with `mktemp -d`.
- Test first: write the failing check at the spec's seam, then the code.
- Plain STE English in docs, comments and commit messages. End commit messages with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Research: give every fact its address, mark inferences, write "unknown" rather than guess.
- Tick your slice's acceptance checks in the spec on your branch only when they pass.
- If a decision is Max's, stop and report it with a recommended answer. Do not guess.
- Final reply: at most 300 words: the branch name and last commit, the files you touched, what works, what does not, test results with the exact commands, and screenshot paths (about 800 px wide, only of what changed, outside the repo).
