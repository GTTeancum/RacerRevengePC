# Texture decoding helpers for img/mask.bin (Sega Hikaru). Run with python -I.
#   python -I tools/mask_tex.py render <off> <w> <h> <fmt> <out.png> [scale]
#   python -I tools/mask_tex.py bank <bank 0..15> <fmt> <out.png> [rowwidth]   (overview: one column per MB)
# fmts: 1555 (A1 R5 G5 B5, bit15=A), 4444 (A4 R4 G4 B4, top nibble=A), 565, i8 (gray), a1555 (ignore alpha)
import sys, numpy as np
from PIL import Image
MASK = __import__('os').path.join(__import__('os').path.dirname(__file__), '..', 'img', 'mask.bin')
def load(off, n):
    with open(MASK, 'rb') as f:
        f.seek(off); return np.frombuffer(f.read(n), dtype=np.uint8)
def texel_words(raw, w, swap=True):
    """16-bit texels of a picture w wide as an (h, w) uint32 array; swap=True undoes the 3D-texture
    pair swap (same rule as decode(); used by export_course.py for RGBA output)."""
    v = np.frombuffer(bytes(raw), dtype='<u2').astype(np.uint32)
    if swap: v = v[:v.size // 2 * 2].reshape(-1, 2)[:, ::-1].ravel()
    h = v.size // w
    return v[:h*w].reshape(h, w)
def decode(raw, fmt, w, swap=False):
    # swap=True for 3D textures: ROM holds the two texels of each 32-bit word swapped (2D images are not)
    if fmt == 'i8':
        p = raw.astype(np.uint8); h = p.size // w; g = p[:h*w].reshape(h, w)
        return np.stack([g, g, g], -1)
    v = raw.view('<u2').astype(np.uint32)
    if swap: v = v[:v.size // 2 * 2].reshape(-1, 2)[:, ::-1].ravel()
    h = v.size // w; v = v[:h*w].reshape(h, w)
    if fmt in ('1555', 'a1555'):
        r = (v >> 10) & 31; g = (v >> 5) & 31; b = v & 31
        rgb = np.stack([r, g, b], -1) * 255 // 31
        if fmt == '1555':  # show transparent (bit15=0) texels as magenta checker-ish dim
            a = (v >> 15) & 1
            rgb = np.where(a[..., None] == 1, rgb, rgb // 3 + np.array([60, 0, 60]))
    elif fmt == '1555bgr':  # A1 B5 G5 R5
        b = (v >> 10) & 31; g = (v >> 5) & 31; r = v & 31
        rgb = np.stack([r, g, b], -1) * 255 // 31
    elif fmt == '4444':
        r = (v >> 8) & 15; g = (v >> 4) & 15; b = v & 15
        rgb = np.stack([r, g, b], -1) * 17
    elif fmt == '4444bgr':  # A4 B4 G4 R4 (top nibble alpha), composited over a gray checker
        a = ((v >> 12) & 15) * 17; b = ((v >> 8) & 15) * 17; g = ((v >> 4) & 15) * 17; r = (v & 15) * 17
        yy, xx = np.indices(v.shape); bg = np.where(((yy // 4 + xx // 4) & 1) == 1, 160, 100)
        rgb = np.stack([(c * a + bg * (255 - a)) // 255 for c in (r, g, b)], -1)
    elif fmt == 'auto':
        raise ValueError('auto handled by caller')
    elif fmt == '4444a':  # alpha channel only
        a = (v >> 12) & 15; rgb = np.stack([a, a, a], -1) * 17
    elif fmt == '565':
        r = (v >> 11) & 31; g = (v >> 5) & 63; b = v & 31
        rgb = np.stack([r * 255 // 31, g * 255 // 63, b * 255 // 31], -1)
    return rgb.astype(np.uint8)
def bpp(fmt): return 1 if fmt == 'i8' else 2
if __name__ == '__main__':
    cmd = sys.argv[1]
    if cmd == 'render':
        off, w, h = (int(x, 0) for x in sys.argv[2:5]); fmt, out = sys.argv[5], sys.argv[6]
        sc = int(sys.argv[7]) if len(sys.argv) > 7 else 1
        im = Image.fromarray(decode(load(off, w*h*bpp(fmt)), fmt, w))
        if sc != 1: im = im.resize((im.width*sc, im.height*sc), Image.NEAREST)
        im.save(out)
    elif cmd == 'sheet':  # contact sheet of table textures: sheet <tsv> <first> <count> <fmt> <out.png> [thumb]
        import csv
        from PIL import ImageDraw
        rows = list(csv.DictReader(open(sys.argv[2]), delimiter='	'))
        a, n, fmt, out = int(sys.argv[3]), int(sys.argv[4]), sys.argv[5], sys.argv[6]
        T = int(sys.argv[7]) if len(sys.argv) > 7 else 96
        cols = 16; sel = rows[a:a+n]; rws = (len(sel) + cols - 1) // cols
        sheet = Image.new('RGB', (cols*T, rws*(T+12)), (90, 90, 90)); dr = ImageDraw.Draw(sheet)
        for k, r in enumerate(sel):
            w, h, off = int(r['w']), int(r['h']), int(r['mask_off'], 16)
            f2 = fmt if fmt != 'auto' else ('4444bgr' if int(r['hdr1'], 16) & 0x04000000 else '1555bgr')
            im = Image.fromarray(decode(load(off, w*h*2), f2, w, swap=True))
            sc = T / max(w, h); im = im.resize((max(1, int(w*sc)), max(1, int(h*sc))), Image.BILINEAR)
            x, y = (k % cols)*T, (k // cols)*(T+12)
            sheet.paste(im, (x, y + 12)); dr.text((x+2, y), '%d %X' % (a+k, off), fill=(255, 255, 0))
        sheet.save(out)
    elif cmd == 'bank':
        bank = int(sys.argv[2], 0); fmt, out = sys.argv[3], sys.argv[4]
        rw = int(sys.argv[5], 0) if len(sys.argv) > 5 else 256
        cols = []
        for mb in range(16):
            o = (bank*16 + mb) << 20
            img = decode(load(o, 1 << 20), fmt, rw)
            img = img[::2, ::2]  # downsample 2x
            cols.append(img); cols.append(np.full((img.shape[0], 4, 3), 255, np.uint8))
        Image.fromarray(np.concatenate(cols, 1)).save(out)
