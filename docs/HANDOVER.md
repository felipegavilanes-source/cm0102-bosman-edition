# Handover, 2026-10-18 (afternoon)

Read this first, then `GLOSSARY.md`. v0.5.0 was published on 2026-10-16 with Mac, Windows and Linux downloads, and the site shows it. The next work is 0.6.0.

The project lives in `/Users/max/Projects/cm0102`: the repo `cm0102-mac/`, `clean-3968/`, `data-3960/`, `data-3965/`, `sdl2/`, `smoke-ref/`, `test-fixtures/` (with `yeovil.sav` and `databases/`: all 49 Update Team databases unpacked, zips in `databases/zips`), `site-release-assets/`, `save-backups/`, `builds/` and `shots/`. Start sessions in `/Users/max/Projects/cm0102`. `build/` is configured with `-DCM0102_EXE=/Users/max/Projects/cm0102/clean-3968/cm0102.exe -DRECOMP_PYTHON=/Users/max/.venvs/cm0102/bin/python`.

## 0.6.0 build state (2026-10-18 afternoon, written for a new account)

Read this section first; the section of 2026-10-17 is older and is in `docs/history/handover-2026-10-17.md`.

**PAUSED 2026-10-18 ~14:45 for the account change. Resume here.**
1. Read every file in `/Users/max/Projects/cm0102/lead-notes/session-states/` (one per track: branch, last commit, what is done, where it stopped, open review items, next steps, exact test commands, decisions for Max). Slices 19, 26 and 27 were still writing theirs at the pause: if a file is missing, read that branch's last commits and its doc (`docs/research/2026-10-18-*.md` on the branch).
2. The 31 lead decisions in `/Users/max/Projects/cm0102/lead-notes/decisions-for-max.md` are confirmed by Max (2026-10-19) and recorded in the spec as Q373 to Q379 and Q387 to Q414.
3. Branch tips at the pause: slice 18 (with slice 15) `claude/slice-18-fb9fae` 34562ef; slice 19 `claude/slice-19-02249b` c3981de; slice 21b `worktree-agent-a53abeb084e1c3a35` 1549374; slice 21c `worktree-agent-aa571b77878dc5408` a3054c8; slice 23 `claude/slice-23-3295ac` 4e29665 (fixes committed, not built or tested); slice 24 `claude/slice-24-72016d` fec0d2a; slice 26 `claude/slice-26-d5d47b` 28a3a76; slice 27 `claude/slice-27-36f393` 9ac19ae; slice 28 `claude/slice-28-a9e4a2` dd231e4; UEFA screen fixes `fix/uefa-screens` 9730af6 (three bugs fixed, same-save not run); own databases `own-db-rebuild` 1c5a83e in `/Users/max/Projects/cm0102/wt-own-db-0405` (own-dbs zips match this branch's signature rows, not main's). None is pushed; all are local branches of this repo.
4. Merge order: fix/uefa-screens (after its same-save run); slice 18 with 15 (after expected files and the Database.cpp 13471 box in its group (b) 2010 run); slice 27; slice 26 (joins 27's 0x40f590 wrap); slices 21b, 21c, 23, 24, 19, 28; own-db-rebuild. For each: the lead runs the same-save set on the branch, merges with --no-ff, builds, runs ctest and same-save against the build before, pushes. Then 15b, 20, 25, 29 and slice 30 (release; Max approves the publish).
5. Not mine, do not merge: the "LazzTommy.sav file issue" session's uncommitted change in `/Users/max/Projects/cm0102/wt-accent-names` (slice 15's path fix replaces it). Old reference builds for comparisons: `/Users/max/Projects/cm0102/ref-builds/`.

