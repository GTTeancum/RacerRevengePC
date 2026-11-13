import sys, json, os
import numpy as np
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools')
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
import softrender as SR
from decode_static48b import decode_psg3
from decode_texture48d import decode_texture
d = r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\course1'
V = json.load(open(os.path.join(d, 'verify.json')))['previews'][0]
cam = dict(eye=np.array(V['eye']), target=np.array(V['target']), up=(0, 1, 0), fov=70, near=float(os.environ.get('NEAR','3')))
skip = sys.argv[1] if len(sys.argv) > 1 else '@@'
P, UV, T, TT, tex, names = [], [], [], [], [], {}
base = 0
for f in os.listdir(os.path.join(d, 'geometry')):
    if skip in f or '_sky_' in f:
        continue
    v, ix, rep = decode_psg3(open(os.path.join(d, 'geometry', f), 'rb').read())
    m = rep['material']
    if m not in names:
        L = decode_texture(open(os.path.join(d, 'textures', m + '.pst'), 'rb').read())[0][-1]
        names[m] = len(tex)
        tex.append(np.frombuffer(L['rgba'], np.uint8).reshape(L['height'], L['width'], 4))
    p = np.array([x[:3] for x in v]) * np.array([1, 1, -1.])
    P.append(p); UV.append(np.array([x[3:5] for x in v])); T.append(np.array(ix).reshape(-1, 3) + base)
    TT.append(np.full(len(ix) // 3, names[m])); base += len(p)
P, UV, T, TT = np.vstack(P), np.vstack(UV), np.vstack(T), np.concatenate(TT)
im, _, _ = SR.render(P, T, cam, 960, 540, uv=UV, tex_of_tri=TT, textures=tex, ambient=0.55)
SR.save(im, os.path.join(os.path.dirname(__file__), 'dbg_chase0_skip_%s.png' % skip.strip('@')))
t = tex[names['eb_te171_b']] if 'eb_te171_b' in names else None
if t is not None:
    print('te171_b texel(0,0)', t[0, 0], 'alpha<128 share', float((t[..., 3] < 128).mean()))
# which triangles cover pixel (480, 500)
S, ok = SR.project(P, cam, 960, 540)
for i, t in enumerate(T):
    if ok[t].all():
        x, y = S[t, 0], S[t, 1]
        if x.min() < 480 < x.max() and y.min() < 500 < y.max():
            print(i, TT[i], [k for k, v in names.items() if v == TT[i]], P[t].round(1).tolist(), UV[t].round(3).tolist(), S[t].round(1).tolist())
