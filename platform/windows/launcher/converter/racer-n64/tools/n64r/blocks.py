#!/usr/bin/env python3
r"""Star Wars Episode I: Racer (N64 USA) block containers.

Reads the four blocks cut from the ROM (work\scratch\racer-n64\blocks\*.bin, big-endian):
  model block   u32 count, then (maskOffset, dataOffset) pairs, then u32 end
  spline block  u32 count, then offsets, then u32 end
  texture block u32 count, then (pixelsOffset, paletteOffset) pairs, then u32 end
  sprite block  u32 count, then offsets, then u32 end
Model data parts on N64 are "Comp"+"Wolf"+u32 size + LZSS stream. Masks are raw.

The LZSS decoder is a port of the logic in swe1r-assets (MIT, A. Kopetsch,
src/SWE1R.Assets.Blocks/ModelBlock/Compression/Compressor.cs, Window.cs, LengthDistancePair.cs):
4096-byte ring window initialised to zero with write position 1; flag byte read LSB first;
bit 1 = literal; bit 0 = u16 BE pair: length = (hi nibble)+2, absolute window position = low 12
bits, position 0 ends the stream.

python -I blocks.py            verify counts + decompress every model, cache to work\models\
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
BLOCKS = os.path.join(ROOT, 'blocks')
CACHE = os.path.join(ROOT, 'work', 'models')

EXPECTED = {  # name: (count, sha1) from swe1r-assets Resources\Metadata\Block.csv (n64 / n64_us)
    'modelblock': (307, 'fcadc43fd32017950fb2197fa8a6b0722f26276a'),
    'splineblock': (91, 'a18f0c66301cf0e11feedafadc1b374fa3b40ed6'),
    'spriteblock': (173, '206763a462c13a393eddc15c8bf8c4cfb5141996'),
    'textureblock': (1648, '9130090c3ceafff6ed05609045909f54f666e54f'),
}


def u32(b, o):
    return struct.unpack_from('>I', b, o)[0]


def read_block(name):
    with open(os.path.join(BLOCKS, name + '.bin'), 'rb') as f:
        return f.read()


class LZError(Exception):
    pass


def decompress(data):
    """N64 'Comp' LZSS. data starts with b'Comp'. Returns bytes; raises LZError on any mismatch."""
    if data[:4] != b'Comp':
        raise LZError('no Comp tag')
    if data[4:8] != b'Wolf':
        raise LZError('no Wolf tag (got %r)' % data[4:8])
    size = u32(data, 8)
    win = bytearray(4096)
    wp = 1
    out = bytearray()
    i = 12
    n = len(data)
    while True:
        if i >= n:
            raise LZError('stream ran out at %d (out %d of %d)' % (i, len(out), size))
        flags = data[i]
        i += 1
        for bit in range(8):
            if flags & (1 << bit):
                b = data[i]
                i += 1
                out.append(b)
                win[wp] = b
                wp = (wp + 1) & 0xFFF
            else:
                v = (data[i] << 8) | data[i + 1]
                i += 2
                src = v & 0xFFF
                if src == 0:
                    if len(out) != size:
                        raise LZError('size %d != header %d' % (len(out), size))
                    return bytes(out)
                ln = (v >> 12) + 2
                for _ in range(ln):
                    b = win[src]
                    src = (src + 1) & 0xFFF
                    out.append(b)
                    win[wp] = b
                    wp = (wp + 1) & 0xFFF


class ModelBlock:
    def __init__(self):
        self.raw = read_block('modelblock')
        self.count = u32(self.raw, 0)
        self.mask_off = [u32(self.raw, 4 + 8 * i) for i in range(self.count)]
        self.data_off = [u32(self.raw, 8 + 8 * i) for i in range(self.count)]
        self.end = u32(self.raw, 4 + 8 * self.count)

    def parts(self, i):
        m0, d0 = self.mask_off[i], self.data_off[i]
        d1 = self.mask_off[i + 1] if i + 1 < self.count else self.end
        return self.raw[m0:d0], self.raw[d0:d1]

    def model(self, i, use_cache=True):
        """(mask bytes, decompressed data bytes) of model i."""
        mask, data = self.parts(i)
        path = os.path.join(CACHE, 'model%03d.bin' % i)
        if use_cache and os.path.exists(path):
            with open(path, 'rb') as f:
                return mask, f.read()
        if data[:4] == b'Comp':
            data = decompress(data)
        if use_cache:
            os.makedirs(CACHE, exist_ok=True)
            with open(path, 'wb') as f:
                f.write(data)
        return mask, data


class SplineBlock:
    def __init__(self):
        self.raw = read_block('splineblock')
        self.count = u32(self.raw, 0)
        self.off = [u32(self.raw, 4 + 4 * i) for i in range(self.count + 1)]

    def item(self, i):
        return self.raw[self.off[i]:self.off[i + 1]]


class TextureBlock:
    def __init__(self):
        self.raw = read_block('textureblock')
        self.count = u32(self.raw, 0)
        self.pix = [u32(self.raw, 4 + 8 * i) for i in range(self.count)]
        self.pal = [u32(self.raw, 8 + 8 * i) for i in range(self.count)]
        self.end = u32(self.raw, 4 + 8 * self.count)

    def _next_after(self, off):
        nxt = self.end
        for o in self.pix + self.pal:
            if off < o < nxt:
                nxt = o
        return nxt

    def item(self, i):
        """(pixel bytes, palette bytes or None). Each part runs to the next part's start."""
        p0 = self.pix[i]
        pix = self.raw[p0:self._next_after(p0)]
        pal = None
        if self.pal[i]:
            q0 = self.pal[i]
            pal = self.raw[q0:self._next_after(q0)]
        return pix, pal


def reloc_words(mask, data_len):
    """Word indices flagged in the relocation mask (bit k MSB-first in byte k/8)."""
    out = []
    nwords = data_len // 4
    for byte_i, b in enumerate(mask):
        if not b:
            continue
        for bit in range(8):
            if b & (0x80 >> bit):
                k = byte_i * 8 + bit
                if k < nwords:
                    out.append(k)
    return out


def main():
    ok = True
    for name, (cnt, sha) in EXPECTED.items():
        raw = read_block(name)
        h = hashlib.sha1(raw).hexdigest()
        c = u32(raw, 0)
        print('%-13s count %4d (expect %4d)  sha1 %s %s' % (name, c, cnt, h, 'OK' if h == sha else 'MISMATCH'))
        ok &= (c == cnt and h == sha)
    mb = ModelBlock()
    fails = []
    comp = 0
    for i in range(mb.count):
        mask, data = mb.parts(i)
        try:
            if data[:4] == b'Comp':
                comp += 1
            m, d = mb.model(i)
            tag = d[:4].decode('latin1')
        except LZError as e:
            fails.append((i, str(e)))
            continue
        if i % 50 == 0:
            print('  model %3d %s %d bytes' % (i, tag, len(d)))
    print('models: %d, compressed %d, decompression failures %d' % (mb.count, comp, len(fails)))
    for f in fails:
        print('  FAIL model %d: %s' % f)
    sb = SplineBlock()
    tb = TextureBlock()
    print('splines %d, textures %d' % (sb.count, tb.count))
    return 0 if ok and not fails else 1


if __name__ == '__main__':
    sys.exit(main())
