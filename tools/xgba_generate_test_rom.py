#!/usr/bin/env python3
from pathlib import Path
import struct
import sys

options = sys.argv[1:-1]
if len(sys.argv) < 2 or any(option not in ("--legacy", "--headless") for option in options):
    raise SystemExit("Usage: xgba_generate_test_rom.py [--legacy] [--headless] OUTPUT.gba")

legacy = "--legacy" in options
headless = "--headless" in options

ROM_SIZE = 16 * 1024 * 1024 if legacy else 256 * 1024 * 1024
EXTENDED_TEST_OFFSET = ROM_SIZE - 0x100
TEST_VALUE = 0xC0FFEE42

path = Path(sys.argv[-1])
path.parent.mkdir(parents=True, exist_ok=True)

header = bytearray(0xC0)
struct.pack_into("<I", header, 0x00, 0xEA00002E)
header[0xA0:0xA9] = b"XGBA TEST"
header[0xAC:0xB0] = b"XGBT"
header[0xB0:0xB2] = b"00"
header[0xB2] = 0x96
if not legacy:
    header[0xB5:0xBC] = b"XGBA256"
header[0xBD] = (-sum(header[0xA0:0xBD]) - 0x19) & 0xFF

with path.open("wb") as rom:
    rom.truncate(ROM_SIZE)
    rom.seek(0)
    rom.write(header)
    rom.seek(0xC0)
    for instruction in (0xE59F0014, 0xE5901000, 0xE59F2010, 0xE5821000, 0xEF000003 if headless else 0xEAFFFFFE):
        rom.write(struct.pack("<I", instruction))
    rom.seek(0xDC)
    rom.write(struct.pack("<I", (0x08000000 if legacy else 0x10000000) + EXTENDED_TEST_OFFSET))
    rom.write(struct.pack("<I", 0x02000100))
    rom.seek(EXTENDED_TEST_OFFSET)
    rom.write(struct.pack("<I", TEST_VALUE))
    if not legacy:
        rom.seek(EXTENDED_TEST_OFFSET + 0x10)
        rom.write(struct.pack("<II", 0xE3A0307B, 0xEAFFFFFE))

        rom.seek(0x140)
        probe = (
            0xE59F0028, 0xE3A01014, 0xE3A02000, 0xE4903004,
            0xE5934000, 0xE0822004, 0xE2511001, 0x1AFFFFFA,
            0xE59F500C, 0xE5852000, 0xEAFFFFFE, 0xE1A00000,
            0x08000200, 0x02000104,
        )
        for instruction in probe:
            rom.write(struct.pack("<I", instruction))

        start = 32 * 1024 * 1024 + 0x1000
        step = ((ROM_SIZE - 0x1000 - start) // 19) & ~3
        for i in range(20):
            offset = start + i * step
            value = 0x12340000 + i * 0x10201
            rom.seek(0x200 + 4 * i)
            rom.write(struct.pack("<I", 0x10000000 + offset))
            rom.seek(offset)
            rom.write(struct.pack("<I", value))

print(f"Created {path}: {ROM_SIZE} bytes; test data at 0x{EXTENDED_TEST_OFFSET:08X}")
