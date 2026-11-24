#!/usr/bin/env python3
r"""N64 texture decoding for the Racer texture block.

The texture block holds raw pixels (+ palette for CI); format / size come from the model's
MaterialTexture (format word = G_IM_FMT << 8 | G_IM_SIZ):
  0x0000 RGBA16 (5551)  0x0003 RGBA32   0x0200 CI4   0x0201 CI8 (palette RGBA5551)
  0x0300 IA4  0x0301 IA8  0x0302 IA16   0x0400 I4   0x0401 I8
(only 0x0003, 0x0200, 0x0201, 0x0400, 0x0401 occur in the track models).
Row 0 of the data is texture row t = 0; we write it as the PNG's top row.
I formats carry intensity in colour and alpha (N64 semantics).

swizzled=True undoes the TMEM odd-row interleave (on odd rows the two 32-bit halves of every
64-bit word are swapped; 64-bit halves for RGBA32). Applied to textures that carry a mip chain
(a second tile spec), as the PC port's deswizzle does (SW_RACER_RE renderer_hook.cpp facts).
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import blocks as B  # noqa: E402

BPP = {0x0000: 16, 0x0002: 16, 0x0003: 32, 0x0200: 4, 0x0201: 8, 0x0300: 4, 0x0301: 8, 0x0302: 16,
       0x0400: 4, 0x0401: 8}
FORMAT_NAMES = {0x0000: 'RGBA16', 0x0002: 'RGBA16', 0x0003: 'RGBA32', 0x0200: 'CI4', 0x0201: 'CI8',
                0x0300: 'IA4', 0x0301: 'IA8', 0x0302: 'IA16', 0x0400: 'I4', 0x0401: 'I8'}


def rgba5551(v):
    v = np.asarray(v, np.uint32)
    r = ((v >> 11) & 31) * 255 // 31
    g = ((v >> 6) & 31) * 255 // 31
    b = ((v >> 1) & 31) * 255 // 31
    a = (v & 1) * 255
    return np.stack([r, g, b, a], -1).astype(np.uint8)


def unswizzle(raw, w, h, bpp):
    row = w * bpp // 8
    unit = 8 if bpp == 32 else 4
    if row % (2 * unit):
        return raw
    a = bytearray(raw)
    for y in range(1, h, 2):
        o = y * row
        if o + row > len(a):
            break
        r = a[o:o + row]
        for x in range(0, row, 2 * unit):
            a[o + x:o + x + unit], a[o + x + unit:o + x + 2 * unit] = r[x + unit:x + 2 * unit], r[x:x + unit]
    return bytes(a)


def decode(pix, pal, fmt, w, h, swizzled=False):
    """Returns (H, W, 4) uint8 RGBA, plus a note string ('' if all well)."""
    bpp = BPP.get(fmt)
    if bpp is None:
        return np.full((h, w, 4), (255, 0, 255, 255), np.uint8), 'unknown format 0x%04x' % fmt
    need = (w * h * bpp + 7) // 8
    note = ''
    if len(pix) < need:
        note = 'pixel data short (%d < %d)' % (len(pix), need)
        pix = pix + bytes(need - len(pix))
    raw = pix[:need]
    if swizzled:
        raw = unswizzle(raw, w, h, bpp)
    b = np.frombuffer(raw, np.uint8)
    if bpp == 4:
        v = np.empty(len(b) * 2, np.uint8)
        v[0::2] = b >> 4
        v[1::2] = b & 15
        v = v[:w * h]
    elif bpp == 8:
        v = b[:w * h]
    elif bpp == 16:
        v = b[:w * h * 2].view('>u2')
    else:
        v = b[:w * h * 4].reshape(-1, 4)
    if fmt in (0x0200, 0x0201):
        n = 16 if fmt == 0x0200 else 256
        if pal is None:
            return np.full((h, w, 4), (255, 0, 255, 255), np.uint8), 'CI texture without palette'
        p = np.frombuffer(pal[:2 * n].ljust(2 * n, b'\0'), '>u2')
        out = rgba5551(p)[v]
    elif fmt == 0x0003:
        out = v
    elif fmt in (0x0000, 0x0002):
        out = rgba5551(v)
    elif fmt == 0x0400:
        i = (v * 17).astype(np.uint8)
        out = np.stack([i, i, i, i], -1)
    elif fmt == 0x0401:
        out = np.stack([v, v, v, v], -1)
    elif fmt == 0x0300:
        i = ((v >> 1) * 255 // 7).astype(np.uint8)
        a = ((v & 1) * 255).astype(np.uint8)
        out = np.stack([i, i, i, a], -1)
    elif fmt == 0x0301:
        i = ((v >> 4) * 17).astype(np.uint8)
        a = ((v & 15) * 17).astype(np.uint8)
        out = np.stack([i, i, i, a], -1)
    else:  # IA16
        i = (v >> 8).astype(np.uint8)
        a = (v & 255).astype(np.uint8)
        out = np.stack([i, i, i, a], -1)
    return np.ascontiguousarray(out.reshape(h, w, 4)), note


def mirror_bake(img, ms, mt):
    """Bake N64 mirrored wrap into a doubled image (u, v then cover two tile repeats)."""
    if ms:
        img = np.concatenate([img, img[:, ::-1]], 1)
    if mt:
        img = np.concatenate([img, img[::-1]], 0)
    return img


class Textures:
    def __init__(self):
        self.tb = B.TextureBlock()

    def get(self, index, fmt, w, h, swizzled=False):
        pix, pal = self.tb.item(index)
        return decode(pix, pal, fmt, w, h, swizzled)


if __name__ == '__main__':
    # python -I textures.py INDEX FORMAT W H OUT.png [swz]
    from PIL import Image
    t = Textures()
    idx, fmt, w, h = int(sys.argv[1]), int(sys.argv[2], 0), int(sys.argv[3]), int(sys.argv[4])
    img, note = t.get(idx, fmt, w, h, len(sys.argv) > 6 and sys.argv[6] == 'swz')
    Image.fromarray(img).save(sys.argv[5])
    print(note or 'ok')
