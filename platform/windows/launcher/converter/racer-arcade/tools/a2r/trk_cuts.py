r"""Cut-camera (intro) shot count and the generated extra shots, shared by every track converter
(arcade trk_convert.py, N64 racer-n64\tools\n2r\n2r_convert.py). One rule for every target code.

How many shots: the original intro plays the course's cut cameras by number. The numbers come from
the course's own intro script audio\scrptLns.ini ([LineN] CameraNumber = a list; read by the intro
director with [CSCameras] NumberOfCameras from the .tob, port course_cut_identity50ci.h /
original_ta_intro_authoring_candidate.h). Stock scripts: TA 1-3, SA 1-2, MA 1,2,4, GA 1,3, RA 1-7,
MB 1,3, GB 1,3, SC 1-3, GC 1-3, RC 1-5, TB 1-5, TC 1-3, MC 1-2 (measured, intro_cameras()).
A converted track packed under a stock code borrows that code's intro script (the port still takes
the intro event offsets from the stock row), so it must carry at least as many cut cameras as the
highest number the script names; otherwise a line points at a camera that does not exist.
Port cap: 1..7 cuts (course_cut_identity50ci.h:75).

The extra shots (4..N) are GENERATED [G], made like the existing generated shots (a2r cut 2: a
camera travelling beside the track, looking ahead along it) and placed the way the originals place
their later cuts (measured on the 57 stock .vub files: later cuts show other parts of the lap, eye
50-400 ft above the track, 80-400 frames, FOV 45-80): a raised travelling shot on the outside of a
bend, spread evenly over the middle of the lap. The converter's own accept() test decides whether a
candidate shot is usable (clear line of sight, inside one zone, ...); a refused candidate moves
along the lap and shortens its travel until one passes.
"""
import os
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import trk_ini  # noqa: E402
import trk_res3  # noqa: E402
import trk_vub  # noqa: E402

REVENGE = (__import__('os').environ['RR_CONVERT_STOCK']+'/TRACKS/')
STOCK = ('ta', 'tb', 'tc', 'rc', 'ma', 'mb', 'mc', 'ra', 'ga', 'gb', 'gc', 'sa', 'sc')
BS = chr(92)
MAX_CUTS = 7               # port: cut count 1..7 (course_cut_identity50ci.h:75)

EXTRA_FRAMES = 180         # 6 s at 30 frames/s (originals' later cuts: 61-410 frames)
# eye placements tried in order: (ft outside the track edge on the outside of the bend, ft above the
# track); the first is the open-air shot, the lower/inner ones are for covered tracks (tunnels)
EYE_PLACES = ((60.0, 120.0), (20.0, 60.0), (-30.0, 30.0))
EXTRA_TRAVELS = (400.0, 250.0, 120.0)   # eye travel along the track, tried in this order
EXTRA_LOOK_FT = 300.0      # look at the track this far ahead of the eye's track point
EXTRA_FOV = 60.0           # vertical degrees (originals 45-80)
LAP_SPAN = (0.15, 0.85)    # extra shots spread over this part of the lap (shots 1-3 show the start)
# Enclosed tracks (tubes, tunnels): when no placement above passes, a last stage flies the eye along
# the racing line itself at chase height (the generated behind-the-grid shot 3 uses 18 ft) and asks the
# converter's covered accept() (same tests, eye clearance COVERED_EYE_CLEAR_FT = the lane tooling's
# wall clearance). Only reached when every open placement failed, so tracks that passed before give
# the same shots byte for byte. [G]
COVERED_UP_FT = 20.0
COVERED_EYE_CLEAR_FT = 15.0
COVERED_LOOKS = (300.0, 150.0)


def intro_cameras(code):
    """Camera numbers the stock intro script of CODE names, sorted; [] for a non-stock code."""
    if code.lower() not in STOCK:
        return []
    r = trk_res3.read_res3(open(REVENGE + code.upper() + '.RES', 'rb').read())
    nm = [n for n in r.names() if n.lower() == 'audio' + BS + 'scrptlns.ini']
    if not nm:
        return []
    cams = set()
    for sec, kv in trk_ini.parse(r.get(nm[0])).sections():
        for k, v in kv:
            if k.lower() == 'cameranumber':
                cams |= {int(x) for x in v.replace(' ', '').split(',') if x}
    return sorted(cams)


