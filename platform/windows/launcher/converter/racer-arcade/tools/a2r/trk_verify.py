r"""Verify converted track files: re-read every output with the a2r readers, apply the port's
reader limits, compare value ranges with the 13 originals, and draw a top view.

    python -I trk_verify.py CODE [CODE ...]       (after trk_convert.py)
Writes export\a2r\<code>\verify\top.png, start.png, verify.json, and export\a2r\ranges.txt.
"""
import glob
import json
import math
import os
import re
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import trk_ini, trk_binparam, trk_col, trk_res3, trk_vub  # noqa: E402

BS = chr(92)
A2R = os.path.join(SCR, 'export', 'a2r')
TRACKS = 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split()
NUM = re.compile(r'^-?\d{1,11}(\.\d{1,6})?$')


def load_spline(data):
    S = trk_ini.parse(data).sections()
    tr = dict(S[0][1])
    nodes = [dict(kv) for n, kv in S[1:]]
    assert int(tr['NumNodes']) == len(nodes)
    P = np.array([[float(x) for x in n['Position'].split(',')] for n in nodes])
    return tr, nodes, P


def spline_stats(tr, nodes, P):
    tw = float(tr['TrackWidth'])
    L = np.array([float(n['LeftTrackWidth']) for n in nodes]) + tw / 2
    R = np.array([float(n['RightTrackWidth']) for n in nodes]) + tw / 2
    nx = [int(n['Next0']) for n in nodes]
    seg = np.linalg.norm(P[nx] - P, axis=1)
    sp = np.array([float(n.get('SpeedLimit', 'nan')) for n in nodes])
    return dict(nodes=len(nodes), spacing_med=float(np.median(seg)), spacing_min=float(seg.min()),
                spacing_max=float(seg.max()), half_left_med=float(np.median(L)), half_right_med=float(np.median(R)),
                width_med=float(np.median(L + R)), width_min=float((L + R).min()), width_max=float((L + R).max()),
                speed_min=float(np.nanmin(sp)), speed_max=float(np.nanmax(sp)),
                progress=sum(1 for n in nodes if 'ProgressNode' in n), D1=float(nodes[1]['DistanceToFinish']))


def strict_spline_check(data, nodes):
    """Port reader limits: <= 256 nodes, <= 64 KB, numbers <= 11 digits / 6 fraction digits."""
    problems = []
    if len(data) > 65536: problems.append('file %d bytes > 65536' % len(data))
    if len(nodes) > 256: problems.append('%d nodes > 256' % len(nodes))
    for n in nodes:
        for k in ('LeftTrackWidth', 'RightTrackWidth', 'SpeedLimit', 'DistanceToFinish'):
            if not NUM.match(n[k]): problems.append('%s=%s' % (k, n[k]))
        for c in n['Position'].split(','):
            if not NUM.match(c): problems.append('Position %s' % c)
    # graph: one loop through every node back to 0
    seen = set(); i = 0
    while i not in seen:
        seen.add(i); i = int(nodes[i]['Next0'])
    if len(seen) != len(nodes) or i != 0: problems.append('Next0 graph is not one loop through all nodes')
    return problems


def gate(P, j, prev, left_w, right_w):
    """Port gates(): bisector of forward/backward rotated about up; returns (left pt, right pt)."""
    cur = P[j]; nxt = P[(j + 1) % len(P)]; prv = P[prev]
    f = nxt - cur; f[1] = 0; f /= np.linalg.norm(f)
    b = prv - cur; b[1] = 0; b /= np.linalg.norm(b)
    ang = math.acos(max(-1.0, min(1.0, float(f @ b))))
    half = ang / 2

    def rot(v, a):
        cr = np.array([v[2], 0.0, -v[0]])
        return math.cos(a) * v + math.sin(a) * cr
    r = rot(f, half); r /= np.linalg.norm(r)
    reverse = float(b @ r) < 0
    if reverse:
        r = rot(f, -half)
    lw = left_w if reverse else -left_w
    rw = -right_w if reverse else right_w
    return cur + lw * r, cur + rw * r


