r"""Course members an ADD-ON course (a course at its own, non-stock code) must carry itself instead of
borrowing them from a stock archive. One rule for every converter (a2r trk_convert.py, n2r n2r_convert.py)
and every add-on course; used only when the course code is not one of the 13 stock codes (a course packed
under a stock code keeps that code's pinned members, see silo-override.md 5).

NOT ORIGINAL BEHAVIOUR: add-on courses are a PC enhancement of the port. Each member below is either generated
from the course's own data or authored minimally in the original file format; every choice is reported as a
manifest flag by the caller, and the stand-ins are listed in notes\addon-out.md.

  tun(phy_class)        tracks\<code>.tun   AUTHORED: the original section/key layout of the 13 stock .tun files,
                                            values of the physics-class donor (TA / TB / TC, conversion-spec 1.4).
                                            STAND-IN: neither source game has race-damage / AI-pace tuning.
  debug_textures()      textures\debug{256,128,64,32}.tga
                                            AUTHORED: 16-bit uncompressed TGA checkerboards, the sizes and pixel
                                            format of the stock files (12 of 13 stock archives carry them under
                                            these fixed names; no port reader uses them). Not stock bytes.
  loading_screen(rgba)  UI\TEXTURES\LD_<CODE>.PST
                                            GENERATED from an image of the course's own geometry: 640x448, kind 1,
                                            one level, 256-colour palette (the layout of LD_TA.PST).
  intro audio           audio\introAud.ini / audio\scrptLns.ini: both converters already write NumEvents=0 /
                        NumberOfLines=0 (no announcer lines exist for a converted course). Original format;
                        the port's intro reader refuses 0 events today (original_ta_intro_authoring_candidate.h:133).
  particle atlas        particle\demoprt.prt/.pst/.psm: NOT replaced. The original names these the same in every
                        course (fixed names, read as 'demoprt.tga' / texture 'demoprt' by the port); they hold the
                        game-wide pod effect art (smoke, dust, sparks, fire, explosions). TA's base atlas is used
                        (stock bytes; the 6 courses with water/emitters add frames to it). Flagged as stock bytes.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import trk_ini  # noqa: E402
import trk_res3  # noqa: E402

BS = chr(92)
REVENGE = (__import__('os').environ['RR_CONVERT_STOCK']+'/TRACKS/')
STOCK_CODES = ('ta', 'tb', 'tc', 'ra', 'rc', 'ma', 'mb', 'mc', 'ga', 'gb', 'gc', 'sa', 'sc')
DONOR_BY_CLASS = {'A': 'TA', 'B': 'TB', 'C': 'TC'}   # conversion-spec 1.4

# The original .tun layout (sections and keys in the order every stock file uses; RA omits two keys,
# the reader skips absent keys, src\assets\impact_authoring49y.cpp:126). Values come from the donor.
TUN_LAYOUT = [
    ('EnvDamage', ['hitXL', 'hitL', 'hitM', 'hitS', 'hitXS', 'pointsXL', 'pointsL', 'pointsM', 'pointsS', 'pointsXS']),
    ('PodDamage', ['hitXL', 'hitL', 'hitM', 'hitS', 'hitXS', 'pointsXL', 'pointsL', 'pointsM', 'pointsS', 'pointsXS',
                   'AIPointsMultiplier', 'AIMultiplierDecrement', 'MinGrindingSpeed', 'FinalDamageThreshold',
                   'FinalDamageIncrement', 'WaitToRepairEnvTime', 'RepairEnvThreshold', 'RepairEnvUntil',
                   'AIEnvPointsMultiplier']),
    ('AIDriving', ['Lookahead', 'BehindPlayerClearance', 'StartLineBoostTime', 'RacePaceCutoffTime', 'RacePace']),
    ('Packing', ['NumPackingFront', 'NumPackingRear', 'FarFrontDist', 'CloseFrontDist', 'FarBehindDist',
                 'CloseBehindDist', 'SlowestSpeedPct', 'FastestSpeedPct']),
]


def is_addon(code):
    return code.lower() not in STOCK_CODES


def _stock(code):
    return trk_res3.read_res3(open(REVENGE + code.upper() + '.RES', 'rb').read())


def tun(phy_class):
    """-> (bytes, provenance). The donor's values in the original layout; every key the donor has is written
    (a key outside TUN_LAYOUT refuses: the layout would be incomplete)."""
    donor = DONOR_BY_CLASS[phy_class]
    r = _stock(donor)
    name = [n for n in r.names() if n.lower().endswith('.tun')][0]
    secs = trk_ini.parse(r.get(name)).sections()
    vals = {(s.lower(), k.lower()): v.strip() for s, kv in secs for k, v in kv}
    known = {(s.lower(), k.lower()) for s, ks in TUN_LAYOUT for k in ks}
    extra = sorted(set(vals) - known)
    if extra:
        raise ValueError('donor %s has .tun keys outside the layout: %s' % (name, extra))
    out = ['; course tuning (original .tun layout), written by the converter for an add-on course',
           '; values: physics class %s donor %s (%s); no source-game data exists for these keys' % (phy_class, donor, name), '']
    n = 0
    for sec, keys in TUN_LAYOUT:
        out.append('[%s]' % sec)
        for k in keys:
            v = vals.get((sec.lower(), k.lower()))
            if v is not None:
                if len(v) > 11:
                    raise ValueError('.tun value longer than the reader takes (11): %s %s' % (k, v))
                out.append('%s = %s' % (k, v))
                n += 1
        out.append('')
    data = ('\r\n'.join(out) + '\r\n').encode('latin-1')
    return data, ('AUTHORED in the original .tun layout (%d keys); VALUES = physics class %s donor %s [D values, '
                  'STAND-IN: no source tuning]' % (n, phy_class, name))


def _tga16(side, cell=8):
    """Uncompressed 16-bit (A1R5G5B5) TGA, bottom-left origin like the stock debug files, checkerboard
    of white / black cells, 26-byte TGA 2.0 footer (no extension area)."""
    hdr = struct.pack('<BBBHHBHHHHBB', 0, 0, 2, 0, 0, 0, 0, 0, side, side, 16, 0)
    white, black = struct.pack('<H', 0x7FFF), struct.pack('<H', 0x0000)
    rows = []
    for y in range(side):
        rows.append(b''.join(white if ((x // cell) + (y // cell)) % 2 == 0 else black for x in range(side)))
    foot = struct.pack('<II', 0, 0) + b'TRUEVISION-XFILE.\0'
    return hdr + b''.join(rows) + foot


def debug_textures():
    """-> [(member, bytes, provenance)] for textures\\debug{256,128,64,32}.tga."""
    out = []
    for side in (256, 128, 64, 32):
        out.append(('textures' + BS + 'debug%d.tga' % side, _tga16(side, max(4, side // 8)),
                    'AUTHORED: %dx%d 16-bit TGA checkerboard (size and pixel format of the stock debug texture; '
                    'fixed name carried by 12 of 13 stock archives, no port reader) [G]' % (side, side)))
    return out


def loading_screen(rgba, what):
    """rgba: HxWx4 uint8 image (any size) -> (PST bytes, provenance): fitted into 640x448 on black, one level,
    kind 1, 256-colour palette (geo_pst quantiser, as trk_convert.py writes the arcade loading screens)."""
    import numpy as np
    from PIL import Image
    import geo_pst
    im = Image.fromarray(np.asarray(rgba, np.uint8)).convert('RGBA')
    im.thumbnail((640, 448))
    canvas = Image.new('RGBA', (640, 448), (0, 0, 0, 255))
    canvas.paste(im, ((640 - im.width) // 2, (448 - im.height) // 2))
    a = np.asarray(canvas).copy()
    a[..., 3] = 255
    pal = geo_pst.quantise(a)
    idx = geo_pst.nearest(a.reshape(-1, 4), pal)
    pst = geo_pst.write(dict(kind=1, palette=pal.tobytes(),
                             levels=[dict(w=640, h=448, data=geo_pst.PERM[idx.astype(np.uint8)].tobytes())]))
    return pst, 'GENERATED: %s on 640x448, kind 1 single level like LD_TA [A-derived]' % what


# ------------------------------------------------------------------ course-screen hologram preview
# dlc.json course_screen {"stock_template": <code>, "preview": <PST>}: the stock hologram plane drawn with the
# package's own image, which must have the size and format of the stock texture it stands in for
# (UI\UICOURSE\UICOURSE.RES ui\textures\holo_<code>.pst: 256x256, kind 1, one level, 256-colour palette).
# Look measured on the stock holo_tb/sc/gc textures (look_holo2.py in work-addon\dlcjson): blue monochrome
# (G = 0.67 B - 10, R = 0.09 B - 8, B up to 201), horizontal scan bands with a 6-row period, alpha plateau 171
# with a soft edge about 40 texels wide. The image is a chase view of the course's own decoded geometry.
HOLO_SIDE = 256
HOLO_ALPHA_EDGE = ((0, 4), (8, 45), (16, 108), (24, 140), (32, 160), (40, 171))   # (texels from edge, alpha)
HOLO_SCAN = (0.80, 0.83, 0.96, 1.0, 0.99, 0.95)                                  # 6-row band, relative brightness


def holo_format(pst):
    """(kind, levels, w, h) of a PST: the hologram preview must equal the stock holo texture's."""
    import geo_pst
    t = geo_pst.parse(pst)
    return (t['kind'], len(t['levels']), t['levels'][-1]['w'], t['levels'][-1]['h'])


