r"""Offline checks for a converted pod (called by pod_convert.py after writing).

1. RES re-read (geo_res.read_res, the RES3 rules of src\assets\import_decode48c.cpp), member list and
   order compared with the target original.
2. Every PSG through the port's decoder tools\assets\decode_scene48c.py decode_psg3_scene; every
   written PST through decode_texture48d.decode_texture; every PSM through parse_material; type-0
   COL through decode_collision48g.decode; type-2 COL through the decodeType2Resource49X admission
   rules (pod_formats.check_type2); MOT through the readMot49O admission rules.
3. Renders of chariot + engines (LOD 0, decoded back from the written RES) next to the target
   original, same camera and scale -> verify\compare.png.
4. manifest.json and converted-content.fragment.json.
"""
import json
import math
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))
PORT_ASSETS = ((__import__('os').environ['RR_CONVERT_ROOT']+'/port')+'/tools/assets')
sys.path.insert(0, PORT_ASSETS)
import decode_scene48c  # noqa: E402  (port decoders)
import decode_texture48d  # noqa: E402
import decode_collision48g  # noqa: E402
import geo_psg  # noqa: E402
import geo_pst  # noqa: E402
import geo_res  # noqa: E402
import pod_formats as PF  # noqa: E402
import pod_psg  # noqa: E402
import softrender as SR  # noqa: E402


def check_mot(b):
    """readMot49O admission (model_geometry49o.cpp:78-127)."""
    assert len(b) <= 256 * 1024 and len(b) >= 28
    tag, nf, dur, rt, one, npos, nrot = struct.unpack_from('<IIfIIII', b, 0)
    assert tag == 0xFFFFFFFE and 1 <= nf <= 256 and math.isfinite(dur) and dur > 0 and rt <= 1 and one == 1
    assert npos <= 64 and nrot <= 64
    at, total = 28, 0
    for ch, cnt in ((0, npos), (1, nrot)):
        st = 40 if ch == 0 else (12 if rt else 8)
        for _ in range(cnt):
            k, row = struct.unpack_from('<II', b, at); at += 8
            assert 1 <= k <= 256 and row < 64
            total += k
            prev = 0
            for j in range(k):
                t = struct.unpack_from('<f', b, at + j * st)[0]
                assert math.isfinite(t) and 0 <= t <= dur and (j == 0 or t > prev)
                prev = t
            at += k * st
    assert total <= 4096 and at == len(b)
    return dict(frames=nf, position_tracks=npos, rotation_tracks=nrot, keys=total)


def model_tris(members, nn, parts=('chariot', 'englt1', 'engrt1'), lod=0):
    """LOD triangles of the pod parts in the pod frame with their page textures."""
    P, T, UV, TT, texs, tix = [], [], [], [], [], {}

    def tex(mat):
        if mat in tix:
            return tix[mat]
        psm = members.get(('textures\\%s.psm' % mat).lower())
        img = None
        if psm is not None:
            f = decode_texture48d.parse_material(psm)
            name = f['passes'][0]['textures'][0]['name']
            pst = members.get(('textures\\%s.pst' % name).lower())
            if pst is not None:
                t = geo_pst.parse(pst)
                if t['kind'] in (0, 1):
                    img = geo_pst.rgba_levels(t)[-1].copy()
                    img[..., 3] = 255   # pod pages: alpha = specular mask, not coverage
        tix[mat] = len(texs)
        texs.append(img)
        return tix[mat]
    for part in parts:
        m = geo_psg.parse(members[(r'pods\pod%s\%s.psg' % (nn, part)).lower()])
        W = pod_psg.world_matrices(m['nodes'])
        for s in geo_psg.vertices(m, min(lod, len(m['lods']) - 1)):
            if s['material'].lower().startswith('flame'):
                continue
            ti = tex(s['material'])
            V = [(np.r_[p, 1.0] @ W[b])[:3] for p, b in zip(s['pos'], s['bone'])]
            base = len(P)
            P += V
            UV += [tuple(u) for u in s['uv']]
            for a, b, c in s['tris']:
                T.append((base + a, base + b, base + c))
                TT.append(ti)
    return np.array(P), np.array(T), np.array(UV), np.array(TT), texs


