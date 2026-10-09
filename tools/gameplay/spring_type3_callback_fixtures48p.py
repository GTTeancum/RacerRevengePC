"""Authentic static type3 spring callback, explicitly initialized bounded objects.

No RAM image, instruction patches, helper stubs or native expected values.
Arithmetic proof is ordinary, individually rounded IEEE binary32, as in48O.
"""
import sys
sys.dont_write_bytecode=True
from pathlib import Path
import hashlib,json,random,struct
from spring_callback_fixtures48n import CallbackOriginal,STATE_OFFSETS
from spring_fixture48j import ROOT
from query_oracle48h import bits,f32

OUT=ROOT/'evidence/springCallback48P'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RANGES=[('static_callback',0x294bb0,0x295260),('append',0x2963e0,0x296484),
        ('strlen',0x3093a8,0x3094e0),('copy_xyz_one_w',0x295260,0x295288),
        ('construct_xyz_one_w',0x106790,0x1067ac)]
COUNT=768
WIDTH=378

def sha(data):return hashlib.sha256(data).hexdigest()
def freeze(frozen=False):
    OUT.mkdir(parents=True,exist_ok=True)
    path=ROOT/'work/native/project/disc/SLUS_202.68'
    if frozen or not path.exists():
        proof=json.loads((OUT/'original-words.json').read_text())
        assert proof['elf_sha256']==ELF_SHA
        expected={'static_callback':'03ff3b599435c8df4f56fa2a97882e499edb138bdb8f57039b487d0675e46c6e',
                  'append':'512336f0e39791a16e862699a97995aa1d52a30e31457aca0fc3a6b84ddcde23',
                  'strlen':'d2ca49978c39d25260ae364bbc13c4ab5b86a0c032f67c5786d68f03b17a6871',
                  'copy_xyz_one_w':'15ea98169383f52902f628ebfd09a1bc1b097d82f756a5f435bf13990cf66ec9',
                  'construct_xyz_one_w':'e0d8c70d997a0996626a12d2184e4cbbfd221c249fea4f54d3c5e505dec51dfd'}
        for b in proof['blocks']:
            assert b['sha256']==expected[b['name']]
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
    return words

class Type3CallbackOriginal(CallbackOriginal):
    def __init__(self,words):
        super().__init__(words);assert self.data==b''
        self.type3_append_indices=[]
    def execute(self,pc):
        if pc==0x295260:assert self.g[5][0]!=0
        if pc==0x2963e0:self.type3_append_indices.append(self.g[16][0])
        return super().execute(pc)

