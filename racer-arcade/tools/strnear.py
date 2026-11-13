# Print strings referenced from code in an address range: python -I strnear.py IMG LO HI
import sys, os, pickle, bisect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image
import sh4an
im = Image(sys.argv[1]); X = sh4an.load(im)
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
S = X["strings"]; out = []
for v, refs in X["lit_refs"].items():
    if v in S:
        for r in refs:
            if lo <= r < hi: out.append((r, sh4an.func_of(X, r), S[v]))
for r, f, s in sorted(out): print("%08X f_%08X %r" % (r, f or 0, s[:80]))
