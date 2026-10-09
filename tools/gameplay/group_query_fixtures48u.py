"""48U full original122660 grouped-collision queries on strict sparse inputs.

No RAM backing, patched words, helper interception, forced returns or native
expected values. Prior fixtures, evidence and interpreter files are read-only.
"""
import copy
import hashlib
import json
import random
import struct
import sys
from pathlib import Path
sys.dont_write_bytecode=True
import type3_query_fixtures48p as old
import candidate_collection_fixtures48s as collection
from type3_query_fixtures48p import Q,SHAPE,RESULT,SPRING,RECORDS,LIST,ENDPOINTS,NORMALS,POINTS,LOCAL_START,LOCAL_DIR,FRACTIONS,HITS,LENGTHS,WORLD_START,WORLD_DIR,SCENE_IDS,BODY,BODY_SCENE,GP,SLOTS,STATE_OFFSETS,IDENTITY,LEAF_TAG,bits,fp,put,get,zero
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/groupQuery48U'
ELF_SHA=old.ELF_SHA
RETURN=0xfffffff0
sha=old.sha

def dump(name,data):
    p=OUT/name;p.parent.mkdir(parents=True,exist_ok=True)
    raw=(json.dumps(data,indent=2)+'\n').encode();p.write_bytes(raw);return sha(raw)

def load_words():
    words,dispatch=old.load_words();extra,table=collection.load_words()
    for pc,w in extra.items():
        assert pc not in words or words[pc]==w
        words[pc]=w
    for pc,w in table.items():
        assert pc not in dispatch or dispatch[pc]==w
        dispatch[pc]=w
    switch=json.loads((old.OUT/'map/dispatch-tables.json').read_text())
    for a in switch['arrays']:
        for i,w in enumerate(a['words']):dispatch[a['va']+4*i]=w
    elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
    for pc,w in list(words.items())+list(dispatch.items()):assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==w,hex(pc)
    return words,dispatch

class GroupQueryOriginal(old.Type3Original):
    def __init__(self,words,identities):
        super().__init__(words,identities);self.full_calls=[];self.group_events=[];self.extension_coverage=set();self.setter_writes=[];self.watch=[]
    def write(self,a,v,size=4):
        if self.executing and any(lo<=a and a+size<=hi for lo,hi in self.watch):self.setter_writes.append(dict(pc=self.pc,address=a,size=size,value=v&((1<<(size*8))-1)))
        return super().write(a,v,size)
    def execute(self,pc):
        w=self.words[pc]
        if w>>26==3:self.full_calls.append(dict(pc=pc,target=(w&0x3ffffff)*4))
        elif w>>26==0 and w&63==9:self.full_calls.append(dict(pc=pc,target=self.g[(w>>21)&31][0]))
        if pc in (0x135f30,0x126e40,0x135cd0,0x135cf0,0x11f680):
            self.group_events.append(dict(pc=pc,first=self.identity(self.g[4][0]),member_index=self.g[5][0] if pc==0x135cf0 else None))
        if pc==0x122a1c:
            self.current.update(returned=self.g[2][0],after=self.pair(),prepared_after=self.prepared());self.calls.append(self.current)
        if pc==0x122b08 and self.pending is not None:
            self.pending.update(state_after=self.state(),records_after=self.records());self.callbacks.append(self.pending);self.pending=None
        # Reuse the exact48S REGIMM64 behavior and load extension locally.
        # No previous interpreter is modified. Existing48P is audited below.
        if w>>26==1 and (w>>16)&31 in (0,1):
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False;self.extension_coverage.add(pc)
            negative=bool(self.g[(w>>21)&31][1]&0x80000000);take=negative==(((w>>16)&31)==0)
            self.branches.setdefault(pc,set()).add(take);imm=w&65535;imm-=65536 if imm&32768 else 0
            return pc+4+imm*4 if take else pc+8
        target=super().execute(pc)
        if w>>26 in (0x20,0x21,0x23,0x24,0x25):
            rt=(w>>16)&31;self.g[rt][1]=0xffffffff if w>>26 in (0x20,0x21,0x23) and self.g[rt][0]&0x80000000 else 0
        return target

