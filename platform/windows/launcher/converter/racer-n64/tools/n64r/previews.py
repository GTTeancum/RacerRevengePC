#!/usr/bin/env python3
r"""Preview images for an exported track (look at them).

preview_persp.png   textured perspective view from behind the spline start, looking along the spline
preview_top.png     top view: collision coloured by surface, spline main path (white), branches
                    (yellow), forks (red), joins (orange), start (green), triggers (cyan)
preview_camera.png  top view with the intro camera eye path (cyan) / target path (magenta) and
                    look lines every 0.5 s, plus three textured frames rendered from the intro camera
Uses the numpy rasteriser work\scratch\racer-arcade\tools\softrender.py for the perspective views.
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, (__import__('os').environ['RR_CONVERT_ROOT']+'/racer-arcade/tools'))
import softrender as SR  # noqa: E402

SURF_COLOURS = [
    ('Lava', (230, 60, 20)), ('Fall', (150, 0, 150)), ('ZOn', (0, 200, 255)), ('ZOff', (0, 110, 150)),
    ('Fast', (40, 220, 40)), ('Slow', (210, 170, 40)), ('Slip', (170, 230, 255)), ('Swmp', (90, 120, 50)),
    ('Wet', (60, 100, 230)), ('Side', (110, 110, 120)), ('Snow', (245, 245, 245)), ('Ruff', (160, 110, 80)),
    ('Dust', (205, 185, 140)), ('Soft', (220, 150, 190)), ('Mirr', (200, 235, 235)), ('Swst', (120, 180, 160)),
    ('NSnw', (210, 210, 200)), ('Flat', (180, 180, 140)), ('NRSp', (255, 120, 120))]


def surf_colour(names):
    for n, c in SURF_COLOURS:
        if n in names:
            return c
    return (165, 160, 150)


class TopView:
    def __init__(self, P, W=1400, pad=40, extra=None):
        # frame on the spline / camera points (extra) padded 15%, so stray far-away collision pieces
        # do not shrink the track; fall back to all points
        if extra is not None and len(extra):
            pts = extra[:, :2]
            lo, hi = pts.min(0), pts.max(0)
            m = (hi - lo).max() * 0.15
            lo, hi = lo - m, hi + m
            if len(P):
                inside = P[(P[:, 0] >= lo[0]) & (P[:, 0] <= hi[0]) & (P[:, 1] >= lo[1]) & (P[:, 1] <= hi[1])][:, :2]
                if len(inside):
                    lo = np.minimum(lo + m, inside.min(0))
                    hi = np.maximum(hi - m, inside.max(0))
        else:
            pts = P[:, :2] if len(P) else np.zeros((1, 2))
            lo, hi = pts.min(0), pts.max(0)
        self.lo = lo
        span = max(hi - self.lo)
        self.s = (W - 2 * pad) / max(span, 1.0)
        self.pad = pad
        self.W = W
        self.H = int((hi[1] - self.lo[1]) * self.s + 2 * pad)

    def xy(self, p):
        p = np.asarray(p, float)
        x = self.pad + (p[..., 0] - self.lo[0]) * self.s
        y = self.H - (self.pad + (p[..., 1] - self.lo[1]) * self.s)
        return np.stack([x, y], -1)


def draw_collision(img, tv, P, T, S=None, grey=False):
    d = ImageDraw.Draw(img)
    if not len(T):
        return
    A, B_, C = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    N = np.cross(B_ - A, C - A)
    nl = np.linalg.norm(N, axis=1)
    nl[nl == 0] = 1
    nz = np.abs(N[:, 2]) / nl
    zc = (A[:, 2] + B_[:, 2] + C[:, 2]) / 3
    order = np.argsort(zc)
    Q = tv.xy(np.stack([A, B_, C], 1))
    for i in order:
        base = (150, 150, 150) if grey else surf_colour(S[i] if S is not None else [])
        k = 0.45 + 0.55 * nz[i]
        col = tuple(int(c * k) for c in base)
        d.polygon([tuple(q) for q in Q[i]], fill=col)


def draw_spline(img, tv, s, g, polys, labels=True):
    d = ImageDraw.Draw(img)
    main = set(zip(g['main_path'], g['main_path'][1:]))
    if g['closed_loop'] and g['main_path']:
        main.add((g['main_path'][-1], g['main_path'][0]))
    for a, b, C in polys:
        q = [tuple(x) for x in tv.xy(C)]
        d.line(q, fill=(255, 255, 255) if (a, b) in main else (255, 220, 0), width=3 if (a, b) in main else 2)
    pts = s['points']
    for p in pts:
        x, y = tv.xy(p['position'])
        r = 3
        col = (40, 40, 40)
        if len(p['next']) > 1:
            col, r = (255, 30, 30), 7
        elif len(p['prev']) > 1:
            col, r = (255, 140, 0), 6
        d.ellipse([x - r, y - r, x + r, y + r], fill=col)
        if labels and (p['index'] % 10 == 0 or len(p['next']) > 1):
            d.text((x + 6, y - 6), '%d/p%d' % (p['index'], p['progress']), fill=(255, 255, 255))
    x, y = tv.xy(pts[0]['position'])
    d.ellipse([x - 9, y - 9, x + 9, y + 9], outline=(0, 255, 0), width=3)
    if pts[0]['next']:
        x2, y2 = tv.xy(pts[pts[0]['next'][0]]['position'])
        d.line([(x, y), (x2, y2)], fill=(0, 255, 0), width=3)


def top(out_dir, Pc, Tc, Sc, s, g, polys, trigs):
    sp = np.array([p['position'] for p in s['points']])
    tv = TopView(Pc, extra=sp)
    img = Image.new('RGB', (tv.W, tv.H), (25, 28, 36))
    draw_collision(img, tv, Pc, Tc, Sc)
    d = ImageDraw.Draw(img)
    for t in trigs:
        x, y = tv.xy(t['center'])
        r = max(4, abs(t['size_xy']) * tv.s)
        d.rectangle([x - r, y - r, x + r, y + r], outline=(0, 255, 255), width=2)
        d.text((x + r + 2, y), str(t['type']), fill=(0, 255, 255))
    draw_spline(img, tv, s, g, polys)
    # legend
    used = set(n for names in Sc for n in names)
    y = 8
    for n, c in SURF_COLOURS:
        if n in used:
            d.rectangle([8, y, 22, y + 12], fill=c)
            d.text((28, y), n, fill=(255, 255, 255))
            y += 16
    d.text((8, y + 4), 'spline %d %s: %d pts, forks %d' % (s['id'], s['name'], s['point_count'], len(g['forks'])),
           fill=(255, 255, 255))
    img.save(os.path.join(out_dir, 'preview_top.png'))


def _spline_dir(s, i):
    pts = s['points']
    p = np.array(pts[i]['position'])
    j = pts[i]['next'][0] if pts[i]['next'] else i
    q = np.array(pts[j]['position'])
    d = q - p
    d[2] = 0
    n = np.linalg.norm(d)
    return p, (d / n if n > 1e-6 else np.array([1.0, 0, 0]))


def render_view(Pv, Tv, UVv, texv, imgs, eye, target, W, H, fov=65):
    cam = dict(eye=eye, target=target, up=(0, 0, 1), fov=fov, near=2.0)
    # crude distance cull to keep the rasteriser fast
    if len(Tv):
        cen = Pv[Tv].mean(1)
        keep = np.linalg.norm(cen - np.asarray(eye), axis=1) < 30000
        T = Tv[keep]
        tex = texv[keep]
    else:
        T, tex = Tv, texv
    img, _, _ = SR.render(Pv, T, cam, W, H, uv=UVv, tex_of_tri=tex, textures=imgs, ambient=0.75,
                          bg=(120, 150, 190))
    return img


def perspective(out_dir, Pv, Tv, UVv, texv, imgs, s, Pc, Tc, W=1280, H=720):
    p, d = _spline_dir(s, 0)
    eye = p - d * 120 + np.array([0, 0, 45.0])
    target = p + d * 300 + np.array([0, 0, 10.0])
    img = render_view(Pv, Tv, UVv, texv, imgs, eye, target, W, H)
    # second view a quarter of the way round
    k = len(s['points']) // 4
    p2, d2 = _spline_dir(s, k)
    img2 = render_view(Pv, Tv, UVv, texv, imgs, p2 - d2 * 120 + np.array([0, 0, 45.0]),
                       p2 + d2 * 300 + np.array([0, 0, 10.0]), W // 2, H // 2)
    p3, d3 = _spline_dir(s, len(s['points']) // 2)
    img3 = render_view(Pv, Tv, UVv, texv, imgs, p3 - d3 * 120 + np.array([0, 0, 45.0]),
                       p3 + d3 * 300 + np.array([0, 0, 10.0]), W // 2, H // 2)
    out = Image.new('RGB', (W, H + H // 2), (0, 0, 0))
    out.paste(Image.fromarray(img), (0, 0))
    out.paste(Image.fromarray(img2), (0, H))
    out.paste(Image.fromarray(img3), (W // 2, H))
    dd = ImageDraw.Draw(out)
    dd.text((8, 8), 'spline point 0 (start)', fill=(255, 255, 0))
    dd.text((8, H + 8), 'spline point %d' % k, fill=(255, 255, 0))
    dd.text((W // 2 + 8, H + 8), 'spline point %d' % (len(s['points']) // 2), fill=(255, 255, 0))
    out.save(os.path.join(out_dir, 'preview_persp.png'))


def camera(out_dir, Pc, Tc, s, polys, cams, vis=None):
    fr = cams.get('frames') or []
    E = np.array([f['eye'] for f in fr]) if fr else np.zeros((0, 3))
    G = np.array([f['target'] for f in fr]) if fr else np.zeros((0, 3))
    extra = np.vstack([E, G]) if len(E) else None
    sp = np.array([p['position'] for p in s['points']])
    tv = TopView(Pc, W=1000, extra=np.vstack([sp, extra]) if extra is not None else sp)
    img = Image.new('RGB', (tv.W, tv.H), (25, 28, 36))
    draw_collision(img, tv, Pc, Tc, grey=True)
    d = ImageDraw.Draw(img)
    for a, b, C in polys:
        d.line([tuple(x) for x in tv.xy(C)], fill=(230, 230, 230), width=1)
    if len(E):
        d.line([tuple(x) for x in tv.xy(E)], fill=(0, 255, 255), width=3)
        d.line([tuple(x) for x in tv.xy(G)], fill=(255, 0, 255), width=3)
        fps = cams.get('frames_fps', 30)
        step = max(1, int(fps / 2))
        for i in range(0, len(fr), step):
            d.line([tuple(tv.xy(E[i])), tuple(tv.xy(G[i]))], fill=(255, 255, 0), width=1)
        x, y = tv.xy(E[0])
        d.ellipse([x - 7, y - 7, x + 7, y + 7], outline=(0, 255, 0), width=3)
    d.text((8, 8), 'intro camera: %s' % cams.get('status'), fill=(255, 255, 255))
    d.text((8, 24), (cams.get('eye_source') or '')[:110], fill=(0, 255, 255))
    if cams.get('camera_spline'):
        d.text((8, 40), 'camera spline %s %s, target %s, %.1f s' % (
            cams['camera_spline']['id'], cams['camera_spline']['name'], cams.get('target_node'),
            cams.get('duration_s', 0)), fill=(255, 0, 255))
    top_img = img
    if vis is not None and len(fr):
        Pv, Tv, UVv, texv, imgs = vis
        frames = []
        for i in (0, len(fr) // 2, len(fr) - 1):
            f = fr[i]
            im = render_view(Pv, Tv, UVv, texv, imgs, f['eye'], f['target'], 400, 225, fov=60)
            im = Image.fromarray(im)
            ImageDraw.Draw(im).text((4, 4), 't=%.2f s' % f['t'], fill=(255, 255, 0))
            frames.append(im)
        W = max(top_img.width, 1220)
        out = Image.new('RGB', (W, top_img.height + 235), (0, 0, 0))
        out.paste(top_img, (0, 0))
        for k, im in enumerate(frames):
            out.paste(im, (k * 410, top_img.height + 5))
        top_img = out
    top_img.save(os.path.join(out_dir, 'preview_camera.png'))
