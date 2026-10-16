# Review brief, 0.5.1 (2026-10-16)

Two fresh Opus review agents read this brief for slice 1 (HD text Large). Each gets one angle. Both read the same diff. The lead gives the worktree and the branch.

## What to review

- The diff: `git diff main...HEAD` in the worktree the lead names. Leave out `docs/` except where the code names a doc as its evidence.
- Spec: `docs/specs/2026-10-16-hd-text-0.5.1-spec.md`, slice 1. Research: `docs/research/2026-10-16-hd-text.md` and the 59 constants in `docs/research/2026-10-16-hd-text-patches.txt`. Terms: `GLOSSARY.md`. Repo rules: `CLAUDE.md`, `docs/HANDOVER.md` ("Rules").

## Angles

1. **Spec agent.** Does the slice do what the spec says: factor 1.35 on the 59 constants, the option `hd_text = large | small` (default large, read at start, HD only) with its row on the Screen tab next to Display mode, the Traditional fonts scaled in HD Large, the linear filter when the HD frame is shown below 1x? Can it change a game decision in widescreen, classic or HD Small (Q152): must be equal to 0.5.0. Do the tests test the seams the spec names (same-save, screens, options), and would they fail if the feature broke?
2. **Standards agent.** Correctness bugs: memory reads and writes out of range (the bitmap font table at 0xae1dac, glyph sizes, calloc and free through `rt_callback`), registers or stack not kept across `rt_callback`, a wrong constant or address in `tools/recomp/hd_text.py`, the texture filter logic in `user32.c` (sharp_picture on and off, full screen, resize), Windows and Mac differences. Then repo standards: comments in plain STE English, every address with its evidence, no original code or data in git.

## Rules

- Read only. Do not edit files, do not commit, do not build, do not run the game. Other test games run on this machine.
- Check each address you rely on in `/Users/max/Projects/cm0102/clean-3968/cm0102.exe` (memory = file offset + 0x400000 for .text) or with the REA tools (`docs/research/agent-brief.md`).
- Report each finding with file and line, what goes wrong, a concrete case, and a severity: must fix before merge, should fix, or note. Mark an inference as an inference.
- Report only what you can show from the code. Write "unknown" rather than guess.
- At most 300 words in the final reply, findings most severe first.