def revenge_grid(P, mtw, nodes):
    """source_grid50ig_fixture.h detail::prepare, ranks 0..7, map node 0 and its predecessor."""
    K = len(P)
    prev = [i for i in range(K) if int(nodes[i]['Next0']) == 0][0]
    lw = float(nodes[0]['LeftTrackWidth']) + mtw / 2
    rw = float(nodes[0]['RightTrackWidth']) + mtw / 2
    g10, g20 = gate(P, 0, prev, lw, rw)
    cur = P[0]
    heading = cur - P[prev]; heading /= np.linalg.norm(heading)
    lat = [(g - cur) / np.linalg.norm(g - cur) for g in (g10, g20)]
    out = []
    for rank in range(8):
        row, col = rank // 3, rank % 3
        lateral_col = row == 2 or col != 1
        side = 0 if col == 0 else 1
        across = 20.0 if row == 2 else 40.0
        at = cur + across * lat[side] if lateral_col else cur.copy()
        out.append(at - row * 100.0 * heading)
    return np.array(out), (g10, g20)


def floor_height(col_items, x, z):
    best = None
    for pos, b in col_items:
        v = b.verts.astype(np.float64) + pos
        f = b.faces['v'].astype(np.int64)
        A, B, C = v[f[:, 0]], v[f[:, 1]], v[f[:, 2]]
        d = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])
        with np.errstate(all='ignore'):
            l0 = ((B[:, 2] - C[:, 2]) * (x - C[:, 0]) + (C[:, 0] - B[:, 0]) * (z - C[:, 2])) / d
            l1 = ((C[:, 2] - A[:, 2]) * (x - C[:, 0]) + (A[:, 0] - C[:, 0]) * (z - C[:, 2])) / d
        l2 = 1 - l0 - l1
        ok = (l0 >= -1e-4) & (l1 >= -1e-4) & (l2 >= -1e-4)
        if ok.any():
            y = (l0 * A[:, 1] + l1 * B[:, 1] + l2 * C[:, 1])[ok].max()
            best = y if best is None else max(best, y)
    return best


def original_ranges():
    rows = {}
    for t in TRACKS:
        r = trk_res3.read_res3(open((__import__('os').environ['RR_CONVERT_STOCK']+'/TRACKS/') + t + '.RES', 'rb').read())
        d = {}
        for kind in 'am':
            nm = [n for n in r.names() if n.lower() == ('tracks' + BS + kind + t + '.spl').lower()][0]
            tr, nodes, P = load_spline(r.get(nm))
            d[kind] = spline_stats(tr, nodes, P)
            d[kind]['bytes'] = len(r.get(nm))
        scb = dict(trk_binparam.decode(r.get('tracks' + BS + t.lower() + '.scb')))
        d['zones'] = int(dict(scb['sceneinfo'])['numzones']); d['portals'] = int(dict(scb['sceneinfo'])['numportals'])
        cols = [n for n in r.names() if n.lower().endswith('.col') and n.lower().startswith('geometry')]
        faces = [len(trk_col.read_col(r.get(n)).members[0][1].faces) for n in cols]
        d['col_files'] = len(cols); d['col_faces_med'] = float(np.median(faces)); d['col_faces_max'] = max(faces)
        pos = trk_ini.parse(r.get([n for n in r.names() if n.lower().endswith('tvcam.pos')][0]))
        d['tv'] = int(pos.get('CameraPositions', 'NumberOfCameras'))
        tob = dict(trk_binparam.decode(r.get('tracks' + BS + t.lower() + '.tob')))
        d['cut'] = int(dict(tob['cscameras'])['numberofcameras'])
        rows[t] = d
    return rows


