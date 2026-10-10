"""Full original295630 supplied-list detector proof; no native-derived expectations.

Only explicitly initialized byte overlays and immutable original instructions.
All observations are passive. Unsupported branches stop before their work.
"""
import copy
import hashlib
import json
import struct
import sys
from pathlib import Path
sys.dont_write_bytecode=True
from query_oracle48h import bits, fp
from spring_callback_fixtures48n import STATE_OFFSETS
from spring_detector_fixtures48o import RANGES, RANGE_HASHES, DISPATCH
import type3_query_fixtures48p as prior
from type3_query_fixtures48p import (Type3Original, Q, SHAPE, RESULT, SPRING, RECORDS, LIST,
    ENDPOINTS, NORMALS, POINTS, LOCAL_START, LOCAL_DIR, FRACTIONS, HITS, LENGTHS,
    WORLD_START, WORLD_DIR, SCENE_IDS, BODY, BODY_SCENE, GP, SLOTS, IDENTITY, LEAF_TAG,
    put, get, zero)
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/springDetectorPipeline48Q'
ELF_SHA=prior.ELF_SHA
D,POINT_RECORDS,PAIRS,COUNT_CELL=0x44000,0x45000,0x46000,0x47000
SCENES=[0x48000+i*0x400 for i in range(4)]
RETURN=0xfffffff0
BOUNDARIES={0x295740:'provider',0x28baf4:'dirty-parent-scene',0x2959ec:'world-registration',
    0x295a1c:'world-query',0x1273a8:'compound-candidate',0x10ebf0:'alternate-normal-helper'}
STAGES={0x295944:'after_numeric',0x295978:'after_reset',0x2959d4:'before_stage',
    0x2959e4:'after_stage',0x122660:'query_entry',0x295a10:'query_return'}

def sha(raw):return hashlib.sha256(raw).hexdigest()
def floats(values):return [bits(x) for x in values]

def load_words():
    words,dispatch=prior.load_words()
    p=json.loads((ROOT/'evidence/springDetector48O/original-words.json').read_text())
    assert p['elf_sha256']==ELF_SHA
    for b,(name,lo,hi) in zip(p['blocks'],RANGES):
        assert (b['name'],b['start'],b['end'])==(name,lo,hi)
        rows=b['instructions'];assert [r['pc'] for r in rows]==list(range(lo,hi,4))
        raw=b''.join(struct.pack('<I',r['word']) for r in rows)
        assert sha(raw)==RANGE_HASHES[name]==b['sha256']
        assert raw==(ROOT/'evidence/springDetector48O'/(name+'.bin')).read_bytes()
        for r in rows:
            assert r['pc'] not in words or words[r['pc']]==r['word']
            words[r['pc']]=r['word']
    dispatch.update(DISPATCH)
    if '--frozen' not in sys.argv:
        elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
        for pc,w in list(words.items())+list(dispatch.items()):
            assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==w,hex(pc)
    return words,dispatch

class PipelineOriginal(Type3Original):
    def __init__(self,words,identities):
        super().__init__(words,identities)
        self.stages=[];self.query_returned=None;self.body_scene_reads=[]
    def detector(self):
        return dict(bounds=get(self,D+0x40,8),pairs=get(self,PAIRS,32),
            point_records=[get(self,POINT_RECORDS+i*96,24) for i in range(4)],
            scenes=[dict(world=get(self,a+0x100,16),scene_local=get(self,a+0xc0,16),
                         flags=self.read(a+0x190,1),parent=self.read(a+0x180)) for a in SCENES],
            query_type=self.read(Q+0x88),spring_state=self.state(),
            staged=get(self,ENDPOINTS,32),metadata=get(self,SCENE_IDS,8),
            static_flags=[self.read(GP-0x7360,1),self.read(GP-0x735c,1)],
            static_words=[self.read(0x443590),self.read(0x443598)],
            body_scene=self.read(BODY+0x288),body_world=get(self,self.read(BODY+0x288)+0x100,16))
    def execute(self,pc):
        if pc in STAGES:self.stages.append(dict(stage=STAGES[pc],pc=f'{pc:08X}',detector=self.detector(),pair=self.pair(),prepared=self.prepared()))
        if pc==0x295a10:self.query_returned=self.g[2][0]
        if pc==0x294bb0:self.body_scene_reads.append(dict(candidate=self.identity(self.g[5][0]),scene=self.read(BODY+0x288),world=get(self,self.read(BODY+0x288)+0x100,16)))
        return super().execute(pc)

