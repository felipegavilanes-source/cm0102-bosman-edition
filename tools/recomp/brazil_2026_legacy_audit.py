#!/usr/bin/env python3
"""Read-only PE32 audit of the original CM 01/02 3.9.68 league builders.

Usage: python3 tools/recomp/brazil_2026_legacy_audit.py path/to/cm0102.exe
No executable is run; this parser only reads PE sections and opcode signatures.
"""
import argparse
import struct
from pathlib import Path


class PE32:
    def __init__(self, filename):
        self.data = Path(filename).read_bytes()
        b = self.data
        if b[:2] != b'MZ':
            raise ValueError('not a DOS/PE executable')
        nt = struct.unpack_from('<I', b, 0x3c)[0]
        if b[nt:nt + 4] != b'PE\0\0':
            raise ValueError('not a PE executable')
        n = struct.unpack_from('<H', b, nt + 6)[0]
        optional_size = struct.unpack_from('<H', b, nt + 20)[0]
        if struct.unpack_from('<H', b, nt + 24)[0] != 0x10b:
            raise ValueError('only PE32 is supported')
        self.image_base = struct.unpack_from('<I', b, nt + 24 + 28)[0]
        self.sections = []
        for i in range(n):
            off = nt + 24 + optional_size + 40 * i
            name = b[off:off + 8].strip(b'\0').decode('ascii', errors='replace')
            va, raw_size, raw_off = (struct.unpack_from('<I', b, off + delta)[0]
                                     for delta in (12, 16, 20))
            self.sections.append((name, self.image_base + va, raw_size, raw_off))

    def read(self, virtual_address, length):
        for name, base, size, file_offset in self.sections:
            if base <= virtual_address and virtual_address + length <= base + size:
                offset = file_offset + virtual_address - base
                return self.data[offset:offset + length]
        raise ValueError(f'virtual address 0x{virtual_address:08x} is out of file')

    def u32(self, addr):
        return struct.unpack('<I', self.read(addr, 4))[0]


def inspect(executable):
    pe = PE32(executable)
    # Vtable +0x8c: set_up_rules; +0x3c: schedule;
    # +0x28: advance_stage; +0x44: stage result.
    classes = [
        ('Brazilian Serie A', 0x967d98, 0x42a5a0, 0x429b50, 0x42a950, 0x42abc0),
        ('Brazilian Serie B', 0x967e4c, 0x42cb70, 0x42c140, 0x42cdd0, 0x42d040),
        ('English Premier Division', 0x969d1c, 0x574b70, 0x573660, 0x5a8f60, 0x686940),
        ('English First Division', 0x969c68, 0x572d90, 0x571980, 0x572ff0, 0x576c50),
    ]
    report = []
    for name, table, rules, dates, stage, result in classes:
        got = tuple(pe.u32(table + slot) for slot in (0x8c, 0x3c, 0x28, 0x44))
        if got != (rules, dates, stage, result):
            raise ValueError(f'vtable mismatch in {name}: got {[hex(v) for v in got]}')
        report.append(f'{name}: schedule=0x{dates:x}, stage=0x{stage:x}, result=0x{result:x}')

    opcode_checks = [
        ('Brazil Serie A: one meeting per pairing', 0x42a5af, b'\x66\x89\x5e\x3c'),
        ('Brazil Serie B: one meeting per pairing', 0x42cb83, b'\x66\x89\x5e\x3c'),
        ('Brazil Serie A: 27 calendar slots', 0x429b8d, b'\x66\xc7\x00\x1b\x00'),
        ('Brazil Serie B: 27 calendar slots', 0x42c17d, b'\x66\xc7\x00\x1b\x00'),
        ('English Premier: 38 calendar slots', 0x57369a, b'\x66\xc7\x00\x26\x00'),
    ]
    for label, addr, expected in opcode_checks:
        if pe.read(addr, len(expected)) != expected:
            raise ValueError(f'opcode mismatch in {label}: original game build may differ')
        report.append(f'{label}: confirmed')
    report.append('AUDIT ONLY: this does not patch the game or establish live gameplay compatibility.')
    return '\n'.join(report)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path, help='locally owned CM 01/02 3.9.68 executable')
    args = parser.parse_args()
    print(inspect(args.exe))


if __name__ == '__main__':
    main()
