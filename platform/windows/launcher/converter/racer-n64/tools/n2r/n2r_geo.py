r"""N64 Racer visual geometry -> Revenge PSG v3 world pieces + PST/PSM textures (one code path for
every N64 track). Called by n2r_convert.py; uses the a2r encoders unchanged (geo_encode, geo_pst,
geo_psm in racer-arcade\tools\a2r).

Rules (provenance in notes\n2r-track.md section 3):
  * positions: n2r_space (scale 1, y/z swap, vertex order reversed) [A + transform].
  * vertex colours: the N64 vertex colours ARE the game's baked lighting (shade input of the
    combiner, formats.md 3); stored as Revenge prelit rgb = min(127, round(c * 128)), alpha 63
    (Revenge opaque prelit alpha, as a2r); blended materials alpha 128 (= 1.0, the N64 texture alpha alone).
    No lighting is generated [A].
    Exception: n2r_look_overrides.json, per-material vertex alpha chosen by the user by eye (NOT faithful,
    flagged in the manifest), matched on the track's model index and N64 material names.
  * normals: flat face normal from the (reversed) winding, needed only by the PSG decoder's
    orientation check (lighting is prelit) [G].
  * double-sided / culled N64 materials: one copy each; the port draws track faces from both sides
    (a2r-geometry.md); N64 back-face culling is not reproduced [flag].
  * translucent N64 render mode (ZMODE_XLU + FORCE_BL, words 0x00104979 / 0x001049d8) -> Revenge alpha
    blend keys exactly as a2r writes them (crowd.psm settings); every other material is opaque and
    its texture alpha is forced to 255 (the N64 opaque render mode ignores texture alpha) [A rule, D keys].
  * clamp_s / clamp_t -> wrapModeHorizontal / wrapModeVertical = 1 (as a2r) [A].
  * material budget: the port admits at most 64 materials on SC; materials whose triangles keep
    every UV inside [0, 1] are paired side by side into 2:1 atlases (same size, same blend) until the
    count fits [G, flagged].
  * pieces: one PSG per (zone, 2016-ft grid cell, material), on the a2r 1/32-ft lattice; zone = the
    zone of the nearest spline sample (given by the caller).
UV split / grid clip helpers are copied from a2r geo_course.py (that module needs the arcade ROM)."""
import json
import os
import sys

import math
import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
A2R = os.path.normpath(os.path.join(HERE, '..', '..', '..', 'racer-arcade', 'tools', 'a2r'))
sys.path.insert(0, HERE)
sys.path.insert(1, A2R)
import geo_encode  # noqa: E402
import geo_pst  # noqa: E402
import geo_psm  # noqa: E402
import n2r_space  # noqa: E402

CELL = 2016.0
LATTICE = 1 / 32.0
QUANTUM_MAX = 0.2
BLEND_MODES = ('0x00104979', '0x001049d8', '0x001049f8')
MAX_MATERIALS = None      # no material cap: the original material/texture tables grow on demand (382B38 / 29E340);
                          # the live limit is the 8 MB track texture budget, checked by trk_silo.py
                          # (work\scratch\track-data-gaps\NOTES.md 4). Atlas packing is off.
FRAMES_PER_SECOND = 60.0  # Revenge race frames per second for per-frame UV velocity (NOTES.md 3: not re-checked)
UV_SLAB = 8.0
SPLIT_SPAN = 31.0


# ------------------------------------------------------------- copied from a2r geo_course.py
def clip_poly(poly, axis, value, keep_below):
    out = []
    for i in range(len(poly)):
        a, b = poly[i], poly[(i + 1) % len(poly)]
        ina = (a[axis] <= value) if keep_below else (a[axis] >= value)
        inb = (b[axis] <= value) if keep_below else (b[axis] >= value)
        if ina:
            out.append(a)
        if ina != inb:
            t = (value - a[axis]) / (b[axis] - a[axis])
            out.append(a + (b - a) * t)
    return out