def course_view(res_bytes, code, at, side=384):
    """RGB chase view of an archive's own decoded geometry: camera 60 ft behind and 28 ft above AI-spline node
    round(at * nodes) of tracks/a<code>.spl, looking 400 ft ahead along the spline. -> (HxWx3 uint8, node)."""
    import numpy as np
    sys.path.insert(0, os.path.join(HERE, '..'))
    import softrender as SR
    import geo_pst
    import trk_verify
    r = trk_res3.read_res3(res_bytes)
    files = {n.lower(): d for n, d in r.items()}
    vis = trk_verify.load_visual(files)
    tex = {n.split(BS)[-1][:-4]: geo_pst.rgba_levels(geo_pst.parse(d))[-1]
           for n, d in files.items() if n.startswith('textures' + BS) and n.endswith('.pst')}
    names = sorted({m.lower() for _, m, _, _, _ in vis})
    textures = [tex.get(m, np.full((8, 8, 4), 200, np.uint8))[..., :3] for m in names]
    ti = {m: i for i, m in enumerate(names)}
    flip = np.array([1, 1, -1.0])            # left-handed game space -> the renderer's right-handed space
    P = np.vstack([v[2] for v in vis]) * flip
    base = np.cumsum([0] + [len(v[2]) for v in vis])[:-1]
    T = np.vstack([v[3] + b for v, b in zip(vis, base)])[:, [0, 2, 1]]
    UV = np.vstack([v[4] for v in vis])
    TT = np.concatenate([np.full(len(v[3]), ti[v[1].lower()]) for v in vis])
    _, _, SP = trk_verify.load_spline(files['tracks' + BS + 'a' + code + '.spl'])
    K = len(SP)
    j = int(round(at * K)) % K
    f = SP[(j + 3) % K] - SP[j]
    f[1] = 0
    f /= np.linalg.norm(f)
    eye = SP[j] - f * 60 + np.array([0, 28.0, 0])
    tgt = SP[j] + f * 400 + np.array([0, 10.0, 0])
    cam = dict(eye=eye * flip, target=tgt * flip, up=(0, 1, 0), fov=70, near=3.0)
    img, _, _ = SR.render(P, T, cam, side, side, uv=UV, tex_of_tri=TT, textures=textures, ambient=0.7, bg=(110, 140, 180))
    return img, j


