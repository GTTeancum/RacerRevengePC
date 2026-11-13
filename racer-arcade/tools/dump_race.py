#!/usr/bin/env python3
"""Dump Racer Arcade race-logic tables (see notes\\race-data.md).

    python -I dump_race.py IMG_DIR OUT_DIR
Writes OUT_DIR\\race-tables.txt (grids, laps, time, AI, pod layout, VTR, STAT/LENS headers)
and OUT_DIR\\race-paths.txt (course centre lines and AI guide edges, every point).
"""
import sys, os, struct

B = 0x0C00FC00
img, out = sys.argv[1], sys.argv[2]
E = open(os.path.join(img, "eprom.bin"), "rb").read()
M = open(os.path.join(img, "mask.bin"), "rb").read()
U = lambda a, n=1: struct.unpack_from("<%dI" % n, E, a - B)
I = lambda a, n=1: struct.unpack_from("<%di" % n, E, a - B)
F = lambda a, n=1: struct.unpack_from("<%df" % n, E, a - B)
NAMES = ["SA_COURSE2", "ETTI4", "MALASTARE", "SA_COURSE4", "Kaadu/course4"]
PILOT = "ALD ANA BEN EBE GAS MAR MAW RAT SEB TEE ODY ARK BOL CLE DUD ELA NEV WAN".split()
L = []
P = L.append


def cstr(a):
    o = a - B; e = E.find(b"\0", o, o + 64)
    return E[o:e].decode("latin1") if e > o else ""


P("# Racer Arcade race-logic tables (tools\\dump_race.py). Course order = course id 0..4.")
P("\n## Start grid  @0C3E9778[sel] -> 24-B records {s32 pod kind, s32 AI level, f32 x,y,z, s16 yaw (65536=360deg), s16 0}, end kind<0")
P("## read by Prep_RaceInit f_0C14B3CC: the record whose kind == player's kind is the player, all others become NPCs")
for c in range(6):
    p = U(0x0C3E9778 + 4 * c)[0]
    P("grid %d @%08X:" % (c, p))
    i = 0
    while True:
        k, lv = I(p + 24 * i, 2)
        if k < 0 or i > 16: break
        x, y, z = F(p + 24 * i + 8, 3); yaw = struct.unpack_from("<hh", E, p + 24 * i + 20 - B)
        P("   pos %2d kind %2d %-3s AI level %d  xyz (%8.2f, %5.2f, %8.2f) yaw %d" % (i, k, PILOT[k] if k < 18 else "?", lv, x, y, z, yaw[0]))
        i += 1
P("\n## Laps  @0C3E8D68[sel] (state+512): " + ", ".join("%s=%d" % (NAMES[c], I(0x0C3E8D68 + 4 * c)[0]) for c in range(4)))
P("\n## Time table @0C3F7128[sel] -> 5 difficulty rows (operator setting, f_0C142AC0) x 8 s32:")
P("## col0 = start time in seconds (x60 -> frames, state+516), col1..4 = time extension per lap (guess), rest 0")
for c in range(4):
    p = U(0x0C3F7128 + 4 * c)[0]
    P("%s @%08X: %s" % (NAMES[c], p, "  ".join(str(I(p + 32 * r, 5)) for r in range(5))))
P("\n## Race select table @0C3E8AC8 {course, music stream}: " + ", ".join(str(I(0x0C3E8AC8 + 8 * k, 2)) for k in range(4)))

P("\n## Course centre line: landscape +24 -> @0C1695E8[idx] {ptr, n}: closed loop of (x,z) f32 pairs, node 0 = (0,0) = start line")
CL = {}
for c in range(5):
    idx = U(0x0C3E7FF8 + 52 * c + 24)[0]
    p, n = U(0x0C1695E8 + 8 * idx, 2)
    pts = [F(p + 8 * i, 2) for i in range(n)]
    seg = sum(((pts[i][0] - pts[i - 1][0]) ** 2 + (pts[i][1] - pts[i - 1][1]) ** 2) ** .5 for i in range(n))
    CL[c] = (p, n, pts)
    P("%s: idx %d @%08X n=%d loop length %.1f  x %.1f..%.1f z %.1f..%.1f" % (NAMES[c], idx, p, n, seg,
      min(q[0] for q in pts), max(q[0] for q in pts), min(q[1] for q in pts), max(q[1] for q in pts)))

P("\n## NPC guide (AI track edges) @0C2F90A8[course] {left list, right list} into @0C2B161C {ptr, n} (x,z) pairs; built by f_0C13AD2E")
GD = {}
for c in range(4):
    l, r = U(0x0C2F90A8 + 8 * c, 2)
    pl, nl = U(0x0C2B161C + 8 * l, 2); pr, nr = U(0x0C2B161C + 8 * r, 2)
    GD[c] = ((pl, nl), (pr, nr))
    P("%s: left list %d @%08X n=%d, right list %d @%08X n=%d" % (NAMES[c], l, pl, nl, r, pr, nr))

P("\n## NPC speed table @0C2F93E0[course] 8 f32 (x60 -> f_0C13AB18(level, int)); index = AI level 0..7 (guess: target speed per level)")
for c in range(4): P("%s: %s" % (NAMES[c], " ".join("%.1f" % x for x in F(0x0C2F93E0 + 32 * c, 8))))
P("\n## NPC per-course (node, value) tables (getters f_0C13C15A.. f_0C13C298): {ptr, n} @0C2F9260/92D0/9338/93B8; meaning unknown (guess: speed/line factor by track position)")
for c, a in enumerate((0x0C2F9260, 0x0C2F92D0, 0x0C2F9338, 0x0C2F93B8)):
    p, n = U(a, 2)
    P("%s?: %s" % (NAMES[c], " ".join("(%g,%g)" % F(p + 8 * i, 2) for i in range(n))))
