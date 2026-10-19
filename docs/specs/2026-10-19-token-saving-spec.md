# Token saving for build sessions
Status: Ready

Interview: the session of 2026-10-19 (Q1 to Q12 of this spec, all accepted as recommended). The numbers below come from the session files on this Mac for 16 to 18 October.

## Problem
The 0.6.0 build used about 46,000 turns in three days. On each turn the whole context goes back to the model, and the average context was about 215,000 tokens. Cache reads (9.9 billion tokens) and cache writes cost almost all of the money. Output was less than 5%. A full week of the Max plan lasted two to three days, and the rest of 0.6.0 costs about $2,000 to $4,000 on extra usage at this rate.

Tool output that stays in the context makes every later turn more expensive. Measured over 16 to 18 October: grep and rg output about 17M tokens, other Bash output (builds, test runs, scripts) about 10M, sed, cat and head reads about 6M, Read of text files about 2.6M, and 689 PNG screenshots. Every slice agent also reads the 79 KB build brief and the 30 KB handover.

## Solution
Agents see short output, and the full output is always in a file they can open. Builds and test runs print the exit code, the error lines, the last lines and the log path. Long grep output stops at 100 lines with a line that says how many lines are left. Screenshots that an agent looks at are at most 1024 px wide. Each fix round starts in a fresh agent, and each agent reads only the part of the brief that it needs. The cache lasts one hour, so a long test run does not make the whole context get written again.

Nothing in this spec changes the game, the tests or what the tests check. No information is lost: every cut says what it cut and where the full text is.

## User stories
1. As Max, I want the rest of 0.6.0 to cost much less, with no change to what the game does or how well it is tested.
2. As an agent, I want a failed build to show me the compiler errors, even when the log is long, so that I fix the right line.
3. As an agent, I want a failed same-save check to show me which part of the save differs, so that I do not have to open the log to know it failed.
4. As an agent, I want the path of the full log, so that I can read more of it when the short view is not enough.
5. As an agent, I want to run one command with its full output when I need it (`RAW=1`), so that the hook never blocks me.
6. As an agent, I want to know when grep output was cut and how many lines are left, so that I narrow the search and do not miss a result.
7. As an agent, I want `grep -l`, `grep -c`, `-m` and a pipe to `head` to work as before, because I already chose their size.
8. As an agent, I want screenshots that I can read at 1024 px, and the full-size file when I name it `-full.png`.
9. As a lead, I want a fix round to go to a fresh agent with the review list, the session-state file and the branch, so that it does not carry a 300,000-token history.
10. As a slice agent, I want to read only the shared facts and my own slice section of the brief.
11. As a new session, I want a short handover with the current state, and the old facts in one place I can open.
12. As a test run that takes 7 minutes, I want the cache to still be there when the run ends.
13. As Max, I want to see the tokens per turn before and after, so that I know the saving is real.
14. As Max, I want my other projects (Leaner, Promega) not to change.
15. As an agent in a worktree, I want the same hooks as the lead, from the repo itself.
16. As a background command (`run_in_background`), I want the same short output and the same log file.
17. As an agent, I want a command that the wrapper does not know to run as it always did.