Y_BAND = 4096.0   # pieces are also cut into 4,096-ft height bands: a PSG piece holds int16 positions on the 1/32-ft lattice
                  # (QUANTUM_MAX 0.2 ft -> at most about 6,500 ft across); tall scaled tracks (Vengeance at 2.5) exceed it in y


def cells_of(P):
    lo = np.floor(P[:, [0, 2]].min(0) / CELL).astype(int)
    hi = np.floor(P[:, [0, 2]].max(0) / CELL).astype(int)
    ylo, yhi = int(np.floor(P[:, 1].min() / Y_BAND)), int(np.floor(P[:, 1].max() / Y_BAND))
    return [(cx, cz, cy) for cx in range(lo[0], hi[0] + 1) for cz in range(lo[1], hi[1] + 1) for cy in range(ylo, yhi + 1)]


def clip_to_cells(P, Nn, UV, C):
    cs = cells_of(P)
    if len(cs) == 1:
        yield cs[0], P, Nn, UV, C
        return
    V = [np.r_[P[i], Nn[i], UV[i], C[i].astype(float)] for i in range(3)]
    for cx, cz, cy in cs:
        poly = V
        for axis, val, below in ((0, cx * CELL, False), (0, (cx + 1) * CELL, True),
                                 (2, cz * CELL, False), (2, (cz + 1) * CELL, True),
                                 (1, cy * Y_BAND, False), (1, (cy + 1) * Y_BAND, True)):
            poly = clip_poly(poly, axis, val, below)
            if len(poly) < 3:
                break
        if len(poly) < 3:
            continue
        for k in range(1, len(poly) - 1):
            tri = np.array([poly[0], poly[k], poly[k + 1]])
            if np.linalg.norm(np.cross(tri[1, :3] - tri[0, :3], tri[2, :3] - tri[0, :3])) < 1e-6:
                continue
            yield (cx, cz, cy), tri[:, :3], tri[:, 3:6], tri[:, 6:8], np.clip(np.round(tri[:, 8:12]), 0, 255).astype(np.uint8)


def split_uv(P, Nn, UV, C):
    span = UV.max(0) - UV.min(0)
    if span.max() <= SPLIT_SPAN:
        yield P, Nn, UV, C
        return
    V = [np.r_[P[i], Nn[i], UV[i], C[i].astype(float)] for i in range(3)]
    lo = np.floor(UV.min(0) / UV_SLAB).astype(int)
    hi = np.floor(UV.max(0) / UV_SLAB).astype(int)
    for a in (0, 1):
        if span[a] <= SPLIT_SPAN:
            hi[a] = lo[a]
    for ku in range(lo[0], hi[0] + 1):
        for kv in range(lo[1], hi[1] + 1):
            poly = V
            for axis, val, below in ((6, ku * UV_SLAB, False), (6, (ku + 1) * UV_SLAB, True),
                                     (7, kv * UV_SLAB, False), (7, (kv + 1) * UV_SLAB, True)):
                if span[axis - 6] <= SPLIT_SPAN:
                    continue
                poly = clip_poly(poly, axis, val, below)
                if len(poly) < 3:
                    break
            if len(poly) < 3:
                continue
            for k in range(1, len(poly) - 1):
                tri = np.array([poly[0], poly[k], poly[k + 1]])
                if np.linalg.norm(np.cross(tri[1, :3] - tri[0, :3], tri[2, :3] - tri[0, :3])) < 1e-6:
                    continue
                yield tri[:, :3], tri[:, 3:6], tri[:, 6:8], np.clip(np.round(tri[:, 8:12]), 0, 255).astype(np.uint8)


def shift_uv(UV):
    if np.abs(UV).max() <= 15.9:
        return UV, False
    return UV - np.round((UV.min(0) + UV.max(0)) / 2), True


# ------------------------------------------------------------- materials
def material_key(e, scroll=None):
    rm = e['render_mode_words'][1]
    k = (e.get('texture'), rm if rm in BLEND_MODES else False, bool(e.get('clamp_s')), bool(e.get('clamp_t')))
    return k + ((scroll,) if scroll else ())


