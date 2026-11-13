#!/usr/bin/env python3
"""Whole-program analysis for the Racer Arcade EPROM program (SH-4 LE).

Builds: strings, PC-relative literal references (mov.l/mov.w/mova), call graph (bsr, and
jsr @rN where rN was loaded from a literal shortly before), function starts (call targets),
data-pointer references to strings.

    python -I sh4an.py IMAGE build            -> writes analysis\\xref.pkl
    python -I sh4an.py IMAGE str  REGEX       -> strings matching REGEX with code/data refs + containing func
    python -I sh4an.py IMAGE refs HEXVALUE    -> code refs to a 32-bit constant
    python -I sh4an.py IMAGE func HEXADDR [N] -> disassemble function containing addr (N instrs max)
    python -I sh4an.py IMAGE callers HEXADDR  -> call sites of function
    python -I sh4an.py IMAGE calls HEXADDR    -> calls made by function
"""
import sys, os, re, pickle, bisect, collections
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, decode, BASE_DEFAULT

# code ranges (CPU addresses): boot stub and main program (see notes\program-map.md)
CODE = [(0x0C010000, 0x0C016D24), (0x0C100000, 0x0C160000)]
HERE = os.path.dirname(os.path.abspath(__file__))
PKL = os.path.join(HERE, "..", "analysis", "xref.pkl")


def in_code(a): return any(lo <= a < hi for lo, hi in CODE)


