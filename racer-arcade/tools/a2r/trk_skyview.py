r"""Look at a converted sky the way the port draws it: every tile decoded through FFmpeg (the port's
IPU path), sky looked up by view direction (camera-centred, cube centre +1024 ft, drawn first, no
depth/fog), clear colour = Fog ColorRGB below the cube's authored tiles; then the decoded PSG
geometry drawn over it. Chase cameras are the arcade preview cameras (export\course<N>\preview,
course_preview.py) through a2r_space, so each view sits next to the arcade's own image.
    python -I trk_skyview.py N [root]
Writes <root>\verify\sky_chase_<k>.png (arcade | converted), sky_seams.png, sky_faces.png.
"""
import json
import os
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(SCR, 'tools'))
import a2r_space, trk_sky, trk_res3, trk_binparam, trk_verify  # noqa: E402
import softrender as SR  # noqa: E402

BS = chr(92)
FLIP = np.array([1.0, 1.0, -1.0])


def pixel_dirs(cam, W, H):
    eye, r, u, f = SR.look_at(cam['eye'], cam['target'], cam['up'])
    foc = (W / 2) / np.tan(np.radians(cam.get('fov', 60)) / 2)
    px, py = np.meshgrid(np.arange(W) + 0.5, np.arange(H) + 0.5)
    d = f[None, None] + ((px - W / 2) / foc)[..., None] * r + (-(py - H / 2) / foc)[..., None] * u
    d = d / np.linalg.norm(d, axis=-1, keepdims=True)
    return d * FLIP          # render space -> Revenge axes


def main(N, root=None):
    code = a2r_space.CODES[N]
    root = root or os.path.join(SCR, 'export', 'a2r', code)
    man = json.load(open(os.path.join(root, 'manifest.json')))
    X = man['transform']
    r = trk_res3.read_res3(open(os.path.join(root, 'install', 'TRACKS', '%s.RES' % man['code'].upper()), 'rb').read())
    files = {n.lower(): d for n, d in r.items()}
    cub = trk_sky.read_cub(files['skies' + BS + man['code'] + '.cub'])
    ff = trk_sky.ffmpeg_path()
    tiles = {k: trk_sky.decode_ipum(v[3], ff) for k, v in cub['tiles'].items()}
    scb = dict(trk_binparam.decode(files['tracks' + BS + man['code'] + '.scb']))
    clear = tuple(int(x) for x in dict(scb['fog'])['colorrgb'].split(','))
    clear_f = np.array(clear, float)
    vd = os.path.join(root, 'verify')
    os.makedirs(vd, exist_ok=True)
    # faces contact sheet (face order +Z -X -Z +X, top; 4x4 tiles, inner 120 texels of each tile)
    sheet = np.zeros((480 * 2, 480 * 4, 3), np.uint8) + np.array(clear, np.uint8)
    for (f, t), img in tiles.items():
        c, rw = t % 4, t // 4
        inner = img[4:124, 4:124, :3]
        if f < 4:
            sheet[rw * 120:(rw + 1) * 120, f * 480 + c * 120:f * 480 + (c + 1) * 120] = inner
        else:
            sheet[480 + rw * 120:480 + (rw + 1) * 120, 480 + c * 120:480 + (c + 1) * 120] = inner
    Image.fromarray(sheet).save(os.path.join(vd, 'sky_faces.png'))
    # seam views: sky only, up close at a side-face corner, at the side/top edge and at the tile rows
    views = []
    for az, el, fov in ((45, 20, 40), (0, 44, 40), (45, 60, 50), (135, 8, 40)):
        a, e = np.radians(az), np.radians(el)
        d = np.array([np.sin(a) * np.cos(e), np.sin(e), np.cos(a) * np.cos(e)])
        cam = dict(eye=np.zeros(3), target=d * FLIP, up=(0, 1, 0), fov=fov)
        dirs = pixel_dirs(cam, 480, 320)
        views.append(np.clip(trk_sky.lookup(dirs, tiles, clear_f), 0, 255).astype(np.uint8))
    Image.fromarray(np.concatenate([np.concatenate(views[:2], 1), np.concatenate(views[2:], 1)], 0)).save(
        os.path.join(vd, 'sky_seams.png'))
    # chase views with geometry (decoded PSGs) over the sky
    vis = trk_verify.load_visual(files)
    tex_names = sorted(set(m for _, m, _, _, _ in vis))
    import geo_pst
    textures, tindex = [], {}
    for i, m in enumerate(tex_names):
        textures.append(geo_pst.rgba_levels(geo_pst.parse(files['textures' + BS + m.lower() + '.pst']))[-1]); tindex[m] = i
    P = np.vstack([v[2] for v in vis]) * FLIP
    base = np.cumsum([0] + [len(v[2]) for v in vis])[:-1]
    T = np.vstack([v[3] + b for v, b in zip(vis, base)])[:, [0, 2, 1]]
    UV = np.vstack([v[4] for v in vis])
    TT = np.concatenate([np.full(len(v[3]), tindex[v[1]]) for v in vis])
    path = np.loadtxt(os.path.join(SCR, 'export', 'course%d' % N, 'path_table.tsv'))
    path = np.c_[path[:, 0], path[:, 2], path[:, 1]]
    n = len(path)
    for k in range(4):
        i = (k * n) // 4; j = (i + 4) % n
        y0 = path[i, 1] if np.isfinite(path[i, 1]) else np.nanmedian(path[:, 1])
        c0 = np.array([path[i, 0], y0, path[i, 2]])
        dv = np.array([path[j, 0] - path[i, 0], 0.0, path[j, 2] - path[i, 2]]); dv /= max(np.linalg.norm(dv), 1e-6)
        eye = a2r_space.points(c0 - dv * 30 + np.array([0, 9.0, 0]), X) * FLIP
        tgt = a2r_space.points(c0 + dv * 120 + np.array([0, 4.0, 0]), X) * FLIP
        cam = dict(eye=eye, target=tgt, up=(0, 1, 0), fov=70, near=3.0)
        bg = trk_sky.lookup(pixel_dirs(cam, 960, 540), tiles, clear_f) / 255.0
        img, _, _ = SR.render(P, T, cam, 960, 540, uv=UV, tex_of_tri=TT, textures=textures, ambient=0.55,
                              img=bg.copy(), zbuf=np.full((540, 960), np.inf))
        arc = os.path.join(SCR, 'export', 'course%d' % N, 'preview', 'chase_%d.png' % k)
        left = np.asarray(Image.open(arc).convert('RGB').resize((960, 540)))
        Image.fromarray(np.concatenate([left, img], 1)).save(os.path.join(vd, 'sky_chase_%d.png' % k))
    print('sky views written to', vd, 'clear', clear)


if __name__ == '__main__':
    main(int(sys.argv[1]), sys.argv[2] if len(sys.argv) > 2 else None)
