"""Initialized original122350 candidate collection proof; genuine instructions only.

The spatial inputs are explicit synthetic QuadTree cache/list objects. They are
not a recovered live world. No RAM, native expected values, instruction patches,
helper/virtual stubs, forced returns, or allocation interception are used.
"""
import copy
import hashlib
import json
import math
import random
import struct
import sys
from pathlib import Path
sys.dont_write_bytecode = True
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits, fp, signed
ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/candidateCollection48S'
MAP = OUT / 'map'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
MAP_SHA = '05ce6db129c12888b32c7892526a957c745e6108e5bf1d602d913c711f735863'
DATA_SHA = '8ee2d9c392e9e54994ff8d6f0ca0060c361b85135bcfeb1ffbfc81fa27556d30'
Q, SHAPE, COUNT, OUTPUT = 0x10000, 0x11000, 0x12000, 0x13000
PAIRS, PAIR_SCENES, SCENES = 0x14000, 0x15000, 0x16000
GP, STACK, RETURN = 0x3dd970, 0x1ff0000, 0xfffffff0
FRAME = STACK - 0x100
VTABLE = 0x3d2340
IDENTITY = [1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.]


def sha(raw): return hashlib.sha256(raw).hexdigest()
def floats(v): return [bits(x) for x in v]
def put(m,a,v):
    for i,x in enumerate(v): m.write(a+4*i,x)
def get(m,a,n): return [m.read(a+4*i) for i in range(n)]
def obj(i): return Q if i==1000 else 0x20000 + (i-1)*0x1000

def load_words():
    raw=(MAP/'original-words.json').read_bytes()
    if MAP_SHA: assert sha(raw)==MAP_SHA, 'Frozen48S map changed'
    p=json.loads(raw); assert p['elf']['sha256']==ELF_SHA
    words={}
    for b in p['blocks']:
        rows=b['instructions']; assert [r['pc'] for r in rows]==list(range(b['start'],b['end_exclusive'],4))
        binary=b''.join(struct.pack('<I',r['word']) for r in rows)
        assert sha(binary)==b['sha256']
        assert binary==(MAP/(b['name']+'.bin')).read_bytes()
        for r in rows:
            assert r['pc'] not in words or words[r['pc']]==r['word']
            words[r['pc']]=r['word']
    data_raw=(MAP/'dispatch-data.json').read_bytes();assert sha(data_raw)==DATA_SHA
    data=json.loads(data_raw); assert data['elf_sha256']==ELF_SHA
    dispatch={}
    for a in data['arrays']:
        binary=struct.pack('<'+'I'*len(a['words']),*a['words'])
        assert sha(binary)==a['sha256'] and binary==(MAP/(a['name']+'.bin')).read_bytes()
        dispatch.update((a['va']+4*i,x) for i,x in enumerate(a['words']))
    if '--frozen' not in sys.argv:
        elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes(); assert sha(elf)==ELF_SHA
        for pc,w in list(words.items())+list(dispatch.items()):
            assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==w,hex(pc)
    assert dispatch[VTABLE+0x30]==0x265040 and dispatch[VTABLE+0x38]==0x2655c0
    return words,dispatch

class CollectorOriginal(SpringOriginal):
    def __init__(self,words):
        super().__init__(data=b'',words=words)
        self.allowed_source_addresses=set(); self.executing=False
        self.read_bytes=set(); self.written_bytes=set(); self.events=[]; self.calls=[]; self.extension_coverage=set();self.lo64=0;self.hi64=0
    def read(self,a,size=4):
        if self.executing:self.read_bytes.update(range(a,a+size))
        return super().read(a,size)
    def write(self,a,v,size=4):
        if self.executing:self.written_bytes.update(range(a,a+size))
        return super().write(a,v,size)
    def identity(self,a):return self.identities.get(a,a)
    def observe(self,pc):
        if pc==0x1223ec:
            self.events.append(dict(event='bounds-return',center=get(self,FRAME+0x90,4),half=get(self,FRAME+0xa0,4)))
        elif pc==0x122420:
            self.events.append(dict(event='outer-entry',identity=self.identity(self.g[16][0]),count=self.read(COUNT)))
        elif pc in (0x126eb0,0x120e70,0x126e40,0x135f30,0x135cf0,0x135cd0):
            self.events.append(dict(event='helper',pc=pc,first=self.identity(self.g[4][0]),second=self.identity(self.g[5][0])))
        elif pc==0x12260c:
            self.events.append(dict(event='iterator-return',identity=self.identity(self.g[2][0]),count=self.read(COUNT)))
        elif pc in (0x122498,0x1225d4):
            self.events.append(dict(event='append',pc=pc,identity=self.identity(self.g[16][0]),current=self.identity(self.read(Q+0xf4)),index=self.read(COUNT)))
        elif pc in (0x265040,0x265c40,0x2655c0):
            self.events.append(dict(event='spatial-call',pc=pc,tree=self.g[4][0]))
    def execute(self,pc):
        self.executing=False; self.observe(pc); w=self.words[pc]
        if w>>26==3:self.calls.append(dict(pc=pc,target=(w&0x3ffffff)*4))
        elif w>>26==0 and w&63==9:self.calls.append(dict(pc=pc,target=self.g[(w>>21)&31][0]))
        self.executing=True
        # Exact ISA extensions only, never replacing a helper or instruction.
        if w>>26==0x3f:
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False;self.extension_coverage.add(pc)
            rs,rt=(w>>21)&31,(w>>16)&31;imm=w&65535;imm-=65536 if imm&32768 else 0
            self.write((self.g[rs][0]+imm)&0xffffffff,self.g[rt][0]|(self.g[rt][1]<<32),8);target=None
        elif w>>26==0 and w&63==0x18:
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False;self.extension_coverage.add(pc)
            rs,rt,rd=(w>>21)&31,(w>>16)&31,(w>>11)&31;product=signed(self.g[rs][0])*signed(self.g[rt][0])
            self.lo64=signed(product&0xffffffff)&0xffffffffffffffff;self.hi64=signed((product>>32)&0xffffffff)&0xffffffffffffffff
            if rd:self.g[rd][0]=self.lo64&0xffffffff;self.g[rd][1]=self.lo64>>32
            target=None
        elif w>>26==0 and w&63==0xb:
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False;self.extension_coverage.add(pc)
            rs,rt,rd=(w>>21)&31,(w>>16)&31,(w>>11)&31
            if rd and (self.g[rt][0] or self.g[rt][1]):self.g[rd][:2]=self.g[rs][:2]
            target=None
        elif w>>26==0 and w&63==0x2b:
            self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False;self.extension_coverage.add(pc)
            rs,rt,rd=(w>>21)&31,(w>>16)&31,(w>>11)&31
            a=self.g[rs][0]|(self.g[rs][1]<<32);b=self.g[rt][0]|(self.g[rt][1]<<32)
            if rd:self.g[rd][0]=int(a<b);self.g[rd][1]=0
            target=None
        elif w>>26==1 and (w>>16)&31 in (0,1):
            self.pc=pc; self.steps+=1; self.coverage.add(pc); self.annul=False
            self.extension_coverage.add(pc)
            negative=bool(self.g[(w>>21)&31][1]&0x80000000)
            take=negative==(((w>>16)&31)==0)
            self.branches.setdefault(pc,set()).add(take); imm=w&65535; imm-=65536 if imm&32768 else 0
            target=pc+4+imm*4 if take else pc+8
        elif w>>26==0x11 and (w>>21)&31==16 and w&63==0x24:
            self.pc=pc; self.steps+=1; self.coverage.add(pc); self.annul=False
            self.extension_coverage.add(pc)
            value=self.f[(w>>11)&31]
            assert math.isfinite(value) and -2147483648<=value<2147483648, 'Outside bounded CVT.W.S ordinary finite scope'
            # EE CVT.W.S conversion uses truncation toward zero.
            self.f[(w>>6)&31]=fp(math.trunc(value)&0xffffffff); target=None
        else:
            target=super().execute(pc)
            if w>>26 in (0x20,0x21,0x23,0x24,0x25):
                # Scalar loads write a sign/zero-extended64-bit EE value.
                # This matters for BLTZ/SLTU; upper128 remains untouched.
                rt=(w>>16)&31
                self.g[rt][1]=0xffffffff if w>>26 in (0x20,0x21,0x23) and self.g[rt][0]&0x80000000 else 0
        self.executing=False; return target


