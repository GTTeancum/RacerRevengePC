r"""The ONE definition of the arcade -> Revenge space transform and course naming.
Every a2r tool (geo_*.py geometry/textures, trk_*.py track data) imports this; nothing else
defines scale, axis flip, winding, origin or height offset.

    revenge = (S*x + ox, S*y + oy, zs*S*z + oz)     S = 3.0, zs = -1 (mirror Z)
    normals  (nx, ny, zs*nz);  triangle winding reversed when zs = -1
    ox, oz: the course's landscape (visual) minimum corner lands on (2000, 2000) ft
    oy: +1000 ft (fixed; arcade y = 0 -> 1000 ft)

Evidence: notes\a2r-geometry.md 1.1 (scale 3.0 from the pod chariot, 2.92-3.01) and 1.2
(arcade right-handed, Revenge left-handed -> negate Z). Origin and height offset are generated
(Revenge tracks are all-positive with ground 77-10,900 ft).

course_transform(N) computes ox/oz from the arcade landscape triangles exactly as the geometry
conversion does (same points, same formula) and caches the result in
export\a2r\space\course<N>.json together with the constants it was made with.
"""
import json
import os

import numpy as np

SCALE = 3.0          # arcade unit -> Revenge foot (geo_scale_pod.py: chariot 2.92..3.01)
MIRROR_Z = True      # geo_handedness.py: arcade right-handed, Revenge left-handed
MIN_CORNER = 2000.0  # landscape minimum corner (x, z) in Revenge feet
BASE_Y = 1000.0      # Revenge y of arcade y = 0 (generated)

# Revenge course code per arcade course (data). eb / lc: conversion-spec.md 2.1;
# ua / td / ka: provisional until Phase 5.
CODES = {0: 'ua', 1: 'eb', 2: 'lc', 3: 'td', 4: 'ka'}

REVENGE_CODES = ('ta', 'tb', 'tc', 'ra', 'rc', 'ma', 'mb', 'mc', 'ga', 'gb', 'gc', 'sa', 'sc')


def course_code(N, override=None):
    """Course code for arcade course N. `override` packs the course under another code (for the silo
    test: an existing Revenge code such as 'sc'); it is the only way to change the code."""
    if override is None:
        return CODES[N]
    o = override.lower()
    if len(o) != 2 or not o.isalpha():
        raise ValueError('course code must be two letters: %r' % override)
    return o


def overrides_original(code):
    """True when `code` names an original Revenge course (its pinned files are then shipped as borrowed)."""
    return code in REVENGE_CODES


HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))


def z_sign():
    return -1.0 if MIRROR_Z else 1.0


def constants():
    return dict(scale=SCALE, z_sign=z_sign(), min_corner=MIN_CORNER, base_y=BASE_Y)


def transform_from_points(P):
    """P (n, 3) arcade landscape points -> transform dict (keys used by every tool and manifest)."""
    zs = z_sign()
    q = np.asarray(P, np.float64) * np.array([SCALE, SCALE, SCALE * zs])
    off = np.array([MIN_CORNER - q[:, 0].min(), BASE_Y, MIN_CORNER - q[:, 2].min()])
    return dict(scale=SCALE, z_sign=zs, offset=off.tolist(),
                formula='revenge = (S*x + ox, S*y + oy, %s*S*z + oz)' % ('-' if MIRROR_Z else '+'))


def points(P, X):
    """arcade points (..., 3) -> Revenge."""
    return np.asarray(P, np.float64) * np.array([X['scale'], X['scale'], X['scale'] * X['z_sign']]) + np.array(X['offset'])


def xz(Q, X):
    """arcade (x, z) pairs (..., 2) -> Revenge (x, z)."""
    Q = np.asarray(Q, np.float64)
    return np.stack([Q[..., 0] * X['scale'] + X['offset'][0],
                     Q[..., 1] * X['scale'] * X['z_sign'] + X['offset'][2]], -1)


def normals(Nn, X):
    return np.asarray(Nn, np.float64) * np.array([1.0, 1.0, X['z_sign']])


def winding(X):
    """Vertex order to apply to every arcade triangle (mirror flips orientation)."""
    return [0, 2, 1] if X['z_sign'] < 0 else [0, 1, 2]


# Arcade collision triangles are wound clockwise seen from their walkable side: cross(v1-v0, v2-v0)
# points DOWN for every non-degenerate floor and edge-strip triangle (Etti IV: 6180 + 2000 of 6184
# records, EPROM 0xC80000; checked per course by trk_convert). Revenge COL faces take their normal from
# the winding and are one-sided, so collision needs the opposite order to the visual geometry.
ARCADE_COLLISION_WINDING_DOWN = True


def collision_order(X):
    """Vertex order for arcade collision triangles so the Revenge face normal points to the
    walkable side (mirror reversal, then the arcade collision winding)."""
    o = winding(X)
    return [o[0], o[2], o[1]] if ARCADE_COLLISION_WINDING_DOWN else o


def triangle(P, Nn, X):
    """One arcade triangle (3, 3) + vertex normals -> Revenge positions, normals, order used."""
    o = winding(X)
    return points(P, X)[o], normals(Nn, X)[o], o


def course_transform(N):
    """Transform of arcade course N (cached; recomputed if the constants change)."""
    cache = os.path.join(SCR, 'export', 'a2r', 'space', 'course%d.json' % N)
    if os.path.exists(cache):
        c = json.load(open(cache))
        if c.get('constants') == constants():
            return c['transform']
    import geo_course   # landscape triangles exactly as the geometry conversion reads them
    tris, _ = geo_course.arcade_triangles(N)
    X = transform_from_points(np.vstack([t['P'] for t in tris['land']]))
    os.makedirs(os.path.dirname(cache), exist_ok=True)
    json.dump(dict(constants=constants(), transform=X), open(cache, 'w'), indent=1)
    return X
