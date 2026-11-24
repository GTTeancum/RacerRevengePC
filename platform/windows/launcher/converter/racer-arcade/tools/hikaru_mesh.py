#!/usr/bin/env python3
"""Hikaru (315-6083 Antarctic) display-list -> triangle mesh decoder, as a library.

Triangle assembly follows Odin exactly (ref\\odin-src\\src\\odin\\machine\\chips\\antarctic\\
antarctic_mesh.cpp Antarctic::push_vertices / add_triangle / begin_mesh, antarctic.cpp run loop,
antarctic_ops.cpp op_push_*):

- a ring of 3 vertices, tmp(0) oldest .. tmp(2) newest (index permutation `at`);
- a position push (12x/1Ax/1Bx) rotates the ring (oldest leaves), or with the p(osition)pivot
  bit (header bit 1) swaps at[1]/at[2] so tmp(0) stays as fan pivot; the new vertex is tmp(2);
  a 12x/1Ax push clears texcoords of that vertex, a 1Bx sets everything;
- 158 (one UV) sets tmp(2)'s texcoords; 0E8 (three UVs) sets tmp(0..2) and may switch texhead;
- whichever push has triangle bits (header bits 11:9) == 7 closes a triangle: tmp(2) takes the
  header of that push (so the 158's winding/no-cull bits count), then (tmp0, tmp1, tmp2), swapped
  to (tmp0, tmp2, tmp1) when the winding bit (bit 0) is set; degenerate (zero-area) triangles are
  dropped; bit 23 (no cull) emits the back face too (flag kept, back face optional);
- the ring is cleared only at list start; meshes split at any non-mesh instruction (recalls etc.)
  but the last vertices carry on into the next mesh (strips run through a recall);
- a mesh takes the material / texhead / poly type current when its first vertex comes.

Static vertex (12C-12F): position = (int32(word) >> 10) * 2^(p-0x8D) / 64 with p from the last
901; normal = int16((word & 0x3FF) << 6) / 16384.

UVs are returned in texels (Odin read_texcoords: s16/16); divide by the texture size (and multiply
by 16 if the texhead 2C1 bit 14 "whole texels" is set) to normalise, as Odin copy_texcoords does.

API:
    rom = Rom(img_dir)                      # mask.bin + eprom.bin, bus-address reader
    m = decode(rom.read(addr, size))        # or decode(bytes)
    m.positions, m.normals, m.uvs, m.alphas # per vertex
    m.tris                                  # list of (a, b, c, mesh_index, flags)
    m.meshes                                # list of dict(mat, tex, polytype, alpha, pattern, bias, ...)
    m.ops                                   # Counter of opcodes seen (for checks)
Run as a script: python -I hikaru_mesh.py IMG_DIR BUSADDR SIZE [out.obj]
"""
import os
import struct
import sys
from collections import Counter

PUSH_STATIC = range(0x12C, 0x130)
PUSH_DYN = range(0x1AC, 0x1B0)
PUSH_ALL = range(0x1B8, 0x1C0)
PUSH_UV1 = range(0x158, 0x15C)
PUSH_UV3 = (0x0E8, 0x0E9)
# opcodes that keep an open mesh open (antarctic_ops.cpp insns(): PUSHES or KEEPS_MESH)
KEEPS = set(PUSH_STATIC) | set(PUSH_DYN) | set(PUSH_ALL) | set(PUSH_UV1) | set(PUSH_UV3) | {0x000, 0x081, 0x091}
OPENS = set(PUSH_STATIC) | set(PUSH_DYN) | set(PUSH_ALL) | set(PUSH_UV1) | set(PUSH_UV3)
KNOWN = OPENS | {0x000, 0x003, 0x004, 0x005, 0x006, 0x011, 0x012, 0x021, 0x043, 0x046, 0x051, 0x052,
                 0x055, 0x061, 0x064, 0x081, 0x082, 0x083, 0x084, 0x088, 0x091, 0x095, 0x0C1, 0x0C3,
                 0x0C4, 0x0D1, 0x101, 0x103, 0x104, 0x113, 0x154, 0x161, 0x181, 0x191, 0x194, 0x1A1,
                 0x1C2}


def insn_size(w):
    return 1 << (((w >> 4) & 3) + 2)


