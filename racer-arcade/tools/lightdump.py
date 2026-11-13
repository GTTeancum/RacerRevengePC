#!/usr/bin/env python3
"""Decode SetLight (f_0C145A0E) calls in the per-course +40 hooks.
SetLight(id, cmd, args..., cmd, args..., 17) - all arguments on the stack (4 bytes each).
cmd: 0 = 3 f32 -> +0 (vector A), 1 = 3 f32 -> +12 (vector B), 2 = int -> +24, 3 = int -> +28,
4 = 3 f32 (colour, packed into +34..+37), 5/6/7/8 = byte -> +38/+39/+40/+41, 9 = f32 -> +44, 10 = f32 -> +48,
11 = state 1, 12 = state 2, 13/14 = +68 = 1/0, 15/16 = +72 = 1/0, 17 = end (f_0C145A0E jump table 0C145AD8).
Straight-line symbolic pass; '?' = value not known statically.
    python -I lightdump.py IMG HOOK [HOOK...]
"""
import sys, os, re, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, decode
import sh4an
NARGS = {0: 3, 1: 3, 2: 1, 3: 1, 4: 3, 5: 1, 6: 1, 7: 1, 8: 1, 9: 1, 10: 1}
NAME = {0: "vecA", 1: "vecB", 2: "i24", 3: "i28", 4: "rgb", 5: "b38", 6: "b39", 7: "b40", 8: "b41",
        9: "f44", 10: "f48", 11: "state1", 12: "state2", 13: "f68=1", 14: "f68=0", 15: "f72=1", 16: "f72=0"}


def f32(im, a): return struct.unpack("<f", struct.pack("<I", im.u32(a)))[0]


def run(im, h):
    ins = sh4an.flow(im, h)
    R = {}; Fr = {}; st = []; out = []
    for a in ins:
        t, I = decode(im.u16(a), a); k = t.split()[0]; ops = t[len(k):].strip()
        ir = lambda s: int(s[1:]) if re.fullmatch(r"r\d+", s) else None
        if "lit" in I:
            v = im.u32(I["lit"]) if I["litsize"] == 4 else (lambda x: x - 0x10000 if x & 0x8000 else x)(im.u16(I["lit"]))
            R[I["dst"]] = v
        elif "mova" in I: R[0] = I["mova"]
        elif "imm" in I: R[I["dst"]] = I["imm"]
        elif k == "mov" and re.fullmatch(r"r\d+,r\d+", ops):
            s, d = map(ir, ops.split(","))
            if s in R: R[d] = R[s]
            else: R.pop(d, None)
        elif k == "add" and ops.startswith("#") and ops.endswith("r15"):
            n = int(ops[1:].split(",")[0])
            if n > 0: del st[len(st) - n // 4:]
        elif k == "add" and ops.startswith("#"):
            n = int(ops[1:].split(",")[0]); r = ir(ops.split(",")[1])
            if r in R and isinstance(R[r], int): R[r] += n
        elif k == "mov.l" and re.fullmatch(r"r\d+,@-r15", ops):
            st.append(R.get(ir(ops.split(",")[0]), "?"))
        elif k == "fmov.s" and re.fullmatch(r"fr\d+,@-r15", ops):
            v = Fr.get(int(ops.split(",")[0][2:]), "?"); st.append(round(v, 4) if v != "?" else v)
        elif k == "fmov.s" and re.fullmatch(r"@r0,fr\d+", ops) and isinstance(R.get(0), int) and im.ok(R[0], 4) and R[0] > 0x0C000000:
            Fr[int(ops.split("fr")[1])] = f32(im, R[0])
        elif k == "fmov.s" and re.fullmatch(r"@\(r0,r\d+\),fr\d+", ops):
            b = ir(ops.split(",")[1][:-1]); d = int(ops.split("fr")[-1])
            if isinstance(R.get(0), int) and isinstance(R.get(b), int) and im.ok(R[0] + R[b], 4): Fr[d] = f32(im, R[0] + R[b])
            else: Fr.pop(d, None)
        elif k == "fmov.s" and re.fullmatch(r"@r\d+,fr\d+", ops):
            b = ir(ops[1:].split(",")[0]); d = int(ops.split("fr")[-1])
            if isinstance(R.get(b), int) and im.ok(R[b], 4) and R[b] > 0x0C000000: Fr[d] = f32(im, R[b])
            else: Fr.pop(d, None)
        elif k == "fldi0": Fr[int(ops[2:])] = 0.0
        elif k == "fldi1": Fr[int(ops[2:])] = 1.0
        elif k == "fneg" and int(ops[2:]) in Fr: Fr[int(ops[2:])] = -Fr[int(ops[2:])]
        elif k == "fmov" and re.fullmatch(r"fr\d+,fr\d+", ops):
            s, d = [int(x[2:]) for x in ops.split(",")]
            if s in Fr: Fr[d] = Fr[s]
            else: Fr.pop(d, None)
        elif k in ("fadd", "fsub", "fmul") and re.fullmatch(r"fr\d+,fr\d+", ops):
            s, d = [int(x[2:]) for x in ops.split(",")]
            if s in Fr and d in Fr: Fr[d] = Fr[d] + Fr[s] if k == "fadd" else Fr[d] - Fr[s] if k == "fsub" else Fr[d] * Fr[s]
            else: Fr.pop(d, None)
        if k in ("jsr", "bsr", "jmp"):
            tgt = I.get("target") if k == "bsr" else R.get(int(ops[2:]))
            # delay slot
            dt, DI = decode(im.u16(a + 2), a + 2); dk = dt.split()[0]; dops = dt[len(dk):].strip()
            if dk == "mov.l" and re.fullmatch(r"r\d+,@-r15", dops): st.append(R.get(int(dops.split(",")[0][1:]), "?"))
            if "imm" in DI: R[DI["dst"]] = DI["imm"]
            if tgt == 0x0C145A0E:
                args = list(reversed(st))
                lid = args[0] if args else "?"; i = 1; parts = []
                while i < len(args):
                    c = args[i]; i += 1
                    if c == 17 or not isinstance(c, int): parts.append("end" if c == 17 else "cmd?%s" % c); break
                    n = NARGS.get(c, 0); parts.append("%s=%s" % (NAME.get(c, c), args[i:i + n] if n != 1 else args[i])); i += n
                out.append("  @%08X SetLight id=%s: %s" % (a, lid, "; ".join(parts)))
            for r in range(8): R.pop(r, None)
            for r in range(12): Fr.pop(r, None)
    return out


im = Image(sys.argv[1]); sh4an.load(im)
for h in sys.argv[2:]:
    print("== hook f_%s" % h)
    for l in run(im, int(h, 16)): print(l)
