r"""PSG version 3 reader and writer (Racer Revenge geometry), for the a2r converter.

Format (D:\Programming\GitHub\RacerRevengePC\docs\asset-decoder48b.md,
D:\Programming\GitHub\RacerRevengePC\tools\assets\decode_scene48c.py, src\assets\import_decode48c.cpp):

  'psg\0', u32 3, u32 nodeCount
  node x nodeCount (224 B): char name[128], f32 matrix[16] (row vectors, translation in row 3),
        f32 boundsCentre[3], f32 boundsHalf[3], f32 radius, i32 parent (-1 = root, parent < index)
  u32 materialCount, char material[64] x materialCount            (-> textures\<name>.psm)
  f32 posScale, f32 normalScale, f32 uScale, f32 vScale, f32 origin[3], u32 worldFlag
  u32 lodCount (1 or 4); per LOD: f32 cutoff, u32 surfaceCount; per surface:
        u32 material, u32 componentWord, f32 uvSize, u16 vertexCount, u16 sourceStripCount,
        u32 payloadBytes (multiple of 16), payload
  if nodeCount > 1: u32 1, u32 paddedBytes=(n+3)&~3, u8 ids[n]=0..n-1, zero pad, u32 n

  payload = groups of VIF UNPACK commands, then zero padding to 16:
        6C01_8000  4 x u32 GIF tag {0x8000|n, 0x30024000, 0x412, 0}
        6Dnn_8001  n x s16[4] position (x, y, z, ADC/bone)   or 6Cnn_8001 n x s32[4] (wide)
        6Ann_8001+n   n x s8[3] normal (padded to 4 bytes)
        65nn_8001+2n  n x s16[2] uv
        6Enn_C001+3n  n x u8[4] colour     (only when componentWord & 0xFF == 0x0F)
        1700_0000  MSCNT
  ADC/bone word: -16*(bone+1) for the two vertices that start a strip, +16*(bone+1) for every
  vertex that closes a triangle. Triangle k of a strip uses (k, k+1, k+2), swapping the first two
  on odd k (decoder: decode_static48b.triangle_list). Strips longer than 16 vertices are split,
  the next group repeating the last two vertices (decode_scene48c.strip_topology); the header
  sourceStripCount counts source strips (continuations not counted).

parse(blob) -> model dict; write(model) -> bytes. write(parse(b)) == b is checked by
geo_roundtrip.py on every original PSG.
"""
import math
import struct

MAGIC = b'psg\0'
GIF_WORDS = (0x30024000, 0x412, 0)


class PsgError(ValueError):
    pass


def _req(c, msg):
    if not c:
        raise PsgError(msg)


def _name(raw):
    text, pad = raw.split(b'\0', 1)
    # keep any non-zero padding verbatim so the writer reproduces it
    return text.decode('ascii'), (None if not any(pad) else raw)


def _pad_name(text, size, raw=None):
    if raw is not None:
        return raw
    b = text.encode('ascii')
    _req(len(b) < size, 'name too long: %s' % text)
    return b + b'\0' * (size - len(b))


