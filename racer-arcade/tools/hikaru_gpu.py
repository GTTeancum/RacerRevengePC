#!/usr/bin/env python3
"""Disassembler for the Sega Hikaru command processor (315-6083 "Antarctic").

Decodes a CP instruction stream (32-bit little-endian words) from a byte buffer
and prints one line per instruction. Field layouts follow the Odin emulator
(github.com/Tovarichtch/Odin, src/odin/machine/chips/antarctic/antarctic_ops.cpp,
antarctic_mesh.cpp, antarctic_decode.hpp); see notes\\hikaru-hardware.md.

Usage (run with python -I):
  python -I hikaru_gpu.py FILE [--offset HEX] [--count N] [--end HEX]
                               [--addr HEX] [--stop-on-return] [--quiet-verts]
  python -I hikaru_gpu.py FILE --scan [--offset HEX] [--end HEX] [--min-verts N]

  --addr   bus/CP address of byte --offset (for jump targets); default = offset
  --scan   list runs that decode as CP mesh lists (static vertex pushes), with
           start offset, vertex count, triangle count and how the run ends.

Status: opcode table and fields are as Odin decodes them; Odin itself marks
several fields HEURISTIC/unknown. Static (12x) vertex decoding follows
Antarctic::op_push_position_static: position = (int32 >> 10) * 2^(p-0x8D) / 64,
normal = signed 10-bit / 256, where p comes from the last 901.
"""
import argparse
import struct
import sys


def insn_size(w):
    """Bytes of an instruction: bits 5:4 of its first word (antarctic_decode.hpp)."""
    return 1 << (((w >> 4) & 3) + 2)


def f32(w):
    return struct.unpack('<f', struct.pack('<I', w & 0xFFFFFFFF))[0]


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= 0xFFFFFFFF
    return v - 0x100000000 if v & 0x80000000 else v


# Names keyed by (word & 0xFFF) first (form in bits 11:9), then (word & 0x1FF).
NAMES_FULL = {
    0x021: 'VP set Z clip (F,f,near)',
    0x221: 'VP set XY clip + offset',
    0x421: 'VP set depth range',
    0x621: 'VP set depth queue (fog)',
    0x091: 'MAT set diffuse',
    0x291: 'MAT set colour1',
    0x491: 'MAT set specular/shininess',
    0x691: 'MAT set ambient (16-bit/ch)',
    0xA91: 'MAT set blend mode (2 words)',
    0x081: 'MAT set unknown 081',
    0x881: 'MAT set flags',
    0xA81: 'MAT set blend mode',
    0xC81: 'MAT set alpha test index',
    0x0C1: 'TEX set bias/filter',
    0x2C1: 'TEX set format/size',
    0x4C1: 'TEX set slot',
    0x161: 'MTX set row',
    0x561: 'LOD set sphere centre',
    0x961: 'LIGHT set vector 9',
    0xB61: 'LIGHT set vector B',
    0x051: 'LIGHT set diffuse',
    0x451: 'LIGHT set spot cone',
    0x101: 'MESH set depth bias',
    0x301: 'MESH set unknown 301',
    0x501: 'MESH set pattern (dither)',
    0x901: 'MESH set static precision',
    0x181: 'FB set blending',
    0x781: 'FB set combiner',
    0x3A1: 'FB set lo addresses',
    0x5A1: 'FB set hi addresses',
}
NAMES = {
    0x000: 'nop',
    0x003: 'VP recall',
    0x004: 'VP commit',
    0x005: 'LOD test sphere',
    0x006: 'unknown 006',
    0x011: 'VP set ambient',
    0x012: 'jump',
    0x021: 'VP set (form?)',
    0x043: 'LIGHTSET recall',
    0x046: 'unknown 046',
    0x051: 'LIGHT set (form?)',
    0x052: 'call',
    0x055: 'LOD set radius',
    0x061: 'LIGHT set attenuation',
    0x064: 'LIGHTSET commit',
    0x081: 'MAT set (form?)',
    0x082: 'return',
    0x083: 'MAT recall',
    0x084: 'MAT commit',
    0x088: 'flush',
    0x091: 'MAT set colour (form?)',
    0x095: 'LOD compare size',
    0x0C1: 'TEX set (form?)',
    0x0C3: 'TEX recall',
    0x0C4: 'TEX commit',
    0x0D1: 'TEX set panning',
    0x0E8: 'push texcoords x3',
    0x0E9: 'push texcoords x3',
    0x101: 'MESH set (form?)',
    0x103: 'set poly type',
    0x104: 'LIGHT commit',
    0x113: 'set poly type (2 words)',
    0x154: 'alpha threshold table',
    0x158: 'push texcoords x1',
    0x161: 'vector (form?)',
    0x181: 'FB (form?)',
    0x191: 'VP set clear colour',
    0x194: 'light table',
    0x1A1: 'FB addresses (form?)',
    0x1C2: 'end (kill)',
}
for _op in range(0x12C, 0x130):
    NAMES[_op] = 'push vertex static (fixed pos+nrm)'