def xref(files, d, zone_models, ok, bad):
    """Every name referenced exists in the archive; every archive member is referenced (or explained)."""
    import geo_psg, geo_psm
    used = set()
    need = []
    for slt in zone_models:
        b = slt[:-4]
        for ext in ('psg', 'col'):
            if 'geometry' + BS + b + '.' + ext in files: used.add('geometry' + BS + b + '.' + ext)
    for n_ in [n_ for n_ in files if n_.endswith('.psg')]:
        for m in geo_psg.parse(files[n_])['materials']:
            need.append((n_, 'textures' + BS + m + '.psm'))
    for n_ in [n_ for n_ in files if n_.endswith('.psm')]:
        f = geo_psm.fields(geo_psm.parse(files[n_]))
        for k, v in [kv for sec in f.values() for kv in sec.items()]:
            if k.lower().startswith('texturemap'):
                if n_.startswith('particle'):
                    continue
                need.append((n_, 'textures' + BS + v + '.pst'))
    code = [n_ for n_ in files if n_.endswith('.scb')][0].split(BS)[-1][:-4]
    tob = dict(trk_binparam.decode(files['tracks' + BS + code + '.tob']))
    for i in range(1, int(dict(tob['cscameras'])['numberofcameras']) + 1):
        need.append(('tob', 'tracks' + BS + '%s%02d.vub' % (code, i)))
    fixed = ['tracks' + BS + x for x in ('%s.scb' % code, '%s.tob' % code, '%s.aub' % code, '%s.tun' % code,
                                         'a%s.spl' % code, 'm%s.spl' % code, '%sTVCam.pos' % code)]
    fixed += ['audio' + BS + 'introAud.ini', 'audio' + BS + 'scrptLns.ini', 'particle' + BS + 'demoprt.prt',
              'particle' + BS + 'demoprt.pst', 'particle' + BS + 'demoprt.psm']
    need += [('fixed name read by the race loader', x) for x in fixed]
    env = dict(d.get('environment', []))
    if 'cubefile' in env:
        need.append(('scb Environment CubeFile', 'skies' + BS + env['cubefile']))
    missing = [(a, b) for a, b in need if b not in files]
    for a, b in missing: bad('%s references missing %s' % (a, b))
    used |= set(b for a, b in need)
    unref = sorted(n_ for n_ in files if n_ not in used and not n_.endswith(('.scn', '.toy')))
    explained = {n_: 'debug texture (12 of 13 originals carry these unreferenced)' for n_ in unref if 'debug' in n_.lower()}
    for n_ in unref:
        if n_ not in explained: bad('unreferenced member ' + n_)
    ok('cross-reference: %d references, all present; unreferenced members: %s' % (
        len(need) + len(zone_models), ', '.join('%s (%s)' % kv for kv in explained.items()) or 'none'))


def load_visual(files):
    """Decoded visual triangles (world coordinates) per PSG: list of (name, material, P, T, UV)."""
    import geo_psg
    out = []
    for n_, d_ in files.items():
        if not n_.endswith('.psg'):
            continue
        mdl = geo_psg.parse(d_)
        for s_ in geo_psg.vertices(mdl):
            if s_['tris']:
                out.append((n_, s_['material'], np.array(s_['pos']), np.array(s_['tris']), np.array(s_['uv'])))
    return out


