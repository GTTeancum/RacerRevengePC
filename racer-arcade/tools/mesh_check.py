#!/usr/bin/env python3
"""Render a model (bus address, size) with hikaru_mesh from 4 directions into one PNG.
python -I mesh_check.py IMG_DIR BUSADDR SIZE OUT.png [--cull]
Back faces (with --cull) are shown red so holes and wrong windings stand out."""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hikaru_mesh as HM  # noqa: E402
import softrender as SR  # noqa: E402

img_dir, addr, size, out = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), sys.argv[4]
m = HM.decode(HM.Rom(img_dir).read(addr, size))
P = np.array(m.positions)
T = np.array([t[:3] for t in m.tris])
c = (P.max(0) + P.min(0)) / 2
r = np.linalg.norm(P - c, axis=1).max()
tiles = []
for k, (az, el) in enumerate(((35, 25), (215, 25), (125, -30), (305, 60))):
    a, e = np.radians(az), np.radians(el)
    d = np.array([np.cos(e) * np.cos(a), np.sin(e), np.cos(e) * np.sin(a)])
    cam = dict(eye=c + d * r * 2.6, target=c, up=(0, 1, 0), fov=45)
    # front faces grey, back faces red: draw both, then mark by winding in screen space
    S, ok = SR.project(P, cam, 512, 512)
    area = (S[T[:, 1], 0] - S[T[:, 0], 0]) * (S[T[:, 2], 1] - S[T[:, 0], 1]) - \
           (S[T[:, 2], 0] - S[T[:, 0], 0]) * (S[T[:, 1], 1] - S[T[:, 0], 1])
    cols = np.where((area > 0)[:, None], [[0.9, 0.25, 0.2]], [[0.8, 0.8, 0.75]])
    im, _, _ = SR.render(P, T, cam, 512, 512, colors=cols)
    tiles.append(im)
top = np.concatenate(tiles[:2], 1)
bot = np.concatenate(tiles[2:], 1)
SR.save(np.concatenate([top, bot], 0), out)
print(len(m.positions), 'verts', len(m.tris), 'tris', HM.edge_report(m))
