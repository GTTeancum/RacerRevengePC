r"""Verify converted course files and render previews from the DECODED files.

For export\a2r\course<N>\:
  1. every PSG: own reader (geo_psg.parse -> write must give the same bytes), the repo's strict
     static decoder (tools\assets\decode_static48b.py decode_psg3: the Python twin of the port's
     src\assets\import_decode48c.cpp decodeMesh48C, same checks incl. winding vs stored normals and
     stored bounds) and the repo scene decoder (decode_scene48c.decode_psg3_scene);
  2. every PST: own reader + repo decode_texture48d.decode_texture (port rule, levels >= 16,
     power of two, palette bit-3/4 swap); every PSM: repo decode_texture48d.parse_material;
  3. previews (preview\*.png) rendered from what the repo decoders returned: top view and the four
     chase views of export\course<N>\preview (same cameras through the same transform). Revenge is
     left-handed, the preview renderer (tools\softrender.py) right-handed, so the decoded data is
     drawn with z negated back, i.e. as a left-handed renderer (Revenge) would show it.
python -B -I geo_verify.py N [--no-render]
"""
import json
import os
import sys
import collections

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, HERE)
sys.path.insert(0, TOOLS)
sys.dont_write_bytecode = True
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
import geo_psg  # noqa: E402
import geo_pst  # noqa: E402
import geo_psm  # noqa: E402
from decode_static48b import decode_psg3  # noqa: E402
from decode_scene48c import decode_psg3_scene  # noqa: E402
from decode_texture48d import decode_texture, parse_material  # noqa: E402


def verify(N, render=True):
    d = os.path.join(ROOT, 'export', 'a2r', 'course%d' % N)
    man = json.load(open(os.path.join(d, 'manifest.json')))
    res = collections.Counter()
    fails = []
    meshes = []
    for f in sorted(os.listdir(os.path.join(d, 'geometry'))):
        b = open(os.path.join(d, 'geometry', f), 'rb').read()
        try:
            assert geo_psg.write(geo_psg.parse(b)) == b
            res['psg_own_reader_ok'] += 1
            v, ix, rep = decode_psg3(b)
            res['psg_static_decoder_ok'] += 1
            res['psg_negative_normal_triangles'] += rep['normal_alignment']['negative']
            sc = decode_psg3_scene(b)
            res['psg_scene_decoder_ok'] += 1
            meshes.append((f, rep['material'], np.array([x[:3] for x in v]), np.array([x[3:5] for x in v]),
                           np.array([x[5:9] for x in v]), np.array(ix).reshape(-1, 3)))
        except Exception as e:  # noqa
            fails.append((f, str(e)))
    tex = {}
    for f in sorted(os.listdir(os.path.join(d, 'textures'))):
        b = open(os.path.join(d, 'textures', f), 'rb').read()
        try:
            if f.endswith('.pst'):
                geo_pst.parse(b)
                levels, meta = decode_texture(b)
                L = levels[-1]
                tex[f[:-4]] = np.frombuffer(L['rgba'], np.uint8).reshape(L['height'], L['width'], 4)
                res['pst_ok'] += 1
            else:
                m = parse_material(b)
                assert geo_psm.write(geo_psm.parse(b)) == b
                assert m['passes'][0]['textures'][0]['name'] == f[:-4]
                res['psm_ok'] += 1
        except Exception as e:  # noqa
            fails.append((f, str(e)))
    print('course %d: %s' % (N, dict(res)))
    for x in fails[:10]:
        print('   FAIL', x)
    out = dict(counts=dict(res), failures=fails)
    if render and meshes:
        out['previews'] = previews(N, d, man, meshes, tex)
    json.dump(out, open(os.path.join(d, 'verify.json'), 'w'), indent=1)
    return res, fails


