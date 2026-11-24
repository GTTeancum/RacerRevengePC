r"""Visible barrier faces with no collision behind them. For every near-vertical visual triangle
(|ny| < 0.3, at least 6 ft tall) whose foot stands on a collision floor face (foot within 12 ft
above it), report it when no generated wall base lies within 12 ft of its foot. Writes a top view
export\...\verify\barrier_gaps.png and prints counts.
    python -I trk_barriercheck.py <RES> <code> <out_png>"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
import trk_res3, trk_col, trk_verify  # noqa: E402
BS = chr(92)


def main(res, code, out):
    r = trk_res3.read_res3(open(res, 'rb').read())
    files = {n.lower(): d for n, d in r.items()}
    floor, walls = [], []
    for n, d in files.items():
        if n.startswith('geometry') and n.endswith('.col'):
            b = trk_col.read_col(d).body
            V = b.verts.astype(np.float64); F = b.faces['v'].astype(np.int64)
            nrm = b.faces['normal'].astype(np.float64)
            for f, nn in zip(F, nrm):
                (floor if nn[1] > 0.05 else walls).append(V[f])
    floor = np.array(floor); walls = np.array(walls)
    wb = []                                  # wall base points every 4 ft
    for w in walls:
        lo = w[np.argsort(w[:, 1])[:2]]
        L = max(1, int(np.linalg.norm(lo[1] - lo[0]) // 4))
        for t in np.linspace(0, 1, L + 1):
            wb.append(lo[0] + (lo[1] - lo[0]) * t)
    wb = np.array(wb)
    A, B, C = floor[:, 0], floor[:, 1], floor[:, 2]
    den = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])

    def floor_y(p):
        with np.errstate(all='ignore'):
            l0 = ((B[:, 2] - C[:, 2]) * (p[0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (p[2] - C[:, 2])) / den
            l1 = ((C[:, 2] - A[:, 2]) * (p[0] - C[:, 0]) + (A[:, 0] - C[:, 0]) * (p[2] - C[:, 2])) / den
        l2 = 1 - l0 - l1
        m = (l0 >= -1e-4) & (l1 >= -1e-4) & (l2 >= -1e-4) & (np.abs(den) > 1e-9)
        if not m.any():
            return None
        ys = l0[m] * A[m, 1] + l1[m] * B[m, 1] + l2[m] * C[m, 1]
        ys = ys[ys <= p[1] + 1.0]
        return ys.max() if len(ys) else None
    vis = trk_verify.load_visual(files)
    gaps, tested = [], 0
    from scipy.spatial import cKDTree
    tree = cKDTree(wb[:, [0, 2]])
    for name, m, P, T, UV in vis:
        for t in T:
            p = P[t]
            e = np.cross(p[1] - p[0], p[2] - p[0]); nl = np.linalg.norm(e)
            if nl < 1e-6 or abs(e[1] / nl) > 0.3 or p[:, 1].ptp() < 6:
                continue
            foot = p[np.argsort(p[:, 1])[:2]].mean(0)
            fy = floor_y(foot)
            if fy is None or foot[1] - fy > 12:
                continue
            tested += 1
            d, _ = tree.query(foot[[0, 2]])
            if d > 12:
                gaps.append((foot, m, d))
    print('barrier-like faces standing on floor:', tested, ' without a wall within 12 ft:', len(gaps))
    from collections import Counter
    print('by material:', Counter(g[1] for g in gaps).most_common(10))
    # top view
    lo = floor[:, :, [0, 2]].reshape(-1, 2).min(0) - 50; hi = floor[:, :, [0, 2]].reshape(-1, 2).max(0) + 50
    S = 2000 / (hi - lo).max(); W, H = (np.ceil((hi - lo) * S) + 1).astype(int)
    im = Image.new('RGB', (W, H), (20, 20, 26)); dr = ImageDraw.Draw(im)
    tp = lambda q: (float((q[0] - lo[0]) * S), float(H - (q[2] - lo[1]) * S))
    for f in floor:
        dr.polygon([tp(v) for v in f], fill=(70, 70, 80))
    for q in wb[::2]:
        x, y = tp(q); dr.point((x, y), fill=(80, 200, 255))
    for foot, m, d in gaps:
        x, y = tp(foot); dr.ellipse((x - 3, y - 3, x + 3, y + 3), fill=(255, 60, 60))
    im.save(out)
    return gaps


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], sys.argv[3])