# ---------------------------------------------------------------- reader
def parse(blob):
    _req(blob[:8] == MAGIC + b'\x03\0\0\0', 'not PSG v3')
    n, = struct.unpack_from('<I', blob, 8)
    _req(1 <= n <= 256, 'node count')
    at = 12
    nodes = []
    for i in range(n):
        name, raw = _name(blob[at:at + 128])
        m = struct.unpack_from('<16f', blob, at + 128)
        b = struct.unpack_from('<7fi', blob, at + 192)
        nodes.append(dict(name=name, name_raw=raw, matrix=list(m), centre=list(b[0:3]), half=list(b[3:6]),
                          radius=b[6], parent=b[7]))
        at += 224
    nm, = struct.unpack_from('<I', blob, at)
    at += 4
    mats = []
    mats_raw = []
    for i in range(nm):
        t, raw = _name(blob[at:at + 64])
        mats.append(t)
        mats_raw.append(raw)
        at += 64
    q = struct.unpack_from('<7fI', blob, at)
    at += 32
    nl, = struct.unpack_from('<I', blob, at)
    at += 4
    lods = []
    for li in range(nl):
        cutoff, ns = struct.unpack_from('<fI', blob, at)
        at += 8
        surfs = []
        for si in range(ns):
            mat, cw, uvs, nv, nst, size = struct.unpack_from('<IIfHHI', blob, at)
            at += 20
            _req(at + size <= len(blob), 'surface extent')
            groups, pad = _parse_payload(blob, at, at + size, cw)
            surfs.append(dict(material=mat, component=cw, uv_size=uvs, vertex_count=nv, strip_count=nst,
                              groups=groups, tail_pad=pad))
            at += size
        lods.append(dict(cutoff=cutoff, surfaces=surfs))
    palette = None
    if n > 1:
        np_, nb = struct.unpack_from('<II', blob, at)
        ids = blob[at + 8:at + 8 + nb]
        cnt, = struct.unpack_from('<I', blob, at + 8 + nb)
        palette = dict(count=np_, bytes=ids, n=cnt)
        at += 12 + nb
    _req(at == len(blob), 'unconsumed bytes %d of %d' % (at, len(blob)))
    return dict(nodes=nodes, materials=mats, materials_raw=mats_raw, quant=list(q[:7]), world=q[7],
                lods=lods, palette=palette)


def _parse_payload(blob, start, end, cw):
    pos = start
    groups = []
    colour = (cw & 0xFF) == 0x0F

    def cmd(expect_cmd, expect_addr, fmt):
        nonlocal pos
        word, = struct.unpack_from('<I', blob, pos)
        c, cnt, addr = word >> 24, (word >> 16) & 0xFF, word & 0xFFFF
        _req(c in expect_cmd and addr == expect_addr(cnt), 'VIF word %08x at %d' % (word, pos))
        pos += 4
        f = fmt[c]
        sz = struct.calcsize(f)
        vals = [struct.unpack_from(f, blob, pos + i * sz) for i in range(cnt)]
        nbytes = sz * cnt
        padded = (nbytes + 3) & ~3
        _req(not any(blob[pos + nbytes:pos + padded]), 'VIF pad')
        pos += padded
        return c, vals

    while pos < end and struct.unpack_from('<I', blob, pos)[0] != 0:
        _, tag = cmd((0x6C,), lambda c: 0x8000, {0x6C: '<4I'})
        _req(len(tag) == 1 and tuple(tag[0][1:]) == GIF_WORDS, 'GIF tag')
        nv = tag[0][0] & 0x7FFF
        _req(tag[0][0] == 0x8000 | nv, 'GIF tag nloop')
        c, xyz = cmd((0x6D, 0x6C), lambda c: 0x8001, {0x6D: '<4h', 0x6C: '<4i'})
        _req(len(xyz) == nv, 'xyz count')
        _, nrm = cmd((0x6A,), lambda c: 0x8001 + nv, {0x6A: '<3b'})
        _, uv = cmd((0x65,), lambda c: 0x8001 + 2 * nv, {0x65: '<2h'})
        rgba = None
        if colour:
            _, rgba = cmd((0x6E,), lambda c: 0xC001 + 3 * nv, {0x6E: '<4B'})
        _req(struct.unpack_from('<I', blob, pos)[0] == 0x17000000, 'MSCNT')
        pos += 4
        groups.append(dict(wide=(c == 0x6C), xyzw=xyz, normal=nrm, uv=uv, rgba=rgba))
    _req(not any(blob[pos:end]), 'payload tail not zero')
    return groups, end - pos