def initialize(words,dispatch,meshes,c,klass=GroupQueryOriginal):
    cs=c['candidates'];q=c['query'];addresses={0:0,Q:100}
    addresses.update({0x20000+i*0x1000:x['id'] for i,x in enumerate(cs)})
    addresses.update({0xe00000+i*0x100:x['id'] for i,x in enumerate(c.get('groups',[]))})
    reverse={v:k for k,v in addresses.items()};m=klass(words,addresses);m.reverse=reverse;m.shape_addresses={};m.g[28][0]=GP;m.record_slots=len(c['initial_records'])
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
    for off,v in ((0x88,c.get('query_tag',3)),(0x8c,SHAPE),(0x90,RESULT),(0x9c,SPRING),(0xa0,BODY),(0xe4,0x294bb0 if q['callback'] else 0),(0xf4,reverse[q['initial_current_candidate']])):m.write(Q+off,v)
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
        obj=reverse[item['id']];alias=item.get('shape_alias_id',item['id']); ai=next(i for i,x in enumerate(cs) if x['id']==alias);target=0x50000+ai*0x20000;m.shape_addresses[item['id']]=target;vbase=target+0x1000;nbase=target+0x7000;nodebase=target+0xc000;facebase=target+0x16000;scene=0xb0001+ci*0x1000
        mesh=meshes[item['mesh']];zero(m,obj,0x100);zero(m,target,0x120)
        write_filter(obj,item['filter'],0x31000+ci*0x1000);sphere(obj,item['sphere']);m.write(obj+2,item['type_tag'],1);m.write(obj+0xe8,item.get('callback',0))
        for off,v in ((0x88,item['shape_tag']),(0x8c,target if item.get('shape_present',True) else 0),(0x9c,scene if item['has_scene'] else 0)):m.write(obj+off,v)
        for i,v in enumerate(bytes.fromhex(item['name_hex'])):m.write(scene+0x38+i,v,1)
        put(m,obj+0x40,item['copied40']);put(m,obj+0x50,item['copied50']);m.vector(target,item['world'])
        for off,key in ((0x40,'local_matrix'),(0x80,'base_matrix'),(0xc0,'previous_matrix')):put(m,target+off,item.get(key,[bits(x) for x in IDENTITY]))
        for off,v in ((0x100,vbase),(0x104,nbase if mesh['normals'] else 0),(0x108,0),(0x114,nodebase if item.get('geometry_present',True) else 0),(0x118,facebase)):m.write(target+off,v)
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
    world=0xe10000;m.write(0x3e1ce8,world);m.write(world+0x590,c.get('group_scale',bits(1.)))
    for i,g in enumerate(c.get('groups',[])):
        a=reverse[g['id']];desc=0xe20000+i*0x1000;members=0xe60000+i*0x1000
        zero(m,a,0x100);write_filter(a,g.get('filter',old.filter_default(g['id'])),0xe90000+i*0x400)
        put(m,a+0x40,g.get('copied40',[bits(101.+j) for j in range(4)]));put(m,a+0x50,g.get('copied50',[bits(111.+j) for j in range(4)]));sphere(a,g.get('sphere',[31.,32.,33.,100.]));m.write(a+0x88,7)
        m.write(a+2,g.get('tag',q['group_tag']),1)
        for k,v in enumerate(g['position']):m.write(a+8+2*k,v,2)
        m.write(a+0xe,i,1);m.write(a+0x10,g['width_byte'],1);m.write(a+0x11,g['height_byte'],1)
        m.write(world+0x6c+4*i,desc);m.write(desc+0x1fc,g.get('count',len(g['members'])));m.write(desc+0x204,members)
        put(m,members,[reverse[x] for x in g['members']])
        for off,v in zip((0x180,0x184,0x18c,0x194),g['dimensions']):m.write(desc+off,v)
    return m

def candidate_state(m,c):
    return [dict(identity=x['id'],flags=m.read(m.reverse[x['id']]+0xf8,1),copied40=get(m,m.reverse[x['id']]+0x40,4),copied50=get(m,m.reverse[x['id']]+0x50,4),
        sphere=get(m,m.reverse[x['id']]+0x60,4)+[m.read(m.reverse[x['id']]+0x80)],shape_words=get(m,m.shape_addresses[x['id']],72)) for x in c['candidates']]


def group_state(m,c):
    return [dict(identity=g['id'],flags=m.read(m.reverse[g['id']]+0xf8,1),copied40=get(m,m.reverse[g['id']]+0x40,4),copied50=get(m,m.reverse[g['id']]+0x50,4),sphere=get(m,m.reverse[g['id']]+0x60,4)+[m.read(m.reverse[g['id']]+0x80)]) for g in c.get('groups',[])]


