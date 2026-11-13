r"""BINARY\0 parameter-block files (.scb .tob .aub) for the a2r track converter.

Layout (repo tools\assets\decode_world48e.py decode_parameter_blocks; original reader is the
port's BINARY1 path):
  'BINARY\0', u32 version 1, u32 block count
  per block: u32 name bytes (incl. NUL), lower-case name + NUL      (blocks sorted by name)
  per block: u32 start, u32 size  (size excludes the block's u32 label count)
  per block at start: u32 label count, labels of 43 bytes {35-byte NUL-padded lower-case key,
       u32 value offset (from after the count), u32 value bytes incl. NUL}  (labels sorted by key)
       then the values as NUL-terminated ASCII, in their order in the text source
Compiling a text file (.scn/.toy/audio ini) = lower-case section and key names, sort both,
keep values verbatim in text order (checked on gc.scn -> gc.scb and gc.toy -> gc.tob).
"""
import struct


class BinParamError(ValueError):
    pass


def _req(c, m):
    if not c:
        raise BinParamError(m)


def decode(data):
    """-> list of (block name, [(key, value)] in value storage order)."""
    _req(data[:7] == b'BINARY\0', 'magic')
    ver, count = struct.unpack_from('<II', data, 7)
    _req(ver == 1, 'version')
    at = 15
    names = []
    for _ in range(count):
        n, = struct.unpack_from('<I', data, at); at += 4
        raw = data[at:at + n]; at += n
        _req(raw.endswith(b'\0') and b'\0' not in raw[:-1], 'block name')
        names.append(raw[:-1].decode('ascii'))
    table = at
    blocks = []
    nxt = table + 8 * count
    for i, name in enumerate(names):
        start, size = struct.unpack_from('<II', data, table + 8 * i)
        _req(start == nxt, 'block order/gap')
        number, = struct.unpack_from('<I', data, start)
        base = start + 4
        labels = []
        for j in range(number):
            e = base + 43 * j
            kraw = data[e:e + 35]
            key = kraw.split(b'\0', 1)[0]
            _req(not any(kraw[len(key):]), 'key padding')
            off, ln = struct.unpack_from('<II', data, e + 35)
            v = data[base + off:base + off + ln]
            _req(v.endswith(b'\0') and b'\0' not in v[:-1], 'value terminator')
            labels.append((key.decode('ascii'), v[:-1].decode('latin-1'), off))
        # value storage order
        order = sorted(range(number), key=lambda j: labels[j][2])
        p = number * 43
        for j in order:
            _req(labels[j][2] == p, 'value gap/overlap')
            p += len(labels[j][1]) + 1
        _req(p == size, 'block size')
        kv = [(labels[j][0], labels[j][1]) for j in order]
        blocks.append((name, kv))
        nxt = base + size
    _req(nxt == len(data), 'trailing bytes')
    return blocks


def encode(blocks):
    """blocks: list of (name, [(key, value)] in value storage order).
    Blocks sorted by name; labels sorted by key, a stable sort so repeated keys (a key or a
    section repeated in the text source; seen in GC/SA .aub, MC .scb, TB/TC .tob) keep their
    storage order. Both are the original rule (all 39 originals byte-identical)."""
    blocks = sorted(blocks, key=lambda b: b[0].encode('ascii'))
    out = bytearray(b'BINARY\0' + struct.pack('<II', 1, len(blocks)))
    for b in blocks:
        raw = b[0].encode('ascii') + b'\0'
        out += struct.pack('<I', len(raw)) + raw
    table = len(out)
    out += b'\0' * (8 * len(blocks))
    for i, b in enumerate(blocks):
        name, kv = b[0], b[1]
        start = len(out)
        out += struct.pack('<I', len(kv))
        base = len(out)
        out += b'\0' * (43 * len(kv))
        loc = []
        for k, v in kv:
            raw = v.encode('latin-1') + b'\0'
            loc.append((len(out) - base, len(raw))); out += raw
        order = sorted(range(len(kv)), key=lambda j: kv[j][0].encode('ascii'))
        for slot, j in enumerate(order):
            kb = kv[j][0].encode('ascii')
            _req(len(kb) < 35, 'key too long')
            p = base + 43 * slot
            out[p:p + 35] = kb + b'\0' * (35 - len(kb))
            struct.pack_into('<II', out, p + 35, *loc[j])
        struct.pack_into('<II', out, table + 8 * i, start, len(out) - start - 4)
    return bytes(out)


def compile_ini(ini_sections, drop_spaces=True):
    """ini_sections: [(section name, [(key, value)])] in text order -> BINARY bytes.
    A section name repeated in the text merges into one block (values appended).
    drop_spaces: the .scn/.toy compiler removed every space inside values (gc.scn
    'SLT=house stands b01.slt' -> gc.scb 'housestandsb01.slt'); the .aub files keep them
    ('Market Table04' in ga.aub), so pass False for .aub."""
    merged = {}
    order = []
    for name, kv in ini_sections:
        n = name.lower()
        if n not in merged:
            merged[n] = []; order.append(n)
        merged[n] += [(k.lower(), v.replace(' ', '') if drop_spaces else v) for k, v in kv]
    return encode([(n, merged[n]) for n in order])
