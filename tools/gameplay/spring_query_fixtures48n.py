"""Full original122660 static spring-query fixtures from initialized typed inputs.

No RAM image, reconstructed snapshot, unrestricted backing bytes, native oracle,
helper interception, or patched instruction is used. Each read must find bytes
explicitly initialized here or written by authentic original instructions.
"""
import copy, hashlib, json, re, struct, sys
from pathlib import Path
sys.dont_write_bytecode=True
from query_oracle48h import bits, fp
from spring_callback_fixtures48n import CallbackOriginal, freeze, STATE_OFFSETS
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/springQuery48N'
IDENTITY=[1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.]
Q,SHAPE,RESULT,LEAF,SPRING,RECORDS,LIST=0x10000,0x11000,0x12000,0x13000,0x14000,0x15000,0x16000
ENDPOINTS,NORMALS,POINTS,TRANSFORMED,FRACTIONS,PAIR_IDS=0x17000,0x18000,0x19000,0x1a000,0x1b000,0x1c000
GP=0x3dd970
LEAF_TAG=0x80000000
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'

def sha(raw):return hashlib.sha256(raw).hexdigest()
def raw_words(m,a,n):return [m.read(a+4*i) for i in range(n)]
def put_words(m,a,values):
    for i,value in enumerate(values):m.write(a+4*i,value)
def zero_object(m,a,n):
    # Chosen synthetic zero-initialized object metadata, never a data fallback.
    for i in range(n):m.write(a+i,0,1)

def load_inputs():
    raw=(ROOT/'tools/gameplay/query_fixture48i.h').read_bytes()
    assert sha(raw)=='20cc0f02a62727a711a7743ce2a5dc4ae05f05fdfa4975539b7f3f4aed4066a1'
    words=[int(x,16) for x in re.findall(rb'0x([0-9a-fA-F]+)u',raw)]
    at=0
    def take(n=1):
        nonlocal at
        v=words[at:at+n];assert len(v)==n;at+=n;return v
    def floats(n):return [fp(x) for x in take(n)]
    assert take(3)==[0x49383452,1,3]
    meshes=[]
    for _ in range(3):
        nv,nn,nb,nf=take(4);world=floats(16);root=take()[0]
        vertices=[floats(3) for _ in range(nv)];normals=[floats(3) for _ in range(nn)]
        nodes=[]
        for _ in range(nb):
            row=floats(7);nodes.append(dict(volume=row[0],center=row[1:4],half=row[4:7],children=take(2)))
        faces=[]
        for _ in range(nf):
            d=floats(1)[0];normal=[x if x<0x80000000 else x-0x100000000 for x in take(3)]
            faces.append(dict(d=d,normal=normal,vertices=take(3)))
        meshes.append(dict(vertices=vertices,normals=normals,nodes=nodes,faces=faces,root=root))
    cases=[]
    for _ in range(take()[0]):
        first=floats(3);second=floats(3);world=floats(16);cs=[]
        for _ in range(take()[0]):
            mesh,cid=take(2);cs.append(dict(mesh=mesh,id=cid,world=floats(16)))
        expected=take(1+10+2+7+2);trace=take(4*take()[0])
        cases.append(dict(first=first,second=second,world=world,candidates=cs))
    assert at==len(words)
    return meshes,cases,dict(path='tools/gameplay/query_fixture48i.h',sha256=sha(raw),words=len(words))

def load_words():
    frozen=(ROOT/'evidence/query48H/original-words.json').read_bytes();assert sha(frozen)=='eeb2bf1d207f466ecbc20704cf892b5f60444630a06cfc148b5deacbd9f99838';proof=json.loads(frozen)
    words={}
    for block in proof['ranges']:
        rows=block['words'];raw=b''.join(struct.pack('<I',r['word']) for r in rows)
        assert sha(raw)==block['sha256']
        assert [r['pc'] for r in rows]==list(range(block['start'],block['end_exclusive'],4))
        words.update((r['pc'],r['word']) for r in rows)
    for pc,word in freeze(frozen=True).items():
        assert pc not in words or words[pc]==word
        words[pc]=word
    spring_raw=(ROOT/'evidence/springOrchestration48K/original-words.json').read_bytes();assert sha(spring_raw)=='4468f347b6aa73bd6b93a8d6422096614e84437db7681407be3b0ae35c771354';spring=json.loads(spring_raw)
    for block in spring['blocks']:
        for row in block['instructions']:
            if 0x296490<=row['pc']<0x2964b8:words[row['pc']]=row['word']
    dispatch_values=[[1184872,1185344,1186032,1186032,1186032,1185924,1185980],
                     [1189712,1189728,1189744,1189760,1189912,1189780,1189808]]
    dispatch={lo+i*4:v for lo,row in zip((0x388af0,0x388b90),dispatch_values) for i,v in enumerate(row)}
    if '--frozen' not in sys.argv:
        elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
        for pc,word in list(words.items())+list(dispatch.items()):
            assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==word,hex(pc)
    return words,dispatch

