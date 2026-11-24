r"""Arcade -> Revenge track converter, track side (phase 1). One code path for every course.

    python -I trk_convert.py COURSE [--out DIR]       (run geo_course.py COURSE first)
    python -I trk_convert.py COURSE --code-as sc --dlc-out DIR [--dlc-disabled]
        also writes the DLC replacement package DIR\<package id>\ (trk_silo.package -> dlc_out.py)
        COURSE = arcade course id 0..4 (argument of the arcade loader f_0C142DDA)

Inputs (all arcade data, read-only):
  centre line, AI edge lines        notes\race-paths.txt          (tools\dump_race.py)
  grid, laps, AI speed table,
  AI racing-line curves             notes\race-tables.txt
  fog colour/distances, sun vector  notes\race-data.md section 15
  collision triangles + sectors     img\eprom.bin / img\mask.bin  via tools\export_course.py
  material ambient colours          export\course<N>\course.json
  visual PSG/PST/PSM                export\a2r\course<N>\ (geo_course.py)
Templates (Revenge originals, read-only): C:\Games\Star Wars Racer Revenge\TRACKS\*.RES

Output: export\a2r\<code>\  install\TRACKS\<CODE>.RES and install\UI\TEXTURES\LD_<CODE>.PST
(laid out as in the game folder), members\ (RES members loose), source\ (.scn/.toy text),
menu\ (LOC strings), manifest.json (per file and field: A arcade, G generated, D donor copy).

Space transform and course codes: a2r_space.py (the only definition).
"""
import argparse
import json
import math
import os
import re
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import trk_ini  # noqa: E402
import trk_binparam  # noqa: E402
import trk_col  # noqa: E402
import trk_res3  # noqa: E402
import trk_vub  # noqa: E402
import a2r_space  # noqa: E402  (the one transform / course-code definition)
import trk_addon  # noqa: E402  (members an add-on course carries itself)

# arcade STAT (wall polygon set) header per course: literal pool of f_0C142D98 @0C142E34
STAT_HEADERS = {0: 0x0C3C2B64, 1: 0x0C3E4AA8, 3: 0x0C3E0924}
# arcade per-course NPC lane routines (race-data.md 13; routine pointers in NPC +0x560/+0x564): segment range
# (lo, hi] in which the NPC stops following the racing-line curve and holds lateral 0.1 (when its lateral
# < 0.5) or 0.9 between the AI edge lists. Course 1 = f_0C13C1B8: words @0C13C232 160 / @0C13C230 216,
# 0.5 @0C13C238, 0.1 @0C13C248, 0.9 @0C13C24C. Converted as a two-lane fork of the AI spline.
AI_LANE_SPLITS = {1: (160, 216, (0.1, 0.9))}
# [Fog] Visibility / Haziness per course where the user has decided against distance fog
# (2026-11-13: "Lose the volumetric distance fog on Etti IV"). Port rule: fog end = min + Visibility *
# (max - min), start = (end - 64) * (1 - Haziness); 1 / 0 = fog only in the last 64 ft of the view.
FOG_FRACTIONS = {1: ('1.0', '0.0')}

BS = chr(92)
REVENGE = (__import__('os').environ['RR_CONVERT_STOCK']+'/TRACKS/')

# ------------------------------------------------------------------ course naming (data)
# Codes: conversion-spec.md 2.1. eb/lc are the spec's choice; ua/td/ka provisional.
COURSES = {
    0: dict(code='ua', name='Tatooine Bantha Tracks', provisional_code=True),
    1: dict(code='eb', name="Etti IV Smuggler's Cove", provisional_code=False),
    2: dict(code='lc', name='Malastare Pixelito Challenge', provisional_code=False),
    3: dict(code='td', name='Tatooine Boonta Classic', provisional_code=True),
    4: dict(code='ka', name='Kaadu oval (not a race in the arcade)', provisional_code=True),
}


def physics_class(code):
    """Original 214954 rule (conversion-spec 2.1): 'a' anywhere -> A, else 'b' -> B, else C;
    exact codes sb -> C, ra/sc -> B."""
    c = 'A' if 'a' in code else 'B' if 'b' in code else 'C'
    if code == 'sb':
        c = 'C'
    if code in ('ra', 'sc'):
        c = 'B'
    return c


DONOR_BY_CLASS = {'A': 'TA', 'B': 'TB', 'C': 'TC'}   # conversion-spec 1.4 (.tun)

# ------------------------------------------------------------------ generated-rule constants
AI_TRACKWIDTH = 100.0      # most common original AI TrackWidth (9 of 13)
AI_WALL_CLEAR_FT = 20.0    # racing line distance from the drivable-floor edge (pod 15 ft clearance + margin)
AI_MIN_HALF = 30.0         # originals: smallest effective AI half-width ~20 ft
MAP_TRACKWIDTH = 200.0     # original map TrackWidth 200-300
# (ZONE_LENGTH_FT retired: zones are slabs on the geometry grid, trk_slabs.py)
NODE_MAX_SPACING = 400.0   # originals: median node spacing 190-470 ft
NODE_MIN_SPACING = 60.0
NODE_TOLERANCE = 6.0       # max deviation of the arcade centre line from a node chord, ft
MAX_NODES = 250            # port track readers hold 256 nodes, 64 KB text
PROGRESS_FRACTIONS = (0.02, 0.25, 0.5, 0.75, 0.97)   # mta: 0.02 0.13 0.61 0.82 0.98
MAP_MARGIN_FT = 30.0
GRID_CLEAR_FT = 300.0      # start grid reaches 200 ft behind map node 0 (3 rows, 100 ft apart)
GRID_PRED_FT = 250.0       # the map node before node 0 (Next0 = 0) lies at least this far behind node 0: the original grid
                           # 2A2880 puts rows 0 / 100 / 200 ft behind node 0 and the AI lap scan credits a lap to a racer that
                           # starts behind that node's gate (stock 209..918; cr16-fix-load.txt fault 10)
GRID_PRED_MIN_FT = 200.0   # writer check: refused at or below this
TV_SPACING_FT = 1800.0     # TA: 19 TV cameras on a 33k lap


def f32(x):
    return struct.unpack('<f', struct.pack('<f', float(x)))[0]


def vec(v, fmt='%f'):
    return ','.join(fmt % f32(c) for c in v)


# ------------------------------------------------------------------ arcade inputs
def parse_race_paths():
    txt = open(os.path.join(SCR, 'notes', 'race-paths.txt')).read().split('\n')
    centre, guide = {}, {}
    cur = None
    for ln in txt:
        m = re.match(r'## centre (\d+) ', ln)
        g = re.match(r'## guide (\d+) ', ln)
        if m:
            cur = ('c', int(m.group(1))); centre[cur[1]] = []; continue
        if g:
            cur = ('g', int(g.group(1))); guide[cur[1]] = []; continue
        if ln.startswith('#') or not ln.strip():
            cur = None if ln.startswith('#') else cur
            continue
        if cur:
            p = [float(x) for x in ln.split()[1:]]
            (centre if cur[0] == 'c' else guide)[cur[1]].append(p)
    return ({k: np.array(v) for k, v in centre.items()}, {k: np.array(v) for k, v in guide.items()})


def parse_race_tables():
    txt = open(os.path.join(SCR, 'notes', 'race-tables.txt')).read()
    grids = {}
    for m in re.finditer(r'grid (\d+) @[0-9A-F]+:\n((?:\s+pos.*\n)+)', txt):
        recs = []
        for r in re.finditer(r'kind\s+(\d+) (\w+) AI level (\d) +xyz \(\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)\) yaw (-?\d+)', m.group(2)):
            recs.append(dict(kind=int(r.group(1)), pilot=r.group(2), level=int(r.group(3)),
                             pos=(float(r.group(4)), float(r.group(5)), float(r.group(6))), yaw=int(r.group(7))))
        grids[int(m.group(1))] = recs
    laps = {}
    m = re.search(r'## Laps .*: (.*)', txt)
    names = ['SA_COURSE2', 'ETTI4', 'MALASTARE', 'SA_COURSE4']
    for i, nm in enumerate(names):
        mm = re.search(nm + r'=(\d+)', m.group(1))
        laps[i] = int(mm.group(1))
    speeds = {}
    sec = txt[txt.index('## NPC speed table'):]
    for i, nm in enumerate(names):
        mm = re.search('\n' + nm + r': ([\d. ]+)', sec)
        speeds[i] = [float(x) for x in mm.group(1).split()]
    return grids, laps, speeds


def parse_racing_lines():
    """race-tables.txt 'NPC per-course (node, value) tables' = racing-line curves (segment, lateral
    0..1; 0 = left guide list, 1 = right), course order 0..3 (race-data.md 13)."""
    txt = open(os.path.join(SCR, 'notes', 'race-tables.txt')).read()
    out = {}
    for i, nm in enumerate(['SA_COURSE2', 'ETTI4', 'MALASTARE', 'SA_COURSE4']):
        m = re.search('\n' + nm + r'\?: ((?:\([\d.]+,[\d.]+\) ?)+)', txt)
        if m:
            out[i] = [tuple(float(x) for x in pr.split(',')) for pr in re.findall(r'\(([\d.]+,[\d.]+)\)', m.group(1))]
    return out


def parse_hooks():
    """race-data.md 15 table: per course fog RGB, fog near, fog far, light vector (or None)."""
    txt = open(os.path.join(SCR, 'notes', 'race-data.md')).read()
    out = {}
    for m in re.finditer(r'\| (\d) \w+ \| (\d+),(\d+),(\d+)[^|]*\| (\d+) \| (\d+) \| ([^|]*)\|', txt):
        v = re.search(r'@[0-9A-F]+ \(([-\d.]+),([-\d.]+),([-\d.]+)\)', m.group(7))
        out[int(m.group(1))] = dict(fog_rgb=tuple(int(m.group(k)) for k in (2, 3, 4)), fog_near=float(m.group(5)),
                                    fog_far=float(m.group(6)), light=tuple(float(x) for x in v.groups()) if v and 'position' not in m.group(7) else None,
                                    light_text=m.group(7).strip())
    return out


def parse_lights():
    """race-data.md 20: per course directional lights [(vector, colour)] and point lights
    [(x, y, z)] (Malastare), plus the point light colour."""
    txt = open(os.path.join(SCR, 'notes', 'race-data.md')).read()
    sec = txt[txt.index('## 20.'):txt.index('## 21.')]
    dirs = {}

    def triple(t):
        v = [float(x) for x in re.findall(r'[-\d.]+', t)]
        return tuple(v) if len(v) == 3 else (v[0],) * 3
    for m in re.finditer(r'^\| (\d) \| ([^|]+)\| (dir|point) \| ([^|]+)\| ([^|]+)\|', sec, re.M):
        c, kind, vec_t, col_t = int(m.group(1)), m.group(3), m.group(4), m.group(5)
        if kind != 'dir':
            continue
        vecs = [triple(x) for x in vec_t.split(' / ')]
        if 'as course' in col_t:
            cols = [d[1] for d in dirs[int(re.search(r'course (\d)', col_t).group(1))]]
        else:
            cols = [triple(x) for x in col_t.split(' / ')]
        dirs.setdefault(c, []).extend(zip(vecs, cols))
    pts = [tuple(float(x) for x in mm.groups()) for mm in
           re.finditer(r'\d+\s+\(([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)\)', sec[sec.index('Malastare point lights'):sec.index('Per frame')])]
    return dirs, {2: pts}


