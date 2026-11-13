#!/usr/bin/env python3
"""SH-4 (little endian) disassembler, written from the SH-4 programming manual encodings.

Library use:
    from sh4dis import Image, decode
    im = Image(path, base=0x0C00FC00)        # file offset 0 == address base
    for line in im.disasm(addr, count): print(line)

CLI:
    python -I sh4dis.py IMAGE ADDR [COUNT] [--base B] [--file-offset]
      ADDR is a CPU address (hex) unless --file-offset is given.

Features: full 16-bit SH-4 ISA incl. FPU (fmov sz=0/1 forms printed as frN; DR/XD naming
is ambiguous without FPSCR.SZ and is noted as fr), PC-relative mov.w / mov.l / mova literal
resolution, branch targets, delay slots (printed with a leading '_'), and light constant
tracking so that 'mov.l @(lit),rN ; jsr @rN' is annotated with the call target.
"""
import sys, struct

BASE_DEFAULT = 0x0C00FC00

def s8(x):  return x - 0x100 if x & 0x80 else x
def s12(x): return x - 0x1000 if x & 0x800 else x

# kind flags
BR, CBR, JMPR, RET, DELAY = 1, 2, 4, 8, 16

def decode(op, pc):
    """Return (text, info) for 16-bit opcode op at address pc.
    info: dict with optional keys: target (branch target), delayed (bool), kind,
          lit (literal address), litsize (2/4), mova (address), call (bool)"""
    n = (op >> 8) & 15; m = (op >> 4) & 15; d4 = op & 15; i8 = op & 0xFF
    hi = op >> 12; lo = op & 15
    R = lambda r: "r%d" % r
    F = lambda r: "fr%d" % r
    I = {}
    if hi == 0:
        fixed = {0x0009: "nop", 0x000B: "rts", 0x0008: "clrt", 0x0018: "sett", 0x0028: "clrmac",
                 0x0019: "div0u", 0x002B: "rte", 0x0038: "ldtlb", 0x001B: "sleep",
                 0x0048: "clrs", 0x0058: "sets"}
        if op in fixed:
            t = fixed[op]
            if op == 0x000B: I.update(delayed=True, kind=RET)
            if op == 0x002B: I.update(delayed=True, kind=RET)
            return t, I
        lb = op & 0xFF
        if lo == 2:
            if m & 8: return "stc r%d_bank,%s" % (m & 7, R(n)), I
            names = {0: "sr", 1: "gbr", 2: "vbr", 3: "ssr", 4: "spc"}
            if m in names: return "stc %s,%s" % (names[m], R(n)), I
        if lb == 0x3A: return "stc sgr,%s" % R(n), I
        if lb == 0xFA: return "stc dbr,%s" % R(n), I
        if lo == 0xA:
            names = {0: "mach", 1: "macl", 2: "pr", 5: "fpul", 6: "fpscr"}
            if m in names: return "sts %s,%s" % (names[m], R(n)), I
        if lb == 0x29: return "movt %s" % R(n), I
        if lb == 0x03: I.update(delayed=True, kind=JMPR, call=True); return "bsrf %s" % R(n), I
        if lb == 0x23: I.update(delayed=True, kind=JMPR); return "braf %s" % R(n), I
        if lb == 0x83: return "pref @%s" % R(n), I
        if lb == 0x93: return "ocbi @%s" % R(n), I
        if lb == 0xA3: return "ocbp @%s" % R(n), I
        if lb == 0xB3: return "ocbwb @%s" % R(n), I
        if lb == 0xC3: return "movca.l r0,@%s" % R(n), I
        if lo == 4: return "mov.b %s,@(r0,%s)" % (R(m), R(n)), I
        if lo == 5: return "mov.w %s,@(r0,%s)" % (R(m), R(n)), I
        if lo == 6: return "mov.l %s,@(r0,%s)" % (R(m), R(n)), I
        if lo == 7: return "mul.l %s,%s" % (R(m), R(n)), I
        if lo == 0xC: return "mov.b @(r0,%s),%s" % (R(m), R(n)), I
        if lo == 0xD: return "mov.w @(r0,%s),%s" % (R(m), R(n)), I
        if lo == 0xE: return "mov.l @(r0,%s),%s" % (R(m), R(n)), I
        if lo == 0xF: return "mac.l @%s+,@%s+" % (R(m), R(n)), I
    elif hi == 1:
        return "mov.l %s,@(%d,%s)" % (R(m), d4 * 4, R(n)), I
    elif hi == 2:
        t = {0: "mov.b %s,@%s", 1: "mov.w %s,@%s", 2: "mov.l %s,@%s", 4: "mov.b %s,@-%s",
             5: "mov.w %s,@-%s", 6: "mov.l %s,@-%s", 7: "div0s %s,%s", 8: "tst %s,%s",
             9: "and %s,%s", 10: "xor %s,%s", 11: "or %s,%s", 12: "cmp/str %s,%s",
             13: "xtrct %s,%s", 14: "mulu.w %s,%s", 15: "muls.w %s,%s"}
        if lo in t: return t[lo] % (R(m), R(n)), I
    elif hi == 3:
        t = {0: "cmp/eq", 2: "cmp/hs", 3: "cmp/ge", 4: "div1", 5: "dmulu.l", 6: "cmp/hi",
             7: "cmp/gt", 8: "sub", 10: "subc", 11: "subv", 12: "add", 13: "dmuls.l",
             14: "addc", 15: "addv"}
        if lo in t: return "%s %s,%s" % (t[lo], R(m), R(n)), I
    elif hi == 4:
        lb = op & 0xFF
        t = {0x00: "shll", 0x10: "dt", 0x20: "shal", 0x01: "shlr", 0x11: "cmp/pz", 0x21: "shar",
             0x04: "rotl", 0x24: "rotcl", 0x05: "rotr", 0x15: "cmp/pl", 0x25: "rotcr",
             0x08: "shll2", 0x18: "shll8", 0x28: "shll16", 0x09: "shlr2", 0x19: "shlr8",
             0x29: "shlr16"}
        if lb in t: return "%s %s" % (t[lb], R(n)), I
        if lb == 0x0B: I.update(delayed=True, kind=JMPR, call=True); return "jsr @%s" % R(n), I
        if lb == 0x1B: return "tas.b @%s" % R(n), I
        if lb == 0x2B: I.update(delayed=True, kind=JMPR); return "jmp @%s" % R(n), I
        if lo == 0xC: return "shad %s,%s" % (R(m), R(n)), I
        if lo == 0xD: return "shld %s,%s" % (R(m), R(n)), I
        if lo == 0xF: return "mac.w @%s+,@%s+" % (R(m), R(n)), I
        cr = {0: "sr", 1: "gbr", 2: "vbr", 3: "ssr", 4: "spc"}
        if lo == 0xE:
            if m & 8: return "ldc %s,r%d_bank" % (R(n), m & 7), I
            if m in cr: return "ldc %s,%s" % (R(n), cr[m]), I
        if lb == 0xFA: return "ldc %s,dbr" % R(n), I
        if lo == 7:
            if m & 8: return "ldc.l @%s+,r%d_bank" % (R(n), m & 7), I
            if m in cr: return "ldc.l @%s+,%s" % (R(n), cr[m]), I
        if lb == 0xF6: return "ldc.l @%s+,dbr" % R(n), I
        if lo == 3:
            if m & 8: return "stc.l r%d_bank,@-%s" % (m & 7, R(n)), I
            if m in cr: return "stc.l %s,@-%s" % (cr[m], R(n)), I
        if lb == 0x32: return "stc.l sgr,@-%s" % R(n), I
        if lb == 0xF2: return "stc.l dbr,@-%s" % R(n), I
        sr = {0: "mach", 1: "macl", 2: "pr", 5: "fpul", 6: "fpscr"}
        if lo == 0xA and m in sr: return "lds %s,%s" % (R(n), sr[m]), I
        if lo == 6 and m in sr: return "lds.l @%s+,%s" % (R(n), sr[m]), I
        if lo == 2 and m in sr: return "sts.l %s,@-%s" % (sr[m], R(n)), I
    elif hi == 5:
        return "mov.l @(%d,%s),%s" % (d4 * 4, R(m), R(n)), I
    elif hi == 6:
        t = {0: "mov.b @%s,%s", 1: "mov.w @%s,%s", 2: "mov.l @%s,%s", 3: "mov %s,%s",
             4: "mov.b @%s+,%s", 5: "mov.w @%s+,%s", 6: "mov.l @%s+,%s", 7: "not %s,%s",
             8: "swap.b %s,%s", 9: "swap.w %s,%s", 10: "negc %s,%s", 11: "neg %s,%s",
             12: "extu.b %s,%s", 13: "extu.w %s,%s", 14: "exts.b %s,%s", 15: "exts.w %s,%s"}
        return t[lo] % (R(m), R(n)), I
    elif hi == 7:
        return "add #%d,%s" % (s8(i8), R(n)), I
    elif hi == 8:
        if n == 0: return "mov.b r0,@(%d,%s)" % (d4, R(m)), I
        if n == 1: return "mov.w r0,@(%d,%s)" % (d4 * 2, R(m)), I
        if n == 4: return "mov.b @(%d,%s),r0" % (d4, R(m)), I
        if n == 5: return "mov.w @(%d,%s),r0" % (d4 * 2, R(m)), I
        if n == 8: return "cmp/eq #%d,r0" % s8(i8), I
        if n in (9, 11, 13, 15):
            tg = (pc + 4 + s8(i8) * 2) & 0xFFFFFFFF
            nm = {9: "bt", 11: "bf", 13: "bt/s", 15: "bf/s"}[n]
            I.update(target=tg, kind=CBR, delayed=n in (13, 15))
            return "%s 0x%08X" % (nm, tg), I
    elif hi == 9:
        a = (pc + 4 + i8 * 2) & 0xFFFFFFFF
        I.update(lit=a, litsize=2, dst=n)
        return "mov.w @(0x%08X),%s" % (a, R(n)), I
    elif hi in (0xA, 0xB):
        tg = (pc + 4 + s12(op & 0xFFF) * 2) & 0xFFFFFFFF
        I.update(target=tg, kind=BR, delayed=True, call=(hi == 0xB))
        return "%s 0x%08X" % ("bra" if hi == 0xA else "bsr", tg), I
    elif hi == 0xC:
        g = {0: ("mov.b r0,@(%d,gbr)", 1), 1: ("mov.w r0,@(%d,gbr)", 2), 2: ("mov.l r0,@(%d,gbr)", 4),
             4: ("mov.b @(%d,gbr),r0", 1), 5: ("mov.w @(%d,gbr),r0", 2), 6: ("mov.l @(%d,gbr),r0", 4)}
        if n in g: return g[n][0] % (i8 * g[n][1]), I
        if n == 3: return "trapa #0x%02X" % i8, I
        if n == 7:
            a = ((pc & ~3) + 4 + i8 * 4) & 0xFFFFFFFF
            I.update(mova=a)
            return "mova @(0x%08X),r0" % a, I
        t = {8: "tst #0x%02X,r0", 9: "and #0x%02X,r0", 10: "xor #0x%02X,r0", 11: "or #0x%02X,r0",
             12: "tst.b #0x%02X,@(r0,gbr)", 13: "and.b #0x%02X,@(r0,gbr)",
             14: "xor.b #0x%02X,@(r0,gbr)", 15: "or.b #0x%02X,@(r0,gbr)"}
        return t[n] % i8, I
    elif hi == 0xD:
        a = ((pc & ~3) + 4 + i8 * 4) & 0xFFFFFFFF
        I.update(lit=a, litsize=4, dst=n)
        return "mov.l @(0x%08X),%s" % (a, R(n)), I
    elif hi == 0xE:
        I.update(imm=s8(i8), dst=n)
        return "mov #%d,%s" % (s8(i8), R(n)), I
    elif hi == 0xF:
        t = {0: "fadd", 1: "fsub", 2: "fmul", 3: "fdiv", 4: "fcmp/eq", 5: "fcmp/gt", 12: "fmov"}
        if lo in t: return "%s %s,%s" % (t[lo], F(m), F(n)), I
        if lo == 6: return "fmov.s @(r0,%s),%s" % (R(m), F(n)), I
        if lo == 7: return "fmov.s %s,@(r0,%s)" % (F(m), R(n)), I
        if lo == 8: return "fmov.s @%s,%s" % (R(m), F(n)), I
        if lo == 9: return "fmov.s @%s+,%s" % (R(m), F(n)), I
        if lo == 10: return "fmov.s %s,@%s" % (F(m), R(n)), I
        if lo == 11: return "fmov.s %s,@-%s" % (F(m), R(n)), I
        if lo == 14: return "fmac fr0,%s,%s" % (F(m), F(n)), I
        if lo == 0xD:
            if op == 0xFBFD: return "frchg", I
            if op == 0xF3FD: return "fschg", I
            t1 = {0: "fsts fpul,%s", 2: "float fpul,%s", 4: "fneg %s", 5: "fabs %s", 6: "fsqrt %s",
                  7: "fsrra %s", 8: "fldi0 %s", 9: "fldi1 %s"}
            if m in t1: return t1[m] % F(n), I
            if m == 1: return "flds %s,fpul" % F(n), I
            if m == 3: return "ftrc %s,fpul" % F(n), I
            if m == 0xA: return "fcnvsd fpul,dr%d" % n, I
            if m == 0xB: return "fcnvds dr%d,fpul" % n, I
            if m == 0xE: return "fipr fv%d,fv%d" % ((n & 3) * 4, (n >> 2) * 4), I
            if m == 0xF and (n & 3) == 1: return "ftrv xmtrx,fv%d" % ((n >> 2) * 4), I
            if m == 0xF and (n & 1) == 0: return "fsca fpul,dr%d" % n, I
    return ".word 0x%04X" % op, I