def _f32(w):
    return struct.unpack('<f', struct.pack('<I', w & 0xFFFFFFFF))[0]


def _s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def _s32(v):
    v &= 0xFFFFFFFF
    return v - 0x100000000 if v & 0x80000000 else v


class Rom:
    """Reads ROM-board bus addresses as the program uses them (notes\\program-map.md 2)."""

    def __init__(self, img_dir):
        self.mask = open(os.path.join(img_dir, 'mask.bin'), 'rb').read()
        self.eprom = open(os.path.join(img_dir, 'eprom.bin'), 'rb').read()

    def locate(self, addr):
        if addr >> 28 == 0xA or addr >> 28 == 0x2:
            return self.mask, addr & 0x0FFFFFFF
        top = addr >> 24
        if top in (0x90, 0x91, 0x10, 0x11):
            return self.eprom, (top & 1) * 0x800000 + (addr & 0xFFFFFF)
        if 0x0C010000 <= addr < 0x0C400000:
            return self.eprom, addr - 0x0C00FC00
        raise ValueError('not a ROM/program address: %08X' % addr)

    def read(self, addr, size):
        buf, off = self.locate(addr)
        return buf[off:off + size]

    def words(self, addr, n):
        return struct.unpack_from('<%dI' % n, *self.locate(addr))

    def floats(self, addr, n):
        return struct.unpack_from('<%df' % n, *self.locate(addr))


class _V:
    """A ring vertex (Antarctic::Vertex): header, body, listed index (NOT_LISTED = None)."""
    __slots__ = ('info', 'pos', 'nrm', 'uv', 'alpha', 'listed', 'mv')

    def __init__(self):
        self.info = 0
        self.pos = (0.0, 0.0, 0.0)
        self.nrm = (0.0, 0.0, 0.0)
        self.uv = (0.0, 0.0)
        self.alpha = 255
        self.listed = None
        self.mv = 0


class Model:
    def __init__(self):
        self.positions = []
        self.normals = []
        self.uvs = []
        self.alphas = []
        self.tris = []          # (a, b, c, mesh, flags) flags: 1 = no cull (both faces)
        self.meshes = []
        self.ops = Counter()
        self.unknown = []       # (offset, word) of words that are no CP instruction
        self.calls = []         # (offset, word0, word1) jumps/calls/LOD found
        self.end = 0

    def bbox(self):
        if not self.positions:
            return None
        xs, ys, zs = zip(*self.positions)
        return (min(xs), min(ys), min(zs)), (max(xs), max(ys), max(zs))


