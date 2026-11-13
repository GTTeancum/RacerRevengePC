r"""Encoder check on originals: decode every one-object / one-LOD PSG of a track (and POD01's wreck
pieces) to triangles, rebuild it from those triangles with geo_encode.build_psg (own stripifier,
quantiser, packer), then decode the rebuilt file with the repo's decoder
(tools\assets\decode_scene48c.py) and compare with the original decoded by the same decoder:
triangle sets with positions within one quantum, normals, UVs and colours exact.
python -B -I geo_encode_check.py [TRACK ...]"""
import os
import sys
import collections

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.dont_write_bytecode = True
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
from geo_res import read_res  # noqa: E402
import geo_psg  # noqa: E402
import geo_encode  # noqa: E402
from decode_scene48c import decode_psg3_scene  # noqa: E402
G = r'C:\Games\Star Wars Racer Revenge'


def tri_list(scene, q):
    """{non-position key (normals, uv, colours, cyclic order fixed): [position triples]}"""
    out = collections.defaultdict(list)
    for s in scene['lods'][0]['surfaces']:
        V = s['vertices_local']
        Nn = s['normals_local']
        for k in range(0, len(s['indices']), 3):
            ids = s['indices'][k:k + 3]
            vs = [(tuple(round(x, 4) for x in Nn[i]), (round(V[i][3], 5), round(V[i][4], 5)), tuple(V[i][5:9]),
                   np.array(V[i][:3])) for i in ids]
            if min(np.abs(vs[a][3] - vs[b][3]).max() for a, b in ((0, 1), (1, 2), (2, 0))) <= 2.02 * q:
                continue  # (near-)degenerate after quantisation: not compared
            r = min(range(3), key=lambda j: (vs[j][:3], tuple(np.round(vs[j][3] / q))))
            vs = vs[r:] + vs[:r]
            out[tuple(v[:3] for v in vs)].append(np.array([v[3] for v in vs]))
    return out


def same_tris(a, b, q):
    if set(a) != set(b):
        return False, 'attribute keys differ'
    for k in a:
        pool = list(b[k])
        if len(pool) != len(a[k]):
            return False, 'count differs'
        for p in a[k]:
            hit = [i for i, x in enumerate(pool) if np.abs(x - p).max() <= 1.01 * q + 1e-4]
            if not hit:
                return False, 'position beyond one quantum'
            pool.pop(hit[0])
    return True, ''


def main(tracks):
    C = collections.Counter()
    bad = []
    for t in tracks:
        path = os.path.join(G, 'TRACKS', t + '.RES') if len(t) == 2 else t
        m, _, _ = read_res(path)
        for name, b in m.items():
            if not name.endswith('.psg'):
                continue
            M = geo_psg.parse(b)
            if len(M['nodes']) != 1 or len(M['lods']) != 1 or len(M['materials']) != 1 or \
                    M['lods'][0]['surfaces'][0]['component'] != 0x00FF010F or \
                    any(g['wide'] for g in M['lods'][0]['surfaces'][0]['groups']):
                C['skipped_layout'] += 1
                continue
            sv = geo_psg.vertices(M)[0]
            q = M['quant']
            P = np.array(sv['pos'])
            Nn = np.array(sv['normal'])
            UV = np.array(sv['uv'])
            Cc = np.array(sv['rgba'], np.uint8)
            # build on the original quantisation origin (the encoder picks its own scale)
            blob, info = geo_encode.build_psg(M['nodes'][0]['name'], M['materials'][0], P, Nn, UV, Cc, sv['tris'],
                                              world=bool(M['world']),
                                              origin=q[4:7] if M['world'] else None)
            a = decode_psg3_scene(b)
            bq = geo_psg.parse(blob)['quant'][0]
            try:
                r = decode_psg3_scene(blob)
            except Exception as e:  # noqa
                C['rebuilt_rejected'] += 1
                bad.append((name, str(e)))
                continue
            qq = max(q[0], bq)
            same, why = same_tris(tri_list(a, qq), tri_list(r, qq), qq)
            C['same_triangles' if same else 'different'] += 1
            C['bytes_original'] += len(b)
            C['bytes_rebuilt'] += len(blob)
            if not same and len(bad) < 6:
                bad.append((name, why))
    print(dict(C))
    for x in bad:
        print('  ', x)
    return C


if __name__ == '__main__':
    main(sys.argv[1:] or ['TA', 'GC'])
