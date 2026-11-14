"""Alpha of the texels actually covered by each poly-type-6 mesh (UV triangles rasterised in texel space)."""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools'))
import numpy as np, pod_export as PE, racer_assets as R
from collections import Counter
from PIL import Image, ImageDraw
def covered(img, info, uvs):
    h, w = img.shape[:2]; k = 16.0 if info['uv_texels'] else 1.0
    mask = Image.new('L', (w * 4, h * 4), 0); d = ImageDraw.Draw(mask)
    for tri in uvs:
        pts = [((u * k) % w * 4, (v * k) % h * 4) for u, v in tri]
        d.polygon(pts, fill=255)
    m = np.array(mask.resize((w, h), Image.BOX)) > 64
    return img[..., 3][m]
for code, mids, lod in [('ARK', [420, 421], 0), ('ANA', [74], 0), ('ANA', [164, 160], 1)]:
    kind = R.CODES.index(code); pd = R.pod(kind)
    tg = R.texgroup(pd['tg_slot0'][0] if lod == 0 else pd['tg_other'][0])
    for mid in mids:
        mm = PE.model_mesh(mid); UV = np.array(mm.uvs)
        for mi, me in enumerate(mm.meshes):
            if me['polytype'] != 6: continue
            tris = [t for t in mm.tris if t[3] == mi]
            if not tris: continue
            tid = tg[me['tex']][0]; img, info = PE.texture_rgba(tid)
            a = covered(img, info, [UV[list(t[:3])] for t in tris])
            c = Counter((a // 17 * 17).tolist())
            print(code, 'm%d me%d tris %d tex %s %dx%d fmt%d' % (mid, mi, len(tris), info['id'], info['w'], info['h'], info['fmt']),
                  'alpha of covered texels:', dict(sorted(c.items())))
