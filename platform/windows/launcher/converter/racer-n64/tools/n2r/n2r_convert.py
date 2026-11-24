r"""N64 Racer track -> complete Revenge track (one code path for every N64 track).

    python -I n2r_convert.py MODEL [--code-as sc] [--out DIR]
    python -I n2r_convert.py MODEL --code-as sc --dlc-out DIR [--dlc-disabled]
        also writes the DLC replacement package DIR\<package id>\ (a2r trk_silo.package -> dlc_out.py)
        MODEL = N64 model index of the track (143 = Spice Mine Run); export from tools\n64r first.

Inputs (read-only): racer-n64\export\<NNN>_<name>\ (geometry/skybox/collision OBJ, materials, splines,
cameras, collision_surfaces, triggers, track.json). Revenge originals C:\Games\Star Wars Racer Revenge\TRACKS
(templates/donors, read-only). Writers: the a2r modules (racer-arcade\tools\a2r), used unchanged.

Output: racer-n64\export\n2r\<code>\ with the same layout as the a2r converter (install\TRACKS\<CODE>.RES,
members\, source\, geometry\ textures\ (intermediate), manifest.json with provenance A/G/D per file
and field, flags, pending). Transform: n2r_space.py (the only definition).
"""
import argparse
import json
import math
import os
import shutil
import sys

import numpy as np
from scipy.spatial import cKDTree

HERE = os.path.dirname(os.path.abspath(__file__))
N64 = os.path.normpath(os.path.join(HERE, '..', '..'))
A2R = os.path.normpath(os.path.join(N64, '..', 'racer-arcade', 'tools', 'a2r'))
sys.path.insert(0, HERE)
sys.path.insert(1, A2R)
import n2r_space  # noqa: E402
import n2r_io  # noqa: E402
import n2r_geo  # noqa: E402
import n2r_n64col  # noqa: E402
import n2r_props  # noqa: E402
import trk_ini  # noqa: E402
import trk_binparam  # noqa: E402
import trk_col  # noqa: E402
import trk_res3  # noqa: E402
import trk_vub  # noqa: E402
import trk_sky  # noqa: E402
import trk_slabs  # noqa: E402
import trk_addon  # noqa: E402  (a2r: members an add-on course carries itself)

BS = chr(92)
REVENGE = (__import__('os').environ['RR_CONVERT_STOCK']+'/TRACKS/')
REVENGE_CODES = ('ta', 'tb', 'tc', 'ra', 'rc', 'ma', 'mb', 'mc', 'ga', 'gb', 'gc', 'sa', 'sc')

# ---------------------------------------------------------------- generated-rule constants (G)
FLOOR_NY = 0.6            # collision face with |normal.y| >= 0.6 (slope <= 53 deg) is floor, else wall
AI_TRACKWIDTH = 100.0     # a2r / most common original AI TrackWidth
MAP_TRACKWIDTH = 200.0
AI_WALL_CLEAR_FT = 20.0   # a2r: racing line kept 20 ft inside the drivable floor
AI_MIN_HALF = 30.0
MAP_MARGIN_FT = 30.0
ZONE_LENGTH_FT = 4000.0   # a2r / originals 3.7-5.4k ft of lap per zone
GRID_CLEAR_FT = 300.0     # start grid reaches 200 ft behind map node 0
START_GROUP_BACK_FT = 1200.0  # slab rule: no zone plane through the start area 1,200 ft behind map node 0 (grid, the
                          # generated cut 2 starts 600-950 ft back, cut 3 220-320 ft back) [G]
START_GROUP_SIDE_FT = 200.0   # ... or within 200 ft either side of the lane there [G]
GRID_PRED_FT = 260.0       # the map node before node 0 (Next0 = 0) lies this far behind node 0 (Revenge ft, not scaled: the
                           # original grid 2A2880 puts rows 0 / 100 / 200 ft behind node 0, and the AI lap scan credits a lap
                           # to a racer that starts behind that node's gate; stock 209..918, cr16-fix-load.txt fault 10)
GRID_PRED_MIN_FT = 200.0   # writer check: refused at or below this
NODE_TOLERANCE = 6.0
NODE_MAX_SPACING = 400.0
NODE_MIN_SPACING = 60.0
MAX_NODES = 250
PROGRESS_FRACTIONS = (0.02, 0.25, 0.5, 0.75, 0.97)
TV_SPACING_FT = 1800.0
ZONE_OVERLAP_FT = 150.0   # a collision triangle belongs to every zone with a lane sample this close
WALL_MIN = 82.0           # a2r: generated wall height when no visual barrier is higher
VOID_DROP_FT = 300.0     # no floor within this drop beyond an open edge = void
VOID_WALL_FT = 40.0      # an N64 wall at a void edge must reach this high above the floor to count
TRIGGER_FAR_FT = 1000.0  # a one-shot trigger that hides its object while every pod is still farther than this from the
                         # object (N64 distant-pod distance, n64-loader-re.md 2.7) -> the object is left out [G threshold]
CUT_FPS = 30.0            # cut-camera frames per second (frameCount30 in the port's cut reader)
# N64 surface handling -> Revenge TerrainZone row (notes\n64-ice-physics.md). Original Revenge reads TerrainZone1..8 from
# the .tob only (276880) and ties a COL object to a row by name (TerrainZoneCollision / ZoneCollisionObjectN, 272520);
# it loads SurfaceFriction / SurfaceDrag / SurfaceTraction and never uses them (only GenerateDust is read: 217A90,
# 2CBDB8, 2CC454). Every stock value is 1.0. The converter writes the N64 multipliers there so the port's switchable
# surface-handling enhancement can key on track data:
#   SurfaceTraction = N64 grip target (Slip 0x20 -> 0.2; swrRace_UpdateSurfaceTag N64 0x80066A80, 0x800AD4B4)
#   SurfaceFriction = N64 speed-factor target (Slow 0x8 -> 0.75, Swst 0x10 -> 0.1; 0x800AD4B0)
#   SurfaceDrag     = 1.0 (no N64 counterpart)
#   GenerateDust    = F where the N64 sprays nothing (Mirr 0x1000 / Soft 0x8000, dust-kick 0x80077C94)
#   SurfaceSpeedBonus = N64 speed-bonus target (Fast 0x4 -> 200; 0x80066D04), default 0 [new key, port enhancement]
#   ZeroGravity     = N64 zero-g strip: 0 none, 1 ZOn 0x1, 2 ZOff 0x2, 3 both (notes\n64-zero-g-physics.md) [new key]
#   MagnetGravity   = N64 surface-relative ("magnet") gravity: 1 where the surface word +0x00 has bit 0x20 (swrRace_UpdateSurfaceTag
#                     N64 0x80066A80 / PC 0x00476EA0 -> flags1 0x400), default 0 (notes\n64-magnet-physics.md) [new key]
# The new keys are written only when the track has such a surface; then every row carries them (row 1 default).
# N64 surface tag 'Magnet' = surface word +0x00 bit 0x20 (not a vehicle-reaction bit); it joins the tag so magnet faces get their
# own COL objects (floor and, for the steep / overhanging magnet faces, an own wall COL) that the TerrainZone row can name.
N64_TERRAIN = (('Slip', 'traction', 0.2), ('Swst', 'friction', 0.1), ('Slow', 'friction', 0.75))
N64_SPEED_BONUS = (('Fast', 200.0),)
N64_NO_DUST = ('Mirr', 'Soft')
RUNUP_FT = 250.0          # a map node is a Revenge reset anchor (228BBC -> 2255B0 puts the pod back on the last map node it
                          # crossed): no node closer than this before a gap in the floor on its link, so a reset pod can
                          # build speed before the gap (the N64 respawn search steps past NRSp / Fall ground, 0x80070BBC) [G]
FAR_GRAPH_FT = 1500.0     # widths are clipped against lane samples farther than this along the graph (both directions)
MIN_HEIGHT_FT = 100.0     # lowest collision / racing line / visual point kept at least this far above Revenge's y = 0 reset [G]
MOVER_NEAR_FT = 120.0     # lane nodes this close (horizontally) to moving scenery written in one pose are checked [G]
MOVER_CLEAR_FT = 30.0     # wanted clearance of the AI lane from it at pod height [G]
FLIGHT_WIDTH_CAP_FT = 300.0  # zero-g flight: lane half-width = horizontal clearance to any collision face at the line, capped [G]
# N64 intro FOV (n64-loader-re.md 5): cMan 'Load' sets zoom +0x2B0 = 100 (0x80043F00); the pre-race camera mode 7
# (0x80041258) writes -1 each frame and the viewport keeps its last FOV (0x80086730 only stores a value > 0), so the
# intro runs at 100 deg. That value is HORIZONTAL: the viewport derives the vertical half-angle as
# atan(tan(fov/2) / aspect) (0x80085FB0). N64 aspect 4:3. The port reads cut-camera FOV as vertical
# (terrain_interactive_loop50du.cpp:2348 verticalFovRadians = fovDegrees).
N64_INTRO_FOV_H = 100.0
# N64 airborne gravity (notes n64-gravity.md): gravityScale [0x800A3080] 32.0 x 0.25 = 8 (0x80065E18), fall step per frame
# dt x 30 x 8 x fallVelocity with fallVelocity += dt in the air (ApplyGravity 0x80067558) = 240 N64 units/s^2. Written as
# [Environment] AirborneGravity = 240 / track scale ft/s^2 (port data, absent on stock: pods keep their PHY value); with
# speeds unchanged a scaled jump lasts SCALE times longer, so the pod lands where the N64 lands
N64_AIRBORNE_GRAVITY = 240.0
N64_ASPECT = 4.0 / 3.0
INTRO_FOV = math.degrees(2 * math.atan(math.tan(math.radians(N64_INTRO_FOV_H / 2)) / N64_ASPECT))


def f32(x):
    import struct
    return struct.unpack('<f', struct.pack('<f', float(x)))[0]


def vec(v, fmt='%f'):
    return ','.join(fmt % f32(c) for c in v)


def right_of(fwd):
    """Revenge gate side rule (a2r trk_convert.right_of): RightTrackWidth along (fz, 0, -fx)."""
    f = np.array([fwd[0], 0.0, fwd[2]]); f /= max(np.linalg.norm(f), 1e-9)
    return np.array([f[2], 0.0, -f[0]])


class FloorIndex:
    """Revenge-space up-facing floor triangles on a 64-ft xz grid: heights under (x, z)."""
    CELLS = 64.0

    def __init__(self, T):
        self.T = np.asarray(T, np.float64)
        self.grid = {}
        lo = np.floor(self.T[:, :, [0, 2]].min(1) / self.CELLS).astype(int)
        hi = np.floor(self.T[:, :, [0, 2]].max(1) / self.CELLS).astype(int)
        for i in range(len(self.T)):
            for cx in range(lo[i, 0], hi[i, 0] + 1):
                for cz in range(lo[i, 1], hi[i, 1] + 1):
                    self.grid.setdefault((cx, cz), []).append(i)

    def heights(self, x, z):
        ids = self.grid.get((int(math.floor(x / self.CELLS)), int(math.floor(z / self.CELLS))))
        if not ids:
            return np.zeros(0)
        T = self.T[ids]
        A, B, C = T[:, 0], T[:, 1], T[:, 2]
        d = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])
        with np.errstate(all='ignore'):
            l0 = ((B[:, 2] - C[:, 2]) * (x - C[:, 0]) + (C[:, 0] - B[:, 0]) * (z - C[:, 2])) / d
            l1 = ((C[:, 2] - A[:, 2]) * (x - C[:, 0]) + (A[:, 0] - C[:, 0]) * (z - C[:, 2])) / d
        l2 = 1 - l0 - l1
        m = (l0 >= -1e-6) & (l1 >= -1e-6) & (l2 >= -1e-6) & (np.abs(d) > 1e-9)
        return l0[m] * A[m, 1] + l1[m] * B[m, 1] + l2[m] * C[m, 1]

    def below(self, p, up=30.0, down=200.0):
        h = self.heights(p[0], p[2])
        h = h[(h <= p[1] + up) & (h >= p[1] - down)]
        return float(h.max()) if len(h) else None

    def near(self, x, z, y, tol):
        h = self.heights(x, z)
        h = h[np.abs(h - y) <= tol]
        return float(h[np.argmin(np.abs(h - y))]) if len(h) else None


class WallIndex:
    """wall triangles on a 64-ft xz grid (bbox grown by REACH): distance from a point, inf beyond REACH."""
    CELLS = 64.0
    REACH = 30.0

    def __init__(self, T):
        self.T = np.asarray(T, np.float64)
        self.grid = {}
        lo = np.floor((self.T[:, :, [0, 2]].min(1) - self.REACH) / self.CELLS).astype(int)
        hi = np.floor((self.T[:, :, [0, 2]].max(1) + self.REACH) / self.CELLS).astype(int)
        for i in range(len(self.T)):
            for cx in range(lo[i, 0], hi[i, 0] + 1):
                for cz in range(lo[i, 1], hi[i, 1] + 1):
                    self.grid.setdefault((cx, cz), []).append(i)
        self.grid = {k: np.array(v) for k, v in self.grid.items()}

    def dist(self, p):
        ids = self.grid.get((int(math.floor(p[0] / self.CELLS)), int(math.floor(p[2] / self.CELLS))))
        if ids is None:
            return np.inf
        d = point_tri_dist(np.asarray(p, float), self.T[ids]).min()
        return d if d <= self.REACH else np.inf


def point_tri_dist(p, T):
    """distance from point p (3,) to triangles T (m,3,3) (Ericson, closest point on triangle)."""
    a, b, c = T[:, 0], T[:, 1], T[:, 2]
    ab, ac, ap = b - a, c - a, p - a
    d1 = (ab * ap).sum(1); d2 = (ac * ap).sum(1)
    bp = p - b; d3 = (ab * bp).sum(1); d4 = (ac * bp).sum(1)
    cp = p - c; d5 = (ab * cp).sum(1); d6 = (ac * cp).sum(1)
    va = d3 * d6 - d5 * d4; vb = d5 * d2 - d1 * d6; vc = d1 * d4 - d3 * d2
    with np.errstate(all='ignore'):
        den = 1.0 / (va + vb + vc)
        v = vb * den; w = vc * den
        q = a + ab * v[:, None] + ac * w[:, None]
        r1 = a
        r2 = a + ab * (d1 / (d1 - d3))[:, None]
        r3 = a + ac * (d2 / (d2 - d6))[:, None]
        r4 = b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)))[:, None]
    out = q.copy()
    m = (va <= 0) & ((d4 - d3) >= 0) & ((d5 - d6) >= 0); out[m] = r4[m]
    m = (vb <= 0) & (d2 >= 0) & (d6 <= 0); out[m] = r3[m]
    m = (d6 >= 0) & (d5 <= d6); out[m] = c[m]
    m = (vc <= 0) & (d1 >= 0) & (d3 <= 0); out[m] = r2[m]
    m = (d3 >= 0) & (d4 <= d3); out[m] = b[m]
    m = (d1 <= 0) & (d2 <= 0); out[m] = r1[m]
    out = np.where(np.isfinite(out), out, a)
    return np.linalg.norm(out - p, axis=1)


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


