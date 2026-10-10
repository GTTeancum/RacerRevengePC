"""Independent, test-only original-word type2/mode0 static query evidence.

The interpreter receives no RAM or device backing. Every data byte is explicitly
initialized from bounded typed resources/fixture state, or written by authentic
instructions. Production code and native output are never an expected-value source.
"""
import argparse, copy, hashlib, json, math, struct, sys
from pathlib import Path
sys.dont_write_bytecode = True
from query_oracle48h import bits, fp, f32, add, sub, mul, div
from spring_callback_fixtures48n import CallbackOriginal, STATE_OFFSETS
from spring_query_fixtures48n import load_words, filter_default, IDENTITY, ELF_SHA
ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/staticType2Producer49X/original-word'
ASSESSMENT = ROOT.parents[1] / 'staticcontact49x-assessment'
Q, SHAPE, RESULT, LEAF, SPRING, RECORDS, LIST = [0x10000+i*0x1000 for i in range(7)]
ENDPOINTS, NORMALS, POINTS, TRANSFORMED, FRACTIONS, PAIR_IDS, QUERY_NODES = [0x17000+i*0x1000 for i in range(7)]
GP = 0x3dd970

def sha(b): return hashlib.sha256(b).hexdigest()
def raw_words(m,a,n): return [m.read(a+4*i) for i in range(n)]
def put_words(m,a,x):
    for i,v in enumerate(x): m.write(a+4*i,v)
def zero(m,a,n):
    for i in range(n): m.write(a+i,0,1)
def run_until(m,pc,end):
    while pc != end:
        target=m.execute(pc)
        if target is not None:
            if not m.annul: assert m.execute(pc+4) is None
            pc=target
        else: pc+=4

SUPPLEMENT_SHA = '928c79906a4c22e0f5df40694d9d24609adce17774468d27c6d83d155910bfa0'
RUNTIME_FIXTURE_SHA = 'a643a537941cda4fbe2c7e510eeb6caa954284348a392dd66589359468e5d1d8'

def authenticated_words():
    words, dispatch = load_words()
    raw=(OUT/'supplemental-original-words.json').read_bytes();assert sha(raw)==SUPPLEMENT_SHA
    supplement=json.loads(raw);assert supplement['elf_sha256']==ELF_SHA
    elf=None
    if '--frozen' not in sys.argv:
        elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
    for block in supplement['ranges']:
        raw=b''.join(struct.pack('<I',w) for w in block['words']);assert sha(raw)==block['sha256']
        assert len(raw)==block['end']-block['start']
        if elf is not None:assert elf[block['start']-0x100000+0x80:block['end']-0x100000+0x80]==raw
        destination=dispatch if block['start']==0x3880f0 else words
        destination.update(zip(range(block['start'],block['end'],4),block['words']))
    return words,dispatch

class Type2Original(CallbackOriginal):
    def __init__(self,words,identities,slots):
        super().__init__(words); assert self.data==b''
        self.identities=identities; self.slots=slots; self.current=None
        self.selected_ids=[0,0,0]; self.pair_owners=[[0,0] for _ in range(slots)]
        self.trace=[]; self.visits=[]; self.frames=[]; self.tree_calls=[]; self.events=[]
        self.lanes=[]; self.callback_calls=[]; self.pending_callback=None
        self.face=0; self.ray=0; self.leaf_hits=[]; self.record_slots=0
        self.optional_normal_addresses=set();self.optional_normal_reads=0
    def read(self,a,size=4):
        if a in self.optional_normal_addresses:self.optional_normal_reads+=1
        return super().read(a,size)
    def identity(self,a):
        if a not in self.identities: raise ValueError(f'unmapped identity {a:08X}')
        return self.identities[a]
    def pair_state(self):
        return dict(count=self.read(RESULT+0x60), selected=raw_words(self,RESULT,20),
            selected_fraction=self.read(RESULT+0x74), selected_flags=self.read(RESULT+0x78),
            selected_ids=self.selected_ids.copy(),
            endpoints=[raw_words(self,ENDPOINTS+32*i,8) for i in range(self.slots)],
            normals=[raw_words(self,NORMALS+16*i,4) for i in range(self.slots)],
            surfaces=[raw_words(self,POINTS+16*i,4) for i in range(self.slots)],
            fractions=raw_words(self,FRACTIONS,self.slots), transformed=raw_words(self,TRANSFORMED,self.slots),
            pair_tags=[self.read(PAIR_IDS+2*i,2) for i in range(self.slots)],
            pair_owners=copy.deepcopy(self.pair_owners))
    def spring_state(self):
        state=[self.read(SPRING+x) for x in STATE_OFFSETS]; state[6]=self.identity(state[6]); return state
    def records(self): return [raw_words(self,RECORDS+96*i,24) for i in range(self.record_slots)]
    def execute(self,pc):
        if pc==0x107170:
            self.frames.append(len(self.trace)); self.trace.append([self.current['identity'],self.g[4][0]&65535,self.g[5][0]&65535,None]); self.visits.append(self.trace[-1][:3]+[0])
        if pc==0x1078e4:
            index=self.frames.pop();self.trace[index][3]=self.g[2][0]
            if self.trace[index][1]&0x8000 and self.trace[index][2]&0x8000:self.visits[index][3]=self.g[2][0]
        if pc==0x107368 and self.g[2][0]:self.visits[self.frames[-1]][3]=3 if self.trace[self.frames[-1]][1]&0x8000 else 2
        if pc==0x10786c:self.visits[self.frames[-1]][3]=2
        if pc==0x1078a0:self.visits[self.frames[-1]][3]=3
        if pc in (0x126eb0,0x120e70,0x121410,0x294bb0): self.events.append([f'{pc:08X}',self.identity(self.g[5][0])])
        if pc==0x121410:
            self.current=dict(identity=self.identity(self.g[5][0]),before=self.pair_state())
            self.face_base=self.read(self.read(self.g[5][0]+0x8c)+0x118)
        if pc==0x1082b0: self.face=(self.g[4][0]-self.face_base)//20; self.ray=self.g[6][0]&65535
        if pc==0x10870c and self.g[2][0]:
            self.leaf_hits.append([self.current['identity'],self.ray,self.face,self.read(self.g[29][0]+0x158)])
        if pc==0x1088d4: self.selected_ids=[self.current['identity'],self.face,self.ray]
        if pc in (0x1087a4,0x1087e8):
            index=self.g[5][0] if pc==0x1087a4 else self.read(RESULT+0x60)
            self.pair_owners[index]=[self.current['identity'],self.face]
        if pc in (0x1214b8,0x121590):
            index=self.g[2][0] if pc==0x121590 else -1
            self.lanes.append(dict(pc=f'{pc:08X}',identity=self.current['identity'],vf9_w=bits(self.v[9][3]),
                index=index,flag=self.read(TRANSFORMED+index*4) if index>=0 else self.read(RESULT+0x78)))
        if pc==0x122778:
            self.current.update(returned=self.g[2][0],after=self.pair_state()); self.tree_calls.append(self.current)
        if pc==0x294bb0:
            self.pending_callback=dict(identity=self.identity(self.g[5][0]),pair=self.pair_state(),state_before=self.spring_state(),records_before=self.records())
        if pc==0x122878 and self.pending_callback is not None:
            self.pending_callback.update(state_after=self.spring_state(),records_after=self.records())
            self.callback_calls.append(self.pending_callback); self.pending_callback=None
        return super().execute(pc)

