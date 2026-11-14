r"""ARK ring tunnel close-up: arcade (one-sided, back faces culled) vs a renderer that draws every
triangle two-sided (the port's GL path). Output export\pods\render-re\ARK_ring_*.png"""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools')); sys.path.insert(0, HERE)
import numpy as np
from PIL import Image, ImageDraw
import pod_export as PE, racer_assets as R, softrender as SR, b2b_all as B
OUT = os.path.join(PE.OUT, 'render-re')
kind = R.CODES.index('ARK'); pd = R.pod(kind); tg = R.texgroup(pd['tg_slot0'][0])
parts = PE.object_parts(pd['parts'][0]['engines'][0], np.eye(4), tg, 'engine0')
mm = parts[0]['mesh']
pr, b2b, M = B.pairs(mm)
P, T, UV, TT, texs = PE.scene_tris(parts)
Pm = np.array(mm.positions)
# which member of each pair faces the ring axis (the tunnel)
c = np.array([0.035, -0.116])
face_in = set(); face_out = set()
for t in b2b:
    a, b, cc = mm.tris[t][:3]
    n = np.cross(Pm[b] - Pm[a], Pm[cc] - Pm[a]); cen = (Pm[a] + Pm[b] + Pm[cc]) / 3
    (face_in if np.dot(n[:2], cen[:2] - c) < 0 else face_out).add(t)
print('back-to-back tris', len(b2b), 'facing tunnel', len(face_in), 'facing ring body', len(face_out))
cams = [('3/4 rear into tunnel', dict(eye=(0.9, 0.5, 4.0), target=(0.0, -0.1, 0.0), up=(0, 1, 0), fov=35, near=0.05)),
        ('front into tunnel', dict(eye=(-0.6, 0.3, -3.5), target=(0.0, -0.1, 0.3), up=(0, 1, 0), fov=35, near=0.05))]
def shot(cam, cull, mark=None):
    cols = np.array([(1, 0.1, 0.1) if (mark and i in mark) else (0.6, 0.6, 0.65) for i in range(len(T))])
    tt = TT.copy()
    if mark: tt[list(mark)] = -1
    img, _, _ = SR.render(P, T, cam, W=560, H=420, uv=UV, tex_of_tri=tt, textures=texs, colors=cols, cull=cull,
                          light=(0.4, 0.8, -0.45), ambient=0.45)
    return Image.fromarray(img)
rows = [('arcade: one-sided (back faces culled)', True, None), ('two-sided, as the port draws pods', False, None),
        ('two-sided, ring-body-facing copy in red', False, face_out)]
out = Image.new('RGB', (560 * len(cams), (420 + 22) * len(rows)), (16, 16, 22)); d = ImageDraw.Draw(out)
for r, (title, cull, mark) in enumerate(rows):
    for k, (cn, cam) in enumerate(cams):
        out.paste(shot(cam, cull, mark), (k * 560, r * 442 + 22))
        d.text((k * 560 + 6, r * 442 + 5), 'ARK engine LOD0, %s - %s' % (title, cn), fill=(255, 220, 90))
out.save(os.path.join(OUT, 'ARK_ring_arcade_vs_twosided.png')); print('wrote')
