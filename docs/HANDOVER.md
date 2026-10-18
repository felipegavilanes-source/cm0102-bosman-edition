# Handover, 2026-10-18 (afternoon)

Read this first, then `GLOSSARY.md`. v0.5.0 was published on 2026-10-16 with Mac, Windows and Linux downloads, and the site shows it. The next work is 0.6.0.

The project lives in `/Users/max/Projects/cm0102`: the repo `cm0102-mac/`, `clean-3968/`, `data-3960/`, `data-3965/`, `sdl2/`, `smoke-ref/`, `test-fixtures/` (with `yeovil.sav` and `databases/`: all 49 Update Team databases unpacked, zips in `databases/zips`), `site-release-assets/`, `save-backups/`, `builds/` and `shots/`. Start sessions in `/Users/max/Projects/cm0102`. `build/` is configured with `-DCM0102_EXE=/Users/max/Projects/cm0102/clean-3968/cm0102.exe -DRECOMP_PYTHON=/Users/max/.venvs/cm0102/bin/python`.

## 0.6.0 build state (2026-10-18 afternoon, written for a new account)

Read this section first; the section of 2026-10-17 below is older.

**PAUSED 2026-10-18 ~14:45 for the account change. Resume here.**
1. Read every file in `/Users/max/Projects/cm0102/lead-notes/session-states/` (one per track: branch, last commit, what is done, where it stopped, open review items, next steps, exact test commands, decisions for Max). Slices 19, 26 and 27 were still writing theirs at the pause: if a file is missing, read that branch's last commits and its doc (`docs/research/2026-10-18-*.md` on the branch).
2. Read `/Users/max/Projects/cm0102/lead-notes/decisions-for-max.md` (31 decisions the lead took overnight; ask Max to confirm them, then record them in the spec as Q373 on).
3. Branch tips at the pause: slice 18 (with slice 15) `claude/slice-18-fb9fae` 34562ef; slice 19 `claude/slice-19-02249b` c3981de; slice 21b `worktree-agent-a53abeb084e1c3a35` 1549374; slice 21c `worktree-agent-aa571b77878dc5408` a3054c8; slice 23 `claude/slice-23-3295ac` 4e29665 (fixes committed, not built or tested); slice 24 `claude/slice-24-72016d` fec0d2a; slice 26 `claude/slice-26-d5d47b` 28a3a76; slice 27 `claude/slice-27-36f393` 9ac19ae; slice 28 `claude/slice-28-a9e4a2` dd231e4; UEFA screen fixes `fix/uefa-screens` 9730af6 (three bugs fixed, same-save not run); own databases `own-db-rebuild` 1c5a83e in `/Users/max/Projects/cm0102/wt-own-db-0405` (own-dbs zips match this branch's signature rows, not main's). None is pushed; all are local branches of this repo.
4. Merge order: fix/uefa-screens (after its same-save run); slice 18 with 15 (after expected files and the Database.cpp 13471 box in its group (b) 2010 run); slice 27; slice 26 (joins 27's 0x40f590 wrap); slices 21b, 21c, 23, 24, 19, 28; own-db-rebuild. For each: the lead runs the same-save set on the branch, merges with --no-ff, builds, runs ctest and same-save against the build before, pushes. Then 15b, 20, 25, 29 and slice 30 (release; Max approves the publish).
5. Not mine, do not merge: the "LazzTommy.sav file issue" session's uncommitted change in `/Users/max/Projects/cm0102/wt-accent-names` (slice 15's path fix replaces it). Old reference builds for comparisons: `/Users/max/Projects/cm0102/ref-builds/`.
 The lead's decision list for Max to confirm (31 items) is in `/Users/max/Projects/cm0102/lead-notes/decisions-for-max.md` (outside the repo): record the confirmed ones in the spec as Q373 on.

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
- Not started: 15b (missing top and second tier clubs; MLS stays at 12), 20 (new divisions), 25 (league phase), 29 (48-team World Cup). Briefs are in `docs/research/build-brief-0.6.0.md`.