def initialize(words,dispatch,meshes,c):
    cs=c['candidates'];q=c['query'];addresses={0:0,Q:100}
    addresses.update({0x20000+i*0x1000:x['id'] for i,x in enumerate(cs)})
    reverse={v:k for k,v in addresses.items()};m=PipelineOriginal(words,addresses);m.g[28][0]=GP;m.record_slots=len(c['initial_records'])
    for a,n in ((Q,0x100),(SHAPE,0x60),(RESULT,0x80),(SPRING,0x140)):zero(m,a,n)
    for a,n in ((ENDPOINTS,32),(NORMALS,16),(POINTS,16),(LOCAL_START,16),(LOCAL_DIR,16),(WORLD_START,16),(WORLD_DIR,16),(SCENE_IDS,8)):
        put(m,a,[bits(50.+i) for i in range(n)])
    put(m,RESULT,[bits(80.+i) for i in range(20)])
    put(m,HITS,c['initial_flags']);put(m,FRACTIONS,c['initial_fractions']);put(m,LENGTHS,c['initial_lengths'])
    for a,v in dispatch.items():m.write(a,v)
    m.vector(0x3e02c0,[0.,0.,0.,1.])
    m.write(GP-0x7ff0,q['collision_tag'],1);m.write(GP-0x7fec,q['group_tag'],1);m.write(GP-0x7f14,0,1)
    for a,v in zip((0x3e0728,0x3e0718,0x3e0720),c['masks']):m.write(a,v)
    def write_filter(obj,f,slot):
        m.write(obj+0xa4,f['category']);m.write(obj+0xb8,f['accept_mask']);m.write(obj+0xf8,f['flags'],1);m.write(obj+0x31,f['flags31'],1)
        for k,off in enumerate((0xbc,0xc8)):
            entry=f['identity_lists'][k];m.write(obj+off,entry['mode']);m.write(obj+off+4,slot+k*0x100);m.write(obj+off+8,len(entry['ids']))
            put(m,slot+k*0x100,[reverse[x] for x in entry['ids']])
        m.write(obj+0xd4,f['category_mode']);m.write(obj+0xd8,slot+0x200);m.write(obj+0xdc,len(f['category_masks']));put(m,slot+0x200,f['category_masks'])
    def sphere(obj,s):m.vector(obj+0x60,s[:3]);m.storef(obj+0x80,s[3])
    write_filter(Q,q['filter'],0x30000)
    for off,v in ((0x88,3),(0x8c,SHAPE),(0x90,RESULT),(0x9c,SPRING),(0xa0,BODY),(0xe4,0x294bb0 if q['callback'] else 0),(0xf4,reverse[q['initial_current_candidate']])):m.write(Q+off,v)
    put(m,Q+0x40,q['copied40']);put(m,Q+0x50,q['copied50'])
    for off,v in ((0,len(c['pairs'])),(4,SCENE_IDS),(8,ENDPOINTS),(0xc,LOCAL_START),(0x10,LOCAL_DIR),(0x14,LENGTHS),(0x18,WORLD_START),(0x1c,WORLD_DIR),(0x44,c['mode'])):m.write(SHAPE+off,v)
    for i,pair in enumerate(c['pairs']):put(m,ENDPOINTS+i*32,pair[0]+pair[1])
    for i,matrix in enumerate(c['scene_matrices']):
        a=0xd0000+i*0x200;m.write(SCENE_IDS+i*4,a if matrix else 0)
        if matrix:m.vector(a+0x100,matrix)
    for off,v in ((0x50,0xdeadbeef),(0x54,NORMALS),(0x58,POINTS),(0x5c,SLOTS),(0x60,c['initial_count']),(0x64,0x87654321),(0x68,HITS),(0x6c,FRACTIONS),(0x70,0xabcdef01),(0x74,bits(.125)),(0x78,0x13572468),(0x7c,0x24681357)):m.write(RESULT+off,v)
    for off,v in zip(STATE_OFFSETS,c['initial_state']):m.write(SPRING+off,reverse[v] if off==0x130 else v)
    m.write(SPRING+0x8c,RECORDS);m.write(BODY+0x288,BODY_SCENE);m.vector(BODY_SCENE+0x100,c['body_world'])
    for i,row in enumerate(c['initial_records']):put(m,RECORDS+i*96,row)
    for ci,item in enumerate(cs):
        obj=reverse[item['id']];target=0x50000+ci*0x20000;vbase=target+0x1000;nbase=target+0x7000;nodebase=target+0xc000;facebase=target+0x16000;scene=0xb0001+ci*0x1000
        mesh=meshes[item['mesh']];zero(m,obj,0x100);zero(m,target,0x120)
        write_filter(obj,item['filter'],0x31000+ci*0x1000);sphere(obj,item['sphere']);m.write(obj+2,item['type_tag'],1)
        for off,v in ((0x88,item['shape_tag']),(0x8c,target),(0x9c,scene if item['has_scene'] else 0)):m.write(obj+off,v)
        for i,v in enumerate(bytes.fromhex(item['name_hex'])):m.write(scene+0x38+i,v,1)
        put(m,obj+0x40,item['copied40']);put(m,obj+0x50,item['copied50']);m.vector(target,item['world'])
        for off,v in ((0x100,vbase),(0x104,nbase if mesh['normals'] else 0),(0x108,0),(0x114,nodebase),(0x118,facebase)):m.write(target+off,v)
        root=mesh.get('root',0);m.write(target+0x10c,(0x8000 if root&LEAF_TAG else 0)|(root&0x7fff),2)
        for i,v in enumerate(mesh['vertices']):m.vector(vbase+i*12,v)
        for i,v in enumerate(mesh['normals']):m.vector(nbase+i*12,v)
        for i,node in enumerate(mesh['nodes']):
            a=nodebase+i*36;m.storef(a,node['volume']);m.vector(a+12,node['center']);m.vector(a+24,node['half'])
            for k,ch in enumerate(node['children']):m.write(a+8+k*2,(0x8000 if ch&LEAF_TAG else 0)|(ch&0x7fff),2)
        for i,face in enumerate(mesh['faces']):
            a=facebase+i*20;m.storef(a+8,face['d'])
            for k,x in enumerate(face['vertices']):m.write(a+k*2,x,2)
            for k,x in enumerate(face['normal']):m.write(a+12+k*2,x,2)
        if item['shape_tag']==1:
            record_worlds=item.get('records',[item['world']]);descriptor=get(m,target+0x100,8)
            for ri,world in enumerate(record_worlds):
                record=target+0x200+ri*0x130;zero(m,record,0x130);m.vector(record,world);put(m,record+0x100,descriptor)
            m.write(target+0xe0,len(record_worlds));m.write(target+0xf0,target+0x200)
    d=c['detector']
    for off,v in ((0x30,SPRING if d['spring_present'] else 0),(0x34,0x4a000 if d['provider_present'] else 0),
                  (0x38,Q if d['query_present'] else 0),(0x60,PAIRS if d['pairs_present'] else 0),
                  (0x68,d['query_enabled']),(0x6c,d['reset']),(0x70,COUNT_CELL),(0x74,LIST if d['explicit_list'] else 0)):
        m.write(D+off,v)
    put(m,D+0x40,d['bounds']);put(m,PAIRS,d['pairs'])
    m.write(SPRING+0x48,d['provider_enabled']);m.write(SPRING+0x80,POINT_RECORDS)
    m.write(Q+0x88,d['initial_query_type']);m.write(Q+0x8c,SHAPE if d['shape_present'] else 0)
    m.write(SHAPE,d['shape_count']);put(m,ENDPOINTS,d['staged']);put(m,SCENE_IDS,d['metadata'])
    m.write(COUNT_CELL,len(c['order']));put(m,LIST,[reverse[cs[i]['id']] for i in c['order']])
    for a,v in zip((GP-0x7360,GP-0x735c),d['static_flags']):m.write(a,v,1)
    put(m,0x443590,[d['static_words'][0]]);put(m,0x443598,[d['static_words'][1]])
    for i,r in enumerate(d['point_records']):
        raw=r['raw'].copy();raw[4:8]=r['local'];raw[16]=SCENES[i]
        put(m,POINT_RECORDS+96*i,raw);put(m,SCENES[i]+0x100,r['world']);put(m,SCENES[i]+0xc0,r['scene_local'])
        m.write(SCENES[i]+0x190,r['flags'],1);m.write(SCENES[i]+0x180,r['parent'])
    binding=d['body_scene_binding'];m.write(BODY+0x288,BODY_SCENE if binding<0 else SCENES[binding])
    return m,reverse

