r"""PST revision 1 reader / writer and RGBA -> PST kind 1 encoder (Racer Revenge textures).

Format (src\assets\import_texture48e.cpp:109-200, docs\asset-decoder48b.md):
  u16 revision 1, u16 flags 0, u32 kind (0 = 4-bit/16 colours, 1 = 8-bit/256, 2 = IPU-compressed 'ipum' level, u32 size first),
  u32 mipCount 1..7, palette (16 or 256 entries x RGBA8; none for kind 2),
  per mip, SMALLEST FIRST, each level twice the previous: u32 height, u32 width, u32 0, u32 0,
  texels (kind 0: w*h/2, low nibble first; kind 1: w*h; kind 2: u32 n + n bytes 'ipum' stream).
Kind 1 colour lookup: rgba = palette[swap_bits_3_4(index)] (GS CSM1 layout, import_texture48e.cpp:189;
docs tools\assets\decode_texture48d.py palette_index). Rows are stored top row first (row 0 = T 0).
Alpha is raw 0..255; opaque = 255 in the originals (palettes of every track).

encode_rgba(levels) writes kind 1 from RGBA images: one 256-colour palette for all levels
(median cut on the base level, then nearest colour for every level), levels smallest first.
"""
import struct

import numpy as np


class PstError(ValueError):
    pass


def _req(c, m):
    if not c:
        raise PstError(m)


def perm(i):
    """Swap bits 3 and 4 of a palette index (an involution)."""
    return (i & ~24) | ((i & 8) << 1) | ((i & 16) >> 1)


PERM = np.array([perm(i) for i in range(256)], np.uint8)


