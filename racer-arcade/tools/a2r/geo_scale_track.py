r"""Scale measurement 2: track half-width.
Revenge: tracks\a<xx>.spl AI spline Left/RightTrackWidth (13 courses, installed RES files).
Arcade: AI guide edge lines (notes\race-paths.txt 'guide' sections, half the left-right distance per
node) and the collision road: walk perpendicular to the centre line over the non-wall collision
triangles (export\course<N>\collision.obj) until the ground ends or a wall (0x8000) starts.
python -B -I geo_scale_track.py"""
import sys, os, re
import numpy as np
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from geo_res import read_res
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
G = r'C:\Games\Star Wars Racer Revenge'
TRACKS = 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split()


def revenge_widths(prefix='a'):
    out = {}
    for t in TRACKS:
        m, _, _ = read_res(os.path.join(G, 'TRACKS', t + '.RES'))
        txt = m[r'tracks' + '\\' + '%s%s.spl' % (prefix, t.lower())].decode('latin1')
        L = [float(x) for x in re.findall(r'LeftTrackWidth=([-\d.]+)', txt)]
        R = [float(x) for x in re.findall(r'RightTrackWidth=([-\d.]+)', txt)]
        h = (np.array(L) + np.array(R)) / 2
        out[t] = dict(n=len(h), mean=h.mean(), median=np.median(h), p10=np.percentile(h, 10), p90=np.percentile(h, 90),
                      capped160=float(np.mean((np.array(L) >= 160) | (np.array(R) >= 160))))
    return out


def arcade_guides():
    txt = open(os.path.join(ROOT, 'notes', 'race-paths.txt')).read()
    out = {}
    for sec in re.split(r'^## ', txt, flags=re.M)[1:]:
        head, *rows = sec.strip().splitlines()
        if head.startswith('guide'):
            n = int(head.split()[1])
            A = np.array([[float(x) for x in r.split()[1:5]] for r in rows if len(r.split()) == 5])
            w = np.hypot(A[:, 2] - A[:, 0], A[:, 3] - A[:, 1]) / 2
            out[n] = dict(n=len(w), mean=w.mean(), median=np.median(w), p10=np.percentile(w, 10), p90=np.percentile(w, 90))
        if head.startswith('centre'):
            n = int(head.split()[1])
            out.setdefault('centre', {})[n] = np.array([[float(x) for x in r.split()[1:3]] for r in rows if len(r.split()) == 3])
    return out


def raster(path, cell=1.0):
    V, groups, cur = [], {}, None
    for ln in open(path):
        if ln.startswith('v '):
            V.append([float(x) for x in ln.split()[1:4]])
        elif ln.startswith('g '):
            cur = ln.split()[1]
            groups[cur] = []
        elif ln.startswith('f '):
            groups[cur].append([int(x) - 1 for x in ln.split()[1:4]])
    V = np.array(V)
    lo = V[:, [0, 2]].min(0) - 2
    hi = V[:, [0, 2]].max(0) + 2
    shape = tuple(((hi - lo) / cell).astype(int) + 1)
    ground = np.zeros(shape, bool)
    wall = np.zeros(shape, bool)
    for g, tris in groups.items():
        tgt = wall if g == 'surface_8000' else ground
        for t in tris:
            P = (V[t][:, [0, 2]] - lo) / cell
            a, b, c = P
            x0, y0 = np.floor(P.min(0)).astype(int)
            x1, y1 = np.ceil(P.max(0)).astype(int)
            xs, ys = np.meshgrid(np.arange(x0, x1 + 1), np.arange(y0, y1 + 1), indexing='ij')
            px, py = xs + 0.5, ys + 0.5
            d = (b[1] - c[1]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[1] - c[1])
            if abs(d) < 1e-9:
                if g == 'surface_8000':  # vertical wall: mark the edge cells
                    for s in np.linspace(0, 1, 64):
                        for p, q in ((a, b), (b, c), (c, a)):
                            x, y = (p + (q - p) * s).astype(int)
                            if 0 <= x < shape[0] and 0 <= y < shape[1]:
                                tgt[x, y] = True
                continue
            l0 = ((b[1] - c[1]) * (px - c[0]) + (c[0] - b[0]) * (py - c[1])) / d
            l1 = ((c[1] - a[1]) * (px - c[0]) + (a[0] - c[0]) * (py - c[1])) / d
            ins = (l0 >= -0.02) & (l1 >= -0.02) & (1 - l0 - l1 >= -0.02)
            xs, ys = xs[ins], ys[ins]
            ok = (xs >= 0) & (ys >= 0) & (xs < shape[0]) & (ys < shape[1])
            tgt[xs[ok], ys[ok]] = True
    return ground, wall, lo, cell