def n64_uv_scrolls(D):
    """N64 texture-scroll animations (Anim list types 0xB = u, 0xC = v; n64-loader-re.md 7).
    swrModel_UpdateTextureScrollAnimation (PC 0x00426..., swrModel.c ~838): MeshMaterial.texture_offset[axis] =
    res * value(t), wrapped into [0, res]; the vertex UV is offset by -texture_offset/res, so the visible texture
    coordinate moves by -d(value)/dt texture repeats per second. -> {material name: (axis, uv units per second)}"""
    sys.path.insert(0, os.path.normpath(os.path.join(HERE, '..', 'n64r')))
    import model as Mo
    M = Mo.Model(int(D['track']['model_index']))
    return model_uv_scrolls(M)

def model_uv_scrolls(M):
    """Linear U/V texture-coordinate rates keyed by this model's material offsets."""
    out = {}
    for a in M.anims:
        if a['type'] not in (0xB, 0xC) or not a['target'] or len(a['times']) != 2:
            continue
        (t0, t1), (v0, v1) = a['times'], (a['values'][0][0], a['values'][1][0])
        if t1 <= t0:
            continue
        out['m%06x' % a['target']] = ('u' if a['type'] == 0xB else 'v', -(v1 - v0) / (t1 - t0))
    return out


# N64 render mode (SetOtherMode_L, n64-loader-re.md 10) -> Revenge PSM keys. Both translucent modes blend
# CLR_IN*A_IN + CLR_MEM*(1-A_IN) (= Revenge A=0 Cs, B=1 Cd, C=0 As, D=1 Cd, the keys of TA crowd.psm);
#   0x001049D8 = G_RM_AA_ZB_XLU_SURF2: Z_CMP without Z_UPD               -> zWriteDisabled 1
#   0x00104979 = the same + Z_UPD + alpha compare THRESHOLD (blend alpha 1 -> alpha > 0 passes)
#                                                                      -> zWriteDisabled 0, alpha test GREATER 0
BLEND_PSM = {
    '0x001049d8': [('alphaBlendingOnOff', '1'), ('inputColorA', '0'), ('inputColorB', '1'), ('inputAlphaC', '0'),
                   ('inputAlphaD', '1'), ('zWriteDisabled', '1'), ('alphaTestOnOff', '0')],
    # Model 293 differs from XLU_SURF2 only by Z_UPD (bit 5).
    '0x001049f8': [('alphaBlendingOnOff', '1'), ('inputColorA', '0'), ('inputColorB', '1'), ('inputAlphaC', '0'),
                   ('inputAlphaD', '1'), ('zWriteDisabled', '0'), ('alphaTestOnOff', '0')],
    '0x00104979': [('alphaBlendingOnOff', '1'), ('inputColorA', '0'), ('inputColorB', '1'), ('inputAlphaC', '0'),
                   ('inputAlphaD', '1'), ('zWriteDisabled', '0'), ('alphaTestOnOff', '1'), ('alphaTestMethod', '6'),
                   ('alphaReferenceValue', '0'), ('processingMethodWhenFailedInAlphaTest', '0')],
}