p, n = U(0x0C2F93D8, 2)
P("table @0C2F93D8 (f_0C13C4A8): " + " ".join("(%g,%g)" % F(p + 8 * i, 2) for i in range(n)))

P("\n## NPC per-pod-kind parameters @0C2F90E0[kind] 4 f32 -> NPC +20..+32 (f_0C13B114); meaning unknown")
for k in range(18): P("kind %2d %s: %s" % (k, PILOT[k], " ".join("%5.2f" % x for x in F(0x0C2F90E0 + 16 * k, 4))))

P("\n## Pod body layout @0C2F904C[kind] -> 168-B block (f_0C13A7F2): s32 nParts, chariot rec, nParts engine recs, +164 flag")
P("## rec = 8 f32 {x, y, z, yaw deg, a, b, c, d}  (a/b/c/d unknown: 100/15/4/6 chariot, 300/30/4/6 engines; guess mass/radius)")
for k in range(18):
    blk = U(0x0C2F904C + 4 * k)[0]
    n = I(blk)[0]
    recs = [F(blk + 4 + 32 * j, 8) for j in range(n + 1)]
    P("kind %2d %s @%08X n=%d flag164=%d: %s" % (k, PILOT[k], blk, n, I(blk + 164)[0],
      " | ".join(",".join("%g" % round(v, 3) for v in r) for r in recs)))

P("\n## Pod table +132 (special per-pod hook, copied to pod state +196): " + ", ".join(
    "%s=%08X" % (PILOT[k], U(0x0C3F6790 + k * 0x88 + 132)[0]) for k in range(18) if U(0x0C3F6790 + k * 0x88 + 132)[0]))
P("## Pilot tables: flag = (slot != 0). Kinds whose player (slot 0) entry differs from the other-slot entry:")
for k in range(18):
    a = U(0x0C2A733C + 96 * k, 2); b = U(0x0C2A6C7C + 96 * k, 2)
    if a != b: P("   %s: slot0 texgroup/object %s, other slots %s" % (PILOT[k], a, b))
P("## Pod water objects @0C300058[kind] 5 words (POD WATER REF, f_0C13FF48):")
for k in range(18): P("   %s: %s" % (PILOT[k], I(0x0C300058 + 20 * k, 5)))

P("\n## VTR (recorded runs, attract demo) mask 0x0F2166C0 directory, data mask 0x0E78A6C0, 12 x 7200 frames x 128 B")
P("## frame: u32 w0 (bits0-4 pod kind, high bits flags), u32 w1, 5 x 24-B part poses {3 packed rotation rows 11:10:11 bits, f32 x,y,z}")
cnt = struct.unpack_from("<2I", M, 0x0F2166C0)
for i in range(cnt[1]):
    e = struct.unpack_from("<5I", M, 0x0F2166C0 + 8 + 20 * i)
    src = 0x0E78A6C0 + i * 0xE1000
    k = struct.unpack_from("<I", M, src + 0x500)[0] & 0x1F
    P("run %2d slot %d frames %d last %d +0xA48? %X  mask %08X  pod kind %d %s" % (i, e[0], e[1], e[2], e[3], src, k, PILOT[k]))

P("\n## STAT (course 0,1,3) / LENS headers: {name, n, poly array, f32 minX, minZ, maxX, maxZ (10-unit margin, checked on STAT 0 vertices), s32 grid cells (30/15), grid ptr}; STAT poly = 68 B {plane nx,ny,nz,d, 4 x xyz (v3=v0 for triangles), u32 flag}")
for nm, a in (("SA_COURSE2_STAT", 0x0C3C2B64), ("S_ETTI4_STAT", 0x0C3E4AA8), ("SA_COURSE4_STAT", 0x0C3E0924),
              ("SA_COURSE2_LENS", 0x0C2CA41C), ("ETTI4_LENS", 0x0C2D51E0), ("MALASTARE_LENS", 0x0C2D83E8),
              ("MALASTARE_LENS2", 0x0C2DFFA8), ("SA_COURSE4_LENS", 0x0C2F7A6C)):
    w = U(a, 9); f = F(a + 12, 4)
    P("%-16s @%08X n=%d polys@%08X bbox %s cells %d grid@%08X" % (nm, a, w[1], w[2], tuple(round(x, 1) for x in f), w[7], w[8]))
open(os.path.join(out, "race-tables.txt"), "w").write("\n".join(L) + "\n")

L = []
P = L.append
P("# Course centre lines (x z per node) and AI guide edges (left x z, right x z). tools\\dump_race.py")
for c in range(5):
    p, n, pts = CL[c]
    P("\n## centre %d %s n=%d" % (c, NAMES[c], n))
    for i, (x, z) in enumerate(pts): P("%4d %10.3f %10.3f" % (i, x, z))
for c in range(4):
    (pl, nl), (pr, nr) = GD[c]
    P("\n## guide %d %s left n=%d right n=%d" % (c, NAMES[c], nl, nr))
    for i in range(max(nl, nr)):
        l = F(pl + 8 * i, 2) if i < nl else (float("nan"),) * 2
        r = F(pr + 8 * i, 2) if i < nr else (float("nan"),) * 2
        P("%4d %10.3f %10.3f   %10.3f %10.3f" % (i, l[0], l[1], r[0], r[1]))
open(os.path.join(out, "race-paths.txt"), "w").write("\n".join(L) + "\n")
print("ok")
