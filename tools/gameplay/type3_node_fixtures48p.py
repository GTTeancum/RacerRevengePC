"""Original 10E080 mode1 sphere/ray-box predicate instruction fixtures.

Start at the authentic function entry; stop at observed predicate boundaries,
without executing or replacing recursion. No guest RAM, stubs, instruction
patches, native expected values, or type2/slab oracle are used.
"""
import sys
sys.dont_write_bytecode=True
import hashlib,itertools,json,math,random,struct
from pathlib import Path
from query_oracle48h import Original,bits,fp,f32
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/type3Node48P'
MAP=ROOT/'evidence/type3Query48P/map'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
MESH_SHA='addcba4fd67f048e49d4ef76a33848dd5f7462853af38f2f38d9e438006d7737'
SHAPE,TREE,NODE,START,DIRECTION,LENGTH=range(0x10000,0x16000,0x1000)
DIVIDE_PCS=(0x10ef54,0x10f020,0x10f0ec,0x10f1b8,0x10f284,0x10f350)
FACE_NAMES=('positive-x','negative-x','positive-z','negative-z','positive-y','negative-y')
def sha(b):return hashlib.sha256(b).hexdigest()
def put(o,a,values):
    for i,x in enumerate(values):o.write(a+4*i,x)
def words():
    proof=json.loads((MAP/'original-words.json').read_text());assert proof['elf']['sha256']==ELF_SHA
    block=next(b for b in proof['blocks'] if b['name']=='mesh_walk')
    rows=block['instructions'];raw=b''.join(struct.pack('<I',r['word']) for r in rows)
    assert sha(raw)==block['sha256']==MESH_SHA and raw==(MAP/'mesh_walk.bin').read_bytes()
    assert [r['pc'] for r in rows]==list(range(block['start'],block['end_exclusive'],4))
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,blocks=[block]),indent=2)+'\n')
    return {r['pc']:r['word'] for r in rows}
class PredicateObserved(Exception):pass
class NodeOriginal(Original):
    def __init__(self,w,kind):
        super().__init__(data=b'',words=w);self.allowed_source_addresses=set()
        self.kind=kind;self.observed=None;self.stop=None;self.divisions=[];self.reads=set();self.executing=False
    def read(self,a,size=4):
        if self.executing:self.reads.update(range(a,a+size))
        return super().read(a,size)
    def execute(self,pc):
        if self.kind=='sphere' and pc in (0x10ee48,0x10f53c):
            self.observed=int(pc==0x10f53c);self.stop=pc;raise PredicateObserved()
        if self.kind=='ray' and pc==0x10f400:
            assert self.g[18][0] in (0,1)
            self.observed=1-self.g[18][0];self.stop=pc;raise PredicateObserved()
        if pc in DIVIDE_PCS:
            denominator=self.f[0];numerator=self.f[2]
            result=super().execute(pc)
            self.divisions.append(dict(pc=f'{pc:08X}',numerator=f'{bits(numerator):08X}',denominator=f'{bits(denominator):08X}',result=f'{bits(self.f[0]):08X}',classification='nan' if math.isnan(self.f[0]) else 'infinity' if math.isinf(self.f[0]) else 'finite'))
            return result
        return super().execute(pc)
def execute(w,kind,c):
    o=NodeOriginal(w,kind);i=c['input']
    if kind=='sphere':
        assert len(i)==10;local=i[:3];radius=i[3];center=i[4:7];half=i[7:10]
    else:
        assert len(i)==15;center=i[9:12];half=i[12:15];local=center;radius=0
        put(o,START,i[:4]);put(o,DIRECTION,i[4:8]);put(o,LENGTH,i[8:9])
    put(o,NODE+0xc,center+half);put(o,TREE,[NODE])
    o.write(SHAPE,1 if kind=='ray' else 0);put(o,SHAPE+0x20,local);o.write(SHAPE+0x40,radius)
    if kind=='ray':
        for off,value in ((0xc,START),(0x10,DIRECTION),(0x14,LENGTH)):o.write(SHAPE+off,value)
    for register,value in zip(range(4,9),(SHAPE,0,TREE,0,1)):o.g[register][0]=value
    initial=set(o.overlay);o.executing=True
    try:o.run(0x10e080);raise AssertionError('predicate boundary not reached')
    except PredicateObserved:pass
    assert o.data==b'' and not o.source_reads and o.observed in (0,1)
    assert not any(0x10f408<=pc<=0x10f438 or 0x10f46c<=pc<=0x10f538 for pc in o.coverage)
    label=dict(name=c['name'],status=o.observed,stop=f'{o.stop:08X}',steps=o.steps,initialized_bytes=len(initial),read_bytes=len(o.reads),divisions=o.divisions)
    return i+[o.observed],o,label

