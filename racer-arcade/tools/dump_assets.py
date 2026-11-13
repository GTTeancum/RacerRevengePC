#!/usr/bin/env python3
"""Dump the Racer Arcade asset tables from the EPROM program and resolve per-course / per-pod
asset lists down to ROM-board addresses.

    python -I dump_assets.py IMG_DIR OUT_DIR

Writes OUT_DIR\\asset-tables-base.txt, -courses.txt, -pods.txt.

Address conventions (see notes\\program-map.md):
  CPU 0x0Cxxxxxx      : program RAM; file offset in eprom.bin = addr - 0x0C00FC00
  ROM bus 0xAxxxxxxx  : mask ROM; mask.bin offset = addr & 0x0FFFFFFF   (bus 0x20000000 + off)
  ROM bus 0x90/0x91.. : EPROM bank 0x10/0x11; eprom.bin offset = (bank-0x10)*8MB + (addr & 0xFFFFFF)
Every id may carry bit 30 (0x40000000): then it indexes the EPROM-resident table instead of
the mask-ROM table (code: f_0C14C880 & co.).
"""
import sys, os, struct

BASE = 0x0C00FC00
img, out = sys.argv[1], sys.argv[2]
E = open(os.path.join(img, "eprom.bin"), "rb").read()


def u32(a): return struct.unpack_from("<I", E, a - BASE)[0]
def i32(a): return struct.unpack_from("<i", E, a - BASE)[0]
def words(a, n): return struct.unpack_from("<%dI" % n, E, a - BASE)


M = open(os.path.join(img, "mask.bin"), "rb").read()


def romwords(v, n):
    if v >> 28 == 0xA: return struct.unpack_from("<%dI" % n, M, v & 0x0FFFFFFF)
    if v >> 24 in (0x90, 0x91):
        return struct.unpack_from("<%dI" % n, E, ((v >> 24) - 0x90) * 0x800000 + (v & 0xFFFFFF))
    return None


def motion_blocks(mid):
    """Motion header (20 B in ROM): frames F, bones N, ptr A (F*4 B), ptr B (N*36 B), ptr C (F*N*36 B)."""
    r = rec("motion", mid)
    if not r: return []
    h = romwords(r[0], 5); F, N = h[0], h[1]; nm = cstr(r[2])
    return [("motion %s %s header F=%d N=%d" % (idname(mid), nm, F, N), r[0], 20),
            ("motion %s frame table" % idname(mid), h[2], F * 4),
            ("motion %s bone table" % idname(mid), h[3], N * 36),
            ("motion %s keys" % idname(mid), h[4], F * N * 36)]


def cstr(a):
    if not (BASE <= a < BASE + len(E)): return None
    o = a - BASE; e = E.find(b"\0", o, o + 120)
    if e < 0: return None
    s = E[o:e]
    if len(s) < 1 or any(c < 0x20 or c > 0x7E for c in s): return None
    return s.decode()


def romloc(v):
    """ROM-board bus address -> 'mask:0xOFFSET' / 'eprom:0xOFFSET'."""
    if v >> 28 == 0xA: return "mask:%08X" % (v & 0x0FFFFFFF)
    if v >> 24 in (0x90, 0x91): return "eprom:%08X" % (((v >> 24) - 0x90) * 0x800000 + (v & 0xFFFFFF))
    if v >> 24 == 0x0C: return "ram:%08X" % v
    return "?%08X" % v


EP = 0x40000000
# (name, mask table, mask count, eprom table, eprom count, record bytes)   [getter function]
TABLES = {
    "motion":   (0x0C240678, 300, 0x0C25C0BC, 3, 12),     # f_0C148AF4 / f_0C148AC8
    "model":    (0x0C17D750, 5240, 0x0C244FC0, 745, 20),  # f_0C148E90 / f_0C148E64
    "object":   (0x0C1970B0, 1529, 0x0C2489F4, 130, 16),  # f_0C148EE6 / f_0C148EBA
    "texture":  (0x0C169738, 2143, 0x0C2432F8, 190, 24),  # f_0C14C7FC / f_0C14C7D0
    "texgroup": (0x0C17CFA4, 97, 0x0C244F08, 8, 20),      # f_0C14C856 / f_0C14C82A
    "texset":   (0x0C17CE64, 16, 0x0C244EE0, 2, 20),      # f_0C14C8C8 / f_0C14C880
    "stream":   (0x0C169610, 21, 0x0C25C13C, 1, 8),       # f_0C14C46C (EPROM count not bounds-checked; entry 1 is zero)
    "pack":     (0x0C1696C0, 14, 0x0C25C14C, 1, 8),       # f_0C14C4A8 (EPROM count not bounds-checked; guess 1)
}