def execute_case(words,dispatch,meshes,c,klass=GroupQueryOriginal):
    m=initialize(words,dispatch,meshes,c,klass);initial=dict(pair=m.pair(),prepared=m.prepared(),state=m.state(),records=m.records(),candidates=candidate_state(m,c),groups=group_state(m,c))
    for x in c['candidates']:
        a=m.reverse[x['id']];s=m.shape_addresses[x['id']];m.watch.extend([(a,a+0x100),(s,s+0x120)])
    put(m,LIST,[m.reverse[i] for i in c['outer_order']]);m.g[4][0]=Q;m.g[5][0]=len(c['outer_order']);m.g[6][0]=LIST
    m.g[29][0]=0x1ff0000;m.g[31][0]=RETURN;initialized=set(m.overlay);m.executing=True;pc=0x122660;boundary=None
    while pc!=RETURN:
        if pc==0x11f7b0 and m.g[3][0]==0:boundary='null-shape0-geometry';break
        if pc==0x294c34:boundary='dynamic-294BB0-body';break
        if pc==0x1229a4 and m.g[5][0]==0:boundary='null-group-member-result';break
        if pc in (0x122b18,0x122888):boundary='member-callback';break
        if pc not in words:boundary='unmapped-original-dependency';break
        target=m.execute(pc)
        if target is not None:
            if not m.annul:assert m.execute(pc+4) is None
            pc=target
        else:pc+=4
    m.executing=False
    result=dict(returned=m.g[2][0] if boundary is None else None,boundary=boundary,stop_pc=pc,requires_mode2=0x10f580 in m.coverage,
        initial=initial,after_prepare=m.after_prepare,pair=m.pair(),prepared=m.prepared(),query_flags=m.read(Q+0xf8,1),current_candidate=m.identity(m.read(Q+0xf4)),
        spring_state=m.state(),records=m.records(),candidates=candidate_state(m,c),groups=group_state(m,c),calls=m.calls,events=m.events,callback_calls=m.callbacks,
        query_copied40=get(m,Q+0x40,4),query_copied50=get(m,Q+0x50,4),trace=m.trace,leaf_updates=m.leaf_updates,selections=m.selections,append_events=m.append_events,
        group_events=m.group_events,full_calls=m.full_calls,setter_writes=m.setter_writes)
    assert m.data==b'' and m.allowed_source_addresses==set() and not m.source_reads
    assert m.read_bytes<=initialized|m.written_bytes
    if boundary is None:assert m.g[29][0]==0x1ff0000
    result['byte_audit']=dict(initialized_bytes=len(initialized),execution_read_bytes=len(m.read_bytes),execution_written_bytes=len(m.written_bytes),source_reads=0)
    return result,m


def case(name):
    c=old.make_case(name);c.update(groups=[dict(id=101,position=[0,0,0],width_byte=0,height_byte=0,dimensions=[bits(x) for x in [2.,1.,3.,5.]],members=[10,11,12])],
        group_scale=bits(1.),outer_order=[101],native_scope='complete',native_reason=None)
    for x in c['candidates']:
        x['mesh']=0;x['local_matrix']=[bits(v) for v in x['world']];x['base_matrix']=[bits(v) for v in IDENTITY];x['previous_matrix']=[bits(31.+i) for i in range(16)]
    return c


