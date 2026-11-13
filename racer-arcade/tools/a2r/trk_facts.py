r"""Content counts ("facts") of a track RES, computed from the files the way the port counts them.
Definitions are checked against the port's pinned values for original SC and TA
(notes\silo-override.md 1.2 / 2.1) by running this module: python -I trk_facts.py
"""
import hashlib
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import trk_res3, trk_binparam, trk_ini, trk_col  # noqa: E402
import geo_psg, geo_psm, geo_pst  # noqa: E402

BS = chr(92)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def facts(res_bytes, code, candidates=False):
    r = trk_res3.read_res3(res_bytes)
    files = {n.lower(): d for n, d in r.items()}
    f = dict(archive_bytes=len(res_bytes), archive_sha256=sha(res_bytes), entries=len(r.members),
             decoded_bytes=len(r.payload))
    scb = trk_binparam.decode(files['tracks' + BS + code + '.scb'])
    d = {}
    for n, kv in scb:
        d.setdefault(n, []).extend(kv)
    si = dict(d['sceneinfo'])
    f['zones'] = int(si['numzones']); f['portals'] = int(si['numportals'])
    rows = []
    for z in range(1, f['zones'] + 1):
        zk = dict(d['zone%d' % z])
        for m in range(1, int(zk['numberofstaticmodels']) + 1):
            mm = dict(d['zone%d-model%d' % (z, m)])
            rows.append((z, mm.get('slt', '').lower()[:-4]))
    f['instances'] = len(rows)
    psg_rows = [b for z, b in rows if 'geometry' + BS + b + '.psg' in files]
    col_rows = [b for z, b in rows if 'geometry' + BS + b + '.col' in files]
    f['placements'] = len(psg_rows)
    f['col_only'] = sum(1 for b in col_rows if 'geometry' + BS + b + '.psg' not in files)
    f['placed_col'] = len(col_rows)
    f['collision_files'] = sum(1 for n in files if n.startswith('geometry' + BS) and n.endswith('.col'))
    for kind in 'am':
        ini = trk_ini.parse(files['tracks' + BS + kind + code + '.spl'])
        f[('ai' if kind == 'a' else 'map') + '_nodes'] = int(ini.get('Track', 'NumNodes'))
    tob = {}
    for n, kv in trk_binparam.decode(files['tracks' + BS + code + '.tob']):
        tob.setdefault(n, []).extend(kv)
    f['cuts'] = int(dict(tob.get('cscameras', [('numberofcameras', '0')]))['numberofcameras'])
    f['triggers'] = int(dict(tob.get('triggers', [('numberoftriggers', '0')])).get('numberoftriggers', 0))
    f['static_model_params'] = int(dict(tob.get('staticmodelparams', [('numberofstaticmodelparams', '0')])).get('numberofstaticmodelparams', 0))
    intro = files.get('audio' + BS + 'introaud.ini')
    f['intro_events'] = int(trk_ini.parse(intro).get('ResponseEmitter', 'NumEvents', 0)) if intro else 0
    sky = dict(d.get('environment', []))
    f['sky_cubes'] = int(sky.get('numberofskycubes', 0)) if 'numberofskycubes' in sky else None
    f['cube_member'] = any(n.startswith('skies' + BS) and n.endswith('.cub') for n in files)
    f['search_types'] = sorted(set(dict(d['zone%d' % z]).get('searchtype', '') for z in range(1, f['zones'] + 1)))
    # visual closure: unique scenes referenced by placements
    scenes = sorted(set(psg_rows))
    surf_scene = surf_place = vtx = idx = 0
    mats = set()
    per = {}
    for b in scenes:
        mdl = geo_psg.parse(files['geometry' + BS + b + '.psg'])
        ns = sum(len(lv['surfaces']) for lv in mdl['lods'][:1])
        vs = geo_psg.vertices(mdl)
        nv = sum(len(s_['pos']) for s_ in vs); ni = sum(3 * len(s_['tris']) for s_ in vs)
        per[b] = (ns, nv, ni)
        for s_ in mdl['lods'][0]['surfaces']:
            mats.add(mdl['materials'][s_['material']].lower())
        surf_scene += ns; vtx += nv; idx += ni
    f['scenes'] = len(scenes)
    f['surfaces'] = sum(per[b][0] for b in psg_rows)
    f['materials'] = len(mats)
    f['vertices'] = sum(per[b][1] for b in psg_rows)     # per placement (port pins SC 239,106 / TA 110,196)
    f['indices'] = sum(per[b][2] for b in psg_rows)      # per placement (SC 411,768 / TA 209,514)
    # images and RGBA bytes: one selected texture (TextureMap_1) per material, its largest level
    # (port pins: SC 81 images = 81 materials, TA 29 = 29). Materials animated through an .atm frame
    # set (SC: sc_crowd, arrowred, floorvent01b; TA: arrowred, crowd) are not modelled here; the
    # converted tracks have none.
    rgba = 0
    atm_materials = []
    for m in sorted(mats):
        psm = files.get('textures' + BS + m + '.psm')
        tex = [v.lower() for sec in geo_psm.fields(geo_psm.parse(psm)).values() for k, v in sec.items()
               if k.lower() == 'texturemap_1'] if psm else []
        pst = files.get('textures' + BS + tex[0] + '.pst') if tex else None
        if pst:
            lv = geo_pst.parse(pst)['levels'][-1]
            rgba += lv['w'] * lv['h'] * 4
        else:
            atm_materials.append(m)
    f['images'] = len(mats)
    f['rgba_bytes'] = rgba
    f['materials_without_plain_texture'] = atm_materials
    return f


if __name__ == '__main__':
    import json
    for code, want in (('sc', dict(entries=984, decoded_bytes=26714112, zones=30, portals=34, instances=4107, map_nodes=179,
                                   ai_nodes=180, placed_col=252, scenes=578, placements=3855, col_only=252, surfaces=4091,
                                   materials=81, vertices=239106, indices=411768, rgba_bytes=2760960, images=81,
                                   triggers=9, static_model_params=451, cuts=3)),
                       ('ta', dict(entries=383, instances=318, zones=9, portals=10, scenes=228, placements=284, col_only=34,
                                   surfaces=353, vertices=110196, indices=209514, rgba_bytes=4038656, images=29, cuts=3))):
        b = open(r'C:/Games/Star Wars Racer Revenge/TRACKS/%s.RES' % code.upper(), 'rb').read()
        f = facts(b, code, candidates=True)
        print(code, json.dumps({k: v for k, v in f.items() if k != 'archive_sha256'}))
        for k, v in want.items():
            print('   %-20s port %10s  ours %10s  %s' % (k, v, f.get(k), 'OK' if f.get(k) == v else 'DIFF'))