**On main (pushed, 423ec0f):** slices 11 to 14, 16 (season change, rule registry, news), 17 (substitutes, foreign rules, work permits with the Brexit rule, windows), 21a (domestic cup round formats), 22 (UEFA 2003/04 to 2008/09, shared round names table `season_round_names`), and the box fixes (news 7797, Database 13126 and 13160, comp_stats 1664, Euro 2004 fill). Each merge passed ctest and the same-save checks against the build before it. The history was rewritten on 2026-10-18 (Max's order) to remove an outside database name: every commit ID from Q340 on changed.

**Branches in progress (all local, not pushed; build on in-review branches is approved):**
- Slices 15 and 18 together: `claude/slice-18-fb9fae` (session "0.6.0 slice 18") holds slice 15 (finished at a9e8176 on `worktree-agent-a94d9c248b2926432`) and slice 18 with its review fixes; it regenerates the expected files once for both. Next: same-save set, then merge (it also brings the UTF-8 path fix, a base game fix for accented league names on macOS).
- Slice 19 (`claude/slice-19-02249b`), on slice 18: play-offs, splits, calendar.
- Slice 21b (`worktree-agent-a53abeb084e1c3a35`): cups that end or pause, round counts, Coppa Italia and KNVB; reviewed, fix round sent; merges after slice 26. Slice 21c (an agent worktree): group-stage cups, four-club super cups, new cups through slice 26's module.
- Slice 23 (`claude/slice-23-3295ac`): reviewed, fix round sent (preliminary rounds kept, real Super Cup weekdays, save and load of the Europa League hand-over).
- Slice 24 (`claude/slice-24-72016d`), on slice 23: 24a reviewed with fixes sent; 24b (Conference League record) next.
- Slice 26 (`claude/slice-26-d5d47b`): parts a and b reviewed, fix round sent; the module `season_comps.h` makes new competition records (911a805 on). Next 26c: December Club World Championship, 32-club Club World Cup, confederation counts, Sudamericana, FIFA Intercontinental Cup, Recopa, Suruga, ACL Two.
- Slice 27 (`claude/slice-27-36f393`): reviewed, fix round sent (Euro qualifying places, Q152 host allocation, rules-off formats); merges before 26 and 28. Both 26 and 27 wrap 0x40f590 and the box fix and 27 both wrap 0x57c710: the merge notes are in the review messages and `slice-28-notes.md`.
- Slice 28 (`claude/slice-28-a9e4a2`), on 27 and 26: Nations League and Euro paths; then the Gold Cup, CONCACAF Nations League and Finalissima on new records.
- Own databases (`own-db-rebuild` in `/Users/max/Projects/cm0102/wt-own-db-0405`): squad lists, key players from FM ratings, loans, careers, reserve squads; one review round done; open: the Summer 2004 Germany box (match_man 1225 in 4 of 37 games), the Spain and Portugal tests, the final rebuild of both seasons with signature rows.
- Not started: 15b (missing top and second tier clubs; MLS stays at 12), 20 (new divisions), 25 (league phase), 29 (48-team World Cup). Briefs are in `docs/research/brief-0.6.0/` (shared.md plus one file per slice).

**Merged 2026-10-19:** `fix/uefa-screens` (merge 423df60): UEFA Cup and Champions Cup Schedule tabs, history rows held twice. ctest 27 of 27; same-save start.sav, prematch.sav and new game equal to the build before (ref-builds/main-841d270); three new-game runs failed first on 120 s setup waits at load 61 and passed alone.

**Done outside the build:** the 2006/07 database is posted on the champman0102 forum (f=33, `/Users/max/Projects/cm0102/forum-packages-work/HANDOVER.md`). Screenshots for the 0.6.0 post and video: `/Users/max/Projects/cm0102/release-0.6.0-images/` with `SHOTLIST.md`. v0.5.1 (Mac fix) has site-assets.zip now, so the site shows 0.5.1. The site has a "Coming in 0.6.0" section (2026-10-19) with `assets/choose-season-2025-mockup.png` (the Choose Season screen with 2025/26 picked) and `assets/new-rules-news-mockup.png` (the 2020/21 new rules news item). The pictures are in `site-release-assets/` and in site-assets.zip on v0.5.1, so the 0.6.0 release zip must carry it too (make_release.sh picks it up from that folder). At the 0.6.0 release, turn the section into the release text.

**Session limit (Max, 2026-10-19 ~17:00):** at most 4 active slice sessions, because 13 at once used 55 % of the 5-hour usage in 75 minutes. Resume a paused session only when an active one is merged. After each merge, log progress in the state files and here. Active: the lead, slice 18+15 (local_cf8a5a93-e1d4-4b6a-81b0-12b3778b4ff2), slice 27 (local_548dc7ac-3e80-43e6-a342-55f3771bfe29), slice 23 (local_6d6bc021-a021-435c-bc6e-5c4c9829dcf0). Paused, each with WIP committed and its state file in `lead-notes/session-states/`:
- slice 26: local_ab0a28e0-5fca-4d55-bd92-f76efe7a0283
- slices 21b + 21c: local_aa422f44-fbb1-40ad-b734-76c62f2d6517
- slice 24: local_755c5eaf-5094-4322-83ab-c16d0b0de828
- slice 19: local_4ebac424-82a7-4531-b975-3c92b1093d47
- slice 28: local_96122d19-e5d8-4160-883f-8c7fc24a73ce
- slice 15b: local_b56ded26-cc18-4d03-94e6-de68f554b7bd
- slice 20: local_682d35eb-7111-483a-8a48-1115cc4f0d9c
- slice 25: local_37eac6dd-a679-45e0-927b-b2a3d9441d6e
- slice 29: local_30be1b53-acff-46de-8b31-8e09eef2cee4
- own databases: local_083fc747-a597-473e-8131-6699c8f40ff0

**Faster process (Max, 2026-10-19):** tracks run in parallel within the session limit; one Opus review per slice (was two); the lead runs the same-save set once, after the merge (not also on the branch); finished branches merge in batches of 2 to 4 with one build, ctest and same-save set per batch (bisect on a failure). Scope stays whole: no slice moves to a later release.

**How to run it:** test game limit 16 machine-wide, load below 24 (Max, 2026-10-19; above 18 games the cores are full); no `pkill`; regenerate only the expected files a slice changes, the lead regenerates the full set on main; slices as sessions (Max sees them); two Opus reviews per slice; the lead runs the same-save set during review.

## History

Older build state (2026-10-17), facts from 0.4.1, 0.4.0 and 0.3.0, and the function map note: `docs/history/handover-2026-10-17.md`. That file also holds the release notes items for slice 30, the known issues for later slices, the worktree clean-up list, the pointer to `/Users/max/Projects/cm0102/HANDOVER-PRIVATE.md`, the pointer to `docs/requests.md` and the Colima start command. Read it before slice 30 or a clean-up.

## Open items

0. **HD mode is removed (Max, 2026-10-16, Q267).** The HD text work (spec `docs/specs/2026-10-16-hd-text-0.5.1-spec.md`, now Dropped) was built, tried by Max on his MacBook Pro and undone: bars around the picture, yellow patches and small text, while widescreen looks good. HD mode itself is gone too; an old `display = hd` gives widescreen. Max's wish for later (2026-10-16): slightly bigger text in widescreen, and bigger text for every item of the left bar (date, Continue Game, the manager's name, Competitions, Nations & Clubs, Find, Game Options), not only Game Options. Grill first. The next release also has the AppImage fix for the AppImage catalogue (pull request AppImage/appimage.github.io#10317: comment `/retest` there after the release). Never name another modder, their patch or tool in any committed file or message.
1a. **Own season databases (Q351, Q381): paused for usage (2026-10-17).** Summer 2004 (2004/05) and Summer 2006 (2006/07) are season databases, built from Football Manager 2005 and 2007 facts with Update Team ratings (`tools/seasons/own_db/`, its README, spec `docs/specs/2026-10-17-own-db-2004-05-spec.md`). PRs #62 and #63 (review fixes) are merged. Next: loans, key players with no record (Q383) and the missing 2002/03 and 2003/04 careers, in one rebuild. The plan, the rebuild steps and the tests are in `/Users/max/Projects/cm0102/fm2005-work/HANDOVER.md` (outside the repo, with the FM data).
5. **0.7.0 easy online play (#45):** network games today need an extra patch and a VPN such as Tailscale; make it quick and easy. The original's network game uses WinSock TCP; `src/recomp_rt/win/net.c` answers every call with "no network". First notes, the ways to connect and the helper server cost: `docs/research/2026-10-16-online-play.md`. Grill before building.
6. **Later: stadium expansion (#55, Max's request).** Research in `docs/research/2026-10-16-stadium-research.md`: the original already has "Expand the Stadium" in Meet with Board and a yearly expansion routine (0x59cfa0), but 7,269 stadiums have expansion capacity 0 (Highbury among them). Seven decisions are open in that file.
7. **Manual checks Max has not done:** the covered-name underline on Match Overview during a goal; the Windows keys (Ctrl+[, F11) on a Windows PC; `remember_window` on two screens and in full screen; the AppImage on a Steam Deck.
8. **Open checks:** #30 "no match in 60 days" has no test; the same-save comparison against a manual Return from holiday is not done.

## The product

- **CM 01/02: Bosman Edition**, an unofficial port of Championship Manager 01/02 (3.9.68) to Mac, Windows and Linux. The whole original is recompiled from x86 to C (`tools/recomp/`, runtime `src/recomp_rt/`, target `cm0102_recomp`).
- GitHub `maxevans60/cm0102-bosman-edition`. Site https://maxevans60.github.io/cm0102-bosman-edition/ from `site/` (pictures come from `site-assets.zip` on the release).
- Pre-releases: v0.1.0 and v0.2.0 (Mac, Windows), v0.3.0 (Mac, Windows, Linux AppImage; three data sets: 3.9.60, 3.9.65, 3.9.68). All 2026-10-14/15.
- **Maybe** (label `maybe`, no milestone): #24 team talks, #36 2D match view. Later, no milestone: #37 Android, #38 iOS.
- Decided against: a "more frequent" violent conduct option (Q61), a screenshot key, a warning before quit (the original has one), highlights of young players (needs hidden potential values), a newer match engine (it would change results).

## How to make a release

1. Bump `project(... VERSION x.y.z)` and add the `CHANGELOG.md` section. Commit.
2. Dry run: `tools/release/make_release.sh --game /Users/max/Projects/cm0102/clean-3968 --data-3960 /Users/max/Projects/cm0102/data-3960 --data-3965 /Users/max/Projects/cm0102/data-3965 --sdl2-mac /Users/max/Projects/cm0102/sdl2 --sdl2-mingw /Users/max/Projects/cm0102/sdl2/mingw-2.32.10 --site-assets /Users/max/Projects/cm0102/site-release-assets --season-dbs /Users/max/Projects/cm0102/test-fixtures/databases/zips --own-dbs /Users/max/Projects/cm0102/own-dbs` (it holds `Summer 2004.zip` and `Summer 2006.zip`, the source zips of our own season databases; `tools/seasons/own_db/README.md` rebuilds them). `--own-dbs` holds the source zip of our own 2006/07 database (Summer 2006). Then the same with `--publish` after Max says yes. Colima must run for the Linux step. The checkout must be clean.
3. The site workflow runs on the release (the Pages environment allows `main` and tags `v*`). If not: `gh workflow run pages.yml --ref main`.
4. Posts: the thread post with two or three pictures and the updated original Reddit post, every release. Use the `release-posts` skill (`.claude/skills/release-posts/`).

## Tests

- `python3 tests/smoke/smoke_run.py build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 /Users/max/Projects/cm0102/smoke-ref --mode classic`, `--mode widescreen` and `--same-save`. Run test games in parallel (Max, 2026-10-15). If a parallel run fails on timing, rerun it alone first.
- `tests/recomp/test_load.py` (see its docstring), `ctest -R options` in `build/`, `tests/smoke/save_folder_run.py`, `tests/smoke/save_backups_run.py`, `tests/seasons/season_screen_run.py` (Choose Season screen, 0.6.0 slice 13; it replaces the Database button test). `tests/smoke/custom_database_run.py` takes `/Users/max/Projects/cm0102/smoke-ref-slice11` as its reference folder: `smoke-ref` has no 3.9.60 pictures.
- `tests/smoke/intro_logo_run.py <program> <game> "<test-fixtures/databases/April 2022>" --logo <site-release-assets/logo.png> --same-save`: the intro logo in classic and widescreen, no folder pictures, and the same save with and without the logo.
- While building, run only the touched test; the full set once per slice.

## Rules

- Token saving (2026-10-19, spec `docs/specs/2026-10-19-token-saving-spec.md`): hooks send builds and tests through `tools/dev/run_logged.py` (full logs in `/Users/max/Projects/cm0102/run-logs/`), cap grep at 100 lines and shrink viewed PNGs to 1024 px. `RAW=1` turns the Bash hook off for one command. Run the Q6 trial in the spec at the next fix rounds.

- Builds go through the machine-wide queue (Max, 2026-10-19): `/Users/max/Projects/cm0102/build-slot.sh cmake --build build -j4` (any build command after the script). At most 3 builds run at once; the script waits for a free slot and keeps the exit code. Never more than `-j4`.
- Ghidra (the rea MCP) takes 2 to 4 cores per open binary. Open a binary only when a decompile is needed, and call `close_binary` when done.
- No stale games (Max, 2026-10-19). When a session stops waiting on a run (result known, run replaced, plan changed, turn ends), it stops that run's games and test scripts by PID, never `pkill`. Debug games opened by hand close once the answer is known. Before a session ends a turn or reports done, it checks `ps -axo pid,ppid,etime,command | grep "[c]m0102_recomp"` for games from its own worktree or scratchpad and stops any that no test script waits on. The lead checks the same list after each merge and at each wake-up, and stops orphans (parent PID 1) and games of finished sessions.
- Never commit the original's code or data, saves, screenshots or the logo. Downloads use the clean files only (`/Users/max/Projects/cm0102/clean-3968`, made by `tools/clean_files/`); the fan pack in `/Users/max/Downloads/Game` is never a build input.
- No feature may change the game's decisions while the player makes the same clicks, except the team that is picked. A change of memory layout counts like opening a screen: the original's results already depend on where its memory blocks sit (Q152). Classic and widescreen saves differ for this reason, as they always have.
- Every option is a line in `settings.txt`. Feature issues labelled `enhancement` show on the site's "Coming next" list live, so their titles are written for players. Bugs use `bug` and never show.
- Grilling before a new feature, then the spec. One shared brief for parallel agents; Opus for research, review and anything a player sees, Sonnet for routine builds.
- Agents must not run `rm` with a wildcard or a variable path.
- Max approves every publish. He has approved pushes for this work.