def plan_materials(D, tris_by_key):
    """-> {key: dict(name, image (h,w,4), uv_map=(su, ou, sv, ov), extra, sources)} with atlas pairs."""
    tdir = os.path.join(D['dir'], 'textures')
    keys = sorted(tris_by_key, key=lambda k: (str(k[0]), str(k[1:])))
    imgs = {}
    for k in keys:
        if k[0] is None:
            im = np.full((16, 16, 4), 255, np.uint8)
        else:
            im = np.asarray(Image.open(os.path.join(tdir, k[0])).convert('RGBA')).copy()
        if not k[1]:
            im[..., 3] = 255
        else:
            # Revenge texture alpha is in GS units, 0x80 = 1.0 (original beam / light-strip PSTs peak at 75-117;
            # the port scales texture alpha by 255/128, material_alpha50gx_fixture.cpp selectSc)
            im[..., 3] = np.round(im[..., 3].astype(np.float64) * 128.0 / 255.0).astype(np.uint8)
        imgs[k] = im
    plan = {k: dict(image=imgs[k], uv_map=(1.0, 0.0, 1.0, 0.0), sources=[k[0]]) for k in keys}
    atlas_pairs = []
    if MAX_MATERIALS is not None and len(keys) > MAX_MATERIALS:
        need = len(keys) - MAX_MATERIALS
        eps = 0.002
        u_in = {k: all(uv[:, 0].min() >= -eps and uv[:, 0].max() <= 1 + eps for uv in tris_by_key[k]) for k in keys}
        v_in = {k: all(uv[:, 1].min() >= -eps and uv[:, 1].max() <= 1 + eps for uv in tris_by_key[k]) for k in keys}
        cand = [k for k in keys if k[0] is not None and (u_in[k] or v_in[k])]
        cand.sort(key=lambda k: len(tris_by_key[k]))
        used = set()
        for i, a in enumerate(cand):
            if len(atlas_pairs) >= need or a in used:
                continue
            for b in cand[i + 1:]:
                if b in used or b[1:] != a[1:] or imgs[a].shape != imgs[b].shape:
                    continue
                axis = 'u' if (u_in[a] and u_in[b]) else 'v' if (v_in[a] and v_in[b]) else None
                if axis is None:
                    continue
                atlas_pairs.append((a, b, axis)); used |= {a, b}
                break
        if len(atlas_pairs) < need:
            raise ValueError('material budget: %d materials, only %d atlas pairs possible' % (len(keys), len(atlas_pairs)))
        for a, b, axis in atlas_pairs:
            if axis == 'u':   # side by side: u squeezed into its half, v keeps repeating
                im = np.concatenate([imgs[a], imgs[b]], 1)
                plan[a] = dict(image=im, uv_map=(0.5, 0.0, 1.0, 0.0), sources=[a[0], b[0]], atlas=axis)
                plan[b] = dict(image=im, uv_map=(0.5, 0.5, 1.0, 0.0), sources=[a[0], b[0]], atlas=axis, alias=a)
            else:             # stacked: v squeezed, u keeps repeating
                im = np.concatenate([imgs[a], imgs[b]], 0)
                plan[a] = dict(image=im, uv_map=(1.0, 0.0, 0.5, 0.0), sources=[a[0], b[0]], atlas=axis)
                plan[b] = dict(image=im, uv_map=(1.0, 0.0, 0.5, 0.5), sources=[a[0], b[0]], atlas=axis, alias=a)
    return plan, atlas_pairs


def material_name(code, k, plan):
    p = plan[k]
    if 'alias' in p:
        k = p['alias']; p = plan[k]
    base = 'white' if k[0] is None else os.path.basename(k[0]).split('_')[0]
    if p.get('atlas'):
        base = 'at' + p['sources'][0].split('_')[0][3:] + '_' + p['sources'][1].split('_')[0][3:]
    return ('%s_%s%s%s%s' % (code, base, {'0x001049d8': '_b', '0x00104979': '_bt', '0x001049f8': '_bz'}.get(k[1], ''), '_c' if (k[2] or k[3]) else '',
                             '_uv' if len(k) > 4 else '')).lower(), k


LOOK_OVERRIDES = os.path.join(HERE, 'n2r_look_overrides.json')


