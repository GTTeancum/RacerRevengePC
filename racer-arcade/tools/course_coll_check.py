# Top views of the 4 race courses' collision, from the addresses the course loader uses.
import numpy as np, os
from PIL import Image, ImageDraw
R=os.path.join(os.path.dirname(__file__),'..','img')
mask=np.memmap(os.path.join(R,'mask.bin'),dtype='<u4',mode='r'); ep=np.memmap(os.path.join(R,'eprom.bin'),dtype='<u4',mode='r')
C=[('0 SA_COURSE2',mask,0x0E000000,0x18EAA0),('1 ETTI4 (eprom)',ep,0xC80000,0x131080),('2 MALASTARE',mask,0x0E620120,0x16A5A0),('3 SA_COURSE4',mask,0x0E18EAA0,0x3154C0)]
S=400; tiles=[]
for name,d,a,sz in C:
    n=(sz-0x20)//196; f=np.asarray(d[a//4:a//4+n*49]).reshape(n,49).view('<f4'); v=f[:,4:13].reshape(-1,3)
    v=v[np.isfinite(v).all(1)&(np.abs(v)<1e5).all(1)]
    lo,hi=v[:,[0,2]].min(0),v[:,[0,2]].max(0); sc=(S-21)/max((hi-lo).max(),1); q=((v[:,[0,2]]-lo)*sc).astype(int)+10
    im=Image.new('RGB',(S,S),'white'); px=im.load()
    for x,z in q: px[int(x),int(z)]=(150,40,40)
    ImageDraw.Draw(im).text((5,5),'%s  %d tris'%(name,n),fill='black'); tiles.append(im)
out=Image.new('RGB',(S*4+18,S),'gray')
for i,t in enumerate(tiles): out.paste(t,(i*(S+6),0))
out.save(os.path.join(os.path.dirname(__file__),'..','notes','mask-images','courses_by_loader.png'))
