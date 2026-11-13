"""Minimal inflate that lists deflate blocks and their symbol counts (diagnostics only)."""
import zlib


class Bits:
    def __init__(self, d):
        self.d = d; self.p = 0; self.b = 0; self.n = 0

    def get(self, k):
        while self.n < k:
            self.b |= self.d[self.p] << self.n; self.p += 1; self.n += 8
        v = self.b & ((1 << k) - 1); self.b >>= k; self.n -= k
        return v


def huff(lengths):
    code = 0; tab = {}
    mx = max(lengths) if lengths else 0
    bl = [0] * (mx + 2)
    for l in lengths:
        if l: bl[l] += 1
    nxt = [0] * (mx + 2)
    for b in range(1, mx + 1):
        code = (code + bl[b - 1]) << 1; nxt[b] = code
    for s, l in enumerate(lengths):
        if l:
            tab[(l, nxt[l])] = s; nxt[l] += 1
    return tab


def dec(bs, tab):
    c = 0; l = 0
    while True:
        c = (c << 1) | bs.get(1); l += 1
        if (l, c) in tab: return tab[(l, c)]


LB = [3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258]
LE = [0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0]
DB = [1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577]
DE = [0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13]


def trace(z):
    bs = Bits(z[2:]); out = []; blocks = []
    while True:
        final = bs.get(1); t = bs.get(2); start = len(out); nl = nm = 0
        toks = []
        if t == 0:
            bs.b = 0; bs.n = 0
            ln = bs.d[bs.p] | bs.d[bs.p + 1] << 8; bs.p += 4
            out += bs.d[bs.p:bs.p + ln]; bs.p += ln
        else:
            if t == 1:
                ll = [8] * 144 + [9] * 112 + [7] * 24 + [8] * 8; dl = [5] * 30
            else:
                hl = bs.get(5) + 257; hd = bs.get(5) + 1; hc = bs.get(4) + 4
                order = [16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15]
                cl = [0] * 19
                for i in range(hc): cl[order[i]] = bs.get(3)
                ct = huff(cl); lens = []
                while len(lens) < hl + hd:
                    s = dec(bs, ct)
                    if s < 16: lens.append(s)
                    elif s == 16: lens += [lens[-1]] * (3 + bs.get(2))
                    elif s == 17: lens += [0] * (3 + bs.get(3))
                    else: lens += [0] * (11 + bs.get(7))
                ll = lens[:hl]; dl = lens[hl:]
            lt = huff(ll); dt = huff(dl)
            while True:
                s = dec(bs, lt)
                if s < 256: out.append(s); nl += 1; toks.append(('L', s))
                elif s == 256: break
                else:
                    s -= 257; ln = LB[s] + bs.get(LE[s])
                    ds = dec(bs, dt); dist = DB[ds] + bs.get(DE[ds])
                    for _ in range(ln): out.append(out[-dist])
                    nm += 1; toks.append(('M', ln, dist))
        blocks.append((t, start, len(out) - start, nl, nm, toks))
        if final: break
    return bytes(out), blocks