def look_overrides(D):
    """User-chosen appearance overrides (n2r_look_overrides.json; NOT faithful values, each one flagged) for this
    track -> ({N64 material name: vertex alpha}, [applied rows]). One generic hook: rows are matched on the track's
    own model index and N64 material names; a row naming a missing or non-blended material is refused."""
    if not os.path.exists(LOOK_OVERRIDES):
        return {}, []
    doc = json.load(open(LOOK_OVERRIDES, encoding='utf-8'))
    model = int(D['track']['model_index'])
    alpha, rows = {}, []
    for r in doc['overrides']:
        if int(r['model']) != model:
            continue
        va = int(r['vertex_alpha'])
        if not 0 <= va <= 128:
            raise ValueError('look override %s: vertex_alpha %d outside 0..128' % (r['n64_materials'], va))
        for m in r['n64_materials']:
            e = D['materials'].get(m)
            if e is None:
                raise ValueError('look override: N64 material %s not in model %d' % (m, model))
            if e.get('texture') != r['n64_texture'] or e['render_mode_words'][1] not in BLEND_MODES:
                raise ValueError('look override: N64 material %s is %s / %s, row expects blended %s' % (
                    m, e.get('texture'), e['render_mode_words'][1], r['n64_texture']))
            alpha[m] = va
        rows.append(dict(r))
    return alpha, rows


# ------------------------------------------------------------- conversion

LAYER_SEP_FT = 1.0       # gap written between coplanar overlapping visual faces (the a2r rule: >= 1 ft keeps a 24-bit depth
                         # buffer with a 1-ft near plane apart up to about 2,000 ft, beyond the N64 fog ends of 2,000-2,500 ft)
COPLANAR_COS = math.cos(math.radians(1.0))
COPLANAR_D_FT = 0.5      # two faces this close to each other's plane (and parallel, overlapping) are one layer


def _clip_convex(subj, clip):
    """Sutherland-Hodgman: convex polygon subj clipped by convex polygon clip (2D, counter-clockwise)."""
    out = list(subj)
    n = len(clip)
    for i in range(n):
        a, b = clip[i], clip[(i + 1) % n]
        inp, out = out, []
        if not inp:
            break
        def inside(q):
            return (b[0] - a[0]) * (q[1] - a[1]) - (b[1] - a[1]) * (q[0] - a[0]) >= -1e-9
        for j in range(len(inp)):
            c, d = inp[j], inp[(j + 1) % len(inp)]
            ic, id_ = inside(c), inside(d)
            if ic:
                out.append(c)
            if ic != id_:
                e1 = (d[0] - c[0], d[1] - c[1]); e2 = (b[0] - a[0], b[1] - a[1])
                den = e1[0] * e2[1] - e1[1] * e2[0]
                if abs(den) > 1e-12:
                    t = ((a[0] - c[0]) * e2[1] - (a[1] - c[1]) * e2[0]) / den
                    out.append((c[0] + e1[0] * t, c[1] + e1[1] * t))
    return out


def _area(poly):
    return 0.5 * abs(sum(poly[i][0] * poly[(i + 1) % len(poly)][1] - poly[(i + 1) % len(poly)][0] * poly[i][1] for i in range(len(poly))))


def _ccw(poly):
    a = sum(poly[i][0] * poly[(i + 1) % len(poly)][1] - poly[(i + 1) % len(poly)][0] * poly[i][1] for i in range(len(poly)))
    return poly if a >= 0 else poly[::-1]


