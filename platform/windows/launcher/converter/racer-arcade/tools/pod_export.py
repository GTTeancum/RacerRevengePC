#!/usr/bin/env python3
"""Racer Arcade pod + pilot exporter: one code path for all 18 kinds.

    python -I tools/pod_export.py [CODE ...]        (default: all 18)

Writes export/pods/<CODE>/:
  textures/t<id>.png                    level-0 texels, RGBA (ABGR1555 / ABGR4444, texel pairs un-swapped)
  <CODE>_pod_tg<g>.gltf/.bin            pod per texture group (colour variant); scenes = LOD parts 0..3
  <CODE>_pod_lod0_tg<g>.obj/.mtl        LOD 0 with the part placement baked in
  <CODE>_pilot[_player].gltf/.bin       pilot: skeleton nodes, meshes, all motions as animations
  <CODE>_pilot[_player]_lod?.obj/.mtl   pilot bind pose
  <CODE>.json                           tables used, placement, motions, cable attach points
and preview images in export/pods/preview/.

Sources (notes/program-map.md, notes/pod-placement.md, notes/pods.md):
  display pod table 0C3F6790 (texgroups, engine count, 4 LOD parts {cockpit, binder, engines[4]}),
  NPC placement table 0C2F904C (body offset, engine offsets + roll; used for all 18 kinds),
  pilot tables 0C2A733C (player slot, flag 0) / 0C2A6C7C (other slots, flag 1),
  motion records: 20-B header {nodes K, frames N, ptr node list K*4, ptr root keys N*36,
  ptr node keys N*K*36}; key = scale xyz (f32), translation xyz (f32), rotation xyz (u32, 65536 = 360 deg),
  rotation (column vectors) = Rz(c) Ry(b) Rx(a); node keys are object-space (not parent-relative).
Mesh decoding: tools/hikaru_mesh.py. Texture index in a model = index into the part's texgroup list.
"""
import json
import math
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hikaru_mesh as HM  # noqa: E402
import racer_assets as R  # noqa: E402
import softrender as SR  # noqa: E402

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
OUT = os.path.join(ROOT, 'export', 'pods')
ROM = HM.Rom(os.path.join(ROOT, 'img'))
NPC_TABLE = 0x0C2F904C
FPS = 60.0  # guess: one motion frame per 60 Hz video frame
CABLES = {  # per-kind cable callbacks (pod table +132), attach points from notes/pod-placement.md
    'ANA': dict(cockpit=[(-0.837, 0.112, -2.098), (0.837, 0.112, -2.098)], engine=[(0, 0.41, 0.25)]),
    'BEN': dict(cockpit=[(-0.78, 0.022, -1.858), (0.78, 0.022, -1.858), (-0.7815, -0.075, -1.858),
                         (0.7815, -0.075, -1.858)], engine=[(-0.6405, 0.6865, 2.3545), (0.6405, 0.6865, 2.3545)]),
    'GAS': dict(cockpit=[(-0.689, 0.113, -0.978), (0.689, 0.113, -0.978)], engine=[(0, 1.2555, 0.705)]),
    'SEB': dict(cockpit=[(-0.645, -0.064, -2.181), (0.645, -0.064, -2.181)], engine=[(0, 1.511, 1.7795)]),
}
NONE = 0xFFFFFFFF


# ------------------------------------------------------------------------------------ textures
_tex_cache = {}


