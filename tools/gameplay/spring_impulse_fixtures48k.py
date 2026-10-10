"""Original297610 differential fixtures for native impact response."""
from pathlib import Path
import hashlib,json,math,random,struct
from spring_fixture48j import SpringOriginal,ROOT
from query_oracle48h import bits,f32
OUT=ROOT/'evidence/spring48K'
OFFSETS=(0x70,0xb0,0xc0,0xd0,0x180,0x190,0x1a0,0x1b0,0x1c0,0x1d0)
OUTPUT_OFFSETS=(0x70,0xb0,0x180,0x190,0x1a0,0x1b0,0x1c0,0x1d0)
def body(o,b,s,values):
 assert len(values)==60
 o.write(b+0x288,s);o.write(s+0x190,1,1);o.vector(s+0x100,values[:16])
 o.vector(b+0x1e0,values[16:19]+[1]);o.storef(b+0x26c,values[19]);o.storef(b+0x270,values[20])
 for i in range(3):o.vector(b+0x140+i*16,values[21+i*3:24+i*3]+[0])
 for i,off in enumerate(OFFSETS):o.vector(b+off,values[30+i*3:33+i*3]+[1])
def initialize(o,inputs):
 assert len(inputs)==149
 normal=0x15000;point=normal+16;spring=0x10000
 o.vector(normal,inputs[:3]+[1]);o.vector(point,inputs[3:6]+[1]);o.vector(0x4435a0,[0,0,0,1])
 p=inputs[8:29];o.write(spring+0x30,0x11000)
 for i,off in enumerate((0x110,0x120,0x100)):o.vector(spring+off,p[i*3:i*3+3]+[1])
 for i,off in enumerate((0x98,0xb0,0xb4,0xb8,0xbc,0xc0,0xcc,0xd0)):o.storef(spring+off,p[9+i])
 for i,off in enumerate((0x5c,0x64,0x6c,0x70)):o.write(spring+off,int(p[17+i]))
 body(o,0x11000,0x13000,inputs[29:89]);body(o,0x12000,0x14000,inputs[89:149])
 o.g[4][0]=spring;o.g[5][0]=normal;o.g[6][0]=point;o.g[7][0]=0x12000 if inputs[7] else 0;o.f[12]=inputs[6]
def result(o,other):
 values=[]
 for b in (0x11000,0x12000):
  if b==0x12000 and not other:values.extend([0]*24);continue
  for off in OUTPUT_OFFSETS:values.extend(o.vec(b+off))
 return values
def main():
 OUT.mkdir(exist_ok=True)
 original=ROOT/'evidence/spring48J/original-words.json'
 assert hashlib.sha256(original.read_bytes()).hexdigest()=='b447e613b37c5ef0bbb503f4c084fbfc30a9715ce4fe5cb0db2d6c1ea9fc258e'
 words={r['pc']:r['word'] for block in json.loads(original.read_text())['blocks'] for r in block['instructions']}
 rng=random.Random(0x48b297610);records=[];coverage=set();branches={};steps=0
 def vec(scale=1):return [f32(rng.uniform(-scale,scale)) for _ in range(3)]
 def makebody(i):
  angle=rng.uniform(-3,3);c=f32(math.cos(angle));s=f32(math.sin(angle))
  m=[c,0,-s,0,0,1,0,0,s,0,c,0]+vec(20)+[1]
  if i%4==0:
   for j in (0,1,2,4,5,6,8,9,10):m[j]=f32(m[j]+rng.uniform(-.1,.1))
  mass=f32(rng.uniform(100,2000))
  if i%16 in (0,1,2):mass=f32(1000+(i%16-1)*.0001)
  inertia=[f32(rng.uniform(.001,.02)) if row==col else 0 for col in range(3) for row in range(3)]
  corrections=([0]*6 if i%3==0 else vec(.1)+vec(.01))
  return m+vec(2)+[mass,f32(1/mass)]+inertia+vec(20)+vec(2)+vec(10)+vec(1)+vec(10000)+vec(50)+corrections+vec(1)+vec(.1)
 for i in range(1024):
  n=vec();length=math.sqrt(sum(x*x for x in n));n=[f32(x/length) for x in n]
  if i%8==0:n=[0,1,0]
  if i%8 in (1,2,3):n=[.6,f32(.8+(i%8-2)*.0000001),0]
  point=[0,0,0] if i%16==0 else vec(20)
  penetration=f32(rng.uniform(-.5,1));other=(i>>4)&1
  p=vec(20)+vec(25)+vec(2)+[f32(rng.uniform(.1,2)) for _ in range(8)]+[i&1,(i>>1)&1,(i>>2)&1,(i>>3)&1]
  a=makebody(i);b=makebody(i+1)
  inputs=n+point+[penetration,other]+p+a+b
  o=SpringOriginal(words=words);initialize(o,inputs);assert o.run(0x297610)==1
  values=result(o,other);assert all(math.isfinite(v) for v in values)
  assert not o.source_reads
  records.append([bits(v) for v in inputs+values]);steps+=o.steps;coverage.update(o.coverage)
  for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
 raw=struct.pack('<II',len(records),197)+b''.join(struct.pack('<197I',*r) for r in records)
 (OUT/'impact-fixtures.bin').write_bytes(raw)
 report=dict(cases=len(records),input_words=149,output_words=48,steps=steps,source_reads=0,
  original_words_sha256=hashlib.sha256(original.read_bytes()).hexdigest(),fixture_sha256=hashlib.sha256(raw).hexdigest(),
  arithmetic='Ordinary individually rounded IEEE binary32 original-word execution; current world scenes, no physical EE exceptional-mode claim',
  coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
 (OUT/'impact-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
