r"""Offline checks of the converted menu archives (export\a2r\ui) with the port's own decoders, and renders
of every new model / image next to the original -> export\a2r\ui\verify\*.png, verify.json.

    python -I ui_verify.py [04 11 ...]

Decoders: tools\assets\decode_scene48c.decode_psg3_scene (PSG), decode_texture48d.decode_texture / parse_material
(PST / PSM), geo_res.read_res (RES3 rules of import_decode48c.cpp); MOT through the readMot49O admission rules
(ui_convert.check_mot); MCF/VUT through the rules of original_ui_rider_character_assets50kg.cpp; LOC through the
rules of original_ui_localization50kg.cpp (467 names, unique, <= 480 chars, max-length table <= 480), for all
five languages.
"""
import json
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
import decode_scene48c  # noqa: E402
import decode_texture48d  # noqa: E402
import geo_psg  # noqa: E402
import geo_pst  # noqa: E402
import geo_res  # noqa: E402
import pod_psg  # noqa: E402
import softrender as SR  # noqa: E402
import trk_res3  # noqa: E402
import ui_convert as U  # noqa: E402

BS = chr(92)
VER = os.path.join(U.OUT, 'verify')


class Side:
    """Original (game folder) or converted (export\\a2r\\ui, falling back to the original for unchanged files)."""
    def __init__(self, converted):
        self.converted = converted
        self.cache = {}

    def arch(self, rel):
        if rel not in self.cache:
            p = os.path.join(U.OUT, rel) if self.converted and os.path.exists(os.path.join(U.OUT, rel)) else os.path.join(U.GAME, rel)
            self.cache[rel] = trk_res3.read_res3(open(p, 'rb').read())
        return self.cache[rel]

    def get(self, rel, name):
        return self.arch(rel).get(name)

    def tex(self, base, rels):
        for rel in rels:
            r = self.arch(rel)
            for n, lo, sz in r.members:
                if n.split(BS)[-1].lower() == base.lower() + '.pst':
                    img, t = U.pst_rgba(r.payload[lo:lo + sz])
                    return img
        return None


def tris_of(blob, side, rels, W=None, keep_alpha=()):
    m = geo_psg.parse(blob)
    W = W if W is not None else pod_psg.world_matrices(m['nodes'])
    P, T, UV, TT, texs, tix = [], [], [], [], [], {}
    for s in geo_psg.vertices(m, 0):
        mt = s['material']
        if mt not in tix:
            im = side.tex(mt, rels)
            if im is not None:
                im = im.copy()
                if mt.lower() not in keep_alpha:
                    im[..., 3] = 255
            tix[mt] = len(texs); texs.append(im)
        b0 = len(P)
        P += [(np.r_[p, 1] @ W[b])[:3] for p, b in zip(s['pos'], s['bone'])]
        UV += [tuple(u) for u in s['uv']]
        T += [(b0 + a, b0 + b, b0 + c) for a, b, c in s['tris']]
        TT += [tix[mt]] * len(s['tris'])
    return np.array(P), np.array(T), np.array(UV), np.array(TT), texs


def render_rows(rows, views, path, title, tile=320, common=True):
    img = Image.new('RGB', (tile * len(views), 22 + len(rows) * (tile + 4)), (18, 18, 24))
    dr = ImageDraw.Draw(img)
    dr.text((6, 4), title, fill=(255, 220, 90))
    allp = np.vstack([r[1][0] for r in rows])
    for ri, (label, s) in enumerate(rows):
        P, T, UV, TT, texs = s
        src = allp if common else P
        c = (src.max(0) + src.min(0)) / 2
        r = np.linalg.norm(src - c, axis=1).max()
        for ci, (vn, d) in enumerate(views):
            d = np.array(d, float); d /= np.linalg.norm(d)
            cam = dict(ortho=True, center=c, size=2.05 * r, dir=-d, up=(0, 0, 1) if vn == 'top' else (0, 1, 0))
            im, _, _ = SR.render(P, T, cam, W=tile, H=tile, uv=UV, tex_of_tri=TT, textures=texs,
                                 light=(0.4, 0.8, 0.45), ambient=0.5, bg=(34, 36, 48))
            img.paste(Image.fromarray(im), (ci * tile, 22 + ri * (tile + 4)))
            dr.text((ci * tile + 4, 24 + ri * (tile + 4)), '%s  %s' % (label, vn), fill=(255, 255, 255))
    img.save(path)


