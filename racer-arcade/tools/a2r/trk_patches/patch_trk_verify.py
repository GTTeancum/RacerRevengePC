"""One-off edit of trk_verify.py: install layout, cross-reference check, AI spline on the decoded
visual road, renders over the decoded PSG geometry. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_verify.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


rep("""    files = {}
    for p in glob.glob(os.path.join(root, '**', '*'), recursive=True):
        rel = os.path.relpath(p, root)
        if os.path.isfile(p) and not rel.startswith('verify') and not rel.endswith(('.json', '.RES')):
            files[rel.replace(os.sep, BS)] = open(p, 'rb').read()
    res = {'checks': [], 'problems': []}
    ok = res['checks'].append; bad = res['problems'].append
    # RES archive re-read: same members, same bytes
    rr = trk_res3.read_res3(open(os.path.join(root, '%s_TRACKSIDE.RES' % code.upper()), 'rb').read())
    for n_, d_ in rr.items():
        if files.get(n_) != d_: bad('RES member differs from loose file: ' + n_)
    ok('RES3 re-read: %d members, all equal to the loose files' % len(rr.members))""",
"""    res = {'checks': [], 'problems': []}
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
    ok('install\\\\TRACKS\\\\%s.RES re-read: %d members, equal to the %d loose copies' % (code.upper(), len(files), loose))
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
    ok('files outside the RES: %s' % ', '.join(sorted(outside)))""")
rep("""            if n_.upper().startswith('OUTSIDE_RES' + BS + 'UI') and [(l['w'], l['h']) for l in t['levels']] != [(640, 448)]:
                bad('loading screen size ' + n_)
""", "")
rep("""        elif e in ('spl', 'pos', 'ini', 'scn', 'toy', 'prt', 'psm', 'tun'):""",
    """        elif e == 'psg':
            import geo_psg
            if geo_psg.write(geo_psg.parse(d_)) != d_: bad('PSG rewrite differs ' + n_)
        elif e in ('spl', 'pos', 'ini', 'scn', 'toy', 'prt', 'psm', 'tun'):""")
rep("""            cn = 'geometry' + BS + mm['slt'][:-4] + '.col'
            if cn not in files: bad('zone model without COL ' + mm['slt'])
            else:""", """            cn = 'geometry' + BS + mm['slt'][:-4] + '.col'
            gn = 'geometry' + BS + mm['slt'][:-4] + '.psg'
            if cn not in files and gn not in files: bad('zone model with neither PSG nor COL ' + mm['slt'])
            if cn in files:""")
rep("""    if len(zone_models) != len(cols): bad('COL files %d vs zone models %d' % (len(cols), len(zone_models)))
    ok('scene: %d zones, %d portals, %d collision models, each SLT resolves to its COL' % (
        Z, int(dict(d['sceneinfo'])['numportals']), len(zone_models)))""",
"""    ok('scene: %d zones, %d portals, %d placed models (%d with COL)' % (
        Z, int(dict(d['sceneinfo'])['numportals']), len(zone_models), len(col_items)))
    xref(files, d, zone_models, ok, bad)
    vis = load_visual(files)""")
rep("""    draw(code, root, col_items, sp, d, grid, gates, man)""",
    """    # AI nodes on the decoded visual road surface
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
    render_geometry(code, root, vis, col_items, sp, man)""")

s = s.replace("\ndef verify(code):", '''
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
        f = geo_psm.fields(geo_psm.parse(files[n_])) if hasattr(geo_psm, 'fields') else {}
        for k, v in (f.items() if isinstance(f, dict) else f):
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
        textures.append(lv[-1].astype(float) / 255.0); tindex[m] = i
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
        im = Image.fromarray((np.clip(img, 0, 1) * 255).astype(np.uint8))
        g = ImageDraw.Draw(im)

        def pr(q):
            S_, okk = SR.project(np.asarray(q, float).reshape(-1, 3) * flip, cam, W, H)
            return [(float(a[0]), float(a[1])) if k else None for a, k in zip(S_, okk)]
        for pos, b, z, fl in col_items:
            v = b.verts.astype(np.float64) + pos
            col = (255, 0, 255) if not fl else (0, 255, 255)
            for f in b.faces['v'][:: (1 if not ortho else 1)]:
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


def verify(code):''')
rep("""        pos = trk_ini.parse(open(os.path.join(root, 'tracks', code + 'TVCam.pos'), 'rb').read())""",
    """        pos = trk_ini.parse(open(os.path.join(root, 'members', 'tracks', code + 'TVCam.pos'), 'rb').read())""")
open(p, 'w').write(s)
print('ok')
