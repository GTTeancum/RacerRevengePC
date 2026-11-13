#!/usr/bin/env python3
"""Star Wars Racer Arcade course exporter (one code path for every course).

    python -I export_course.py N [--no-preview]        N = 0..4 (argument of the loader f_0C142DDA)

Writes D:\\Programming\\GitHub\\RacerRevengePC\\work\\scratch\\racer-arcade\\export\\course<N>\\:
  landscape.obj/.mtl   visual landscape (all landscape objects, node transforms applied)
  sky.obj/.mtl         landscape record +20 object (sky dome)
  tex\\t<id>.png       level-0 texels of every texture the course uses (RGBA)
  collision.obj        collision triangles; collision_tris.tsv per-triangle words kept
  lens.obj, stat.obj   the *_LENS / *_STAT quad lists (68-byte records) as quads
  course.json          tables, fog/draw floats, per-mesh material/texture, path table, open fields
  preview\\*.png       top view (textured), chase views, collision-vs-visual overlay

Data path (notes\\program-map.md 5, notes\\courses.md):
  course table 0C3E4B30 [N] = (landscape id, collision id, stat id)
  landscape 0C3E7FF8 + 52*id: +0 texset, +4..+16 objects, +20 sky object, +24 path idx,
            +28/+32 floats, +36 int, +40..+48 callbacks
  object record -> nodes (104 B: +0 model, +4 parent, +8 child, +12 sibling, +16 ?, +20 sphere
            xyz r, +36 ?, +40 4x4 matrix row-major, translation in row 3) ; root model = attribute
            (material) block shared by the object's geometry models
  model record: geometry addr, size, attribute addr, size, n attributes
  texset list A (12 B {texture id, x, y}) position k = texhead index k (f_0C14CD40 commits 10C4|k<<16)
  texture record: addr, log16 w, log16 h, bytes, 0C1 word, 2C1 word
  collision 0C3E4ACC + 12*id: (block, root, size): 196-byte triangle records + 0x20 footer
"""
import json
import math
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hikaru_mesh as HM  # noqa: E402
import mask_tex  # noqa: E402  (shared texel decoder: pair swap)

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
IMG = os.path.join(ROOT, 'img')
BASE = 0x0C00FC00
EP = 0x40000000
TABLES = {  # (mask table, count, eprom table, count, record bytes) -- notes\program-map.md 4
    'model': (0x0C17D750, 5240, 0x0C244FC0, 745, 20),
    'object': (0x0C1970B0, 1529, 0x0C2489F4, 130, 16),
    'texture': (0x0C169738, 2143, 0x0C2432F8, 190, 24),
    'texset': (0x0C17CE64, 16, 0x0C244EE0, 2, 20),
}
NAMES = {0: 'SA_COURSE2 (Tatooine Bantha Tracks)', 1: 'ETTI4 (Etti IV Smuggler\'s Cove)',
         2: 'MALASTARE (Malastare Pixelito Challenge)', 3: 'SA_COURSE4 (Tatooine Boonta Classic)',
         4: 'course 4 (Kaadu scene, guess)'}
STAT_PTR = {0: 0x0C3C2B64, 1: 0x0C3E4AA8, 3: 0x0C3E0924}
LENS_TAB = 0x0C2F7A90
PATH_TAB = 0x0C1695E8

rom = HM.Rom(IMG)
E = rom.eprom


def w(a, n):
    return struct.unpack_from('<%dI' % n, E, a - BASE)


def fl(a, n):
    return struct.unpack_from('<%df' % n, E, a - BASE)


def cstr(a):
    o = a - BASE
    if not 0 <= o < len(E):
        return None
    return E[o:E.find(b'\0', o)].decode('latin1')


