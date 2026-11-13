r"""Extra static objects that the arcade's per-course hook +44 loads and places
(notes\race-hooks.txt, race-data.md 15): courses 0 and 3. Converted to PSG/PST/PSM with the
geometry helper's own pipeline (geo_course helpers, geo_encode, geo_pst, geo_psm: used, not changed)
and placed as world pieces like the landscape.

Placement call f_0C146F3A(ctx, obj, x, y, z, a): the 4th value is a uniform SCALE, not a yaw:
at 0C146FCC the bounding-sphere radius (node +32) is multiplied by it (fmul fr15, fr4), and the
objects that get 45..180 are unit-sized (1229: 1 x 1 x 0.8 units) while the world-sized ones
(1240, 1251, 1254, E3-E5) get 1.0-1.5. No rotation is applied. So arcade point = obj point * a
+ (x, y, z) (A-derived from the code; the order scale-then-translate is the usual matrix push,
0C146F9E translate then the scaled draw).
"""
import os
import re
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import a2r_space  # noqa: E402
import geo_course as GC  # noqa: E402
import geo_encode  # noqa: E402
import geo_pst  # noqa: E402
import geo_psm  # noqa: E402

EC, HM = GC.EC, GC.HM
HOOK_OF_COURSE = {0: 'f_0C147078', 3: 'f_0C147430'}   # landscape +44 per course (race-hooks.txt header)


def parse_placements():
    txt = open(os.path.join(SCR, 'notes', 'race-hooks.txt')).read()
    out = {}
    cur = None
    for ln in txt.split('\n'):
        m = re.match(r'== hook (f_[0-9A-F]+)', ln)
        if m:
            cur = m.group(1); out[cur] = []; continue
        m = re.match(r'\s+place @([0-9A-F]+) obj (E?\d+) .*fr4..7 = ([-\d.]+), ([-\d.]+), ([-\d.]+), ([-\d.]+)', ln)
        if m and cur:
            oid = m.group(2)
            oid = (0x40000000 | int(oid[1:])) if oid.startswith('E') else int(oid)
            out[cur].append(dict(at=m.group(1), obj=oid, pos=tuple(float(m.group(k)) for k in (3, 4, 5)),
                                 scale=float(m.group(6))))
    return out


def object_triangles(oid, texlist):
    """Per-triangle records of one arcade object, same fields as geo_course.arcade_triangles."""
    r, nodes = EC.object_nodes(oid)
    mats = {}
    rm = EC.rec('model', r[2]) if r[2] != 0xFFFFFFFF else None
    if rm and rm[2]:
        mats = HM.decode_materials(EC.rom.read(rm[2], rm[3]))
    out = []
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
            out.append(rec)
            if nocull:
                out.append(dict(rec, P=rec['P'][[0, 2, 1]], N=-rec['N'][[0, 2, 1]], UV=rec['UV'][[0, 2, 1]],
                                A=rec['A'][[0, 2, 1]], back=True))
    return out


def convert(N, X, code, existing_materials):
    """-> dict(psg=[(name, bytes, info)], textures=[(material, pst bytes, psm bytes)], placements=[...])."""
    hook = HOOK_OF_COURSE.get(N)
    res = dict(psg=[], textures=[], placements=[])
    if not hook:
        return res
    pl = parse_placements().get(hook, [])
    land, coll, stat = EC.w(0x0C3E4B30 + 12 * N, 3)
    L = EC.w(0x0C3E7FF8 + 52 * land, 13)
    ts = EC.rec('texset', L[0])
    texlist = [EC.w(ts[1] + 12 * k, 3)[0] for k in range(ts[2])] if ts[1] else []
    mats_used = set()
    for i, p in enumerate(pl):
        tris = object_triangles(p['obj'], texlist)
        buckets = {}
        items = []
        for t in tris:
            t = dict(t, P=t['P'] * p['scale'] + np.array(p['pos']))   # arcade placement: scale, then move
            P, Nn, UV, A = GC.apply(t, X)
            e = np.cross(P[1] - P[0], P[2] - P[0])
            if np.linalg.norm(e) < 1e-9:
                continue
            if float(e @ Nn.sum(0)) < 0:
                Nn = -Nn
            items.append([P, Nn, UV, GC.bake(Nn, t['mat'], A), GC.material_key(t), GC.layer_value(t)])
        # depth bias -> ordered draw layers moved along the vertex normals (geometry helper's rule,
        # geo_course.layer_value / LAYER_STEP), ranked within this object
        rank = {v: k for k, v in enumerate(sorted({it[5] for it in items}))}
        for P, Nn, UV, C, key, lay in items:
            k = rank[lay]
            if k:
                nn = Nn / np.maximum(np.linalg.norm(Nn, axis=1, keepdims=True), 1e-12)
                P = P + nn * (k * GC.LAYER_STEP)
            for P1, N1, UV1, C1 in GC.split_uv(P, Nn, UV, C):
                uv, _ = GC.shift_uv(UV1)
                buckets.setdefault(key, []).append((P1, N1, uv, C1))
        for key, lst in sorted(buckets.items(), key=lambda kv: str(kv[0])):
            mname = GC.material_name(code, key)
            mats_used.add(key)
            P = np.vstack([x[0] for x in lst]); Nn = np.vstack([x[1] for x in lst])
            UV = np.vstack([x[2] for x in lst]); C = np.vstack([x[3] for x in lst])
            T = np.arange(len(P)).reshape(-1, 3)
            name = '%s_h%02d_%s_%s' % (code, i, EC.idname(p['obj']).lower(), mname[len(code) + 1:])
            blob, info = geo_encode.build_psg(name, mname, P, Nn, UV, C, T, world=True)
            if info['quantum_ft'] > GC.QUANTUM_MAX:
                raise ValueError('%s quantum %.3f ft' % (name, info['quantum_ft']))
            info['bounds_min'] = P.min(0).tolist(); info['bounds_max'] = P.max(0).tolist()
            res['psg'].append((name, blob, info))
        res['placements'].append(dict(p, triangles=len(tris), obj=EC.idname(p['obj'])))
    for key in sorted(mats_used):
        mname = GC.material_name(code, key)
        if mname in existing_materials:
            continue
        tid, blend = key
        if tid >= 0:
            rgba, ti = EC.texture_rgba(tid)
        else:
            rgba, ti = np.full((16, 16, 4), 255, np.uint8), dict(id='white')
        pst, pinfo = geo_pst.encode_rgba(rgba)
        extra = []
        if tid >= 0 and not ti['repeat_u']:
            extra.append(('wrapModeHorizontal', '1'))
        if tid >= 0 and not ti['repeat_v']:
            extra.append(('wrapModeVertical', '1'))
        if blend:
            extra += [('alphaBlendingOnOff', '1'), ('inputColorA', '0'), ('inputColorB', '1'),
                      ('inputAlphaC', '0'), ('inputAlphaD', '1')]
        res['textures'].append((mname, pst, geo_psm.material(mname, 'PreLitCompressed', extra)))
    return res
