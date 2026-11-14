"""All 18 kinds: back-to-back (coplanar, overlapping, opposite-facing) triangle pairs and exact
duplicates per pod part model, LOD 0..3. Vectorised version of overlap.py."""
import os, sys, json
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import numpy as np, pod_export as PE, racer_assets as R

def pairs(mm, tol=0.01):
    P = np.array(mm.positions); T = np.array([t[:3] for t in mm.tris]); M = np.array([t[3] for t in mm.tris])
    A, B, C = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    N = np.cross(B - A, C - A); ar = np.linalg.norm(N, axis=1); ok = ar > 0
    N[ok] /= ar[ok, None]; cen = (A + B + C) / 3
    out = []
    for i in np.flatnonzero(ok):
        d = (cen - A[i]) @ N[i]
        nd = N @ N[i]
        cand = np.flatnonzero((np.abs(d) < tol) & (nd < -0.999) & ok)
        cand = cand[cand != i]
        if not len(cand): continue
        v0, v1 = B[i] - A[i], C[i] - A[i]; v2 = cen[cand] - A[i]
        d00, d01, d11 = v0 @ v0, v0 @ v1, v1 @ v1
        d20, d21 = v2 @ v0, v2 @ v1; den = d00 * d11 - d01 * d01
        v = (d11 * d20 - d01 * d21) / den; w = (d00 * d21 - d01 * d20) / den
        hit = cand[(v > 0.01) & (w > 0.01) & (v + w < 0.99)]
        out += [(int(i), int(j)) for j in hit]
    tris = {k for p in out for k in p}
    return out, tris, M

if __name__ == '__main__':
    res = {}
    for code in R.CODES:
        kind = R.CODES.index(code); pd = R.pod(kind); seen = set(); rows = []
        for lod in range(4):
            part = pd['parts'][lod]
            for role, oid in [('cockpit', part['objA']), ('binder', part['objB'])] + [('engine%d' % i, part['engines'][i]) for i in range(pd['engines'])]:
                if oid == PE.NONE: continue
                for n in R.object_nodes(oid)['nodes']:
                    mid = n['model']
                    if mid == PE.NONE or mid in seen: continue
                    seen.add(mid); mm = PE.model_mesh(mid)
                    if mm is None or not mm.tris: continue
                    pr, tris, M = pairs(mm)
                    if tris:
                        rows.append(dict(lod=lod, role=role, model=R.idname(mid), tris=len(mm.tris), b2b_tris=len(tris),
                                         meshes=sorted({int(M[t]) for t in tris}), nocull_in_b2b=sum(mm.tris[t][4] for t in tris)))
        res[code] = rows
        print(code, [(r['lod'], r['role'], r['model'], r['b2b_tris'], r['meshes'], r['nocull_in_b2b']) for r in rows])
    json.dump(res, open(os.path.join(HERE, 'b2b_all.json'), 'w'), indent=1)