def initial_state(slots,record_slots=32,capacity=32):
    return dict(selected=[bits(50.+i) for i in range(20)],selected_fraction=bits(.375),selected_flags=0x9a0055f0,
        endpoints=[[0x3e800000+8*i+j for j in range(8)] for i in range(slots)],
        normals=[[0x3f000000+4*i+j for j in range(4)] for i in range(slots)],
        surfaces=[[0x3f800000+4*i+j for j in range(4)] for i in range(slots)],
        fractions=[0x3e000000+i for i in range(slots)],transformed=[0x45670000+i for i in range(slots)],
        pair_tags=[0x1234+i for i in range(slots)],count=0,
        spring=[0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666,0,0x77777777,0,capacity,0x88888888],
        records=[[0x42000000+100*i+j for j in range(24)] for i in range(record_slots)])

def make_candidate(mesh,identity=10,world=None):
    return dict(mesh=mesh,id=identity,world=world or IDENTITY.copy(),filter=filter_default(identity),
        sphere=[0.,0.,0.,1e6],type_tag=7,name_hex=b'roadfr3.col\0'.hex(),has_scene=True,
        copied40=[bits(30.+identity+i) for i in range(4)],copied50=[bits(40.+identity+i) for i in range(4)])
def make_case(name,shape,shapes,candidates,order=None,world=None,capacity=32):
    return dict(name=name,query=dict(shape=shape,world=world or IDENTITY.copy(),sphere=[0.,0.,0.,1e6],
        filter=filter_default(100),collision_tag=7,group_tag=9,callback=True,scene_id=37,mode=0,
        copied40=[bits(x) for x in (1.,2.,3.,4.)],copied50=[bits(x) for x in (5.,6.,7.,8.)]),
        candidates=candidates,order=list(range(len(candidates))) if order is None else list(order),masks=[8,2,4],
        initial=initial_state(len(shapes[shape]['rays'])+2,32,capacity),reset_before=False,reset_after=False)

