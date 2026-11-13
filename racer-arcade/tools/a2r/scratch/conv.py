import sys,collections,math;sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
from geo_res import read_res
import geo_psg
m,o,i=read_res(r'C:\Games\Star Wars Racer Revenge\TRACKS\%s.RES'%sys.argv[1])
gs=collections.Counter(); W=collections.Counter(); cont=0; n=0
rows=[]
for name,b in m.items():
    if not name.endswith('.psg'): continue
    M=geo_psg.parse(b); n+=1
    q=M['quant']; W[(M['world'],len(M['nodes']),len(M['lods']),len(M['materials']),len(M['lods'][0]['surfaces']))]+=1
    allx=[];
    for s in M['lods'][0]['surfaces']:
        for g in s['groups']:
            gs[len(g['xyzw'])]+=1
            allx+= [max(abs(v[0]),abs(v[1]),abs(v[2])) for v in g['xyzw']]
    nd=M['nodes'][0]
    if len(rows)<12 or name.startswith('geometry\zone01ground'):
        s=M['lods'][0]['surfaces'][0]
        uvmax=max(max(abs(u[0]),abs(u[1])) for g in s['groups'] for u in g['uv'])
        nmax=max(max(abs(k) for k in v) for g in s['groups'] for v in g['normal'])
        rows.append((name,M['world'],'q=%s'%[round(x,6) for x in q],'maxint',max(allx),'half',[round(x,2) for x in nd['half']],'r',round(nd['radius'],2),'c',[round(x,1) for x in nd['centre']],'uvsize',s['uv_size'],'uvmax',uvmax,'nmax',nmax,'cut',M['lods'][0]['cutoff'],'comp','%08x'%s['component'],'nst',s['strip_count'],'nv',s['vertex_count']))
print(n, W.most_common(10)); print(sorted(gs.items()))
for r in rows[:30]: print(r)