class QueryOriginal(CallbackOriginal):
    def __init__(self,words,identities):
        super().__init__(words);assert self.data==b''
        self.identities=identities;self.events=[];self.candidates=[];self.current=None
        self.selected_ids=[0,0];self.pair_ids=[0,0];self.face=0;self.face_base=0;self.lanes=[]
        self.callback_calls=[];self.pending_callback=None;self.trace=[];self.record_slots=0
    def identity(self,a):
        if a not in self.identities:raise ValueError(f'unmapped synthetic identity {a:08x}')
        return self.identities[a]
    def pair_state(self):
        return dict(count=self.read(RESULT+0x60),endpoints=raw_words(self,ENDPOINTS,8),
            normal=raw_words(self,NORMALS,4),surface=raw_words(self,POINTS,4),
            fraction=self.read(FRACTIONS),transformed=self.read(TRANSFORMED),pair_index=self.read(PAIR_IDS,2),
            selected=raw_words(self,RESULT,20)+[self.read(RESULT+0x74),self.read(RESULT+0x78)],
            selected_ids=self.selected_ids.copy(),pair_ids=self.pair_ids.copy())
    def spring_state(self):
        result=[self.read(SPRING+x) for x in STATE_OFFSETS];result[6]=self.identity(result[6]);return result
    def records(self):return [raw_words(self,RECORDS+i*96,24) for i in range(self.record_slots)]
    def execute(self,pc):
        if pc==0x107170:
            tag=self.g[5][0]&65535;self.trace.append([self.current['identity'],tag&0x7fff,bool(tag&0x8000),False])
        if pc in (0x107368,0x10724c):self.trace[-1][3]=bool(self.g[2][0])
        if pc in (0x126eb0,0x120e70,0x121410,0x294bb0):
            self.events.append([f'{pc:08X}',self.identity(self.g[5][0])])
        if pc==0x121410:
            self.current=dict(identity=self.identity(self.g[5][0]),before=self.pair_state())
            self.face_base=self.read(self.read(self.g[5][0]+0x8c)+0x118)
        if pc==0x1082b0:self.face=(self.g[4][0]-self.face_base)//20
        if pc==0x1088d4:self.selected_ids=[self.current['identity'],self.face]
        if pc in (0x1087a4,0x1087e8):self.pair_ids=[self.current['identity'],self.face]
        if pc==0x121590:
            self.lanes.append(dict(identity=self.current['identity'],vf9_w=bits(self.v[9][3]),
                pair_index=self.g[2][0],flag=self.read(TRANSFORMED+self.g[2][0]*4)))
        if pc==0x122778:
            self.current.update(returned=self.g[2][0],after=self.pair_state());self.candidates.append(self.current)
        if pc==0x294bb0:
            self.pending_callback=dict(identity=self.identity(self.g[5][0]),pair=self.pair_state(),state_before=self.spring_state(),records_before=self.records())
        if pc==0x122878 and self.pending_callback is not None:
            self.pending_callback.update(state_after=self.spring_state(),records_after=self.records());self.callback_calls.append(self.pending_callback);self.pending_callback=None
        return super().execute(pc)

def filter_default(identity,category=4):
    return dict(identity=identity,category=category,accept_mask=0xffffffff,flags=4,flags31=1,
        identity_lists=[dict(mode=1,ids=[]),dict(mode=1,ids=[])],category_mode=1,category_masks=[])

def make_case(name,order=(0,),capacity=4):
    candidates=[]
    for i,y in enumerate((0.5,-0.5,0.5)):
        matrix=IDENTITY.copy();matrix[13]=y
        candidates.append(dict(mesh=3,id=10+i,world=matrix,filter=filter_default(10+i),
            sphere=[0.,0.,0.,100.],type_tag=7,name_hex=b'roadfr3.col\0'.hex(),has_scene=True,
            copied40=[bits(30.+i+j) for j in range(4)],copied50=[bits(40.+i+j) for j in range(4)]))
    return dict(name=name,query=dict(first=[bits(.25),bits(1.),bits(.125),11],
        second=[bits(.25),bits(-1.),bits(.125),0x80000000],world=IDENTITY.copy(),sphere=[0.,0.,0.,100.],
        filter=filter_default(100),collision_tag=7,group_tag=9,callback=True,scene_id=37,
        copied40=[bits(x) for x in (1.,2.,3.,4.)],copied50=[bits(x) for x in (5.,6.,7.,8.)]),
        candidates=candidates,order=list(order),masks=[8,2,4],
        initial_state=[0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666,0,0x77777777,0,capacity,0x88888888],
        initial_records=[[0x42000000+100*i+j for j in range(24)] for i in range(6)],reset_before=False,reset_after=False)

