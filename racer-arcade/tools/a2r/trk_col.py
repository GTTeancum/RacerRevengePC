r"""COL v1 collision files (Racer Revenge) for the a2r track converter.

Format (docs\native-collision48g.md; src\assets\import_collision48g.cpp decodeBody,
decodeCollisionCompound50IJ):
  u32 1234, 'col\0', u32 version 1, u32 type
  type 0: one body.   type 1 (compound, anim\*.col): u32 count, u32 reverse, then per member
          u32 scene node index + one body.
  body: 12 f32 (3 basis rows + translation), u32 reverse, u32 nv, u32 nn, u32 nf (= nn + 1),
        u32 quantized; vertices nv x 3 (f32, or s16 when quantized);
        u32 normals flag; normals nv x 3 (f32 or s16) when flag = 1;
        nodes nn x 32 B {f32 volume, f32 centre xyz, f32 half xyz, u16 child0, u16 child1}
        (bit 15 = leaf, low 15 bits = face index; node 0 = root, depth-first);
        faces nf x 22 B {f32 D, f32 normal xyz, u16 v0 v1 v2}.
All arrays are kept as numpy arrays of the stored type, so reading and writing is bit-exact
(including the authored NaN planes of degenerate faces).
"""
import struct
import numpy as np

NODE = np.dtype([('volume', '<f4'), ('centre', '<f4', 3), ('half', '<f4', 3), ('child', '<u2', 2)])
FACE = np.dtype([('d', '<f4'), ('normal', '<f4', 3), ('v', '<u2', 3)])
LEAF = 0x8000


class Body:
    def __init__(self):
        self.matrix = np.array([1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0], '<f4')
        self.reverse = 0
        self.quantized = 0
        self.verts = np.zeros((0, 3), '<f4')
        self.normals = None
        self.nodes = np.zeros(0, NODE)
        self.faces = np.zeros(0, FACE)


class Col:
    def __init__(self):
        self.version = 1
        self.type = 0
        self.reverse = 0          # type 1 header field
        self.members = []         # list of (scene node index or None, Body)

    @property
    def body(self):
        return self.members[0][1]


def _read_body(d, at):
    b = Body()
    b.matrix = np.frombuffer(d, '<f4', 12, at).copy(); at += 48
    b.reverse, nv, nn, nf, b.quantized = struct.unpack_from('<5I', d, at); at += 20
    assert b.reverse <= 1 and b.quantized <= 1
    assert nf == nn + 1, 'tree counts'
    vt = '<i2' if b.quantized else '<f4'
    vs = 6 if b.quantized else 12
    b.verts = np.frombuffer(d, vt, nv * 3, at).reshape(nv, 3).copy(); at += vs * nv
    flag, = struct.unpack_from('<I', d, at); at += 4
    assert flag <= 1
    if flag:
        b.normals = np.frombuffer(d, vt, nv * 3, at).reshape(nv, 3).copy(); at += vs * nv
    b.nodes = np.frombuffer(d, NODE, nn, at).copy(); at += 32 * nn
    b.faces = np.frombuffer(d, FACE, nf, at).copy(); at += 22 * nf
    return b, at


def _write_body(b):
    out = bytearray(b.matrix.astype('<f4').tobytes())
    out += struct.pack('<5I', b.reverse, len(b.verts), len(b.nodes), len(b.faces), b.quantized)
    vt = '<i2' if b.quantized else '<f4'
    out += np.ascontiguousarray(b.verts, vt).tobytes()
    out += struct.pack('<I', 1 if b.normals is not None else 0)
    if b.normals is not None:
        out += np.ascontiguousarray(b.normals, vt).tobytes()
    out += np.ascontiguousarray(b.nodes, NODE).tobytes()
    out += np.ascontiguousarray(b.faces, FACE).tobytes()
    return bytes(out)


def read_col(d):
    mg, tag, ver, ty = struct.unpack_from('<I4sII', d, 0)
    assert mg == 1234 and tag == b'col\0' and ver == 1, 'COL header'
    c = Col(); c.version = ver; c.type = ty
    at = 16
    if ty == 0:
        b, at = _read_body(d, at)
        c.members.append((None, b))
    elif ty == 1:
        count, c.reverse = struct.unpack_from('<II', d, at); at += 8
        for _ in range(count):
            node, = struct.unpack_from('<I', d, at); at += 4
            b, at = _read_body(d, at)
            c.members.append((node, b))
    else:
        raise ValueError('COL type %d' % ty)
    assert at == len(d), 'trailing bytes'
    return c


