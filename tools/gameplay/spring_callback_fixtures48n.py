"""Freeze authentic static spring callback and execute initialized test objects.

No guest RAM image, instruction patch, intercepted helper, or native expected
value is used. Original type2 callback and append helper run end to end. strlen
runs its original scalar byte path using an explicitly unaligned synthetic name.
"""
import sys
sys.dont_write_bytecode=True
from pathlib import Path
import hashlib,json,random,struct
from spring_fixture48j import SpringOriginal,ROOT

OUT=ROOT/'evidence/springCallback48N'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RANGES=[('static_callback',0x294bb0,0x295260),('append',0x2963e0,0x296484),
        ('strlen',0x3093a8,0x3094e0),('detector_init_anchor',0x2953a0,0x295630),
        ('pair_world_transform',0x121590,0x121638),('leaf_w_initialization',0x108378,0x1083d8)]
STATE_OFFSETS=(0x38,0x3c,0x40,0x44,0x74,0x78,0x130,0x7c,0x84,0x88,0xd4)
COUNT=640
WIDTH=350

def sha(data):return hashlib.sha256(data).hexdigest()
def freeze(frozen=False):
    OUT.mkdir(parents=True,exist_ok=True)
    path=ROOT/'work/native/project/disc/SLUS_202.68'
    if frozen or not path.exists():
        proof=json.loads((OUT/'original-words.json').read_text())
        assert proof['elf_sha256']==ELF_SHA
        # The self-contained archive proof remains useful without the ELF; raw
        # blocks and their exact words are checked against pinned block hashes.
        expected={'static_callback':'03ff3b599435c8df4f56fa2a97882e499edb138bdb8f57039b487d0675e46c6e','append':'512336f0e39791a16e862699a97995aa1d52a30e31457aca0fc3a6b84ddcde23','strlen':'d2ca49978c39d25260ae364bbc13c4ab5b86a0c032f67c5786d68f03b17a6871','detector_init_anchor':'bb6a8d901f0a57615fd3c0cfefaf4a15f0c5d4b440327a1019e4d041e3406e41','pair_world_transform':'d1331c9da967ec5b1381532a0b39078ba1a966cc6a644095748f31f967c55362','leaf_w_initialization':'c449f617ab80e9395d60d05501829146b723647fd78e4ef3e48138950ee03293'}
        for b in proof['blocks']:assert b['sha256']==expected[b['name']]
    else:
        elf=path.read_bytes();assert sha(elf)==ELF_SHA
        blocks=[]
        for name,lo,hi in RANGES:
            raw=elf[lo-0x100000+0x80:hi-0x100000+0x80]
            rows=[dict(pc=pc,word=struct.unpack_from('<I',raw,pc-lo)[0]) for pc in range(lo,hi,4)]
            blocks.append(dict(name=name,start=lo,end=hi,sha256=sha(raw),instructions=rows))
            (OUT/(name+'.bin')).write_bytes(raw)
            (OUT/(name+'.txt')).write_text('\n'.join(f"{r['pc']:08X} {r['word']:08X}" for r in rows)+'\n')
        proof=dict(elf_sha256=ELF_SHA,blocks=blocks,extraction='Immutable ELF PT_LOAD address mapping: file_offset=VA-0x100000+0x80')
        (OUT/'original-words.json').write_text(json.dumps(proof,indent=2)+'\n')
    words={}
    assert len(proof['blocks'])==len(RANGES)
    for b,(name,lo,hi) in zip(proof['blocks'],RANGES):
        assert (b['name'],b['start'],b['end'])==(name,lo,hi)
        assert [r['pc'] for r in b['instructions']]==list(range(lo,hi,4))
        raw=b''.join(struct.pack('<I',r['word']) for r in b['instructions'])
        assert sha(raw)==b['sha256'] and raw==(OUT/(name+'.bin')).read_bytes()
        words.update((r['pc'],r['word']) for r in b['instructions'])
    assert words[0x295458]==0xAC4300E4 # original stores callback address into query+E4
    return words