def texture_rgba(tid):
    """Level 0 of texture `tid` as HxWx4 uint8 plus info. ROM holds IDMA-source texels with the two
    texels of each 32-bit word swapped (hikaru-hardware.md 3.7, Odin copy_level; checked by eye on
    font texture 923: unswapped edges zig-zag)."""
    if tid in _tex_cache:
        return _tex_cache[tid]
    t = R.texture(tid)
    w, h = t['w'], t['h']
    raw = np.frombuffer(ROM.read(t['addr'], w * h * 2), '<u2').astype(np.uint32)
    raw = raw.reshape(-1, 2)[:, ::-1].reshape(h, w)
    if t['fmt'] == 1:
        a, b, g, r = (raw >> 12) & 15, (raw >> 8) & 15, (raw >> 4) & 15, raw & 15
        img = (np.stack([r, g, b, a], -1) * 17).astype(np.uint8)
    elif t['fmt'] == 0:
        a, b, g, r = (raw >> 15) & 1, (raw >> 10) & 31, (raw >> 5) & 31, raw & 31
        img = np.stack([r * 255 // 31, g * 255 // 31, b * 255 // 31, a * 255], -1).astype(np.uint8)
    else:
        raise ValueError('texture %s format %d not handled' % (R.idname(tid), t['fmt']))
    h1 = t['h1']
    info = dict(id=R.idname(tid), w=w, h=h, fmt=t['fmt'], repeat_u=(h1 >> 22) & 1, repeat_v=(h1 >> 23) & 1,
                mirror_u=(h1 >> 24) & 1, mirror_v=(h1 >> 25) & 1, uv_texels=(h1 >> 14) & 1,
                texmode=(h1 >> 29) & 7, rom='%08X' % t['addr'], alpha=bool((img[..., 3] < 255).any()),
                semi=bool(((img[..., 3] > 0) & (img[..., 3] < 255)).any()))
    _tex_cache[tid] = (img, info)
    return img, info


# ------------------------------------------------------------------------------------ meshes
def model_mesh(mid):
    m = R.model(mid)
    if m is None or not m['geom']:
        return None
    return HM.decode(ROM.read(m['geom'], m['geom_size']))


def model_materials(mid):
    m = R.model(mid)
    if m is None or not m['attr']:
        return {}
    return HM.decode_materials(ROM.read(m['attr'], m['attr_size']))


def mrow(T=(0, 0, 0), rz_deg=0.0):
    """Row-vector 4x4 (Hikaru convention: p' = [p 1] M, translation in row 3)."""
    c, s = math.cos(math.radians(rz_deg)), math.sin(math.radians(rz_deg))
    M = np.eye(4)
    M[:3, :3] = np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]]).T  # column-vector Rz -> row form
    M[3, :3] = T
    return M


def object_parts(oid, M_place, texgroup, tag):
    """Every model node of object `oid`: list of dict(mesh, node matrix (row), placement, texgroup)."""
    out = []
    if oid is None or oid == NONE:
        return out
    o = R.object_nodes(oid)
    if o is None:
        return out
    for k, n in enumerate(o['nodes']):
        if n['model'] == NONE:
            continue
        mm = model_mesh(n['model'])
        if mm is None or not mm.tris:
            continue
        out.append(dict(name='%s_o%s_n%d_m%s' % (tag, R.idname(oid), k, R.idname(n['model'])), mesh=mm,
                        node=np.array(n['mtx']).reshape(4, 4), place=M_place, texgroup=texgroup,
                        mats=model_materials(n['model']), model=n['model'], obj=oid))
    return out


def npc_placement(kind):
    """NPC table 0C2F904C: 168-B record: +0 engine count, +4 body part, +36+i*32 engine part i;
    part = x, y, z offset, +12 roll degrees (notes/pod-placement.md)."""
    rec = R.u32(NPC_TABLE + kind * 4)
    n = R.i32(rec)

    def part(a):
        x, y, z, k = struct.unpack_from('<4f', R.E, a - R.BASE)
        return dict(offset=(x, y, z), roll_deg=k)
    return dict(record='%08X' % rec, engines=n, body=part(rec + 4),
                engine=[part(rec + 36 + i * 32) for i in range(n)], flags=R.i32(rec + 164))


