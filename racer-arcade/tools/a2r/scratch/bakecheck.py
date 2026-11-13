"""Bake-only views of a course: shipped bake (normals turned to the winding), bake from the arcade's
own normals, and the faces whose normals were turned (red). python -I bakecheck.py N"""
import os, sys
import numpy as np
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE)); sys.path.insert(0, os.path.join(HERE, '..', '..'))
import geo_course as G, a2r_space, softrender as SR
N = int(sys.argv[1])
SCR = os.path.normpath(os.path.join(HERE, '..', '..', '..'))
tris, _ = G.arcade_triangles(N)
X = G.transform_fn(tris['land'])
FLIP = np.array([1.0, 1.0, -1.0])
P_, C1, C2, C3 = [], [], [], []
nflip = 0
for t in tris['land']:
    P, Nn, UV, A = G.apply(t, X)
    e = np.cross(P[1] - P[0], P[2] - P[0])
    if np.linalg.norm(e) < 1e-9: continue
    f = float(e @ Nn.sum(0)) < 0
    nflip += f
    c_orig = G.bake(Nn, t['mat'], A)[:, :3] / 128.0
    c_ship = G.bake(-Nn if f else Nn, t['mat'], A)[:, :3] / 128.0
    P_.append(P * FLIP); C1.append(c_ship); C2.append(c_orig)
    C3.append(np.tile([1, 0, 0] if f else ([0.3, 0.3, 0.9] if t['back'] else [0.7, 0.7, 0.7]), (3, 1)))
P = np.vstack(P_); T = np.arange(len(P)).reshape(-1, 3)[:, [0, 2, 1]]
print('faces', len(T), 'turned', nflip, 'back', sum(t['back'] for t in tris['land']))
path = np.loadtxt(os.path.join(SCR, 'export', 'course%d' % N, 'path_table.tsv'))
path = np.c_[path[:, 0], path[:, 2], path[:, 1]]
n = len(path); rows = []
for k in range(4):
    i = (k * n) // 4; j = (i + 4) % n
    y0 = path[i, 1] if np.isfinite(path[i, 1]) else np.nanmedian(path[:, 1])
    c0 = np.array([path[i, 0], y0, path[i, 2]])
    dv = np.array([path[j, 0] - path[i, 0], 0.0, path[j, 2] - path[i, 2]]); dv /= max(np.linalg.norm(dv), 1e-6)
    eye = a2r_space.points(c0 - dv * 30 + np.array([0, 9.0, 0]), X) * FLIP
    tgt = a2r_space.points(c0 + dv * 120 + np.array([0, 4.0, 0]), X) * FLIP
    cam = dict(eye=eye, target=tgt, up=(0, 1, 0), fov=70, near=3.0)
    ims = [SR.render(P, T, cam, 640, 360, colors=np.vstack(C).reshape(-1, 3, 3).mean(1), ambient=1.0)[0] for C in (C1, C2, C3)]
    rows.append(np.concatenate(ims, 1))
Image.fromarray(np.concatenate(rows, 0)).save(os.path.join(HERE, 'bakecheck_c%d.png' % N))