## Decisions
- Q1. The settings and hooks go in the repo's committed `.claude/settings.json` and `.claude/hooks/`. A copy of the settings goes in `/Users/max/Projects/cm0102/.claude/settings.json` for sessions started in the parent folder, and its hook paths point into `cm0102-mac`. The user settings stay empty.
- Q2. One wrapper runs builds and tests. It writes the full output to a log file in `/Users/max/Projects/cm0102/run-logs/` (outside every worktree, one file per run with the time and the command name). It prints the exit code, up to 60 matched error lines, the last 40 lines and the log path. The error patterns cover compiler errors, Python tracebacks, failed asserts, ctest failures, same-save differences and the game's error boxes (a source file name with a line number). The wrapper keeps the command's exit code.
- Q3. A PreToolUse hook on Bash sends known commands through the wrapper: `cmake --build`, `cmake` configure, `ctest`, `make`, `ninja`, every `tests/**/*_run.py` and `*_check.py`, `smoke_run.py`, `make_release.sh` and the recomp tools that print per file. `RAW=1` at the start of a command turns the hook off for that command. A command the hook does not know is not changed.
- Q4. The same hook cuts grep and rg output at 100 lines and adds "N more lines: narrow the search or use -l". It leaves a command alone if it has `-l`, `-L`, `-c`, `-m`, `--count`, `--files-with-matches`, or a pipe to `head`, `wc`, `sort` or another command.
- Q5. rtk is not installed.
- Q6. Each review fix round goes to a fresh agent with the review list, the session-state file and the branch. The slice-build skill says so.
- Q7. The build brief becomes a shared file (shared facts, same-save checks, files that two slices share, speed, rules) and one file per slice. The text does not change. A check proves that every line of the old brief is in exactly one new file.
- Q8. The old sections of the handover (17 October state, 0.5.0 and earlier facts) move to `docs/history/` with one pointer line. The 18 October state and the rules stay.
- Q9. A PreToolUse hook on Read gives the agent a copy of a PNG at most 1024 px wide (made with `sips`, kept in `run-logs/shots-1024/`). A file whose name ends in `-full.png` is not changed. The test scripts and their comparisons still use the full-size files.
- Q10. slice-build and same-save-check tell agents to run independent commands in one call and to chain build and test in one command.
- Q11. `promptCacheTtl` and `subagentPromptCacheTtl` are set to `1h`. The slice that sets them first checks that the installed Claude Code knows these keys.
- Q12. `tools/dev/token_report.py` reads the session files and prints tokens per day and model, tokens per turn and tool output by tool. A fix round after the change is compared with 16 to 18 October.
- Graphify is not installed. Measured on 16 to 18 October, searches of source code (the runtime, the tools and the generated C) were about 9% of grep output, about 1.5M tokens. Most grep output came from game data, saves, logs and the function and data maps, which graphify does not index. Its skill and hook would also add text to every turn.

## Testing
- Slice 1: `tests/dev/test_hooks.py` feeds the hooks the JSON that Claude Code sends and checks the command they return. It also runs the wrapper on small sample commands: one that fails with a compiler error in the middle of 5,000 lines, one Python traceback, one same-save difference, one game error box, one success. Each must show its error lines and the right exit code. It runs in ctest.
- Slice 2: a script checks that the lines of the old brief and the old handover each appear in exactly one new file.
- Slice 3: a reviewer reads the skill text against the decisions. There is no automatic test.
- Slice 4: the script must give the same figures as the measurement of 19 October for 16 to 18 October, within 1%.

## Out of scope
- Changes to the game, the tests or what they check.
- Lower caps in `BASH_MAX_OUTPUT_LENGTH` or the auto-compact threshold. Compaction can lose detail that a slice needs.
- rtk and graphify.
- Changes to other projects or to the user settings.

## Open decisions
None.

## Slices
The four slices touch different files and can run at the same time.

### 1. Wrapper, hooks and settings
Blocked by: none
Delivers: short build, test and grep output with the full log in a file, smaller screenshots, and the 1-hour cache (Q1 to Q4, Q9, Q11).
Merge danger: two-way door. Blast radius: every Bash command and every Read of a PNG in every session of this project. A wrong pattern can hide an error or break a command. Two Opus reviews: one against this spec, one that tries to make a failure disappear from the short view.
- [ ] `tests/dev/test_hooks.py` passes in ctest.
- [ ] A real failed build in a scratch copy shows the compiler error in the short view.
- [ ] A real `smoke_run.py --same-save` run gives the same result with and without the hook.
- [ ] `RAW=1` gives the full output.
- [ ] A worktree session gets the hooks from the repo.

### 2. Brief split and short handover
Blocked by: none
Delivers: each slice agent reads only the shared facts and its own section, and new sessions read a short handover (Q7, Q8).
Merge danger: two-way door. Blast radius: an agent misses a fact if a line is lost. The line check guards it.
- [ ] The line check passes for the brief and the handover.
- [ ] Every pointer to the old brief in the skills and the handover points to the new files.

### 3. Skill rules for fresh fix rounds and fewer turns
Blocked by: none
Delivers: slice-build and same-save-check say: fresh agent per fix round, independent commands in one call, build and test in one command, `RAW=1` only when needed (Q6, Q10).
Merge danger: two-way door. Blast radius: the way agents work. One Opus review against this spec.
- [ ] The reviewer finds every rule of Q6 and Q10 in the skills, and no rule that contradicts the existing ones.

### 4. Token report
Blocked by: none
Delivers: `tools/dev/token_report.py` (Q12).
Merge danger: two-way door. Blast radius: none for the game. It only reads session files.
- [ ] Its figures for 16 to 18 October match the 19 October measurement within 1%.
- [ ] After the first fix round with slices 1 to 3 merged, the lead compares tokens per turn with 16 to 18 October and writes the result in this spec.
