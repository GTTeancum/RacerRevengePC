r"""Mesh -> PSG v3 encoder (one static object, one LOD, one material per file), for a2r.

Conventions measured on the original Revenge files (tools\a2r\geo_roundtrip.py, scratch\pack.py,
notes\a2r-geometry.md):
  * world-space pieces: worldFlag 1, quantisation origin = piece centre; local props: worldFlag 0,
    origin 0. posScale = largest |p - origin| / 32767 (originals hit 32767 / 32766 exactly).
  * normalScale = 1/127 (f32 3C010204), uScale = vScale = 1/2047 (f32 3A001002): normals are s8
    round(n * 127), UVs s16 round(u * 2047) (so |u| <= 16).
  * componentWord 00FF010F (position, normal, uv, colour), one LOD with cutoff FLT_MAX (7F7FFFFF).
  * strips packed greedily into groups of <= 16 vertices; a strip that does not fit the room left
    starts a new group; a strip longer than 16 is split, the next group repeating its last two
    vertices with the ADC (-16) flag. Header vertexCount counts every packed vertex, stripCount
    counts source strips. This rule reproduces every surface of TA, GC, MC, RA and POD01 byte for
    byte (scratch\pack.py).
  * uvSize (used by the texture-demand code, original_standard_material_object50la.cpp:1473):
    the originals track the largest world-length / uv-length ratio of an edge within ~5 %; this
    encoder writes that maximum (generated, conservative: asks for at least the original detail).
"""
import math
import struct

import numpy as np

import geo_psg

F32 = lambda x: struct.unpack('<f', struct.pack('<f', x))[0]  # noqa: E731
NORMAL_SCALE = F32(1 / 127)
UV_SCALE = F32(1 / 2047)
FLT_MAX = struct.unpack('<f', struct.pack('<I', 0x7F7FFFFF))[0]
COMPONENT = 0x00FF010F
GROUP_CAP = 16
assert struct.pack('<f', NORMAL_SCALE).hex() == '0402013c' and struct.pack('<f', UV_SCALE).hex() == '0210003a'


# ---------------------------------------------------------------- strips
def stripify(tris, max_len=GROUP_CAP - 1):
    """max_len 15 (default): no strip needs a packet continuation, so the files stay inside the
    port's strict 48B decoder (import_decode48c.cpp:510 'psg.strip_count_variant'); and no strip is
    exactly 16 long, so a following strip that happens to share its last edge can never look like a
    continuation to tools/assets/decode_scene48c.py (the C++ scene decoder,
    import_scene48d.cpp:215-224, resolves that case by the header count anyway)."""
    return _stripify(tris, max_len)


def _stripify(tris, max_len):
    """tris: list of (a, b, c) vertex ids, each oriented as it must decode. Returns a list of
    strips (vertex id lists) whose decoded triangles (k even: s[k], s[k+1], s[k+2]; k odd:
    s[k+1], s[k], s[k+2]) reproduce every input triangle with its cyclic orientation."""
    edge = {}
    for t, (a, b, c) in enumerate(tris):
        for u, v in ((a, b), (b, c), (c, a)):
            edge.setdefault((u, v), []).append(t)
    used = [False] * len(tris)

    def third(t, u, v):
        a, b, c = tris[t]
        for x, y, z in ((a, b, c), (b, c, a), (c, a, b)):
            if x == u and y == v:
                return z
        return None

    def grow(strip, mark):
        taken = []
        while len(strip) < max_len:
            # next triangle j = len-2 is (s[j], s[j+1], new) for even j, (s[j+1], s[j], new) for
            # odd j: it must contain that directed edge
            k = len(strip) - 2
            u, v = (strip[k], strip[k + 1]) if k % 2 == 0 else (strip[k + 1], strip[k])
            nxt = None
            for t in edge.get((u, v), ()):
                if not used[t] and t not in taken:
                    w = third(t, u, v)
                    if w is not None:
                        nxt = (t, w)
                        break
            if nxt is None:
                return taken
            taken.append(nxt[0])
            strip.append(nxt[1])
            if mark:
                used[nxt[0]] = True
        return taken

    strips = []
    for t0 in range(len(tris)):
        if used[t0]:
            continue
        a, b, c = tris[t0]
        best = None
        for rot in ((a, b, c), (b, c, a), (c, a, b)):
            used[t0] = True
            trial = list(rot)
            taken = grow(trial, False)
            used[t0] = False
            if best is None or len(taken) > len(best[1]):
                best = (rot, taken)
        used[t0] = True
        s = list(best[0])
        grow(s, True)
        strips.append(s)
    return strips


def strip_triangles(s):
    out = []
    for k in range(len(s) - 2):
        out.append((s[k + 1], s[k], s[k + 2]) if k & 1 else (s[k], s[k + 1], s[k + 2]))
    return out


def pack(strips_attr):
    """strips_attr: list of strips, each a list of vertex attribute tuples (xyzw, n, uv, rgba) with
    w = -16 for the first two and +16 for the rest. Returns groups (lists of tuples)."""
    groups, cur = [], []
    for st in strips_attr:
        st = list(st)
        while True:
            room = GROUP_CAP - len(cur)
            if len(st) <= room:
                cur += st
                break
            if cur:
                groups.append(cur)
                cur = []
                continue
            groups.append(st[:GROUP_CAP])
            st = [((v[0][0], v[0][1], v[0][2], -abs(v[0][3])),) + v[1:] for v in st[GROUP_CAP - 2:GROUP_CAP]] + st[GROUP_CAP:]
    if cur:
        groups.append(cur)
    return groups


