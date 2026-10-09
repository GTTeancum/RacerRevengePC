"""Independent 10E080 one-pair leaf instruction fixtures, no RAM backing.

Original helpers run unchanged. Inputs are small explicit arrays; omitted stack
edge slots remain uninitialized and abort execution at a documented boundary.
No type2 oracle, native result, helper stub, instruction patch or read fallback.
"""
import sys
sys.dont_write_bytecode=True
import copy,hashlib,itertools,json,math,random,struct
from pathlib import Path
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits,fp,f32
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/type3Leaf48P'
MAP=ROOT/'evidence/type3Query48P/map'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
PINS={'mesh_walk':'addcba4fd67f048e49d4ef76a33848dd5f7462853af38f2f38d9e438006d7737',
      'vector_constructor':'e0d8c70d997a0996626a12d2184e4cbbfd221c249fea4f54d3c5e505dec51dfd',
      'vector_dot_normalize_vertices':'d3c25b2e05e4be07647479c89a18c1a7dc02ecbffb44b5c60019fcb8bc948662',
      'vector_scale':'2b6b27055de73efe24887d4b776dca7e761633f8538c49a8f625c3af51db5694'}
SHAPE,TREE,RESULT,DESC,FACE,VERTS,NORMALS=range(0x10000,0x17000,0x1000)
START,DIRECTION,LENGTH,PAIR_NORMAL,PAIR_POINT,HIT,FRACTION=range(0x17000,0x1e000,0x1000)
WIDTH,INPUT=78,53

def sha(b):return hashlib.sha256(b).hexdigest()
def put(o,a,values):
    for i,x in enumerate(values):o.write(a+4*i,x)
def get(o,a,count):return [o.read(a+4*i) for i in range(count)]
def words():
    proof=json.loads((MAP/'original-words.json').read_text());assert proof['elf']['sha256']==ELF_SHA
    result={};blocks=[]
    for b in proof['blocks']:
        if b['name'] not in PINS:continue
        rows=b['instructions'];raw=b''.join(struct.pack('<I',r['word']) for r in rows)
        assert sha(raw)==b['sha256']==PINS[b['name']] and raw==(MAP/(b['name']+'.bin')).read_bytes()
        assert [r['pc'] for r in rows]==list(range(b['start'],b['end_exclusive'],4))
        result.update((r['pc'],r['word']) for r in rows);blocks.append(b)
    assert len(blocks)==len(PINS)
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,blocks=blocks),indent=2)+'\n')
    return result

class LeafOriginal(SpringOriginal):
    def __init__(self,w):
        super().__init__(words=w);self.allowed_source_addresses=set();self.hit=None
        self.executing=False;self.reads=set();self.writes=set()
    def read(self,a,size=4):
        if self.executing:self.reads.update(range(a,a+size))
        return super().read(a,size)
    def write(self,a,v,size=4):
        if self.executing:self.writes.update(range(a,a+size))
        return super().write(a,v,size)
    def execute(self,pc):
        if pc==0x10ec18:self.hit=self.g[2][0]
        w=self.words[pc]
        if w>>26==0x3f:
            # MIPS SD, the actual instruction used to pass helper argument9.
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
            rs=(w>>21)&31;rt=(w>>16)&31;imm=w&65535;imm=imm-65536 if imm&32768 else imm
            self.write((self.g[rs][0]+imm)&0xffffffff,self.g[rt][0]|(self.g[rt][1]<<32),8)
            return None
        return super().execute(pc)

def output(o):
    return get(o,FRACTION,1)+get(o,PAIR_NORMAL,4)+get(o,PAIR_POINT,4)+get(o,HIT,1)+get(o,RESULT+0x74,1)+get(o,RESULT+0x20,12)+get(o,RESULT+0x60,1)

