import sys, collections
import numpy as np
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import geo_course as GC
N = int(sys.argv[1]) if len(sys.argv) > 1 else 1
tr, _ = GC.arcade_triangles(N)
X = GC.transform_fn(tr['land'])
stat = collections.defaultdict(list)
for t in tr['land']:
    P, Nn, UV, A = GC.apply(t, X)
    e = np.cross(P[1] - P[0], P[2] - P[0])
    e /= np.linalg.norm(e)
    key = (GC.EC.idname(t['tid']) if t['tid'] is not None else None, t['mat'].get('shading'), t['back'])
    nl = np.linalg.norm(Nn, axis=1)
    stat[key].append((float(np.mean([(n / l) @ e if l > 0 else 9 for n, l in zip(Nn, nl)])), float(e[1]), float(nl.mean())))
for k, v in sorted(stat.items(), key=lambda kv: -len(kv[1]))[:14]:
    a = np.array(v)
    print(k, len(v), 'N.face mean %.2f share<0 %.2f | face_up mean %.2f share<0 %.2f | |N| %.2f' % (
        a[:, 0].mean(), (a[:, 0] < 0).mean(), a[:, 1].mean(), (a[:, 1] < 0).mean(), a[:, 2].mean()))