# ---------------------------------------------------------------- writer
def _payload(groups, cw):
    out = bytearray()
    colour = (cw & 0xFF) == 0x0F

    def emit(c, cnt, addr, fmt, vals):
        out.extend(struct.pack('<I', (c << 24) | (cnt << 16) | addr))
        body = b''.join(struct.pack(fmt, *v) for v in vals)
        out.extend(body + b'\0' * (((len(body) + 3) & ~3) - len(body)))

    for g in groups:
        n = len(g['xyzw'])
        _req(3 <= n <= 255, 'group size')
        emit(0x6C, 1, 0x8000, '<4I', [(0x8000 | n,) + GIF_WORDS])
        if g.get('wide'):
            emit(0x6C, n, 0x8001, '<4i', g['xyzw'])
        else:
            emit(0x6D, n, 0x8001, '<4h', g['xyzw'])
        emit(0x6A, n, 0x8001 + n, '<3b', g['normal'])
        emit(0x65, n, 0x8001 + 2 * n, '<2h', g['uv'])
        if colour:
            emit(0x6E, n, 0xC001 + 3 * n, '<4B', g['rgba'])
        out.extend(struct.pack('<I', 0x17000000))
    return bytes(out)


def write(model):
    nodes = model['nodes']
    out = bytearray(MAGIC + struct.pack('<II', 3, len(nodes)))
    for nd in nodes:
        out += _pad_name(nd['name'], 128, nd.get('name_raw'))
        out += struct.pack('<16f', *nd['matrix'])
        out += struct.pack('<7fi', *nd['centre'], *nd['half'], nd['radius'], nd['parent'])
    out += struct.pack('<I', len(model['materials']))
    raws = model.get('materials_raw') or [None] * len(model['materials'])
    for t, raw in zip(model['materials'], raws):
        out += _pad_name(t, 64, raw)
    out += struct.pack('<7fI', *model['quant'], model['world'])
    out += struct.pack('<I', len(model['lods']))
    for lod in model['lods']:
        out += struct.pack('<fI', lod['cutoff'], len(lod['surfaces']))
        for s in lod['surfaces']:
            body = _payload(s['groups'], s['component'])
            pad = s.get('tail_pad')
            if pad is None:
                pad = (-len(body)) % 16
            body += b'\0' * pad
            out += struct.pack('<IIfHHI', s['material'], s['component'], s['uv_size'], s['vertex_count'],
                               s['strip_count'], len(body))
            out += body
    if len(nodes) > 1:
        pal = model.get('palette')
        if pal is None:
            nb = (len(nodes) + 3) & ~3
            pal = dict(count=1, bytes=bytes(range(len(nodes))) + b'\0' * (nb - len(nodes)), n=len(nodes))
        out += struct.pack('<II', pal['count'], len(pal['bytes'])) + pal['bytes'] + struct.pack('<I', pal['n'])
    return bytes(out)


# ---------------------------------------------------------------- semantic view
def vertices(model, lod=0):
    """Decoded vertices per surface of one LOD: list of dict(material, pos[], normal[], uv[], rgba[],
    bone[], tris[]) in node-local space (positions = q * scale + origin)."""
    q = model['quant']
    out = []
    for s in model['lods'][lod]['surfaces']:
        P, N, UV, C, B, T = [], [], [], [], [], []
        for g in s['groups']:
            base = len(P)
            start = None
            for i, (x, y, z, w) in enumerate(g['xyzw']):
                P.append((x * q[0] + q[4], y * q[0] + q[5], z * q[0] + q[6]))
                N.append(tuple(k * q[1] for k in g['normal'][i]))
                UV.append((g['uv'][i][0] * q[2], g['uv'][i][1] * q[3]))
                C.append(tuple(g['rgba'][i]) if g['rgba'] else None)
                B.append(abs(w) // 16 - 1)
                if w < 0:
                    if start is None or i - start >= 2:
                        start = i
                    continue
                k = i - start - 2
                a, b, c = base + i - 2, base + i - 1, base + i
                T.append((b, a, c) if k & 1 else (a, b, c))
        out.append(dict(material=model['materials'][s['material']], pos=P, normal=N, uv=UV, rgba=C, bone=B,
                        tris=T))
    return out
