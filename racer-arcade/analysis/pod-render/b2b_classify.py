"""Classify each back-to-back triangle: 'interior' = its front looks into closed geometry (a ray from its
centroid along its normal first meets a back face of the same model) -> never visible with culling;
'open' = its front looks at open space or a front face -> visible in the arcade."""
import os, sys, json, importlib.util
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import numpy as np, pod_export as PE, racer_assets as R
sys.path.insert(0, HERE)
from b2b_all import pairs

def classify(mm):
    P = np.array(mm.positions); T = np.array([t[:3] for t in mm.tris])
    A, B, C = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    N = np.cross(B - A, C - A); ln = np.linalg.norm(N, axis=1); ln[ln == 0] = 1; N = N / ln[:, None]
    pr, b2b, M = pairs(mm)
    out = {}
    e1, e2 = B - A, C - A
    for t in b2b:
        o = (A[t] + B[t] + C[t]) / 3 + N[t] * 1e-3; d = N[t]
        h = np.cross(d, e2); a = (e1 * h).sum(1); ok = np.abs(a) > 1e-12
        f = np.where(ok, 1 / np.where(ok, a, 1), 0); s = o - A
        u = f * (s * h).sum(1); q = np.cross(s, e1); v = f * (q @ d); tt = f * (e2 * q).sum(1)
        hit = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (tt > 1e-4)
        if not hit.any(): out[t] = 'open'; continue
        tmin = tt[hit].min(); near = np.flatnonzero(hit & (tt < tmin + 1e-3))
        front = (N[near] @ d) < 0  # surface facing the ray origin
        out[t] = 'open' if front.any() else 'interior'
    return out, M

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
                cl, M = classify(mm)
                if cl:
                    c = {'interior': 0, 'open': 0}
                    for v in cl.values(): c[v] += 1
                    rows.append(dict(lod=lod, role=role, model=int(mid), interior=c['interior'], open=c['open'],
                                     interior_tris=sorted(int(k) for k, v in cl.items() if v == 'interior')))
    res[code] = rows
    print(code, ' '.join('L%d/%s/m%d: %d interior, %d open' % (r['lod'], r['role'], r['model'], r['interior'], r['open']) for r in rows))
json.dump(res, open(os.path.join(HERE, 'b2b_classified.json'), 'w'), indent=1)