def execute_case(words,dispatch,meshes,c):
    m,reverse=initialize(words,dispatch,meshes,c)
    initial=dict(detector=m.detector(),pair=m.pair(),prepared=m.prepared(),state=m.state(),records=m.records())
    initialized=set(m.overlay);m.g[4][0]=D;m.g[28][0]=GP;m.g[29][0]=0x1ff0000;m.g[31][0]=RETURN
    pc=0x295630
    while pc!=RETURN and pc not in BOUNDARIES:
        # Staging accepts an absent shape, but full query would dereference it.
        if pc==0x122660 and not c['detector']['shape_present']:break
        m.executing=True;target=m.execute(pc)
        if target is not None:
            if not m.annul:assert m.execute(pc+4) is None
            pc=target
        else:pc+=4
        m.executing=False
    m.executing=False
    stopped=BOUNDARIES.get(pc,'absent-query-shape' if pc==0x122660 else None)
    result=dict(detector_returned=m.g[2][0] if pc==RETURN else None,query_returned=m.query_returned,
        boundary=stopped,stop_pc=f'{pc:08X}',initial=initial,detector=m.detector(),stages=m.stages,
        after_prepare=getattr(m,'after_prepare',None),pair=m.pair(),prepared=m.prepared(),
        query_flags=m.read(Q+0xf8,1),current_candidate=m.identity(m.read(Q+0xf4)),spring_state=m.state(),records=m.records(),
        candidates=[dict(identity=x['id'],flags=m.read(reverse[x['id']]+0xf8,1),copied40=get(m,reverse[x['id']]+0x40,4),copied50=get(m,reverse[x['id']]+0x50,4)) for x in c['candidates']],
        calls=m.calls,events=m.events,callback_calls=m.callbacks,query_copied40=get(m,Q+0x40,4),query_copied50=get(m,Q+0x50,4),
        trace=m.trace,leaf_updates=m.leaf_updates,selections=m.selections,append_events=m.append_events,body_scene_reads=m.body_scene_reads,
        incomplete_dispatch=m.current if stopped in ('compound-candidate','alternate-normal-helper') else None)
    assert m.data==b'' and not m.source_reads and m.allowed_source_addresses==set()
    assert not ({0x2e83d0,0x2897b0,0x120230,0x121920,0x10f580,0x1273a8}&m.coverage)
    result['byte_audit']=dict(initialized_bytes=len(initialized),execution_read_bytes=len(m.read_bytes),execution_written_bytes=len(m.written_bytes),source_reads=0)
    return result,m