def arcade_collision(cid):
    """-> tris (M,3,3) arcade units, surface (M,), face normal (M,3), sectors [(quad(5,2), tris)]."""
    cwd = os.getcwd()
    sys.path.insert(0, os.path.join(SCR, 'tools'))
    import export_course as EC   # loads img\ (read-only)
    os.chdir(cwd)
    info, R, F = EC.collision(cid)
    n = len(R)
    tris = F[:, 4:13].reshape(n, 3, 3).astype(np.float64)
    surf = (R[:, 48] & 0xFFFF).astype(np.int64)
    fn = F[:, 13:16].astype(np.float64)
    blk, root, size = EC.w(0x0C3E4ACC + 12 * cid, 3)
    rom = EC.rom
    nsec, arr = rom.words(root, 4)[:2]
    sectors = []
    for s in range(nsec):
        q = np.array(rom.floats(arr + 44 * s, 10)).reshape(5, 2)
        p = rom.words(arr + 44 * s + 40, 1)[0]
        lst = []
        while True:
            v = struct.unpack_from('<i', *rom.locate(p))[0]
            if v < 0:
                break
            lst.append(v); p += 4
        sectors.append((q, np.array(lst, np.int64)))
    return info, tris, surf, fn, sectors


# ------------------------------------------------------------------ geometry helpers
def point_in_quad(q, x, z):
    """q (5,2) closed polygon."""
    inside = False
    for i in range(4):
        x1, z1 = q[i]; x2, z2 = q[i + 1]
        if (z1 > z) != (z2 > z):
            xi = x1 + (z - z1) * (x2 - x1) / (z2 - z1)
            if x < xi:
                inside = not inside
    return inside


def heights_under(tris, x, z):
    A, B, C = tris[:, 0], tris[:, 1], tris[:, 2]
    d = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])
    with np.errstate(all='ignore'):
        l0 = ((B[:, 2] - C[:, 2]) * (x - C[:, 0]) + (C[:, 0] - B[:, 0]) * (z - C[:, 2])) / d
        l1 = ((C[:, 2] - A[:, 2]) * (x - C[:, 0]) + (A[:, 0] - C[:, 0]) * (z - C[:, 2])) / d
    l2 = 1 - l0 - l1
    ok = (l0 >= -1e-4) & (l1 >= -1e-4) & (l2 >= -1e-4) & (np.abs(d) > 1e-9)
    return l0[ok] * A[ok, 1] + l1[ok] * B[ok, 1] + l2[ok] * C[ok, 1]


def right_of(fwd):
    """Revenge gate side rule (track_navigation_shared50mb.inl gates(), rotateVector49U):
    RightTrackWidth goes along (fz, 0, -fx) of the horizontal forward vector."""
    f = np.array([fwd[0], 0.0, fwd[2]]); f /= np.linalg.norm(f)
    return np.array([f[2], 0.0, -f[0]])


