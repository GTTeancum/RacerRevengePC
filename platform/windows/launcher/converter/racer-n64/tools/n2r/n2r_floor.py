r"""N64 floor table under the racing line -> tracks\<code>.nft (layout confirmed by the port session; notes\n64-hazards-spec.md
sections 7-8). One rule for every track.

Written when the track needs the floor under the flying line: it has a ZeroGravity row (in flight the surface rules and
the hazard gates read the floor under the line, n64-zero-g-physics.md 2.1). Refused (converter error) on a track with a
forked segment until the N64 bake of forked paths is decoded.

N64 rule (decoded, spec 7): per link, t = 0 then += 0.1f (float32) while t < 1; slot = trunc(f32(f32(band + t) * 10));
ray from the spline point itself, (0,0,-1), 5000 ft, facing test off, closest face; none: (0,0,+1); table[slot] = mesh
(NULL kept). At track load: no trigger fired, no animation posed (state 'load' collision model). Lookup: slot =
band * 10 + trunc(t * 10), i.e. the sample taken at the start of the cursor's tenth.
The descent filter at bake time is taken as this track's admitted set (ALL mask with the speed bit, every region); the
exact mask slots at the bake call were not traced [flag].
"""
import numpy as np

import n2r_io
import n2r_n64col
import n2r_space

BS = chr(92)
SAMPLES_PER_LINK = 10
RAY_FT = 5000.0          # 0x800ADC50
f32 = np.float32


def needed(conv):
    return bool(getattr(conv, 'has_zero_g', False))


def _ray(C, keep, o, d):
    F = C.F[keep]; N = C.N[keep]; ids = np.flatnonzero(keep)
    facing = N @ d
    ok = np.abs(facing) > 1e-9
    dist = ((F[:, 0] - o) * N).sum(1) / np.where(ok, facing, 1.0)
    good = ok & (dist >= 0) & (dist <= RAY_FT)
    if not good.any():
        return None
    H = o + dist[:, None] * d
    e0 = np.cross(F[:, 1] - F[:, 0], H - F[:, 0]); e1 = np.cross(F[:, 2] - F[:, 1], H - F[:, 1]); e2 = np.cross(F[:, 0] - F[:, 2], H - F[:, 2])
    inside = good & ((e0 * N).sum(1) >= -1e-6) & ((e1 * N).sum(1) >= -1e-6) & ((e2 * N).sum(1) >= -1e-6)
    if not inside.any():
        return None
    k = np.flatnonzero(inside)[np.argmin(dist[inside])]
    return int(ids[k]), float(dist[k])


def build(conv):
    D = conv.D; spl = D['splines']; pts = spl['points']
    forks = [p['index'] for p in pts if len(p['next']) > 1]
    if forks:
        raise SystemExit('n2r_floor: the N64 floor table is needed (zero-g track) but the spline forks at points %s; the N64 bake of '
                         'forked paths (ForEachSample lookahead / branch bands) is not decoded: refusing to guess' % forks)
    trk = D['track']
    C = n2r_n64col.N64Collision(conv.model, int(trk['planet_track_number']), planet=int(trk['planet_index']),
                                triggers=D['triggers']['triggers'], state='load')
    keep = np.array([C.mesh_track_ok(i) for i in range(len(C.mesh_list))])[C.mesh_of] & ~C.degenerate
    X = conv.X
    # converted map line: main nodes in order with their curve index (polyline 96 samples per N64 link)
    sel, at_m, K = conv.main_sel, conv.at_m, conv.K
    main = spl['graph']['main_path']
    ncurve = len(conv.Cm)
    P = conv.PA
    samples, surf_of, surfaces = [], {}, []
    for a in main:
        b = pts[a]['next'][0]
        t = f32(0)
        while t < f32(1):
            slot = int(np.trunc(f32(f32(f32(a) + t) * f32(SAMPLES_PER_LINK))))
            p = n2r_io.bezier(np.array(pts[a]['position']), np.array(pts[a]['handle_out']), np.array(pts[b]['handle_in']),
                              np.array(pts[b]['position']), [float(t)])[0]
            h = _ray(C, keep, p, np.array([0.0, 0.0, -1.0])); dr = 'D'
            if h is None:
                h = _ray(C, keep, p, np.array([0.0, 0.0, 1.0])); dr = 'U'
            if h is None:
                si, dist, dr = 0, 0.0, 'N'
            else:
                mo = C.mesh_list[C.mesh_of[h[0]]]['mesh']['off']
                if mo not in surf_of:
                    beh = C.mesh_list[C.mesh_of[h[0]]]['behaviour']
                    surfaces.append(dict(mesh=mo, first=slot, objects=sorted(set(conv.col_instances_of_mesh.get(mo, [])))))
                    surf_of[mo] = len(surfaces)
                si, dist = surf_of[mo], h[1] * X['scale']   # N64 units -> ft
            # where the slot starts on the converted line: the converted link around this curve sample, projected
            ci = (at_m[a] + int(round(float(t) * 96))) % ncurve
            j = max([q for q, k in enumerate(sel) if k <= ci], default=len(sel) - 1)
            nj = (j + 1) % K
            q = n2r_space.points(p, X)
            A, B = P[j], P[nj]
            ab = B - A
            u = float(np.clip((q - A) @ ab / max(ab @ ab, 1e-9), 0.0, 1.0))
            samples.append((a, slot - a * 10, j, nj, u, si, dist, dr, slot))
            t = f32(t + f32(0.1))
    # the N64 spline-ambiguous bit is set when a mesh is hit from two traversal orders; on an unforked track there is one order
    for s in surfaces:
        s['ambiguous'] = False
    return samples, surfaces


def text(samples, surfaces):
    out = ['[FloorTable]', 'Version=1', 'SamplesPerLink=%d' % SAMPLES_PER_LINK, 'LineGraph=map',
           'NumSurfaces=%d' % len(surfaces), 'NumSamples=%d' % len(samples), '']
    for i, s in enumerate(surfaces, 1):
        out += ['[Surface%d]' % i, 'Mesh=0x%x' % s['mesh'], 'Objects=%s' % (','.join(s['objects']) if s['objects'] else 'none'),
                'Ambiguous=%s' % ('T' if s['ambiguous'] else 'F'), 'FirstSlot=%d' % s['first'], '']
    out.append('[Samples]')
    for n, (band, tenth, fr, to, u, si, dist, dr, slot) in enumerate(samples):
        out.append('S%d=%d,%d,%d,%d,%.6f,%d,%.1f,%s' % (n, band, tenth, fr, to, u, si, dist, dr))
    return ('\r\n'.join(out) + '\r\n').encode('ascii')


def write(conv):
    if not needed(conv):
        return
    samples, surfaces = build(conv)
    conv.put('tracks' + BS + '%s.nft' % conv.code, text(samples, surfaces),
             'A-derived N64 floor table under the racing line (n2r_floor.py: N64 bake 0x8007F098 replayed, slots placed on the '
             'converted map line); port data, read by the zero-g line table when present')
    nul = sum(1 for s in samples if s[5] == 0)
    noobj = [hex(s['mesh']) for s in surfaces if not s['objects']]
    conv.report['n64_floor_table'] = dict(samples=len(samples), surfaces=len(surfaces), null_slots=nul,
                                          up_rays=sum(1 for s in samples if s[7] == 'U'), surfaces_without_static_object=noobj)
    conv.flags.append('N64 floor table tracks\\%s.nft: %d samples (10 per N64 link, start of each tenth), %d surfaces, %d NULL slots, '
                      'surfaces with no static object %s [A-derived; port data]' % (conv.code, len(samples), len(surfaces), nul, noobj))
