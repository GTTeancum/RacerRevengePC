r"""Trigger-started animations at the state a pod at race speed meets (one rule for every N64 track).

N64 trigger 101 (StartAnimDoor, handler 0x8007CE30 via the 100-108 jump table at 0x800ADBE4):
  * swrObjTrig_FindOrCreate (PC 0x47C7D0) takes the affected node's loaded animations of type 8 (axis-angle) and
    9 (translation), sets their time 0 and speed 0;
  * the handler clears the animation's 0x10 flag at +0x100 and, once more than 3.0 s (0x40400000) have passed on the
    trigger's timer, sets the speed to +20 when the animation time is <= 0 (0x80006EB4 a1 = 0x41A0) or -20 otherwise.
  * the trigger object's FLAG_ACTIVE (0x1) is set by that first activation, so its timer only starts then: the FIRST
    activation freezes the door at time 0 (its closed pose, = the node matrix) and cannot start it; an activation
    more than 3 s later opens it (+20, 13.33 s / 20 = 0.67 s), the next one more than 3 s after that closes it (-20),
    and so on (notes\n64-hazards-spec.md 4). Before any activation the door loops on its own (ENABLED | LOOP).
STAND-IN: in the default build the converter writes the door at its LAST key (open), visual and collision: the first
pod actually meets it frozen closed, later pods open or closed by the 3-s toggle; the closed pose leaves the exact
racing line clear but stands 8-10 ft beside / above it. Revenge has no data the port plays to move it until the
animation path lands; with --n64-hazards the door becomes an AnimationToggle row instead (n2r_hazards.py) [flag].
Matrices are the model's row-vector convention (p' = p @ M, model.local_matrix). Axis-angle keys (x, y, z, deg):
the local 3x3 rows = Rodrigues(axis, angle) transposed (checked: Vengeance door node 0x2E534 matrix == key 0).
"""
import math

import numpy as np

TRIGGER_PLAYS_ANIMATION_TO_END = (101, 307)
# 307 (Aquilaris door, set-up 0x8007D7EC -> 0x8007B544(node, 0x10, 0, 1.0)): every axis-angle / translation animation in the
# affected node's SUBTREE loses LOOP and plays once at speed +1 from time 0 (time <= 0 -> +speed) at track load, so it
# stands at its LAST key long before the race starts (the 3.27-s door is done during the intro); the handler 0x8007D1F8
# reverses it (0x8007B544 again: time > 0 -> -1.0) on an activation more than 3.0 s after the previous one. Written at the
# last key = the state every pod meets until the first pod reaches the trigger [A state, flag: the toggle is not reproduced].
TRIGGER_POSES_SUBTREE = (307,)
# 312 (Malastare falling pillar, set-up 0x8007D944 -> 0x8007B544(node, 0x10000010, 0, 1.0)): every axis-angle / translation
# animation in the affected node's subtree loses ENABLED (0x10000000, SW_RACER_RE ANIMATION_ENABLED) and LOOP (0x10) at track
# load, so the node stands at rest (key 0 = the node matrix) although its file flags say ENABLED | LOOP. The handler
# 0x8007D248 (trigger flag 0x20 = local player only, set by the set-up; one-shot) sets ENABLED again at speed 0.6 (the pillar
# falls once in 1.57 / 0.6 = 2.6 s and stays down) plus a camera shake ('cMan' 'Shak'). Written at rest = the state every pod
# meets until the local player reaches the trigger [A state, flag: the fall is not reproduced].
TRIGGER_STOPS_AT_LOAD = (312,)
ANIM_AXIS_ANGLE = 0x8
ANIM_TRANSLATION = 0x9


def rodrigues(axis, deg):
    a = np.asarray(axis, float)
    a = a / max(np.linalg.norm(a), 1e-12)
    t = math.radians(deg)
    K = np.array([[0, -a[2], a[1]], [a[2], 0, -a[0]], [-a[1], a[0], 0]])
    return np.eye(3) + math.sin(t) * K + (1 - math.cos(t)) * K @ K


