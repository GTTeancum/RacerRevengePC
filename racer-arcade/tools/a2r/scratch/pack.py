import sys,collections,glob,os;sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
from geo_res import read_res
import geo_psg
def strips_of(s):
    """source strips as lists of vertex tuples, merging 16-vertex continuations"""
    out=[]; prev_full=False
    for gi,g in enumerate(s['groups']):
        verts=list(zip(g['xyzw'],g['normal'],g['uv'],g['rgba'] or [None]*len(g['xyzw'])))
        i=0; n=len(verts)
        while i<n:
            j=i+2
            while j<n and verts[j][0][3]>0: j+=1
            st=verts[i:j]
            if i==0 and out and prev_full and [(v[0][:3],)+v[1:] for v in out[-1][-2:]]==[(v[0][:3],)+v[1:] for v in st[:2]] and len(out[-1])>=16 :
                out[-1]=out[-1]+st[2:]
            else: out.append(st)
            prev_full = (i==0 and j==n and n==16) or (j==n and j-i==16 and i==0)
            prev_full = (j==n and n==16 and i==0) or (j==n and j-i==16)
            i=j
    return out
def pack(strips, cap=16):
    groups=[]; cur=[]
    for st in strips:
        st=list(st)
        while True:
            room=cap-len(cur)
            if len(st)<=room:
                cur+=st; break
            if cur:  # does not fit: close group
                groups.append(cur); cur=[]; continue
            # longer than a whole group: split with 2-vertex overlap
            head=st[:cap]; groups.append(head)
            st=[(v[0][:3]+(-abs(v[0][3]),),)+v[1:] for v in st[cap-2:cap]]+st[cap:]
    if cur: groups.append(cur)
    return groups
G=r'C:\Games\Star Wars Racer Revenge'
files=[os.path.join(G,'TRACKS',t+'.RES') for t in sys.argv[1:]] or [os.path.join(G,'TRACKS','TA.RES'),os.path.join(G,'PODS','POD01','POD01.RES')]
C=collections.Counter(); bad=[]
for f in files:
    m,o,i=read_res(f)
    for name,b in m.items():
        if not name.endswith('.psg'): continue
        M=geo_psg.parse(b)
        for lod in M['lods']:
            for s in lod['surfaces']:
                st=strips_of(s)
                C['src_ok' if len(st)==s['strip_count'] else 'src_bad']+=1
                gs=pack(st)
                orig=[list(zip(g['xyzw'],g['normal'],g['uv'],g['rgba'] or [None]*len(g['xyzw']))) for g in s['groups']]
                ok = gs==orig
                C['pack_ok' if ok else 'pack_bad']+=1
                if not ok and len(bad)<5: bad.append((name,[len(x) for x in orig][:12],[len(x) for x in gs][:12],[len(x) for x in st][:12]))
print(C); [print(b) for b in bad]
