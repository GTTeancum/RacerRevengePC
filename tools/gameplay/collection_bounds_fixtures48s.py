"""Independent original1202E0 bounds fixtures; strict synthetic overlay only.

All expected numeric/cache words come from unmodified original execution.
No native expected values, guest snapshots, patches, helper or virtual stubs.
"""
from pathlib import Path
import copy,hashlib,json,math,random,struct,sys
sys.dont_write_bytecode=True
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits,fp
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/collectionBounds48S';MAP=OUT/'map'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
MAP_SHA='1c7ea5b4e5bc4a347a0cafdea35f869aac50888fa078716d4e6d3ffe31d4b66e'
Q,SHAPE,ENDPOINTS,METADATA,SCENES,CENTER,HALF=0x10000,0x11000,0x20000,0x40000,0x50000,0x60000,0x61000
STACK,RETURN=0x1ff0000,0xfffffff0
IDENTITY=[1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.]
def sha(v):return hashlib.sha256(v).hexdigest()
def floats(v):return [bits(x) for x in v]
def put(m,a,v):
 for i,x in enumerate(v):m.write(a+4*i,x)
def get(m,a,n):return [m.read(a+4*i) for i in range(n)]
def load_words():
 raw=(MAP/'original-words.json').read_bytes();assert sha(raw)==MAP_SHA
 p=json.loads(raw);assert p['elf']['sha256']==ELF_SHA
 words={}
 for b in p['blocks']:
  rows=b['instructions'];assert [r['pc'] for r in rows]==list(range(b['start'],b['end_exclusive'],4))
  raw=b''.join(struct.pack('<I',r['word']) for r in rows)
  assert sha(raw)==b['sha256'] and raw==(MAP/(b['name']+'.bin')).read_bytes()
  for r in rows:assert r['pc'] not in words;words[r['pc']]=r['word']
 d=json.loads((MAP/'dispatch-data.json').read_text());assert d['elf_sha256']==ELF_SHA
 dispatch={}
 for a in d['arrays']:
  raw=struct.pack('<'+'I'*len(a['words']),*a['words']);assert sha(raw)==a['sha256'] and raw==(MAP/(a['name']+'.bin')).read_bytes()
  dispatch.update((a['va']+4*i,w) for i,w in enumerate(a['words']))
 if '--frozen' not in sys.argv:
  elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==ELF_SHA
  h=struct.unpack_from('<16sHHIIIIIHHHHHH',elf);segments=[struct.unpack_from('<8I',elf,h[5]+i*h[9]) for i in range(h[10])]
  assert tuple(p['pt_load']) in segments
  for pc,w in list(words.items())+list(dispatch.items()):assert struct.unpack_from('<I',elf,pc-0x100000+128)[0]==w,hex(pc)
 return words,dispatch
class BoundsOriginal(SpringOriginal):
 def __init__(self,words):
  super().__init__(data=b'',words=words);self.allowed_source_addresses=set();self.active=False;self.reads=set();self.writes=set();self.calls=[]
 def read(self,a,size=4):
  if self.active:self.reads.update(range(a,a+size))
  return super().read(a,size)
 def write(self,a,v,size=4):
  if self.active:self.writes.update(range(a,a+size))
  return super().write(a,v,size)
 def execute(self,pc):
  if self.words[pc]>>26==3:self.calls.append([pc,(self.words[pc]&0x3ffffff)*4])
  self.active=True
  target=super().execute(pc);self.active=False;return target

def case(name,tag=3):
 return dict(name=name,shape_tag=tag,world=floats(IDENTITY),local_center=floats([2.,-3.,5.,1.]),local_half=floats([1.,2.,3.,1.]),
  cached_center=[0x7fc12345,0x80000000,0x4f123456,0xffc54321],cached_radius=0x7fa76543,pair_count=1,
  endpoints=[floats([-1.,-2.,-3.,1.]),floats([3.,4.,5.,1.])],endpoint_scenes=None,scenes=[],native_status='Complete')
