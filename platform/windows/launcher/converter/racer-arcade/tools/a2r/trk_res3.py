r"""RES3 archive reader + writer for the a2r track converter (track helper's copy; res3.py in this
folder belongs to the geometry helper and is read-only).

Layout (port decoder: src\assets\import_decode48c.cpp decodeResourceData48C):
  u32 3, u32 meta (= tableBytes + 12), u32 0x340, u32 0, u32 tableBytes
  u16 compressed size per 24576-byte block, then u16 0          (tableBytes bytes)
  u32 member count, u32 index bytes
  per member: u32 name length, name (ASCII, backslashes), u32 logical offset, u32 bytes
  per block: one zlib stream of exactly 24576 bytes, then 0xFF

Original layout rule (checked on all 13 tracks by trk_roundtrip.py): members in index order,
each starting on a block boundary, zero padding up to the next boundary.
Compression: zlib 1.1.x level 6 behaviour, reproduced by trk_deflate11.py (byte-identical on
every block of the 13 original tracks).
"""
import struct
import zlib
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import trk_deflate11

BLOCK = 24576
HDR_CODEC = 0x340


def deflate_block(raw):
    return trk_deflate11.compress(raw)


class Res3:
    def __init__(self):
        self.members = []       # (name, logical offset, bytes)
        self.payload = b''      # decoded logical storage, block multiple
        self.zblocks = None     # original compressed blocks (when read from a file)

    def get(self, name):
        for n, lo, sz in self.members:
            if n.lower() == name.lower():
                return self.payload[lo:lo + sz]
        raise KeyError(name)

    def has(self, name):
        return any(n.lower() == name.lower() for n, _, _ in self.members)

    def names(self):
        return [m[0] for m in self.members]

    def items(self):
        for n, lo, sz in self.members:
            yield n, self.payload[lo:lo + sz]


def read_res3(data):
    v, meta, codec, z, tb = struct.unpack_from('<5I', data, 0)
    assert v == 3 and codec == HDR_CODEC and z == 0, 'not RES3'
    assert tb >= 2 and tb % 2 == 0 and 20 + tb == 8 + meta, 'size table'
    nb = tb // 2 - 1
    sizes = struct.unpack_from('<%dH' % nb, data, 20)
    assert struct.unpack_from('<H', data, 20 + nb * 2)[0] == 0
    assert all(sizes), 'zero block size'
    off = 20 + tb
    cnt, ib = struct.unpack_from('<2I', data, off)
    off += 8
    end = off + ib
    r = Res3()
    for _ in range(cnt):
        n, = struct.unpack_from('<I', data, off); off += 4
        name = data[off:off + n].decode('ascii'); off += n
        lo, sz = struct.unpack_from('<2I', data, off); off += 8
        assert lo + sz <= nb * BLOCK, 'member extent'
        r.members.append((name, lo, sz))
    assert off == end, 'index suffix'
    out = bytearray(); zb = []
    for s in sizes:
        blk = data[off:off + s]
        assert data[off + s] == 0xFF, 'block boundary'
        d = zlib.decompressobj()
        raw = d.decompress(blk)
        assert len(raw) == BLOCK and d.eof and not d.unused_data, 'block'
        out += raw; zb.append(blk)
        off += s + 1
    assert off == len(data), 'trailing bytes'
    r.payload = bytes(out)
    r.zblocks = zb
    return r


def encode_res3(r):
    """Encode members + payload exactly as the original tool did."""
    assert len(r.payload) % BLOCK == 0
    blocks = []
    for i in range(len(r.payload) // BLOCK):
        blocks.append(deflate_block(r.payload[i * BLOCK:(i + 1) * BLOCK]))
    for b in blocks:
        assert 0 < len(b) < 65536, 'block does not fit u16 size'
    table = struct.pack('<%dH' % (len(blocks) + 1), *[len(b) for b in blocks], 0)
    idx = bytearray()
    for name, lo, sz in r.members:
        nb = name.encode('ascii')
        assert nb and all(0 < c < 128 for c in nb)
        idx += struct.pack('<I', len(nb)) + nb + struct.pack('<2I', lo, sz)
    out = bytearray(struct.pack('<5I', 3, len(table) + 12, HDR_CODEC, 0, len(table)))
    out += table
    out += struct.pack('<2I', len(r.members), len(idx)) + idx
    for b in blocks:
        out += b + b'\xff'
    return bytes(out)


def build_res3(files):
    """files: list of (name, bytes) in archive order -> Res3 laid out by the original rule."""
    r = Res3()
    buf = bytearray()
    for name, data in files:
        assert len(data) > 0, 'empty member (never seen in originals)'
        r.members.append((name, len(buf), len(data)))
        buf += data
        buf += b'\0' * ((-len(buf)) % BLOCK)
    r.payload = bytes(buf)
    return r


def write_res3(files):
    return encode_res3(build_res3(files))
