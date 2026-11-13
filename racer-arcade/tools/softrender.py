#!/usr/bin/env python3
"""Small numpy z-buffer rasteriser for preview images (flat Lambert, optional textures).

render(P, T, camera, W, H, uv=None, tex_of_tri=None, textures=None, colors=None, cull=False)
  P: (N,3) float positions, T: (M,3) int triangles
  camera: dict(eye=, target=, up=, fov=deg) perspective, or dict(ortho=True, center=, size=, dir=, up=)
  uv: (N,2) normalised texture coordinates (v down = image row), tex_of_tri: (M,) int index into
  textures (list of HxWx4 uint8 arrays) or -1; colors: (M,3) per-triangle base colour (0..1)
Returns an HxWx3 uint8 image. Triangles are drawn in a Python loop over triangles with a numpy
bounding-box rasteriser: fine for ~200k triangles at 1280x720 in a minute or two.
"""
import math

import numpy as np


def look_at(eye, target, up):
    eye = np.asarray(eye, float)
    f = np.asarray(target, float) - eye
    f /= np.linalg.norm(f)
    r = np.cross(f, up)
    if np.linalg.norm(r) < 1e-9:
        r = np.cross(f, [1, 0, 0])
    r /= np.linalg.norm(r)
    u = np.cross(r, f)
    return eye, r, u, f


def project(P, camera, W, H):
    if camera.get('ortho'):
        c = np.asarray(camera['center'], float)
        eye, r, u, f = look_at(c - np.asarray(camera['dir'], float) * 1e5, c, camera['up'])
        d = P - c
        x = d @ r
        y = d @ u
        z = (P - eye) @ f
        s = min(W, H) / camera['size']
        return np.stack([W / 2 + x * s, H / 2 - y * s, z], 1), np.ones(len(P), bool)
    eye, r, u, f = look_at(camera['eye'], camera['target'], camera['up'])
    d = P - eye
    x, y, z = d @ r, d @ u, d @ f
    near = camera.get('near', 0.5)
    ok = z > near
    zz = np.where(ok, z, near)
    foc = (W / 2) / math.tan(math.radians(camera.get('fov', 60)) / 2)
    return np.stack([W / 2 + x / zz * foc, H / 2 - y / zz * foc, z], 1), ok


def clip_near(P, T, camera, uv, tex_of_tri, colors):
    """Clips triangles that cross the near plane (Sutherland-Hodgman against z = near)."""
    eye, r, u, f = look_at(camera['eye'], camera['target'], camera['up'])
    near = camera.get('near', 0.5) * 1.01
    dz = (P - eye) @ f - near
    front = dz > 0
    nf = front[T].sum(1)
    mixed = np.flatnonzero((nf > 0) & (nf < 3))
    if not len(mixed):
        return P, T, uv, tex_of_tri, colors
    newP, newUV, newT, src = [], [], [], []
    base = len(P)
    have_uv = uv is not None
    for i in mixed:
        poly = []
        idx = T[i]
        for k in range(3):
            a, b = idx[k], idx[(k + 1) % 3]
            da, db = dz[a], dz[b]
            if da > 0:
                poly.append((P[a], uv[a] if have_uv else None))
            if (da > 0) != (db > 0):
                t = da / (da - db)
                poly.append((P[a] + (P[b] - P[a]) * t,
                             (uv[a] + (uv[b] - uv[a]) * t) if have_uv else None))
        ids = []
        for p, q in poly:
            ids.append(base + len(newP))
            newP.append(p)
            newUV.append(q if have_uv else (0.0, 0.0))
        for k in range(1, len(ids) - 1):
            newT.append((ids[0], ids[k], ids[k + 1]))
            src.append(i)
    keep = np.ones(len(T), bool)
    keep[mixed] = False
    src = np.array(src, int)
    P2 = np.vstack([P, np.array(newP)]) if newP else P
    T2 = np.vstack([T[keep], np.array(newT, int).reshape(-1, 3)])
    uv2 = np.vstack([uv, np.array(newUV)]) if have_uv and newP else uv
    order = np.r_[np.flatnonzero(keep), src]
    tex2 = np.asarray(tex_of_tri)[order] if tex_of_tri is not None else None
    col2 = np.asarray(colors)[order] if colors is not None else None
    return P2, T2, uv2, tex2, col2