def render_pair(tgt, conv, path, title):
    sets = [tgt, conv]
    allp = np.vstack([s[0] for s in sets])
    c = (allp.max(0) + allp.min(0)) / 2
    r = np.linalg.norm(allp - c, axis=1).max()
    views = [('3/4 front', (0.75, 0.45, 0.8)), ('side (left)', (-1, 0.05, 0)), ('top', (0, 1, 0.001)), ('3/4 rear', (-0.7, 0.5, -0.8))]
    tile = 420
    img = Image.new('RGB', (tile * len(views), 2 * tile + 40), (18, 18, 24))
    dr = ImageDraw.Draw(img)
    dr.text((6, 4), title, fill=(255, 220, 90))
    for row, (label, s) in enumerate(zip(('target original', 'converted'), sets)):
        P, T, UV, TT, texs = s
        for col, (vn, d) in enumerate(views):
            d = np.array(d, float); d /= np.linalg.norm(d)
            cam = dict(ortho=True, center=c, size=2.1 * r, dir=-d, up=(0, 0, 1) if vn == 'top' else (0, 1, 0))
            im, _, _ = SR.render(P, T, cam, W=tile, H=tile, uv=UV, tex_of_tri=TT, textures=texs,
                                 light=(0.4, 0.8, 0.45), ambient=0.45, bg=(30, 32, 44))
            img.paste(Image.fromarray(im), (col * tile, 20 + row * (tile + 10)))
            dr.text((col * tile + 6, 24 + row * (tile + 10)), '%s  %s' % (label, vn), fill=(255, 255, 255))
    img.save(path)