def execute(words,dispatch,c):
 m=BoundsOriginal(words)
 for a,w in dispatch.items():m.write(a,w)
 m.write(Q+0x88,c['shape_tag']);m.write(Q+0x8c,SHAPE);put(m,Q+0x60,c['cached_center']);m.write(Q+0x80,c['cached_radius'])
 put(m,CENTER,[0xdead0000+i for i in range(4)]);put(m,HALF,[0xbeef0000+i for i in range(4)])
 if c['shape_tag'] in (1,6):
  put(m,SHAPE+(0x80 if c['shape_tag']==1 else 0),c['world'])
  put(m,SHAPE+(0 if c['shape_tag']==1 else 0x40),c['local_center'])
  put(m,SHAPE+(0x10 if c['shape_tag']==1 else 0x50),c['local_half'])
 elif c['shape_tag']==3:
  m.write(SHAPE,c['pair_count']&0xffffffff);m.write(SHAPE+4,METADATA);m.write(SHAPE+8,ENDPOINTS)
  for i,v in enumerate(c['endpoints']):put(m,ENDPOINTS+16*i,v)
  for i,b in enumerate(c['endpoint_scenes'] if c['endpoint_scenes'] is not None else [0]*max(0,c['pair_count']*2)):m.write(METADATA+4*i,SCENES+0x200*(b-1) if b else 0)
  for i,v in enumerate(c['scenes']):put(m,SCENES+0x200*i+0x100,v)
 initial=dict(m.overlay)
 m.g[4][0]=Q;m.g[5][0]=CENTER;m.g[6][0]=HALF;m.g[29][0]=STACK;m.g[31][0]=RETURN
 pc=0x1202e0;boundary=None
 unsupported={0x120378,0x12044c,0x1208e4,0x1209f8}
 while pc!=RETURN:
  if pc in unsupported or (pc==0x120c4c and c['shape_tag'] not in (1,3,6)):boundary='unsupported-shape-tag';break
  target=m.execute(pc)
  if target is not None:
   if not m.annul:assert m.execute(pc+4) is None
   pc=target
  else:pc+=4
 assert m.data==b'' and m.allowed_source_addresses==set() and not m.source_reads
 allowed=set(range(STACK-0x500,STACK))|set(range(CENTER,CENTER+16))|set(range(HALF,HALF+12))
 if c['shape_tag']==3:allowed|=set(range(Q+0x60,Q+0x70))|set(range(Q+0x80,Q+0x84))
 assert m.writes<=allowed,[(hex(x)) for x in m.writes-allowed]
 assert all(m.overlay[a]==v for a,v in initial.items() if a not in allowed)
 expected=dict(center=get(m,CENTER,4),half=get(m,HALF,4),cached_center=get(m,Q+0x60,4),cached_radius=m.read(Q+0x80),boundary=boundary,
  stop_pc=pc,steps=m.steps,calls=m.calls,source_reads=0,read_bytes=len(m.reads),write_bytes=len(m.writes))
 assert expected['half'][3]==0xbeef0003
 c['expected']=expected
 return m