def execute_case(words,dispatch,meshes,case):
    cs=case['candidates'];q=case['query'];addresses={0:0,Q:100}
    for i,c in enumerate(cs):addresses[0x20000+i*0x1000]=c['id']
    reverse={v:k for k,v in addresses.items()};m=QueryOriginal(words,addresses);m.g[28][0]=GP;m.record_slots=len(case['initial_records'])
    for base,size in ((Q,0x100),(SHAPE,0x60),(RESULT,0x80),(SPRING,0x140)):
        zero_object(m,base,size)
    for base,n in ((ENDPOINTS,8),(NORMALS,4),(POINTS,4),(TRANSFORMED,1),(FRACTIONS,1),(PAIR_IDS,1)):
        put_words(m,base,[0x3e800000+i for i in range(n)])
    # Initial selected fields are explicit finite sentinels. Reset changes only its actual fields.
    put_words(m,RESULT,[bits(50.+i) for i in range(20)])
    for a,v in dispatch.items():m.write(a,v)
    m.vector(0x3e02c0,[0.,0.,0.,1.])
    m.write(GP-0x7ff0,q['collision_tag'],1);m.write(GP-0x7fec,q['group_tag'],1)
    m.write(GP-0x7f14,0,1)
    for a,v in zip((0x3e0728,0x3e0718,0x3e0720),case['masks']):m.write(a,v)
    def write_filter(obj,f,slot):
        m.write(obj+0xa4,f['category']);m.write(obj+0xb8,f['accept_mask']);m.write(obj+0xf8,f['flags'],1);m.write(obj+0x31,f['flags31'],1)
        for k,off in enumerate((0xbc,0xc8)):
            entry=f['identity_lists'][k];m.write(obj+off,entry['mode']);m.write(obj+off+4,slot+k*0x100);m.write(obj+off+8,len(entry['ids']))
            put_words(m,slot+k*0x100,[reverse[x] for x in entry['ids']])
        m.write(obj+0xd4,f['category_mode']);m.write(obj+0xd8,slot+0x200);m.write(obj+0xdc,len(f['category_masks']));put_words(m,slot+0x200,f['category_masks'])
    def sphere(obj,s):m.vector(obj+0x60,s[:3]);m.storef(obj+0x80,s[3])
    write_filter(Q,q['filter'],0x30000);sphere(Q,q['sphere'])
    for off,v in ((0x88,2),(0x8c,SHAPE),(0x90,RESULT),(0x9c,SPRING),(0xe4,0x294bb0 if q['callback'] else 0)):m.write(Q+off,v)
    put_words(m,Q+0x40,q['copied40']);put_words(m,Q+0x50,q['copied50'])
    m.write(Q+0xf4,reverse[q.get('initial_current_candidate',0)])
    m.vector(SHAPE,q['world']);m.write(SHAPE+0x40,0x8000,2);m.write(SHAPE+0x44,q['scene_id']);m.write(SHAPE+0x48,1);m.write(SHAPE+0x50,LEAF);m.write(SHAPE+0x54,1)
    put_words(m,LEAF,q['first']+q['second'])
    for off,v in ((0x50,ENDPOINTS),(0x54,NORMALS),(0x58,POINTS),(0x5c,1),(0x64,TRANSFORMED),(0x6c,FRACTIONS),(0x70,PAIR_IDS)):m.write(RESULT+off,v)
    for off,v in zip(STATE_OFFSETS,case['initial_state']):m.write(SPRING+off,reverse[v] if off==0x130 else v)
    m.write(SPRING+0x8c,RECORDS)
    for i,row in enumerate(case['initial_records']):put_words(m,RECORDS+i*96,row)
    for ci,c in enumerate(cs):
        obj=reverse[c['id']];target=0x50000+ci*0x20000;vbase=target+0x1000;nbase=target+0x7000;nodebase=target+0xc000;facebase=target+0x16000;scene=0xb0001+ci*0x1000
        mesh=meshes[c['mesh']];zero_object(m,obj,0x100);zero_object(m,target,0x120)
        write_filter(obj,c['filter'],0x31000+ci*0x1000);sphere(obj,c['sphere']);m.write(obj+2,c['type_tag'],1)
        for off,v in ((0x88,0),(0x8c,target),(0x9c,scene if c['has_scene'] else 0)):m.write(obj+off,v)
        for i,v in enumerate(bytes.fromhex(c['name_hex'])):m.write(scene+0x38+i,v,1)
        put_words(m,obj+0x40,c['copied40']);put_words(m,obj+0x50,c['copied50']);m.vector(target,c['world'])
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
    if case['reset_before']:m.g[4][0]=SPRING;m.run(0x296490)
    put_words(m,LIST,[reverse[cs[i]['id']] for i in case['order']])
    m.g[4][0]=Q;m.g[5][0]=len(case['order']);m.g[6][0]=LIST
    returned=m.run(0x122660)
    result=dict(returned=returned,query_flags=m.read(Q+0xf8,1),current_candidate=m.identity(m.read(Q+0xf4)),
        pair=m.pair_state(),spring_state=m.spring_state(),records=[raw_words(m,RECORDS+i*96,24) for i in range(len(case['initial_records']))],
        candidates=[dict(identity=c['id'],flags=m.read(reverse[c['id']]+0xf8,1),copied40=raw_words(m,reverse[c['id']]+0x40,4),copied50=raw_words(m,reverse[c['id']]+0x50,4)) for c in cs],
        tree_calls=m.candidates,events=m.events,world_lanes=m.lanes,callback_calls=m.callback_calls,
        query_copied40=raw_words(m,Q+0x40,4),query_copied50=raw_words(m,Q+0x50,4),trace=m.trace,
        counters=dict(nodes_visited=sum(not x[2] for x in m.trace),nodes_pruned=sum(not x[2] and not x[3] for x in m.trace),leaves_tested=sum(x[2] for x in m.trace),leaf_hits=sum(x[2] and x[3] for x in m.trace)))
    if case['reset_after']:
        m.g[4][0]=SPRING;m.run(0x296490);result['after_reset']=dict(spring_state=m.spring_state(),records=[raw_words(m,RECORDS+i*96,24) for i in range(len(case['initial_records']))])
    assert m.data==b'' and not m.source_reads
    for lane in m.lanes:assert lane['vf9_w']==bits(1.),lane
    if result['pair']['count']:
        assert result['pair']['endpoints']==q['first']+q['second']
        assert result['pair']['normal'][3]==bits(2.) and result['pair']['surface'][3]==bits(1.)
    return result,m

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    meshes,oldcases,source=load_inputs();words,dispatch=load_words()
    meshes.append(dict(vertices=[[-2.,0.,-2.],[0.,0.,2.],[2.,0.,-2.],[18.,0.,-2.],[20.,0.,2.],[22.,0.,-2.]],normals=[],
        nodes=[dict(volume=48.,center=[10.,0.,0.],half=[12.,.25,2.],children=[LEAF_TAG,LEAF_TAG|1])],
        faces=[dict(d=0.,normal=[0,32767,0],vertices=[0,1,2]),dict(d=0.,normal=[0,32767,0],vertices=[3,4,5])],root=0))
    cases=[make_case('near-then-far',(0,1)),make_case('far-then-near',(1,0)),make_case('equal-tie',(0,2)),make_case('repeat',(0,0,0))]
    def add(name,order=(0,1),capacity=4,change=None):
        c=make_case(name,order,capacity)
        if change:change(c)
        cases.append(c)
    for capacity in (0,1,2,4):
        for order in ((0,1),(1,0),(0,2),(0,0,0)):
            add('capacity-'+str(capacity)+'-order-'+''.join(map(str,order)),order,capacity)
    add('empty-list',())
    add('callback-disabled',change=lambda c:c['query'].update(callback=False))
    add('outside-triangle',change=lambda c:c['query'].update(first=[bits(10.),bits(1.),bits(10.),11],second=[bits(10.),bits(-1.),bits(10.),0x80000000]))
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
    add('query-scene-zero',change=lambda c:c['query'].update(scene_id=0))
    add('prior-static-current',change=lambda c:c['initial_state'].__setitem__(6,12))
    def dynamic(c):c['initial_state'][6]=12;c['candidates'][2]['filter']['category']=2
    add('prior-dynamic-current',change=dynamic)
    add('initial-count-at-capacity',capacity=2,change=lambda c:c['initial_state'].__setitem__(8,2))
    add('initial-count-over-capacity',capacity=2,change=lambda c:c['initial_state'].__setitem__(8,4))
    def reset(c):c['initial_state'][8]=3;c['initial_state'][6]=12;c['reset_before']=True;c['reset_after']=True
    add('explicit-reset-before-and-after',capacity=2,change=reset)
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
    # Retain every complete typed48I query geometry case; metadata is newly chosen,
    # never claimed to be a saved spring state or reconstructed live query object.
    for i,old in enumerate(oldcases):
        c=make_case(f'typed48I-complete-geometry-{i:02}',())
        c['query'].update(first=[*[bits(x) for x in old['first']],11],second=[*[bits(x) for x in old['second']],0x80000000],world=old['world'])
        c['candidates']=[]
        for j,item in enumerate(old['candidates']):
            template=make_case('unused')['candidates'][j]
            template.update(mesh=item['mesh'],world=item['world'])
            c['candidates'].append(template)
        c['order']=list(range(len(c['candidates'])));cases.append(c)
    # Original11DD00 does not clear query+F4. Only collision-tag iteration writes it.
    add('empty-list-preserves-query-current',(),change=lambda c:c['query'].update(initial_current_candidate=12))
    def wrong_tag_only(c):
        c['query']['initial_current_candidate']=12;c['candidates'][0]['type_tag']=8
    add('wrong-tag-only-preserves-query-current',(0,),change=wrong_tag_only)
    coverage=set();branches={};steps=0
    for case in cases:
        expected,m=execute_case(words,dispatch,meshes,case);case['expected']=expected
        coverage.update(m.coverage);steps+=m.steps
        for pc,outcomes in m.branches.items():branches.setdefault(pc,set()).update(outcomes)
    # Explicit ordering assertions use the actual full oracle traces.
    near,far,tie,repeat=[c['expected'] for c in cases[:4]]
    assert [x['returned'] for x in near['tree_calls']]==[1,1]
    assert [x['identity'] for x in near['callback_calls']]==[10,11]
    assert near['callback_calls'][1]['pair']['pair_ids']==[10,0]
    assert far['callback_calls'][1]['pair']['pair_ids']==[10,0]
    assert tie['pair']['selected_ids']==[12,0] and tie['pair']['pair_ids']==[10,0]
    assert repeat['spring_state'][8]==3
    assert all(c['expected']['callback_calls']==[] for c in cases if c['name']=='callback-disabled')
    fixture=dict(schema=1,state_offsets=[f'{x:X}' for x in STATE_OFFSETS],meshes=meshes,cases=cases)
    raw=(json.dumps(fixture,separators=(',',':'))+'\n').encode();(OUT/'native-fixtures.json').write_bytes(raw)
    frozen=dict(elf_sha256=ELF_SHA,dispatch=dispatch,executed_words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)])
    (OUT/'executed-original-words.json').write_text(json.dumps(frozen,indent=2)+'\n')
    sources=[]
    for p in ('evidence/query48H/original-words.json','evidence/springCallback48N/original-words.json','evidence/springOrchestration48K/original-words.json','tools/gameplay/query_fixture48i.h','tools/gameplay/query_oracle48h.py','tools/gameplay/spring_fixture48j.py','tools/gameplay/spring_callback_fixtures48n.py'):
        sources.append(dict(path=p,sha256=sha((ROOT/p).read_bytes())))
    report=dict(cases=len(cases),full_original_queries=len(cases),source_reads=0,steps=steps,
        fixture_sha256=sha(raw),fixture_bytes=len(raw),elf_sha256=ELF_SHA,sources=sources,
        full_type2_query=True,callback=0x294bb0,entry=0x122660,append=0x2963e0,
        no_original_words_patched=True,no_calls_stubbed=True,no_RAM_reads_or_reconstruction=True,
        no_native_expected_values=True,no_uninitialized_read_fallback=True,
        geometry_provenance='Complete typed48I meshes/cases (including two full source-derived meshes), plus explicit two-triangle synthetic mesh',
        metadata_provenance='Explicit synthetic object/filter/sphere/state/record/global initialization only; no saved live spring state claim',
        numerical_domain='Individually rounded host IEEE binary32 ordinary finite geometry; not all physical EE/VU exceptional modes',
        callbacks=sum(len(c['expected']['callback_calls']) for c in cases),
        queries_with_geometry_hits=sum(bool(c['expected']['returned']) for c in cases),
        candidate_geometry_hits=sum(bool(call['returned']) for c in cases for call in c['expected']['tree_calls']),
        tree_calls=sum(len(c['expected']['tree_calls']) for c in cases),
        all_world_entry_vf9_w=bits(1.),transformed_normal_w=bits(2.),transformed_surface_w=bits(1.),
        endpoints_w_preserved=[11,0x80000000],coverage=[f'{pc:08X}' for pc in sorted(coverage)],
        branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
    (OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','sources')},indent=2))
if __name__=='__main__':main()
