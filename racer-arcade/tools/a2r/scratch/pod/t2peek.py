import sys, os, struct
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import trk_col
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
n = sys.argv[1]
m, order, info = read_res(R % (n, n))
for name in sys.argv[2:]:
    b = m[(r'pods\pod%s\%s' % (n, name)).lower()]
    ty = struct.unpack_from('<I', b, 12)[0]
    if ty == 2:
        M = np.frombuffer(b, '<f4', 16, 16).reshape(4,4)
        nr, nn, nl = struct.unpack_from('<3I', b, 80)
        L = np.frombuffer(b, '<f4', nr*6, 92+32*nn).reshape(nr, 2, 3)
        nodes = [struct.unpack_from('<7fHH', b, 92+32*i) for i in range(nn)]
        print(name, 'M', M.round(3).tolist(), 'rays', nr)
        for r in L: print('   ', r[0].round(2), r[1].round(2))
        print('   node0', np.round(nodes[0][:7],2), nodes[0][7:], 'node1', np.round(nodes[1][:7],2), [hex(x) for x in nodes[1][7:]])
    else:
        c = trk_col.read_col(b); bd = c.body
        print(name, 'type0 mat', bd.matrix.round(2).tolist(), 'nv', len(bd.verts), 'nf', len(bd.faces), 'norm', bd.normals is not None)
        print('   verts', bd.verts.round(2).tolist())
        print('   faces', bd.faces['v'].tolist(), bd.faces['normal'].round(2).tolist()[:3])
        print('   nodes', [(round(float(x['volume']),1), x['centre'].round(2).tolist(), x['half'].round(2).tolist(), [hex(c) for c in x['child']]) for x in bd.nodes][:4])