def execute_case(words,dispatch,shapes,meshes,case):
    cs=case['candidates']; q=case['query']; shape=shapes[q['shape']]; init=case['initial']; slots=len(init['fractions'])
    addresses={0:0,Q:100};addresses.update({0x20000+i*0x1000:c['id'] for i,c in enumerate(cs)})
    reverse={v:k for k,v in addresses.items()};m=Type2Original(words,addresses,slots);m.g[28][0]=GP;m.record_slots=len(init['records'])
    for base,size in ((Q,0x100),(SHAPE,0x60),(RESULT,0x80),(SPRING,0x140)):zero(m,base,size)
    for field,base,stride in [('endpoints',ENDPOINTS,32),('normals',NORMALS,16),('surfaces',POINTS,16)]:
        for i,row in enumerate(init[field]):put_words(m,base+i*stride,row)
    for field,base in [('fractions',FRACTIONS),('transformed',TRANSFORMED)]:put_words(m,base,init[field])
    for i,x in enumerate(init['pair_tags']):m.write(PAIR_IDS+2*i,x,2)
    put_words(m,RESULT,init['selected']);m.write(RESULT+0x74,init['selected_fraction']);m.write(RESULT+0x78,init['selected_flags']);m.write(RESULT+0x60,init['count'])
    for a,v in dispatch.items():m.write(a,v)
    m.vector(0x3e02c0,[0.,0.,0.,1.])
    # Source-authenticated cold axis values. Lazy scratch vectors are written by
    # the original query, not initialized as an unbounded zero fallback.
    for a,v in [(0x3df540,[1.,0.,0.,1.]),(0x3df550,[0.,1.,0.,1.]),(0x3df560,[0.,0.,1.,1.])]:m.vector(a,v)
    for i in range(15):m.vector(0x3dfe80+i*16,[0.,0.,0.,1.])
    m.write(GP-0x7ff0,q['collision_tag'],1);m.write(GP-0x7fec,q['group_tag'],1);m.write(GP-0x7f14,0,1);
    for off in range(0x7eec,0x7f11,4):m.write(GP-off,0,1)
    for a,v in zip((0x3e0728,0x3e0718,0x3e0720),case['masks']):m.write(a,v)
    def write_filter(obj,f,slot):
        for off,key in [(0xa4,'category'),(0xb8,'accept_mask')]:m.write(obj+off,f[key])
        m.write(obj+0xf8,f['flags'],1);m.write(obj+0x31,f['flags31'],1)
        for k,off in enumerate((0xbc,0xc8)):
            e=f['identity_lists'][k];m.write(obj+off,e['mode']);m.write(obj+off+4,slot+k*0x100);m.write(obj+off+8,len(e['ids']));put_words(m,slot+k*0x100,[reverse[x] for x in e['ids']])
        m.write(obj+0xd4,f['category_mode']);m.write(obj+0xd8,slot+0x200);m.write(obj+0xdc,len(f['category_masks']));put_words(m,slot+0x200,f['category_masks'])
    def sphere(obj,s):m.vector(obj+0x60,s[:3]);m.storef(obj+0x80,s[3])
    write_filter(Q,q['filter'],0x50000);sphere(Q,q['sphere'])
    for off,v in ((0x88,2),(0x8c,SHAPE),(0x90,RESULT),(0x9c,SPRING),(0xe4,0x294bb0 if q['callback'] else 0)):m.write(Q+off,v)
    put_words(m,Q+0x40,q['copied40']);put_words(m,Q+0x50,q['copied50']);m.write(Q+0xf4,reverse[q.get('initial_current_candidate',0)])
    m.vector(SHAPE,q['world']);m.write(SHAPE+0x40,shape['root'],2);m.write(SHAPE+0x44,q['scene_id']);m.write(SHAPE+0x48,len(shape['rays']));m.write(SHAPE+0x4c,QUERY_NODES);m.write(SHAPE+0x50,LEAF);m.write(SHAPE+0x54,q['mode'])
    for i,ray in enumerate(shape['rays']):put_words(m,LEAF+i*32,ray)
    def write_nodes(base,nodes):
        for i,n in enumerate(nodes):
            a=base+36*i;m.storef(a,n['volume']);m.storef(a+4,n['radius']);m.vector(a+12,n['center']);m.vector(a+24,n['half'])
            for k,ch in enumerate(n['children']):m.write(a+8+2*k,ch,2)
    write_nodes(QUERY_NODES,shape['nodes'])
    for off,v in ((0x50,ENDPOINTS),(0x54,NORMALS),(0x58,POINTS),(0x5c,len(shape['rays'])),(0x64,TRANSFORMED),(0x6c,FRACTIONS),(0x70,PAIR_IDS)):m.write(RESULT+off,v)
    for off,v in zip(STATE_OFFSETS,init['spring']):m.write(SPRING+off,reverse[v] if off==0x130 else v)
    m.write(SPRING+0x8c,RECORDS)
    for i,row in enumerate(init['records']):put_words(m,RECORDS+96*i,row)
    for ci,c in enumerate(cs):
        obj=reverse[c['id']];target=0x600000+ci*0x40000;vbase=target+0x1000;nbase=target+0x10000;nodebase=target+0x20000;facebase=target+0x34000;scene=0x80001+ci*0x1000
        mesh=meshes[c['mesh']];zero(m,obj,0x100);zero(m,target,0x120)
        write_filter(obj,c['filter'],0xa0000+ci*0x400);sphere(obj,c['sphere']);m.write(obj+2,c['type_tag'],1)
        for off,v in ((0x88,0),(0x8c,target),(0x9c,scene if c['has_scene'] else 0)):m.write(obj+off,v)
        for i,v in enumerate(bytes.fromhex(c['name_hex'])):m.write(scene+0x38+i,v,1)
        put_words(m,obj+0x40,c['copied40']);put_words(m,obj+0x50,c['copied50']);m.vector(target,c['world'])
        for off,v in ((0x100,vbase),(0x104,nbase if mesh['normals'] else 0),(0x108,0),(0x114,nodebase),(0x118,facebase)):m.write(target+off,v)
        m.write(target+0x10c,mesh.get('root',0),2)
        for i,v in enumerate(mesh['vertices']):m.vector(vbase+12*i,v)
        for i,v in enumerate(mesh['normals']):m.vector(nbase+12*i,v)
        m.optional_normal_addresses.update(range(nbase,nbase+12*len(mesh['normals'])))
        write_nodes(nodebase,mesh['nodes'])
        for i,face in enumerate(mesh['faces']):
            a=facebase+i*20;m.storef(a+8,face['d'])
            for k,x in enumerate(face['vertices']):m.write(a+k*2,x,2)
            for k,x in enumerate(face['normal']):m.write(a+12+k*2,x,2)
    if case['reset_before']:m.g[4][0]=SPRING;m.run(0x296490)
    put_words(m,LIST,[reverse[cs[i]['id']] for i in case['order']]);m.g[4][0]=Q;m.g[5][0]=len(case['order']);m.g[6][0]=LIST
    returned=m.run(0x122660)
    result=dict(returned=returned,query_flags=m.read(Q+0xf8,1),current_candidate=m.identity(m.read(Q+0xf4)),pair=m.pair_state(),spring_state=m.spring_state(),records=m.records(),
        candidates=[dict(identity=c['id'],flags=m.read(reverse[c['id']]+0xf8,1),copied40=raw_words(m,reverse[c['id']]+0x40,4),copied50=raw_words(m,reverse[c['id']]+0x50,4)) for c in cs],
        tree_calls=m.tree_calls,events=m.events,world_lanes=m.lanes,callback_calls=m.callback_calls,leaf_hits=m.leaf_hits,trace=m.trace,visits=m.visits,
        query_copied40=raw_words(m,Q+0x40,4),query_copied50=raw_words(m,Q+0x50,4),optional_normal_reads=m.optional_normal_reads)
    if case['reset_after']:
        m.g[4][0]=SPRING;m.run(0x296490);result['after_reset']=dict(spring_state=m.spring_state(),records=m.records())
    assert m.data==b'' and not m.source_reads and not m.frames
    if q['mode']==0:assert m.optional_normal_reads==0
    for lane in m.lanes:assert lane['vf9_w']==bits(1.)
    final=result['pair']
    for i in range(final['count']):
        assert final['endpoints'][i]==shape['rays'][final['pair_tags'][i]&32767]
        assert final['normals'][i][3]==bits(2.) and final['surfaces'][i][3]==bits(1.)
    for field in ('endpoints','normals','surfaces','fractions','transformed','pair_tags'):
        assert final[field][final['count']:]==init[field][final['count']:]
    if returned:
        assert final['selected'][11]==bits(2.) and final['selected'][15]==bits(1.) and final['selected'][19]==bits(3.)
    else:
        assert final['selected'][:8]==[0,0,0,bits(1.)]*2 and final['selected'][8:]==init['selected'][8:]
        assert final['selected_flags']==init['selected_flags']
    return result,m