def road_halfwidth(course, centre):
    ground, wall, lo, cell = raster(os.path.join(ROOT, 'export', 'course%d' % course, 'collision.obj'))
    C = centre
    D = np.roll(C, -1, 0) - np.roll(C, 1, 0)
    D /= np.linalg.norm(D, axis=1)[:, None]
    Nrm = np.c_[-D[:, 1], D[:, 0]]
    half = []
    for p, n in zip(C, Nrm):
        side = []
        for sgn in (1, -1):
            dist = 0.0
            for s in np.arange(0, 400, 0.5):
                q = ((p + sgn * s * n) - lo) / cell
                x, y = int(q[0]), int(q[1])
                if not (0 <= x < ground.shape[0] and 0 <= y < ground.shape[1]) or wall[x, y] or not ground[x, y]:
                    break
                dist = s
            side.append(dist)
        half.append(sum(side) / 2)
    h = np.array(half)
    return dict(n=len(h), mean=h.mean(), median=np.median(h), p10=np.percentile(h, 10), p90=np.percentile(h, 90))


def main():
    rv = revenge_widths()
    ag = arcade_guides()
    print('REVENGE AI spline half-width (ft): course n mean median p10 p90 share-capped-at-160')
    for t, d in rv.items():
        print('  %s %4d %7.1f %7.1f %7.1f %7.1f %.2f' % (t, d['n'], d['mean'], d['median'], d['p10'], d['p90'], d['capped160']))
    allrv = np.array([d['median'] for d in rv.values()])
    print('  median of course medians %.1f, range %.1f..%.1f' % (np.median(allrv), allrv.min(), allrv.max()))
    mv = revenge_widths('m')
    print('REVENGE map (lap) spline half-width (ft):')
    for t, d in mv.items():
        print('  %s %4d %7.1f %7.1f %7.1f %7.1f' % (t, d['n'], d['mean'], d['median'], d['p10'], d['p90']))
    allmv = np.array([d['median'] for d in mv.values()])
    print('  median of course medians %.1f, range %.1f..%.1f' % (np.median(allmv), allmv.min(), allmv.max()))
    print('ARCADE AI guide half-width (units) / collision road half-width (units):')
    gm, rm = [], []
    for c in range(4):
        g = ag[c]
        r = road_halfwidth(c, ag['centre'][c])
        gm.append(g['median'])
        rm.append(r['median'])
        print('  course %d guide mean %.1f median %.1f p10 %.1f p90 %.1f | road mean %.1f median %.1f p10 %.1f p90 %.1f'
              % (c, g['mean'], g['median'], g['p10'], g['p90'], r['mean'], r['median'], r['p10'], r['p90']))
    print('RATIO Revenge AI median / Arcade guide median: %.2f (per course %s)' % (
        np.median(allrv) / np.median(gm), [round(np.median(allrv) / x, 2) for x in gm]))
    print('RATIO Revenge AI median / Arcade road median: %.2f (per course %s)' % (
        np.median(allrv) / np.median(rm), [round(np.median(allrv) / x, 2) for x in rm]))
    print('RATIO Revenge map-spline median / Arcade road median: %.2f (per course %s)' % (
        np.median(allmv) / np.median(rm), [round(np.median(allmv) / x, 2) for x in rm]))
    print('RATIO Revenge map-spline median / Arcade guide median: %.2f' % (np.median(allmv) / np.median(gm)))


if __name__ == '__main__':
    main()
