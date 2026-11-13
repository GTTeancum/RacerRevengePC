r"""Arcade course -> Revenge geometry (PSG v3) + textures (PST kind 1 + PSM), one code path for
every course.   python -B -I geo_course.py N [N ...]     (N = 0..4, arcade loader argument)

Reads the arcade ROM images through tools\export_course.py (same tables, same display-list decoder
tools\hikaru_mesh.py, same texel decoder tools\mask_tex.py). Writes only
D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\course<N>\:
    geometry\<code>_g<cx>_<cz>_<mat>.psg   landscape, clipped to a 2048-ft grid, one material each
    geometry\<code>_sky_<mat>.psg          sky dome (local object, origin 0)
    textures\<code>_<mat>.pst / .psm       one per arcade texture (+ blend variant)
    manifest.json                          transform, light, per-file info, generated/flagged items

Global conversion (notes\a2r-geometry.md 1):
    Revenge = (S*x + ox, S*y + oy, -S*z + oz), S = 3.0 (pod-size measurement), z negated because
    the arcade frame is right-handed and Revenge left-handed; winding reversed with it.
    ox, oz put the landscape minimum corner at (2000, 2000); oy = 1000 (guess, flagged).
Generated (no arcade source, flagged in the manifest):
    vertex colours: arcade lights at run time; baked here as
        colour = clamp(ambient + diffuse * max(0, N.L) * SUN)  for Gouraud/flat materials,
        colour = diffuse                                         for unlit materials,
    L = normalize(0.4, 1.0, 0.3) (Revenge frame), SUN = 0.7, stored as round(colour * 128) capped
    127, alpha round(vertexAlpha * 63 / 255) (Revenge prelit range: rgb 0..127, opaque alpha 63).
"""
import json
import math
import os
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
sys.path.insert(0, HERE)
sys.path.insert(0, TOOLS)
import export_course as EC  # noqa: E402  (arcade tables, ROM reader, texture decoder)
import hikaru_mesh as HM  # noqa: E402
import geo_encode  # noqa: E402
import geo_pst  # noqa: E402
import geo_psm  # noqa: E402

ROOT = EC.ROOT
SCALE = 3.0          # arcade unit -> Revenge foot (geo_scale_pod.py: chariot 2.92..3.01)
MIRROR_Z = True      # geo_handedness.py / notes: arcade right-handed, Revenge left-handed
MIN_CORNER = 2000.0  # Revenge tracks: all coordinates positive, minimum corner a few thousand ft
BASE_Y = 1000.0      # guess: Revenge ground heights 77..10,900; fog/sky tuned for 1,000-2,000
CELL = 2048.0        # grid cell (ft): half-extent 1,024 ft -> position quantum <= 0.2 ft needs
QUANTUM_MAX = 0.2    # half-extent <= 6,553 ft; the 2048 grid keeps it far below unless y is huge
LIGHT = np.array([0.4, 1.0, 0.3]) / np.linalg.norm([0.4, 1.0, 0.3])
SUN = 0.7
BIAS_FT = 0.1 / 511  # ft per Hikaru depth-bias unit (-511 -> 0.1 ft toward the viewer), generated
CODES = {0: 'xa', 1: 'eb', 2: 'lc', 3: 'xd', 4: 'xk'}  # data: Revenge course code per arcade course
# eb / lc are the planned codes (PORTING-PLAN.md Phase 3); xa / xd / xk are placeholders until
# Phase 5 assigns codes to the Tatooine courses and the cut Kaadu course.