def filter_default():
    return dict(category=4,accept_mask=0xffffffff,flags=4,flags31=1,
        identity_lists=[dict(mode=1,ids=[]),dict(mode=1,ids=[])],category_mode=1,category_masks=[])

def make_case(name):
    cs=[dict(id=i,tag=7,epoch=0,filter=filter_default(),center=floats([0.,0.,0.,1.]),radius=bits(1.)) for i in range(1,9)]
    return dict(name=name,entry='collector',capacity=64,modes=[2,1,1],query=dict(
        flags=0x26,shape_tag=1,center=floats([0.,0.,0.,1.]),radius=bits(2.),
        local_center=floats([0.,0.,0.,1.]),local_half=floats([1.,2.,3.,1.]),world=floats(IDENTITY),
        filter=filter_default(),current=8,pairs=[[floats([-.5,-1.,-.25,1.]),floats([.5,1.,.25,1.])]],scenes=[None,None]),
        candidates=cs,groups=[],trees=[dict(items=[1,2,3],epoch=10,flags=1,blocked=0,cache=[0,0,1023,1023])],
        zones=[[None,0]],initial_count=0x87654321,initial_output=[0xde000000+i for i in range(128)],
        source='explicit',collision_tag=7,group_tag=9,debug_init=[1,1],debug_flags=[0,0],group_scale=bits(1.),fallback_zone=None)


