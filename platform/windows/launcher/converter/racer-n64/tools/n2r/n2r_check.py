r"""Offline checks and previews for a converted N64 track.
    python -I n2r_check.py CODE [--no-render]     (after n2r_convert.py; CODE = packed code, e.g. sc)

1. every RES member decoded by the port's offline decoders (RacerRevengePC\tools\assets: decode_scene48c
   PSG, decode_collision48g COL, decode_texture48d PST/PSM) and re-read/re-written byte-identical by the
   a2r readers (INI, BINARY, VUB, CUB, COL, PSG, PST);
2. spline reader limits and graph (every node reachable from 0, every path returns to 0);
3. facts and the SC limits (trk_facts / trk_silo.check);
4. trk_colcheck.run as it is (arcade definitions), then the general collision check: floor faces up,
   open floor edges near a lane without anything to stop a pod, lane sweep along every spline link
   (pod-height distance to any wall face, floor under every 10-ft sample in the zone tracked through
   the portals), start slots, portals through the grid;
5. trk_barriercheck;
6. previews: top view, and textured views of the N64 export and of the decoded converted PSGs from
   the same cameras (verify\cmp_*.png). LOOK at them.
"""
import json
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
N64 = os.path.normpath(os.path.join(HERE, '..', '..'))
A2R = os.path.normpath(os.path.join(N64, '..', 'racer-arcade', 'tools', 'a2r'))
REPO = os.path.normpath(os.path.join(N64, '..', '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(1, A2R)
sys.path.insert(2, os.path.join(REPO, 'tools', 'assets'))
sys.path.insert(3, os.path.normpath(os.path.join(A2R, '..')))
import n2r_space, n2r_io  # noqa: E402
import trk_res3, trk_col, trk_ini, trk_binparam, trk_vub, trk_sky, trk_facts, trk_verify  # noqa: E402
import geo_psg, geo_pst, geo_psm  # noqa: E402
from n2r_convert import FloorIndex, point_tri_dist, seg_hits_tris  # noqa: E402

BS = chr(92)
POD_CLEAR = 15.0
# output parent of n2r_convert.py --out (default export\n2r); set N2R_OUT for another track's output
OUT_PARENT = os.environ.get('N2R_OUT') or os.path.join(N64, 'export', 'n2r')


def decoders(files):
    import decode_scene48c as DS
    import decode_collision48g as DC
    import decode_texture48d as DT
    out = dict(psg=0, col=0, pst=0, psm=0, ini=0, binary=0, vub=0, cub=0, other=0, failures=[])
    for n, d in files.items():
        try:
            if n.endswith('.psg'):
                DS.decode_psg3_scene(d); assert geo_psg.write(geo_psg.parse(d)) == d; out['psg'] += 1
            elif n.endswith('.col'):
                DC.decode(d); assert trk_col.write_col(trk_col.read_col(d)) == d; out['col'] += 1
            elif n.endswith('.pst'):
                if 'particle' in n:
                    out['other'] += 1; continue
                DT.decode_texture(d); assert geo_pst.write(geo_pst.parse(d)) == d; out['pst'] += 1
            elif n.endswith('.psm'):
                DT.parse_material(d); out['psm'] += 1
            elif n.endswith(('.spl', '.pos', '.ini', '.prt')):
                trk_ini.parse(d); out['ini'] += 1
            elif n.endswith(('.scb', '.tob', '.aub')):
                assert trk_binparam.encode(trk_binparam.decode(d)) == d; out['binary'] += 1
            elif n.endswith('.vub'):
                v = trk_vub.read_vub(d); assert trk_vub.write_vub_raw(v) == d; out['vub'] += 1
            elif n.endswith('.cub'):
                c = trk_sky.read_cub(d); out['cub'] += 1
                ff = trk_sky.ffmpeg_path()
                if ff:
                    trk_sky.decode_ipum(c['tiles'][(0, 4)][3], ff)
            else:
                out['other'] += 1
        except Exception as e:   # noqa
            out['failures'].append('%s: %r' % (n, e))
    return out


def spline_graph(data):
    tr, nodes, P = trk_verify.load_spline(data)
    probs = []
    if len(data) > 65536:
        probs.append('bytes %d' % len(data))
    if len(nodes) > 256:
        probs.append('nodes %d' % len(nodes))
    for n in nodes:
        for k in ('LeftTrackWidth', 'RightTrackWidth', 'SpeedLimit', 'DistanceToFinish'):
            if not trk_verify.NUM.match(n[k]):
                probs.append('%s=%s' % (k, n[k]))
        for c in n['Position'].split(','):
            if not trk_verify.NUM.match(c):
                probs.append('Position %s' % c)
    nxt = [[int(n['Next%d' % k]) for k in range(int(n['NumNextNodes']))] for n in nodes]
    if any(j < 0 or j >= len(nodes) for ns in nxt for j in ns):
        probs.append('Next out of range')
    seen, st = set(), [0]
    while st:
        j = st.pop()
        if j in seen:
            continue
        seen.add(j); st += nxt[j]
    if len(seen) != len(nodes):
        probs.append('%d nodes unreachable from 0' % (len(nodes) - len(seen)))
    # main loop by Next0 returns to 0
    j, k = int(nodes[0]['Next0']), 0
    while j != 0 and k <= len(nodes):
        j = nxt[j][0]; k += 1
    if j != 0:
        probs.append('Next0 chain does not return to node 0')
    forks = [(i, ns) for i, ns in enumerate(nxt) if len(ns) > 1]
    return probs, nxt, P, nodes, forks


def paths(nxt):
    """main loop by Next0, then every branch from every fork (also forks on branches) to the first node already walked."""
    main, j = [0], nxt[0][0]
    while j != 0:
        main.append(j); j = nxt[j][0]
    out = [main + [0]]
    walked = set(main)
    todo = [(f, b) for f in main for b in nxt[f][1:]]
    while todo:
        f, b = todo.pop(0)
        p, j = [f], b
        while j not in walked:
            p.append(j); walked.add(j)
            todo += [(j, b2) for b2 in nxt[j][1:]]
            j = nxt[j][0]
        out.append(p + [j])
    return out


def col_sets(files, scb):
    zone_models = {}
    for n_, kv in scb:
        if '-model' in n_ and n_.startswith('zone'):
            zone_models.setdefault(int(n_[4:n_.index('-')]), []).append(dict(kv)['slt'].lower()[:-4])
    floor, wall, zfloor = [], [], {}
    kinds = {}
    for n, d in files.items():
        if n.startswith('geometry') and n.endswith('.col'):
            b = trk_col.read_col(d).body
            V = b.verts.astype(np.float64); F = V[b.faces['v'].astype(np.int64)]
            base = n.split(BS)[-1][:-4]
            nrm = b.faces['normal'].astype(np.float64)
            kinds[base] = (F, nrm)
            if 'floor' in base:
                # Revenge: a floor COL is ground AND obstacle (every COL is STATICOBJECT, track_collision_tags50an.cpp:7);
                # N64 sloped faces sit in floor COLs (the N64 ground ray reaches every up-facing face), so faces
                # steeper than 53 deg also count as walls for the clearance checks
                floor.append(F); wall.append(F[nrm[:, 1] < 0.6])
            else:
                wall.append(F)
    # one copy of every face (zones repeat faces)
    def uniq(L):
        A = np.concatenate(L) if L else np.zeros((0, 3, 3))
        _, k = np.unique(np.round(A.reshape(len(A), -1), 2), axis=0, return_index=True)
        return [A[np.sort(k)]]
    floor, wall = uniq(floor), uniq(wall)
    for z, ms in zone_models.items():
        zf = [kinds[m][0] for m in ms if m in kinds and 'floor' in m]
        if zf:
            zfloor[z] = FloorIndex(np.concatenate(zf))
    return kinds, np.concatenate(floor), np.concatenate(wall), zfloor


def general_colcheck(files, code, scb):
    kinds, FL, WL, zfloor = col_sets(files, scb)
    out = {}
    up = sum(int((nrm[:, 1] > 0.05).sum()) for b, (F, nrm) in kinds.items() if 'floor' in b)
    dn = sum(int((nrm[:, 1] <= 0.05).sum()) for b, (F, nrm) in kinds.items() if 'floor' in b)
    out['floor_faces_up'] = up; out['floor_faces_not_up'] = dn
    wy = np.concatenate([np.abs(nrm[:, 1]) for b, (F, nrm) in kinds.items() if 'floor' not in b])
    out['wall_faces'] = len(wy); out['wall_faces_steeper_than_53deg'] = int((wy < 0.6).sum())
    # spline lanes
    probs, nxt, AP, nodes, forks = spline_graph(files['tracks' + BS + 'a' + code + '.spl'])
    ps = paths(nxt)
    smp, starts = [], set()
    for p in ps:
        starts.add(len(smp))
        for i in range(len(p) - 1):
            a, b = AP[p[i]], AP[p[i + 1]]
            n = max(1, int(np.linalg.norm(b - a) // 10))
            smp += [a + (b - a) * k / n for k in range(n)]
    smp = np.array(smp)
    from scipy.spatial import cKDTree
    wc = WL.mean(1); wr = np.linalg.norm(WL - wc[:, None], axis=2).max(1); wt = cKDTree(wc)
    dmin = np.full(len(smp), np.inf)
    for i, s in enumerate(smp):
        q = s + np.array([0, 8.0, 0])
        ids = wt.query_ball_point(q, r=float(wr.max()) + 40)
        if ids:
            ids = np.array(ids)
            dmin[i] = point_tri_dist(q, WL[ids]).min()
    out['lane_paths'] = len(ps); out['lane_samples'] = len(smp)
    out['lane_min_wall_ft'] = round(float(dmin.min()), 1)
    close = smp[dmin < POD_CLEAR]
    out['lane_samples_closer_than_15ft'] = int(len(close))
    spots = []
    for i in np.argsort(dmin):
        if dmin[i] >= POD_CLEAR or len(spots) >= 12:
            break
        if all(np.linalg.norm(smp[i][[0, 2]] - np.array(s_[:2])) > 150 for s_ in spots):
            spots.append((round(float(smp[i][0])), round(float(smp[i][2])), round(float(dmin[i]), 1)))
    out['lane_close_spots'] = spots
    out['lane_samples_closer_than_8ft'] = int((dmin < 8).sum())
    # floor under every sample in its zone (through portals)
    Q = [np.array([[float(x) for x in dict(scb_d(scb)['portal%d' % i])['vertex%d' % k].split(',')] for k in (1, 2, 3, 4)])
         for i in range(1, int(dict(scb_d(scb)['sceneinfo'])['numportals']) + 1)]
    conn = [(int(dict(scb_d(scb)['portal%d' % (i + 1)])['connect1']), int(dict(scb_d(scb)['portal%d' % (i + 1)])['connect2'])) for i in range(len(Q))]
    import trk_colcheck as CC

    def fy(z, p):
        F = zfloor.get(z)
        return None if F is None else F.near(p[0], p[2], p[1], 25.0)
    zone = None
    dbg_t = dbg_z = None
    dbgp = os.path.join(OUT_PARENT, code, 'debug.npz')
    if os.path.exists(dbgp):
        dd_ = np.load(dbgp); dbg_t = cKDTree(dd_['S']); dbg_z = dd_['Sz']
    nofloor, steps, prev = [], [], None
    for i in range(len(smp)):
        if i in starts:
            # a path starts at node 0 or at a fork: its zone is the converter's zone of that lane point
            # (zones share faces, so 'first zone with floor here' is not a zone test)
            zone = int(dbg_z[dbg_t.query(smp[i])[1]]) if dbg_t is not None else next((z for z in sorted(zfloor) if fy(z, smp[i]) is not None), zone)
            if i:
                # a new path starts at a node already walked: its zone = the zone found there
                pass
        else:
            for qi, q in enumerate(Q):
                if CC._seg_quad(smp[i - 1] + [0, 20.0, 0], smp[i] + [0, 20.0, 0], q):
                    a, b = conn[qi]; zone = b if zone == a else a if zone == b else zone
        y = fy(zone, smp[i])
        if y is None:
            nofloor.append((round(float(smp[i][0])), round(float(smp[i][2])), zone)); prev = None; continue
        if prev is not None and abs(y - prev) > 3.0 and i not in starts:
            ys = [fy(zone, smp[i - 1] + (smp[i] - smp[i - 1]) * k / 10.0) for k in range(11)]
            ys = [v for v in ys if v is not None]
            jump = max((abs(ys[k + 1] - ys[k]) for k in range(len(ys) - 1)), default=0)
            if jump > 1.0:
                steps.append((round(float(smp[i][0])), round(float(smp[i][2])), round(jump, 1)))
        prev = y
    out['lane_samples_without_floor_in_zone'] = len(nofloor); out['lane_no_floor_points'] = nofloor[:10]
    out['lane_floor_steps_over_1ft'] = len(steps); out['lane_step_points'] = steps[:10]
    # start slots
    mtr, mnodes, MP = trk_verify.load_spline(files['tracks' + BS + 'm' + code + '.spl'])
    tw = float(mtr['TrackWidth'])
    grid, _ = trk_verify.revenge_grid(MP, tw, mnodes)
    slot = []
    for g in grid:
        off = g - MP[0]
        pts, run, j = [], 0.0, 0
        while run < 800:
            a, b = MP[j], MP[int(mnodes[j]['Next0'])]
            n = max(1, int(np.linalg.norm(b - a) // 10))
            pts += [a + (b - a) * k / n + off for k in range(n)]
            run += np.linalg.norm(b - a); j = int(mnodes[j]['Next0'])
        d = []
        for p in pts:
            q = p + np.array([0, 8.0, 0]); ids = wt.query_ball_point(q, r=float(wr.max()) + 40)
            d.append(point_tri_dist(q, WL[np.array(ids)]).min() if ids else np.inf)
        slot.append(round(float(min(d)), 1))
    out['start_slot_min_wall_ft'] = slot
    allf = FloorIndex(FL)
    drv = FL[np.array([np.cross(t[1] - t[0], t[2] - t[0])[1] / max(np.linalg.norm(np.cross(t[1] - t[0], t[2] - t[0])), 1e-9) for t in FL]) >= 0.6]
    out['start_slots_on_floor'] = int(sum(allf.near(g[0], g[2], g[1], 20.0) is not None for g in grid))
    out['portals_between_node0_and_slots'] = [(k, i + 1) for k, g in enumerate(grid) for i, q in enumerate(Q)
                                              if CC._seg_quad(MP[0] + [0, 20.0, 0], g + [0, 20.0, 0], q)]
    # open floor edges near the lanes with nothing to stop a pod
    edges = {}
    for i, tri in enumerate(drv):
        for k in range(3):
            e = frozenset((tuple(np.float32(tri[k]).tolist()), tuple(np.float32(tri[(k + 1) % 3]).tolist())))
            edges.setdefault(e, []).append((i, k))
    st = cKDTree(smp)
    stats = dict(open=0, near_lane=0, seam=0, walled=0, transverse_or_ledge=0, unwalled_side=0)
    unwalled, voids = [], []
    for e, own in edges.items():
        if len(own) != 1:
            continue
        stats['open'] += 1
        i, k = own[0]; tri = drv[i]; a, b, third = tri[k], tri[(k + 1) % 3], tri[(k + 2) % 3]
        mid = (a + b) / 2
        dd, si = st.query(mid)
        if dd > 400 or abs(smp[si][1] - mid[1]) > 150:
            continue
        stats['near_lane'] += 1
        ed = b - a; ed[1] = 0; el = np.linalg.norm(ed)
        if el < 1e-6:
            continue
        ed /= el; on = np.array([ed[2], 0, -ed[0]])
        if on @ (third - mid) > 0:
            on = -on
        seam = cov = void = cov40 = 0
        for t in (0.25, 0.5, 0.75):
            q = a + (b - a) * t
            if allf.near(*(q + on * 4.0)[[0, 2]], q[1], 6.0) is not None:
                seam += 1; continue
            pv = q + on * 12.0; hb = allf.heights(pv[0], pv[2])
            isv = not ((hb <= q[1] + 6.0) & (hb >= q[1] - 300.0)).any()
            void += isv
            ids = wt.query_ball_point(q + [0, 8.0, 0], r=float(wr.max()) + 12)
            if ids:
                ids = np.array(ids); d = point_tri_dist(q + np.array([0, 8.0, 0]), WL[ids])
                if (d < 10).any() and (WL[ids[d < 10]][:, :, 1].max() > q[1] + 40).any():
                    cov40 += 1
            ids = wt.query_ball_point(q + [0, 8.0, 0], r=float(wr.max()) + 12)
            if ids:
                ids = np.array(ids); d = point_tri_dist(q + np.array([0, 8.0, 0]), WL[ids])
                if (d < 10).any() and (WL[ids[d < 10]][:, :, 1].max() > q[1] + 8).any():
                    cov += 1
        if seam < 2 and void >= 2 and seam + cov40 < 2:
            stats['void_unwalled'] = stats.get('void_unwalled', 0) + 1
            voids.append(np.round(mid[[0, 2]]).tolist())
        if seam >= 2:
            stats['seam'] += 1
        elif seam + cov >= 2:
            stats['walled'] += 1
        else:
            j = si + 1 if si + 1 < len(smp) else si - 1
            f = smp[j] - smp[si]; f[1] = 0; f /= max(np.linalg.norm(f), 1e-9)
            if abs(ed @ f) < 0.5:
                stats['transverse_or_ledge'] += 1
            else:
                stats['unwalled_side'] += 1; unwalled.append(np.round(mid[[0, 2]]).tolist())
    out['open_floor_edges'] = stats; out['unwalled_side_edges_at'] = unwalled[:10]; out['void_unwalled_at'] = voids[:12]
    out['spline_problems'] = probs
    out['forks'] = [(i, ns) for i, ns in forks]
    return out, smp, grid


def scb_d(scb):
    d = {}
    for n, kv in scb:
        d.setdefault(n, []).extend(kv)
    return d


# ------------------------------------------------------------ previews
def converted_visual(files, X):
    tex = {}
    for n, d in files.items():
        if n.endswith('.pst') and n.startswith('textures'):
            tex[n.split(BS)[-1][:-4]] = geo_pst.rgba_levels(geo_pst.parse(d))[-1]
    P, T, UV, tid, imgs, idx = [], [], [], [], [], {}
    base = 0
    for name, m, p, t, uv in trk_verify.load_visual(files):
        m = m.lower()
        if m not in idx:
            idx[m] = len(imgs); im = tex.get(m)
            imgs.append(im if im is not None else np.full((16, 16, 4), 200, np.uint8))
        q = np.asarray(p, float) - np.array(X['offset'])
        q = n2r_space.inverse_dirs(q / X['scale'], X)
        P.append(q); T.append(np.asarray(t) + base); UV.append(np.asarray(uv)); tid += [idx[m]] * len(t); base += len(q)
    return np.vstack(P), np.vstack(T), np.vstack(UV), np.array(tid), imgs


def n64_visual(D):
    from PIL import Image as I
    P, T, UV, tid, imgs, idx = [], [], [], [], [], {}
    base = 0
    for g in D['geometry']:
        e = D['materials'][g['mtl']]
        t = e.get('texture')
        if t not in idx:
            idx[t] = len(imgs)
            im = np.asarray(I.open(os.path.join(D['dir'], 'textures', t)).convert('RGBA')) if t else np.full((16, 16, 4), 255, np.uint8)
            imgs.append(im)
        n = len(g['P'])
        P.append(g['P'].reshape(-1, 3)); UV.append(g['UV'].reshape(-1, 2)); T.append(np.arange(n * 3).reshape(-1, 3) + base)
        tid += [idx[t]] * n; base += n * 3
    return np.vstack(P), np.vstack(T), np.vstack(UV), np.array(tid), imgs


def render(vis, eye, tgt, W=640, H=360):
    import softrender as SR
    Pv, Tv, UVv, tv, imgs = vis
    cen = Pv[Tv].mean(1)
    keep = np.linalg.norm(cen - eye, axis=1) < 9000
    cam = dict(eye=eye, target=tgt, up=(0, 0, 1), fov=65, near=2.0)
    img, _, _ = SR.render(Pv, Tv[keep], cam, W, H, uv=UVv, tex_of_tri=tv[keep], textures=[i[..., :3] for i in imgs], ambient=0.75, bg=(120, 150, 190))
    return Image.fromarray(img)


def previews(code, root, files, X, D, smp, grid, cmp=True):
    vd = os.path.join(root, 'verify'); os.makedirs(vd, exist_ok=True)
    scb = trk_binparam.decode(files['tracks' + BS + code + '.scb'])
    kinds, FL, WL, _ = col_sets(files, scb)
    dbg = np.load(os.path.join(root, 'debug.npz'))
    # top view
    allp = FL.reshape(-1, 3)
    lo = allp[:, [0, 2]].min(0) - 100; hi = allp[:, [0, 2]].max(0) + 100
    S = 2400 / (hi - lo).max(); Wd, Hd = (np.ceil((hi - lo) * S) + 1).astype(int)
    im = Image.new('RGB', (Wd, Hd), (18, 18, 24)); dr = ImageDraw.Draw(im)
    tp = lambda q: (float((q[0] - lo[0]) * S), float(Hd - (q[2] - lo[1]) * S))
    for b, (F, nrm) in kinds.items():
        col = (90, 90, 100) if 'base' in b else (150, 130, 90) if 'dust' in b else (60, 150, 60) if 'fast' in b else None
        for f in F:
            if col:
                dr.polygon([tp(v) for v in f], fill=col)
    for b, (F, nrm) in kinds.items():
        if 'floor' in b:
            continue
        col = (255, 60, 220) if 'genwall' in b else (230, 120, 255)
        for f in F:
            o = np.argsort(f[:, 1])[:2]
            dr.line([tp(f[o[0]]), tp(f[o[1]])], fill=col, width=1)
    probs, nxt, AP, nodes, forks = spline_graph(files['tracks' + BS + 'a' + code + '.spl'])
    for j, ns in enumerate(nxt):
        for k, n_ in enumerate(ns):
            dr.line([tp(AP[j]), tp(AP[n_])], fill=(255, 230, 0) if nodes[j]['BranchID'] == '0' else (255, 150, 0), width=2)
    for i in range(1, int(dict(scb_d(scb)['sceneinfo'])['numportals']) + 1):
        v = [np.array([float(x) for x in dict(scb_d(scb)['portal%d' % i])['vertex%d' % k].split(',')]) for k in (1, 3)]
        dr.line([tp(v[0]), tp(v[1])], fill=(0, 220, 255), width=3)
    for g in grid:
        x, y = tp(g); dr.ellipse((x - 3, y - 3, x + 3, y + 3), fill=(255, 255, 255))
    for key, c in (('cut0', (255, 255, 255)), ('cut1', (180, 180, 180)), ('cut2', (120, 120, 120))):
        pth = dbg[key]
        dr.line([tp(q) for q in pth], fill=c, width=1)
    for q in dbg['tv']:
        x, y = tp(q); dr.rectangle((x - 2, y - 2, x + 2, y + 2), fill=(60, 120, 255))
    im.save(os.path.join(vd, 'top.png'))
    if not cmp:
        return
    # same-camera comparisons, N64 frame
    nv = n64_visual(D)
    cv = converted_visual(files, X)
    spl = D['splines']
    views = []

    def along(pid, back=120, up=45, ahead=300):
        p = np.array(spl['points'][pid]['position']); h = np.array(spl['points'][pid]['handle_out']) - p
        h /= np.linalg.norm(h)
        return p - h * back + [0, 0, up], p + h * ahead + [0, 0, 10]
    # the start, every fork, and points spread over the lap (the same list on every track; SMR's old list was
    # start, fork23, fork10, mine15, p33, fork49, p55, p88)
    npt = len(spl['points'])
    forks_ = sorted(set(spl['graph']['forks']))
    spread = [int(round(npt * f)) for f in (0.15, 0.35, 0.55, 0.75, 0.9)]
    for label, pid in [('start', 0)] + [('fork%d' % f, f) for f in forks_] + [('p%d' % q, q) for q in spread if q not in forks_]:
        if pid < npt:
            views.append((label,) + along(pid))
    import n2r_convert as NCV
    intro = D['cameras']['intro']; ek = NCV.intro_eye_keys(intro)
    views.append(('intro0', np.array(ek['positions'][0]), np.array(intro['target_keys']['positions'][0])))
    views.append(('intro_end', np.array(ek['positions'][-1]), np.array(intro['target_keys']['positions'][-1])))
    for label, eye, tgt in views:
        a = render(nv, eye, tgt); b = render(cv, eye, tgt)
        out = Image.new('RGB', (1290, 380), (0, 0, 0)); out.paste(a, (0, 20)); out.paste(b, (650, 20))
        d = ImageDraw.Draw(out); d.text((4, 4), 'N64 export: %s' % label, fill=(255, 255, 0)); d.text((654, 4), 'converted (decoded PSG): %s' % label, fill=(255, 255, 0))
        out.save(os.path.join(vd, 'cmp_%s.png' % label))


def main():
    code = sys.argv[1]
    root = os.path.join(OUT_PARENT, code)
    man = json.load(open(os.path.join(root, 'manifest.json')))
    res = os.path.join(root, 'install', 'TRACKS', '%s.RES' % code.upper())
    blob = open(res, 'rb').read()
    r = trk_res3.read_res3(blob)
    files = {n.lower(): d for n, d in r.items()}
    rep = {}
    rep['archive'] = dict(bytes=len(blob), members=len(r.members), rewrite_identical=trk_res3.write_res3(list(r.items())) == blob)
    rep['decoders'] = decoders(files)
    for k in ('a', 'm'):
        rep['spline_' + k] = spline_graph(files['tracks' + BS + k + code + '.spl'])[0]
    f = trk_facts.facts(blob, code)
    f['texture_source_bytes'] = sum(len(d) for n, d in r.items() if n.lower().startswith('textures' + BS) and n.lower().endswith('.pst'))
    import trk_silo
    # trk_silo now has one COMMON_LIMITS table for every code (multi-slot-ready.md 2.3)
    _, f2, _, lim, today = trk_silo._checked(code, root, quiet=True) if code in trk_silo.ORIGINAL_PINS else (None, f, None, [], [])
    ok = all(x['ok'] for x in lim)
    rep['limits_ok'] = ok; rep['limits'] = [(x['check'], x['value'], x['op'], x['limit'], x['ok']) for x in lim]
    rep['port_standins_today'] = today
    rep['facts'] = {k: v for k, v in f.items() if k != 'archive_sha256'}
    import trk_colcheck
    try:
        rep['trk_colcheck_as_is'] = trk_colcheck.run(res, code)
    except Exception as e:   # noqa
        rep['trk_colcheck_as_is'] = 'error %r' % e
    scb = trk_binparam.decode(files['tracks' + BS + code + '.scb'])
    g, smp, grid = general_colcheck(files, code, scb)
    rep['general_colcheck'] = g
    import trk_barriercheck
    vd = os.path.join(root, 'verify'); os.makedirs(vd, exist_ok=True)
    gaps = trk_barriercheck.main(res, code, os.path.join(vd, 'barrier_gaps.png'))
    rep['barriercheck_gaps'] = len(gaps)
    json.dump(rep, open(os.path.join(vd, 'check.json'), 'w'), indent=1, default=str)
    print(json.dumps(rep, indent=1, default=str)[:12000])
    D = n2r_io.load(man['model'])
    previews(code, root, files, man['transform'], D, smp, grid, cmp='--no-render' not in sys.argv)


if __name__ == '__main__':
    main()
