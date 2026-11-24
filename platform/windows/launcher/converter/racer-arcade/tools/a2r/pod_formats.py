r"""Pod-side Revenge formats for a2r: MOT (motion) writer/reader, COL type 2 (ray query) reader/writer,
COL structure remapping (type 0 boxes and type 2 rays), texture atlas pages.

MOT (src\assets\model_geometry49o.cpp readMot49O, sampler src\game\model_motion49t.cpp):
  u32 0xFFFFFFFE, u32 frameCount, f32 duration, u32 rotationType (0 ByteAxis), u32 1,
  u32 positionTrackCount, u32 rotationTrackCount;
  position tracks: u32 keyCount, u32 row, keys x 40 B {f32 time, pos[3], incoming[3], outgoing[3]}
  rotation tracks: u32 keyCount, u32 row, keys x 8 B {f32 time, u8 angle, s8 axis[3]}
  ByteAxis quaternion: w = cos(a*pi/256), xyz = axis/127 * sin(a*pi/256) (table sineWords[64] = sin(pi/4));
  rotation matrix rows (row-vector local) from q exactly as model_motion49t rotationMatrix.
  Rows = model binding rows (pod_psg.rows), not PSG indices. Each track needs >= 2 keys.

COL type 2 (src\game\type2_resource49x.cpp decodeType2Resource49X):
  u32 1234, 'col\0', u32 1, u32 2, f32 matrix[16], u32 rayCount, u32 nodeCount, u32 leafCount
  (= rayCount = nodeCount + 1), nodes x 32 B {f32 volume, centre[3], half[3], u16 child0, child1
  (bit 15 = leaf / ray index)}, rays x 24 B {xyz first, xyz second}. Byte count = 92 + 32n + 24l.
"""
import math
import struct

import numpy as np

import trk_col


# ---------------------------------------------------------------- MOT
def read_mot(b):
    tag, nf, dur, rt, one, npos, nrot = struct.unpack_from('<IIfIIII', b, 0)
    assert tag == 0xFFFFFFFE and one == 1
    at = 28
    pos, rot = [], []
    for ch, cnt in ((0, npos), (1, nrot)):
        st = 40 if ch == 0 else (12 if rt else 8)
        for _ in range(cnt):
            k, row = struct.unpack_from('<II', b, at); at += 8
            keys = [b[at + j * st:at + (j + 1) * st] for j in range(k)]; at += k * st
            (pos if ch == 0 else rot).append((row, keys))
    assert at == len(b)
    return dict(frames=nf, duration=dur, rtype=rt, pos=pos, rot=rot)


def quat_from_rows(L3):
    """Row-vector 3x3 local rotation -> (w, x, y, z) with model_motion49t's matrix convention
    (m[1] = 2(xy + wz) etc. = transpose of the column-vector matrix)."""
    Rc = np.asarray(L3, float).T
    t = np.trace(Rc)
    if t > 0:
        s = math.sqrt(t + 1) * 2
        q = (0.25 * s, (Rc[2, 1] - Rc[1, 2]) / s, (Rc[0, 2] - Rc[2, 0]) / s, (Rc[1, 0] - Rc[0, 1]) / s)
    elif Rc[0, 0] > Rc[1, 1] and Rc[0, 0] > Rc[2, 2]:
        s = math.sqrt(1 + Rc[0, 0] - Rc[1, 1] - Rc[2, 2]) * 2
        q = ((Rc[2, 1] - Rc[1, 2]) / s, 0.25 * s, (Rc[0, 1] + Rc[1, 0]) / s, (Rc[0, 2] + Rc[2, 0]) / s)
    elif Rc[1, 1] > Rc[2, 2]:
        s = math.sqrt(1 + Rc[1, 1] - Rc[0, 0] - Rc[2, 2]) * 2
        q = ((Rc[0, 2] - Rc[2, 0]) / s, (Rc[0, 1] + Rc[1, 0]) / s, 0.25 * s, (Rc[1, 2] + Rc[2, 1]) / s)
    else:
        s = math.sqrt(1 + Rc[2, 2] - Rc[0, 0] - Rc[1, 1]) * 2
        q = ((Rc[1, 0] - Rc[0, 1]) / s, (Rc[0, 2] + Rc[2, 0]) / s, (Rc[1, 2] + Rc[2, 1]) / s, 0.25 * s)
    q = np.array(q)
    return q / np.linalg.norm(q)


def rows_from_quat(q):
    w, x, y, z = q
    n = w * w + x * x + y * y + z * z
    s = 2 / n
    return np.array([[1 - s * (y * y + z * z), s * (x * y + w * z), s * (x * z - w * y)],
                     [s * (x * y - w * z), 1 - s * (x * x + z * z), s * (y * z + w * x)],
                     [s * (x * z + w * y), s * (y * z - w * x), 1 - s * (x * x + y * y)]])