def trigger_poses(M, triggers):
    """-> {node offset: posed local 4x4}, plus a report list. M = n64r model.Model."""
    by_target = {}
    for a in M.anims:
        if a['target']:
            by_target.setdefault(a['target'], []).append(a)
    poses, rep = {}, []
    for off in load_stopped_nodes(M, triggers):
        if any(a['type'] in (ANIM_AXIS_ANGLE, ANIM_TRANSLATION) for a in by_target.get(off, [])):
            poses[off] = M.local_matrix(M.nodes[off]).copy()
            rep.append(dict(trigger=TRIGGER_STOPS_AT_LOAD, node='0x%x' % off, animations='stopped at load: rest pose (node matrix)'))
    for tr in triggers:
        if tr['type'] not in TRIGGER_PLAYS_ANIMATION_TO_END or not tr.get('affected_node'):
            continue
        off0 = int(tr['affected_node'], 16) if isinstance(tr['affected_node'], str) else int(tr['affected_node'])
        if M.nodes.get(off0) is None:
            continue
        offs = [off0]
        if tr['type'] in TRIGGER_POSES_SUBTREE:
            offs = [nn['off'] for nn, _, _ in M.walk(M.nodes[off0], 'all')]
        for off in offs:
            _pose_one(M, tr, off, by_target, poses, rep)
    return poses, rep


def load_stopped_nodes(M, triggers):
    """nodes whose animations a TRIGGER_STOPS_AT_LOAD set-up disables (the affected node's whole subtree)."""
    out = set()
    for tr in triggers:
        if tr['type'] not in TRIGGER_STOPS_AT_LOAD or not tr.get('affected_node'):
            continue
        off0 = int(tr['affected_node'], 16) if isinstance(tr['affected_node'], str) else int(tr['affected_node'])
        if M.nodes.get(off0) is not None:
            out |= {nn['off'] for nn, _, _ in M.walk(M.nodes[off0], 'all')}
    return out


def _pose_one(M, tr, off, by_target, poses, rep):
    if True:
        n = M.nodes[off]
        L = M.local_matrix(n).copy()
        used = []
        for a in by_target.get(off, []):
            if not a['values']:
                continue
            last = a['values'][-1]
            if a['type'] == ANIM_AXIS_ANGLE:
                # check the convention against key 0 (must reproduce the node's own rotation at load)
                R0 = rodrigues(a['values'][0][:3], a['values'][0][3]).T
                ok0 = bool(np.allclose(R0, M.local_matrix(n)[:3, :3], atol=5e-3))
                L[:3, :3] = rodrigues(last[:3], last[3]).T
                used.append(('axis_angle', '0x%x' % a['off'], round(a['duration'], 2), [round(x, 3) for x in last], 'key0 == node matrix' if ok0 else 'KEY0 MISMATCH'))
            elif a['type'] == ANIM_TRANSLATION:
                L[3, :3] = last[:3]
                used.append(('translation', '0x%x' % a['off'], round(a['duration'], 2), [round(x, 1) for x in last]))
        if used:
            poses[off] = L
            rep.append(dict(trigger=tr['type'], node='0x%x' % off, animations=used))


LANE_CLEAR_FT = 15.0   # collision of a looping animated node closer than this to the racing line at rest is "in the line": the lane tooling's own wall clearance (n2r_convert select(), 15 ft) [G]
LOOP_STEP_S = 0.1      # the loop is searched at this time step (N64 interpolation: slerp / linear between keys)


def _key_locals(M, n, anims):
    """posed local matrices at each keyframe of the node's axis-angle / translation loop (None if their key counts differ)."""
    use = [a for a in anims if a['type'] in (ANIM_AXIS_ANGLE, ANIM_TRANSLATION) and a['values']]
    if not use or len(set(len(a['values']) for a in use)) != 1:
        return None, None
    out = []
    for k in range(len(use[0]['values'])):
        L = M.local_matrix(n).copy()
        for a in use:
            v = a['values'][k]
            if a['type'] == ANIM_AXIS_ANGLE:
                L[:3, :3] = rodrigues(v[:3], v[3]).T
            else:
                L[3, :3] = v[:3]
        out.append(L)
    return out, [a['times'][k] for k in range(len(use[0]['values']))] if use[0]['times'] else None


