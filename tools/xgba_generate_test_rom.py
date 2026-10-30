#!/usr/bin/env python3
from pathlib import Path
import struct
import sys

ROM_SIZE = 256 * 1024 * 1024
EXTENDED_TEST_OFFSET = ROM_SIZE - 0x100
TEST_VALUE = 0xC0FFEE42

if len(sys.argv) != 2:
    raise SystemExit("Usage: xgba_generate_test_rom.py OUTPUT.gba")

path = Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)

header = bytearray(0xC0)
struct.pack_into("<I", header, 0x00, 0xEA00002E)
header[0xA0:0xA9] = b"XGBA TEST"
header[0xAC:0xB0] = b"XGBT"
header[0xB0:0xB2] = b"00"
header[0xB2] = 0x96
header[0xB5:0xBC] = b"XGBA256"
header[0xBD] = (-sum(header[0xA0:0xBD]) - 0x19) & 0xFF

with path.open("wb") as rom:
    rom.truncate(ROM_SIZE)
    rom.seek(0)
    rom.write(header)
    rom.seek(0xC0)
    for instruction in (0xE59F0014, 0xE5901000, 0xE59F2010, 0xE5821000, 0xEAFFFFFE):
        rom.write(struct.pack("<I", instruction))
    rom.seek(0xDC)
    rom.write(struct.pack("<I", 0x1FFFFF00))
    rom.write(struct.pack("<I", 0x02000100))
    rom.seek(EXTENDED_TEST_OFFSET)
    rom.write(struct.pack("<I", TEST_VALUE))

print(f"Created {path}: {ROM_SIZE} bytes; marker at 0x{EXTENDED_TEST_OFFSET:08X}")
