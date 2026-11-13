r"""Handedness evidence from the arcade's own left/right-named pilot motions.

The arcade names pilot motions L_/R_ (GMOT_ANA_L_chira = glance left, R_chira = glance right,
L_/R_contact = hit from the left/right, L_/R_crash). The pod faces -z (pod-placement.md: cockpit at
+z behind the engines, centre line leaves the start heading -z), y is up. If the "left" motions turn
the head / move the body toward -x, then left = -x while facing -z with y up, i.e. +x is to the
right: a RIGHT-handed frame (OpenGL-like; Odin renders through glFrustum, eye looking down -z).
Revenge is left-handed (right = Up x Look, src\assets\import_collision48g.h:81), so one axis must be
negated on conversion.

Reads export\pods\<CODE>\<CODE>_pilot_player.gltf (or _pilot.gltf), samples node world matrices.
python -B -I geo_handedness.py [CODE ...]
"""
import json
import os
import struct
import sys

import numpy as np

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


def load(path):
    G = json.load(open(path))
    binp = os.path.join(os.path.dirname(path), G['buffers'][0]['uri'])
    B = open(binp, 'rb').read()

    def acc(i):
        a = G['accessors'][i]
        bv = G['bufferViews'][a['bufferView']]
        n = {'SCALAR': 1, 'VEC2': 2, 'VEC3': 3, 'VEC4': 4, 'MAT4': 16}[a['type']]
        off = bv.get('byteOffset', 0) + a.get('byteOffset', 0)
        return np.frombuffer(B, '<f4', count=a['count'] * n, offset=off).reshape(a['count'], n)
    return G, acc


def quat_mat(q):
    x, y, z, w = q
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def world(G, trs):
    n = len(G['nodes'])
    parent = [-1] * n
    for i, nd in enumerate(G['nodes']):
        for c in nd.get('children', []):
            parent[c] = i
    W = [None] * n

    def get(i):
        if W[i] is None:
            t, r, s = trs[i]
            M = np.eye(4)
            M[:3, :3] = quat_mat(r) * s
            M[:3, 3] = t
            W[i] = M if parent[i] < 0 else get(parent[i]) @ M
        return W[i]
    for i in range(n):
        get(i)
    return W


def bind_trs(G):
    out = []
    for nd in G['nodes']:
        out.append((np.array(nd.get('translation', [0, 0, 0]), float), np.array(nd.get('rotation', [0, 0, 0, 1]), float),
                    np.array(nd.get('scale', [1, 1, 1]), float)))
    return out


def sample(G, acc, anim, frac):
    trs = [list(x) for x in bind_trs(G)]
    for ch in anim['channels']:
        smp = anim['samplers'][ch['sampler']]
        t = acc(smp['input'])[:, 0]
        v = acc(smp['output'])
        k = int(round(frac * (len(t) - 1)))
        node, path = ch['target']['node'], ch['target']['path']
        idx = {'translation': 0, 'rotation': 1, 'scale': 2}[path]
        trs[node][idx] = v[k].astype(float)
    return world(G, trs)


def main(codes):
    rows = []
    for code in codes:
        d = os.path.join(ROOT, 'export', 'pods', code)
        p = os.path.join(d, '%s_pilot_player.gltf' % code)
        if not os.path.exists(p):
            p = os.path.join(d, '%s_pilot.gltf' % code)
        if not os.path.exists(p):
            continue
        G, acc = load(p)
        Wb = world(G, bind_trs(G))
        pos = np.array([w[:3, 3] for w in Wb[:-1]])
        head = int(np.argmax(pos[:, 1]))  # highest joint in the bind pose
        anims = {a['name']: a for a in G['animations']}
        for name, a in anims.items():
            tag = name.replace('GMOT_', '').replace('PATCH_', '').replace('Y_', '')
            parts = tag.split('_')
            side = parts[1] if len(parts) > 1 else ''
            if side not in ('L', 'R'):
                continue
            best = None
            for frac in np.linspace(0, 1, 61):
                W = sample(G, acc, a, frac)
                d = np.array([W[i][:3, 3] - Wb[i][:3, 3] for i in range(len(Wb) - 1)])
                k = int(np.argmax(np.abs(d[:, 0])))  # joint with the largest sideways move
                cand = (abs(d[k, 0]), frac, k, d[k, 0], float(d[:, 0].mean()))
                if best is None or cand[0] > best[0]:
                    best = cand
            rows.append((code, name, side, best[1], best[2], best[3], best[4]))
    print('code motion side peak_frac joint largest_joint_dx body_mean_dx   (dx < 0 = toward -x)')
    for r in rows:
        print('%s %-34s %s %.2f %2d %+7.3f %+7.3f' % r)
    agree = sum(1 for r in rows if abs(r[5]) > 1e-3 and ((r[2] == 'L') == (r[5] < 0)))
    moving = sum(1 for r in rows if abs(r[5]) > 1e-3)
    print('motions that move sideways: %d; "L" toward -x / "R" toward +x: %d' % (moving, agree))
    return rows


if __name__ == '__main__':
    main(sys.argv[1:] or ['ANA', 'BEN', 'GAS', 'SEB'])
