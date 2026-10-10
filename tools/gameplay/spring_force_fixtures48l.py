"""Offline original297190/296C10 fixtures; no native expected values or patches.

All executed instructions come from hash-verified original-word evidence. Every
object/data byte read is explicitly initialized or written by original code.
"""
import sys
sys.dont_write_bytecode=True
from pathlib import Path
import hashlib,json,math,random,re,struct
from spring_fixture48j import SpringOriginal,ROOT
from query_oracle48h import bits,fp,f32
OUT=ROOT/'evidence/springResponse48L'
COUNT=2048
INPUT_WORDS=76
OUTPUT_WORDS=15

def sha(data):return hashlib.sha256(data).hexdigest()
def original_words():
    paths=[ROOT/'evidence/springOrchestration48K/original-words.json',ROOT/'evidence/spring48J/original-words.json']
    expected=['4468f347b6aa73bd6b93a8d6422096614e84437db7681407be3b0ae35c771354',
              'b447e613b37c5ef0bbb503f4c084fbfc30a9715ce4fe5cb0db2d6c1ea9fc258e']
    words={}
    for path,digest in zip(paths,expected):
        assert sha(path.read_bytes())==digest
        for block in json.loads(path.read_text())['blocks']:
            for row in block['instructions']:
                if row['pc'] in words:assert words[row['pc']]==row['word']
                words[row['pc']]=row['word']
    return words,{str(p.relative_to(ROOT)):s for p,s in zip(paths,expected)}

def initialize(o,inputs):
    assert len(inputs)==INPUT_WORDS
    spring=0x10000;normal=0x15000;point=0x15010
    o.vector(normal,inputs[:3]+[1]);o.vector(point,inputs[3:6]+[1]);o.vector(0x4435a0,[0,0,0,1])
    o.write(spring+0x30,0x11000)
    o.vector(spring+0x120,inputs[8:11]+[1]);o.vector(spring+0x100,inputs[11:14]+[1])
    o.storef(spring+0xb4,inputs[17])
    for off,value in zip((0x90,0x94,0x9c,0xa0,0xa4,0xa8),inputs[18:24]):o.storef(spring+off,value)
    for off,value in zip((0x54,0x58,0x70,0x6c),inputs[24:28]):o.write(spring+off,int(value))
    for b,s,values in ((0x11000,0x13000,inputs[28:52]),(0x12000,0x14000,inputs[52:76])):
        o.write(b+0x288,s);o.vector(s+0x100,values[:16]);o.vector(b+0x1e0,inputs[14:17]+[1])
        o.storef(b+0x26c,values[16]);o.storef(b+0x270,values[17])
        o.vector(b+0x180,values[18:21]+[1]);o.vector(b+0x190,values[21:24]+[1])
    o.g[4][0]=spring;o.g[5][0]=normal;o.g[6][0]=point;o.g[7][0]=0x12000 if inputs[7] else 0;o.f[12]=inputs[6]

def secondary_original_words():
    path=OUT/'secondary-original-words.json'
    elf_path=ROOT/'work/native/project/disc/SLUS_202.68'
    generated_path=ROOT/'work/native/project/generated/FUN_002983c0_0x2983c0.cpp'
    if elf_path.exists() and generated_path.exists():
        elf=elf_path.read_bytes();assert sha(elf)=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
        generated=generated_path.read_bytes();rows=[]
        for line,text in enumerate(generated.decode().splitlines(),1):
            match=re.search(r'// 0x([0-9a-f]+): 0x([0-9a-f]+)\s+(.*)',text)
            if match:
                pc=int(match[1],16);word=int(match[2],16)
                if 0x2983c0<=pc<0x298474:
                    assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==word
                    rows.append(dict(pc=pc,word=word,assembly=match[3],line=line))
        assert [r['pc'] for r in rows]==list(range(0x2983c0,0x298474,4))
        raw=elf[0x2983c0-0x100000+0x80:0x298474-0x100000+0x80]
        proof=dict(elf_sha256=sha(elf),generated_source=str(generated_path.relative_to(ROOT)),generated_sha256=sha(generated),
            blocks=[dict(name='secondary_inward_velocity',start=0x2983c0,end=0x298474,sha256=sha(raw),instructions=rows)])
        path.write_text(json.dumps(proof,indent=2)+'\n')
    else:
        assert sha(path.read_bytes())=='ac3ccaa51207b47d2ba41e7afe6d0542c66c11eb6b1b90ebee50e9a224b62b4c'
        proof=json.loads(path.read_text())
        assert proof['elf_sha256']=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    block=proof['blocks'][0]
    assert block['start']==0x2983c0 and block['end']==0x298474
    rows=block['instructions'];assert [r['pc'] for r in rows]==list(range(0x2983c0,0x298474,4))
    assert sha(b''.join(struct.pack('<I',r['word']) for r in rows))==block['sha256']
    return {r['pc']:r['word'] for r in rows}

