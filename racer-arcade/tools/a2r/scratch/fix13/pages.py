"""Before/after texture pages: old = silo\assets (previous conversion), new = export (this conversion)."""
import sys,os,numpy as np
HERE=r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r'
sys.path.insert(0,HERE)
import geo_res,geo_pst
from PIL import Image, ImageDraw
SCR=r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade'
OUTD=os.path.join(HERE,'scratch','fix13')
rows=[]
for nn,pages in [('04',['3']),('15',['3']),('23',['3']),('11',['3']),('17',['3','1'])]:
    old=geo_res.read_res(os.path.join(SCR,'silo','assets','PODS','POD'+nn,'POD%s.RES'%nn))[0]
    new=geo_res.read_res(os.path.join(SCR,'export','a2r','pods','POD'+nn,'install','PODS','POD'+nn,'POD%s.RES'%nn))[0]
    for pg in pages:
        k='textures\%s_%s.pst'%(nn,pg)
        a=geo_pst.rgba_levels(geo_pst.parse(old[k]))[-1][...,:3]; b=geo_pst.rgba_levels(geo_pst.parse(new[k]))[-1][...,:3]
        rows.append(('POD%s %s_%s'%(nn,nn,pg),a,b))
S=2
W=sum(max(a.shape[1],b.shape[1])*S+20 for _,a,b in rows); H=max(a.shape[0]+b.shape[0] for _,a,b in rows)*S+80
img=Image.new('RGB',(W,H),(20,20,26)); dr=ImageDraw.Draw(img); x=0
for name,a,b in rows:
    dr.text((x+2,2),name,fill=(255,220,90))
    dr.text((x+2,16),'before %dx%d'%(a.shape[1],a.shape[0]),fill=(255,255,255))
    img.paste(Image.fromarray(a).resize((a.shape[1]*S,a.shape[0]*S),Image.NEAREST),(x,30))
    y=40+a.shape[0]*S
    dr.text((x+2,y),'after %dx%d'%(b.shape[1],b.shape[0]),fill=(255,255,255))
    img.paste(Image.fromarray(b).resize((b.shape[1]*S,b.shape[0]*S),Image.NEAREST),(x,y+14))
    x+=max(a.shape[1],b.shape[1])*S+20
img.save(os.path.join(OUTD,'pages_before_after.png')); print(img.size)