def initialize(words,dispatch,c):
    m=CollectorOriginal(words);m.g[28][0]=GP;m.identities={0:0,Q:1000}
    m.identities.update({obj(x['id']):x['id'] for x in c['candidates']+c['groups']})
    for a,w in dispatch.items():m.write(a,w)
    for a,v in zip((GP-0x7370,GP-0x736c),c['debug_init']):m.write(a,v,1)
    for a,v in zip((0x443528,0x443530),c['debug_flags']):m.write(a,v)
    m.write(0x442e18,0);m.write(0x36c100,1)
    m.write(GP-0x7ff0,c['collision_tag'],1);m.write(GP-0x7fec,c['group_tag'],1)
    def filt(a,f,slot):
        for off,key in ((0xa4,'category'),(0xb8,'accept_mask'),(0xd4,'category_mode')):m.write(a+off,f[key])
        m.write(a+0xf8,f['flags'],1);m.write(a+0x31,f['flags31'],1)
        for k,off in enumerate((0xbc,0xc8)):
            e=f['identity_lists'][k];m.write(a+off,e['mode']);m.write(a+off+4,slot+k*0x100);m.write(a+off+8,e.get('count',len(e['ids'])))
            put(m,slot+k*0x100,[obj(i) if i else 0 for i in e['ids']])
        m.write(a+0xd8,slot+0x200);m.write(a+0xdc,f.get('category_count',len(f['category_masks'])));put(m,slot+0x200,f['category_masks'])
    q=c['query'];filt(Q,q['filter'],0x700000)
    m.write(Q+0xf8,q['flags'],1);m.write(Q+2,c['collision_tag'],1);m.write(Q,0,2)
    for off,v in ((0x88,q['shape_tag']),(0x8c,SHAPE),(0xf4,obj(q['current']) if q['current'] else 0),
                  (0xe0,0x810000 if c['source']=='explicit' else 0),(0x80,q['radius'])):m.write(Q+off,v)
    put(m,Q+0x60,q['center']);m.write(COUNT,c['initial_count']);put(m,OUTPUT,c['initial_output'])
    if q['shape_tag'] in (1,6):
        put(m,SHAPE+(0x80 if q['shape_tag']==1 else 0),q['world'])
        put(m,SHAPE+(0 if q['shape_tag']==1 else 0x40),q['local_center'])
        put(m,SHAPE+(0x10 if q['shape_tag']==1 else 0x50),q['local_half'])
    elif q['shape_tag']==3:
        m.write(SHAPE,q.get('pair_count',len(q['pairs'])));m.write(SHAPE+4,PAIR_SCENES);m.write(SHAPE+8,PAIRS)
        for i,pair in enumerate(q['pairs']):put(m,PAIRS+i*32,pair[0]+pair[1])
        for i,matrix in enumerate(q['scenes']):
            a=SCENES+0x200*i;m.write(PAIR_SCENES+4*i,a if matrix else 0)
            if matrix:put(m,a+0x100,matrix)
    for x in c['candidates']:
        a=obj(x['id']);m.write(a,x['epoch'],2);m.write(a+2,x['tag'],1);filt(a,x['filter'],0x701000+0x400*x['id'])
        put(m,a+0x60,x['center']);m.write(a+0x80,x['radius'])
        if 'shape_tag' in x:
            m.write(a+0x88,x['shape_tag']);m.write(a+0x8c,0xa80000+0x100*x['id'] if x.get('shape_present',True) else 0)
            put(m,0xa80000+0x100*x['id'],x.get('shape_words',[0x41000000+j for j in range(64)]))
    trees=[];layouts=[];pool_addresses=[]
    for i,t in enumerate(c['trees']):
        a=0x800000+i*0x1000;trees.append(a)
        base=0x900000+i*0x10000;pool=0xb00000+i*0x1000;chunk=0xb10000+i*0x1000;pool_addresses.append(pool)
        nodes=t.get('source_nodes',[dict(items=t['items'],children=[])])
        selected=t.get('selected_nodes',[0] if t['items'] else [])
        node_addresses=[base+0x20+j*0x20 for j in range(len(nodes))]
        layout=dict(tree=a,pool=pool,chunk=chunk,nodes=[],selected_links=[],pool_capacity=t.get('pool_capacity',32))
        for off,v in ((8,VTABLE),(0x30,bits(-100.)),(0x38,bits(-100.)),(0x48,bits(65536.)),(0x4c,bits(65536.)),
                      (0x58,1024),(0x5c,bits(1.)),(0x898,t['blocked']),(0xa4,chunk+8 if selected else 0),
                      (0xa8,0),(0xac,0),(0xb0,0),(0xb4,0),(0x94,node_addresses[0] if nodes else 0),(0x9c,pool)):
            m.write(a+off,v)
        m.write(a+0x14,t['epoch'],2);m.write(a+0x16,t['flags'],1);put(m,a+0x8a8,t['cache'])
        if t.get('carry_block') is not None:m.write(a+0xb0,base+0x100+0x20*t['carry_block'])
        if t.get('cursor_index') is not None:m.write(a+0xa8,chunk+8+8*t['cursor_index'])
        m.write(a+0xac,t.get('initial_slot',0))
        for off,key in ((0x30,'origin_x'),(0x38,'origin_z'),(0x48,'size_x'),(0x4c,'size_z'),(0x5c,'scale'),(0x58,'quantum')):
            if key in t:m.write(a+off,t[key])
        for j,n in enumerate(nodes):
            blocks=base+0x100+j*0x1000;children=base+0xd000+j*0x20;node=node_addresses[j]
            chunks=[n['items'][k:k+4] for k in range(0,len(n['items']),4)]
            m.write(node,children if n.get('children') else 0);m.write(node+4,blocks if chunks else 0)
            if n.get('children'):put(m,children,[node_addresses[v] if v is not None else 0 for v in n['children']])
            ls=[]
            for k,part in enumerate(chunks):
                values=part+[0]*(4-len(part));block=blocks+k*0x20;next_block=block+0x20 if k+1<len(chunks) else 0
                put(m,block,[next_block]+[obj(v) if v else 0 for v in values]);ls.append(dict(address=block,next=next_block,objects=values))
            layout['nodes'].append(dict(address=node,children=n.get('children',[]),blocks=ls))
        for j,n in enumerate(selected):
            address=chunk+8+j*8;next_link=address+8 if j+1<len(selected) else 0
            put(m,address,[next_link,node_addresses[n]]);layout['selected_links'].append(dict(address=address,next=next_link,node=n))
        # Original pool metadata. Active/reserve chunks and free records
        # are real linked inputs to105720/105990; no pool return is injected.
        capacity=t.get('pool_capacity',32);used=t.get('pool_used',len(selected))
        active=t.get('active_chunks',1);reserve=t.get('reserve_chunks',0)
        chunks=[chunk+0x200*j for j in range(active+reserve)]
        assert chunks, 'At least one explicit backed pool chunk is required'
        if len(chunks)>1:layout['extra_chunks']=[dict(address=v,entry_count=capacity) for v in chunks[1:]]
        for j,ch in enumerate(chunks):
            next_chunk=chunks[j+1] if (j+1<active or active<=j and j+1<len(chunks)) else 0
            m.write(ch,next_chunk)
            for k in range(len(selected) if j==0 else 0,capacity):
                put(m,ch+8+k*8,[chunk+8 if selected else 0,node_addresses[0] if nodes else 0])
        free=t.get('free_entries',[])
        entries=[ch+8+j*8 for ch in chunks for j in range(capacity)]
        for j,k in enumerate(free):m.write(entries[k],entries[free[j+1]] if j+1<len(free) else 0)
        for off,v in ((0x10,capacity),(0x14,chunks[0] if active else 0),(0x18,chunks[active-1] if active else 0),
                      (0x1c,chunks[active] if reserve else 0),(0x20,used),(0x24,8),(0x28,8+8*capacity),
                      (0x2c,entries[free[0]] if free else 0),(0x30,(8+8*capacity)*len(chunks)),(0x34,8*used)):
            m.write(pool+off,v)
        layouts.append(layout)
    m.layouts=layouts;m.pool_addresses=pool_addresses
    for i,z in enumerate(c['zones']):
        link=0x810000+i*0x20;zone=0x820000+c.get('zone_aliases',list(range(len(c['zones']))))[i]*0x100
        put(m,link,[link+0x20 if i+1<len(c['zones']) else 0,zone]);m.write(zone+0x34,trees[z[0]] if z[0] is not None else 0);m.write(zone+0x38,trees[z[1]] if z[1] is not None else 0)
    m.write(0x443148,0x810000);m.write(0x443108,0x820000+0x100*c.get('zone_aliases',list(range(len(c['zones']))))[c['fallback_zone']] if c['fallback_zone'] is not None else 0)
    for a in (GP-0x7368,GP-0x7364):m.write(a,1,1)
    m.write(0x443538,int(c.get('later_method_diagnostics',False)));m.write(0x443540,int(c.get('later_method_diagnostics',False)))
    m.tree_addresses=trees
    if c['groups']:
        world=0xa00000;m.write(0x3e1ce8,world);m.write(world+0x590,c['group_scale'])
        for n,g in enumerate(c['groups']):
            a=obj(g['id']);desc=0xa10000+n*0x1000;members=0xa20000+n*0x1000
            m.write(a,g['epoch'],2);m.write(a+2,g.get('tag',9),1)
            for k,v in enumerate(g['position']):m.write(a+8+2*k,v,2)
            m.write(a+0xe,n,1);m.write(a+0x10,g['width_byte'],1);m.write(a+0x11,g['height_byte'],1)
            m.write(world+0x6c+4*n,desc);m.write(desc+0x1fc,g.get('count',len(g['members'])));m.write(desc+0x204,members)
            put(m,members,[obj(i) for i in g['members']])
            for off,v in zip((0x180,0x184,0x18c,0x194),g['dimensions']):m.write(desc+off,v)
    return m


def state(m,c):
    return dict(count=m.read(COUNT),output=[m.identity(x) for x in get(m,OUTPUT,len(c['initial_output']))],
        query=dict(epoch=m.read(Q,2),flags=m.read(Q+0xf8,1),current=m.identity(m.read(Q+0xf4)),center=get(m,Q+0x60,4),radius=m.read(Q+0x80)),
        candidates=[dict(id=x['id'],epoch=m.read(obj(x['id']),2),flags=m.read(obj(x['id'])+0xf8,1),center=get(m,obj(x['id'])+0x60,4),radius=m.read(obj(x['id'])+0x80),
            shape_words=get(m,0xa80000+0x100*x['id'],64) if 'shape_tag' in x else None) for x in c['candidates']],
        groups=[dict(id=g['id'],epoch=m.read(obj(g['id']),2)) for g in c['groups']],
        trees=[dict(epoch=m.read(a+0x14,2),flags=m.read(a+0x16,1),blocked=m.read(a+0x898),cache=get(m,a+0x8a8,4),auxiliary=m.read(a+0xb4),head=m.read(a+0xa4),next=m.read(a+0xa8),slot=m.read(a+0xac),block=m.read(a+0xb0)) for a in m.tree_addresses],
        pools=[dict(capacity=m.read(a+0x10),active=m.read(a+0x14),current=m.read(a+0x18),reserve=m.read(a+0x1c),used=m.read(a+0x20),free=m.read(a+0x2c),bytes=m.read(a+0x34)) for a in m.pool_addresses],
        pool_blocks=[dict(address=ch,next=m.read(ch)) for x in m.layouts for ch in [x['chunk']]+[v['address'] for v in x.get('extra_chunks',[])]],
        pool_entries=[[dict(address=ch+8+j*8,next=m.read(ch+8+j*8),node=m.read(ch+12+j*8)) for ch in [x['chunk']]+[v['address'] for v in x.get('extra_chunks',[])] for j in range(x['pool_capacity'])] for x in m.layouts],
        last_tree=m.read(0x442e18),debug_init=[m.read(a,1) for a in (GP-0x7370,GP-0x736c)],debug_flags=[m.read(a) for a in (0x443528,0x443530)])


