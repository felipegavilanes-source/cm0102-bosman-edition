# Review brief, 0.5.0 (2026-10-16)

Two fresh Opus review agents read this brief for each slice. Each gets one angle. Both read the same diff. The lead gives the worktree, the branch and the slice number.

## What to review

- The diff: `git diff main...HEAD` in the worktree the lead names. Leave out `docs/` except where the code names a doc as its evidence.
- Spec: `docs/specs/2026-10-16-features-0.5.0-spec.md`, the slice the lead names. Build brief: `docs/research/build-brief-0.5.0.md`. Terms: `GLOSSARY.md`. Repo rules: `CLAUDE.md`, `docs/HANDOVER.md` ("Rules").
- The research for the slice, as the build brief names it.

## Angles

1. **Spec agent.** Does the slice do what the spec says, with the option name and default the spec gives? Can it change a match result or a game decision while the player makes the same clicks (Q152)? Does the option off give the original's behaviour exactly? Do the tests test the seam the spec names, and would they fail if the feature broke?
2. **Standards agent.** Correctness bugs: memory reads and writes out of range, registers or stack not kept across `rt_callback`, wraps or `INSNS` entries that act on the wrong site or skip the address check, state left set after a screen closes, file writes that can lose data, Windows and Mac differences. Then repo standards: comments in plain STE English, every address with its evidence, no original code or data in git.

## Rules

- Read only. Do not edit files, do not commit, do not build, do not run the game. Other test games run on this machine.
- Check each address you rely on in `/Users/max/Projects/cm0102/clean-3968/cm0102.exe` (memory = file offset + 0x400000 for .text) or with the REA tools (`docs/research/agent-brief.md`).
- Report each finding with file and line, what goes wrong, a concrete case, and a severity: must fix before merge, should fix, or note. Mark an inference as an inference.
- Report only what you can show from the code. Write "unknown" rather than guess.
- At most 300 words in the final reply, findings most severe first.
