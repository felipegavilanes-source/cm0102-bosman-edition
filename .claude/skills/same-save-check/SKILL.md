---
name: same-save-check
description: Proves that a Bosman Edition change keeps the same game (rule Q152) with the same-save, match and screen checks, and reads their results. Use for "same-save", "same game rule", "Q152", "does this change decisions", "saves differ", "the match check failed", "LAYOUT_PARTS", before a merge of game code, or for the full test set at the end of a build.
---

# Same-save check

Rule (Q152): no feature may change the game's decisions while the player makes the same clicks, except the team that is picked. The original's results depend on where its memory blocks sit, so opening any screen changes later days. A check therefore compares the same steps with the option on and off, never a run that opens a screen against one that does not.

## The checks

Run from the repo with `G=/Users/max/Projects/cm0102/clean-3968`:

- `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav` (load start.sav, holiday, save 14 days later)
- the same with `--save prematch.sav`
- `--same-save` without `--save` (a new game)
- `--match` (prematch.sav, the Basel friendly, widescreen)
- screen checks that open a screen in both runs: `tests/smoke/same_save_views.py ... --check tactics` and `--check match-tactics`, `smoke_run.py ... --same-save --save start.sav --options`, `--search`

Each run list is in `smoke_run.py` (`SAME_SAVE_RUNS`, `MATCH_RUNS`). A new switch adds itself to the `off` run and adds a pair `<name>-on` and `<name>-off` to both lists. A change with no option adds a test-only environment variable that turns it off and a `_env` run (for example `sleep-off` with `RECOMP_IDLE_SLEEP=0`), or compares against another build with `--ref-build`.

## Full test set (once per build, at the end)

The checks above, plus in parallel: smoke in classic, widescreen, HD and HD Small (`smoke_run.py build/cm0102_recomp $G /Users/max/Projects/cm0102/smoke-ref --mode <mode>`, modes `classic`, `widescreen`, `hd`, `hd-small`), `tests/smoke/hd_text_run.py build/cm0102_recomp $G <out>` (HD Large screens, look at them), `ctest -R options` in `build/`, `tests/recomp/test_load.py <exe> <data> <load program>` (build the program first, see its docstring), `tests/recomp/test_crashguards.py <exe>`, `tests/smoke/picker_check.py build/cm0102_recomp $G start.sav`. Start them from one script and write each log to a file. Wait on a file the script writes at the end.

## Reading the results

- `ok` and `PASS`: equal saves.
- `again` differs from `normal`: the runs are not repeatable, nearly always machine load. Check `uptime`. Rerun the check alone when the load is low. This is not a finding.
- A run with `no saved game (code 3)`: a wait step timed out under load. Rerun alone.
- A difference only in the five `LAYOUT_PARTS` (`fifa_rankings.dat`, `general.dat`, `national_teams.dat`, `staff history.tmp`, `transfer.dat`) when a screen opens with the option on: memory layout. It is accepted for the `off` run only, never for `on again`. Say so to Max.
- Any other part differs and `again` is equal: the change alters the game. Stop, find the cause, and report it. Do not widen the accepted parts.
- A smoke screen that differs where the change draws something new: look at the picture. If the change is the intended one, keep a copy of the old reference in `smoke-ref/backup-...` and re-record with `--record`.

## Traps

- Text the game draws changes later days. A test that shows a path with the run's folder name gives different saves for the same clicks. Keep drawn text the same in both runs.
- The mouse position after a click changes hover labels with `clickable_names` on. Keep the steps the same in both runs.
- `RECOMP_FIXED_CLOCK=1` still counts real milliseconds. Holiday holds and commentary waits measure real time.
