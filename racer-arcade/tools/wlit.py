# List functions whose code loads a given 16-bit (mov.w) or 32-bit (mov.l) literal value.
# python -I wlit.py IMG HEXVAL [HEXVAL...]
import sys, os, bisect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, decode
import sh4an, numpy as np
im = Image(sys.argv[1]); X = sh4an.load(im)
vals = [int(v, 16) for v in sys.argv[2:]]
w = np.frombuffer(im.data, dtype="<u2")
for lo, hi in sh4an.CODE:
    s = w[(lo - im.base) // 2:(hi - im.base) // 2]
    for i in np.nonzero((s >> 12) == 9)[0].tolist():
        pc = lo + 2 * i; op = int(s[i]); L = pc + 4 + (op & 0xFF) * 2
        if im.ok(L, 2) and im.u16(L) in vals:
            print("%08X mov.w =0x%X  f_%08X" % (pc, im.u16(L), sh4an.func_of(X, pc) or 0))
for v in vals:
    for c in X["lit_refs"].get(v, []): print("%08X mov.l =0x%X  f_%08X" % (c, v, sh4an.func_of(X, c) or 0))
