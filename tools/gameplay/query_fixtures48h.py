"""Generate differential fixtures by executing immutable original leaf words."""
import itertools
import random
from query_extract48h import RAM, digest
from query_oracle48h import *

EMPTY=bytes(0x2000000)
WORDS={w['pc']:w['word'] for r in json.loads((OUT/'original-words.json').read_text())['ranges'] for w in r['words']}

def original_leaf(vertices,normals,start,direction,length):
    class LeafOracle(Original):
        def __init__(self):super().__init__(EMPTY,WORDS);self.uninitialized_edge_reads=[]
        def read(self,a,size=4):
            if self.pc in (0x10c920,0x10c924,0x10cdc8,0x10cdcc) and any(a+i not in self.overlay for i in range(size)):
                self.uninitialized_edge_reads.append({'pc':f'{self.pc:08X}','address':f'{a:08X}'})
            return super().read(a,size)
    m=LeafOracle()
    m.write(0x3dfe60,0x2000);m.write(0x3df5b0,1);m.write(0x2000,0x3000)
    m.write(0x2004,0x3100 if normals else 0)
    for i,p in enumerate(vertices):
        m.vector(0x3000+i*12,p);m.write(0x1000+i*2,i,2)
        if normals:m.vector(0x3100+i*12,normals[i])
    m.vector(0x4000,[*direction,1.]);m.vector(0x4010,[*start,1.]);m.storef(0x4040,-1.)
    for r,a in zip(range(4,10),[0x1000,0x4000,0x4010,0x4020,0x4030,0x4040]):m.g[r][0]=a
    m.f[12]=length
    hit=m.run(0x10b840)
    return m,hit,m.vec(0x4020),m.vec(0x4030),m.loadf(0x4040)