def parse(blob):
    rev, flags, kind, mips = struct.unpack_from('<HHII', blob, 0)
    _req(rev == 1 and flags == 0 and kind in (0, 1, 2), 'PST variant %d/%d/%d' % (rev, flags, kind))
    _req(1 <= mips <= 7, 'mip count')
    npal = {0: 16, 1: 256, 2: 0}[kind]
    at = 12
    palette = blob[at:at + 4 * npal]
    at += 4 * npal
    levels = []
    for _ in range(mips):
        h, w, a, b = struct.unpack_from('<4I', blob, at)
        _req((a, b) == (0, 0), 'mip words')
        if kind == 2:  # IPU-compressed level ('ipum' stream): u32 byte count, then the stream
            nb, = struct.unpack_from('<I', blob, at + 16)
            levels.append(dict(w=w, h=h, data=blob[at + 20:at + 20 + nb], ipu=True))
            at += 20 + nb
            continue
        nb = {0: w * h // 2, 1: w * h}[kind]
        levels.append(dict(w=w, h=h, data=blob[at + 16:at + 16 + nb]))
        at += 16 + nb
    _req(at == len(blob), 'PST suffix %d/%d' % (at, len(blob)))
    return dict(kind=kind, palette=palette, levels=levels)


def write(t):
    out = bytearray(struct.pack('<HHII', 1, 0, t['kind'], len(t['levels'])))
    out += t['palette']
    for lv in t['levels']:
        out += struct.pack('<4I', lv['h'], lv['w'], 0, 0)
        if lv.get('ipu'):
            out += struct.pack('<I', len(lv['data']))
        out += lv['data']
    return bytes(out)


def rgba_levels(t):
    """Decode kind 0/1 to RGBA arrays (h, w, 4), smallest first (port decoder rule)."""
    pal = np.frombuffer(t['palette'], np.uint8).reshape(-1, 4)
    out = []
    for lv in t['levels']:
        d = np.frombuffer(lv['data'], np.uint8)
        if t['kind'] == 1:
            idx = PERM[d]
        elif t['kind'] == 0:
            idx = np.stack([d & 15, d >> 4], -1).ravel()
        else:
            raise PstError('kind 2 RGBA not needed here')
        out.append(pal[idx].reshape(lv['h'], lv['w'], 4))
    return out


# ---------------------------------------------------------------- encoder
def _median_cut(px, n):
    """px: (k, 4) uint8 unique-ish colours with weights folded in by repetition. Returns (n, 4)."""
    boxes = [px.astype(np.int32)]
    while len(boxes) < n:
        # split the box with the largest range * population
        best, bi = -1, -1
        for i, b in enumerate(boxes):
            if len(b) < 2:
                continue
            r = (b.max(0) - b.min(0)).max() * np.sqrt(len(b))
            if r > best:
                best, bi = r, i
        if bi < 0 or best <= 0:
            break
        b = boxes.pop(bi)
        ch = int(np.argmax(b.max(0) - b.min(0)))
        b = b[np.argsort(b[:, ch], kind='stable')]
        h = len(b) // 2
        boxes += [b[:h], b[h:]]
    return np.array([np.round(b.mean(0)) for b in boxes if len(b)], np.int32).clip(0, 255)


def quantise(base, n=256):
    """base: (h, w, 4) uint8. Returns palette (n, 4) uint8 (unused entries repeat the last)."""
    px = base.reshape(-1, 4)
    uniq = np.unique(px, axis=0)
    if len(uniq) <= n:
        pal = uniq.astype(np.int32)
    else:
        pal = _median_cut(px, n)
        # two k-means refinement passes (alpha weighted like colour)
        for _ in range(2):
            idx = nearest(px, pal)
            for k in range(len(pal)):
                sel = px[idx == k]
                if len(sel):
                    pal[k] = np.round(sel.mean(0))
    pal = np.vstack([pal, np.repeat(pal[-1:], n - len(pal), 0)]) if len(pal) < n else pal[:n]
    return pal.astype(np.uint8)


def nearest(px, pal, chunk=65536):
    px = px.astype(np.int32)
    pal = pal.astype(np.int32)
    out = np.empty(len(px), np.int32)
    for s in range(0, len(px), chunk):
        d = ((px[s:s + chunk, None, :] - pal[None, :, :]) ** 2).sum(-1)
        out[s:s + chunk] = d.argmin(1)
    return out


def box_mips(base, min_side=16, max_levels=7):
    """Level 0 first: base, then 2x2 box-filtered halves while both sides stay >= min_side."""
    levels = [base]
    while len(levels) < max_levels:
        a = levels[-1].astype(np.float32)
        h, w = a.shape[:2]
        if h // 2 < min_side or w // 2 < min_side:
            break
        a = a.reshape(h // 2, 2, w // 2, 2, 4).mean((1, 3))
        levels.append(np.round(a).clip(0, 255).astype(np.uint8))
    return levels


def encode_rgba(base, max_side=1024, min_side=16, max_levels=7):
    """base (h, w, 4) uint8, power-of-two sides. Returns (pst bytes, info)."""
    h, w = base.shape[:2]
    _req(h & (h - 1) == 0 and w & (w - 1) == 0, 'power of two')
    while h > max_side or w > max_side:
        base = base.reshape(h // 2, 2, w // 2, 2, 4).mean((1, 3)).round().astype(np.uint8)
        h, w = base.shape[:2]
    if h < min_side or w < min_side:  # port rejects levels below 16: upscale by repetition
        fy, fx = max(1, min_side // h), max(1, min_side // w)
        base = np.repeat(np.repeat(base, fy, 0), fx, 1)
    levels = box_mips(base, min_side, max_levels)
    pal = quantise(levels[0])
    lv_out = []
    err = []
    for im in reversed(levels):  # smallest first
        idx = nearest(im.reshape(-1, 4), pal)
        err.append(float(np.abs(pal[idx].astype(int) - im.reshape(-1, 4)).mean()))
        lv_out.append(dict(w=im.shape[1], h=im.shape[0], data=PERM[idx.astype(np.uint8)].tobytes()))
    t = dict(kind=1, palette=pal.tobytes(), levels=lv_out)
    return write(t), dict(w=int(base.shape[1]), h=int(base.shape[0]), mips=len(levels),
                          mean_abs_error_per_channel=round(err[-1], 3))