def visual_height(vis, p):
    """Signed distance from p down/up to the nearest visual surface vertically below or above (ft)."""
    best = None
    for n_, mat, P, T, UV in vis:
        if '_sky_' in n_:
            continue
        lo = P.min(0); hi = P.max(0)
        if not (lo[0] <= p[0] <= hi[0] and lo[2] <= p[2] <= hi[2]):
            continue
        A, B, C = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
        d = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])
        with np.errstate(all='ignore'):
            l0 = ((B[:, 2] - C[:, 2]) * (p[0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (p[2] - C[:, 2])) / d
            l1 = ((C[:, 2] - A[:, 2]) * (p[0] - C[:, 0]) + (A[:, 0] - C[:, 0]) * (p[2] - C[:, 2])) / d
        l2 = 1 - l0 - l1
        okm = (l0 >= -1e-4) & (l1 >= -1e-4) & (l2 >= -1e-4) & (np.abs(d) > 1e-9)
        if okm.any():
            y = (l0 * A[:, 1] + l1 * B[:, 1] + l2 * C[:, 1])[okm]
            k = np.argmin(np.abs(y - p[1]))
            if best is None or abs(y[k] - p[1]) < abs(best - p[1]):
                best = y[k]
    return best


def render_geometry(code, root, vis, col_items, sp, man):
    """Top view and two chase views: decoded PSG geometry (textured), collision wireframe, AI spline,
    AI gates, cut/TV camera positions. Drawn mirrored back (z negated) for the right-handed renderer."""
    sys.path.insert(0, os.path.join(SCR, 'tools'))
    import softrender as SR
    import geo_pst
    tex_names = sorted(set(m for _, m, _, _, _ in vis))
    textures, tindex = [], {}
    for i, m in enumerate(tex_names):
        pth = os.path.join(root, 'members', 'textures', m + '.pst')
        lv = geo_pst.rgba_levels(geo_pst.parse(open(pth, 'rb').read()))
        textures.append(lv[-1]); tindex[m] = i
    flip = np.array([1, 1, -1.0])
    land = [v for v in vis if '_sky_' not in v[0]]
    P = np.vstack([v[2] for v in land]) * flip
    base = np.cumsum([0] + [len(v[2]) for v in land])[:-1]
    T = np.vstack([v[3] + b for v, b in zip(land, base)])[:, [0, 2, 1]]
    UV = np.vstack([v[4] for v in land])
    TT = np.concatenate([np.full(len(v[3]), tindex[v[1]]) for v in land])
    tr, nodes, SP, st = sp['a']
    vd = os.path.join(root, 'verify')

    def overlay(img, cam, W, H, ortho):
        from PIL import Image, ImageDraw
        im = Image.fromarray(img)
        g = ImageDraw.Draw(im)

        def pr(q):
            S_, okk = SR.project(np.asarray(q, float).reshape(-1, 3) * flip, cam, W, H)
            return [(float(a[0]), float(a[1])) if k else None for a, k in zip(S_, okk)]
        for pos, b, z, fl in col_items:
            if fl and ortho:
                continue   # top view: walls only, the floor triangles would hide the road
            v = b.verts.astype(np.float64) + pos
            col = (255, 0, 255) if not fl else (0, 255, 255)
            for f in b.faces['v']:
                pts = pr(v[list(f) + [f[0]]])
                if all(pts):
                    g.line(pts, fill=col, width=1)
        pts = pr(np.vstack([SP, SP[:1]]) + np.array([0, 1.0, 0]))
        for a, b2 in zip(pts[:-1], pts[1:]):
            if a and b2:
                g.line([a, b2], fill=(255, 235, 0), width=3)
        tw = float(tr['TrackWidth'])
        prev = {int(n['Next0']): i for i, n in enumerate(nodes)}
        for j in range(len(SP)):
            lw = float(nodes[j]['LeftTrackWidth']) + tw / 2; rw = float(nodes[j]['RightTrackWidth']) + tw / 2
            a, b2 = gate(SP, j, prev[j], lw, rw)
            q = pr(np.array([a, b2]) + np.array([0, 1.0, 0]))
            if q[0] and q[1]:
                g.line(q, fill=(255, 140, 0), width=2)
        posf = trk_ini.parse(open(os.path.join(root, 'members', 'tracks', code + 'TVCam.pos'), 'rb').read())
        for nm_, kv in posf.sections():
            if nm_.startswith('Camera') and nm_ != 'CameraPositions':
                q = pr(np.array([float(x) for x in dict(kv)['Position'].split(',')]))[0]
                if q: g.rectangle([q[0] - 4, q[1] - 4, q[0] + 4, q[1] + 4], fill=(80, 140, 255))
        for i in (1, 2, 3):
            fr = trk_vub.read_vub(open(os.path.join(root, 'members', 'tracks', '%s%02d.vub' % (code, i)), 'rb').read())
            q = [pr(np.array(f['pos']))[0] for f in fr[::10]]
            for a, b2 in zip(q[:-1], q[1:]):
                if a and b2: g.line([a, b2], fill=(255, 255, 255), width=2)
        return im
    lo, hi = P.min(0), P.max(0)
    size = max(hi[0] - lo[0], hi[2] - lo[2]) * 1.04
    top = dict(ortho=True, center=(lo + hi) / 2, size=size, dir=(0, -1, 0), up=(0, 0, -1))
    img, _, _ = SR.render(P, T, top, 1400, 1400, uv=UV, tex_of_tri=TT, textures=textures, ambient=0.6)
    overlay(img, top, 1400, 1400, True).save(os.path.join(vd, 'geo_top.png'))
    K = len(SP)
    for name, j in (('geo_chase_start.png', 0), ('geo_chase_mid.png', K // 2)):
        f = SP[(j + 2) % K] - SP[j]; f[1] = 0; f /= np.linalg.norm(f)
        eye = SP[j] - f * 120 + np.array([0, 45.0, 0])
        tgt = SP[j] + f * 400 + np.array([0, 5.0, 0])
        cam = dict(eye=eye * flip, target=tgt * flip, up=(0, 1, 0), fov=70, near=3.0)
        img, _, _ = SR.render(P, T, cam, 960, 540, uv=UV, tex_of_tri=TT, textures=textures, ambient=0.55)
        overlay(img, cam, 960, 540, False).save(os.path.join(vd, name))


def verify(code, root=None):
    root = root or os.path.join(A2R, code)
    man = json.load(open(os.path.join(root, 'manifest.json')))
    res = {'checks': [], 'problems': []}
    ok = res['checks'].append; bad = res['problems'].append
    # the RES archive is the reference: every member is read from it; loose copies must agree
    rr = trk_res3.read_res3(open(os.path.join(root, 'install', 'TRACKS', '%s.RES' % code.upper()), 'rb').read())
    files = dict(rr.items())
    loose = 0
    for pth in glob.glob(os.path.join(root, 'members', '**', '*'), recursive=True):
        if os.path.isfile(pth):
            rel = os.path.relpath(pth, os.path.join(root, 'members')).replace(os.sep, BS)
            loose += 1
            if files.get(rel) != open(pth, 'rb').read(): bad('loose member differs from RES: ' + rel)
    if loose != len(files): bad('loose members %d vs RES members %d' % (loose, len(files)))
    ok('install\\TRACKS\\%s.RES re-read: %d members, equal to the %d loose copies' % (code.upper(), len(files), loose))
    for pth in glob.glob(os.path.join(root, 'source', '**', '*'), recursive=True):
        if os.path.isfile(pth):
            files[os.path.relpath(pth, os.path.join(root, 'source')).replace(os.sep, BS)] = open(pth, 'rb').read()
    outside = {}
    for pth in glob.glob(os.path.join(root, 'install', '**', '*'), recursive=True):
        rel = os.path.relpath(pth, os.path.join(root, 'install')).replace(os.sep, BS)
        if os.path.isfile(pth) and not rel.upper().startswith('TRACKS'):
            outside[rel] = open(pth, 'rb').read()
    for rel, d_ in outside.items():
        if rel.upper().endswith('.PST'):
            import geo_pst
            t = geo_pst.parse(d_)
            if geo_pst.write(t) != d_ or [(l['w'], l['h']) for l in t['levels']] != [(640, 448)]:
                bad('loading screen ' + rel)
    ok('files outside the RES: %s' % ', '.join(sorted(outside)))
    # every file by type
    cols = []
    for n_, d_ in sorted(files.items()):
        e = n_.rsplit('.', 1)[-1].lower()
        if e == 'col':
            c = trk_col.read_col(d_)
            if trk_col.write_col(c) != d_: bad('COL rewrite differs ' + n_)
            st = trk_col.check_body(c.body)
            if st['max_plane_residual'] > 0.05: bad('COL plane residual %s %s' % (n_, st))
            cols.append((n_, c, st))
        elif e in ('scb', 'tob', 'aub'):
            b = trk_binparam.decode(d_)
            if trk_binparam.encode(b) != d_: bad('BINARY rewrite differs ' + n_)
            src = n_[:-4] + ('.scn' if e == 'scb' else '.toy' if e == 'tob' else '')
            if src in files and trk_binparam.compile_ini(trk_ini.parse(files[src]).sections()) != d_:
                bad('compiled text differs from ' + n_)
        elif e == 'vub':
            fr = trk_vub.read_vub(d_)
            if trk_vub.write_vub_raw(fr) != d_: bad('VUB rewrite differs ' + n_)
        elif e == 'pst':
            import geo_pst
            t = geo_pst.parse(d_)
            if geo_pst.write(t) != d_: bad('PST rewrite differs ' + n_)
        elif e == 'tga':
            if d_[2] != 2: bad('TGA type ' + n_)
        elif e == 'cub':
            import trk_sky
            cb = trk_sky.read_cub(d_)
            if len(cb['tiles']) != 48: bad('sky cube tiles ' + n_)
        elif e == 'psg':
            import geo_psg
            if geo_psg.write(geo_psg.parse(d_)) != d_: bad('PSG rewrite differs ' + n_)
        elif e in ('spl', 'pos', 'ini', 'scn', 'toy', 'prt', 'psm', 'tun'):
            ini = trk_ini.parse(d_)
            if trk_ini.write_lossless(ini) != d_: bad('text rewrite differs ' + n_)
    ok('every file re-read by its reader and re-written byte-identical (%d files)' % len(files))
    # splines
    sp = {}
    for kind in 'am':
        nm = 'tracks' + BS + kind + code + '.spl'
        tr, nodes, P = load_spline(files[nm])
        pr = strict_spline_check(files[nm], nodes)
        for p_ in pr: bad(nm + ': ' + p_)
        sp[kind] = (tr, nodes, P, spline_stats(tr, nodes, P))
        sp[kind][3]['bytes'] = len(files[nm])
    ok('splines pass the port reader limits (<=256 nodes, <=64 KB, number syntax, one closed Next0 loop)')
    # scene
    scb = trk_binparam.decode(files['tracks' + BS + code + '.scb'])
    d = dict(scb)
    Z = int(dict(d['sceneinfo'])['numzones'])
    zone_models = []
    col_items = []
    for z in range(1, Z + 1):
        nmod = int(dict(d['zone%d' % z])['numberofstaticmodels'])
        for m in range(1, nmod + 1):
            mm = dict(d['zone%d-model%d' % (z, m)])
            cn = 'geometry' + BS + mm['slt'][:-4] + '.col'
            gn = 'geometry' + BS + mm['slt'][:-4] + '.psg'
            if cn not in files and gn not in files: bad('zone model with neither PSG nor COL ' + mm['slt'])
            if cn in files:
                pos = np.array([float(x) for x in mm['position'].split(',')])
                col_items.append((pos, trk_col.read_col(files[cn]).body, z, 'floor' in cn))
            zone_models.append(mm['slt'])
    ok('scene: %d zones, %d portals, %d placed models (%d with COL)' % (
        Z, int(dict(d['sceneinfo'])['numportals']), len(zone_models), len(col_items)))
    xref(files, d, zone_models, ok, bad)
    vis = load_visual(files)
    # spline nodes on collision floor
    tr, nodes, P, st = sp['a']
    floors = [(p, b) for p, b, z, fl in col_items if fl]
    off = []
    for j in range(len(P)):
        h = floor_height(floors, P[j, 0], P[j, 2])
        off.append(np.nan if h is None else P[j, 1] - h)
    off = np.array(off)
    miss = int(np.isnan(off).sum())
    if miss: bad('%d AI nodes have no collision floor below them' % miss)
    ok('AI nodes on the converted collision floor: %d/%d, height above floor max %.2f ft' % (
        len(P) - miss, len(P), float(np.nanmax(np.abs(off)))))
    # Revenge start grid (port rule) on the floor
    mtr, mnodes, MP, _ = sp['m']
    grid, gates = revenge_grid(MP, float(mtr['TrackWidth']), mnodes)
    gh = [floor_height(floors, g[0], g[2]) for g in grid]
    if any(h is None for h in gh): bad('Revenge grid slot off the collision floor')
    ok('Revenge 8-pod grid (map node 0 + predecessor, ranks 0..7): all slots over collision floor: %s' % all(h is not None for h in gh))
    res['grid'] = [list(np.round(g, 1)) for g in grid]
    res['spline'] = {k: v[3] for k, v in sp.items()}
    res['zones'] = Z; res['portals'] = int(dict(d['sceneinfo'])['numportals'])
    res['col_files'] = len(cols)
    res['col_faces_med'] = float(np.median([s['faces'] for _, _, s in cols]))
    res['col_faces_max'] = max(s['faces'] for _, _, s in cols)
    res['tv'] = int(trk_ini.parse(files['tracks' + BS + code + 'TVCam.pos']).get('CameraPositions', 'NumberOfCameras'))
    res['cut'] = int(dict(dict(trk_binparam.decode(files['tracks' + BS + code + '.tob']))['cscameras'])['numberofcameras'])
    # AI nodes on the decoded visual road surface
    vh = []
    for j in range(len(P)):
        h = visual_height(vis, P[j])
        vh.append(np.nan if h is None else P[j, 1] - h)
    vh = np.array(vh)
    ok('AI nodes over decoded visual geometry: %d/%d; node height minus nearest visual surface: median %.2f ft, '
       '95 %% within %.2f ft' % (np.isfinite(vh).sum(), len(P), float(np.nanmedian(vh)), float(np.nanpercentile(np.abs(vh), 95))))
    if np.isfinite(vh).sum() < len(P): bad('%d AI nodes with no visual geometry below/above' % (len(P) - np.isfinite(vh).sum()))
    res['visual_height'] = dict(median=float(np.nanmedian(vh)), p95=float(np.nanpercentile(np.abs(vh), 95)))
    draw(code, root, col_items, sp, d, grid, gates, man)
    render_geometry(code, root, vis, col_items, sp, man)
    json.dump(res, open(os.path.join(root, 'verify', 'verify.json'), 'w'), indent=1, default=float)
    return res


ZCOL = [(80, 110, 160), (90, 150, 90), (160, 120, 70), (120, 90, 160), (70, 140, 150), (150, 150, 80),
        (160, 90, 110), (100, 100, 100)]


def draw(code, root, col_items, sp, d, grid, gates, man, ):
    os.makedirs(os.path.join(root, 'verify'), exist_ok=True)
    allv = np.concatenate([b.verts.astype(np.float64) + p for p, b, z, fl in col_items])
    lo = allv.min(0); hi = allv.max(0)

    def render(fname, lo2, hi2, W, detail):
        span = max(hi2[0] - lo2[0], hi2[2] - lo2[2])
        s = (W - 40) / span
        H = int((hi2[2] - lo2[2]) * s) + 40
        Wd = int((hi2[0] - lo2[0]) * s) + 40
        im = Image.new('RGB', (Wd, H), (12, 12, 16))
        g = ImageDraw.Draw(im)

        def tp(p):   # x right, z up on the image
            return (20 + (p[0] - lo2[0]) * s, H - 20 - (p[2] - lo2[2]) * s)
        for pos, b, z, fl in col_items:
            v = b.verts.astype(np.float64) + pos
            col = ZCOL[(z - 1) % len(ZCOL)] if fl else (200, 40, 200)
            for f in b.faces['v']:
                g.polygon([tp(v[i]) for i in f], fill=col if fl else None, outline=col if not fl else None)
        for i in range(1, int(dict(d['sceneinfo'])['numportals']) + 1):
            pv = dict(d['portal%d' % i])
            a = [float(x) for x in pv['vertex1'].split(',')]; b2 = [float(x) for x in pv['vertex4'].split(',')]
            g.line([tp(a), tp(b2)], fill=(255, 255, 255), width=3 if detail else 2)
        for kind, colr in (('m', (255, 150, 40)), ('a', (60, 220, 255))):
            tr, nodes, P, st = sp[kind]
            tw = float(tr['TrackWidth'])
            K = len(P)
            prev = {int(n['Next0']): i for i, n in enumerate(nodes)}
            for j in range(K):
                lw = float(nodes[j]['LeftTrackWidth']) + tw / 2; rw = float(nodes[j]['RightTrackWidth']) + tw / 2
                a, b2 = gate(P, j, prev[j], lw, rw)
                g.line([tp(a), tp(b2)], fill=colr, width=1)
                if detail:
                    g.ellipse([tp(a)[0] - 2, tp(a)[1] - 2, tp(a)[0] + 2, tp(a)[1] + 2], fill=(0, 255, 0))
                    g.ellipse([tp(b2)[0] - 2, tp(b2)[1] - 2, tp(b2)[0] + 2, tp(b2)[1] + 2], fill=(255, 0, 0))
        tr, nodes, P, st = sp['a']
        for j in range(len(P)):
            g.line([tp(P[j]), tp(P[int(nodes[j]['Next0'])])], fill=(255, 235, 0), width=2)
        for j, n in enumerate(sp['m'][1]):
            if 'ProgressNode' in n:
                q = tp(sp['m'][2][j]); g.rectangle([q[0] - 5, q[1] - 5, q[0] + 5, q[1] + 5], outline=(255, 255, 255), width=2)
        q = tp(P[0]); g.ellipse([q[0] - 8, q[1] - 8, q[0] + 8, q[1] + 8], outline=(255, 255, 255), width=3)
        q2 = tp(P[1]); g.line([q, q2], fill=(255, 255, 255), width=3)
        for gp in grid:
            q = tp(gp); g.ellipse([q[0] - 4, q[1] - 4, q[0] + 4, q[1] + 4], fill=(0, 255, 0))
        for gp in man['report'].get('arcade_grid_converted', []):
            q = tp(gp); g.line([q[0] - 4, q[1] - 4, q[0] + 4, q[1] + 4], fill=(255, 60, 60), width=2)
            g.line([q[0] - 4, q[1] + 4, q[0] + 4, q[1] - 4], fill=(255, 60, 60), width=2)
        pos = trk_ini.parse(open(os.path.join(root, 'members', 'tracks', code + 'TVCam.pos'), 'rb').read())
        for nm_, kv in pos.sections():
            if nm_.startswith('Camera') and nm_ != 'CameraPositions':
                p = [float(x) for x in dict(kv)['Position'].split(',')]
                q = tp(p); g.rectangle([q[0] - 3, q[1] - 3, q[0] + 3, q[1] + 3], fill=(80, 120, 255))
        g.text((24, 6), '%s  floor by zone, walls magenta, AI spline yellow + AI gates cyan, map gates orange, portals white, '
               'node 0 circle, progress squares, Revenge grid green, arcade grid red x, TV cams blue' % code,
               fill=(255, 255, 255))
        im.save(os.path.join(root, 'verify', fname))
    render('top.png', lo, hi, 2000, False)
    p0 = sp['a'][2][0]
    render('start.png', p0 - np.array([700, 0, 700]), p0 + np.array([700, 0, 700]), 1400, True)


def main():
    codes = sys.argv[1:]
    orig = original_ranges()
    out = {}
    for c in codes:
        out[c] = verify(c)
        print(c, 'problems:', out[c]['problems'] or 'none')
        for x in out[c]['checks']:
            print('   ', x)
    # range table
    lines = ['value ranges: 13 Revenge originals vs converted (feet)', '']
    hdr = '%-5s %5s %5s %5s %5s %6s %6s %6s %6s %6s %6s %5s %5s %5s %6s %6s %4s %4s' % (
        'track', 'aN', 'mN', 'aSp', 'aSpMx', 'aWid', 'aWmin', 'aWmax', 'mWid', 'mWmin', 'mWmax', 'prog', 'zone', 'port',
        'cols', 'fcMed', 'tv', 'cut')
    lines.append(hdr)

    def row(name, r):
        a, m = r['spline']['a'] if 'spline' in r else r['a'], r['spline']['m'] if 'spline' in r else r['m']
        return '%-5s %5d %5d %5.0f %5.0f %6.0f %6.0f %6.0f %6.0f %6.0f %6.0f %5d %5d %5d %6d %6.0f %4d %4d' % (
            name, a['nodes'], m['nodes'], a['spacing_med'], a['spacing_max'], a['width_med'], a['width_min'], a['width_max'],
            m['width_med'], m['width_min'], m['width_max'], m['progress'], r['zones'], r['portals'], r['col_files'],
            r['col_faces_med'], r['tv'], r['cut'])
    for t, r in orig.items():
        lines.append(row(t, r))
    lines.append('')
    for c, r in out.items():
        lines.append(row(c, r))
    lines.append('')
    lines.append('lap length (map D1+seg) originals: ' + ', '.join('%s %.0f' % (t, r['m']['D1']) for t, r in orig.items()))
    lines.append('lap length converted: ' + ', '.join('%s %.0f' % (c, r['spline']['m']['D1']) for c, r in out.items()))
    open(os.path.join(A2R, 'ranges.txt'), 'w').write('\n'.join(lines) + '\n')
    print('\n'.join(lines))


if __name__ == '__main__':
    main()