def sphere_cases():
    result=[];rng=random.Random(0x48eedd)
    def case(name,local=(0.,0.,0.),radius=0.,center=(0.,0.,0.),half=(1.,1.,1.)):
        result.append(dict(name=name,input=[bits(f32(x)) for x in (*local,radius,*center,*half)]))
    case('inside');case('expanded-corner-inclusive',local=(2.,2.,2.),radius=1.)
    for axis,side in itertools.product(range(3),(-1,1)):
        for distance in (0.,1.,fp(0x3f800001),2.,fp(0x40000001)):
            for radius in (0.,1.):
                p=[0.]*3;p[axis]=side*distance
                case(f'axis{axis}-side{side}-distance{bits(distance):08X}-radius{radius}',local=p,radius=radius)
    for z in (0.,-0.):case(f'zero-extent-{bits(z):08X}',local=(z,z,z),half=(z,z,z),radius=z)
    largest=fp(0x7f7fffff);tiny=fp(1)
    case('overflow-difference-separated',local=(-largest,0.,0.),center=(largest,0.,0.))
    case('overflow-difference-and-bound-inclusive',local=(-largest,0.,0.),radius=largest,center=(largest,0.,0.),half=(largest,1.,1.))
    case('subnormal-inclusive',local=(tiny,-tiny,tiny),half=(tiny,tiny,tiny))
    for k in range(400):
        center=[f32(rng.uniform(-100,100)) for _ in range(3)]
        half=[f32(rng.uniform(0,20)) for _ in range(3)];radius=f32(rng.uniform(0,20))
        local=[f32(center[j]+rng.uniform(-45,45)) for j in range(3)]
        case(f'random-sphere-{k}',local,radius,center,half)
    return result

def ray_cases():
    result=[];rng=random.Random(0x48ef38)
    def case(name,start=(0.,0.,0.),direction=(0.,0.,0.),length=1.,center=(0.,0.,0.),half=(1.,1.,1.)):
        i=[bits(f32(x)) for x in start]+[0x7fc12345]+[bits(f32(x)) for x in direction]+[0xff812345]
        i += [bits(f32(x)) for x in (length,*center,*half)]
        result.append(dict(name=name,input=i))
    for length in (-2.,-0.,0.,1.,2.):
        case(f'inside-length-{bits(length):08X}',length=length)
        case(f'boundary-inside-length-{bits(length):08X}',start=(1.,-1.,1.),length=length)
    for axis,side in itertools.product(range(3),(-1.,1.)):
        for length in (-1.,-0.,0.,fp(0x3f7fffff),1.,fp(0x3f800001),2.,3.):
            start=[0.]*3;start[axis]=2.*side;direction=[0.]*3;direction[axis]=-side
            case(f'axis{axis}-side{side}-length{bits(length):08X}',start,direction,length)
        for z in (0.,-0.):
            start=[0.]*3;start[axis]=2.*side
            case(f'parallel-axis{axis}-side{side}-zero{bits(z):08X}',start,[z]*3,4.)
    for axis in range(3):
        other=(axis+1)%3
        for edge in (-1.,1.,fp(0x3f800001)):
            start=[0.]*3;start[axis]=2.;start[other]=edge;direction=[0.]*3;direction[axis]=-1.
            case(f'edge-axis{axis}-coord{bits(edge):08X}',start,direction,1.)
    # Grid includes face-plane 0/0 and signed-zero divide cases, crossing edges,
    # all direction sign choices, and later-face paths after earlier rejections.
    for k,start in enumerate(itertools.product((-2.,-1.,0.,1.,2.),repeat=3)):
        for d,direction in enumerate(itertools.product((-1.,0.,1.),repeat=3)):
            case(f'grid-{k}-{d}',start,direction,2.)
    for z in (0.,-0.):
        case(f'zero-box-inside-{bits(z):08X}',start=(z,z,z),half=(z,z,z),length=-1.)
        case(f'zero-box-cross-{bits(z):08X}',start=(1.,0.,0.),direction=(-1.,z,z),half=(z,z,z))
    largest=fp(0x7f7fffff);tiny=fp(1)
    case('finite-direction-divide-overflow',start=(2.,0.,0.),direction=(-tiny,0.,0.))
    case('finite-face-add-overflow',start=(-largest,0.,0.),direction=(largest,0.,0.),center=(largest,0.,0.),half=(largest,1.,1.),length=2.)
    case('finite-face-sub-overflow',start=(largest,0.,0.),direction=(-largest,0.,0.),center=(-largest,0.,0.),half=(largest,1.,1.),length=2.)
    case('subnormal-time',start=(tiny,0.,0.),direction=(-largest,0.,0.),half=(0.,0.,0.),length=0.)
    case('negative-time-underflow-to-minus-zero',start=(tiny,0.,0.),direction=(largest,0.,0.),half=(0.,0.,0.),length=-0.)
    case('subnormal-direction',start=(tiny*2,0.,0.),direction=(-tiny,0.,0.),half=(tiny,tiny,tiny))
    for k in range(500):
        center=[f32(rng.uniform(-100,100)) for _ in range(3)];half=[f32(rng.uniform(0,20)) for _ in range(3)]
        start=[f32(center[j]+rng.uniform(-40,40)) for j in range(3)]
        direction=[f32(rng.uniform(-3,3)) for _ in range(3)]
        case(f'random-ray-{k}',start,direction,f32(rng.uniform(-1,40)),center,half)
    return result