def layer_coplanar(items, mats):
    """Coplanar overlapping visual faces fight in Revenge (the port draws every face from both sides with GL_LESS) where the
    N64 showed one of them: a one-sided (back-culled) face back to back with another is only seen from its own front side, and
    of two faces facing the same way the N64 shows the one drawn later (RDP z compare passes a coincident later fragment;
    display-list order = the model's mesh order = items order). Faithful look in Revenge: separate them by LAYER_SEP_FT -
    back-to-back culled faces each move half the gap toward their visible side; of a same-facing pair the later one moves the
    full gap toward its visible side. Visual only (collision unchanged). Returns (items, stats)."""
    def visible_dir(it):
        cull = (mats.get(it[5]) or {}).get('cull', 'none') if it[5] else 'none'
        return 1.0 if cull == 'back' else -1.0 if cull == 'front' else 0.0
    N = np.array([it[2] for it in items]); C = np.array([it[1].mean(0) for it in items])
    big = np.argmax(np.abs(N), 1)
    sgn = np.sign(N[np.arange(len(N)), big]); sgn[sgn == 0] = 1
    Nc = N * sgn[:, None]; Dc = (Nc * C).sum(1)
    groups = {}
    for i in range(len(items)):
        key = (tuple(np.round(Nc[i] * 40).astype(int)), int(np.floor(Dc[i] / COPLANAR_D_FT)))
        groups.setdefault(key, []).append(i)
    shift = np.zeros(len(items)); seen = set(); stats = dict(back_pairs=0, facing_pairs=0, faces_moved=0)
    for key, ids in groups.items():
        cand = list(ids)
        for dk in (-1, 1):
            cand += groups.get((key[0], key[1] + dk), [])
        if len(cand) < 2:
            continue
        for a_i in ids:
            for b_i in cand:
                if b_i <= a_i or (a_i, b_i) in seen:
                    continue
                seen.add((a_i, b_i))
                if abs(N[a_i] @ N[b_i]) < COPLANAR_COS or abs(Nc[a_i] @ (C[b_i] - C[a_i])) > COPLANAR_D_FT:
                    continue
                if items[a_i][6] is not None or items[b_i][6] is not None:
                    continue
                # overlap on the common plane
                n = Nc[a_i]; u = np.cross(n, [0.0, 1.0, 0.0] if abs(n[1]) < 0.9 else [1.0, 0.0, 0.0]); u /= np.linalg.norm(u); v = np.cross(n, u)
                pa = _ccw([(float(q @ u), float(q @ v)) for q in items[a_i][1]]); pb = _ccw([(float(q @ u), float(q @ v)) for q in items[b_i][1]])
                inter = _clip_convex(pa, pb)
                if len(inter) < 3:
                    continue
                ar = _area(inter)
                if ar < 0.25 or ar < 0.01 * min(_area(pa), _area(pb)):
                    continue
                same = N[a_i] @ N[b_i] > 0
                if same:
                    stats['facing_pairs'] += 1
                    vd = visible_dir(items[b_i]) or 1.0
                    shift[b_i] = max(shift[b_i], LAYER_SEP_FT) if vd > 0 else min(shift[b_i], -LAYER_SEP_FT)
                else:
                    stats['back_pairs'] += 1
                    for i_ in (a_i, b_i):
                        vd = visible_dir(items[i_])
                        if vd:      # back to back: each face a full LAYER_SEP_FT toward its visible side (2 ft apart: past the fog end)
                            shift[i_] = vd * max(abs(shift[i_]), LAYER_SEP_FT)
    out = []
    for i, it in enumerate(items):
        if shift[i]:
            stats['faces_moved'] += 1
            it = (it[0], it[1] + it[2] * shift[i]) + tuple(it[2:])
        out.append(it)
    return out, stats


