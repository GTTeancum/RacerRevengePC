import os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import pod_export as PE, racer_assets as R, numpy as np
mid = int(sys.argv[1]); tg = int(sys.argv[2])
mm = PE.model_mesh(mid); mats = PE.model_materials(mid)
print('tris', len(mm.tris), 'bbox', mm.bbox())
grp = R.texgroup(tg)
for i, me in enumerate(mm.meshes):
    n = sum(1 for t in mm.tris if t[3] == i)
    tid = grp[me['tex']][0] if me['tex'] is not None and me['tex'] < len(grp) else None
    info = PE.texture_rgba(tid) if tid is not None else None
    print(i, n, 'pt', me['polytype'], 'pa', me['poly_alpha'], 'mat', mats.get(me['mat']), 'tex', tid,
          info and (info[1]['w'], info[1]['h'], info[1]['fmt'], int(info[0][..., 3].min()), int(info[0][..., 3].max())))
print('alphas', sorted(set(mm.alphas)))
