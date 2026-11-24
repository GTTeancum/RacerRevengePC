r"""Readers for the n64r export folder (racer-n64\export\<NNN>_<name>\). Read-only, N64 space."""
import json
import os

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
N64 = os.path.normpath(os.path.join(HERE, '..', '..'))
EXPORT = os.path.join(N64, 'export')


def track_dir(model):
    for d in os.listdir(EXPORT):
        if d.startswith('%03d_' % model):
            return os.path.join(EXPORT, d)
    raise FileNotFoundError(model)


def read_obj(path):
    """-> list of groups: dict(name, mtl, P (n,3), C (n,3) or None, UV (n,2) image-v-down, T (m,3))
    (OBJ indices are global; each group gets its own local arrays)."""
    V, VC, VT = [], [], []
    groups = []
    cur = None
    for ln in open(path):
        if ln.startswith('v '):
            f = [float(x) for x in ln.split()[1:]]
            V.append(f[:3]); VC.append(f[3:6] if len(f) >= 6 else [1.0, 1.0, 1.0])
        elif ln.startswith('vt '):
            u, v = [float(x) for x in ln.split()[1:3]]
            VT.append((u, 1.0 - v))
        elif ln.startswith('g '):
            cur = dict(name=ln.split()[1], mtl=None, F=[])
            groups.append(cur)
        elif ln.startswith('usemtl '):
            cur['mtl'] = ln.split()[1]
        elif ln.startswith('f '):
            cur['F'].append([tuple(int(x) - 1 if x else -1 for x in (t.split('/') + ['', ''])[:2]) for t in ln.split()[1:]])
    V = np.array(V); VC = np.array(VC); VT = np.array(VT) if VT else np.zeros((0, 2))
    out = []
    for g in groups:
        if not g['F']:
            continue
        F = np.array(g['F'])                     # (m, 3, 2): vertex, uv
        vi = F[:, :, 0]; ti = F[:, :, 1]
        out.append(dict(name=g['name'], mtl=g['mtl'], P=V[vi], C=VC[vi],
                        UV=VT[ti] if (ti >= 0).all() and len(VT) else np.zeros(vi.shape + (2,))))
    return out          # per group: P (m,3,3), C (m,3,3), UV (m,3,2) per triangle corner


def load(model=143):
    d = track_dir(model)
    j = lambda n: json.load(open(os.path.join(d, n)))
    return dict(dir=d, track=j('track.json'), materials=j('materials.json'), splines=j('splines.json'),
                cameras=j('cameras.json'), surfaces=j('collision_surfaces.json'), triggers=j('triggers.json'),
                geometry=read_obj(os.path.join(d, 'geometry.obj')),
                skybox=read_obj(os.path.join(d, 'skybox.obj')),
                collision=read_obj(os.path.join(d, 'collision.obj')))


def bezier(p0, p1, p2, p3, t):
    t = np.asarray(t, float)[:, None]
    return ((1 - t) ** 3) * p0 + 3 * ((1 - t) ** 2) * t * p1 + 3 * (1 - t) * t * t * p2 + t ** 3 * p3


def segment_curve(spl, a, b, n=64):
    pa, pb = spl['points'][a], spl['points'][b]
    return bezier(np.array(pa['position']), np.array(pa['handle_out']), np.array(pb['handle_in']),
                  np.array(pb['position']), np.linspace(0, 1, n + 1))