class Image:
    def __init__(self, path, base=BASE_DEFAULT):
        self.data = open(path, "rb").read()
        self.base = base
        self.end = base + len(self.data)

    def ok(self, a, n=1): return self.base <= a and a + n <= self.end
    def u16(self, a): return struct.unpack_from("<H", self.data, a - self.base)[0]
    def u32(self, a): return struct.unpack_from("<I", self.data, a - self.base)[0]
    def u8(self, a): return self.data[a - self.base]

    def cstr(self, a, maxlen=80):
        """Printable NUL-terminated string at a, or None."""
        if not self.ok(a): return None
        o = a - self.base; out = bytearray()
        while o < len(self.data) and len(out) < maxlen:
            c = self.data[o]
            if c == 0: break
            if c < 0x20 and c not in (9, 10, 13): return None
            if c > 0x7E: return None
            out.append(c); o += 1
        if len(out) < 2: return None
        return out.decode("ascii").replace("\n", "\\n")

    def describe(self, v):
        s = self.cstr(v) if (v & 0) == 0 else None
        return ' "%s"' % s if s else ""

    def disasm(self, addr, count=64, labels=None, track=True):
        """Yield text lines for count instructions starting at addr."""
        labels = labels or {}
        regs = {}
        delay_next = False
        pending_reset = False
        a = addr
        for _ in range(count):
            if not self.ok(a, 2): break
            op = self.u16(a)
            txt, I = decode(op, a)
            cmt = []
            if "lit" in I and self.ok(I["lit"], I["litsize"]):
                if I["litsize"] == 2:
                    v = self.u16(I["lit"]); v = v - 0x10000 if v & 0x8000 else v
                    cmt.append("=0x%X (%d)" % (v & 0xFFFFFFFF, v)); regs[I["dst"]] = v & 0xFFFFFFFF
                else:
                    v = self.u32(I["lit"]); cmt.append("=0x%08X%s%s" % (v, self.describe(v),
                        (" <%s>" % labels[v]) if v in labels else ""))
                    regs[I["dst"]] = v
            elif "mova" in I:
                cmt.append("r0=0x%08X%s" % (I["mova"], self.describe(I["mova"]))); regs[0] = I["mova"]
            elif "imm" in I:
                regs[I["dst"]] = I["imm"] & 0xFFFFFFFF
            else:
                # crude invalidation: any write to rN destroys tracking
                parts = txt.split()
                if len(parts) > 1 and parts[1].split(",")[-1].startswith("r"):
                    dst = parts[1].split(",")[-1]
                    if dst[1:].isdigit() and not txt.startswith(("cmp", "tst", "mov.l r", "mov.w r", "mov.b r")):
                        regs.pop(int(dst[1:]), None)
            if I.get("kind") == JMPR and txt.split()[0] in ("jsr", "jmp"):
                r = int(txt.split("@r")[1])
                if r in regs:
                    t = regs[r]; cmt.append("-> 0x%08X%s" % (t, (" <%s>" % labels[t]) if t in labels else ""))
            if "target" in I and I["target"] in labels:
                cmt.append("<%s>" % labels[I["target"]])
            pre = "_" if delay_next else " "
            lab = ("%s:\n" % labels[a]) if a in labels else ""
            yield "%s%08X: %04X %s%-30s%s" % (lab, a, op, pre, txt, ("; " + "  ".join(cmt)) if cmt else "")
            if pending_reset:
                regs = {}; pending_reset = False
            delay_next = bool(I.get("delayed"))
            if I.get("kind") in (BR, JMPR, RET) and not I.get("call"):
                pending_reset = True      # after the delay slot, flow does not fall through
            if txt.startswith(("jsr", "bsr")):
                for r in range(8): regs.pop(r, None)   # caller-saved clobbered after call
            a += 2