def build(im):
    data = im.data; base = im.base
    w16 = np.frombuffer(data[:len(data) // 2 * 2], dtype="<u2")
    # strings
    strings = {}
    for m in re.finditer(rb"[\x20-\x7E\t\n]{3,}\x00", data):
        strings[base + m.start()] = m.group()[:-1].decode("ascii")
    lit_refs = collections.defaultdict(list)    # value -> [instr addr]
    litslot = {}                                 # instr addr -> (slot, value)
    calls = collections.defaultdict(set)         # caller site -> target
    for lo, hi in CODE:
        o0 = (lo - base) // 2; o1 = (hi - base) // 2
        seg = w16[o0:o1]
        idx = np.nonzero((seg >> 12) == 0xD)[0]
        for i in idx.tolist():
            pc = lo + i * 2; op = int(seg[i])
            L = ((pc & ~3) + 4 + (op & 0xFF) * 4)
            if not im.ok(L, 4): continue
            v = im.u32(L); lit_refs[v].append(pc); litslot[pc] = (L, v, (op >> 8) & 15)
        idx = np.nonzero((seg >> 12) == 0xC)[0]
        for i in idx.tolist():
            op = int(seg[i])
            if (op >> 8) & 15 != 7: continue
            pc = lo + i * 2; L = ((pc & ~3) + 4 + (op & 0xFF) * 4)
            lit_refs[L].append(pc)               # mova: address itself is the reference
        idx = np.nonzero((seg >> 12) == 0xB)[0]
        for i in idx.tolist():
            pc = lo + i * 2; op = int(seg[i])
            d = op & 0xFFF; d = d - 0x1000 if d & 0x800 else d
            calls[pc] = pc + 4 + d * 2
        idx = np.nonzero(((seg & 0xF0FF) == 0x400B) | ((seg & 0xF0FF) == 0x402B))[0]
        for i in idx.tolist():
            pc = lo + i * 2; r = (int(seg[i]) >> 8) & 15
            for k in range(1, 16):
                q = pc - 2 * k
                if q in litslot and litslot[q][2] == r:
                    t = litslot[q][1]
                    if in_code(t) and not (t & 1):
                        calls[pc] = t
                    break
                op = int(w16[(q - base) // 2])
                # stop at anything else that obviously writes r (mov rm,rN / mov.l @..,rN etc.)
                if (op >> 12) in (6, 0xE, 9) and ((op >> 8) & 15) == r: break
                if (op >> 12) == 5 and ((op >> 8) & 15) == r: break
    funcs = set(calls.values())
    # also literal values that point at a prologue are probably function pointers
    for v in list(lit_refs):
        if in_code(v) and not (v & 1) and (im.u16(v) == 0x4F22 or (im.u16(v) & 0xFF0F) == 0x2F06):
            funcs.add(v)
    # data pointers to strings (aligned words anywhere outside code)
    w32 = np.frombuffer(data[:len(data) // 4 * 4], dtype="<u4")
    sarr = np.array(sorted(strings), dtype=np.uint32)
    hit = np.isin(w32, sarr)
    data_refs = collections.defaultdict(list)
    for i in np.nonzero(hit)[0].tolist():
        a = base + i * 4
        if not in_code(a): data_refs[int(w32[i])].append(a)
    return dict(strings=strings, lit_refs=dict(lit_refs), calls=dict(calls),
                funcs=sorted(funcs), data_refs=dict(data_refs))


def load(im):
    if not os.path.exists(PKL):
        os.makedirs(os.path.dirname(PKL), exist_ok=True)
        X = build(im); pickle.dump(X, open(PKL, "wb"))
    return pickle.load(open(PKL, "rb"))


def func_of(X, a):
    f = X["funcs"]; i = bisect.bisect_right(f, a) - 1
    return f[i] if i >= 0 else None


def func_end(X, im, start, maxn=4000):
    """End = first rts (+delay slot) after which the next call-target starts or we hit maxn."""
    f = X["funcs"]; i = bisect.bisect_right(f, start)
    nxt = f[i] if i < len(f) else start + maxn * 2
    return min(nxt, start + maxn * 2)


def flow(im, start, limit=20000):
    """Recursive descent from start: returns sorted instruction addresses reachable without
    following calls (bsr/jsr return to the next instruction). Stops paths at rts/rte/jmp/braf."""
    seen = set(); work = [start]
    while work and len(seen) < limit:
        a = work.pop()
        while a not in seen and im.ok(a, 2):
            seen.add(a)
            txt, I = decode(im.u16(a), a)
            if txt.startswith(".word"): break
            k = txt.split()[0]
            if I.get("delayed"):
                seen.add(a + 2)
                if k in ("bra",): work.append(I["target"]); break
                if k in ("bt/s", "bf/s"): work.append(I["target"]); a += 4; continue
                if k in ("rts", "rte", "jmp", "braf"): break
                a += 4; continue          # bsr/jsr/bsrf: fall through after delay slot
            if k in ("bt", "bf"): work.append(I["target"])
            a += 2
    return sorted(seen)


def labels_for(X):
    L = {}
    for a in X["funcs"]: L[a] = "f_%08X" % a
    return L


def main():
    im = Image(sys.argv[1], BASE_DEFAULT)
    cmd = sys.argv[2]
    if cmd == "build":
        if os.path.exists(PKL): os.remove(PKL)
        X = load(im); print("strings %d, literal values %d, call sites %d, funcs %d" % (
            len(X["strings"]), len(X["lit_refs"]), len(X["calls"]), len(X["funcs"]))); return
    X = load(im)
    if cmd == "str":
        rx = re.compile(sys.argv[3])
        for a, s in sorted(X["strings"].items()):
            if not rx.search(s): continue
            cr = X["lit_refs"].get(a, []); dr = X["data_refs"].get(a, [])
            print("%08X %r" % (a, s))
            for c in cr: print("    code %08X in f_%08X" % (c, func_of(X, c) or 0))
            for d in dr: print("    data %08X" % d)
    elif cmd == "refs":
        v = int(sys.argv[3], 16)
        for c in X["lit_refs"].get(v, []): print("code %08X in f_%08X" % (c, func_of(X, c) or 0))
        for d in X["data_refs"].get(v, []): print("data %08X" % d)
    elif cmd == "func":
        a = int(sys.argv[3], 16); n = 600
        s = func_of(X, a) if "--exact" not in sys.argv else a
        L = labels_for(X)
        ins = flow(im, s)
        # print contiguous runs of reachable code, each run via the linear disassembler
        runs = []; r0 = ins[0]; prev = ins[0]
        for x in ins[1:]:
            if x != prev + 2: runs.append((r0, prev)); r0 = x
            prev = x
        runs.append((r0, prev))
        for (r0, r1) in runs:
            for line in im.disasm(r0, (r1 - r0) // 2 + 1, labels=L): print(line)
            print("         ----")
    elif cmd == "callers":
        a = int(sys.argv[3], 16)
        for site, t in sorted(X["calls"].items()):
            if t == a: print("%08X in f_%08X" % (site, func_of(X, site) or 0))
    elif cmd == "calls":
        a = int(sys.argv[3], 16); s = func_of(X, a); e = func_end(X, im, s)
        for site, t in sorted(X["calls"].items()):
            if s <= site < e: print("%08X -> f_%08X" % (site, t))


if __name__ == "__main__":
    main()