def byte_axis(q):
    """Quaternion -> (angle byte, axis s8 x3); best of the candidate encodings (checked by decode)."""
    q = np.asarray(q, float)
    if q[0] < 0:
        q = -q
    half = math.acos(max(-1.0, min(1.0, q[0])))
    k = int(round(half * 256 / math.pi)) % 256
    v = q[1:]
    vn = np.linalg.norm(v)
    if k == 0 or vn < 1e-9:
        return 0, (127, 0, 0)
    a = np.clip(np.round(v / vn * 127), -127, 127).astype(int)
    return k, tuple(int(x) for x in a)


def decode_byte_axis(k, a):
    s = math.sin(k * math.pi / 256)
    return np.array([math.cos(k * math.pi / 256)] + [x / 127 * s for x in a])


def write_mot(frames, duration, pos_tracks, rot_tracks):
    """pos_tracks: list of (row, [(t, xyz)]); rot_tracks: list of (row, [(t, rows3x3)]). Constant
    Bezier tangents (incoming = outgoing = 0)."""
    out = bytearray(struct.pack('<IIfIIII', 0xFFFFFFFE, frames, duration, 0, 1, len(pos_tracks), len(rot_tracks)))
    for row, keys in pos_tracks:
        assert len(keys) >= 2
        out += struct.pack('<II', len(keys), row)
        for t, p in keys:
            out += struct.pack('<10f', t, *p, 0, 0, 0, 0, 0, 0)
    for row, keys in rot_tracks:
        assert len(keys) >= 2
        out += struct.pack('<II', len(keys), row)
        for t, L3 in keys:
            k, a = byte_axis(quat_from_rows(L3))
            out += struct.pack('<fBbbb', t, k, *a)
    m = read_mot(bytes(out))
    assert m['frames'] == frames
    return bytes(out)


# ---------------------------------------------------------------- COL type 2
class Type2:
    def __init__(self):
        self.matrix = np.eye(4, dtype='<f4')
        self.nodes = None     # structured array trk_col.NODE
        self.rays = None      # (n, 2, 3) float32


def read_type2(b):
    mg, tag, ver, ty = struct.unpack_from('<I4sII', b, 0)
    assert mg == 1234 and tag == b'col\0' and ver == 1 and ty == 2
    t = Type2()
    t.matrix = np.frombuffer(b, '<f4', 16, 16).reshape(4, 4).copy()
    nr, nn, nl = struct.unpack_from('<3I', b, 80)
    assert nl == nr == nn + 1 and len(b) == 92 + 32 * nn + 24 * nl, 'type2 counts'
    t.nodes = np.frombuffer(b, trk_col.NODE, nn, 92).copy()
    t.rays = np.frombuffer(b, '<f4', nr * 6, 92 + 32 * nn).reshape(nr, 2, 3).copy()
    return t


def write_type2(t):
    nn, nr = len(t.nodes), len(t.rays)
    out = bytearray(struct.pack('<I4sII', 1234, b'col\0', 1, 2))
    out += np.ascontiguousarray(t.matrix, '<f4').tobytes()
    out += struct.pack('<3I', nr, nn, nr)
    out += np.ascontiguousarray(t.nodes, trk_col.NODE).tobytes()
    out += np.ascontiguousarray(t.rays, '<f4').tobytes()
    return bytes(out)


def check_type2(b):
    """The decoder's admission rules (counts, size, finite values, non-negative halves, node 0 root,
    every node and leaf reached exactly once)."""
    t = read_type2(b)
    nn, nl = len(t.nodes), len(t.rays)
    assert 0 < nn <= 32767 and nl == nn + 1
    m = t.matrix.reshape(-1)
    assert np.isfinite(m).all() and m[3] == 0 and m[7] == 0 and m[11] == 0 and m[15] == 1
    assert np.isfinite(t.rays).all()
    assert (t.nodes['half'] >= 0).all() and (t.nodes['volume'] >= 0).all()
    seen_n = np.zeros(nn, int); seen_l = np.zeros(nl, int)
    for i in range(nn):
        for c in t.nodes[i]['child']:
            if c & 0x8000:
                seen_l[c & 0x7FFF] += 1
            else:
                assert c != 0
                seen_n[c] += 1
    assert seen_n[0] == 0 and (seen_n[1:] == 1).all() and (seen_l == 1).all()
    return dict(nodes=nn, rays=nl, bytes=len(b))


