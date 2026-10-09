"""Original 122660 + 1B19B4 before/after fixtures, no game process launched."""
import copy,hashlib,json,random,struct
from pathlib import Path
from query_extract48h import ROOT,RAM
from query_oracle48h import Original,bits,fp,f32

OUT=ROOT/'evidence/query48J';OUT.mkdir(parents=True,exist_ok=True)
QUERY=0xd113f0;OBJECTS=[0x9e7d30,0x9f7350];IDENTITIES={QUERY:100,OBJECTS[0]:1,OBJECTS[1]:2,0:0,0x6000:300,0x6010:301}
def identity(a):
    if a not in IDENTITIES:raise ValueError(f'unmapped identity {a:08x}')
    return IDENTITIES[a]
def address(i):return next(a for a,v in IDENTITIES.items() if v==i)
def sha(data):return hashlib.sha256(data).hexdigest()
def run_until(m,pc,end):
    while pc!=end:
        target=m.execute(pc)
        if target is not None:
            if not m.annul:
                assert m.execute(pc+4) is None
            pc=target
        else:pc+=4

def read_filter(m,obj):
    return {'identity':identity(obj),'category':m.read(obj+0xa4),'accept_mask':m.read(obj+0xb8),
      'identity_lists':[{'mode':m.read(obj+off),'ids':[identity(m.read(m.read(obj+off+4)+4*i)) for i in range(m.read(obj+off+8))]} for off in (0xbc,0xc8)],
      'category_mode':m.read(obj+0xd4),'category_masks':[m.read(m.read(obj+0xd8)+4*i) for i in range(m.read(obj+0xdc))],
      'flags':m.read(obj+0xf8,1),'flags31':m.read(obj+0x31,1)}
def write_filter(m,obj,f,slot):
    m.write(obj+0xa4,f['category']);m.write(obj+0xb8,f['accept_mask'])
    m.write(obj+0xf8,f['flags'],1);m.write(obj+0x31,f['flags31'],1)
    for i,off in enumerate((0xbc,0xc8)):
        a=slot+i*0x100;l=f['identity_lists'][i]
        m.write(obj+off,l['mode']);m.write(obj+off+4,a);m.write(obj+off+8,len(l['ids']))
        for k,x in enumerate(l['ids']):m.write(a+k*4,address(x))
    m.write(obj+0xd4,f['category_mode']);m.write(obj+0xd8,slot+0x200);m.write(obj+0xdc,len(f['category_masks']))
    for k,x in enumerate(f['category_masks']):m.write(slot+0x200+k*4,x)
def read_surface(m):return {'flags':[m.read(QUERY+x) for x in (0x108,0x110,0x114)],'point':m.vec(QUERY+0x150),'normal':m.vec(QUERY+0x170)}
def write_surface(m,s):
    for x,v in zip((0x108,0x110,0x114),s['flags']):m.write(QUERY+x,v)
    m.vector(QUERY+0x150,s['point']);m.vector(QUERY+0x170,s['normal'])
def read_state(m):
    count=m.read(QUERY+0x1e8)
    return {'surface':read_surface(m),'collected':[identity(m.read(m.read(QUERY+0x1ec)+i*4)) for i in range(count)],
      'excluded_owner':identity(m.read(QUERY+0x12c)),'current_candidate':identity(m.read(QUERY+0xf4))}
def read_candidate(m,obj,index):
    return {'mesh':index,'filter':read_filter(m,obj),'sphere':[*m.vec(obj+0x60),m.loadf(obj+0x80)],
      'world':m.vec(m.read(obj+0x8c),16),'owner':identity(m.read(obj+0x9c)),
      'type_tag':m.read(obj+2,1),'response_callback':bool(m.read(obj+0xe8)),
      'copied40':m.vec(obj+0x40,4),'copied50':m.vec(obj+0x50,4)}

