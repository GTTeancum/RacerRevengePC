r"""Scale measurement 1: pod size. Arcade ANA (export\pods\ANA\ANA_pod_lod0_tg2.obj, parts placed by
the game's own offsets) against Revenge POD01 (Young Anakin, PODS\POD01\POD01.RES).
python -B -I geo_scale_pod.py"""
import sys, os, math
import numpy as np
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from geo_res import read_res
import geo_psg

ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))


def obj_parts(path):
    parts, cur, V = {}, None, []
    idx = {}
    for ln in open(path):
        if ln.startswith('o '):
            cur = ln.split()[1]
            parts[cur] = set()
        elif ln.startswith('v '):
            V.append([float(x) for x in ln.split()[1:4]])
        elif ln.startswith('f ') and cur:
            for t in ln.split()[1:]:
                parts[cur].add(int(t.split('/')[0]) - 1)
    V = np.array(V)
    return {k: V[sorted(v)] for k, v in parts.items()}


def bind_points(M):
    nodes = M['nodes']
    W = []
    for i, nd in enumerate(nodes):
        m = np.array(nd['matrix']).reshape(4, 4)
        if nd['parent'] >= 0:
            m = m @ W[nd['parent']]
        W.append(m)
    pts = []
    for s in geo_psg.vertices(M):
        for p, b in zip(s['pos'], s['bone']):
            pts.append((np.r_[p, 1.0] @ W[b])[:3])
    return np.array(pts), W


def box(p):
    return p.min(0), p.max(0)


def main():
    A = obj_parts(os.path.join(ROOT, 'export', 'pods', 'ANA', 'ANA_pod_lod0_tg2.obj'))
    ck = [v for k, v in A.items() if k.startswith('cockpit')][0]
    e0 = [v for k, v in A.items() if k.startswith('engine0')][0]
    e1 = [v for k, v in A.items() if k.startswith('engine1')][0]
    a = {}
    a['cockpit_size'] = ck.max(0) - ck.min(0)
    a['engine_size'] = e0.max(0) - e0.min(0)
    a['cockpit_centre'] = (ck.max(0) + ck.min(0)) / 2
    a['engine_centres'] = [(e.max(0) + e.min(0)) / 2 for e in (e0, e1)]
    a['engine_gap_x'] = abs(a['engine_centres'][1][0] - a['engine_centres'][0][0])
    allp = np.vstack([ck, e0, e1])
    a['overall'] = allp.max(0) - allp.min(0)
    ec = (a['engine_centres'][0] + a['engine_centres'][1]) / 2
    a['engine_to_cockpit'] = float(np.linalg.norm(a['cockpit_centre'] - ec))
    a['cockpit_radius_box_halfdiag'] = float(np.linalg.norm(a['cockpit_size']) / 2)

    m, _, _ = read_res(r'C:\Games\Star Wars Racer Revenge\PODS\POD01\POD01.RES')
    r = {}
    ch, W = bind_points(geo_psg.parse(m[r'pods\pod01\chariot.psg']))
    el, _ = bind_points(geo_psg.parse(m[r'pods\pod01\englt1.psg']))
    er, _ = bind_points(geo_psg.parse(m[r'pods\pod01\engrt1.psg']))
    r['chariot_size'] = ch.max(0) - ch.min(0)
    r['engine_size'] = el.max(0) - el.min(0)
    r['chariot_radius_box_halfdiag'] = float(np.linalg.norm(r['chariot_size']) / 2)
    # engines are separate models positioned by the PHY (LeftEnginePosition etc.)
    phy = m[r'pods\pod01\pod01.phy'].decode('latin1')
    keys = {}
    for ln in phy.splitlines():
        if '=' in ln:
            k, v = ln.split('=', 1)
            keys[k.strip()] = v.split(';')[0].strip()
    r['phy_engine_keys'] = {k: v for k, v in keys.items() if 'Position' in k or 'Offset' in k or 'Length' in k}
    print('ARCADE ANA (units):')
    for k, v in a.items():
        print('  %-28s %s' % (k, np.round(v, 3).tolist() if hasattr(v, '__len__') else round(v, 3)))
    print('REVENGE POD01 (ft):')
    for k, v in r.items():
        print('  %-28s %s' % (k, np.round(v, 3).tolist() if isinstance(v, np.ndarray) else v))
    rat = dict(
        chariot_length=r['chariot_size'][2] / a['cockpit_size'][2],
        chariot_width=r['chariot_size'][0] / a['cockpit_size'][0],
        chariot_height=r['chariot_size'][1] / a['cockpit_size'][1],
        engine_length=r['engine_size'][2] / a['engine_size'][2],
        engine_width=r['engine_size'][0] / a['engine_size'][0],
        engine_height=r['engine_size'][1] / a['engine_size'][1],
        chariot_halfdiag=r['chariot_radius_box_halfdiag'] / a['cockpit_radius_box_halfdiag'],
        radius_6p52_vs_arcade_halfdiag=6.515963554382324 / a['cockpit_radius_box_halfdiag'],
        rest_20p78_vs_arcade_engine_to_cockpit=20.78443717956543 / a['engine_to_cockpit'],
    )
    print('RATIOS Revenge/Arcade:')
    for k, v in rat.items():
        print('  %-40s %.3f' % (k, v))
    return a, r, rat


if __name__ == '__main__':
    main()
