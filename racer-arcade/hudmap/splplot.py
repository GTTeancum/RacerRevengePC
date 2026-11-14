import sys,re
sys.path.insert(0,r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/tools/a2r')
import trk_res3
from PIL import Image,ImageDraw
def nodes(path):
    r=trk_res3.read_res3(open(path,'rb').read()); t=r.get(r'tracks\msc.spl').decode('latin1')
    out={}
    for m in re.finditer(r'\[Node(\d+)\](.*?)(?=\[Node|\Z)',t,re.S):
        i=int(m.group(1)); b=m.group(2)
        p=[float(v) for v in re.search(r'Position=([^\r\n]+)',b).group(1).split(',')]
        nx=[int(v) for v in re.findall(r'Next\d+=(\d+)',b)]
        fl=re.search(r'SecretBranch=(\d+)',b)
        out[i]=(p,nx)
    return out
srcs={'orig':r'C:/Games/Star Wars Racer Revenge/TRACKS/SC.RES','eb':r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/silo/assets/TRACKS/SC.RES','lc':r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/silo/assets-lc/TRACKS/SC.RES'}
allp={k:nodes(v) for k,v in srcs.items()}
xs=[p[0] for d in allp.values() for p,_ in d.values()]; zs=[p[2] for d in allp.values() for p,_ in d.values()]
x0,x1,z0,z1=min(xs),max(xs),min(zs),max(zs); S=900/max(x1-x0,z1-z0)
im=Image.new('RGB',(940,940),'white'); d=ImageDraw.Draw(im)
col={'orig':'red','eb':'blue','lc':'green'}
for k,n in allp.items():
    for i,(p,nx) in n.items():
        for j in nx:
            q=n[j][0]; d.line([(20+(p[0]-x0)*S,920-(p[2]-z0)*S),(20+(q[0]-x0)*S,920-(q[2]-z0)*S)],fill=col[k],width=2)
    p=n[0][0]; d.ellipse([20+(p[0]-x0)*S-5,920-(p[2]-z0)*S-5,20+(p[0]-x0)*S+5,920-(p[2]-z0)*S+5],outline=col[k])
    print(k,len(n),n[0][0])
im.save('splines_world.png')
