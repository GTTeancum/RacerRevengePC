# Untextured 3/4 thumbnails of a range of objects (to identify unreferenced pod pieces).
# python -I tools/pod_objsheet.py out.png first last [texgroup]
import os, sys
import numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pod_export as PE, racer_assets as R, softrender as SR
out, a, b = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
tg = R.texgroup(int(sys.argv[4])) if len(sys.argv) > 4 else []
T = 160; ids = list(range(a, b + 1)); cols = 10
sheet = Image.new('RGB', (cols * T, ((len(ids) + cols - 1) // cols) * T), (20, 20, 20)); d = ImageDraw.Draw(sheet)
for k, oid in enumerate(ids):
    parts = PE.object_parts(oid, np.eye(4), tg, 'o')
    x, y = (k % cols) * T, (k // cols) * T
    if parts:
        P, Tr, UV, TT, texs = PE.scene_tris(parts)
        c = (P.max(0) + P.min(0)) / 2; r = np.abs(P - c).max() * 1.1 + 1e-6
        dv = np.array([0.7, 0.45, -0.75]); dv /= np.linalg.norm(dv)
        img, _, _ = SR.render(P, Tr, dict(ortho=True, center=c, size=2 * r, dir=-dv, up=(0, 1, 0)), W=T, H=T,
                              uv=UV if tg else None, tex_of_tri=TT if tg else None, textures=texs if tg else None)
        sheet.paste(Image.fromarray(img), (x, y))
        nm = len(parts); ext = (P.max(0) - P.min(0))
        d.text((x + 2, y + 2), '%d n%d %.1fx%.1fx%.1f' % (oid, nm, *ext), fill=(255, 255, 0))
    else:
        d.text((x + 2, y + 2), '%d -' % oid, fill=(128, 128, 128))
sheet.save(out)
