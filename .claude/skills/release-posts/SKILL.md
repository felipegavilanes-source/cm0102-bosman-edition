---
name: release-posts
description: Writes the two posts for each Bosman Edition release on the champman0102 forum, the thread post (a reply with two or three pictures of what is new) and the updated original post (the first post of the thread), plus an optional Reddit update, and makes the pictures. Use for "update post", "release post", "forum post", "thread post", "Reddit post", "original post", "announce the release", or at the end of a release slice. Max posts them himself; this skill only drafts.
---

# Release posts

Each release gets two posts in the Bosman Edition thread on the champman0102 forum (champman0102.co.uk). Max posts both himself, after the release is published. Never post, and never open a site to post.

1. The thread post is a reply in the thread. It has two or three pictures of the biggest new features, each with a short paragraph under it. Mark each place as `[picture n: file]`; Max attaches the files.
2. The updated original post is the new text of the first post of the thread. It explains the work (the recompile, the official 2008 files, the same-game rule and how it is tested), puts the current release first, lists what the port adds, lists every release with its date and a one-line summary, and ends with what comes next, the credits and the bug report line. Start from the original post in the previous `release-x.y.z-posts.md` and bring every part up to date.
3. Optional: a short "Update: x.y.z is out" list for the end of the Reddit post (https://www.reddit.com/r/footballmanagergames/comments/1x6ih6l/ive_rebuilt_cm_0102_to_run_natively_on_mac/), where Max posts as u/IndividualAd6919.

## Inputs

- The release section of `CHANGELOG.md` (the facts).
- The spec of the release in `docs/specs/` (what each feature does, and its option name and default).
- `docs/requests.md`: credit each player whose request ships, by name and where they asked (for example "gundenaf on Reddit").
- `gh release list`: the dates of earlier releases for the list in the original post.
- The previous `/Users/max/Projects/cm0102/release-*-posts.md` (the voice and the layout to follow).

## Output

- `/Users/max/Projects/cm0102/release-x.y.z-posts.md`, outside the repo. It starts with one line that says Max posts these himself.
- Pictures in `/Users/max/Projects/cm0102/release-x.y.z-images/`, outside the repo. Name them `1-<feature>.png`, `2-<feature>.png` and so on, in the order of the post. Never commit them: they show the original game's screens.

## Pictures

1. Make them from the release build, after the version bump: some screens show the version (the options box does).
2. Use widescreen (1280 x 800) at full size. Do not scale down.
3. Load `start.sav` (Arsenal, 14.7.01) or `prematch.sav` from `/Users/max/Projects/cm0102/test-fixtures`. Use a temporary `RECOMP_USER_DIR`. Never use Max's own saves.
4. Reuse the dumps of the feature's own smoke test where one exists (for example `tests/smoke/foreign_run.py` and `tests/smoke/options_box_run.py` write each screen to their out folder). Otherwise write a `tests/fixtures/fixtures.py play` script with `dump` steps.
5. Look at each picture before you use it. It must show the feature at once, with no debug text and no cut labels. The manager name in test saves is "Test Smoke", which is fine.

## Rules for the text

- Say what each feature does, in plain words, for a player. Name the screen and the menu item where it opens.
- Put the biggest player-facing features first, with the pictures. Fixes and small changes go in an "Also new" list.
- Claim only what the tests show. Do not write "no feature changes the game's decisions" unless the release's same-save checks show it for every option. Say which options take effect only at the next start.
- No public text names patch authors, even when a feature follows their patch.
- End the thread post with what comes next (from the roadmap in `docs/HANDOVER.md`) and the line asking for bug reports with the computer and the clicks.
- British English. No exclamation marks. Apply the humanizer skill to both posts before you show them to Max: fix every §1 to §5 tell, and remove dashes and bold labels on list items. The bold "Update: x.y.z is out" line of the Reddit update is the one bold line.
- Put `[date]` where the release date goes if the release is not published yet.
- After Max posts, add the shipped version to the rows of `docs/requests.md`.
