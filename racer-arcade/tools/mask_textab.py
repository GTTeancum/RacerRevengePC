# Extract texture records from the program (img/eprom.bin). Record = 6 LE words:
#   ptr (0xA0000000 + mask offset), wlog, hlog, byte size, hdr0 (0x000100C1), hdr1 (0xXXXXXXC1)
#   width = 16<<wlog, height = 16<<hlog (inferred: size == 2*w*h*4/3 for a full mip chain)
# Usage: python -I tools/mask_textab.py img/eprom.bin notes/mask-textures.tsv
import sys, numpy as np, collections
e = np.fromfile(sys.argv[1], dtype='<u4')
i = np.arange(e.size - 6)
m = ((e[i] >> 28) == 0xA) & ((e[i+4] & 0xFF) == 0xC1) & ((e[i+5] & 0xFF) == 0xC1) & (e[i+1] < 8) & (e[i+2] < 8)
idx = np.flatnonzero(m)
rows = []
for j in idx:
    p, wl, hl, sz, h0, h1 = (int(x) for x in e[j:j+6])
    rows.append((j*4, p & 0x0FFFFFFF, wl, hl, sz, h0, h1))
with open(sys.argv[2], 'w') as f:
    f.write("eprom_off\tmask_off\tw\th\tsize\thdr0\thdr1\n")
    for r in rows:
        f.write("%06X\t%08X\t%d\t%d\t%X\t%08X\t%08X\n" % (r[0], r[1], 16 << r[2], 16 << r[3], r[4], r[5], r[6]))
print(len(rows), 'records')
print('hdr0', collections.Counter('%08X' % r[5] for r in rows).most_common(8))
print('hdr1&FFFF00FF', collections.Counter('%08X' % (r[6] & 0xFFFF00FF) for r in rows).most_common(12))
print('hdr1 low16', collections.Counter('%04X' % (r[6] & 0xFFFF) for r in rows).most_common(12))
