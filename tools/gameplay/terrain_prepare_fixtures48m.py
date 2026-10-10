"""Bounded numeric preparation fixtures; no game process or full RAM input.

Uses the unchanged 48H original-word executor with explicitly initialized test
objects. Active queries stop BEFORE 11FFA4 calls spatial registration. Disabled
and absent-body paths return naturally. Saved header values are merely inputs
to synthetic continuation cases, never a reconstructed/full simulation tick.
"""
import copy
import hashlib
import json
import random
import re
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode=True
from query_oracle48h import Original, bits, fp

ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/terrainPrepare48M'
ELF=ROOT/'work/native/project/disc/SLUS_202.68'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RANGES=[(0x1b1800,0x1b1a30),(0x11ff90,0x11ffa4),
        (0x11f680,0x11f6c8),(0x11fa30,0x11fc80),(0x11fe28,0x11fe44),
        (0x106790,0x1067ac),(0x14d840,0x14d8bc)]
Q,BODY,SCENE,SHAPE,LEAF=0x1000,0x2000,0x3000,0x4000,0x5000
RETURN=0xfffffff0

def sha(data):return hashlib.sha256(data).hexdigest()
def floats(xs):return [bits(x) for x in xs]
def words_header(path,name):
    source=path.read_text()
    match=re.search(r'\b'+name+r'\s*\[\]\s*=\s*\{(.*?)\};',source,re.S)
    if not match:raise ValueError('missing fixture array '+name)
    return [int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)u?',match[1])]
def run_to_boundary(m):
    pc=0x1b1800
    while pc not in (0x11ffa4,RETURN):
        target=m.execute(pc)
        if target is None:pc+=4
        else:
            if not m.annul:assert m.execute(pc+4) is None
            pc=target
    return pc