def execute_case(words,dispatch,c):
    m=initialize(words,dispatch,c);c['spatial_layout']=m.layouts;initialized=set(m.overlay);initial=state(m,c);c['initial_state']=initial
    m.g[4][0]=Q;m.g[5][0]=c['capacity'];m.g[6][0]=COUNT;m.g[7][0]=OUTPUT
    for i,v in enumerate(c['modes']):m.g[8+i][0]=v
    m.g[29][0]=STACK;m.g[31][0]=RETURN
    pc=0x122350;boundary=None
    while pc!=RETURN:
        if pc in (0x3075a8,0x1336c0):boundary='diagnostic-output-helper';break
        if pc==0x105870:boundary='spatial-pool-growth';break
        if pc==0x135f08:boundary='group-shape0-transform-helper';break
        if pc in (0x120378,0x12044c,0x1208e4,0x1209f8) or (pc==0x120c4c and c['query']['shape_tag'] not in (1,3,6)):
            boundary='unsupported-query-bounds';break
        if pc==0x1207ac and signed(m.read(SHAPE))<=0:boundary='nonpositive-shape3-pair-count';break
        if pc==0x1225b8 and m.g[5][0]==0:boundary='null-group-member-result';break
        if pc==0x29471c and m.g[4][0]==0:boundary='null-spatial-method';break
        if pc not in words:boundary='unmapped-original-dependency';break
        target=m.execute(pc)
        if target is not None:
            if not m.annul:assert m.execute(pc+4) is None
            pc=target
        else:pc+=4
    assert m.data==b'' and m.allowed_source_addresses==set() and not m.source_reads
    assert m.read_bytes <= initialized | m.written_bytes
    assert not ({0x305700,0x2897b0,0x289940}&m.coverage), 'Reached allocator/registration outside this scope'
    result=state(m,c)
    result.update(boundary=boundary,stop_pc=pc,residual_v0=m.g[2][0] if pc==RETURN else None,
        events=m.events,calls=m.calls,steps=m.steps,extension_coverage=[f'{a:08X}' for a in sorted(m.extension_coverage)],byte_audit=dict(initialized_bytes=len(initialized),
            execution_read_bytes=len(m.read_bytes),execution_written_bytes=len(m.written_bytes),source_reads=0))
    if pc==RETURN:assert m.g[29][0]==STACK
    # Only the explicit output prefix is overwritten. Group expansion may
    # exceed the nominal capacity; actual storage is independently sufficient.
    n=result['count']; assert n<=len(c['initial_output'])
    assert result['output'][n:]==initial['output'][n:]
    return result,m


