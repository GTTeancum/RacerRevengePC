import os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import numpy as np, pod_export as PE
for mid in (416, 3, 74, 1677, 420):
    mm = PE.model_mesh(mid); P = np.array(mm.positions); Nv = np.array(mm.normals)
    agree = tot = 0
    for a, b, c, mi, fl in mm.tris:
        n = np.cross(P[b] - P[a], P[c] - P[a])
        vn = Nv[a] + Nv[b] + Nv[c]
        if np.linalg.norm(n) == 0 or np.linalg.norm(vn) == 0: continue
        tot += 1; agree += np.dot(n, vn) > 0
    print(mid, 'tris', tot, 'cross(B-A,C-A) agrees with vertex normals', agree)