def previews(N, d, man, meshes, tex):
    import softrender as SR
    pdir = os.path.join(d, 'preview')
    os.makedirs(pdir, exist_ok=True)
    X = man['transform']
    land = [m for m in meshes if '_sky_' not in m[0]]
    sky = [m for m in meshes if '_sky_' in m[0]]
    names = sorted(tex)
    tindex = {n: i for i, n in enumerate(names)}
    textures = [tex[n] for n in names]

    def arrays(ms):
        P, UV, C, T, TT = [], [], [], [], []
        base = 0
        for f, mat, p, uv, c, ix in ms:
            P.append(p * np.array([1, 1, -1.0]))  # draw left-handed data with the right-handed renderer
            UV.append(uv)
            C.append(c)
            T.append(ix + base)
            TT.append(np.full(len(ix), tindex.get(mat, -1)))
            base += len(p)
        return np.vstack(P), np.vstack(UV), np.vstack(C), np.vstack(T), np.concatenate(TT)

    P, UV, C, T, TT = arrays(land)
    lo, hi = P[np.unique(T)].min(0), P[np.unique(T)].max(0)
    centre = (lo + hi) / 2
    size = max(hi[0] - lo[0], hi[2] - lo[2]) * 1.04
    top = dict(ortho=True, center=centre, size=size, dir=(0, -1, 0), up=(0, 0, -1))
    img, _, _ = SR.render(P, T, top, 1400, 1400, uv=UV, tex_of_tri=TT, textures=textures, ambient=0.6)
    SR.save(img, os.path.join(pdir, 'top_textured.png'))
    tricol = C[T][:, :, :3].mean(1) * 2 / 255.0
    # back faces culled (two-sided arcade faces are stored as two coplanar triangles; without culling
    # the preview z-fights between the lit top and the unlit underside). softrender culls
    # screen-area > 0; with the z-negated drawing that is the Revenge FRONT face, so the triangles
    # are handed over reversed (checked by eye: the unreversed call shows only undersides).
    img, _, _ = SR.render(P, T[:, [0, 2, 1]], top, 1400, 1400, colors=np.clip(tricol, 0, 1), ambient=1.0, cull=True)
    SR.save(img, os.path.join(pdir, 'top_baked_light.png'))
    # chase cameras of export\course<N>\preview (course_preview.py), through the same transform
    path = np.loadtxt(os.path.join(ROOT, 'export', 'course%d' % N, 'path_table.tsv'))
    path = np.c_[path[:, 0], path[:, 2], path[:, 1]]  # x, y, z
    import a2r_space  # the one transform definition

    def tr(p):  # arcade -> Revenge -> drawn (z negated back)
        q = a2r_space.points(p, X)
        return q * np.array([1, 1, -1.0])
    Ps, UVs, Cs, Ts, TTs = arrays(sky) if sky else (None,) * 5
    n = len(path)
    out = []
    for k in range(4):
        i = (k * n) // 4
        j = (i + 4) % n
        y0 = path[i, 1] if np.isfinite(path[i, 1]) else np.nanmedian(path[:, 1])
        c0 = np.array([path[i, 0], y0, path[i, 2]])
        dirv = np.array([path[j, 0] - path[i, 0], 0.0, path[j, 2] - path[i, 2]])
        dirv /= max(np.linalg.norm(dirv), 1e-6)
        eye = c0 - dirv * 30 + np.array([0, 9.0, 0])
        tgt = c0 + dirv * 120 + np.array([0, 4.0, 0])
        cam = dict(eye=tr(eye), target=tr(tgt), up=(0, 1, 0), fov=70, near=1.0 * X['scale'])
        imf = zb = None
        if sky:
            _, zb, imf = SR.render(Ps, Ts, cam, 960, 540, uv=UVs, tex_of_tri=TTs, textures=textures, ambient=1.0)
            zb[:] = np.inf
        im, _, _ = SR.render(P, T, cam, 960, 540, uv=UV, tex_of_tri=TT, textures=textures, ambient=0.55, img=imf, zbuf=zb)
        SR.save(im, os.path.join(pdir, 'chase_%d.png' % k))
        out.append(dict(view=k, eye=cam['eye'].tolist(), target=cam['target'].tolist()))
    # side-by-side sheet: original export preview (left) vs decoded conversion (right)
    from PIL import Image
    pairs = [('top_textured.png', 700)] + [('chase_%d.png' % k, 480) for k in range(4)]
    rows = []
    for name, w in pairs:
        a = Image.open(os.path.join(ROOT, 'export', 'course%d' % N, 'preview', name)).convert('RGB')
        b = Image.open(os.path.join(pdir, name)).convert('RGB')
        h = int(a.height * w / a.width)
        rows.append((a.resize((w, h)), b.resize((w, h))))
    W = max(r[0].width for r in rows) * 2 + 10
    H = sum(r[0].height for r in rows) + 10 * len(rows)
    sheet = Image.new('RGB', (W, H), (20, 20, 20))
    y = 0
    for a, b in rows:
        sheet.paste(a, (0, y))
        sheet.paste(b, (a.width + 10, y))
        y += a.height + 10
    sheet.save(os.path.join(pdir, 'compare_original_vs_decoded.png'))
    return out


if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    for a in args:
        verify(int(a), render='--no-render' not in sys.argv)
