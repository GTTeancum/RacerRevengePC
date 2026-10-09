"""48T original group-member/tag0 matrix-setter proof, sparse explicit data only.

This adds no ISA semantics. Genuine 135CF0, 11F680 and 122350 instructions
execute naturally, with no native expected values, RAM image or helper stubs.
All 48S files are read-only inputs. New evidence belongs only to groupShape48T.
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
import candidate_collection_fixtures48s as old
from query_oracle48h import bits, fp, signed
ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/groupShape48T'
MEMBER, SHAPE, GEOMETRY, MATRIX, GROUP, WORLD, DESC, MEMBERS = range(0x20000,0xa0000,0x10000)
sha = old.sha
put, get, floats = old.put, old.get, old.floats


def write_json(name,data):
    raw=(json.dumps(data,indent=2)+'\n').encode();(OUT/name).write_bytes(raw);return sha(raw)


def freeze():
    """Independently freeze exact full entries and dispatch from authorized ELF."""
    OUT.mkdir(parents=True,exist_ok=True);mp=OUT/'map';mp.mkdir(exist_ok=True)
    elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==old.ELF_SHA
    source=json.loads((old.MAP/'original-words.json').read_text());blocks=[]
    for block in source['blocks']:
        if block['name'] not in ('group_member','shape_matrix_setter'):continue
        b=copy.deepcopy(block);raw=elf[b['start']-0x100000+0x80:b['end_exclusive']-0x100000+0x80]
        assert sha(raw)==b['sha256'] and raw==(old.MAP/(b['name']+'.bin')).read_bytes()
        assert raw==b''.join(struct.pack('<I',r['word']) for r in b['instructions'])
        (mp/(b['name']+'.bin')).write_bytes(raw)
        (mp/(b['name']+'.txt')).write_bytes((old.MAP/(b['name']+'.txt')).read_bytes());blocks.append(b)
    assert len(blocks)==2
    write_json('map/original-words.json',dict(format='group-shape-source-map48t-v1',elf=source['elf'],pt_load=source['pt_load'],
        no_guest_ram_read=True,no_patched_words=True,source48s_sha256=sha((old.MAP/'original-words.json').read_bytes()),blocks=blocks))
    data=json.loads((old.MAP/'dispatch-data.json').read_text())
    a=next(x for x in data['arrays'] if x['name']=='shape_setter_switch')
    raw=elf[a['file_offset']:a['file_offset']+a['bytes']];assert sha(raw)==a['sha256']
    (mp/'shape_setter_switch.bin').write_bytes(raw)
    write_json('map/dispatch-data.json',dict(elf_sha256=old.ELF_SHA,arrays=[a]))
    return old.load_words()


class GroupOriginal(old.CollectorOriginal):
    """Passive write/read tracing; unchanged 48S ISA implementation."""
    def __init__(self,words):
        super().__init__(words);self.watch=[];self.write_trace=[];self.data_read_trace=[]
    def write(self,a,v,size=4):
        if self.executing and any(lo<=a and a+size<=hi for lo,hi in self.watch):
            self.write_trace.append(dict(pc=self.pc,address=a,size=size,value=v&((1<<(size*8))-1)))
        return super().write(a,v,size)
    def read(self,a,size=4):
        if self.executing and any(lo<=a and a+size<=hi for lo,hi in self.watch):
            self.data_read_trace.append(dict(pc=self.pc,address=a,size=size))
        return super().read(a,size)


def helper_case(name,entry='group_member'):
    m=[0xa5000000+i for i in range(64)];m[34]=0;m[35]=SHAPE
    s=[0x42000000+i for i in range(72)]
    s[0:16]=floats([float(i+31) for i in range(16)])
    s[16:32]=floats(old.IDENTITY);s[32:48]=floats(old.IDENTITY[:12]+[31.,-23.,17.,1.])
    s[48:64]=floats([float(-i-51) for i in range(16)]);s[69]=GEOMETRY
    g=floats([91.,92.,93.,.25,-.5,.75,1.,2.,3.])
    return dict(name=name,entry=entry,member_words=m,shape_words=s,shape_geometry_words=g,
        input_matrix=floats(old.IDENTITY[:12]+[2.,3.,4.,1.]),position=[2,3,4],width_byte=32,height_byte=64,
        dimensions=floats([2.,1.,3.,5.]),group_scale=bits(1.),shape_present=True,geometry_present=True)


def helper_cases():
    cs=[]
    def add(name,edit=None,entry='group_member'):
        c=helper_case(name,entry)
        if edit:edit(c)
        cs.append(c);return c
    for entry in ('group_member','shape_matrix_setter'):
        add(entry+'-identity',entry=entry)
        matrices=[('scaled',[2.,0.,0.,0.,0.,3.,0.,0.,0.,0.,4.,0.,13.,-17.,19.,1.]),
            ('rotated',[0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,13.,-17.,19.,1.]),
            ('sheared',[1.,.5,-.25,0.,2.,3.,.75,0.,-.125,4.,2.,0.,13.,-17.,19.,1.]),
            ('all-w',[1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,16.]),
            ('signed-zero',[-0.,0.,-0.,-0.,0.,-0.,0.,-0.,-0.,0.,-0.,-0.,-0.,-0.,-0.,-0.])]
        for label,matrix in matrices:
            def change(c,matrix=matrix):
                c['shape_words'][32:48]=floats(matrix);c['input_matrix']=floats(matrix)
                c['shape_words'][16:32]=floats([.5,-1.,2.,-0.,0.,.25,-.5,3.,1.,2.,-3.,4.,-2.,.125,7.,-0.])
            add(entry+'-'+label,change,entry)
        for flag in (0,1,0xffffffff,0x80000000):
            def dirty(c,flag=flag):
                c['shape_words'][64:69]=[flag,flag^0xffffffff,0xdeadbeef,0xfeedcafe,flag]
                c['shape_words'][70:72]=[0xbaadf00d,0x80000000];c['member_words'][62]=flag
            add(entry+'-dirty-'+str(flag),dirty,entry)
        for half in ([0.,-0.,0.],[-1.,-2.,-3.],[.125,.25,.5],[1000.,.001,17.]):
            add(entry+'-half-'+str(half),lambda c,h=half:c['shape_geometry_words'].__setitem__(slice(6,9),floats(h)),entry)
        for center in ([0.,-0.,0.],[-0.,-0.,-0.],[1.25,-2.5,4.75]):
            add(entry+'-center-'+str(center),lambda c,v=center:c['shape_geometry_words'].__setitem__(slice(3,6),floats(v)),entry)
    for byte in (0,1,127,128,255):
        add('group-signed-byte-'+str(byte),lambda c,b=byte:c.update(width_byte=b,height_byte=b))
    for scale in (0.,-0.,.125,-2.,1.25):
        add('group-scale-'+str(bits(scale)),lambda c,s=scale:c.update(group_scale=bits(s),position=[65535,32768,65534]))
    for tag in (1,2,3,4,5,6,7,0xffffffff):
        add('group-comparison-tag-'+str(tag),lambda c,t=tag:c['member_words'].__setitem__(34,t))
    for tag in (3,4,7,0xffffffff):
        add('setter-noop-tag-'+str(tag),lambda c,t=tag:c['member_words'].__setitem__(34,t),'shape_matrix_setter')
    add('group-null-shape',lambda c:c.update(shape_present=False))
    for entry in ('group_member','shape_matrix_setter'):
        add(entry+'-null-geometry-boundary',lambda c:c.update(geometry_present=False),entry)
    rng=random.Random(0x48a135cf0)
    for i in range(128):
        entry='group_member' if i%2==0 else 'shape_matrix_setter'
        c=add('finite-random-'+str(i),entry=entry)
        for sl in (slice(16,32),slice(32,48)):c['shape_words'][sl]=floats([rng.uniform(-8.,8.) for _ in range(16)])
        c['input_matrix']=floats([rng.uniform(-8.,8.) for _ in range(16)])
        c['shape_geometry_words'][3:9]=floats([rng.uniform(-5.,5.) for _ in range(6)])
        c['position']=[rng.randrange(65536) for _ in range(3)];c['group_scale']=bits(rng.choice([.015625,.125,-.25,1.]))
        c['width_byte']=rng.randrange(256);c['height_byte']=rng.randrange(256)
    return cs


def run_loop(m,pc,collector=False,c=None):
    boundary=None
    while pc!=old.RETURN:
        if pc==0x11f7b0 and m.g[3][0]==0:boundary='null-shape0-geometry';break
        if collector:
            if pc in (0x3075a8,0x1336c0):boundary='diagnostic-output-helper';break
            if pc==0x105870:boundary='spatial-pool-growth';break
            if pc in (0x120378,0x12044c,0x1208e4,0x1209f8) or (pc==0x120c4c and c['query']['shape_tag'] not in (1,3,6)):
                boundary='unsupported-query-bounds';break
            if pc==0x1207ac and signed(m.read(old.SHAPE))<=0:boundary='nonpositive-shape3-pair-count';break
            if pc==0x1225b8 and m.g[5][0]==0:boundary='null-group-member-result';break
            if pc==0x29471c and m.g[4][0]==0:boundary='null-spatial-method';break
        if pc not in m.words:boundary='unmapped-original-dependency';break
        target=m.execute(pc)
        if target is not None:
            if not m.annul:assert m.execute(pc+4) is None
            pc=target
        else:pc+=4
    return boundary,pc


def audit(m,initial):
    assert m.data==b'' and m.allowed_source_addresses==set() and not m.source_reads
    assert m.read_bytes <= initial | m.written_bytes
    return dict(initialized_bytes=len(initial),execution_read_bytes=len(m.read_bytes),execution_written_bytes=len(m.written_bytes),source_reads=0)


def execute_helper(words,dispatch,c):
    m=GroupOriginal(words);m.identities={0:0,MEMBER:1,GROUP:101}
    c['member_words'][35]=SHAPE if c['shape_present'] else 0;c['shape_words'][69]=GEOMETRY if c['geometry_present'] else 0
    for a,values in ((MEMBER,c['member_words']),(SHAPE,c['shape_words']),(GEOMETRY,c['shape_geometry_words']),(MATRIX,c['input_matrix'])):put(m,a,values)
    for a,w in dispatch.items():m.write(a,w)
    m.write(0x3e1ce8,WORLD);m.write(WORLD+0x590,c['group_scale']);m.write(WORLD+0x6c,DESC)
    m.write(GROUP+0xe,0,1);m.write(GROUP+0x10,c['width_byte'],1);m.write(GROUP+0x11,c['height_byte'],1)
    for i,v in enumerate(c['position']):m.write(GROUP+8+2*i,v,2)
    for off,v in zip((0x180,0x184,0x18c,0x194),c['dimensions']):m.write(DESC+off,v)
    m.write(DESC+0x204,MEMBERS);m.write(MEMBERS,MEMBER)
    m.watch=[(MEMBER,MEMBER+256),(SHAPE,SHAPE+288),(GEOMETRY,GEOMETRY+36)]
    m.g[4][0]=GROUP if c['entry']=='group_member' else MEMBER;m.g[5][0]=0 if c['entry']=='group_member' else MATRIX
    m.g[29][0]=old.STACK;m.g[31][0]=old.RETURN;initial=set(m.overlay)
    boundary,pc=run_loop(m,0x135cf0 if c['entry']=='group_member' else 0x11f680)
    r=dict(member_words=get(m,MEMBER,64),shape_words=get(m,SHAPE,72),shape_geometry_words=get(m,GEOMETRY,9),
        return_member=(m.g[2][0]==MEMBER) if c['entry']=='group_member' and boundary is None else None,
        boundary=boundary,stop_pc=pc,steps=m.steps,write_trace=m.write_trace,byte_audit=audit(m,initial),calls=m.calls)
    if boundary is None:assert m.g[29][0]==old.STACK
    assert r['shape_geometry_words']==c['shape_geometry_words']
    return r,m


def collector_case(name):
    c=old.make_case(name);c['modes']=[1,1,0];c['zones']=[[0,None]];c['trees'][0]['items']=[101]
    c['groups']=[dict(id=101,epoch=0,position=[2,3,4],width_byte=32,height_byte=64,dimensions=floats([2.,1.,3.,5.]),members=[1,2,3])]
    for x in c['candidates'][:3]:
        h=helper_case('unused');x.update(shape_tag=0,shape_words=h['shape_words'],shape_geometry_words=h['shape_geometry_words'],shape_alias_id=x['id'],geometry_alias_id=x['id'])
    return c


def collector_cases():
    cs=[]
    def add(name,edit=None):
        c=collector_case(name)
        if edit:edit(c)
        cs.append(c);return c
    add('group-shape0-complete')
    for members in ([1],[1,1,2],[1,2,1],[3,2,1],[]):add('members-'+str(members),lambda c,v=members:c['groups'][0].update(members=v))
    for cap in (0,1,2,3):add('capacity-'+str(cap),lambda c,v=cap:c.update(capacity=v))
    for byte in (0,127,128,255):add('signed-scale-byte-'+str(byte),lambda c,v=byte:c['groups'][0].update(width_byte=v,height_byte=v))
    for scale in (0.,-0.,.125,-2.):add('world-scale-'+str(bits(scale)),lambda c,v=scale:c.update(group_scale=bits(v)))
    for tag in (1,2,3,4,5,6,7,0xffffffff):add('comparison-tag-'+str(tag),lambda c,v=tag:[x.update(shape_tag=v) for x in c['candidates'][:3]])
    add('null-shape-boundary',lambda c:c['candidates'][0].update(shape_present=False))
    add('null-geometry-boundary',lambda c:c['candidates'][0].update(geometry_present=False))
    add('disabled-query',lambda c:c['query'].update(flags=0))
    add('ignore-group',lambda c:c['modes'].__setitem__(2,1))
    add('sphere-rejected',lambda c:(c['modes'].__setitem__(1,0),c['groups'][0].update(position=[60000,3,60000])))
    add('sphere-touching',lambda c:(c['modes'].__setitem__(1,0),c['groups'][0].update(position=[0,65535,0])))
    add('dirty-shape-metadata',lambda c:[x['shape_words'].__setitem__(slice(64,69),[0xffffffff,0xdeadbeef,0xbaadf00d,1,0x80000000]) for x in c['candidates'][:3]])
    add('shared-shape-backing',lambda c:[x.update(shape_alias_id=1,geometry_alias_id=1) for x in c['candidates'][:3]])
    add('shared-geometry-backing',lambda c:[x.update(geometry_alias_id=1) for x in c['candidates'][:3]])
    def two_groups(c):
        g=copy.deepcopy(c['groups'][0]);g.update(id=102,position=[19,23,29],members=[2,1,2]);c['groups'].append(g);c['trees'][0]['items']=[101,102]
    add('two-groups-repeated-members',two_groups)
    add('two-groups-shared-shape',lambda c:(two_groups(c),[x.update(shape_alias_id=1,geometry_alias_id=1) for x in c['candidates'][:3]]))
    add('group-followed-direct-member',lambda c:c['trees'][0].update(items=[101,1]))
    add('direct-member-followed-group',lambda c:c['trees'][0].update(items=[1,101]))
    add('group-repeat-across-trees',lambda c:(c['trees'].append(dict(items=[101],epoch=20,flags=1,blocked=0,cache=[0,0,1023,1023])),c.update(zones=[[0,None],[1,None]])))
    add('member-filter-bypassed',lambda c:[x['filter'].update(flags=0,flags31=0,category=0) for x in c['candidates'][:3]])
    for i,h in enumerate(helper_cases()):
        if h['entry']!='group_member' or h['member_words'][34]!=0 or not h['shape_present'] or not h['geometry_present']:continue
        if not any(key in h['name'] for key in ('signed-zero','all-w','scaled','rotated','sheared','finite-random-')):continue
        def matrix(c,h=h):
            for x in c['candidates'][:3]:x.update(shape_words=copy.deepcopy(h['shape_words']),shape_geometry_words=copy.deepcopy(h['shape_geometry_words']))
            c['group_scale']=h['group_scale'];c['groups'][0].update(position=h['position'],width_byte=h['width_byte'],height_byte=h['height_byte'])
        add('collector-'+h['name'],matrix)
    return cs


def initialize_collector(words,dispatch,c):
    # Reuse the frozen48S explicit initializer, then transfer its sparse overlay.
    base=old.initialize(words,dispatch,c);m=GroupOriginal(words)
    for k,v in base.__dict__.items():setattr(m,k,v)
    m.watch=[];m.write_trace=[];m.data_read_trace=[];m.shape_addresses={};m.geometry_addresses={}
    initialized_shapes=set();initialized_geometry=set()
    for x in c['candidates']:
        if 'shape_tag' not in x:continue
        sid=x.get('shape_alias_id',x['id']);gid=x.get('geometry_alias_id',x['id'])
        a=0xc00000+sid*0x200;g=0xd00000+gid*0x100
        m.shape_addresses[x['id']]=a;m.geometry_addresses[x['id']]=g
        x['shape_words'][69]=g if x.get('geometry_present',True) else 0
        if a not in initialized_shapes:put(m,a,x['shape_words']);initialized_shapes.add(a)
        if g not in initialized_geometry:put(m,g,x['shape_geometry_words']);initialized_geometry.add(g)
        m.write(old.obj(x['id'])+0x8c,a if x.get('shape_present',True) else 0)
        m.watch.extend([(a,a+288),(old.obj(x['id'])+0x60,old.obj(x['id'])+0x84),(g,g+36)])
    return m


def collector_state(m,c):
    r=old.state(m,c)
    for x,out in zip(c['candidates'],r['candidates']):
        if 'shape_tag' not in x:continue
        out['shape_words']=get(m,m.shape_addresses[x['id']],72)
        out['shape_geometry_words']=get(m,m.geometry_addresses[x['id']],9)
    return r


def execute_collector(words,dispatch,c):
    m=initialize_collector(words,dispatch,c);c['spatial_layout']=m.layouts;initial=set(m.overlay);c['initial_state']=collector_state(m,c)
    m.g[4][0]=old.Q;m.g[5][0]=c['capacity'];m.g[6][0]=old.COUNT;m.g[7][0]=old.OUTPUT
    for i,v in enumerate(c['modes']):m.g[8+i][0]=v
    m.g[29][0]=old.STACK;m.g[31][0]=old.RETURN
    boundary,pc=run_loop(m,0x122350,True,c);r=collector_state(m,c)
    r.update(boundary=boundary,stop_pc=pc,residual_v0=m.g[2][0] if pc==old.RETURN else None,events=m.events,calls=m.calls,
        steps=m.steps,extension_coverage=[f'{a:08X}' for a in sorted(m.extension_coverage)],byte_audit=audit(m,initial))
    if boundary is None:assert m.g[29][0]==old.STACK
    assert r['output'][r['count']:]==c['initial_state']['output'][r['count']:]
    return r,m


def preserved_audit(words,dispatch):
    saved_path=old.OUT/'native-fixtures.json';saved_raw=saved_path.read_bytes();saved=json.loads(saved_raw)['cases']
    cases=old.make_cases();assert len(cases)==len(saved)==239
    for c,want in zip(cases,saved):
        r,m=old.execute_case(words,dispatch,c);assert r==want['expected'],c['name']
        c['expected']=r;c['native_status']=want['native_status'];assert c==want,c['name']
    assert saved_path.read_bytes()==saved_raw
    # A separate prior numerical/caller corpus exercises scalar and VU paths
    # through the same unchanged interpreter; never regenerate its files.
    import vehicle_contact_fixtures48r as prev
    pw,pd=prev.load_words();prior_path=prev.OUT/'native-fixtures.json';prior_raw=prior_path.read_bytes();prior=json.loads(prior_raw)['cases']
    for c in prior:
        inp=copy.deepcopy(c);want=inp.pop('expected');r,m=prev.execute_case(pw,pd,inp);assert r==want,c['name']
    assert prior_path.read_bytes()==prior_raw
    return dict(isa_extension_added=False,unchanged48s_cases=len(cases),unchanged48s_fixture_sha256=sha(saved_raw),
        unchanged48r_numerical_cases=len(prior),unchanged48r_fixture_sha256=sha(prior_raw),
        interpreter_sources=[dict(path=p,sha256=sha((ROOT/p).read_bytes())) for p in ('tools/gameplay/query_oracle48h.py','tools/gameplay/spring_fixture48j.py','tools/gameplay/candidate_collection_fixtures48s.py')])


def semantic_checks(helpers,collectors):
    by={c['name']:c for c in helpers};cb={c['name']:c for c in collectors};checks=[]
    def check(name,condition):
        assert condition,name;checks.append(dict(name=name,passed=True))
    valid=[c for c in helpers if c['member_words'][34]==0 and c['shape_present'] and not c['expected']['boundary']]
    for entry in ('group_member','shape_matrix_setter'):
        cc=[c for c in valid if c['entry']==entry]
        check(entry+'-sphere-W-is-positive-one',all(c['expected']['member_words'][27]==bits(1.) for c in cc))
        check(entry+'-geometry-is-read-only',all(c['expected']['shape_geometry_words']==c['shape_geometry_words'] for c in cc))
        check(entry+'-local-and-metadata-preserved',all(c['expected']['shape_words'][16:32]==c['shape_words'][16:32] and c['expected']['shape_words'][64:]==c['shape_words'][64:] for c in cc))
        check(entry+'-only-member-sphere-changes',all(all(c['member_words'][i]==c['expected']['member_words'][i] for i in range(64) if i not in (24,25,26,27,32)) for c in cc))
        check(entry+'-radius-not-matrix-scaled',all(by[entry+'-'+label]['expected']['member_words'][32]==by[entry+'-identity']['expected']['member_words'][32] for label in ('scaled','rotated','sheared','all-w','signed-zero')))
        check(entry+'-dirty-state-ignored',all(by[entry+'-dirty-'+str(flag)]['expected']['shape_words'][:64]==by[entry+'-identity']['expected']['shape_words'][:64] and by[entry+'-dirty-'+str(flag)]['expected']['member_words'][24:28]==by[entry+'-identity']['expected']['member_words'][24:28] for flag in (0,1,0xffffffff,0x80000000)))
    check('group-previous-cache-captures-new-translation',all(c['expected']['shape_words'][48:64]==c['expected']['shape_words'][32:48] for c in valid if c['entry']=='group_member'))
    check('direct-setter-previous-cache-captures-old-world',all(c['expected']['shape_words'][48:64]==c['shape_words'][32:48] for c in valid if c['entry']=='shape_matrix_setter'))
    check('direct-setter-copies-full-input-matrix',all(c['expected']['shape_words'][32:48]==c['input_matrix'] for c in valid if c['entry']=='shape_matrix_setter'))
    check('group-preserves-world-basis-and-W',all(c['expected']['shape_words'][32:44]==c['shape_words'][32:44] and c['expected']['shape_words'][47]==c['shape_words'][47] for c in valid if c['entry']=='group_member'))
    check('group-negative-zero-scale-retained-in-caches',by['group-scale-2147483648']['expected']['shape_words'][44:47]==[0x80000000]*3)
    check('group-scale-ratios-are-computed-but-unused',all(by['group-signed-byte-'+str(v)]['expected']['shape_words']==by['group_member-identity']['expected']['shape_words'] for v in (0,1,127,128,255)))
    check('group-noop-tags-preserve-member-and-shape',all(by['group-comparison-tag-'+str(t)]['expected']['shape_words']==by['group-comparison-tag-'+str(t)]['shape_words'] and by['group-comparison-tag-'+str(t)]['expected']['member_words']==by['group-comparison-tag-'+str(t)]['member_words'] for t in (1,2,3,6,7,0xffffffff)))
    check('direct-noop-tags-preserve-member-and-shape',all(by['setter-noop-tag-'+str(t)]['expected']['shape_words']==by['setter-noop-tag-'+str(t)]['shape_words'] and by['setter-noop-tag-'+str(t)]['expected']['member_words']==by['setter-noop-tag-'+str(t)]['member_words'] for t in (3,4,7,0xffffffff)))
    check('group-null-shape-returns-null-without-writes',by['group-null-shape']['expected']['return_member'] is False and not by['group-null-shape']['expected']['write_trace'])
    check('tag0-no-dynamic-dependency',all(c['expected']['calls']==([dict(pc=0x135f08,target=0x11f680)] if c['entry']=='group_member' else []) for c in valid))
    trace=by['group_member-identity']['expected']['write_trace']
    check('group-translation-precedes-setter-cache-copy',[(e['pc'],e['address']) for e in trace[:3]]==[(0x135ef8,SHAPE+0xb0),(0x135f00,SHAPE+0xb4),(0x135f0c,SHAPE+0xb8)])
    check('setter-cache-write-order',trace[3]['address']==SHAPE+0xc0 and trace[19]['address']==SHAPE+0x80 and trace[35]['address']==SHAPE)
    for n in (1,2,3):check('collector-over-capacity-'+str(n),cb['capacity-'+str(n)]['expected']['count']==3)
    check('collector-capacity0-does-not-touch-shape',cb['capacity-0']['expected']['candidates']==cb['capacity-0']['initial_state']['candidates'])
    check('collector-shared-shape-state-propagates',len({tuple(x['shape_words']) for x in cb['two-groups-shared-shape']['expected']['candidates'][:3]})==1)
    check('collector-repeated-member-expands-output',cb['members-[1, 1, 2]']['expected']['output'][:3]==[101]*3 and cb['members-[1, 1, 2]']['expected']['query']['current']==2)
    check('collector-two-groups-order',cb['two-groups-repeated-members']['expected']['output'][:6]==[101]*3+[102]*3)
    check('collector-member-filter-bypassed',cb['member-filter-bypassed']['expected']['count']==3)
    check('null-geometry-boundary-is-after-matrices-before-sphere',all(c['expected']['stop_pc']==0x11f7b0 and c['expected']['member_words']==c['member_words'] and c['expected']['shape_words'][:16]!=c['shape_words'][:16] for c in helpers if c['name'].endswith('null-geometry-boundary')))
    write_json('semantic-checks.json',dict(format='group-shape-semantic-checks48t-v1',checks=checks))
    return checks


def main():
    words,dispatch=freeze();helpers=helper_cases();collectors=collector_cases();coverage=set();steps=0;boundaries=[]
    for kind,cases,execute in (('helper',helpers,execute_helper),('collector',collectors,execute_collector)):
        for c in cases:
            try:r,m=execute(words,dispatch,c)
            except Exception:print('FAILED',kind,c['name'],file=sys.stderr);raise
            c['expected']=r;coverage.update(m.coverage);steps+=m.steps
            if kind=='helper':
                c['native_supported']=c['entry']=='group_member' and c['member_words'][34]==0 and c['shape_present']
                c['native_status']=('Complete' if r['boundary'] is None else 'MissingGeometry') if c['native_supported'] else 'OriginalOnly'
            else:c['native_status']='Complete' if r['boundary'] is None else 'UnsupportedMemberShape'
            if r['boundary']:boundaries.append(dict(kind=kind,name=c['name'],boundary=r['boundary'],pc=r['stop_pc']))
    semantic=semantic_checks(helpers,collectors)
    helper_sha=write_json('helper-fixtures.json',dict(format='initialized-group-shape48t-v1',cases=helpers))
    collector_sha=write_json('collector-fixtures.json',dict(format='initialized-candidate-collection48t-v1',cases=collectors))
    report=dict(elf_sha256=old.ELF_SHA,helper_cases=len(helpers),collector_cases=len(collectors),
        helper_entry_breakdown=dict(group_member=sum(c['entry']=='group_member' for c in helpers),shape_matrix_setter=sum(c['entry']=='shape_matrix_setter' for c in helpers)),
        helper_native_breakdown=dict(group_complete=sum(c['native_status']=='Complete' for c in helpers),group_pointer_boundary=sum(c['native_status']=='MissingGeometry' for c in helpers),direct_setter_original_only=sum(c['entry']=='shape_matrix_setter' for c in helpers),comparison_or_nullshape_original_only=sum(c['entry']=='group_member' and not c['native_supported'] for c in helpers)),
        collector_breakdown=dict(complete=sum(c['native_status']=='Complete' for c in collectors),pointer_boundary=sum(c['native_status']!='Complete' for c in collectors),shared_backing_cases=[c['name'] for c in collectors if any(x.get('shape_alias_id',x['id'])!=x['id'] or x.get('geometry_alias_id',x['id'])!=x['id'] for x in c['candidates'])]),
        native_execution_performed=False,
        numeric_scope='Individually rounded IEEE binary32 ordinary finite active operands and results, signed zeros included. Physical EE/VU exceptional modes, nonfinite consumed data, overflow/underflow and arbitrary invalid addresses are not proved. Unused metadata words remain raw and unvalidated by this source proof.',
        native_boundary_policy='Null geometry and null member shape are precisely reached original pointer boundaries. Original partial state is recorded for source fidelity; transactional bounded native entry must return its explicit error without publishing that partial state. Direct11F680 and group comparison-tag original-only cases are not native differential claims.',
        generator_sha256=sha(Path(__file__).read_bytes()),
        natural_returns=len(helpers)+len(collectors)-len(boundaries),boundaries=boundaries,steps=steps,source_reads=0,
        helper_fixture_sha256=helper_sha,collector_fixture_sha256=collector_sha,semantic_checks=len(semantic),
        helper_native_supported=sum(c['native_supported'] for c in helpers),
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],preserved_audit=preserved_audit(words,dispatch))
    write_json('executed-original-words.json',dict(elf_sha256=old.ELF_SHA,words=[dict(pc=pc,word=words[pc]) for pc in sorted(coverage)]))
    write_json('proof-report.json',report)
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','preserved_audit')},indent=2))
if __name__=='__main__':main()