def cases():
    cs=[]
    def add(name,edit=None,scope='complete',reason=None):
        c=case(name)
        if edit:edit(c)
        c['native_scope']=scope;c['native_reason']=reason;cs.append(c);return c
    add('group-shape0-three-members')
    for members in ([],[10],[10,10,11],[10,11,10],[12,11,10]):add('member-order-'+str(members),lambda c,v=members:c['groups'][0].update(members=v))
    for count in (0,0xffffffff):add('signed-group-count-'+str(count),lambda c,v=count:c['groups'][0].update(count=v))
    for order in ([101,101],[10,101],[101,10],[10,101,11,101],[101,11,101,10],[]):add('outer-order-'+str(order),lambda c,v=order:c.update(outer_order=v))
    for pos in ([0,65535,0],[500,0,500],[0,1,0],[0,2,0]):add('group-position-'+str(pos),lambda c,v=pos:c['groups'][0].update(position=v))
    add('group-disabled-bit0',lambda c:c['query']['filter'].update(flags=c['query']['filter']['flags']|1))
    add('direct-unaffected-by-group-bit0',lambda c:(c['query']['filter'].update(flags=c['query']['filter']['flags']|1),c.update(outer_order=[10,101])))
    add('wrong-group-tag',lambda c:c['groups'][0].update(tag=42))
    add('direct-tag-precedence',lambda c:(c['query'].update(group_tag=7),c.update(outer_order=[10])))
    add('group-member-tag-ignored',lambda c:[x.update(type_tag=42) for x in c['candidates']])
    add('group-filter-bypass',lambda c:[x['filter'].update(category=0,accept_mask=0,flags=0) for x in c['candidates']])
    add('direct-filter-block',lambda c:(c.update(outer_order=[10]),c['candidates'][0]['filter'].update(category=0,accept_mask=0,flags=0)))
    add('inactive-first-member',lambda c:c['candidates'][0]['filter'].update(flags31=0))
    add('inactive-all-members',lambda c:[x['filter'].update(flags31=0) for x in c['candidates']])
    add('inactive-member-still-mutated',lambda c:(c['groups'][0].update(position=[1,2,3]),c['candidates'][0]['filter'].update(flags31=0)))
    for tag in (2,3,4,5,6,7,0xffffffff):
        add('member-shape-tag-'+str(tag),lambda c,t=tag:[x.update(shape_tag=t) for x in c['candidates']],
            'complete',None)
    add('group-member-compound-shape1',lambda c:c['candidates'][0].update(shape_tag=1),'unsupported','compound-shape1')
    for members in ([100],[100,10],[10,100]):add('query-member-'+str(members),lambda c,v=members:c['groups'][0].update(members=v))
    for cap in (0,1,2,3,12):add('record-capacity-'+str(cap),lambda c,v=cap:c['initial_state'].__setitem__(9,v))
    for count in (11,12,13):add('record-start-count-'+str(count),lambda c,v=count:c['initial_state'].__setitem__(8,v))
    add('callback-disabled',lambda c:c['query'].update(callback=False))
    add('callback-skip-mask',lambda c:[x['filter'].update(category=8) for x in c['candidates']])
    add('callback-material-disabled',lambda c:[x['filter'].update(category=0) for x in c['candidates']])
    for name in ('roadfr9.col\0','roadfx3.col\0','roadfr/.col\0','road\0'):
        add('material-'+name[:-1],lambda c,n=name:[x.update(name_hex=n.encode().hex()) for x in c['candidates']])
    add('callback-member-owner-boundary',lambda c:c['candidates'][0].update(callback=0x294bb0),'unsupported','member-callback')
    add('callback-owner-boundary-with-query-null',lambda c:(c['query'].update(callback=False),c['candidates'][0].update(callback=0x294bb0)),'unsupported','member-callback')
    for key in ('shape_present','geometry_present'):add('null-'+key,lambda c,k=key:c['candidates'][1].update({k:False}),'unsupported',key)
    def two(c):
        g=copy.deepcopy(c['groups'][0]);g.update(id=102,position=[1,0,0],members=[11,10,11]);c['groups'].append(g);c['outer_order']=[101,102,101]
    add('repeated-groups-shared-members',two)
    def shared(c):
        for x in c['candidates']:x.update(shape_alias_id=10,local_matrix=c['candidates'][0]['local_matrix'].copy())
    add('shared-shape-members',shared)
    add('shared-shape-repeated-groups',lambda c:(two(c),shared(c)))
    add('shared-shape-direct-group-order',lambda c:(shared(c),c.update(outer_order=[10,101,11,101,12])))
    for count in (2,3,4):
        def multi(c,n=count):
            c['pairs']=[copy.deepcopy(c['pairs'][0]) for _ in range(n)];c['scene_matrices']=[None]*(2*n);c['initial_state'][7]=n
            for i,p in enumerate(c['pairs']):p[0][0]=p[1][0]=bits(.25 if i%2==0 else 20.25)
        add('multiple-rays-'+str(count),multi)
    add('group-negative-height',lambda c:c['groups'][0].update(height_byte=128))
    add('group-negative-width',lambda c:c['groups'][0].update(width_byte=255))
    for scale in (0.,-0.,.125,-2.):add('world-scale-'+str(bits(scale)),lambda c,v=scale:(c.update(group_scale=bits(v)),c['groups'][0].update(position=[2,3,4])))
    for kind,matrix in [('scale',[2.,0.,0.,0.,0.,3.,0.,0.,0.,0.,.5,0.,0.,0.,0.,1.]),('rotate',[0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,0.,0.,0.,1.]),('shear',[1.,.25,0.,0.,.5,1.,0.,0.,0.,.125,1.,0.,0.,0.,0.,1.]),('full-w',[1.,0.,0.,.25,0.,1.,0.,.5,0.,0.,1.,.125,0.,0.,0.,2.])]:
        add('base-matrix-'+kind,lambda c,v=matrix:[x.update(base_matrix=[bits(f) for f in v]) for x in c['candidates']], 'complete',None)
    add('nonzero-endpoint-metadata',lambda c:c['scene_matrices'].__setitem__(0,IDENTITY.copy()),'unsupported','endpoint-metadata')
    add('group-sphere-exact-touch',lambda c:(c.update(pairs=[[[bits(v) for v in (0.,1.,0.,1.)],[bits(v) for v in (0.,-1.,0.,1.)]]]),c['groups'][0].update(position=[4,0,0])))
    add('group-sphere-just-outside',lambda c:(c.update(pairs=[[[bits(v) for v in (0.,1.,0.,1.)],[bits(v) for v in (0.,-1.,0.,1.)]]]),c['groups'][0].update(position=[4,0,0],dimensions=[bits(v) for v in (2.,1.,2.9999995,5.)])))
    for tag in (4,5):
        def compact(c,t=tag):
            for x in c['candidates']:x.update(shape_tag=t,shape_alias_id=10,local_matrix=c['candidates'][0]['local_matrix'].copy())
            c['groups'][0].update(position=[1,2,3],width_byte=255,height_byte=128)
        add('shared-compact-shape-'+str(tag),compact)
        add('repeated-compact-shape-'+str(tag),lambda c,t=tag:(compact(c,t),two(c)))
    add('member-sphere-strict-equality',lambda c:([x.update(mesh=1,local_matrix=[bits(v) for v in IDENTITY]) for x in c['candidates']],c.update(pairs=[[[bits(v) for v in (0.,1.,0.,1.)],[bits(v) for v in (0.,-1.,0.,1.)]]])))
    add('normal-mode1',lambda c:(c.update(mode=1),[x.update(mesh=2) for x in c['candidates']]))
    add('normal-mode2',lambda c:(c.update(mode=2),[x.update(mesh=2) for x in c['candidates']]),'unsupported','normal-mode2')
    add('normal-mode2-inactive',lambda c:(c.update(mode=2),[x.update(mesh=2) or x['filter'].update(flags31=0) for x in c['candidates']]))
    add('late-unsupported-compound-inactive',lambda c:(c['candidates'][0].update(shape_tag=1),c['candidates'][0]['filter'].update(flags31=0)))
    add('late-member-callback-inactive',lambda c:(c['candidates'][0].update(callback=0x294bb0),c['candidates'][0]['filter'].update(flags31=0)))
    add('early-null-shape-group-rejected',lambda c:(c['candidates'][0].update(shape_present=False),c['groups'][0].update(position=[500,0,500])))
    add('early-null-geometry-group-disabled',lambda c:(c['candidates'][0].update(geometry_present=False),c['query']['filter'].update(flags=c['query']['filter']['flags']|1)))
    rng=random.Random(0x48a122660)
    for i in range(32):
        def randomized(c):
            c['groups'][0].update(position=[rng.randrange(3),0,rng.randrange(2)],members=[rng.choice([10,11,12]) for _ in range(1+rng.randrange(5))],width_byte=rng.randrange(256),height_byte=rng.randrange(256))
            c['outer_order']=[rng.choice([10,11,101]) for _ in range(1+rng.randrange(4))]
            for x in c['candidates']:
                local=IDENTITY.copy();local[13]=rng.choice([-.75,-.5,0.,.25,.5,.75]);x['local_matrix']=[bits(v) for v in local]
            c['initial_state'][9]=rng.randrange(13)
        add('finite-seeded-'+str(i),randomized)
    add('shared-shape-partial-final-group',lambda c:(two(c),shared(c),c.update(outer_order=[101,102])))
    def far_shared(c):
        two(c);shared(c);c['outer_order']=[101,102,12]
        c['groups'][1].update(position=[100,0,0],members=[10],dimensions=[bits(v) for v in (2.,1.,200.,200.)])
    add('shared-shape-stale-sphere-then-direct',far_shared)
    add('dynamic-callback-body-boundary',lambda c:c['candidates'][1]['filter'].update(category=2),'unsupported','dynamic-294BB0-body')
    for previous_dynamic in (False,True):
        def previous(c,d=previous_dynamic):
            c['groups'][0]['members']=[10];c['initial_state'][6]=11;c['candidates'][1]['filter']['category']=2 if d else 4
        add('prior-callback-owner-'+('dynamic' if previous_dynamic else 'static'),previous)
    def direct_sphere(c):
        c['groups'][0].update(position=[0,100,0],count=1)
        c['outer_order']=[101,11,10]
        for f in [c['query']['filter']]+[x['filter'] for x in c['candidates']]:
            f['identity_lists']=[dict(mode=1,ids=[101]),dict(mode=1,ids=[101])];f['category_masks']=[0]
        for x in c['candidates']:x['mesh']=2
    add('group-updated-far-sphere-direct-mask-retained',direct_sphere)
    return cs

