"""Find coincident / coplanar-overlapping triangles inside one pod model (z-fight candidates)."""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import numpy as np, pod_export as PE, racer_assets as R

def tri_info(mm):
    P = np.array(mm.positions)
    out = []
    for a, b, c, mi, fl in mm.tris:
        A, B, C = P[a], P[b], P[c]
        n = np.cross(B - A, C - A); ar = np.linalg.norm(n)
        out.append((A, B, C, n / ar if ar else n, ar / 2, mi, fl))
    return out

def overlaps(mm, tol=0.01):
    T = tri_info(mm)
    res = []
    keys = {}
    for i, t in enumerate(T):
        k = tuple(sorted(tuple(np.round(v, 3)) for v in t[:3]))
        keys.setdefault(k, []).append(i)
    same = [v for v in keys.values() if len(v) > 1]
    # coplanar overlap: centroid of j inside plane of i (dist < tol) and inside tri i
    cop = []
    for i, ti in enumerate(T):
        A, B, C, n, ar, mi, fl = ti
        if ar == 0: continue
        for j in range(i + 1, len(T)):
            tj = T[j]
            cj = (tj[0] + tj[1] + tj[2]) / 3
            if abs(np.dot(cj - A, n)) > tol: continue
            if abs(abs(np.dot(n, tj[3])) - 1) > 1e-3: continue
            # barycentric inside test
            v0, v1, v2 = B - A, C - A, cj - A
            d00, d01, d11, d20, d21 = v0 @ v0, v0 @ v1, v1 @ v1, v2 @ v0, v2 @ v1
            den = d00 * d11 - d01 * d01
            if den == 0: continue
            v = (d11 * d20 - d01 * d21) / den; w = (d00 * d21 - d01 * d20) / den
            if v > 0.01 and w > 0.01 and v + w < 0.99:
                cop.append((i, j, mi, tj[5], round(float(np.dot(n, tj[3])), 3)))
    return same, cop

if __name__ == "__main__":
 for mid in [int(x) for x in sys.argv[1:]]:
    mm = PE.model_mesh(mid)
    same, cop = overlaps(mm)
    print('model', mid, 'tris', len(mm.tris), 'identical-vertex groups', len(same), 'coplanar overlaps', len(cop))
    from collections import Counter
    print('  same by mesh', Counter(tuple(mm.tris[i][3] for i in g) for g in same))
    print('  coplanar by mesh pair/normal-dot', Counter((a[2], a[3], a[4]) for a in cop))
