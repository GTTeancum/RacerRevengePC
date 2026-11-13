# List aligned words in the program image (RAM-loaded part) that look like ROM-board bus addresses:
#   0xA0000000-0xAFFFFFFF -> mask ROM offset (v & 0x0FFFFFFF)
#   0x90000000-0x91FFFFFF -> EPROM bus 0x10/0x11 bank (file = bank-0x10)*8MB + (v & 0xFFFFFF))
# Groups consecutive hits into runs (gap <= 32 bytes) and prints the runs with code xrefs to run start.
import sys, os, struct, pickle, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
B = 0x0C00FC00
d = open(sys.argv[1], "rb").read()
end = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x400000   # file offset limit
w = np.frombuffer(d[:end], dtype="<u4")
m = ((w >> 28) == 0xA) | ((w >> 25) == (0x90 >> 1))
idx = np.nonzero(m)[0]
X = pickle.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "analysis", "xref.pkl"), "rb"))
runs = []; cur = []
for i in idx.tolist():
    if 0x400 // 4 <= i < 0x7124 // 4 or 0xF0400 // 4 <= i < 0x150400 // 4: continue   # skip code
    if cur and i - cur[-1] > 8: runs.append(cur); cur = []
    cur.append(i)
if cur: runs.append(cur)
for r in runs:
    a0 = B + r[0] * 4; a1 = B + r[-1] * 4
    refs = []
    for k in range(r[0] - 4, r[0] + 1):
        for c in X["lit_refs"].get(B + k * 4, []): refs.append("%08X(+%d)" % (c, (r[0] - k) * 4))
        for c in X["data_refs"].get(B + k * 4, []): pass
    print("%08X-%08X n=%3d first=%08X last=%08X refs:%s" % (a0, a1 + 4, len(r), w[r[0]], w[r[-1]], " ".join(refs[:4])))
