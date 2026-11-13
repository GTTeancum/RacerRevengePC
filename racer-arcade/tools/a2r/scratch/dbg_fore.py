import sys, json, os
import numpy as np
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools')
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
import softrender as SR
from decode_static48b import decode_psg3
d = r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\course1'
V = json.load(open(os.path.join(d, 'verify.json')))['previews'][int(sys.argv[1]) if len(sys.argv) > 1 else 0]
cam = dict(eye=np.array(V['eye']), target=np.array(V['target']), up=(0, 1, 0), fov=70, near=3.0)
best = []
for f in os.listdir(os.path.join(d, 'geometry')):
    v, ix, rep = decode_psg3(open(os.path.join(d, 'geometry', f), 'rb').read())
    P = np.array([x[:3] for x in v]) * np.array([1, 1, -1.])
    UV = np.array([x[3:5] for x in v])
    T = np.array(ix).reshape(-1, 3)
    S, ok = SR.project(P, cam, 960, 540)
    for t in T:
        if not ok[t].all():
            if False:
                best.append((f, 'near-crossing', P[t].round(1).tolist(), UV[t].round(3).tolist()))
            continue
        x, y = S[t, 0], S[t, 1]
        if x.min() < 480 < x.max() and y.min() < 500 < y.max():
            best.append((f, 'covers', P[t].round(1).tolist(), UV[t].round(3).tolist(), S[t, 2].round(1).tolist()))
print('eye', V['eye'])
for b in best[:14]:
    print(b)
