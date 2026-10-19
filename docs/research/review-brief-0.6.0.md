# Review brief, 0.6.0 (2026-10-17)

Two fresh Opus review agents read this brief for each slice. Each gets one angle. Both read the same diff. The lead gives the worktree, the branch, the slice number and the risk the lead sees.

## What to review

- The diff: `git diff main...HEAD` in the worktree the lead names. Leave out `docs/` except where the code names a doc as its evidence, and except the research docs the slice writes (slice 11: the first-season branch table and the save part proof).
- Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md`, the slice the lead names. Build brief: `docs/research/build-brief-0.6.0/shared.md` plus the slice file. Terms: `GLOSSARY.md`. Repo rules: `CLAUDE.md`, `docs/HANDOVER.md` ("Rules").

## Angles

1. **Spec agent.** Does the slice do what the spec and the build brief say, with the option name and default the spec gives? Does the base game (2001/02, 3.9.68, `changing_rules` off) and every old save stay byte for byte the same as 0.5.0 (Q152, Q241, Q246)? Can the change alter a game decision while the player makes the same clicks? Do the tests test the seam the spec names, would they fail if the feature broke, and were they run against the 0.5.0 build?
2. **Standards agent.** Correctness bugs: memory reads and writes out of range, registers or stack not kept across `rt_callback`, wraps or `INSNS` entries that act on the wrong site or skip the address check, state left set after a game ends or a screen closes, file writes and unpacks that can lose data or leave a half folder, Windows and Mac path differences. Then repo standards: comments in plain STE English, every address with its evidence, no original code or data in git, no name of another modder, their patch or tool anywhere in the diff.

## Extra checks for slice 11 (one-way door: the save format)

- The season part is absent from every base game save and every re-save of an old save. Show the code path that decides it.
- The proof that the original's loader skips an unknown part: check the cited addresses yourself. If the proof is an inference, say so: it is a must-fix.
- The part has a format version and a length, and a reader that meets a newer version or a short part fails safe (it refuses the load with a message, or reads only the fields it knows). No crash, no wrong season.
- The start year and `changing_rules` are set before any game code reads them at a load, and reset at the end of a game and at a new game. Check the order against the load and new game code.
- Each moved first-season branch gives today's result when the start year is 2001. Check at least ten sites against the exe, the structure branches among them.

## Rules

- Read only. Do not edit files, do not commit, do not build, do not run the game. Other test games run on this machine.
- Check each address you rely on in `/Users/max/Projects/cm0102/clean-3968/cm0102.exe` (memory = file offset + 0x400000 for .text) or with the REA tools (`docs/research/agent-brief.md`).
- Report each finding with file and line, what goes wrong, a concrete case, and a severity: must fix before merge, should fix, or note. Mark an inference as an inference.
- Report only what you can show from the code. Write "unknown" rather than guess.
- At most 300 words in the final reply, findings most severe first.
