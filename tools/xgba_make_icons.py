import struct
import sys
from pathlib import Path


def main():
    icon = Path(sys.argv[1]).read_bytes()
    if icon[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("The XGBA icon must be a PNG")

    width, height = struct.unpack(">II", icon[16:24])
    if width != 128 or height != 128:
        raise ValueError("Expected a 128x128 XGBA master icon")

    ico_header = struct.pack("<HHH", 0, 1, 1)
    ico_entry = struct.pack("<BBBBHHII", 128, 128, 0, 0, 1, 32, len(icon), 22)
    Path(sys.argv[2]).write_bytes(ico_header + ico_entry + icon)

    icns_part = b"ic07" + struct.pack(">I", len(icon) + 8) + icon
    icns = b"icns" + struct.pack(">I", len(icns_part) + 8) + icns_part
    Path(sys.argv[3]).write_bytes(icns)


if __name__ == "__main__":
    main()
