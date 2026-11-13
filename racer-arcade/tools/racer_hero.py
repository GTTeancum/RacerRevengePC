r"""Presentation renders of arcade racers: full pod (3/4 front, perspective) and a cockpit + pilot
close-up, LOD0 pod with its slot-0 texture group and the pilot seated as in the game's placement.
    python -I racer_hero.py CODE [CODE ...]      -> export\pods\hero\<CODE>.png, sheet.png"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
import pod_export as PE, racer_assets as R, softrender as SR  # noqa: E402

OUT = os.path.join(PE.OUT, 'hero')
SS = 2  # supersampling


def shot(parts, d, W, H, fov, fill=1.12, bg=(26, 28, 38), focus=None):
    P, T, UV, TT, texs = PE.scene_tris(parts)
    F = PE.scene_tris(focus)[0] if focus else P
    c = (F.max(0) + F.min(0)) / 2
    r = np.linalg.norm(F - c, axis=1).max()
    d = np.asarray(d, float); d /= np.linalg.norm(d)
    dist = r * fill / np.tan(np.radians(fov) / 2)
    cam = dict(eye=c + d * dist, target=c, up=(0, 1, 0), fov=fov, near=dist * 0.01)
    img, _, _ = SR.render(P, T, cam, W * SS, H * SS, uv=UV, tex_of_tri=TT, textures=texs,
                          light=(0.4, 0.8, -0.45), ambient=0.5, bg=bg)
    return Image.fromarray(img).resize((W, H), Image.LANCZOS)


def hero(code):
    kind = R.CODES.index(code)
    pd = R.pod(kind)
    pl = PE.npc_placement(kind)
    body = PE.mrow(pl['body']['offset'], pl['body']['roll_deg'])
    tg0 = R.texgroup(pd['tg_slot0'][0])
    pod = PE.pod_parts(kind, 0, tg0)
    pilot, _, _ = PE.pilot_parts(kind, 1, body)
    full = shot(pod + pilot, (0.75, 0.42, -0.78), 1200, 700, 32, fill=1.45)
    cock = [q for q in pod if q['name'].startswith('cockpit')]
    close = shot(cock + pilot, (0.85, 0.45, -0.6), 600, 700, 30, fill=2.6, focus=pilot)
    out = Image.new('RGB', (1800, 740), (16, 16, 22))
    out.paste(full, (0, 40)); out.paste(close, (1200, 40))
    ImageDraw.Draw(out).text((12, 12), '%s  %s   (Racer Arcade pod LOD0 + pilot)' % (code, NAMES.get(code, '')), fill=(255, 220, 90))
    os.makedirs(OUT, exist_ok=True)
    out.save(os.path.join(OUT, '%s.png' % code))
    return out


NAMES = dict(ARK='Ark "Bumpy" Roose', BOL='Boles Roor', EBE='Ebe Endocott', ELA='Elan Mak',
             NEV='Neva Kee', RAT='Ratts Tyerell')

if __name__ == '__main__':
    codes = [c.upper() for c in sys.argv[1:]] or list(NAMES)
    ims = [hero(c) for c in codes]
    if len(ims) > 1:
        sh = Image.new('RGB', (1800, 740 * len(ims)))
        for i, im in enumerate(ims):
            sh.paste(im, (0, 740 * i))
        sh.resize((900, 370 * len(ims)), Image.LANCZOS).save(os.path.join(OUT, 'sheet.png'))
    print('written to', OUT)
