r"""Multi-node / multi-LOD / multi-surface PSG v3 encoder for converted pods (a2r).

geo_encode.build_psg writes one node, one LOD, one material (course pieces). Pod models need the
layout of the original pod PSGs (measured on PODS\PODnn\*.psg, see notes\pod-convert.md):
  * worldFlag 0, quantisation origin 0, one posScale for the whole file (largest |local| / 32767);
  * normalScale 1/127, uScale = vScale = 1/2047 (same constants as the course files);
  * componentWord taken from the target original (pods: 0x00000107 = position, normal, uv; no colour);
  * vertices carry their node in the ADC word (+-16*(node+1)); positions are node-local;
  * every node's stored bounds = box of the decoded vertices bound to it over ALL LODs
    (tools\assets\decode_scene48c.py checks exactly that), zero box for nodes without vertices;
  * matrix palette trailer = identity ids (geo_psg.write default) when nodeCount > 1.
Strips: geo_encode.stripify (max 15, no continuation packets), groups of <= 16 (geo_encode.pack).
"""
import math
import struct

import numpy as np

import geo_encode
import geo_psg

F32 = geo_encode.F32
IDENT = [1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1.]


def build(nodes, lods, component, materials=None, extra_materials=()):
    """nodes: list of dict(name, matrix (16 floats, row vectors), parent).
    lods: list of dict(cutoff, surfaces=[dict(material (name), P (n,3) node-local, N (n,3), UV (n,2),
          bone (n,), tris (m,3))]). Triangles must already be oriented so cross(b-a, c-a) follows the
          vertex normals. Returns (bytes, info)."""
    colour = (component & 0xFF) == 0x0F
    assert not colour, 'pod surfaces carry no vertex colour'
    allP = [s['P'][np.unique(np.asarray(s['tris']).reshape(-1))] for L in lods for s in L['surfaces'] if len(s['tris'])]
    ext = max(float(np.abs(p).max()) for p in allP)
    scale = F32(max(ext, 1e-6) / 32767.0)
    while max(float(np.abs(np.round(p / scale)).max()) for p in allP) > 32767:
        scale = F32(scale * 1.0000001 + 1e-12)
    mats = []
    out_lods = []
    node_pts = [[] for _ in nodes]
    stats = dict(triangles=0, dropped_turned_slivers=0, packed_vertices=0)
    for L in lods:
        surfs = []
        for s in L['surfaces']:
            tris = np.asarray(s['tris'], int).reshape(-1, 3)
            if not len(tris):
                continue
            P = np.asarray(s['P'], float); N = np.asarray(s['N'], float); UV = np.asarray(s['UV'], float)
            B = np.asarray(s['bone'], int)
            Q = np.round(P / scale).astype(int)
            nl = np.linalg.norm(N, axis=1, keepdims=True); nl[nl == 0] = 1
            NQ = np.clip(np.round(N / nl * 127), -127, 127).astype(int)
            UQ = np.round(UV / geo_encode.UV_SCALE).astype(int)
            used = np.unique(tris)
            if np.abs(UQ[used]).max() > 32767:
                raise ValueError('UV outside +-16')
            key, vid, attrs = {}, np.empty(len(P), int), []
            for i in used:
                k = (tuple(Q[i]), tuple(NQ[i]), tuple(UQ[i]), int(B[i]))
                if k not in key:
                    key[k] = len(attrs); attrs.append(k)
                vid[i] = key[k]
            keep = []
            for t in tris:
                q = Q[t] * scale
                f = P[t]
                if np.cross(f[1] - f[0], f[2] - f[0]) @ N[t].sum(0) > 0 and \
                        np.cross(q[1] - q[0], q[2] - q[0]) @ NQ[t].sum(0) < 0:
                    stats['dropped_turned_slivers'] += 1
                    continue
                if len({tuple(Q[i]) for i in t}) < 3:
                    stats['dropped_turned_slivers'] += 1   # collapsed to a line by quantisation
                    continue
                keep.append(t)
            t2 = [tuple(vid[t]) for t in keep]
            strips = geo_encode.stripify(t2)
            canon = lambda t: min((t[0], t[1], t[2]), (t[1], t[2], t[0]), (t[2], t[0], t[1]))  # noqa: E731
            assert sorted(canon(t) for st in strips for t in geo_encode.strip_triangles(st)) == sorted(canon(t) for t in t2)
            sa = []
            for st in strips:
                row = []
                for j, v in enumerate(st):
                    q, n, uv, b = attrs[v]
                    w = 16 * (b + 1)
                    row.append(((q[0], q[1], q[2], -w if j < 2 else w), n, uv, None))
                sa.append(row)
            groups = geo_encode.pack(sa)
            nv = sum(len(g) for g in groups)
            assert nv <= 65535
            for q, n, uv, b in attrs:
                node_pts[b].append(np.array(q, float) * scale)
            mats.append(s['material'])
            Pk = P[np.unique(np.asarray(keep).reshape(-1))] if keep else P[:1]
            surfs.append(dict(material=len(mats) - 1, component=component,
                              uv_size=F32(geo_encode.uv_size(P, UV, [tuple(t) for t in keep]) if keep else 1.0),
                              vertex_count=nv, strip_count=len(strips), tail_pad=None,
                              groups=[dict(wide=False, xyzw=[v[0] for v in g], normal=[v[1] for v in g],
                                           uv=[v[2] for v in g], rgba=None) for g in groups]))
            stats['triangles'] += len(keep)
            stats['packed_vertices'] += nv
        out_lods.append(dict(cutoff=L['cutoff'], surfaces=surfs))
    out_nodes = []
    for i, nd in enumerate(nodes):
        pts = node_pts[i]
        if pts:
            pts = np.array(pts)
            # stored the way the decoder recomputes: decoded float32 positions
            pts = pts.astype(np.float32).astype(float)
            lo, hi = pts.min(0), pts.max(0)
            c, h = (lo + hi) / 2, (hi - lo) / 2
        else:
            c = h = np.zeros(3)
        mtx = [F32(x) for x in nd['matrix']]
        mtx[3] = mtx[7] = mtx[11] = 0.0      # affine (decoder: m[3] = m[7] = m[11] = 0, m[15] = 1)
        mtx[15] = 1.0
        out_nodes.append(dict(name=nd['name'], name_raw=None, matrix=mtx,
                              centre=[F32(x) for x in c], half=[F32(x) for x in h],
                              radius=F32(float(np.linalg.norm(h))), parent=int(nd['parent'])))
    # extra_materials: unreferenced material-table entries appended after every surface's own entry (the
    # target's table layout: e.g. a stock engine lists its flame material once per LOD although its last LOD
    # has no flame surface; the port's flame owner counts table entries, srcssets\pod_flame50w.cpp:70)
    mats += list(extra_materials)
    model = dict(nodes=out_nodes, materials=mats, materials_raw=[None] * len(mats),
                 quant=[scale, geo_encode.NORMAL_SCALE, geo_encode.UV_SCALE, geo_encode.UV_SCALE, 0.0, 0.0, 0.0],
                 world=0, lods=out_lods, palette=None)
    blob = geo_psg.write(model)
    assert geo_psg.write(geo_psg.parse(blob)) == blob
    stats.update(bytes=len(blob), nodes=len(nodes), quantum_ft=scale, materials=mats,
                 lods=[len(L['surfaces']) for L in out_lods])
    return blob, stats


def rows(nodes):
    """Motion binding rows (original 152EA0 enumeration: siblings in source order, then each
    sibling's child group; model_bindings49o.h)."""
    kids = {}
    for i, n in enumerate(nodes):
        kids.setdefault(n['parent'], []).append(i)
    out = []

    def group(g):
        out.extend(g)
        for i in g:
            if i in kids:
                group(kids[i])
    group(kids[-1])
    return out


def world_matrices(nodes):
    W = []
    for nd in nodes:
        m = np.array(nd['matrix'], float).reshape(4, 4)
        W.append(m if nd['parent'] < 0 else m @ W[nd['parent']])
    return W