def main():
    elf=ELF.read_bytes();assert sha(elf)==ELF_SHA
    word_at=lambda pc:struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]
    words={pc:word_at(pc) for a,b in RANGES for pc in range(a,b,4)}
    frozen=json.loads((ROOT/'evidence/query48H/original-words.json').read_text())
    frozen_words={w['pc']:w['word'] for r in frozen['ranges'] for w in r['words']}
    for pc,w in words.items():
        if pc in frozen_words:assert frozen_words[pc]==w
    assert word_at(0x388a08)==0x11fa30
    source=ROOT/'tools/gameplay/contact_words48g.h'
    terrain=words_header(source,'rr_saved_terrain_48g')
    scene=words_header(source,'rr_saved_scene_48g')[0x100//4:0x140//4]
    constants=words_header(source,'rr_terrain_constants_48g')[:4]
    assert constants==[0,0,0,0x3f800000]
    previous=words_header(ROOT/'tools/gameplay/terrain_query_fixtures48j.h','terrainQueryFixtures48J')[3:29]
    get=lambda o,n=1:terrain[o//4:o//4+n]
    state=[*get(0x104),*get(0x10c),*get(0x11c),*get(0x180,3),*get(0x190,3),*get(0x1d0,3),
           *get(0x108),*get(0x110),*get(0x114),*get(0x150,3),*get(0x170,3),
           *range(101,117),7,305,306,*previous,*get(0x118)]
    assert len(state)==67
    # Input layout: seven gates/scope words, scene16, lower3, state67.
    seed=[1,1,0,0,2,1,0,*scene,*get(0x140,3),*state]
    cases=[];coverage=set();branches={};total=0

    class Guarded(Original):
        def __init__(self):
            super().__init__(words=words);self.allowed_source_addresses=set();self.read_addresses=set()
        def read(self,a,size=4):
            self.read_addresses.update(range(a,a+size))
            return super().read(a,size)

    def oracle(request):
        linked,enabled,aux120,aux124,shape_type,pair_count,tree=request[:7]
        assert not aux120 and not aux124 and shape_type==2 and pair_count==1 and not tree
        matrix=request[7:23];lower=request[23:26];s=request[26:]
        m=Guarded()
        def put(addr,values):
            for i,w in enumerate(values):m.write(addr+4*i,w)
        def vec(addr,values):put(addr,[*values,0x3f800000])
        put(Q+0x128,[BODY if linked else 0]);put(BODY+0x288,[SCENE]);put(SCENE+0x100,matrix)
        put(Q+0x100,[enabled]);put(Q+0x120,[0,0]);put(Q+0x88,[2,SHAPE])
        put(Q+0xe0,[0x6000]) # Opaque registration identity; deliberately never read.
        put(SHAPE+0x4c,[0,LEAF]);put(0x388a08,[word_at(0x388a08)]);put(0x434260,constants)
        for off,w in zip((0x104,0x10c,0x11c),s[:3]):put(Q+off,[w])
        for off,start in ((0x180,3),(0x190,6),(0x1d0,9),(0x150,15),(0x170,18)):
            vec(Q+off,s[start:start+3])
        for off,w in zip((0x108,0x110,0x114),s[12:15]):put(Q+off,[w])
        put(0x7000,s[21:37]);put(Q+0x1ec,[0x7000]);put(Q+0x1e8,[s[37]])
        put(Q+0x12c,[s[38]]);put(Q+0xf4,[s[39]])
        vec(LEAF,s[40:43]);vec(LEAF+16,s[43:46]);put(SHAPE,s[46:62])
        vec(Q+0x60,s[62:65]);put(Q+0x80,[s[65]]);put(Q+0x118,[s[66]])
        vec(Q+0x140,lower)
        m.g[4][0]=Q;m.g[29][0]=0x1ff0000;m.g[31][0]=RETURN
        boundary=run_to_boundary(m)
        executed_reads=set(m.read_addresses)
        read=lambda addr,n=1:[m.read(addr+4*i) for i in range(n)]
        out=[*read(Q+0x104),*read(Q+0x10c),*read(Q+0x11c),
             *read(Q+0x180,3),*read(Q+0x190,3),*read(Q+0x1d0,3),
             *read(Q+0x108),*read(Q+0x110),*read(Q+0x114),
             *read(Q+0x150,3),*read(Q+0x170,3),*read(0x7000,16),
             *read(Q+0x1e8),*read(Q+0x12c),*read(Q+0xf4),
             *read(LEAF,3),*read(LEAF+16,3),*read(SHAPE,16),
             *read(Q+0x60,3),*read(Q+0x80),*read(Q+0x118)]
        status=2 if not linked else 1 if not enabled else 0
        assert boundary==(0x11ffa4 if status==0 else RETURN)
        assert not m.source_reads
        assert not any(Q+0xe0<=a<Q+0xe4 for a in executed_reads)
        assert 0x120230 not in m.coverage and 0x122660 not in m.coverage
        return out,status,m,boundary

    def add_case(name,change=None):
        nonlocal total
        request=copy.deepcopy(seed)
        if change:change(request)
        out,status,m,boundary=oracle(request)
        cases.append({'name':name,'input_words':request,'status':status,'output_words':out,
                      'instructions':m.steps,'stop_pc':f'{boundary:08X}',
                      'executed_pcs':[f'{pc:08X}' for pc in sorted(m.coverage)],'source_byte_reads':0})
        coverage.update(m.coverage);total+=m.steps
        for pc,values in m.branches.items():branches.setdefault(pc,set()).update(values)

    add_case('retained-values-synthetic-continuation-prior-contact')
    add_case('retained-values-synthetic-continuation-airborne',lambda r:r.__setitem__(26,0))
    for linked in (0,1):
        for enabled in (0,1):
            for active in (0,1,2,0xffffffff):
                def change(r,linked=linked,enabled=enabled,active=active):
                    r[0]=linked;r[1]=enabled;r[26]=active
                    r[26+12:26+15]=[71,72,73];r[26+66]=74
                    r[26+9:26+12]=floats([11,-12,13])
                add_case(f'gates-body{linked}-enabled{enabled}-active{active}',change)
    matrices={
        'identity':[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
        'quarter-turn':[0,0,-1,0,0,1,0,0,1,0,0,0,10,-20,30,1],
        'nonorthogonal':[2,.25,-.5,0,1,3,.75,0,-.125,.5,.25,0,-10,20,-30,1],
        'zero-basis':[0,0,0,0,0,0,0,0,0,0,0,0,10,20,30,1],
        'signed-zero':[-0.,0,-0.,0,0,1,-0.,0,-0.,0,1,0,-0.,0,-0.,1]}
    pairs={
        'vertical':([0,3,0],[0,-28,0]),'zero-length':([2,-3,4],[2,-3,4]),
        'origin':([0,0,0],[0,0,0]),'xyz':([-3,8,2],[7,-4,6]),
        'reverse':([0,-28,0],[0,3,0]),
        'signed-zero':([-0.,0,-0.],[0,-0.,0]),
        'small':([2**-30,-2**-28,2**-26],[-2**-30,2**-28,-2**-26])}
    for label,matrix in matrices.items():
        for pair,(first,second) in pairs.items():
            for active in (0,1):
                def change(r,matrix=matrix,first=first,second=second,active=active):
                    r[7:23]=floats(matrix);r[26]=active
                    r[26+40:26+46]=floats(first+second)
                    # Intentionally distinct component lower and leaf endpoints.
                    r[23:26]=floats([1,-5,2])
                add_case(f'{label}-{pair}-active{active}',change)
    for count in (0,16,17,0xffffffff):
        add_case('reset-does-not-read-old-count-'+str(count),lambda r,count=count:r.__setitem__(26+37,count))
    for word in (0x3f7fffff,0x3f800000,0x3f800001):
        def change(r,word=word):r[26+40:26+46]=[0,0,0,word,0,0]
        add_case('adjacent-endpoint-'+hex(word),change)
    rng=random.Random(0x484d)
    for index in range(128):
        def change(r):
            r[0]=int(rng.randrange(16)!=0);r[1]=int(rng.randrange(8)!=0);r[26]=rng.choice([0,1,3])
            matrix=[rng.uniform(-2,2) if i%4!=3 else 0 for i in range(12)]
            matrix += [rng.uniform(-15000,15000) for _ in range(3)]+[1]
            r[7:23]=floats(matrix);r[23:26]=floats([rng.uniform(-30,30) for _ in range(3)])
            r[26+40:26+46]=floats([rng.uniform(-100,100) for _ in range(6)])
        add_case('deterministic-'+str(index),change)

    OUT.mkdir(parents=True,exist_ok=True)
    (OUT/'original-words.json').write_text(json.dumps({
        'elf_sha256':ELF_SHA,'address_mapping':'ELF offset = VA - 0x100000 + 0x80',
        'words':[{'pc':f'{pc:08X}','word':f'{word:08X}'} for pc,word in sorted(words.items())],
        'dispatch_word':{'address':'00388A08','word':'0011FA30'}},indent=2)+'\n')
    fixture_words=[0x4d383452,1,len(cases)]
    for case in cases:fixture_words.extend([*case['input_words'],case['status'],*case['output_words']])
    binary=struct.pack('<'+'I'*len(fixture_words),*fixture_words)
    (OUT/'fixtures.bin').write_bytes(binary)
    header=['// Generated test-only numeric preparation fixtures.','#pragma once','#include <cstdint>',
            'static const std::uint32_t terrainPrepareFixtures48M[]={']
    header += [','.join(f'0x{x:08x}u' for x in fixture_words[i:i+12])+',' for i in range(0,len(fixture_words),12)]
    header.append('};');(OUT/'fixtures.h').write_text('\n'.join(header)+'\n')
    (OUT/'fixtures.json').write_text(json.dumps({'layout':{'input':'bodyLinked,queryEnabled,aux120,aux124,shapeType,pairCount,hasTree,scene16,lower3,state67',
        'state':'active,pending,detached,previousPoint3,previousNormal3,largestChange3,forceAngle,hit110,hit114,point3,normal3,collected16,count,excluded,current,first3,second3,world16,center3,radius,above118'},
        'scope':'Synthetic explicit inputs. Ready path stops before 11FFA4; no spatial index, query or whole tick.', 'cases':cases},indent=2)+'\n')
    proof={'scope':'Original 1B1800 numeric prefix and type2 single-pair setter only; spatial registration not executed or suppressed.',
      'oracle':'Unmodified query_oracle48h.Original; empty source bytes; every input byte explicitly initialized; all source reads forbidden.',
      'saved_values':'Frozen headers supply selected values only. No ee.bin read or reconstructed dump. Registered pointer remains opaque and unread.',
      'elf_sha256':sha(elf),'cases':len(cases),'instructions':total,'unique_words':len(coverage),
      'source_byte_reads':0,'status_counts':{str(s):sum(c['status']==s for c in cases) for s in (0,1,2)},
      'ranges':[{'start':f'{a:08X}','end_exclusive':f'{b:08X}','sha256':sha(elf[a-0x100000+0x80:b-0x100000+0x80])} for a,b in RANGES],
      'dispatch':{'address':'00388A08','word':'0011FA30','source':'hash-verified ELF'},
      'branches':{f'{pc:08X}':sorted(values) for pc,values in sorted(branches.items())},
      'executed_pcs':[f'{pc:08X}' for pc in sorted(coverage)],
      'source_hashes':{str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in [Path(__file__),ROOT/'tools/gameplay/query_oracle48h.py',ROOT/'evidence/query48H/original-words.json',source,ROOT/'tools/gameplay/terrain_query_fixtures48j.h']},
      'fixture_hashes':{name:sha((OUT/name).read_bytes()) for name in ['fixtures.bin','fixtures.h','fixtures.json','original-words.json']}}
    (OUT/'proof.json').write_text(json.dumps(proof,indent=2)+'\n')
    print(json.dumps({k:proof[k] for k in ('cases','instructions','unique_words','source_byte_reads','status_counts')}))

if __name__=='__main__':main()