# ------------------------------------------------------------------------------------ glTF writer
class Gltf:
    def __init__(self):
        self.j = dict(asset=dict(version='2.0', generator='racer-arcade pod_export.py'), scenes=[], nodes=[],
                      meshes=[], materials=[], textures=[], images=[], samplers=[], accessors=[],
                      bufferViews=[], buffers=[], animations=[])
        self.bin = bytearray()
        self.img_index = {}
        self.mat_index = {}
        self.samp_index = {}

    def view(self, data, target=None):
        while len(self.bin) % 4:
            self.bin.append(0)
        v = dict(buffer=0, byteOffset=len(self.bin), byteLength=len(data))
        if target:
            v['target'] = target
        self.bin += data
        self.j['bufferViews'].append(v)
        return len(self.j['bufferViews']) - 1

    def accessor(self, arr, ctype, typ, target=None, minmax=False):
        arr = np.ascontiguousarray(arr)
        a = dict(bufferView=self.view(arr.tobytes(), target), componentType=ctype, count=int(arr.shape[0]), type=typ)
        if minmax:
            a['min'] = [float(x) for x in arr.reshape(arr.shape[0], -1).min(0)]
            a['max'] = [float(x) for x in arr.reshape(arr.shape[0], -1).max(0)]
        self.j['accessors'].append(a)
        return len(self.j['accessors']) - 1

    def material(self, tid, info, double, mat):
        key = (tid, double)
        if key in self.mat_index:
            return self.mat_index[key]
        md = dict(name='t%s%s' % (info['id'] if info else 'none', '_2s' if double else ''),
                  pbrMetallicRoughness=dict(metallicFactor=0.0, roughnessFactor=0.9))
        if mat and 'diffuse' in mat:
            md['pbrMetallicRoughness']['baseColorFactor'] = [c / 255 for c in mat['diffuse']] + [1.0]
        if info is not None:
            if tid not in self.img_index:
                self.j['images'].append(dict(uri='textures/t%s.png' % info['id']))
                wrap = lambda rep, mir: 33648 if mir else (10497 if rep else 33071)  # noqa: E731
                sk = (wrap(info['repeat_u'], info['mirror_u']), wrap(info['repeat_v'], info['mirror_v']))
                if sk not in self.samp_index:
                    self.j['samplers'].append(dict(magFilter=9729, minFilter=9987, wrapS=sk[0], wrapT=sk[1]))
                    self.samp_index[sk] = len(self.j['samplers']) - 1
                self.j['textures'].append(dict(source=len(self.j['images']) - 1, sampler=self.samp_index[sk]))
                self.img_index[tid] = len(self.j['textures']) - 1
            md['pbrMetallicRoughness']['baseColorTexture'] = dict(index=self.img_index[tid])
            if info['semi']:
                md['alphaMode'] = 'BLEND'
            elif info['alpha']:
                md['alphaMode'] = 'MASK'
                md['alphaCutoff'] = 0.5
        if double:
            md['doubleSided'] = True
        self.j['materials'].append(md)
        self.mat_index[key] = len(self.j['materials']) - 1
        return self.mat_index[key]

    def mesh(self, part):
        """glTF mesh from a decoded model; one primitive per (texture, double-sided)."""
        mm, tg = part['mesh'], part['texgroup']
        P = np.array(mm.positions, np.float32)
        N = np.array(mm.normals, np.float32)
        nl = np.linalg.norm(N, axis=1)
        N[nl > 0] /= nl[nl > 0][:, None]
        N[nl == 0] = (0, 1, 0)
        groups = {}
        for a, b, c, mi, fl in mm.tris:
            groups.setdefault((mm.meshes[mi]['tex'], mm.meshes[mi]['mat'], fl & 1), []).append((a, b, c))
        prims = []
        for (ti, matn, dbl), tris in sorted(groups.items(), key=lambda kv: str(kv[0])):
            tid = tg[ti][0] if (ti is not None and ti < len(tg)) else None
            img, info = texture_rgba(tid) if tid is not None else (None, None)
            idx = sorted({i for t in tris for i in t})
            remap = {o: n for n, o in enumerate(idx)}
            p = P[idx]
            uv = np.array([mm.uvs[i] for i in idx], np.float32)
            if info is not None:
                sc = 16.0 if info['uv_texels'] else 1.0
                uv = uv * sc / np.array([info['w'], info['h']], np.float32)
            T = np.array([[remap[a], remap[b], remap[c]] for a, b, c in tris], np.uint32)
            attrs = dict(POSITION=self.accessor(p, 5126, 'VEC3', 34962, True),
                         NORMAL=self.accessor(N[idx], 5126, 'VEC3', 34962))
            if info is not None:
                attrs['TEXCOORD_0'] = self.accessor(uv, 5126, 'VEC2', 34962)
            prims.append(dict(attributes=attrs, indices=self.accessor(T.reshape(-1), 5125, 'SCALAR', 34963),
                              material=self.material(tid, info, dbl, part['mats'].get(matn))))
        self.j['meshes'].append(dict(name=part['name'], primitives=prims))
        return len(self.j['meshes']) - 1

    def node(self, **kw):
        self.j['nodes'].append(kw)
        return len(self.j['nodes']) - 1

    def save(self, path):
        base = os.path.splitext(os.path.basename(path))[0]
        self.j['buffers'] = [dict(byteLength=len(self.bin), uri=base + '.bin')]
        for k in ('animations', 'images', 'textures', 'samplers', 'materials'):
            if not self.j[k]:
                del self.j[k]
        with open(path, 'w') as f:
            json.dump(self.j, f, indent=1)
        with open(os.path.splitext(path)[0] + '.bin', 'wb') as f:
            f.write(self.bin)