class CallbackOriginal(SpringOriginal):
    def __init__(self,words):
        super().__init__(words=words);self.material=0;self.append_calls=[]
    def execute(self,pc):
        if pc==0x294dd4:self.material=self.g[20][0]
        if pc==0x2963e0:self.append_calls.append(self.g[11][0])
        w=self.words[pc]
        if w>>26==0 and w&63==0x2d:
            # Authentic MIPS64 DADDU (strlen's pointer move), not a helper stub.
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
            rs=(w>>21)&31;rt=(w>>16)&31;rd=(w>>11)&31
            x=self.g[rs][0]|(self.g[rs][1]<<32);y=self.g[rt][0]|(self.g[rt][1]<<32)
            z=(x+y)&0xffffffffffffffff
            self.g[rd][0]=z&0xffffffff;self.g[rd][1]=z>>32;self.g[0]=[0]*4
            return None
        return super().execute(pc)

def fixture(words,row,rng):
    names=[b'roadfr0.col\0',b'roadfr9.col\0',b'roadfrA.col\0',b'roadfr/.col\0',
           b'roadfr\xff.col\0',b'roadfx3.col\0',b'roadfr2\0',b'road\0',
           b'roadfr1.extra.fr9\0',b'road.fr9\0',b'fr3.\0',b'roadfr-\0',
           b'roadfr\x80\0',b'roadfr\x7f\0',b'roadfr5.\0',b'roadFR2.col\0']
    scenario=row%32
    name=names[(row//4)%len(names)]
    # The first64 cases enumerate all suffix shapes without a skip/material
    # bypass; the rest combine prefixes, capacities and prior identities.
    cat=4 if row<64 else (4,0,8,12,4,4,4,0)[row%8]
    skip,dynamic,material=8,2,4
    pair_count=(row//3)%5
    capacity=(row//5)%6
    old_count=(row//7)%7
    state=[rng.getrandbits(32) for _ in range(11)]
    state[6]=(0,18,18)[row%3];state[8]=old_count;state[9]=capacity
    current_category=(0,0,2)[row%3]
    has_scene=row<64 or row%11!=0
    # Last material and untouched record bytes intentionally arbitrary.
    pairs=[rng.getrandbits(32) for _ in range(64)]
    initial=[rng.getrandbits(32) for _ in range(120)]
    if row%13==0:
        pairs[:16]=[0,0x80000000,0x7f800000,0x7fc12345,0xff800000,0x7f812345,1,0x7fffffff]*2
    scene_id=37 if row%4 else 0
    fields=[17,cat,current_category,skip,dynamic,material,2,scene_id,int(has_scene),len(name),pair_count,5]
    name_words=list(struct.unpack('<8I',name.ljust(32,b'\0')))
    original=CallbackOriginal(words)
    query,spring,candidate,previous,shape,result=0x10000,0x11000,0x12000,0x13000,0x14000,0x15000
    material_scene=0x16001 # scene+38 is unaligned, selects original strlen byte path
    endpoints,normals,points,records=0x17000,0x18000,0x19000,0x20000
    for off,value in zip(STATE_OFFSETS,state):
        if off==0x130:value=previous if value else 0
        original.write(spring+off,value)
    original.write(spring+0x8c,records)
    original.write(previous+0xa4,current_category)
    original.write(candidate+0xa4,cat);original.write(candidate+0x9c,material_scene if has_scene else 0)
    for i,value in enumerate(name):original.write(material_scene+0x38+i,value,1)
    for offset,value in ((0x9c,spring),(0xa0,0xdeadbeef),(0x88,2),(0x8c,shape),(0x90,result)):
        original.write(query+offset,value)
    original.write(shape+0x44,scene_id)
    for offset,value in ((0x50,endpoints),(0x54,normals),(0x58,points),(0x60,pair_count)):
        original.write(result+offset,value)
    original.write(0x3e0728,skip);original.write(0x3e0718,dynamic);original.write(0x3e0720,material)
    for i in range(4):
        for lane,value in enumerate(pairs[i*16:i*16+8]):original.write(endpoints+i*32+lane*4,value)
        for lane,value in enumerate(pairs[i*16+8:i*16+12]):original.write(normals+i*16+lane*4,value)
        for lane,value in enumerate(pairs[i*16+12:i*16+16]):original.write(points+i*16+lane*4,value)
    for i,value in enumerate(initial):original.write(records+i*4,value)
    original.g[4][0]=query;original.g[5][0]=candidate;original.run(0x294bb0)
    expected_state=[original.read(spring+off) for off in STATE_OFFSETS]
    expected_state[6]={0:0,previous:18,candidate:17}[expected_state[6]]
    expected_records=[original.read(records+i*4) for i in range(120)]
    skipped=bool(cat&skip)
    appended=expected_state[8]-state[8]
    summary=[1 if skipped else 0,appended,0 if skipped else pair_count-appended,original.material]
    assert len(original.append_calls)==(0 if skipped else pair_count)
    assert not original.source_reads
    payload=fields+state+name_words+pairs+initial+expected_state+expected_records+summary
    assert len(payload)==WIDTH
    return payload,original,dict(case=row,scenario=scenario,name_hex=name.hex(),skip=skipped,pairs=pair_count,capacity=capacity,initial_count=old_count)

def lane_probes(words):
    cases=[]
    for initial_w in (-2.,0.,1.,2.5):
        for flags in ([0,0],[1,0],[0,1],[1,1]):
            o=CallbackOriginal(words)
            # Enter the real per-pair world-transform loop at its initialized
            # boundary. This is a local lane proof, not a full tree execution.
            result,target,tmp,normals,points,transformed=0x1000,0x2000,0x3000,0x4000,0x5000,0x6000
            for off,value in ((0x54,normals),(0x58,points),(0x64,transformed),(0x60,2)):o.write(result+off,value)
            o.vector(target,[1,0,0,0,0,1,0,0,0,0,1,0,7,8,9,1])
            for i,flag in enumerate(flags):
                o.write(transformed+i*4,flag)
                o.vector(normals+i*16,[2+i,3+i,4+i,10+i])
                o.vector(points+i*16,[5+i,6+i,7+i,20+i])
            o.g[16][0]=target;o.g[3][0]=result;o.g[6][0]=tmp;o.g[5][0]=1;o.g[2][0]=0;o.v[9][3]=initial_w
            pc=0x121590
            while pc!=0x121638:
                next_pc=o.execute(pc)
                if next_pc is not None:
                    if not o.annul:assert o.execute(pc+4) is None
                    pc=next_pc
                else:pc+=4
            assert not o.source_reads
            cases.append(dict(incoming_vf9_w=initial_w,initial_flags=flags,normal_vec4=[o.vec(normals+i*16,4) for i in range(2)],
                surface_vec4=[o.vec(points+i*16,4) for i in range(2)],final_flags=[o.read(transformed+i*4) for i in range(2)],steps=o.steps))
    report=dict(scope='Original121590..121634 only, entering an explicitly initialized boundary; original10838C/1083C0 W=1 initialization is frozen separately',
        source_reads=0,no_original_words_patched=True,cases=cases)
    (OUT/'world-lane-probes.json').write_text(json.dumps(report,indent=2)+'\n')

def main():
    words=freeze('--frozen' in sys.argv);lane_probes(words);rng=random.Random(0x48a294bb0)
    records=[];coverage=set();branches={};steps=0;labels=[]
    for i in range(COUNT):
        payload,o,label=fixture(words,i,rng);records.append(payload);labels.append(label)
        coverage.update(o.coverage);steps+=o.steps
        for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
    for pc in (0x294be4,0x294bf8,0x294c10,0x294d3c,0x294d48,0x294d64,0x294d6c,0x294d80,0x294d94,0x294db4,0x294dc4,0x294e70,0x296420):
        assert branches[pc]=={False,True},hex(pc)
    raw=struct.pack('<II',COUNT,WIDTH)+b''.join(struct.pack('<'+str(WIDTH)+'I',*r) for r in records)
    (OUT/'static-callback-fixtures.bin').write_bytes(raw)
    report=dict(cases=COUNT,words_per_case=WIDTH,input_words=215,output_words=135,steps=steps,
        source_reads=0,no_original_words_patched=True,no_native_expected_values=True,
        elf_sha256=ELF_SHA,original_words_sha256=sha((OUT/'original-words.json').read_bytes()),fixture_sha256=sha(raw),
        scope='End-to-end original static type2 callback and append, synthetic initialized objects; scalar original strlen path, no dynamic virtual calls or tags3/5',
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},cases_detail=labels)
    (OUT/'static-callback-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','cases_detail')}))
if __name__=='__main__':main()