def main():
    raw=RAM.read_bytes();assert sha(raw)=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
    original=Original(raw);original.g[28][0]=0x3dd970
    original.g[4][0]=QUERY;original.g[29][0]=0x1ff0000;original.g[31][0]=0xfffffff0
    assert original.read(QUERY+0x120)==0 and original.read(QUERY+0x124)==0
    run_until(original,0x1b1800,0x1b1904)
    # This is a precise boundary, before matrix/spatial-index refresh. The saved
    # prepared query matrix and bounds are explicit inputs to the later phase.
    prefix_pcs=set(original.coverage);prefix_reads=set(original.source_reads)
    shape=original.read(QUERY+0x8c);leaf=original.read(shape+0x50)
    state=read_state(original);state['current_candidate']=0
    q={'segment':{'first':original.vec(leaf),'second':original.vec(leaf+16),'world':original.vec(shape,16)},
      'sphere':[*original.vec(QUERY+0x60),original.loadf(QUERY+0x80)],'filter':read_filter(original,QUERY),
      'masks':[original.read(a) for a in (0x3e0720,0x4342b0,0x3e0718)],'prepared':state,
      'copied40':original.vec(QUERY+0x40,4),'copied50':original.vec(QUERY+0x50,4),
      'collision_tag':original.read(0x3dd970-0x7ff0,1),'group_tag':original.read(0x3dd970-0x7fec,1),'callback':True}
    candidates=[read_candidate(original,a,i) for i,a in enumerate(OBJECTS)]
    cases=[];source_reads=set(prefix_reads);coverage=set(prefix_pcs)
    replay_data=raw;read_guard=None
    class Trace(Original):
        def __init__(self):super().__init__(replay_data);self.events=[];self.allowed_source_addresses=read_guard
        def execute(self,pc):
            if pc==0x126eb0:self.events.append(['sphere',identity(self.g[5][0])])
            if pc==0x120e70:self.events.append(['filter',identity(self.g[5][0])])
            if pc==0x121410:self.events.append(['tree',identity(self.g[5][0])])
            if pc==0x1b0e50:self.events.append(['callback',identity(self.g[5][0])])
            return super().execute(pc)
    def add_case(name,change=None):
        request=copy.deepcopy(q);cs=copy.deepcopy(candidates);order=[0,1]
        if change:change(request,cs,order)
        m=Trace();m.g[28][0]=0x3dd970
        write_filter(m,QUERY,request['filter'],0x10000)
        m.vector(shape,request['segment']['world']);m.vector(leaf,request['segment']['first']);m.vector(leaf+16,request['segment']['second'])
        m.vector(QUERY+0x60,request['sphere'][:3]);m.storef(QUERY+0x80,request['sphere'][3])
        m.vector(QUERY+0x40,request['copied40']);m.vector(QUERY+0x50,request['copied50'])
        write_surface(m,request['prepared']['surface'])
        m.write(QUERY+0x12c,address(request['prepared']['excluded_owner']));m.write(QUERY+0xf4,address(request['prepared']['current_candidate']))
        m.write(QUERY+0x1e8,len(request['prepared']['collected']))
        for i,v in enumerate(request['prepared']['collected']):m.write(m.read(QUERY+0x1ec)+4*i,address(v))
        for a,v in zip((0x3e0720,0x4342b0,0x3e0718),request['masks']):m.write(a,v)
        m.write(QUERY+0xe4,0x1b0e50 if request['callback'] else 0)
        m.write(0x3dd970-0x7ff0,request['collision_tag'],1);m.write(0x3dd970-0x7fec,request['group_tag'],1)
        for i,c in enumerate(cs):
            obj=OBJECTS[i];write_filter(m,obj,c['filter'],0x11000+i*0x1000)
            m.vector(obj+0x60,c['sphere'][:3]);m.storef(obj+0x80,c['sphere'][3]);m.vector(m.read(obj+0x8c),c['world'])
            m.write(obj+0x9c,address(c['owner']));m.write(obj+2,c['type_tag'],1);assert not c['response_callback']
            m.vector(obj+0x40,c['copied40']);m.vector(obj+0x50,c['copied50'])
        for i,index in enumerate(order):m.write(0x15000+i*4,OBJECTS[index])
        m.g[4][0]=QUERY;m.g[5][0]=len(order);m.g[6][0]=0x15000
        returned=m.run(0x122660)
        # Enter the original assembly tail with its real stack epilogue satisfied.
        # No instruction or branch is patched and no numerical helper is replaced.
        m.g[17][0]=QUERY;m.write(0x1ff0020,0xfffffff0)
        m.run(0x1b19b4)
        expected={'hit':bool(returned),'query_flags':m.read(QUERY+0xf8,1),'state':read_state(m),
          'selected':[m.read(0x9512e0+off+i*4) for off in (0x30,0x20,0x40) for i in range(3)]+[m.read(0x951354)] if returned else [],
          'effects':[{'identity':c['filter']['identity'],'flags':m.read(OBJECTS[i]+0xf8,1),'copied40':m.vec(OBJECTS[i]+0x40,4),'copied50':m.vec(OBJECTS[i]+0x50,4)} for i,c in enumerate(cs)],
          'events':m.events,'instructions':m.steps}
        cases.append({'name':name,'query':request,'candidates':cs,'order':order,'expected':expected})
        source_reads.update(m.source_reads);coverage.update(m.coverage)
    add_case('saved-prepared-list-to-surface')
    for who in ('query','floor'):
        for slot in (0,1):
            for mode in (0,1,2):
                for member in (False,True):
                    def change(r,c,o,who=who,slot=slot,mode=mode,member=member):
                        f=r['filter'] if who=='query' else c[1]['filter'];target=2 if who=='query' else 100
                        f['identity_lists'][slot]={'mode':mode,'ids':[target if member else 300]}
                    add_case(f'{who}-identity{slot}-mode{mode}-member{member}',change)
    for who in ('query','floor'):
        for mode in (0,1,2):
            for masks in ([],[0],[0x20],[0xffffffff]):
                def change(r,c,o,who=who,mode=mode,masks=masks):
                    f=r['filter'] if who=='query' else c[1]['filter'];f['category_mode']=mode;f['category_masks']=masks
                add_case(f'{who}-categories-{mode}-{masks}',change)
    for flag in (0,1,4,16,32,255):
        add_case('floor-flags-'+str(flag),lambda r,c,o,flag=flag:c[1]['filter'].update(flags=flag))
    for flag in (0,2,3,255):
        add_case('floor-active-'+str(flag),lambda r,c,o,flag=flag:c[1]['filter'].update(flags31=flag))
    for category in (0,8,0x20,0x28,0x84,0xac,0xffffffff):
        add_case('floor-category-'+str(category),lambda r,c,o,category=category:c[1]['filter'].update(category=category))
    for mask in (0,8,0x10,0x20,0xffffffff):
        add_case('query-accept-mask-'+str(mask),lambda r,c,o,mask=mask:r['filter'].update(accept_mask=mask))
    for tag in (0,1,2,5,255):
        add_case('floor-skipped-type-'+str(tag),lambda r,c,o,tag=tag:c[1].update(type_tag=tag))
    add_case('reversed-candidates',lambda r,c,o:o.reverse())
    add_case('no-candidates',lambda r,c,o:o.clear())
    add_case('callback-disabled',lambda r,c,o:r.update(callback=False))
    for count in (0,14,15,16):
        def change(r,c,o,count=count):r['prepared']['collected']=[1]*count;o[:]=[1,1,1]
        add_case('collect-cap-'+str(count),change)
    for owner in (0,300):
        def change(r,c,o,owner=owner):c[1]['filter']['category']=0xac;c[1]['owner']=owner
        add_case('force-angle-and-exclusion-'+str(owner),change)
    for delta in (-1.,0.,1.):
        def change(r,c,o,delta=delta):r['prepared']['surface']['flags'][1]=1;r['prepared']['surface']['point'][1]=f32(fp(cases[0]['expected']['selected'][1])+delta)
        add_case('prepared-auxiliary-height-'+str(delta),change)
    # Exact/adjacent radius boundary is exercised around a unit axis distance;
    # target sphere is metadata, independent of unchanged original mesh data.
    for x in (1.,fp(0x3f800001),fp(0x3f7fffff),2.):
        def change(r,c,o,x=x):r['sphere']=[0.,0.,0.,.5];c[1]['sphere']=[x,0.,0.,.5]
        add_case('sphere-boundary-'+str(x),change)
    rng=random.Random(0x48_4a)
    for i in range(32):
        def change(r,c,o):
            r['filter']['category']=rng.getrandbits(8);r['filter']['accept_mask']=rng.getrandbits(8)
            r['filter']['category_mode']=rng.randrange(3);r['filter']['category_masks']=[rng.getrandbits(8) for _ in range(rng.randrange(4))]
            for t in c:
                t['filter']['category']=rng.getrandbits(8);t['filter']['flags']=rng.getrandbits(8);t['filter']['flags31']=rng.getrandbits(8)
                t['filter']['category_mode']=rng.randrange(3);t['filter']['category_masks']=[rng.getrandbits(8) for _ in range(rng.randrange(4))]
        add_case('mixed-filter-'+str(i),change)
    report={'scope':'Original prepared122660 plus1B19B4 assembly, not full1B1800 spatial refresh','snapshot_sha256':sha(raw),
      'prefix_proof':'Original1B1800 to1B1904 with both optional source pointers null; only prepared surface seed retained',
      'prefix_pcs':[f'{x:08X}' for x in sorted(prefix_pcs)],'cases':cases,'executed_pcs':[f'{x:08X}' for x in sorted(coverage)]}
    (OUT/'fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    # Preserve source bytes used by every original run, plus exact dispatch data.
    spans=[]
    for a in sorted(source_reads):
        if spans and a==spans[-1][1]:spans[-1][1]+=1
        else:spans.append([a,a+1])
    sparse=bytearray();slices=[]
    for a,b in spans:
        data=raw[a:b];slices.append({'address':a,'bytes':b-a,'file_offset':len(sparse),'sha256':sha(data)});sparse.extend(data)
    replay_data=bytearray(len(raw));read_guard=set(source_reads)
    for record in slices:
        a=record['address'];n=record['bytes'];offset=record['file_offset'];replay_data[a:a+n]=sparse[offset:offset+n]
    saved_cases=cases;cases=[]
    for case in saved_cases:
        def restore(r,c,o,case=case):
            r.clear();r.update(copy.deepcopy(case['query']));c[:]=copy.deepcopy(case['candidates']);o[:]=case['order']
        add_case(case['name'],restore)
        assert cases[-1]==case,case['name']
    (OUT/'sparse-replay.json').write_text(json.dumps({'result':'PASS','queries':len(cases),'source_bytes':len(sparse),'slices':len(slices),
      'guard':'Every original-data read had to belong to a preserved byte address; all before/after outputs and event traces matched full-source runs'},indent=2)+'\n')
    (OUT/'original-data.bin').write_bytes(sparse)
    (OUT/'original-data.json').write_text(json.dumps({'source':RAM.relative_to(ROOT).as_posix(),'sha256':sha(raw),'fixture_sha256':sha(sparse),'slices':slices},indent=2)+'\n')
    words=[0x4a383452,1,len(cases)]
    def vals(xs):words.extend(int(x)&0xffffffff for x in xs)
    def floats(xs):vals(bits(x) for x in xs)
    def flt(f):
        vals([f['identity'],f['category'],f['accept_mask'],f['flags'],f['flags31']])
        for l in f['identity_lists']:vals([l['mode'],len(l['ids']),*l['ids']])
        vals([f['category_mode'],len(f['category_masks']),*f['category_masks']])
    def state(s):
        vals(s['surface']['flags']);floats(s['surface']['point']+s['surface']['normal'])
        vals([len(s['collected']),*s['collected'],s['excluded_owner'],s['current_candidate']])
    for case in cases:
        r=case['query'];floats(r['segment']['first']+r['segment']['second']+r['segment']['world']+r['sphere']);flt(r['filter'])
        vals(r['masks']);state(r['prepared']);floats(r['copied40']+r['copied50']);vals([r['collision_tag'],r['group_tag'],r['callback']])
        vals([len(case['candidates'])])
        for c in case['candidates']:
            vals([c['mesh']]);floats(c['world']+c['sphere']);flt(c['filter']);vals([c['owner'],c['type_tag'],c['response_callback']]);floats(c['copied40']+c['copied50'])
        vals([len(case['order']),*case['order']]);e=case['expected'];vals([e['hit'],e['query_flags']]);state(e['state'])
        vals(e['selected'] if e['hit'] else [0]*10)
        for effect in e['effects']:vals([effect['identity'],effect['flags']]);floats(effect['copied40']+effect['copied50'])
    rawwords=struct.pack('<'+'I'*len(words),*words);(OUT/'fixtures.bin').write_bytes(rawwords)
    lines=['#pragma once','#include <cstdint>','static const std::uint32_t terrainQueryFixtures48J[]={']
    lines.extend(','.join(f'0x{x:08x}u' for x in words[i:i+12])+',' for i in range(0,len(words),12));lines.append('};')
    (ROOT/'tools/gameplay/terrain_query_fixtures48j.h').write_text('\n'.join(lines)+'\n')
    print(json.dumps({'cases':len(cases),'hits':sum(c['expected']['hit'] for c in cases),'words':len(words),'source_data_bytes':len(sparse),'source_data_slices':len(slices),'original_words_executed':len(coverage)}))

if __name__=='__main__':main()
