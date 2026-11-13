#!/usr/bin/env python3
r"""podre_npcpods: decode the NPC per-kind part table (ptr array 0C2F904C, 18 x 168-byte records).
    python -I podre_npcpods.py ..\img\eprom.bin
Record: +0 int part count; +4 body part def (32 B); +36 + i*32 part def i (4 slots); +164 int flags.
Part def: +0 x,y,z local offset; +12 f (x 0C13A830 const); +16,+20,+24,+28 floats.
"""
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, BASE_DEFAULT
CODES = "ALD ANA BEN EBE GAS MAR MAW RAT SEB TEE ODY ARK BOL CLE DUD ELA NEV WAN".split()
im = Image(sys.argv[1], BASE_DEFAULT)
f = lambda a: struct.unpack("<f", struct.pack("<I", im.u32(a)))[0]
def part(a): return "(%g, %g, %g) k=%g a=%g b=%g c=%g d=%g" % tuple(f(a + 4 * i) for i in range(8))
for k in range(18):
    r = im.u32(0x0C2F904C + 4 * k); n = im.u32(r)
    print("%2d %s rec %08X count %d flags %08X" % (k, CODES[k], r, n, im.u32(r + 164)))
    print("   body  " + part(r + 4))
    for i in range(4): print("   eng%d  " % i + part(r + 36 + 32 * i) + ("" if i < n else "  (unused)"))
