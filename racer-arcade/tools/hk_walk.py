"""Interim Hikaru display-list walker for exports: meshes with material/texture recall indices,
static and dynamic vertices, texcoords and triangles. Triangle assembly follows notes/hikaru-hardware.md 3.5
(3-entry window; the 158 texcoord push with T=7 closes a triangle; W bit 0 -> (0,-1,-2), 1 -> (0,-2,-1);
p bit keeps vertex -2 as fan pivot). To be replaced by tools/hikaru_mesh.py when it exists."""
import struct
def s32(v): return v - (1 << 32) if v & 0x80000000 else v
def s16(v): v &= 0xFFFF; return v - 0x10000 if v & 0x8000 else v
def f(v): return struct.unpack('<f', struct.pack('<I', v))[0]
def walk(buf, off, size):
    end = off + size; prec = 1.0; mat = tex = None
    meshes = []; cur = None; ring = []; pivot = None
    def newmesh():
        nonlocal cur, ring, pivot
        cur = dict(mat=mat, tex=tex, prec=prec, V=[], N=[], UV=[], T=[]); meshes.append(cur); ring = []; pivot = None
    while off + 4 <= end:
        w = struct.unpack_from('<I', buf, off)[0]
        sz = 1 << (((w >> 4) & 3) + 2)
        if off + sz > end: break
        ws = struct.unpack_from('<%dI' % (sz // 4), buf, off)
        op = w & 0x1FF; full = w & 0xFFF
        if w == 0: off += 4; continue
        if full == 0x083 and (w & 0x1000): mat = w >> 16; cur = None
        elif op == 0x0C3 and (w & 0x1000): tex = w >> 16; cur = None
        elif full == 0x901: prec = 2.0 ** (((w >> 16) & 0xFF) - 0x8D); cur = None
        elif op in (0x12C, 0x12D, 0x12E, 0x12F, 0x1AC, 0x1AD, 0x1AE, 0x1AF):
            if cur is None: newmesh()
            if op < 0x130:
                p = [(s32(x) >> 10) * prec / 64 for x in ws[1:4]]; n = [s16((x & 0x3FF) << 6) / 16384 for x in ws[1:4]]
            else:
                p = [f(x) for x in ws[1:4]]; n = [0, 0, 0]
            cur['V'].append(p); cur['N'].append(n); cur['UV'].append((0, 0)); i = len(cur['V']) - 1
            if w & 2 and len(ring) >= 2: pivot = ring[-2]
            ring.append(i); ring = ring[-3:]
        elif op in (0x158, 0x159, 0x15A, 0x15B) and cur is not None and cur['V']:
            cur['UV'][-1] = (s16(ws[1]) / 16.0, s16(ws[1] >> 16) / 16.0)
            if ((w >> 9) & 7) == 7 and len(ring) == 3:
                a, b, c = ring[2], ring[1], ring[0]
                if pivot is not None: c = pivot
                cur['T'].append((a, b, c) if not (w & 1) else (a, c, b))
        elif full == 0x082 or op == 0x1C2:
            if ((w >> 12) & 0xF) == 0: break
        off += sz
    return [m for m in meshes if m['V']]