def secondary_fixtures():
    words=secondary_original_words();rng=random.Random(0x48c2983c0);records=[];coverage=set();steps=0
    def vec():return [f32(rng.uniform(-30,30)) for _ in range(3)]
    for i in range(512):
        normal=vec();cached=vec();velocity=vec()
        if i<16:
            normal=[0,1,0];cached=[0,[-2,-fp(0x00800000),-0.,0.,fp(0x00800000),2,0,0][i%8],0]
            if i>=8:velocity=[-0.,0.,-0.]
        o=SpringOriginal(words=words);o.write(0x10030,0x11000)
        o.vector(0x10120,cached+[1]);o.vector(0x15000,normal+[1]);o.vector(0x11070,velocity+[1])
        o.g[4][0]=0x10000;o.g[5][0]=0x15000
        # Deliberately uninitialized ignored point/other arguments.
        o.g[6][0]=0x16000;o.g[7][0]=0x17000;o.f[12]=fp(0x7fc00000)
        o.run(0x2983c0);result=o.vec(0x11070)
        assert not o.source_reads and all(math.isfinite(v) for v in result)
        records.append([bits(v) for v in normal+cached+velocity+result]);coverage.update(o.coverage);steps+=o.steps
    assert coverage==set(words)
    raw=struct.pack('<II',len(records),12)+b''.join(struct.pack('<12I',*row) for row in records)
    (OUT/'secondary-fixtures.bin').write_bytes(raw)
    report=dict(cases=len(records),input_words=9,output_words=3,steps=steps,source_reads=0,no_original_words_patched=True,
        original_words_sha256=sha((OUT/'secondary-original-words.json').read_bytes()),fixture_sha256=sha(raw),
        arithmetic='Original-word execution, individually rounded ordinary finite IEEE binary32; not physical EE exceptional modes',
        coverage=[f'{pc:08X}' for pc in sorted(coverage)])
    (OUT/'secondary-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='coverage'}))

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    secondary_fixtures()
    words,hashes=original_words()
    rng=random.Random(0x48c297190);records=[];coverage=set();branches={};steps=0;cases=[]
    def vec(scale=1):return [f32(rng.uniform(-scale,scale)) for _ in range(3)]
    def body(i):
        angle=rng.uniform(-3,3);c=f32(math.cos(angle));s=f32(math.sin(angle))
        m=[c,0,-s,0,0,1,0,0,s,0,c,0]+vec(20)+[1]
        if i%4==0:
            for j in (0,1,2,4,5,6,8,9,10):m[j]=f32(m[j]+rng.uniform(-.1,.1))
        mass=f32(rng.uniform(1,2000))
        # Deliberately not always reciprocal: stored inverse mass is authoritative.
        inverse=f32(rng.uniform(.0001,.01)) if i%3==0 else f32(1/mass)
        return m+[mass,inverse]+vec(100)+vec(50)
    for i in range(COUNT):
        flags=i%32;scenario=i//32;other=flags&1;impact=(flags>>1)&1;other_spring=(flags>>2)&1
        custom=(flags>>3)&1;yaw=(flags>>4)&1
        n=vec();length=math.sqrt(sum(x*x for x in n));n=[f32(x/length) for x in n]
        point=vec(20);velocity=vec(30);penetration=f32(rng.uniform(-1,3))
        coefficients=[2,10]+[f32(rng.uniform(-10,100)) for _ in range(4)]
        a=body(i);b=body(i+1);name='seeded-ordinary-finite'
        if scenario<32:
            n=[0,1,0];velocity=[0,-2,0];coefficients=[2,10,8,4,16,2];penetration=1
            if scenario in (0,1,2):
                penetration=fp(0x40000000+scenario-1);name='penetration-upper-neighbor'
            elif scenario in (3,4,5):
                velocity=[0,fp(0x41200000+scenario-4),0];name='speed-upper-neighbor'
            elif scenario in (6,7,8):
                velocity=[0,-fp(0x41200000+scenario-7),0];name='speed-lower-neighbor'
            elif scenario in (9,10,11):
                # Both selected pairs cross the nonnegative magnitude boundary.
                coefficients=[2,10,8,4,8,4];velocity=[0,2,0]
                penetration=fp(0x3f800000+scenario-10);name='force-zero-neighbor'
            elif scenario in (12,13,14):
                a[16]=fp(0x447a0000+scenario-13);b[16]=1000
                a[17]=.003;b[17]=.007;name='mass-equality-neighbor'
            elif scenario==15:point=[0,0,0];name='zero-point-sentinel'
            elif scenario==16:point=[-0.,0,-0.];name='signed-zero-point-sentinel'
            elif scenario==17:penetration=-1;name='negative-depth-no-lower-clamp'
            elif scenario==18:coefficients[0]=-1;name='negative-penetration-limit'
            elif scenario==19:coefficients[1]=-10;name='negative-speed-limit-order'
            elif scenario==20:n=[0,0,0];name='zero-normal'
            elif scenario==21:n=[-0.,-1,-0.];name='signed-zero-normal'
            elif scenario==22:a[16]=b[16]=0;name='zero-masses'
            elif scenario==23:a[16]=-2;b[16]=-1;name='negative-finite-masses'
            elif scenario==24:a[17]=b[17]=0;name='zero-inverse-masses'
            elif scenario==25:coefficients[2:]=[0,0,0,0];name='zero-coefficients'
            elif scenario==26:velocity=[0,0,0];name='zero-speed'
            elif scenario==27:coefficients[0]=0;name='zero-penetration-limit'
            elif scenario==28:coefficients[1]=0;name='zero-speed-limit'
            elif scenario==29:penetration=-0.;name='signed-zero-penetration'
            elif scenario==30:point=[0,1,0];name='nonzero-point-simple-arm'
            elif scenario==31:a[17]=-.003;b[17]=-.007;name='negative-finite-inverse-masses'
        inputs=n+point+[penetration,other]+velocity+vec(2)+vec(2)+[f32(rng.uniform(-2,2))]+coefficients+[impact,other_spring,custom,yaw]+a+b
        o=SpringOriginal(words=words);initialize(o,inputs);o.run(0x297190)
        values=o.vec(0x1ff0000-0x90+0x50)+o.vec(0x11000+0x180)+o.vec(0x11000+0x190)
        values+=(o.vec(0x12000+0x180)+o.vec(0x12000+0x190)) if other else [0]*6
        assert len(values)==OUTPUT_WORDS and all(math.isfinite(v) for v in values)
        assert not o.source_reads
        records.append([bits(v) for v in inputs+values]);steps+=o.steps;coverage.update(o.coverage)
        for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
        cases.append({'case':i,'scenario':name,'flags':[other,impact,other_spring,custom,yaw]})
    for pc in (0x2971cc,0x2971d4,0x2971e0,0x297200,0x29721c,0x297328):assert branches[pc]=={False,True}
    width=INPUT_WORDS+OUTPUT_WORDS
    raw=struct.pack('<II',COUNT,width)+b''.join(struct.pack('<'+str(width)+'I',*row) for row in records)
    (OUT/'spring-force-fixtures.bin').write_bytes(raw)
    report=dict(cases=COUNT,input_words=INPUT_WORDS,output_words=OUTPUT_WORDS,steps=steps,source_reads=0,
        original_words_sha256=hashes,fixture_sha256=sha(raw),no_original_words_patched=True,
        arithmetic='Original-word execution, individually rounded ordinary finite IEEE binary32; not physical EE exceptional modes',
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},case_labels=cases)
    (OUT/'spring-force-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','case_labels')}))
if __name__=='__main__':main()
