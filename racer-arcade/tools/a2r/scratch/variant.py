import sys, os, glob, collections, struct
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
from geo_res import read_res
G = r'C:\Games\Star Wars Racer Revenge'
files = sorted(glob.glob(G + r'\TRACKS\*.RES')) + sorted(glob.glob(G + r'\PODS\*\*.RES'))
comp = collections.Counter(); lods = collections.Counter(); ex = {}
for f in files:
    m, order, info = read_res(f)
    for name, b in m.items():
        if not name.endswith('.psg'): continue
        n, = struct.unpack_from('<I', b, 8); at = 12 + 224*n
        nm, = struct.unpack_from('<I', b, at); at += 4 + 64*nm + 32
        nl, = struct.unpack_from('<I', b, at); at += 4; lods[nl] += 1
        for li in range(nl):
            cut, ns = struct.unpack_from('<fI', b, at); at += 8
            for si in range(ns):
                mat, cw, uvs, nv, nst, size = struct.unpack_from('<IIfHHI', b, at); at += 20
                comp['%08x' % cw] += 1; ex.setdefault('%08x' % cw, (os.path.basename(f), name, at))
                at += size
print(comp, lods); print(ex)