def make_case(name,order=(0,),point_count=2,shape_count=2):
    c=prior.make_case(name,order);c['scene_matrices']=[None]*8
    c['pairs']=[[[bits(.25),bits(1.),bits(.125),bits(1.)],[bits(.25),bits(-1.),bits(.125),bits(1.)]] for _ in range(4)]
    c['initial_state'][7]=point_count
    records=[]
    for i,p in enumerate(((.25,-1.,.125,77.),(.25,3.,.125,-23.),(.75,-1.,.125,99.),(.75,3.,.125,-99.))):
        records.append(dict(local=floats(p),world=floats(IDENTITY),scene_local=floats(IDENTITY),flags=1,parent=0,raw=[0x42000000+100*i+j for j in range(24)]))
    c['detector']=dict(spring_present=True,pairs_present=True,provider_present=False,provider_enabled=False,
        query_present=True,query_enabled=True,reset=True,explicit_list=True,shape_present=True,shape_count=shape_count,
        initial_query_type=0x76543210,bounds=floats([101.,102.,103.,-11.,201.,202.,203.,-12.]),
        pairs=[v for pair in c['pairs'] for endpoint in pair for v in endpoint],staged=floats([400.+i for i in range(32)]),
        metadata=[0x12340000+i for i in range(8)],point_records=records,body_scene_binding=-1,
        static_flags=[1,1],static_words=[0x13579bdf,0x2468ace0])
    # Stage zeros active metadata; unused metadata tails are deliberately invalid
    # opaque words, which must never be dereferenced by original preparation.
    return c

