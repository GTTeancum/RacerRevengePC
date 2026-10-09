"""Original type3 limited-list queries using only explicitly initialized bytes.

The oracle executes immutable ELF/frozen words; instrumentation only observes.
No RAM backing, type2 shape adapter, instruction patch, helper interception,
forced return, native expected value, or uninitialized-read fallback is used.
"""
import copy
import hashlib
import json
import struct
import sys
from pathlib import Path
sys.dont_write_bytecode=True
from query_oracle48h import bits, fp, vu_minmax
from spring_callback_fixtures48n import CallbackOriginal, STATE_OFFSETS
from spring_query_fixtures48n import load_inputs, load_words as base_words, filter_default, IDENTITY, LEAF_TAG
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/type3Query48P'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
Q,SHAPE,RESULT,SPRING,RECORDS,LIST=0x10000,0x11000,0x12000,0x14000,0x15000,0x16000
ENDPOINTS,NORMALS,POINTS,LOCAL_START,LOCAL_DIR,FRACTIONS=0x17000,0x18000,0x19000,0x1a000,0x1b000,0x1c000
HITS,LENGTHS,WORLD_START,WORLD_DIR,SCENE_IDS=0x1d004,0x1e000,0x1f000,0x40000,0x41000
BODY,BODY_SCENE=0x42000,0x43000
GP=0x3dd970
SLOTS=4

def sha(b):return hashlib.sha256(b).hexdigest()
def get(m,a,n):return [m.read(a+4*i) for i in range(n)]
def put(m,a,values):
    for i,value in enumerate(values):m.write(a+4*i,value)
def zero(m,a,n):
    for i in range(n):m.write(a+i,0,1)
def load_words():
    words,dispatch=base_words()
    source=OUT/'map/original-words.json';source_raw=source.read_bytes()
    assert sha(source_raw)=='95197f103be3974748ef2566f789762c27775ebc1fcfd998a29bc92b6746d986'
    proof=json.loads(source_raw)
    assert proof['elf']['sha256']==ELF_SHA
    for block in proof['blocks']:
        rows=block['instructions'];raw=b''.join(struct.pack('<I',r['word']) for r in rows)
        assert sha(raw)==block['sha256']
        assert [r['pc'] for r in rows]==list(range(block['start'],block['end_exclusive'],4))
        for r in rows:
            assert r['pc'] not in words or words[r['pc']]==r['word']
            words[r['pc']]=r['word']
    callback=ROOT/'evidence/springCallback48P/original-words.json'
    if callback.exists():
        raw=callback.read_bytes();assert sha(raw)=='a2cb8378c16adcd4f93f4a8748d7fe1540ecbcefac6aa2a45d2da4bd5b7a9ad2'
        p=json.loads(raw)
        for b in p['blocks']:
            rows=b['instructions'];raw=b''.join(struct.pack('<I',r['word']) for r in rows)
            assert sha(raw)==b['sha256']
            for r in rows:
                assert r['pc'] not in words or words[r['pc']]==r['word']
                words[r['pc']]=r['word']
    if '--frozen' not in sys.argv:
        elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
        for pc,word in words.items():assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==word,hex(pc)
    return words,dispatch

