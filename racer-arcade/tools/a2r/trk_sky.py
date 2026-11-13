r"""Arcade sky dome -> Revenge sky cube (skies\<code>.cub). One code path for every course.

Revenge sky (port: tests\native\sky50gw_fixture.h, recover_sky50gw.py, docs\sky-render50gw.md;
all 12 original .cub files share this layout):
  u32 palette count 1, u32 cube count 1, u32 offsets 16, 99100
  palette block (start/count/compressed, 768 RGB, 32768 + 65536 lookup tables; not used by IPUM
      images; copied byte for byte from TA's cube)
  cube: u32 format 32, u16 flags 0, u16 0, u16 face masks x6 (00ff 00ff 00ff 00ff ffff 0000: side
      faces only their upper 8 tiles, top all 16, no bottom), u32 2-bit size codes x6 (2 = 128),
      9 f32 origin (0,0,0) look (0,0,1) up (0,1,0), 12 bytes zero (80 bytes in all)
  per authored tile (face order +Z, -X, -Z, +X, +Y, -Y; tile = row*4 + column, row 0 at the top):
      11 u32 descriptor (0,32,0,16,4096,4,0,0,0,0,0), then 4 IPUM mips 16/32/64/128, each
      u32 bytes + 'ipum' u32 size u16 w u16 h u32 1, one IPU intra frame, 00 00 01 B0, 00 00 01 B1
Drawing: camera-centred (eye at origin, cube centre +1024 ft up), half extent 25,600, each tile's UV
spans 0.03125..0.96875 (4-texel margins), drawn first without depth or fog; below the cube's lower
edge the clear colour = Fog ColorRGB shows.

IPU frame (FFmpeg ipu_decode_frame): flags byte 0 (MPEG-2 intra blocks, DC precision 8 bits, linear
quantiser, table B.14, zigzag); per 16x16 macroblock: address increment '1' (not before the first),
type '01' + 5-bit quantiser code (first) or '1'; 4 Y + Cb + Cr 8x8 blocks: DC size VLC + differential,
AC coefficients each as escape (000001, 6-bit run, 12-bit level), EOB '10'; byte align.
Colour: BT.601 limited range, decoded by the port with the PS2 RGB16 conversion (pure black would read
as transparent, so the encoder keeps every texel above it).
"""
import os
import struct
import subprocess
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

HALF = 25600.0
YOFF = 1024.0
MASKS = (0x00FF, 0x00FF, 0x00FF, 0x00FF, 0xFFFF, 0x0000)
SIZES = (0xAAAA, 0xAAAA, 0xAAAA, 0xAAAA, 0xAAAAAAAA, 0)
DESCRIPTOR = (0, 32, 0, 16, 4096, 4, 0, 0, 0, 0, 0)
UV_LO, UV_HI = 0.03125, 0.96875
QCODE = 2                      # quantiser_scale_code (linear: qscale 4)

W_INTRA = np.array([
    8, 16, 19, 22, 26, 27, 29, 34, 16, 16, 22, 24, 27, 29, 34, 37, 19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40, 22, 26, 27, 29, 32, 35, 40, 48, 26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69, 27, 29, 35, 38, 46, 56, 69, 83], float).reshape(8, 8)
ZIGZAG = [0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20,
          13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59,
          52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63]
DC_LUMA = ['100', '00', '01', '101', '110', '1110', '11110', '111110', '1111110', '11111110', '111111110', '111111111']
DC_CHROMA = ['00', '01', '10', '110', '1110', '11110', '111110', '1111110', '11111110', '111111110', '1111111110',
             '1111111111']
_u = np.arange(8)
DCT = np.array([[(np.sqrt(0.5) if k == 0 else 1.0) * 0.5 * np.cos((2 * x + 1) * k * np.pi / 16) for x in _u] for k in _u])