# ---------------------------------------------------------------- arcade scene with per-triangle data
def arcade_triangles(N):
    land, coll, stat = EC.w(0x0C3E4B30 + 12 * N, 3)
    L = EC.w(0x0C3E7FF8 + 52 * land, 13)
    ts = EC.rec('texset', L[0])
    texlist = [EC.w(ts[1] + 12 * k, 3)[0] for k in range(ts[2])] if ts[1] else []
    out = dict(land=[], sky=[])
    objs = [(oid, 'land') for oid in L[1:5] if oid != 0xFFFFFFFF] + [(L[5], 'sky')]
    for oid, tag in objs:
        r, nodes = EC.object_nodes(oid)
        mats = {}
        rm = EC.rec('model', r[2]) if r[2] != 0xFFFFFFFF else None
        if rm and rm[2]:
            mats = HM.decode_materials(EC.rom.read(rm[2], rm[3]))
        for k, nd in enumerate(nodes):
            if nd['model'] == 0xFFFFFFFF:
                continue
            mr = EC.rec('model', nd['model'])
            if not mr or not mr[0]:
                continue
            m = HM.decode(EC.rom.read(mr[0], mr[1]))
            if not m.positions:
                continue
            M = EC.world_matrix(nodes, k)
            P = (np.c_[np.array(m.positions), np.ones(len(m.positions))] @ M)[:, :3]
            Nn = np.array(m.normals) @ M[:3, :3]
            for a, b, c, mi, nocull in m.tris:
                mesh = m.meshes[mi]
                tex = mesh['tex']
                tid = texlist[tex] if tex is not None and tex < len(texlist) else None
                mat = mats.get(mesh['mat'], {})
                uvs = []
                for v in (a, b, c):
                    u, vv = m.uvs[v]
                    if tid is not None:
                        tr = EC.rec('texture', tid)
                        k16 = 16.0 if (tr[5] >> 14) & 1 else 1.0
                        u, vv = u * k16 / (16 << tr[1]), vv * k16 / (16 << tr[2])
                    uvs.append((u, vv))
                rec = dict(P=P[[a, b, c]], N=Nn[[a, b, c]], UV=np.array(uvs),
                           A=np.array([m.alphas[a], m.alphas[b], m.alphas[c]], float),
                           tid=tid, mat=mat, polytype=mesh['polytype'], bias=mesh.get('bias', 0), back=False)
                out[tag].append(rec)
                if nocull:  # both faces: the back face, lit with reversed normals (Odin add_triangle)
                    out[tag].append(dict(rec, P=rec['P'][[0, 2, 1]], N=-rec['N'][[0, 2, 1]], UV=rec['UV'][[0, 2, 1]],
                                         A=rec['A'][[0, 2, 1]], back=True))
    return out, texlist


# ---------------------------------------------------------------- conversion pieces
def transform_fn(land_tris):
    P = np.vstack([t['P'] for t in land_tris])
    zs = -1.0 if MIRROR_Z else 1.0
    q = P * np.array([SCALE, SCALE, SCALE * zs])
    off = np.array([MIN_CORNER - q[:, 0].min(), BASE_Y, MIN_CORNER - q[:, 2].min()])
    return dict(scale=SCALE, z_sign=zs, offset=off.tolist(),
                formula='revenge = (S*x + ox, S*y + oy, %s*S*z + oz)' % ('-' if MIRROR_Z else '+'))


def apply(t, X):
    S, zs, off = X['scale'], X['z_sign'], np.array(X['offset'])
    P = t['P'] * np.array([S, S, S * zs]) + off
    Nn = t['N'] * np.array([1, 1, zs])
    order = [0, 2, 1] if zs < 0 else [0, 1, 2]  # mirror flips orientation: reverse winding
    return P[order], Nn[order], t['UV'][order], t['A'][order]


def bake(Nn, mat, A):
    """Revenge prelit RGBA per vertex (generated lighting)."""
    diff = np.array(mat.get('colour1', mat.get('diffuse', (255, 255, 255))), float) / 255.0
    amb = np.array([min(c, 255) for c in mat.get('ambient', (255, 255, 255))], float) / 255.0
    out = []
    for n, a in zip(Nn, A):
        ln = np.linalg.norm(n)
        n = n / ln if ln > 0 else np.array([0, 1.0, 0])
        if mat.get('shading', 1) in (1, 2):
            c = amb + diff * max(0.0, float(n @ LIGHT)) * SUN
        else:
            c = diff
        rgb = np.minimum(127, np.round(np.clip(c, 0, 1) * 128)).astype(int)
        out.append((int(rgb[0]), int(rgb[1]), int(rgb[2]), int(round(a * 63 / 255))))
    return np.array(out, np.uint8)