def make_cases():
    cases=[]
    def add(name,order=(0,),points=2,shape=2,change=None):
        c=make_case(name,order,points,shape)
        if change:change(c)
        cases.append(c);return c
    add('prepared-hit')
    add('empty-list',())
    add('geometry-miss',change=lambda c:c['candidates'][0]['world'].__setitem__(12,100.))
    for order in ((0,1),(1,0),(0,2),(0,0,0)):
        add('ordered-'+''.join(map(str,order)),order)
    add('hit-then-miss',(0,1),change=lambda c:c['candidates'][1]['world'].__setitem__(12,100.))
    for spring in (False,True):
        for query in (False,True):
            for enabled in (False,True):
                for reset in (False,True):
                    add(f'gates-spring{int(spring)}-query{int(query)}-enabled{int(enabled)}-reset{int(reset)}',change=lambda c,s=spring,q=query,e=enabled,r=reset:c['detector'].update(spring_present=s,query_present=q,query_enabled=e,reset=r))
    for n in range(1,5):
        for k in range(5):
            add(f'independent-points{n}-shape{k}',(0,1),n,k)
    for flag in (0,1,2,3,0x80,0x81,0xfe,0xff):
        for binding in (-1,0,1,3):
            def dirty(c,flag=flag,binding=binding):
                c['detector']['body_scene_binding']=binding
                for i,r in enumerate(c['detector']['point_records']):
                    r['flags']=flag;r['world'][12:15]=floats([7.,-3.,5.]);r['scene_local'][12:15]=floats([.125*i,0.,0.])
            add(f'scene-flags{flag}-body-binding{binding}',change=dirty)
    for reset in (False,True):
        for capacity in (0,1,2,4,12):
            for count in (0,1,4):
                def capacity_case(c,r=reset,cap=capacity,count=count):
                    c['detector']['reset']=r;c['initial_state'][8:10]=[count,cap]
                add(f'reset{int(reset)}-capacity{capacity}-initial{count}',(0,1),4,4,capacity_case)
    for gate in ('callback-disabled','skip-category','no-scene','filter-reject','wrong-tag','sphere-miss','initial-current'):
        def gates(c,g=gate):
            if g=='callback-disabled':c['query']['callback']=False
            elif g=='skip-category':c['candidates'][0]['filter']['category']=8
            elif g=='no-scene':c['candidates'][0]['has_scene']=False
            elif g=='filter-reject':c['query']['filter']['accept_mask']=0
            elif g=='wrong-tag':c['candidates'][0]['type_tag']=123
            elif g=='sphere-miss':c['candidates'][0]['sphere']=[1000.,0.,0.,1.]
            else:c['initial_state'][6]=11;c['query']['initial_current_candidate']=12;c['detector']['reset']=False
        add(gate,change=gates)
    for mesh in (4,5,6,8,9,10):
        add(f'geometry-mesh{mesh}',(0,1),4,4,lambda c,m=mesh:(c['candidates'][0].update(mesh=m,world=IDENTITY.copy()),c.update(mode=1 if m in (8,9) else 0)))
    for binding in (-1,0,1):
        def bound(c,binding=binding):
            c['detector']['body_scene_binding']=binding
            for r in c['detector']['point_records']:
                r['flags']=0;r['world']=floats([0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,7.,-3.,5.,1.]);r['scene_local']=floats(IDENTITY)
        add('binding-refresh-distinguishing-'+str(binding),(0,1),2,2,bound)
    contrast=add('binding-independent-old-matrix',(0,1),2,2,lambda c:bound(c,-1))
    contrast['body_world']=[0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,7.,-3.,5.,1.]
    families={
        'rotate-z':[0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'rotate-y':[0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,3.,-2.,5.,1.],
        'rotate37':[.8,.6,0.,0.,-.6,.8,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'reflection':[-1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'scale':[2.,0.,0.,0.,0.,.5,0.,0.,0.,0.,1.5,0.,3.,-2.,5.,1.],
        'shear':[1.,0.,0.,0.,.5,1.,.25,0.,.125,0.,1.,0.,3.,-2.,5.,1.]}
    for family,matrix in families.items():
        for dirty in (False,True):
            for binding in (-1,0):
                def transforms(c,matrix=matrix,dirty=dirty,binding=binding):
                    c['detector']['body_scene_binding']=binding
                    for r in c['detector']['point_records']:
                        r['scene_local' if dirty else 'world']=floats(matrix);r['flags']=0 if dirty else 1
                    for i,item in enumerate(c['candidates']):item['world']=matrix.copy()
                    if matrix[0]==2.:
                        for r in c['detector']['point_records']:r['local'][0]=r['local'][2]=0
                add(f'transform-{family}-dirty{int(dirty)}-binding{binding}',(0,1),4,4,transforms)
    for mesh in (8,9):
        add(f'distinct-normal-tie-{mesh}',(0,),2,2,lambda c,m=mesh:(c.update(mode=1),c['candidates'][0].update(mesh=m,world=IDENTITY.copy())))
    add('provider-enabled-absent',change=lambda c:c['detector'].update(provider_present=False,provider_enabled=True))
    for reset in (False,True):
        add('natural-static-initializers-reset'+str(int(reset)),change=lambda c,r=reset:c['detector'].update(static_flags=[0,0],reset=r))
    for enabled in (False,True):
        add('provider-present-disabled-query'+str(int(enabled)),change=lambda c,e=enabled:c['detector'].update(provider_present=True,provider_enabled=False,query_enabled=e))
    for n in (0,0xffffffff,0x80000000):
        add('no-pairs-inactive-pointcount-'+str(n),points=n,shape=0,change=lambda c:c['detector'].update(pairs_present=False,query_enabled=False))
    # Each probe runs only to the actual unimplemented branch, with no simulated return.
    add('boundary-provider',change=lambda c:c['detector'].update(provider_present=True,provider_enabled=True))
    add('boundary-provider-query-disabled',change=lambda c:c['detector'].update(provider_present=True,provider_enabled=True,query_enabled=False))
    add('boundary-no-pairs-provider',change=lambda c:c['detector'].update(pairs_present=False,provider_present=True,provider_enabled=True))
    add('no-pairs-provider-enabled-absent',change=lambda c:c['detector'].update(pairs_present=False,provider_present=False,provider_enabled=True,query_enabled=False))
    add('boundary-dirty-parent',change=lambda c:c['detector']['point_records'][0].update(flags=0,parent=0x4c000))
    add('prepared-opaque-parent',change=lambda c:c['detector']['point_records'][0].update(flags=1,parent=0x4c000))
    add('boundary-world-registration',change=lambda c:c['detector'].update(pairs_present=False,initial_query_type=3))
    add('boundary-world-query',change=lambda c:c['detector'].update(explicit_list=False))
    add('boundary-compound',change=lambda c:c['candidates'][0].update(shape_tag=1))
    add('boundary-alternate-normal',change=lambda c:(c.update(mode=2),c['candidates'][0].update(mesh=7)))
    add('alternate-normal-lazy-miss',change=lambda c:(c.update(mode=2),c['candidates'][0].update(mesh=7),c['candidates'][0]['world'].__setitem__(12,100.)))
    add('alternate-normal-filter-reject',change=lambda c:(c.update(mode=2),c['candidates'][0].update(mesh=7),c['query']['filter'].update(accept_mask=0)))
    add('boundary-absent-query-shape',change=lambda c:c['detector'].update(shape_present=False))
    return cases

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    source=ROOT/'evidence/type3Query48P/native-fixtures.json';source_raw=source.read_bytes()
    assert sha(source_raw)=='a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f'
    # Reuse only explicit mesh inputs. No prior expected result drives this execution.
    meshes=json.loads(source_raw)['meshes'];words,dispatch=load_words();cases=make_cases()
    coverage=set();branches={};steps=0
    for c in cases:
        try:r,m=execute_case(words,dispatch,meshes,c)
        except Exception:
            print('FAILED CASE',c['name'],file=sys.stderr);raise
        c['numeric_scope']='nonfinite-IEEE-probe' if c['detector']['shape_count']==0 and r['query_returned'] is not None else 'ordinary-finite'
        c['expected']=r;coverage.update(m.coverage);steps+=m.steps
        for pc,outcomes in m.branches.items():branches.setdefault(pc,set()).update(outcomes)
    for pc in (0x295648,0x295674,0x295684,0x295690,0x295710,0x295720,0x295968,0x29597c,0x295988,0x295994,0x2959f8,0x28bae0,0x28baec,0x11ca90,0x11cb10):
        assert branches[pc]=={False,True},f'missing branch outcome at {pc:08X}'
    by={c['name']:c for c in cases}
    assert all(c['expected']['detector_returned']==1 for c in cases if not c['expected']['boundary'])
    assert by['prepared-hit']['expected']['query_returned']==1
    assert by['geometry-miss']['expected']['query_returned']==0
    assert {c['expected']['boundary'] for c in cases if c['expected']['boundary']}==set(BOUNDARIES.values())|{'absent-query-shape'}
    for c in cases:
        d=c['detector'];r=c['expected'];n=d['shape_count']
        stages={s['stage']:s for s in r['stages']}
        if 'after_stage' in stages and d['shape_present']:
            staged=stages['after_stage']['detector'];assert staged['metadata'][:2*n]==[0]*(2*n)
            assert staged['metadata'][2*n:]==d['metadata'][2*n:]
            assert staged['staged'][:8*n]==staged['pairs'][:8*n]
            assert staged['staged'][8*n:]==d['staged'][8*n:]
        for e in r['body_scene_reads']:
            i=d['body_scene_binding'];assert e['world']==(r['detector']['scenes'][i]['world'] if i>=0 else floats(c['body_world']))
    bound=by['binding-refresh-distinguishing-0']['expected'];unbound=by['binding-refresh-distinguishing--1']['expected']
    assert bound['body_scene_reads'] and bound['initial']['detector']['body_world']!=bound['body_scene_reads'][0]['world']
    assert bound['records']==unbound['records']
    assert bound['records']!=by['binding-independent-old-matrix']['expected']['records']
    assert by['scene-flags0-body-binding3']['expected']['body_scene_reads'][0]['world']==by['scene-flags0-body-binding3']['detector']['point_records'][3]['world']
    assert by['distinct-normal-tie-8']['expected']['pair']['normals'][:3]==floats([0.,1.,0.])
    assert by['distinct-normal-tie-9']['expected']['pair']['normals'][:3]==floats([1.,0.,0.])
    assert all(c['expected']['query_returned']==1 for c in cases if c['name'].startswith('transform-'))
    fixture=dict(schema=1,state_offsets=[f'{x:X}' for x in STATE_OFFSETS],array_slots=4,meshes=meshes,cases=cases)
    raw=(json.dumps(fixture,separators=(',',':'))+'\n').encode();(OUT/'native-fixtures.json').write_bytes(raw)
    manifest=dict(elf_sha256=ELF_SHA,dispatch=dispatch,executed_words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)])
    (OUT/'executed-original-words.json').write_text(json.dumps(manifest,indent=2)+'\n')
    paths=['tools/gameplay/spring_detector_pipeline_fixtures48q.py','tools/gameplay/spring_detector_fixtures48o.py',
           'tools/gameplay/type3_query_fixtures48p.py','tools/gameplay/query_oracle48h.py','tools/gameplay/spring_fixture48j.py',
           'tools/gameplay/spring_callback_fixtures48n.py','tools/gameplay/spring_query_fixtures48n.py',
           'evidence/springDetector48O/original-words.json','evidence/type3Query48P/map/original-words.json',
           'evidence/springCallback48P/original-words.json','evidence/type3Query48P/native-fixtures.json']
    report=dict(cases=len(cases),natural_returns=sum(not c['expected']['boundary'] for c in cases),
        full_supplied_list_queries=sum(c['expected']['query_returned'] is not None for c in cases),
        query_hits=sum(c['expected']['query_returned']==1 for c in cases),query_misses=sum(c['expected']['query_returned']==0 for c in cases),
        callbacks=sum(len(c['expected']['callback_calls']) for c in cases),boundary_cases={c['name']:c['expected']['boundary'] for c in cases if c['expected']['boundary']},
        ordinary_finite_cases=sum(c['numeric_scope']=='ordinary-finite' for c in cases),nonfinite_IEEE_probes=sum(c['numeric_scope']=='nonfinite-IEEE-probe' for c in cases),
        steps=steps,unique_words=len(coverage),source_reads=0,fixture_sha256=sha(raw),fixture_bytes=len(raw),elf_sha256=ELF_SHA,
        entry=0x295630,query=0x122660,callback=0x294bb0,no_original_words_patched=True,no_calls_stubbed=True,
        no_RAM_reads_or_reconstruction=True,no_native_expected_values=True,no_uninitialized_read_fallback=True,
        geometry_provenance='Pinned48P explicit mesh input objects only; prior expected outputs are not consumed',
        numeric_domain='Individually rounded host IEEE binary32 finite geometry plus4 labeled zero-shape-count IEEE infinity probes; no physical EE/VU exceptional-mode equivalence',
        body_binding='Explicit caller input: -1 independent body_world;0..3 aliases that detector point scene. Callback reads observe scene world after authentic refresh.',
        sources=[dict(path=p,sha256=sha((ROOT/p).read_bytes())) for p in paths],
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
    (OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('sources','coverage','branches')},indent=2))
if __name__=='__main__':main()