def decode(buf, stop_on_return=True, start=0, texheads=None):
    """Decodes a CP list in `buf` (bytes) from `start`. texheads: optional dict index -> (2C1 word)
    used only to apply the 0E8 texhead switch; not needed for the course data."""
    m = Model()
    ring = [_V(), _V(), _V()]
    at = [0, 1, 2]
    num_verts = 0
    state = dict(mat=None, tex=None, mat_base=0, tex_base=0, polytype=0, poly_alpha=255,
                 pattern=0, bias=0, precision=1.0, mat_set=None)
    in_mesh = False
    cur = None
    off = start
    n = len(buf)

    def tmp(i):
        return ring[at[i]]

    def begin_mesh(addr):
        nonlocal cur, num_verts
        cur = len(m.meshes)
        m.meshes.append(dict(mat=state['mat'], tex=state['tex'], polytype=state['polytype'],
                             poly_alpha=state['poly_alpha'], pattern=state['pattern'],
                             bias=state['bias'], precision=state['precision'], addr=addr))
        if num_verts > 3:
            num_verts = 3
        for v in ring:          # copy_material() marks them changed: listed again in this mesh
            v.listed = None

    def list_vertex(v):
        if v.listed is None:
            v.listed = len(m.positions)
            m.positions.append(v.pos)
            m.normals.append(v.nrm)
            m.uvs.append(v.uv)
            m.alphas.append(v.alpha)
        return v.listed

    def add_triangle():
        if num_verts < 3:
            return
        a, b, c = tmp(0), tmp(1), tmp(2)
        e1 = [b.pos[i] - a.pos[i] for i in range(3)]
        e2 = [c.pos[i] - a.pos[i] for i in range(3)]
        nx = e1[1] * e2[2] - e1[2] * e2[1]
        ny = e1[2] * e2[0] - e1[0] * e2[2]
        nz = e1[0] * e2[1] - e1[1] * e2[0]
        if a.mv == b.mv == c.mv and nx * nx + ny * ny + nz * nz <= 0.0:
            return
        nocull = 1 if (c.info & 0x800000) else 0
        if c.info & 1:
            b, c = c, b
        m.tris.append((list_vertex(a), list_vertex(b), list_vertex(c), cur, nocull))

    def push(info, pos=None, nrm=None, uv=None, uv3=None):
        nonlocal num_verts
        t = (info >> 9) & 7
        if t not in (0, 7):
            m.unknown.append((off, info))
        if uv3 is None:
            if pos is not None:
                if not (info & 2):
                    out = at[0]
                    at[0] = at[1]
                    at[1] = at[2]
                    at[2] = out
                else:
                    at[1], at[2] = at[2], at[1]
                last = tmp(2)
                last.info = 0
                if nrm is None or uv is None:
                    last.nrm = (0.0, 0.0, 0.0)
                    last.uv = (0.0, 0.0)
                last.pos = pos
                # vertex alpha times polygon alpha (copy_colors)
                last.alpha = max(0, min(255, int(state['poly_alpha'] * (info >> 24) / 255.0)))
                last.mv = (1 if info & 0x1000 else 0) + (0 if info & 0x2000 else 64) + (0 if info & 0x4000 else 128)
                num_verts += 1
            if nrm is not None:
                tmp(2).nrm = nrm
            if uv is not None:
                tmp(2).uv = uv
            tmp(2).listed = None
        else:
            if num_verts < 3:
                return
            for i in range(3):
                tmp(2 - i).uv = uv3[i]
                tmp(2 - i).listed = None
        if t == 7:
            tmp(2).info = info
            add_triangle()

    while off + 4 <= n:
        w = struct.unpack_from('<I', buf, off)[0]
        size = insn_size(w)
        if off + size > n:
            break
        words = struct.unpack_from('<%dI' % (size // 4), buf, off)
        op = w & 0x1FF
        if op not in KNOWN:
            m.unknown.append((off, w))
            break
        m.ops[op] += 1
        if not in_mesh and op in OPENS:
            begin_mesh(off)
            in_mesh = True
        elif in_mesh and op not in KEEPS:
            in_mesh = False

        if op in PUSH_STATIC:
            scale = state['precision'] / 64.0
            pos = tuple((_s32(x) >> 10) * scale for x in words[1:4])
            nrm = tuple(_s16((x & 0x3FF) << 6) / 16384.0 for x in words[1:4])
            push(w, pos=pos, nrm=nrm)
        elif op in PUSH_DYN:
            push(w, pos=tuple(_f32(x) for x in words[1:4]))
        elif op in PUSH_ALL:
            push(w, pos=tuple(_f32(x) for x in words[1:4]), nrm=tuple(_f32(x) for x in words[5:8]),
                 uv=(_s16(words[4]) / 16.0, _s16(words[4] >> 16) / 16.0))
        elif op in PUSH_UV1:
            push(w, uv=(_s16(words[1]) / 16.0, _s16(words[1] >> 16) / 16.0))
        elif op in PUSH_UV3:
            idx = state['tex_base'] + (w >> 16)
            if idx != state['tex']:
                # Odin switches texhead (new mesh) when the table entry differs
                state['tex'] = idx
                in_mesh = True
                begin_mesh(off)
            push(w & 0xFFFF, uv3=[(_s16(x) / 16.0, _s16(x >> 16) / 16.0) for x in words[1:4]])
        elif op == 0x083:
            idx = w >> 16
            if w & 0x1000:
                state['mat'] = state['mat_base'] + idx
            else:
                state['mat_base'] = idx
        elif op == 0x0C3:
            idx = w >> 16
            if w & 0x1000:
                state['tex'] = (state['tex_base'] + idx) & 0xFFFF
            else:
                state['tex_base'] = idx
        elif op in (0x103, 0x113):
            state['polytype'] = (w >> 9) & 7
            state['poly_alpha'] = w >> 24
        elif op == 0x101:
            form = (w >> 8) & 0xF
            if form == 1:
                state['bias'] = ((w >> 16) & 0x7FF) - ((w >> 15) & 0x800)
            elif form == 5:
                state['pattern'] = (w >> 16) & 0x1F
            elif form == 9:
                state['precision'] = 2.0 ** (((w >> 16) & 0xFF) - 0x8D)
        elif op in (0x012, 0x052, 0x005, 0x055, 0x095, 0x161):
            m.calls.append((off,) + tuple(words))
        off += size
        if stop_on_return and op in (0x082, 0x1C2) and ((w >> 12) & 0xF) == 0:
            break
    m.end = off
    return m


def decode_materials(buf):
    """Material definition list (a model's attribute block): returns {index: dict} of the scratch
    material at each 084 commit (diffuse 091, colour1 291, specular 491, ambient 691, flags 881,
    blend A81, alpha index C81)."""
    mats = {}
    cur = {}
    off = 0
    while off + 4 <= len(buf):
        w = struct.unpack_from('<I', buf, off)[0]
        size = insn_size(w)
        if off + size > len(buf):
            break
        words = struct.unpack_from('<%dI' % (size // 4), buf, off)
        full = w & 0xFFF
        op = w & 0x1FF
        if full == 0x091:
            cur['diffuse'] = (words[1] & 0xFF, (words[1] >> 8) & 0xFF, (words[1] >> 16) & 0xFF)
        elif full == 0x291:
            cur['colour1'] = (words[1] & 0xFF, (words[1] >> 8) & 0xFF, (words[1] >> 16) & 0xFF)
        elif full == 0x491:
            cur['specular'] = (words[1] & 0xFF, (words[1] >> 8) & 0xFF, (words[1] >> 16) & 0xFF, words[1] >> 24)
        elif full == 0x691:
            cur['ambient'] = (w >> 16, words[1] & 0xFFFF, words[1] >> 16)
        elif full == 0x881:
            f = (w >> 16) & 0xFF
            cur['flags'] = f
            cur['shading'] = f & 3
            cur['fog'] = (f >> 2) & 1
            cur['textured'] = (f >> 3) & 1
            cur['alpha_mode'] = (f >> 4) & 1
            cur['highlight'] = (f >> 5) & 1
        elif full == 0xA81:
            cur['blend'] = (w >> 16) & 3
        elif full == 0xC81:
            cur['alpha_index'] = (w >> 16) & 0x3F
        elif op == 0x084:
            mats[(w >> 16) & 0x3FF] = dict(cur)
        elif op == 0x082:
            break
        elif op not in KNOWN:
            break
        off += size
    return mats


def write_obj(m, path, name='m'):
    with open(path, 'w') as f:
        f.write('o %s\n' % name)
        for p in m.positions:
            f.write('v %.5f %.5f %.5f\n' % p)
        for p in m.uvs:
            f.write('vt %.5f %.5f\n' % p)
        for p in m.normals:
            f.write('vn %.4f %.4f %.4f\n' % p)
        for a, b, c, _, _ in m.tris:
            f.write('f %d/%d/%d %d/%d/%d %d/%d/%d\n' % ((a + 1,) * 3 + (b + 1,) * 3 + (c + 1,) * 3))


def edge_report(m):
    """Open-edge statistics by position (not by vertex index): edges used once = boundary."""
    key = {}
    for i, p in enumerate(m.positions):
        key[i] = (round(p[0], 4), round(p[1], 4), round(p[2], 4))
    edges = Counter()
    for a, b, c, _, _ in m.tris:
        for u, v in ((a, b), (b, c), (c, a)):
            ku, kv = key[u], key[v]
            if ku != kv:
                edges[(min(ku, kv), max(ku, kv))] += 1
    once = sum(1 for e, k in edges.items() if k == 1)
    return dict(edges=len(edges), boundary=once, nonmanifold=sum(1 for k in edges.values() if k > 2))


if __name__ == '__main__':
    img, addr, size = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
    rom = Rom(img)
    mm = decode(rom.read(addr, size))
    print('verts %d tris %d meshes %d ops %s unknown %s calls %d' % (
        len(mm.positions), len(mm.tris), len(mm.meshes),
        ' '.join('%03X:%d' % kv for kv in sorted(mm.ops.items())), mm.unknown[:3], len(mm.calls)))
    print(edge_report(mm))
    if len(sys.argv) > 4:
        write_obj(mm, sys.argv[4])
