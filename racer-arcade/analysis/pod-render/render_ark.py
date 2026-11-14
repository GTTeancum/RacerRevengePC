r"""ARK engine renders: textured, per-mesh false colour, opposite-facing coplanar pairs, LOD2 no-cull,
cockpit poly-type-6 (translucent) meshes separately. Output export\pods\render-re\."""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools')); sys.path.insert(0, HERE)
import numpy as np
from PIL import Image, ImageDraw
import pod_export as PE, racer_assets as R, softrender as SR
import overlap as OV
OUT = os.path.join(PE.OUT, 'render-re'); os.makedirs(OUT, exist_ok=True)
PAL = [(0.9, 0.2, 0.2), (0.2, 0.8, 0.2), (0.2, 0.4, 1.0), (1.0, 0.85, 0.1), (1.0, 0.2, 1.0), (0.1, 0.9, 0.9),
       (1.0, 0.55, 0.1), (0.6, 0.3, 1.0), (0.6, 0.6, 0.6), (0.4, 0.2, 0.1), (0.9, 0.9, 0.9)]
kind = R.CODES.index('ARK'); pd = R.pod(kind); tg = R.texgroup(pd['tg_slot0'][0])

def parts_of(oid, tagname):
    return PE.object_parts(oid, np.eye(4), tg, tagname)

def views(parts, title, fname, colour_fn=None, sel=None, cull=False, dirs=None):
    P, T, UV, TT, texs = PE.scene_tris(parts)
    # per-tri metadata
    meta = [(q, mi, fl, ti) for q in parts for ti, (a, b, c, mi, fl) in enumerate(q['mesh'].tris)]
    keep = np.array([sel(*m) if sel else True for m in meta], bool)
    cols = np.array([colour_fn(*m) if colour_fn else (0.75, 0.75, 0.7) for m in meta])
    TT2 = TT.copy() if colour_fn is None else np.full(len(TT), -1)
    c = (P.max(0) + P.min(0)) / 2; r = np.abs(P - c).max() * 1.2
    dirs = dirs or [('front (-z)', (0, 0, -1)), ('rear (+z)', (0, 0, 1)), ('3/4 rear', (0.6, 0.35, 0.75)), ('side +x', (1, 0, 0))]
    tiles = []
    for name, d in dirs:
        d = np.array(d, float); d /= np.linalg.norm(d)
        dist = r * 3.2
        cam = dict(eye=c + d * dist, target=c, up=(0, 1, 0), fov=40, near=dist * 0.05)
        img, _, _ = SR.render(P, T[keep], cam, W=480, H=480, uv=UV, tex_of_tri=TT2[keep], textures=texs,
                              colors=cols[keep], cull=cull, light=(0.4, 0.8, -0.45), ambient=0.45)
        tiles.append((name, Image.fromarray(img)))
    out = Image.new('RGB', (480 * len(tiles), 504), (16, 16, 22)); dr = ImageDraw.Draw(out)
    for i, (name, t) in enumerate(tiles):
        out.paste(t, (i * 480, 24)); dr.text((i * 480 + 6, 6), '%s  %s' % (title, name), fill=(255, 220, 90))
    out.save(os.path.join(OUT, fname)); print('wrote', fname)

e0 = parts_of(pd['parts'][0]['engines'][0], 'engine0')
mm = e0[0]['mesh']
views(e0, 'ARK engine0 LOD0 m416 textured', 'ARK_engine_lod0_textured.png')
views(e0, 'ARK engine0 LOD0 textured, back faces culled', 'ARK_engine_lod0_textured_cull.png', cull=True)
views(e0, 'ARK engine0 LOD0 mesh colours', 'ARK_engine_lod0_meshes.png', colour_fn=lambda q, mi, fl, ti: PAL[mi % len(PAL)])
same, cop = OV.overlaps(mm)
bad = {i for p in cop for i in p[:2]}
views(e0, 'ARK engine0 LOD0: back-to-back coplanar tris (red)', 'ARK_engine_lod0_backtoback.png',
      colour_fn=lambda q, mi, fl, ti: (1, 0.1, 0.1) if ti in bad else (0.55, 0.55, 0.6))
views(e0, 'ARK engine0 LOD0 mesh 4 only (tex 206)', 'ARK_engine_lod0_mesh4.png', sel=lambda q, mi, fl, ti: mi == 4)
e2 = parts_of(pd['parts'][2]['engines'][0], 'engine0')
views(e2, 'ARK engine0 LOD2 m417: no-cull tris (yellow)', 'ARK_engine_lod2_nocull.png',
      colour_fn=lambda q, mi, fl, ti: (1, 0.85, 0.1) if fl else (0.55, 0.55, 0.6))
ck = parts_of(pd['parts'][0]['objA'], 'cockpit')
pt = lambda q, mi, fl, ti: q['mesh'].meshes[mi]['polytype'] == 6
views(ck, 'ARK cockpit LOD0: poly type 6 (translucent, blue)', 'ARK_cockpit_lod0_translucent.png',
      colour_fn=lambda q, mi, fl, ti: (0.2, 0.5, 1.0) if pt(q, mi, fl, ti) else (0.55, 0.55, 0.6),
      dirs=[('front', (0, 0.3, -1)), ('3/4', (0.7, 0.45, -0.75)), ('side', (1, 0.1, 0)), ('top', (0, 1, 0.01))])
views(ck, 'ARK cockpit LOD0: translucent meshes only, textured', 'ARK_cockpit_lod0_translucent_only.png', sel=pt,
      dirs=[('front', (0, 0.3, -1)), ('3/4', (0.7, 0.45, -0.75)), ('side', (1, 0.1, 0)), ('top', (0, 1, 0.01))])
