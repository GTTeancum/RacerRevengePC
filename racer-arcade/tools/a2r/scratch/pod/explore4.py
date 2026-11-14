import sys, os
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
n = sys.argv[1]
m, order, info = read_res(R % (n, n))
for part in sys.argv[2:]:
    c = geo_psg.parse(m[r'pods\pod%s\%s.psg' % (n, part)])
    print('==', part)
    for i, nd in enumerate(c['nodes']):
        M = np.array(nd['matrix']).reshape(4,4)
        print('%2d %-20s p%2d T=%s rotdiag=%s c=%s h=%s r=%.2f' % (i, nd['name'], nd['parent'], np.round(M[3,:3],3), np.round(np.diag(M)[:3],3), np.round(nd['centre'],2), np.round(nd['half'],2), nd['radius']))
    for li, L in enumerate(c['lods']):
        vs = geo_psg.vertices(dict(c, lods=c['lods']), li)
        for s in vs:
            b = np.bincount(s['bone'], minlength=len(c['nodes']))
            P = np.array(s['pos'])
            print('  lod', li, s['material'], 'verts', len(P), 'tris', len(s['tris']), 'bones', {c['nodes'][k]['name']+str(k): int(v) for k, v in enumerate(b) if v}, 'rgba', s['rgba'][0], 'bbox', np.round(P.min(0),2), np.round(P.max(0),2))