def meshes():
    mesh=old.synthetic_mesh();tangent=copy.deepcopy(mesh);tangent['nodes'][0].update(center=[2.,0.,0.],half=[1.,0.,0.])
    normals=copy.deepcopy(mesh);normals['normals']=[[0.,1.,0.] for _ in mesh['vertices']]
    return [mesh,tangent,normals]


def freeze_map(words,dispatch):
    elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
    blocks=[]
    for name,lo,hi in [('limited_list',0x122660,0x122ba0),('group_count',0x135cd0,0x135cf0),('group_member',0x135cf0,0x135f24),('group_radius',0x135f30,0x135f80),('sphere_xz',0x126e40,0x126eb0),('shape_matrix_setter',0x11f680,0x11fe44)]:
        raw=elf[lo-0x100000+0x80:hi-0x100000+0x80];rows=[dict(pc=pc,word=words[pc]) for pc in range(lo,hi,4)]
        assert raw==b''.join(struct.pack('<I',r['word']) for r in rows)
        (OUT/'map').mkdir(exist_ok=True);(OUT/'map'/f'{name}.bin').write_bytes(raw)
        blocks.append(dict(name=name,start=lo,end_exclusive=hi,sha256=sha(raw),instructions=rows))
    dump('map/original-words.json',dict(elf_sha256=ELF_SHA,blocks=blocks,file_offset='VA-0x100000+0x80',no_ram=True))
    tables=[]
    for name,lo,count in [('direct-query-switch',0x388b90,7),('group-query-switch',0x388b70,7),('shape-setter-switch',0x388a00,7)]:
        raw=elf[lo-0x100000+0x80:lo-0x100000+0x80+4*count];values=[dispatch[lo+4*i] for i in range(count)]
        assert raw==struct.pack('<7I',*values);(OUT/'map'/f'{name}.bin').write_bytes(raw);tables.append(dict(name=name,va=lo,words=values,sha256=sha(raw)))
    dump('map/dispatch-data.json',dict(elf_sha256=ELF_SHA,arrays=tables))


