"""RES3 archive reader (Racer Revenge), read-only, for the a2r geometry converter (geo_*).

Layout (src\\assets\\import_decode48c.cpp:147-286): u32 3, u32 meta, u32 0x340, u32 0,
u32 tableBytes (20 + tableBytes == 8 + meta); u16 compressed size per 24,576-byte block, ending
in 0; u32 member count, u32 index bytes; per member {u32 name length, name, u32 logical offset,
u32 bytes}; then the blocks, each one zlib stream followed by 0xFF.
"""
import struct
import zlib

BLOCK = 24576


class ResError(ValueError):
    pass


def _req(c, msg):
    if not c:
        raise ResError(msg)


def read_res(path):
    """Returns (members: dict lower-name -> bytes, order: list of (name, offset, size), info)."""
    raw = open(path, 'rb').read()
    _req(len(raw) >= 28, 'short RES')
    v, meta, codec, flags, table = struct.unpack_from('<5I', raw, 0)
    _req(v == 3 and codec == 0x340 and flags == 0, 'not RES3/0x340')
    _req(table >= 2 and table % 2 == 0 and 20 + table == 8 + meta, 'size table')
    nblk = table // 2 - 1
    sizes = struct.unpack_from('<%dH' % (nblk + 1), raw, 20)
    _req(sizes[-1] == 0 and all(sizes[:-1]), 'size table terminator')
    off = 20 + table
    count, ibytes = struct.unpack_from('<II', raw, off)
    off += 8
    end = off + ibytes
    order = []
    for _ in range(count):
        n, = struct.unpack_from('<I', raw, off)
        off += 4
        name = raw[off:off + n].decode('ascii')
        off += n
        lo, nb = struct.unpack_from('<II', raw, off)
        off += 8
        order.append((name, lo, nb))
    _req(off == end, 'index suffix')
    flat = bytearray()
    for s in sizes[:-1]:
        d = zlib.decompressobj()
        blk = d.decompress(raw[off:off + s])
        _req(len(blk) == BLOCK and d.eof and not d.unused_data, 'block')
        _req(raw[off + s] == 0xFF, 'block boundary')
        flat += blk
        off += s + 1
    _req(off == len(raw), 'trailing')
    members = {}
    for name, lo, nb in order:
        _req(lo + nb <= len(flat), 'member extent')
        members[name.lower()] = bytes(flat[lo:lo + nb])
    return members, order, dict(path=str(path), bytes=len(raw), blocks=nblk, members=count)


if __name__ == '__main__':
    import sys
    m, order, info = read_res(sys.argv[1])
    print(info)
    for name, lo, nb in order[:int(sys.argv[2]) if len(sys.argv) > 2 else 40]:
        print('%9d %8d %s' % (lo, nb, name))