def convert(D, X, code, zone_of_points, out_dir, uv_scroll=True, anim_groups=None):
    """zone_of_points(P (n,3) Revenge) -> zone ids. Returns manifest dict; writes geometry\\*.psg and
    textures\\*.pst/.psm into out_dir."""
    mats = D['materials']
    order = n2r_space.winding()
    tris_by_key = {}
    items = []
    stats = dict(triangles_in=0, degenerate_dropped=0, uv_shifted=0, uv_split_pieces=0, clipped_pieces=0)
    scrolls = n64_uv_scrolls(D) if uv_scroll else {}
    ov_alpha, ov_rows = look_overrides(D)
    ov_tris = {}
    for g in D['geometry']:
        e = mats[g['mtl']] if g['mtl'] else dict(texture=None, render_mode_words=['', ''])
        sc = e.get('model_uv_scroll') or scrolls.get(g['mtl'])
        if sc:
            # the exported PNG of a mirrored texture is doubled and its UVs halved (formats.md 3)
            half = (e.get('mirror_s') and sc[0] == 'u') or (e.get('mirror_t') and sc[0] == 'v')
            sc = (sc[0], sc[1] * (0.5 if half else 1.0))
        k = material_key(e, sc)
        for P, C, UV in zip(g['P'], g['C'], g['UV']):
            stats['triangles_in'] += 1
            P = n2r_space.points(P, X)[order]
            C = C[order]; UV = UV[order]
            n = np.cross(P[1] - P[0], P[2] - P[0]); ln = np.linalg.norm(n)
            if ln < 1e-6:
                stats['degenerate_dropped'] += 1
                continue
            tris_by_key.setdefault(k, []).append(UV)
            items.append((k, P, n / ln, UV, C, g['mtl'], (anim_groups or {}).get(g['name'])))
    items, layer_stats = layer_coplanar(items, mats)
    stats.update(layer_stats)
    plan, pairs = plan_materials(D, tris_by_key)
    names = {}
    for k in plan:
        names[k] = material_name(code, k, plan)
    # zones of every triangle centroid
    anim_buckets = {}
    cen = np.array([it[1].mean(0) for it in items])
    zt = zone_of_points(cen)
    buckets = {}
    for (k, P, n, UV, C, mtl, anim), z in zip(items, zt):
        su, ou, sv, ov = plan[k]['uv_map']
        uv = UV.copy()
        if plan[k].get('atlas'):
            uv = np.stack([uv[:, 0] * su + ou, uv[:, 1] * sv + ov], 1)
        Nn = np.repeat(n[None], 3, 0)
        rgba = np.zeros((3, 4), np.uint8)
        rgba[:, :3] = np.minimum(127, np.round(np.clip(C, 0, 1) * 128)).astype(np.uint8)
        # opaque: Revenge prelit alpha 63 as every original piece. Blended N64 materials: the N64 shade alpha is the
        # per-vertex fog value, not a transparency (combiner alpha = TEXEL0 only, n64-loader-re.md 10.1), while the port
        # (as the GS) modulates texture alpha by vertex alpha in 128-based units (material_alpha50gx_fixture.cpp: scale
        # (255/128)^2): vertex alpha 0x80 = 1.0 keeps the N64 texture alpha unchanged (63 halved it)
        rgba[:, 3] = 128 if k[1] else 63
        if mtl in ov_alpha:
            # user-chosen look (n2r_look_overrides.json), not the faithful value above [flag]
            rgba[:, 3] = ov_alpha[mtl]
            ov_tris[mtl] = ov_tris.get(mtl, 0) + 1
        mname, mk = names[k]
        pieces = list(split_uv(P, Nn, uv, rgba))
        stats['uv_split_pieces'] += len(pieces) - 1
        if anim is not None:
            # --n64-hazards: an animation row's own geometry (n2r_hazards.py), not a static world piece
            for P1, N1, UV1, C1 in pieces:
                uv2, sh = shift_uv(UV1)
                anim_buckets.setdefault(anim, {}).setdefault(mname, []).append((P1, N1, uv2, C1))
            continue
        for P1, N1, UV1, C1 in pieces:
            for cell, p, nn, uv2, c in clip_to_cells(P1, N1, UV1, C1):
                stats['clipped_pieces'] += 1
                uv2, sh = shift_uv(uv2)
                stats['uv_shifted'] += sh
                # zone of the clipped piece itself (a triangle across a cell line goes to the zone of each part; with
                # slab zones on the cell grid every piece then lies inside its own zone)
                zc = int(zone_of_points(p.mean(0)[None])[0])
                buckets.setdefault((zc, cell, mname), []).append((p, nn, uv2, c))
    gdir = os.path.join(out_dir, 'geometry'); tdir = os.path.join(out_dir, 'textures')
    for d_ in (gdir, tdir):
        os.makedirs(d_, exist_ok=True)
        for f in os.listdir(d_):
            os.remove(os.path.join(d_, f))
    files = []
    mat_index = {}
    for (z, cell, mname), lst in sorted(buckets.items()):
        mi = mat_index.setdefault(mname, len(mat_index))
        P = np.vstack([x[0] for x in lst]); Nn = np.vstack([x[1] for x in lst])
        UV = np.vstack([x[2] for x in lst]); C = np.vstack([x[3] for x in lst])
        T = np.arange(len(P)).reshape(-1, 3)
        name = '%s_z%02d_%02d%02d_m%02d' % (code, z, cell[0], cell[1], mi) + ('_y%d' % cell[2] if cell[2] else '')
        blob, info = geo_encode.build_psg(name, mname, P, Nn, UV, C, T, world=True, lattice=LATTICE)
        if info['quantum_ft'] > QUANTUM_MAX:
            raise ValueError('%s quantum %.3f ft' % (name, info['quantum_ft']))
        open(os.path.join(gdir, name + '.psg'), 'wb').write(blob)
        info.update(file='geometry\\%s.psg' % name, zone=z, cell=list(cell))
        files.append(info)
    textures = []
    done = set()
    for k in plan:
        mname, mk = names[k]
        if mname in done:
            continue
        done.add(mname)
        pst, pinfo = geo_pst.encode_rgba(plan[mk]['image'])
        extra = []
        if mk[2]:
            extra.append(('wrapModeHorizontal', '1'))
        if mk[3]:
            extra.append(('wrapModeVertical', '1'))
        if mk[1]:
            extra += BLEND_PSM[mk[1]]
        program = 'PreLitCompressed'
        if len(mk) > 4:
            # Revenge scrolls a pass only with the VertexAnimation leaf (29A310 -> 29A354 uvs_animate); velocity is
            # texture units per drawn frame and the matrix step translation is MINUS the velocity (29A910), so a
            # visible +w units/s needs velocity -w/60. The only texture-transform microprograms are DiffuseTexTrans
            # and DiffuseTexTransCompressed (exe strings 2A56D0 / 2A5840; every stock scrolling PSM uses the latter):
            # the surface is then lit by the Revenge lights instead of its N64 baked colours [flag]
            axis, w = mk[4]
            program = 'DiffuseTexTransCompressed'
            extra += [('VertexAnimation', 'unique'), ('uvs_animate', '1'),
                      ('TexGenMatrixRow1', '1.0, 0.0, 0.0, 0.0'), ('TexGenMatrixRow2', '0.0, 1.0, 0.0, 0.0'),
                      ('TexGenMatrixRow3', '0.0, 0.0, 1.0, 0.0'), ('TexGenMatrixRow4', '0.0, 0.0, 0.0, 1.0'),
                      ('u_velocity', '%.9f' % (-w / FRAMES_PER_SECOND if axis == 'u' else 0.0)),
                      ('v_velocity', '%.9f' % (-w / FRAMES_PER_SECOND if axis == 'v' else 0.0))]
        psm = geo_psm.material(mname, program, extra)
        open(os.path.join(tdir, mname + '.pst'), 'wb').write(pst)
        open(os.path.join(tdir, mname + '.psm'), 'wb').write(psm)
        textures.append(dict(material=mname, n64_textures=plan[mk]['sources'], blend=mk[1], atlas=bool(plan[mk].get('atlas')),
                             program=program, uv_scroll=list(mk[4]) if len(mk) > 4 else None,
                             psm_extra=extra, **pinfo))
    for r in ov_rows:
        r['triangles'] = {m: ov_tris.get(m, 0) for m in r['n64_materials']}
        r['revenge_materials'] = sorted({names[k][0] for (k, _, _, _, _, m, _) in items if m in r['n64_materials']})
    out = dict(code=code, transform=X, stats=stats, files=files, textures=textures, look_overrides=ov_rows,
                atlas_pairs=[[a[0], b[0], ax] for a, b, ax in pairs], materials=len(textures),
                triangles_out=sum(f['triangles'] for f in files))
    if anim_groups is not None:
        out['anim_buckets'] = anim_buckets
    return out
