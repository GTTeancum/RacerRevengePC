import sys, os
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg, trk_col
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
n = sys.argv[1]
m, order, info = read_res(R % (n, n))
for name, lo, nb in order:
    l = name.lower()
    if l.endswith('.col'):
        c = trk_col.read_col(m[l]); b = c.body
        print(name, 'type', c.type, 'mat', b.matrix.round(2).tolist(), 'nv', len(b.verts), 'nf', len(b.faces), 'normals', b.normals is not None, 'q', b.quantized, 'bbox', b.verts.min(0).round(2), b.verts.max(0).round(2))
c = geo_psg.parse(m[r'pods\pod%s\chariot.psg' % n])
print('pal', c['palette']['count'], c['palette']['bytes'][:40], c['palette']['n'])
for L in c['lods']:
    print([(hex(s['component']), round(s['uv_size'],3), s['vertex_count'], s['strip_count'], max(len(g['xyzw']) for g in s['groups'])) for s in L['surfaces']])
