r"""N64 Racer collision exactly as the game's loader and ray tests see it (N64 space, +Z up).

Read straight from the model block with tools\n64r\model.py. Every rule below is RE'd; evidence in
notes\n64-loader-re.md (section numbers in brackets):

  * Descent filter [2.2]: the collision ray/sphere walkers (N64 0x80004DF8 ray, 0x80003E48 sphere;
    PC swrModel_CollideNodeRecursiveRay 0x00444BF0) enter a child node only when
        (child.flags2 & ALL) == ALL   and   (child.flags2 & ANY) != 0
    ALL = mask slot 5 (0x8009A28C), set at race load (0x8005F2EC) to (0x10 << planetTrack) | 2,
          plus bit 0x4 while the pod's speed is above 150 (0x8006B5A0, around its collision);
    ANY = mask slot 3 (0x8009A290) = the pod's collisionToggles (+0x26C), loaded before each pod's
          collision (0x8006B5D0).
  * collisionToggles [2.3]: 0xFFFFFF00 at pod init (0x8007302C); every frame the surface under the pod
    applies  t = (t & ~behaviour[0x34]) | behaviour[0x38];  t &= ~0xFF  (0x80066B1C, PC 0x00476EA0).
  * Faces [2.4]: collision vertices s16, strips flip winding every step; quads split (0,1,3),(1,2,3);
    plane normal = cross(b-a, c-b) (PC rdMath_CalcSurfaceNormal2, swrModel_MeshIterateOverCollisionFaces
    0x004439F0).
  * Sides [2.5]: MeshMaterial.type & 0x8 -> the ray accepts only faces it meets head-on (dot(n, dir) <= 0);
    without 0x8 both sides (selectFaceAcceptance / swrModel_ClassifyMaterialFacing 0x00431880).
    0x40 (mirrored) never occurs on SMR collision.
  * Position grid (Beedo's Wild Ride RE, work\bwr\n64-surf\grid.py): race load 0x80060AC0 sets 0x800A5B5C = 2 for
    planet 1 track 0 and 1 for planet 3 track 2 (the original's own branch, on its own planet/track numbers). Then
    0x8007DA20 / 0x8007DBA4 / 0x8007DED8 rewrite every mesh group's flags2 at load to (flags2 & 0xFF) | grid mask of
    its AABB (basic parents: the OR of their children; nothing under a transformed node changes), and every frame
    0x8006B304 replaces each pod's collision toggles by the grid cells its x, y lie in (with margins), just before its
    collision; the per-surface update (above) still runs but its value is overwritten before any collision use.
    The camera / visual masks keep the per-surface rule. Mode 2 constants 0x800ADC20..2C; mode 1 0x800ADC10.. (mode
    1 not checked on a track).
  * Trigger set-up and one-shot handlers (n64-loader-re.md 4 / 10.2, Ando RE): type 202 (rock) clears the speed bit
    0x4 of its affected node at load (solid only for pods at 150 or slower); types 100 (ice block), 103 (tent) and 202
    hide their affected node and remove its collision for good when a pod enters (100 / 202 only above speed 150).
    state='race' (default) gives the collision a pod faster than 150 meets: those nodes have no collision.
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.normpath(os.path.join(HERE, '..', 'n64r')))
import model as Mo  # noqa: E402

TOGGLE_INIT = 0xFFFFFF00
SPEED_BIT = 0x4          # ALL gains this bit while speed > 150
# 0x80060AC0: (planet index, planet track number) -> grid mode; mode -> (ymin, ymax, xmax, xmin) (0x800ADC10 / 0x800ADC20)
GRID_MODE = {(3, 2): 1, (1, 0): 2}
GRID_BOUNDS = {1: (-8500.0, 3500.0, 2500.0, -5500.0), 2: (-9300.0, 9000.0, 7500.0, -7500.0)}
HAZARD_ANIM_TRIGGERS = (101, 102, 306, 307, 312)       # hazards on: their affected nodes become animation rows (n2r_hazards.py)
TRIGGER_CLEARS_SPEED_BIT = (202, 212)       # load set-up (Ando 0x8007E180 path; 212 branch: 0x8007D628 clears flags2 0x4, trigger flag |= 0x2)
TRIGGER_REMOVES_COLLISION = (100, 102, 103, 202, 212)  # handlers 0x8007C818 (100, 102, 202; 212 via 0x8007D0AC) / 0x8007CEC0 (103): hide + no collision
# 102 (asteroid, Oovo IV): jump table 0x800ADBE4 entry 2 -> 0x8007CEAC -> jal 0x8007C818, the same handler as 100
# (PC swrObjTrig_HandleCrashHitTrigger 0x47CA90: node hidden, collision flag cleared, thrust x Decelerator(2.5),
# 'Hitt' 300; one-shot: swrObjTrig_MaybeResetAnimation sets the trigger's handled bit)


class PositionGrid:
    """0x8007DA20 table, 0x8007DBA4 node mask, 0x8006B304 pod toggles (N64 x, y)."""
    def __init__(self, mode):
        self.mode = mode
        ymin, ymax, xmax, xmin = GRID_BOUNDS[mode]
        dx = (xmax - xmin) / 3; dy = (ymax - ymin) / 8
        self.X = [xmin, xmin + dx, xmax - dx, xmax]
        self.Y = [ymin] + [ymin + k * dy for k in range(1, 7)] + [ymax - dy, ymax]
        self.WX = dx / 4; self.WY = dy / 4

    def pod_toggles(self, x, y):
        X, Y, WX, WY = self.X, self.Y, self.WX, self.WY
        v0 = 0x800000 if Y[7] - WY < y else 0
        for k in range(6, 0, -1):
            if y < Y[k + 1] + WY and Y[k] - WY < y:
                v0 |= 0x10000 << k
        if y < Y[1] + WY:
            v0 |= 0x10000
        t = 0
        if X[2] - WX < x:
            t = v0 >> 8
        if x < X[2] + WX and X[1] - WX < x:
            t |= v0
        if x < X[1] + WX:
            t |= (v0 << 8) & 0xFFFFFFFF
        return t

    def node_mask(self, aabb):
        X, Y = self.X, self.Y
        mnx, mny, _, mxx, mxy, _ = aabb
        v = 0
        for k, m in ((1, 0xFF0000), (2, 0xFE0000), (3, 0xFC0000), (4, 0xF80000), (5, 0xF00000), (6, 0xE00000), (7, 0xC00000), (8, 0x800000)):
            if mny < Y[k]:
                v = m; break
        if v:
            if mxy < Y[0]:
                v = 0
            else:
                for k, m in ((1, 0x10000), (2, 0x30000), (3, 0x70000), (4, 0xF0000), (5, 0x1F0000), (6, 0x3F0000), (7, 0x7F0000)):
                    if mxy < Y[k]:
                        v &= m; break
        c0 = (v << 8) & 0xFFFFFFFF; c1 = v; c2 = v >> 8
        if mxx < X[0] or X[1] < mnx:
            c0 = 0
        if mxx < X[1] or X[2] < mnx:
            c1 = 0
        if mxx < X[2] or X[3] < mnx:
            c2 = 0
        return c0 | c1 | c2


def all_mask(planet_track, fast=True):
    m = (0x10 << planet_track) | 2
    return m | (SPEED_BIT if fast else 0)


def apply_toggles(t, beh):
    if beh is None:
        return t
    t = (t & ~(beh['unk34'] & 0xFFFFFFFF)) | (beh['unk38'] & 0xFFFFFFFF)
    return t & 0xFFFFFF00


class N64Collision:
    def __init__(self, model_index, planet_track, planet=None, triggers=(), state='race', line=None, hazards=False):
        self.M = M = Mo.Model(model_index)
        # hazards=True (converter switch --n64-hazards, n2r_hazards.py): trigger targets (101 door, 102 asteroids) and looping
        # animated scenery with collision become Revenge animation rows; they leave the static collision (anim_nodes) and
        # none of the static stand-ins (102 collision removal, 101 open pose, lane-clear pose) is applied
        self.hazards = hazards
        self.anim_nodes = {}
        self.track = planet_track
        faces, chains, meshes, beh, sided = [], [], [], [], []
        root = M.nodes[M.header_nodes[0]]
        self.mesh_list = []
        # load-time flags2 (position grid rewrite, trigger set-up), keyed by node offset
        self.grid = PositionGrid(GRID_MODE[(planet, planet_track)]) if (planet, planet_track) in GRID_MODE else None
        self.f2 = {}
        if self.grid:
            def gwalk(n, in_d):          # 0x8007DED8
                t = n['type']
                if t == Mo.NODE_MESHGROUP:
                    v = 0 if in_d else self.grid.node_mask(n['aabb'])
                    if not in_d:
                        self.f2[n['off']] = (n['flags2'] & 0xFF) | v
                    return v
                if not (t & 0x4000):
                    return 0
                d = in_d or bool(t & 0x8000)
                v = 0
                for c in n['children']:
                    v |= gwalk(c, d)
                if not d:
                    self.f2[n['off']] = (n['flags2'] & 0xFF) | v
                return v
            gwalk(root, False)
        self.removed = set()
        self.trigger_effects = []
        for tr in triggers:
            a = tr.get('affected_node')
            if not a:
                continue
            a = int(a, 16) if isinstance(a, str) else int(a)
            if tr['type'] in TRIGGER_CLEARS_SPEED_BIT:
                self.f2[a] = self.f2.get(a, M.nodes[a]['flags2']) & ~SPEED_BIT
                self.trigger_effects.append((tr['type'], hex(a), 'speed bit cleared at load'))
            if hazards and tr['type'] in HAZARD_ANIM_TRIGGERS:
                if tr['type'] == 307:
                    targets = {anim['target'] for anim in M.anims if anim['type'] in (8, 9)}
                    for node, _, _ in M.walk(M.nodes[a], 'all'):
                        if node['off'] in targets:
                            self.anim_nodes[node['off']] = 307
                else:
                    self.anim_nodes.setdefault(a, tr['type'])
                continue
            if state == 'race' and tr['type'] in TRIGGER_REMOVES_COLLISION:
                # a trigger is only tested for the surface under the pod (0x800665A4) or as the track's one global
                # trigger (Ando: the 202 rock, 0x800A66D4): carried only by the object's own meshes, it fires only
                # while a pod rides on the object, so the object stays a solid obstacle at race speed
                own = set(self.subtree_meshes(a))
                carriers = set(int(x, 16) for x in tr.get('meshes') or [])
                if tr['type'] != 202 and carriers and carriers <= own:
                    self.trigger_effects.append((tr['type'], hex(a), 'kept solid: its trigger is carried only by its own meshes'))
                    continue
                self.removed.add(a)
                self.trigger_effects.append((tr['type'], hex(a), 'collision removed (one-shot trigger fired at race speed)'))

        # trigger-started animations at the state a pod at race speed meets (n2r_anim.py: 101 door -> last key)
        import n2r_anim
        # (hazards on: the trigger types written as animation rows are left at rest; every other type keeps its static pose)
        self.posed, self.posed_report = n2r_anim.trigger_poses(
            M, [t for t in triggers if not (hazards and t['type'] in HAZARD_ANIM_TRIGGERS)]) if state == 'race' else ({}, [])

        def rec(n, W, chain):
            W = self.posed.get(n['off'], M.local_matrix(n)) @ W
            if n['type'] == Mo.NODE_MESHGROUP:
                for m in n['meshes']:
                    if not m['coll_vtx_ptr'] or m['coll_vtx_count'] < 3:
                        continue
                    V, T = M.collision_tris(m)
                    if not len(T):
                        continue
                    P = Mo.apply(W, V)
                    mat = m['material']
                    fl = mat['flags'] if mat else 0
                    # facing rule (N64 0x80017E88 called from the ray walker 0x80004D48..0x80004D90, PC
                    # swrModel_ClassifyMaterialFacing 0x00431880 + selectFaceAcceptance): type 0x8 -> 1 (front faces only),
                    # 0x40 -> 2 (back faces only: a one-sided face met from behind, i.e. the reversed face; the hit
                    # returns the flipped normal), 0x48 -> 3 (neither side: the ray never takes it), else 0 (both sides)
                    if fl & 0x40:
                        if fl & 0x8:
                            self.facing3_skipped = getattr(self, 'facing3_skipped', {}); self.facing3_skipped[m['off']] = len(T)
                            continue
                        T = T[:, [0, 2, 1]]
                        self.mirrored_reversed = getattr(self, 'mirrored_reversed', {}); self.mirrored_reversed[m['off']] = len(T)
                    one_sided = bool(fl & 0x8) or bool(fl & 0x40)
                    mi = len(self.mesh_list)
                    self.mesh_list.append(dict(mesh=m, node=n, chain=tuple(self.f2.get(o, f) for o, f in chain),
                                               chain_nodes=tuple(o for o, f in chain), world=W, mat_flags=fl,
                                               one_sided=one_sided, behaviour=m['behaviour']))
                    for t in T:
                        faces.append(P[t]); meshes.append(mi)
                return
            for c in n['children']:
                rec(c, W, chain + ((c['off'], c['flags2']),))

        rec(root, np.eye(4), ())
        self.F = np.array(faces)                      # (n,3,3) N64 order
        self.mesh_of = np.array(meshes)
        # looping animated scenery with collision in the racing line at its rest pose: posed at the keyframe of its
        # own loop that keeps the line clearest (n2r_anim.lane_clear_poses) [A keyframe, G choice]
        self.lane_pose_report = []
        self.lane_posed = set()
        if hazards:
            # nodes a trigger set-up stops or poses (307 doors, 312) are not free-running loops
            for off in n2r_anim.looping_collision_nodes(self, M, n2r_anim.load_stopped_nodes(M, triggers) | set(self.posed)):
                self.anim_nodes.setdefault(off, 'loop')
        if line is not None and state == 'race' and not hazards:
            extra, self.lane_pose_report = n2r_anim.lane_clear_poses(self, M, np.asarray(line, float))
            if extra and os.environ.get('N2R_TEST_DROP_LANE_MOVERS') == '1':
                # TEST ONLY (never installed for play): the moving node's collision left out, to drive past it in tests
                self.removed |= set(extra)
                self.lane_pose_report.append('TEST ONLY: collision of %s left out (N2R_TEST_DROP_LANE_MOVERS=1)' % ['0x%x' % o for o in extra])
            if extra:
                self.lane_posed = set(extra)
                self.posed.update(extra)
                faces.clear(); meshes.clear(); self.mesh_list = []
                rec(root, np.eye(4), ())
                self.F = np.array(faces)
                self.mesh_of = np.array(meshes)
        cr = np.cross(self.F[:, 1] - self.F[:, 0], self.F[:, 2] - self.F[:, 1])
        ln = np.linalg.norm(cr, axis=1)
        self.degenerate = ln < 1e-9
        self.N = cr / np.where(self.degenerate, 1, ln)[:, None]
        self.one_sided = np.array([self.mesh_list[i]['one_sided'] for i in self.mesh_of])

    def subtree_meshes(self, node_off):
        out = []

        def rec(n):
            out.extend(m['off'] for m in n['meshes'])
            for c in n['children']:
                rec(c)
        rec(self.M.nodes[node_off])
        return out

    # -- descent filter -------------------------------------------------------------------
    def mesh_track_ok(self, mi, fast=True, include_anim=False):
        A = all_mask(self.track, fast)
        if self.removed.intersection(self.mesh_list[mi]['chain_nodes']):
            return False
        if not include_anim and self.anim_nodes and (set(self.anim_nodes) & (set(self.mesh_list[mi]['chain_nodes']) | {self.mesh_list[mi]['node']['off']})):
            return False
        return all((f & A) == A for f in self.mesh_list[mi]['chain'])

    def pod_toggles(self, p, surface_toggles):
        """the toggles a pod's collision uses at N64 point p: the position grid where the track has one, else the
        per-surface value"""
        return self.grid.pod_toggles(p[0], p[1]) if self.grid else surface_toggles

    def mesh_active(self, mi, toggles, fast=True):
        if not self.mesh_track_ok(mi, fast):
            return False
        return all((f & toggles) != 0 for f in self.mesh_list[mi]['chain'])

    def region_need(self, mi):
        """the region bits each chain node needs (one of them must be on), as a list of masks."""
        return [f & 0xFFFFFF00 for f in self.mesh_list[mi]['chain']]

    def face_mask(self, toggles, fast=True):
        ok = np.array([self.mesh_active(i, toggles, fast) for i in range(len(self.mesh_list))])
        return ok[self.mesh_of]

    # -- ray test (exact rule) --------------------------------------------------------------
    def ray(self, o, d, maxd, mask):
        """closest hit of ray o + t d, t in [0, maxd], over faces in mask. -> (t, face, normal) or None"""
        F = self.F[mask]; N = self.N[mask]; ids = np.flatnonzero(mask); os_ = self.one_sided[mask]
        facing = N @ d
        ok = ~((facing > 0) & os_) & (np.abs(facing) > 1e-4)
        if not ok.any():
            return None
        F, N, ids, facing = F[ok], N[ok], ids[ok], facing[ok]
        dist = ((F[:, 0] - o) * N).sum(1) / facing
        good = (dist >= 0) & (dist <= maxd)
        if not good.any():
            return None
        F, N, ids, facing, dist = F[good], N[good], ids[good], facing[good], dist[good]
        H = o + dist[:, None] * d
        e0 = np.cross(F[:, 1] - F[:, 0], H - F[:, 0])
        e1 = np.cross(F[:, 2] - F[:, 1], H - F[:, 1])
        e2 = np.cross(F[:, 0] - F[:, 2], H - F[:, 2])
        inside = ((e0 * N).sum(1) >= -1e-6) & ((e1 * N).sum(1) >= -1e-6) & ((e2 * N).sum(1) >= -1e-6)
        if not inside.any():
            return None
        k = np.flatnonzero(inside)[np.argmin(dist[inside])]
        n = N[k] if facing[k] <= 0 else -N[k]
        return float(dist[k]), int(ids[k]), n
