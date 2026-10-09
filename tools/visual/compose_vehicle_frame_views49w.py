#!/usr/bin/env python3
"""Compose pinned 49W pixels without extra draws, camera changes or pose edits."""
import hashlib
import json
from pathlib import Path
from PIL import Image, ImageDraw, PngImagePlugin, features
ROOT = Path(__file__).resolve().parents[2]
E = ROOT / 'evidence/frameBridge49W'
SOURCE = E / 'views/pass1'
OUT = E / 'presentation-final'
FRAMES = (0, 31, 95, 143)
BACKGROUND = (18, 22, 30, 255)
LIMITS = ('Supplied initial pose/velocities/history and no-hit query; support is synthetic354 counter-gravity. '
          'Native prephysics graphics roots; palettes evaluated on a disposable copy. '
          'No authentic spawn/contact producer, complete original frame/gameplay, lighting or FPS claim.')
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def need(v, s):
    if not v: raise ValueError(s)
def load(name, raw=False):
    p = SOURCE / (name + ('.raw.png' if raw else '.png'))
    report = json.loads((SOURCE / (name + '.json')).read_text())
    expected = next(i for i in report['images'] if Path(i['path']).name == p.name)
    need(sha(p) == expected['sha256'], 'Source PNG hash differs')
    with Image.open(p) as image: result = image.convert('RGBA')
    need(hashlib.sha256(result.tobytes()).hexdigest() == expected['rgba8_sha256'], 'Source pixels differ')
    return p, result

def text_lines(image, lines, xy=(10, 6), spacing=16):
    draw = ImageDraw.Draw(image)
    for i, line in enumerate(lines): draw.text((xy[0], xy[1]+i*spacing), line, fill='white')

def paste_exact(sheet, tile, xy):
    sheet.paste(tile, xy)
    need(sheet.crop((*xy, xy[0]+tile.width, xy[1]+tile.height)).tobytes() == tile.tobytes(), 'Tile pixels changed')

def save_png(image, path, description):
    metadata = PngImagePlugin.PngInfo()
    metadata.add_text('Title', '49W source-ordered native advancing-frame diagnostic')
    metadata.add_text('Description', description + ' ' + LIMITS)
    with path.open('xb') as handle: image.save(handle, format='PNG', pnginfo=metadata)
    with Image.open(path) as decoded:
        need(decoded.convert('RGBA').tobytes() == image.tobytes(), 'Composed PNG pixels changed')
    return {'path': str(path.relative_to(ROOT)), 'bytes': path.stat().st_size, 'sha256': sha(path),
            'rgba8_sha256': hashlib.sha256(image.tobytes()).hexdigest(), 'dimensions': list(image.size)}