for _op in range(0x1AC, 0x1B0):
    NAMES[_op] = 'push vertex dynamic (float pos)'
for _op in range(0x1B8, 0x1C0):
    NAMES[_op] = 'push vertex all (float pos,uv,nrm)'
for _op in range(0x158, 0x15C):
    NAMES[_op] = 'push texcoords x1'

PUSH_OPS = set(range(0x12C, 0x130)) | set(range(0x1AC, 0x1B0)) | set(range(0x1B8, 0x1C0))
VALID_OPS = set(NAMES)


def name_of(w):
    op = w & 0x1FF
    if op in PUSH_OPS or op in (0x158, 0x159, 0x15A, 0x15B, 0x0E8, 0x0E9):
        return NAMES[op]
    return NAMES_FULL.get(w & 0xFFF) or NAMES.get(op)


def vertex_header(w):
    """Common header of 12x/1Ax/1Bx: AAAAAAAA C---OO-- uPRSTTTo oooootpW."""
    bits = []
    if w & 1:
        bits.append('W')          # winding (0,-2,-1)
    if w & 2:
        bits.append('pivot')      # -2 kept as fan pivot
    t = (w >> 9) & 7
    bits.append('tri' if t == 7 else ('T=%d' % t if t else 'push'))
    if w & 0x1000:
        bits.append('mv2')
    if w & 0x2000:
        bits.append('R')
    if w & 0x4000:
        bits.append('P')
    if w & 0x8000:
        bits.append('u')
    uvo = (w >> 18) & 3
    if uvo:
        bits.append('uvofs%d' % uvo)
    if w & 0x800000:
        bits.append('nocull')
    bits.append('a=%02X' % (w >> 24))
    return ' '.join(bits)


class State:
    def __init__(self):
        self.precision = 1.0  # 2^(log-0x8D) from 901; 1.0 until seen
        self.verts = 0
        self.tris = 0