def cases():
 cs=[]
 def add(c):cs.append(c);return c
 for tag in (1,6):
  add(case('tag%d-identity-preserves-cached-raw'%tag,tag))
  c=add(case('tag%d-full-affine-negative-half'%tag,tag));c['world']=floats([2.,.2,-.3,math.nan,-.7,3.,.1,math.inf,.25,-.8,-4.,-math.inf,17.,-23.,91.,math.nan]);c['local_half']=floats([-1.,2.,-3.,math.nan]);c['local_center'][3]=0x7fc45678
 add(case('tag3-optional-null-metadata'))
 c=add(case('tag3-explicit-zero-metadata'));c['endpoint_scenes']=[0,0]
 c=add(case('tag3-distinct-cached-scenes'));c['endpoint_scenes']=[1,2];c['scenes']=[floats([2.,.5,-.25,math.nan,0.,3.,.2,math.inf,.25,-.5,4.,-math.inf,7.,8.,9.,math.nan]),floats(IDENTITY)];c['scenes'][1][12:15]=floats([-20.,30.,40.])
 c=add(copy.deepcopy(c));c['name']='tag3-shared-scene-identity';c['endpoint_scenes']=[1,1]
 c=add(copy.deepcopy(c));c['name']='tag3-first-transformed-second-raw';c['endpoint_scenes']=[1,0]
 c=add(copy.deepcopy(c));c['name']='tag3-first-raw-second-transformed';c['endpoint_scenes']=[0,1]
 c=add(case('tag3-cached-negative-zero-returned-positive-zero'));c['endpoints']=[[0x80000000]*3+[0x7fc12345],[0x80000000]*3+[0xffc76543]]
 c=add(case('tag3-mixed-signed-zero'));c['endpoints']=[[0x80000000,0,0x80000000,0x7fa00001],[0,0x80000000,0,0xffa00001]]
 c=add(case('tag3-ignored-raw-tail'));c['endpoints']+=[[0x7f800000,0xff800000,0x7fc11111,0x7fa12345]];c['endpoint_scenes']=[0,0,0xffffffff];c['scenes']=[[0x7fc11111]*16]
 for tag in (0,2,4,5,7,0xffffffff):
  c=add(case('unsupported-tag-'+str(tag),tag));c['native_status']='UnsupportedBounds'
 for count in (0,-1,-2147483648):
  c=add(case('nonpositive-pairs-'+str(count)));c['pair_count']=count;c['native_status']='InvalidInput'
 for tag in (1,3,6):
  for word,label in ((0x7f800000,'infinity'),(0x7fc12345,'nan'),(0x7f7fffff,'overflow')):
   c=add(case(f'tag{tag}-{label}',tag));c['native_status']='NonFinite'
   if tag==3:c['endpoints'][0][0]=word;c['endpoints'][1][0]=word if label=='overflow' else bits(3.)
   else:c['world'][0]=word;c['local_center'][0]=bits(4.)
 c=add(case('tag3-transform-nonfinite'));c['endpoint_scenes']=[1,0];c['scenes']=[floats(IDENTITY)];c['scenes'][0][0]=0x7f800000;c['native_status']='NonFinite'
 rng=random.Random(0x48_1202e0)
 for n in range(240):
  tag=(1,3,3,6)[n%4];c=add(case(f'random-{n:03d}-tag{tag}',tag))
  def mat():return floats([rng.uniform(-3,3) if i%4!=3 else fp(rng.getrandbits(32)) for i in range(12)]+[rng.uniform(-100,100) for _ in range(3)]+[fp(rng.getrandbits(32))])
  def vec():return floats([rng.uniform(-100,100) for _ in range(3)])+[rng.getrandbits(32)]
  c['world']=mat();c['local_center']=vec();c['local_half']=vec()
  if tag==3:
   c['pair_count']=1+n%7;c['endpoints']=[vec() for _ in range(c['pair_count']*2)];c['scenes']=[mat(),mat()];c['endpoint_scenes']=[rng.randrange(3) for _ in c['endpoints']]
 c=add(case('tag3-257-pairs-no-gameplay-cap'));c['pair_count']=257;c['endpoints']=[floats([float(i),float(-i),float(i%13),1.]) for i in range(514)]
 return cs

def main():
 words,dispatch=load_words();cs=cases();coverage=set();branches={};steps=0;changed=0
 for c in cs:
  m=execute(words,dispatch,c);coverage.update(m.coverage);steps+=m.steps
  for pc,v in m.branches.items():branches.setdefault(pc,set()).update(v)
  changed+=c['native_status']=='Complete' and c['expected']['cached_center']!=c['expected']['center']
 fixture=dict(format='collection-bounds-fixtures48s-v1',cases=cs)
 raw=(json.dumps(fixture,indent=2)+'\n').encode();(OUT/'fixtures.json').write_bytes(raw)
 report=dict(format='collection-bounds-original-proof48s-v1',cases=len(cs),complete_cases=sum(c['native_status']=='Complete' for c in cs),
  boundary_cases=sum(c['native_status']=='UnsupportedBounds' for c in cs),nonfinite_cases=sum(c['native_status']=='NonFinite' for c in cs),invalid_count_cases=sum(c['native_status']=='InvalidInput' for c in cs),
  steps=steps,cached_vs_returned_different_cases=changed,tag3_cached_vs_returned_different_cases=sum(c['shape_tag']==3 and c['native_status']=='Complete' and c['expected']['cached_center']!=c['expected']['center'] for c in cs),source_reads=0,data_bytes=0,strict_overlay=True,native_expected=False,patched_words=False,helper_stubs=False,
  elf_sha256=ELF_SHA,map_sha256=MAP_SHA,fixtures_sha256=sha(raw),model='Separately rounded ordinary binary32; corrected audited VU selector. No physical EE exceptional-mode claim.',
  interpreter_sha256=sha((ROOT/'tools/gameplay/query_oracle48h.py').read_bytes()),extension_sha256=sha((ROOT/'tools/gameplay/spring_fixture48j.py').read_bytes()),
  coverage=[f'{x:08X}' for x in sorted(coverage)],branches={f'{k:08X}':sorted(v) for k,v in sorted(branches.items())})
 (OUT/'original-execution.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
