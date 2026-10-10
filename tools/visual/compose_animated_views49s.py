#!/usr/bin/env python3
"""Compose exact verified native frame pixels for inspection, without new draws."""
import hashlib
import json
from pathlib import Path
from PIL import Image, ImageDraw, PngImagePlugin, features
ROOT=Path(__file__).resolve().parents[2]
E=ROOT/'evidence/modelPalette49S/frames'
SOURCE=E/'pass-4'
OUT=E/'presentation'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def need(v,s):
    if not v:raise ValueError(s)
def load(name,raw=False):
    suffix='.raw.png' if raw else '.png'
    p=SOURCE/(name+suffix)
    report=json.loads((SOURCE/(name+'.json')).read_text())
    expected=next(i for i in report['images']if i['path'].endswith(suffix) and (not raw or i['kind']=='raw framebuffer'))
    # Raw is the first recorded image; choose the exact suffix/name when the
    # caption suffix also matches a raw filename.
    expected=next(i for i in report['images']if Path(i['path']).name==p.name)
    need(sha(p)==expected['sha256'],'Source PNG hash')
    with Image.open(p)as im: result=im.convert('RGBA')
    need(hashlib.sha256(result.tobytes()).hexdigest()==expected['rgba8_sha256'],'Source pixels')
    return p,result

def save_png(image,path,description):
    metadata=PngImagePlugin.PngInfo();metadata.add_text('Title','Recovered Pod01 engine animation diagnostic')
    metadata.add_text('Description',description)
    with path.open('xb')as f:image.save(f,format='PNG',pnginfo=metadata)
    with Image.open(path)as decoded:need(decoded.convert('RGBA').tobytes()==image.tobytes(),'Composed PNG pixels changed')
    return {'path':str(path.relative_to(ROOT)),'bytes':path.stat().st_size,'sha256':sha(path),
            'rgba8_sha256':hashlib.sha256(image.tobytes()).hexdigest(),'dimensions':list(image.size)}

def main():
    need(not OUT.exists(),'Use a new presentation directory');OUT.mkdir()
    loaded=[load(f'frame-{f:03d}-full-motion')for f in(0,31,63,95)]
    sheet=Image.new('RGBA',(1280,1144),(18,22,30,255));draw=ImageDraw.Draw(sheet)
    draw.text((10,6),'Recovered Pod01 engine animation: four native samples',fill='white')
    draw.text((10,22),'Supplied stationary pose and static Chariot. Diagnostic materials; no spawn, IK or gameplay/FPS claim.',fill='white')
    for i,(_,im)in enumerate(loaded):
        need(im.size==(640,552),'Captioned frame extent');xy=((i%2)*640,40+(i//2)*552);sheet.paste(im,xy)
        need(sheet.crop((*xy,xy[0]+640,xy[1]+552)).tobytes()==im.tobytes(),'Tile pixels changed')
    results=[save_png(sheet,OUT/'pod01-four-native-frames.png','Exact unscaled captioned native frames0,31,63,95. Supplied stationary pose/current visual state; static Chariot and diagnostic materials.')]
    crop_box=(128,120,448,340)
    control_names=['frame-095-full-motion','frame-095-frozen-nodes','frame-095-frozen-roots']
    controls=[load(name,True)for name in control_names]
    strip=Image.new('RGBA',(1920,494),(18,22,30,255));draw=ImageDraw.Draw(strip)
    labels=['Frame95: complete engine node/root output','Control: engine node locals frozen at frame0','Control: engine roots frozen at frame0']
    for i,((_,im),label)in enumerate(zip(controls,labels)):
        cropped=im.crop(crop_box).resize((640,440),Image.Resampling.NEAREST)
        strip.paste(cropped,(640*i,54));draw.text((640*i+8,6),label,fill='white')
        draw.text((640*i+8,22),'2x nearest crop of the same fixed-camera framebuffer',fill='white')
        draw.text((640*i+8,38),'Supplied pose; static Chariot; diagnostic material profile',fill='white')
        need(strip.crop((640*i,54,640*(i+1),494)).tobytes()==cropped.tobytes(),'Control tile changed')
    results.append(save_png(strip,OUT/'pod01-motion-controls.png','Explicit counterfactual controls. Crop128,120,448,340 enlarged2x by nearest pixels; original camera and raw framebuffer remain unchanged.'))
    animation=None
    if features.check('webp'):
        path=OUT/'pod01-four-sample-loop.webp';frames=[im for _,im in loaded]
        need(not path.exists(),'No animation overwrite')
        frames[0].save(path,format='WEBP',save_all=True,append_images=frames[1:],duration=600,loop=0,lossless=True,method=6)
        with Image.open(path)as decoded:
            need(decoded.n_frames==4,'Animation frame count')
            for i,im in enumerate(frames):decoded.seek(i);need(decoded.convert('RGBA').tobytes()==im.tobytes(),'Lossless animation pixels changed')
        animation={'path':str(path.relative_to(ROOT)),'bytes':path.stat().st_size,'sha256':sha(path),'frames':4,
                   'presentation_frame_duration_ms':600,'loop':True,'native_frame_numbers':[0,31,63,95],
                   'scope':'Four exact sampled native images presented as a loop; not realtime performance or a game frame rate.'}
    report={'status':'passed','source_pass':'pass-4','source_pngs':{str(p.relative_to(ROOT)):sha(p)for p,_ in loaded+controls},
            'images':results,'animation':animation,'control_crop_box':list(crop_box),'draws_performed':0,
            'pixel_preservation':'Every source PNG and every unscaled/cropped tile checked; every decoded animation frame exactly matches its source RGBA.'}
    (OUT/'presentation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
if __name__=='__main__':main()