def selftest():
    """Check decoder against hand-assembled encodings from the SH-4 manual."""
    cases = [
        (0x0009, 0, "nop"), (0x000B, 0, "rts"), (0x6013, 0, "mov r1,r0"),
        (0xE1A0, 0, "mov #-96,r1"), (0x4128, 0, "shll16 r1"), (0x7F04, 0, "add #4,r15"),
        (0x4F22, 0, "sts.l pr,@-r15"), (0x4F26, 0, "lds.l @r15+,pr"), (0x2F86, 0, "mov.l r8,@-r15"),
        (0x430B, 0, "jsr @r3"), (0x402B, 0, "jmp @r0"), (0x8901, 0x1000, "bt 0x00001006"),
        (0x8BFE, 0x1000, "bf 0x00001000"), (0xAFFE, 0x1000, "bra 0x00001000"),
        (0xB000, 0x1000, "bsr 0x00001004"), (0xD101, 0x1002, "mov.l @(0x00001008),r1"),
        (0x9101, 0x1002, "mov.w @(0x00001008),r1"), (0xC701, 0x1002, "mova @(0x00001008),r0"),
        (0xF12C, 0, "fmov fr2,fr1"), (0xF00D, 0, "fsts fpul,fr0"), (0xF01D, 0, "flds fr0,fpul"),
        (0xF22D, 0, "float fpul,fr2"), (0xF33D, 0, "ftrc fr3,fpul"), (0xF18D, 0, "fldi0 fr1"),
        (0xF19D, 0, "fldi1 fr1"), (0xFBFD, 0, "frchg"), (0xF3FD, 0, "fschg"),
        (0xF4ED, 0, "fipr fv0,fv4"), (0xF9FD, 0, "ftrv xmtrx,fv8"), (0xF56E, 0, "fmac fr0,fr6,fr5"),
        (0x4F6A, 0, "lds r15,fpscr"), (0x0F5A, 0, "sts fpul,r15"), (0x400E, 0, "ldc r0,sr"),
        (0x412E, 0, "ldc r1,vbr"), (0x0183, 0, "pref @r1"), (0x4A1C, 0, "shad r1,r10"),
        (0x3E4C, 0, "add r4,r14"), (0x52F3, 0, "mov.l @(12,r15),r2"), (0x1F23, 0, "mov.l r2,@(12,r15)"),
        (0x8524, 0, "mov.w @(8,r2),r0"), (0xC620, 0, "mov.l @(128,gbr),r0"), (0x0123, 0, "braf r1"),
        (0x4110, 0, "dt r1"), (0x88FF, 0, "cmp/eq #-1,r0"), (0xC90F, 0, "and #0x0F,r0"),
        (0x6E3C, 0, "extu.b r3,r14"), (0x0029, 0, "movt r0"),
    ]
    bad = 0
    for op, pc, want in cases:
        got = decode(op, pc)[0]
        if got != want:
            print("FAIL %04X: got %r want %r" % (op, got, want)); bad += 1
    # every opcode decodes without exception
    und = sum(1 for op in range(0x10000) if decode(op, 0)[0].startswith(".word"))
    print("selftest: %d/%d ok, %d undefined opcodes of 65536" % (len(cases) - bad, len(cases), und))
    return bad == 0


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--selftest":
        sys.exit(0 if selftest() else 1)
    args = [x for x in sys.argv[1:] if not x.startswith("--")]
    base = BASE_DEFAULT
    for x in sys.argv[1:]:
        if x.startswith("--base="): base = int(x[7:], 16)
    im = Image(args[0], base)
    a = int(args[1], 16)
    if "--file-offset" in sys.argv: a += base
    cnt = int(args[2], 0) if len(args) > 2 else 64
    for line in im.disasm(a, cnt): print(line)
