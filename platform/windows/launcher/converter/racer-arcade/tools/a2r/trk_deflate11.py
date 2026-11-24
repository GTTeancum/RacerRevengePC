r"""zlib 1.1.x-compatible deflate (level 6) for byte-identical RES3 blocks.

Finding (notes\a2r-track.md): the original RES3 blocks were written by a zlib 1.1.x encoder.
Its match finder gives exactly the same tokens as today's zlib at level 6 / memLevel 8, but
1.1.x's _tr_tally also ended a deflate block early when, at every 4096th symbol,
    matches < symbols/2  and  estimated_bits/8 < input_bytes/2
(that test moved under #ifdef TRUNCATE_BLOCK in zlib 1.2.0). So:
  1. tokens = today's zlib output, decoded to (literal | length,distance) tokens;
  2. block boundaries re-decided with the 1.1.x rule (plus the usual lit_bufsize-1 = 16383);
  3. each block Huffman-coded with a port of zlib trees.c (build_tree / gen_bitlen /
     gen_codes / build_bl_tree / stored-fixed-dynamic choice).
"""
import zlib
import numpy as np

LITERALS = 256
L_CODES = 286
D_CODES = 30
BL_CODES = 19
MAX_BITS = 15
MAX_BL_BITS = 7
HEAP_SIZE = 2 * L_CODES + 1
END_BLOCK = 256
REP_3_6, REPZ_3_10, REPZ_11_138 = 16, 17, 18
EXTRA_LBITS = [0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0]
EXTRA_DBITS = [0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13]
EXTRA_BLBITS = [0]*16 + [2, 3, 7]
BL_ORDER = [16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15]
LBASE = [3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258]
DBASE = [1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,
         6145,8193,12289,16385,24577]

# length (3..258) -> code index 0..28; distance (1..32768) -> dcode
LEN_CODE = [0] * 259
for c in range(29):
    for l in range(LBASE[c], LBASE[c] + (1 << EXTRA_LBITS[c])):
        if l <= 258:
            LEN_CODE[l] = c
LEN_CODE[258] = 28
DIST_CODE = [0] * 32769
for c in range(30):
    for d in range(DBASE[c], min(DBASE[c] + (1 << EXTRA_DBITS[c]), 32769)):
        DIST_CODE[d] = c


def _bitrev(code, ln):
    r = 0
    for _ in range(ln):
        r = (r << 1) | (code & 1); code >>= 1
    return r


def _gen_codes(lens, max_code):
    bl_count = [0] * (MAX_BITS + 1)
    for n in range(max_code + 1):
        bl_count[lens[n]] += 1
    nxt = [0] * (MAX_BITS + 1)
    code = 0
    for b in range(1, MAX_BITS + 1):
        code = (code + bl_count[b - 1]) << 1
        nxt[b] = code
    codes = [0] * len(lens)
    for n in range(max_code + 1):
        l = lens[n]
        if l:
            codes[n] = _bitrev(nxt[l], l); nxt[l] += 1
    return codes


STATIC_LLEN = [8]*144 + [9]*112 + [7]*24 + [8]*8
STATIC_LCODE = _gen_codes(STATIC_LLEN, 287)
STATIC_DLEN = [5] * 30
STATIC_DCODE = [_bitrev(n, 5) for n in range(30)]


# ---------------- decode today's zlib stream into tokens ----------------
def _table(lengths):
    mx = max(lengths)
    codes = _gen_codes(lengths + [0] * (MAX_BITS + 1), len(lengths) - 1) if mx else []
    size = 1 << mx
    tab = [None] * size
    for s, l in enumerate(lengths):
        if l:
            c = codes[s]
            for hi in range(0, size, 1 << l):
                tab[c | hi] = (s, l)
    return tab, mx


