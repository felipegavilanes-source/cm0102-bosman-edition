### Slice 24, UEFA 2021/22 to 2023/24 (Opus), after slice 23
Waits for: slice 23 on main (spec "Blocked by: 23").

Research:
- `uefa.tsv` rows of 2021/22 to 2023/24, the same columns as slice 23, the Conference League rows (`comp_file` `new`, `UEFA Europa Conference League` to 2023/24, `UEFA Conference League` from 2024/25). `uefa.md` change points U8 and U9, "Conflicts for Max".
- `2026-10-16-sweep-europe.md` 2.3 (points 3 and 4: what a Conference League object needs), 1.2 (the Europe array: count 4 at 0x8313bd, 0x10 bytes at 0x8313bb), 3.2. `2026-10-16-sweep-international.md` 7 (the round rule flags at round template +0xd: 0x1 penalties, 0x2 extra time).

Steps:
1. **Conference League (U8):** a new competition through the shared module. It needs a key id, a Cup-based object with the `Uefa_Cup` slots (0, 1, 2, 8, 9, 15, 17, 22, 24, 35), a place function in the seeding and entry slots for clubs that drop from the other two cups (sweep 2.3 point 4). The Europe array holds 4 objects: the Intertoto slot (index 2) is free from 2009/10, or the array grows. The Intertoto class itself cannot be reused (sweep 2.3 point 3, inference there).
2. **Europa League cut to 32 groups (U8)** and the knockout play-off round, in the slice 23 table.
3. **No away goals (U9):** from 2021/22 in all three competitions. Where the match code applies away goals is unknown: the fixture screen shows the away goals text when a digit test of the competition id passes (0x45e0b1) and bits 0x4 and 0x4000 are clear (0x45e0b9). Find the rule in the match code and cite it before you change it.
4. **Russia (Q293, Q297 to Q327):** no UEFA places while suspended, from 2022/23. A place cut during 2021/22 keeps the start value (Q278).
5. Names without sponsor names (Q360) from slice 15's table.
6. **Stop and report to Max:** the record of the Conference League. The databases have no such record, and the Cup Winners' Cup record (0x9cf6ec) is matched but has no object (sweep 1.4). Recommended answer: a new record in memory, so the old Cup Winners' Cup history keeps its name, not a rename of that record.

Test seam: rules report `--year 2021 --database "<October 2021>" --last 2022` and `--year 2022` (October 2022, Russia), season change runs across 2020/21 to 2021/22 with changing rules on, a check that the Conference League has its clubs in each round and ends with a winner, and that no tie in 2021/22 is decided on away goals (match log). Same-save checks (Q342) and repeatability.

Shares: the Europe array and `uefa_seeding` with slices 22, 23 and 25, the new competitions module. Merge order: after 23, before 25.
