#!/usr/bin/env python3
"""Exact nearest-pixel closeups of the independently checked Chariot controls."""
import hashlib
import json
from pathlib import Path
from PIL import Image, ImageDraw
import compose_chariot_views49t as base

def main():
    names=['frame-095-full-motion','frame-095-frozen-chariot-locals','frame-095-ik-omitted']
    labels=['Original motion and both hand IK chains','Control: Chariot locals frozen at frame0','Control: six source IK joint updates omitted']
    box=(328,240,440,312)
    sheet=Image.new('RGBA',(1344,342),(18,22,30,255));draw=ImageDraw.Draw(sheet);pins={}
    for i,(name,label)in enumerate(zip(names,labels)):
        p,im=base.load(name,True);pins[str(p.relative_to(base.ROOT))]=base.sha(p)
        crop=im.crop(box).resize((448,288),Image.Resampling.NEAREST);sheet.paste(crop,(i*448,54))
        draw.text((i*448+8,6),label,fill='white')
        draw.text((i*448+8,22),'4x nearest pixels; same frame95, camera and root',fill='white')
        draw.text((i*448+8,38),'Supplied root/body inputs; diagnostic materials',fill='white')
        base.need(sheet.crop((i*448,54,(i+1)*448,342)).tobytes()==crop.tobytes(),'Detail tile changed')
    row=base.save_png(sheet,base.OUT/'pod01-chariot-ik-closeup.png','Exact crop328,240,440,312 enlarged4x by nearest pixels from frame95. Full original internal animation and hand IK compared with frozen-Chariot and omitted-IK display controls. Supplied root/body scene; diagnostic materials.')
    report={'status':'passed','source_pngs':pins,'crop_box':list(box),'nearest_integer_scale':4,'draws_performed':0,'images':[row]}
    p=base.OUT/'detail-presentation.json'
    with p.open('x')as f:f.write(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report))
if __name__=='__main__':main()