def preserved_files():
    paths=[]
    for directory in ('evidence/candidateCollection48S','evidence/groupShape48T'):
        paths.extend(p for p in (ROOT/directory).rglob('*') if p.is_file())
    paths += [ROOT/'tools/gameplay'/p for p in ('query_oracle48h.py','spring_fixture48j.py','spring_callback_fixtures48n.py','spring_query_fixtures48n.py','type3_query_fixtures48p.py','candidate_collection_fixtures48s.py','group_shape_fixtures48t.py')]
    return {str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in sorted(paths)}


def audit_adapter():
    words,dispatch=old.load_words();p=json.loads((old.OUT/'native-fixtures.json').read_text());saved=old.Type3Original;old.Type3Original=GroupQueryOriginal
    try:
        for c in p['cases']:
            result,m=old.execute_case(words,dispatch,p['meshes'],c);assert result==c['expected'],c['name']
    finally:old.Type3Original=saved
    r=dict(prior_cases=len(p['cases']),expected_deltas=0,source_reads=0,adapter='unchanged48S REGIMM64 and scalar-load upper64 sign/zero extension composed locally with48P',prior_fixture_sha256=sha((old.OUT/'native-fixtures.json').read_bytes()))
    dump('isa-adapter-audit.json',r);return r


def assertions(cs):
    by={c['name']:c for c in cs};r=lambda name:by[name]['expected'];checks={}
    def check(name,v):assert v,name;checks[name]=True
    a=r('group-shape0-three-members')
    check('full-entry-hits-three-members',[x['identity'] for x in a['calls']]==[10,11,12] and a['returned']==1)
    check('callback-members-not-group',[x['identity'] for x in a['callback_calls']]==[10,11,12])
    check('group-skips-ordinary-sphere-filter',not any(x['target'] in (0x126eb0,0x120e70) for x in a['full_calls']))
    check('group-count-rechecked-four-times',sum(x['target']==0x135cd0 for x in a['full_calls'])==4)
    check('helper-before-dispatch',[x['target'] for x in a['full_calls'] if x['target'] in (0x135cf0,0x11f680,0x126f40)]==[0x135cf0,0x11f680,0x126f40]*3)
    check('group-bit0-blocks-only-groups',not r('group-disabled-bit0')['calls'] and len(r('direct-unaffected-by-group-bit0')['calls'])==1)
    check('direct-tag-wins-group-tag',not r('direct-tag-precedence')['group_events'] and r('direct-tag-precedence')['returned']==1)
    check('wrong-group-tag-no-helper',not r('wrong-group-tag')['group_events'])
    check('group-member-tag-ignored',r('group-member-tag-ignored')['returned']==1)
    check('bilateral-filter-bypassed',r('group-filter-bypass')['returned']==1 and not r('direct-filter-block')['calls'])
    check('group-xz-ignores-y',len(r('group-position-[0, 65535, 0]')['calls'])==3)
    check('group-sphere-inclusive-touch',len(r('group-sphere-exact-touch')['calls'])==3 and not r('group-sphere-just-outside')['calls'])
    check('member-sphere-strict-equality',len(r('member-sphere-strict-equality')['calls'])==3 and not r('member-sphere-strict-equality')['trace'])
    check('member-count-signed',not r('signed-group-count-4294967295')['calls'] and not r('signed-group-count-0')['calls'])
    check('repeated-members-not-deduplicated',[x['identity'] for x in r('member-order-[10, 10, 11]')['calls']]==[10,10,11])
    check('repeated-groups-not-deduplicated',len(r('outer-order-[101, 101]')['calls'])==6)
    check('mixed-order-preserved',[x['identity'] for x in r('outer-order-[10, 101, 11, 101]')['calls']]==[10,10,11,12,11,10,11,12])
    b=r('inactive-member-still-mutated')
    check('inactive-member-helper-mutation',b['candidates'][0]['shape_words']!=b['initial']['candidates'][0]['shape_words'] and b['calls'][0]['returned']==0)
    check('inactive-clears-hits-before-other-resets',r('inactive-all-members')['pair']['hit_flags'][0]==0 and r('inactive-all-members')['pair']['fractions'][0]==by['inactive-all-members']['initial_fractions'][0])
    check('callback-disabled-keeps-hit',r('callback-disabled')['returned']==1 and not r('callback-disabled')['callback_calls'])
    check('zero-record-capacity-keeps-geometric-hit',r('record-capacity-0')['returned']==1 and r('record-capacity-0')['spring_state'][8]==0 and len(r('record-capacity-0')['append_events'])==3)
    check('saturated-records-unchanged',r('record-start-count-12')['records']==r('record-start-count-12')['initial']['records'])
    check('skipped-callback-does-not-append',not r('callback-skip-mask')['append_events'] and len(r('callback-skip-mask')['callback_calls'])==3)
    check('material-name-used',r('material-roadfr9.col')['append_events'][0]['material']==9)
    check('member-callback-boundary-after-query',len(r('callback-member-owner-boundary')['callback_calls'])==1 and r('callback-member-owner-boundary')['stop_pc']==0x122b18)
    check('member-callback-independent-of-query',len(r('callback-owner-boundary-with-query-null')['callback_calls'])==0 and r('callback-owner-boundary-with-query-null')['stop_pc']==0x122b18)
    check('null-shape-exact-dereference-boundary',r('null-shape_present')['stop_pc']==0x1229a4)
    check('null-geometry-exact-dereference-boundary',r('null-geometry_present')['stop_pc']==0x11f7b0)
    check('shape-alias-shared-mutations',all(x['shape_words']==r('shared-shape-members')['candidates'][0]['shape_words'] for x in r('shared-shape-members')['candidates']))
    shared=r('shared-shape-partial-final-group')
    check('shape-alias-does-not-refresh-other-member-sphere',shared['candidates'][1]['sphere']!=shared['candidates'][2]['sphere'] and shared['candidates'][1]['shape_words']==shared['candidates'][2]['shape_words'])
    stale=r('shared-shape-stale-sphere-then-direct')
    check('stale-member-sphere-changes-later-dispatch-reset',[x['identity'] for x in stale['calls']]==[10,11,12,10,12] and stale['calls'][-1]['before']['count']==1 and stale['calls'][-1]['after']['count']==0)
    check('dynamic-callback-stops-before-body-dependency',r('dynamic-callback-body-boundary')['stop_pc']==0x294c34 and len(r('dynamic-callback-body-boundary')['callback_calls'])==1)
    check('prior-dynamic-callback-owner-updated',r('prior-callback-owner-dynamic')['spring_state'][6]==10 and r('prior-callback-owner-static')['spring_state'][6]==11)
    check('query-self-member-not-dropped',[x['identity'] for x in r('query-member-[10, 100]')['calls']]==[10,100] and r('query-member-[10, 100]')['current_candidate']==100)
    check('query-self-member-late-reset',r('query-member-[10, 100]')['returned']==1 and r('query-member-[10, 100]')['pair']['count']==0)
    check('source-compound-is-distinct',r('group-member-compound-shape1')['returned']==1 and by['group-member-compound-shape1']['native_scope']=='unsupported')
    check('normal-mode2-reach-sensitive',r('normal-mode2')['requires_mode2'] and not r('normal-mode2-inactive')['requires_mode2'])
    check('early-guards-avoid-invalid-member-backing',r('early-null-shape-group-rejected')['boundary'] is None and r('early-null-geometry-group-disabled')['boundary'] is None)
    retained=r('group-updated-far-sphere-direct-mask-retained')
    check('ordinary-gate-uses-updated-member-sphere',[x['identity'] for x in retained['calls']]==[10,11] and [x['returned'] for x in retained['calls']]==[0,1])
    check('ordinary-updated-sphere-miss-preserves-intervening-hit-mask',retained['returned']==1 and retained['current_candidate']==10 and retained['pair']['hit_flags'][0]==1 and retained['pair']==retained['calls'][-1]['after'])
    for c in cs:
        x=c['expected']
        check(c['name']+'-source-read-zero',x['byte_audit']['source_reads']==0)
        check(c['name']+'-groups-unmodified',x['groups']==x['initial']['groups'])
        for event in x['append_events']:assert event['count_after']==event['count_before']+int(event['count_before']<event['capacity'])
    return checks