# ---------------------------------------------------------------- PSG
def uv_size(P, UV, tris):
    best = 0.0
    for a, b, c in tris:
        for u, v in ((a, b), (b, c), (c, a)):
            l2 = math.hypot(UV[u][0] - UV[v][0], UV[u][1] - UV[v][1])
            if l2 > 1e-9:
                best = max(best, float(np.linalg.norm(P[u] - P[v])) / l2)
    return best if best > 0 else 1.0


def build_psg(name, material, P, N, UV, C, tris, world=True, origin=None, lattice=None):
    """One static PSG: node `name`, one material. lattice: fixed position step (a power of two in
    ft) with the origin snapped to it, so pieces of one course quantise shared vertices identically. P (n,3) float, N (n,3) unit normals, UV (n,2)
    normalised, C (n,4) uint8 RGBA (Revenge prelit range), tris (m,3) indices, oriented so that
    cross(b-a, c-a) points along the vertex normals. Returns (bytes, info)."""
    P = np.asarray(P, float)
    N = np.asarray(N, float)
    UV = np.asarray(UV, float)
    C = np.asarray(C, np.uint8)
    tris = np.asarray(tris, int).reshape(-1, 3)
    used = np.unique(tris)
    lo, hi = P[used].min(0), P[used].max(0)
    centre = (lo + hi) / 2
    o = centre if (world and origin is None) else (np.zeros(3) if origin is None else np.asarray(origin, float))
    if lattice:
        # shared lattice: origin snapped to the step, step doubled only if the piece is too tall
        o = np.round(o / lattice) * lattice
    o = np.array([F32(x) for x in o])
    ext = np.abs(P[used] - o).max()
    scale = F32(max(ext, 1e-6) / 32767.0)
    if lattice:
        scale = F32(lattice)
        while ext / scale > 32767:
            scale = F32(scale * 2)
    while np.abs(np.round((P[used] - o) / scale)).max() > 32767:
        scale = F32(scale * 1.0000001 + 1e-12)
    Q = np.round((P - o) / scale).astype(int)
    NQ = np.clip(np.round(N * 127), -127, 127).astype(int)
    UQ = np.round(UV / UV_SCALE).astype(int)
    if np.abs(UQ[used]).max() > 32767:
        raise ValueError('%s: UV outside +-16 after shifting' % name)
    # vertex identity = the packed attribute tuple (exact duplicates merge, like an indexed mesh)
    key = {}
    vid = np.empty(len(P), int)
    attrs = []
    for i in used:
        k = (tuple(Q[i]), tuple(NQ[i]), tuple(UQ[i]), tuple(int(x) for x in C[i]))
        if k not in key:
            key[k] = len(attrs)
            attrs.append(k)
        vid[i] = key[k]
    # a sliver narrower than one position step can turn over when its corners are rounded: drop
    # those (they cover less than a step; the strict decoders refuse a triangle whose winding
    # disagrees with its stored normals)
    keep, flipped = [], 0
    for t in tris:
        q = Q[t] * scale
        f = P[t]
        before = np.cross(f[1] - f[0], f[2] - f[0]) @ N[t].sum(0)
        if before > 0 and np.cross(q[1] - q[0], q[2] - q[0]) @ (NQ[t].sum(0)) < 0:
            flipped += 1
            continue
        keep.append(t)
    tris = np.array(keep, int).reshape(-1, 3)
    t2 = [tuple(vid[t]) for t in tris]
    strips = stripify(t2)
    # self-check: decoded strips == input triangles (cyclic orientation kept)
    def canon(t):
        a, b, c = t
        r = min((a, b, c), (b, c, a), (c, a, b))
        return r
    got = sorted(canon(t) for s in strips for t in strip_triangles(s))
    want = sorted(canon(t) for t in t2)
    assert got == want, 'stripifier lost or reoriented triangles'
    sa = []
    for s in strips:
        st = []
        for j, v in enumerate(s):
            q, n, uv, rgba = attrs[v]
            st.append(((q[0], q[1], q[2], -16 if j < 2 else 16), n, uv, rgba))
        sa.append(st)
    groups = pack(sa)
    nv = sum(len(g) for g in groups)
    if nv > 65535:
        raise ValueError('%s: %d packed vertices > 65535' % (name, nv))
    half = (hi - lo) / 2
    model = dict(
        nodes=[dict(name=name, name_raw=None, matrix=[1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1.],
                    centre=[F32(x) for x in centre], half=[F32(x) for x in half],
                    radius=F32(float(np.linalg.norm(half))), parent=-1)],
        materials=[material], materials_raw=[None],
        quant=[scale, NORMAL_SCALE, UV_SCALE, UV_SCALE, o[0], o[1], o[2]], world=1 if world else 0,
        lods=[dict(cutoff=FLT_MAX, surfaces=[dict(
            material=0, component=COMPONENT, uv_size=F32(uv_size(P, UV, tris)), vertex_count=nv,
            strip_count=len(strips), tail_pad=None,
            groups=[dict(wide=False, xyzw=[v[0] for v in g], normal=[v[1] for v in g], uv=[v[2] for v in g],
                         rgba=[v[3] for v in g]) for g in groups])])],
        palette=None)
    blob = geo_psg.write(model)
    return blob, dict(name=name, material=material, triangles=len(tris), dropped_turned_slivers=flipped, unique_vertices=len(attrs),
                      packed_vertices=nv, strips=len(strips), groups=len(groups), quantum_ft=scale,
                      bytes=len(blob), bounds_min=lo.tolist(), bounds_max=hi.tolist())