def verify(pod):
    nn, T = pod.nn, pod.T
    out = pod.out
    vdir = os.path.join(out, 'verify')
    os.makedirs(vdir, exist_ok=True)
    res_path = os.path.join(out, 'install', 'PODS', 'POD%s' % nn, 'POD%s.RES' % nn)
    members, order, info = geo_res.read_res(res_path)
    names = [o[0] for o in order]
    rep = dict(res=dict(path=res_path, bytes=info['bytes'], blocks=info['blocks'], members=info['members'],
                        target_bytes=len(T.raw), target_members=len(T.names)))
    rep['members_equal_target'] = names == T.names
    rep['target_order_kept'] = [n for n in names if n in T.names] == T.names
    rep['extra'] = [n for n in names if n not in T.names]
    rep['missing'] = [n for n in T.names if n not in names]
    written = {n.lower(): b for n, b in pod.members}
    rep['reread_identical'] = all(members[n.lower()] == written[n.lower()] for n in names)
    dec = {}
    for n in names:
        low = n.lower()
        b = members[low]
        try:
            if low.endswith('.psg'):
                sc = decode_scene48c.decode_psg3_scene(b)
                al = {}
                for L in sc['lods']:
                    for s in L['surfaces']:
                        for k, v in s['normal_alignment'].items():
                            al[k] = al.get(k, 0) + v
                dec[n] = dict(ok=True, nodes=len(sc['nodes']), lods=sc['lod_flags'], vertices=sc['decoded_vertices_all_lods'], normal_alignment=al)
            elif low.endswith('.pst'):
                d, _ = decode_texture48d.decode_texture(b)
                dec[n] = dict(ok=True, levels=[(x['width'], x['height']) for x in d])
            elif low.endswith('.psm'):
                decode_texture48d.parse_material(b)
                dec[n] = dict(ok=True)
            elif low.endswith('.col'):
                ty = struct.unpack_from('<I', b, 12)[0]
                if ty == 0:
                    c = decode_collision48g.decode(b)
                    dec[n] = dict(ok=True, type=0, faces=len(c.get('faces', [])) if isinstance(c, dict) else None)
                else:
                    dec[n] = dict(ok=True, type=ty, **PF.check_type2(b))
            elif low.endswith('.mot'):
                dec[n] = dict(ok=True, **check_mot(b))
        except Exception as e:  # noqa: BLE001
            dec[n] = dict(ok=False, error='%s: %s' % (type(e).__name__, e))
    rep['decoders'] = dec
    rep['decoder_failures'] = {n: d for n, d in dec.items() if not d['ok']}
    # renders + sizes
    tm = T.members
    tgt = model_tris(tm, nn)
    conv = model_tris(members, nn)
    render_pair(tgt, conv, os.path.join(vdir, 'compare.png'),
                'POD%s  target original (top) vs converted %s %s (bottom), LOD 0 chariot + engines, same camera' % (nn, pod.code, pod.T.nn))
    tgc = model_tris(tm, nn, ('chariot',))
    cvc = model_tris(members, nn, ('chariot',))
    render_pair(tgc, cvc, os.path.join(vdir, 'compare_chariot.png'), 'POD%s chariot (with pilot) target vs converted' % nn)
    # wreck pieces (pod frame) and LOD 1 (pilot rigid) next to the target's
    pnames = [os.path.basename(n)[:-4].lower() for n in names if n.lower().endswith('.psg')
              and os.path.basename(n).lower()[:6] in ('englt1', 'engrt1') and len(os.path.basename(n)) == len('englt1a.psg')]
    render_pair(model_tris(tm, nn, pnames), model_tris(members, nn, pnames), os.path.join(vdir, 'compare_pieces.png'),
                'POD%s wreck pieces %s, target vs converted (pod frame)' % (nn, ' '.join(pnames)))
    render_pair(model_tris(tm, nn, lod=1), model_tris(members, nn, lod=1), os.path.join(vdir, 'compare_lod1.png'),
                'POD%s LOD 1 target vs converted' % nn)
    # motions: decoded ByteAxis rotation vs the PSG local rotation of the same row
    ch = geo_psg.parse(members[(r'pods\pod%s\chariot.psg' % nn).lower()])
    rws = pod_psg.rows(ch['nodes'])
    mot_err = {}
    for n in names:
        if n.lower().endswith('.mot') and n.lower() in {m['name'].lower() for m in pod.manifest if m['source'] != 'D'}:
            mt = PF.read_mot(members[n.lower()])
            worst = 0.0
            for row, keys in mt['rot']:
                L3 = np.array(ch['nodes'][rws[row]]['matrix']).reshape(4, 4)[:3, :3]
                for k in keys:
                    t, a, x, y, z = struct.unpack('<fBbbb', k)
                    Rq = PF.rows_from_quat(PF.decode_byte_axis(a, (x, y, z)))
                    c = (np.trace(Rq.T @ L3) - 1) / 2
                    worst = max(worst, math.degrees(math.acos(max(-1, min(1, c)))))
            mot_err[n] = round(worst, 2)
    rep['motion_rotation_error_deg'] = mot_err
    sz = {}
    for label, s in (('target', tgt), ('converted', conv)):
        lo, hi = s[0].min(0), s[0].max(0)
        sz[label] = dict(min=lo.round(2).tolist(), max=hi.round(2).tolist(), size=(hi - lo).round(2).tolist())
    for part in ('chariot', 'englt1', 'engrt1'):
        for label, mm in (('target', tm), ('converted', members)):
            p = model_tris(mm, nn, (part,))[0]
            sz.setdefault(part, {})[label] = (p.max(0) - p.min(0)).round(2).tolist()
    rep['size_ft'] = sz
    # facts the port pins
    def nodes_of(mm, part):
        return len(geo_psg.parse(mm[(r'pods\pod%s\%s.psg' % (nn, part)).lower()])['nodes'])
    pieces = sum(1 for n in names if n.lower().endswith('.psg') and os.path.basename(n.lower())[:6] in ('englt1', 'engrt1')
                 and len(os.path.basename(n)) == len('englt1a.psg'))
    facts = dict(entries=len(names), decoded_bytes=info['blocks'] * 24576,
                 chariot_nodes=nodes_of(members, 'chariot'), englt1_nodes=nodes_of(members, 'englt1'),
                 engrt1_nodes=nodes_of(members, 'engrt1'),
                 wreck_pieces=1 + pieces, wreck_collections=len(T.wreck()),
                 chariot_motions=sum(1 for m in pod.manifest if m['name'].lower().endswith('.mot') and 'eng' not in m['name'].lower()),
                 target_chariot_nodes=nodes_of(tm, 'chariot'))
    rep['facts'] = facts
    man = dict(schema='a2r-pod-manifest/1', arcade=dict(code=pod.code, kind=pod.kind, pilot=pod.code and __import__('pod_convert').PILOT_NAMES[pod.code]),
               packed_as='POD%s' % nn, converter='tools\\a2r\\pod_convert.py %s %s' % (pod.code, nn),
               transform=dict(scale=float(__import__('a2r_space').SCALE), z_sign=float(__import__('a2r_space').z_sign()),
                              origin=pod.origin, note='arcade pod frame -> Revenge: x3, z mirrored (a2r_space); origin shift along z only'),
               pages=pod.page_mat, chariot_nodes=[n['name'] for n in pod.ch_nodes],
               hand_nodes=pod.stats['chariot'].get('hand_nodes'), engines=pod.engine_source,
               cable_source=pod.cable_source, notes=pod.notes, checks=rep,
               members=pod.manifest)
    json.dump(man, open(os.path.join(out, 'manifest.json'), 'w'), indent=1, default=str)
    import hashlib
    res = open(res_path, 'rb').read()
    frag = dict(path='PODS/POD%s/POD%s.RES' % (nn, nn), bytes=len(res), sha256=hashlib.sha256(res).hexdigest(),
                replaces=dict(bytes=len(T.raw), sha256=hashlib.sha256(T.raw).hexdigest()),
                pod_number=nn, arcade_code=pod.code, facts=facts,
                members=[dict(name=n, offset=lo, bytes=nb, sha256=hashlib.sha256(members[n.lower()]).hexdigest(),
                              borrowed=T.has(n) and hashlib.sha256(members[n.lower()]).hexdigest() == hashlib.sha256(T.get(n)).hexdigest(),
                              source=[m for m in pod.manifest if m['name'] == n][0]['source'])
                         for n, lo, nb in order],
                cam=dict(path='PODS/POD%s/POD%s.CAM' % (nn, nn), bytes=len(T.cam), sha256=hashlib.sha256(T.cam).hexdigest(),
                         borrowed=True, note='target camera presets unchanged (identical file)'))
    json.dump(frag, open(os.path.join(out, 'converted-content.fragment.json'), 'w'), indent=1)
    pod.report = rep
    return rep