POD_VIEWS = [('3/4 front', (0.75, 0.45, 0.8)), ('side', (-1, 0.05, 0)), ('top', (0, 1, 0.001)), ('front', (0, 0.15, 1))]
CHAR_VIEWS = [('front', (0, 0.1, 1)), ('3/4', (0.7, 0.25, 0.7)), ('side', (1, 0.05, 0))]


def decode_checks(rep):
    frag = json.load(open(os.path.join(U.OUT, 'converted-content.fragment.json')))
    for row in frag['files']:
        rel = row['path'].replace('/', BS)
        p = os.path.join(U.OUT, rel)
        raw = open(p, 'rb').read()
        members, order, info = geo_res.read_res(p)
        ok = len(raw) == row['bytes'] and U.sha(raw) == row['sha256'] and len(order) == len(row['members'])
        orig = trk_res3.read_res3(open(os.path.join(U.GAME, rel), 'rb').read())
        ok = ok and [o[0] for o in order] == [m[0] for m in orig.members]
        res = dict(file_matches_row=ok, members=len(order), decoded=info)
        dec = {}
        for m in row['members']:
            if not m['changed']:
                continue
            n = m['name']; b = members[n.lower()]
            assert U.sha(b) == m['sha256']
            lw = n.lower()
            try:
                if lw.endswith('.psg'):
                    sc = decode_scene48c.decode_psg3_scene(b)
                    dec[n] = dict(ok=True, nodes=len(sc['nodes']), vertices=sc['decoded_vertices_all_lods'])
                elif lw.endswith('.pst'):
                    d, _ = decode_texture48d.decode_texture(b)
                    dec[n] = dict(ok=True, levels=[(x['width'], x['height']) for x in d])
                elif lw.endswith('.mot'):
                    dec[n] = dict(ok=True, **U.check_mot(b))
                elif lw.endswith('.mcf'):
                    dec[n] = dict(ok=True, **check_mcf(n, b, members))
                elif lw.endswith('.vut'):
                    dec[n] = dict(ok=True, **check_vut(n, b, members))
                elif lw.endswith('.phy'):
                    t = U.ini(b.decode('latin1'))
                    dec[n] = dict(ok=True, name=t['Pod Setup']['Name'], prefix=t['Pod Setup']['Prefix'], bytes=len(b))
                else:
                    dec[n] = dict(ok=True, bytes=len(b))
            except Exception as e:  # noqa: BLE001
                dec[n] = dict(ok=False, error='%s: %s' % (type(e).__name__, e))
        if rel == 'LOC.RES':
            res['loc'] = check_loc(members)
        res['decoders'] = dec
        res['failures'] = {k: v for k, v in dec.items() if not v['ok']}
        rep[row['path']] = res
        print('%-28s row %s  decoded %d changed members, failures %d' % (row['path'], ok, len(dec), len(res['failures'])))


def check_mcf(name, b, members):
    t = U.ini(b.decode('latin1'))
    g = t['General info']
    assert int(g['NumberOfMotions']) == 6 and not g.get('ContentDirectory') and g['SLTFile'].endswith('.slt') and g['VUTfilename'].endswith('.vut')
    base = g['SLTFile'][:-4]
    need = {base + s for s in ('_ent', '_sel', '_breathe', '_idl1', '_idl2', '_idl3')}
    seen = set()
    for i in range(1, 7):
        s = t['MOTION %d' % i]
        assert s['VUE'][:-4] == s['NAME'] and s['NAME'] in need and s['NAME'] not in seen
        assert int(s['FrameCount']) > 0 and float(s['FPS']) > 0 and s['Looping'].lower() in ('yes', 'no', '0', '1', 't', 'f')
        seen.add(s['NAME'])
        mot = members[('ui' + BS + 'anim' + BS + s['NAME'] + '.mot').lower()]
        m = U.check_mot(mot)
        assert abs((int(s['FrameCount']) - 1) / float(s['FPS']) - m['duration']) < 1e-3 and m['frames'] == int(s['FrameCount'])
    assert members[('ui' + BS + 'anim' + BS + base + '.psg').lower()] and members[('ui' + BS + 'anim' + BS + g['VUTfilename']).lower()]
    return dict(motions=sorted(seen))