def rec(kind, ident):
    mt, mn, et, en, sz = TABLES[kind]
    if ident < 0: return None
    if ident & EP:
        i = ident & ~EP
        return words(et + i * sz, sz // 4) if i < en else None
    return words(mt + ident * sz, sz // 4) if ident < mn else None


def idname(ident):
    if ident == 0xFFFFFFFF or ident < 0: return "-1"
    return ("E%d" % (ident & ~EP)) if ident & EP else "%d" % ident


def model_blocks(mid):
    r = rec("model", mid)
    if not r: return []
    return [("model %s geometry" % idname(mid), r[0], r[1]), ("model %s attributes(n=%d)" % (idname(mid), r[4]), r[2], r[3])]


def object_models(oid, depth=0):
    """Object -> model ids (root model + per-node model ids). Nodes are 104-byte records."""
    r = rec("object", oid)
    if not r: return []
    nodes, n, root, flags = r
    ms = []
    if root != 0xFFFFFFFF: ms.append(root)
    for k in range(n):
        m = u32(nodes + k * 104)
        if m != 0xFFFFFFFF: ms.append(m)
    return ms


def texgroup_textures(gid, kind="texgroup"):
    r = rec(kind, gid)
    if not r: return []
    flag, la, na, lb, nb = r
    return [u32(la + 12 * k) for k in range(na)] if la else []


def texture_block(tid):
    r = rec("texture", tid)
    if not r: return None
    w, h = 16 << r[1], 16 << r[2]
    return ("texture %s %dx%d texhead %08X %08X" % (idname(tid), w, h, r[4], r[5]), r[0], r[3])


def fmt_block(b):
    name, addr, size = b
    if addr == 0: return "    %-14s size %08X  %s" % ("(none)", size, name)
    return "    %-14s size %08X  %s" % (romloc(addr), size, name)


L = []
def P(s=""): L.append(s)


# ---------------------------------------------------------------- base tables
P("# Racer Arcade asset tables (decoded from eprom.bin by tools\\dump_assets.py)")
P("# columns: index, ROM location (mask:/eprom: file offsets), size, other fields")
for kind, (mt, mn, et, en, sz) in TABLES.items():
    for tag, t, n in (("mask-set", mt, mn), ("EPROM-set (id|0x40000000)", et, en)):
        P(); P("## %s table %s @%08X, %d records x %d bytes" % (kind, tag, t, n, sz))
        for i in range(n):
            r = words(t + i * sz, sz // 4)
            if kind == "motion":
                h = r[0]
                P("%4d %-14s id/flags %08X  name %s" % (i, romloc(h), r[1], cstr(r[2])))
            elif kind == "model":
                P("%4d geom %-14s %08X  attr %-14s %08X  nattr %d" % (i, romloc(r[0]), r[1], romloc(r[2]), r[3], r[4]))
            elif kind == "object":
                ident = i | (EP if t == et else 0)
                P("%4d nodes@%08X n=%d root=%s flags=%X models=%s" % (i, r[0], r[1], idname(r[2]), r[3],
                  ",".join(idname(m) for m in object_models(ident))))
            elif kind == "texture":
                P("%4d %-14s %08X  %dx%d  texhead %08X %08X" % (i, romloc(r[0]), r[3], 16 << r[1], 16 << r[2], r[4], r[5]))
            elif kind in ("texgroup", "texset"):
                ident = i | (EP if t == et else 0)
                tx = texgroup_textures(ident, kind)
                P("%4d flag=%d listA@%08X n=%d listB@%08X n=%d textures=%s" % (i, r[0], r[1], r[2], r[3], r[4],
                  ",".join(idname(x) for x in tx)))
            else:
                P("%4d %-14s size %08X" % (i, romloc(r[0]), r[1]))
open(os.path.join(out, "asset-tables-base.txt"), "w").write("\n".join(L) + "\n")

# ---------------------------------------------------------------- courses
L = []
P("# Per-course asset lists (tools\\dump_assets.py). Course index = argument of f_0C142DDA.")
P("# course table @0C3E4B30 (12B: landscape, collision, stat), landscape @0C3E7FF8 (52B),")
P("# collision @0C3E4ACC (12B: block, root ptr, size), STAT via f_0C142D98, music via @0C3E8AC8")
NAMES = {0: "SA_COURSE2", 1: "ETTI4", 2: "MALASTARE", 3: "SA_COURSE4", 4: "(no STAT/LENS name)"}
music = {}
for k in range(4):
    c, s = words(0x0C3E8AC8 + 8 * k, 2); music[c] = s
for c in range(5):
    land, coll, stat = words(0x0C3E4B30 + 12 * c, 3)
    P(); P("## course %d  %s" % (c, NAMES[c]))
    lr = words(0x0C3E7FF8 + 52 * land, 13)
    P("  landscape %d: texset %s, objects %s / %s, sky-ish table idx %d, floats %r, %d, callbacks %08X %08X %08X" % (
        land, idname(lr[0]), ",".join(idname(x) for x in lr[1:5]), idname(lr[5]), lr[6],
        tuple(round(struct.unpack('<f', struct.pack('<I', x))[0], 1) for x in lr[7:9]), lr[9], lr[10], lr[11], lr[12]))
    cb = words(0x0C3E4ACC + 12 * coll, 3)
    P("  collision %d:" % coll)
    P(fmt_block(("collision (root struct at %s)" % romloc(cb[1]), cb[0], cb[2])))
    stp = {0: 0x0C3C2B64, 3: 0x0C3E0924, 1: 0x0C3E4AA8}.get(stat)
    P("  STAT %d: %s" % (stat, ("RAM struct @%08X" % stp) if stp else "none"))
    if c in music:
        st = rec("stream", music[c]); P("  music stream %d:" % music[c]); P(fmt_block(("stream", st[0], st[1])))
    P("  landscape textures (texset %s list A):" % idname(lr[0]))
    for t in texgroup_textures(lr[0], "texset"):
        b = texture_block(t)
        if b: P(fmt_block(b))
    seen = set()
    for oid in list(lr[1:6]):
        if oid == 0xFFFFFFFF: continue
        ms = object_models(oid)
        P("  landscape object %s -> %d models" % (idname(oid), len(ms)))
        for m in ms:
            if m in seen: continue
            seen.add(m)
            for b in model_blocks(m): P(fmt_block(b))
open(os.path.join(out, "asset-tables-courses.txt"), "w").write("\n".join(L) + "\n")

# ---------------------------------------------------------------- pods & pilots
L = []
P("# Per-pod / per-pilot asset lists (tools\\dump_assets.py)")
P("# pod table @0C3F6790: 18 kinds x 0x88 bytes (DefineDispPod f_0C1439EE)")
P("#   +0 texgroup[4] (slot 0, by colour/val 0..3)  +16 texgroup[4] (other slots)  +32 engine count")
P("#   +36 part[4] x {objA, objB, engine obj[4]} (24 bytes)   +132 copied to pod state +196")
P("# pilot tables (DefinePilot f_0C133134): flag==0 @0C2A733C, flag!=0 @0C2A6C7C; 18 x 96 bytes:")
P("#   +0 texgroup, +4 object, +8 motion[22]")


def motion_name(mid):
    r = rec("motion", mid)
    return cstr(r[2]) if r else None


def pod_dump(k):
    a = 0x0C3F6790 + k * 0x88
    w = words(a, 34)
    P("  texgroups slot0 %s  others %s  engines %d  +132=%08X" % (
        ",".join(idname(x) for x in w[0:4]), ",".join(idname(x) for x in w[4:8]), w[8], w[33]))
    objs = []
    for p in range(4):
        part = w[9 + p * 6: 9 + p * 6 + 6]
        P("  part %d: objA %s objB %s engines %s" % (p, idname(part[0]), idname(part[1]), ",".join(idname(x) for x in part[2:6])))
        objs += [x for x in part if x != 0xFFFFFFFF]
    return objs, [x for x in w[0:8] if x != 0xFFFFFFFF]


def blocks_for(objs, tgs):
    seen = set(); tseen = set()
    for o in objs:
        for m in object_models(o):
            if m in seen: continue
            seen.add(m)
            for b in model_blocks(m): P(fmt_block(b))
    for g in tgs:
        for t in texgroup_textures(g):
            if t in tseen: continue
            tseen.add(t); b = texture_block(t)
            if b: P(fmt_block(b))


def pilot_code(k):
    w = words(0x0C2A733C + 96 * k, 24)
    for m in w[2:]:
        n = motion_name(m) if m != 0xFFFFFFFF else None
        if n and n.startswith("GMOT_"): return n.split("_")[1]
    return "?"


for k in range(18):
    P(); P("## pod kind %d (pilot %s)" % (k, pilot_code(k)))
    objs, tgs = pod_dump(k)
    blocks_for(objs, tgs)
for tab, nm in ((0x0C2A733C, "flag==0"), (0x0C2A6C7C, "flag!=0")):
    for k in range(18):
        w = words(tab + 96 * k, 24)
        mot = [(i, w[2 + i]) for i in range(22) if w[2 + i] != 0xFFFFFFFF]
        P(); P("## pilot kind %d %s (%s table)  texgroup %s  object %s" % (k, pilot_code(k), nm, idname(w[0]), idname(w[1])))
        P("  motions: " + ", ".join("%d:%s=%s" % (i, idname(m), motion_name(m)) for i, m in mot))
        blocks_for([w[1]], [w[0]])
        for i, m in mot:
            for b in motion_blocks(m): P(fmt_block(b))
open(os.path.join(out, "asset-tables-pods.txt"), "w").write("\n".join(L) + "\n")
print("ok")