# ---------------------------------------------------------------- remapping
def remap(P, src_lo, src_hi, dst_lo, dst_hi):
    """Per-axis affine map of points from one box onto another (structure-preserving resize)."""
    P = np.asarray(P, float)
    s_lo, s_hi = np.asarray(src_lo, float), np.asarray(src_hi, float)
    d_lo, d_hi = np.asarray(dst_lo, float), np.asarray(dst_hi, float)
    span = s_hi - s_lo
    k = np.where(span > 1e-9, (d_hi - d_lo) / np.where(span > 1e-9, span, 1), 1.0)
    return d_lo + (P - s_lo) * k


def _refit_nodes(nodes, child_box):
    """Recompute node boxes bottom-up from their children (leaf boxes from child_box(index))."""
    nn = len(nodes)
    box = [None] * nn

    def get(i):
        if box[i] is not None:
            return box[i]
        los, his = [], []
        for c in nodes[i]['child']:
            c = int(c)
            lo, hi = child_box(c & 0x7FFF) if c & 0x8000 else get(c)
            los.append(lo); his.append(hi)
        box[i] = (np.min(los, 0), np.max(his, 0))
        return box[i]
    for i in range(nn):
        lo, hi = get(i)
        c = (lo + hi) / 2; h = (hi - lo) / 2
        nodes[i]['centre'] = c; nodes[i]['half'] = h
        nodes[i]['volume'] = 8 * h[0] * h[1] * h[2]
    return nodes


def remap_col(blob, src_box, dst_box):
    """Target COL (type 0 or 2) -> same structure, every position mapped src_box -> dst_box; planes,
    node boxes and volumes recomputed. Returns (bytes, kind)."""
    ty = struct.unpack_from('<I', blob, 12)[0]
    if ty == 2:
        t = read_type2(blob)
        t.rays = remap(t.rays.reshape(-1, 3), *src_box, *dst_box).reshape(-1, 2, 3).astype('<f4')
        r = t.rays.astype(float)
        _refit_nodes(t.nodes, lambda i: (r[i].min(0), r[i].max(0)))
        out = write_type2(t)
        check_type2(out)
        return out, 2
    c = trk_col.read_col(blob)
    assert c.type == 0
    b = c.body
    assert not b.quantized
    b.verts = remap(b.verts, *src_box, *dst_box).astype('<f4')
    v = b.verts.astype(float)
    idx = b.faces['v'].astype(int)
    a, bb, cc = v[idx[:, 0]], v[idx[:, 1]], v[idx[:, 2]]
    n = np.cross(bb - a, cc - a)
    ln = np.linalg.norm(n, axis=1); ln[ln == 0] = 1
    n /= ln[:, None]
    b.faces['normal'] = n
    b.faces['d'] = -(n * a).sum(1)
    if b.normals is not None:
        b.normals = None   # not present in the pod originals; recomputing would be invented
    fl = np.minimum(np.minimum(a, bb), cc); fh = np.maximum(np.maximum(a, bb), cc)
    _refit_nodes(b.nodes, lambda i: (fl[i], fh[i]))
    out = trk_col.write_col(c)
    trk_col.check_body(trk_col.read_col(out).body)
    return out, 0


def col_points(blob):
    ty = struct.unpack_from('<I', blob, 12)[0]
    if ty == 2:
        return read_type2(blob).rays.reshape(-1, 3).astype(float)
    return trk_col.read_col(blob).body.verts.astype(float)


# ---------------------------------------------------------------- atlas pages
def shelf_pack(sizes, W, H):
    """sizes: list of (w, h). Returns list of (x, y) or None if they do not fit W x H."""
    order = sorted(range(len(sizes)), key=lambda i: (-sizes[i][1], -sizes[i][0]))
    pos = [None] * len(sizes)
    x = y = shelf_h = 0
    for i in order:
        w, h = sizes[i]
        if w > W:
            return None
        if x + w > W:
            y += shelf_h; x = 0; shelf_h = 0
        if y + h > H:
            return None
        pos[i] = (x, y)
        x += w
        shelf_h = max(shelf_h, h)
    return pos