def main():
    fixtures=[];coverage=set();branches={};classes={};rng=random.Random(0x48_48)
    def case(tag,vs,ns,start,direction,length):
        vs=[[f32(x) for x in v] for v in vs];ns=[[f32(x) for x in n] for n in ns] if ns else None
        start=list(map(f32,start));direction=list(map(f32,direction));length=f32(length)
        m,hit,point,normal,t=original_leaf(vs,ns,start,direction,length)
        coverage.update(m.coverage)
        for a,b in m.branches.items():branches.setdefault(a,set()).update(b)
        status=3 if hit and (m.uninitialized_edge_reads or not all(math.isfinite(x) for x in normal)) else hit
        floats=[*sum(vs,[]),*sum(ns or [[0.]*3]*3,[]),*start,*direction,length,*point,*normal,t]
        fixtures.append({'class':tag,'input_words':[bits(v) for v in floats[:25]],'has_normals':bool(ns),'expected_status':status,'original_hit':hit,'output_words':[bits(v) for v in floats[25:]],'original_steps':m.steps,'uninitialized_original_edge_reads':m.uninitialized_edge_reads})
        classes[tag]=classes.get(tag,0)+1
    base=[[-2.,0.,-2.],[0.,0.,2.],[2.,0.,-2.]]
    normals=[[.125,1.,0.],[0.,1.,.25],[-.125,1.,0.]]
    for perm in itertools.permutations(range(3)):
        for axis in range(3):
            rotate=lambda v:v[axis:]+v[:axis]
            for ns in (None,normals):
                for x,z in [(.25,.125),(-.5,-.25),(4.,0.),(0.,-2.),(-2.,-2.),(0.,2.)]:
                    v=[rotate(base[i]) for i in perm];n=[rotate(normals[i]) for i in perm] if ns else None
                    # Smooth-normal slice coinciding with a triangle vertex may
                    # read an uninitialized second edge. This is a contract
                    # boundary, explicitly represented rather than blessed.
                    case('axis-permutation-boundary',v,n,rotate([x,1.,z]),rotate([0.,-1.,0.]),2.)
    for y,length in [(0.,2.),(2.,2.),(2.001,2.),(-.001,2.),(1.,0.),(1.,-1.)]:
        case('segment-endpoints',base,None,[.25,y,.125],[0.,-1.,0.],length)
    case('parallel',base,None,[0.,1.,0.],[1.,0.,0.],3.)
    case('degenerate',[[0.,0.,0.]]*3,None,[0.,1.,0.],[0.,-1.,0.],2.)
    for i in range(480):
        vs=[[rng.uniform(-100,100) for _ in range(3)] for _ in range(3)]
        a=[vs[1][j]-vs[0][j] for j in range(3)];b=[vs[2][j]-vs[0][j] for j in range(3)]
        n=[a[(j+1)%3]*b[(j+2)%3]-a[(j+2)%3]*b[(j+1)%3] for j in range(3)]
        norm=math.sqrt(sum(x*x for x in n));n=[x/norm for x in n]
        weights=[.2,.3,.5] if i%4 else [-.3,.6,.7]
        point=[sum(vs[k][j]*weights[k] for k in range(3)) for j in range(3)]
        start=[point[j]+n[j]*10 for j in range(3)];direction=[-x for x in n]
        ns=[[rng.uniform(-.3,.3)+n[j] for j in range(3)] for k in range(3)] if i%2 else None
        case('random-3d',vs,ns,start,direction,20.)
    # Capture exact inputs to every original leaf reached in the saved hit tree.
    class Capture(Original):
        def execute(self,pc):
            if pc==0x10b840:
                leaf=self.g[4][0];desc=self.read(0x3dfe60);vp=self.read(desc);np=self.read(desc+4)
                assert self.read(desc+8)==0
                ids=[self.read(leaf+i*2,2) for i in range(3)]
                vs=[self.vec(vp+i*12) for i in ids];ns=[self.vec(np+i*12) for i in ids] if np else None
                case('saved47W-leaf',vs,ns,self.vec(self.g[6][0]),self.vec(self.g[5][0]),self.f[12])
            return super().execute(pc)
    saved=RAM.read_bytes();assert digest(saved)=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
    replay=[]
    for candidate in (0x9e7d30,0x9f7350):
        m=Capture(saved,WORDS);m.g[28][0]=0x3dd970
        m.write(0x3df5b8,0x9512e0);m.storef(0x951354,1);m.write(0x951340,0)
        m.g[4][0]=0xd113f0;m.g[5][0]=candidate
        hit=m.run(0x121410)
        values=[m.read(0x951310+i*4) for i in range(3)]+[m.read(0x951300+i*4) for i in range(3)]+[m.read(0x951354)]
        expected=[struct.unpack_from('<I',saved,0x951310+i*4)[0] for i in range(3)]+[struct.unpack_from('<I',saved,0x951300+i*4)[0] for i in range(3)]+[struct.unpack_from('<I',saved,0x951354)[0]]
        if hit:assert values==expected,(values,expected)
        replay.append({'candidate':f'{candidate:08X}','hit':hit,'steps':m.steps,'unique_words':len(m.coverage),'output_words':values,'matches_saved_contact':values==expected if hit else None})
    report={'oracle_domain':'host IEEE binary32 round-to-nearest; finite fixtures, explicit boundary cases, not all physical EE/VU FP modes','fixtures':fixtures,'classes':classes,'unique_leaf_words':len(coverage),'branch_outcomes':{f'{a:08X}':sorted(b) for a,b in sorted(branches.items())},'saved47W_replay':replay}
    (OUT/'leaf-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    lines=['#pragma once','#include <cstdint>','struct QueryFixture48H { std::uint32_t input[25]; bool normals; unsigned status; std::uint32_t output[7]; };','static const QueryFixture48H queryFixtures48H[] = {']
    for c in fixtures:
        fmt=lambda xs:','.join(f'0x{x:08x}u' for x in xs)
        lines.append('{{'+fmt(c['input_words'])+'},'+str(c['has_normals']).lower()+','+str(c['expected_status'])+',{'+fmt(c['output_words'])+'}},')
    lines+=['};']
    (ROOT/'tools/gameplay/query_fixtures48h.h').write_text('\n'.join(lines)+'\n')
    print(json.dumps({'fixtures':len(fixtures),'classes':classes,'unique_leaf_words':len(coverage),'saved47W_replay':replay}))

if __name__=='__main__':main()