def source_radius(h):
    square=add(add(mul(h[0],h[0]),mul(h[1],h[1])),mul(h[2],h[2]))
    if square==0:return 0.
    return div(1.,f32(1./math.sqrt(abs(square))))

def synthetic_inputs():
    rays=[[bits(x) for x in [.25,1.,.125]]+[11]+[bits(x) for x in [.25,-1.,.125]]+[0x80000000],
          [bits(x) for x in [-.25,1.,.25]]+[0x7fc12345]+[bits(x) for x in [-.25,-1.,.25]]+[0xdeadbeef]]
    node=dict(volume=8.,radius=source_radius([1.,1.,1.]),center=[0.,0.,0.],half=[1.,1.,1.],children=[0x8000,0x8001])
    shapes=[dict(name='synthetic-two-ray',root=0,nodes=[node],rays=rays,provenance='explicit synthetic finite XYZ; opaque caller W')]
    vertices=[[-2.,0.,-2.],[0.,0.,2.],[2.,0.,-2.],[18.,0.,-2.],[20.,0.,2.],[22.,0.,-2.]]
    faces=[dict(d=0.,normal=[0,32767,0],vertices=[0,1,2]),dict(d=0.,normal=[0,32767,0],vertices=[3,4,5])]
    meshes=[dict(name='synthetic-two-triangle',vertices=vertices,normals=[],root=0,
        nodes=[dict(volume=48.,radius=source_radius([12.,.25,2.]),center=[10.,0.,0.],half=[12.,.25,2.],children=[0x8000,0x8001])],faces=faces)]
    return shapes,meshes

class NumericOriginal(CallbackOriginal):
    def execute(self,pc):
        w=self.words[pc]
        if w>>26==0x11 and (w>>21)&31==16 and w&63==0x24:
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
            value=self.f[(w>>11)&31];assert math.isfinite(value)
            self.f[(w>>6)&31]=fp(max(-2147483648,min(2147483647,int(value)))&0xffffffff)
            return None
        return super().execute(pc)

def original_pack_normal(words,values):
    m=NumericOriginal(words);base=0x10000;m.g[29][0]=base;m.f[2]=32767.
    m.vector(base+0x70,values);run_until(m,0x126320,0x126378)
    result=[m.read(base+0xd0+2*i,2) for i in range(3)]
    assert m.data==b'' and not m.source_reads
    return [v-65536 if v&32768 else v for v in result]

INPUT_SHA256 = {'original-runtime-nodes.json': '4f52564e783ebdfc885a14984193bd43cecd6e16d28f1e69ab666d28691bbab6', 'ta-collision-inventory.json': '8689f541c86b9079c01484d7c6de59782a0e925406b53dc5e63b8c66b61c59a7', 'ta-static-bindings.json': 'b087193adb028e544d4a47b5fd4874916cd98fef185317e326f3fa8648eeae5a'}

