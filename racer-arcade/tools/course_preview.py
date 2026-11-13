#!/usr/bin/env python3
"""Preview images for export_course.py (called from it): top view textured, three chase-height
views along the track, collision-vs-visual overlays. y is up in course space."""
import os

import numpy as np
from PIL import Image, ImageDraw

import softrender as SR

SURF_COL = {}


def surf_colour(s):
    if s not in SURF_COL:
        rng = np.random.default_rng(s * 7919 + 13)
        SURF_COL[s] = tuple(int(x) for x in rng.integers(60, 255, 3))
    return SURF_COL[s]


def _scene_arrays(scene, texcache, sky=False):
    idx = [i for i, g in enumerate(scene.groups) if (g[0] == 'SKY') == sky]
    T = np.array(scene.T)[idx] if idx else np.zeros((0, 3), int)
    tids = sorted({scene.groups[i][2] for i in idx if scene.groups[i][2] >= 0})
    tmap = {t: k for k, t in enumerate(tids)}
    textures = [texcache[t] for t in tids]
    tex_of_tri = np.array([tmap.get(scene.groups[i][2], -1) for i in idx], int)
    return T, tex_of_tri, textures


def previews(out, scene, texcache, V, R, stat_items, lens_items, path=None):
    pdir = os.path.join(out, 'preview')
    os.makedirs(pdir, exist_ok=True)
    P = np.array(scene.P)
    UV = np.array(scene.UV)
    T, tex_of_tri, textures = _scene_arrays(scene, texcache)
    used = np.unique(T)
    lo, hi = P[used].min(0), P[used].max(0)
    centre = (lo + hi) / 2
    size = max(hi[0] - lo[0], hi[2] - lo[2]) * 1.04
    W = H = 1400
    top = dict(ortho=True, center=centre, size=size, dir=(0, -1, 0), up=(0, 0, -1))
    img, _, _ = SR.render(P, T, top, W, H, uv=UV, tex_of_tri=tex_of_tri, textures=textures, ambient=0.6)
    SR.save(img, os.path.join(pdir, 'top_textured.png'))
    flat, _, _ = SR.render(P, T, top, W, H, ambient=0.25)
    SR.save(flat, os.path.join(pdir, 'top_flat.png'))

    # collision overlay on the flat top view
    S, _ = SR.project(V.reshape(-1, 3), top, W, H)
    S = S.reshape(-1, 3, 3)
    ov = Image.fromarray(flat).convert('RGBA')
    lay = Image.new('RGBA', ov.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(lay)
    surf = (R[:, 48] & 0xFFFF).astype(int)
    for t in range(len(S)):
        c = surf_colour(int(surf[t]))
        d.polygon([(S[t, k, 0], S[t, k, 1]) for k in range(3)], fill=c + (90,), outline=c + (200,))
    ov = Image.alpha_composite(ov, lay)
    dd = ImageDraw.Draw(ov)
    y = 8
    for s in sorted(set(surf.tolist())):
        dd.rectangle((8, y, 28, y + 14), fill=surf_colour(s))
        dd.text((34, y), 'surface %X: %d tris' % (s, int((surf == s).sum())), fill=(255, 255, 255))
        y += 18
    if path is not None:
        Sp, _ = SR.project(np.nan_to_num(path), top, W, H)
        pts = [tuple(q[:2]) for q in Sp]
        dd.line(pts + pts[:1], fill=(255, 255, 0), width=2)
        dd.ellipse((pts[0][0] - 6, pts[0][1] - 6, pts[0][0] + 6, pts[0][1] + 6), outline=(255, 0, 0), width=3)
        dd.text((pts[0][0] + 8, pts[0][1] - 6), 'path[0]', fill=(255, 255, 0))
    ov.convert('RGB').save(os.path.join(pdir, 'overlay_collision_top.png'))

    # height check: collision vertex y minus visual surface y below/above it (top-down z-buffer)
    _, zb_top, _ = SR.render(P, T, dict(ortho=True, center=centre, size=size, dir=(0, -1, 0), up=(0, 0, -1)), 700, 700)
    # zbuf in ortho = distance from a camera 1e5 above centre along -y
    Sv, _ = SR.project(V.reshape(-1, 3), dict(ortho=True, center=centre, size=size, dir=(0, -1, 0), up=(0, 0, -1)), 700, 700)
    xi = np.clip(Sv[:, 0].astype(int), 0, 699)
    yi = np.clip(Sv[:, 1].astype(int), 0, 699)
    vis_top_y = centre[1] + 1e5 - zb_top[yi, xi]
    dy = V.reshape(-1, 3)[:, 1] - vis_top_y
    good = np.isfinite(dy)
    stats = dict(n=int(good.sum()), median=float(np.median(dy[good])) if good.any() else None,
                 within_2=float((np.abs(dy[good]) < 2).mean()) if good.any() else None,
                 within_10=float((np.abs(dy[good]) < 10).mean()) if good.any() else None)
    with open(os.path.join(pdir, 'collision_vs_visual_height.txt'), 'w') as f:
        f.write('collision vertex y minus topmost visual surface y at the same x,z (700px grid)\n%s\n' % stats)

    # chase views along the track: points on the most common driving surface (not 0x8000)
    cand = [s for s in np.unique(surf) if s != 0x8000]
    counts = {s: int((surf == s).sum()) for s in cand}
    main = max(counts, key=counts.get)
    cen = V.mean(1)
    sel = np.flatnonzero(surf == main)
    rng = np.random.default_rng(1)
    picks = []
    # spread picks: farthest-point sampling on the main surface centroids
    first = sel[rng.integers(len(sel))]
    picks.append(first)
    for _ in range(3):
        dmin = np.min([np.linalg.norm(cen[sel] - cen[p], axis=1) for p in picks], axis=0)
        picks.append(sel[int(np.argmax(dmin))])
    sky_T, sky_tex, sky_textures = _scene_arrays(scene, texcache, sky=True)
    cams = []
    if path is not None and np.isfinite(path[:, 1]).sum() > 4:
        n = len(path)
        for k in range(4):
            i = (k * n) // 4
            j = (i + 4) % n
            y0 = path[i, 1] if np.isfinite(path[i, 1]) else np.nanmedian(path[:, 1])
            c0 = np.array([path[i, 0], y0, path[i, 2]])
            dirv = np.array([path[j, 0] - path[i, 0], 0.0, path[j, 2] - path[i, 2]])
            dirv /= max(np.linalg.norm(dirv), 1e-6)
            cams.append((c0, dirv))
    else:
        for p in picks:
            c0 = cen[p]
            near = sel[np.linalg.norm(cen[sel] - c0, axis=1) < 150]
            pts = cen[near][:, [0, 2]] - c0[[0, 2]]
            dxz = np.linalg.svd(pts - pts.mean(0))[2][0] if len(pts) > 3 else np.array([1.0, 0.0])
            cams.append((c0, np.array([dxz[0], 0.0, dxz[1]])))
    for k, (c0, dirv) in enumerate(cams):
        eye = c0 - dirv * 30 + np.array([0, 9.0, 0])
        cam = dict(eye=eye, target=c0 + dirv * 120 + np.array([0, 4.0, 0]), up=(0, 1, 0), fov=70, near=1.0)
        im, zb, imf = SR.render(P, sky_T, cam, 960, 540, uv=UV, tex_of_tri=sky_tex, textures=sky_textures,
                                ambient=1.0) if len(sky_T) else (None, None, None)
        if imf is not None:
            zb[:] = np.inf  # sky behind everything
        im, zb, imf = SR.render(P, T, cam, 960, 540, uv=UV, tex_of_tri=tex_of_tri, textures=textures,
                                ambient=0.55, img=imf, zbuf=zb)
        SR.save(im, os.path.join(pdir, 'chase_%d.png' % k))
        # same view: visual flat grey + collision wireframe on top
        flat, zbf, _ = SR.render(P, T, cam, 960, 540, ambient=0.3)
        Sc, ok = SR.project(V.reshape(-1, 3), cam, 960, 540)
        Sc = Sc.reshape(-1, 3, 3)
        okc = ok.reshape(-1, 3).all(1)
        o = Image.fromarray(flat)
        dr = ImageDraw.Draw(o)
        for t in np.flatnonzero(okc):
            if np.linalg.norm(cen[t] - eye) > 600:
                continue
            c = surf_colour(int(surf[t]))
            dr.line([tuple(Sc[t, i, :2]) for i in (0, 1, 2, 0)], fill=c, width=1)
        o.save(os.path.join(pdir, 'chase_%d_collision.png' % k))
    return stats
