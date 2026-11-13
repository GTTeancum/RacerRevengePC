import sys, os, numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')); import trk_res3, trk_col
r=trk_res3.read_res3(open(sys.argv[1],'rb').read()); per={}
for n,lo,sz in r.members:
    if not(n.lower().startswith('geometry') and n.lower().endswith('.col')): continue
    b=trk_col.read_col(r.payload[lo:lo+sz]).body; y=b.faces['normal'][:,1]
    k='wall' if 'wall' in n else 'floor'
    a=per.setdefault(k,[0,0]); a[0]+=int((y>0.5).sum()); a[1]+=int((y<-0.5).sum())
print('up/down faces by kind', per)