def load_actual_inputs(words):
    inputs=OUT/'inputs';inputs.mkdir(exist_ok=True)
    runtime_path=inputs/'original-runtime-nodes.json'
    if not runtime_path.exists():
        source=ROOT.parents[1]/'type2-resource49x-proof/fixtures.json'
        proof=json.loads(source.read_text())
        runtime_path.write_text(json.dumps(dict(queries=proof['queries'],targets=proof['targets'],
            source_fixture_sha256=sha(source.read_bytes())),separators=(',',':'))+'\n')
        (inputs/'runtime-source-report.json').write_bytes((source.parent/'source-report.json').read_bytes())
        (inputs/'ta-collision-inventory.json').write_bytes((ASSESSMENT/'asset-inventory/ta-collision-inventory.json').read_bytes())
        (inputs/'ta-static-bindings.json').write_bytes((ASSESSMENT/'asset-inventory/ta-static-bindings.json').read_bytes())
        for name in ('podboxm.col','chariotm.col'):(inputs/name).write_bytes((ASSESSMENT/'asset-inventory'/name).read_bytes())
    for name,digest in INPUT_SHA256.items():assert sha((inputs/name).read_bytes())==digest
    runtime=json.loads(runtime_path.read_text());assert runtime['source_fixture_sha256']==RUNTIME_FIXTURE_SHA;shapes=[];meshes=[]
    for q in runtime['queries']:
        raw=(inputs/q['name']).read_bytes();assert sha(raw)==q['source_sha256']
        nr,nn,nl=struct.unpack_from('<3I',raw,80);assert (nr,nn,nl)==(q['ray_count'],q['node_count'],q['ray_count'])
        nodes=[dict(volume=fp(n['volume_word']),radius=fp(n['radius_word']),center=list(map(fp,n['center_words'])),
            half=list(map(fp,n['half_words'])),children=n['children']) for n in q['nodes']]
        xyz=[list(struct.unpack_from('<6I',raw,92+32*nn+24*i)) for i in range(nr)]
        assert xyz==q['ray_xyz_words']
        # Deliberately distinct opaque supplied state, never inferred heap bytes.
        rays=[row[:3]+[0x80000000+i]+row[3:]+[0x7fc10000+i] for i,row in enumerate(xyz)]
        shapes.append(dict(name=q['name'],root=0,nodes=nodes,rays=rays,source_sha256=sha(raw),
            source_path=str((inputs/q['name']).relative_to(ROOT)),max_depth=q['max_depth'],
            provenance='Complete source XYZ/tree; runtime node words from independent original arithmetic windows; W explicitly caller supplied'))
    inventory=json.loads((inputs/'ta-collision-inventory.json').read_text())
    sys.path.insert(0,str(ROOT/'tools/assets'))
    from decode_collision48g import decode
    byname={t['member']:t for t in runtime['targets']}
    for item in inventory:
        if not item['static_instances']:continue
        cache=item['cache_copies'][0];raw=(ROOT/cache['path']).read_bytes();assert sha(raw)==cache['sha256']
        original=raw[-item['source_bytes']:];assert sha(original)==item['source_sha256'];parsed=decode(original)
        radii=byname[item['member']]['radius_words'];assert len(radii)==len(parsed['nodes'])
        nodes=[]
        for n,radius in zip(parsed['nodes'],radii):
            nodes.append(dict(volume=n['volume'],radius=fp(radius),center=n['center'],half=n['half'],
                children=[(x&32767)|(32768 if x&0x80000000 else 0) for x in n['children']]))
        faces=[dict(d=f['d'],normal=original_pack_normal(words,f['normal']),vertices=f['vertices']) for f in parsed['faces']]
        meshes.append(dict(name=item['member'],root=0,nodes=nodes,faces=faces,vertices=parsed['vertices'],normals=parsed['vertex_normals'],
            source_sha256=item['source_sha256'],cache_path=cache['path'],cache_sha256=cache['sha256'],max_depth=parsed['maximum_tree_depth'],
            provenance='Complete original source mesh and optional normals; face normals packed by original126320..126374; radii original126168..126200'))
    return shapes,meshes

def face_pose(shape,mesh,face_index=None):
    def area(f):
        a,b,c=[mesh['vertices'][i] for i in f['vertices']]
        ab=[b[i]-a[i] for i in range(3)];ac=[c[i]-a[i] for i in range(3)]
        return sum((ab[(i+1)%3]*ac[(i+2)%3]-ab[(i+2)%3]*ac[(i+1)%3])**2 for i in range(3))
    face_index=max(range(len(mesh['faces'])),key=lambda i:area(mesh['faces'][i])) if face_index is None else face_index
    face=mesh['faces'][face_index];normal=face['normal']
    ray_index=min(range(len(shape['rays'])),key=lambda i:sum((fp(shape['rays'][i][4+j])-fp(shape['rays'][i][j]))*normal[j] for j in range(3)))
    ray=shape['rays'][ray_index];world=IDENTITY.copy()
    for k in range(3):
        centroid=sum(mesh['vertices'][v][k] for v in face['vertices'])/3.
        midpoint=(fp(ray[k])+fp(ray[k+4]))*.5
        world[12+k]=f32(centroid-midpoint)
    return world,face_index,ray_index

