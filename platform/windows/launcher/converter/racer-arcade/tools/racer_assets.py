"""Shared accessors for Racer Arcade program tables (eprom.bin) and ROM data (mask.bin / eprom.bin).
Import from the tools folder (python -I with sys.path insert). Conventions: notes/program-map.md.
  CPU 0x0Cxxxxxx -> eprom.bin offset addr - 0x0C00FC00
  ROM bus 0xAxxxxxxx -> mask.bin offset addr & 0x0FFFFFFF; 0x90xxxxxx -> eprom off; 0x91xxxxxx -> eprom 0x800000+off
  id bit 30 (0x40000000) -> EPROM-resident table
"""
import os, struct
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
BASE = 0x0C00FC00
E = open(os.path.join(ROOT, 'img', 'eprom.bin'), 'rb').read()
M = open(os.path.join(ROOT, 'img', 'mask.bin'), 'rb').read()
EP = 0x40000000
TABLES = {  # kind: (mask table, count, eprom table, count, record bytes)
    'motion':   (0x0C240678, 300, 0x0C25C0BC, 3, 12),
    'model':    (0x0C17D750, 5240, 0x0C244FC0, 745, 20),
    'object':   (0x0C1970B0, 1529, 0x0C2489F4, 130, 16),
    'texture':  (0x0C169738, 2143, 0x0C2432F8, 190, 24),
    'texgroup': (0x0C17CFA4, 97, 0x0C244F08, 8, 20),
    'texset':   (0x0C17CE64, 16, 0x0C244EE0, 2, 20),
}
CODES = 'ALD ANA BEN EBE GAS MAR MAW RAT SEB TEE ODY ARK BOL CLE DUD ELA NEV WAN'.split()

def u32(a): return struct.unpack_from('<I', E, a - BASE)[0]
def i32(a): return struct.unpack_from('<i', E, a - BASE)[0]
def f32(a): return struct.unpack_from('<f', E, a - BASE)[0]
def words(a, n): return struct.unpack_from('<%dI' % n, E, a - BASE)
def cstr(a):
    o = a - BASE
    if not (0 <= o < len(E)): return None
    e = E.find(b'\0', o, o + 120)
    return E[o:e].decode('latin1') if e > o else None
def rom(addr):
    """(buffer, offset) for a ROM-bus or RAM address."""
    if addr >> 28 == 0xA: return M, addr & 0x0FFFFFFF
    if addr >> 24 in (0x90, 0x91): return E, ((addr >> 24) - 0x90) * 0x800000 + (addr & 0xFFFFFF)
    if addr >> 24 == 0x0C: return E, addr - BASE
    raise ValueError('address %08X' % addr)
def rombytes(addr, n):
    b, o = rom(addr); return b[o:o + n]
def romwords(addr, n, fmt='I'):
    b, o = rom(addr); return struct.unpack_from('<%d%s' % (n, fmt), b, o)
def rec(kind, ident):
    mt, mn, et, en, sz = TABLES[kind]
    if ident is None or ident < 0 or ident == 0xFFFFFFFF: return None
    if ident & EP:
        i = ident & ~EP; return words(et + i * sz, sz // 4) if i < en else None
    return words(mt + ident * sz, sz // 4) if ident < mn else None
def idname(i):
    if i is None or i == 0xFFFFFFFF or i < 0: return '-1'
    return ('E%d' % (i & ~EP)) if i & EP else '%d' % i
def model(mid):
    r = rec('model', mid)
    return None if r is None else dict(geom=r[0], geom_size=r[1], attr=r[2], attr_size=r[3], n_attr=r[4])
def object_nodes(oid):
    """Object record {nodes ptr, node count, root model, flags}; node = 104 B (26 words)."""
    r = rec('object', oid)
    if r is None: return None
    nodes, n, root, flags = r
    out = []
    for k in range(n):
        a = nodes + k * 104
        w = words(a, 26)
        out.append(dict(addr=a, model=w[0], raw=w, mtx=struct.unpack_from('<16f', E, a + 40 - BASE)))
    return dict(nodes=out, root=root, flags=flags, ptr=nodes)
def texgroup(gid):
    r = rec('texgroup', gid)
    if r is None: return []
    flag, la, na, lb, nb = r
    return [words(la + 12 * k, 3) for k in range(na)] if la else []
def texture(tid):
    r = rec('texture', tid)
    if r is None: return None
    ptr, wl, hl, size, h0, h1 = r
    return dict(addr=ptr, w=16 << wl, h=16 << hl, size=size, h0=h0, h1=h1, fmt=(h1 >> 26) & 7)
POD_TABLE = 0x0C3F6790
def pod(kind):
    a = POD_TABLE + kind * 0x88; w = words(a, 0x88 // 4)
    parts = [dict(objA=w[9 + p*6], objB=w[10 + p*6], engines=list(w[11 + p*6:15 + p*6])) for p in range(4)]
    return dict(tg_slot0=list(w[0:4]), tg_other=list(w[4:8]), engines=w[8], parts=parts, x132=w[33], raw=w)
PILOT_TABLES = (0x0C2A733C, 0x0C2A6C7C)  # flag == 0, flag != 0
def pilot(kind, flag=0):
    a = PILOT_TABLES[1 if flag else 0] + kind * 96; w = words(a, 24)
    return dict(texgroup=w[0], object=w[1], motions=list(w[2:24]))
def motion(mid):
    r = rec('motion', mid)
    if r is None: return None
    h = romwords(r[0], 5)
    return dict(hdr=r[0], F=h[0], N=h[1], frames=h[2], bones=h[3], keys=h[4], idflags=r[1], name=cstr(r[2]))