def mat_list(M):
    return [float(x) for x in np.asarray(M, float).reshape(-1)]  # row-major Mrow == glTF column-major Mcol


# ------------------------------------------------------------------------------------ OBJ writer
def world_matrix(part):
    return part['node'] @ part['place']


def write_obj_full(parts, path, texdir='textures'):
    """OBJ with per-corner UVs normalised by the face's texture size, v flipped (OBJ v up)."""
    base = os.path.splitext(path)[0]
    used = {}
    lines = ['# racer-arcade pod_export.py; Hikaru coordinates (y up, forward -z)',
             'mtllib %s.mtl' % os.path.basename(base)]
    vo = 1
    vto = 1
    for part in parts:
        mm, tg = part['mesh'], part['texgroup']
        P = np.c_[np.array(mm.positions), np.ones(len(mm.positions))] @ world_matrix(part)
        lines.append('o %s' % part['name'])
        lines += ['v %.5f %.5f %.5f' % tuple(p[:3]) for p in P]
        cur = 'none'
        for a, b, c, mi, fl in mm.tris:
            ti = mm.meshes[mi]['tex']
            tid = tg[ti][0] if (ti is not None and ti < len(tg)) else None
            name = 't%s' % R.idname(tid) if tid is not None else 'untextured'
            if name != cur:
                cur = name
                used[name] = tid
                lines.append('usemtl %s' % name)
            if tid is not None:
                info = texture_rgba(tid)[1]
                sc = 16.0 if info['uv_texels'] else 1.0
                for i in (a, b, c):
                    u, v = mm.uvs[i]
                    lines.append('vt %.6f %.6f' % (u * sc / info['w'], 1 - v * sc / info['h']))
                lines.append('f %d/%d %d/%d %d/%d' % (a + vo, vto, b + vo, vto + 1, c + vo, vto + 2))
                vto += 3
            else:
                lines.append('f %d %d %d' % (a + vo, b + vo, c + vo))
        vo += len(mm.positions)
    with open(path, 'w') as f:
        f.write('\n'.join(lines) + '\n')
    with open(base + '.mtl', 'w') as f:
        for name, tid in used.items():
            f.write('newmtl %s\nKd 1 1 1\n' % name)
            if tid is not None:
                f.write('map_Kd %s/t%s.png\n' % (texdir, R.idname(tid)))
            f.write('\n')


# ------------------------------------------------------------------------------------ motions
def rot_col(a, b, c):
    """Column-vector rotation Rz(c) Ry(b) Rx(a), angles in u16 units (65536 = 360 deg)."""
    a, b, c = (x * 2 * math.pi / 65536 for x in (a, b, c))
    ca, sa, cb, sb, cc, sc = math.cos(a), math.sin(a), math.cos(b), math.sin(b), math.cos(c), math.sin(c)
    Rx = np.array([[1, 0, 0], [0, ca, -sa], [0, sa, ca]])
    Ry = np.array([[cb, 0, sb], [0, 1, 0], [-sb, 0, cb]])
    Rz = np.array([[cc, -sc, 0], [sc, cc, 0], [0, 0, 1]])
    return Rz @ Ry @ Rx


