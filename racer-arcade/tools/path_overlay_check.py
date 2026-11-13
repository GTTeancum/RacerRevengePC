# Overlay decoded centre lines on collision (top view) for courses 1 and 2.
import numpy as np, os, re
from PIL import Image, ImageDraw
B=os.path.join(os.path.dirname(__file__),'..'); R=B+'/img'
mask=np.memmap(R+'/mask.bin',dtype='<u4',mode='r'); ep=np.memmap(R+'/eprom.bin',dtype='<u4',mode='r')
txt=open(B+'/notes/race-paths.txt').read().split('## ')
cent={}
for s in txt:
    m=re.match(r'centre (\d)',s)
    if m: cent[int(m.group(1))]=np.array([[float(a),float(b)] for a,b in re.findall(r'^\s*\d+\s+(\S+)\s+(\S+)\s*$',s,re.M)])
C=[(1,ep,0xC80000,0x131080),(2,mask,0x0E620120,0x16A5A0)]
S=600; out=Image.new('RGB',(2*S+6,S),'gray')
for k,(c,d,a,sz) in enumerate(C):
    n=(sz-0x20)//196; v=np.asarray(d[a//4:a//4+n*49]).reshape(n,49).view('<f4')[:,4:13].reshape(-1,3)
    v=v[np.isfinite(v).all(1)&(np.abs(v)<1e5).all(1)][:,[0,2]]; p=cent[c]
    allp=np.vstack([v,p]); lo=allp.min(0); sc=(S-20)/(allp.max(0)-lo).max()
    im=Image.new('RGB',(S,S),'white'); dr=ImageDraw.Draw(im); px=im.load()
    for x,z in ((v-lo)*sc+10).astype(int): px[int(x),int(z)]=(170,170,170)
    q=[tuple(t) for t in ((p-lo)*sc+10)]; dr.line(q+[q[0]],fill=(220,0,0),width=2); dr.ellipse([q[0][0]-5,q[0][1]-5,q[0][0]+5,q[0][1]+5],outline='blue',width=2)
    dr.text((5,5),'course %d: grey=collision red=centre line blue=node 0'%c,fill='black'); out.paste(im,(k*(S+6),0))
out.save(B+'/notes/mask-images/centreline_overlay.png')