def clip_poly(poly, axis, value, keep_below):
    """Sutherland-Hodgman against the plane coordinate[axis] = value. poly: list of attribute
    vectors (position first 3 entries)."""
    out = []
    for i in range(len(poly)):
        a, b = poly[i], poly[(i + 1) % len(poly)]
        ina = (a[axis] <= value) if keep_below else (a[axis] >= value)
        inb = (b[axis] <= value) if keep_below else (b[axis] >= value)
        if ina:
            out.append(a)
        if ina != inb:
            t = (value - a[axis]) / (b[axis] - a[axis])
            out.append(a + (b - a) * t)
    return out


def cells_of(P):
    lo = np.floor(P[:, [0, 2]].min(0) / CELL).astype(int)
    hi = np.floor(P[:, [0, 2]].max(0) / CELL).astype(int)
    return [(cx, cz) for cx in range(lo[0], hi[0] + 1) for cz in range(lo[1], hi[1] + 1)]


def clip_to_cells(P, Nn, UV, C):
    """Yields (cell, P, N, UV, C) triangles of one input triangle clipped to the grid."""
    cs = cells_of(P)
    if len(cs) == 1:
        yield cs[0], P, Nn, UV, C
        return
    V = [np.r_[P[i], Nn[i], UV[i], C[i].astype(float)] for i in range(3)]
    for cx, cz in cs:
        poly = V
        for axis, val, below in ((0, cx * CELL, False), (0, (cx + 1) * CELL, True),
                                 (2, cz * CELL, False), (2, (cz + 1) * CELL, True)):
            poly = clip_poly(poly, axis, val, below)
            if len(poly) < 3:
                break
        if len(poly) < 3:
            continue
        for k in range(1, len(poly) - 1):
            tri = np.array([poly[0], poly[k], poly[k + 1]])
            e = np.cross(tri[1, :3] - tri[0, :3], tri[2, :3] - tri[0, :3])
            if np.linalg.norm(e) < 1e-6:
                continue
            col = np.clip(np.round(tri[:, 8:12]), 0, 255).astype(np.uint8)
            yield (cx, cz), tri[:, :3], tri[:, 3:6], tri[:, 6:8], col


UV_SLAB = 8.0


def split_uv(P, Nn, UV, C):
    """Triangles whose UVs span more than 15 repeats cannot be stored (s16 / 2047). Cut them along
    the lines u = k*8 and v = k*8 (same Sutherland-Hodgman clip as the grid; a shared edge with the
    same UVs is cut at the same points from both sides, so no cracks). Yields triangles."""
    span = UV.max(0) - UV.min(0)
    if span.max() <= 15.0:
        yield P, Nn, UV, C
        return
    V = [np.r_[P[i], Nn[i], UV[i], C[i].astype(float)] for i in range(3)]
    lo = np.floor(UV.min(0) / UV_SLAB).astype(int)
    hi = np.floor(UV.max(0) / UV_SLAB).astype(int)
    for a in (0, 1):
        if span[a] <= 15.0:
            hi[a] = lo[a]  # no cut along this axis: one slab
    for ku in range(lo[0], hi[0] + 1):
        for kv in range(lo[1], hi[1] + 1):
            poly = V
            for axis, val, below in ((6, ku * UV_SLAB, False), (6, (ku + 1) * UV_SLAB, True),
                                     (7, kv * UV_SLAB, False), (7, (kv + 1) * UV_SLAB, True)):
                if span[axis - 6] <= 15.0:
                    continue
                poly = clip_poly(poly, axis, val, below)
                if len(poly) < 3:
                    break
            if len(poly) < 3:
                continue
            for k in range(1, len(poly) - 1):
                tri = np.array([poly[0], poly[k], poly[k + 1]])
                if np.linalg.norm(np.cross(tri[1, :3] - tri[0, :3], tri[2, :3] - tri[0, :3])) < 1e-6:
                    continue
                yield tri[:, :3], tri[:, 3:6], tri[:, 6:8], np.clip(np.round(tri[:, 8:12]), 0, 255).astype(np.uint8)