class Bits:
    def __init__(self):
        self.parts = []

    def put(self, v, n):
        if n:
            self.parts.append(format(v & ((1 << n) - 1), '0%db' % n))

    def code(self, s):
        self.parts.append(s)

    def bytes(self):
        s = ''.join(self.parts)
        s += '0' * ((-len(s)) % 8)
        return int(s, 2).to_bytes(len(s) // 8, 'big') if s else b''


def rgb_to_ycc(rgb):
    r, g, b = [rgb[..., i].astype(np.float64) for i in range(3)]
    y = 16 + 0.257 * r + 0.504 * g + 0.098 * b
    cb = 128 - 0.148 * r - 0.291 * g + 0.439 * b
    cr = 128 + 0.439 * r - 0.368 * g - 0.071 * b
    return y, cb, cr


def encode_ipu(rgb):
    """rgb (h, w, 3) uint8, h and w multiples of 16 -> IPUM bytes (header, frame, B0, B1)."""
    h, w = rgb.shape[:2]
    assert h % 16 == 0 and w % 16 == 0
    rgb = np.maximum(rgb, 12).astype(np.uint8)   # keep away from PS2 'black = transparent'
    y, cb, cr = rgb_to_ycc(rgb)
    cb = cb.reshape(h // 2, 2, w // 2, 2).mean((1, 3))
    cr = cr.reshape(h // 2, 2, w // 2, 2).mean((1, 3))
    qscale = QCODE * 2
    bits = Bits()
    bits.put(0, 8)                                # flags
    last = [128, 128, 128]
    first = True
    for my in range(0, h, 16):
        for mx in range(0, w, 16):
            if not first:
                bits.code('1')                    # macroblock_address_increment = 1
            if first:
                bits.code('01'); bits.put(QCODE, 5)
                first = False
            else:
                bits.code('1')
            blocks = [y[my:my + 8, mx:mx + 8], y[my:my + 8, mx + 8:mx + 16],
                      y[my + 8:my + 16, mx:mx + 8], y[my + 8:my + 16, mx + 8:mx + 16],
                      cb[my // 2:my // 2 + 8, mx // 2:mx // 2 + 8], cr[my // 2:my // 2 + 8, mx // 2:mx // 2 + 8]]
            for n, blk in enumerate(blocks):
                F = DCT @ blk @ DCT.T
                comp = 0 if n < 4 else n - 3
                dc = int(np.clip(np.round(F[0, 0] / 8.0), 0, 255))
                diff = dc - last[comp]
                last[comp] = dc
                size = int(abs(diff)).bit_length()
                bits.code((DC_LUMA if n < 4 else DC_CHROMA)[size])
                if size:
                    bits.put(diff if diff > 0 else diff + (1 << size) - 1, size)
                q = np.round(16.0 * F / (qscale * W_INTRA)).astype(int).reshape(64)
                run = 0
                for zi in range(1, 64):
                    lv = int(np.clip(q[ZIGZAG[zi]], -2047, 2047))
                    if lv == 0:
                        run += 1
                        continue
                    bits.code('000001'); bits.put(run, 6); bits.put(lv, 12)
                    run = 0
                bits.code('10')                   # end of block
    frame = bits.bytes() + b'\0\0\1\xb0'
    body = struct.pack('<HHI', w, h, 1) + frame + b'\0\0\1\xb1'
    return b'ipum' + struct.pack('<I', len(body)) + body


def ipu_rgba(yuv, w, h):
    """Port / tools\\assets\\decode_source_textures50fc.py colour conversion (vectorised)."""
    n = w * h
    Y = np.frombuffer(yuv, np.uint8, n).reshape(h, w).astype(np.int64)
    U = np.frombuffer(yuv, np.uint8, n // 4, n).reshape(h // 2, w // 2).astype(np.int64)
    V = np.frombuffer(yuv, np.uint8, n // 4, n + n // 4).reshape(h // 2, w // 2).astype(np.int64)
    U = np.repeat(np.repeat(U, 2, 0), 2, 1) - 128
    V = np.repeat(np.repeat(V, 2, 0), 2, 1) - 128
    yy = (149 * np.maximum(0, Y - 16)) // 64
    ch = [(yy + (204 * V) // 64 + 1) // 2, (yy + (-104 * V) // 64 + (-50 * U) // 64 + 1) // 2, (yy + (258 * U) // 64 + 1) // 2]
    D = np.array(((-4, 0, -3, 1), (2, -2, 3, -1), (-3, 1, -4, 0), (3, -1, 2, -2)))
    Dt = np.tile(D, (h // 4 + 1, w // 4 + 1))[:h, :w]
    rgb = [(np.clip(np.clip(c, 0, 255) + Dt, 0, 255) // 8) * 8 for c in ch]
    out = np.stack(rgb + [np.where((rgb[0] + rgb[1] + rgb[2]) > 0, 255, 0)], -1).astype(np.uint8)
    return out


def ffmpeg_path():
    import shutil
    return shutil.which('ffmpeg')


def decode_ipum(data, ffmpeg=None):
    """Decode one IPUM with FFmpeg exactly as the repo's 50FC tool does -> RGBA (h, w, 4)."""
    ffmpeg = ffmpeg or ffmpeg_path()
    magic, size, w, h, frames = struct.unpack_from('<4sIHHI', data)
    assert magic == b'ipum' and size == len(data) - 8 and frames == 1 and data[16] == 0
    assert data[-8:] == b'\0\0\1\xb0\0\0\1\xb1'
    cmd = [ffmpeg, '-hide_banner', '-loglevel', 'error', '-nostdin', '-cpuflags', '0', '-flags', '+bitexact', '-idct', 'simple',
           '-threads', '1', '-err_detect', 'explode', '-f', 'ipu', '-i', 'pipe:0', '-frames:v', '1', '-pix_fmt', 'yuv420p',
           '-f', 'rawvideo', 'pipe:1']
    p = subprocess.run(cmd, input=data[:-4], capture_output=True, timeout=60)
    if p.returncode or p.stderr or len(p.stdout) != w * h * 3 // 2:
        raise ValueError('FFmpeg IPU decode failed: %r' % p.stderr[:300])
    return ipu_rgba(p.stdout, w, h)


# ---------------------------------------------------------------- cube geometry (port sky50gw)
def face_point(face, gx, gy):
    """Grid coordinates (0..512, y down) of a face -> cube-space position (centre at the origin)."""
    right = (gx - 256.0) * 100.0
    up = (256.0 - gy) * 100.0
    if face == 0:
        return np.stack([right, up, np.full_like(right, HALF)], -1)
    if face == 1:
        return np.stack([np.full_like(right, -HALF), up, right], -1)
    if face == 2:
        return np.stack([-right, up, np.full_like(right, -HALF)], -1)
    if face == 3:
        return np.stack([np.full_like(right, HALF), up, -right], -1)
    if face == 4:
        return np.stack([right, np.full_like(right, HALF), -up], -1)
    raise ValueError(face)


def tile_directions(face, tile, n=128):
    """Unit view directions (Revenge axes, eye at the origin) of every texel centre of a tile."""
    col, row = tile % 4, tile // 4
    s = (np.arange(n) + 0.5) / n
    g = (s - UV_LO) / (UV_HI - UV_LO) * 128.0
    gx, gy = np.meshgrid(col * 128 + g, row * 128 + g)
    p = face_point(face, gx, gy) + np.array([0.0, YOFF, 0.0])
    return p / np.linalg.norm(p, axis=-1, keepdims=True)


def authored_tiles():
    return [(f, t) for f in range(6) for t in range(16) if MASKS[f] & (1 << t)]


def lookup(dirs, tiles_rgb, clear):
    """Port drawing rule for verification: direction (Revenge axes) -> sky colour or the clear
    colour where no authored tile is hit. tiles_rgb: {(face, tile): (128,128,3)}."""
    d = dirs.reshape(-1, 3)
    out = np.tile(np.array(clear, np.float64), (len(d), 1))
    best_t = np.full(len(d), np.inf)
    for face in range(5):
        axis, sign = {0: (2, 1), 1: (0, -1), 2: (2, -1), 3: (0, 1), 4: (1, 1)}[face]
        c = HALF + (YOFF if axis == 1 else 0.0)
        with np.errstate(all='ignore'):
            t = (sign * c) / d[:, axis]
        ok = (t > 0) & (t < best_t)
        p = d * t[:, None] - np.array([0.0, YOFF, 0.0])
        if face == 0:
            gx, gy = p[:, 0] / 100 + 256, 256 - p[:, 1] / 100
        elif face == 1:
            gx, gy = p[:, 2] / 100 + 256, 256 - p[:, 1] / 100
        elif face == 2:
            gx, gy = -p[:, 0] / 100 + 256, 256 - p[:, 1] / 100
        elif face == 3:
            gx, gy = -p[:, 2] / 100 + 256, 256 - p[:, 1] / 100
        else:
            gx, gy = p[:, 0] / 100 + 256, 256 + p[:, 2] / 100
        ok &= (gx >= 0) & (gx <= 512) & (gy >= 0) & (gy <= 512)
        col = np.clip((gx // 128).astype(int), 0, 3); row = np.clip((gy // 128).astype(int), 0, 3)
        tile = row * 4 + col
        authored = (MASKS[face] >> tile) & 1
        sel = ok & (authored == 1)
        best_t = np.where(ok, t, best_t)
        out[ok & (authored == 0)] = clear
        idx = np.flatnonzero(sel)
        for tl in set(tile[idx].tolist()):
            ii = idx[tile[idx] == tl]
            fx = (gx[ii] - (tl % 4) * 128) / 128; fy = (gy[ii] - (tl // 4) * 128) / 128
            u = UV_LO + fx * (UV_HI - UV_LO); v = UV_LO + fy * (UV_HI - UV_LO)
            img = tiles_rgb[(face, tl)]
            out[ii] = img[np.clip((v * 128).astype(int), 0, 127), np.clip((u * 128).astype(int), 0, 127), :3]
    return out.reshape(dirs.shape[:-1] + (3,))


# ---------------------------------------------------------------- arcade dome rendering
def render_tiles(sky_tris, X, eye_arcade, textures):
    """Ray-cast the arcade dome (decoded triangles in ARCADE space, with UV and texture id) from
    eye_arcade into every authored tile. Returns {(face, tile): (128,128,3) uint8}, miss counts."""
    P = np.array([t['P'] for t in sky_tris], np.float64) - eye_arcade            # (m, 3, 3)
    UV = np.array([t['UV'] for t in sky_tris], np.float64)
    tid = [t['tid'] for t in sky_tris]
    v0, e1, e2 = P[:, 0], P[:, 1] - P[:, 0], P[:, 2] - P[:, 0]
    out, misses = {}, {}
    zs = X['z_sign']
    for face, tile in authored_tiles():
        d = tile_directions(face, tile).reshape(-1, 3) * np.array([1.0, 1.0, zs])   # Revenge -> arcade
        best = np.full(len(d), np.inf); bi = np.full(len(d), -1); bu = np.zeros(len(d)); bv = np.zeros(len(d))
        for k in range(len(P)):
            pv = np.cross(d, e2[k]); det = pv @ e1[k]
            with np.errstate(all='ignore'):
                inv = 1.0 / det
                tv = -v0[k]
                u = (pv @ tv) * inv
                qv = np.cross(tv, e1[k])
                v = (d @ qv) * inv
                t = (qv @ e2[k]) * inv
            hit = (np.abs(det) > 1e-12) & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > 0) & (t < best)
            best[hit] = t[hit]; bi[hit] = k; bu[hit] = u[hit]; bv[hit] = v[hit]
        rgb = np.zeros((len(d), 3))
        hitm = bi >= 0
        for k in set(bi[hitm].tolist()):
            ii = np.flatnonzero(bi == k)
            uv = UV[k, 0] * (1 - bu[ii] - bv[ii])[:, None] + UV[k, 1] * bu[ii][:, None] + UV[k, 2] * bv[ii][:, None]
            tex = textures.get(tid[k])
            if tex is None:
                rgb[ii] = 200
                continue
            th, tw = tex.shape[:2]
            x = uv[:, 0] * tw - 0.5; y = uv[:, 1] * th - 0.5
            x0 = np.floor(x).astype(int); y0 = np.floor(y).astype(int); fx = x - x0; fy = y - y0
            def s(yy, xx):
                return tex[yy % th, xx % tw, :3].astype(np.float64)
            rgb[ii] = (s(y0, x0) * ((1 - fx) * (1 - fy))[:, None] + s(y0, x0 + 1) * (fx * (1 - fy))[:, None] +
                       s(y0 + 1, x0) * ((1 - fx) * fy)[:, None] + s(y0 + 1, x0 + 1) * (fx * fy)[:, None])
        img = rgb.reshape(128, 128, 3)
        miss = ~hitm.reshape(128, 128)
        misses[(face, tile)] = int(miss.sum())
        out[(face, tile)] = (img, miss)
    # fill rays that miss the dome (below its rim) with the colour of the lowest hit in their column
    # on the same face, so no dome edge can show
    for face in range(5):
        tl = [t for f, t in authored_tiles() if f == face]
        if face < 4:
            for c in range(4):
                col_tiles = [t for t in tl if t % 4 == c]
                stack = np.concatenate([out[(face, t)][0] for t in sorted(col_tiles)], 0)
                mstack = np.concatenate([out[(face, t)][1] for t in sorted(col_tiles)], 0)
                for x in range(128):
                    good = np.flatnonzero(~mstack[:, x])
                    if len(good):
                        last = good.max()
                        stack[last + 1:, x] = stack[last, x]
                        first = good.min()
                        stack[:first, x] = stack[first, x]
                for i, t in enumerate(sorted(col_tiles)):
                    out[(face, t)] = (stack[i * 128:(i + 1) * 128], np.zeros((128, 128), bool))
    res = {}
    for k, (img, m) in out.items():
        if m.any():
            img = img.copy(); img[m] = img[~m].mean(0) if (~m).any() else 128
        res[k] = np.clip(np.round(img), 0, 255).astype(np.uint8)
    return res, misses


def mips(img):
    lv = [img]
    while lv[-1].shape[0] > 16:
        a = lv[-1].astype(np.float64)
        h = a.shape[0] // 2
        lv.append(np.round(a.reshape(h, 2, h, 2, 3).mean((1, 3))).astype(np.uint8))
    return lv[::-1]    # 16, 32, 64, 128


def build_cub(tiles, palette_block):
    """tiles {(face, tile): (128,128,3)} -> CUB bytes (layout in the module doc)."""
    assert len(palette_block) == 99100 - 16
    out = bytearray(struct.pack('<4I', 1, 1, 16, 99100)) + palette_block
    hdr = struct.pack('<IHH', 32, 0, 0) + struct.pack('<6H', *MASKS) + struct.pack('<6I', *SIZES) + \
        struct.pack('<9f', 0, 0, 0, 0, 0, 1, 0, 1, 0)
    hdr += b'\0' * (80 - len(hdr))
    out += hdr
    for face, tile in authored_tiles():
        out += struct.pack('<11I', *DESCRIPTOR)
        for im in mips(tiles[(face, tile)]):
            ipu = encode_ipu(im)
            out += struct.pack('<I', len(ipu)) + ipu
    return bytes(out)


def read_cub(b):
    """-> dict(masks, sizes, pose, tiles={(face, tile): [ipum bytes x4]}) with every check the port does."""
    assert struct.unpack_from('<4I', b) == (1, 1, 16, 99100), 'CUB tables'
    assert struct.unpack_from('<3I', b, 16) == (0, 81, 0), 'CUB palette header'
    at = 99100
    assert struct.unpack_from('<IHH', b, at) == (32, 0, 0), 'format/flags'
    masks = struct.unpack_from('<6H', b, at + 8); sizes = struct.unpack_from('<6I', b, at + 20)
    pose = struct.unpack_from('<9f', b, at + 44)
    at += 80
    tiles = {}
    for face, mask in enumerate(masks):
        for tile in range(16):
            if not mask & (1 << tile):
                continue
            assert (sizes[face] >> (tile * 2)) & 3 == 2
            assert struct.unpack_from('<11I', b, at) == DESCRIPTOR, 'tile descriptor'
            at += 44
            lv = []
            for dim in (16, 32, 64, 128):
                n, = struct.unpack_from('<I', b, at); at += 4
                ipu = b[at:at + n]
                magic, size, w, h, fr = struct.unpack_from('<4sIHHI', ipu)
                assert (magic, size, w, h, fr) == (b'ipum', n - 8, dim, dim, 1) and ipu[16] == 0
                assert ipu[-8:] == b'\0\0\1\xb0\0\0\1\xb1' and 25 <= n <= 65536
                lv.append(ipu); at += n
            tiles[(face, tile)] = lv
    assert at == len(b) and len(tiles) == 48, 'tile closure'
    return dict(masks=masks, sizes=sizes, pose=pose, tiles=tiles)


FADE_ROWS = 12   # texel rows above the lower authored edge that fade into the clear colour (G)


def fade_horizon(tiles):
    """Fade the lowest authored rows (side faces, tile row 1) into one horizon colour, so the edge
    between the cube and the clear colour below it cannot be seen. Returns that colour, which is
    written as Fog ColorRGB (the port's clear colour). Modifies tiles in place."""
    band = np.concatenate([tiles[(f, t)][128 - FADE_ROWS - 4:124].reshape(-1, 3).astype(np.float64)
                           for f in range(4) for t in (4, 5, 6, 7)])
    clear = band.mean(0)
    rows = np.arange(128)
    w = np.clip((rows - (124 - FADE_ROWS)) / FADE_ROWS, 0, 1)[:, None, None]   # 0 above, 1 at the edge
    for f in range(4):
        for t in (4, 5, 6, 7):
            img = tiles[(f, t)].astype(np.float64)
            tiles[(f, t)] = np.clip(np.round(img * (1 - w) + clear * w), 0, 255).astype(np.uint8)
    return tuple(int(round(c)) for c in clear)