def cut_count(code, base, borrows_intro):
    """(number of cut cameras to write, the camera numbers the intro script names).
    base = the converter's own shots; borrows_intro = the archive carries CODE's stock intro script."""
    cams = intro_cameras(code) if borrows_intro else []
    n = max([base] + cams)
    if n > MAX_CUTS:
        raise ValueError('intro script of %s names camera %d > the port cap %d' % (code, n, MAX_CUTS))
    return n, cams


class Loop:
    """The main lap as a closed polyline of spline nodes, sampled by arc length."""

    def __init__(self, P, R, left, right):
        self.P = np.asarray(P, np.float64)
        self.R = np.asarray(R, np.float64)
        self.left = np.asarray(left, np.float64); self.right = np.asarray(right, np.float64)
        seg = np.linalg.norm(np.roll(self.P, -1, 0) - self.P, axis=1)
        self.arc = np.r_[0.0, np.cumsum(seg)]
        self.L = float(self.arc[-1])

    def _at(self, s):
        s = s % self.L
        j = int(np.searchsorted(self.arc, s, side='right') - 1)
        j = min(j, len(self.P) - 1)
        k = (j + 1) % len(self.P)
        t = (s - self.arc[j]) / max(self.arc[j + 1] - self.arc[j], 1e-9)
        return j, k, t

    def point(self, s):
        j, k, t = self._at(s)
        return self.P[j] + (self.P[k] - self.P[j]) * t

    def right_vec(self, s):
        j, k, t = self._at(s)
        r = self.R[j] * (1 - t) + self.R[k] * t
        return r / max(np.linalg.norm(r), 1e-9)

    def half(self, s, side):
        j, k, t = self._at(s)
        w = self.right if side > 0 else self.left
        return float(w[j] * (1 - t) + w[k] * t)

    def outside(self, s):
        """+1 when the bend at s turns left (outside = right), -1 when it turns right."""
        a = self.point(s) - self.point(s - 200.0); b = self.point(s + 200.0) - self.point(s)
        turn = a[0] * b[2] - a[2] * b[0]
        # Revenge right vector is (fz, 0, -fx): a positive cross (x-z) turn bends toward -right
        return 1.0 if turn > 0 else -1.0


def shot(loop, s0, travel, place=EYE_PLACES[0], frames=EXTRA_FRAMES, look_ft=EXTRA_LOOK_FT):
    """One raised travelling shot starting at arc s0: [(look, up, pos, fov)], eyes, targets.
    place = (ft outside the edge, ft up), or (None, ft up) for an eye on the racing line itself."""
    side_ft, up_ft = place
    side = loop.outside(s0 + travel / 2)
    keys, eyes, tgts = [], [], []
    for i in range(frames):
        t = i / (frames - 1)
        s = s0 + travel * t
        lateral = 0.0 if side_ft is None else max(0.0, loop.half(s, side) + side_ft)
        eye = loop.point(s) + loop.right_vec(s) * side * lateral + np.array([0.0, up_ft, 0.0])
        tgt = loop.point(s + look_ft) + np.array([0.0, 10.0, 0.0])
        keys.append(trk_vub.look_up(eye, tgt) + (tuple(eye), EXTRA_FOV))
        eyes.append(eye); tgts.append(tgt)
    return keys, np.array(eyes), np.array(tgts), side


