"""Authentic 11CA80 staging and type2 11FF60 numeric-prefix fixtures.

Reads immutable ELF instructions only. Original(data=b'') uses explicit tiny
objects; no RAM, native expected values, helper hooks, patched calls or returns.
11FF60 cases stop before the registration call at 11FF74, never fake its return.
"""
import copy, hashlib, json, random, struct, sys
from pathlib import Path
sys.dont_write_bytecode = True
from query_oracle48h import Original, bits
ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT/'evidence/queryPrepare48O'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RANGES = [
 ('pair_copy',0x11ca80,0x11cb20,'05fb87232185b927f953359af1071bb6d40073247878e0bf64c7bad8185bbbd3'),
 ('setter_dispatch',0x11f680,0x11f6c8,'d5c20b3f5c3e3470c52b8085ee85786ae251f73dd3e260f359f753fe294078e2'),
 ('type2_setter',0x11fa30,0x11fc80,'d524acc490cecf518f8a119da55a737257b30202d2c0960f4a93c21e7790ccb9'),
 ('setter_return',0x11fe28,0x11fe44,'a1938475ad0697e56cbd6df982a72c09cc5974e5f84be8d0f649417750aa2c46'),
 ('scene_setter_wrapper',0x11fe50,0x11ff90,'28b248d8d055cecf850d82e110347c6a3f584e85aa11069ce8b70760a2b7a0f8'),
 ('vector_constructor',0x106790,0x1067ac,'e0d8c70d997a0996626a12d2184e4cbbfd221c249fea4f54d3c5e505dec51dfd'),
 ('scene_world_matrix',0x28b550,0x28b694,'ea8ffe28588370fcbf876be092ca5a3fa433bcc9f312e6c6bc3429c640f38ac9')]
DISPATCH = {0x388a08:0x11fa30,0x388a28:0x11fed8}
Q,SHAPE,SOURCE,SOURCE_AUX,DEST,DEST_AUX,SCENE,TREE,PAIR = [0x10000+i*0x1000 for i in range(9)]
RETURN = 0xfffffff0
CAPACITY = 16
IDENTITY = [1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.]
def sha(raw): return hashlib.sha256(raw).hexdigest()
def fwords(values): return [bits(v) for v in values]
def put(m, address, values):
 for i, word in enumerate(values): m.write(address+4*i,word)