def execute(w,c):
    o=LeafOriginal(w);i=c['input'];assert len(i)==INPUT
    put(o,VERTS,i[:9]);put(o,NORMALS,i[9:18]);has,mode=i[18:20]
    put(o,DESC,[VERTS,NORMALS if has else 0,0]);put(o,TREE,[0,FACE]);put(o,FACE,[0x00010000,0x00000002,0x7fc00000,0x8000ffff,0x01234567])
    o.write(0x3dfe60,DESC);o.write(0x3df5b0,mode)
    put(o,START,i[20:24]);put(o,DIRECTION,i[24:28]);put(o,LENGTH,i[28:29])
    for off,val in ((0,1),(0xc,START),(0x10,DIRECTION),(0x14,LENGTH)):o.write(SHAPE+off,val)
    for off,val in ((0x54,PAIR_NORMAL),(0x58,PAIR_POINT),(0x68,HIT),(0x6c,FRACTION)):o.write(RESULT+off,val)
    put(o,FRACTION,i[29:30]);put(o,PAIR_NORMAL,i[30:34]);put(o,PAIR_POINT,i[34:38]);put(o,HIT,i[38:39])
    put(o,RESULT+0x74,i[39:40]);put(o,RESULT+0x20,i[40:52]);put(o,RESULT+0x60,i[52:53])
    for r,v in zip(range(4,9),(SHAPE,0x8000,TREE,RESULT,1)):o.g[r][0]=v
    initialized=set(o.overlay);o.executing=True;boundary=None
    try:
        returned=o.run(0x10e080);assert returned==1 and o.hit in (0,1)
        status=o.hit
    except ValueError as e:
        # These exact loads read the original helper's missing edge pointer.
        if o.pc not in (0x1108b4,0x1108b8,0x110d80,0x110d84) or 'uninitialized fixture read' not in str(e):raise
        boundary=str(e);status=3
    o.executing=False
    assert o.data==b'' and not o.source_reads
    assert not (o.reads & set(range(FACE+6,FACE+20))), 'leaf read packed face plane'
    result=output(o) if boundary is None else i[29:]
    assert len(result)==24
    return i+[status]+result,o,dict(name=c['name'],status=status,stop=boundary,steps=o.steps,initialized_bytes=len(initialized),read_bytes=len(o.reads),written_bytes=len(o.writes))