def make_cases():
    cases=[]
    def add(name,edit=None):
        c=make_case(name)
        if edit:edit(c)
        cases.append(c);return c
    def group(c,members=(1,2,3),tag=5):
        c['modes']=[1,0,0];c['zones']=[[0,None]]
        c['trees'][0]['items']=[101]
        c['groups']=[dict(id=101,epoch=0,position=[2,3,4],width_byte=32,height_byte=64,
            dimensions=floats([2.,1.,3.,5.]),members=list(members))]
        for x in c['candidates']:
            if x['id'] in members:x.update(shape_tag=tag)
    add('basic-mode2')
    add('basic-mode1',lambda c:c.update(modes=[1,0,0],zones=[[0,None]]))
    for f in (0,1,4,0x20,0x80,0xfd):add('disabled-flags-'+str(f),lambda c,f=f:c['query'].update(flags=f))
    for items in ([],[0],[1,0,2,1,3,0,2],[3,2,1],[1,2,3,4,5,6,7,8]):
        add('items-'+','.join(map(str,items)),lambda c,o=items:c['trees'][0].update(items=o))
    for cap in (0,1,2,3,4,64,0xffffffff,0x80000000):add('capacity-'+str(cap),lambda c,k=cap:c.update(capacity=k))
    for n in (63,64,65,72):
        def many(c,n=n):
            c['candidates']=[dict(id=i,tag=7,epoch=0,filter=filter_default(),center=floats([0.,0.,0.,1.]),radius=bits(1.)) for i in range(1,n+1)]
            c['trees'][0]['items']=list(range(1,n+1))
        add('nominal64-input'+str(n),many)
    for mode in (0,1,2,3,4,0xffffffff):
        add('mode-bits-'+str(mode),lambda c,k=mode:c.update(modes=[k,1,1],zones=[[0,0]]))
    for skip in (0,1,2,0xffffffff):
        for dist in (0.,3.,3.00001,99.):
            def sphere(c,skip=skip,dist=dist):
                c['modes'][1]=skip;c['trees'][0]['items']=[1];c['candidates'][0]['center'][0]=bits(dist)
            add(f'sphere-skip{skip}-distance{dist}',sphere)
    for tag in (0,7,9,255):add('direct-type-'+str(tag),lambda c,t=tag:c['candidates'][0].update(tag=t))
    add('type-identifiers-overridden',lambda c:(c.update(collision_tag=42,group_tag=43),[x.update(tag=42) for x in c['candidates']]))
    add('source-global-default',lambda c:c.update(source='global'))
    for blocked in (0,1,0xffffffff):add('tree-blocked-'+str(blocked),lambda c,b=blocked:c['trees'][0].update(blocked=b))
    for epoch in (0,1,65534,65535):
        add('tree-epoch-'+str(epoch),lambda c,e=epoch:c['trees'][0].update(epoch=e))
    for epoch in (0,10,11,12,65535):add('candidate-epoch-'+str(epoch),lambda c,e=epoch:[x.update(epoch=e) for x in c['candidates']])
    for same_epoch in (True,False):
        for middle_empty in (True,False):
            def zones(c,same=same_epoch,empty=middle_empty):
                t=c['trees'][0];t['items']=[1,2]
                u=copy.deepcopy(t);u.update(items=[] if empty else [2,3],epoch=10 if same else 20)
                v=copy.deepcopy(t);v.update(items=[4],epoch=30)
                c['trees']=[t,u,v];c['zones']=[[None,0],[None,1],[None,2]]
            add(f'multiple-zones-same{int(same_epoch)}-empty{int(middle_empty)}',zones)
    add('both-methods-order',lambda c:(c['trees'].append(dict(items=[4,5],epoch=20,flags=1,blocked=0,cache=[0,0,1023,1023])),c.update(modes=[3,1,1],zones=[[0,1]])))
    add('fallback-zone-already-current',lambda c:c.update(fallback_zone=0))
    for carry in (0,1):
        add('cached-carry-block-'+str(carry),lambda c,k=carry:c['trees'][0].update(items=[1,2,3,4,5,6,7,8],carry_block=k))
    for f in (0,1,4,0x10,0x14,0xff):add('candidate-flags-'+str(f),lambda c,f=f:c['candidates'][0]['filter'].update(flags=f))
    for f in (0,1,2,3,0xff):add('candidate-flags31-'+str(f),lambda c,f=f:c['candidates'][0]['filter'].update(flags31=f))
    for mask,category in ((0,4),(1,4),(4,4),(0x80000000,0x80000000)):
        add(f'accept-mask{mask}-category{category}',lambda c,m=mask,k=category:(c['query']['filter'].update(accept_mask=m),c['candidates'][0]['filter'].update(category=k)))
    add('self-query-candidate',lambda c:c['trees'][0].update(items=[1000,1]))
    for side in ('query','candidate'):
        for lane in (0,1):
            for mode in (0,1,2):
                for present in (False,True):
                    def flist(c,side=side,lane=lane,mode=mode,present=present):
                        f=c['query']['filter'] if side=='query' else c['candidates'][0]['filter']
                        f['identity_lists'][lane]=dict(mode=mode,ids=[2,1 if side=='query' else 1000] if present else [2,3])
                    add(f'identity-{side}-lane{lane}-mode{mode}-present{int(present)}',flist)
        for mode in (0,1,2):
            for masks in ([],[1],[4],[1,4]):
                def categories(c,side=side,mode=mode,masks=masks):
                    f=c['query']['filter'] if side=='query' else c['candidates'][0]['filter'];f.update(category_mode=mode,category_masks=masks)
                add(f'category-{side}-mode{mode}-masks'+','.join(map(str,masks)),categories)
    for tag in (1,6,3):
        add('query-shape-'+str(tag),lambda c,t=tag:c['query'].update(shape_tag=t))
    for n in (0,0xffffffff):add('boundary-shape3-count-'+str(n),lambda c,n=n:c['query'].update(shape_tag=3,pair_count=n))
    for tag in (0,2,4,5,7,0xffffffff):add('boundary-query-tag-'+str(tag),lambda c,t=tag:c['query'].update(shape_tag=t))
    transforms=[('identity',IDENTITY),('translation',IDENTITY[:12]+[3.,-5.,7.,1.]),
        ('rotation',[0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,3.,-5.,7.,1.]),
        ('shear',[1.,0.,.5,0.,.25,2.,0.,0.,0.,.5,1.,0.,3.,-5.,7.,1.])]
    for name,matrix in transforms:
        for side in (0,1,2):
            def bounds(c,matrix=matrix,side=side):
                q=c['query'];q['shape_tag']=3;q['scenes']=[floats(matrix) if side!=1 else None,floats(matrix) if side!=0 else None]
                q['pairs']=[[floats([-2.,3.,-4.,-0.]),floats([5.,-6.,7.,9.])]]
            add(f'shape3-{name}-scene{side}',bounds)
    add('shape3-two-pairs',lambda c:c['query'].update(shape_tag=3,pairs=[[floats([-2.,3.,-4.,1.]),floats([5.,-6.,7.,1.])],[floats([-1.,4.,-3.,1.]),floats([6.,-5.,8.,1.])]],scenes=[None]*4))
    add('shape3-signed-zero',lambda c:c['query'].update(shape_tag=3,pairs=[[floats([-0.,0.,-0.,-0.]),floats([0.,-0.,0.,0.])]]))
    for members in ([],[1],[1,2,3],[1,1,2]):add('group-members-'+','.join(map(str,members)),lambda c,mm=members:group(c,mm))
    for tag in (1,4,5,6):add('group-member-tag-'+str(tag),lambda c,t=tag:group(c,tag=t))
    for cap in (0,1,2,3):add('group-capacity-'+str(cap),lambda c,k=cap:(group(c),c.update(capacity=k)))
    for skip in (0,1):
        for ignore in (0,1):
            add(f'group-skip{skip}-ignore{ignore}',lambda c,s=skip,i=ignore:(group(c),c['modes'].__setitem__(1,s),c['modes'].__setitem__(2,i)))
    add('group-ignores-member-filters',lambda c:(group(c),[x['filter'].update(flags=0,flags31=0,category=0) for x in c['candidates']]))
    add('group-followed-by-direct-capacity1',lambda c:(group(c),c.update(capacity=1),c['trees'][0].update(items=[101,4])))
    for d in (0,1,2):
        add('boundary-debug-'+str(d),lambda c,d=d:c.update(debug_init=[0 if d!=1 else 1,0 if d!=0 else 1],debug_flags=[1,1]))
    add('boundary-cache-mismatch',lambda c:c['trees'][0].update(cache=[1,0,1023,1023]))
    add('uncached-root',lambda c:c['trees'][0].update(flags=0,selected_nodes=[],pool_used=0))
    add('boundary-pool-exhaustion',lambda c:c['trees'][0].update(flags=0,selected_nodes=[],pool_used=0,pool_capacity=0))
    for quantum in (1024,32768):
        def hierarchy(c,q=quantum):
            c['trees'][0].update(flags=0,selected_nodes=[],pool_used=0,quantum=q,source_nodes=[
                dict(items=[1],children=[1,2,3,4]),dict(items=[2],children=[]),dict(items=[3],children=[]),
                dict(items=[4],children=[]),dict(items=[5],children=[])])
        add('uncached-four-children-quantum'+str(quantum),hierarchy)
    def whole_tree(c):
        c['query']['local_center']=floats([16384.,0.,16384.,1.]);c['query']['local_half']=floats([16384.,1.,16384.,1.])
        c['trees'][0].update(flags=0,selected_nodes=[],pool_used=0,origin_x=bits(0.),origin_z=bits(0.),
            source_nodes=[dict(items=[1],children=[1,2,3,4]),dict(items=[2],children=[]),dict(items=[3],children=[]),
                          dict(items=[4],children=[]),dict(items=[5],children=[])])
    add('uncached-whole-subtree-order',whole_tree)
    for stale in (False,True):
        add('disjoint-open-stale-cursor'+str(int(stale)),lambda c,k=stale:c['trees'][0].update(origin_x=bits(1000.),cursor_index=0 if k else None))
    for byte in (0,1,127,128,255):
        add('group-signed-scale-byte'+str(byte),lambda c,b=byte:(group(c),c['groups'][0].update(width_byte=b,height_byte=b)))
    for distance in (6,7,65535):
        add('group-xz-distance'+str(distance),lambda c,d=distance:(group(c),c['groups'][0].update(position=[d,65535,0])))
    add('group-members-count-negative',lambda c:(group(c),c['groups'][0].update(count=0xffffffff)))
    add('group-null-member-shape',lambda c:(group(c),c['candidates'][0].update(shape_present=False)))
    add('group-shape0-boundary',lambda c:group(c,tag=0))
    add('group-before-direct',lambda c:(group(c),c['trees'][0].update(items=[101,4])))
    add('direct-before-group',lambda c:(group(c),c['trees'][0].update(items=[4,101])))
    add('group-repeat-across-trees',lambda c:(group(c),c['trees'].append(dict(items=[101],epoch=20,flags=1,blocked=0,cache=[0,0,1023,1023])),c.update(zones=[[0,None],[1,None]])))
    for side in ('query','candidate'):
        for lane in (0,1):
            add(f'identity-negative-count-{side}-{lane}',lambda c,s=side,l=lane:(c['query']['filter'] if s=='query' else c['candidates'][0]['filter'])['identity_lists'][l].update(count=0xffffffff,ids=[]))
    # Bounds that produce fractional/negative truncated coordinates, while
    # leaving open inside the tree's spatial extent. Cache misses execute the
    # authentic close, pool reset, selection and acquire chain.
    for center in (-.25,.25,1.75):
        add('fractional-open-'+str(center),lambda c,x=center:(c['query'].update(local_center=floats([x,0.,x,1.])),c['trees'][0].update(origin_x=bits(0.),origin_z=bits(0.),quantum=1,cache=[123]*4)))
    add('group-member-is-query',lambda c:group(c,members=(1000,1)))
    add('group-member-also-direct',lambda c:(group(c),c['trees'][0].update(items=[1,101,2])))
    add('same-direct-and-group-type',lambda c:(c.update(collision_tag=9,group_tag=9),[x.update(tag=9) for x in c['candidates']]))
    add('pool-free-pop',lambda c:c['trees'][0].update(flags=0,selected_nodes=[],pool_capacity=2,pool_used=2,free_entries=[0,1]))
    add('pool-spare-append-active',lambda c:c['trees'][0].update(flags=0,selected_nodes=[],pool_capacity=1,pool_used=1,reserve_chunks=1))
    add('pool-spare-first-active',lambda c:c['trees'][0].update(flags=0,selected_nodes=[],pool_capacity=1,pool_used=1,active_chunks=0,reserve_chunks=1))
    add('pool-reset-append-spare',lambda c:c['trees'][0].update(cache=[123]*4,pool_capacity=1,pool_used=1,active_chunks=2,reserve_chunks=1))
    def pool_two(c):
        c['trees'][0].update(cache=[123]*4,pool_capacity=1,pool_used=1,active_chunks=1,reserve_chunks=1,
            source_nodes=[dict(items=[1],children=[1,None,None,None]),dict(items=[2],children=[])])
    add('pool-reset-then-two-chunks',pool_two)
    def free_two(c):
        c['trees'][0].update(flags=0,selected_nodes=[],pool_capacity=2,pool_used=2,free_entries=[0,1],
            source_nodes=[dict(items=[1],children=[1,None,None,None]),dict(items=[2],children=[])])
    add('pool-two-free-pops',free_two)
    def fallback(c):
        c['trees'].append(dict(items=[4,5],epoch=20,flags=1,blocked=0,cache=[0,0,1023,1023]))
        c.update(zones=[[None,0],[None,1]],fallback_zone=0)
    add('fallback-positive-earlier-zone',fallback)
    for side in ('query','candidate'):
        for mode in (0xffffffff,0,1):
            def category_negative(c,side=side,mode=mode):
                f=c['query']['filter'] if side=='query' else c['candidates'][0]['filter']
                f.update(category_mode=mode,category_count=0xffffffff,category_masks=[])
            add(f'category-negative-count-{side}-mode{mode}',category_negative)
    for slot in (5,0x7fffffff):
        add('disjoint-stale-overfull-slot'+str(slot),lambda c,n=slot:c['trees'][0].update(
            origin_x=bits(1000.),cursor_index=0,carry_block=0,initial_slot=n,items=[1,2,3,4,5,6,7,8]))
    add('repeated-links-same-zone-fallback',lambda c:c.update(zones=[[None,0],[None,0]],zone_aliases=[0,0],fallback_zone=0))
    def deep_tree(c,mixed=False):
        whole_tree(c)
        c['trees'][0]['source_nodes']=[dict(items=[1],children=[1,2,None,None]),
            dict(items=[2],children=[3,None,None,None]),dict(items=[3],children=[6,None,None,None]),
            dict(items=[4],children=[4,5,None,None]),dict(items=[5],children=[]),dict(items=[6],children=[]),
            dict(items=[7],children=[])]
        if mixed:
            c['query']['local_center']=floats([8192.,0.,8192.,1.]);c['query']['local_half']=floats([8192.,1.,8192.,1.])
    add('uncached-deep-full-subtree',deep_tree)
    add('uncached-deep-mixed-subtree',lambda c:deep_tree(c,True))
    for diagnostics in (False,True):
        for fallback_enabled in (False,True):
            def later_null(c,d=diagnostics,f=fallback_enabled):
                c['trees'][0]['items']=[1]
                c.update(zones=[[None,0],[None,None]],later_method_diagnostics=d,fallback_zone=0 if f else None)
            add(f'later-null-diagnostic{int(diagnostics)}-fallback{int(fallback_enabled)}',later_null)
    for fallback_enabled in (False,True):
        def null_middle(c,f=fallback_enabled):
            c['trees'][0]['items']=[1];c['trees'].append(dict(items=[4],epoch=20,flags=1,blocked=0,cache=[0,0,1023,1023]))
            c.update(zones=[[None,0],[None,None],[None,1]],later_method_diagnostics=False,fallback_zone=0 if f else None)
        add('null-middle-with-later-zone-fallback'+str(int(fallback_enabled)),null_middle)
    return cases


