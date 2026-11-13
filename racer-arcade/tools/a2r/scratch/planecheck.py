# Compare stored face plane (normal, D) with the plane computed from the triangle's vertices.
import sys, os, numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import trk_res3, trk_col
def run(path, label, limit=60):
    r = trk_res3.read_res3(open(path,'rb').read())
    rows = []
    for n, lo, sz in r.members:
        if not (n.lower().startswith('geometry') and n.lower().endswith('.col')): continue
        b = trk_col.read_col(r.payload[lo:lo+sz]).body
        V = b.verts.astype(float)
        M = b.matrix.astype(float)
        for f in b.faces:
            nrm = f['normal'].astype(float); v = V[list(f['v'])]
            g = np.cross(v[1]-v[0], v[2]-v[0]); L = np.linalg.norm(g)
            if L == 0 or not np.isfinite(nrm).all(): continue
            g /= L
            rows.append((n, g @ nrm, f['d'], -(nrm @ v[0]), nrm @ v[0], nrm[1], g[1]))
    a = np.array([x[1:] for x in rows], float)
    print('==', label, len(rows), 'faces')
    print('  dot(stored normal, computed normal): +1 share %.3f, -1 share %.3f' % ((a[:,0] > 0.99).mean(), (a[:,0] < -0.99).mean()))
    print('  D == -n.v0 share %.3f, D == +n.v0 share %.3f' % ((abs(a[:,1]-a[:,2]) < 0.05).mean(), (abs(a[:,1]-a[:,3]) < 0.05).mean()))
    print('  stored normal y: up(>0.5) %.3f down(<-0.5) %.3f side %.3f' % ((a[:,4] > .5).mean(), (a[:,4] < -.5).mean(), (abs(a[:,4]) <= .5).mean()))
    print('  computed normal y: up %.3f down %.3f side %.3f' % ((a[:,5] > .5).mean(), (a[:,5] < -.5).mean(), (abs(a[:,5]) <= .5).mean()))
    names = sorted({x[0] for x in rows if 'wall' in x[0]})[:1]
    for nm in names:
        s = np.array([x[1:] for x in rows if x[0] == nm]); print('  ', nm, 'faces', len(s), 'stored-y up/down/side', (s[:,4]>.5).sum(), (s[:,4]<-.5).sum(), (abs(s[:,4])<=.5).sum())
run(sys.argv[1], 'converted'); run(sys.argv[2], 'original')
