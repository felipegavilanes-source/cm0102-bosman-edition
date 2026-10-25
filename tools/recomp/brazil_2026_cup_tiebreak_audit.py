#!/usr/bin/env python3
"""Read-only audit of the *owned original* CM 01/02 3.9.68 Cup tie breaker.

Usage:
    python3 tools/recomp/brazil_2026_cup_tiebreak_audit.py /path/to/cm0102.exe

Does NOT execute, modify, copy or package the game. Failure means the
2026 Brazilian access-stage override must remain disabled on this binary.
"""
import argparse
from pathlib import Path

from brazil_2026_legacy_audit import PE32


# All addresses are original game virtual addresses (32-bit PE image).
# Reference: objdump -d -M intel on original CM 01/02 3.9.68.
ORIGINAL_TIE_SIGNATURES = (
    (0x572AF3, bytes.fromhex("66 c7 46 0d 04 02"),
     "English semi-final format = 0x0204 at round +0x0d"),
    (0x51B741, bytes.fromhex("66 8b 48 0d"),
     "Cup stage reads that round's tie flags"),
    (0x51B745, bytes.fromhex("66 89 4c 24 6c"),
     "Cup stage carries tie flags to the match setup"),
    (0x51F30F, bytes.fromhex("66 8b 47 38"),
     "Cup tie resolver reads the saved fixture flag word"),
    (0x51F652, bytes.fromhex("81 e2 00 08 00 00"),
     "On level two-leg aggregate, 0x0800 would choose first entrant"),
    (0x51F675, bytes.fromhex("81 e2 00 10 00 00"),
     "On level two-leg aggregate, 0x1000 chooses second entrant"),
    (0x51F680, bytes.fromhex("c6 47 3e 08"),
     "The second entrant receives the qualified marker"),
    (0x51F687, bytes.fromhex("b8 02 00 00 00"),
     "The tie resolver returns second entrant as winner"),
)


def inspect(executable: Path) -> str:
    pe = PE32(executable)
    for address, expected, meaning in ORIGINAL_TIE_SIGNATURES:
        actual = pe.read(address, len(expected))
        if actual != expected:
            raise ValueError(
                f"Unknown game executable at {address:#08x} ({meaning}): "
                f"wanted {expected.hex()}, found {actual.hex()}"
            )
    return "\n".join(
        [
            "Original 3.9.68 two-legged tie resolver: VERIFIED (read-only).",
            "English semi-final flags 0x0204 + Brazilian 0x1000 => 0x1204.",
            "On equal aggregate, the SECOND seeded side advances, before away-goals "
            "and penalty decisions.",
            "Brazilian brackets: 6th/3rd and 5th/4th, so the better-ranked side "
            "is second in both ties.",
            "This validates the binary code path, NOT the playable schedule, "
            "save/reload, or promotion-chain behavior.",
        ]
    )


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("exe", type=Path, help="locally owned CM 01/02 3.9.68 PE32 file")
    args = p.parse_args()
    print(inspect(args.exe))


if __name__ == "__main__":
    main()