def build_cases(words,shapes,meshes):
    cases=[make_case('two-ray-smoke',0,shapes,[make_candidate(0)])]
    def add_case(name,shape=0,mesh=0,order=(0,1),capacity=32,change=None):
        candidates=[]
        for i,y in enumerate((.5,-.5,.5)):
            world=IDENTITY.copy();world[13]=y;candidates.append(make_candidate(mesh,10+i,world))
        case=make_case(name,shape,shapes,candidates,order=order,capacity=capacity)
        if change:change(case)
        cases.append(case);return case
    for name,order in [('near-then-far',(0,1)),('far-then-near',(1,0)),('candidate-equal-tie',(0,2)),('repeat-candidate',(0,0,0)),('empty-list',())]:add_case(name,order=order)
    for cap in (0,1,2,4,32):
        for order in ((0,1),(1,0),(0,2),(0,0,0)):add_case('capacity-'+str(cap)+'-order-'+''.join(map(str,order)),order=order,capacity=cap)
    for name,change in [
        ('callback-disabled',lambda c:c['query'].update(callback=False)),
        ('sphere-miss',lambda c:[x.update(sphere=[3e6,0,0,1]) for x in c['candidates']]),
        ('wrong-type',lambda c:c['candidates'][0].update(type_tag=8)),
        ('inactive',lambda c:c['candidates'][0]['filter'].update(flags31=0)),
        ('disabled',lambda c:c['candidates'][0]['filter'].update(flags=0)),
        ('query-mask-reject',lambda c:c['query']['filter'].update(accept_mask=0)),
        ('skip-first-mask',lambda c:c['candidates'][0]['filter'].update(category=12)),
        ('skip-all',lambda c:[x['filter'].update(category=12) for x in c['candidates']]),
        ('material-bypass',lambda c:c['candidates'][0]['filter'].update(category=16)),
        ('no-scene',lambda c:c['candidates'][0].update(has_scene=False)),
        ('material-per-candidate',lambda c:c['candidates'][1].update(name_hex=b'roadfr8.col\0'.hex())),
        ('scene-zero',lambda c:c['query'].update(scene_id=0)),
        ('prior-current',lambda c:c['initial']['spring'].__setitem__(6,12)),
        ('initial-full',lambda c:c['initial']['spring'].__setitem__(8,4)),
        ('initial-overfull',lambda c:c['initial']['spring'].__setitem__(8,9)),
        ('empty-preserves-current',lambda c:c['query'].update(initial_current_candidate=12))]:
        add_case(name,order=() if name=='empty-preserves-current' else (0,1),capacity=4,change=change)
    def reset(c):c['initial']['spring'][8]=3;c['reset_before']=True;c['reset_after']=True
    add_case('reset-before-and-after',capacity=4,change=reset)
    # Both-node strict volume split, equality must select target.
    for volume,name in [(128.,'greater'),(48.,'equal'),(8.,'smaller')]:
        shape=copy.deepcopy(shapes[0]);shape['name']='synthetic-volume-'+name;shape['nodes'][0]['volume']=volume
        shapes.append(shape);add_case('both-node-volume-'+name,shape=len(shapes)-1,order=(0,))
    # Optional smooth normals are retained under mode0, so an accidental mode1
    # route is observably wrong even in the absence of real-mesh interpolation.
    smooth=copy.deepcopy(meshes[0]);smooth['name']='synthetic-optional-normals';smooth['normals']=[[.25,1.,.125]]*len(smooth['vertices'])
    meshes.append(smooth);add_case('mode0-optional-normals',mesh=len(meshes)-1)
    tie=copy.deepcopy(meshes[0]);tie['name']='synthetic-equal-face-ties';tie['vertices']=tie['vertices'][:3]*2
    tie['nodes'][0].update(center=[0.,0.,0.],half=[2.,.25,2.],radius=source_radius([2.,.25,2.]))
    meshes.append(tie);add_case('equal-face-ties',mesh=len(meshes)-1,order=(0,2))
    # Spatially separate ray hits: target child order changes pair discovery,
    # independently of the query's fixed child/ray index order.
    partition=copy.deepcopy(meshes[0]);partition['name']='synthetic-partitioned-pair-order'
    partition['vertices']=[[.10,0.,-.2],[.25,0.,.7],[.40,0.,-.2],[-.40,0.,-.2],[-.25,0.,.7],[-.10,0.,-.2]]
    partition['nodes'][0].update(center=[0.,0.,.25],half=[.4,.25,.45],radius=source_radius([.4,.25,.45]))
    meshes.append(partition);partition_id=len(meshes)-1
    add_case('target-dependent-pair-order',mesh=partition_id,order=(0,))
    reversed_partition=copy.deepcopy(partition);reversed_partition['name']+='-reversed';reversed_partition['nodes'][0]['children'].reverse()
    meshes.append(reversed_partition);add_case('target-reversed-pair-order',mesh=len(meshes)-1,order=(0,))
    # Affine transforms include rotations and non-orthonormal finite bases; the
    # source uses transpose affine inverses without repairing such inputs.
    for i,matrix in enumerate([
        [0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,3.,4.,5.,1.],
        [1.,.1,0.,0.,0.,1.,.2,0.,.05,0.,1.,0.,-4.,5.,-6.,1.]]):
        def transform(c,matrix=matrix):
            c['query']['world']=matrix.copy()
            for j,item in enumerate(c['candidates']):item['world']=matrix.copy();item['world'][13]=add(item['world'][13],(.5,-.5,.5)[j])
        add_case('prepared-affine-transform-'+str(i),change=transform)
    # Both-node closest-point equality is rejected. The axis bounds alone are
    # inclusive. Radius1 follows original RSQRT/reciprocal for half(.6,.8,0).
    tangent=copy.deepcopy(meshes[0]);tangent['name']='synthetic-sphere-box-tangent'
    tangent['vertices']=[[1.4,-.8,0.],[2.,.8,0.],[2.6,-.8,0.]]*2
    tangent['vertices']=[[f32(x) for x in v] for v in tangent['vertices']]
    tangent['faces']=[dict(d=0.,normal=[0,0,-32766],vertices=[0,1,2]),dict(d=0.,normal=[0,0,-32766],vertices=[3,4,5])]
    tangent['nodes'][0].update(volume=0.,center=[2.,0.,0.],half=[f32(.6),f32(.8),0.],radius=source_radius([f32(.6),f32(.8),0.]))
    assert bits(tangent['nodes'][0]['radius'])==bits(1.)
    for center,name in [(fp(bits(2.)-1),'inside'),(2.,'equal'),(fp(bits(2.)+1),'outside')]:
        variant=copy.deepcopy(tangent);variant['name']+='-'+name;variant['nodes'][0]['center'][0]=center
        meshes.append(variant);cases.append(make_case('node-sphere-tangent-'+name,0,shapes,[make_candidate(len(meshes)-1)]))
    zero_shape=copy.deepcopy(shapes[0]);zero_shape['name']='synthetic-zero-radius-query'
    zero_shape['nodes'][0].update(volume=0.,center=[0.,0.,0.],half=[0.,0.,0.],radius=0.)
    for ray in zero_shape['rays']:
        for k in (0,1,2,4,5,6):ray[k]=0
    shapes.append(zero_shape);cases.append(make_case('node-zero-query-radius',len(shapes)-1,shapes,[make_candidate(0)]))
    zero_mesh=copy.deepcopy(tangent);zero_mesh['name']='synthetic-zero-radius-target'
    zero_mesh['nodes'][0].update(volume=0.,center=[0.,0.,0.],half=[0.,0.,0.],radius=0.)
    zero_mesh['vertices']=[[0.,0.,0.]]*6
    meshes.append(zero_mesh);cases.append(make_case('node-zero-target-radius',0,shapes,[make_candidate(len(meshes)-1)]))
    # Each real query tree against every complete static TA mesh. Placements are
    # explicitly chosen local finite poses to exercise each source mesh; no live
    # registration or candidate order is implied.
    actual_shapes,actual_meshes=load_actual_inputs(words)
    shape_base=len(shapes);mesh_base=len(meshes);shapes.extend(actual_shapes);meshes.extend(actual_meshes)
    for si,shape in enumerate(actual_shapes):
        for mi,mesh in enumerate(actual_meshes):
            world,face,ray=face_pose(shape,mesh)
            case=make_case(shape['name']+'-TA-'+str(mi).zfill(2),shape_base+si,shapes,[make_candidate(mesh_base+mi)],world=world)
            case['fixture_placement']=dict(target_face=face,query_ray=ray,purpose='Chosen face-centroid crossing; not live state')
            cases.append(case)
        floor=actual_meshes[4];world,face,ray=face_pose(shape,floor)
        for name,order,offsets in [('near-far',(0,1),(.25,-.25)),('far-near',(1,0),(.25,-.25)),('equal',(0,1),(0.,0.)),('repeat',(0,0,0),(0.,0.))]:
            cs=[]
            for i,y in enumerate(offsets):
                target=IDENTITY.copy();target[13]=y;item=make_candidate(mesh_base+4,10+i,target);item['name_hex']=('roadfr'+str(3+5*i)+'.col\0').encode().hex();cs.append(item)
            cases.append(make_case(shape['name']+'-actual-floor-'+name,shape_base+si,shapes,cs,order=order,world=world,capacity=17))
        # Explicit authored-placement list is fixture supplied, never claimed as
        # original live broad-phase discovery. All37 authored transforms/order
        # below are used unchanged in this stated synthetic enumeration.
        bindings=json.loads((OUT/'inputs/ta-static-bindings.json').read_text());cs=[]
        lookup={mesh['name'].lower():mesh_base+i for i,mesh in enumerate(actual_meshes)}
        for i,b in enumerate(bindings):
            assert b['look']==[0.,0.,1.] and b['up']==[0.,1.,0.]
            target=IDENTITY.copy();target[12:15]=b['position'];cs.append(make_candidate(lookup[b['collision_member'].lower()],10+i,target))
        floor_binding=next(b for b in bindings if b['collision_member'].lower()==floor['name'].lower())
        placed=world.copy();placed[12:15]=[add(world[12+i],floor_binding['position'][i]) for i in range(3)]
        for reverse in (False,True):
            order=list(range(len(cs)))
            if reverse:order.reverse()
            c=make_case(shape['name']+'-explicit37-authored-placements-'+('reverse' if reverse else 'forward'),shape_base+si,shapes,cs,order=order,world=placed,capacity=9)
            c['candidate_order_provenance']='Caller-supplied SCB enumeration for fixture only; no live candidate construction or registration'
            cases.append(c)
    return cases