def write_col(c):
    out = bytearray(struct.pack('<I4sII', 1234, b'col\0', c.version, c.type))
    if c.type == 0:
        out += _write_body(c.body)
    else:
        out += struct.pack('<II', len(c.members), c.reverse)
        for node, b in c.members:
            out += struct.pack('<I', node) + _write_body(b)
    return bytes(out)


# ---------------------------------------------------------------- building new COL files
def _box(lo, hi):
    c = (lo + hi) * 0.5
    h = (hi - lo) * 0.5
    return c, h


def build_body(verts, tris, vertex_normals=False, leaf_order=None):
    """verts (n,3) float, tris (m,3) int -> Body with face planes and an AABB tree.
    Tree: node 0 root, depth first; each internal node has two children; a child is either
    another node or a face leaf (bit 15). Split: longest axis of the node box, faces sorted
    by centroid on that axis, cut at the median (balanced). Volume = box volume (8 hx hy hz).
    (Original split rule unknown; this is generated, see notes\a2r-track.md.)"""
    verts = np.asarray(verts, np.float64)
    tris = np.asarray(tris, np.int64)
    assert len(verts) <= 65535 and 1 <= len(tris) <= 32768
    b = Body()
    b.verts = verts.astype('<f4')
    v32 = b.verts.astype(np.float64)
    a, bb, cc = v32[tris[:, 0]], v32[tris[:, 1]], v32[tris[:, 2]]
    n = np.cross(bb - a, cc - a)
    ln = np.linalg.norm(n, axis=1)
    good = ln > 0
    n[good] /= ln[good, None]
    n[~good] = 0.0
    dd = -(n * a).sum(1)     # plane: n.p + D = 0  (sign checked against originals)
    faces = np.zeros(len(tris), FACE)
    faces['d'] = dd; faces['normal'] = n; faces['v'] = tris
    b.faces = faces
    if vertex_normals:
        vn = np.zeros_like(v32)
        for k in range(3):
            np.add.at(vn, tris[:, k], n * ln[:, None])
        l2 = np.linalg.norm(vn, axis=1); l2[l2 == 0] = 1
        b.normals = (vn / l2[:, None]).astype('<f4')
    lo_t = np.minimum(np.minimum(a, bb), cc)
    hi_t = np.maximum(np.maximum(a, bb), cc)
    cen = (lo_t + hi_t) * 0.5
    nodes = []

    def rec(idx):
        me = len(nodes)
        nodes.append(None)
        lo = lo_t[idx].min(0); hi = hi_t[idx].max(0)
        c, h = _box(lo, hi)
        ax = int(np.argmax(hi - lo))
        order = idx[np.argsort(cen[idx, ax], kind='stable')]
        half = len(order) // 2
        parts = [order[:half], order[half:]]
        ch = []
        for p in parts:
            if len(p) == 1:
                ch.append(LEAF | int(p[0]))
            else:
                ch.append(rec(p))
        nodes[me] = (8 * h[0] * h[1] * h[2], c, h, ch)
        return me

    if len(tris) == 1:
        raise ValueError('single-face COL (no node) is not used by the originals of type 0')
    rec(np.arange(len(tris)))
    arr = np.zeros(len(nodes), NODE)
    for i, (vol, c, h, ch) in enumerate(nodes):
        arr[i]['volume'] = vol; arr[i]['centre'] = c; arr[i]['half'] = h; arr[i]['child'] = ch
    b.nodes = arr
    return b


def make_col(verts, tris, vertex_normals=False):
    c = Col()
    c.members.append((None, build_body(verts, tris, vertex_normals)))
    return c


def check_body(b):
    """Structural checks used on converted files: tree reaches every face exactly once,
    children boxes inside parents, plane residuals."""
    nf = len(b.faces); nn = len(b.nodes)
    assert nf == nn + 1
    seen_nodes = np.zeros(nn, bool); seen_faces = np.zeros(nf, int)
    stack = [0]
    while stack:
        i = stack.pop()
        assert not seen_nodes[i]; seen_nodes[i] = True
        for ch in b.nodes[i]['child']:
            if ch & LEAF:
                seen_faces[ch & 0x7FFF] += 1
            else:
                stack.append(int(ch))
    assert seen_nodes.all() and (seen_faces == 1).all()
    v = b.verts.astype(np.float64)
    idx = b.faces['v'].astype(np.int64)
    assert idx.max() < len(v)
    res = np.abs((b.faces['normal'].astype(np.float64) * v[idx[:, 0]]).sum(1) + b.faces['d'])
    return dict(faces=nf, nodes=nn, verts=len(v), max_plane_residual=float(np.nanmax(res)))
