# Wall collision diagnosis: facing (normal vs. direction to track centre) and height, converted vs original.
import sys, os, numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import trk_res3, trk_col
def members(path):
    r = trk_res3.read_res3(open(path,'rb').read())
    return {n: r.payload[lo:lo+sz] for n, lo, sz in r.members}
def report(path, label):
    m = members(path)
    cols = {n: trk_col.read_col(d) for n, d in m.items() if n.lower().endswith('.col') and n.lower().startswith('geometry')}
    floorV = np.vstack([c.body.verts.astype(float) for n, c in cols.items() if 'floor' in n.lower()] or [np.zeros((1,3))])
    cen_xz = floorV[:, [0, 2]]
    print('==', label, len(cols), 'col files; names sample:', list(cols)[:6])
    tot = dict(n=0, inward=0, vert=0)
    hts = []
    for n, c in cols.items():
        if 'floor' in n.lower(): continue
        b = c.body; V = b.verts.astype(float); F = b.faces
        for f in F:
            nrm = f['normal'].astype(float)
            if not np.isfinite(nrm).all(): continue
            tri = V[list(f['v'])]; ctr = tri.mean(0)
            if abs(nrm[1]) > 0.5: continue  # not wall-like
            tot['vert'] += 1
            # nearest floor vertex = track side
            d = cen_xz - ctr[[0, 2]]; k = np.argmin((d**2).sum(1)); to = np.array([d[k,0], 0, d[k,1]])
            tot['inward'] += (nrm @ to) > 0
            hts.append(tri[:,1].max() - floorV[k,1])
    print('  wall-like faces', tot['vert'], 'facing the floor side', tot['inward'],
          'height above nearest floor ft: median %.1f p10 %.1f p90 %.1f' % tuple(np.percentile(hts, [50,10,90])) if hts else '')
report(sys.argv[1], 'converted'); report(sys.argv[2], 'original')