def decode(words, st, addr):
    """Text for one instruction given its words."""
    w = words[0]
    op = w & 0x1FF
    full = w & 0xFFF
    if op in range(0x12C, 0x130):
        scale = st.precision / 64.0
        pos = [(s32(x) >> 10) * scale for x in words[1:4]]
        nrm = [s16((x & 0x3FF) << 6) / 16384.0 for x in words[1:4]]
        st.verts += 1
        if ((w >> 9) & 7) == 7:
            st.tris += 1
        return '%s pos=(%.4f %.4f %.4f) n=(%.3f %.3f %.3f)' % (
            vertex_header(w), *pos, *nrm)
    if op in range(0x1AC, 0x1B0):
        st.verts += 1
        if ((w >> 9) & 7) == 7:
            st.tris += 1
        return '%s pos=(%g %g %g)' % (vertex_header(w), *[f32(x) for x in words[1:4]])
    if op in range(0x1B8, 0x1C0):
        st.verts += 1
        if ((w >> 9) & 7) == 7:
            st.tris += 1
        return '%s pos=(%g %g %g) uv=(%g %g) n=(%g %g %g)' % (
            vertex_header(w), *[f32(x) for x in words[1:4]],
            s16(words[4]) / 16, s16(words[4] >> 16) / 16, *[f32(x) for x in words[5:8]])
    if op in (0x158, 0x159, 0x15A, 0x15B):
        tri = ((w >> 9) & 7) == 7
        st.tris += tri
        return '%suv=(%g %g)' % ('tri ' if tri else '', s16(words[1]) / 16, s16(words[1] >> 16) / 16)
    if op in (0x0E8, 0x0E9):
        st.tris += ((w >> 9) & 7) == 7
        uv = ['(%g %g)' % (s16(x) / 16, s16(x >> 16) / 16) for x in words[1:4]]
        return 'texhead+%d %s' % (w >> 16, ' '.join(uv))
    if op in (0x012, 0x052):
        tgt = words[1] * 4 + (addr if w & 0x800 else 0)
        return 'cond=%X%s -> %08X%s' % ((w >> 12) & 0xF, ' rel' if w & 0x800 else '',
                                       tgt & 0xFFFFFFFF,
                                       (' id/size=%d' % (w >> 16)) if op == 0x012 else '')
    if op == 0x082:
        return 'cond=%X' % ((w >> 12) & 0xF)
    if op == 0x083:
        return 'n=%d %s' % (w >> 16, 'recall' if w & 0x1000 else 'set base')
    if op == 0x084:
        return 'n=%d' % ((w >> 16) & 0x3FF)
    if op == 0x0C3:
        return 'n=%d %s' % (w >> 16, 'recall' if w & 0x1000 else 'set base')
    if op == 0x0C4:
        return 'n=%d' % ((w >> 16) & 0x3FF)
    if op == 0x003:
        return 'vp=%d%s%s' % ((w >> 16) & 7, ' push' if w & 0x2000 else '', ' pop' if w & 0x4000 else '')
    if op == 0x004:
        return 'vp=%d' % ((w >> 16) & 7)
    if op == 0x103 or op == 0x113:
        return 'type=%d alpha=%02X' % ((w >> 9) & 7, w >> 24)
    if full == 0x101:
        b = ((w >> 16) & 0x7FF) - ((w >> 15) & 0x800)
        return 'bias=%d' % b
    if full == 0x501:
        return 'pattern=%02X' % ((w >> 16) & 0x1F)
    if full == 0x901:
        lg = (w >> 16) & 0xFF
        st.precision = 2.0 ** (lg - 0x8D)
        return 'log=%02X -> unit 2^%d/64' % (lg, lg - 0x8D)
    if full == 0x2C1:
        return 'fmt=%d %dx%d texmode=%d repU=%d repV=%d mirU=%d mirV=%d uvtexels=%d b=%d' % (
            (w >> 26) & 7, 16 << ((w >> 16) & 7), 16 << ((w >> 19) & 7), (w >> 29) & 7,
            (w >> 22) & 1, (w >> 23) & 1, (w >> 24) & 1, (w >> 25) & 1, (w >> 14) & 1, (w >> 15) & 1)
    if full == 0x4C1:
        return 'bank=%d slot=(%02X,%02X) -> texel (%d,%d)' % (
            (w >> 12) & 1, (w >> 16) & 0xFF, w >> 24,
            (((w >> 16) & 0xFF) - 0x80) * 16, ((w >> 24) - 0xC0) * 16)
    if full == 0x0C1:
        b = (w >> 20) & 0xFF
        return 'filter=%d bias=%d' % ((w >> 16) & 3, b - 256 if b & 0x80 else b)
    if full == 0x881:
        # XXXXXXXX xyhatzSS: SS 17:16, z 18, t 19, a 20, h 21, y 22, x 23
        return 'shade=%d fog=%d tex=%d alpha=%d hilite=%d raw=%02X' % (
            (w >> 16) & 3, (w >> 18) & 1, (w >> 19) & 1, (w >> 20) & 1, (w >> 21) & 1, (w >> 16) & 0xFF)
    if full == 0xA81:
        return 'blend=%d' % ((w >> 16) & 3)
    if full == 0xC81:
        return 'alpha idx=%d' % ((w >> 16) & 0x3F)
    if full in (0x091, 0x291, 0x491):
        x = words[1]
        return 'rgb=(%d %d %d) x=%02X' % (x & 0xFF, (x >> 8) & 0xFF, (x >> 16) & 0xFF, x >> 24)
    if full == 0x691:
        return 'rgb=(%d %d %d)' % (w >> 16, words[1] & 0xFFFF, words[1] >> 16)
    if op == 0x161:
        v = ' '.join('%g' % f32(x) for x in words[1:4])
        if full == 0x161:
            return 'col=%d%s%s%s (%s)' % ((w >> 16) & 3, ' second' if w & 0x40000 else '',
                                         ' local' if w & 0x2000 else '', ' last' if w & 0x4000 else '', v)
        return '(%s)' % v
    if full == 0x021:
        return 'F=%g f=%g near=%g' % (f32(words[1]), f32(words[2]), f32(words[3]))
    if full == 0x421:
        return 'min=%g max=%g vp#=%d' % (f32(words[1]), f32(words[2]), words[3] >> 29)
    if op == 0x055:
        return 'radius=%g' % f32(words[1])
    if op == 0x005:
        return 'radius=%g' % f32(w & 0xFFFFF000)
    if op == 0x095:
        return 'form=%X H=%d L=%d' % ((w >> 12) & 0xF, words[1] >> 16, words[1] & 0xFFFF)
    if op == 0x043:
        return 'n=%d mask=%X%s' % ((w >> 16) & 0xFF, (w >> 24) & 0xF, ' base' if w & 0x1000 else '')
    if op == 0x064:
        return 'set=%d lights=%d,%d,%d,%d' % ((w >> 16) & 0xFF, words[1] & 0x3FF, (words[1] >> 16) & 0x3FF,
                                              words[2] & 0x3FF, (words[2] >> 16) & 0x3FF)
    if op == 0x104:
        return 'n=%d' % ((w >> 16) & 0x3FF)
    return ''


