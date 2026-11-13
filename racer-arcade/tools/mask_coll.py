# Top-view render of the 5 collision blocks (196-byte triangle records) at mask 0x0E000000..0x0E78A6C0.
# Record (49 words): centroid x,y,z, radius, v0 xyz, v1 xyz, v2 xyz, 3 vertex normals xyz, edge data..., ids/flags (last 5 words).
# Block list = program table at eprom 0x3D4ECC: (start, footer ptr, size). Usage: python -I tools/mask_coll.py out.png
import sys, numpy as np
from PIL import Image
d = np.memmap(__import__('os').path.join(__import__('os').path.dirname(__file__), '..', 'img', 'mask.bin'), dtype='<u4', mode='r')
BLOCKS = [(0x0E000000, 0x18EAA0), (0x0E18EAA0, 0x3154C0), (0x0E4A3F60, 0x131080), (0x0E5D4FE0, 0x4B140), (0x0E620120, 0x16A5A0)]
S = 400; tiles = []
for a, sz in BLOCKS:
    n = (sz - 0x20) // 196
    f = np.asarray(d[a//4:a//4 + n*49]).reshape(n, 49).view('<f4')
    v = f[:, 4:13].reshape(-1, 3)
    ok = np.isfinite(v).all(1) & (np.abs(v) < 1e5).all(1); v = v[ok]
    lo, hi = v[:, [0, 2]].min(0), v[:, [0, 2]].max(0); sc = (S-1) / max((hi-lo).max(), 1)
    q = ((v[:, [0, 2]] - lo) * sc).astype(int)
    y = v[:, 1]; yn = ((y - y.min()) / max(np.ptp(y), 1e-6) * 200 + 55).astype(np.uint8)
    im = np.full((S, S, 3), 255, np.uint8)
    im[q[:, 1], q[:, 0]] = np.stack([yn, 80 + 0*yn, 255 - yn], -1)
    print('%08X n=%d tris  x %.0f..%.0f  y %.0f..%.0f  z %.0f..%.0f' % (a, n, lo[0], hi[0], y.min(), y.max(), lo[1], hi[1]))
    tiles.append(im)
Image.fromarray(np.concatenate([np.pad(t, ((0, 0), (0, 6), (0, 0)), constant_values=128) for t in tiles], 1)).save(sys.argv[1])