def main():
    OUT.mkdir(parents=True,exist_ok=True);w=words();reports={}
    for kind,cases,width in (('sphere',sphere_cases(),11),('ray',ray_cases(),16)):
        rows=[];labels=[];coverage=set();branches={};steps=0;classifications={k:0 for k in ('finite','infinity','nan')}
        for c in cases:
            row,o,label=execute(w,kind,c);rows.append(row);labels.append(label);coverage.update(o.coverage);steps+=o.steps
            for pc,outcomes in o.branches.items():branches.setdefault(pc,set()).update(outcomes)
            for division in o.divisions:classifications[division['classification']]+=1
        raw=struct.pack('<II',len(rows),width)+b''.join(struct.pack('<'+str(width)+'I',*r) for r in rows)
        (OUT/(kind+'-fixtures.bin')).write_bytes(raw)
        report=dict(cases=len(rows),words_per_case=width,steps=steps,fixture_sha256=sha(raw),status_counts={str(k):sum(r[-1]==k for r in rows) for k in (0,1)},division_result_counts=classifications,coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},cases_detail=labels)
        (OUT/(kind+'-fixtures.json')).write_text(json.dumps(report,indent=2)+'\n');reports[kind]={k:v for k,v in report.items() if k not in ('coverage','branches','cases_detail')}
    assert all(reports['ray']['division_result_counts'].values())
    # Every original condition at this boundary has both branch outcomes.
    for kind,pcs in [('sphere',(0x10ede0,0x10ee10,0x10ee40)),('ray',(0x10eef8,0x10ef10,0x10ef28)+tuple(pc for base in DIVIDE_PCS for pc in (base+20,base+40,base+100,base+160)))]:
        branches=json.loads((OUT/(kind+'-fixtures.json')).read_text())['branches']
        for pc in pcs:assert branches[f'{pc:08X}']==[False,True],(kind,hex(pc),branches.get(f'{pc:08X}'))
    report=dict(format='type3-mode1-node48p-v1',data_bytes=0,source_reads=0,no_original_words_patched=True,no_helper_stubs=True,no_native_expected_values=True,no_recursion_executed=True,no_mode0_claim=True,elf_sha256=ELF_SHA,original_words_sha256=sha((OUT/'original-words.json').read_bytes()),interpreter_sha256=sha((ROOT/'tools/gameplay/query_oracle48h.py').read_bytes()),instruction_entry='0010E080',sphere_observation='0010EE48 pass / 0010F53C reject, before executing observation instruction',ray_observation='0010F400 before branch/recursion; s2==1 pass, s2==0 reject',domain='Finite IEEE binary32 source XYZ/length, nonnegative finite radius/half; ignored W arbitrary. Individually rounded arithmetic with gradual underflow, masked exceptions; transient infinity/NaN preserved. Not physical EE exceptional behavior.',status_enum={'0':'Overlap','1':'Separated','2':'InvalidInput (native guard only)'},fixtures=reports)
    (OUT/'node-fixtures.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
if __name__=='__main__':main()