def isa_checks(words):
    """Isolated model checks, not collector/gameplay execution claims.

    Every opcode is the frozen authentic word at its original PC. Expected
    instruction behavior comes from the cited ISA implementation, never native
    candidate code. Original words are neither changed nor replaced.
    """
    rows=[]
    def machine():
        m=CollectorOriginal(words);m.identities={};return m
    for low,high in ((0,0),(0x80000000,0),(0xffffffff,0),(0,0x80000000),(1,0xffffffff),(0x7fffffff,0xffffffff),(0xffffffff,0x7fffffff)):
        m=machine();m.g[2][:2]=[low,high];target=m.execute(0x1224cc)
        assert not m.annul and m.execute(0x1224d0) is None
        expected=0x1224e4 if high&0x80000000 else 0x1224d4
        assert target==expected and m.g[3][0]==low>>1
        rows.append(dict(opcode='BLTZ64+genuine-delay-SRL',pc=0x1224cc,input=[low,high],target=target,delay_result=m.g[3][0]))
    for v in (0,0x7fff,0x8000,0xffff):
        m=machine();m.g[16][0]=0x10000;m.g[2]=[0,0xffffffff,0x13579bdf,0x2468ace0];m.write(0x10008,v,2)
        assert m.execute(0x1224c8) is None and m.g[2]==[v,0,0x13579bdf,0x2468ace0]
        rows.append(dict(opcode='LHU-zero-extend64',pc=0x1224c8,input=v,result=m.g[2]))
    for pc,base,off,reg,values,kind in ((0x1223f4,21,0xe0,5,(0,0x7fffffff,0x80000000,0xffffffff),'LW'),
        (0x135f30,4,0x11,3,(0,127,128,255),'LB'),(0x122380,4,0xf8,5,(0,127,128,255),'LBU')):
        for value in values:
            m=machine();m.g[base][0]=0x10000;m.g[reg][1]=0x11223344;m.g[reg][2:]=[55,66]
            m.write(0x10000+off,value,4 if kind=='LW' else 1);assert m.execute(pc) is None
            decoded=value if kind=='LW' or kind=='LBU' else value-256 if value&128 else value
            expected=decoded&0xffffffff;upper=0xffffffff if kind in ('LW','LB') and expected&0x80000000 else 0
            assert m.g[reg]==[expected,upper,55,66]
            rows.append(dict(opcode=kind+'-extend64',pc=pc,input=value,result=m.g[reg]))
    for x in (0.,-0.,.25,-.25,.999,-.999,1.5,-1.5,16777216.,2147483520.,-2147483648.):
        m=machine();m.f[5]=fp(bits(x));assert m.execute(0x26513c) is None
        expected=math.trunc(fp(bits(x)))&0xffffffff;assert bits(m.f[5])==expected
        rows.append(dict(opcode='CVT.W.S-in-range-truncate',pc=0x26513c,input=bits(x),result=bits(m.f[5])))
    for a,b in ((0,8),(1,8),(31,8),(0xffffffff,8),(0x80000000,0xffffffff),(0x7fffffff,0x7fffffff)):
        m=machine();m.g[3][:2]=[a,0xdeadbeef];m.g[5][0]=b;assert m.execute(0x10574c) is None
        product=signed(a)*signed(b);lo=signed(product&0xffffffff)&0xffffffffffffffff;hi=signed((product>>32)&0xffffffff)&0xffffffffffffffff
        assert m.lo64==lo and m.hi64==hi and m.g[3][:2]==[lo&0xffffffff,lo>>32]
        rows.append(dict(opcode='MULT-rd32-signextend64-and-HILO',pc=0x10574c,input=[a,b],rd=m.g[3][:2],lo64=lo,hi64=hi))
    for cond in ([0,0],[1,0],[0,1]):
        m=machine();m.g[2][:2]=cond;m.g[17]=[0x89abcdef,0x01234567,11,22];m.g[3]=[33,44,55,66]
        assert m.execute(0x2653bc) is None
        assert m.g[3]==([0x89abcdef,0x01234567,55,66] if any(cond) else [33,44,55,66])
        rows.append(dict(opcode='MOVN64-preserve-upper128',pc=0x2653bc,condition=cond,result=m.g[3]))
    for a,b in ((0,0),(0,1),(0,0x100000000),(0xffffffffffffffff,0),(0xffffffff,0x100000000)):
        m=machine();m.g[3][:2]=[b&0xffffffff,b>>32]
        # Authentic SLTU a0,zero,v1 has the first operand fixed to zero.
        if a:continue
        assert m.execute(0x2946e0) is None and m.g[4][:2]==[int(a<b),0]
        rows.append(dict(opcode='SLTU64',pc=0x2946e0,input=[a,b],result=m.g[4][:2]))
    m=machine();m.g[29][0]=0x10000;m.g[2]=[0x89abcdef,0x01234567,55,66]
    assert m.execute(0x2652d0) is None and m.read(0x10000,8)==0x0123456789abcdef
    rows.append(dict(opcode='SD64-little-endian',pc=0x2652d0,input=m.g[2],result=m.read(0x10000,8)))
    result=dict(format='candidate-collection-isa-audit48s-v1',checks=len(rows),rows=rows,
        sources=[dict(path='third_party/pcsx2-scalar47m/R5900OpcodeImpl.cpp',sha256=sha((ROOT/'third_party/pcsx2-scalar47m/R5900OpcodeImpl.cpp').read_bytes()),
            functions=['LHU','LW','LBU','LB','LH','SLTU','MULT','MOVN','SD']),
            dict(url='https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/Interpreter.cpp',accessed='2026-10-06',lines='333-370',finding='BLTZ and BGEZ read signed64 SD[0], bit63 controls the sign'),
            dict(url='https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp',accessed='2026-10-06',lines='237-240',finding='CVT_W truncates toward zero within range; this bounded extension rejects nonfinite and out-of-range conversions')],
        scope='These are isolated interpreter instruction-model checks. Gameplay proof starts only at122350 and returns naturally or stops at an explicit reached dependency. No instruction patch/stub is used.')
    (OUT/'isa-checks.json').write_text(json.dumps(result,indent=2)+'\n')
    return result


