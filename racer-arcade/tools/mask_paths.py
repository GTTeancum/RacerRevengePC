# Plot the 12 recorded pod-pose runs at mask 0x0E78A6C0 (86402 x 128-byte records, 7200 per run)
# as x/z top views. Record: w0 hdr (low 16 = run id?), w1=0xFFE, then 5 slots of [3 packed words, x, y, z];
# slots 0..2 are a triangle (nose, rear-left, rear-right). Usage: python -I tools/mask_paths.py out.png
import sys, numpy as np
from PIL import Image, ImageDraw
d = np.memmap(__import__('os').path.join(__import__('os').path.dirname(__file__), '..', 'img', 'mask.bin'), dtype='<u4', mode='r')
w = np.asarray(d[0x0E78A6C0//4:0x0E78A6C0//4 + 86400*32]).reshape(-1, 32); f = w.view('<f4')
S = 300; tiles = []
for r in range(12):
    p = f[r*7200:(r+1)*7200][:, [5, 7]]
    lo, hi = p.min(0), p.max(0); sc = (S-20) / max((hi-lo).max(), 1)
    im = Image.new('RGB', (S, S), 'white'); dr = ImageDraw.Draw(im)
    q = (p - lo) * sc + 10
    dr.line([tuple(x) for x in q[::4]], fill=(0, 0, 160), width=1)
    dr.text((4, 4), 'run %d hdr %08X  span %.0f x %.0f' % (r, w[r*7200+1, 0], *(hi-lo)), fill='red')
    tiles.append(im)
out = Image.new('RGB', (S*4, S*3), 'gray')
for i, t in enumerate(tiles): out.paste(t, ((i % 4)*S, (i//4)*S))
out.save(sys.argv[1])
