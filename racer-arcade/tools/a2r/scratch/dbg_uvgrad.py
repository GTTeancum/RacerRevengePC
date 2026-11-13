import sys, os
import numpy as np
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
import geo_course as GC
from decode_static48b import decode_psg3
mat = sys.argv[1] if len(sys.argv) > 1 else 'E154'


def dens(P, UV, T):
    out = []
    for t in T:
        p, u = P[t], UV[t]
        a3 = np.linalg.norm(np.cross(p[1] - p[0], p[2] - p[0])) / 2
        a2 = abs((u[1, 0] - u[0, 0]) * (u[2, 1] - u[0, 1]) - (u[2, 0] - u[0, 0]) * (u[1, 1] - u[0, 1])) / 2
        if a3 > 1:
            out.append(np.sqrt(a2 / a3))
    return np.array(out)


tr, _ = GC.arcade_triangles(1)
X = GC.transform_fn(tr['land'])
ds = []
for t in tr['land']:
    if t['tid'] is not None and GC.EC.idname(t['tid']) == mat:
        P, N, UV, A = GC.apply(t, X)
        ds.append(dens(P, UV, [np.array([0, 1, 2])]))
ds = np.concatenate(ds)
print('arcade (scaled) uv per ft: median %.4f p10 %.4f p90 %.4f n %d' % (np.median(ds), np.percentile(ds, 10), np.percentile(ds, 90), len(ds)))
d = r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\course1\geometry'
ds = []
for f in os.listdir(d):
    if f.endswith('_t%s.psg' % mat.lower()):
        v, ix, rep = decode_psg3(open(os.path.join(d, f), 'rb').read())
        P = np.array([x[:3] for x in v]); UV = np.array([x[3:5] for x in v])
        ds.append(dens(P, UV, np.array(ix).reshape(-1, 3)))
ds = np.concatenate(ds)
print('decoded output uv per ft:  median %.4f p10 %.4f p90 %.4f n %d' % (np.median(ds), np.percentile(ds, 10), np.percentile(ds, 90), len(ds)))
