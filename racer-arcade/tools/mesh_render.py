# Quick visual check: decode Hikaru static-vertex mesh runs and render flat-shaded views + OBJ.
import sys, struct, math, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hikaru_gpu as H
from PIL import Image, ImageDraw
def s32(v): return v-(1<<32) if v&0x80000000 else v
def extract(buf, start, end):
    prec=1.0; V=[]; T=[]; ring=[]; off=start; pivot=None
    while off+4<=end:
        w=struct.unpack_from('<I',buf,off)[0]; nm=H.name_of(w)
        if nm is None or w==0: break
        sz=H.insn_size(w); words=struct.unpack_from('<%dI'%(sz//4),buf,off); op=w&0x1FF; full=w&0xFFF
        if full==0x901: prec=2.0**(((w>>16)&0xFF)-0x8D)
        if op in range(0x12C,0x130) or op in range(0x1AC,0x1B0) or op in range(0x1B8,0x1C0):
            if op<0x130: p=[(s32(x)>>10)*prec/64 for x in words[1:4]]
            else: p=[struct.unpack('<f',struct.pack('<I',x))[0] for x in words[1:4]]
            V.append(p); idx=len(V)-1
            if w&2: pivot=None
            ring.append(idx); ring=ring[-3:]
            if w&2: pivot=ring[-2] if len(ring)>=2 else None
            if ((w>>9)&7)==7 and len(ring)==3:
                a,b,c=ring
                if pivot is not None: a=pivot
                T.append((a,b,c) if not (w&1) else (a,c,b))
        elif op in (0x158,0x159,0x15A,0x15B):
            if ((w>>9)&7)==7 and len(ring)==3: T.append(tuple(ring))
        off+=sz
        if op in (0x082,0x1C2) and ((w>>12)&0xF)==0: break
    return V,T,off
def render(V,T,path,yaw=0.6,pitch=0.4,S=512):
    import numpy as np
    P=np.array(V); c=(P.max(0)+P.min(0))/2; r=np.abs(P-c).max() or 1; P=(P-c)/r
    cy,sy,cp,sp=math.cos(yaw),math.sin(yaw),math.cos(pitch),math.sin(pitch)
    x=P[:,0]*cy+P[:,2]*sy; z=-P[:,0]*sy+P[:,2]*cy; y=P[:,1]*cp-z*sp; z=P[:,1]*sp+z*cp
    img=Image.new('RGB',(S,S),(30,30,40)); d=ImageDraw.Draw(img); L=np.array([0.3,0.5,-0.8]); L/=np.linalg.norm(L)
    Q=np.stack([x,y,z],1); order=sorted(range(len(T)),key=lambda i:-sum(Q[j,2] for j in T[i]))
    for i in order:
        a,b,cc=[Q[j] for j in T[i]]; n=np.cross(b-a,cc-a); nn=np.linalg.norm(n)
        k=abs(n@L)/nn if nn else 0.5; g=int(60+180*k)
        d.polygon([(S/2+q[0]*S*0.45,S/2-q[1]*S*0.45) for q in (a,b,cc)],fill=(g,g,int(g*0.9)))
    img.save(path)
if __name__=='__main__':
    f,start,end,out=sys.argv[1],int(sys.argv[2],16),int(sys.argv[3],16),sys.argv[4]
    buf=open(f,'rb').read()
    AV=[];AT=[];off=start;n=0
    while off<end:
        V,T,o2=extract(buf,off,end)
        if V:
            b=len(AV); AV+=V; AT+=[(a+b,c+b,d+b) for a,c,d in T]; n+=1
        off=o2+4 if o2<=off else o2
    with open(out+'.obj','w') as fo:
        for v in AV: fo.write('v %f %f %f\n'%tuple(v))
        for t in AT: fo.write('f %d %d %d\n'%(t[0]+1,t[1]+1,t[2]+1))
    print(n,'runs',len(AV),'verts',len(AT),'tris')
    render(AV,AT,out+'_a.png'); render(AV,AT,out+'_b.png',yaw=2.2,pitch=-0.3)
