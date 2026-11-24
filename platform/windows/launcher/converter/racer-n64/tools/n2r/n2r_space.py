r"""The ONE definition of the N64 Racer -> Revenge space transform. Every n2r tool imports this;
nothing else defines scale, axes, winding, origin or height offset.

    revenge = (S*x + ox,  S*z + oy,  S*y + oz)        S = SCALE (user-chosen 2.2; faithful 1.0)
    normals  (nx, nz, ny);  every triangle's vertex order reversed (axis swap, det = -1)

Evidence (notes\n2r-track.md section 1):
  * Scale 1.0: SWE1R units are feet. The PC game sets its 3D-audio scale with
    IA3d4::SetUnitsPerMeter(3.28) (ref\SW_RACER_RE\src\Swr\swrSound.c:322): 3.28 game units per
    metre = 1 unit per foot, the Revenge unit. Cross-check (n2r_measure.py): Spice Mine Run road
    half-width at scale 1 against the Revenge originals' map-gate half-widths.
  * Axes: Racer is right-handed, +Z up. rdCamera_PerspProject (ref\SW_RACER_RE\src\Engine\rdCamera.c:298)
    puts camera-space +x to screen right, +z to screen up, +y = depth: right = +x, forward = +y,
    up = +z, x cross y = z -> right-handed. Revenge is left-handed, Y up, right = +x, forward = +z
    (a2r-geometry.md 1.2, POD01 left parts at -x). Swapping y and z maps one onto the other with
    left and right preserved (no mirror): a determinant -1 axis permutation turns a right-handed
    frame into a left-handed one. The permutation flips the algebraic sign of every cross product,
    so triangle vertex order is reversed to keep cross(b-a, c-a) on the same physical side.
  * ox/oz: the visual bounding box minimum corner (x, y) lands on (2000, 2000) ft, as the a2r
    tracks; oy: N64 z = 0 -> 1000 ft (generated). Revenge tracks are all-positive.
"""
import json
import os

import numpy as np

SCALE = float(os.environ.get('N2R_SCALE', '2.2'))   # USER-CHOSEN SCALE (2026-11-23): 1 N64 unit = 2.2 ft.
# Revenge pods are about 2x the width of the same racers' N64 pods (pod_scale.py, median 2.0 over 11 racers), so at 1:1
# the roads feel half as wide; the user chose 2.2 (notes n64-scale.md section 7). Everything spatial scales with it
# (geometry, collision, lines, triggers, cameras, zone slabs, fog distances, floor-table distances); speeds and times do not.
# The faithful unit (audio 3.28 units per metre) is 1.0: N2R_SCALE=1 or n2r_convert --scale 1.
MIN_CORNER = 2000.0
BASE_Y = 1000.0
PERM = (0, 2, 1)          # revenge axis k takes N64 axis PERM[k]
NATURAL_CODE = 'ns'       # Spice Mine Run's own code (kept for callers); every track: natural_code(name)

HERE = os.path.dirname(os.path.abspath(__file__))
N64 = os.path.normpath(os.path.join(HERE, '..', '..'))


# Own course codes of N64 tracks (add-on course codes, DLC DESIGN.md step B): fixed per track so that two
# tracks never share a code. Agreed: ns, nb (dlc-out.md 5.1; main session decision 2026-11-15), nv
# (n2r-ven.md); nu, nr, nc (n2r-bub.md, n2r-scr.md, n2r-bar.md). A track not in the table gets the provisional initials rule below. None may equal a stock
# code (ta tb tc rc ma mb mc ra ga gb gc sa sc, also sb).
NATURAL_CODES = {
    'spice mine run': 'ns',
    "beedo's wild ride": 'nb',
    'vengeance': 'nv',
    "bumpy's breakers": 'nu',
    "scrapper's run": 'nr',
    'baroo coast': 'nc',
    'dug derby': 'nd',
    'malastare 100': 'nm',
}


def natural_code(name):
    """own two-letter code of an N64 track: NATURAL_CODES, else (provisional) the initials of its first two
    words; the silo packs a track under a Revenge code with --code-as."""
    fixed = NATURAL_CODES.get(name.strip().lower())
    if fixed:
        return fixed
    w = [x for x in ''.join(ch if ch.isalnum() or ch == ' ' else ' ' for ch in name.lower()).split() if x != 's']
    return (w[0][0] + (w[1][0] if len(w) > 1 else w[0][1])) if w else 'xx'


def constants():
    return dict(scale=SCALE, perm=list(PERM), min_corner=MIN_CORNER, base_y=BASE_Y)


def transform_for(track_json):
    bb = json.load(open(track_json))['bbox_visual']
    lo = np.array(bb[0])
    off = [MIN_CORNER - SCALE * lo[0], BASE_Y, MIN_CORNER - SCALE * lo[1]]
    return dict(scale=SCALE, perm=list(PERM), offset=off,
                formula='revenge = (S*x + ox, S*z + oy, S*y + oz), S = %g, vertex order reversed' % SCALE)


def points(P, X):
    P = np.asarray(P, np.float64)
    return P[..., list(X['perm'])] * X['scale'] + np.array(X['offset'])


def dirs(D, X):
    """directions / normals (no offset, no scale)."""
    return np.asarray(D, np.float64)[..., list(X['perm'])]


def inverse_dirs(D, X):
    D = np.asarray(D, np.float64)
    out = np.empty_like(D)
    for k, p in enumerate(X['perm']):
        out[..., p] = D[..., k]
    return out


def winding():
    """vertex order applied to every N64 triangle (visual and collision)."""
    return [0, 2, 1]
