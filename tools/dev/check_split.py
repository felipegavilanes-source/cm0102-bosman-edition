#!/usr/bin/env python3
"""Prove the split of the build brief and the handover lost no line (token-saving spec, Q7 and Q8).

Every non-empty line of the old brief and the old handover (commit 9c5036b) must appear
in the new files, as many times as before and in one file only. Added pointer and index
lines are listed as allowed. Run from the repo root: python3 tools/dev/check_split.py
"""
import collections, pathlib, subprocess, sys

OLD_COMMIT = '9c5036b'
BRIEF_OLD = 'docs/research/build-brief-0.6.0.md'
BRIEF_DIR = pathlib.Path('docs/research/brief-0.6.0')
HAND_OLD = 'docs/HANDOVER.md'
HAND_NEW = [pathlib.Path('docs/HANDOVER.md'), pathlib.Path('docs/history/handover-2026-10-17.md')]

# Lines whose words changed only to point at the new files: new line -> old line.
EDITED = {
    "- Not started: 15b (missing top and second tier clubs; MLS stays at 12), 20 (new divisions), 25 (league phase), 29 (48-team World Cup). Briefs are in `docs/research/brief-0.6.0/` (shared.md plus one file per slice).":
    "- Not started: 15b (missing top and second tier clubs; MLS stays at 12), 20 (new divisions), 25 (league phase), 29 (48-team World Cup). Briefs are in `docs/research/build-brief-0.6.0.md`.",
    'Read this section first; the section of 2026-10-17 is older and is in `docs/history/handover-2026-10-17.md`.':
    'Read this section first; the section of 2026-10-17 below is older.',
}
# Added lines of the handover (headings and pointers).
ADDED_HAND = {
    '## History',
    'Older build state (2026-10-17), facts from 0.4.1, 0.4.0 and 0.3.0, and the function map note: `docs/history/handover-2026-10-17.md`. That file also holds the release notes items for slice 30, the known issues for later slices, the worktree clean-up list, the pointer to `/Users/max/Projects/cm0102/HANDOVER-PRIVATE.md`, the pointer to `docs/requests.md` and the Colima start command. Read it before slice 30 or a clean-up.',
    '## Open items',
    '# Handover history, up to 2026-10-17',
    'Moved from `docs/HANDOVER.md` on 2026-10-19. Open work with a plain place in the list is in "Open items" of `docs/HANDOVER.md`. The rest stays here.',
}


def old_lines(path):
    text = subprocess.check_output(['git', 'show', f'{OLD_COMMIT}:{path}'], text=True)
    return [l for l in text.split('\n') if l.strip()]


def new_lines(paths):
    return [(p, l) for p in paths for l in p.read_text().split('\n') if l.strip()]


def compare(name, old, new, added_ok, in_index=None):
    old_c = collections.Counter(old)
    new_c = collections.Counter()
    where = collections.defaultdict(set)
    for p, l in new:
        l = EDITED.get(l, l)
        if in_index and in_index(p, l):
            continue
        if l in added_ok and l not in old_c:
            continue
        new_c[l] += 1
        where[l].add(str(p))
    bad = 0
    for l in old_c:
        if new_c[l] != old_c[l]:
            print(f'{name}: line count {old_c[l]} -> {new_c[l]}: {l[:90]}')
            bad += 1
    for l in new_c:
        if l not in old_c:
            print(f'{name}: new line not allowed: {l[:90]}')
            bad += 1
    # A line that was once in the old file must sit in one new file. Repeated lines may sit in several.
    for l, ps in where.items():
        if old_c[l] == 1 and len(ps) != 1:
            print(f'{name}: line in several files: {l[:90]}')
            bad += 1
    print(f'{name}: {sum(old_c.values())} old lines, {sum(new_c.values())} new lines, {bad} problems')
    return bad


def main():
    brief_files = sorted(BRIEF_DIR.glob('*.md'))
    shared = BRIEF_DIR / 'shared.md'
    index_start = 'Read this file, then only the file of your slice.'

    def in_index(p, l):
        # the added index block of shared.md: its heading, intro line and one line per slice file
        if p != shared:
            return False
        return l == '## Slice files' or l.startswith(index_start) or (
            l.startswith('- `') and any(l.startswith(f'- `{f.name}`: ') for f in brief_files))

    bad = compare('brief', old_lines(BRIEF_OLD), new_lines(brief_files), set(), in_index)
    text = pathlib.Path(BRIEF_OLD).read_text()
    for f in brief_files:
        if f'`{f.name}`' not in text:
            print(f'pointer file {BRIEF_OLD} does not list {f.name}')
            bad += 1
    bad += compare('handover', old_lines(HAND_OLD), new_lines(HAND_NEW), ADDED_HAND)
    print('OK' if not bad else 'FAILED')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