def quat(Rm):
    """Rotation matrix (column vectors) -> glTF quaternion x, y, z, w."""
    m = Rm
    t = m[0, 0] + m[1, 1] + m[2, 2]
    if t > 0:
        s = math.sqrt(t + 1) * 2
        q = ((m[2, 1] - m[1, 2]) / s, (m[0, 2] - m[2, 0]) / s, (m[1, 0] - m[0, 1]) / s, 0.25 * s)
    elif m[0, 0] > m[1, 1] and m[0, 0] > m[2, 2]:
        s = math.sqrt(1 + m[0, 0] - m[1, 1] - m[2, 2]) * 2
        q = (0.25 * s, (m[0, 1] + m[1, 0]) / s, (m[0, 2] + m[2, 0]) / s, (m[2, 1] - m[1, 2]) / s)
    elif m[1, 1] > m[2, 2]:
        s = math.sqrt(1 + m[1, 1] - m[0, 0] - m[2, 2]) * 2
        q = ((m[0, 1] + m[1, 0]) / s, 0.25 * s, (m[1, 2] + m[2, 1]) / s, (m[0, 2] - m[2, 0]) / s)
    else:
        s = math.sqrt(1 + m[2, 2] - m[0, 0] - m[1, 1]) * 2
        q = ((m[0, 2] + m[2, 0]) / s, (m[1, 2] + m[2, 1]) / s, 0.25 * s, (m[1, 0] - m[0, 1]) / s)
    q = np.array(q)
    return q / np.linalg.norm(q)


def decompose_row(M):
    """Row-vector 4x4 -> (T, quaternion, scale) of the column-vector form."""
    M = np.asarray(M, float).reshape(4, 4)
    Mc = M[:3, :3].T
    s = np.linalg.norm(Mc, axis=0)
    s[s == 0] = 1
    Rm = Mc / s
    if np.linalg.det(Rm) < 0:
        s[0] = -s[0]
        Rm[:, 0] = -Rm[:, 0]
    return M[3, :3], quat(Rm), s


def motion_data(mid):
    """Decoded motion: dict(name, nodes, frames, root keys (N,9), node keys (N,K,9)) with rotation u16."""
    m = R.motion(mid)
    K, N = m['F'], m['N']  # racer_assets names: F = node count, N = frame count (see notes/pods.md)
    nodes = list(R.romwords(m['frames'], K))
    def keys(addr, n):
        raw = np.array(R.romwords(addr, n * 9), np.uint32).reshape(n, 9)
        f = raw.view(np.float32).astype(float)
        return np.c_[f[:, :6], raw[:, 6:9].astype(float)]
    return dict(id=R.idname(mid), name=m['name'], nodes=nodes, frames=N,
                root=keys(m['bones'], N), keys=keys(m['keys'], N * K).reshape(N, K, 9),
                hdr='%08X' % m['hdr'], idflags='%08X' % m['idflags'])


def key_row(k):
    """Key (sx sy sz tx ty tz a b c) -> row-vector 4x4."""
    M = np.eye(4)
    M[:3, :3] = (rot_col(*k[6:9]) * np.array(k[0:3])).T  # column form R*diag(S); row form = transpose
    M[3, :3] = k[3:6]
    return M


# ------------------------------------------------------------------------------------ export: pod
def pod_parts(kind, lod, texgroup):
    pd = R.pod(kind)
    pl = npc_placement(kind)
    part = pd['parts'][lod]
    body = mrow(pl['body']['offset'], pl['body']['roll_deg'])
    out = object_parts(part['objA'], body, texgroup, 'cockpit')
    out += object_parts(part['objB'], body, texgroup, 'binder')
    for i in range(pd['engines']):
        e = pl['engine'][i] if i < len(pl['engine']) else dict(offset=(0, 0, 0), roll_deg=0)
        out += object_parts(part['engines'][i], mrow(e['offset'], e['roll_deg']), texgroup, 'engine%d' % i)
    return out


def pilot_parts(kind, flag, place):
    p = R.pilot(kind, flag)
    o = R.object_nodes(p['object'])
    tg = R.texgroup(p['texgroup'])
    mats = model_materials(o['root']) if o['root'] != NONE else {}
    parts = object_parts(p['object'], place, tg, 'pilot')
    for q in parts:
        q['mats'] = mats or q['mats']
    return parts, p, o