def negative_controls(words,dispatch,fixture):
    shapes,meshes,cases=fixture['shapes'],fixture['meshes'],fixture['cases'];byname={c['name']:c for c in cases};results=[]
    def compare(name,base,changed,field,description):
        left=base['expected'];right=changed
        for key in field:left=left[key];right=right[key]
        assert left!=right,name
        results.append(dict(name=name,baseline=base['name'],field=list(field),baseline_sha256=sha(json.dumps(left,separators=(',',':')).encode()),
            altered_sha256=sha(json.dumps(right,separators=(',',':')).encode()),description=description,rejected=True))
    a,b=byname['target-dependent-pair-order'],byname['target-reversed-pair-order']
    compare('target-child-order',a,b['expected'],('pair','pair_tags'),'Authentic words, target children exchanged; pair order differs.')
    a,b=byname['near-then-far'],byname['far-then-near']
    compare('candidate-order',a,b['expected'],('records',),'Authentic words, supplied candidate enumeration reversed; cumulative callback records differ.')
    a,b=byname['both-node-volume-equal'],byname['both-node-volume-greater']
    compare('volume-equality-split',a,b['expected'],('visits',),'Authentic words, explicit volume control changes split side; exact visit trace detects it.')
    a=byname['two-ray-smoke'];changed_shapes=copy.deepcopy(shapes);changed_shapes[0]['nodes'][0]['children'].reverse()
    altered,_=execute_case(words,dispatch,changed_shapes,meshes,a)
    compare('query-child-order',a,altered,('pair','pair_tags'),'Authentic words, query children exchanged; pair order differs.')
    a=byname['mode0-optional-normals'];mode1=copy.deepcopy(a);mode1['query']['mode']=1
    altered,_=execute_case(words,dispatch,shapes,meshes,mode1)
    compare('mode1-smoothing',a,altered,('pair','normals'),'Authentic words, mode1 input consumes retained optional normals and changes output words.')
    a=byname['two-ray-smoke'];changed_shapes=copy.deepcopy(shapes);changed_shapes[0]['rays'][0][3]^=0x12345678
    altered,_=execute_case(words,dispatch,changed_shapes,meshes,a)
    assert altered['visits']==a['expected']['visits'] and altered['pair']['fractions']==a['expected']['pair']['fractions']
    compare('opaque-W-copy',a,altered,('records',),'Only caller opaque endpoint W changed; geometry unchanged but full callback bytes differ.')
    # Counterfactual policy replay uses authentic geometric leaf-hit events. It
    # is explicitly not represented as an original instruction execution.
    a=byname['equal-face-ties'];events=a['expected']['leaf_hits'];wrong_pairs={};wrong_selected=None
    for identity,ray,face,fraction in events:
        if ray not in wrong_pairs or fp(fraction)<=fp(wrong_pairs[ray][2]):wrong_pairs[ray]=(identity,face,fraction)
        if wrong_selected is None or fp(fraction)<fp(wrong_selected[3]):wrong_selected=(identity,face,ray,fraction)
    wrong=copy.deepcopy(a['expected']);wrong['pair']['pair_owners'][:wrong['pair']['count']]=[list(wrong_pairs[t][:2]) for t in wrong['pair']['pair_tags'][:wrong['pair']['count']]]
    compare('per-pair-nonstrict-policy-replay',a,wrong,('pair','pair_owners'),'Counterfactual <= policy replay over authentic leaf events changes first-tie ownership.')
    wrong=copy.deepcopy(a['expected']);wrong['pair']['selected_ids']=list(wrong_selected[:3])
    compare('global-strict-policy-replay',a,wrong,('pair','selected_ids'),'Counterfactual < policy replay over authentic leaf events changes last-tie selection.')
    a=byname['repeat-candidate'];assert len(a['expected']['callback_calls'])==3
    count=a['expected']['spring_state'][8]-a['initial']['spring'][8]
    assert count==6 and a['expected']['pair']['count']==2
    results.append(dict(name='final-only-callback-count',baseline=a['name'],rejected=True,
        authentic_appended=count,counterfactual_final_only_appended=2,
        description='Three authentic callbacks append the full cumulative two-pair span each time; final-only cannot reproduce six records.'))
    raw_rejected=False
    for a in cases:
        if '-TA-' not in a['name']:continue
        shape_index=a['query']['shape'];shape=shapes[shape_index];raw=(OUT/'inputs'/shape['name']).read_bytes()
        changed=copy.deepcopy(shape)
        for i,n in enumerate(changed['nodes']):
            row=struct.unpack_from('<7f2H',raw,92+32*i);n.update(center=list(row[1:4]),half=list(row[4:7]),radius=source_radius(row[4:7]))
        changed_shapes=shapes.copy();changed_shapes[shape_index]=changed
        altered,_=execute_case(words,dispatch,changed_shapes,meshes,a)
        if altered['visits']!=a['expected']['visits']:
            compare('raw-file-nodes',a,altered,('visits',),'Authentic words, raw file centers/half/radii substituted for runtime quantization; visit trace differs.')
            raw_rejected=True;break
    assert raw_rejected,'No raw-file-node witness found'
    return dict(no_original_instruction_word_mutations=True,controls=results,
        native_implementation_mutations='Separately owned by native test runner; policy replay controls above are not native mutation-test claims.')

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--frozen',action='store_true');ap.add_argument('--smoke',action='store_true');ap.add_argument('--actual-smoke',action='store_true');args=ap.parse_args()
    OUT.mkdir(parents=True,exist_ok=True);words,dispatch=authenticated_words();shapes,meshes=synthetic_inputs()
    if args.actual_smoke:
        shapes,meshes=load_actual_inputs(words);pose,face,ray=face_pose(shapes[0],meshes[4]);cases=[make_case('podboxm-full-zone01-floor',0,shapes,[make_candidate(4)],world=pose)]
    elif args.smoke:cases=[make_case('two-ray-smoke',0,shapes,[make_candidate(0)])]
    else:cases=build_cases(words,shapes,meshes)
    coverage=set();branches={};steps=0
    for i,c in enumerate(cases):
        result,m=execute_case(words,dispatch,shapes,meshes,c);c['expected']=result
        coverage.update(m.coverage);steps+=m.steps
        for pc,outcomes in m.branches.items():branches.setdefault(pc,set()).update(outcomes)
        print(json.dumps(dict(index=i,name=c['name'],returned=result['returned'],count=result['pair']['count'],steps=m.steps,visits=len(m.visits))),flush=True)
    fixture=dict(schema=1,state_offsets=[f'{x:X}' for x in STATE_OFFSETS],shapes=shapes,meshes=meshes,cases=cases)
    raw=(json.dumps(fixture,separators=(',',':'))+'\n').encode();path=OUT/('smoke.json' if args.smoke or args.actual_smoke else 'native-fixtures.json');path.write_bytes(raw)
    if args.smoke or args.actual_smoke:return
    controls=negative_controls(words,dispatch,fixture);(OUT/'negative-controls.json').write_text(json.dumps(controls,indent=2)+'\n')
    report=dict(schema=1,cases=len(cases),source_reads=0,steps=steps,fixture_sha256=sha(raw),fixture_bytes=len(raw),elf_sha256=ELF_SHA,
        entry='00122660',static_dispatch='00121410',tree='00107170',leaf='001082B0',callback='00294BB0',append='002963E0',
        no_original_words_patched=True,no_calls_stubbed=True,no_RAM_reads_or_reconstruction=True,no_native_expected_values=True,no_uninitialized_read_fallback=True,
        numerical_domain='Individually rounded host IEEE binary32 ordinary finite geometry; not all physical EE/VU exceptional modes',
        callbacks=sum(len(c['expected']['callback_calls']) for c in cases),queries_with_geometry_hits=sum(bool(c['expected']['returned']) for c in cases),
        candidate_geometry_hits=sum(bool(x['returned']) for c in cases for x in c['expected']['tree_calls']),
        leaf_hits=sum(len(c['expected']['leaf_hits']) for c in cases),tree_calls=sum(len(c['expected']['tree_calls']) for c in cases),
        optional_normal_reads=sum(c['expected']['optional_normal_reads'] for c in cases),
        world_entry_w=sorted({x['vf9_w'] for c in cases for x in c['expected']['world_lanes']}),
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
    (OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    (OUT/'executed-original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,dispatch=dispatch,
        executed_words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)]),indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')},indent=2))
if __name__=='__main__':main()