class Conv:
    def __init__(self, model, out_root, code_as=None, uv_scroll=True, hazards=False, phy_class=None):
        self.uv_scroll = uv_scroll
        self.hazards = hazards      # --n64-hazards (n2r_hazards.py): trigger / animation rows instead of the static stand-ins
        self.hz = None
        self.model = model
        self.D = n2r_io.load(model)
        self.name = self.D['track']['name']
        self.natural_code = n2r_space.natural_code(self.name)
        self.code = (code_as or self.natural_code).lower()
        self.override = self.code != self.natural_code
        # PHY class: code-letter rule for a stock code; an add-on course (own code) may name it (dlc.json phy_class)
        if phy_class and self.code in REVENGE_CODES:
            raise ValueError('--phy-class applies to add-on codes only; stock code %s keeps the code-letter rule' % self.code)
        self.cls = (phy_class or physics_class(self.code)).upper()
        self.X = n2r_space.transform_for(os.path.join(self.D['dir'], 'track.json'))
        # heights tied to the N64 racing line, in N64 units, scaled with the track (n2r_space.SCALE): the line runs 21.5 units
        # above its floor; a line point within 80 units above floor counts as on it; pod-height wall tests 8 ft above the floor
        S_ = self.X['scale']
        self.line_hover, self.line_floor_reach, self.pod_test_drop = 21.5 * S_, 80.0 * S_, 21.5 * S_ - 8.0
        self.out = os.path.join(out_root, self.code)
        self.files, self.prov, self.flags, self.report, self.extra = {}, {}, [], {}, {}

    def put(self, name, data, prov):
        self.files[name] = data
        self.prov[name] = prov

    def P(self, p):
        return n2r_space.points(p, self.X)

    # ------------------------------------------------------------ collision classification
    def collision(self):
        """N64 collision exactly as the game tests it (n2r_n64col.py, notes\\n64-loader-re.md 2):
        the faces the descent filter admits for this planet track, each in its N64 winding and with its
        N64 side rule. Revenge collision is one-sided (front only) and splits ground from obstacle by COL
        name, while the N64 ray tests every face both ways:
          * one-sided N64 face (MeshMaterial 0x8): written once, as wound;
          * two-sided N64 face: written twice, both orientations;
          * a written face whose normal points up (N64 nz > 0) is reachable by the N64 ground ray
            (straight down, front faces) -> floor COL ('floor' in the name: ground AND obstacle in Revenge);
          * every other written face (vertical, overhang, ceiling, tunnel roof) -> obstacle-only COL.
        No face is reoriented, dropped or generated."""
        order = n2r_space.winding()
        spl_ = self.D['splines']
        line_ = np.vstack([n2r_io.segment_curve(spl_, p['index'], b, 40) for p in spl_['points'] for b in p['next']])
        C = n2r_n64col.N64Collision(self.model, int(self.D['track']['planet_track_number']), planet=int(self.D['track']['planet_index']),
                                    triggers=self.D['triggers']['triggers'], line=line_, hazards=self.hazards)
        if self.hazards:
            import n2r_hazards
            self.hz = n2r_hazards.Plan(self, C)
            if self.hz.collision_only:
                self.report['n64_hazards_collision_only_animated'] = self.hz.collision_only
        self.n64c = C
        if C.grid:
            self.flags.append('collision regions: this track (planet %d, track %d) uses the N64 position grid (mode %d, 0x80060AC0): mesh-group '
                              'collision masks rewritten from their bounding boxes at load and each pod\'s collision toggles taken from the grid '
                              'cell it is in (0x8006B304), not from the surface under it [A]' % (int(self.D['track']['planet_index']), C.track, C.grid.mode))
        if getattr(C, 'mirrored_reversed', None) or getattr(C, 'facing3_skipped', None):
            mr = getattr(C, 'mirrored_reversed', {}); f3 = getattr(C, 'facing3_skipped', {})
            self.report['collision_facing_rule'] = dict(back_only_reversed={hex(k): v for k, v in mr.items()}, never_hit={hex(k): v for k, v in f3.items()})
            self.flags.append('collision facing rule (N64 0x80017E88): %d faces of material type 0x40 (back faces only) written reversed as '
                              'one-sided; %d faces of type 0x48 (the ray takes neither side) left out [A]' % (sum(mr.values()), sum(f3.values())))
        if C.trigger_effects:
            self.flags.append('trigger set-up / one-shot triggers applied to the collision (race-speed state, pod faster than 150): %s; the '
                              'visual objects stay as at load (Revenge has no data the port plays to hide them) [A state, flag]' % C.trigger_effects)
            self.report['trigger_collision_effects'] = C.trigger_effects
        mtrack = np.array([C.mesh_track_ok(i) for i in range(len(C.mesh_list))])
        keep = mtrack[C.mesh_of] & ~C.degenerate
        self.pose_visuals(C)
        self.lift_above_zero(C, keep)
        T, surf, mesh, n64z = [], [], [], []
        self.mesh_area = {}
        fall_left_out = 0
        for f in np.flatnonzero(keep):
            mi = int(C.mesh_of[f])
            b = C.mesh_list[mi]['behaviour']
            tags = (list(b['surface']) + (['Magnet'] if b.get('word00', 0) & 0x20 else [])) if b else []
            tag = '+'.join(tags) if tags else 'none'
            self.mesh_area[mi] = self.mesh_area.get(mi, 0.0) + 0.5 * float(np.linalg.norm(np.cross(C.F[f, 1] - C.F[f, 0], C.F[f, 2] - C.F[f, 0])))
            if 'Fall' in tags:
                fall_left_out += 1
                continue
            tri = self.P(C.F[f])[order]
            nz = float(C.N[f, 2])
            sides = [(tri, nz)] if C.one_sided[f] else [(tri, nz), (tri[[0, 2, 1]], -nz)]
            for t_, z_ in sides:
                T.append(t_); surf.append(tag); mesh.append(mi); n64z.append(z_)
        if fall_left_out:
            self.report['fall_faces_left_out'] = fall_left_out
            self.flags.append('Fall surface (%d N64 faces): the N64 respawns a pod that is on it (0x80066BA4); Revenge has no such surface but '
                              'resets every racer that drops below y = 0 (228BBC). The Fall faces are left out of the collision so a pod falls on '
                              'below the track and Revenge resets it [A-derived mapping, flag: where other N64 collision lies under a Fall face a '
                              'pod lands there instead]' % fall_left_out)
        self.hidden_visual_meshes = self.far_trigger_hidden(C)
        T = np.array(T); surf = np.array(surf); mesh = np.array(mesh); n64z = np.array(n64z)
        cr = np.cross(T[:, 1] - T[:, 0], T[:, 2] - T[:, 0]); ln = np.linalg.norm(cr, axis=1)
        deg = ln < 1e-6
        ny = np.where(deg, 0.0, cr[:, 1] / np.where(deg, 1.0, ln))
        # the transform keeps the physical side: Revenge ny must equal N64 nz for every face
        assert (np.abs(ny - n64z)[~deg] < 1e-3).all(), 'winding/axis transform does not preserve the face side'
        floor = (~deg) & (ny > 1e-3)
        wall = (~deg) & ~floor
        drive = floor & (ny >= FLOOR_NY)
        n_one = int((keep & C.one_sided).sum()); n_two = int((keep & ~C.one_sided).sum())
        self.report['collision_in'] = dict(n64_faces=int(len(C.F)), admitted_for_track=int(keep.sum()), one_sided=n_one, two_sided=n_two,
                                           written=len(T), floor=int(floor.sum()), obstacle=int(wall.sum()),
                                           obstacle_facing_down=int((wall & (ny < -0.2)).sum()))
        self.flags.append('collision: %d N64 faces admitted for planet track %d by the N64 descent filter (flags2 ALL=0x%x, ANY = region toggles), '
                          '%d one-sided (written as wound) + %d two-sided (written both ways) = %d Revenge faces; %d face up (N64 ground ray '
                          'reaches them) -> floor COLs, %d vertical / overhang / ceiling -> obstacle COLs (%d of them face down: tunnel roofs '
                          'and overhangs, kept as the N64 keeps them). No face reoriented, dropped or generated [A]' % (
                              int(keep.sum()), C.track, n2r_n64col.all_mask(C.track), n_one, n_two, len(T), floor.sum(), wall.sum(),
                              (wall & (ny < -0.2)).sum()))
        self.T, self.surf, self.floor_m, self.wall_m, self.deg = T, surf, floor, wall, deg
        # faces of looping animated scenery written at a lane-clear time of its loop (n2r_anim.lane_clear_poses): the lane
        # tooling keeps the AI lane clear of them (lanes())
        movers = set(i for i, ml in enumerate(C.mesh_list) if (set(ml['chain_nodes']) | {ml['node']['off']}) & getattr(C, 'lane_posed', set()))
        self.mover_m = np.isin(mesh, list(movers)) & ~deg if movers else np.zeros(len(T), bool)
        self.drive_m, self.col_mesh = drive, mesh
        # lane tooling only (node placement, widths, checks): drivable = up-facing with slope <= 53 deg [G],
        # everything else blocks; the COL files use floor_m / wall_m above
        self.block_m = (~deg) & ~drive

    def pose_visuals(self, C):
        """visual meshes under a node that a trigger animates (n2r_anim.py: 101 door at its last key) moved to the same pose as
        their collision (n2r_n64col applies the pose to the collision)."""
        if not getattr(C, 'posed', None):
            return
        import n2r_anim
        maps = n2r_anim.subtree_point_maps(C.M, C.posed)
        moved = []
        for off, A in maps.items():
            meshes = set(C.subtree_meshes(off))
            n_ = 0
            for g in self.D['geometry']:
                if g['name'].startswith('mesh_') and int(g['name'][5:], 16) in meshes:
                    g['P'] = g['P'] @ A[:3, :3] + A[3, :3]; n_ += len(g['P'])
            moved.append(('0x%x' % off, n_))
        self.report['trigger_animation_poses'] = dict(poses=C.posed_report, lane_clear_poses=C.lane_pose_report, visual_triangles_moved=moved)
        self.flags.append('animated nodes written in a pose other than rest (n2r_anim.py): trigger-started, at the state a pod at race speed '
                          'meets %s [A state, flag: a later toggle back is not reproduced]; looping scenery whose collision is in the racing line '
                          'at rest, at the keyframe of its own loop farthest from the line %s [A keyframe, G choice, flag: the N64 keeps it '
                          'moving]; visual triangles moved %s; the collision of the same nodes is posed the same way' % (
                              C.posed_report, C.lane_pose_report, moved))

    def lift_above_zero(self, C, keep):
        """Revenge resets every racer whose height drops below 0 (228BBC -> 2255B0); the N64 has no kill height. The whole track
        (written ground collision, the racing line, the visual geometry) is kept at least MIN_HEIGHT_FT above 0: the base offset
        (N64 z = 0 -> BASE_Y) is raised in 100-ft steps when anything would sit lower (work\\scratch\\zero-g\\NOTES.md 3) [G]."""
        fall = np.array([bool(C.mesh_list[i]['behaviour'] and 'Fall' in (C.mesh_list[i]['behaviour']['surface'] or [])) for i in range(len(C.mesh_list))])
        # faces a pod can rest on: written up-facing (one-sided with N64 nz > 0, two-sided with nz != 0); the foot of a wall
        # reaching down into a void (Beedo's: one mesh to N64 z -1105) is not ground
        up = (C.N[:, 2] > 1e-3) | (~C.one_sided & (np.abs(C.N[:, 2]) > 1e-3))
        k = keep & ~fall[C.mesh_of] & up
        zc = float(C.F[k][:, :, 2].min())
        spl = self.D['splines']
        zs = min(float(n2r_io.segment_curve(spl, p['index'], b, 32)[:, 2].min()) for p in spl['points'] for b in p['next'])
        zv = min(float(g['P'][..., 2].min()) for g in self.D['geometry'])
        y0 = float(self.X['offset'][1])
        lowest = y0 + min(zc, zs, zv)
        lift = 0.0
        if lowest < MIN_HEIGHT_FT:
            lift = math.ceil((MIN_HEIGHT_FT - lowest) / 100.0) * 100.0
            self.X['offset'][1] = y0 + lift
        self.report['height_offset'] = dict(base_y=y0, lift=lift, offset_y=float(self.X['offset'][1]), lowest_collision_y=round(zc + self.X['offset'][1], 1),
                                            lowest_line_y=round(zs + self.X['offset'][1], 1), lowest_visual_y=round(zv + self.X['offset'][1], 1))
        self.flags.append('height: N64 z = 0 -> %.0f ft (base %.0f + lift %.0f); lowest ground collision %.0f ft, racing line %.0f ft, visual %.0f ft, all above '
                          'the %.0f-ft margin over Revenge\'s below-zero reset [G]' % (
                              self.X['offset'][1], y0, lift, zc + self.X['offset'][1], zs + self.X['offset'][1], zv + self.X['offset'][1], MIN_HEIGHT_FT))

    def zero_g_latch(self, segs, state, toggles):
        """N64 zero-g latch along the racing line (n64-zero-g-physics.md 3): each curve sample (96 per segment, as polyline())
        takes the surface the N64 down ray (2 ft above, reach 10,000, exact side rule, descent filter with the current region
        toggles) finds under the line -- in ZON that is the spline-baked mesh under the pod's progress (0x8006B948), outside
        ZON the pod's own ground ray, which for a pod on the line finds the same floor. ZOn -> ZON; then ZOff && ZON -> ZOFF
        (0x80066C5C, 0x80066C78). Returns per-sample ZON flags, end state, end toggles."""
        C = self.n64c
        spl = self.D['splines']
        down = np.array([0.0, 0.0, -1.0])
        out = []
        for a_, b_ in segs:
            for pnt in n2r_io.segment_curve(spl, a_, b_, 96)[:-1]:
                tc = C.pod_toggles(pnt, toggles)
                if tc not in self._zg_mask:
                    self._zg_mask[tc] = C.face_mask(tc)
                h = C.ray(pnt + np.array([0.0, 0.0, 2.0]), down, 10000.0, self._zg_mask[tc])
                beh = C.mesh_list[C.mesh_of[h[1]]]['behaviour'] if h else None
                toggles = n2r_n64col.apply_toggles(toggles, beh)
                tags = (beh['surface'] or []) if beh else []
                if 'ZOn' in tags:
                    state = True
                if 'ZOff' in tags and state:
                    state = False
                out.append(state)
        return np.array(out, bool), state, toggles

    def far_trigger_hidden(self, C):
        """objects a one-shot trigger hides (and strips of collision) while every pod is still more than TRIGGER_FAR_FT
        away: left out of the visuals too. Trigger volume (0x800665A4): a horizontal segment through the centre,
        perpendicular to the direction, half-length size_xy / 2; a pod is inside within size_xy / 2 of it."""
        hidden = set(); rows = []
        groups = {g['name']: g for g in self.D['geometry']}
        for tr in self.D['triggers']['triggers']:
            a = tr.get('affected_node')
            if not a or int(a, 16) not in C.removed or tr['type'] not in n2r_n64col.TRIGGER_REMOVES_COLLISION:
                continue
            meshes = C.subtree_meshes(int(a, 16))
            P = [groups['mesh_%06x' % m]['P'].reshape(-1, 3) for m in meshes if 'mesh_%06x' % m in groups]
            P += [C.F[C.mesh_of == i].reshape(-1, 3) for i, ml in enumerate(C.mesh_list) if ml['mesh']['off'] in meshes]
            P = np.vstack([p for p in P if len(p)])
            c = np.array(tr['center'], float)[:2]; d = np.array(tr['direction'], float)[:2]
            d /= max(np.linalg.norm(d), 1e-9); s = np.array([-d[1], d[0]]); h = tr['size_xy'] / 2.0
            q = P[:, :2] - c
            along = np.clip(q @ s, -h, h)
            dist = float(np.linalg.norm(q - along[:, None] * s[None], axis=1).min() - h)
            far = dist > TRIGGER_FAR_FT
            rows.append((tr['type'], a, round(dist, 1), 'left out' if far else 'drawn'))
            if far:
                hidden |= set(meshes)
        if rows:
            self.report['one_shot_trigger_objects'] = rows
            self.flags.append('one-shot trigger objects (type, node, distance from the trigger volume to the object, visual): %s; objects whose '
                              'trigger fires more than %.0f ft before a pod can reach them are left out (the N64 hides them before any pod '
                              'is near), the others are drawn as at load; their collision is as a pod at race speed meets it [A state, G '
                              'threshold, flag: the break / burst is not reproduced]' % (rows, TRIGGER_FAR_FT))
        return hidden

    # ------------------------------------------------------------ spline graph -> nodes
    def nodes(self):
        spl = self.D['splines']
        pts = spl['points']
        g = spl['graph']
        main = g['main_path']

        topo = {0} | set(g['forks']) | set(g['joins'])
        for br in g['branches']:
            topo |= {br['from_point'], br['to_point']}
        Fn = FloorIndex(self.T[self.drive_m])
        wallT = self.T[self.block_m]
        wc = wallT.mean(1); wtree = cKDTree(wc); wrad = float(np.linalg.norm(wallT - wc[:, None], axis=2).max())

        WI = WallIndex(wallT)

        def clearance(Q):
            """pod-height distance (hover point - 13.5 ft = 8 ft above the floor) to the nearest collision wall
            (inf beyond 30 ft)."""
            return np.array([WI.dist(q - np.array([0, self.pod_test_drop, 0])) for q in Q])

        def polyline(pids, closed):
            """dense Revenge-space curve through N64 points pids (+ back to pids[0] when closed);
            returns curve and the curve index of every point."""
            C, at = [], {}
            segs = list(zip(pids, pids[1:] + ([pids[0]] if closed else [])))
            for a_, b_ in segs:
                c = self.P(n2r_io.segment_curve(spl, a_, b_, 96))
                at.setdefault(a_, len(C))
                C += list(c[:-1])
            if not closed:
                at[pids[-1]] = len(C); C.append(self.P(pts[pids[-1]]['position']))
            C = np.array(C)
            return C, at

        # Preserve the original Bezier approach on both branches through a sliding door.
        # The ordinary chord tolerance and full road width let the pilot cut across a panel.
        panel_rows = [r for r in self.hz.rows if r['kind'] == 307] if self.hazards else []
        self.door_centres = np.array([self.P(self.hz.pivot_n64(r)) for r in panel_rows])
        self.door_approach_ft = NODE_MAX_SPACING * self.X['scale'] / 2

        def door_anchors(curve):
            if not len(self.door_centres):
                return set()
            distances = cKDTree(self.door_centres).query(curve)[0]
            inside = np.flatnonzero(distances <= self.door_approach_ft)
            anchors = set()
            for run in np.split(inside, np.flatnonzero(np.diff(inside) > 1) + 1):
                if len(run):
                    anchors.update((int(run[0]), int(run[distances[run].argmin()]), int(run[-1])))
            return anchors

        def select(C, forced, tol, maxsp, closed):
            """greedy chord selection over the whole path, forced indices kept; then refined so no chord
            runs closer to a wall than the curve itself does (and never closer than 15 ft when the curve is clear)."""
            forced = forced | door_anchors(C)
            n = len(C)
            end = n if closed else n - 1
            seg = np.linalg.norm(np.diff(np.vstack([C, C[:1]]) if closed else C, axis=0), axis=1)
            arc = np.r_[0, np.cumsum(seg)]
            sel, last = [0], 0
            for i in range(1, end + 1):
                ii = i % n if closed else i
                dev = 0.0
                if i - last > 1:
                    a_, b_ = C[last], C[ii]; ch = b_ - a_; L = np.linalg.norm(ch)
                    if L > 0:
                        w = C[last + 1:i] - a_
                        dev = np.linalg.norm(np.cross(w, ch / L), axis=1).max()
                if i == end:
                    break
                if i in forced:
                    sel.append(i); last = i; continue
                if arc[i] - arc[last] >= minsp and (dev > tol or arc[i] - arc[last] > maxsp):
                    k = i - 1 if i - 1 > last else i
                    sel.append(k); last = k
            if closed and arc[n] - arc[sel[-1]] < minsp and sel[-1] not in forced:
                sel.pop()
            # refinement against the walls
            cl, cf, cfl = cache[id(C)]
            span_result = {}     # (a_, b_) -> node to add or None: a span's test depends only on its two ends, so a span
                                 # left unsplit by the previous round is not tested again (same result, far less time)
            for _ in range(60):
                added = False
                bounds_ = sel + [n if closed else n - 1]
                new = []
                for a_, b_ in zip(bounds_, bounds_[1:]):
                    if b_ - a_ < 4:
                        continue
                    if (a_, b_) in span_result:
                        if span_result[(a_, b_)] is not None:
                            new.append(span_result[(a_, b_)])
                        continue
                    span_result[(a_, b_)] = None
                    t = np.linspace(0, 1, max(3, int((arc[b_] - arc[a_]) // (10.0 * S_n))))
                    Q = C[a_] + (C[b_ % n] - C[a_]) * t[:, None]
                    cqs = clearance(Q)
                    span = np.arange(a_, (b_ if b_ < n else n - 1) + 1)
                    near = span[np.argmin(np.linalg.norm(C[span][None, :, :] - Q[:, None, :], axis=2), axis=1)]
                    # a chord sample is bad when it is clearly worse than the curve beside it: closer than
                    # 15 ft to a wall where the curve is not, or off the floor where the curve is on it
                    rr_ = right_of(C[b_ % n] - C[a_])

                    def beside(q):     # floor at the lane's own height within 150 ft to either side
                        return any(Fn.near(*(q + rr_ * sg * d_)[[0, 2]], q[1] - self.line_hover, 30.0) is not None
                                   for d_ in range(8, 150, 8) for sg in (1.0, -1.0))
                    # in zero-g flight the lane is the N64 line itself, not the floor: no off-floor test there
                    offf = np.array([(not cfl[k_]) and Fn.below(q, up=10.0, down=self.line_floor_reach) is None and (cf[k_] or beside(q)) for q, k_ in zip(Q, near)])
                    wbad = (cqs < 15.0) & (cqs < cl[near] - 2.0)
                    if offf.any() or wbad.any():
                        pri = np.where(offf, 1e6, np.where(wbad, np.minimum(cl[near], 1e5) - np.minimum(cqs, 1e5), -1.0))
                        qw = Q[int(np.argmax(pri))]
                        k = a_ + 1 + int(np.argmin(np.linalg.norm(C[a_ + 1:b_] - qw, axis=1)))
                        if min(abs(arc[k] - arc[a_]), abs(arc[b_] - arc[k])) >= 20.0 * S_n:
                            new.append(k)
                            span_result[(a_, b_)] = k
                if not new:
                    break
                sel = sorted(set(sel) | set(new))
            return sel

        # node spacing follows the N64 curve: tolerance and spacings are N64 lengths scaled with the track (n2r_space.SCALE), so a
        # scaled track gets the same node density along its curve as at 1:1 (and stays under MAX_NODES without the retry loop)
        S_n = self.X['scale']
        tol, maxsp, minsp = NODE_TOLERANCE * S_n, NODE_MAX_SPACING * S_n, NODE_MIN_SPACING * S_n
        self.curve_moved = 0
        Cm, at_m = polyline(main, True)
        cache = {}
        # zero-g flight along the line (N64 latch, zero_g_latch): only on a track with ZOn / ZOff floor
        self.has_zero_g = any('ZOn' in s_.split('+') for s_ in set(self.surf[self.floor_m]))
        self._zg_mask = {}
        segs_m = list(zip(main, main[1:] + [main[0]]))
        if self.has_zero_g:
            st_, tg_ = False, n2r_n64col.TOGGLE_INIT
            _, st_, tg_ = self.zero_g_latch(segs_m, st_, tg_)            # lap 1 settles the latch and the region toggles
            fl_m, at_state = [], {}
            for a_, b_ in segs_m:                                         # lap 2, state kept at every point for branches
                at_state[a_] = (st_, tg_)
                f_, st_, tg_ = self.zero_g_latch([(a_, b_)], st_, tg_)
                fl_m.append(f_)
            fl_m = np.concatenate(fl_m)
        else:
            fl_m, at_state = np.zeros(len(Cm), bool), {}

        def prep(C, fl):
            cache[id(C)] = (clearance(C), np.array([Fn.below(c, up=10.0, down=self.line_floor_reach) is not None for c in C]), fl)
        prep(Cm, fl_m)
        bpoly = []
        for bid, br in enumerate(g['branches'], 1):
            Cb, at_b = polyline(br['points'], False)
            if self.has_zero_g:
                st_, tg_ = at_state.get(br['from_point'], (False, n2r_n64col.TOGGLE_INIT))
                fl_b, _, _ = self.zero_g_latch(list(zip(br['points'], br['points'][1:])), st_, tg_)
                fl_b = np.r_[fl_b, fl_b[-1:]]                             # the end point (rejoin) carries the last state
            else:
                fl_b = np.zeros(len(Cb), bool)
            prep(Cb, fl_b)
            bpoly.append((bid, br, Cb, at_b))
        # a node at every zero-g entry and exit sample (where the latch flips)
        edges_m = set(int(i) for i in np.flatnonzero(fl_m != np.roll(fl_m, 1))) if self.has_zero_g else set()
        exits_m = set(i for i in edges_m if not fl_m[i])
        self.zero_g_events = [('enter' if fl_m[i] else 'exit', i, round(float(np.sum(np.linalg.norm(np.diff(Cm[:i + 1], axis=0), axis=1))), 1)) for i in sorted(edges_m)]
        # grid heading: the N64 orients the whole grid by the spline tangent at point 0 (n64-loader-re.md 11);
        # Revenge takes the grid heading from map node 0 and its predecessor (track_start50ad.cpp:82,
        # trk_verify.revenge_grid), and that predecessor must lie more than 200 ft behind node 0 (GRID_PRED_FT).
        # Rule: no node in the last GRID_PRED_FT of the lap; the predecessor sits GRID_PRED_FT behind node 0 on the
        # line through node 0 along the N64 tangent at point 0, so the grid heading is the N64 tangent exactly
        seg_m = np.linalg.norm(np.diff(np.vstack([Cm, Cm[:1]]), axis=0), axis=1)
        back = np.cumsum(seg_m[::-1])
        grid_pred = len(Cm) - 1 - int(np.searchsorted(back, GRID_PRED_FT))
        p0 = pts[main[0]]
        t0 = n2r_space.dirs(np.array(p0['handle_out'], float) - np.array(p0['handle_in'], float), self.X)
        t0 = t0 / max(np.linalg.norm(t0), 1e-9)
        pred_point = Cm[0] - t0 * GRID_PRED_FT
        closing_forced = [at_m[p_] for p_ in main if p_ in topo and at_m[p_] > grid_pred]
        if closing_forced:
            raise ValueError('a fork / join lies within %.0f ft before node 0 (curve samples %s); the grid predecessor rule cannot '
                             'drop it' % (GRID_PRED_FT, closing_forced))
        while True:
            hover, kind, point_of, node_of = [], [], {}, {}
            sel = select(Cm, set(at_m[p_] for p_ in main if p_ in topo) | {grid_pred} | edges_m, tol, maxsp, True)
            sel = [k for k in sel if k <= grid_pred]          # no map node in the last GRID_PRED_FT before node 0
            flight = []
            for k in sel:
                # the exit sample itself is still flown (the pod leaves flight there and follows the line down in Recover): on the line
                hover.append(pred_point if k == grid_pred else Cm[k]); kind.append(0); flight.append(bool(fl_m[k]) or k in exits_m)
            for p_ in main:
                if at_m[p_] in sel:
                    node_of[p_] = sel.index(at_m[p_]); point_of[node_of[p_]] = p_
            K = len(hover)
            nxt = [[(j + 1) % K] for j in range(K)]
            branches = []
            todo = list(bpoly)
            guard = 0
            while todo and guard < 100:
                guard += 1
                bid, br, Cb, at_b = todo.pop(0)
                if br['from_point'] not in node_of:
                    todo.append((bid, br, Cb, at_b)); continue
                p = br['points']
                forced = set(at_b[p_] for p_ in p[1:-1] if p_ in topo)
                fl_b = cache[id(Cb)][2]
                forced |= set(int(i) for i in np.flatnonzero(fl_b[1:] != fl_b[:-1]) + 1)
                sb = select(Cb, forced | {len(Cb) - 1}, tol, maxsp, False)
                sb = [k for k in sb if k != 0 and k != len(Cb) - 1]
                if not sb:
                    sb = [len(Cb) // 2]
                first = len(hover)
                for k in sb:
                    hover.append(Cb[k]); kind.append(bid); flight.append(bool(fl_b[k]) or (0 < k and bool(fl_b[k - 1]) and not fl_b[k]))
                for p_ in p[1:-1]:
                    if at_b[p_] in sb:
                        node_of[p_] = first + sb.index(at_b[p_]); point_of[node_of[p_]] = p_
                last = len(hover) - 1
                for j in range(first, last):
                    nxt.append([j + 1])
                nxt.append(['to', p[-1]])
                nxt[node_of[p[0]]].append(first)
                branches.append(dict(id=bid, from_point=p[0], to_point=p[-1], first=first, last=last))
            if len(hover) <= MAX_NODES:
                break
            if maxsp < 800.0 * S_n:
                maxsp = min(800.0 * S_n, maxsp * 1.1)
            else:
                tol *= 1.25
        for j in range(len(nxt)):
            if nxt[j][0] == 'to':
                nxt[j] = [node_of[nxt[j][1]]]
        self.flags.append('nodes: %d (%d main + %d on %d branches) from the N64 racing-line graph (spline %d %s, %d points, %d forks / %d joins); '
                          'start, fork and join points are nodes; the Bezier path between them is subdivided at chord tolerance %.1f ft / spacing '
                          '<= %.0f ft, plus extra nodes wherever a chord would run closer than 15 ft to a collision wall or leave the floor '
                          '[A graph and curve, G subdivision]' % (
                              len(hover), K, len(hover) - K, len(branches), spl['id'], spl['name'], len(pts), len(g['forks']), len(g['joins']), tol, maxsp))
        self.hover = np.array(hover); self.kind = np.array(kind); self.nxt = nxt; self.K = K
        self.main_sel, self.at_m, self.Cm = list(sel), dict(at_m), Cm      # N64 floor table (n2r_floor.py)
        self.node_flight = np.array(flight, bool)
        if self.has_zero_g:
            self.report['zero_g'] = dict(events_main_curve=self.zero_g_events, flight_nodes=int(self.node_flight.sum()),
                                         flight_main_nodes=[int(j) for j in np.flatnonzero(self.node_flight[:K])])
            self.flags.append('zero-g: N64 latch simulated along the racing line (ZOn -> flight, ZOff -> leave; down ray from the line, '
                              'n64-zero-g-physics.md 3): events (kind, curve sample, ft along the lap) %s; a node at every entry and exit; '
                              '%d nodes in flight sit ON the N64 line (Revenge gate tests are height-limited and resets put a pod on its '
                              'node), every other node on the floor with LineHeight [A latch, G placement]' % (self.zero_g_events, int(self.node_flight.sum())))
        self.branches = branches; self.node_of = node_of; self.point_of = point_of
        prv = [None] * len(hover)
        for j, ns in enumerate(nxt):
            for k, n_ in enumerate(ns):
                if prv[n_] is None or (k == 0 and kind[n_] == kind[j]):
                    prv[n_] = j
        self.prv = prv

    # ------------------------------------------------------------ floor heights, widths, lane shift
    def lanes(self):
        F = FloorIndex(self.T[self.drive_m])
        self.F = F
        walls = self.T[self.block_m]
        self.walls_n64 = walls
        wc = walls.mean(1); self.wall_tree = cKDTree(wc[:, [0, 2]])
        self.wall_rad = np.linalg.norm(walls - wc[:, None], axis=2).max(1)
        N = len(self.hover)
        Pn = self.hover.copy()
        missing = []
        moved = []
        fl_n = self.node_flight
        for j in range(N):
            if fl_n[j]:
                continue                  # zero-g flight: the node stays on the N64 line (Pn = hover)
            y = F.below(self.hover[j], up=15.0, down=250.0 * self.X['scale'])
            if y is None or self.hover[j][1] - y > self.line_floor_reach:
                y0 = y
                # the N64 curve is a guide line and leaves the floor in places (a branch start cutting across the
                # gap between two bridges): move the node sideways onto the floor, AI_WALL_CLEAR_FT inside its edge
                t = self.hover[self.nxt[j][0]] - self.hover[self.prv[j]]
                r = right_of(t); hit = None
                for d in np.arange(4.0, 150.0, 4.0):
                    for sg in (1.0, -1.0):
                        q = self.hover[j] + r * sg * d
                        if F.near(q[0], q[2], self.hover[j][1] - self.line_hover, 30.0) is not None:
                            hit = (sg, d); break
                    if hit:
                        break
                y = y0
                if hit:
                    for extra in (AI_WALL_CLEAR_FT, 10.0, 0.0):
                        q = self.hover[j] + r * hit[0] * (hit[1] + extra)
                        y = F.near(q[0], q[2], self.hover[j][1] - self.line_hover, 30.0)
                        if y is not None:
                            Pn[j] = (q[0], y, q[2]); moved.append(j); break
                if not hit:
                    if y0 is None:
                        missing.append(j); Pn[j, 1] -= self.line_hover
                    else:
                        Pn[j, 1] = y0
            else:
                Pn[j, 1] = y
        self.report['nodes_moved_onto_floor'] = moved
        self.report['nodes_without_floor_below'] = missing
        self.flags.append('node heights: the N64 spline runs about 21.5 ft above its floor (hover height); every node is put on the '
                          'collision floor directly below it; %d nodes with no floor below (or only one more than 80 ft down) but floor beside them at their own height were moved sideways onto it (%s), %d found none '
                          'and were lowered by 21.5 ft [A-derived, G move]' % (len(moved), moved, len(missing)))
        # run-up before gaps: a node whose outgoing link meets a gap (no drivable floor within 60 ft below the lane)
        # less than RUNUP_FT after it is moved back along its incoming link
        runup = []
        protected = {0, self.prv[0]}
        for j in range(N):
            if j in protected or fl_n[j]:
                continue
            for n_ in self.nxt[j]:
                if fl_n[n_]:
                    continue              # a link into zero-g flight is flown, not driven: no floor gap rule
                a, b = Pn[j], Pn[n_]; L = float(np.linalg.norm(b - a))
                gap = next((s_ for s_ in np.arange(10.0, L, 10.0) if F.below(a + (b - a) * s_ / L + np.array([0, 0.0, 0]), up=15.0, down=60.0) is None), None)
                if gap is None or gap >= RUNUP_FT:
                    continue
                pj = Pn[self.prv[j]]; back = pj - Pn[j]; lb = float(np.linalg.norm(back))
                move = min(RUNUP_FT - gap, lb - NODE_MIN_SPACING * self.X['scale'])
                if move <= 5.0:
                    continue
                q = Pn[j] + back / lb * move
                y = F.near(q[0], q[2], q[1], 30.0)
                if y is None:
                    continue
                runup.append((j, round(float(gap)), round(float(move))))
                Pn[j] = (q[0], y, q[2])
                break
        self.report['nodes_moved_for_runup'] = runup
        if runup:
            self.flags.append('run-up: %d nodes moved back so that no map node (Revenge reset anchor) lies less than %.0f ft before a gap in '
                              'the floor on its link (node, gap ft, moved ft): %s [G]' % (len(runup), RUNUP_FT, runup))
        fwd = np.array([Pn[self.nxt[j][0]] - Pn[self.prv[j]] for j in range(N)])
        R = np.array([right_of(f) for f in fwd])
        ext_l = np.zeros(N); ext_r = np.zeros(N)
        Tall = self.T[~self.deg]
        Tc = Tall.mean(1); Tr = np.linalg.norm(Tall - Tc[:, None], axis=2).max(1)
        for j in range(N):
            if fl_n[j]:
                # zero-g flight: no floor to walk; the half-width is the horizontal clearance from the line to any collision
                # face (tube wall, floor, ceiling), capped at FLIGHT_WIDTH_CAP_FT [G]
                ext_r[j] = self.air_clear(Pn[j], R[j], Tall, Tc, Tr); ext_l[j] = self.air_clear(Pn[j], -R[j], Tall, Tc, Tr)
                continue
            ext_r[j] = self.walk(Pn[j], R[j]); ext_l[j] = self.walk(Pn[j], -R[j])
        if fl_n.any():
            self.report['zero_g']['flight_half_widths_ft'] = dict(left=np.round(ext_l[fl_n]).tolist(), right=np.round(ext_r[fl_n]).tolist())
        # keep the lane AI_WALL_CLEAR_FT inside the floor
        shifted = 0
        PA = Pn.copy()
        for j in range(N):
            if fl_n[j]:
                continue                  # the N64 line is kept exactly in flight
            lo, hi = -ext_l[j], ext_r[j]
            s = 0.0
            if hi - lo < 2 * AI_WALL_CLEAR_FT:
                s = (lo + hi) / 2
            elif hi < AI_WALL_CLEAR_FT:
                s = hi - AI_WALL_CLEAR_FT
            elif lo > -AI_WALL_CLEAR_FT:
                s = lo + AI_WALL_CLEAR_FT
            if s:
                q = Pn[j] + R[j] * s
                y = F.near(q[0], q[2], Pn[j][1], 15.0)
                PA[j] = (q[0], y if y is not None else Pn[j][1], q[2]); shifted += 1
                ext_r[j] -= s; ext_l[j] += s
        self.flags.append('lane: %d nodes moved sideways to stay %.0f ft inside the drivable floor (N64 racing line kept elsewhere) [G]' % (shifted, AI_WALL_CLEAR_FT))
        # moving scenery written in one pose (n2r_anim.lane_clear_poses): the N64 line passes it within a few ft because the
        # N64 obstacle moves on; a static copy in the lane stops Revenge pods for good. Nodes within MOVER_NEAR_FT of it are
        # moved sideways (inside the floor, AI_WALL_CLEAR_FT from its edge) to the offset with the most clearance at pod
        # height, MOVER_CLEAR_FT wanted [G lane tooling]
        mv = []
        if self.mover_m.any():
            MT = self.T[self.mover_m]; MC = MT.mean(1)
            for j in range(N):
                if fl_n[j] or j in (0, self.prv[0]):
                    continue
                if np.linalg.norm((MC - PA[j])[:, [0, 2]], axis=1).min() > MOVER_NEAR_FT:
                    continue

                def clr(q):
                    return min(float(point_tri_dist(q + np.array([0, h_, 0]), MT).min()) for h_ in (4.0, 12.0, 20.0))
                c0 = clr(PA[j])
                if c0 >= MOVER_CLEAR_FT:
                    continue
                cand = [(c0, 0.0)]
                for s in np.arange(-(ext_l[j] - AI_WALL_CLEAR_FT), ext_r[j] - AI_WALL_CLEAR_FT + 1e-6, 4.0):
                    q = PA[j] + R[j] * s
                    y = F.near(q[0], q[2], PA[j][1], 15.0)
                    if y is not None:
                        cand.append((clr(np.array([q[0], y, q[2]])), float(s)))
                ok_ = [c_ for c_ in cand if c_[0] >= MOVER_CLEAR_FT]
                best = min(ok_, key=lambda c_: abs(c_[1])) if ok_ else max(cand)
                if best[1]:
                    q = PA[j] + R[j] * best[1]
                    PA[j] = (q[0], F.near(q[0], q[2], PA[j][1], 15.0), q[2])
                    ext_r[j] -= best[1]; ext_l[j] += best[1]
                    mv.append((j, round(c0, 1), round(best[0], 1), round(best[1], 1)))
            self.report['nodes_moved_clear_of_moving_scenery'] = mv
            self.flags.append('lane: %d nodes moved sideways clear of moving scenery written in one pose (node, clearance before, after, '
                              'offset ft): %s [G]' % (len(mv), mv))
        self.PA, self.R, self.ext_l, self.ext_r = PA, R, ext_l, ext_r
        # dense lane samples (zones, portals, checks)
        S, Sn, Sk, St = [], [], [], []
        for j in range(N):
            for n_ in self.nxt[j]:
                a, b = PA[j], PA[n_]
                m = max(1, int(np.linalg.norm(b - a) // 25))
                for t in range(m):
                    S.append(a + (b - a) * t / m); Sn.append(j); Sk.append(n_); St.append(t / m)
        self.S = np.array(S); self.Sn = np.array(Sn); self.Sk = np.array(Sk); self.St = np.array(St)

    @staticmethod
    def air_clear(p, d, T, Tc, Tr, cap=FLIGHT_WIDTH_CAP_FT):
        """distance along the horizontal ray p + t d to the nearest collision face (either side), at most cap."""
        ids = np.flatnonzero(np.linalg.norm(Tc - (p + d * cap / 2), axis=1) <= Tr + cap / 2 + 1)
        if not len(ids):
            return cap
        A = T[ids]
        e1 = A[:, 1] - A[:, 0]; e2 = A[:, 2] - A[:, 0]
        pv = np.cross(d, e2); det = (e1 * pv).sum(1)
        with np.errstate(all='ignore'):
            inv = 1.0 / det; tv = p - A[:, 0]
            u = (tv * pv).sum(1) * inv; qv = np.cross(tv, e1); v = (qv @ d) * inv; t = (qv * e2).sum(1) * inv
        ok = (np.abs(det) > 1e-9) & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > 0) & (t <= cap)
        return float(t[ok].min()) if ok.any() else cap

    def walk(self, p, d, limit=1500.0, step=4.0):
        """distance from p along horizontal d over continuous floor (height within 15 ft of the
        previous step), stopping at a collision wall crossed at pod height."""
        s, y = 0.0, p[1]
        while s < limit:
            q = p + d * (s + step)
            y2 = self.F.near(q[0], q[2], y, 15.0)
            if y2 is None:
                break
            a = np.array([p[0] + d[0] * s, y + 8.0, p[2] + d[2] * s]); b = np.array([q[0], y2 + 8.0, q[2]])
            ids = self.wall_tree.query_ball_point([(a[0] + b[0]) / 2, (a[2] + b[2]) / 2], r=step + 400.0)
            if ids:
                ids = np.array(ids)
                ids = ids[np.linalg.norm(self.walls_n64[ids].mean(1)[:, [0, 2]] - b[[0, 2]], axis=1) <= self.wall_rad[ids] + step]
                if len(ids) and seg_hits_tris(a, b, self.walls_n64[ids]).any():
                    break
            s += step; y = y2
        return s

    # ------------------------------------------------------------ zones
    def zones(self):
        K, N = self.K, len(self.PA)
        P = self.PA
        seg = np.array([np.linalg.norm(P[(j + 1) % K] - P[j]) for j in range(K)])
        arc = np.r_[0, np.cumsum(seg)]; L = arc[-1]
        self.arc, self.lap_main = arc, L
        # main index of a node: main nodes are 0..K-1; a branch maps to its main fork / rejoin
        bmain_from, bmain_to = {}, {}

        def main_from(j):
            while j >= K:
                br = [b for b in self.branches if b['first'] <= j <= b['last']][0]
                j = self.node_of[br['from_point']]
            return j

        def main_to(j):
            while j >= K:
                br = [b for b in self.branches if b['first'] <= j <= b['last']][0]
                j = self.node_of[br['to_point']]
            return j
        unsafe = np.zeros(K, bool)
        for br in self.branches:
            a = main_from(self.node_of[br['from_point']]); b = main_to(self.node_of[br['to_point']])
            bmain_from[br['id']], bmain_to[br['id']] = a, b
            k = (a + 1) % K
            while k != b:
                unsafe[k] = True; k = (k + 1) % K
        # no zone boundary inside zero-g flight (nodes on the line high above the floor; the portal is sized at the lane)
        unsafe |= self.node_flight[:K]
        self.unsafe = unsafe
        # ---- zones and portals: vertical slabs on the 2016-ft geometry grid, each portal the whole plane across the world
        # (racer-arcade\tools\a2r\trk_slabs.py, the arcade converter's rule; notes\arcade-visibility.md). Revenge draws a
        # zone only through the portal chain from the camera's zone, clipped to the portal footprints (27F650 / 27F930 /
        # 2805B4); with lane-crossing portals sized at the road, scenery seen across open ground or behind a zone boundary
        # was cut away (Baroo Coast 11 %, Bumpy's 22 % of visible scenery, work\n3\vis_loss.py). With slabs every line of
        # sight between zones passes through whole-plane portals in order and no visual piece crosses a plane (n2r_geo
        # clips every piece to the same grid). The N64 itself culls nothing by Revenge zone (its region toggles are kept
        # for collision membership below). One rule for every track [G layout].
        bb = np.array(self.D['track']['bbox_visual'], float)
        corners = np.array([[x, y, z] for x in bb[:, 0] for y in bb[:, 1] for z in bb[:, 2]])
        vis_pts = np.vstack([self.P(corners), self.T.reshape(-1, 3), P])
        cell_lo = np.floor(vis_pts[:, [0, 2]].min(0) / trk_slabs.CELL).astype(int)
        cell_hi = np.floor(vis_pts[:, [0, 2]].max(0) / trk_slabs.CELL).astype(int)
        # point sets that must each stay in one zone: the start area (grid, cuts 2 / 3) and the N64 intro eye path (cut 1)
        gpts = [P[0]]
        back, j = 0.0, 0
        while back < START_GROUP_BACK_FT:
            jp = self.prv[j]
            back += float(np.linalg.norm(P[j] - P[jp]))
            for q_ in (P[jp], (P[jp] + P[j]) / 2):
                f_ = P[j] - P[jp]; f_[1] = 0; f_ /= max(np.linalg.norm(f_), 1e-9); r_ = np.array([f_[2], 0, -f_[0]])
                gpts += [q_ + r_ * START_GROUP_SIDE_FT, q_ - r_ * START_GROUP_SIDE_FT]
            j = jp
            if j == 0:
                break
        intro = self.D['cameras']['intro']
        ek = intro_eye_keys(intro)
        eye_path = self.P(np.array(ek['positions'], float))
        groups = [np.array(gpts), eye_path]
        slab_log = []
        slabs = trk_slabs.choose(cell_lo, cell_hi, P, groups, self.T, P[0], log=slab_log)
        if slabs.Z > trk_slabs.MAX_ZONES:
            raise ValueError('%d zones > %d' % (slabs.Z, trk_slabs.MAX_ZONES))
        self.slabs = slabs
        Z = slabs.Z
        node_zone = np.asarray(slabs.zone(P), int)
        self.Z, self.node_zone = Z, node_zone
        self.bounds = []
        self.sample_zone = np.asarray(slabs.zone(self.S), int)
        self.report['zone_layout'] = dict(axis='xyz'[slabs.axis], planes=slabs.planes.tolist(), zone_of_slab=slabs.zone_of_slab.tolist(),
                                          candidates=slab_log)
        self.flags.append('zones: %d vertical slabs between the planes %s = %s ft (lines of the 2016-ft geometry grid; zone 1 holds map '
                          'node 0; no plane within %.0f ft of a spline node, through the start area (%.0f ft behind node 0, +-%.0f ft) or '
                          'through the N64 intro eye path); collision faces within %.0f ft past a plane are in both zones '
                          '(trk_slabs.py, the arcade rule) [G layout]' % (
                              Z, 'xyz'[slabs.axis], [round(float(c)) for c in slabs.planes], trk_slabs.NODE_CLEAR_FT,
                              START_GROUP_BACK_FT, START_GROUP_SIDE_FT, trk_slabs.COL_MARGIN_FT))
        self.sample_tree = cKDTree(self.S)
        # lane samples (+ in zero-g flight the floor straight below them) with their slab zones: the near-lane part of the
        # collision membership rule (tri_zones)
        ZS, ZZ = [self.S], [self.sample_zone]
        flight_s = self.node_flight[self.Sn] & self.node_flight[self.Sk]
        if flight_s.any():
            Fall_ = FloorIndex(self.T[self.floor_m])
            fp = []
            for i in np.flatnonzero(flight_s):
                y = Fall_.below(self.S[i], up=0.0, down=10000.0)
                if y is not None:
                    fp.append((i, (self.S[i][0], y, self.S[i][2])))
            if fp:
                ZS.append(np.array([q for _, q in fp])); ZZ.append(self.sample_zone[[i for i, _ in fp]])
            self.report['zero_g']['zone_floor_samples'] = len(fp)
        self.zone_S = np.vstack(ZS); self.zone_Sz = np.concatenate(ZZ); self.zone_tree = cKDTree(self.zone_S)
        self.zones_with_lanes = set(int(z) for z in self.sample_zone)

    def zone_of_points(self, Q):
        """the slab zone of each point (visual pieces, cut cameras)"""
        return np.asarray(self.slabs.zone(np.asarray(Q, float)), int)

    def overlap_zones(self, T):
        """the slab zones each triangle overlaps, COL_MARGIN_FT past a plane included (trk_slabs, floor continues past a portal)"""
        ax = self.slabs.axis
        lo = T[:, :, ax].min(1); hi = T[:, :, ax].max(1)
        return [set(self.slabs.zones_overlapping(lo[t], hi[t], trk_slabs.COL_MARGIN_FT)) for t in range(len(T))]

    def tri_zones(self, T):
        out = []
        cen = T.mean(1); rad = np.linalg.norm(T - cen[:, None], axis=2).max(1)
        _, near = self.zone_tree.query(cen)
        for t in range(len(T)):
            ids = self.zone_tree.query_ball_point(cen[t], r=rad[t] + ZONE_OVERLAP_FT)
            zs = {int(self.zone_Sz[near[t]])}
            if ids:
                ids = np.array(ids)
                d = np.array([point_tri_dist(self.zone_S[i], T[t:t + 1])[0] for i in ids])
                zs |= set(int(z) for z in self.zone_Sz[ids[d < ZONE_OVERLAP_FT]])
            out.append(zs)
        return out

    # ------------------------------------------------------------ walls
    def regions(self):
        """N64 region toggles along every spline link (n64-loader-re.md 2.3), exactly as the game updates them for
        a pod: start 0xFFFFFF00, then at each step the surface found by the N64 ground ray (straight down, front
        faces, descent filter with ANY = current toggles) applies (t & ~b[0x34]) | b[0x38]. Three laps over the
        whole graph; every state met on each link is kept. Also records the camera fog / light state the same
        surfaces set (cMan 0x80040CE8). -> per Revenge zone: the set of toggle states, fog and light records."""
        C = self.n64c
        spl = self.D['splines']; pts = spl['points']
        down = np.array([0.0, 0.0, -1.0])
        mask_cache = {}

        def mask(t):
            if t not in mask_cache:
                mask_cache[t] = C.face_mask(t)
            return mask_cache[t]
        from collections import deque, defaultdict
        samples = []          # (revenge point, toggles, behaviour or None)
        edge_cache = {}
        q = deque([(0, n2r_n64col.TOGGLE_INIT, 0)]); seen = set()
        while q:
            a, t, lap = q.popleft()
            if (a, t, lap) in seen or lap > 2:
                continue
            seen.add((a, t, lap))
            for b in pts[a]['next']:
                key = (a, b, t)
                if key not in edge_cache:
                    tt, out = t, []
                    for pnt in n2r_io.segment_curve(spl, a, b, 40)[1:]:
                        tc = C.pod_toggles(pnt, tt)
                        h = C.ray(pnt + np.array([0, 0, 10.0]), down, 10000.0, mask(tc))
                        beh = C.mesh_list[C.mesh_of[h[1]]]['behaviour'] if h else None
                        tt = n2r_n64col.apply_toggles(tt, beh)
                        out.append((pnt, C.pod_toggles(pnt, tt), beh, h is not None))
                    edge_cache[key] = (tt, out)
                tt, out = edge_cache[key]
                samples += out
                q.append((b, tt, lap + (1 if b == 0 else 0)))
        fsamples = []
        q = deque([(0, n2r_n64col.TOGGLE_INIT, None, 0)]); seen = set()
        while q:
            a, t, cam, lap = q.popleft()
            if (a, t, cam, lap) in seen or lap > 2:
                continue
            seen.add((a, t, cam, lap))
            for b in pts[a]['next']:
                tt, out = edge_cache[(a, b, t)]
                for pnt, _, beh, _ in out:
                    if beh is not None and beh['fog_flags'] & 3:
                        c0, s0, e0 = cam or (tuple(beh['fog_color']), beh['fog_start'], beh['fog_end'])
                        if beh['fog_flags'] & 1:
                            c0, s0 = tuple(beh['fog_color']), beh['fog_start']
                        if beh['fog_flags'] & 2:
                            e0 = beh['fog_end']
                        cam = (c0, s0, e0)
                    if lap >= 1 and cam is not None:      # lap 1 onwards: no start-up state from before the first record
                        fsamples.append((pnt, cam))
                q.append((b, tt, cam, lap + (1 if b == 0 else 0)))
        fogst = defaultdict(lambda: defaultdict(int))
        from collections import Counter
        self.fog_lane = Counter(cam for _, cam in fsamples)
        if fsamples:
            for (pnt, cam), z in zip(fsamples, self.zone_of_points(np.array([self.P(f_[0]) for f_ in fsamples]))):
                fogst[int(z)][cam] += 1
        P = np.array([self.P(s_[0]) for s_ in samples])
        Zs = self.zone_of_points(P)
        zs = defaultdict(set); fog = defaultdict(lambda: defaultdict(int)); light = defaultdict(lambda: defaultdict(int))
        for (pnt, tt, beh, hit), z in zip(samples, Zs):
            zs[int(z)].add(tt)
            if beh is not None:
                if beh['fog_flags'] & 1:
                    fog[int(z)][(tuple(beh['fog_color']), beh['fog_start'], beh['fog_end'])] += 1
                if beh['light_flags']:
                    light[int(z)][(tuple(beh['ambient_color']), tuple(beh['light_color']), tuple(beh['light_vector']), beh['light_flags'])] += 1
        self.zone_states = {z: zs[z] for z in range(1, self.Z + 1)}
        self.zone_fog_records = {z: (max(fog[z].items(), key=lambda kv: kv[1])[0] if fog[z] else None) for z in range(1, self.Z + 1)}
        # the fog the N64 camera actually holds along the lanes of each zone: the most common state per zone
        self.zone_fog = {z: (max(fogst[z].items(), key=lambda kv: kv[1])[0] if fogst[z] else self.zone_fog_records[z]) for z in range(1, self.Z + 1)}
        self.report['zone_fog_states'] = {z: sorted([[list(k[0]), k[1], k[2], v] for k, v in fogst[z].items()], key=lambda r: -r[3])[:4]
                                          for z in range(1, self.Z + 1)}
        self.report['zone_fog_old_rule'] = {z: (list(v[0]), v[1], v[2]) if v else None for z, v in self.zone_fog_records.items()}
        self.zone_light = {z: (max(light[z].items(), key=lambda kv: kv[1])[0] if light[z] else None) for z in range(1, self.Z + 1)}
        self.report['region_states_per_zone'] = {z: sorted('0x%08x' % t for t in self.zone_states[z]) for z in self.zone_states}
        self.report['spline_samples_without_n64_ground'] = int(sum(1 for s_ in samples if not s_[3]))
        nm = len(C.mesh_list)
        self.zone_mesh_active = {z: np.array([any(C.mesh_active(i, t) for t in self.zone_states[z]) for i in range(nm)]) for z in self.zone_states}
        self.flags.append('zones x N64 regions: the N64 toggles collision/visual regions by the surface under the pod; simulated over 3 laps of '
                          'the whole spline graph (%d distinct states); a collision face is written into every Revenge zone in which its '
                          'N64 region is switched on at some point of the lanes of that zone [A]' % len(set().union(*self.zone_states.values())))

    def walls(self):
        T = self.T
        wall_ids = np.flatnonzero(self.wall_m)
        S2 = cKDTree(self.S[:, [0, 2]])
        # every N64 obstacle face as the N64 winds it (one-sided) or both ways (two-sided, see collision())
        out = [(t, T[t]) for t in wall_ids]
        self.n64_walls = out
        self.report['ceilings_dropped'] = 0
        # open floor edges near the lanes: generate a vertical wall only where nothing stops a pod
        Fl = T[self.drive_m]
        edges = {}
        for i, tri in enumerate(Fl):
            for k in range(3):
                a_, b_ = tuple(np.float32(tri[k]).tolist()), tuple(np.float32(tri[(k + 1) % 3]).tolist())
                edges.setdefault(frozenset((a_, b_)), []).append((i, k))
        WT = np.array([w for _, w in out]) if out else np.zeros((0, 3, 3))
        wtree = cKDTree(WT.mean(1)[:, [0, 2]]) if len(WT) else None
        wrad = np.linalg.norm(WT - WT.mean(1)[:, None], axis=2).max(1) if len(WT) else None
        gen, stats = [], dict(open_edges=0, near_track=0, seam=0, covered_by_n64_wall=0, transverse=0, generated=0, off_track=0, void_side=0, void_transverse=0)
        F = self.F
        self.open_edges = []
        for e, own in edges.items():
            if len(own) != 1:
                continue
            stats['open_edges'] += 1
            i, k = own[0]
            tri = Fl[i]; a = tri[k]; b = tri[(k + 1) % 3]; third = tri[(k + 2) % 3]
            mid = (a + b) / 2
            dd, si = self.sample_tree.query(mid)
            if dd > 400 or abs(self.S[si][1] - mid[1]) > 150:
                stats['off_track'] += 1; continue
            stats['near_track'] += 1
            ed = (b - a); ed[1] = 0; el = np.linalg.norm(ed)
            if el < 1e-6:
                continue
            ed /= el
            outn = np.array([ed[2], 0, -ed[0]])
            if outn @ (third - mid) > 0:
                outn = -outn
            # beyond the edge: floor at the same height (seam), a lower floor (ledge), or nothing within
            # VOID_DROP_FT (void: a pod going over falls out of the world, which the port's AI cannot recover)
            seam = cover = void = 0
            for t_ in (0.25, 0.5, 0.75):
                q = a + (b - a) * t_
                pr = q + outn * 4.0
                if F.near(pr[0], pr[2], q[1], 6.0) is not None:
                    seam += 1; continue
                pv = q + outn * 12.0
                hb = F.heights(pv[0], pv[2])
                is_void = not ((hb <= q[1] + 6.0) & (hb >= q[1] - VOID_DROP_FT)).any()
                void += is_void
                need = VOID_WALL_FT if is_void else 8.0
                if wtree is not None:
                    ids = wtree.query_ball_point(q[[0, 2]], r=float(wrad.max()) + 10)
                    if ids:
                        ids = np.array(ids)
                        dd_ = point_tri_dist(q + np.array([0, 8.0, 0]), WT[ids])
                        hit = ids[dd_ < 10.0]
                        if len(hit) and (WT[hit][:, :, 1].max() > q[1] + need).any():
                            cover += 1
            if seam >= 2:
                stats['seam'] += 1; continue
            if seam + cover >= 2:
                stats['covered_by_n64_wall'] += 1; continue
            j = self.Sn[si]; f = self.PA[self.nxt[j][0]] - self.PA[j]; f[1] = 0; f /= max(np.linalg.norm(f), 1e-9)
            if abs(ed @ f) < 0.5:
                # an edge across the direction of travel: a jump lip when floor lies ahead (landing), else a void edge
                m_ = (a + b) / 2
                landing = any(len([h for h in F.heights(*(m_ + f * d_)[[0, 2]]) if m_[1] - VOID_DROP_FT <= h <= m_[1] + 30]) for d_ in range(20, 400, 20))
                if landing or void < 2:
                    stats['transverse'] += 1; self.open_edges.append((a, b, 'transverse')); continue
                stats['void_transverse'] += 1
            elif void >= 2:
                stats['void_side'] += 1
            # the N64 has nothing here: a pod that leaves the floor falls (N64 respawn rules, n64-loader-re.md 6);
            # recorded for the checks, no wall is generated
            stats['generated'] += 1
            self.open_edges.append((a, b, 'open_void' if void >= 2 else 'open_side'))
        self.gen_walls = gen
        self.report['open_edges'] = stats
        self.flags.append('open floor edges within 400 ft of a lane (diagnostic, nothing generated): %d; %d seams between N64 meshes, '
                          '%d with an N64 obstacle face at their foot, %d across the direction of travel with floor ahead or below (ledges / jump lips); '
                          '%d with nothing beside them in the N64 collision either (%d side edges over a void, %d cross edges over a void): '
                          'the N64 lets a pod fall there too [A]' % (
                              stats['near_track'], stats['seam'], stats['covered_by_n64_wall'], stats['transverse'], stats['generated'],
                              stats['void_side'], stats['void_transverse']))

    # ------------------------------------------------------------ main
    def run(self):
        self.collision()
        self.nodes()
        self.lanes()
        self.zones()
        self.regions()
        self.walls()
        self.splines()
        self.collision_files()
        self.visuals()
        self.sky()
        self.portals_scene()
        self.cameras()
        self.donors()
        import n2r_floor
        n2r_floor.write(self)

    def splines(self):
        N, K = len(self.PA), self.K
        P = self.PA
        nxt = self.nxt
        # distances to finish: main nodes along the main loop; branch nodes to their rejoin
        dtf = np.full(N, np.nan)
        acc = 0.0
        dtf[0] = 0.0
        for j in range(K - 1, 0, -1):
            acc += np.linalg.norm(P[(j + 1) % K] - P[j]); dtf[j] = acc
        for _ in range(N):
            for j in range(K, N):
                if np.isnan(dtf[j]) and not np.isnan(dtf[nxt[j][0]]):
                    dtf[j] = dtf[nxt[j][0]] + np.linalg.norm(P[nxt[j][0]] - P[j])
        assert not np.isnan(dtf).any()
        self.dtf = dtf
        lap = float(np.sum([np.linalg.norm(P[(j + 1) % K] - P[j]) for j in range(K)]))
        self.lap = lap
        speed = np.zeros(N)
        for j in range(N):
            a = (P[j] - P[self.prv[j]])[[0, 2]]; b = (P[nxt[j][0]] - P[j])[[0, 2]]
            ang = math.acos(max(-1, min(1, a @ b / max(np.linalg.norm(a) * np.linalg.norm(b), 1e-9))))
            rad = 0.5 * (np.linalg.norm(a) + np.linalg.norm(b)) / max(ang, 1e-6)
            speed[j] = 300.0 if rad < 300 else 400.0 if rad < 500 else 600.0
        self.flags.append('SpeedLimit: G (a2r rule: 600; 400 below 500 ft turn radius; 300 below 300 ft)')
        # widths
        ai_l = np.maximum(self.ext_l - AI_WALL_CLEAR_FT, AI_MIN_HALF); ai_r = np.maximum(self.ext_r - AI_WALL_CLEAR_FT, AI_MIN_HALF)
        map_l = self.ext_l + MAP_MARGIN_FT; map_r = self.ext_r + MAP_MARGIN_FT
        # clip to half the distance to other parts of the course (lane samples far along the graph)
        far_clip = 0
        # graph distance (both directions) between nodes: a lane sample is 'another part of the course' only when
        # neither end of its link lies within FAR_GRAPH_FT of node j along the graph (the old distance-to-finish test
        # treated branch nodes next to their fork as far, because branch distances run via the rejoin node)
        import heapq
        adj = {k: [] for k in range(N)}
        for a_ in range(N):
            for b_ in nxt[a_]:
                w_ = float(np.linalg.norm(P[b_] - P[a_])); adj[a_].append((b_, w_)); adj[b_].append((a_, w_))
        near = []
        for j in range(N):
            dist_ = {j: 0.0}; hq = [(0.0, j)]
            while hq:
                d_, k = heapq.heappop(hq)
                if d_ > dist_.get(k, 1e18) or d_ > FAR_GRAPH_FT:
                    continue
                for k2, w_ in adj[k]:
                    if d_ + w_ < dist_.get(k2, 1e18):
                        dist_[k2] = d_ + w_; heapq.heappush(hq, (d_ + w_, k2))
            near.append({k for k, d_ in dist_.items() if d_ <= FAR_GRAPH_FT})
        for j in range(N):
            ids = self.sample_tree.query_ball_point(P[j], r=float(max(map_l[j], map_r[j]) * 2 + 10))
            ids = [i for i in ids if self.Sn[i] not in near[j] and self.Sk[i] not in near[j]]
            if ids:
                d = np.linalg.norm(self.S[ids] - P[j], axis=1).min()
                for arr in (map_l, map_r, ai_l, ai_r):
                    if arr[j] > 0.5 * d:
                        arr[j] = max(0.5 * d, 8.0); far_clip += 1
        if len(self.door_centres):
            # Keep the original two lines distinct. Do not give passing AI a lateral target
            # anywhere on the road through the panels: that surface includes their swept paths.
            approach = cKDTree(self.door_centres).query(P)[0] <= self.door_approach_ft + self.pod_test_drop
            ai_l[approach] = np.minimum(ai_l[approach], AI_MIN_HALF)
            ai_r[approach] = np.minimum(ai_r[approach], AI_MIN_HALF)
            self.report['sliding_door_ai_nodes'] = np.flatnonzero(approach).tolist()
        self.ai_l, self.ai_r, self.map_l, self.map_r = ai_l, ai_r, map_l, map_r
        self.flags.append('widths: floor cross-section walked from each node (stops at floor end or an N64 wall); AI = that '
                          'minus %.0f ft (at least %.0f), map = that + %.0f ft; %d values clipped to half the distance to other '
                          'track parts [A-derived, G margins]' % (AI_WALL_CLEAR_FT, AI_MIN_HALF, MAP_MARGIN_FT, far_clip))
        # progress nodes: main nodes where no branch runs alongside, near the a2r lap fractions
        sfrac = (lap - dtf[:K]) % lap / lap; sfrac[0] = 0
        safe = [j for j in range(1, K) if not self.unsafe[j]]
        prog = sorted(set(min(safe, key=lambda j: abs(sfrac[j] - f)) for f in PROGRESS_FRACTIONS))
        self.prog = prog
        self.flags.append('ProgressNode: 5 lap fractions %s -> main nodes %s, only where no branch runs alongside (a pod on any '
                          'branch passes all of them) [G]' % (PROGRESS_FRACTIONS, prog))

        # LineHeight (new per-node key, zero-g tracks only; the port's zero-g line table, native_zero_gravity50zg.h): height of
        # the N64 racing line above the node, ft; 0 on flight nodes (they sit on the line). Original Revenge and the port's
        # spline readers read only Position / NumNextNodes / Next* (2A1260) and ignore it.
        line_h = np.where(self.node_flight, 0.0, np.clip(self.hover[:, 1] - P[:, 1], 0.0, 5000.0))
        if self.has_zero_g:
            self.report['zero_g']['line_height_ft'] = dict(min=round(float(line_h[~self.node_flight].min()), 1),
                                                           median=round(float(np.median(line_h[~self.node_flight])), 1),
                                                           max=round(float(line_h.max()), 1))
            self.flags.append('LineHeight on every node (zero-g track): N64 line height above the node, median %.1f ft, max %.1f ft off the '
                              'zero-g section; 0 on the %d flight nodes [A-derived; new key]' % (
                                  np.median(line_h[~self.node_flight]), line_h.max(), int(self.node_flight.sum())))

        # writer check (cr16-fix-load.txt fault 10): the node whose Next0 is 0 lies more than GRID_PRED_MIN_FT behind node 0
        pts0 = self.D['splines']['points'][self.D['splines']['graph']['main_path'][0]]
        preds0 = [j for j in range(N) if int(nxt[j][0]) == 0]
        d_pred = [float(np.linalg.norm(P[j] - P[0])) for j in preds0]
        if not preds0 or min(d_pred) <= GRID_PRED_MIN_FT:
            raise ValueError('map node before node 0 (Next0 = 0) only %s ft behind node 0 (need > %.0f; the back grid rows would '
                             'start behind its gate and get lap 1 at once)' % ([round(d, 1) for d in d_pred], GRID_PRED_MIN_FT))
        hd = P[0] - P[preds0[0]]; hd[1] = 0.0; hd /= max(np.linalg.norm(hd), 1e-9)
        tn = n2r_space.dirs(np.array(pts0['handle_out'], float) - np.array(pts0['handle_in'], float), self.X); tn[1] = 0.0
        tn /= max(np.linalg.norm(tn), 1e-9)
        self.report['grid_predecessor'] = dict(node=preds0, distance_ft=[round(d, 1) for d in d_pred],
                                               heading_vs_n64_tangent_deg=round(float(np.degrees(np.arccos(np.clip(hd @ tn, -1, 1)))), 2))
        self.flags.append('grid predecessor: map node %s, %s ft behind node 0 (> %.0f; rows 0/100/200 of the original grid lie between its '
                          'gate and node 0), on the N64 tangent at point 0 (grid heading %.2f deg from it) [A heading, G distance]' % (
                              preds0, [round(d, 1) for d in d_pred], GRID_PRED_MIN_FT, self.report['grid_predecessor']['heading_vs_n64_tangent_deg']))

        def spline(kind):
            tw = AI_TRACKWIDTH if kind == 'a' else MAP_TRACKWIDTH
            lw = (ai_l if kind == 'a' else map_l) - tw / 2
            rw = (ai_r if kind == 'a' else map_r) - tw / 2
            secs = [('Track', [('NumNodes', str(N)), ('TrackWidth', '%f' % tw)])]
            for j in range(N):
                kv = [('Position', vec(P[j])), ('LeftTrackWidth', '%f' % f32(lw[j])), ('RightTrackWidth', '%f' % f32(rw[j])),
                      ('SpeedLimit', '%f' % speed[j]), ('DistanceToFinish', '%f' % f32(dtf[j])), ('BranchID', str(int(self.kind[j])))]
                if kind == 'm' and j in prog:
                    kv.append(('ProgressNode', '1'))
                if self.has_zero_g:
                    kv.append(('LineHeight', '%f' % f32(line_h[j])))
                kv.append(('NumNextNodes', str(len(nxt[j]))))
                kv += [('Next%d' % k, str(int(n_))) for k, n_ in enumerate(nxt[j])]
                secs.append(('Node%d' % j, kv))
            return trk_ini.write_canonical(secs)
        self.put('tracks' + BS + 'a%s.spl' % self.code, spline('a'),
                 'A N64 racing-line graph (points, forks, joins) on the collision floor; G subdivision, widths margins, speed; distances A-derived')
        self.put('tracks' + BS + 'm%s.spl' % self.code, spline('m'),
                 'A N64 racing-line graph; G widths from the collision cross-section, progress nodes')
        self.speed = speed

    def dtf_s(self, i):
        j, n_ = self.Sn[i], self.Sk[i]
        a, b = self.PA[j], self.PA[n_]
        t = np.linalg.norm(self.S[i] - a) / max(np.linalg.norm(b - a), 1e-9)
        dj = self.dtf[j]; dn = self.dtf[n_]
        if n_ == 0:
            dn = 0.0
        return dj + (dn - dj) * t if n_ != 0 else dj * (1 - t)

    def collision_files(self):
        Z = self.Z
        self.col_tag = {}
        self.models = {z: [] for z in range(1, Z + 1)}
        col_names = []

        self.col_instances_of_mesh = {}      # N64 mesh offset -> static COL instance names (N64 floor table Objects)
        Tkey = {}
        for t_ in range(len(self.T)):
            Tkey.setdefault(self.T[t_].astype(np.float32).tobytes(), self.n64c.mesh_list[self.col_mesh[t_]]['mesh']['off'])

        def mesh_off_of(T3):
            return [Tkey.get(np.asarray(tri, np.float64).astype(np.float32).tobytes()) for tri in np.asarray(T3)]

        def emit(z, base, T3, prov):
            V = np.asarray(T3).reshape(-1, 3).astype(np.float32).astype(np.float64)
            uniq, inv = np.unique(V, axis=0, return_inverse=True)
            faces = inv.reshape(-1, 3)
            keep = (faces[:, 0] != faces[:, 1]) & (faces[:, 1] != faces[:, 2]) & (faces[:, 0] != faces[:, 2])
            faces = faces[keep]
            if len(faces) < 1:
                return
            if len(faces) == 1:
                # the original COL bodies always have a node (trk_col refuses a single-face body): the one face is
                # written twice (same triangle, same hits) so the tree has its root node [G, flagged in the report]
                faces = np.vstack([faces, faces])
                self.report.setdefault('single_face_cols_doubled', []).append(base)
            self.put('geometry' + BS + base + '.col', trk_col.write_col(trk_col.make_col(uniq, faces)), prov)
            col_names.append(base)
            for mo in set(mesh_off_of(T3)):
                self.col_instances_of_mesh.setdefault(mo, []).append(base)
            self.models[z].append(dict(slt=base + '.slt', faces=len(faces)))
        fid = np.flatnonzero(self.floor_m)
        zact = self.zone_mesh_active
        # zone membership = N64 region switched on somewhere in the zone, plus (as before) every zone with a lane sample
        # within ZONE_OVERLAP_FT: the portal-tracked Revenge zone and the zone a lane sample is filed under can differ
        # near boundaries and on parallel branches; checked: no face the N64 has switched off lies within 120 ft of a
        # lane sample in the N64 state there, so the proximity part never adds a face the N64 would ignore at that place
        # slab rule: a face goes into each slab it overlaps (+ COL_MARGIN_FT) in which its N64 region is switched on at some
        # lane point of that slab, or which has a lane sample within ZONE_OVERLAP_FT of it, or which has no lane at all
        Wtri = np.array([w for _, w in self.n64_walls]) if self.n64_walls else np.zeros((0, 3, 3))
        fzp = self.tri_zones(self.T[fid]); fov = self.overlap_zones(self.T[fid])
        wzp = self.tri_zones(Wtri) if len(Wtri) else []; wov = self.overlap_zones(Wtri) if len(Wtri) else []
        nolane = lambda z: z not in self.zones_with_lanes   # noqa: E731

        def member(ov, prox, mi):
            m_ = set(z for z in ov if (z in zact and zact[z][mi]) or z in prox or nolane(z))
            # a face the rule above files nowhere (its N64 region is on only for pods on lanes outside the slabs it
            # overlaps) stays in the slabs it overlaps: every admitted N64 face is written (conservation)
            return m_ or set(ov)
        fz = [member(ov, p_, self.col_mesh[t]) for t, p_, ov in zip(fid, fzp, fov)]
        wz = [member(ov, p_, self.col_mesh[t]) for (t, _), p_, ov in zip(self.n64_walls, wzp, wov)]
        self.report['floor_triangles_in_two_or_more_zones'] = int(sum(len(z) > 1 for z in fz))
        self.report['faces_in_no_zone'] = int(sum(len(z) == 0 for z in fz) + sum(len(z) == 0 for z in wz))
        tagname = {'none': 'base', 'Dust': 'dust', 'Fast': 'fast'}
        for z in range(1, Z + 1):
            carrier = (lambda t: self.n64c.mesh_list[self.col_mesh[t]]['mesh']['off'] in self.hz.carriers) if self.hz else (lambda t: False)
            for tag in sorted(set(self.surf[fid])):
                ids = [t for t, zs in zip(fid, fz) if z in zs and self.surf[t] == tag and not carrier(t)]
                if ids:
                    self.col_tag['%s_z%02d_%s_floor_col' % (self.code, z, tagname.get(tag, tag.lower().replace('+', '')))] = tag
                    emit(z, '%s_z%02d_%s_floor_col' % (self.code, z, tagname.get(tag, tag.lower().replace('+', ''))), self.T[ids],
                         'A N64 collision, up-facing faces as wound by the N64 (zone %d by N64 region, surface %s); G tree/planes' % (z, tag))
            w = [tri for (t, tri), zs in zip(self.n64_walls, wz) if z in zs and not carrier(t) and 'Magnet' not in self.surf[t].split('+')]
            # magnet faces that do not face up (tube walls / overhangs): own obstacle COL, so TerrainZoneCollision can tag them
            for mtag in sorted(set(self.surf[t] for (t, _), zs in zip(self.n64_walls, wz) if z in zs and 'Magnet' in self.surf[t].split('+'))):
                wm = [tri for (t, tri), zs in zip(self.n64_walls, wz) if z in zs and not carrier(t) and self.surf[t] == mtag]
                if wm:
                    base = '%s_z%02d_%s_wall_col' % (self.code, z, mtag.lower().replace('+', ''))
                    self.col_tag[base] = mtag
                    emit(z, base, np.array(wm), 'A N64 collision, obstacle faces with N64 magnet gravity (surface word bit 0x20), surface %s, '
                         'as wound by the N64 (zone %d by N64 region)' % (mtag, z))
            if self.hz:
                # --n64-hazards: every trigger carrier mesh as its own static COL instances (SurfaceObject names them)
                for mo in sorted(self.hz.carriers):
                    for tag in sorted(set(self.surf[fid])):
                        ids = [t for t, zs in zip(fid, fz) if z in zs and self.surf[t] == tag and self.n64c.mesh_list[self.col_mesh[t]]['mesh']['off'] == mo]
                        if ids:
                            base = '%s_z%02d_m%05x_%s_floor_col' % (self.code, z, mo, tagname.get(tag, tag.lower().replace('+', '')))
                            self.col_tag[base] = tag
                            emit(z, base, self.T[ids], 'A N64 collision of trigger carrier mesh 0x%x (zone %d, surface %s), own instance for SurfaceObject' % (mo, z, tag))
                            self.hz.carrier_cols.setdefault(mo, []).append(base)
                    wl = [tri for (t, tri), zs in zip(self.n64_walls, wz) if z in zs and self.n64c.mesh_list[self.col_mesh[t]]['mesh']['off'] == mo]
                    if wl:
                        base = '%s_z%02d_m%05x_wall_col' % (self.code, z, mo)
                        emit(z, base, np.array(wl), 'A N64 collision of trigger carrier mesh 0x%x (zone %d), own instance for SurfaceObject' % (mo, z))
                        self.hz.carrier_cols.setdefault(mo, []).append(base)
            if w:
                emit(z, '%s_z%02d_wall_col' % (self.code, z), np.array(w), 'A N64 collision, obstacle faces as wound by the N64 (zone %d by N64 region)' % z)
        self.report['collision_files'] = len(col_names)
        self.flags.append('collision files: %d, world coordinates at Position 0, one collision-only placed model each, every zone SearchType '
                          'List; floor split by N64 surface (base / dust / fast) [A, G split]' % len(col_names))
        # every node needs floor in its own zone
        miss = []
        for j in range(len(self.PA)):
            z = self.node_zone[j]
            if self.node_flight[j]:
                # in flight: some floor straight below the node (any distance within the N64 reach) in its own zone
                ids = [t for t, zs in zip(fid, fz) if z in zs]
                ok = bool(ids) and FloorIndex(self.T[ids]).below(self.PA[j], up=0.0, down=10000.0) is not None
                if not ok:
                    miss.append(j)
                continue
            ids = [t for t, zs in zip(fid, fz) if z in zs and self.drive_m[t]]
            ok = FloorIndex(self.T[ids]).near(self.PA[j][0], self.PA[j][2], self.PA[j][1], 3.0) is not None if ids else False
            if not ok:
                miss.append(j)
        self.report['nodes_without_floor_in_own_zone'] = miss

    def visuals(self):
        gout = os.path.join(self.out, 'geometry_build')
        hid = getattr(self, 'hidden_visual_meshes', set())
        if hid:
            self.D['geometry'] = [g for g in self.D['geometry'] if not (g['name'].startswith('mesh_') and int(g['name'][5:], 16) in hid)]
        pg, pm, prep = n2r_props.props(self.D, os.path.join(gout, 'props'))
        if pg:
            self.D['geometry'] = self.D['geometry'] + pg
            self.D['materials'] = dict(self.D['materials'], **pm)
            self.report['code_placed_props'] = prep
            self.flags.append('code-placed props (models the N64 race load places at trigger positions, n2r_props.py): %s, at their load '
                              'state, no collision [A; flag: their pass animation (falling flag) is not reproduced]' % prep)
        tg, tm, trep = n2r_props.track_models(self.D, os.path.join(gout, 'props'))
        if tg:
            self.D['geometry'] = self.D['geometry'] + tg
            self.D['materials'] = dict(self.D['materials'], **tm)
            self.report['code_placed_track_models'] = trep
            self.flags.append('code-placed track model (the N64 race load places it for this planet / track, n2r_props.TRACK_MODELS): %s; '
                              'visual only (no N64 collision) [A; flag: texture flipbook written static at its t = 0 frame]' % trep)
        anim_groups = None
        if self.hz:
            dg, dm, downer = self.hz.debris_groups(os.path.join(gout, 'fx'))
            if dg:
                self.D['geometry'] = self.D['geometry'] + dg
                self.D['materials'] = dict(self.D['materials'], **dm)
            anim_groups = self.hz.mesh_stem()
            anim_groups.update(downer)
        man = n2r_geo.convert(self.D, self.X, self.natural_code, self.zone_of_points, gout, uv_scroll=self.uv_scroll, anim_groups=anim_groups)
        if self.hz:
            afiles, arep = self.hz.anim_files(man.pop('anim_buckets'))
            for nm, blob in afiles.items():
                self.put(nm, blob, 'A N64 animated object (n2r_hazards.py, --n64-hazards): geometry / collision / motion of the N64 node')
            self.report['n64_hazards_animations'] = arep
            tv = sum(r['packed_vertices'] for r in arep)
            self.report['n64_hazards_window'] = dict(packed_vertices=tv, vertex_budget=8192, rows=len(arep))
            self.flags.append('n64 hazards (--n64-hazards): %d animation rows, %d packed vertices (window budget 8192) [A objects, port data]' % (len(arep), tv))
        for r_ in man.get('look_overrides', []):
            self.flags.append('LOOK OVERRIDE (user-approved look choice, not the faithful N64 value; n2r_look_overrides.json): %s, '
                              'N64 materials %s -> %s, vertex alpha %d instead of %d, %s triangles' % (
                                  r_['what'], r_['n64_materials'], r_['revenge_materials'], r_['vertex_alpha'],
                                  r_['faithful_value'], r_['triangles']))
        man['source'] = dict(model=self.model, export=self.D['dir'])
        self.geo_manifest_path = os.path.join(gout, 'manifest.json')
        json.dump(man, open(self.geo_manifest_path, 'w'), indent=1, default=float)
        vis = {}
        for f in man['files']:
            nm = f['file'].split(BS)[-1][:-4]
            self.put('geometry' + BS + nm + '.psg', open(os.path.join(gout, 'geometry', nm + '.psg'), 'rb').read(),
                     'A N64 visual geometry (vertex colours = N64 baked lighting), n2r_geo.py; G piece split')
            vis.setdefault(f['zone'], []).append(dict(slt=nm + '.slt', visual=True))
        for t in man['textures']:
            for ext in ('pst', 'psm'):
                nm = 'textures' + BS + t['material'] + '.' + ext
                self.put(nm, open(os.path.join(gout, 'textures', t['material'] + '.' + ext), 'rb').read(),
                         ('A N64 texture %s' % ' + '.join(str(x) for x in t['n64_textures']) + (' (G 2:1 atlas)' if t['atlas'] else '')) if ext == 'pst'
                         else 'G material (PreLitCompressed%s)' % (', alpha blend keys from a2r [D]' if t['blend'] else ''))
        for z in self.models:
            self.models[z] = vis.get(z, []) + self.models[z]
        self.report['visual'] = dict(psg=len(man['files']), materials=man['materials'], triangles_in=man['stats']['triangles_in'],
                                     triangles_out=man['triangles_out'], atlas_pairs=man['atlas_pairs'])
        self.flags.append('visual: %d PSG world pieces (zone x 2016-ft cell x material) from %d N64 triangles; %d materials; atlas pairs %s '
                          '(no material cap; the live limit is the 8 MB track texture budget) [A geometry/colours/textures, G split]; scrolling N64 materials: %s' % (
                              len(man['files']), man['stats']['triangles_in'], man['materials'], man['atlas_pairs'],
                              [(t['material'], t['uv_scroll'], t['program']) for t in man['textures'] if t.get('uv_scroll')] or 'none'))
        self.flags.append('visual: N64 back-face culling (%d culled materials) not reproduced: the port draws track faces from both sides [flag]' %
                          sum(1 for m in self.D['materials'].values() if m['cull'] != 'none'))

    def sky(self):
        """N64 skybox (camera-centred dome, local coordinates) ray-cast into the Revenge sky cube."""
        from PIL import Image
        tris, texs = [], {}
        mats = self.D['materials']
        order = n2r_space.winding()
        untextured = 0
        NV = 32
        bary = np.stack(np.meshgrid(np.arange(NV) / (NV - 1.0), np.arange(NV) / (NV - 1.0)), -1)   # [v][u] -> (u, v)
        for g in self.D['skybox']:
            tex = mats[g['mtl']]['texture']
            if tex is not None and tex not in texs:
                texs[tex] = np.asarray(Image.open(os.path.join(self.D['dir'], 'textures', tex)).convert('RGBA'))
            for P, UV, C in zip(g['P'], g['UV'], g['C']):
                if tex is None:
                    # untextured dome part (e.g. the lower ring under the horizon): its look is the N64 vertex colours
                    # (shade input); written as a small texture holding the colour interpolated across the triangle
                    # [A colours; GUESS: the texel it would read from the previously loaded TMEM is taken as white]
                    u, v = bary[..., 0], bary[..., 1]
                    w0 = np.clip(1 - u - v, 0, 1); s_ = w0 + u + v
                    img = (C[0][None, None] * (w0 / s_)[..., None] + C[1][None, None] * (u / s_)[..., None] + C[2][None, None] * (v / s_)[..., None])
                    tid = '__vc%d' % untextured; untextured += 1
                    texs[tid] = np.concatenate([np.clip(np.round(img * 255), 0, 255), np.full((NV, NV, 1), 255.0)], -1).astype(np.uint8)
                    UVt = np.array([[0.5 / NV, 0.5 / NV], [(NV - 0.5) / NV, 0.5 / NV], [0.5 / NV, (NV - 0.5) / NV]])
                    tris.append(dict(P=n2r_space.dirs(P, self.X)[order], UV=UVt[order], tid=tid))
                    continue
                tris.append(dict(P=n2r_space.dirs(P, self.X)[order], UV=UV[order], tid=tex))
        # sample each N64 sky texture with its own tile clamp bits (materials clamp_s / clamp_t): the dome's rings run their UVs
        # 0..1 on clamped tiles, so wrapping blended the opposite texture edge into every ring and triangle edge (light seam lines)
        clamp = {}
        for g in self.D['skybox']:
            e = mats[g['mtl']]
            if e.get('texture'):
                clamp[e['texture']] = (bool(e.get('clamp_s')), bool(e.get('clamp_t')))
        tiles, misses = trk_sky.render_tiles(tris, dict(z_sign=1.0), np.zeros(3), texs, clamp=clamp)
        fog = self.fog_values()
        clear = tuple(fog['color'])
        # fade the lowest texel rows into the N64 fog colour (the port's clear colour under the cube)
        rows = np.arange(128)
        w = np.clip((rows - (124 - trk_sky.FADE_ROWS)) / trk_sky.FADE_ROWS, 0, 1)[:, None, None]
        for f in range(4):
            for t in (4, 5, 6, 7):
                img = tiles[(f, t)].astype(np.float64)
                tiles[(f, t)] = np.clip(np.round(img * (1 - w) + np.array(clear) * w), 0, 255).astype(np.uint8)
        rta = trk_res3.read_res3(open(REVENGE + 'TA.RES', 'rb').read())
        palette = rta.get('skies' + BS + 'ta.cub')[16:99100]
        cub = trk_sky.build_cub(tiles, palette)
        self.sky_tiles = tiles
        self.put('skies' + BS + '%s.cub' % self.code, cub,
                 'A N64 skybox (%d triangles, %d textures, %d untextured vertex-colour triangles) ray-cast from its centre into the Revenge sky cube; '
                 'G layout/IPU; palette block from ta.cub [D]' % (len(tris), len(texs) - untextured, untextured))
        self.report['sky'] = dict(triangles=len(tris), textures=sorted(t_ for t_ in texs if not t_.startswith('__vc')), untextured_triangles=untextured,
                                  rays_missing=int(sum(misses.values())), bytes=len(cub))
        self.flags.append('sky: N64 skybox -> skies\\%s.cub, cast from the dome centre (the N64 skybox is camera-centred like the Revenge cube); '
                          '%d rays missed the dome; lowest %d texel rows fade into the N64 fog colour %s [A, G]' % (
                              self.code, sum(misses.values()), trk_sky.FADE_ROWS, clear))

    def fog_values(self):
        """track-wide fog / light from the N64 surface records of the floor near the lanes (weighted mode)."""
        if hasattr(self, '_fog'):
            return self._fog
        from collections import Counter
        cf, cl = Counter(), Counter()
        for mi, mm in enumerate(self.n64c.mesh_list):
            s = mm['behaviour']
            if not s or mi not in self.mesh_area:
                continue
            w = int(round(self.mesh_area.get(mi, 0.0)))
            if s['fog_flags'] & 1:
                cf[(tuple(s['fog_color']), s['fog_start'], s['fog_end'])] += w
            if s['light_flags']:
                cl[(tuple(s['ambient_color']), tuple(s['light_color']), tuple(s['light_vector']), s['light_flags'])] += w
        (col, st, en), nf = cf.most_common(1)[0]
        how = 'the most common record by collision area, %d of %d ft2 of fogged collision' % (nf, sum(cf.values()))
        if getattr(self, 'fog_lane', None):
            # the fog the N64 camera holds most along the lanes (3 laps over the whole graph, flag 1 colour + start,
            # flag 2 end): what the player sees most of the lap
            (col, st, en), nl_ = self.fog_lane.most_common(1)[0]
            how = 'the fog state the N64 camera holds most along the lanes, %d of %d lane samples' % (nl_, sum(self.fog_lane.values()))
        (amb, lcol, lvec, lfl), nl = cl.most_common(1)[0]
        self._fog = dict(color=list(col), start=st, end=en, n=nf, of=sum(cf.values()), how=how, ambient=list(amb), light_color=list(lcol),
                         light_vector=list(lvec), light_flags=lfl, nl=nl, ofl=sum(cl.values()),
                         fog_variants=[[list(k[0]), k[1], k[2], v] for k, v in cf.most_common(8)],
                         light_variants=[[list(k[0]), list(k[1]), list(k[2]), k[3], v] for k, v in cl.most_common(6)])
        return self._fog

    def portals_scene(self):
        sl = self.slabs
        bb = np.array(self.D['track']['bbox_visual'], float)
        corners = np.array([[x, y, z] for x in bb[:, 0] for y in bb[:, 1] for z in bb[:, 2]])
        boxpts = np.vstack([self.T.reshape(-1, 3), self.PA, self.P(corners)] + list(getattr(self, 'cut_paths_pre', [])))
        wlo = boxpts.min(0) - np.array([trk_slabs.CELL, 2000.0, trk_slabs.CELL])
        whi = boxpts.max(0) + np.array([trk_slabs.CELL, 4000.0, trk_slabs.CELL])
        portals = trk_slabs.portals(sl, wlo, whi)
        self.portals = portals
        self.report['zone_layout'].update(world_lo=wlo.tolist(), world_hi=whi.tolist())
        self.flags.append('portals: %d, each the whole slab plane across the world box (y %.0f..%.0f ft), so every line of sight between '
                          'zones passes through them in order (trk_slabs.portals) [G]' % (len(portals), wlo[1], whi[1]))
        fog = self.fog_values()
        rgc = trk_res3.read_res3(open(REVENGE + 'GC.RES', 'rb').read())
        gc_terrain = [s for s in trk_ini.parse(rgc.get('gc.scn')).sections() if s[0].lower().startswith('terrainzone')]
        Z = self.Z
        scn = [('SceneInfo', [('NumPortals', str(len(portals))), ('NumZones', str(Z))])]
        for i, p in enumerate(portals, 1):
            scn.append(('Portal%d' % i, [('Vertex%d' % (k + 1), vec(v)) for k, v in enumerate(p['v'])] +
                        [('Connect1', str(p['c'][0])), ('Connect2', str(p['c'][1]))]))
        # fog far distance: the original race load fixes far 4000 / near 1333.3 / base 64 (SLUS 1AA158 -> 13E920:
        # end = near + Visibility x (far - near), start = (1 - Haziness) x (end - 64)). N64 fog ends (N64 units x track
        # scale) can lie beyond 4000 ft; then the track carries [Fog] FarDistance = F (port data, absent on every stock
        # course: far stays 4000) with F = the largest fog end written (global fog and every zone), rounded up, and every
        # Visibility / Haziness is computed against F
        S_f = self.X['scale']
        ends = [fog['end'] * S_f] + [(self.zone_fog.get(z) or (None, fog['start'], fog['end']))[2] * S_f for z in range(1, self.Z + 1)]
        far_ft = float(math.ceil(max(ends) - 1e-6))
        far_written = far_ft > 4000.0
        if not far_written:
            far_ft = 4000.0
        if far_written and not (1333.3 < far_ft <= 100000.0):
            raise ValueError('fog far distance %.0f ft outside the port admission (1333.3, 100000]' % far_ft)
        self.report['fog_far_distance'] = dict(written=far_written, far_ft=far_ft, largest_end_ft=round(max(ends), 1))

        def fogvh(st, en):
            st, en = st * S_f, en * S_f   # N64 fog distances are N64 units
            v = min(1.0, max(0.0, (en - 1333.3) / (far_ft - 1333.3)))
            h = min(1.0, max(0.0, 1.0 - st / max(1.0, (1333.3 + v * (far_ft - 1333.3)) - 64.0)))
            return v, h
        zone_fog_rows = []
        for z in range(1, Z + 1):
            kv = [('NumberOfStaticModels', str(len(self.models[z]))), ('SearchType', 'List'), ('ZoneReverb', '0')]
            zf = self.zone_fog.get(z) or (tuple(fog['color']), fog['start'], fog['end'])
            v_, h_ = fogvh(zf[1], zf[2])
            kv += [('FogColorRGB', '%d,%d,%d' % tuple(zf[0])), ('FogVisibility', '%.6f' % v_), ('FogHaziness', '%.6f' % h_)]
            zone_fog_rows.append((z, list(zf[0]), zf[1], zf[2]))
            scn.append(('Zone%d' % z, kv))
            for m_i, m in enumerate(self.models[z], 1):
                scn.append(('Zone%d-Model%d' % (z, m_i), [('SLT', m['slt']), ('Position', '0.0,0.0,0.0'),
                            ('LookVector', '0.000000,0.000000,1.000000'), ('UpVector', '0.000000,1.000000,0.000000'), ('Uselighting', 'F')]))
        T = self.T.reshape(-1, 3); cmin, cmax = T.min(0), T.max(0); ctr = (cmin + cmax) / 2
        lv = np.array(fog['light_vector'], float)
        if np.linalg.norm(lv) > 100:      # a far point: direction from the course centre toward it
            bb = np.array(self.D['track']['bbox_visual']); lv = lv - bb.mean(0)
            how = 'a position (far point); direction from the course centre toward it'
        else:                             # a direction the light travels: toward-light = -v
            lv = -lv
            how = 'a direction of travel; toward-light = -vector'
        ldir = n2r_space.dirs(lv, self.X); ldir = ldir / np.linalg.norm(ldir)
        self.flags.append('Light2 vector read as %s [interpretation, G]' % how)
        sun_pos = np.array([ctr[0], cmin[1], ctr[2]]) + ldir * 8000.0
        vis, haz = fogvh(fog['start'], fog['end'])
        scn += [
            ('Environment', [('TerrainWidth', '5'), ('TerrainBreadth', '5'), ('TerrainWidthScale', '24.0'), ('TerrainBreadthScale', '24.0'),
                             ('ParticleTextureMapName', 'demoprt.tga'), ('CubeFile', '%s.cub' % self.code),
                             ('AirborneGravity', '%.4f' % (N64_AIRBORNE_GRAVITY / self.X['scale']))]),
            # the game halves every light ColorRGB (128 = full) but applies DirectionalLightColorForZoneN raw
            # (track-data-gaps\NOTES.md 1): the zone value is written halved so it matches the N64 colour
            # at the same brightness as the global Light2
            ('Lights', [('NumberOfLights', '2')] + [('DirectionalLightColorForZone%d' % z, '%d,%d,%d' % tuple(
                int(round(c / 2.0)) for c in (self.zone_light.get(z) or (None, fog['light_color']))[1]))
                                                     for z in range(1, Z + 1)]),
            ('Light1', [('Type', 'Ambient'), ('ColorRGB', '%d,%d,%d' % tuple(fog['ambient']))]),
            ('Light2', [('Type', 'Directional'), ('ColorRGB', '%d,%d,%d' % tuple(fog['light_color'])),
                        ('Position', '%.3f,%.3f,%.3f' % tuple(sun_pos)), ('TargetPosition', '%.1f,%.1f,%.1f' % (ctr[0], cmin[1], ctr[2])),
                        ('PlaceLightAtInfinity', 'T'), ('CastsShadows', 'T'), ('EmitsLight', 'T'), ('HasLensFlare', 'F'),
                        ('LensFlareColorRGB', '255,255,240'), ('LensFlareBrightness', '1.0'), ('LensFlareLayersVisible', 'T,T,T,T,T')]),
            ('GhostOpacity', [('Reset', '0.40'), ('Ghost', '0.53')]),
            ('Fog', [('ColorRGB', '%d,%d,%d' % tuple(fog['color'])), ('Visibility', '%.4f' % vis), ('Haziness', '%.4f' % haz)]
             + ([('FarDistance', '%d' % int(far_ft))] if far_written else [])),
            ('CSCameras', [('NumberOfCameras', '0')]),
        ] + gc_terrain
        self.flags += [
            'Fog: ColorRGB %s, N64 fog start %s / end %s (%s; N64 fog is per '
            'surface, Revenge has one global fog) -> Visibility %.3f / Haziness %.3f with the original rule end = 1333.3 + V*(far - 1333.3), '
            'start = (end - 64)*(1 - H), far = %.0f ft (%s); N64 fog distances x track scale %g [A values, G mapping]; N64 fog records by area: %s' % (
                fog['color'], fog['start'], fog['end'], fog['how'], vis, haz, far_ft,
                '[Fog] FarDistance written: the largest fog end of the track, port data, absent on stock' if far_written else 'the original 4000, no key',
                S_f, fog['fog_variants']),
            'Light1 ambient %s, Light2 directional colour %s toward N64 %s (flags %d; the most common light record by collision area, %d of %d ft2; '
            'these light the pods - the track is prelit by its vertex colours); HasLensFlare F (no sun on this course) [A]; other '
            'records %s' % (fog['ambient'], fog['light_color'], fog['light_vector'], fog['light_flags'], fog['nl'], fog['ofl'], fog['light_variants'][1:]),
            'per-zone fog (SCB ZoneN FogColorRGB/Visibility/Haziness, the keys original SC uses) and DirectionalLightColorForZoneN from the N64 '
            'surface records under the lanes of each zone (the N64 camera takes fog colour/start/end and light from the surface under its pod, '
            '0x80040CE8): %s [A values, G mapping]; the port applies one global fog per course (terrain_interactive_publication50du.cpp:1474) '
            'so these show only once the port honours zone fog' % zone_fog_rows,
            'Environment: CubeFile %s.cub; no water / ecosystem file; terrain keys from gc.scn [D]; GhostOpacity TA/GC values [D]' % self.code,
        ]
        b = trk_ini.write_canonical(scn)
        self.put('tracks' + BS + '%s.scn' % self.code, b, 'text source of the .scb')
        self.put('tracks' + BS + '%s.scb' % self.code, trk_binparam.compile_ini(trk_ini.parse(b).sections()), 'compiled from the .scn (original compile rule)')
        self.report['zone_models'] = {z: len(self.models[z]) for z in self.models}

    def cameras(self):
        rta = trk_res3.read_res3(open(REVENGE + 'TA.RES', 'rb').read())
        tmpl = trk_vub.read_vub(rta.get('tracks' + BS + 'ta01.vub'))[0]['raw']
        P, K = self.PA, self.K
        cams, prov = [], []
        intro = self.D['cameras']['intro']
        ek, tk = intro_eye_keys(intro), intro['target_keys']
        dur = float(intro['duration_s'])
        nfr = int(round(dur * CUT_FPS)) + 1

        def interp(keys, t):
            ts = np.array(keys['times']); ps = np.array(keys['positions'])
            return np.array([np.interp(t, ts, ps[:, k]) for k in range(3)])
        k1 = []
        self.intro_targets = []
        for i in range(nfr):
            t = i / CUT_FPS
            eye = self.P(interp(ek, t)); tgt = self.P(interp(tk, t))
            self.intro_targets.append(tgt)
            k1.append(trk_vub.look_up(eye, tgt) + (tuple(eye), INTRO_FOV))
        cams.append(k1); prov.append('A N64 intro camera (eye = %s, target = trackpoint keys, %.2f s at the game timing) at %d fps; FOV: N64 %.0f deg horizontal '
                                     '(4:3) = %.2f deg vertical [A]; record layout ta01.vub [D]' % (intro['eye_source'], dur, CUT_FPS, N64_INTRO_FOV_H, INTRO_FOV))

        def at(j, back=0.0, side=0.0, up=0.0):
            f = P[self.nxt[j][0]] - P[j]; f[1] = 0; f /= np.linalg.norm(f)
            return P[j] - f * back + self.R[j] * side + np.array([0, up, 0])
        gcen = P[0] - (P[0] - P[K - 1]) / np.linalg.norm(P[0] - P[K - 1]) * 100.0
        # every surface (visual + collision) as height samples, to keep generated eyes under roofs
        vis_all = np.concatenate([self.P(g['P'].reshape(-1, 3)).reshape(-1, 3, 3) for g in self.D['geometry']] + [self.T])
        allidx = FloorIndex(vis_all)
        lowered = 0

        def under_roof(eye, ground_y, clear=30.0):
            nonlocal lowered
            h = allidx.heights(eye[0], eye[2])
            roof = h[h > ground_y + 2 * clear]
            if len(roof) and eye[1] > roof.min() - clear:
                eye = eye.copy(); eye[1] = max(ground_y + clear, roof.min() - clear); lowered += 1
            return eye
        # line of sight eye -> grid centre must be free of every surface (the a2r path starts 950 ft behind and
        # 420 ft up, which on SMR is outside the start hangar looking through its end): shorten the start
        # distance until the whole path sees the grid
        cen_all = vis_all.mean(1)

        def blocked(eye):
            mid = (eye + gcen) / 2; r = np.linalg.norm(eye - gcen) / 2 + 200.0
            ids = np.flatnonzero(np.linalg.norm(cen_all - mid, axis=1) < r)
            return bool(len(ids) and seg_hits_tris(eye, gcen + np.array([0, 10.0, 0]), vis_all[ids]).any())

        def crowded(eye, clear=40.0):
            ids = np.flatnonzero(np.linalg.norm(cen_all - eye, axis=1) < clear + 400.0)
            return bool(len(ids) and point_tri_dist(eye, vis_all[ids]).min() < clear)

        def blocked_or_crowded(eye):
            return blocked(eye) or crowded(eye)

        side = min(120, self.map_r[0])

        def path(b0, t):
            eye = at(0, back=(b0 - 50) * (1 - t) + 50, up=420 - 250 * t, side=side)
            return under_roof(eye, P[0][1])
        # the a2r side offset first; where even the shortest path stays blocked (a narrow start corridor), the eye
        # is brought inside the floor edge, then onto the lane
        for side in (min(120, self.map_r[0]), max(0.0, self.ext_r[0] - AI_WALL_CLEAR_FT), 0.0):
            b0 = 950.0
            while b0 > 150 and any(blocked_or_crowded(path(b0, t)) for t in np.linspace(0, 1, 16)):
                b0 -= 50.0
            if not any(blocked_or_crowded(path(b0, t)) for t in np.linspace(0, 1, 16)):
                break
        self.report['cut2_side_ft'] = float(side)
        lowered = 0
        k2 = [trk_vub.look_up(e, gcen) + (tuple(e), 60.0) for e in (path(b0, i / 299) for i in range(300))]
        still = sum(blocked_or_crowded(np.array(k[2])) for k in k2[::10])
        cams.append(k2); prov.append('G grid flyover (a2r rule shortened to start %.0f ft behind the grid so the grid stays in view and no surface is within 40 ft of the eye; kept 30 ft '
                                     'under any surface over the grid floor), 300 frames; the N64 has no track data for this shot (its pod sweep, '
                                     'camera mode 7, is per-pod code); record layout ta01.vub [D]' % b0)
        self.flags.append('cut camera 2 (generated): start distance %.0f ft (a2r 950) for a clear view of the grid, %d eye positions lowered '
                          'under surfaces above them; sampled frames still blocked: %d' % (b0, lowered, still))
        WI3 = WallIndex(self.walls_n64)

        def bad3(eye):     # the eye inside / against a wall, or the view of the grid blocked by any surface
            mid = (eye + tgt3) / 2; r = np.linalg.norm(eye - tgt3) / 2 + 200.0
            ids = np.flatnonzero(np.linalg.norm(cen_all - mid, axis=1) < r)
            return WI3.dist(eye) < 12.0 or bool(len(ids) and seg_hits_tris(eye, tgt3, vis_all[ids]).any())
        # the a2r path (320 -> 260 ft behind node 0) first; shortened while any sampled eye is against a wall or blind
        # target: map node 3 (a2r); where that view is blocked from every start distance, the grid centre
        found = False
        for tgt3 in (P[min(3, K - 1)] + np.array([0, 10, 0]), gcen + np.array([0, 10.0, 0])):
            for b3 in range(320, 79, -20):
                if not any(bad3(at(0, back=b3 - 60 * t, up=18)) for t in np.linspace(0, 1, 16)):
                    found = True
                    break
            if found:
                break
        self.report['cut3_clear'] = found
        k3 = []
        for i in range(150):
            t = i / 149
            eye = at(0, back=b3 - 60 * t, up=18)
            k3.append(trk_vub.look_up(eye, tgt3) + (tuple(eye), 65.0))
        self.report['cut3_back_ft'] = b3
        cams.append(k3); prov.append('G behind the grid at pod height (a2r rule, start %d ft behind node 0 so the eye is clear of walls and sees '
                                     'the grid), 150 frames; record layout ta01.vub [D]' % b3)
        # 4..N: as many cuts as the borrowed intro script of the target code names (a2r trk_cuts.py, one rule) [G]
        import trk_cuts
        borrows_intro = self.override and self.code in REVENGE_CODES
        n_cuts, script_cams = trk_cuts.cut_count(self.code, len(cams), borrows_intro)
        self.cut_log = []
        if n_cuts > len(cams):
            view = trk_cuts.clear_view(vis_all)

            def accept(eyes, tgts):   # one zone for the whole shot (a cut keeps its InitialZone), clear view
                return len(set(int(z) for z in self.zone_of_points(eyes))) == 1 and view(eyes, tgts)
            view_c = trk_cuts.clear_view(vis_all, eye_clear_ft=trk_cuts.COVERED_EYE_CLEAR_FT)

            def accept_covered(eyes, tgts):   # enclosed tracks: eye on the racing line (trk_cuts covered stage)
                return len(set(int(z) for z in self.zone_of_points(eyes))) == 1 and view_c(eyes, tgts)
            extra = trk_cuts.extra_shots(trk_cuts.Loop(P[:K], self.R[:K], self.ext_l[:K], self.ext_r[:K]),
                                         n_cuts - len(cams), accept, self.cut_log, accept_covered)
            for keys in extra:
                cams.append(keys); prov.append('G raised travelling shot (trk_cuts.py: outside of a bend in the middle of the lap, '
                                               'one zone, clear view), %d frames; record layout ta01.vub [D]' % len(keys))
        self.flags.append('cut cameras: %d (intro script of %s names cameras %s; port 1..7)%s' % (
            n_cuts, self.code if borrows_intro else 'this course', script_cams or 'none',
            ('; 4-%d generated [G] %s' % (n_cuts, self.cut_log)) if n_cuts > 3 else ''))
        toy = [('CSCameras', [('NumberOfCameras', str(len(cams)))])]
        for i, keys in enumerate(cams, 1):
            nm = 'tracks' + BS + '%s%02d.vue' % (self.code, i)
            self.put('tracks' + BS + '%s%02d.vub' % (self.code, i), trk_vub.make_vub(tmpl, nm, keys), prov[i - 1])
            iz = int(self.zone_of_points(np.array([keys[0][2]]))[0])
            span = sorted(set(int(z) for z in self.zone_of_points(np.array([k[2] for k in keys]))))
            if len(span) > 1:
                self.flags.append('cut camera %d: its eye path crosses zone planes %s (InitialZone = the zone of its first eye point) '
                                  '[check]' % (i, span))
            self.report.setdefault('cut_zone_spans', {})[i] = span
            # slab rule: the cut's InitialZone is the slab of its eye path (the intro eye path is kept inside one slab by the
            # zone choice); whole-plane portals then reach whatever the shot looks at (the earlier "zone the shot looks at"
            # stand-in for lane portals is retired)
            toy.append(('CSCamera%d' % i, [('InitialZone', str(iz))]))
        toy.append(('PodRacer', [('TrackAllowFloor', '1')]))
        toy += self.terrain_zones()
        if self.hz:
            hs = self.hz.sections()
            toy += hs
            self.report['n64_hazards_triggers'] = dict(rows=len(self.hz.triggers), cap_n64_rows=32,
                                                      types={str(k): sum(1 for t in self.hz.triggers if t['type'] == k) for k in (101, 102, 104, 306, 307, 312, 314)})
        b = trk_ini.write_canonical(toy)
        self.put('tracks' + BS + '%s.toy' % self.code, b, 'text source of the .tob')
        self.put('tracks' + BS + '%s.tob' % self.code, trk_binparam.compile_ini(trk_ini.parse(b).sections()),
                 'compiled from the .toy; cut cameras 1 A (N64 intro) + 2 G; PodRacer TrackAllowFloor=1 as every original')
        self.flags.append('cut cameras: 1 = N64 intro (%d frames) [A], 2 = grid flyover, 3 = behind the grid [G]%s; no animations, triggers, '
                          'emitters or static model params' % (nfr, ('; 4-%d raised travelling shots [G]' % len(cams)) if len(cams) > 3 else ''))
        self.cut_paths = [np.array([k[2] for k in c]) for c in cams]
        # TV cameras along the map spline (a2r rule)
        lap = self.lap
        ntv = int(min(40, max(15, round(lap / TV_SPACING_FT))))
        pos = [('CameraPositions', [('NumberOfCameras', str(ntv)), ('SwitchEvery', '20')])]
        sdist = (lap - self.dtf[:K]) % lap; sdist[0] = 0
        self.tv = []
        for i in range(ntv):
            j = int(np.argmin(np.abs(sdist - (i + 0.5) * lap / ntv)))
            a = (P[j] - P[(j - 1) % K])[[0, 2]]; b = (P[(j + 1) % K] - P[j])[[0, 2]]
            side = 1.0 if a[0] * b[1] - a[1] * b[0] > 0 else -1.0
            w = (self.ext_r[j] if side > 0 else self.ext_l[j]) * 0.8
            eye = P[j] + self.R[j] * side * w + np.array([0, 60.0, 0])
            self.tv.append(eye)
            pos.append(('Camera%d' % (i + 1), [
                ('Position', '%.3f,%.3f,%.3f' % tuple(eye)), ('MaxFOV', '40.0'), ('MinFOV', '40.0'),
                ('MaxFOVDistance', '670.0'), ('MinFOVDistance', '312.0'), ('TV', '100'), ('Follow', '0'),
                ('LeftSide', '0'), ('RightSide', '0'), ('LookBack', '0'), ('FirstPerson', '0'), ('Blur', '0'),
                ('InitialZone', str(int(self.zone_of_points(np.array([eye]))[0]))), ('NumNextCameras', '1'), ('NextCamera1', str(i + 2 if i + 1 < ntv else 1))]))
        self.put('tracks' + BS + '%sTVCam.pos' % self.code, trk_ini.write_canonical(pos),
                 'G TV cameras along the map spline, inside the floor edge on the outside of bends (FOV values from taTVCam.pos [D])')
        self.report['tv_cameras'] = ntv

    def terrain_zones(self):
        """TerrainZone rows + TerrainZoneCollision for the floor COLs whose N64 surface changes handling or dust (see N64_TERRAIN).
        Row 1 is the default (every COL not listed belongs to it, 27B004). Nothing is written when no surface needs a row."""
        def row(tag):
            tags = tag.split('+')
            fr, tr = 1.0, 1.0
            for t, key, v in N64_TERRAIN:      # order Slip, Swst, Slow: Swst is tested after Slow and wins (0x80066A80)
                if t in tags:
                    if key == 'traction':
                        tr = v
                    elif fr == 1.0:
                        fr = v
            bonus = max([0.0] + [v for t, v in N64_SPEED_BONUS if t in tags])
            zg = (1 if 'ZOn' in tags else 0) | (2 if 'ZOff' in tags else 0)
            r = (fr, 1.0, tr, not any(t in tags for t in N64_NO_DUST), bonus, zg)
            return r + ((1,) if 'Magnet' in tags else (0,)) if has_mg else r   # magnet column only on tracks that have it
        has_mg = any('Magnet' in t.split('+') for t in self.col_tag.values())
        rows = [(1.0, 1.0, 1.0, True, 0.0, 0) + ((0,) if has_mg else ())]
        objs = []
        for base, tag in sorted(self.col_tag.items()):
            r = row(tag)
            if r == rows[0]:
                continue
            if r not in rows:
                rows.append(r)
            objs.append((base, rows.index(r) + 1, tag))
        if not objs:
            return []
        assert len(rows) <= 8, 'Revenge reads TerrainZone1..8 only (276880)'
        f = lambda v: ('%.4f' % v).rstrip('0').rstrip('.') if v != 1.0 else '1.0'
        use_bonus = any(r[4] for r in rows); use_zg = any(r[5] for r in rows); use_mg = has_mg
        out = []
        for i, r_ in enumerate(rows, 1):
            fr, dr, tr, dust, bonus, zg = r_[:6]
            kv = [('SurfaceFriction', f(fr)), ('SurfaceDrag', f(dr)), ('SurfaceTraction', f(tr)),
                  ('GenerateDust', 'T' if dust else 'F'), ('GenerateDirt', 'T' if dust else 'F')]
            if use_bonus:
                kv.append(('SurfaceSpeedBonus', '%.1f' % bonus))
            if use_zg:
                kv.append(('ZeroGravity', str(zg)))
            if use_mg:
                kv.append(('MagnetGravity', str(r_[6])))
            out.append(('TerrainZone%d' % i, kv))
        out.append(('TerrainZoneCollision', [('NumCollisionObjects', str(len(objs)))]))
        for i, (base, k, tag) in enumerate(objs, 1):
            out.append(('ZoneCollisionObject%d' % i, [('OBJ', base + '.col'), ('TerrainZone', str(k))]))
        self.report['terrain_zones'] = dict(rows=[list(r) for r in rows], objects=[[b, k, t] for b, k, t in objs])
        self.flags.append('TerrainZone (tob): %d rows %s (friction, drag, traction, dust, speed bonus, zero-g) and %d COL objects mapped from N64 '
                          'surfaces (Slip -> SurfaceTraction 0.2, Slow -> SurfaceFriction 0.75, Swst -> 0.1, Fast -> SurfaceSpeedBonus 200, '
                          'ZOn / ZOff -> ZeroGravity 1 / 2, surface word bit 0x20 -> MagnetGravity 1, Mirr / Soft -> no dust); original '
                          'Revenge loads these and uses only GenerateDust; the values are for the port\'s switchable surface-handling '
                          'enhancement [A values, mapping G, notes\\n64-ice-physics.md]' % (len(rows), rows, len(objs)))
        return out

    def donors(self):
        rta = trk_res3.read_res3(open(REVENGE + 'TA.RES', 'rb').read())
        donor = {'A': 'TA', 'B': 'TB', 'C': 'TC'}[self.cls]
        rdn = trk_res3.read_res3(open(REVENGE + donor + '.RES', 'rb').read())
        tun = [n_ for n_ in rdn.names() if n_.lower().endswith('.tun')][0]
        if self.code not in REVENGE_CODES:     # add-on course: own .tun in the original layout (a2r trk_addon.py)
            self.put('tracks' + BS + '%s.tun' % self.code, *trk_addon.tun(self.cls))
        else:
            self.put('tracks' + BS + '%s.tun' % self.code, rdn.get(tun), 'D byte copy of %s (physics class %s donor)' % (tun, self.cls))
        for nm in ('particle' + BS + 'demoprt.prt', 'particle' + BS + 'demoprt.pst', 'particle' + BS + 'demoprt.psm'):
            self.put(nm, rta.get(nm), 'D byte copy from TA.RES (shared particle atlas)')
        # material audio: generated from this track's own collision objects (a2r trk_aub.py, one rule for every code)
        import trk_aub
        cols = sorted(n_[len('geometry' + BS):-4] for n_ in self.files if n_.lower().endswith('.col'))
        aub, aub_rep = trk_aub.generate(cols)
        self.report['aub'] = aub_rep
        self.put('tracks' + BS + '%s.aub' % self.code, aub, trk_aub.provenance(aub_rep))
        self.put('audio' + BS + 'introAud.ini', trk_ini.write_canonical([('ResponseEmitter', [('NumEvents', '0')])]), 'G no announcer lines')
        self.put('audio' + BS + 'scrptLns.ini', trk_ini.write_canonical([('General', [('NumberOfLines', '0')])]), 'G no announcer lines')
        if self.code not in REVENGE_CODES:     # add-on course: authored debug textures, not stock bytes
            for nm, d_, p_ in trk_addon.debug_textures():
                self.put(nm, d_, p_)
        else:
            for nm in [n_ for n_ in rta.names() if n_.lower().startswith('textures' + BS + 'debug')]:
                self.put(nm, rta.get(nm), 'D byte copy from TA.RES (debug textures carried by 12 of 13 originals)')
        self.borrowed = []
        if self.override and self.code in REVENGE_CODES:
            rt = trk_res3.read_res3(open(REVENGE + self.code.upper() + '.RES', 'rb').read())
            # the .aub is generated (above), not borrowed: the stock one names the stock COL objects and positional sounds
            for nm in ['tracks' + BS + '%s.tun' % self.code, 'audio' + BS + 'introAud.ini',
                       'audio' + BS + 'scrptLns.ini', 'particle' + BS + 'demoprt.prt', 'particle' + BS + 'demoprt.pst', 'particle' + BS + 'demoprt.psm']:
                real = [n_ for n_ in rt.names() if n_.lower() == nm.lower()][0]
                for old in [k for k in self.files if k.lower() == nm.lower()]:
                    del self.files[old]; self.prov.pop(old, None)
                self.put(real, rt.get(real), 'BORROWED: byte copy of the original %s.RES member (the port pins it; silo-override.md 5)' % self.code.upper())
                self.borrowed.append(real)
            self.borrowed.append('UI' + BS + 'TEXTURES' + BS + 'LD_%s.PST (original kept in place, not written)' % self.code.upper())
            self.flags.append('packed as original course code %s: borrowed %s [D]' % (self.code, ', '.join(self.borrowed)))
        # surfaces and triggers with no Revenge equivalent
        from collections import Counter
        sc = Counter()
        for m in self.D['surfaces']['meshes']:
            s = m.get('surface') or {}
            for tag in s.get('surface') or []:
                sc[tag] += m['tri_count']
            for f in s.get('word00_flags') or []:
                sc['word00:' + f] += m['tri_count']
        tc = Counter((t['type'], t['type_name']) for t in self.D['triggers']['triggers'])
        self.report['n64_surfaces'] = dict(sc); self.report['n64_triggers'] = {'%d %s' % k: v for k, v in tc.items()}
        self.pending = [
            ('N64 surface Fast (speed boost, %d collision triangles)' % sc.get('Fast', 0),
             'no Revenge equivalent found (Revenge has no speed-pad surface); kept as plain floor in *_fast_floor_col'),
            ('N64 surface Dust (dust kick-up, %d triangles)' % sc.get('Dust', 0), 'kept as floor in *_dust_floor_col; Revenge dust comes from its own pod effects, no per-surface key mapped'),
            ('N64 behaviour flag force_full_raycast (%d triangles)' % sc.get('word00:force_full_raycast', 0), 'engine-internal ground query hint; nothing to map'),
            ('N64 triggers: %s' % ', '.join('%d x type %s' % (v, k) for k, v in self.report['n64_triggers'].items()),
             'flame jets (type 211) and the dozer (310) are scripted hazards/animations driven by game code; Revenge .tob Triggers/Animations '
             'are not generated: no data-only equivalent; listed in the manifest with positions'),
            ('N64 animated scenery (%d animations in the model) and texture scrolling' % self.D['track']['counts']['animations'], 'not converted (static geometry at its rest pose)'),
            ('N64 light streaks (%d LStr records)' % len(self.D['track']['light_streaks']), 'no Revenge equivalent'),
            ('per-surface fog / light zones', 'Revenge has one global fog and lights; the most common N64 record is used'),
            ('back-face culling of N64 culled materials', 'the port draws track faces from both sides'),
            ('UI hologram, LOC strings, music, announcer', 'not made; SC original LD_SC.PST, introAud/scrptLns kept'),
        ]
        if getattr(self, 'has_zero_g', False):
            self.pending.append(('zero-gravity section (N64 ZOn / ZOff)', 'written as TerrainZone ZeroGravity rows, flight nodes on the N64 line and '
                                 'LineHeight elsewhere; needs the port\'s zero-g enhancement (main 5ab5207, switch RR_ZERO_GRAVITY)'))
        else:
            self.pending.append(('Oovo IV tracks (note for later)', 'zero-gravity sections (surface ZOn/ZOff) need a data-driven rule before conversion'))
        self.report['triggers_list'] = [dict(type=t['type'], name=t['type_name'], centre_revenge=self.P(t['center']).round(1).tolist())
                                        for t in self.D['triggers']['triggers']]

    # ------------------------------------------------------------ output
    def write(self):
        for sub in ('install', 'members', 'source'):
            shutil.rmtree(os.path.join(self.out, sub), ignore_errors=True)
        os.makedirs(self.out, exist_ok=True)
        src = {nm: d for nm, d in self.files.items() if nm.endswith(('.scn', '.toy'))}
        names = sorted([nm for nm in self.files if nm not in src],
                       key=lambda s_: (0 if s_.endswith('.tob') else 1 if s_.endswith('.aub') else 2 if s_.endswith('.scb') else 3, s_.lower()))
        for name in names:
            pth = os.path.join(self.out, 'members', *name.split(BS)); os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(self.files[name])
        for name, d in src.items():
            pth = os.path.join(self.out, 'source', *name.split(BS)); os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(d)
        res_path = os.path.join(self.out, 'install', 'TRACKS', '%s.RES' % self.code.upper())
        os.makedirs(os.path.dirname(res_path), exist_ok=True)
        open(res_path, 'wb').write(trk_res3.write_res3([(nm, self.files[nm]) for nm in names]))
        if self.code not in REVENGE_CODES:     # add-on course: own loading screen from its packed geometry
            self.loading_screen(res_path)
        for name, (data, prov) in self.extra.items():
            pth = os.path.join(self.out, 'install', *name.split(BS))
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(data)
        self.report.update(code=self.code, natural_code=self.natural_code, cls=self.cls, nodes=len(self.PA), main_nodes=self.K,
                           branches=len(self.branches), zones=self.Z, lap_ft=round(self.lap, 1), transform=self.X)
        man = dict(source_kind='n64', model=self.model, course=None, name=self.name, natural_code=self.natural_code, code=self.code,
                   physics_class=self.cls, transform=self.X, report=self.report, flags=self.flags,
                   geometry_manifest=self.geo_manifest_path,
                   converter_command='racer-n64\\tools\\n2r\\n2r_convert.py %d --code-as %s' % (self.model, self.code),
                   res='install\\TRACKS\\%s.RES' % self.code.upper(),
                   files={nm: dict(bytes=len(self.files[nm]), provenance=self.prov[nm]) for nm in names},
                   sources={nm: len(d) for nm, d in src.items()},
                   outside_res={n_: dict(bytes=len(d_), provenance=p_) for n_, (d_, p_) in self.extra.items()},
                   pending=[dict(item=a, reason=b) for a, b in self.pending])
        open(os.path.join(self.out, 'manifest.json'), 'w').write(json.dumps(man, indent=1, default=float))
        # verification helpers (for n2r_check)
        np.savez(os.path.join(self.out, 'debug.npz'), PA=self.PA, S=self.S, Sz=self.sample_zone, tv=np.array(self.tv),
                 flight=self.node_flight, hover=self.hover, nxt0=np.array([n_[0] for n_ in self.nxt]),
                 open_edges=np.array([np.r_[a, b] for a, b, k in self.open_edges]) if self.open_edges else np.zeros((0, 6)),
                 open_kind=np.array([k for a, b, k in self.open_edges]), cut0=self.cut_paths[0], cut1=self.cut_paths[1], cut2=self.cut_paths[2])


def _loading_screen(self, res_path):
    """UI\\TEXTURES\\LD_<CODE>.PST for an add-on course: textured overview of the archive's OWN decoded geometry
    (the packed PSG/PST through n2r_check's renderer), eye above the lap centre looking down at 60 deg, framed on
    the map nodes; a2r trk_addon.loading_screen writes the PST. The arcade converter uses its textured top view."""
    import n2r_check as NC
    import softrender as SR
    files = {n.lower(): d for n, d in trk_res3.read_res3(open(res_path, 'rb').read()).items()}
    Pv, Tv, UVv, tv, imgs = NC.converted_visual(files, self.X)
    Pn = n2r_space.inverse_dirs((np.asarray(self.PA, float) - np.array(self.X['offset'])) / self.X['scale'], self.X)
    lo, hi = Pn.min(0), Pn.max(0)
    ctr = (lo + hi) / 2
    W, H = 640, 448
    span = float(max(hi[0] - lo[0], (hi[1] - lo[1]) * W / H))
    dist = span * 0.5 / math.tan(math.radians(65 / 2)) * 1.15
    eye = ctr + np.array([0.0, -dist * math.cos(math.radians(60)), dist * math.sin(math.radians(60))])
    cam = dict(eye=eye, target=ctr, up=(0, 0, 1), fov=65, near=2.0)
    img, _, _ = SR.render(Pv, Tv, cam, W, H, uv=UVv, tex_of_tri=tv, textures=[i[..., :3] for i in imgs], ambient=0.75, bg=(0, 0, 0))
    rgba = np.concatenate([np.asarray(img, np.uint8), np.full((H, W, 1), 255, np.uint8)], 2)
    pst, prov = trk_addon.loading_screen(rgba, 'overview of the converted geometry (decoded packed PSG/PST, eye above the '
                                         'lap centre at 60 deg)')
    self.extra['UI' + BS + 'TEXTURES' + BS + 'LD_%s.PST' % self.code.upper()] = (pst, prov)
    self.flags.append('loading screen UI\\TEXTURES\\LD_%s.PST: %s' % (self.code.upper(), prov))


Conv.loading_screen = _loading_screen


def intro_eye_keys(intro):
    """the intro eye as (times, positions): the N64 eye empty's keys where the model has one, else the export's
    eye frames (camera spline at the timing recorded in the export, see its eye_source)"""
    if intro.get('eye_keys'):
        return intro['eye_keys']
    return dict(times=[f['t'] for f in intro['frames']], positions=[f['eye'] for f in intro['frames']])


def physics_class(code):
    c = 'A' if 'a' in code else 'B' if 'b' in code else 'C'
    if code == 'sb':
        c = 'C'
    if code in ('ra', 'sc'):
        c = 'B'
    return c


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('model', type=int)
    ap.add_argument('--code-as', default=None)
    ap.add_argument('--scale', type=float, default=None, help='ft per N64 unit (default n2r_space.SCALE, user-chosen; faithful 1.0)')
    ap.add_argument('--no-uv-scroll', action='store_true', help='write the scrolling N64 material as a static one')
    ap.add_argument('--out', default=os.path.join(N64, 'export', 'n2r'))
    ap.add_argument('--dlc-out', default=None, help='with --code-as: also write the DLC replacement package into this folder')
    ap.add_argument('--dlc-disabled', action='store_true', help='write the package with "enabled": false')
    ap.add_argument('--phy-class', default=None, choices=['A', 'B', 'C', 'a', 'b', 'c'],
                    help='add-on course (own code) only: explicit PHY class (dlc.json phy_class); picks the .tun donor')
    ap.add_argument('--n64-hazards', action='store_true', help='(the default now) write N64 trigger / animation rows (n2r_hazards.py); kept for old command lines')
    ap.add_argument('--no-n64-hazards', action='store_true', help='write the static stand-ins instead of the N64 hazard rows (port builds before cr-29)')
    a = ap.parse_args()
    if a.scale is not None:
        n2r_space.SCALE = float(a.scale)
    if a.dlc_out and not a.code_as:
        raise SystemExit('--dlc-out writes a replacement package: it needs --code-as <stock course code>')
    c = Conv(a.model, a.out, a.code_as, uv_scroll=not a.no_uv_scroll, hazards=not a.no_n64_hazards, phy_class=a.phy_class)
    c.run()
    c.write()
    print(json.dumps({k: v for k, v in c.report.items() if k not in ('triggers_list',)}, default=float)[:3000])
    for f in c.flags:
        print('-', f[:300])
    if a.dlc_out:
        import trk_silo
        trk_silo.package(a.code_as.lower(), a.out, a.dlc_out, None, not a.dlc_disabled)


if __name__ == '__main__':
    main()