def check_vut(name, b, members):
    psg = geo_psg.parse(members[name.lower()[:-4] + '.psg'])
    names = {n['name'] for n in psg['nodes']}
    lines = [ln for ln in b.decode('ascii').splitlines() if ln.strip()]
    missing = [ln.split('=')[0] for ln in lines if ln.split('=')[0] not in names]
    assert not missing, missing
    sc = decode_scene48c.decode_psg3_scene(members[name.lower()[:-4] + '.psg'])
    return dict(bindings=len(lines), psg_nodes=len(sc['nodes']))


def check_loc(members):
    out = {}
    loc = members['loc' + BS + 'english' + BS + 'ui.loc']
    import struct
    count = struct.unpack_from('<H', loc, 26)[0]
    first, table = struct.unpack_from('<II', loc, 4)
    names = loc[first:table - 1].split(b'\0')[:count]
    assert count == 467 and len(set(names)) == 467 and all(0 < len(n) <= 480 for n in names)
    for d, L in U.LANGS:
        assert members[('loc' + BS + d + BS + 'ui.loc')] == loc
        s = members[('loc' + BS + d + BS + 'l_ui_%s.bin' % L).lower()].split(b'\0')
        assert len(s) >= count
        for i in range(count):
            mx = struct.unpack_from('<H', loc, table + 8 * i + 4)[0]
            assert len(s[i]) <= 480 and mx <= 480 and len(s[i]) <= mx, (L, i, s[i], mx)
        out[L] = [s[i].decode('latin1') for i in range(177, 200)]
    return out


