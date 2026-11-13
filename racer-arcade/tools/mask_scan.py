# Coarse per-block statistics of img/mask.bin. Usage: python -I tools/mask_scan.py img/mask.bin out.tsv [blocksize]
import sys, numpy as np
path, out = sys.argv[1], sys.argv[2]
bs = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x10000
d = np.memmap(path, dtype=np.uint8, mode='r')
with open(out, 'w') as f:
    f.write("off\tzero\tff\tH8\tfloat\tw16top\tuniq16\tmode\n")
    for o in range(0, d.size, bs):
        b = np.asarray(d[o:o+bs])
        c = np.bincount(b, minlength=256).astype(float); p = c[c > 0] / b.size
        H = -(p*np.log2(p)).sum()
        w = b.view('<u4'); e = (w >> 23) & 0xff
        fl = np.mean(((e >= 0x70) & (e <= 0x8c)) | (w == 0) | (w == 0x80000000))  # plausible float32 magnitude 2^-15..2^13
        h = b.view('<u2')
        top = np.mean(h >> 15)
        u = np.unique(h).size
        mode = int(np.bincount(w & 0xffffffff if False else b).argmax())
        f.write(f"{o:08X}\t{c[0]/b.size:.3f}\t{c[255]/b.size:.3f}\t{H:.2f}\t{fl:.2f}\t{top:.2f}\t{u}\t{mode:02X}\n")