def extra_shots(loop, count, accept, log=None, accept_covered=None):
    """count generated shots for the cut numbers after the converter's own ones.
    accept(eyes (n,3), targets (n,3)) -> True when the shot is usable (converter's own checks);
    accept_covered: the same checks with the covered eye clearance, for the on-the-line stage.
    Returns [keys]; raises when no candidate passes (nothing is written then)."""
    out = []
    a, b = LAP_SPAN
    span = (b - a) * loop.L
    for i in range(count):
        centre = loop.L * a + span * (i + 0.5) / count
        reach = 0.5 * span / count
        offsets = [0.0]
        step = 150.0
        k = 1
        while k * step <= reach:
            offsets += [k * step, -k * step]
            k += 1
        found = None
        for place in EYE_PLACES:
            for d in offsets:
                for travel in EXTRA_TRAVELS:
                    s0 = centre + d - travel / 2
                    keys, eyes, tgts, side = shot(loop, s0, travel, place)
                    if accept(eyes, tgts):
                        found = (keys, s0, travel, side, place)
                        break
                if found:
                    break
            if found:
                break
        if not found and accept_covered is not None:
            for look in COVERED_LOOKS:
                for d in offsets:
                    for travel in EXTRA_TRAVELS:
                        s0 = centre + d - travel / 2
                        keys, eyes, tgts, side = shot(loop, s0, travel, (None, COVERED_UP_FT), look_ft=look)
                        if accept_covered(eyes, tgts):
                            found = (keys, s0, travel, side, (None, COVERED_UP_FT))
                            break
                    if found:
                        break
                if found:
                    break
        if not found:
            raise ValueError('generated cut %d: no clear shot within %.0f ft of lap position %.0f ft' % (i + 1, reach, centre))
        keys, s0, travel, side, place = found
        out.append(keys)
        if log is not None:
            log.append(dict(lap_ft=round(float(s0 % loop.L), 1), travel_ft=travel, side='right' if side > 0 else 'left',
                            outside_edge_ft=place[0] if place[0] is not None else 'on the racing line (covered stage)', up_ft=place[1],
                            frames=len(keys), eye_start=[round(float(x), 1) for x in keys[0][2]]))
    return out


def seg_hits_tris(p, q, T):
    """True where the segment p->q crosses triangle T (Moller-Trumbore)."""
    d = q - p
    e1 = T[:, 1] - T[:, 0]; e2 = T[:, 2] - T[:, 0]
    pv = np.cross(d, e2); det = (e1 * pv).sum(1)
    with np.errstate(all='ignore'):
        inv = 1.0 / det
        tv = p - T[:, 0]
        u = (tv * pv).sum(1) * inv
        qv = np.cross(tv, e1)
        v = (qv * d).sum(1) * inv
        t = (qv * e2).sum(1) * inv
    return (np.abs(det) > 1e-9) & (u >= 0) & (v >= 0) & (u + v <= 1) & (t >= 0) & (t <= 1)


def point_tri_dist(p, T):
    """Distance from point p to each triangle of T (n,3,3)."""
    a, b, c = T[:, 0], T[:, 1], T[:, 2]
    ab, ac, ap = b - a, c - a, p - a
    n = np.cross(ab, ac)
    nn = (n * n).sum(1)
    with np.errstate(all='ignore'):
        t = (ap * n).sum(1) / nn
        q = p - n * t[:, None]                       # projection on the plane
        # barycentric of q
        v0, v1, v2 = ab, ac, q - a
        d00 = (v0 * v0).sum(1); d01 = (v0 * v1).sum(1); d11 = (v1 * v1).sum(1)
        d20 = (v2 * v0).sum(1); d21 = (v2 * v1).sum(1)
        den = d00 * d11 - d01 * d01
        v = (d11 * d20 - d01 * d21) / den; w = (d00 * d21 - d01 * d20) / den
    inside = (v >= 0) & (w >= 0) & (v + w <= 1) & (nn > 1e-12)
    out = np.where(inside, np.abs(t) * np.sqrt(nn), np.inf)

    def seg(x, y):
        d = y - x; L = np.maximum((d * d).sum(1), 1e-12)
        u = np.clip(((p - x) * d).sum(1) / L, 0, 1)
        return np.linalg.norm(x + d * u[:, None] - p, axis=1)
    return np.minimum(out, np.minimum(seg(a, b), np.minimum(seg(b, c), seg(c, a))))


def clear_view(T, samples=12, eye_clear_ft=60.0):
    """A standard accept() part: from every sampled frame the eye sees its target past every
    triangle of T (n,3,3), and no triangle comes within eye_clear_ft of the eye."""
    T = np.asarray(T, np.float64)
    cen = T.mean(1)
    rad = np.linalg.norm(T - cen[:, None], axis=2).max(1)

    def ok(eyes, tgts):
        for i in np.linspace(0, len(eyes) - 1, samples).astype(int):
            e, g = eyes[i], tgts[i]
            mid = (e + g) / 2; r = np.linalg.norm(e - g) / 2
            ids = np.flatnonzero(np.linalg.norm(cen - mid, axis=1) < r + rad)
            if len(ids) and seg_hits_tris(e, g, T[ids]).any():
                return False
            near = np.flatnonzero(np.linalg.norm(cen - e, axis=1) < eye_clear_ft + rad)
            if len(near) and point_tri_dist(e, T[near]).min() < eye_clear_ft:
                return False
        return True
    return ok