**Bugs found on main, assigned:** UEFA Cup Schedule tab dates ("52180") and Champions Cup Phase 2 listing in 2004/05, Premier history 2010 row twice (fix branch `fix/uefa-screens` by the slice 22 agent).

**Done outside the build:** the 2006/07 database is posted on the champman0102 forum (f=33, `/Users/max/Projects/cm0102/forum-packages-work/HANDOVER.md`). Screenshots for the 0.6.0 post and video: `/Users/max/Projects/cm0102/release-0.6.0-images/` with `SHOTLIST.md`. v0.5.1 (Mac fix) has site-assets.zip now, so the site shows 0.5.1.

**How to run it:** test game limit 12 machine-wide, load below 18; no `pkill`; regenerate only the expected files a slice changes, the lead regenerates the full set on main; slices as sessions (Max sees them); two Opus reviews per slice; the lead runs the same-save set during review.

## 0.6.0 build state (2026-10-17 evening, written for a new account)

The lead session stopped here because its usage ran out. Everything below is on GitHub (main and two `wip/` branches) or on this Mac. Use the `slice-build` skill to go on.

**Read first:** this section, `GLOSSARY.md`, the spec `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (Decisions to Q360; the next free question number is Q361), `docs/research/build-brief-0.6.0.md` (one section per slice, shared facts, same-save checks, rules) and `docs/research/review-brief-0.6.0.md`. Private notes outside the repo: `/Users/max/Projects/cm0102/HANDOVER-PRIVATE.md` (read it: one task there may not go in git).

**On main (all reviewed by two Opus agents, merged, pushed, checks pass):**
- Slice 11, season core: start year at run time, `bosman_season.dat` part in the save (written only for a non-base game), option `changing_rules` (default on). Docs `docs/research/2026-10-17-season-save-part.md`, `2026-10-17-first-season-branches.md`.
- Slice 11b, right ages and dates: measured date shifts per database and per field, applied in memory at load. Doc `2026-10-17-season-date-shifts.md`.
- Slice 11c, competition history 2002 to the start season, researched (2,251 rows plus a 2000/01 pass), filled in memory. Doc `2026-10-17-competition-history-fill.md`, data `docs/research/history-0.6.0/`.
- Slice 12, rules table reader (`Seasons/rules.tsv` in the download) and the rules report (`RECOMP_SEASON_JUMP`, `tests/seasons/rules_report_check.py`; per slice run `--year X --last X+1`, about 6 to 7 minutes; the full jump to 2027/28 about 56 minutes, once before the release). Doc `2026-10-17-rules-report.md`.
- Slice 13, Choose Season screen, layout C (Q346): decade rows, ten columns, strip below the grid in both modes, game fonts, database name in font 6 in widescreen. Mockups `docs/research/mockups/2026-10-17-season-screen-c/`.
- Slice 14, season databases in the download (`Seasons/<name>.zip`, unpacked on first pick, recognised by `club.dat`/`staff.dat` SHA-256).
- Our own 2004/05 and 2006/07 databases (Q351, Q381; PRs #62, #63, #64). Follow-up work (loans, key players Q383, 2002/03 and 2003/04 careers) is in `/Users/max/Projects/cm0102/fm2005-work/HANDOVER.md`.
- Select All fix: the staff reserve sized from the data (Database.cpp 1844 gone). Doc `2026-10-17-select-all-databases.md`.
- Licence: GPL-3.0 from 0.6.0 (CC0 up to 0.5.0); the function and data maps stay CC BY 4.0. Say so in the 0.6.0 changelog.

**Paused, not merged (pushed to GitHub as `wip/` branches; the local worktrees under `.claude/worktrees/` hold the same commits):**
- **Slice 16, season change and news**: branch `wip/slice-16` at f67ea69 (local `worktree-agent-a383031353b901183`; the agent was stopped mid-fix, so build and test it before anything else). Built and reviewed; the review fix round was in progress. Merge it before slice 15. Open items (from the two reviews, sent to the agent; the WIP commit message says which are done): (1) registry hooks before `start_new_season`, at load and at a new game, scope by nation and entry, and competitions that start in a later season; (2) one news item per season at the July change, no early start for calendar-year leagues; (3) tell a real new season from a moved entry; (4) tests read the news item and play on after a load, base checks with `--ref-050`; (5) a v2 reader that meets a later version keeps the renames; (6) rename chains keyed by record; notes (MAX_PARTS evidence, item size comment, cut long news text, news type default case); Q358 (seasons written "2009/10", not "2009/0", via 0x669630; the original text with `RECOMP_SEASON_CODE=off`).
- **Slice 15, club moves and names**: branch `wip/slice-15` at a33318f (local `worktree-agent-a94d9c248b2926432`; stopped mid-fix: it was rewriting the moves section, build and test it first). Built and reviewed; the fix round was in progress. Open items: (1) keep listed clubs up to the count the class takes, not the database count (GreSecond 14 at 0x5ebcc2), fix the expected files; (2) the "Disk Space Error" box (file_llist.cpp line 161, `fopen "r+b"` at 0x58e5c1 fails on an existing list file): find and fix the cause, remove the `SKIP_SEASON` row; (3) after slice 16 is on main, rename through `season_change_rename` and drop the save-time name restore; (4) Q359: fill lower tiers and B-team lists with same-nation clubs with no division, leave the about 80 top and second tier places open for slice 15b; (5) a resized division with no list (Polish Second 2004, 2006); (6) French Lower Division shows 0 clubs; (7) the check compares club names, not only counts; (8) two name bugs in `make_names.py`; (9) a name miss in `missing.tsv` (Operario 2005).

**Our own 2004/05 and 2006/07 databases (Q351, Q381): two open lines of work.**
- Integration in the game is done (PRs #62, #63, #64 on main: signature and date-shift rows, tiles, release zips, history fill). The source zips are in `/Users/max/Projects/cm0102/own-dbs` (the release script takes `--own-dbs`).
- Database follow-up (loans, key players with no record Q383, the 2002/03 and 2003/04 careers, one rebuild with new signature rows): plan and steps in `/Users/max/Projects/cm0102/fm2005-work/HANDOVER.md`; the 2006/07 notes in `/Users/max/Projects/cm0102/fm2007-work/HANDOVER.md` and `FINDINGS.md`; build commands in `tools/seasons/own_db/README.md`.
- Forum release packages for both databases (so players of the original game can use them too): a session was working on it when this account stopped. Its state is in `/Users/max/Projects/cm0102/forum-packages-work/HANDOVER.md` if that session wrote it; if the file is missing, ask Max what the packages should hold. Nothing is posted to the forum without Max.

**Sessions of this task group in the Claude app (state on 2026-10-17 evening):**
- "2004/05 database from FM 2005 facts (worktree)": done (PRs #62, #63, #64 merged). Its open follow-up is in `/Users/max/Projects/cm0102/fm2005-work/HANDOVER.md`.
- "2006/07 database from FM2007 facts (worktree)": database done and merged; asked to write its current state and open items to `/Users/max/Projects/cm0102/fm2007-work/HANDOVER.md` (if that file still shows only the starting plan, read `fm2007-work/FINDINGS.md` and ask Max).
- "Forum release packages for 2004/05 and 2006/07": was still running; asked to stop and write `/Users/max/Projects/cm0102/forum-packages-work/HANDOVER.md`.
- "Register the 2006/07 season database in 0.6.0": done; its PR #61 is closed, replaced by #62.
- "Read an outside 2004/05 database data format with REA": finished and closed for good. Its work was deleted on Max's word; never continue it.

**Next slices, in order** (Opus for review and anything a player sees; Sonnet where reasonable for routine slices, Max 2026-10-17):
1. Finish and merge slice 16, then slice 15 (re-run the base checks after each merge).
2. In parallel after 16: slice 17 (substitutes, foreign rules, permits, windows; Sonnet build, Opus reviews), 18 (league sizes, promotion, relegation; Opus), 21 (domestic cups; Sonnet build), 22 (UEFA 2003/04 to 2008/09; Opus). Their brief sections are ready.
3. After 15: slice 15b (Q359: about 80 real top and second tier clubs as new records; write its brief section).
4. Then 19 (play-offs, splits, calendar), 20 (new divisions), 23, 24, 25 (UEFA eras), 26 (FIFA and other club competitions), 27 (international competitions), 28 (Nations League and Euro paths), 29 (48-team World Cup). Write a brief section for each before it starts. Slice 30 is the release; Max approves the publish.

**How each slice runs:** the agent starts with `git merge main`, reads the brief, builds test first, runs at most 3 test games at a time (`uptime` above about 20 makes runs fail on timing; rerun alone), and reports in at most 300 words. Two fresh Opus reviewers (spec, standards) per slice; fixes go back to the same agent in one message; a design change after review gets one more review. The lead merges with `git merge --no-ff`, rebuilds `build/`, runs `ctest` and `smoke_run.py build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 --same-save --save start.sav --ref-050 /Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp` (and `--save prematch.sav`, and a new game), then pushes. Pushes for this work are approved; Max approves every publish.

**Known issues for later slices:** Ligue 1 has 18 clubs from 2023/24 but the class gives 20 (slice 18); the MLS code knows its 12 clubs by name and stops at usa_mls.cpp 1819 on any other (slice 18); the Belgian mapping (Q292) and the Swedish, Finnish, Portuguese, Spanish and Turkish group splits (slice 18 and 19); Scottish cup entrants and academy sides (slice 21); 1,641 real clubs that databases lack (Q359: lower tiers filled from the database, about 80 as real records in 15b).

**Release notes items (slice 30):** careers that a database never updated stop in 2001 (Q348); from September 2009 most players joined their club in 2008 (Q349); award history keeps its years (Q350); the 2000/01 Portuguese Second League runner-up is kept as Varzim (three clubs on 64 points); licence change to GPL-3.0.

**Other sessions on this Mac (may still show in the Claude app):** a "forum release packages" session started from the 2006/07 work (its purpose is not checked by the lead; look at it before relying on it). The 2004/05 and 2006/07 database sessions are done.

**Clean-up when convenient:** the merged worktrees under `.claude/worktrees/` (a20ca97, a6a0e70, a870b2f, ace4b1c, aded5da, af01d0a, af4a418) and `/Users/max/Projects/cm0102/wt-season-2006`, `wt-own-db-0405`, `wt-remove-hd`, `wt-hd-text` can be removed with `git worktree remove` once nothing is open in them. Keep `wt-ref-0.5.0` (the 0.5.0 reference build) until 0.6.0 ships.


## Next session: 0.6.0 build

0. **HD mode is removed (Max, 2026-10-16, Q267).** The HD text work (spec `docs/specs/2026-10-16-hd-text-0.5.1-spec.md`, now Dropped) was built, tried by Max on his MacBook Pro and undone: bars around the picture, yellow patches and small text, while widescreen looks good. HD mode itself is gone too; an old `display = hd` gives widescreen. Max's wish for later (2026-10-16): slightly bigger text in widescreen, and bigger text for every item of the left bar (date, Continue Game, the manager's name, Competitions, Nations & Clubs, Find, Game Options), not only Game Options. Grill first. The next release also has the AppImage fix for the AppImage catalogue (pull request AppImage/appimage.github.io#10317: comment `/retest` there after the release). Never name another modder, their patch or tool in any committed file or message.

1. **0.6.0 seasons: see "0.6.0 build state" above.**
1a. **Own season databases (Q351, Q381): paused for usage (2026-10-17).** Summer 2004 (2004/05) and Summer 2006 (2006/07) are season databases, built from Football Manager 2005 and 2007 facts with Update Team ratings (`tools/seasons/own_db/`, its README, spec `docs/specs/2026-10-17-own-db-2004-05-spec.md`). PRs #62 and #63 (review fixes) are merged. Next: loans, key players with no record (Q383) and the missing 2002/03 and 2003/04 careers, in one rebuild. The plan, the rebuild steps and the tests are in `/Users/max/Projects/cm0102/fm2005-work/HANDOVER.md` (outside the repo, with the FM data).
2. **Function map: the sweep and the review pass are done (2026-10-17).** Every one of the 6,863 game and library functions has a sweep name: Opus for the game (6,613 rows), Sonnet for the 250 library functions without asserts (Max's choice, 2026-10-17). One doc per source file is in `docs/research/function-map/`, with `index.md` and `names.tsv`. The estimate doc `docs/research/2026-10-16-function-map-estimate.md` records the checks of each wave. Spec: `docs/specs/2026-10-16-function-map-spec.md`, slice 3 done. The review pass is done too (2026-10-17): fresh Opus reviewers and third checks checked all 2,002 medium, low, shared stub and false entry rows, and 842 rows changed. Then the data map and the confidence pass ran (spec `docs/specs/2026-10-17-data-map-and-confidence-spec.md`, shipped 2026-10-17). Every global has a name in `docs/research/data-map/`. Confidence is now 5,702 high, 1,133 medium and 28 low. Max accepted the rows that are not high by group. Their reasons are in `function-map/not-high.tsv`. Read a function's code before you rely on a medium name. The work folder handover is `/Users/max/Projects/cm0102/funcmap-work/HANDOVER.md`.
3. **0.5.0 is published (2026-10-16)** with Mac, Windows and Linux downloads. Issues and milestone closed, spec marked shipped, the site shows 0.5.0 with the new logo. Posts (BBCode, ready to paste): `/Users/max/Projects/cm0102/release-0.5.0-posts.md`, pictures on the site (`assets/post-0.5.0-*.png`). Max posted them. Max accepted three build results (spec, Decisions). Build notes: worktree agents are cut from an older commit, so each brief says `git merge main` first. A wait loop with `pgrep -f "<name>"` matches its own command line and waits for ever.
4. **Skills for this work** (`.claude/skills/`): `slice-build`, `same-save-check`, `add-option`, `release`, `release-posts` and `league-research` (how to research a league or competition for the rules table). Each release gets a thread post (two or three pictures) and an updated first post in the Bosman Edition thread on the champman0102 forum, in BBCode, plus an optional Reddit update.
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

Function map (2026-10-17): `docs/research/function-map/index.md` gives the source file and a sweep name of every function of the original (Opus for the game, Sonnet for the library). After the review pass and the confidence pass, 5,702 rows are high, 1,133 medium and 28 low. `function-map/not-high.tsv` gives the reason for each row that is not high. A medium name is still a hint. Read the code before you rely on it. Data map (2026-10-17): `docs/research/data-map/index.md` and `globals.tsv` name all 4,428 globals that game and library functions touch (3,808 high), with their readers and writers. Scripts for both maps: `tools/research/function_map/README.md`.

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

- Never commit the original's code or data, saves, screenshots or the logo. Downloads use the clean files only (`/Users/max/Projects/cm0102/clean-3968`, made by `tools/clean_files/`); the fan pack in `/Users/max/Downloads/Game` is never a build input.
- No feature may change the game's decisions while the player makes the same clicks, except the team that is picked. A change of memory layout counts like opening a screen: the original's results already depend on where its memory blocks sit (Q152). Classic and widescreen saves differ for this reason, as they always have.
- Every option is a line in `settings.txt`. Feature issues labelled `enhancement` show on the site's "Coming next" list live, so their titles are written for players. Bugs use `bug` and never show.
- Grilling before a new feature, then the spec. One shared brief for parallel agents; Opus for research, review and anything a player sees, Sonnet for routine builds.
- Agents must not run `rm` with a wildcard or a variable path.
- Max approves every publish. He has approved pushes for this work.