def maxrects_pack(sizes, W, H):
    """MaxRects (best short side fit), largest area first. sizes: list of (w, h). Returns list of
    (x, y) or None if they do not fit W x H. Packs tighter than shelf_pack for mixed cell sizes."""
    order = sorted(range(len(sizes)), key=lambda i: (-sizes[i][0] * sizes[i][1], -max(sizes[i])))
    free = [(0, 0, W, H)]
    pos = [None] * len(sizes)
    for i in order:
        w, h = sizes[i]
        best = None
        for fx, fy, fw, fh in free:
            if w <= fw and h <= fh:
                key = (min(fw - w, fh - h), max(fw - w, fh - h), fy, fx)
                if best is None or key < best[0]:
                    best = (key, fx, fy)
        if best is None:
            return None
        _, x, y = best
        pos[i] = (x, y)
        nxt = []
        for fx, fy, fw, fh in free:      # split every free rectangle the new cell overlaps
            if x >= fx + fw or x + w <= fx or y >= fy + fh or y + h <= fy:
                nxt.append((fx, fy, fw, fh))
                continue
            if x > fx:
                nxt.append((fx, fy, x - fx, fh))
            if x + w < fx + fw:
                nxt.append((x + w, fy, fx + fw - x - w, fh))
            if y > fy:
                nxt.append((fx, fy, fw, y - fy))
            if y + h < fy + fh:
                nxt.append((fx, y + h, fw, fy + fh - y - h))
        free = [r for k, r in enumerate(nxt) if not any(
            k != j and r[0] >= o[0] and r[1] >= o[1] and r[0] + r[2] <= o[0] + o[2] and r[1] + r[3] <= o[1] + o[3]
            and (r != o or j < k) for j, o in enumerate(nxt))]
    return pos


def pack(sizes, W, H):
    """Best of the two packers (maxrects first)."""
    return maxrects_pack(sizes, W, H) or shelf_pack(sizes, W, H)


def _components(nv, faces):
    par = list(range(nv))

    def f(i):
        while par[i] != i:
            par[i] = par[par[i]]; i = par[i]
        return i
    for a, b, c in faces:
        par[f(b)] = f(a); par[f(c)] = f(a)
    groups = {}
    for i in range(nv):
        groups.setdefault(f(i), []).append(i)
    return list(groups.values())


def remap_podbox_parts(blob, t_pair, m_pair, t_ch, m_ch, gap_min=1.0):
    """Type-0 pod shape (podboxf: one hull over the engine pair and the chariot behind it, vertices
    ahead of the engine-pair rear = engine part, behind it = chariot part). Every vertex maps with its
    part: engine part t_pair -> m_pair; chariot part x/y t_ch -> m_ch and z piecewise from the target
    chariot rear .. engine rear onto the converted chariot rear .. engine rear (at least gap_min ft
    when the converted chariot is not behind the engines). Overhangs beyond a part box are kept in
    feet; z order is preserved, so no face turns inside out. Returns (bytes, rows)."""
    c = trk_col.read_col(blob)
    b = c.body
    assert c.type == 0 and not b.quantized
    V = b.verts.astype(float)
    idx = b.faces['v'].astype(int)
    t_pair = [np.asarray(x, float) for x in t_pair]; m_pair = [np.asarray(x, float) for x in m_pair]
    t_ch = [np.asarray(x, float) for x in t_ch]; m_ch = [np.asarray(x, float) for x in m_ch]

    def axis_map(v, src, dst):
        vc = np.clip(v, src[0], src[1])
        return remap(vc, src[0], src[1], dst[0], dst[1]) + (v - vc)
    tz0, mz0 = t_pair[0][2] - 1.0, m_pair[0][2] - 1.0   # 1 ft tolerance: hull rear faces sit just behind the engine geometry
    t_rear = min(V[:, 2].min(), tz0 - 1e-3)
    m_rear = min(m_ch[0][2] + (t_rear - t_ch[0][2]), mz0 - gap_min)
    out = np.empty_like(V)
    rows = [0, 0]
    for k, v in enumerate(V):
        if v[2] >= tz0 - 1e-6:
            out[k] = axis_map(v, t_pair, m_pair); rows[0] += 1
        else:
            out[k, :2] = axis_map(v, t_ch, m_ch)[:2]
            out[k, 2] = m_rear + (v[2] - t_rear) * (mz0 - m_rear) / (tz0 - t_rear)
            rows[1] += 1
    rows = [('engine part', rows[0]), ('chariot part', rows[1]),
            ('z', round(float(out[:, 2].min()), 2), round(float(out[:, 2].max()), 2))]
    b.verts = out.astype('<f4')
    v = b.verts.astype(float)
    a, bb, cc = v[idx[:, 0]], v[idx[:, 1]], v[idx[:, 2]]
    n = np.cross(bb - a, cc - a)
    ln = np.linalg.norm(n, axis=1); ln[ln == 0] = 1
    n /= ln[:, None]
    b.faces['normal'] = n
    b.faces['d'] = -(n * a).sum(1)
    if b.normals is not None:
        b.normals = None
    fl = np.minimum(np.minimum(a, bb), cc); fh = np.maximum(np.maximum(a, bb), cc)
    _refit_nodes(b.nodes, lambda i: (fl[i], fh[i]))
    new = trk_col.write_col(c)
    trk_col.check_body(trk_col.read_col(new).body)
    return new, rows
