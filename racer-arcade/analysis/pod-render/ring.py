import os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools')); sys.path.insert(0, HERE)
import numpy as np, pod_export as PE, overlap as OV
from collections import Counter
mm = PE.model_mesh(416); P = np.array(mm.positions); UV = np.array(mm.uvs)
T = [t for t in mm.tris if t[3] == 4]
idx = sorted({i for t in T for i in t[:3]})
print('mesh4 bbox', P[idx].min(0), P[idx].max(0))
# ring axis: centre of the big ring (x,y) from vertices with |x|<1.5
ringv = [i for i in idx if abs(P[i, 0]) < 2 and P[i, 1] > -1.5]
c = P[ringv].mean(0); print('ring centre guess', c)
same, cop = OV.overlaps(mm)
pairs = [(i, j) for i, j, *_ in cop]
rows = []
for i, t in enumerate(mm.tris):
    if t[3] != 4: continue
    A, B, C = P[list(t[:3])]
    cen = (A + B + C) / 3; n = np.cross(B - A, C - A); n /= np.linalg.norm(n)
    rad = cen[:2] - c[:2]; r = np.linalg.norm(rad)
    rows.append((i, round(r, 3), round(float(np.dot(n[:2], rad / r)), 2), round(float(n[2]), 2), round(cen[2], 2)))
inpair = {k for p in pairs for k in p}
print('pairs', pairs)
for r in rows:
    if r[0] in inpair: print('pair tri', r, 'uv', np.round(UV[list(mm.tris[r[0]][:3])], 2).tolist())
print('radius histogram (r, radial-normal sign):', Counter((round(r[1], 1), 'out' if r[2] > 0.5 else 'in' if r[2] < -0.5 else 'axial') for r in rows if r[1] < 3))