def fixture(words,row,rng):
    names=[b'roadfr0.col\0',b'roadfr9.col\0',b'roadfrA.col\0',b'roadfr/.col\0',
           b'roadfr\xff.col\0',b'roadfx3.col\0',b'roadfr2\0',b'road\0',
           b'roadfr1.extra.fr9\0',b'road.fr9\0',b'fr3.\0',b'roadfr-\0',
           b'roadfr\x80\0',b'roadfr\x7f\0',b'roadfr5.\0',b'roadFR2.col\0']
    name=names[(row//4)%len(names)]
    cat=4 if row<64 else (4,0,8,12,4,4,4,0)[row%8]
    skip,dynamic,material=8,2,4
    point_count=(row//3)%5
    if row>=128 and row%31==0:point_count=0xffffffff
    if row>=128 and row%37==0:point_count=0x80000000
    capacity=(row//5)%6;old_count=(row//7)%7
    state=[rng.getrandbits(32) for _ in range(11)]
    state[6]=(0,18,18)[row%3];state[7]=point_count;state[8]=old_count;state[9]=capacity
    current_category=(0,0,2)[row%3]
    has_scene=row<64 or row%11!=0
    def xyz():return [bits(f32(rng.uniform(-16,16))) for _ in range(3)]
    direction=xyz()+[rng.getrandbits(32)]
    matrix=[bits(f32(rng.uniform(-2,2))) for _ in range(16)]
    for i in (3,7,11,15):matrix[i]=rng.getrandbits(32)
    if row%4==0:matrix=[bits(x) for x in [1,0,0,0,0,1,0,0,0,0,1,0,7,8,9,1]]
    if row%17==0:
        # Both signed zeros exercise the VU column + zero copy before transpose.
        matrix=[(0x80000000 if (row+i)&1 else 0) for i in range(16)]
        direction=[0x80000000,0,0x80000000,0x7fc12345]
    pairs=[]
    flags=[]
    for i in range(4):
        start=xyz()+[rng.getrandbits(32)];delta=xyz()+[rng.getrandbits(32)]
        normal=[rng.getrandbits(32) for _ in range(4)];point=[rng.getrandbits(32) for _ in range(4)]
        if row%13==0:normal=[0,0x80000000,0x7fc12345,0xff812345];point=[0x7f800000,0xff800000,1,0x7fffffff]
        flag=(0,1,0x80000000,0xffffffff)[(row+i)%4]
        if row%16==0:flag=0
        if row%16==1:flag=1
        if not flag:start[:3]=[0x7fc54321]*3;delta[:3]=[0x7f800000]*3
        flags.append(flag)
        pairs+=start+delta+normal+point+[flag,37+i if (row+i)%3 else 0]
    initial=[rng.getrandbits(32) for _ in range(120)]
    fields=[17,cat,current_category,skip,dynamic,material,3,int(has_scene),len(name),4,5,0]
    name_words=list(struct.unpack('<8I',name.ljust(32,b'\0')))
    o=Type3CallbackOriginal(words)
    query,spring,candidate,previous,shape,result=0x10000,0x11000,0x12000,0x13000,0x14000,0x15000
    material_scene=0x16001
    starts,deltas,normals,points,records=0x17000,0x18000,0x19000,0x1a000,0x20000
    hit_flags,scenes,common,body,body_scene=0x21000,0x22000,0x23000,0x24000,0x25000
    for off,value in zip(STATE_OFFSETS,state):o.write(spring+off,previous if off==0x130 and value else value)
    o.write(spring+0x8c,records);o.write(previous+0xa4,current_category)
    o.write(candidate+0xa4,cat);o.write(candidate+0x9c,material_scene if has_scene else 0)
    for i,value in enumerate(name):o.write(material_scene+0x38+i,value,1)
    for offset,value in ((0x9c,spring),(0xa0,body),(0x88,3),(0x8c,shape),(0x90,result)):o.write(query+offset,value)
    for offset,value in ((0x04,scenes),(0x14,common),(0x18,starts),(0x1c,deltas)):o.write(shape+offset,value)
    for offset,value in ((0x54,normals),(0x58,points),(0x68,hit_flags)):o.write(result+offset,value)
    o.write(body+0x288,body_scene)
    for i,value in enumerate(matrix):o.write(body_scene+0x100+i*4,value)
    for i,value in enumerate(direction):o.write(common+i*4,value)
    o.write(0x3e0728,skip);o.write(0x3e0718,dynamic);o.write(0x3e0720,material)
    for i in range(4):
        for j,address in enumerate((starts,deltas,normals,points)):
            for lane,value in enumerate(pairs[i*18+j*4:i*18+j*4+4]):o.write(address+i*16+lane*4,value)
        o.write(hit_flags+i*4,pairs[i*18+16]);o.write(scenes+i*4,pairs[i*18+17])
    for i,value in enumerate(initial):o.write(records+i*4,value)
    before=o.overlay.copy()
    o.g[4][0]=query;o.g[5][0]=candidate;o.run(0x294bb0)
    mutable={spring+off+j for off in STATE_OFFSETS for j in range(4)}|set(range(records,records+480))
    assert all(o.overlay[a]==value for a,value in before.items() if a not in mutable)
    expected_state=[o.read(spring+off) for off in STATE_OFFSETS]
    expected_state[6]={0:0,previous:18,candidate:17}[expected_state[6]]
    expected_records=[o.read(records+i*4) for i in range(120)]
    skipped=bool(cat&skip);appended=expected_state[8]-state[8]
    active_count=point_count if point_count<0x80000000 else 0
    expected_indices=[] if skipped else [i for i in range(active_count) if flags[i]]
    assert o.type3_append_indices==expected_indices
    assert not o.source_reads and o.data==b''
    summary=[int(skipped),appended,len(expected_indices)-appended,o.material]
    payload=fields+state+name_words+direction+matrix+pairs+initial+expected_state+expected_records+summary
    assert len(payload)==WIDTH
    return payload,o,dict(case=row,name_hex=name.hex(),skip=skipped,point_count_word=point_count,hit_flags=flags,
        append_indices=expected_indices,capacity=capacity,initial_count=old_count)

def main():
    words=freeze('--frozen' in sys.argv);rng=random.Random(0x48a294e84)
    rows=[];coverage=set();branches={};steps=0;labels=[]
    for i in range(COUNT):
        payload,o,label=fixture(words,i,rng);rows.append(payload);labels.append(label)
        coverage.update(o.coverage);steps+=o.steps
        for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
    for pc in (0x294be4,0x294bf8,0x294c10,0x294d3c,0x294d48,0x294d64,0x294d6c,0x294d80,0x294d94,0x294db4,0x294dc4,0x294eb8,0x295080,0x296420):
        assert branches[pc]=={False,True},hex(pc)
    raw=struct.pack('<II',COUNT,WIDTH)+b''.join(struct.pack('<'+str(WIDTH)+'I',*r) for r in rows)
    (OUT/'static-type3-callback-fixtures.bin').write_bytes(raw)
    report=dict(cases=COUNT,words_per_case=WIDTH,input_words=243,output_words=135,steps=steps,source_reads=0,
        data_bytes=0,no_original_words_patched=True,no_helper_stubs=True,no_native_expected_values=True,
        elf_sha256=ELF_SHA,original_words_sha256=sha((OUT/'original-words.json').read_bytes()),fixture_sha256=sha(raw),
        scope='End-to-end original static type3 callback and append; explicit bounded objects; scalar original strlen; ordinary IEEE binary32; no dynamic virtual calls, query dispatcher, tags2/5 or scene refresh',
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},cases_detail=labels)
    (OUT/'static-type3-callback-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','cases_detail')}))
if __name__=='__main__':main()
