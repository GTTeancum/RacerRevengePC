# Point-cloud preview of Hikaru GPU vertex commands in mask model data.
# Static vertex push (opcode low 9 bits 0x12C-0x12F, see tools/hikaru_gpu.py from the hardware notes):
# 3 words follow; position = int32 >> 10 (scaled by the 901 precision, ignored here since only shape matters),
# low 10 bits of each word = normal component.
# Usage: python -I tools/mask_gpu.py <off> <size> out.png
import sys, os, numpy as np
from PIL import Image
d = np.memmap(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'img', 'mask.bin'), dtype='<u4', mode='r')
off, size, out = int(sys.argv[1], 0), int(sys.argv[2], 0), sys.argv[3]
w = np.asarray(d[off//4:(off+size)//4])
# walk instructions (size = 4 << bits 5:4 of the first word), tracking the 901 precision
pts = []; k = 0; prec = 1.0
while k < w.size:
    x = int(w[k]); n = 1 << ((x >> 4) & 3)
    if (x & 0xFFF) == 0x901: prec = 2.0 ** (((x >> 16) & 0xFF) - 0x8D)
    if (x & 0x1FF) in (0x12C, 0x12D, 0x12E, 0x12F) and k + 3 < w.size:
        pts.append([(int(np.int32(w[k+j])) >> 10) * prec / 64 for j in (1, 2, 3)])
    k += n
p = np.array(pts)
print(len(p), 'vertices; bbox', p.min(0).round(2), p.max(0).round(2))
S = 400; views = []
for a, b in ((0, 1), (0, 2), (2, 1)):  # front x/y, top x/z, side z/y
    q = p[:, [a, b]]; lo = q.min(0); sc = (S - 10) / max(np.ptp(q, 0).max(), 1e-9)
    q = ((q - lo) * sc + 5).astype(int); im = np.full((S, S, 3), 255, np.uint8)
    for dy in (0, 1):
        for dx in (0, 1):
            im[np.clip(S - 1 - q[:, 1] - dy, 0, S-1), np.clip(q[:, 0] + dx, 0, S-1)] = (0, 0, 0)
    views.append(im); views.append(np.full((S, 4, 3), 128, np.uint8))
Image.fromarray(np.concatenate(views, 1)).save(out)