def cases():
    result=[];rng=random.Random(0x48e080)
    base=[[-2.,0.,-2.],[0.,0.,2.],[2.,0.,-2.]]
    ns=[[.125,1.,0.],[0.,1.,.25],[-.125,1.,0.]]
    def case(name,vs=base,normals=None,mode=0,start=(.25,1.,.125),direction=(0.,-1.,0.),length=2.,prior=-1.,selected=1.,count=0):
        i=[bits(f32(x)) for x in sum(vs,[])+sum(normals or [[0.]*3]*3,[])]+[int(normals is not None),mode]
        i += [bits(x) for x in start]+[0x7fc12345]+[bits(x) for x in direction]+[0x80000000]+[bits(length)]
        i += [bits(prior)]+[bits(x) for x in (.125,.75,-.5)]+[0xff812345]+[bits(x) for x in (101.,102.,103.)]+[0x80000000]+[0x12345678]
        i += [bits(selected)]+[bits(x) for x in range(80,92)]+[count]
        result.append(dict(name=name,input=i))
    for perm in itertools.permutations(range(3)):
        for axis in range(3):
            rotate=lambda v:list(v[axis:])+list(v[:axis])
            for mode in (0,1):
                for x,z in ((.25,.125),(-.5,-.25),(4.,0.),(0.,-2.),(-2.,-2.),(0.,2.)):
                    case(f'permutation-{perm}-axis{axis}-mode{mode}-point{x},{z}',[rotate(base[j]) for j in perm],[rotate(ns[j]) for j in perm] if mode else None,mode,rotate([x,1.,z]),rotate([0.,-1.,0.]))
    for prior in (-2.,-1.,-.0,0.,.25,.5,.75,1.,2.):
        for selected in (-1.,0.,.25,.5,.75,1.,2.):
            case(f'fraction-state-{prior}-{selected}',prior=prior,selected=selected,count=0xffffffff)
    for y,length in ((0.,2.),(2.,2.),(2.001,2.),(-.001,2.),(1.,0.),(1.,-1.)):
        case(f'endpoint-{y}-{length}',start=(.25,y,.125),length=length)
    case('parallel',direction=(1.,0.,0.));case('degenerate',vs=[[0.,0.,0.]]*3)
    for mode in (0,1,2,0xffffffff):case(f'no-normal-mode{mode}',mode=mode)
    case('present-normals-mode0',normals=ns)
    case('nonimproving-interpolation-boundary',normals=ns,mode=1,start=(-2.,1.,-2.),prior=.25)
    for mode in (2,0xffffffff):
        for prior in (.25,.5):case(f'nonimproving-other-mode-{mode}-{prior}',normals=ns,mode=mode,prior=prior)
        case(f'miss-other-mode-{mode}',normals=ns,mode=mode,start=(4.,1.,0.))
    for scale in (.125,.5,2.,3.25,-2.):case(f'nonunit-direction-{scale}',direction=(0.,-scale,0.))
    for k in range(600):
        vs=[[f32(rng.uniform(-100,100)) for _ in range(3)] for _ in range(3)]
        a=[vs[1][j]-vs[0][j] for j in range(3)];b=[vs[2][j]-vs[0][j] for j in range(3)]
        n=[a[(j+1)%3]*b[(j+2)%3]-a[(j+2)%3]*b[(j+1)%3] for j in range(3)]
        norm=math.sqrt(sum(x*x for x in n));n=[x/norm for x in n]
        weights=[.2,.3,.5] if k%4 else [-.3,.6,.7]
        p=[sum(vs[t][j]*weights[t] for t in range(3)) for j in range(3)]
        normals=[[f32(n[j]+rng.uniform(-.3,.3)) for j in range(3)] for _ in range(3)] if k%2 else None
        case(f'random3d-{k}',vs,normals,int(normals is not None),[f32(p[j]+n[j]*10) for j in range(3)],[f32(-x) for x in n],20.,(-1.,.25,.5,.75)[k%4],(.25,.75,1.)[k%3])
    return result

def main():
    OUT.mkdir(parents=True,exist_ok=True);w=words();rows=[];labels=[];coverage=set();branches={};steps=0
    for c in cases():
        row,o,label=execute(w,c);rows.append(row);labels.append(label);coverage.update(o.coverage);steps+=o.steps
        for pc,outcomes in o.branches.items():branches.setdefault(pc,set()).update(outcomes)
    for pc in (0x10e290,0x10e2a0,0x10e31c,0x10e338,0x10eaec,0x10eb04,0x10eb34,0x10eb44,0x10ec18,0x10ec50):
        assert branches[pc]=={False,True},hex(pc)
    raw=struct.pack('<II',len(rows),WIDTH)+b''.join(struct.pack('<'+str(WIDTH)+'I',*r) for r in rows)
    (OUT/'leaf-fixtures.bin').write_bytes(raw)
    report=dict(cases=len(rows),words_per_case=WIDTH,input_words=INPUT,steps=steps,data_bytes=0,source_reads=0,
        no_original_words_patched=True,no_helper_stubs=True,no_native_expected_values=True,elf_sha256=ELF_SHA,
        fixture_sha256=sha(raw),original_words_sha256=sha((OUT/'original-words.json').read_bytes()),
        instruction_entry='0010E080',normal_exit='0010F570 -> sentinel',
        boundary_policy='Stop on authentic uninitialized edge-pointer load; native unsupported status leaves all state unchanged.',
        domain='Individually rounded ordinary finite IEEE binary32; float vertex/normal arrays, mode0/1; not all physical EE/VU exceptions.',
        status_counts={str(k):sum(r[INPUT]==k for r in rows) for k in range(4)},
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},cases_detail=labels)
    (OUT/'leaf-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','cases_detail')}))
if __name__=='__main__':main()