def _quat(axis, deg):
    a = np.asarray(axis, float); a = a / max(np.linalg.norm(a), 1e-12); h = math.radians(deg) / 2
    return np.r_[a * math.sin(h), math.cos(h)]


def _slerp(q0, q1, t):
    d = float(q0 @ q1)
    if d < 0:
        q1 = -q1; d = -d
    if d > 0.9995:
        q = q0 + t * (q1 - q0)
        return q / np.linalg.norm(q)
    th = math.acos(min(1.0, d))
    return (math.sin((1 - t) * th) * q0 + math.sin(t * th) * q1) / math.sin(th)


def _pose_at(M, n, anims, time):
    """posed local at loop time (PC swrModel_AnimationInterpolateAxisAngle 0x425B?0: quaternion slerp between keys with the
    linear factor of swrModel_AnimationComputeInterpFactor; translation linear)."""
    L = M.local_matrix(n).copy()
    for a in anims:
        ts, vs = a['times'], a['values']
        if not vs:
            continue
        if len(ts) < 2:
            k, f = 0, 0.0
        else:
            tt = time % ts[-1] if (a['flags'] & 0x10) and ts[-1] > 0 else time
            k = max(0, min(len(ts) - 2, int(np.searchsorted(ts, tt, side='right')) - 1))
            f = min(1.0, max(0.0, (tt - ts[k]) / max(ts[k + 1] - ts[k], 1e-9)))
        if a['type'] == ANIM_AXIS_ANGLE:
            v0 = vs[k]; v1 = vs[min(k + 1, len(vs) - 1)]
            q = _slerp(_quat(v0[:3], v0[3]), _quat(v1[:3], v1[3]), f)
            ang = 2 * math.degrees(math.acos(max(-1.0, min(1.0, q[3]))))
            ax = q[:3] / max(np.linalg.norm(q[:3]), 1e-12)
            L[:3, :3] = rodrigues(ax, ang).T if np.linalg.norm(q[:3]) > 1e-9 else np.eye(3)
        elif a['type'] == ANIM_TRANSLATION:
            v0 = np.asarray(vs[k][:3]); v1 = np.asarray(vs[min(k + 1, len(vs) - 1)][:3])
            L[3, :3] = v0 + f * (v1 - v0)
    return L


def lane_clear_poses(C, M, line):
    """Looping animated scenery that carries collision (the N64 moves the collision with the node: the descent multiplies
    node transforms, n64-loader-re.md 7) has no Revenge counterpart the port plays; the converter writes one static pose.
    Rest pose (the load state) is kept unless its admitted collision comes within LANE_CLEAR_FT of the racing line; then
    the node is written at the keyframe of its own loop whose collision is farthest from the line (Vengeance: the
    three-armed fan over the corridor at line point 40, node 0x3575C, 24-s loop, 3.9 ft from the line at rest, 10.9 ft at\n    best: no time of its loop clears the line by 15 ft) [A loop, G choice of the time]. line = N64-space points along every link. Returns ({node: local}, report)."""
    from scipy.spatial import cKDTree
    tree = cKDTree(line)
    by_target = {}
    for a in M.anims:
        if a['target'] and a['target'] in M.nodes and a['type'] in (ANIM_AXIS_ANGLE, ANIM_TRANSLATION):
            by_target.setdefault(a['target'], []).append(a)
    out, rep = {}, []
    for off, anims in by_target.items():
        if off in C.posed:
            continue
        ids = [i for i, ml in enumerate(C.mesh_list) if (off in ml['chain_nodes'] or ml['node']['off'] == off) and C.mesh_track_ok(i)]
        if not ids:
            continue
        sel = np.isin(C.mesh_of, ids)
        F = C.F[sel]

        def dist(Fx):
            pts = np.vstack([Fx.reshape(-1, 3), Fx.mean(1), (Fx[:, 0] + Fx[:, 1]) / 2, (Fx[:, 1] + Fx[:, 2]) / 2, (Fx[:, 2] + Fx[:, 0]) / 2])
            return float(tree.query(pts)[0].min())
        d0 = dist(F)
        if d0 >= LANE_CLEAR_FT:
            continue
        Wr = node_world(M, off)
        loop = max([a['times'][-1] for a in anims if a['times']] or [0.0])
        if Wr is None or loop <= 0:
            rep.append(dict(node='0x%x' % off, rest_clearance_ft=round(d0, 1), chosen='rest (loop not usable)'))
            continue
        Wparent = np.linalg.inv(M.local_matrix(M.nodes[off])) @ Wr
        best = (d0, None)
        curve = []
        for time in np.arange(0.0, loop, LOOP_STEP_S):
            L = _pose_at(M, M.nodes[off], anims, float(time))
            A = np.linalg.inv(Wr) @ (L @ Wparent)
            dd = dist(F @ A[:3, :3] + A[3, :3])
            curve.append(dd)
            if dd > best[0] + 1e-6:
                best = (dd, float(time), L)
        r = dict(node='0x%x' % off, animations=[(a['type_name'], '0x%x' % a['off'], round(a['duration'], 2)) for a in anims],
                 collision_faces=int(sel.sum()), rest_clearance_ft=round(d0, 1),
                 loop_s=round(loop, 2), clearance_over_loop_ft=dict(min=round(min(curve), 1), max=round(max(curve), 1),
                                                                     share_of_loop_clear=round(float(np.mean(np.array(curve) >= LANE_CLEAR_FT)), 3)))
        if best[1] is not None:
            out[off] = best[2]
            r.update(chosen='loop time %.1f s' % best[1], clearance_ft=round(best[0], 1))
        else:
            r.update(chosen='rest (no time clearer)')
        rep.append(r)
    return out, rep


