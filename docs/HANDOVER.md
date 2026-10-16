# Handover, 2026-10-16

Read this first, then `GLOSSARY.md`. v0.5.0 was published on 2026-10-16 with Mac, Windows and Linux downloads, and the site shows it. The next work is 0.6.0.

The project lives in `/Users/max/Projects/cm0102`: the repo `cm0102-mac/`, `clean-3968/`, `data-3960/`, `data-3965/`, `sdl2/`, `smoke-ref/`, `test-fixtures/` (with `yeovil.sav` and `databases/`: all 49 Update Team databases unpacked, zips in `databases/zips`), `site-release-assets/`, `save-backups/`, `builds/` and `shots/`. Start sessions in `/Users/max/Projects/cm0102`. `build/` is configured with `-DCM0102_EXE=/Users/max/Projects/cm0102/clean-3968/cm0102.exe -DRECOMP_PYTHON=/Users/max/.venvs/cm0102/bin/python`.

## Next session: 0.6.0 build

0. **HD mode is removed (Max, 2026-10-16, Q267).** The HD text work (spec `docs/specs/2026-10-16-hd-text-0.5.1-spec.md`, now Dropped) was built, tried by Max on his MacBook Pro and undone: bars around the picture, yellow patches and small text, while widescreen looks good. HD mode itself is gone too; an old `display = hd` gives widescreen. Max's wish for later (2026-10-16): slightly bigger text in widescreen, and bigger text for every item of the left bar (date, Continue Game, the manager's name, Competitions, Nations & Clubs, Find, Game Options), not only Game Options. Grill first. The next release also has the AppImage fix for the AppImage catalogue (pull request AppImage/appimage.github.io#10317: comment `/retest` there after the release). Never name another modder, their patch or tool in any committed file or message.

1. **0.6.0 seasons: the spec is ready.** `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (interview of 2026-10-16, Q231 to Q266; the next free question number is Q268; Q267 is the removal of HD mode). One release for #50 to #53, every league exact, rules that change each July (option `changing_rules`, default on), one season database per season in the download. Slices 1 to 10 are research and can run in parallel; slice 3 (the season database table) needs Max's approval. Issue text of #50 to #53 changed on 2026-10-16.
2. **Function map, for the 0.6.0 research after the interview.** The Haiku first pass of all 6,863 game and library functions is on main: one doc per source file in `docs/research/function-map/`, the index `index.md` and `names.tsv`. Its names are a first guess: of 14 proven functions, 8 agree, 1 in part and 5 are wrong. Spec: `docs/specs/2026-10-16-function-map-spec.md` (slice 3, an Opus sweep of one file, runs on demand). The Opus sweep of `league.cpp` (62 functions) and `Cup.cpp` (47) is on main; the 4 functions that research cites agree. Before the 0.6.0 research, ask for sweeps of only the other files 0.6.0 touches: the rule books (`rb_*.cpp`), the European competitions (eurocomp), the international competitions (intercomp), the world cup and the transfer rules.
3. **0.5.0 is published (2026-10-16)** with Mac, Windows and Linux downloads. Issues and milestone closed, spec marked shipped, the site shows 0.5.0 with the new logo. Posts (BBCode, ready to paste): `/Users/max/Projects/cm0102/release-0.5.0-posts.md`, pictures on the site (`assets/post-0.5.0-*.png`). Max posted them. Max accepted three build results (spec, Decisions). Build notes: worktree agents are cut from an older commit, so each brief says `git merge main` first. A wait loop with `pgrep -f "<name>"` matches its own command line and waits for ever.
4. **Skills for this work** (`.claude/skills/`): `slice-build`, `same-save-check`, `add-option`, `release` and `release-posts`. Each release gets a thread post (two or three pictures) and an updated first post in the Bosman Edition thread on the champman0102 forum, in BBCode, plus an optional Reddit update.
5. **0.7.0 easy online play (#45):** network games today need an extra patch and a VPN such as Tailscale; make it quick and easy. The original's network game uses WinSock TCP; `src/recomp_rt/win/net.c` answers every call with "no network". First notes, the ways to connect and the helper server cost: `docs/research/2026-10-16-online-play.md`. Grill before building.
6. **Later: stadium expansion (#55, Max's request).** Research in `docs/research/2026-10-16-stadium-research.md`: the original already has "Expand the Stadium" in Meet with Board and a yearly expansion routine (0x59cfa0), but 7,269 stadiums have expansion capacity 0 (Highbury among them). Seven decisions are open in that file.
7. **Manual checks Max has not done:** the covered-name underline on Match Overview during a goal; the Windows keys (Ctrl+[, F11) on a Windows PC; `remember_window` on two screens and in full screen; the AppImage on a Steam Deck.
8. **Open checks:** #30 "no match in 60 days" has no test; the same-save comparison against a manual Return from holiday is not done.
9. Player requests and credits: `docs/requests.md`. Releases are announced in the Bosman Edition thread on the champman0102 forum (the `release-posts` skill).
10. The build worktrees of 0.4.0 and 0.5.0 were removed on 2026-10-16 (Max). Their branches were merged into main.

Facts from 0.4.1:
- Intro logo (Q205 to Q209): a data folder never supplies a `.rgn` file. `Data/bosman_logo.rgn` (made by `tools/release/make_intro_logo.py` from the site logo, in the downloads only) replaces `Data/logo.rgn`. The intro fade 0x5e30e0 allocates one block per colour of the picture (0x5e31e3), so the logo has exactly the 46 colours of the SI logo. With 3248 colours the save after 14 days differed in every part.
- The save of a game depends on the game folder path: the same game in another folder differs in `human_manager.dat`. Compare runs in the same folder.

Facts from 0.4.0:
- **Same game rule (Q152):** a feature must not change the game's decisions while the player makes the same clicks. The original's results depend on where its memory blocks sit: opening any screen, a box or a different display mode changes later days. Same-save checks therefore compare the same steps with the option on and off.
- Max's Yeovil save had Skalidis's release clause removed on 2026-10-16 through the game's own Save Game (backup `save-backups/Yeovil-2026-10-16-before-skalidis-edit.sav`). A direct call of the save function 0x82e430 failed on that screen with "human_manager..cpp 2710". A Python edit of `contract.dat` is not possible yet: the member has no fixed record size.
- `tools/save/savfile.py` reads and writes the .sav container; `tools/save/edit_money.py` sets a club's bank balance.
- The picker scores by the average of the key shown attributes for the position times fit / 20 (Q138 to Q140), with an exact assignment (Q130). Best XI, squad depth and the comparison share `picker_shown_attribute` for other clubs.
- Update Team databases need their whole folder, euro.cfg included; a mismatched euro.cfg made the original loop forever until the skip guard at 0x905a4c.
- Questions for Max go in full in the last message of a turn, never only above the wrap-up box (Max, 2026-10-16).

Facts from 0.3.0:
- Saved test games in `/Users/max/Projects/cm0102/test-fixtures`: `start.sav` (Arsenal 14.7.01, first friendly 18.7.01) and `prematch.sav` (16.7.01, Basel friendly 20.7.01; another game, other fixtures).
- Wait steps in scripts (`src/recomp_rt/win/waits.c`), `--match` and `--match --speed fastest` in `tests/smoke/smoke_run.py`.
- In the original the commentary speed already changes match results (background matches step while a line waits). So a speed is checked for repeatability, not against another speed (Q99).
- Game addresses that the runtime calls go through `tools/recomp/callbacks.py` (checked bytes) or `tools/recomp/callsites.py`.
- Docker for the Linux build: Colima with `--vm-type vz --vz-rosetta` (`colima start --arch aarch64 --cpu 6 --memory 8 --vm-type vz --vz-rosetta`).
- Contract clauses in memory and how to edit a save through the game: `docs/research/2026-10-15-contract-clauses.md`. Max's Yeovil save had Moukoko's release clause removed on 2026-10-15 (backup in `/Users/max/Projects/cm0102/save-backups`).
- The old preview server on port 8765 serves the old folder. Use the `site-8766` entry in `/Users/max/Projects/cm0102/.claude/launch.json`.

Function map (2026-10-16): `docs/research/function-map/index.md` gives the source file and a Haiku first-pass name of every function of the original. Names are hints (8 of 14 proven checks right). Sweep a file with Opus before you rely on it: `tools/research/function_map/README.md`.

## The product

- **CM 01/02: Bosman Edition**, an unofficial port of Championship Manager 01/02 (3.9.68) to Mac, Windows and Linux. The whole original is recompiled from x86 to C (`tools/recomp/`, runtime `src/recomp_rt/`, target `cm0102_recomp`).
- GitHub `maxevans60/cm0102-bosman-edition`. Site https://maxevans60.github.io/cm0102-bosman-edition/ from `site/` (pictures come from `site-assets.zip` on the release).
- Pre-releases: v0.1.0 and v0.2.0 (Mac, Windows), v0.3.0 (Mac, Windows, Linux AppImage; three data sets: 3.9.60, 3.9.65, 3.9.68). All 2026-10-14/15.
- **Maybe** (label `maybe`, no milestone): #24 team talks, #36 2D match view. Later, no milestone: #37 Android, #38 iOS.
- Decided against: a "more frequent" violent conduct option (Q61), a screenshot key, a warning before quit (the original has one), highlights of young players (needs hidden potential values), a newer match engine (it would change results).

## How to make a release

1. Bump `project(... VERSION x.y.z)` and add the `CHANGELOG.md` section. Commit.
2. Dry run: `tools/release/make_release.sh --game /Users/max/Projects/cm0102/clean-3968 --data-3960 /Users/max/Projects/cm0102/data-3960 --data-3965 /Users/max/Projects/cm0102/data-3965 --sdl2-mac /Users/max/Projects/cm0102/sdl2 --sdl2-mingw /Users/max/Projects/cm0102/sdl2/mingw-2.32.10 --site-assets /Users/max/Projects/cm0102/site-release-assets`. Then the same with `--publish` after Max says yes. Colima must run for the Linux step. The checkout must be clean.
3. The site workflow runs on the release (the Pages environment allows `main` and tags `v*`). If not: `gh workflow run pages.yml --ref main`.
4. Posts: the thread post with two or three pictures and the updated original Reddit post, every release. Use the `release-posts` skill (`.claude/skills/release-posts/`).

## Tests

- `python3 tests/smoke/smoke_run.py build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 /Users/max/Projects/cm0102/smoke-ref --mode classic`, `--mode widescreen` and `--same-save`. Run test games in parallel (Max, 2026-10-15). If a parallel run fails on timing, rerun it alone first.
- `tests/recomp/test_load.py` (see its docstring), `ctest -R options` in `build/`, `tests/smoke/save_folder_run.py`, `tests/smoke/save_backups_run.py`, `tests/smoke/database_order_run.py` (Database button order). `tests/smoke/custom_database_run.py` takes `/Users/max/Projects/cm0102/smoke-ref-slice11` as its reference folder: `smoke-ref` has no 3.9.60 pictures.
- `tests/smoke/intro_logo_run.py <program> <game> "<test-fixtures/databases/April 2022>" --logo <site-release-assets/logo.png> --same-save`: the intro logo in classic and widescreen, no folder pictures, and the same save with and without the logo.
- While building, run only the touched test; the full set once per slice.

## Rules

- Never commit the original's code or data, saves, screenshots or the logo. Downloads use the clean files only (`/Users/max/Projects/cm0102/clean-3968`, made by `tools/clean_files/`); the fan pack in `/Users/max/Downloads/Game` is never a build input.
- No feature may change the game's decisions while the player makes the same clicks, except the team that is picked. A change of memory layout counts like opening a screen: the original's results already depend on where its memory blocks sit (Q152). Classic and widescreen saves differ for this reason, as they always have.
- Every option is a line in `settings.txt`. Feature issues labelled `enhancement` show on the site's "Coming next" list live, so their titles are written for players. Bugs use `bug` and never show.
- Grilling before a new feature, then the spec. One shared brief for parallel agents; Opus for research, review and anything a player sees, Sonnet for routine builds.
- Agents must not run `rm` with a wildcard or a variable path.
- Max approves every publish. He has approved pushes for this work.