class Type3Original(CallbackOriginal):
    def __init__(self,words,identities):
        super().__init__(words);assert self.data==b''
        self.allowed_source_addresses=set();self.identities=identities
        self.executing=False;self.read_bytes=set();self.written_bytes=set()
        self.events=[];self.calls=[];self.callbacks=[];self.current=None;self.pending=None
        self.trace=[];self.walk_stack=[];self.leaf_updates=[];self.record_slots=0;self.append_events=[];self.selections=[];self.active_candidate=0;self.vu_minmax_deltas=[]
    def identity(self,a):
        assert a in self.identities,hex(a)
        return self.identities[a]
    def read(self,a,size=4):
        if self.executing:self.read_bytes.update(range(a,a+size))
        return super().read(a,size)
    def write(self,a,v,size=4):
        if self.executing:self.written_bytes.update(range(a,a+size))
        return super().write(a,v,size)
    def state(self):
        result=[self.read(SPRING+x) for x in STATE_OFFSETS]
        result[6]=self.identity(result[6]);return result
    def records(self):return [get(self,RECORDS+i*96,24) for i in range(self.record_slots)]
    def pair(self):
        return dict(count=self.read(RESULT+0x60),selected=get(self,RESULT,20),selected_fraction=self.read(RESULT+0x74),
                    hit_flags=get(self,HITS,SLOTS),fractions=get(self,FRACTIONS,SLOTS),normals=get(self,NORMALS,SLOTS*4),
                    points=get(self,POINTS,SLOTS*4),other_fields=get(self,RESULT+0x78,2))
    def prepared(self):
        return dict(scene_ids=get(self,SCENE_IDS,SLOTS*2),endpoints=get(self,ENDPOINTS,SLOTS*8),
                    local_starts=get(self,LOCAL_START,SLOTS*4),local_directions=get(self,LOCAL_DIR,SLOTS*4),
                    lengths=get(self,LENGTHS,SLOTS),world_starts=get(self,WORLD_START,SLOTS*4),
                    world_directions=get(self,WORLD_DIR,SLOTS*4),centers=get(self,SHAPE+0x20,8),radius=self.read(SHAPE+0x40),
                    query_sphere=get(self,Q+0x60,4)+[self.read(Q+0x80)])
    def execute(self,pc):
        if pc in (0x126eb0,0x120e70,0x126f40,0x294bb0):self.events.append([f'{pc:08X}',self.identity(self.read(Q+0xf4))])
        if pc==0x1226a8:self.after_prepare=dict(pair=self.pair(),prepared=self.prepared())
        if pc==0x126f40:
            self.active_candidate=self.identity(self.g[5][0]);self.current=dict(identity=self.active_candidate,before=self.pair(),prepared_before=self.prepared())
        if pc==0x12278c:
            self.current.update(returned=self.g[2][0],after=self.pair(),prepared_after=self.prepared());self.calls.append(self.current)
        if pc==0x10e080:
            tag=self.g[5][0]&65535
            self.trace.append(dict(candidate=self.active_candidate,tag=tag&0x7fff,leaf=bool(tag&0x8000)))
        if pc==0x10ec20:
            self.leaf_updates.append(dict(candidate=self.active_candidate,pair=self.g[23][0],
                fraction=self.read(self.g[21][0]),tested_fraction=bits(self.f[20]),face=(self.read(self.g[29][0]+0xe0)-self.face_base)//20))
        if pc==0x10ec58:self.selections.append(dict(candidate=self.active_candidate,pair=self.g[23][0],fraction=bits(self.f[1]),face=(self.read(self.g[29][0]+0xe0)-self.face_base)//20))
        if pc==0x2963e0:
            self.append_events.append(dict(candidate=self.active_candidate,pair=self.g[16][0],count_before=self.read(SPRING+0x84),capacity=self.read(SPRING+0x88),start=get(self,self.g[5][0],4),end=get(self,self.g[6][0],4),normal=get(self,self.g[7][0],4),point=get(self,self.g[8][0],4),scene=self.g[10][0],material=self.g[11][0]))
        if pc==0x29506c:self.append_events[-1].update(count_after=self.read(SPRING+0x84))
        if pc==0x111300:
            self.face_base=self.read(self.g[6][0]+4)
        if pc==0x294bb0:self.pending=dict(identity=self.identity(self.g[5][0]),pair=self.pair(),state_before=self.state(),records_before=self.records())
        if pc==0x122878 and self.pending is not None:
            self.pending.update(state_after=self.state(),records_after=self.records());self.callbacks.append(self.pending);self.pending=None
        w=self.words[pc]
        if w>>26==0x12 and ((w>>21)&31)>=16 and w&63 in (0x2b,0x2f):
            mask=(w>>21)&15;rs=(w>>11)&31;rt=(w>>16)&31;maximum=(w&63)==0x2b
            for lane in range(4):
                if mask&(8>>lane):
                    x,y=self.v[rs][lane],self.v[rt][lane];old=bits(max(x,y) if maximum else min(x,y));new=bits(vu_minmax(x,y,maximum))
                    if old!=new:self.vu_minmax_deltas.append(dict(pc=f'{pc:08X}',lane=lane,first=bits(x),second=bits(y),legacy=old,corrected=new))
        if w>>26==0x12 and ((w>>21)&31)>=16 and w&0x7ff==0x33c:
            # Authentic VMOVE destination=ft, source=fs, selected lane mask.
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
            mask=(w>>21)&15;rt=(w>>16)&31;rd=(w>>11)&31;x=self.v[rd].copy()
            for i in range(4):
                if mask&(8>>i):self.v[rt][i]=x[i]
            return None
        if w>>26==0x3f:
            # Authentic MIPS SD used by original interpolation helper calls.
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
            rs=(w>>21)&31;rt=(w>>16)&31;imm=w&65535;imm=imm-65536 if imm&32768 else imm
            self.write((self.g[rs][0]+imm)&0xffffffff,self.g[rt][0]|(self.g[rt][1]<<32),8)
            return None
        return super().execute(pc)

def make_case(name,order=(0,),pairs=None,capacity=12):
    pairs=pairs if pairs is not None else [[[bits(.25),bits(1.),bits(.125),11],[bits(.25),bits(-1.),bits(.125),0x80000000]]]
    cs=[]
    for i,y in enumerate((.5,-.5,.5)):
        world=IDENTITY.copy();world[13]=y
        cs.append(dict(mesh=3,id=10+i,world=world,filter=filter_default(10+i),sphere=[0.,0.,0.,100.],
            type_tag=7,shape_tag=0,name_hex=b'roadfr3.col\0'.hex(),has_scene=True,
            copied40=[bits(30.+i+j) for j in range(4)],copied50=[bits(40.+i+j) for j in range(4)]))
    return dict(name=name,entry='limited',pairs=pairs,scene_matrices=[None]*(len(pairs)*2),mode=0,
        query=dict(filter=filter_default(100),collision_tag=7,group_tag=9,callback=True,initial_current_candidate=0,
                   copied40=[bits(x) for x in (1.,2.,3.,4.)],copied50=[bits(x) for x in (5.,6.,7.,8.)]),
        candidates=cs,order=list(order),masks=[8,2,4],body_world=IDENTITY.copy(),
        initial_state=[0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666,0,len(pairs),0,capacity,0x88888888],
        initial_records=[[0x42000000+100*i+j for j in range(24)] for i in range(12)],
        initial_lengths=[bits(2.)]*SLOTS,initial_flags=[0x12345678+i for i in range(SLOTS)],
        initial_fractions=[bits(.75+i) for i in range(SLOTS)],initial_count=0x98765432,
        reset_before=False,reset_after=False)

def execute_case(words,dispatch,meshes,c,legacy_vu_minmax=False):
    cs=c['candidates'];q=c['query'];addresses={0:0,Q:100}
    addresses.update({0x20000+i*0x1000:x['id'] for i,x in enumerate(cs)})
    reverse={v:k for k,v in addresses.items()};m=Type3Original(words,addresses);m.legacy_vu_minmax=legacy_vu_minmax;m.g[28][0]=GP;m.record_slots=len(c['initial_records'])
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
    initial=dict(pair=m.pair(),prepared=m.prepared(),state=m.state(),records=m.records())
    if c['reset_before']:m.g[4][0]=SPRING;m.executing=True;m.run(0x296490);m.executing=False
    put(m,LIST,[reverse[cs[i]['id']] for i in c['order']]);m.g[4][0]=Q;m.g[5][0]=len(c['order']);m.g[6][0]=LIST
    dispatch_initial=None;initialized=set(m.overlay);m.executing=True
    if c['entry']=='limited':returned=m.run(0x122660)
    else:
        m.g[4][0]=Q;m.run(0x11dd00);m.after_prepare=dict(pair=m.pair(),prepared=m.prepared())
        if c.get('direct_result_overrides'):
            # Explicit input initialization at the direct-dispatch boundary.
            # This is not an original-instruction write or helper interception.
            m.executing=False;overrides=c['direct_result_overrides']
            m.write(RESULT+0x60,overrides['count']);m.write(RESULT+0x74,overrides['selected_fraction']);put(m,FRACTIONS,overrides['fractions'])
            dispatch_initial=dict(pair=m.pair(),prepared=m.prepared());m.executing=True
        m.g[4][0]=SHAPE;m.g[5][0]=reverse[cs[c['order'][0]]['id']];m.g[6][0]=RESULT
        returned=m.run(0x126f40);m.current.update(returned=returned,after=m.pair(),prepared_after=m.prepared());m.calls.append(m.current)
    m.executing=False
    result=dict(returned=returned,requires_mode2=0x10f580 in m.coverage,initial=initial,after_prepare=m.after_prepare,pair=m.pair(),prepared=m.prepared(),
        query_flags=m.read(Q+0xf8,1),current_candidate=m.identity(m.read(Q+0xf4)),spring_state=m.state(),records=m.records(),
        candidates=[dict(identity=x['id'],flags=m.read(reverse[x['id']]+0xf8,1),copied40=get(m,reverse[x['id']]+0x40,4),copied50=get(m,reverse[x['id']]+0x50,4)) for x in cs],
        calls=m.calls,events=m.events,callback_calls=m.callbacks,query_copied40=get(m,Q+0x40,4),query_copied50=get(m,Q+0x50,4),trace=m.trace,leaf_updates=m.leaf_updates,selections=m.selections,append_events=m.append_events)
    if dispatch_initial is not None:result['dispatch_initial']=dispatch_initial
    if c['reset_after']:
        m.g[4][0]=SPRING;m.executing=True;m.run(0x296490);m.executing=False
        result['after_reset']=dict(spring_state=m.state(),records=m.records())
    assert m.data==b'' and not m.source_reads
    result['byte_audit']=dict(initialized_bytes=len(initialized),execution_read_bytes=len(m.read_bytes),execution_written_bytes=len(m.written_bytes),source_reads=len(m.source_reads))
    return result,m

def synthetic_mesh():
    return dict(vertices=[[-2.,0.,-2.],[0.,0.,2.],[2.,0.,-2.],[18.,0.,-2.],[20.,0.,2.],[22.,0.,-2.]],normals=[],
        nodes=[dict(volume=48.,center=[10.,0.,0.],half=[12.,.25,2.],children=[LEAF_TAG,LEAF_TAG|1])],
        faces=[dict(d=0.,normal=[0,32767,0],vertices=[0,1,2]),dict(d=0.,normal=[0,32767,0],vertices=[3,4,5])],root=0)

def make_cases(oldcases):
    cases=[make_case('near-then-far',(0,1)),make_case('far-then-near',(1,0)),make_case('equal-tie',(0,2)),make_case('repeat',(0,0,0))]
    def add(name,order=(0,1),capacity=12,change=None,pairs=None):
        c=make_case(name,order,pairs,capacity)
        if change:change(c)
        cases.append(c);return c
    for capacity in (0,1,2,4):
        for order in ((0,1),(1,0),(0,2),(0,0,0)):
            add('capacity-'+str(capacity)+'-order-'+''.join(map(str,order)),order,capacity)
    add('empty-list',())
    add('callback-disabled',change=lambda c:c['query'].update(callback=False))
    add('outside-triangle',change=lambda c:c.update(pairs=[[[bits(10.),bits(1.),bits(10.),11],[bits(10.),bits(-1.),bits(10.),0x80000000]]]))
    add('sphere-miss',change=lambda c:c['candidates'][0].update(sphere=[1000.,0.,0.,1.]))
    add('wrong-type-tag',change=lambda c:c['candidates'][0].update(type_tag=8))
    add('candidate-inactive',change=lambda c:c['candidates'][0]['filter'].update(flags31=0))
    add('candidate-disabled',change=lambda c:c['candidates'][0]['filter'].update(flags=0))
    add('query-accept-mask-reject',change=lambda c:c['query']['filter'].update(accept_mask=0))
    add('skip-callback-category',change=lambda c:c['candidates'][0]['filter'].update(category=12))
    add('skip-all-callback-categories',change=lambda c:[x['filter'].update(category=12) for x in c['candidates']])
    add('material-mask-bypass',change=lambda c:c['candidates'][0]['filter'].update(category=16))
    add('no-material-scene',change=lambda c:c['candidates'][0].update(has_scene=False))
    add('material-differs-per-candidate',change=lambda c:c['candidates'][1].update(name_hex=b'roadfr8.col\0'.hex()))
    add('prior-static-current',change=lambda c:c['initial_state'].__setitem__(6,12))
    def dynamic(c):c['initial_state'][6]=12;c['candidates'][2]['filter']['category']=2
    add('prior-dynamic-current',change=dynamic)
    add('initial-count-at-capacity',capacity=2,change=lambda c:c['initial_state'].__setitem__(8,2))
    add('initial-count-over-capacity',capacity=2,change=lambda c:c['initial_state'].__setitem__(8,4))
    def reset(c):c['initial_state'][8]=3;c['initial_state'][6]=12;c['reset_before']=True;c['reset_after']=True
    add('explicit-reset-before-and-after',capacity=2,change=reset)
    for tag in (2,3,4,5,6,0xffffffff):
        add('unsupported-candidate-shape-'+str(tag),change=lambda c,tag=tag:c['candidates'][0].update(shape_tag=tag))
    for who in ('query','candidate'):
        for slot in range(2):
            for mode in (0,1,2):
                for member in (False,True):
                    def change(c,who=who,slot=slot,mode=mode,member=member):
                        f=c['query']['filter'] if who=='query' else c['candidates'][0]['filter']
                        f['identity_lists'][slot]=dict(mode=mode,ids=[(10 if who=='query' else 100) if member else 12])
                    add(f'{who}-identity{slot}-mode{mode}-member{member}',change=change)
        for mode in (0,1,2):
            for masks in ([],[0],[4],[0xffffffff]):
                def change(c,who=who,mode=mode,masks=masks):
                    f=c['query']['filter'] if who=='query' else c['candidates'][0]['filter'];f.update(category_mode=mode,category_masks=masks)
                add(f'{who}-categories-{mode}-{masks}',change=change)
    add('empty-list-preserves-query-current',(),change=lambda c:c['query'].update(initial_current_candidate=12))
    def wrong_tag(c):c['query']['initial_current_candidate']=12;c['candidates'][0]['type_tag']=8
    add('wrong-tag-only-preserves-query-current',(0,),change=wrong_tag)
    # Unequal pair lengths and a miss expose callback's componentwise use of
    # the first three length words, not a presumed per-pair scalar length.
    pp=[[[bits(.25),bits(1.),bits(.125),11],[bits(.25),bits(-1.),bits(.125),0x80000000]],
        [[bits(10.),bits(2.),bits(10.),12],[bits(10.),bits(-2.),bits(10.),13]],
        [[bits(20.),bits(3.),bits(.125),14],[bits(20.),bits(-3.),bits(.125),15]],
        [[bits(.5),bits(4.),bits(.125),16],[bits(.5),bits(-4.),bits(.125),17]]]
    for n in (2,3,4):
        for order in ((0,1),(1,0),(0,2),(0,0,0)):
            add(f'multiple-pairs-{n}-order-'+''.join(map(str,order)),order,pairs=copy.deepcopy(pp[:n]))
    for capacity in (0,1,2,3,4):add(f'multiple-pairs-capacity-{capacity}',(0,1),capacity,pairs=copy.deepcopy(pp))
    for count in (0,1,2,4):
        add(f'callback-loop-count-{count}',(0,),pairs=copy.deepcopy(pp),change=lambda c,count=count:c['initial_state'].__setitem__(7,count))
    for length in (-7.,0.,9.):add(f'stale-second-length-{length}',(0,),change=lambda c,length=length:c['initial_lengths'].__setitem__(1,bits(length)))
    for i in (0,1,2):
        def endpoint(c,i=i):c['candidates'][0]['world'][13]=(-1.,1.,2.)[i]
        add(('fraction-one-keeps-selected','fraction-zero','beyond-segment')[i],(0,),change=endpoint)
    def miss_second(c):c['candidates'][1]['world'][12]=100.
    add('hit-then-geometry-miss',(0,1),change=miss_second)
    for mesh in (4,5,6):add(f'within-mesh-order-{mesh}',(0,),change=lambda c,mesh=mesh:c['candidates'][0].update(mesh=mesh,world=IDENTITY.copy()))
    for mode in (1,2,0xffffffff):add('no-normal-array-mode-'+str(mode),(0,),change=lambda c,mode=mode:c.update(mode=mode))
    for mode in (0,1,2):add('normal-array-mode-'+str(mode),(0,),change=lambda c,mode=mode:(c.update(mode=mode),c['candidates'][0].update(mesh=7)))
    def scenes(c):
        c['scene_matrices']=[]
        for i in range(len(c['pairs'])*2):
            matrix=IDENTITY.copy();matrix[13]=.125*i;c['scene_matrices'].append(matrix)
    add('distinct-endpoint-scenes',(0,1),pairs=copy.deepcopy(pp[:3]),change=scenes)
    def body(c):
        c['body_world']=[0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,7.,-3.,5.,1.]
    add('callback-body-inverse',(0,1),pairs=copy.deepcopy(pp[:3]),change=body)
    for i,old in enumerate(oldcases):
        c=make_case(f'typed48I-complete-geometry-{i:02}',())
        # Type3 takes world endpoint data, so execute the supplied pair's world
        # transform through original11DD00 by attaching that matrix to both endpoints.
        c['pairs']=[[[*[bits(x) for x in old['first']],11],[*[bits(x) for x in old['second']],0x80000000]]]
        c['scene_matrices']=[old['world'],old['world']]
        c['candidates']=[]
        for j,item in enumerate(old['candidates']):
            template=make_case('unused')['candidates'][j];template.update(mesh=item['mesh'],world=item['world']);c['candidates'].append(template)
        c['order']=list(range(len(c['candidates'])));cases.append(c)
    for count in (0,1,2):
        add(f'compound-records-{count}',(0,),change=lambda c,count=count:c['candidates'][0].update(shape_tag=1,records=[IDENTITY.copy() for _ in range(count)]))
    for ys in ((.5,-.5),(-.5,.5)):
        def compound(c,ys=ys):
            mats=[]
            for y in ys:
                mat=IDENTITY.copy();mat[13]=y;mats.append(mat)
            c['candidates'][0].update(shape_tag=1,records=mats)
        add('compound-order-'+str(ys),(0,),change=compound)
    for name,change in (('inactive',lambda c:c['candidates'][0]['filter'].update(flags31=0)),('sphere-miss',lambda c:c['candidates'][0].update(sphere=[1000.,0.,0.,1.]))):
        c=add('direct-dispatch-'+name,(0,),change=change);c['entry']='direct'
    def mode2_miss(c):
        c['mode']=2;c['candidates'][0].update(mesh=7);c['candidates'][0]['world'][12]=100.
    add('normal-array-mode-2-geometry-miss',(0,),change=mode2_miss)
    def mode2_filtered(c):
        c['mode']=2;c['candidates'][0].update(mesh=7);c['query']['filter']['accept_mask']=0
    add('normal-array-mode-2-filter-reject',(0,),change=mode2_filtered)
    # Opposite zero signs and endpoint orders exercise whole preparation rather
    # than only isolated selector instructions. All backing bytes stay explicit.
    for reverse in (False,True):
        for mask in range(4):
            a=[0x80000000 if mask&1 else 0,bits(1.),0x80000000 if mask&2 else 0,11]
            b=[0 if mask&1 else 0x80000000,bits(-1.),0 if mask&2 else 0x80000000,0x80000000]
            pair=[a,b] if not reverse else [b,a]
            add(f'mixed-zero-crossing-{mask}-reverse-{int(reverse)}',(0,1),pairs=[pair])
        for mask in range(8):
            a=[0x80000000 if mask&(1<<i) else 0 for i in range(3)]+[11]
            b=[0 if mask&(1<<i) else 0x80000000 for i in range(3)]+[0x80000000]
            pair=[a,b] if not reverse else [b,a]
            add(f'mixed-zero-degenerate-{mask}-reverse-{int(reverse)}',(0,),pairs=[pair])
    transform_families={
        'rotation-z-90':[0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'rotation-y-90':[0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,3.,-2.,5.,1.],
        'rotation-z-approx37':[.8,.6,0.,0.,-.6,.8,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'reflection-x':[-1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'nonuniform-scale':[2.,0.,0.,0.,0.,.5,0.,0.,0.,0.,1.5,0.,3.,-2.,5.,1.],
        'shear':[1.,0.,0.,0.,.5,1.,.25,0.,.125,0.,1.,0.,3.,-2.,5.,1.],
    }
    for family,base_matrix in transform_families.items():
        matrix=base_matrix
        axis=max(range(3),key=lambda i:abs(matrix[4+i]));sign=1. if matrix[4+axis]>0 else -1.
        for scenario,order in (('hit',(0,)),('near-then-far',(0,1)),('far-then-near',(1,0)),('endpoint-one',(0,)),('body-callback',(0,1))):
            matrix=base_matrix.copy()
            if family=='rotation-z-approx37' and scenario=='endpoint-one':matrix[12:15]=[0.,0.,0.]
            values=(2.,0.) if scenario=='endpoint-one' else (1.,-1.)
            pair=[]
            for i,factor in enumerate(values):
                point=matrix[12:15].copy();point[axis]+=factor*sign
                pair.append([*[bits(x) for x in point],11 if i==0 else 0x80000000])
            c=add(f'candidate-transform-{family}-{scenario}',order,pairs=[pair])
            c['transform_family']=family;c['transform_scenario']=scenario
            for i,candidate in enumerate(c['candidates']):
                world=matrix.copy()
                if len(order)>1:world[12+axis]+=(.5,-.5,.5)[i]*sign
                candidate['world']=world
            if scenario=='body-callback':c['body_world']=[0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,7.,-3.,5.,1.]
    for mesh,normal in ((8,[0.,1.,0.]),(9,[1.,0.,0.])):
        def distinguishable(c,mesh=mesh):c['mode']=1;c['candidates'][0].update(mesh=mesh,world=IDENTITY.copy())
        c=add('equal-fraction-distinct-normal-'+str(mesh),(0,),change=distinguishable);c['expected_first_normal']=[bits(x) for x in normal]
    for gate in ('inactive','sphere-miss','sphere-tangent'):
        c=add('direct-dispatch-stale-'+gate,(0,));c['entry']='direct'
        c['direct_result_overrides']=dict(count=7,selected_fraction=bits(.375),fractions=[bits(.625),bits(1.75),bits(2.75),bits(3.75)])
        if gate=='inactive':c['candidates'][0]['filter']['flags31']=0
        elif gate=='sphere-miss':c['candidates'][0]['sphere']=[1000.,0.,0.,1.]
        else:c['candidates'][0]['sphere']=[2.25,0.,.125,1.]
    deep_pairs=copy.deepcopy(pp[:2]);deep_pairs[1]=[[bits(40.25),bits(1.),bits(.125),12],[bits(40.25),bits(-1.),bits(.125),13]]
    c=add('deeper-tree-multiple-pairs',(0,1),pairs=deep_pairs)
    for candidate in c['candidates']:candidate['mesh']=10
    return cases

def assertions(cases):
    by={c['name']:c for c in cases}
    r=lambda n:by[n]['expected']
    assert r('near-then-far')['pair']['fractions'][0]==bits(.75)
    assert r('far-then-near')['pair']['fractions'][0]==bits(.25)
    assert [x['identity'] for x in r('near-then-far')['callback_calls']]==[10,11]
    assert r('repeat')['spring_state'][8]==3
    miss=r('hit-then-geometry-miss');assert miss['returned']==1 and miss['pair']['count']==0
    assert miss['pair']['hit_flags'][0]==0 and miss['pair']['fractions'][0]==bits(-1.)
    assert miss['pair']['normals']==miss['calls'][0]['after']['normals'] and miss['pair']['points']==miss['calls'][0]['after']['points']
    tie=r('within-mesh-order-4');assert tie['pair']['count']==2 and tie['spring_state'][8]==1
    assert [x['face'] for x in tie['selections']]==[0]
    assert [x['face'] for x in r('within-mesh-order-5')['selections']]==[0]
    assert [x['face'] for x in r('within-mesh-order-6')['selections']]==[0,1]
    end=r('fraction-one-keeps-selected');assert end['returned']==1 and end['selections']==[] and end['pair']['selected_fraction']==bits(1.)
    assert end['pair']['selected'][8:11]==end['after_prepare']['pair']['selected'][8:11]
    assert r('fraction-zero')['pair']['fractions'][0]==0x80000000
    assert r('callback-disabled')['callback_calls']==[]
    assert r('normal-array-mode-2')['requires_mode2']
    for name in ('normal-array-mode-2-geometry-miss','normal-array-mode-2-filter-reject','no-normal-array-mode-2'):assert not r(name)['requires_mode2']
    for name in ('direct-dispatch-inactive','direct-dispatch-sphere-miss'):
        result=r(name);assert result['pair']['hit_flags'][0]==0
        for key in ('count','fractions','normals','points','selected_fraction'):assert result['pair'][key]==result['after_prepare']['pair'][key],(name,key)
    scenes=r('distinct-endpoint-scenes');assert [e['scene'] for e in scenes['append_events']]==[0xd0000,0xd0400,0xd0000,0xd0400]
    for case in cases:
        result=case['expected'];n=len(case['pairs'])
        if case['name'].startswith('direct-dispatch-stale-'):
            before=result['dispatch_initial']['pair'];assert result['returned']==0 and result['pair']['hit_flags'][0]==0
            for key in ('count','fractions','normals','points','selected','selected_fraction'):assert result['pair'][key]==before[key],(case['name'],key)
            assert result['pair']['count']==7 and result['pair']['selected_fraction']==bits(.375)
        if case['name']=='deeper-tree-multiple-pairs':
            assert result['returned']==1 and result['pair']['count']==2 and result['pair']['hit_flags'][:2]==[1,1]
            assert {row['tag'] for row in result['trace'] if not row['leaf']}=={0,1,2}
        if case['name'].startswith('candidate-transform-'):
            assert all(x is None for x in case['scene_matrices'])
            assert result['returned']==1,case['name']
            if case['transform_scenario']=='endpoint-one':
                assert result['pair']['fractions'][0]==bits(1.) and result['selections']==[],case['name']
            if case['transform_scenario']=='near-then-far':assert abs(fp(result['pair']['fractions'][0])-.75)<1.e-6,case['name']
            if case['transform_scenario']=='far-then-near':assert abs(fp(result['pair']['fractions'][0])-.25)<1.e-6,case['name']
        if case['name'].startswith('equal-fraction-distinct-normal-'):
            assert result['pair']['count']==2 and result['pair']['fractions'][0]==bits(.5)
            assert result['pair']['normals'][:3]==case['expected_first_normal']
            assert [x['face'] for x in result['selections']]==[0]
            assert [x['face'] for x in result['leaf_updates']]==[0,1]
        for call in result['calls']:
            assert call['after']['hit_flags'][n:]==case['initial_flags'][n:]
            assert call['after']['fractions'][n:]==case['initial_fractions'][n:]
        for event in result['append_events']:
            assert event['count_after']==event['count_before']+(event['count_before']<event['capacity'])
            assert event['start'][3]==bits(1.) and event['end'][3]==bits(1.)
            pair=event['pair'];lengths=result['after_prepare']['prepared']['lengths']
            if case['name'].startswith('stale-second-length-'):assert event['end'][1]==bits(1.-fp(lengths[1]))
    return dict(ordering=True,strict_ties=True,endpoint_one_stale_selection=True,zero_fraction_signed_zero=True,stale_arrays=True,capacity=True,callback_scene_stride_mismatch=True,componentwise_lengths=True,nontrivial_candidate_transforms=True,distinguishable_per_pair_ties=True)

def main():
    OUT.mkdir(parents=True,exist_ok=True);meshes,oldcases,source=load_inputs();meshes.append(synthetic_mesh())
    for heights in ((0.,0.),(.5,-.5),(-.5,.5)):
        mesh=synthetic_mesh();mesh['vertices']=[[-2.,heights[0],-2.],[0.,heights[0],2.],[2.,heights[0],-2.],[-2.,heights[1],-2.],[0.,heights[1],2.],[2.,heights[1],-2.]]
        mesh['nodes'][0].update(center=[0.,0.,0.],half=[2.,1.,2.]);meshes.append(mesh)
    mesh=synthetic_mesh();mesh['normals']=[[0.,1.,0.]]*len(mesh['vertices']);meshes.append(mesh)
    for first,second in (([0.,1.,0.],[1.,0.,0.]),([1.,0.,0.],[0.,1.,0.])):
        mesh=copy.deepcopy(meshes[4]);mesh['normals']=[first.copy() for _ in range(3)]+[second.copy() for _ in range(3)];meshes.append(mesh)
    deep=dict(vertices=[],normals=[],nodes=[dict(volume=128.,center=[30.,0.,0.],half=[32.,.25,2.],children=[1,2]),dict(volume=48.,center=[10.,0.,0.],half=[12.,.25,2.],children=[LEAF_TAG,LEAF_TAG|1]),dict(volume=48.,center=[50.,0.,0.],half=[12.,.25,2.],children=[LEAF_TAG|2,LEAF_TAG|3])],faces=[],root=0)
    for index,x in enumerate((0.,20.,40.,60.)):
        deep['vertices'] += [[x-2.,0.,-2.],[x,0.,2.],[x+2.,0.,-2.]]
        deep['faces'].append(dict(d=0.,normal=[0,32767,0],vertices=[3*index,3*index+1,3*index+2]))
    meshes.append(deep)
    words,dispatch=load_words();cases=make_cases(oldcases);coverage=set();branches={};steps=0;vu_changes=[]
    for c in cases:
        try:expected,m=execute_case(words,dispatch,meshes,c)
        except Exception:
            print('FAILED CASE',c['name'],file=sys.stderr);raise
        c['expected']=expected;coverage.update(m.coverage);steps+=m.steps
        if c['name'].startswith('mixed-zero-'):
            legacy,lm=execute_case(words,dispatch,meshes,c,legacy_vu_minmax=True)
            diffs=[]
            def difference(a,b,path='expected'):
                if isinstance(a,dict):
                    for key in a:difference(a[key],b[key],path+'.'+key)
                elif isinstance(a,list):
                    assert len(a)==len(b),(c['name'],path)
                    for index,(x,y) in enumerate(zip(a,b)):difference(x,y,f'{path}[{index}]')
                elif a!=b:diffs.append(dict(path=path,legacy=a,corrected=b))
            difference(legacy,expected)
            vu_changes.append(dict(name=c['name'],selected_lane_differences=m.vu_minmax_deltas,output_differences=diffs,legacy_steps=lm.steps,corrected_steps=m.steps))
        for pc,outcomes in m.branches.items():branches.setdefault(pc,set()).update(outcomes)
    assert any(c['selected_lane_differences'] for c in vu_changes)
    invariants=assertions(cases)
    baseline=json.loads((ROOT/'evidence/vuMinMax48P/type3Query48P-before/native-fixtures.json').read_text())
    old_cases={c['name']:c for c in baseline['cases']}
    for c in cases:
        if c['name'] in old_cases:
            observed=copy.deepcopy(c['expected']);observed.pop('requires_mode2');assert observed==old_cases[c['name']]['expected'],c['name']
    extension_baseline_path=OUT/'before-transform-extension/native-fixtures.json'
    extension_baseline_raw=extension_baseline_path.read_bytes()
    assert sha(extension_baseline_raw)=='4cee7d2a492a75f34ac65c5823432fe957783979747f8066b459b0ce946c1a14'
    extension_baseline=json.loads(extension_baseline_raw);new_by_name={c['name']:c for c in cases}
    for original in extension_baseline['cases']:assert new_by_name[original['name']]==original,original['name']
    extension_summary=dict(preserved_cases=len(extension_baseline['cases']),preserved_expected_deltas=0,added_transform_cases=sum(c['name'].startswith('candidate-transform-') for c in cases),added_distinguishable_ties=sum(c['name'].startswith('equal-fraction-distinct-normal-') for c in cases),added_direct_stale_gate_cases=sum(c['name'].startswith('direct-dispatch-stale-') for c in cases),added_deeper_tree_cases=sum(c['name']=='deeper-tree-multiple-pairs' for c in cases),baseline_sha256=sha(extension_baseline_raw))
    (OUT/'transform-extension.json').write_text(json.dumps(extension_summary,indent=2)+'\n')
    all_differences=[d for case in vu_changes for d in case['output_differences']]
    assert all((d['legacy'],d['corrected'])==(0x80000000,0) and ('centers' in d['path'] or 'query_sphere' in d['path']) for d in all_differences)
    vu_impact=dict(selected_lane_differences=sum(len(c['selected_lane_differences']) for c in vu_changes),observed_output_word_differences=len(all_differences),difference_class='Only -0 to +0 center/query-sphere words, including repeated snapshots; no return, hit, fraction, normal, point, or callback-record changes',legacy_audit_steps=sum(c['legacy_steps'] for c in vu_changes),baseline_cases=len(old_cases),baseline_output_deltas=0,new_mixed_zero_cases=len(vu_changes),new_cases_with_lane_deltas=sum(bool(c['selected_lane_differences']) for c in vu_changes),new_cases_with_output_deltas=sum(bool(c['output_differences']) for c in vu_changes),cases=vu_changes,legacy_switch='Original.legacy_vu_minmax=True; scalar48O correction remains enabled',source_reads=0)
    (OUT/'vu-minmax-impact.json').write_text(json.dumps(vu_impact,indent=2)+'\n')
    fixture=dict(schema=1,state_offsets=[f'{x:X}' for x in STATE_OFFSETS],array_slots=SLOTS,meshes=meshes,cases=cases)
    raw=(json.dumps(fixture,separators=(',',':'))+'\n').encode();(OUT/'native-fixtures.json').write_bytes(raw)
    executed=dict(elf_sha256=ELF_SHA,dispatch=dispatch,executed_words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)])
    (OUT/'executed-original-words.json').write_text(json.dumps(executed,indent=2)+'\n')
    sources=[]
    for p in ('evidence/query48H/original-words.json','evidence/type3Query48P/map/original-words.json','evidence/springCallback48P/original-words.json','evidence/springOrchestration48K/original-words.json','tools/gameplay/query_fixture48i.h','tools/gameplay/query_oracle48h.py','tools/gameplay/spring_fixture48j.py','tools/gameplay/spring_callback_fixtures48n.py','tools/gameplay/spring_query_fixtures48n.py','tools/gameplay/type3_query_fixtures48p.py'):
        sources.append(dict(path=p,sha256=sha((ROOT/p).read_bytes())))
    report=dict(cases=len(cases),full_original_type3_queries=sum(c['entry']=='limited' for c in cases),direct_original_dispatches=sum(c['entry']=='direct' for c in cases),source_reads=0,steps=steps,fixture_sha256=sha(raw),fixture_bytes=len(raw),elf_sha256=ELF_SHA,sources=sources,
        transform_extension=extension_summary,invariants=invariants,mode2_required_cases=sum(c['expected']['requires_mode2'] for c in cases),vu_minmax_impact={k:v for k,v in vu_impact.items() if k!='cases'},entry=0x122660,dispatcher=0x126f40,walker=0x10e080,callback=0x294bb0,append=0x2963e0,
        no_original_words_patched=True,no_calls_stubbed=True,no_RAM_reads_or_reconstruction=True,no_native_expected_values=True,no_uninitialized_read_fallback=True,
        callbacks=sum(len(c['expected']['callback_calls']) for c in cases),queries_with_geometry_hits=sum(bool(c['expected']['returned']) for c in cases),
        candidate_geometry_hits=sum(bool(x['returned']) for c in cases for x in c['expected']['calls']),dispatcher_calls=sum(len(c['expected']['calls']) for c in cases),
        geometry_provenance='Complete typed48I meshes/cases plus explicit synthetic triangle trees; no implicit type2 adapter',
        metadata_provenance='Explicit synthetic initialized objects/filters/body-scenes/state/records and data globals;3E02C0=(0,0,0,1) is fixture input, not an ELF constant',
        numerical_domain='Individually rounded host IEEE binary32 ordinary finite geometry; not all physical EE/VU exceptional modes',
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
    (OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','sources')},indent=2))
if __name__=='__main__':main()