def export_pod(kind):
    code = R.CODES[kind]
    d = os.path.join(OUT, code)
    os.makedirs(os.path.join(d, 'textures'), exist_ok=True)
    pd = R.pod(kind)
    pl = npc_placement(kind)
    # colour variant v (DefineDispPod val 0..3): LOD 0 (player slot) uses tg_slot0[v], LODs 1-3 tg_other[v]
    variants = sorted({(pd['tg_slot0'][v], pd['tg_other'][v]) for v in range(4)})
    groups = sorted({g for pair in variants for g in pair})
    meta = dict(kind=kind, code=code, pod_table='%08X' % (R.POD_TABLE + kind * 0x88),
                texgroups_slot0=pd['tg_slot0'], texgroups_other=pd['tg_other'], engines=pd['engines'],
                draw_callback='%08X' % pd['x132'], placement=pl,
                parts=[{k: (R.idname(v) if not isinstance(v, list) else [R.idname(x) for x in v]) for k, v in p.items()}
                       for p in pd['parts']], cables=CABLES.get(code), files=[])
    body = mrow(pl['body']['offset'], pl['body']['roll_deg'])
    for g0, g1 in variants:
        vname = 'tg%d' % g0 if g0 == g1 else 'tg%d-%d' % (g0, g1)
        gl = Gltf()
        for lod in range(4):
            parts = pod_parts(kind, lod, R.texgroup(g0 if lod == 0 else g1))
            root_children = []
            for q in parts:
                mi = gl.mesh(q)
                n_node = gl.node(name=q['name'], mesh=mi, matrix=mat_list(q['node']))
                root_children.append(gl.node(name=q['name'] + '_place', matrix=mat_list(q['place']), children=[n_node]))
            if lod <= 1:  # the pilot is drawn for LOD 0/1 only, in the cockpit frame (pod-placement.md)
                pparts, _, _ = pilot_parts(kind, 0 if lod == 0 else 1, body)
                for q in pparts:
                    mi = gl.mesh(q)
                    n_node = gl.node(name=q['name'], mesh=mi, matrix=mat_list(q['node']))
                    root_children.append(gl.node(name=q['name'] + '_seat', matrix=mat_list(q['place']), children=[n_node]))
            root = gl.node(name='%s_lod%d' % (code, lod), children=root_children)
            gl.j['scenes'].append(dict(name='LOD%d' % lod, nodes=[root]))
        gl.j['scene'] = 0
        fn = '%s_pod_%s.gltf' % (code, vname)
        gl.save(os.path.join(d, fn))
        meta['files'].append(fn)
        parts0 = pod_parts(kind, 0, R.texgroup(g0))
        fn = '%s_pod_lod0_tg%d.obj' % (code, g0)
        write_obj_full(parts0, os.path.join(d, fn))
        meta['files'].append(fn)
    for tid in {t[0] for g in groups for t in R.texgroup(g)}:
        img, info = texture_rgba(tid)
        Image.fromarray(img).save(os.path.join(d, 'textures', 't%s.png' % info['id']))
    return meta


