"""Render each converted engine (engine 0, LOD 0, arcade geometry) from rear / side / 3-4 rear."""
import sys,os,numpy as np
HERE=r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r'
sys.path.insert(0,HERE);sys.path.insert(0,os.path.dirname(HERE))
import pod_convert as C, pod_export as PE, softrender as SR
from PIL import Image, ImageDraw
OUTD=os.path.join(HERE,'scratch','fix13')
codes=sys.argv[1:] or list(C.TARGETS)
tile=360
for code in codes:
    p=C.Pod(code,C.TARGETS[code]); p.gather()
    tris=p.lods[0]['engines'][0]
    P=np.vstack([t['P'] for t in tris]); T=np.arange(len(P)).reshape(-1,3)
    texs=[];tix={};UV=[];TT=[]
    for t in tris:
        k=t['key']
        if k not in tix:
            if k[0]=='t': img,_=PE.texture_rgba(k[1]); img=img.copy(); img[...,3]=255
            else: img=np.zeros((8,8,4),np.uint8); img[...,:3]=k[1]; img[...,3]=255
            tix[k]=len(texs); texs.append(img)
        UV+=list(t['UV']); TT.append(tix[k])
    UV=np.array(UV); TT=np.array(TT)
    c=(P.max(0)+P.min(0))/2; r=np.linalg.norm(P-c,axis=1).max()
    views=[('rear (from -z)',(0,0,-1)),('side',(-1,0,0)),('3/4 rear',(-0.6,0.4,-0.7)),('top',(0,1,0.001))]
    img=Image.new('RGB',(tile*len(views),tile+20),(18,18,24)); dr=ImageDraw.Draw(img)
    dr.text((4,2),'%s engine 0 (forward +z), box %s..%s'%(code,P.min(0).round(1),P.max(0).round(1)),fill=(255,220,90))
    for col,(vn,d) in enumerate(views):
        d=np.array(d,float); d/=np.linalg.norm(d)
        cam=dict(ortho=True,center=c,size=2.1*r,dir=-d,up=(0,0,1) if vn=='top' else (0,1,0))
        im,_,_=SR.render(P,T,cam,W=tile,H=tile,uv=UV,tex_of_tri=TT,textures=texs,light=(0.4,0.8,-0.45),ambient=0.5,bg=(30,32,44))
        img.paste(Image.fromarray(im),(col*tile,20)); dr.text((col*tile+4,22),vn,fill=(255,255,255))
    img.save(os.path.join(OUTD,'eng_%s.png'%code))
    print(code,'ok')
