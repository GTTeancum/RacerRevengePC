#!/usr/bin/env python3
"""Extract hard-coded object placements from the per-course landscape hooks.

Linear symbolic pass over a function: tracks integer registers holding constants
(mov #, mov.w/mov.l literal, mova, add #, mov rm,rn, mov.l @rN with rN = known global
slot) and FP registers (fmov.s @r0 after mova, fldi0/1, fmov, fneg, fadd/fsub/fmul of
known values). Logs:
  - loads:  object id passed to LoadStaticObject stubs (f_0C14930E..20) and the global slot
            the result is stored to (mov.l r0,@rN).
  - places: calls of a placement function with (r5 = object slot value, fr4..fr7).
    python -I hookplace.py IMG FUNCADDR [FUNCADDR...]
Branches are ignored (straight-line), so results inside loops/conditionals are approximate.
"""
import sys, os, re, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, decode
import sh4an

LOADERS = {0x0C149314, 0x0C14930E, 0x0C14931A, 0x0C149320}
PLACE = {0x0C146F3A: "place", 0x0C1432C0: "anim?", 0x0C146006: "anim?", 0x0C145A0E: "f_0C145A0E",
         0x0C13790C: "kaadu?", 0x0C137AD6: "kaadu?", 0x0C136A0A: "kaadu?"}


def f32(im, a): return struct.unpack("<f", struct.pack("<I", im.u32(a)))[0]


def run(im, X, start):
    ins = sh4an.flow(im, start)
    R = {}; Fr = {}; slot_of = {}; out = []
    pending = None
    for a in ins:
        op = im.u16(a); t, I = decode(op, a)
        k = t.split()[0]; ops = t[len(k):].strip()
        def ireg(s): return int(s[1:]) if re.fullmatch(r"r\d+", s) else None
        if "lit" in I:
            v = im.u32(I["lit"]) if I["litsize"] == 4 else (lambda x: x - 0x10000 if x & 0x8000 else x)(im.u16(I["lit"]))
            R[I["dst"]] = v & 0xFFFFFFFF
        elif "mova" in I: R[0] = I["mova"]
        elif "imm" in I: R[I["dst"]] = I["imm"] & 0xFFFFFFFF
        elif k == "add" and ops.startswith("#"):
            n = int(ops[1:].split(",")[0]); r = ireg(ops.split(",")[1])
            if r in R: R[r] = (R[r] + n) & 0xFFFFFFFF
        elif k == "mov" and re.fullmatch(r"r\d+,r\d+", ops):
            s, d = map(ireg, ops.split(","))
            if s in R: R[d] = R[s]
            else: R.pop(d, None)
        elif k == "mov.l" and re.fullmatch(r"@r\d+,r\d+", ops):
            s, d = ops[1:].split(","); s, d = ireg(s), ireg(d)
            if s in R and R[s] in slot_of: R[d] = ("slot", R[s])
            else: R.pop(d, None)
        elif k == "mov.l" and re.fullmatch(r"r0,@r\d+", ops) and pending is not None:
            d = ireg(ops.split("@")[1])
            if d in R: slot_of[R[d]] = pending; out.append("  load obj %s -> slot %08X" % (pending, R[d]))
            pending = None
        elif k == "fmov.s" and re.fullmatch(r"@r0,fr\d+", ops) and 0 in R and isinstance(R[0], int) and im.ok(R[0], 4):
            Fr[int(ops.split("fr")[1])] = f32(im, R[0])
        elif k == "fldi0": Fr[int(ops[2:])] = 0.0
        elif k == "fldi1": Fr[int(ops[2:])] = 1.0
        elif k == "fmov" and re.fullmatch(r"fr\d+,fr\d+", ops):
            s, d = [int(x[2:]) for x in ops.split(",")]
            if s in Fr: Fr[d] = Fr[s]
            else: Fr.pop(d, None)
        elif k == "fneg":
            r = int(ops[2:])
            if r in Fr: Fr[r] = -Fr[r]
        elif k in ("fadd", "fsub", "fmul") and re.fullmatch(r"fr\d+,fr\d+", ops):
            s, d = [int(x[2:]) for x in ops.split(",")]
            if s in Fr and d in Fr:
                Fr[d] = Fr[d] + Fr[s] if k == "fadd" else Fr[d] - Fr[s] if k == "fsub" else Fr[d] * Fr[s]
            else: Fr.pop(d, None)
        elif k in ("fmov.s",) and re.fullmatch(r"@.*,fr\d+", ops):
            Fr.pop(int(ops.split("fr")[-1]), None)
        else:
            parts = ops.split(",")
            if parts and ireg(parts[-1]) is not None and not k.startswith(("cmp", "tst", "mov.l r", "mov.w r", "mov.b r")):
                if not (k.startswith("mov") and ops.startswith("r") and "@" in ops):
                    R.pop(ireg(parts[-1]), None)
        tgt = None
        if k in ("jsr", "jmp"):
            r = int(ops[2:]); tgt = R.get(r)
        elif k == "bsr": tgt = I["target"]
        if tgt is not None or k in ("jsr", "bsr"):
            # delay slot executes first; peek it
            dop = im.u16(a + 2); dt, DI = decode(dop, a + 2)
            dk = dt.split()[0]; dops = dt[len(dk):].strip()
            if "imm" in DI: R[DI["dst"]] = DI["imm"] & 0xFFFFFFFF
            if "lit" in DI and DI["litsize"] == 2:
                v = im.u16(DI["lit"]); R[DI["dst"]] = (v - 0x10000 if v & 0x8000 else v) & 0xFFFFFFFF
            m = re.fullmatch(r"@\((\d+),r(\d+)\),r(\d+)", dops) if dk == "mov.l" else None
            if m: R.pop(int(m.group(3)), None)
            if dk == "fmov" and re.fullmatch(r"fr\d+,fr\d+", dops):
                s, d = [int(x[2:]) for x in dops.split(",")]
                if s in Fr: Fr[d] = Fr[s]
            if tgt in LOADERS:
                pending = R.get(4)
                pend_txt = pending
                pending = ("%d" % pending) if isinstance(pending, int) and pending < 0x10000 else (
                    "E%d" % (pending & 0xFFFF) if isinstance(pending, int) and pending & 0x40000000 else str(pending))
            elif tgt in PLACE:
                r5 = R.get(5)
                obj = slot_of.get(r5[1]) if isinstance(r5, tuple) else None
                fv = [Fr.get(i) for i in (4, 5, 6, 7)]
                out.append("  %s @%08X obj %s (slot %s)  fr4..7 = %s" % (PLACE[tgt], a, obj,
                    ("%08X" % r5[1]) if isinstance(r5, tuple) else r5,
                    ", ".join("%.2f" % v if v is not None else "?" for v in fv)))
            for r in range(8):
                if r not in (4, 5, 6, 7) or True: R.pop(r, None)
            for r in range(12): Fr.pop(r, None)
    return out


def main():
    im = Image(sys.argv[1]); X = sh4an.load(im)
    for s in sys.argv[2:]:
        a = int(s, 16)
        print("== hook f_%08X" % a)
        for l in run(im, X, a): print(l)


if __name__ == "__main__":
    main()
