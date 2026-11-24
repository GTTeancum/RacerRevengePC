r"""Single Event course background of an ADD-ON course: UI\TEXTURES\BKG_<CODE>.PST for dlc.json "course_background".

NOT ORIGINAL BEHAVIOUR: add-on courses are a PC enhancement of the port; the image is GENERATED, not original art.

Stock (DLC DESIGN.md correction 2026-11-16; the Course screen shows the course's planet only as this full-screen
picture behind the menu plus the rotating blue hologram card):
  UI\UICOURSE\UICOURSE.RES  UI\Textures\{tat,gam,ryl,sul,mon}_bkg.pst, one per planet, picked by planet through the
  jump table 1DDF00 -> 3AF300 (port: src\assets\original_ui_course_background50kg.cpp, planets 0 1 2 4 5).
  Each is 287,772 bytes: PST revision 1, kind 1, ONE level 640x448, 256-colour palette (alpha 255), CSM1 index
  bit swap; the port decodes exactly that layout (decodeOriginalUiCourseBackground50KG).
  Look of all five: a photo-like wide view of the planet's landmarks, soft horizontal motion blur, muted
  desaturated colour, a strong vignette, and one common reticle overlay baked into the pixels (thin light lines,
  darker chevron strips). tat and gam are the same photo mirrored and tinted.

This module (one rule for every add-on course; the caller's course table only gives the camera as data):
  background_overlay(UICOURSE.RES)  MEASURES the overlay (line alpha + colour, chevron darkening, vignette) on the
                                    player's five stock backgrounds at build time; no stock pixels are copied.
  background_scene / background_render  establishing view (camera off the track, high or distant, horizon about
                                    central; user direction 2026-11-16: the stock pictures are epic landmark views, not
                                    on-course shots) of the archive's own decoded geometry (textures x vertex colours,
                                    128 = 1.0), sky cube (port drawing rule, trk_sky.lookup) and softened scb fog.
  course_background(...)            view -> motion blur -> grading -> centre haze -> vignette -> overlay -> grain ->
                                    640x448 kind 1 one level (geo_pst quantiser).
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import trk_res3  # noqa: E402

BS = chr(92)
BKG_W, BKG_H = 640, 448
BKG_BYTES = 287772
BKG_STOCK = ('tat', 'gam', 'ryl', 'sul', 'mon')
BKG_BLUR = (14.0, 8.0, 1.0)        # horizontal motion blur px at 640 wide: centre, extra at the edges, growth exponent [G]
BKG_FOV = 62.0                     # establishing-view field of view, degrees horizontal [G]
BKG_FOG = (3.0, 0.55)              # fog distances x3 and at most 55 % fog for distant establishing views [G]
BKG_SAT = 0.78                     # saturation kept (stock backgrounds are muted) [G, judged on the comparison sheet]
BKG_LEVELS = (26.0, 192.0)         # luminance p1 / p99 after grading, before the vignette [G]
BKG_MID = 0.50                     # median luminance as a fraction of BKG_LEVELS (stock means 76-89 after the vignette) [G]
BKG_GLOW = 16.0                   # centre haze (stock: bright light at the vanishing point) [G]
BKG_CHROMA_K = 0.75                # colour kept before the stock cap; the five then average about the stock five [G, measured]
BKG_GRAIN = 2.5                    # film grain sigma [G]


def stock_backgrounds(uicourse_res_bytes):
    """-> {planet: (448, 640, 3) uint8} decoded stock backgrounds (layout asserted like the port's decoder)."""
    import geo_pst
    r = trk_res3.read_res3(uicourse_res_bytes)
    files = {n.lower(): d for n, d in r.items()}
    out = {}
    for p in BKG_STOCK:
        b = files['ui' + BS + 'textures' + BS + p + '_bkg.pst']
        t = geo_pst.parse(b)
        assert len(b) == BKG_BYTES and (t['kind'], len(t['levels']), t['levels'][0]['w'], t['levels'][0]['h']) == (1, 1, BKG_W, BKG_H)
        out[p] = geo_pst.rgba_levels(t)[0][..., :3]
    return out


def background_format(pst):
    """(bytes, kind, levels, w, h) of a PST; a course background must equal the stock (287772, 1, 1, 640, 448)."""
    import geo_pst
    t = geo_pst.parse(pst)
    return (len(pst), t['kind'], len(t['levels']), t['levels'][0]['w'], t['levels'][0]['h'])


def port_layout_ok(b):
    """Mirror of decodeOriginalUiCourseBackground50KG's layout test (src\\assets\\original_ui_course_background50kg.cpp):
    287,772 bytes; u32 1, 1, 1 at 0 / 4 / 8; height 448 at 1036, width 640 at 1040, zeros at 1044 / 1048."""
    import struct
    w = lambda at: struct.unpack_from('<I', b, at)[0]   # noqa: E731
    return len(b) == BKG_BYTES and (w(0), w(4), w(8), w(1036), w(1040), w(1044), w(1048)) == (1, 1, 1, 448, 640, 0, 0)


def port_decode(b):
    """The port decoder's pixel rule (palette[(i & ~24) | ((i & 8) << 1) | ((i & 16) >> 1)], indices at 1052)."""
    pal = np.frombuffer(b, np.uint8, 1024, 12).reshape(256, 4)
    i = np.frombuffer(b, np.uint8, BKG_W * BKG_H, 1052).astype(int)
    e = (i & ~24) | ((i & 8) << 1) | ((i & 16) >> 1)
    return pal[e].reshape(BKG_H, BKG_W, 4)


def background_scene(res_bytes, code):
    """Decoded geometry (textures x vertex colour), sky cube, fog and AI spline of a course archive."""
    import geo_pst
    import geo_psg
    import trk_verify
    import trk_sky
    import trk_binparam
    r = trk_res3.read_res3(res_bytes)
    files = {n.lower(): d for n, d in r.items()}
    flip = np.array([1, 1, -1.0])
    tex = {n.split(BS)[-1][:-4]: geo_pst.rgba_levels(geo_pst.parse(d))[-1]
           for n, d in files.items() if n.startswith('textures' + BS) and n.endswith('.pst')}
    P, T, UV, TT, VC, names = [], [], [], [], [], {}
    base = 0
    for n_, d_ in sorted(files.items()):
        if not n_.endswith('.psg'):
            continue
        for s_ in geo_psg.vertices(geo_psg.parse(d_)):
            if not s_['tris']:
                continue
            ti = names.setdefault(s_['material'].lower(), len(names))
            p = np.array(s_['pos'], float)
            t = np.array(s_['tris'], int)
            if s_['rgba'] and all(x is not None for x in s_['rgba']):
                c = np.array(s_['rgba'], float)
            else:
                c = np.full((len(p), 4), 128.0)
            P.append(p * flip)
            T.append(t[:, [0, 2, 1]] + base)
            UV.append(np.array(s_['uv'], float))
            TT.append(np.full(len(t), ti))
            VC.append(c[t][:, :, :3].mean(1) / 128.0)
            base += len(p)
    textures = [None] * len(names)
    for m, i in names.items():
        textures[i] = tex.get(m, np.full((8, 8, 4), 200, np.uint8))
    scb = dict(trk_binparam.decode(files['tracks' + BS + code + '.scb']))
    fog = dict(scb['fog'])
    clear = np.array([float(x) for x in fog['colorrgb'].split(',')])
    vis_, haz = float(fog.get('visibility', 1)), float(fog.get('haziness', 0))
    fog_end = 1333 + vis_ * 2667                       # port rule (converter flags: end = 1333 + V*2667)
    fog_start = (fog_end - 64) * (1 - haz)
    cub_name = 'skies' + BS + dict(scb['environment']).get('cubefile', code + '.cub').lower()
    tiles = None
    if cub_name in files:
        cub = trk_sky.read_cub(files[cub_name])
        ff = trk_sky.ffmpeg_path()
        tiles = {k: trk_sky.decode_ipum(v[3], ff) for k, v in cub['tiles'].items()}
    _, _, SP = trk_verify.load_spline(files['tracks' + BS + 'a' + code + '.spl'])
    return dict(P=np.vstack(P), T=np.vstack(T), UV=np.vstack(UV), TT=np.concatenate(TT), VC=np.vstack(VC),
                textures=textures, clear=clear, fog=(fog_start, fog_end), tiles=tiles, spline=SP, code=code)


def background_render(scene, eye, target, W, H, fov=BKG_FOV):
    """Establishing view of the course from a camera placed off the track (eye / target in Revenge axes, feet):
    textured, vertex-coloured geometry, sky cube behind (port drawing rule), fog by depth stretched by BKG_FOG
    (distant views would otherwise fog out completely) and capped. -> HxWx3 uint8."""
    sys.path.insert(0, os.path.join(HERE, '..'))
    import softrender as SR
    import trk_sky
    flip = np.array([1, 1, -1.0])
    cam = dict(eye=np.asarray(eye, float) * flip, target=np.asarray(target, float) * flip, up=(0, 1, 0), fov=fov, near=4.0)
    if scene['tiles'] is not None:
        e_, r_, u_, f_ = SR.look_at(cam['eye'], cam['target'], cam['up'])
        foc = (W / 2) / np.tan(np.radians(fov) / 2)
        px, py = np.meshgrid(np.arange(W) + 0.5, np.arange(H) + 0.5)
        d = f_[None, None] + ((px - W / 2) / foc)[..., None] * r_ + (-(py - H / 2) / foc)[..., None] * u_
        d = d / np.linalg.norm(d, axis=-1, keepdims=True) * flip
        sky = trk_sky.lookup(d, scene['tiles'], scene['clear'])
    else:
        sky = np.broadcast_to(scene['clear'], (H, W, 3)).astype(float)
    # pass 1 textures (no lighting: the tracks are prelit), pass 2 vertex colours on the same depth test
    tex_img, zb, _ = SR.render(scene['P'], scene['T'], cam, W, H, uv=scene['UV'], tex_of_tri=scene['TT'],
                               textures=scene['textures'], ambient=1.0, img=np.zeros((H, W, 3)), zbuf=np.full((H, W), np.inf))
    vc_img, _, _ = SR.render(scene['P'], scene['T'], cam, W, H, colors=np.clip(scene['VC'], 0, 1), ambient=1.0,
                             img=np.zeros((H, W, 3)), zbuf=np.full((H, W), np.inf))
    geo = tex_img.astype(float) * (vc_img.astype(float) / 255.0)
    hit = np.isfinite(zb)
    fs, fe = scene['fog'][0] * BKG_FOG[0], scene['fog'][1] * BKG_FOG[0]
    k = BKG_FOG[1] * np.clip((np.where(hit, zb, fe) - fs) / max(fe - fs, 1.0), 0, 1)[..., None]
    geo = geo * (1 - k) + scene['clear'] * k
    out = np.where(hit[..., None], geo, sky)
    return np.clip(out, 0, 255).astype(np.uint8)


def background_overlay(uicourse_res_bytes):
    """Reticle overlay and vignette measured on the five stock course backgrounds (common to all five).
    -> dict(alpha HxW, C line colour, band HxW log factor, vignette HxW factor, facts)."""
    from scipy import ndimage as nd
    ims = np.stack([v.astype(float) for v in stock_backgrounds(uicourse_res_bytes).values()])
    n_img = len(ims)
    sym = lambda a: (a + a[:, ::-1]) / 2     # noqa: E731  (the reticle is left-right symmetric)
    bg = np.stack([np.stack([nd.median_filter(im[..., c], size=7) for c in range(3)], -1) for im in ims])
    d = ims - bg
    dm = sym(d.mean(-1).mean(0))
    # lines: I = bg + a (C - bg); per pixel over 5 images x 3 channels; C from the strong line pixels
    xs = bg.reshape(n_img, -1, 3).transpose(1, 0, 2).reshape(-1, n_img * 3)
    ys = d.reshape(n_img, -1, 3).transpose(1, 0, 2).reshape(-1, n_img * 3)
    xm, ym = xs.mean(1, keepdims=True), ys.mean(1, keepdims=True)
    slope = ((xs - xm) * (ys - ym)).sum(1) / np.maximum(((xs - xm) ** 2).sum(1), 1e-6)
    icpt = ym[:, 0] - slope * xm[:, 0]
    strong = (dm.ravel() > 15) & (-slope > 0.05)
    C = float(np.median(icpt[strong] / -slope[strong]))
    w = C - xs
    a = ((w * ys).sum(1) / np.maximum((w * w).sum(1), 1e-6)).reshape(BKG_H, BKG_W)
    a = np.clip(sym(a), 0, 0.5)
    a[dm < 2.0] = 0
    # chevron strips: darker than their surroundings in every image (log residual against a 31-px median)
    lines = nd.binary_dilation(dm > 3.0, iterations=2)
    L = np.stack([nd.median_filter(np.log(im.mean(-1) + 8), 5) for im in ims])
    res = sym(np.stack([l_ - nd.median_filter(l_, 31) for l_ in L]).mean(0))
    band = (res < -0.04) & ~lines
    lab, n = nd.label(band)
    size = nd.sum(np.ones_like(lab), lab, range(1, n + 1))
    keep = np.isin(lab, 1 + np.flatnonzero(size > 300))
    keep = nd.binary_closing(keep | (lines & nd.binary_dilation(keep, iterations=3)), iterations=2)
    blog = np.where(keep, np.clip(nd.gaussian_filter(np.where(keep, res, 0), 1.5) /
                                  np.maximum(nd.gaussian_filter(keep.astype(float), 1.5), 1e-3), -0.35, 0), 0)
    # vignette: median radial profile of the five images, normalised at 0.3 of the half-diagonal ellipse
    yy, xx = np.mgrid[0:BKG_H, 0:BKG_W]
    rr = np.sqrt(((xx - (BKG_W - 1) / 2) / (BKG_W / 2)) ** 2 + ((yy - (BKG_H - 1) / 2) / (BKG_H / 2)) ** 2)
    Ln = np.log(ims.mean(-1) + 8)
    bins = np.arange(0, 1.45, 0.1)
    prof = np.array([[np.median(l_[(rr >= b) & (rr < b + 0.1)]) for b in bins] for l_ in Ln])
    prof = np.exp(prof - prof[:, 3:4])
    vprof = np.minimum(np.median(prof, 0), 1.0)
    vig = np.interp(rr, bins + 0.05, vprof)
    lum5 = ims @ np.array([0.299, 0.587, 0.114])
    chroma5 = (ims.max(-1) - ims.min(-1)).mean((1, 2))
    return dict(alpha=a, C=C, band=blog, vignette=vig,
                stock_lum_std=float(lum5.std((1, 2)).mean()), stock_chroma_mean=float(chroma5.mean()),
                stock_chroma_max=float(chroma5.max()),
                facts=dict(stock_lum_mean=round(float(lum5.mean()), 1), stock_lum_std_mean=round(float(lum5.std((1, 2)).mean()), 1),
                           stock_chroma_mean=round(float(chroma5.mean()), 1), stock_chroma_max=round(float(chroma5.max()), 1),
                           line_colour=round(C, 1), line_alpha_max=round(float(a.max()), 3), band_pixels=int(keep.sum()),
                           band_factor_min=round(float(np.exp(blog.min())), 3),
                           vignette_profile=[round(float(v), 3) for v in vprof]))


def motion_blur(img):
    """Horizontal box blur whose length grows from the centre column toward the left/right edges (stock look)."""
    H, W = img.shape[:2]
    c0, extra, ex = BKG_BLUR
    s = W / 640.0
    cx = (W - 1) / 2
    xs = np.arange(W)
    half = np.round((c0 + extra * (np.abs(xs - cx) / cx) ** ex) * s / 2).astype(int)
    pad = int(half.max()) + 1
    p = np.pad(img, ((0, 0), (pad, pad), (0, 0)), mode='edge')
    cs = np.concatenate([np.zeros((H, 1, img.shape[2])), np.cumsum(p, 1)], 1)
    lo = xs + pad - half
    hi = xs + pad + half + 1
    return (cs[:, hi] - cs[:, lo]) / (hi - lo)[None, :, None]


def treat(view, ov, exposure=1.0):
    """Rendered view (any size, 640x448 aspect) -> treated (448, 640, 3) float image in the stock look."""
    from PIL import Image
    from scipy import ndimage as nd
    im = np.asarray(Image.fromarray(view).resize((BKG_W, BKG_H), Image.LANCZOS), float)
    im = motion_blur(im)
    im = np.stack([nd.gaussian_filter(im[..., c], (1.6, 1.2)) for c in range(3)], -1)
    lum = im @ np.array([0.299, 0.587, 0.114])
    im = lum[..., None] + BKG_SAT * (im - lum[..., None])
    lo, hi = np.percentile(lum, 1), np.percentile(lum, 99)
    t = np.clip((im - lo) / max(hi - lo, 1.0), 0, 1.2)
    med = float(np.clip(np.median((lum - lo) / max(hi - lo, 1.0)), 0.05, 0.95))
    gamma = float(np.clip(np.log(BKG_MID) / np.log(med), 0.7, 1.4))   # median luminance -> BKG_MID of the range (stock exposure)
    pre = np.power(t, gamma) * (BKG_LEVELS[1] - BKG_LEVELS[0]) + BKG_LEVELS[0]
    yy, xx = np.mgrid[0:BKG_H, 0:BKG_W]
    rr = np.sqrt(((xx - (BKG_W - 1) / 2) / (BKG_W / 2)) ** 2 + ((yy - (BKG_H - 1) / 2) / (BKG_H / 2)) ** 2)
    glow = BKG_GLOW * np.exp(-(rr / 0.32) ** 2)[..., None]

    def finish(x):
        x = (x + glow) * ov['vignette'][..., None] * np.exp(ov['band'])[..., None]
        a = ov['alpha'][..., None]
        return x * (1 - a) + ov['C'] * a

    def stats(x):
        return float((x @ np.array([0.299, 0.587, 0.114])).std()), float((x.max(-1) - x.min(-1)).mean())

    # match the stock five as measured (background_overlay): contrast at most their mean luminance spread, colour
    # BKG_CHROMA_K of our own but at most their most colourful picture (sul); a few passes because the vignette and
    # overlay change both measures. Then the course table's exposure (data).
    s0, c0 = stats(finish(pre))
    want_s = min(s0, ov['stock_lum_std'])
    want_c = min(c0 * BKG_CHROMA_K, ov['stock_chroma_max'])
    ks, kc = 1.0, 1.0
    for _ in range(3):
        ks *= want_s / max(stats(finish(_adjust(pre, ks, kc)))[0], 1e-3)
        kc *= want_c / max(stats(finish(_adjust(pre, ks, kc)))[1], 1e-3)
    im = finish(_adjust(pre, ks, kc)) * exposure
    return im + np.random.default_rng(0).normal(0, BKG_GRAIN, im.shape[:2])[..., None]


def _adjust(x, ks, kc):
    """Scale luminance spread around its mean by ks and colour (distance from grey) by kc."""
    lum = x @ np.array([0.299, 0.587, 0.114])
    m = lum.mean()
    return (m + (lum - m) * ks)[..., None] + (x - lum[..., None]) * kc


def encode(im):
    """(448, 640, 3) float -> PST kind 1, one level, 256 colours, alpha 255 (the stock background layout)."""
    import geo_pst
    rgba = np.concatenate([np.clip(np.round(im), 0, 255), np.full((BKG_H, BKG_W, 1), 255.0)], -1).astype(np.uint8)
    pal = geo_pst.quantise(rgba)
    idx = geo_pst.nearest(rgba.reshape(-1, 4), pal)
    return geo_pst.write(dict(kind=1, palette=pal.tobytes(),
                              levels=[dict(w=BKG_W, h=BKG_H, data=geo_pst.PERM[idx.astype(np.uint8)].tobytes())]))


def course_background(res_bytes, code, view, uicourse_res_bytes, overlay=None):
    """view = dict(eye=(x, y, z), target=(x, y, z)[, fov][, exposure]) in Revenge axes (feet): an establishing camera placed off the
    track (the caller's course table, data only). -> (PST bytes, RGB image as the port decodes it, provenance, facts)."""
    ov = overlay or background_overlay(uicourse_res_bytes)
    scene = background_scene(res_bytes, code)
    img = background_render(scene, view['eye'], view['target'], BKG_W * 2, BKG_H * 2, view.get('fov', BKG_FOV))
    pst = encode(treat(img, ov, view.get('exposure', 1.0)))
    back = port_decode(pst)[..., :3]
    facts = dict(view=view, overlay=ov['facts'], mean_rgb=[round(float(v), 1) for v in back.reshape(-1, 3).mean(0)],
                 std_rgb=[round(float(v), 1) for v in back.reshape(-1, 3).std(0)])
    prov = (r'GENERATED: establishing view of the course archive decoded geometry and sky cube from a camera off the track '
            r'(eye %s, target %s), soft horizontal motion blur, muted photo-like colour, vignette and reticle overlay measured on '
            r'the five stock UICOURSE backgrounds (no stock pixels copied); 640x448 kind 1 one level like UI\Textures\<planet>_bkg.pst'
            % (list(view['eye']), list(view['target'])))
    return pst, back, prov, facts
