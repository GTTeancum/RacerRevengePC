# Find tables in the program (img/eprom.bin) of pointers into a mask range (0xA0000000+off).
# Usage: python -I tools/mask_ptrtabs.py <lo_off> <hi_off> [minrun]
# Prints runs of records: same stride, >= minrun pointers in range.
import sys, numpy as np
e = np.fromfile('img/eprom.bin', dtype='<u4')
lo, hi = int(sys.argv[1], 0) | 0xA0000000, int(sys.argv[2], 0) | 0xA0000000
minrun = int(sys.argv[3]) if len(sys.argv) > 3 else 4
idx = np.flatnonzero((e >= lo) & (e < hi))
used = set()
for stride in (1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16):
    s = set(idx.tolist())
    for j in idx:
        if j in used or (j - stride) in s: continue
        k = j; n = 0
        while k in s: n += 1; k += stride
        if n >= minrun:
            vals = e[j:k:stride] & 0x0FFFFFFF
            print('eprom %06X stride %2d words n=%4d  mask %08X..%08X  first: %s' % (j*4, stride, n, vals.min(), vals.max(), ' '.join('%08X' % v for v in e[j:j+min(stride*2, 12)])))
            used.update(range(j, k, stride))