def shift_uv(UV):
    """Integer shift so the triangle's UVs fit the s16 / 2047 range (|u| <= 16); repeat-wrapped
    textures look the same. Only applied when needed."""
    if np.abs(UV).max() <= 15.9:
        return UV, False
    return UV - np.floor(UV.min(0)), True


def material_key(t):
    blend = t['polytype'] == 6
    return (t['tid'] if t['tid'] is not None else -1, blend)


def material_name(code, key):
    tid, blend = key
    base = '%s_t%s' % (code, EC.idname(tid)) if tid >= 0 else '%s_white' % code
    return (base + '_b' if blend else base).lower()


# ---------------------------------------------------------------- main conversion
def convert(N):
    code = CODES[N]
    out = os.path.join(ROOT, 'export', 'a2r', 'course%d' % N)
    for sub in ('geometry', 'textures'):
        os.makedirs(os.path.join(out, sub), exist_ok=True)
    tris, texlist = arcade_triangles(N)
    X = transform_fn(tris['land'])
    stats = dict(triangles_in=len(tris['land']), back_faces=sum(t['back'] for t in tris['land']),
                 winding_vs_normals_flipped=0, uv_shifted=0, clipped_pieces=0, degenerate_dropped=0)
    buckets = {}
    for tag in ('land', 'sky'):
        for t in tris[tag]:
            P, Nn, UV, A = apply(t, X)
            e = np.cross(P[1] - P[0], P[2] - P[0])
            if np.linalg.norm(e) < 1e-9:
                stats['degenerate_dropped'] += 1
                continue
            if float(e @ Nn.sum(0)) < 0:
                # the visible side is the winding (Odin add_triangle); normals that point the other
                # way are turned round so the lighting and the decoder's normal check agree
                Nn = -Nn
                stats['winding_vs_normals_flipped'] += 1
            if t.get('bias'):
                # Hikaru mesh depth bias (coplanar decal layers, e.g. Etti IV road -511 over a
                # translucent tint +32). Revenge has no per-mesh depth bias: move the triangle along
                # its visible-side normal instead (generated; negative bias = toward the viewer)
                P = P + e / np.linalg.norm(e) * (-t['bias'] * BIAS_FT)
                stats['depth_bias_offset_tris'] = stats.get('depth_bias_offset_tris', 0) + 1
            C = bake(Nn, t['mat'], A)
            key = material_key(t)
            uvp = list(split_uv(P, Nn, UV, C))
            stats['uv_split_pieces'] = stats.get('uv_split_pieces', 0) + len(uvp) - 1
            for P1, N1, UV1, C1 in uvp:
                if tag == 'sky':
                    uv, sh = shift_uv(UV1)
                    stats['uv_shifted'] += sh
                    buckets.setdefault(('sky', key), []).append((P1, N1, uv, C1))
                    continue
                pieces = list(clip_to_cells(P1, N1, UV1, C1))
                stats['clipped_pieces'] += len(pieces) - 1
                for cell, p, n, uv, c in pieces:
                    uv, sh = shift_uv(uv)
                    stats['uv_shifted'] += sh
                    buckets.setdefault((cell, key), []).append((p, n, uv, c))
    # geometry
    files = []
    mats_used = set()
    for (cell, key), lst in sorted(buckets.items(), key=lambda kv: str(kv[0])):
        mname = material_name(code, key)
        mats_used.add(key)
        P = np.vstack([x[0] for x in lst])
        Nn = np.vstack([x[1] for x in lst])
        UV = np.vstack([x[2] for x in lst])
        C = np.vstack([x[3] for x in lst])
        T = np.arange(len(P)).reshape(-1, 3)
        if cell == 'sky':
            name = '%s_sky_%s' % (code, mname[len(code) + 1:])
            blob, info = geo_encode.build_psg(name, mname, P, Nn, UV, C, T, world=False, origin=(0, 0, 0))
            info['note'] = 'sky dome: local object, quantum exempt from the 0.2 ft rule (always far away)'
        else:
            name = '%s_g%02d_%02d_%s' % (code, cell[0], cell[1], mname[len(code) + 1:])
            blob, info = geo_encode.build_psg(name, mname, P, Nn, UV, C, T, world=True)
            if info['quantum_ft'] > QUANTUM_MAX:
                raise ValueError('%s quantum %.3f ft > %.2f' % (name, info['quantum_ft'], QUANTUM_MAX))
        open(os.path.join(out, 'geometry', name + '.psg'), 'wb').write(blob)
        info['file'] = 'geometry\\%s.psg' % name
        files.append(info)
    # textures + materials
    textures = []
    for key in sorted(mats_used):
        tid, blend = key
        mname = material_name(code, key)
        if tid >= 0:
            rgba, ti = EC.texture_rgba(tid)
            if ti['mirror_u'] or ti['mirror_v']:
                raise ValueError('mirrored-repeat texture %s: not used by any course; add a bake step' % ti['id'])
        else:
            rgba, ti = np.full((16, 16, 4), 255, np.uint8), dict(id='white', note='generated for untextured materials')
        pst, pinfo = geo_pst.encode_rgba(rgba)
        extra = []
        if tid >= 0 and not ti['repeat_u']:
            extra.append(('wrapModeHorizontal', '1'))
        if tid >= 0 and not ti['repeat_v']:
            extra.append(('wrapModeVertical', '1'))
        if blend:  # Hikaru translucent poly type 6 -> Revenge alpha blend (crowd.psm settings)
            extra += [('alphaBlendingOnOff', '1'), ('inputColorA', '0'), ('inputColorB', '1'),
                      ('inputAlphaC', '0'), ('inputAlphaD', '1')]
        psm = geo_psm.material(mname, 'PreLitCompressed', extra)
        open(os.path.join(out, 'textures', mname + '.pst'), 'wb').write(pst)
        open(os.path.join(out, 'textures', mname + '.psm'), 'wb').write(psm)
        textures.append(dict(material=mname, arcade_texture=ti.get('id'), arcade_format=ti.get('fmt'),
                             psm_extra=extra, **pinfo))
    q = [f['quantum_ft'] for f in files if 'sky' not in f['name']]
    man = dict(course=N, code=code, name=EC.NAMES[N], transform=X,
               light=dict(direction_revenge=LIGHT.tolist(), sun=SUN, generated=True),
               grid_cell_ft=CELL, stats=stats,
               summary=dict(psg_files=len(files), textures=len(textures), max_quantum_ft=max(q), median_quantum_ft=float(np.median(q)),
                            triangles_out=sum(f['triangles'] for f in files),
                            bytes_geometry=sum(f['bytes'] for f in files)),
               generated=['vertex colours (baked sun + material ambient/diffuse; arcade lights at run time)',
                          'grid split (2048 ft cells) and node names',
                          'uvSize metadata (max edge ratio)', 'texture mip levels (box filter) and 256-colour palettes',
                          'PSM files (PreLitCompressed; alpha blend keys for translucent poly type 6)',
                          'Y offset +1000 ft (guess)',
                          'Hikaru depth bias turned into a small offset along the face normal (0.1 ft per 511)'],
               open=['additive blend modes (material blend 2/3) mapped to plain alpha blend',
                     'arcade fog / light values not found yet (courses.md)'],
               files=files, textures=textures)
    json.dump(man, open(os.path.join(out, 'manifest.json'), 'w'), indent=1)
    print('course %d %s: %d PSG, %d textures, triangles %d -> %d, max quantum %.4f ft, stats %s' % (
        N, code, len(files), len(textures), stats['triangles_in'], man['summary']['triangles_out'], max(q), stats))
    return man


if __name__ == '__main__':
    for a in sys.argv[1:]:
        convert(int(a))
