"""Per-mesh draw state of every pod part model (all 18 kinds, LOD 0..3): poly type, poly alpha,
vertex alpha range, texcoord-offset (0D1) slot bits 19:18, no-cull tris, material (881/A81/C81),
texture (format, alpha present). Read-only. Run: python -I dump_states.py [CODE ...]"""
import os, sys, struct, json
from collections import Counter, defaultdict
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import hikaru_mesh as HM, racer_assets as R, pod_export as PE
import numpy as np

def header_stats(buf):
    """Walk the CP list like HM.decode and collect per-mesh vertex-header stats."""
    st = []
    in_mesh = False; off = 0; tex = None; tex_base = 0
    cur = None
    while off + 4 <= len(buf):
        w = struct.unpack_from('<I', buf, off)[0]
        size = HM.insn_size(w); op = w & 0x1FF
        if op not in HM.KNOWN: break
        if not in_mesh and op in HM.OPENS:
            st.append(Counter()); cur = st[-1]; in_mesh = True
        elif in_mesh and op not in HM.KEEPS:
            in_mesh = False
        if op == 0x0C3:
            if w & 0x1000: tex = (tex_base + (w >> 16)) & 0xFFFF
            else: tex_base = w >> 16
        if op in HM.PUSH_UV3:
            idx = tex_base + (w >> 16)
            if idx != tex:
                tex = idx; st.append(Counter()); cur = st[-1]; in_mesh = True
        if op in HM.OPENS:
            if op in HM.PUSH_STATIC or op in HM.PUSH_DYN or op in HM.PUSH_ALL:
                cur['alpha%d' % (w >> 24)] += 1
            cur['slot%d' % ((w >> 18) & 3)] += 1
            if w & 0x800000: cur['nocullbit'] += 1
            if w & 0x4: cur['tpivot'] += 1
        if op in (0x0D1,):
            cur and cur.update(['0D1'])
        off += size
        if op in (0x082, 0x1C2) and ((w >> 12) & 0xF) == 0: break
    return st

def mat_raw(mid):
    """raw 881/A81/C81 words per committed material"""
    m = R.model(mid)
    buf = HM.Rom(os.path.join(HERE, '..', '..', 'img')).read(m['attr'], m['attr_size'])
    out = {}; cur = {}; off = 0
    while off + 4 <= len(buf):
        w = struct.unpack_from('<I', buf, off)[0]; size = HM.insn_size(w)
        full = w & 0xFFF; op = w & 0x1FF
        if full in (0x881, 0xA81, 0xC81, 0x081): cur['%03X' % full] = '%08X' % w
        elif full in (0x091, 0x291, 0x491, 0x691):
            cur['%03X' % full] = '%08X %08X' % struct.unpack_from('<2I', buf, off)
        elif op == 0x084: out[(w >> 16) & 0x3FF] = dict(cur)
        elif op == 0x082: break
        elif op not in HM.KNOWN:
            cur.setdefault('other', []).append('%08X' % w)
        off += size
    return out

def main():
    codes = sys.argv[1:] or R.CODES
    res = {}
    for code in codes:
        kind = R.CODES.index(code)
        pd = R.pod(kind)
        tg0 = R.texgroup(pd['tg_slot0'][0]); tg1 = R.texgroup(pd['tg_other'][0])
        seen = set(); rows = []
        for lod in range(4):
            part = pd['parts'][lod]
            objs = [('cockpit', part['objA']), ('binder', part['objB'])] + \
                   [('engine%d' % i, part['engines'][i]) for i in range(pd['engines'])]
            for role, oid in objs:
                if oid in (None, PE.NONE): continue
                o = R.object_nodes(oid)
                for k, n in enumerate(o['nodes']):
                    mid = n['model']
                    if mid == PE.NONE or (mid in seen): continue
                    seen.add(mid)
                    mm = PE.model_mesh(mid)
                    if mm is None: continue
                    mats = PE.model_materials(mid); raw = mat_raw(mid)
                    hs = header_stats(R.rombytes(R.model(mid)['geom'], R.model(mid)['geom_size']))
                    tg = tg0 if lod == 0 else tg1
                    for mi, me in enumerate(mm.meshes):
                        tris = [t for t in mm.tris if t[3] == mi]
                        if not tris: continue
                        ti = me['tex']; tid = tg[ti][0] if ti is not None and ti < len(tg) else None
                        tinfo = PE.texture_rgba(tid)[1] if tid is not None else None
                        img = PE.texture_rgba(tid)[0] if tid is not None else None
                        va = sorted({mm.alphas[i] for t in tris for i in t[:3]})
                        mat = mats.get(me['mat'], {})
                        rows.append(dict(lod=lod, role=role, obj=R.idname(oid), node=k, model=R.idname(mid),
                            mesh=mi, tris=len(tris), nocull=sum(t[4] for t in tris), polytype=me['polytype'],
                            poly_alpha=me['poly_alpha'], vtx_alpha=[va[0], va[-1]], mat=me['mat'],
                            mat_dec={k2: mat.get(k2) for k2 in ('shading', 'fog', 'textured', 'alpha_mode', 'blend', 'alpha_index')},
                            mat_raw=raw.get(me['mat']), tex=ti, tid=tinfo['id'] if tinfo else None,
                            tsize='%dx%d' % (tinfo['w'], tinfo['h']) if tinfo else None,
                            tfmt=tinfo['fmt'] if tinfo else None, texmode=tinfo['texmode'] if tinfo else None,
                            tex_alpha=(int(img[..., 3].min()), int(img[..., 3].max())) if img is not None else None,
                            hdr=dict(hs[mi]) if mi < len(hs) else None, bias=me['bias'], pattern=me['pattern']))
        res[code] = rows
    json.dump(res, open(os.path.join(HERE, 'mesh_states.json'), 'w'), indent=1)
    for code, rows in res.items():
        print('==', code)
        for r in rows:
            special = r['polytype'] != 1 or r['vtx_alpha'] != [255, 255] or r['mat_dec'].get('alpha_mode') or \
                      r['mat_dec'].get('blend') or (r['hdr'] and any(k.startswith('slot') and k != 'slot0' for k in r['hdr']))
            print('%s L%d %-8s o%-5s m%-5s me%-2d tris%4d nc%4d pt%d pa%3d va%s mat%s %s tex%s %s %s f%s tm%s ta%s slots%s %s' % (
                '*' if special else ' ', r['lod'], r['role'], r['obj'], r['model'], r['mesh'], r['tris'], r['nocull'],
                r['polytype'], r['poly_alpha'], r['vtx_alpha'], r['mat'],
                {k: v for k, v in r['mat_dec'].items()}, r['tex'], r['tid'], r['tsize'], r['tfmt'], r['texmode'],
                r['tex_alpha'], sorted(k for k in (r['hdr'] or {}) if k.startswith('slot')), r['mat_raw']))
main()