def main():
    before=preserved_files();OUT.mkdir(parents=True,exist_ok=True);adapter=audit_adapter();words,dispatch=load_words();freeze_map(words,dispatch)
    mm=meshes();cs=cases();coverage=set();branches={};steps=0
    for c in cs:
        try:result,m=execute_case(words,dispatch,mm,c)
        except Exception:print('FAILED',c['name'],file=sys.stderr);raise
        c['expected']=result;coverage.update(m.coverage);steps+=m.steps
        for pc,outcomes in m.branches.items():branches.setdefault(pc,set()).update(outcomes)
    baseline=json.loads((OUT/'before-final-extension/native-fixtures.json').read_text())
    byname={c['name']:c for c in cs}
    for c in baseline['cases']:assert byname[c['name']]==c,c['name']
    assert len(baseline['cases'])==122
    dump('final-extension.json',dict(preserved_cases=122,changed_cases=0,added_cases=len(cs)-122,baseline_sha256=sha((OUT/'before-final-extension/native-fixtures.json').read_bytes())))
    prefix=json.loads((OUT/'before-direct-sphere-extension/native-fixtures.json').read_text())
    for c in prefix['cases']:assert byname[c['name']]==c,c['name']
    assert len(prefix['cases'])==127
    dump('direct-sphere-extension.json',dict(preserved_cases=127,changed_cases=0,added_cases=len(cs)-127,baseline_sha256=sha((OUT/'before-direct-sphere-extension/native-fixtures.json').read_bytes())))
    checks=assertions(cs);dump('semantic-checks.json',checks)
    fixture=dict(schema=1,state_offsets=[f'{x:X}' for x in STATE_OFFSETS],array_slots=SLOTS,meshes=mm,cases=cs)
    raw=(json.dumps(fixture,separators=(',',':'))+'\n').encode();(OUT/'native-fixtures.json').write_bytes(raw)
    dump('executed-original-words.json',dict(elf_sha256=ELF_SHA,dispatch=dispatch,executed_words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)]))
    after=preserved_files();assert before==after
    preserved_counts={name:len(json.loads((ROOT/path).read_text())['cases']) for name,path in [('48S','evidence/candidateCollection48S/native-fixtures.json'),('48T-helper','evidence/groupShape48T/helper-fixtures.json'),('48T-collector','evidence/groupShape48T/collector-fixtures.json')]}
    assert preserved_counts=={'48S':239,'48T-helper':187,'48T-collector':110}
    dump('preserved-files.json',dict(counts=preserved_counts,files=before,changed_files=[]))
    report=dict(elf_sha256=ELF_SHA,cases=len(cs),entry=0x122660,steps=steps,source_reads=0,natural_returns=sum(c['expected']['boundary'] is None for c in cs),
        native_complete=sum(c['native_scope']=='complete' for c in cs),native_unsupported=sum(c['native_scope']=='unsupported' for c in cs),
        boundaries=[dict(name=c['name'],boundary=c['expected']['boundary'],pc=c['expected']['stop_pc']) for c in cs if c['expected']['boundary']],
        original_only_complete=[dict(name=c['name'],reason=c['native_reason']) for c in cs if c['native_scope']=='unsupported' and c['expected']['boundary'] is None],
        preserved_counts=preserved_counts,adapter=adapter,fixture_sha256=sha(raw),fixture_bytes=len(raw),generator_sha256=sha(Path(__file__).read_bytes()),
        no_patched_words=True,no_helper_stubs=True,no_ram_reads_or_retrieval=True,no_native_expected_values=True,no_uninitialized_read_fallback=True,
        semantic_checks=len(checks),callbacks=sum(len(c['expected']['callback_calls']) for c in cs),dispatcher_calls=sum(len(c['expected']['calls']) for c in cs),
        coverage=[f'{x:08X}' for x in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
    dump('proof-report.json',report);print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')},indent=2))
if __name__=='__main__':main()