def renders(nn):
    os.makedirs(VER, exist_ok=True)
    O, N = Side(False), Side(True)
    rider = [r'UI\UIRIDER\UIRIDER.RES', r'UI\UIPODTEX.RES']
    g = os.path.join('ui', 'geometry', '%s.psg' % nn)
    rows = [('original %s.psg' % nn, tris_of(O.get(r'UI\UIRIDER\UIRIDER.RES', g), O, rider)),
            ('converted %s.psg' % nn, tris_of(N.get(r'UI\UIRIDER\UIRIDER.RES', g), N, rider))]
    render_rows(rows, POD_VIEWS, os.path.join(VER, 'pod_%s.png' % nn), 'UIRIDER ui\\geometry\\%s.psg  original (top) vs converted (bottom), same camera; black = shadow quad (alpha not shown)' % nn)
    ge = r'UI\UIGARAGE\%sE.RES' % nn
    g = os.path.join('ui', 'geometry', '%se.psg' % nn)
    rows = [('original %se.psg' % nn, tris_of(O.get(ge, g), O, [ge])), ('converted %se.psg' % nn, tris_of(N.get(ge, g), N, [ge]))]
    render_rows(rows, POD_VIEWS, os.path.join(VER, 'garage_%s.png' % nn), 'UIGARAGE %sE.RES ui\\geometry\\%se.psg  original vs converted, same camera' % (nn, nn))
    # character posed by each motion (frame at 40 %)
    rows = []
    for label, side in (('original', O), ('converted', N)):
        r = side.arch(r'UI\UIRIDER\UIRIDER.RES')
        m = geo_psg.parse(r.get(os.path.join('ui', 'anim', '%sc.psg' % nn)))
        for suf in ('idl1', 'ent', 'sel'):
            mot = U.read_mot(r.get(os.path.join('ui', 'anim', '%sc_%s.mot' % (nn, suf))))
            W = U.pose_world(m, mot, 0.4 * mot['duration'])
            rows.append(('%s %sc_%s t=%.1fs' % (label, nn, suf, 0.4 * mot['duration']),
                         tris_of(r.get(os.path.join('ui', 'anim', '%sc.psg' % nn)), side, [r'UI\UIRIDER\UIRIDER.RES'], W=W)))
    render_rows(rows, CHAR_VIEWS, os.path.join(VER, 'character_%s.png' % nn),
                'UIRIDER ui\\anim\\%sc.psg posed by its MOTs: original (rows 1-3) vs converted (rows 4-6), same camera (motion space)' % nn, tile=260)
    # images
    items = [(r'UI\UIRIDER\UIRIDER.RES', '%s_flrg' % nn), (r'UI\TEXTURES\OVERLAYS.RES', '%s_fsm' % nn),
             (r'UI\UIRIDER\UIRIDER.RES', '%s_shdw' % nn), (r'UI\UIBKG.RES', '%s_bkg' % nn),
             (r'UI\UIPODTEX.RES', '%s_1' % nn), (r'UI\UIPODTEX.RES', '%s_2' % nn), (r'UI\UIPODTEX.RES', '%s_3' % nn),
             (r'UI\UIRIDER\UIRIDER.RES', '%sc_1' % nn), (r'UI\UIRIDER\UIRIDER.RES', '%sc_2' % nn),
             (r'UI\UIGARAGE\%sE.RES' % nn, '%se_1' % nn)]
    T = 200
    sheet = Image.new('RGB', (len(items) * (T + 6), 2 * (T + 20) + 20), (40, 40, 48))
    dr = ImageDraw.Draw(sheet)
    dr.text((4, 2), 'POD%s menu images: original (top) / converted (bottom); flags and shadow on magenta to show alpha' % nn, fill=(255, 220, 90))
    for i, (rel, base) in enumerate(items):
        for j, side in enumerate((O, N)):
            im = side.tex(base, [rel])
            x, y = i * (T + 6), 20 + j * (T + 20)
            if im is None:
                dr.text((x + 4, y + 30), '%s: IPU (kind 2),\nnot decodable here' % base, fill=(255, 120, 120))
                continue
            pil = Image.fromarray(im, 'RGBA')
            if base.endswith(('_flrg', '_fsm', '_shdw')):
                bg = Image.new('RGBA', pil.size, (255, 0, 255, 255))
                if base.endswith('_shdw'):
                    bg = Image.new('RGBA', pil.size, (230, 230, 230, 255))
                    a = np.asarray(pil)[..., 3].astype(float)
                    pil = Image.fromarray(np.dstack([np.zeros_like(a), np.zeros_like(a), np.zeros_like(a), np.clip(a * 3.5, 0, 255)]).astype(np.uint8), 'RGBA')
                bg.alpha_composite(pil); pil = bg
            pil = pil.convert('RGB')
            pil.thumbnail((T, T), Image.NEAREST if pil.width < T else Image.LANCZOS)
            if pil.width < T and pil.height < T:
                f = T // max(pil.width, pil.height); pil = pil.resize((pil.width * f, pil.height * f), Image.NEAREST)
            sheet.paste(pil, (x, y))
            dr.text((x + 2, y + T + 2), '%s %dx%d' % (base, im.shape[1], im.shape[0]), fill=(255, 255, 255))
    sheet.save(os.path.join(VER, 'images_%s.png' % nn))


def main(pods):
    rep = {}
    decode_checks(rep)
    for nn in pods:
        renders(nn)
        print('rendered', nn)
    json.dump(rep, open(os.path.join(VER, 'verify.json'), 'w'), indent=1, default=str)


if __name__ == '__main__':
    os.makedirs(VER, exist_ok=True)
    main(sys.argv[1:] or ['04', '11', '15', '17', '23', '24'])