def semantic_checks(cases):
    by={c['name']:c for c in cases};checks=[]
    def check(name,predicate):
        assert predicate,name;checks.append(dict(name=name,passed=True))
    def result(n):return by[n]['expected']
    check('all-disabled-count-cleared',all(c['expected']['count']==0 and c['expected']['query']['flags']==(c['query']['flags']&~32) and not c['expected']['events'] for c in cases if c['name'].startswith('disabled-flags-')))
    check('capacity0-advances-and-stamps',result('capacity-0')['candidates'][0]['epoch']==11 and result('capacity-0')['count']==0 and result('capacity-0')['candidates'][0]['flags']==4)
    check('capacity1-advances-extra-object',result('capacity-1')['candidates'][1]['epoch']==11 and result('capacity-1')['candidates'][1]['flags']==4 and result('capacity-1')['query']['current']==1)
    check('capacity64-stamps65th-before-stop',result('nominal64-input65')['count']==64 and result('nominal64-input65')['candidates'][64]['epoch']==11 and result('nominal64-input65')['candidates'][64]['flags']==4)
    check('epoch-suppresses-repeated-object',result('items-1,0,2,1,3,0,2')['output'][:3]==[1,2,3])
    check('group-exceeds-nominal-capacity',all(result('group-capacity-'+str(n))['count']==3 and result('group-capacity-'+str(n))['output'][:3]==[101]*3 for n in (1,2,3)))
    check('group-members-update-flags-current',result('group-capacity-1')['query']['current']==3 and all(x['flags']&16 for x in result('group-capacity-1')['candidates'][:3]))
    check('group-bypasses-member-filters',result('group-ignores-member-filters')['output'][:3]==[101]*3 and not any(e['event']=='helper' and e['pc'] in (0x126eb0,0x120e70) for e in result('group-ignores-member-filters')['events']))
    check('empty-middle-source-terminates',all(result(n)['output'][:result(n)['count']]==[1,2] for n in ('multiple-zones-same1-empty1','multiple-zones-same0-empty1')))
    check('different-tree-epoch-allows-repeat',result('multiple-zones-same0-empty0')['output'][:5]==[1,2,2,3,4])
    check('same-tree-epoch-suppresses-shared-id',result('multiple-zones-same1-empty0')['output'][:4]==[1,2,3,4])
    check('cached-rewind-preserves-block-carryover',result('cached-carry-block-1')['output'][:4]==[5,6,7,8])
    check('disjoint-open-retains-old-cursor',result('disjoint-open-stale-cursor1')['output'][:3]==[1,2,3] and result('disjoint-open-stale-cursor1')['trees'][0]['epoch']==10)
    check('new-search-parent-first-reverse-children',result('uncached-whole-subtree-order')['output'][:5]==[1,5,4,3,2])
    check('pool-growth-exact-boundary',result('boundary-pool-exhaustion')['boundary']=='spatial-pool-growth' and result('boundary-pool-exhaustion')['stop_pc']==0x105870)
    check('source-no-stubs-or-allocator',all(c['expected']['byte_audit']['source_reads']==0 and not any(k['target']==0x305700 for k in c['expected']['calls']) for c in cases))
    check('shape3-recomputes-query-sphere',result('query-shape-3')['query']['center']!=by['query-shape-3']['query']['center'] or result('query-shape-3')['query']['radius']!=by['query-shape-3']['query']['radius'])
    check('shape1-and6-preserve-query-sphere',all(result('query-shape-'+str(t))['query']['center']==by['query-shape-'+str(t)]['query']['center'] and result('query-shape-'+str(t))['query']['radius']==by['query-shape-'+str(t)]['query']['radius'] for t in (1,6)))
    check('pool-free-pop-preserves-current-used',result('pool-free-pop')['pools'][0]['used']==2 and result('pool-free-pop')['pools'][0]['bytes']==24 and result('pool-free-pop')['pools'][0]['free']!=0)
    check('pool-two-free-pops-exhaust-free-list',result('pool-two-free-pops')['pools'][0]['used']==2 and result('pool-two-free-pops')['pools'][0]['free']==0 and result('pool-two-free-pops')['pools'][0]['bytes']==32)
    check('pool-reserve-appended-to-active',result('pool-spare-append-active')['pool_blocks'][0]['next']==result('pool-spare-append-active')['pools'][0]['current'])
    check('pool-reset-appends-existing-reserve',result('pool-reset-append-spare')['pool_blocks'][1]['next']==result('pool-reset-append-spare')['pool_blocks'][2]['address'])
    check('positive-fallback-reopens-earlier-zone',result('fallback-positive-earlier-zone')['output'][:8]==[1,2,3,4,5,1,2,3])
    check('overfull-stale-slot-skips-whole-block',all(result('disjoint-stale-overfull-slot'+str(v))['output'][:4]==[5,6,7,8] for v in (5,0x7fffffff)))
    check('repeated-links-share-zone-identity',result('repeated-links-same-zone-fallback')['output'][:6]==[1,2,3,1,2,3] and result('repeated-links-same-zone-fallback')['count']==6)
    check('deep-full-subtree-enters-recursive-helper',any(e['target']==0x2654c0 and e['pc']==0x265534 for e in result('uncached-deep-full-subtree')['calls']))
    check('deep-mixed-subtree-enters-both-selectors',any(e['target']==0x265330 and e['pc']==0x265458 for e in result('uncached-deep-mixed-subtree')['calls']) and any(e['target']==0x2654c0 and e['pc']==0x265534 for e in result('uncached-deep-mixed-subtree')['calls']))
    check('later-null-disabled-diagnostic-falls-through',result('later-null-diagnostic0-fallback0')['count']==1 and result('later-null-diagnostic0-fallback0')['boundary'] is None)
    check('later-null-disabled-diagnostic-reaches-fallback',result('later-null-diagnostic0-fallback1')['output'][:2]==[1,1] and result('later-null-diagnostic0-fallback1')['count']==2)
    check('later-null-enabled-diagnostic-stops-external-output',all(result('later-null-diagnostic1-fallback'+str(v))['boundary']=='diagnostic-output-helper' for v in (0,1)))
    check('null-middle-without-fallback-skips-later-zone',result('null-middle-with-later-zone-fallback0')['count']==1 and result('null-middle-with-later-zone-fallback0')['output'][0]==1)
    check('null-middle-fallback-keeps-source-link-cursor',result('null-middle-with-later-zone-fallback1')['output'][:4]==[1,1,4,1] and result('null-middle-with-later-zone-fallback1')['count']==4)
    (OUT/'semantic-checks.json').write_text(json.dumps(dict(format='candidate-collection-semantic-checks48s-v1',checks=checks),indent=2)+'\n')
    return checks