def render(P, T, camera, W=1280, H=720, uv=None, tex_of_tri=None, textures=None, colors=None,
           light=(0.3, 0.8, 0.4), bg=(40, 44, 60), cull=False, ambient=0.35, img=None, zbuf=None):
    P = np.asarray(P, float)
    T = np.asarray(T, int)
    if not camera.get('ortho') and len(T):
        P, T, uv, tex_of_tri, colors = clip_near(P, T, camera, uv, tex_of_tri, colors)
    S, ok = project(P, camera, W, H)
    if img is None:
        img = np.empty((H, W, 3), float)
        img[:] = np.asarray(bg, float) / 255.0
    if zbuf is None:
        zbuf = np.full((H, W), np.inf)
    L = np.asarray(light, float)
    L /= np.linalg.norm(L)
    A, B, C = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    N = np.cross(B - A, C - A)
    nn = np.linalg.norm(N, axis=1)
    nn[nn == 0] = 1
    shade = ambient + (1 - ambient) * np.abs((N / nn[:, None]) @ L)
    for i in range(len(T)):
        a, b, c = T[i]
        if not (ok[a] and ok[b] and ok[c]):
            continue
        x0, y0, z0 = S[a]
        x1, y1, z1 = S[b]
        x2, y2, z2 = S[c]
        area = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0)
        if area == 0 or (cull and area > 0):
            continue
        xmin = max(int(math.floor(min(x0, x1, x2))), 0)
        xmax = min(int(math.ceil(max(x0, x1, x2))), W - 1)
        ymin = max(int(math.floor(min(y0, y1, y2))), 0)
        ymax = min(int(math.ceil(max(y0, y1, y2))), H - 1)
        if xmin > xmax or ymin > ymax:
            continue
        if (xmax - xmin + 1) * (ymax - ymin + 1) > 4_000_000:
            continue
        xs = np.arange(xmin, xmax + 1) + 0.5
        ys = np.arange(ymin, ymax + 1) + 0.5
        X, Y = np.meshgrid(xs, ys)
        w0 = ((x1 - X) * (y2 - Y) - (x2 - X) * (y1 - Y)) / area
        w1 = ((x2 - X) * (y0 - Y) - (x0 - X) * (y2 - Y)) / area
        w2 = 1 - w0 - w1
        inside = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
        if not inside.any():
            continue
        # perspective-correct interpolation of depth and uv
        iz = w0 / z0 + w1 / z1 + w2 / z2 if not camera.get('ortho') else None
        if iz is None:
            z = w0 * z0 + w1 * z1 + w2 * z2
        else:
            z = 1 / iz
        sub = zbuf[ymin:ymax + 1, xmin:xmax + 1]
        m = inside & (z < sub)
        if not m.any():
            continue
        base = np.array(colors[i] if colors is not None else (0.75, 0.75, 0.7))
        t = tex_of_tri[i] if tex_of_tri is not None else -1
        if t is not None and t >= 0 and uv is not None and textures is not None and textures[t] is not None:
            tex = textures[t]
            th, tw = tex.shape[:2]
            if iz is None:
                u = w0 * uv[a, 0] + w1 * uv[b, 0] + w2 * uv[c, 0]
                v = w0 * uv[a, 1] + w1 * uv[b, 1] + w2 * uv[c, 1]
            else:
                u = (w0 * uv[a, 0] / z0 + w1 * uv[b, 0] / z1 + w2 * uv[c, 0] / z2) * z
                v = (w0 * uv[a, 1] / z0 + w1 * uv[b, 1] / z1 + w2 * uv[c, 1] / z2) * z
            ui = (np.floor(u[m] * tw).astype(np.int64)) % tw
            vi = (np.floor(v[m] * th).astype(np.int64)) % th
            col = tex[vi, ui, :3] / 255.0
            if tex.shape[2] == 4:
                keep = tex[vi, ui, 3] >= 128  # alpha test for the preview (cut-out trees etc.)
                if not keep.all():
                    mm = np.zeros_like(m)
                    mm[m] = keep
                    m = mm
                    col = col[keep]
        else:
            col = np.broadcast_to(base, (int(m.sum()), 3))
        sub[m] = z[m]
        img[ymin:ymax + 1, xmin:xmax + 1][m] = col * shade[i]
    return (np.clip(img, 0, 1) * 255).astype(np.uint8), zbuf, img


def save(img, path):
    from PIL import Image
    Image.fromarray(img).save(path)