def disasm(buf, start, end, addr, count=None, stop_on_return=False, quiet_verts=False, out=sys.stdout):
    st = State()
    off = start
    n = 0
    while off + 4 <= end:
        w = struct.unpack_from('<I', buf, off)[0]
        size = insn_size(w)
        nwords = size // 4
        if off + size > len(buf):
            break
        words = list(struct.unpack_from('<%dI' % nwords, buf, off))
        name = name_of(w)
        a = addr + (off - start)
        if name is None:
            print('%08X  %s  ??? (not a known CP opcode %03X)' % (a, ' '.join('%08X' % x for x in words),
                                                                  w & 0x1FF), file=out)
            break
        text = decode(words, st, a)
        if not (quiet_verts and (w & 0x1FF) in PUSH_OPS | {0x158, 0x159, 0x15A, 0x15B}):
            print('%08X  %-35s %-34s %s' % (a, ' '.join('%08X' % x for x in words), name, text), file=out)
        off += size
        n += 1
        if count and n >= count:
            break
        if stop_on_return and (w & 0x1FF) in (0x082, 0x1C2) and ((w >> 12) & 0xF) == 0:
            break
    print('-- %d instructions, %d vertices, %d triangles, ended at offset %X' % (n, st.verts, st.tris, off),
          file=out)
    return off


def scan(buf, start, end, min_verts, out=sys.stdout):
    """Finds runs of valid CP instructions containing static vertex pushes."""
    off = start - (start % 4)
    while off + 16 <= end:
        w = struct.unpack_from('<I', buf, off)[0]
        if (w & 0x1FF) not in range(0x12C, 0x130) or (w >> 24) == 0:
            off += 4
            continue
        # walk back over plain set-up instructions to the run start
        run = off
        # walk forward
        p, verts, tris, n, last = off, 0, 0, 0, None
        while p + 4 <= end:
            x = struct.unpack_from('<I', buf, p)[0]
            nm = name_of(x)
            if nm is None or x == 0:
                last = 'stop %08X' % x
                break
            op = x & 0x1FF
            if op in PUSH_OPS:
                verts += 1
            if op in PUSH_OPS or op in (0x158, 0x159, 0x15A, 0x15B, 0x0E8, 0x0E9):
                tris += ((x >> 9) & 7) == 7
            p += insn_size(x)
            n += 1
            if op == 0x082 or op == 0x1C2:
                last = 'return' if op == 0x082 else 'end'
                break
        if verts >= min_verts:
            print('%08X..%08X  %6d insns %6d verts %6d tris  %s' % (run, p, n, verts, tris, last), file=out)
        off = max(p, off + 4)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('file')
    ap.add_argument('--offset', type=lambda s: int(s, 16), default=0)
    ap.add_argument('--end', type=lambda s: int(s, 16))
    ap.add_argument('--count', type=int)
    ap.add_argument('--addr', type=lambda s: int(s, 16))
    ap.add_argument('--stop-on-return', action='store_true')
    ap.add_argument('--quiet-verts', action='store_true')
    ap.add_argument('--scan', action='store_true')
    ap.add_argument('--min-verts', type=int, default=3)
    a = ap.parse_args()
    with open(a.file, 'rb') as f:
        f.seek(a.offset)
        length = (a.end - a.offset) if a.end else (0x100000 if not a.scan else -1)
        buf = f.read(length) if length > 0 else f.read()
    end = len(buf)
    if a.scan:
        # report absolute offsets
        class Shift:
            def write(self, s):
                sys.stdout.write(s)
        sys.stdout.flush()
        import io
        tmp = io.StringIO()
        scan(buf, 0, end, a.min_verts, out=tmp)
        for line in tmp.getvalue().splitlines():
            rng, rest = line.split('  ', 1)
            s, e = rng.split('..')
            print('%08X..%08X  %s' % (int(s, 16) + a.offset, int(e, 16) + a.offset, rest))
        return
    disasm(buf, 0, end, a.addr if a.addr is not None else a.offset, a.count, a.stop_on_return, a.quiet_verts)


if __name__ == '__main__':
    main()