def node_world(M, off):
    """rest world matrix of node off (local @ parent ... root), 'all' walk; None if not reached."""
    for n, W, path in M.walk(M.header_nodes[0], select='all'):
        if n['off'] == off:
            return W
    return None


def subtree_point_maps(M, poses):
    """-> {node offset: A} so that a rest-pose world point p under that node maps to p @ A[:3,:3] + A[3,:3] posed."""
    out = {}
    for off, Lp in poses.items():
        Wr = node_world(M, off)
        if Wr is None:
            continue
        Lr = M.local_matrix(M.nodes[off])
        Wparent = np.linalg.inv(Lr) @ Wr
        Wp = Lp @ Wparent
        out[off] = np.linalg.inv(Wr) @ Wp
    return out


def looping_collision_nodes(C, M, stopped=()):
    """nodes driven by a looping axis-angle / translation animation (flags ENABLED | LOOP from load) whose subtree carries
    collision admitted for this track: the N64 moves that collision with the node (Vengeance: the fan 0x3575C)."""
    out = []
    for a in M.anims:
        off = a['target']
        if not off or off not in M.nodes or a['type'] not in (ANIM_AXIS_ANGLE, ANIM_TRANSLATION) or off in out or off in stopped:
            continue
        if not (a['flags'] & 0x10000000 and a['flags'] & 0x10):
            continue
        if any((off in ml['chain_nodes'] or ml['node']['off'] == off) and C.mesh_track_ok(i, include_anim=True)
               for i, ml in enumerate(C.mesh_list)):
            out.append(off)
    return out


def motion_frames(M, off, step=0.5, source_keys=False):
    """[(t, A)] over the node's own animation: A = row-vector 4x4 taking a rest-pose world point to its posed world
    position at time t (N64 slerp / linear interpolation, _pose_at). Duration = the longest key time."""
    anims = [a for a in M.anims if a['target'] == off and a['type'] in (ANIM_AXIS_ANGLE, ANIM_TRANSLATION) and a['values']]
    Wr = node_world(M, off)
    if not anims or Wr is None:
        return [], 0.0
    loop = max(a['times'][-1] for a in anims if a['times'])
    Wparent = np.linalg.inv(M.local_matrix(M.nodes[off])) @ Wr
    ts = sorted({0.0, loop, *(t for a in anims for t in a['times'])}) if source_keys else list(np.arange(0.0, loop, step)) + [loop]
    out = []
    for t in ts:
        L = _pose_at(M, M.nodes[off], anims, float(t)) if t < loop else _pose_at(M, M.nodes[off], anims, float(loop) - 1e-6)
        out.append((float(t), np.linalg.inv(Wr) @ (L @ Wparent)))
    return out, float(loop)