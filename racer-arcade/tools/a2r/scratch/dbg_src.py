import sys, numpy as np
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import geo_course as GC
tr, _ = GC.arcade_triangles(1)
X = GC.transform_fn(tr['land'])
print(X)
for t in tr['land']:
    if t['tid'] is None or GC.EC.idname(t['tid']) != 'E154': continue
    P, N, UV, A = GC.apply(t, X)
    c = np.array([12990.0, 11100.0])
    if P[:, 0].min() <= c[0] <= P[:, 0].max() and P[:, 2].min() <= c[1] <= P[:, 2].max():
        print('back' if t['back'] else 'front', 'bias', t['bias'], t['polytype'], t['mat'].get('textured'), P.round(1).tolist(), UV.round(3).tolist())