def rec(kind, ident):
    mt, mn, et, en, sz = TABLES[kind]
    if ident == 0xFFFFFFFF:
        return None
    if ident & EP:
        i = ident & ~EP
        return w(et + i * sz, sz // 4) if i < en else None
    return w(mt + ident * sz, sz // 4) if ident < mn else None


def idname(i):
    return 'E%d' % (i & ~EP) if i & EP else '%d' % i


def romloc(v):
    if v >> 28 == 0xA:
        return 'mask:%08X' % (v & 0x0FFFFFFF)
    if v >> 24 in (0x90, 0x91):
        return 'eprom:%08X' % (((v >> 24) - 0x90) * 0x800000 + (v & 0xFFFFFF))
    return '%08X' % v


# ------------------------------------------------------------------ textures
SWAP_TEXEL_PAIRS = True  # 3D textures hold the two texels of each 32-bit word swapped (mask-map.md)


def texture_rgba(tid, swap=None):
    """Level 0 of a texture as HxWx4 uint8. ABGR1555: A15 B14:10 G9:5 R4:0; ABGR4444: A B G R
    nibbles high to low (notes\\hikaru-hardware.md 3.7, verified by eye in notes\\mask-map.md).
    Texel pairs are un-swapped with mask_tex.texel_words (the shared decoder)."""
    r = rec('texture', tid)
    W, H = 16 << r[1], 16 << r[2]
    fmt = (r[5] >> 26) & 7
    raw = mask_tex.texel_words(rom.read(r[0], W * H * 2), W, SWAP_TEXEL_PAIRS if swap is None else swap)
    if fmt == 1:
        a, b, g, rr = (raw >> 12) & 15, (raw >> 8) & 15, (raw >> 4) & 15, raw & 15
        out = np.stack([rr, g, b, a], -1) * 17
    else:
        a, b, g, rr = (raw >> 15) & 1, (raw >> 10) & 31, (raw >> 5) & 31, raw & 31
        out = np.stack([rr * 255 // 31, g * 255 // 31, b * 255 // 31, a * 255], -1)
    return out.astype(np.uint8), dict(id=idname(tid), addr=romloc(r[0]), w=W, h=H, fmt=fmt,
                                      w0C1='%08X' % r[4], w2C1='%08X' % r[5],
                                      repeat_u=(r[5] >> 22) & 1, repeat_v=(r[5] >> 23) & 1,
                                      mirror_u=(r[5] >> 24) & 1, mirror_v=(r[5] >> 25) & 1,
                                      uv_texels=(r[5] >> 14) & 1, texmode=(r[5] >> 29) & 7)


# ------------------------------------------------------------------ objects -> meshes
def object_nodes(oid):
    r = rec('object', oid)
    nodes = []
    for k in range(r[1]):
        a = r[0] + 104 * k
        h = w(a, 10)
        nodes.append(dict(model=h[0], parent=h[1], child=h[2], sibling=h[3], w16=h[4],
                          sphere=fl(a + 20, 4), w36=h[9], matrix=fl(a + 40, 16)))
    return r, nodes


def world_matrix(nodes, k):
    m = np.eye(4)
    while k != 0xFFFFFFFF and k < len(nodes):
        m = m @ np.array(nodes[k]['matrix']).reshape(4, 4)
        k = nodes[k]['parent']
    return m


class Scene:
    def __init__(self):
        self.P, self.N, self.UV, self.T, self.groups = [], [], [], [], []
        self.mesh_info = []
        self.ops = {}
        self.unknown = []

    def add_object(self, oid, texlist, tag):
        r, nodes = object_nodes(oid)
        root = r[2]
        mats = {}
        rm = rec('model', root) if root != 0xFFFFFFFF else None
        if rm and rm[2]:
            mats = HM.decode_materials(rom.read(rm[2], rm[3]))
        info = dict(object=idname(oid), nodes=len(nodes), root_model=idname(root) if root != 0xFFFFFFFF else None,
                    flags=r[3], materials=len(mats), models=0)
        for k, nd in enumerate(nodes):
            if nd['model'] == 0xFFFFFFFF:
                continue
            mr = rec('model', nd['model'])
            if not mr or not mr[0]:
                continue
            m = HM.decode(rom.read(mr[0], mr[1]))
            for op, c in m.ops.items():
                self.ops['%03X' % op] = self.ops.get('%03X' % op, 0) + c
            if m.unknown:
                self.unknown.append((idname(nd['model']), m.unknown[:2]))
            if m.calls:
                self.unknown.append((idname(nd['model']), 'calls/LOD/matrix ops %d' % len(m.calls)))
            M = world_matrix(nodes, k)
            base = len(self.P)
            pos = np.array(m.positions) if m.positions else np.zeros((0, 3))
            if len(pos):
                pos = np.c_[pos, np.ones(len(pos))] @ M
                self.P += [tuple(p[:3]) for p in pos]
                nrm = np.array(m.normals) @ M[:3, :3]
                self.N += [tuple(n) for n in nrm]
            for mi, mesh in enumerate(m.meshes):
                tex = mesh['tex']
                tid = texlist[tex] if tex is not None and tex < len(texlist) else None
                self.mesh_info.append(dict(model=idname(nd['model']), node=k, mesh=mi, mat=mesh['mat'],
                                           texhead=tex, texture=idname(tid) if tid is not None else None,
                                           polytype=mesh['polytype'], pattern=mesh['pattern'],
                                           bias=mesh['bias'], tag=tag))
            # uv normalisation per mesh texture (Odin copy_texcoords)
            uvs = list(m.uvs)
            vert_mesh = {}
            for a, b, c, mi, fl_ in m.tris:
                for v in (a, b, c):
                    vert_mesh[v] = mi
            for v, mi in vert_mesh.items():
                tex = m.meshes[mi]['tex']
                tid = texlist[tex] if tex is not None and tex < len(texlist) else None
                if tid is not None:
                    tr = rec('texture', tid)
                    k16 = 16.0 if (tr[5] >> 14) & 1 else 1.0
                    uvs[v] = (uvs[v][0] * k16 / (16 << tr[1]), uvs[v][1] * k16 / (16 << tr[2]))
            self.UV += uvs
            for a, b, c, mi, fl_ in m.tris:
                mesh = m.meshes[mi]
                tex = mesh['tex']
                tid = texlist[tex] if tex is not None and tex < len(texlist) else None
                mat = mats.get(mesh['mat'], {})
                key = (tag, mesh['mat'], tid if tid is not None else -1, mesh['polytype'])
                self.T.append((base + a, base + b, base + c))
                self.groups.append(key)
                if fl_:
                    self.T.append((base + a, base + c, base + b))
                    self.groups.append(key)
            info['models'] += 1
        return info, mats


def write_obj(path, scene, mtlname, sel=None):
    P, UV, N = scene.P, scene.UV, scene.N
    order = sorted(range(len(scene.T)), key=lambda i: scene.groups[i])
    if sel is not None:
        order = [i for i in order if sel(scene.groups[i])]
    used = sorted({v for i in order for v in scene.T[i]})
    remap = {v: j + 1 for j, v in enumerate(used)}
    with open(path, 'w') as f:
        f.write('# Star Wars Racer Arcade course export (tools\\export_course.py); native Hikaru units, y up\n')
        f.write('mtllib %s\n' % mtlname)
        for v in used:
            f.write('v %.4f %.4f %.4f\n' % P[v])
        for v in used:
            f.write('vt %.5f %.5f\n' % (UV[v][0], 1.0 - UV[v][1]))
        for v in used:
            f.write('vn %.4f %.4f %.4f\n' % N[v])
        cur = None
        for i in order:
            g = scene.groups[i]
            if g != cur:
                cur = g
                f.write('usemtl %s\n' % mtl_name(g))
            a, b, c = (remap[x] for x in scene.T[i])
            f.write('f %d/%d/%d %d/%d/%d %d/%d/%d\n' % (a, a, a, b, b, b, c, c, c))
    return len(order), len(used)


def mtl_name(g):
    tag, mat, tid, pt = g
    return '%s_m%s_t%s_p%d' % (tag, mat, idname(tid) if tid >= 0 else 'none', pt)


def write_mtl(path, scene, mats_by_tag, alpha_tex=()):
    with open(path, 'w') as f:
        for g in sorted(set(scene.groups)):
            tag, mat, tid, pt = g
            mt = mats_by_tag.get(tag, {}).get(mat, {})
            d = mt.get('colour1', (255, 255, 255))
            f.write('newmtl %s\n' % mtl_name(g))
            f.write('# hikaru material %s: %s polytype %d\n' % (mat, json.dumps(mt), pt))
            f.write('Kd %.3f %.3f %.3f\n' % tuple(c / 255.0 for c in d))
            amb = mt.get('ambient', (0, 0, 0))
            f.write('Ka %.3f %.3f %.3f\n' % tuple(min(c, 255) / 255.0 for c in amb))
            if tid >= 0:
                f.write('map_Kd tex/t%s.png\n' % idname(tid))
                if tid in alpha_tex:  # texture has transparent texels (1555 A=0, 4444 A<15)
                    f.write('map_d tex/t%s.png\n' % idname(tid))
            f.write('\n')


# ------------------------------------------------------------------ collision
def collision(cid):
    blk, root, size = w(0x0C3E4ACC + 12 * cid, 3)
    raw = rom.read(blk, size)
    n = (size - 0x20) // 196
    R = np.frombuffer(raw[:n * 196], '<u4').reshape(n, 49)
    # the triangle array ends where the spatial index lists start: the first "record" whose
    # radius word (3) is a small integer (verified: adjacency 44-46 and w47 stay < count, w48>>16 =
    # own index, counts 8000/6184/7352/16000/1520)
    n = int(np.argmax(R[:, 3] < 0x01000000))
    R = R[:n]
    F = R.view('<f4')
    rootw = rom.words(root, 4)  # {grid/cell count?, ptr to spatial index, block base, 0}
    return dict(block=romloc(blk), root=romloc(root), size=size, n=n,
                root_words=['%08X' % x for x in rootw]), R, F


def ground_heights(V, surf, xz):
    """Height of the highest non-wall collision triangle under each (x, z) point (nan if none)."""
    keep = surf != 0x8000
    A, B, C = V[keep, 0], V[keep, 1], V[keep, 2]
    out = np.full(len(xz), np.nan)
    for i, (x, z) in enumerate(xz):
        d = (B[:, 2] - C[:, 2]) * (A[:, 0] - C[:, 0]) + (C[:, 0] - B[:, 0]) * (A[:, 2] - C[:, 2])
        with np.errstate(all='ignore'):
            l0 = ((B[:, 2] - C[:, 2]) * (x - C[:, 0]) + (C[:, 0] - B[:, 0]) * (z - C[:, 2])) / d
            l1 = ((C[:, 2] - A[:, 2]) * (x - C[:, 0]) + (A[:, 0] - C[:, 0]) * (z - C[:, 2])) / d
        l2 = 1 - l0 - l1
        ins = (l0 >= -1e-4) & (l1 >= -1e-4) & (l2 >= -1e-4) & (d != 0)
        if ins.any():
            y = l0[ins] * A[ins, 1] + l1[ins] * B[ins, 1] + l2[ins] * C[ins, 1]
            out[i] = y.max()
    return out


def write_collision(out, R, F):
    n = len(R)
    V = F[:, 4:13].reshape(n, 3, 3)
    with open(os.path.join(out, 'collision.obj'), 'w') as f:
        f.write('# collision triangles (196-byte records, words 4..12 = v0 v1 v2); see collision_tris.tsv\n')
        surf = R[:, 48] & 0xFFFF
        for t in range(n):
            for k in range(3):
                f.write('v %.4f %.4f %.4f\n' % tuple(V[t, k]))
        for s in sorted(set(surf.tolist())):
            f.write('g surface_%X\n' % s)
            for t in np.flatnonzero(surf == s):
                f.write('f %d %d %d\n' % (3 * t + 1, 3 * t + 2, 3 * t + 3))
    with open(os.path.join(out, 'collision_tris.tsv'), 'w') as f:
        f.write('tri\tcentre_x\tcentre_y\tcentre_z\tradius\tadj0\tadj1\tadj2\tw47\tw48\tsurface(w48&FFFF)\tw48>>16'
                '\tface_n(w22-24)\traw_w13_43_hex\n')
        for t in range(n):
            f.write('%d\t%.3f\t%.3f\t%.3f\t%.3f\t%d\t%d\t%d\t%d\t%08X\t%X\t%X\t%.4f,%.4f,%.4f\t%s\n' % (
                t, *F[t, 0:4], *(np.int32(R[t, 44:47])), np.int32(R[t, 47]), R[t, 48], R[t, 48] & 0xFFFF,
                R[t, 48] >> 16, *F[t, 22:25], ' '.join('%08X' % x for x in R[t, 13:44])))
    return V


# ------------------------------------------------------------------ quad lists (STAT / LENS)
def quads(ptr):
    name = cstr(w(ptr, 1)[0])
    hdr = w(ptr, 8)
    cnt, arr = hdr[1], hdr[2]
    Q = np.frombuffer(E, '<f4', count=cnt * 17, offset=arr - BASE).reshape(cnt, 17)
    Wd = np.frombuffer(E, '<u4', count=cnt * 17, offset=arr - BASE).reshape(cnt, 17)
    return dict(name=name, ptr='%08X' % ptr, count=cnt, array='%08X' % arr,
                header_floats=[round(x, 3) for x in fl(ptr + 12, 4)],
                header_words=['%08X' % x for x in hdr[3:8]]), Q, Wd


def write_quads(path, items):
    with open(path, 'w') as f:
        f.write('# 68-byte records: plane normal xyz, d, 4 corners xyz, 1 word (kept as comment)\n')
        nv = 0
        for info, Q, Wd in items:
            f.write('g %s\n' % info['name'])
            for i in range(len(Q)):
                for k in range(4):
                    f.write('v %.4f %.4f %.4f\n' % tuple(Q[i, 4 + 3 * k:7 + 3 * k]))
                f.write('# w16=%08X\nf %d %d %d %d\n' % (Wd[i, 16], nv + 1, nv + 2, nv + 3, nv + 4))
                nv += 4


# ------------------------------------------------------------------ main
def export(N, preview=True):
    out = os.path.join(ROOT, 'export', 'course%d' % N)
    os.makedirs(os.path.join(out, 'tex'), exist_ok=True)
    os.makedirs(os.path.join(out, 'preview'), exist_ok=True)
    land, coll, stat = w(0x0C3E4B30 + 12 * N, 3)
    L = w(0x0C3E7FF8 + 52 * land, 13)
    Lf = fl(0x0C3E7FF8 + 52 * land, 13)
    ts = rec('texset', L[0])
    texlist = [w(ts[1] + 12 * k, 3) for k in range(ts[2])] if ts[1] else []
    regions = [w(ts[3] + 16 * k, 4) for k in range(ts[4])] if ts[3] else []
    tex_ids = [t[0] for t in texlist]
    J = dict(course=N, name=NAMES[N], landscape=land, collision_id=coll, stat_id=stat,
             landscape_record=dict(texset=idname(L[0]), objects=[idname(x) if x != 0xFFFFFFFF else None for x in L[1:5]],
                                   sky_object=idname(L[5]), path_index=L[6],
                                   floats_28_32=[Lf[7], Lf[8]], int_36=L[9],
                                   callbacks=['%08X' % x for x in L[10:13]]),
             texset=dict(flag=ts[0], listA=[dict(texhead=k, texture=idname(t[0]), x=t[1], y=t[2]) for k, t in enumerate(texlist)],
                         listB_regions=[list(r) for r in regions]))
    scene = Scene()
    mats_by_tag = {}
    objinfo = []
    for k, oid in enumerate(L[1:5]):
        if oid == 0xFFFFFFFF:
            continue
        info, mats = scene.add_object(oid, tex_ids, 'L%d' % k)
        mats_by_tag['L%d' % k] = mats
        objinfo.append(info)
    sky_info, sky_mats = scene.add_object(L[5], tex_ids, 'SKY')
    mats_by_tag['SKY'] = sky_mats
    objinfo.append(sky_info)
    J['objects'] = objinfo
    J['materials'] = {tag: {str(k): v for k, v in m.items()} for tag, m in mats_by_tag.items()}
    J['cp_opcode_counts'] = scene.ops
    J['decode_notes'] = [str(u) for u in scene.unknown[:50]]
    J['meshes'] = scene.mesh_info
    used_tex = sorted({g[2] for g in scene.groups if g[2] >= 0})
    texinfo = {}
    from PIL import Image
    texcache = {}
    for t in sorted(set(used_tex) | set(tex_ids)):
        rgba, ti = texture_rgba(t)
        Image.fromarray(rgba).save(os.path.join(out, 'tex', 't%s.png' % idname(t)))
        texinfo[idname(t)] = ti
        texcache[t] = rgba
    J['textures'] = texinfo
    missing = sorted({m['texhead'] for m in scene.mesh_info if m['texhead'] is not None and m['texhead'] >= len(tex_ids)})
    J['texheads_beyond_listA'] = missing
    nl, vl = write_obj(os.path.join(out, 'landscape.obj'), scene, 'course.mtl', sel=lambda g: g[0] != 'SKY')
    ns, vs = write_obj(os.path.join(out, 'sky.obj'), scene, 'course.mtl', sel=lambda g: g[0] == 'SKY')
    alpha_tex = {t for t, im in texcache.items() if (im[..., 3] < 255).any()}
    for t in alpha_tex:
        texinfo[idname(t)]['has_alpha'] = True
    write_mtl(os.path.join(out, 'course.mtl'), scene, mats_by_tag, alpha_tex)
    J['counts'] = dict(landscape_tris=nl, landscape_verts=vl, sky_tris=ns, sky_verts=vs)
    cinfo, R, F = collision(coll)
    V = write_collision(out, R, F)
    cinfo['surfaces'] = {('%X' % k): int(v) for k, v in zip(*np.unique(R[:, 48] & 0xFFFF, return_counts=True))}
    J['collision'] = cinfo
    # STAT / LENS quad lists
    stat_items = [quads(STAT_PTR[stat])] if stat in STAT_PTR else []
    lens_items = [quads(p) for p in w(LENS_TAB + 16 * N, 4) if p and p != 0xFFFFFFFF]
    if stat_items:
        write_quads(os.path.join(out, 'stat.obj'), stat_items)
    if lens_items:
        write_quads(os.path.join(out, 'lens.obj'), lens_items)
    J['stat'] = [i[0] for i in stat_items]
    J['lens'] = [i[0] for i in lens_items]
    # path table (landscape +24): {RAM ptr, count} of float pairs
    pp, pc = w(PATH_TAB + 8 * L[6], 2)
    pairs = np.frombuffer(E, '<f4', count=pc * 2, offset=pp - BASE).reshape(pc, 2)
    J['path_table'] = dict(index=L[6], ptr='%08X' % pp, count=pc,
                           first=[[round(float(a), 3), round(float(b), 3)] for a, b in pairs[:8]],
                           last=[[round(float(a), 3), round(float(b), 3)] for a, b in pairs[-4:]])
    # The pairs are the track centre line in (x, z), closed loop, first point (0, 0) (its extent
    # matches the collision x/z extent of each course; steps are even: 20-34 units, 8 for course 4).
    # y is taken from the collision ground under each point (not stored in the table).
    gy = ground_heights(V, (R[:, 48] & 0xFFFF), pairs.astype(float))
    np.savetxt(os.path.join(out, 'path_table.tsv'), np.c_[pairs, gy], fmt='%.5f', delimiter='\t',
               header='landscape +24 table %d @%08X: %d (x, z) pairs = track centre line (closed); '
                      'col 3 = collision ground y under the point (derived)' % (L[6], pp, pc))
    with open(os.path.join(out, 'path.obj'), 'w') as f:
        f.write('# track centre line (landscape +24 table), y from collision ground\n')
        for (x, z), y in zip(pairs, gy):
            f.write('v %.4f %.4f %.4f\n' % (x, 0.0 if np.isnan(y) else y, z))
        f.write('l %s 1\n' % ' '.join(str(i + 1) for i in range(len(pairs))))
    J['path_table']['length'] = float(np.linalg.norm(np.diff(np.r_[pairs, pairs[:1]], axis=0), axis=1).sum())
    J['path_table']['points_without_ground'] = int(np.isnan(gy).sum())
    Pv = np.array(scene.P)
    land_idx = [i for i, g in enumerate(scene.groups) if g[0] != 'SKY']
    if land_idx:
        lv = np.array(scene.T)[land_idx].ravel()
        J['landscape_bbox'] = [Pv[lv].min(0).round(2).tolist(), Pv[lv].max(0).round(2).tolist()]
    J['collision_bbox'] = [V.reshape(-1, 3).min(0).round(2).tolist(), V.reshape(-1, 3).max(0).round(2).tolist()]
    with open(os.path.join(out, 'course.json'), 'w') as f:
        json.dump(J, f, indent=1)
    print('course %d %s: landscape %d tris, sky %d tris, %d textures, collision %d tris, unknown %d, texheads beyond list A %s'
          % (N, NAMES[N], nl, ns, len(texinfo), cinfo['n'], len(scene.unknown), missing))
    if preview:
        import course_preview
        course_preview.previews(out, scene, texcache, V, R, stat_items, lens_items,
                                path=np.c_[pairs[:, 0], gy, pairs[:, 1]])
    return J


if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    for a in args:
        export(int(a), preview='--no-preview' not in sys.argv)
