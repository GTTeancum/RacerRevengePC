r"""Collision checks on a converted track RES (world-space COLs at Position 0):
- face orientation by kind (floor files must face up, wall files must be vertical and face the track);
- cracks: every edge of the floor union that is used by only one floor face must carry a wall base
  (no open floor edge anywhere, including zone splits);
- zone-split edges coincide exactly (an edge shared by floor faces of two different COL files);
- every AI spline node has a floor face directly below it.
    python -I trk_colcheck.py <RES> <code>
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import trk_res3, trk_col, trk_binparam, trk_ini  # noqa: E402

BS = chr(92)


def run(path, code):
    r = trk_res3.read_res3(open(path, 'rb').read())
    files = {n.lower(): d for n, d in r.items()}
    scb = dict(trk_binparam.decode(files['tracks' + BS + code + '.scb']))
    pos = {}
    for n, kv in scb.items() if isinstance(scb, dict) else []:
        if '-model' in n:
            d = dict(kv)
            pos[d['slt'].lower()[:-4]] = np.array([float(x) for x in d['position'].split(',')])
    out = dict(floor_up=0, floor_down=0, floor_side=0, wall_vertical=0, wall_other=0)
    edges = {}
    wall_base = set()
    floor_faces = []
    for n, d in files.items():
        if not (n.startswith('geometry') and n.endswith('.col')):
            continue
        b = trk_col.read_col(d).body
        base = n.split(BS)[-1][:-4]
        V = b.verts.astype(np.float64) + pos.get(base, 0.0)
        F = b.faces['v'].astype(np.int64)
        nrm = b.faces['normal'].astype(np.float64)
        if 'floor' in base:
            out['floor_up'] += int((nrm[:, 1] > 0.05).sum()); out['floor_down'] += int((nrm[:, 1] < -0.05).sum())
            out['floor_side'] += int((np.abs(nrm[:, 1]) <= 0.05).sum())
            for f in F:
                tri = [tuple(np.float32(V[i]).tolist()) for i in f]
                floor_faces.append(np.array(tri))
                for k in range(3):
                    edges.setdefault(frozenset((tri[k], tri[(k + 1) % 3])), set()).add(base)
                    edges.setdefault(('n', frozenset((tri[k], tri[(k + 1) % 3]))), [0])[0] += 1
        else:
            out['wall_vertical'] += int((np.abs(nrm[:, 1]) < 0.01).sum()); out['wall_other'] += int((np.abs(nrm[:, 1]) >= 0.01).sum())
            for f in F:
                tri = [tuple(np.float32(V[i]).tolist()) for i in f]
                # top vertices have a partner straight below them in the same triangle
                tops = [t for t in tri if any(u != t and u[0] == t[0] and u[2] == t[2] and u[1] < t[1] for u in tri)]
                lo = [t for t in tri if t not in tops]
                if len(lo) == 2:
                    wall_base.add(frozenset(lo))
    open_edges = [e for e in edges if not isinstance(e, tuple) and edges[('n', e)][0] == 1]
    uncovered = [e for e in open_edges if e not in wall_base]
    split_edges = sum(1 for e in edges if not isinstance(e, tuple) and len(edges[e]) > 1)
    out.update(open_floor_edges=len(open_edges), open_edges_without_wall=len(uncovered), zone_split_edges_exact=split_edges)
    # AI nodes over floor
    ini = trk_ini.parse(files['tracks' + BS + 'a' + code + '.spl'])
    P = np.array([[float(x) for x in kv[0][1].split(',')] for n, kv in ini.sections()[1:]])
    FF = np.array(floor_faces)
    A, B, C = FF[:, 0], FF[:, 1], FF[:, 2]
    miss = 0
    for p in P:
        d = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])
        with np.errstate(all='ignore'):
            l0 = ((B[:, 2] - C[:, 2]) * (p[0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (p[2] - C[:, 2])) / d
            l1 = ((C[:, 2] - A[:, 2]) * (p[0] - C[:, 0]) + (A[:, 0] - C[:, 0]) * (p[2] - C[:, 2])) / d
        okm = (l0 >= -1e-4) & (l1 >= -1e-4) & (1 - l0 - l1 >= -1e-4) & (np.abs(d) > 1e-9)
        miss += not okm.any()
    out['ai_nodes'] = len(P); out['ai_nodes_without_floor'] = miss
    # walls facing the track: horizontal normal toward the nearest map-spline node
    mini = trk_ini.parse(files['tracks' + BS + 'm' + code + '.spl'])
    MP = np.array([[float(x) for x in kv[0][1].split(',')] for n, kv in mini.sections()[1:]])
    face_in = face_tot = 0
    for n, d in files.items():
        if n.startswith('geometry') and n.endswith('.col') and 'floor' not in n:
            b = trk_col.read_col(d).body
            V = b.verts.astype(np.float64)
            for f, nrm in zip(b.faces['v'], b.faces['normal'].astype(np.float64)):
                c = V[list(f)].mean(0)
                k = np.argmin(((MP[:, [0, 2]] - c[[0, 2]]) ** 2).sum(1))
                face_tot += 1
                face_in += (nrm[[0, 2]] @ (MP[k] - c)[[0, 2]]) > 0
    out['wall_faces'] = face_tot; out['wall_faces_facing_track'] = face_in
    out.update(sweep(files, code, pos))
    return out


POD_CLEAR = 15.0     # ft: pod half-width (POD01 chariot radius 6.5 ft, engines wider) + margin


def _wall_bases(files, pos):
    segs = []
    for n, d in files.items():
        if n.startswith('geometry') and n.endswith('.col') and 'floor' not in n:
            b = trk_col.read_col(d).body
            V = b.verts.astype(np.float64) + pos.get(n.split(BS)[-1][:-4], 0.0)
            for f in b.faces['v']:
                t = V[list(f)]
                lo = t[:, 1].min()
                base = t[np.abs(t[:, 1] - lo) < 1e-3]
                if len(base) == 2:
                    segs.append((base[0], base[1], t[:, 1].max()))
    A = np.array([s_[0] for s_ in segs]); B = np.array([s_[1] for s_ in segs]); top = np.array([s_[2] for s_ in segs])
    return A, B, top


def _dist(P, A, B, top):
    """horizontal distance from points P (n,3) to wall base segments whose height range covers P."""
    a = A[:, [0, 2]]; b = B[:, [0, 2]]; ab = b - a; L2 = np.maximum((ab ** 2).sum(1), 1e-9)
    out = np.full(len(P), np.inf)
    for i, p in enumerate(P):
        ok = (p[1] > np.minimum(A[:, 1], B[:, 1]) - 50) & (p[1] < top)
        if not ok.any():
            continue
        t = np.clip(((p[[0, 2]] - a[ok]) * ab[ok]).sum(1) / L2[ok], 0, 1)
        q = a[ok] + ab[ok] * t[:, None]
        out[i] = np.sqrt(((q - p[[0, 2]]) ** 2).sum(1)).min()
    return out


def sweep(files, code, pos):
    """Pod-sized sweeps: along the AI spline (every 10 ft) and, from every Revenge start slot, along
    the map spline for the first 800 ft at the slot's lateral offset. Each sample must stay
    POD_CLEAR away from every wall base (and no sweep segment may cross a wall)."""
    import trk_verify
    A, B, top = _wall_bases(files, pos)
    tr, nodes, AP = trk_verify.load_spline(files['tracks' + BS + 'a' + code + '.spl'])
    smp = []
    for i in range(len(AP)):
        a, b = AP[i], AP[(i + 1) % len(AP)]
        n = max(1, int(np.linalg.norm(b - a) // 10))
        smp += [a + (b - a) * k / n for k in range(n)]
    smp = np.array(smp)
    d = _dist(smp, A, B, top)
    mtr, mnodes, MP = trk_verify.load_spline(files['tracks' + BS + 'm' + code + '.spl'])
    tw = float(trk_ini.parse(files['tracks' + BS + 'm' + code + '.spl']).get('Track', 'TrackWidth'))
    grid, _ = trk_verify.revenge_grid(MP, tw, mnodes)
    slot_min = []
    for g in grid:
        off = g - MP[0]                      # keep the slot's offset from spline node 0 for 800 ft
        pts, run, j = [], 0.0, 0
        while run < 800 and j < len(MP):
            a, b = MP[j], MP[(j + 1) % len(MP)]
            n = max(1, int(np.linalg.norm(b - a) // 10))
            pts += [a + (b - a) * k / n + off for k in range(n)]
            run += np.linalg.norm(b - a); j += 1
        slot_min.append(float(_dist(np.array(pts), A, B, top).min()))
    # portals between map node 0 and any start slot (none on the 13 originals)
    scb = dict(trk_binparam.decode(files['tracks' + BS + code + '.scb']))
    Q = [np.array([[float(x) for x in dict(scb['portal%d' % i])['vertex%d' % k].split(',')] for k in (1, 2, 3, 4)])
         for i in range(1, int(dict(scb['sceneinfo'])['numportals']) + 1)]
    split = [(k, i + 1) for k, g in enumerate(grid) for i, q in enumerate(Q)
             if _seg_quad(MP[0] + np.array([0, 20.0, 0]), g + np.array([0, 20.0, 0]), q)]
    # zone-aware floor along the AI lane: walk the samples, switch zone at every portal crossed, and
    # require a floor face below each sample in that zone's COL files; also report floor steps
    zfloor = {}
    for n_, kv in trk_binparam.decode(files['tracks' + BS + code + '.scb']):
        if '-model' in n_ and n_.startswith('zone'):
            z_ = int(n_[4:n_.index('-')])
            base = dict(kv)['slt'].lower()[:-4]
            cn = 'geometry' + BS + base + '.col'
            if cn in files and 'floor' in base:
                b_ = trk_col.read_col(files[cn]).body
                V = b_.verts.astype(np.float64) + pos.get(base, 0.0)
                zfloor.setdefault(z_, []).append(V[b_.faces['v'].astype(np.int64)])
    zfloor = {k: np.concatenate(v) for k, v in zfloor.items()}
    conn = [(int(dict(scb['portal%d' % (i + 1)])['connect1']), int(dict(scb['portal%d' % (i + 1)])['connect2'])) for i in range(len(Q))]

    def floor_y(z_, p):
        F = zfloor.get(z_)
        if F is None:
            return None
        A_, B_, C_ = F[:, 0], F[:, 1], F[:, 2]
        dd = (B_[:, 2] - C_[:, 2]) * (A_[:, 0] - C_[:, 0]) + (C_[:, 0] - B_[:, 0]) * (A_[:, 2] - C_[:, 2])
        with np.errstate(all='ignore'):
            l0 = ((B_[:, 2] - C_[:, 2]) * (p[0] - C_[:, 0]) + (C_[:, 0] - B_[:, 0]) * (p[2] - C_[:, 2])) / dd
            l1 = ((C_[:, 2] - A_[:, 2]) * (p[0] - C_[:, 0]) + (A_[:, 0] - C_[:, 0]) * (p[2] - C_[:, 2])) / dd
        ok_ = (l0 >= -1e-4) & (l1 >= -1e-4) & (1 - l0 - l1 >= -1e-4) & (np.abs(dd) > 1e-9)
        if not ok_.any():
            return None
        y = (l0 * A_[:, 1] + l1 * B_[:, 1] + (1 - l0 - l1) * C_[:, 1])[ok_]
        return float(y[np.argmin(np.abs(y - p[1]))])
    zone = next((z_ for z_ in sorted(zfloor) if floor_y(z_, smp[0]) is not None), None)
    no_floor, steps, slopes, prev_y = [], [], [], None
    for i_ in range(len(smp)):
        if i_:
            for qi, q_ in enumerate(Q):
                if _seg_quad(smp[i_ - 1] + np.array([0, 20.0, 0]), smp[i_] + np.array([0, 20.0, 0]), q_):
                    a_, b_ = conn[qi]
                    zone = b_ if zone == a_ else a_ if zone == b_ else zone
        y = floor_y(zone, smp[i_])
        if y is None:
            no_floor.append((round(float(smp[i_][0])), round(float(smp[i_][2])), zone))
            prev_y = None
            continue
        if prev_y is not None and abs(y - prev_y) > 3.0:
            # re-sample every 1 ft: a real step jumps > 1 ft between neighbouring samples, a slope does not
            ys = [floor_y(zone, smp[i_ - 1] + (smp[i_] - smp[i_ - 1]) * k_ / 10.0) for k_ in range(11)]
            ys = [v for v in ys if v is not None]
            jump = max((abs(ys[k_ + 1] - ys[k_]) for k_ in range(len(ys) - 1)), default=0.0)
            if jump > 1.0:
                steps.append((round(float(smp[i_][0])), round(float(smp[i_][2])), round(jump, 2)))
            else:
                slopes.append(abs(y - prev_y) / max(np.linalg.norm((smp[i_] - smp[i_ - 1])[[0, 2]]), 1e-6))
        prev_y = y
    bad = smp[d < POD_CLEAR]
    return dict(ai_lane_samples=len(smp), ai_lane_min_wall_ft=round(float(d.min()), 1),
                ai_lane_samples_closer_than_clearance=int((d < POD_CLEAR).sum()),
                ai_lane_close_points=np.round(bad[:6][:, [0, 2]], 0).tolist(),
                start_slot_min_wall_ft=[round(x, 1) for x in slot_min],
                start_slots_closer_than_clearance=int(sum(x < POD_CLEAR for x in slot_min)),
                portals_between_node0_and_start_slots=split,
                ai_lane_samples_without_floor_in_current_zone=len(no_floor), ai_lane_no_floor_points=no_floor[:8],
                ai_lane_floor_steps_over_1ft=len(steps), ai_lane_floor_step_points=steps[:8],
                ai_lane_steepest_slope_deg=round(float(np.degrees(np.arctan(max(slopes)))), 1) if slopes else None)


def _seg_quad(a, b, q):
    n = np.cross(q[1] - q[0], q[3] - q[0]); da = (a - q[0]) @ n; db = (b - q[0]) @ n
    if da * db > 0 or da == db:
        return False
    p = a + (b - a) * (da / (da - db))
    for v0, v1, v2 in ((q[0], q[1], q[2]), (q[0], q[2], q[3])):
        e0 = v1 - v0; e1 = v2 - v0; w = p - v0
        d00, d01, d11, d20, d21 = e0 @ e0, e0 @ e1, e1 @ e1, w @ e0, w @ e1
        den = d00 * d11 - d01 * d01
        if den == 0:
            continue
        vv = (d11 * d20 - d01 * d21) / den; ww = (d00 * d21 - d01 * d20) / den
        if vv >= -1e-6 and ww >= -1e-6 and vv + ww <= 1 + 1e-6:
            return True
    return False


if __name__ == '__main__':
    print(run(sys.argv[1], sys.argv[2]))