def tokens_of(z):
    """Decode a zlib stream into a token list: int literal, or (length, distance)."""
    data = np.frombuffer(z[2:], dtype=np.uint8)
    bits = np.unpackbits(data, bitorder='little').astype(np.int64)
    n = len(bits)
    pad = np.concatenate([bits, np.zeros(32, np.int64)])
    W = np.zeros(n, np.int64)
    for k in range(16):
        W |= pad[k:k + n] << k
    W = W.tolist()
    p = 0
    toks = []

    def get(k):
        nonlocal p
        v = W[p] & ((1 << k) - 1); p += k
        return v
    while True:
        final = get(1); t = get(2)
        if t == 0:
            p = (p + 7) & ~7
            ln = get(16); get(16)
            byte = p >> 3
            toks.extend(z[2 + byte:2 + byte + ln]); p += ln * 8
        else:
            if t == 1:
                ll, dl = STATIC_LLEN, STATIC_DLEN
            else:
                hl = get(5) + 257; hd = get(5) + 1; hc = get(4) + 4
                cl = [0] * 19
                for i in range(hc):
                    cl[BL_ORDER[i]] = get(3)
                ct, cm = _table(cl)
                lens = []
                while len(lens) < hl + hd:
                    s, l = ct[W[p] & ((1 << cm) - 1)]; p += l
                    if s < 16: lens.append(s)
                    elif s == 16: lens += [lens[-1]] * (3 + get(2))
                    elif s == 17: lens += [0] * (3 + get(3))
                    else: lens += [0] * (11 + get(7))
                ll = lens[:hl]; dl = lens[hl:]
            lt, lm = _table(ll)
            dt, dm = _table(dl) if max(dl) else (None, 0)
            lmask = (1 << lm) - 1
            dmask = (1 << dm) - 1
            while True:
                s, l = lt[W[p] & lmask]; p += l
                if s < 256:
                    toks.append(s)
                elif s == 256:
                    break
                else:
                    c = s - 257
                    e = EXTRA_LBITS[c]
                    ln = LBASE[c] + (W[p] & ((1 << e) - 1) if e else 0); p += e
                    ds, l2 = dt[W[p] & dmask]; p += l2
                    e = EXTRA_DBITS[ds]
                    dist = DBASE[ds] + (W[p] & ((1 << e) - 1) if e else 0); p += e
                    toks.append((ln, dist))
        if final:
            break
    return toks


