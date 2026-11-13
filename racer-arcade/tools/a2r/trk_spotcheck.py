r"""Close look at the converted collision around given points (Revenge x, z): close top view and a
side section of the collision faces, with the Revenge start slots, AI spline and the points marked.
    python -I trk_spotcheck.py <RES> <code> x,z [x,z ...]
Writes export\a2r\<code>\verify\spot_<i>_top.png / _side.png and prints the faces near each point."""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import trk_res3, trk_col, trk_binparam, trk_ini, trk_verify  # noqa: E402

BS = chr(92)


def load(path, code):
    r = trk_res3.read_res3(open(path, 'rb').read())
    files = {n.lower(): d for n, d in r.items()}
    scb = trk_binparam.decode(files['tracks' + BS + code + '.scb'])
    pos = {}
    for n, kv in scb:
        if '-model' in n:
            d = dict(kv)
            pos[d['slt'].lower()[:-4]] = np.array([float(x) for x in d['position'].split(',')])
    faces = []   # (name, tri(3,3), normal)
    for n, d in files.items():
        if n.startswith('geometry') and n.endswith('.col'):
            b = trk_col.read_col(d).body
            base = n.split(BS)[-1][:-4]
            V = b.verts.astype(np.float64) + pos.get(base, 0.0)
            for f, nr in zip(b.faces['v'], b.faces['normal']):
                faces.append((base, V[list(f)], nr.astype(np.float64)))
    _, mnodes, MP = trk_verify.load_spline(files['tracks' + BS + 'm' + code + '.spl'])
    _, anodes, AP = trk_verify.load_spline(files['tracks' + BS + 'a' + code + '.spl'])
    mtr = trk_ini.parse(files['tracks' + BS + 'm' + code + '.spl']).section('Track')
    grid, _ = trk_verify.revenge_grid(MP, float(mtr['TrackWidth']), mnodes)
    return faces, grid, MP, AP


def draw(faces, grid, MP, AP, c, half, fn_top, fn_side, extra=()):
    W = 900
    s = W / (2 * half)
    near = [(n, t, nr) for n, t, nr in faces if np.abs(t[:, [0, 2]] - c).max() < half * 1.5 and
            (np.abs(t[:, 0] - c[0]).min() < half and np.abs(t[:, 2] - c[1]).min() < half)]
    im = Image.new('RGB', (W, W), (10, 10, 14)); g = ImageDraw.Draw(im)

    def tp(p):
        return (W / 2 + (p[0] - c[0]) * s, W / 2 - (p[2] - c[1]) * s)
    for n, t, nr in sorted(near, key=lambda x: 'wall' in x[0]):
        if 'wall' in n:
            g.line([tp(t[0]), tp(t[1]), tp(t[2]), tp(t[0])], fill=(255, 0, 255), width=3)
        else:
            col = (60, 110, 160) if nr[1] > 0.5 else (200, 200, 60) if nr[1] > 0 else (255, 60, 60)
            g.polygon([tp(v) for v in t], fill=col, outline=(20, 20, 20))
    for i in range(len(AP)):
        g.line([tp(AP[i]), tp(AP[(i + 1) % len(AP)])], fill=(255, 235, 0), width=2)
    for i in range(len(MP)):
        g.line([tp(MP[i]), tp(MP[(i + 1) % len(MP)])], fill=(255, 140, 0), width=1)
    for k, p in enumerate(grid):
        q = tp(p); g.ellipse([q[0] - 6, q[1] - 6, q[0] + 6, q[1] + 6], outline=(0, 255, 0), width=2)
        g.text((q[0] + 7, q[1] - 6), str(k), fill=(0, 255, 0))
    for p in extra:
        q = tp((p[0], 0, p[1])); g.line([q[0] - 9, q[1] - 9, q[0] + 9, q[1] + 9], fill=(255, 255, 255), width=3)
        g.line([q[0] - 9, q[1] + 9, q[0] + 9, q[1] - 9], fill=(255, 255, 255), width=3)
    g.text((6, 6), 'top: floor blue (up), yellow (steep), red (down); walls magenta; AI yellow; map orange; grid green; point x', fill=(255, 255, 255))
    im.save(fn_top)
    # side section along z (x within +-40 ft of the point), y vertical
    ys = [t[:, 1] for n, t, nr in near]
    ymin = min(y.min() for y in ys) if ys else 0; ymax = max(y.max() for y in ys) if ys else 1
    im2 = Image.new('RGB', (W, 500), (10, 10, 14)); g2 = ImageDraw.Draw(im2)
    sy = 450 / max(ymax - ymin, 1)

    def sp(p):
        return (W / 2 + (p[2] - c[1]) * s, 470 - (p[1] - ymin) * sy)
    for n, t, nr in near:
        if np.abs(t[:, 0] - c[0]).min() < 40:
            g2.line([sp(t[0]), sp(t[1]), sp(t[2]), sp(t[0])], fill=(255, 0, 255) if 'wall' in n else (80, 160, 220), width=2)
    for k, p in enumerate(grid):
        if abs(p[0] - c[0]) < 60:
            q = sp(p); g2.ellipse([q[0] - 5, q[1] - 5, q[0] + 5, q[1] + 5], outline=(0, 255, 0), width=2)
    g2.text((6, 6), 'side section along z (|x - %.0f| < 40 ft), y up; walls magenta, floor blue; grid green' % c[0], fill=(255, 255, 255))
    im2.save(fn_side)
    return near


if __name__ == '__main__':
    path, code = sys.argv[1], sys.argv[2]
    faces, grid, MP, AP = load(path, code)
    print('grid', np.round(grid, 1).tolist())
    out = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(path))), 'verify')
    for i, a in enumerate(sys.argv[3:]):
        x, z = [float(v) for v in a.split(',')]
        near = draw(faces, grid, MP, AP, np.array([x, z]), 250.0, os.path.join(out, 'spot_%d_top.png' % i),
                    os.path.join(out, 'spot_%d_side.png' % i), extra=[(x, z)])
        walls = [(n, t) for n, t, nr in near if 'wall' in n and np.abs(t[:, 0] - x).min() < 120 and np.abs(t[:, 2] - z).min() < 120]
        print('point', x, z, 'faces near', len(near), 'walls within 120 ft', len(walls))
        for n, t in walls[:12]:
            print('   ', n, np.round(t, 1).tolist())