def get(m, address, count): return [m.read(address+4*i) for i in range(count)]
def freeze(frozen=False):
 OUT.mkdir(parents=True,exist_ok=True)
 path = OUT/'original-words.json'
 if not frozen:
  elf = (ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
  assert sha(elf)==ELF_SHA
  old = json.loads((ROOT/'evidence/query48H/original-words.json').read_text())
  old_words = {r['pc']:r['word'] for b in old['ranges'] for r in b['words']}
  blocks=[]
  for name,lo,hi,expected in RANGES:
   raw=elf[lo-0x100000+0x80:hi-0x100000+0x80];assert sha(raw)==expected
   rows=[dict(pc=pc,word=struct.unpack_from('<I',raw,pc-lo)[0]) for pc in range(lo,hi,4)]
   for row in rows:
    if row['pc'] in old_words: assert row['word']==old_words[row['pc']]
   blocks.append(dict(name=name,start=lo,end=hi,sha256=expected,instructions=rows))
   (OUT/(name+'.bin')).write_bytes(raw)
  for address,word in DISPATCH.items():assert struct.unpack_from('<I',elf,address-0x100000+0x80)[0]==word
  path.write_text(json.dumps(dict(elf_sha256=ELF_SHA,address_mapping='VA-0x100000+0x80',blocks=blocks,
      dispatch=[dict(address=a,word=w) for a,w in DISPATCH.items()]),indent=2)+'\n')
 proof=json.loads(path.read_text());assert proof['elf_sha256']==ELF_SHA
 assert len(proof['blocks'])==len(RANGES)
 words={}
 for block,(name,lo,hi,expected) in zip(proof['blocks'],RANGES):
  assert (block['name'],block['start'],block['end'],block['sha256'])==(name,lo,hi,expected)
  rows=block['instructions'];assert [r['pc'] for r in rows]==list(range(lo,hi,4))
  raw=b''.join(struct.pack('<I',r['word']) for r in rows)
  assert sha(raw)==expected and raw==(OUT/(name+'.bin')).read_bytes()
  words.update((r['pc'],r['word']) for r in rows)
 assert {r['address']:r['word'] for r in proof['dispatch']}==DISPATCH
 return words

class Guarded(Original):
 def __init__(self,words):
  super().__init__(data=b'',words=words);self.read_addresses=set()
 def read(self,address,size=4):
  self.read_addresses.update(range(address,address+size))
  return super().read(address,size)

def run_to_boundary(m, start, boundary):
 m.g[29][0]=0x1ff0000;m.g[31][0]=RETURN;pc=start
 while pc not in (boundary,RETURN):
  target=m.execute(pc)
  if target is None:pc+=4
  else:
   if not m.annul:assert m.execute(pc+4) is None
   pc=target
 assert m.data==b'' and not m.source_reads
 return pc

def staging_case(words,case):
 m=Guarded(words);put(m,Q+0x88,[case['tag'],SHAPE if case['shape_linked'] else 0])
 put(m,Q+0x60,case['stale_center']);put(m,Q+0x80,[case['stale_radius']])
 put(m,SHAPE,[case['count'],DEST_AUX,DEST]);put(m,SOURCE,case['source']);put(m,SOURCE_AUX,case['source_aux'])
 put(m,DEST,case['initial_dest']);put(m,DEST_AUX,case['initial_aux'])
 m.g[4][0]=Q;m.g[5][0]=SOURCE;m.g[6][0]=SOURCE_AUX if case['has_aux'] else 0
 stop=run_to_boundary(m,0x11ca80,RETURN);reads=m.read_addresses.copy()
 output=dict(tag=m.read(Q+0x88),dest=get(m,DEST,128),aux=get(m,DEST_AUX,32),
             stale_center=get(m,Q+0x60,4),stale_radius=m.read(Q+0x80))
 copied=2*case['count'] if case['shape_linked'] else 0
 assert output['tag']==3
 assert output['dest']==case['source'][:4*copied]+case['initial_dest'][4*copied:]
 assert output['aux']==(case['source_aux'][:copied] if case['has_aux'] else [0]*copied)+case['initial_aux'][copied:]
 assert output['stale_center']==case['stale_center'] and output['stale_radius']==case['stale_radius']
 if not copied:assert not any(SOURCE<=a<SOURCE+512 for a in reads)
 if not case['has_aux']:assert not any(SOURCE_AUX<=a<SOURCE_AUX+128 for a in reads)
 assert not any(Q+0x60<=a<Q+0x84 for a in reads)
 return output,m,stop

def sphere_case(words,case):
 m=Guarded(words)
 put(m,Q+0x88,[2,SHAPE]);put(m,Q+0xe0,[0x1c000]) # Opaque nonnull registration; not read.
 put(m,Q+0x60,case['initial_center']);put(m,Q+0x80,[case['initial_radius']])
 put(m,SHAPE,case['initial_world']);put(m,SHAPE+0x40,case['shape_metadata'])
 put(m,SHAPE+0x44,[SCENE]);put(m,SHAPE+0x4c,[TREE if case['has_tree'] else 0,PAIR])
 put(m,TREE+0xc,case['tree_center']+case['tree_half']);put(m,PAIR,case['pair'])
 put(m,SCENE+0x100,case['scene']);m.write(SCENE+0x190,case['scene_flags'],1)
 # The dirty/prepared gate is explicit, not synthesized. No parent/local matrix exists.
 assert case['scene_flags']&1
 for a,w in DISPATCH.items():m.write(a,w)
 m.g[4][0]=Q;stop=run_to_boundary(m,0x11ff60,0x11ff74);reads=m.read_addresses.copy()
 assert stop==0x11ff74 and 0x120230 not in m.coverage and 0x11ff74 not in m.coverage
 assert not any(Q+0xe0<=a<Q+0xe4 for a in reads)
 assert 0x2897b0 not in m.coverage and not any(SCENE+0x180<=a<SCENE+0x184 for a in reads)
 output=dict(world=get(m,SHAPE,16),center=get(m,Q+0x60,4),radius=m.read(Q+0x80),scene_flags=m.read(SCENE+0x190,1),
             metadata=[m.read(SHAPE+o) for o in (0x40,0x48,0x54)])
 assert output['world']==case['scene'] and output['center'][3]==bits(1.)
 assert output['metadata']==[case['shape_metadata'][i] for i in (0,2,5)]
 if case['has_tree']:assert not any(PAIR<=a<PAIR+32 for a in reads)
 else:assert not any(TREE<=a<TREE+36 for a in reads)
 return output,m,stop

def main():
 words=freeze('--frozen' in sys.argv);rng=random.Random(0x48304f);staging=[];sphere=[]
 coverage=set();branches={};steps=0
 def add(kind,case):
  nonlocal steps
  out,m,stop=(staging_case if kind=='staging' else sphere_case)(words,case)
  record=dict(name=case.pop('name'),input=case,output=out,instructions=m.steps,stop_pc=f'{stop:08X}',source_byte_reads=0)
  (staging if kind=='staging' else sphere).append(record);coverage.update(m.coverage);steps+=m.steps
  for pc,v in m.branches.items():branches.setdefault(pc,set()).update(v)
 payload=[0,0x80000000,0x7fc12345,0x7f812345,0x7f800000,0xff800000,0x00000001,0xdeadbeef]
 for linked in (False,True):
  for aux in (False,True):
   for count in (0,1,2,3,8,15,16):
    for tag in (2,3,0xffffffff):
     case=dict(name=f'copy-linked{int(linked)}-aux{int(aux)}-count{count}-tag{tag}',shape_linked=linked,has_aux=aux,count=count,tag=tag,
         source=[payload[i%8] if count%2 else rng.getrandbits(32) for i in range(128)],
         source_aux=[rng.getrandbits(32) for _ in range(32)],initial_dest=[rng.getrandbits(32) for _ in range(128)],
         initial_aux=[rng.getrandbits(32) for _ in range(32)],stale_center=[rng.getrandbits(32) for _ in range(4)],stale_radius=rng.getrandbits(32))
     add('staging',case)
 matrices={'identity':IDENTITY,'translated':[1,0,0,0,0,1,0,0,0,0,1,0,10,-20,30,1],
   'quarter-turn':[0,0,-1,0,0,1,0,0,1,0,0,0,10,-20,30,1],
   'nonorthogonal':[2,.25,-.5,0,1,3,.75,0,-.125,.5,.25,0,-10,20,-30,1],
   'zero-basis':[0]*12+[10,20,30,1],
   'signed-zero':[-0.,0,-0.,0,0,1,-0.,0,-0.,0,1,0,-0.,0,-0.,1]}
 shapes=[('vertical',[0,3,0,0],[0,-28,0,0],[2,3,4],[2,5,1]),
   ('zero-length',[2,-3,4,0],[2,-3,4,0],[2,-3,4],[0,0,0]),
   ('reversed',[7,-4,6,0],[-3,8,2,0],[-3,8,2],[5,6,2]),
   ('zero',[-0.,0,-0.,0],[0,-0.,0,0],[-0.,0,-0.],[0,-0.,0]),
   ('small',[2**-30,-2**-28,2**-26,0],[-2**-30,2**-28,-2**-26,0],[2**-28,0,2**-26],[2**-30,2**-28,2**-26])]
 for matrix_name,matrix in matrices.items():
  for shape_name,first,second,center,half in shapes:
   for tree in (False,True):
    for flags in (1,255):
     pair=fwords(first+second);pair[3]=0x7f812345;pair[7]=0xffc12345 # Explicit unconsumed lanes.
     add('sphere',dict(name=f'{matrix_name}-{shape_name}-tree{int(tree)}-flags{flags}',has_tree=tree,scene=fwords(matrix),scene_flags=flags,
       tree_center=fwords(center),tree_half=fwords(half),pair=pair,initial_world=[rng.getrandbits(32) for _ in range(16)],
       initial_center=[rng.getrandbits(32) for _ in range(4)],initial_radius=rng.getrandbits(32),shape_metadata=[0x8000,0,47,0,0,1]))
 for i in range(128):
  matrix=[rng.uniform(-2,2) if j%4!=3 else 0 for j in range(12)]+[rng.uniform(-1e4,1e4) for _ in range(3)]+[1]
  add('sphere',dict(name=f'deterministic-{i}',has_tree=bool(i%2),scene=fwords(matrix),scene_flags=1,
    tree_center=fwords([rng.uniform(-100,100) for _ in range(3)]),tree_half=fwords([rng.uniform(0,100) for _ in range(3)]),
    pair=fwords([rng.uniform(-100,100) for _ in range(8)]),initial_world=[rng.getrandbits(32) for _ in range(16)],
    initial_center=[rng.getrandbits(32) for _ in range(4)],initial_radius=rng.getrandbits(32),shape_metadata=[0x8000,0,i,0,0,1]))
 (OUT/'fixtures.json').write_text(json.dumps(dict(staging=staging,sphere=sphere),indent=2)+'\n')
 proof=dict(scope='Isolated original11CA80 natural return and type2 original11FF60 prefix ending before11FF74 registration.',
  source='Hash-verified immutable ELF instructions only; Original(data=b\'\'); explicit synthetic objects; no hooks or patched words.',
  model='Individually rounded ordinary host binary32; no physical EE exceptional-mode guarantee.',
  cases=len(staging)+len(sphere),staging_cases=len(staging),sphere_cases=len(sphere),instructions=steps,source_byte_reads=0,
  unique_words=len(coverage),executed_pcs=[f'{pc:08X}' for pc in sorted(coverage)],
  branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},
  original_elf_sha256=ELF_SHA,
  source_hashes={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in [Path(__file__),ROOT/'tools/gameplay/query_oracle48h.py',OUT/'original-words.json']},
  fixture_sha256=sha((OUT/'fixtures.json').read_bytes()))
 (OUT/'proof.json').write_text(json.dumps(proof,indent=2)+'\n')
 print(json.dumps({k:v for k,v in proof.items() if k not in ('executed_pcs','branches','source_hashes')}))
if __name__=='__main__':main()