# ---------------- trees.c port ----------------
def _build_tree(freq, static_len_tab, extra, extra_base, elems, max_length):
    """Returns (lens, max_code, opt_len_add, static_len_add). Port of zlib build_tree."""
    tree_freq = list(freq) + [0] * (HEAP_SIZE - len(freq))
    tree_len = [0] * HEAP_SIZE
    dad = [0] * HEAP_SIZE
    depth = [0] * HEAP_SIZE
    heap = [0] * HEAP_SIZE
    heap_len = 0
    heap_max = HEAP_SIZE
    max_code = -1
    for n in range(elems):
        if tree_freq[n] != 0:
            heap_len += 1; heap[heap_len] = max_code = n; depth[n] = 0
        else:
            tree_len[n] = 0
    opt = 0; stat = 0
    while heap_len < 2:
        if max_code < 2:
            max_code += 1; node = max_code
        else:
            node = 0
        heap_len += 1; heap[heap_len] = node
        tree_freq[node] = 1; depth[node] = 0; opt -= 1
        if static_len_tab is not None:
            stat -= static_len_tab[node]

    def smaller(n, m):
        return tree_freq[n] < tree_freq[m] or (tree_freq[n] == tree_freq[m] and depth[n] <= depth[m])

    def pqdownheap(k):
        v = heap[k]; j = k << 1
        while j <= heap_len:
            if j < heap_len and smaller(heap[j + 1], heap[j]):
                j += 1
            if smaller(v, heap[j]):
                break
            heap[k] = heap[j]; k = j; j <<= 1
        heap[k] = v

    for n in range(heap_len // 2, 0, -1):
        pqdownheap(n)
    node = elems
    while True:
        n = heap[1]; heap[1] = heap[heap_len]; heap_len -= 1; pqdownheap(1)
        m = heap[1]
        heap_max -= 1; heap[heap_max] = n
        heap_max -= 1; heap[heap_max] = m
        tree_freq[node] = tree_freq[n] + tree_freq[m]
        depth[node] = (depth[n] if depth[n] >= depth[m] else depth[m]) + 1
        dad[n] = dad[m] = node
        heap[1] = node; node += 1
        pqdownheap(1)
        if heap_len < 2:
            break
    heap_max -= 1; heap[heap_max] = heap[1]

    # gen_bitlen
    bl_count = [0] * (MAX_BITS + 1)
    tree_len[heap[heap_max]] = 0
    overflow = 0
    h = heap_max + 1
    while h < HEAP_SIZE:
        n = heap[h]
        bits = tree_len[dad[n]] + 1
        if bits > max_length:
            bits = max_length; overflow += 1
        tree_len[n] = bits
        if n > max_code:
            h += 1; continue
        bl_count[bits] += 1
        xbits = extra[n - extra_base] if n >= extra_base else 0
        f = tree_freq[n]
        opt += f * (bits + xbits)
        if static_len_tab is not None:
            stat += f * (static_len_tab[n] + xbits)
        h += 1
    if overflow:
        while True:
            bits = max_length - 1
            while bl_count[bits] == 0:
                bits -= 1
            bl_count[bits] -= 1
            bl_count[bits + 1] += 2
            bl_count[max_length] -= 1
            overflow -= 2
            if overflow <= 0:
                break
        h = HEAP_SIZE
        for bits in range(max_length, 0, -1):
            n = bl_count[bits]
            while n != 0:
                h -= 1
                m = heap[h]
                if m > max_code:
                    continue
                if tree_len[m] != bits:
                    opt += (bits - tree_len[m]) * tree_freq[m]
                    tree_len[m] = bits
                n -= 1
    return tree_len[:elems], max_code, opt, stat


def _scan_tree(lens, max_code, bl_freq):
    prevlen = -1; nextlen = lens[0]; count = 0
    max_count, min_count = (7, 4) if nextlen else (138, 3)
    ext = lens[:max_code + 1] + [0xFFFF]
    for n in range(max_code + 1):
        curlen = nextlen; nextlen = ext[n + 1]
        count += 1
        if count < max_count and curlen == nextlen:
            continue
        elif count < min_count:
            bl_freq[curlen] += count
        elif curlen != 0:
            if curlen != prevlen:
                bl_freq[curlen] += 1
            bl_freq[REP_3_6] += 1
        elif count <= 10:
            bl_freq[REPZ_3_10] += 1
        else:
            bl_freq[REPZ_11_138] += 1
        count = 0; prevlen = curlen
        if nextlen == 0:
            max_count, min_count = 138, 3
        elif curlen == nextlen:
            max_count, min_count = 6, 3
        else:
            max_count, min_count = 7, 4


def _send_tree(w, lens, max_code, blcodes, bllens):
    prevlen = -1; nextlen = lens[0]; count = 0
    max_count, min_count = (7, 4) if nextlen else (138, 3)
    ext = lens[:max_code + 1] + [0xFFFF]
    for n in range(max_code + 1):
        curlen = nextlen; nextlen = ext[n + 1]
        count += 1
        if count < max_count and curlen == nextlen:
            continue
        elif count < min_count:
            for _ in range(count):
                w.bits(blcodes[curlen], bllens[curlen])
        elif curlen != 0:
            if curlen != prevlen:
                w.bits(blcodes[curlen], bllens[curlen]); count -= 1
            w.bits(blcodes[REP_3_6], bllens[REP_3_6]); w.bits(count - 3, 2)
        elif count <= 10:
            w.bits(blcodes[REPZ_3_10], bllens[REPZ_3_10]); w.bits(count - 3, 3)
        else:
            w.bits(blcodes[REPZ_11_138], bllens[REPZ_11_138]); w.bits(count - 11, 7)
        count = 0; prevlen = curlen
        if nextlen == 0:
            max_count, min_count = 138, 3
        elif curlen == nextlen:
            max_count, min_count = 6, 3
        else:
            max_count, min_count = 7, 4


class BitWriter:
    def __init__(self):
        self.out = bytearray(); self.acc = 0; self.n = 0

    def bits(self, v, k):
        self.acc |= v << self.n; self.n += k
        while self.n >= 8:
            self.out.append(self.acc & 0xFF); self.acc >>= 8; self.n -= 8

    def windup(self):
        if self.n:
            self.out.append(self.acc & 0xFF)
        self.acc = 0; self.n = 0


def _flush_block(w, toks, raw, start, end, eof):
    lfreq = [0] * L_CODES; dfreq = [0] * D_CODES
    for t in toks:
        if type(t) is int:
            lfreq[t] += 1
        else:
            lfreq[LEN_CODE[t[0]] + LITERALS + 1] += 1
            dfreq[DIST_CODE[t[1]]] += 1
    lfreq[END_BLOCK] = 1
    llens, lmax, lopt, lstat = _build_tree(lfreq, STATIC_LLEN, EXTRA_LBITS, LITERALS + 1, L_CODES, MAX_BITS)
    dlens, dmax, dopt, dstat = _build_tree(dfreq, STATIC_DLEN, EXTRA_DBITS, 0, D_CODES, MAX_BITS)
    blfreq = [0] * BL_CODES
    _scan_tree(llens, lmax, blfreq)
    _scan_tree(dlens, dmax, blfreq)
    bllens, blmax, blopt, _ = _build_tree(blfreq, None, EXTRA_BLBITS, 0, BL_CODES, MAX_BL_BITS)
    opt_len = lopt + dopt + blopt
    max_blindex = BL_CODES - 1
    while max_blindex >= 3 and bllens[BL_ORDER[max_blindex]] == 0:
        max_blindex -= 1
    opt_len += 3 * (max_blindex + 1) + 5 + 5 + 4
    static_len = lstat + dstat
    opt_lenb = (opt_len + 3 + 7) >> 3
    static_lenb = (static_len + 3 + 7) >> 3
    if static_lenb <= opt_lenb:
        opt_lenb = static_lenb
    stored_len = end - start
    if stored_len + 4 <= opt_lenb:
        w.bits((0 << 1) + eof, 3)
        w.windup()
        w.out += bytes([stored_len & 0xFF, stored_len >> 8, (~stored_len) & 0xFF, ((~stored_len) >> 8) & 0xFF])
        w.out += raw[start:end]
        return
    if static_lenb == opt_lenb:
        w.bits((1 << 1) + eof, 3)
        lc, ll, dc, dl = STATIC_LCODE, STATIC_LLEN, STATIC_DCODE, STATIC_DLEN
    else:
        w.bits((2 << 1) + eof, 3)
        lmax_send = lmax + 1; dmax_send = dmax + 1
        w.bits(lmax_send - 257, 5); w.bits(dmax_send - 1, 5); w.bits(max_blindex + 1 - 4, 4)
        blcodes = _gen_codes(bllens + [0] * 20, BL_CODES - 1)
        for r in range(max_blindex + 1):
            w.bits(bllens[BL_ORDER[r]], 3)
        _send_tree(w, llens, lmax, blcodes, bllens)
        _send_tree(w, dlens, dmax, blcodes, bllens)
        lc = _gen_codes(llens + [0] * 2, lmax); ll = llens
        dc = _gen_codes(dlens + [0] * 2, dmax); dl = dlens
    bits = w.bits
    for t in toks:
        if type(t) is int:
            bits(lc[t], ll[t])
        else:
            ln, dist = t
            c = LEN_CODE[ln]
            bits(lc[c + 257], ll[c + 257])
            e = EXTRA_LBITS[c]
            if e:
                bits(ln - LBASE[c], e)
            dcd = DIST_CODE[dist]
            bits(dc[dcd], dl[dcd])
            e = EXTRA_DBITS[dcd]
            if e:
                bits(dist - DBASE[dcd], e)
    bits(lc[END_BLOCK], ll[END_BLOCK])


def split_points(toks, lit_bufsize=16384):
    """Block ends (token index, byte end) by the zlib 1.1.x _tr_tally rule."""
    ends = []
    last_lit = 0; matches = 0; dfreq = [0] * D_CODES
    pos = 0; block_start = 0
    ntok = len(toks)
    for i, t in enumerate(toks):
        if type(t) is int:
            strstart = pos + 1          # literal tallied with strstart = its position + 1
            adv = 1
        else:
            strstart = pos + 1          # match tallied with strstart = match start + 1
            matches += 1
            dfreq[DIST_CODE[t[1]]] += 1
            adv = t[0]
        last_lit += 1
        flush = False
        if (last_lit & 0xFFF) == 0:
            out_length = last_lit * 8
            in_length = strstart - block_start
            for dc in range(D_CODES):
                out_length += dfreq[dc] * (5 + EXTRA_DBITS[dc])
            out_length >>= 3
            if matches < last_lit // 2 and out_length < in_length // 2:
                flush = True
        if last_lit == lit_bufsize - 1:
            flush = True
        pos += adv
        # The final token, when a literal, is tallied by deflate_slow's end-of-input path,
        # which ignores the flush request; a final match honours it (then an empty last block).
        if flush and not (i == ntok - 1 and type(t) is int):
            ends.append((i + 1, pos))
            last_lit = 0; matches = 0; dfreq = [0] * D_CODES; block_start = pos
    ends.append((ntok, pos))
    return ends


def compress(raw):
    raw = bytes(raw)
    modern = zlib.compressobj(6, zlib.DEFLATED, 15, 8, zlib.Z_DEFAULT_STRATEGY)
    z = modern.compress(raw) + modern.flush()
    toks = tokens_of(z)
    w = BitWriter()
    s_tok = 0; s_pos = 0
    ends = split_points(toks)
    for k, (e_tok, e_pos) in enumerate(ends):
        _flush_block(w, toks[s_tok:e_tok], raw, s_pos, e_pos, 1 if k == len(ends) - 1 else 0)
        s_tok, s_pos = e_tok, e_pos
    w.windup()
    a = zlib.adler32(raw) & 0xFFFFFFFF
    return b'\x78\x9c' + bytes(w.out) + a.to_bytes(4, 'big')