def main():
    OUT.mkdir(parents=True,exist_ok=True);words,dispatch=load_words();isa=isa_checks(words);cases=make_cases()
    coverage=set();branches={};steps=0
    for c in cases:
        try:r,m=execute_case(words,dispatch,c)
        except Exception:print('FAILED CASE:',c['name'],file=sys.stderr);raise
        c['expected']=r
        c['native_status']={None:'Complete','null-spatial-method':'MissingMethod','nonpositive-shape3-pair-count':'InvalidInput',
            'unsupported-query-bounds':'UnsupportedBounds','spatial-pool-growth':'AllocatorBoundary',
            'null-group-member-result':'UnsupportedMemberShape','group-shape0-transform-helper':'UnsupportedMemberShape','diagnostic-output-helper':'MissingMethod'}[r['boundary']]
        coverage.update(m.coverage);steps+=m.steps
        for pc,outcomes in m.branches.items():branches.setdefault(pc,set()).update(outcomes)
    archived=json.loads((OUT/'archive215/native-fixtures.json').read_text())
    assert cases[:215]==archived['cases'], 'Changed a preserved215-case object'
    semantic=semantic_checks(cases)
    raw=(json.dumps(dict(format='initialized-candidate-collection48s-v1',cases=cases),indent=2)+'\n').encode()
    (OUT/'native-fixtures.json').write_bytes(raw)
    report=dict(cases=len(cases),natural_returns=sum(c['expected']['boundary'] is None for c in cases),
        boundaries=[dict(name=c['name'],boundary=c['expected']['boundary'],pc=c['expected']['stop_pc']) for c in cases if c['expected']['boundary']],
        steps=steps,source_reads=0,isa_checks=isa['checks'],semantic_checks=len(semantic),fixture_sha256=sha(raw),original_words_sha256=sha((MAP/'original-words.json').read_bytes()),
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
    report['sources']=[dict(path=p,sha256=sha((ROOT/p).read_bytes())) for p in (
        'tools/gameplay/candidate_collection_fixtures48s.py','tools/gameplay/query_oracle48h.py','tools/gameplay/spring_fixture48j.py')]
    (OUT/'executed-original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,original_words_sha256=MAP_SHA,
        words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)]),indent=2)+'\n')
    (OUT/'proof-report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','boundaries')}))
if __name__=='__main__':main()