def hologram_preview(res_bytes, code, at):
    """-> (PST bytes, RGBA preview image, provenance) for UI/TEXTURES/HOLO_<CODE>.PST."""
    import numpy as np
    from PIL import Image
    import geo_pst
    img, node = course_view(res_bytes, code, at)
    im = np.asarray(Image.fromarray(img).resize((HOLO_SIDE, HOLO_SIDE), Image.LANCZOS), float)
    lum = im @ np.array([0.299, 0.587, 0.114])
    lo, hi = np.percentile(lum, 2), np.percentile(lum, 98)
    l_ = np.clip((lum - lo) / max(hi - lo, 1.0), 0, 1)
    scan = np.array(HOLO_SCAN)[np.arange(HOLO_SIDE) % len(HOLO_SCAN)][:, None]
    b = np.clip((40 + 161 * l_) * scan, 0, 201)
    g = np.clip(0.67 * b - 10, 0, 255)
    r_ = np.clip(0.09 * b - 8, 0, 255)
    xs = np.arange(HOLO_SIDE)
    edge = np.minimum(xs, HOLO_SIDE - 1 - xs)
    d = np.minimum(edge[:, None], edge[None, :])
    alpha = np.interp(d, [p[0] for p in HOLO_ALPHA_EDGE], [p[1] for p in HOLO_ALPHA_EDGE])
    rgba = np.round(np.stack([r_, g, b, alpha], -1)).astype(np.uint8)
    pal = geo_pst.quantise(rgba)
    idx = geo_pst.nearest(rgba.reshape(-1, 4), pal)
    pst = geo_pst.write(dict(kind=1, palette=pal.tobytes(),
                             levels=[dict(w=HOLO_SIDE, h=HOLO_SIDE, data=geo_pst.PERM[idx.astype(np.uint8)].tobytes())]))
    back = geo_pst.rgba_levels(geo_pst.parse(pst))[-1]
    return pst, back, ('GENERATED: chase view of the archive\'s own decoded geometry at AI-spline node %d (at=%.2f), '
                       'hologram look measured on the stock holo textures; 256x256 kind 1 one level like holo_<code>.pst' % (node, at))