def main():
    need(not OUT.exists(), 'Use a new presentation directory')
    OUT.mkdir()
    loaded = {scenario: [load(f'{scenario}-frame{f:03d}-full-motion') for f in FRAMES]
              for scenario in ('support', 'no-support')}
    pairs = []
    for i, frame in enumerate(FRAMES):
        pair = Image.new('RGBA', (1280, 632), BACKGROUND)
        text_lines(pair, ['Native advancing frame: synthetic support vs no-support fallthrough',
                          f'Frame {frame} | same initial state, controls and timing; same fixed49H camera and depth-tested scene',
                          'No-support disappears below the authored track under the supplied no-hit query. Diagnostic materials.'])
        for j, scenario in enumerate(('support', 'no-support')):
            im = loaded[scenario][i][1]
            need(im.size == (640, 568), 'Captioned frame extent differs')
            paste_exact(pair, im, (640*j, 64))
        pairs.append(pair)
    sheet = Image.new('RGBA', (1280, 632*4), BACKGROUND)
    for i, pair in enumerate(pairs): paste_exact(sheet, pair, (0, i*632))
    images = [save_png(sheet, OUT/'pod01-support-vs-no-support-four-frames.png',
                       'Exact unscaled paired frame pixels at0,31,95,143; left synthetic support, right no-support. Late support views are distant; no-support is not visible from frame31.')]
    images.append(save_png(pairs[1], OUT/'pod01-frame31-support-vs-no-support.png',
                           'Exact unscaled frame31 comparison of synthetic support and no-support fallthrough.'))
    support_sheet = Image.new('RGBA', (1280, 1176), BACKGROUND)
    text_lines(support_sheet, ['Synthetic support: four native advancing samples, fixed49H camera',
                               'Original native mode progression; supplied counter-gravity force is not terrain contact.'])
    for i, (_, im) in enumerate(loaded['support']): paste_exact(support_sheet, im, ((i%2)*640, 40+(i//2)*568))
    images.append(save_png(support_sheet, OUT/'pod01-supported-four-native-frames.png',
                           'Four unscaled synthetic-support frames, no camera fitting or pose changes.'))
    # Same crop for all controls. Nearest pixels only; original raw images remain intact.
    crop_box = (128, 96, 544, 384)
    control_names = ['support-frame031-full-motion', 'support-frame031-physical-copy-only',
                     'support-frame031-frozen-roots', 'no-support-frame031-full-motion']
    controls = [load(name, True) for name in control_names]
    labels = [
        ('Native frame31: synthetic support', 'Actual prephysics graphics roots; advancing native state'),
        ('Display control: physical-copy-only', 'Chariot root replaced; both engine components unchanged'),
        ('Display control: frozen-roots', 'All three component roots frozen; current internal motion'),
        ('NO-SUPPORT FALLTHROUGH: frame31', 'No support force; below-track vehicle is not visible')]
    strip = Image.new('RGBA', (1664, 1296), BACKGROUND)
    for i, ((_, im), lines) in enumerate(zip(controls, labels)):
        x, y = (i%2)*832, (i//2)*648
        text_lines(strip, [*lines, '2x nearest crop of identical49H camera; diagnostic materials.',
                           'Supplied inputs and no-hit query; no authentic contact/gameplay/FPS.'], (x+8, y+5))
        crop = im.crop(crop_box).resize((832, 576), Image.Resampling.NEAREST)
        paste_exact(strip, crop, (x, y+72))
    images.append(save_png(strip, OUT/'pod01-frame31-controls-and-fallthrough.png',
                           'Frame31 controls and no-support, each exact crop128,96,544,384 enlarged2x nearest.'))
    animation = None
    need(features.check('webp'), 'Installed Pillow requires WebP for the requested exact four-sample loop')
    path = OUT / 'pod01-support-vs-no-support-four-sample-loop.webp'
    pairs[0].save(path, format='WEBP', save_all=True, append_images=pairs[1:], duration=900,
                  loop=0, lossless=True, method=6)
    with Image.open(path) as decoded:
        need(decoded.n_frames == 4, 'Animation frame count differs')
        for i, im in enumerate(pairs):
            decoded.seek(i)
            need(decoded.convert('RGBA').tobytes() == im.tobytes(), 'Lossless animation pixels changed')
    animation = {'path':str(path.relative_to(ROOT)), 'bytes':path.stat().st_size, 'sha256':sha(path),
                 'frames':4, 'presentation_frame_duration_ms':900, 'loop':True, 'native_frame_numbers':list(FRAMES),
                 'dimensions':[1280,632], 'decoded_frame_rgba8_sha256':[hashlib.sha256(im.tobytes()).hexdigest() for im in pairs],
                 'scope':'Four exact sampled image pairs; presentation timing is not native realtime, gameplay or FPS.'}
    sources = [item for group in loaded.values() for item in group] + controls
    report = {'schema':'advancing49w.presentation.v1', 'status':'passed', 'source_pass':'pass1',
              'source_pngs':{str(p.relative_to(ROOT)):sha(p) for p,_ in sources}, 'images':images,
              'animation':animation, 'control_crop_box':list(crop_box), 'draws_performed':0,
              'camera_changed':False, 'poses_changed':False,
              'visibility_limit':'With the preserved49H camera and depth-tested TA, no-support frames31/95/143 have identical empty-track pixels. Supported95/143 are distant but visible.',
              'limits':LIMITS,
              'pixel_preservation':'Every source PNG and composed tile checked; every decoded WebP frame equals the source image pair exactly.'}
    (OUT/'presentation.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'status':report['status'], 'images':images, 'animation':animation}))
if __name__ == '__main__': main()