# ------------------------------------------------------------------------------------ export: pilot
def export_pilot(kind, meta):
    code = R.CODES[kind]
    d = os.path.join(OUT, code)
    variants = [(0, '_player'), (1, '')]
    if R.pilot(kind, 0) == R.pilot(kind, 1):
        variants = [(1, '')]
    meta['pilots'] = []
    for flag, suf in variants:
        parts, p, o = pilot_parts(kind, flag, np.eye(4))
        tg = R.texgroup(p['texgroup'])
        for t in tg:
            img, info = texture_rgba(t[0])
            Image.fromarray(img).save(os.path.join(d, 'textures', 't%s.png' % info['id']))
        gl = Gltf()
        # skeleton: one glTF node per object node, flat under the root (motion keys are object-space)
        node_ix = {}
        mesh_of = {q['name'].split('_n')[1].split('_')[0]: q for q in parts}
        skel = []
        for k, n in enumerate(o['nodes']):
            r = n['raw']
            T, Q, S = decompose_row(n['mtx'])
            nd = dict(name='node%02d' % k, translation=[float(x) for x in T], rotation=[float(x) for x in Q],
                      scale=[float(x) for x in S],
                      extras=dict(parent=int(r[1]) if r[1] != NONE else -1, child=int(r[2]) if r[2] != NONE else -1,
                                  sibling=int(r[3]) if r[3] != NONE else -1, model=R.idname(r[0])))
            q = mesh_of.get(str(k))
            if q is not None:
                nd['mesh'] = gl.mesh(q)
            node_ix[k] = gl.node(**nd)
            skel.append(dict(index=k, parent=nd['extras']['parent'], child=nd['extras']['child'],
                             sibling=nd['extras']['sibling'], model=nd['extras']['model'],
                             bind_matrix_row=[float(x) for x in n['mtx']], sphere=list(struct.unpack('<4f', struct.pack('<4I', *r[5:9])))))
        root = gl.node(name='%s_pilot%s' % (code, suf), children=[node_ix[k] for k in sorted(node_ix)])
        gl.j['scenes'] = [dict(name='pilot', nodes=[root])]
        gl.j['scene'] = 0
        mots = []
        for slot, mid in enumerate(p['motions']):
            if mid == NONE:
                continue
            md = motion_data(mid)
            N = md['frames']
            times = gl.accessor(np.arange(N, dtype=np.float32) / FPS, 5126, 'SCALAR', None, True)
            chans, samps = [], []

            def add(node, Ks):
                TT, QQ, SS = [], [], []
                prev = None
                for k in Ks:
                    T, Qn, S = decompose_row(key_row(k))
                    if prev is not None and np.dot(prev, Qn) < 0:
                        Qn = -Qn
                    prev = Qn
                    TT.append(T); QQ.append(Qn); SS.append(S)
                for path, arr, typ in (('translation', TT, 'VEC3'), ('rotation', QQ, 'VEC4'), ('scale', SS, 'VEC3')):
                    samps.append(dict(input=times, output=gl.accessor(np.array(arr, np.float32), 5126, typ), interpolation='LINEAR'))
                    chans.append(dict(sampler=len(samps) - 1, target=dict(node=node, path=path)))
            add(root, md['root'])
            bad = [nd for nd in md['nodes'] if nd not in node_ix]
            for j, nd in enumerate(md['nodes']):
                if nd in node_ix:
                    add(node_ix[nd], md['keys'][:, j])
            gl.j['animations'].append(dict(name=md['name'] or 'motion_%s' % md['id'], channels=chans, samplers=samps))
            mots.append(dict(slot=slot, id=md['id'], name=md['name'], frames=N, nodes=md['nodes'], header=md['hdr'],
                             idflags=md['idflags'], missing_nodes=bad))
        fn = '%s_pilot%s.gltf' % (code, suf)
        gl.save(os.path.join(d, fn))
        write_obj_full(parts, os.path.join(d, '%s_pilot%s_bind.obj' % (code, suf)))
        with open(os.path.join(d, '%s_pilot%s_skeleton.json' % (code, suf)), 'w') as f:
            json.dump(dict(object=R.idname(p['object']), root_model=R.idname(o['root']), nodes=skel), f, indent=1)
        meta['pilots'].append(dict(flag=flag, table='%08X' % R.PILOT_TABLES[flag], texgroup=R.idname(p['texgroup']),
                                   object=R.idname(p['object']), file=fn, motions=mots))


# ------------------------------------------------------------------------------------ previews
def scene_tris(parts, extra_rows=None):
    P, T, UV, TT, texs, tix = [], [], [], [], [], {}
    off = 0
    for q in parts:
        mm, tg = q['mesh'], q['texgroup']
        W = q.get('override', world_matrix(q))
        V = (np.c_[np.array(mm.positions), np.ones(len(mm.positions))] @ W)[:, :3]
        uvs = np.array(mm.uvs, float)
        for a, b, c, mi, fl in mm.tris:
            ti = mm.meshes[mi]['tex']
            tid = tg[ti][0] if (ti is not None and ti < len(tg)) else None
            if tid is not None:
                img, info = texture_rgba(tid)
                if tid not in tix:
                    tix[tid] = len(texs)
                    texs.append(img)
                sc = (16.0 if info['uv_texels'] else 1.0) / np.array([info['w'], info['h']])
                for i in (a, b, c):
                    P.append(V[i]); UV.append(uvs[i] * sc)
                TT.append(tix[tid])
            else:
                for i in (a, b, c):
                    P.append(V[i]); UV.append((0, 0))
                TT.append(-1)
            T.append((off, off + 1, off + 2))
            off += 3
    return np.array(P), np.array(T), np.array(UV), np.array(TT), texs