# ------------------------------------------------------------------ main conversion
class Conv:
    def __init__(self, course, out_root, code_as=None, phy_class=None):
        self.course = course
        self.cfg = COURSES[course]
        self.natural_code = a2r_space.CODES[course]
        self.code = a2r_space.course_code(course, code_as)     # the one override parameter
        self.override = self.code != self.natural_code
        # PHY class: the original's code-letter rule for a stock code; an add-on course (own code) may name
        # its class explicitly (dlc.json phy_class, DESIGN.md step B note); it picks the .tun donor (trk_addon)
        if phy_class and not trk_addon.is_addon(self.code):
            raise ValueError('--phy-class applies to add-on codes only; stock code %s keeps the code-letter rule' % self.code)
        self.cls = (phy_class or physics_class(self.code)).upper()
        self.XF = a2r_space.course_transform(course)
        self.out = os.path.join(out_root, self.code)
        self.files = {}          # member name -> bytes
        self.prov = {}           # member name -> provenance note
        self.flags = []          # field-level provenance lines
        self.report = {}

    # -------------------------------------------------- transform (a2r_space, nothing local)
    def X(self, p):
        return a2r_space.points(p, self.XF)

    def X2(self, q):
        return a2r_space.xz(q, self.XF)

    def put(self, name, data, prov):
        self.files[name] = data
        self.prov[name] = prov

    # -------------------------------------------------- run
    def run(self):
        centre_all, guide_all = parse_race_paths()
        grids, laps, speeds = parse_race_tables()
        info, tris_a, surf, fn_a, sectors = arcade_collision(self.course)
        tris = self.X(tris_a)
        fn = a2r_space.normals(fn_a, self.XF)
        self.tris, self.surf = tris, surf
        nsec = len(sectors)
        secq = [self.X2(q) for q, _ in sectors]
        sect_tris = [t for _, t in sectors]
        self.secq = secq
        floor = surf != 0x8000

        # ---- centre line (closed loop), ground height from the sector's floor triangles
        c_a = centre_all[self.course]
        n = len(c_a)
        cxz = self.X2(c_a)
        sec_of = np.zeros(n, np.int64)
        for k in range(n):
            exp = k * nsec / n
            cand = [s for s in range(nsec) if point_in_quad(secq[s], *cxz[k])]
            if not cand:   # outside every sector quad: nearest sector centre
                cen = np.array([q[:4].mean(0) for q in secq])
                cand = [int(np.argmin(np.linalg.norm(cen - cxz[k], axis=1)))]
            sec_of[k] = min(cand, key=lambda s: min(abs(s - exp), nsec - abs(s - exp)))
        cy = np.zeros(n)
        for k in range(n):
            s = sec_of[k]
            ids = np.concatenate([sect_tris[(s + d) % nsec] for d in (-1, 0, 1)])
            ids = ids[floor[ids]]
            h = heights_under(tris[ids], *cxz[k])
            if len(h) == 0:
                h = heights_under(tris[floor], *cxz[k])
            cy[k] = h.max() if len(h) else np.nan
        bad = np.isnan(cy)
        if bad.any():
            idx = np.arange(n)
            cy[bad] = np.interp(idx[bad], idx[~bad], cy[~bad], period=n)
        C = np.stack([cxz[:, 0], cy, cxz[:, 1]], 1)
        seg = np.linalg.norm(np.roll(C, -1, 0) - C, axis=1)
        arc = np.concatenate([[0], np.cumsum(seg)])           # arc[n] = loop length
        L = arc[-1]
        fwd = np.roll(C, -1, 0) - np.roll(C, 1, 0)
        R = np.array([right_of(f) for f in fwd])

        # ---- start: map node 0 = centre point at the front row of the arcade grid
        grid = grids.get(self.course)
        start = 0
        if grid:
            gxz = self.X2(np.array([[g['pos'][0], g['pos'][2]] for g in grid]))
            # arc position of each grid record = its nearest centre point (first quarter of the lap)
            near = [int(np.argmin(np.linalg.norm(cxz[: n // 4] - p, axis=1))) for p in gxz]
            start = max(near)
            self.flags.append('start node: arcade centre point %d (front row of arcade grid %d) [A]' % (start, self.course))
        else:
            self.flags.append('start node: arcade centre point 0 (course has no arcade grid) [A]')
        order = [(start + i) % n for i in range(n)]        # arcade indices in Revenge order

        # ---- AI racing line at every arcade centre-line point (race-data.md 13), kept AI_WALL_CLEAR_FT
        # inside the drivable floor (the generated walls stand on its edges)
        guide0 = guide_all.get(self.course)
        curves0 = parse_racing_lines()
        RL = cxz.copy()
        rl_shifted = 0
        if guide0 is not None and self.course in curves0:
            gl0 = self.X2(guide0[:n, 0:2]); gr0 = self.X2(guide0[:n, 2:4])
            bp0 = np.array(curves0[self.course])
            lat0 = np.interp(np.arange(n, dtype=float), bp0[:, 0], bp0[:, 1])
            for k in range(n):
                q = gl0[k] + (gr0[k] - gl0[k]) * lat0[k]
                ids = np.concatenate([sect_tris[(sec_of[k] + d_) % nsec] for d_ in (-1, 0, 1)])
                ids = ids[floor[ids]]
                Tq = tris[ids]; Aq = Tq; Bq = np.roll(Tq, -1, axis=1)
                fq = np.array([-R[k][2], 0.0, R[k][0]]); q3 = np.array([q[0], C[k][1], q[1]])
                da = (Aq - q3) @ fq; db = (Bq - q3) @ fq
                with np.errstate(all='ignore'):
                    tt = np.where(np.abs(da - db) > 1e-9, da / (da - db), 0.5)
                Pt = Aq + (Bq - Aq) * tt[..., None]
                okq = ((da * db) <= 0) & (np.abs(Pt[..., 1] - C[k][1]) < 300.0)
                if okq.any():
                    latq = (Pt - q3) @ R[k]
                    lo_q, hi_q = latq[okq].min(), latq[okq].max()
                    shift = 0.0
                    if hi_q - lo_q < 2 * AI_WALL_CLEAR_FT:
                        shift = (lo_q + hi_q) / 2
                    elif hi_q < AI_WALL_CLEAR_FT:
                        shift = hi_q - AI_WALL_CLEAR_FT
                    elif lo_q > -AI_WALL_CLEAR_FT:
                        shift = lo_q + AI_WALL_CLEAR_FT
                    if shift:
                        q = q + shift * R[k][[0, 2]]; rl_shifted += 1
                RL[k] = q

        # ---- node selection (shared by AI and map splines); chords follow the centre line AND the
        # racing line within the tolerance
        tol = NODE_TOLERANCE
        maxsp = NODE_MAX_SPACING
        while True:
            sel = [0]
            last = 0
            for i in range(1, n):
                a = C[order[last]]; b = C[order[i]]
                chord = np.linalg.norm((b - a)[[0, 2]])
                d_arc = (arc[order[i]] - arc[order[last]]) % L
                dev = 0.0
                if i - last > 1 and chord > 0:
                    u = (b - a)[[0, 2]] / chord
                    for j in range(last + 1, i):
                        w = (C[order[j]] - a)[[0, 2]]
                        dev = max(dev, abs(w[0] * u[1] - w[1] * u[0]))
                    ra, rb = RL[order[last]], RL[order[i]]
                    rc = np.linalg.norm(rb - ra)
                    if rc > 0:
                        ru = (rb - ra) / rc
                        for j in range(last + 1, i):
                            w = RL[order[j]] - ra
                            dev = max(dev, abs(w[0] * ru[1] - w[1] * ru[0]))
                if d_arc >= NODE_MIN_SPACING and (dev > tol or d_arc > maxsp):
                    sel.append(i - 1 if i - 1 > last else i)
                    last = sel[-1]
            # the closing nodes: the node before node 0 must lie GRID_PRED_FT or more behind it (node 0 itself is not moved)
            while len(sel) > 1 and np.linalg.norm(C[order[sel[-1]]] - C[order[0]]) < GRID_PRED_FT:
                sel.pop()
            if len(sel) <= MAX_NODES:
                break
            tol *= 1.25
            maxsp *= 1.1     # a long lap needs wider spacing to stay within the node limit
        self.flags.append('nodes: %d chosen from %d arcade centre points, chord tolerance %.1f ft, max spacing %.0f ft [G]' % (len(sel), n, tol, maxsp))
        K = len(sel)
        ai_idx = [order[i] for i in sel]
        P = C[ai_idx]
        Rn = R[ai_idx]

        # ---- zones: see "zones and portals" below (they need the nodes, lane forks and cut camera paths)

        # ---- widths
        guide = guide_all.get(self.course)
        ai_right = np.zeros(K); ai_left = np.zeros(K)
        if guide is not None:
            gl = self.X2(guide[:n, 0:2]); gr = self.X2(guide[:n, 2:4])
            for j, k in enumerate(ai_idx):
                a = np.dot(np.r_[gl[k][0] - cxz[k][0], 0, gl[k][1] - cxz[k][1]], R[k])
                b = np.dot(np.r_[gr[k][0] - cxz[k][0], 0, gr[k][1] - cxz[k][1]], R[k])
                ai_right[j] = max(a, b, 1.0); ai_left[j] = max(-min(a, b), 1.0)
            ai_src = 'A (arcade AI edge lines)'
        # map widths from the collision cross-section (sector triangles near the node)
        map_right = np.zeros(K); map_left = np.zeros(K)
        for j, k in enumerate(ai_idx):
            # cross-section: intersect the triangle edges of the node's sector (and neighbours)
            # with the horizontal line through the node along R
            s = sec_of[k]
            ids = np.concatenate([sect_tris[(s + d) % nsec] for d in (-1, 0, 1)])
            T3 = tris[ids]
            A = T3; B = np.roll(T3, -1, axis=1)
            f = np.array([-R[k][2], 0, R[k][0]])
            da = ((A - C[k]) @ f); db = ((B - C[k]) @ f)
            cross = (da * db) <= 0
            with np.errstate(all='ignore'):
                t = da / (da - db)
            t = np.where(np.isfinite(t), t, 0.5)
            Pt = A + (B - A) * t[..., None]
            lat = (Pt - C[k]) @ R[k]
            dy = np.abs(Pt[..., 1] - C[k][1])
            ok = cross & (dy < 300.0)
            if ok.any():
                map_right[j] = max(lat[ok].max(), 1.0); map_left[j] = max(-lat[ok].min(), 1.0)
            else:
                map_right[j] = map_left[j] = 100.0
        if guide is None:   # no arcade AI corridor (course 4): AI corridor = 80 % of the walls
            ai_right = 0.8 * map_right; ai_left = 0.8 * map_left
            ai_src = 'G (80 % of the collision cross-section; the course has no arcade AI edge lines)'
        map_right += MAP_MARGIN_FT; map_left += MAP_MARGIN_FT
        # keep map gates from reaching another part of the course
        for j, k in enumerate(ai_idx):
            far = np.abs(((arc - arc[k] + L / 2) % L) - L / 2)[:n] > 1500.0
            if far.any():
                d = np.linalg.norm(cxz[far] - cxz[k], axis=1).min()
                map_right[j] = min(map_right[j], 0.5 * d); map_left[j] = min(map_left[j], 0.5 * d)
                ai_right[j] = min(ai_right[j], 0.5 * d); ai_left[j] = min(ai_left[j], 0.5 * d)
        self.flags.append('AI widths: ' + ai_src + '; map widths: G (collision cross-section + %.0f ft, clipped to half the distance to other track parts)' % MAP_MARGIN_FT)

        # ---- AI racing line (race-data.md 13): lateral 0..1 between the guide edge lists per segment
        curves = parse_racing_lines()
        PA = P.copy()
        if guide is not None and self.course in curves:
            bp = np.array(curves[self.course])
            seg_idx = np.arange(n, dtype=float)
            lateral = np.interp(seg_idx, bp[:, 0], bp[:, 1])   # breakpoints past n never reached (13)
            shifted = rl_shifted
            for j, k in enumerate(ai_idx):
                q = RL[k].copy()
                ids = np.concatenate([sect_tris[(sec_of[k] + d_) % nsec] for d_ in (-1, 0, 1)])
                ids = ids[floor[ids]]
                # keep the line AI_WALL_CLEAR_FT inside the drivable floor (the generated walls stand on
                # its edges): cross-section of the floor triangles along R through q
                Tq = tris[ids]; Aq = Tq; Bq = np.roll(Tq, -1, axis=1)
                fq = np.array([-R[k][2], 0.0, R[k][0]]); q3 = np.array([q[0], C[k][1], q[1]])
                da = (Aq - q3) @ fq; db = (Bq - q3) @ fq
                with np.errstate(all='ignore'):
                    tt = np.where(np.abs(da - db) > 1e-9, da / (da - db), 0.5)
                Pt = Aq + (Bq - Aq) * tt[..., None]
                okq = ((da * db) <= 0) & (np.abs(Pt[..., 1] - C[k][1]) < 300.0)
                if okq.any():
                    latq = (Pt - q3) @ R[k]
                    lo_q, hi_q = latq[okq].min(), latq[okq].max()
                    shift = 0.0
                    if hi_q - lo_q < 2 * AI_WALL_CLEAR_FT:
                        shift = (lo_q + hi_q) / 2
                    elif hi_q < AI_WALL_CLEAR_FT:
                        shift = hi_q - AI_WALL_CLEAR_FT
                    elif lo_q > -AI_WALL_CLEAR_FT:
                        shift = lo_q + AI_WALL_CLEAR_FT
                    if shift:
                        q = q + shift * R[k][[0, 2]]
                h = heights_under(tris[ids], *q)
                y = h[np.argmin(np.abs(h - C[k][1]))] if len(h) else C[k][1]
                PA[j] = (q[0], y, q[1])
                a = np.dot(np.r_[gl[k][0] - q[0], 0, gl[k][1] - q[1]], R[k])
                b = np.dot(np.r_[gr[k][0] - q[0], 0, gr[k][1] - q[1]], R[k])
                ai_right[j] = max(a, b, AI_MIN_HALF); ai_left[j] = max(-min(a, b), AI_MIN_HALF)
            self.flags.append('AI racing line: %d arcade line points moved inward to stay %.0f ft inside the drivable floor (walls stand on its edges) [G]' % (shifted, AI_WALL_CLEAR_FT))
            self.flags.append('AI gate half-widths: distance from the racing line to each AI edge, at least %.0f ft (the arcade line often runs on an edge) [A, G floor]' % AI_MIN_HALF)
            self.flags.append('AI spline positions: A-derived arcade racing line (curve @0C2F9260+ per course, lateral between the AI edge lists, race-data.md 13); widths to those edges [A]')
        else:
            self.flags.append('AI spline positions: centre line (no arcade racing-line curve for this course) [A centre, G]')

        # ---- two-lane fork where the arcade NPC routine holds one of two lanes (AI_LANE_SPLITS) [A]
        lane = None
        if guide is not None and self.course in AI_LANE_SPLITS:
            s_lo, s_hi, (l_a, l_b) = AI_LANE_SPLITS[self.course]
            js = [j for j, k in enumerate(ai_idx) if s_lo < k <= s_hi]
            if js and js[0] > 0 and js[-1] < K - 1 and js == list(range(js[0], js[-1] + 1)):
                def lane_pt(k, f_):
                    q = gl[k] + f_ * (gr[k] - gl[k])
                    ids = np.concatenate([sect_tris[(sec_of[k] + d_) % nsec] for d_ in (-1, 0, 1)])
                    h = heights_under(tris[ids[floor[ids]]], *q)
                    y = h[np.argmin(np.abs(h - C[k][1]))] if len(h) else C[k][1]
                    return np.array([q[0], y, q[1]])
                LA = np.array([lane_pt(ai_idx[j], l_a) for j in js])
                LB = np.array([lane_pt(ai_idx[j], l_b) for j in js])
                half = np.array([max(8.0, 0.5 * min(l_a, 1 - l_b) * np.linalg.norm(gr[ai_idx[j]] - gl[ai_idx[j]])) for j in js])
                lane = dict(js=js, A=LA, B=LB, half=half)
                PA[js] = LA
                ai_left[js] = half; ai_right[js] = half
                map_left[js] = half; map_right[js] = half
                self.flags.append('AI lanes: arcade NPC routine of course %d (segments %d..%d hold lateral %.1f or %.1f) -> '
                                  'AI and map spline fork: main nodes %d..%d on lane %.1f, branch 1 (%d nodes) on lane %.1f, '
                                  'rejoining at node %d; gate half-width = half the lane-to-edge distance [A routine, G fork layout]'
                                  % (self.course, s_lo, s_hi, l_a, l_b, js[0], js[-1], l_a, len(js), l_b, js[-1] + 1))
            else:
                self.flags.append('AI lanes: course %d lane range has no interior AI nodes; not forked [pending]' % self.course)

        # ---- distances, speed limits, progress nodes
        nxt = np.roll(np.arange(K), -1)
        seg3 = np.linalg.norm(P[nxt] - P, axis=1)
        dtf = np.zeros(K)
        acc = 0.0
        for j in range(K - 1, 0, -1):
            acc += seg3[j]
            dtf[j] = acc
        lap = seg3.sum()
        prv = np.roll(np.arange(K), 1)
        speed = np.zeros(K)
        for j in range(K):
            a = (P[j] - P[prv[j]])[[0, 2]]; b = (P[nxt[j]] - P[j])[[0, 2]]
            ang = math.acos(max(-1, min(1, a @ b / np.linalg.norm(a) / np.linalg.norm(b))))
            rad = 0.5 * (np.linalg.norm(a) + np.linalg.norm(b)) / max(ang, 1e-6)
            speed[j] = 300.0 if rad < 300 else 400.0 if rad < 500 else 600.0
        self.flags.append('SpeedLimit: G (600; 400 below 500 ft turn radius; 300 below 300 ft; original values are hand-set 200-1000)')
        sfrac = (lap - dtf) % lap / lap
        sfrac[0] = 0.0
        prog = sorted(set(int(np.argmin(np.abs(sfrac - f))) for f in PROGRESS_FRACTIONS) - {0})
        self.flags.append('ProgressNode: G at lap fractions %s -> nodes %s' % (PROGRESS_FRACTIONS, prog))

        def dist_to_finish(Q):
            sg = np.linalg.norm(Q[nxt] - Q, axis=1)
            out = np.zeros(K); acc_ = 0.0
            for j in range(K - 1, 0, -1):
                acc_ += sg[j]; out[j] = acc_
            return out

        def spline(kind):
            Q = PA if kind == 'a' else P
            if kind == 'm' and lane is not None:
                # the map spline forks around the same obstacle (originals fork both splines): main
                # nodes on lane A, branch on lane B, so resets and the map arrow never sit inside it
                Q = P.copy(); Q[lane['js']] = lane['A']
            dq = dist_to_finish(Q)
            tw = AI_TRACKWIDTH if kind == 'a' else MAP_TRACKWIDTH
            lw = (ai_left if kind == 'a' else map_left) - tw / 2
            rw = (ai_right if kind == 'a' else map_right) - tw / 2
            secs = [('Track', [('NumNodes', str(K)), ('TrackWidth', '%f' % tw)])]
            d_pred = [float(np.linalg.norm(Q[j] - Q[0])) for j in range(K) if int(nxt[j]) == 0]
            if kind == 'm' and (not d_pred or min(d_pred) <= GRID_PRED_MIN_FT):
                raise ValueError('%s spline: node before node 0 only %s ft behind it (need > %.0f; back grid rows would get lap 1 '
                                 'at once)' % (kind, [round(d, 1) for d in d_pred], GRID_PRED_MIN_FT))
            self.report.setdefault('grid_predecessor_ft', {})[kind] = [round(d, 1) for d in d_pred]
            for j in range(K):
                kv = [('Position', vec(Q[j])), ('LeftTrackWidth', '%f' % f32(lw[j])),
                      ('RightTrackWidth', '%f' % f32(rw[j])), ('SpeedLimit', '%f' % speed[j]),
                      ('DistanceToFinish', '%f' % f32(dq[j])), ('BranchID', '0')]
                if kind == 'm' and j in prog:
                    kv.append(('ProgressNode', '1'))
                if lane is not None and j == lane['js'][0] - 1:
                    kv += [('NumNextNodes', '2'), ('Next0', str(int(nxt[j]))), ('Next1', str(K))]
                else:
                    kv += [('NumNextNodes', '1'), ('Next0', str(int(nxt[j])))]
                secs.append(('Node%d' % j, kv))
            if lane is not None:
                LB, js = lane['B'], lane['js']
                rejoin = js[-1] + 1
                m_ = len(js)
                db = np.zeros(m_)
                acc_ = float(dq[rejoin]) + float(np.linalg.norm(Q[rejoin] - LB[-1]))
                for i_ in range(m_ - 1, -1, -1):
                    db[i_] = acc_
                    if i_:
                        acc_ += float(np.linalg.norm(LB[i_] - LB[i_ - 1]))
                for i_ in range(m_):
                    j = js[i_]
                    kv = [('Position', vec(LB[i_])), ('LeftTrackWidth', '%f' % f32(lane['half'][i_] - tw / 2)),
                          ('RightTrackWidth', '%f' % f32(lane['half'][i_] - tw / 2)), ('SpeedLimit', '%f' % speed[j]),
                          ('DistanceToFinish', '%f' % f32(db[i_])), ('BranchID', '1'),
                          ('NumNextNodes', '1'), ('Next0', str(K + i_ + 1 if i_ < m_ - 1 else rejoin))]
                    secs.append(('Node%d' % (K + i_), kv))
                secs[0] = ('Track', [('NumNodes', str(K + m_)), ('TrackWidth', '%f' % tw)])
            return trk_ini.write_canonical(secs)

        self.put('tracks' + BS + 'a%s.spl' % self.code, spline('a'), 'A racing line + AI edge lines; G node choice, speed; distances A-derived')
        self.put('tracks' + BS + 'm%s.spl' % self.code, spline('m'), 'A centre line; G widths from collision, progress nodes')

        # ---- cut camera paths (written to the .vub files below); computed here because the zone layout
        # keeps each of them inside one zone (in the port a cut camera keeps its InitialZone for the whole cut)
        def at(j, back=0.0, side=0.0, up=0.0):
            f = P[nxt[j]] - P[j]; f[1] = 0; f /= np.linalg.norm(f)
            return P[j] - f * back + Rn[j] * side + np.array([0, up, 0])

        gcen = P[0] - (P[0] - P[prv[0]]) / np.linalg.norm(P[0] - P[prv[0]]) * 100.0
        cams = []
        # 1: high flyover from behind the grid to above it, looking at the grid centre (300 frames)
        k1 = []
        for i in range(300):
            t = i / 299
            eye = at(0, back=900 * (1 - t) + 50, up=420 - 250 * t, side=120)
            k1.append(trk_vub.look_up(eye, gcen) + (tuple(eye), 60.0))
        cams.append(k1)
        # 2: low side shot travelling along the first nodes, looking ahead (250 frames)
        k2 = []
        j_end = max(1, min(K - 1, int(np.searchsorted((lap - dtf) % lap, 1200.0))))
        for i in range(250):
            t = i / 249
            jj = int(t * j_end)
            eye = at(jj, side=-(ai_left[jj] + 40), up=35)
            tgt = P[min(jj + 2, K - 1)] + np.array([0, 15, 0])
            k2.append(trk_vub.look_up(eye, tgt) + (tuple(eye), 54.0))
        cams.append(k2)
        # 3: behind the grid at pod height looking down the start straight (150 frames)
        k3 = []
        for i in range(150):
            t = i / 149
            eye = at(0, back=320 - 60 * t, up=18)
            tgt = P[min(3, K - 1)] + np.array([0, 10, 0])
            k3.append(trk_vub.look_up(eye, tgt) + (tuple(eye), 65.0))
        cams.append(k3)
        # 4..N: as many cuts as the borrowed intro script of the target code names (trk_cuts.py) [G]
        import trk_cuts
        borrows_intro = self.override and a2r_space.overrides_original(self.code)
        n_cuts, script_cams = trk_cuts.cut_count(self.code, len(cams), borrows_intro)
        self.cut_log = []
        if n_cuts > len(cams):
            import geo_course as GC_
            self._atris = GC_.arcade_triangles(self.course)
            land = self.X(np.array([t['P'] for t in self._atris[0]['land']]).reshape(-1, 3)).reshape(-1, 3, 3)
            view = trk_cuts.clear_view(np.concatenate([land, tris]))
            import trk_slabs as TS_

            def accept(eyes, tgts, view=view):
                # inside one cell of the 2016-ft geometry grid, clear of its lines by the slab rule's margin,
                # so the shot never changes the zone layout and stays inside one zone (a cut keeps its InitialZone)
                for ax in (0, 2):
                    c_ = np.floor(eyes[:, ax] / TS_.CELL)
                    if c_.min() != c_.max():
                        return False
                    lo_ = c_[0] * TS_.CELL
                    if eyes[:, ax].min() - lo_ < TS_.GROUP_CLEAR_FT or lo_ + TS_.CELL - eyes[:, ax].max() < TS_.GROUP_CLEAR_FT:
                        return False
                return view(eyes, tgts)
            view_c = trk_cuts.clear_view(np.concatenate([land, tris]), eye_clear_ft=trk_cuts.COVERED_EYE_CLEAR_FT)

            def accept_covered(eyes, tgts):   # enclosed tracks: same cell rule, covered eye clearance (trk_cuts)
                return accept(eyes, tgts, view_c)
            cams += trk_cuts.extra_shots(trk_cuts.Loop(P, Rn, map_left, map_right), n_cuts - len(cams), accept, self.cut_log,
                                         accept_covered)
        self.flags.append('cut cameras: %d (intro script of %s names cameras %s; port 1..7) [%s]' % (
            n_cuts, self.code if borrows_intro else 'this course', script_cams or 'none',
            'G 1-3 a2r shots; G %d-%d generated raised travelling shots (trk_cuts.py) %s' % (4, n_cuts, self.cut_log)
            if n_cuts > 3 else 'G'))

        # ---- zones and portals: vertical slabs on the geometry grid (trk_slabs.py, notes\arcade-visibility.md)
        # The arcade has no occlusion data: it frustum-culls its landscape blocks and draws the nearest
        # ones by depth. Revenge draws a zone only through portals, so the zones are slabs between
        # planes on lines of the 2016-ft geometry grid (no visual piece crosses one) and each portal is
        # the whole plane across the world: every line of sight passes through the portals in order.
        import trk_slabs
        gdir = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course)
        gman = json.load(open(os.path.join(gdir, 'manifest.json')))
        if abs(gman['grid_cell_ft'] - trk_slabs.CELL) > 1e-9:
            raise ValueError('geometry grid %s ft != trk_slabs.CELL' % gman['grid_cell_ft'])
        land_files = [f for f in gman['files'] if '_sky_' not in f['file']]
        pc = np.array([(np.array(f['bounds_min']) + np.array(f['bounds_max'])) / 2 for f in land_files])
        pcell = np.floor(pc[:, [0, 2]] / trk_slabs.CELL).astype(int)
        nodes_all = np.vstack([P, PA] + ([lane['B']] if lane is not None else []))
        # start grid: centre line from GRID_CLEAR_FT behind map node 0 up to node 0, 100 ft either side
        gpts = [P[0]]
        k_ = start
        for _ in range(n):
            gpts += [C[k_] + R[k_] * 100.0, C[k_] - R[k_] * 100.0]
            if (arc[start] - arc[k_]) % L >= GRID_CLEAR_FT:
                break
            k_ = (k_ - 1) % n
        groups = [np.array(gpts)] + [np.array([k[2] for k in keys]) for keys in cams]
        # each hook +44 object stays inside one slab too (a piece across a plane would be cut by the scissor)
        import trk_hookobjects
        hk = trk_hookobjects.convert(self.course, self.XF, self.natural_code, set(t['material'] for t in gman['textures']))
        groups += [np.array([info['bounds_min'], info['bounds_max']], np.float64) for _, _, info in hk['psg']]
        slab_log = []
        slabs = trk_slabs.choose(pcell.min(0), pcell.max(0), nodes_all, groups, tris, P[0], log=slab_log)
        Z = slabs.Z
        if Z > trk_slabs.MAX_ZONES:
            raise ValueError('%d zones > %d' % (Z, trk_slabs.MAX_ZONES))
        tlo = tris[:, :, slabs.axis].min(1); thi = tris[:, :, slabs.axis].max(1)
        tri_zones = [set(slabs.zones_overlapping(tlo[t], thi[t], trk_slabs.COL_MARGIN_FT)) for t in range(len(tris))]
        tri_zone = slabs.zone(tris.mean(1))
        in_zone = np.zeros((Z + 1, len(tris)), bool)
        for t, zs_ in enumerate(tri_zones):
            for z_ in zs_:
                in_zone[z_, t] = True
        node_zone = slabs.zone(P)
        # world box: collision, visual pieces, nodes, camera paths; the portals reach past it
        boxpts = np.vstack([tris.reshape(-1, 3), nodes_all] + groups +
                           [np.array([f['bounds_min'], f['bounds_max']]) for f in land_files])
        wlo = boxpts.min(0) - np.array([trk_slabs.CELL, 2000.0, trk_slabs.CELL])
        whi = boxpts.max(0) + np.array([trk_slabs.CELL, 4000.0, trk_slabs.CELL])
        portals = trk_slabs.portals(slabs, wlo, whi)
        self.slabs = slabs
        self.report['zone_layout'] = dict(axis='xyz'[slabs.axis], planes=slabs.planes.tolist(),
                                          zone_of_slab=slabs.zone_of_slab.tolist(), world_lo=wlo.tolist(), world_hi=whi.tolist(),
                                          candidates=slab_log)
        self.report['triangles_in_two_zones'] = int(sum(len(x) > 1 for x in tri_zones))
        self.flags.append('zones: %d vertical slabs between the planes %s = %s ft (lines of the 2016-ft geometry grid; '
                          'zone 1 holds map node 0; no plane within %.0f ft of a spline node or through the start grid or a '
                          'cut camera path); collision faces within %.0f ft past a plane are in both zones; no ExclusionList '
                          '(the arcade never hides landscape by position) [A no-occlusion rule, G layout]' % (
                              Z, 'xyz'[slabs.axis], [round(float(p)) for p in slabs.planes], trk_slabs.NODE_CLEAR_FT,
                              trk_slabs.COL_MARGIN_FT))
        self.flags.append('portals: %d, each the whole slab plane across the world box (y %.0f..%.0f ft), so every '
                          'line of sight between zones passes through them in order [G]' % (len(portals), wlo[1], whi[1]))

        # ---- collision split per zone and surface; one collision-only placed model each
        models = {z: [] for z in range(1, Z + 1)}
        col_names = []
        order = a2r_space.collision_order(self.XF)
        TC = tris[:, order]                                    # Revenge collision triangles (world)
        cr = np.cross(TC[:, 1] - TC[:, 0], TC[:, 2] - TC[:, 0])
        ln = np.linalg.norm(cr, axis=1)
        degenerate = ln < 1e-9
        ny = np.where(degenerate, 0.0, cr[:, 1] / np.where(degenerate, 1.0, ln))
        down = int(((ny < 0) & ~degenerate).sum())
        self.report['collision_down_facing_from_winding'] = down
        if down:
            raise ValueError('%d arcade collision triangles face down after the winding rule' % down)
        self.flags.append('collision orientation: from the arcade winding through a2r_space.collision_order '
                          '(every arcade collision triangle is wound with its normal down; all %d non-degenerate '
                          'faces now face up) [A]' % int((~degenerate).sum()))
        strip = surf == 0x8000

        # walls (G, derived from arcade data): every edge between drivable floor and an 0x8000 strip,
        # and every open outer edge of a strip, gets a vertical wall facing the track
        def vkey(v):
            return tuple(np.float32(v).tolist())
        edge_owner = {}
        for t in range(len(tris_a)):
            if degenerate[t]:
                continue
            for k in range(3):
                a_, b_ = vkey(tris_a[t, k]), vkey(tris_a[t, (k + 1) % 3])
                edge_owner.setdefault(frozenset((a_, b_)), []).append((t, k))
        # visual barrier heights: decoded PSG vertices of the geometry conversion, 8-ft grid
        gdir_ = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course)
        import geo_psg
        vgrid = {}
        for f_ in json.load(open(os.path.join(gdir_, 'manifest.json')))['files']:
            if '_sky_' in f_['file']:
                continue
            mdl = geo_psg.parse(open(os.path.join(gdir_, 'geometry', f_['file'].split(BS)[-1]), 'rb').read())
            for s_ in geo_psg.vertices(mdl):
                for q in s_['pos']:
                    vgrid.setdefault((int(q[0] // 8), int(q[2] // 8)), []).append(q)
        WALL_MIN = 82.0    # Revenge originals: median wall top 82 ft above the nearest floor (wallcheck.py)
        WALL_MAX = 400.0

        def wall_height(m):
            top = 0.0
            cx, cz = int(m[0] // 8), int(m[2] // 8)
            for dx in (-1, 0, 1):
                for dz in (-1, 0, 1):
                    for q in vgrid.get((cx + dx, cz + dz), ()):
                        if (q[0] - m[0]) ** 2 + (q[2] - m[2]) ** 2 < 36.0 and m[1] + 1.0 < q[1] < m[1] + WALL_MAX:
                            top = max(top, q[1] - m[1])
            return max(WALL_MIN, top), top > WALL_MIN
        wall_tris = {z: [] for z in range(1, Z + 1)}
        n_inner = n_outer = n_visual = 0
        for e, own in edge_owner.items():
            kinds = [strip[t] for t, k in own]
            if len(own) == 2 and kinds[0] != kinds[1]:
                inner_t = own[0][0] if not kinds[0] else own[1][0]      # the drivable floor side
                n_inner += 1
            elif len(own) == 1 and strip[own[0][0]]:
                inner_t = own[0][0]                                      # outer open edge of a strip
                n_outer += 1
            else:
                continue
            t, k = own[0]
            a_ = self.X(tris_a[t, k]); b_ = self.X(tris_a[t, (k + 1) % 3])
            mid = (a_ + b_) / 2
            h, from_visual = wall_height(mid)
            n_visual += from_visual
            up = np.array([0.0, h, 0.0])
            q = [np.array([a_, b_, b_ + up]), np.array([a_, b_ + up, a_ + up])]
            inside = TC[inner_t].mean(0)
            nrm = np.cross(q[0][1] - q[0][0], q[0][2] - q[0][0])
            if (nrm[[0, 2]] @ (inside - mid)[[0, 2]]) < 0:
                q = [x[[0, 2, 1]] for x in q]
            for z_ in tri_zones[inner_t]:
                wall_tris[z_].extend(q)
        # STAT: the arcade's second collision set (race-data.md 6 / 16): vertical wall polygons the pod
        # physics reads (f_0C102378 / f_0C102992 / f_0C10586C) - e.g. Etti IV's centre divider, which has
        # collision floor under it and no 0x8000 strip. Headers from the literal pool of the course
        # loader f_0C142D98 (@0C142E34: SA_COURSE2, SA_COURSE4, S_ETTI4); courses 2 and 4 have none [A].
        n_stat = 0
        hdr = STAT_HEADERS.get(self.course)
        if hdr is not None:
            E_ = open(os.path.join(SCR, 'img', 'eprom.bin'), 'rb').read()
            EB = 0x0C00FC00
            n_poly, arr = struct.unpack_from('<2I', E_, hdr + 4 - EB)
            cent_floor = TC.mean(1)
            for i_ in range(n_poly):
                v_ = struct.unpack_from('<4f12fI', E_, arr + 68 * i_ - EB)
                pl = np.array(v_[:3]); Q4 = np.array(v_[4:16]).reshape(4, 3)
                polys = [Q4[[0, 1, 2]]] if np.allclose(Q4[3], Q4[0]) else [Q4[[0, 1, 2]], Q4[[0, 2, 3]]]
                for tri in polys:
                    q = self.X(tri)
                    nr = np.cross(q[1] - q[0], q[2] - q[0])
                    if np.linalg.norm(nr) < 1e-9:
                        continue
                    want = a2r_space.normals(pl, self.XF)          # the arcade plane normal, Revenge frame
                    if nr @ want < 0:
                        q = q[[0, 2, 1]]
                    c_ = q.mean(0)
                    d2 = np.sum((cent_floor[:, [0, 2]] - c_[[0, 2]]) ** 2, 1)
                    near = np.flatnonzero(d2 < 60.0 ** 2)
                    if not len(near):
                        near = [int(np.argmin(d2))]
                    zs = set()
                    for t_ in near:
                        zs |= tri_zones[t_]
                    for z_ in zs:
                        wall_tris[z_].append(q)
                    n_stat += 1
            self.flags.append('STAT: %d arcade wall polygons (header @%08X, f_0C142D98 literal pool) -> collision walls, '
                              'facing as the arcade plane normal, in the zones of the floor within 60 ft [A geometry, G zones]'
                              % (n_stat, hdr))
        self.report['walls'] = dict(floor_strip_edges=n_inner, strip_outer_edges=n_outer, height_from_visual=n_visual,
                                    stat_polygons=n_stat)
        self.flags.append('walls: GENERATED vertical faces (derived from arcade data) on %d floor/strip boundary edges and %d '
                          'outer strip edges, facing the track; height = the visual barrier top above the edge where it is '
                          'higher than %.0f ft (%d edges), else %.0f ft (Revenge originals median); the 0x8000 strips stay as '
                          'up-facing floor, as in the arcade [G]' % (n_inner, n_outer, WALL_MIN, n_visual, WALL_MIN))

        def emit(z, base, T3, prov):
            V = T3.reshape(-1, 3).astype(np.float32).astype(np.float64)    # world coordinates, Position 0
            uniq, inv = np.unique(V, axis=0, return_inverse=True)
            faces = inv.reshape(-1, 3)
            keep = (faces[:, 0] != faces[:, 1]) & (faces[:, 1] != faces[:, 2]) & (faces[:, 0] != faces[:, 2])
            faces = faces[keep]
            col = trk_col.make_col(uniq, faces)
            self.put('geometry' + BS + base + '.col', trk_col.write_col(col), prov)
            col_names.append(base)
            models[z].append(dict(slt=base + '.slt', pos=None, faces=len(faces)))
        for z in range(1, Z + 1):
            ids_z = np.flatnonzero(in_zone[z] & ~degenerate)
            codes = sorted(set(surf[ids_z].tolist()))
            small = [c_ for c_ in codes if np.sum(surf[ids_z] == c_) < 2]
            for c_ in codes:
                ids = ids_z[surf[ids_z] == c_]
                if c_ in small:
                    continue
                extra = [ids_z[surf[ids_z] == s2] for s2 in small] if c_ == max(codes, key=lambda x: np.sum(surf[ids_z] == x)) else []
                ids = np.concatenate([ids] + extra)
                kind = 'strip' if c_ == 0x8000 else 's%02X' % c_
                emit(z, '%s_z%02d_%s_floor_col' % (self.code, z, kind), TC[ids],
                     'A arcade collision (zone %d, surface %X), oriented by the arcade winding; G tree/planes/split' % (z, c_))
            if len(wall_tris[z]) >= 2:
                emit(z, '%s_z%02d_wall_col' % (self.code, z), np.array(wall_tris[z]),
                     'G vertical walls along the arcade 0x8000 edge strips (derived from arcade data)')
        self.report['collision_files'] = len(col_names)

        # every spline node (AI line, map line, fork branch) needs a floor face below it in the COL files
        # of its own zone (the slab that holds it)
        missing = []
        for tag_, Q_ in (('ai', PA), ('map', P)) + ((('fork', lane['B']),) if lane is not None else ()):
            for j_, q_ in enumerate(Q_):
                z = int(slabs.zone(q_))
                ids = np.flatnonzero(in_zone[z] & ~degenerate)
                h = heights_under(TC[ids], q_[0], q_[2])
                if not len(h):
                    missing.append(j_ if tag_ == 'ai' else '%s%d' % (tag_, j_))
        self.report['ai_nodes_without_floor_in_own_zone'] = missing
        if missing:
            self.flags.append('AI nodes without a floor face in their own zone: %s' % missing)

        # ---- visual models and textures from the geometry conversion (geo_course.py)
        gdir = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course)
        gman = json.load(open(os.path.join(gdir, 'manifest.json')))
        if gman['code'] != self.natural_code or gman['transform'] != self.XF:
            raise ValueError('geometry output was made with another code/transform: rerun geo_course.py %d' % self.course)
        nvis = 0
        vis = {}
        spill = 0.0
        for f in gman['files']:
            name = f['file'].split(BS)[-1][:-4]
            if '_sky_' in name:
                continue   # the arcade dome becomes the sky cube below, not a scene model
            data = open(os.path.join(gdir, 'geometry', name + '.psg'), 'rb').read()
            self.put('geometry' + BS + name + '.psg', data, 'geometry helper (geo_course.py): arcade landscape, A + G lighting')
            # the zone of the slab holding the piece: geo_course.py clips every piece to the 2016-ft grid
            # and the slab planes are grid lines, so a piece lies in exactly one slab (spill below = the
            # decal layer offsets, at most LAYER_STEP_MAX)
            lo_ = np.array(f['bounds_min']); hi_ = np.array(f['bounds_max'])
            z = int(slabs.zone((lo_ + hi_) / 2))
            s_ = int(slabs.slab(((lo_ + hi_) / 2)[slabs.axis]))
            a_, b_ = slabs.slab_range(s_)
            spill = max(spill, a_ - lo_[slabs.axis], hi_[slabs.axis] - b_)
            # world pieces are placed at 0,0,0 like TA Zone01GroundStrips / GC gc_z1_track_a
            vis.setdefault(z, []).append(dict(slt=name + '.slt', pos=None, visual=True))
            nvis += 1
        used_mats = set(f['material'] for f in gman['files'] if '_sky_' not in f['file'])
        for t in gman['textures']:
            if t['material'] not in used_mats:
                continue   # dome-only texture: now inside the sky cube
            for ext in ('pst', 'psm'):
                nm = 'textures' + BS + t['material'] + '.' + ext
                self.put(nm, open(os.path.join(gdir, 'textures', t['material'] + '.' + ext), 'rb').read(),
                         'geometry helper (geo_course.py): arcade texture %s' % t.get('arcade_texture'))
        for name, blob, info in hk['psg']:
            self.put('geometry' + BS + name + '.psg', blob,
                     'A arcade object placed by the course hook +44 (notes\\race-hooks.txt), converted with the geometry helper pipeline')
            lo_ = np.array(info['bounds_min']); hi_ = np.array(info['bounds_max'])
            s_ = int(slabs.slab(((lo_ + hi_) / 2)[slabs.axis]))
            a_, b_ = slabs.slab_range(s_)
            spill = max(spill, a_ - lo_[slabs.axis], hi_[slabs.axis] - b_)
            vis.setdefault(int(slabs.zone((lo_ + hi_) / 2)), []).append(dict(slt=name + '.slt', pos=None, visual=True))
        for mname, pst, psm in hk['textures']:
            self.put('textures' + BS + mname + '.pst', pst, 'A arcade texture of a hook +44 object (geo_pst encoder)')
            self.put('textures' + BS + mname + '.psm', psm, 'G material for a hook +44 object (geo_psm)')
        if hk['placements']:
            self.report['hook_objects'] = hk['placements']
            self.flags.append('hook +44 static objects: %d placements -> %d PSG, %d new textures; placement = object x scale + (x, y, z), '
                              'the 4th value is a scale (f_0C146F3A multiplies the bounding radius by it at 0C146FCC) [A, A-derived]' % (
                                  len(hk['placements']), len(hk['psg']), len(hk['textures'])))
        for z in models:   # visual pieces first in each zone, then its collision models
            models[z] = vis.get(z, []) + models[z]
        self.report['visual_models'] = sum(1 for f in gman['files'] if '_sky_' not in f['file'])

        # ---- sky: the arcade dome rendered into Revenge's sky cube (trk_sky.py)
        import trk_sky
        import geo_course as GC_
        atris, _ = getattr(self, '_atris', None) or GC_.arcade_triangles(self.course)
        sky = atris['sky']
        SP = np.array([t['P'] for t in sky]).reshape(-1, 3)
        lo_, hi_ = SP.min(0), SP.max(0)
        # projection centre = the typical track position: centroid of the arcade centre line, at the
        # median track ground height plus the arcade chase-camera height (9 units above the line,
        # course_preview.py). The arcade dome is a finite world object; seen from the dome centre its
        # clouds came out smaller and lower than from the track.
        ground_a = np.median((C[:, 1] - self.XF['offset'][1]) / self.XF['scale'])
        eye = np.array([c_a[:, 0].mean(), ground_a + 9.0, c_a[:, 1].mean()])
        texs = {}
        for t in sky:
            if t['tid'] is not None and t['tid'] not in texs:
                texs[t['tid']] = GC_.EC.texture_rgba(t['tid'])[0]
        tiles, misses = trk_sky.render_tiles(sky, self.XF, eye, texs)
        horizon = trk_sky.fade_horizon(tiles)
        rta_ = trk_res3.read_res3(open(REVENGE + 'TA.RES', 'rb').read())
        palette = rta_.get('skies' + BS + 'ta.cub')[16:99100]
        cub = trk_sky.build_cub(tiles, palette)
        self.put('skies' + BS + '%s.cub' % self.code, cub,
                 'A arcade sky dome (%d triangles, texture %s) ray-cast from its centre into the Revenge sky cube; '
                 'G cube layout, IPU encoding; palette block copied from ta.cub [D]' % (len(sky), sorted(texs)))
        # horizon colour = bottom texel rows of the lowest authored side tiles (rows 4..7 -> tile row 1)
        self.sky_horizon = horizon
        self.report['sky'] = dict(member='skies' + BS + '%s.cub' % self.code, bytes=len(cub), tiles=len(tiles),
                                  dome_triangles=len(sky), eye_arcade=eye.tolist(), rays_missing_dome=int(sum(misses.values())),
                                  horizon_rgb=self.sky_horizon)
        self.flags.append('sky: arcade dome -> skies\\%s.cub (48 tiles x 4 IPU mips, the originals\' layout); the dome is no longer a '
                          'scene model; %d of %d rays below the dome rim take the colour of the lowest dome texel in their '
                          'column; the lowest %d texel rows fade into the horizon colour %s, which is also Fog ColorRGB (the clear colour below the cube) [A, G]' % (self.code, sum(misses.values()), 48 * 128 * 128, trk_sky.FADE_ROWS, horizon))
        self.report['textures'] = len(gman['textures'])
        self.report['visual_spill_ft'] = round(float(spill), 3)
        self.flags.append('visual models: %d PSG world pieces placed at 0,0,0, each in the zone of the slab that holds it (pieces are clipped to the 2016-ft grid and the slab planes are grid lines; largest reach past its slab %.2f ft, the decal layer offsets) [geometry A, zone G]' % (sum(1 for f in gman['files'] if '_sky_' not in f['file']), spill))

        # ---- portals: made with the zones (trk_slabs.portals), one whole plane per slab boundary

        # ---- template data (donor files)
        donor = DONOR_BY_CLASS[self.cls]
        self.donor = donor
        rta = trk_res3.read_res3(open(REVENGE + 'TA.RES', 'rb').read())
        rgc = trk_res3.read_res3(open(REVENGE + 'GC.RES', 'rb').read())
        rdn = trk_res3.read_res3(open(REVENGE + donor + '.RES', 'rb').read())
        gc_scn = trk_ini.parse(rgc.get('gc.scn')).sections()
        gc_terrain = [s for s in gc_scn if s[0].lower().startswith('terrainzone')]

        # ---- course.json values (fog floats, material ambient)
        cj = json.load(open(os.path.join(SCR, 'export', 'course%d' % self.course, 'course.json')))
        amb = [tuple(v['ambient']) for Lm in cj['materials'].values() for v in Lm.values()]
        amb = tuple(int(np.median([a[i] for a in amb])) for i in range(3)) if amb else (80, 80, 90)
        fogf = cj['landscape_record']['floats_28_32']
        hook = parse_hooks().get(self.course)

        # ---- scene (.scn text -> .scb)
        scn = [('SceneInfo', [('NumPortals', str(len(portals))), ('NumZones', str(Z))])]
        for i, p in enumerate(portals, 1):
            scn.append(('Portal%d' % i, [('Vertex%d' % (k + 1), vec(v)) for k, v in enumerate(p['v'])] +
                        [('Connect1', str(p['c'][0])), ('Connect2', str(p['c'][1]))]))
        for z in range(1, Z + 1):
            scn.append(('Zone%d' % z, [('NumberOfStaticModels', str(len(models[z]))), ('SearchType', 'List'),
                                       ('ZoneReverb', '0')]))
            for m_i, m in enumerate(models[z], 1):
                scn.append(('Zone%d-Model%d' % (z, m_i), [('SLT', m['slt']), ('Position', '0.0,0.0,0.0' if m['pos'] is None else vec(m['pos'])),
                            ('LookVector', '0.000000,0.000000,1.000000'), ('UpVector', '0.000000,1.000000,0.000000'),
                            ('Uselighting', 'F')]))
        cmin = tris.reshape(-1, 3).min(0); cmax = tris.reshape(-1, 3).max(0)
        ctr = (cmin + cmax) / 2
        ldirs, lpoints = parse_lights()
        dl = ldirs.get(self.course, [])
        sun_dir = np.array([0.6, 1.0, 0.6]); sun_col = (170, 170, 160)
        if dl:
            sun_dir = a2r_space.normals(np.array(dl[0][0]), self.XF)   # vector toward the light (sun above: +y)
            sun_col = tuple(int(round(min(1.0, c) * 255)) for c in dl[0][1])
        sun_dir = sun_dir / np.linalg.norm(sun_dir)
        sun_pos = np.array([ctr[0], cmin[1], ctr[2]]) + sun_dir * 8000.0
        flare = self.course != 2     # arcade sun lens flare (flare objects 1219-1225) on courses 0, 1, 3, 4 (race-data.md 18)
        point_lights = []
        for i_, q in enumerate(lpoints.get(self.course, [])):
            wq = self.X(np.array(q))
            j_ = int(np.argmin(np.linalg.norm(C - wq, axis=1)))
            point_lights.append(('Light%d' % (3 + i_), [
                ('Type', 'ObjectPoint'), ('ColorRGB', '242,242,191'), ('Position', vec(wq)),
                ('InitialZone', str(int(slabs.zone(wq)))), ('EmitsLight', '1'),
                ('Attenuation', '1.000000,0.000000,0.000400'), ('HasLightFlare', '0')]))
        fog_rgb = self.sky_horizon     # clear colour under the sky = fog colour: matched to the sky horizon
        scn += [
            ('Environment', [('TerrainWidth', '5'), ('TerrainBreadth', '5'),
                             ('TerrainWidthScale', '24.0'), ('TerrainBreadthScale', '24.0'),
                             ('ParticleTextureMapName', 'demoprt.tga'), ('CubeFile', '%s.cub' % self.code)]),
            ('Lights', [('NumberOfLights', str(2 + len(point_lights)))]),
            ('Light1', [('Type', 'Ambient'), ('ColorRGB', '%d,%d,%d' % amb)]),
            ('Light2', [('Type', 'Directional'), ('ColorRGB', '%d,%d,%d' % sun_col),
                        ('Position', '%.3f,%.3f,%.3f' % tuple(sun_pos)),
                        ('TargetPosition', '%.1f,%.1f,%.1f' % (ctr[0], cmin[1], ctr[2])),
                        ('PlaceLightAtInfinity', 'T'), ('CastsShadows', 'T'), ('EmitsLight', 'T'),
                        ('HasLensFlare', 'T' if flare else 'F'), ('LensFlareColorRGB', '255,255,240'), ('LensFlareBrightness', '1.0'),
                        ('LensFlareLayersVisible', 'T,T,T,T,T')])] + point_lights + [
            ('GhostOpacity', [('Reset', '0.40'), ('Ghost', '0.53')]),
            ('Fog', [('ColorRGB', '%d,%d,%d' % tuple(fog_rgb))] +
                    ([('Visibility', FOG_FRACTIONS[self.course][0]), ('Haziness', FOG_FRACTIONS[self.course][1])]
                     if self.course in FOG_FRACTIONS else [('Visibility', '0.5')])),
            ('CSCameras', [('NumberOfCameras', '0')]),
        ] + gc_terrain
        self.flags += [
            'Environment: CubeFile %s.cub (single sky cube, as TA/TB/MA/GA/GB/GC) [A-derived]; no water file' % self.code + ' (the arcade has no water physics, race-data.md 21), no ecosystem; terrain keys copied from GC [G/D]',
            'collision sectors (track slices) read from the course: %d [A]' % nsec,
            'Lights: Light1 ambient %s = median arcade material ambient [A]' % (amb,),
            ('Fog: ColorRGB %s = the sky horizon colour (it is also the clear colour under the sky cube) [A-derived]; '
             'arcade fog colour %s (hook +40) recorded, not used; arcade fog near/far %s units = %s ft; '
             'Visibility 0.5 from GC [D] (mapping from distances to Visibility unknown)' % (
                 fog_rgb, hook['fog_rgb'], (hook['fog_near'], hook['fog_far']) if hook else '-',
                 (hook['fog_near'] * self.XF['scale'], hook['fog_far'] * self.XF['scale']) if hook else '-')) if hook else
            'Fog: GC values [D] (no arcade fog entry)',
            ('Fog: Visibility %s / Haziness %s by user decision (no distance fog) [user]' % FOG_FRACTIONS[self.course])
            if self.course in FOG_FRACTIONS else 'Fog: Visibility 0.5 [D]',
('Light2 = arcade light 0: vector %s, colour %s (race-data.md 20) [A], mirrored with the course; flags GC [D]; '
             'HasLensFlare %s (arcade sun lens flare on courses 0, 1, 3, 4, race-data.md 18) [A]' % (dl[0][0], dl[0][1], 'T' if flare else 'F'))
            if dl else 'Light2 sun G (no arcade light entry)',
            ('arcade lights not written: %s (Revenge tracks carry one directional light each; a second one is unproven in the port) [pending]' %
             ['%s %s' % (v, c_) for v, c_ in dl[1:]]) if len(dl) > 1 else 'no further arcade directional lights',
            ('%d arcade point lights (race-data.md 20, @0C3E8120) -> ObjectPoint Light3..; colour 0.95,0.95,0.75 [A]; '
             'range 20..50 units (60..150 ft) -> Attenuation 1,0,0.0004 (10 %% at 150 ft) [G]; per-frame flicker between ranges '
             '(20,50)/(15,45) not written: steady light at the larger range (the port reads HasFlasher only in the patterns of '
             'original MB/GB rows, and an on/off pattern was a generated stand-in for a range flicker anyway) [pending]' % len(point_lights)) if point_lights else 'no arcade point lights',
            'GhostOpacity: TA/GC values [D]; TerrainZone1..8 copied from gc.scn [D]',
        ]
        scn_bytes = trk_ini.write_canonical(scn)
        self.put('tracks' + BS + '%s.scn' % self.code, scn_bytes, 'text source of the .scb (G/A/D per section, manifest flags)')
        self.put('tracks' + BS + '%s.scb' % self.code, trk_binparam.compile_ini(trk_ini.parse(scn_bytes).sections()),
                 'compiled from the .scn with the original compile rule')
        self.report['zone_models'] = {z: len(models[z]) for z in models}

        # ---- cut cameras (.vub) + .toy/.tob
        tmpl = trk_vub.read_vub(rta.get('tracks' + BS + 'ta01.vub'))[0]['raw']

        def zone_at(p):
            return int(slabs.zone(np.asarray(p, np.float64)))
        for i, keys in enumerate(cams, 1):     # the layout keeps every cut inside one zone; prove it
            zs_cut = sorted(set(int(z) for z in slabs.zone(np.array([k[2] for k in keys]))))
            if len(zs_cut) != 1:
                raise ValueError('cut camera %d crosses zones %s (a cut keeps its InitialZone)' % (i, zs_cut))

        toy = [('CSCameras', [('NumberOfCameras', str(len(cams)))])]
        for i, keys in enumerate(cams, 1):
            nm = 'tracks' + BS + '%s%02d.vue' % (self.code, i)
            self.put('tracks' + BS + '%s%02d.vub' % (self.code, i), trk_vub.make_vub(tmpl, nm, keys),
                     'G camera path; record layout and runtime words from ta01.vub [D]')
            toy.append(('CSCamera%d' % i, [('InitialZone', str(zone_at(np.array(keys[0][2]))))]))
        toy.append(('PodRacer', [('TrackAllowFloor', '1')]))
        toy_bytes = trk_ini.write_canonical(toy)
        self.put('tracks' + BS + '%s.toy' % self.code, toy_bytes, 'text source of the .tob')
        self.put('tracks' + BS + '%s.tob' % self.code, trk_binparam.compile_ini(trk_ini.parse(toy_bytes).sections()),
                 'compiled from the .toy; %d G cut cameras, PodRacer TrackAllowFloor=1 as every original' % len(cams))
        self.flags.append('cut cameras: 1-3 G paths (flyover, side, grid) of 300/250/150 frames%s; no animations, triggers, emitters, static model params [G]'
                          % (('; 4-%d G raised travelling shots of %d frames' % (len(cams), trk_cuts.EXTRA_FRAMES)) if len(cams) > 3 else ''))

        # ---- TV cameras (.pos)
        ntv = int(min(40, max(15, round(lap / TV_SPACING_FT))))
        pos = [('CameraPositions', [('NumberOfCameras', str(ntv)), ('SwitchEvery', '20')])]
        sdist = (lap - dtf) % lap
        sdist[0] = 0
        for i in range(ntv):
            j = int(np.argmin(np.abs(sdist - (i + 0.5) * lap / ntv)))
            # outside of the bend
            a = (P[j] - P[prv[j]])[[0, 2]]; b = (P[nxt[j]] - P[j])[[0, 2]]
            turn = a[0] * b[1] - a[1] * b[0]
            side = 1.0 if turn > 0 else -1.0
            w = (map_right[j] if side > 0 else map_left[j]) * 0.8
            eye = P[j] + Rn[j] * side * w + np.array([0, 60.0, 0])
            pos.append(('Camera%d' % (i + 1), [
                ('Position', '%.3f,%.3f,%.3f' % tuple(eye)), ('MaxFOV', '40.0'), ('MinFOV', '40.0'),
                ('MaxFOVDistance', '670.0'), ('MinFOVDistance', '312.0'), ('TV', '100'), ('Follow', '0'),
                ('LeftSide', '0'), ('RightSide', '0'), ('LookBack', '0'), ('FirstPerson', '0'), ('Blur', '0'),
                ('InitialZone', str(int(slabs.zone(eye)))), ('NumNextCameras', '1'),
                ('NextCamera1', str(i + 2 if i + 1 < ntv else 1))]))
        self.put('tracks' + BS + '%sTVCam.pos' % self.code, trk_ini.write_canonical(pos),
                 'G TV cameras along the map spline (FOV values from taTVCam.pos Camera1 [D])')

        # ---- donor copies
        tun_name = [n_ for n_ in rdn.names() if n_.lower().endswith('.tun')][0]
        if trk_addon.is_addon(self.code):     # add-on course: own .tun in the original layout (trk_addon.py)
            self.put('tracks' + BS + '%s.tun' % self.code, *trk_addon.tun(self.cls))
        else:
            self.put('tracks' + BS + '%s.tun' % self.code, rdn.get(tun_name),
                     'D byte copy of %s (physics class %s donor)' % (tun_name, self.cls))
        for nm in ('particle' + BS + 'demoprt.prt', 'particle' + BS + 'demoprt.pst', 'particle' + BS + 'demoprt.psm'):
            self.put(nm, rta.get(nm), 'D byte copy from TA.RES (shared particle atlas)')
        # material audio: generated from this track's own collision objects (trk_aub.py), for every code;
        # with no source surface sound types it is one default material (rock) with TA's rows, no ambient sounds
        import trk_aub
        aub, aub_rep = trk_aub.generate(col_names)
        self.report['aub'] = aub_rep
        self.put('tracks' + BS + '%s.aub' % self.code, aub, trk_aub.provenance(aub_rep))
        self.put('audio' + BS + 'introAud.ini', trk_ini.write_canonical([('ResponseEmitter', [('NumEvents', '0')])]),
                 'G no announcer lines (none exist for this planet); reader acceptance of 0 unchecked')
        self.put('audio' + BS + 'scrptLns.ini', trk_ini.write_canonical([('General', [('NumberOfLines', '0')])]),
                 'G no announcer lines')

        # debug textures every original but SA carries (byte copies)
        if trk_addon.is_addon(self.code):     # add-on course: authored debug textures, not stock bytes
            for nm, d_, p_ in trk_addon.debug_textures():
                self.put(nm, d_, p_)
        else:
            for nm in [n_ for n_ in rta.names() if n_.lower().startswith('textures' + BS + 'debug')]:
                self.put(nm, rta.get(nm), 'D byte copy from TA.RES (debug textures carried by 12 of 13 originals)')

        # ---- files outside the track RES
        self.extra = {}
        # loading screen UI\TEXTURES\LD_XX.PST: 640x448 kind 1 single level like LD_TA (arcade top view)
        try:
            from PIL import Image
            import geo_pst   # geometry helper's PST quantiser (used read-only)
            src = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course, 'preview', 'top_textured.png')
            im = Image.open(src).convert('RGBA')
            im.thumbnail((640, 448))
            canvas = Image.new('RGBA', (640, 448), (0, 0, 0, 255))
            canvas.paste(im, ((640 - im.width) // 2, (448 - im.height) // 2))
            a = np.asarray(canvas).copy(); a[..., 3] = 255
            pal = geo_pst.quantise(a)
            idx = geo_pst.nearest(a.reshape(-1, 4), pal)
            pst = geo_pst.write(dict(kind=1, palette=pal.tobytes(),
                                     levels=[dict(w=640, h=448, data=geo_pst.PERM[idx.astype(np.uint8)].tobytes())]))
            self.extra['UI' + BS + 'TEXTURES' + BS + 'LD_%s.PST' % self.code.upper()] = (
                pst, 'A-derived: arcade textured top view of the converted geometry (export\\a2r\\course%d\\preview\\top_textured.png) on 640x448 like LD_TA' % self.course)
        except Exception as e:   # noqa
            self.flags.append('loading screen not written: %r' % e)
        names = {0: ('TATOOINE', 'BANTHA TRACKS'), 1: ('ETTI IV', "SMUGGLER'S COVE"), 2: ('MALASTARE', 'PIXELITO CHALLENGE'),
                 3: ('TATOOINE', 'BOONTA EVE CLASSIC'), 4: ('NABOO', 'KAADU OVAL')}[self.course]
        planet_id = 'STR_PLANET_' + names[0].replace(' ', '_')
        lines = ['; new LOC strings for %s (English from the arcade course-select screen, notes\\CONTEXT.md roster);' % self.code,
                 '; other languages: same text until translated [A names, G ids]',
                 '%s = %s' % (planet_id, names[0]),
                 'STR_TRACK_%s_%s = %s' % (names[0].replace(' ', '_'), self.cls, names[1])]
        self.extra['menu' + BS + 'loc_strings.txt'] = ('\r\n'.join(lines).encode('latin-1') + b'\r\n',
                                                     'A course names; G string ids; LOC.RES itself not rewritten')
        self.pending = [
            ('skies\\%s.cub' % self.code, 'not used: the arcade sky dome is a placed model and NumberOfSkyCubes=0 (SC precedent)'),
            ('scrolling texture (courses 1 and 2: GPU texture panning slot 1, 3/16 texel per frame in V, race-data.md 18)',
             'which arcade materials use slot 1 is not known yet (race-data.md 24); Revenge animates textures as .atm frame sets, not yet written'),
            ('AI pace and rubber band (race-data.md 13: speed table = target lap time per AI level, per-lap ease-off, x0.95..x1.1 catch-up)',
             'Revenge AI pace lives in the .tun [AIDriving] (donor copy); no proven mapping yet'),
            ('UI\\UICOURSE\\UICOURSE.RES: ui\\anim\\holo_%s.psg/.mcf, ui\\textures\\holo_%s.pst' % (self.code, self.code),
             'course-select hologram not made yet'),
            ('anim\\* + .tob [Animations]', 'none needed on the race courses: the arcade "Kaadu" routine is the sun lens flare (race-data.md 18); Kaadu creatures exist only on the unreachable course 4' +
             (' (pending for this course)' if self.course == 4 else '')),
            ('particle\\*.emt + .tob [ParticleEmitters]', 'none: the arcade has no per-course emitters'),
            ('tracks\\%s.wtt (+ Environment WaterFile)' % self.code, 'none: the arcade has no water physics (race-data.md 21)' +
             ('; it draws mirrored pod reflections across arcade y = 0 = Revenge y %.0f ft on this course (no Revenge file equivalent)' % self.XF['offset'][1]
              if self.course == 1 else '')),
            ('tracks\\%s.esb (+ EcosystemFile)' % self.code, 'none: the arcade has no billboard ecosystems'),
            ('ambient / positional sounds (.aub SoulTreeSounds)', 'none: the arcade has no per-course or positional sound (race-data.md 22); shared sound packs and one music stream only'),
            ('AUDIO\\SPEECH\\DAX\\TRACKS\\<planet>\\*.VAG, audio\\introAud.ini events', 'no Revenge announcer lines for this course'),
            ('race music, AUDIO\\MUSIC\\RACE\\INTROS\\*_INT.VAG', 'arcade SPSD stream %s (race-select table) needs the Revenge music container' %
             {0: 20, 1: 6, 2: 19, 3: 1}.get(self.course, '-')),
            ('fog Visibility/Haziness, per-block fog factors (hook +48), arcade fill lights',
             'fog colour, sun, flare and point lights are in; the fog near/far -> Visibility mapping, the +48 block factors and a second directional light have no proven Revenge mapping'),
        ]

        # ---- packing under an original Revenge code: ship that course's own pinned files
        self.borrowed = []
        if self.override and a2r_space.overrides_original(self.code):
            rt = trk_res3.read_res3(open(REVENGE + self.code.upper() + '.RES', 'rb').read())
            # the .aub is no longer borrowed: the stock one names the stock course's COL objects and places its
            # positional sounds in the stock geometry; ours is generated from this track's collision (above)
            for nm in ['tracks' + BS + '%s.tun' % self.code,
                       'audio' + BS + 'introAud.ini', 'audio' + BS + 'scrptLns.ini',
                       'particle' + BS + 'demoprt.prt', 'particle' + BS + 'demoprt.pst', 'particle' + BS + 'demoprt.psm']:
                real = [n_ for n_ in rt.names() if n_.lower() == nm.lower()][0]
                for old in [k for k in self.files if k.lower() == nm.lower()]:
                    del self.files[old]; self.prov.pop(old, None)
                self.put(real, rt.get(real), 'BORROWED: byte copy of the original %s.RES member (the port pins it; silo-override.md 5)' % self.code.upper())
                self.borrowed.append(real)
            ld = 'UI' + BS + 'TEXTURES' + BS + 'LD_%s.PST' % self.code.upper()
            self.extra.pop(ld, None)
            self.borrowed.append(ld + ' (original kept in place, not written)')
            self.flags.append('packed as original course code %s (override of %s): borrowed %s [D]' % (
                self.code, self.natural_code, ', '.join(self.borrowed)))

        # ---- keep for verification
        self.P, self.K, self.nxt, self.prv = P, K, nxt, prv
        self.ai_l, self.ai_r, self.map_l, self.map_r, self.Rn = ai_left, ai_right, map_left, map_right, Rn
        self.portals, self.node_zone, self.tri_zone, self.Z = portals, node_zone, tri_zone, Z
        self.prog, self.lap = prog, lap
        self.report.update(dict(code=self.code, cls=self.cls, donor=donor, nodes=K, zones=Z, lap_ft=round(lap, 1),
                                arcade_laps=laps.get(self.course), arcade_ai_speeds=speeds.get(self.course),
                                tv_cameras=ntv, transform=self.XF))
        if grid:
            self.report['arcade_grid_converted'] = [list(np.round(self.X(np.array(g['pos'])), 1)) for g in grid]

    # -------------------------------------------------- output
    def write(self):
        r"""export\a2r\<code>\: install\ = files laid out as in the game folder (TRACKS\<CODE>.RES,
        UI\TEXTURES\LD_<CODE>.PST), members\ = the RES members loose, source\ = .scn/.toy text,
        menu\ = LOC strings, manifest.json. The folder is rebuilt from scratch."""
        import shutil
        keep_verify = os.path.join(self.out, 'verify')
        for sub in ('install', 'members', 'source', 'menu', 'outside_res', 'tracks', 'geometry', 'textures',
                    'particle', 'audio'):
            shutil.rmtree(os.path.join(self.out, sub), ignore_errors=True)
        for f_ in os.listdir(self.out) if os.path.isdir(self.out) else []:
            if f_.endswith('.RES'):
                os.remove(os.path.join(self.out, f_))
        os.makedirs(self.out, exist_ok=True)
        src = {nm: d for nm, d in self.files.items() if nm.endswith(('.scn', '.toy'))}
        names = sorted([nm for nm in self.files if nm not in src],
                       key=lambda s_: (0 if s_.endswith('.tob') else 1 if s_.endswith('.aub') else
                                       2 if s_.endswith('.scb') else 3, s_.lower()))
        for name in names:
            pth = os.path.join(self.out, 'members', *name.split(BS))
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(self.files[name])
        for name, d in src.items():
            pth = os.path.join(self.out, 'source', *name.split(BS))
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(d)
        res_path = os.path.join(self.out, 'install', 'TRACKS', '%s.RES' % self.code.upper())
        os.makedirs(os.path.dirname(res_path), exist_ok=True)
        open(res_path, 'wb').write(trk_res3.write_res3([(nm, self.files[nm]) for nm in names]))
        for name, (data, prov) in self.extra.items():
            base = 'menu' if name.startswith('menu') else 'install'
            rel = name.split(BS)[1:] if base == 'menu' else name.split(BS)
            pth = os.path.join(self.out, base, *rel)
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(data)
        man = dict(course=self.course, name=self.cfg['name'], code=self.code, provisional_code=self.cfg['provisional_code'],
                   physics_class=self.cls, transform=self.XF, report=self.report, flags=self.flags,
                   res='install\\TRACKS\\%s.RES' % self.code.upper(),
                   files={nm: dict(bytes=len(self.files[nm]), provenance=self.prov[nm]) for nm in names},
                   sources={nm: len(d) for nm, d in src.items()},
                   outside_res={n_: dict(bytes=len(d_), provenance=p_) for n_, (d_, p_) in self.extra.items()},
                   pending=[dict(item=a, reason=b) for a, b in self.pending])
        open(os.path.join(self.out, 'manifest.json'), 'w').write(json.dumps(man, indent=1, default=float))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('course', type=int)
    ap.add_argument('--out', default=None)
    ap.add_argument('--code-as', default=None, help='pack under this course code (a2r_space.course_code override)')
    ap.add_argument('--dlc-out', default=None, help='with --code-as: also write the DLC replacement package into this folder')
    ap.add_argument('--dlc-disabled', action='store_true', help='write the package with "enabled": false')
    ap.add_argument('--phy-class', default=None, choices=['A', 'B', 'C', 'a', 'b', 'c'],
                    help='add-on course (own code) only: explicit PHY class (dlc.json phy_class); picks the .tun donor')
    a = ap.parse_args()
    if a.dlc_out and not a.code_as:
        raise SystemExit('--dlc-out writes a replacement package: it needs --code-as <stock course code>')
    out = a.out or (os.path.join(SCR, 'export', 'a2r', 'override') if a.code_as else os.path.join(SCR, 'export', 'a2r'))
    c = Conv(a.course, out, a.code_as, a.phy_class)
    c.run()
    c.write()
    print(json.dumps(c.report, default=float)[:2000])
    if a.dlc_out:
        import trk_silo
        trk_silo.package(a.code_as.lower(), out, a.dlc_out, None, not a.dlc_disabled)
    return c


if __name__ == '__main__':
    main()
