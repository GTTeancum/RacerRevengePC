"""Converted engines with their flames (flames drawn flat orange), rear and side views, from the written RES."""
import sys,os,numpy as np
HERE=r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r'
sys.path.insert(0,HERE);sys.path.insert(0,os.path.dirname(HERE))
import geo_res,geo_psg,pod_psg,softrender as SR
from PIL import Image,ImageDraw
import pod_verify as V
SCR=r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade'
tile=300
rows=[]
for code,nn in [('ARK','04'),('BOL','15'),('EBE','23'),('ELA','24'),('NEV','11'),('RAT','17')]:
    m=geo_res.read_res(os.path.join(SCR,'export','a2r','pods','POD'+nn,'install','PODS','POD'+nn,'POD%s.RES'%nn))[0]
    P,T,UV,TT,texs=V.model_tris(m,nn,('englt1',))
    e=geo_psg.parse(m[r'pods\pod%s\englt1.psg'%nn]); W=pod_psg.world_matrices(e['nodes'])
    fl=np.zeros((4,4,4),np.uint8); fl[...,0]=255; fl[...,1]=140; fl[...,2]=20; fl[...,3]=255
    texs=list(texs)+[fl]; fi=len(texs)-1
    P=list(P); T=list(T); UV=list(UV); TT=list(TT)
    for s in geo_psg.vertices(e,0):
        if not s['material'].lower().startswith('flame'): continue
        base=len(P)
        for p,b in zip(s['pos'],s['bone']): P.append((np.r_[p,1.0]@W[b])[:3]); UV.append((0.5,0.5))
        for a,b,c in s['tris']: T.append((base+a,base+b,base+c)); TT.append(fi)
    rows.append((code,nn,np.array(P),np.array(T),np.array(UV),np.array(TT),texs))
views=[('rear',(0,0.15,-1)),('3/4 rear',(-0.7,0.4,-0.7)),('side',(-1,0.05,0))]
img=Image.new('RGB',(tile*len(views),len(rows)*(tile+16)),(18,18,24)); dr=ImageDraw.Draw(img)
for r,(code,nn,P,T,UV,TT,texs) in enumerate(rows):
    c=(P.max(0)+P.min(0))/2; rad=np.linalg.norm(P-c,axis=1).max()
    for col,(vn,d) in enumerate(views):
        d=np.array(d,float); d/=np.linalg.norm(d)
        cam=dict(ortho=True,center=c,size=2.1*rad,dir=-d,up=(0,1,0))
        im,_,_=SR.render(P,T,cam,W=tile,H=tile,uv=UV,tex_of_tri=TT,textures=texs,light=(0.4,0.8,-0.45),ambient=0.55,bg=(30,32,44))
        img.paste(Image.fromarray(im),(col*tile,r*(tile+16)+16)); dr.text((col*tile+4,r*(tile+16)+2),'%s POD%s left engine + flames (orange)  %s'%(code,nn,vn),fill=(255,220,90))
img.save(os.path.join(HERE,'scratch','fix13','flames_offline.png')); print('ok')
