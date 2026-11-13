r"""Close-up check of a chase view: three panels, side by side
  1. export\course<N>\preview\chase_<k>.png   arcade export preview (its renderer ignores the Hikaru
     101 depth bias, so coplanar decals win or lose by draw order)
  2. arcade data with the bias rule (raw arcade triangles + original arcade textures; each bias layer
     moved toward the viewer in draw order, Odin depth_bias.hpp rule, geo_course.layer_value)
  3. the converted files decoded by the repo decoders (export\a2r\course<N>\geometry, textures)
python -B -I geo_bias_check.py N k [x0 y0 x1 y1]   (crop box in the 960x540 view, default whole)
Writes tools\a2r\scratch\bias_check_c<N>_v<k>.png
"""
import os
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))
sys.dont_write_bytecode = True
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
import softrender as SR  # noqa: E402
import geo_course as GC  # noqa: E402
from decode_static48b import decode_psg3  # noqa: E402
from decode_texture48d import decode_texture  # noqa: E402

ROOT = GC.ROOT


def camera(N, k, X):
    path = np.loadtxt(os.path.join(ROOT, 'export', 'course%d' % N, 'path_table.tsv'))
    path = np.c_[path[:, 0], path[:, 2], path[:, 1]]
    n = len(path)
    i = (k * n) // 4
    j = (i + 4) % n
    y0 = path[i, 1] if np.isfinite(path[i, 1]) else np.nanmedian(path[:, 1])
    c0 = np.array([path[i, 0], y0, path[i, 2]])
    d = np.array([path[j, 0] - path[i, 0], 0.0, path[j, 2] - path[i, 2]])
    d /= max(np.linalg.norm(d), 1e-6)
    S, zs, off = X['scale'], X['z_sign'], np.array(X['offset'])

    def tr(p):
        return (np.asarray(p, float) * np.array([S, S, S * zs]) + off) * np.array([1, 1, -1.0])
    return dict(eye=tr(c0 - d * 30 + np.array([0, 9.0, 0])), target=tr(c0 + d * 120 + np.array([0, 4.0, 0])),
                up=(0, 1, 0), fov=70, near=3.0)


def arcade_layered(N, cam):
    tr, _ = GC.arcade_triangles(N)
    X = GC.transform_fn(tr['land'])
    items = []
    for t in tr['land']:
        P, Nn, UV, A = GC.apply(t, X)
        e = np.cross(P[1] - P[0], P[2] - P[0])
        if np.linalg.norm(e) < 1e-9:
            continue
        if float(e @ Nn.sum(0)) < 0:
            Nn = -Nn
        items.append((P, Nn, UV, t['tid'], GC.layer_value(t)))
    rank = {v: i for i, v in enumerate(sorted({it[4] for it in items}))}
    texcache, tindex = [], {}
    Ps, UVs, TT = [], [], []
    for P, Nn, UV, tid, L in items:
        n = Nn / np.maximum(np.linalg.norm(Nn, axis=1, keepdims=True), 1e-12)
        P = P + n * rank[L] * GC.LAYER_STEP
        if tid is not None and tid not in tindex:
            tindex[tid] = len(texcache)
            texcache.append(GC.EC.texture_rgba(tid)[0])
        Ps.append(P * np.array([1, 1, -1.0]))
        UVs.append(UV)
        TT.append(tindex.get(tid, -1))
    P = np.vstack(Ps)
    T = np.arange(len(P)).reshape(-1, 3)
    return SR.render(P, T, cam, 960, 540, uv=np.vstack(UVs), tex_of_tri=np.array(TT), textures=texcache, ambient=0.55)[0], X


def converted(N, cam):
    d = os.path.join(ROOT, 'export', 'a2r', 'course%d' % N)
    P, UV, T, TT, tex, names = [], [], [], [], [], {}
    base = 0
    for f in os.listdir(os.path.join(d, 'geometry')):
        if '_sky_' in f:
            continue
        v, ix, rep = decode_psg3(open(os.path.join(d, 'geometry', f), 'rb').read())
        m = rep['material']
        if m not in names:
            L = decode_texture(open(os.path.join(d, 'textures', m + '.pst'), 'rb').read())[0][-1]
            names[m] = len(tex)
            tex.append(np.frombuffer(L['rgba'], np.uint8).reshape(L['height'], L['width'], 4))
        p = np.array([x[:3] for x in v]) * np.array([1, 1, -1.0])
        P.append(p)
        UV.append(np.array([x[3:5] for x in v]))
        T.append(np.array(ix).reshape(-1, 3) + base)
        TT.append(np.full(len(ix) // 3, names[m]))
        base += len(p)
    return SR.render(np.vstack(P), np.vstack(T), cam, 960, 540, uv=np.vstack(UV), tex_of_tri=np.concatenate(TT),
                     textures=tex, ambient=0.55)[0]


def main(N, k, box=None):
    tr, _ = GC.arcade_triangles(N)
    X = GC.transform_fn(tr['land'])
    cam = camera(N, k, X)
    a = np.asarray(Image.open(os.path.join(ROOT, 'export', 'course%d' % N, 'preview', 'chase_%d.png' % k)).convert('RGB'))
    b, _ = arcade_layered(N, cam)
    c = converted(N, cam)
    if box:
        x0, y0, x1, y1 = box
        a, b, c = a[y0:y1, x0:x1], b[y0:y1, x0:x1], c[y0:y1, x0:x1]
    gap = np.zeros((a.shape[0], 8, 3), np.uint8)
    im = Image.fromarray(np.concatenate([a, gap, b, gap, c], 1))
    if box:
        im = im.resize((im.width * 3, im.height * 3), Image.NEAREST)
    out = os.path.join(HERE, 'scratch', 'bias_check_c%d_v%d.png' % (N, k))
    im.save(out)
    print(out)


if __name__ == '__main__':
    N, k = int(sys.argv[1]), int(sys.argv[2])
    main(N, k, [int(x) for x in sys.argv[3:7]] if len(sys.argv) >= 7 else None)
