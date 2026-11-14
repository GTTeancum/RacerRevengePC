import sys, os
import numpy as np
T = r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools'
sys.path.insert(0, T)
import pod_export as PE, racer_assets as R
for code in sys.argv[1:]:
    kind = R.CODES.index(code)
    pd = R.pod(kind); pl = PE.npc_placement(kind)
    body = PE.mrow(pl['body']['offset'], 0)
    stats = {}
    for lod in range(4):
        tg = R.texgroup(pd['tg_slot0'][0] if lod == 0 else pd['tg_other'][0])
        parts = PE.pod_parts(kind, lod, tg)
        if lod < 2:
            pp, p, o = PE.pilot_parts(kind, 0 if lod == 0 else 1, body); parts += pp
        for q in parts:
            mm = q['mesh']; tgq = q['texgroup']
            grp = q['name'].split('_')[0]
            for a, b, c, mi, fl in mm.tris:
                ti = mm.meshes[mi]['tex']
                tid = tgq[ti][0] if ti is not None and ti < len(tgq) else None
                k = (grp, tid)
                s = stats.setdefault(k, dict(n=0, lo=np.array([1e9,1e9]), hi=-np.array([1e9,1e9]), mats=set(), fl=0, poly=set()))
                s['n'] += 1; s['fl'] += fl; s['poly'].add(mm.meshes[mi]['polytype'])
                s['mats'].add(mm.meshes[mi]['mat'])
                if tid is not None:
                    img, info = PE.texture_rgba(tid)
                    sc = (16.0 if info['uv_texels'] else 1.0) / np.array([info['w'], info['h']])
                    uv = np.array([mm.uvs[i] for i in (a, b, c)]) * sc
                    s['lo'] = np.minimum(s['lo'], uv.min(0)); s['hi'] = np.maximum(s['hi'], uv.max(0)); s['info'] = info
    area = {}
    for (g, tid), s in sorted(stats.items(), key=lambda kv: str(kv[0])):
        inf = s.get('info')
        if inf: area[g] = area.get(g, 0) + inf['w'] * inf['h']
        print(code, g, tid, s['n'], 'uv', np.round(s['lo'],2), np.round(s['hi'],2), 'mats', s['mats'], 'nocull', s['fl'], 'poly', s['poly'],
              (inf['w'], inf['h'], 'rep', inf['repeat_u'], inf['repeat_v'], 'mir', inf['mirror_u'], inf['mirror_v'], 'alpha', inf['alpha'], inf['semi']) if inf else '')
    print(code, 'texel area per group', area)
