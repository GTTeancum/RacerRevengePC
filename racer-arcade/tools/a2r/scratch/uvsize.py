import sys,collections,math,os,struct;sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
from geo_res import read_res
import geo_psg
G=r'C:\Games\Star Wars Racer Revenge'
m,o,i=read_res(os.path.join(G,'TRACKS',sys.argv[1]+'.RES'))
qs=collections.Counter(); k=0
for name,b in m.items():
    if not name.endswith('.psg'): continue
    M=geo_psg.parse(b); q=M['quant']
    qs[tuple(struct.pack('<f',x).hex() for x in q[1:4])]+=1
    sv=geo_psg.vertices(M)
    for s,sd in zip(sv,M['lods'][0]['surfaces']):
        L3=L2=0; A3=A2=0; tex=[]; mx=0; me=0; mpos=0
        for a,b_,c in s['tris']:
            P=[s['pos'][x] for x in (a,b_,c)]; U=[s['uv'][x] for x in (a,b_,c)]
            u=[P[1][j]-P[0][j] for j in range(3)]; v=[P[2][j]-P[0][j] for j in range(3)]
            cr=(u[1]*v[2]-u[2]*v[1],u[2]*v[0]-u[0]*v[2],u[0]*v[1]-u[1]*v[0]); A3+=math.sqrt(sum(x*x for x in cr))/2
            a3=math.sqrt(sum(x*x for x in cr))/2; a2=abs((U[1][0]-U[0][0])*(U[2][1]-U[0][1])-(U[2][0]-U[0][0])*(U[1][1]-U[0][1]))/2
            if a2>0: mx=max(mx,math.sqrt(a3/a2))
            for e in ((0,1),(1,2),(2,0)):
                l3=math.dist(P[e[0]],P[e[1]]); l2=math.dist(U[e[0]],U[e[1]])
                if l2>0: me=max(me,l3/l2)
                mpos=max(mpos,l3)
            A2+=abs((U[1][0]-U[0][0])*(U[2][1]-U[0][1])-(U[2][0]-U[0][0])*(U[1][1]-U[0][1]))/2
        # max extent
        ext=max(max(p[j] for p in s['pos'])-min(p[j] for p in s['pos']) for j in range(3))
        if k<25: print(name, s['material'], 'uvsize %.2f'%sd['uv_size'], 'sqrt(A3/A2) %.2f'%(math.sqrt(A3/A2) if A2 else -1), 'A3 %.1f A2 %.3f'%(A3,A2), 'sqrtA3 %.2f'%math.sqrt(A3), 'ext %.1f'%ext, 'nt',len(s['tris']), 'maxtri %.2f maxedge %.2f maxlen %.2f'%(mx,me,mpos))
        k+=1
print(qs)