def render_views(parts, path, title):
    P, T, UV, TT, texs = scene_tris(parts)
    c = (P.max(0) + P.min(0)) / 2
    r = np.abs(P - c).max() * 1.15
    views = [('front', (0, 0, -1)), ('side', (1, 0, 0)), ('3/4', (0.7, 0.45, -0.75)), ('top', (0, 1, 0.001))]
    tiles = []
    for name, dvec in views:
        dvec = np.array(dvec, float); dvec /= np.linalg.norm(dvec)
        cam = dict(ortho=True, center=c, size=2 * r, dir=-dvec, up=(0, 0, -1) if name == 'top' else (0, 1, 0))
        img, _, _ = SR.render(P, T, cam, W=360, H=360, uv=UV, tex_of_tri=TT, textures=texs)
        tiles.append(Image.fromarray(img))
    out = Image.new('RGB', (360 * 4, 380), (20, 20, 20))
    from PIL import ImageDraw
    dr = ImageDraw.Draw(out)
    for i, (t, (name, _)) in enumerate(zip(tiles, views)):
        out.paste(t, (i * 360, 20))
        dr.text((i * 360 + 4, 4), '%s  %s' % (title, name), fill=(255, 255, 0))
    out.save(path)


def preview(kind):
    code = R.CODES[kind]
    pd = R.pod(kind)
    pl = npc_placement(kind)
    body = mrow(pl['body']['offset'], pl['body']['roll_deg'])
    tg = R.texgroup(pd['tg_other'][0])
    pv = os.path.join(OUT, 'preview')
    os.makedirs(pv, exist_ok=True)
    parts = pod_parts(kind, 1, tg)
    pp, p, o = pilot_parts(kind, 1, body)
    render_views(parts + pp, os.path.join(pv, '%s_pod_lod1.png' % code), '%s pod LOD1 tg%d + pilot' % (code, pd['tg_other'][0]))
    cock = [q for q in parts if q['name'].startswith('cockpit')]
    render_views(cock + pp, os.path.join(pv, '%s_cockpit_pilot.png' % code), '%s cockpit LOD1 + pilot' % code)
    tg0 = R.texgroup(pd['tg_slot0'][0])
    render_views(pod_parts(kind, 0, tg0), os.path.join(pv, '%s_pod_lod0.png' % code), '%s pod LOD0 tg%d' % (code, pd['tg_slot0'][0]))
    # pilot: bind pose, and frame 0 of its first motion
    pp, p, o = pilot_parts(kind, 1, np.eye(4))
    render_views(pp, os.path.join(pv, '%s_pilot_bind.png' % code), '%s pilot bind' % code)
    mids = [m for m in p['motions'] if m != NONE]
    if mids:
        md = motion_data(mids[0])
        mid_f = md['frames'] // 2
        posed = []
        for q in pp:
            k = int(q['name'].split('_n')[1].split('_')[0])
            if k in md['nodes']:
                q = dict(q)
                q['override'] = key_row(md['keys'][mid_f, md['nodes'].index(k)]) @ key_row(md['root'][mid_f])
            posed.append(q)
        render_views(posed, os.path.join(pv, '%s_pilot_motion.png' % code),
                     '%s %s f%d' % (code, md['name'], mid_f))


def main():
    codes = sys.argv[1:] or R.CODES
    os.makedirs(OUT, exist_ok=True)
    for code in codes:
        kind = R.CODES.index(code.upper())
        meta = export_pod(kind)
        export_pilot(kind, meta)
        with open(os.path.join(OUT, code, '%s.json' % code), 'w') as f:
            json.dump(meta, f, indent=1, default=str)
        preview(kind)
        print(code, 'done:', ', '.join(meta['files']), '| pilots', [p['file'] for p in meta['pilots']])


if __name__ == '__main__':
    main()
