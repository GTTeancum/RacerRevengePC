#!/usr/bin/env python3
r"""podre_tab: dump a RAM-image table as u32 / float words.
    python -I podre_tab.py ..\img\eprom.bin ADDR RECSIZE COUNT [f|x]
"""
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, BASE_DEFAULT
im = Image(sys.argv[1], BASE_DEFAULT)
a = int(sys.argv[2], 16); rs = int(sys.argv[3], 0); n = int(sys.argv[4], 0)
mode = sys.argv[5] if len(sys.argv) > 5 else "x"
for i in range(n):
    r = a + i * rs; out = []
    for j in range(0, rs, 4):
        v = im.u32(r + j)
        if mode == "f":
            f = struct.unpack("<f", struct.pack("<I", v))[0]
            out.append(("%g" % f) if (v == 0 or 1e-6 < abs(f) < 1e7) else "%08X" % v)
        else: out.append("%08X" % v)
    print("%2d %08X: %s" % (i, r, " ".join(out)))
