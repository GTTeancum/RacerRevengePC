"""Offline original297390/296C10 fixtures; never linked into production."""
from pathlib import Path
import hashlib,json,random,struct,math
from query_oracle48h import Original,bits,fp,f32,signed
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/spring48J'
class SpringOriginal(Original):
 def execute(self,pc):
  w=self.words[pc]
  if w>>26==0x11 and (w>>21)&31==16 and w&63 in (0x28,0x29):
   self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
   x=self.f[(w>>11)&31];y=self.f[(w>>16)&31]
   # Scalar EE MIN.S/MAX.S orders signed words, reversing two negatives.
   # In particular MIN(-0,+0)=-0 and MAX(+0,-0)=+0. The pre-48O
   # float-comparison extension is archived in evidence/minmaxAudit48O.
   a,b=signed(bits(x)),signed(bits(y))
   integer_max=(w&63==0x28)!=(a<0 and b<0)
   self.f[(w>>6)&31]=fp((max(a,b) if integer_max else min(a,b))&0xffffffff)
   return None
  return super().execute(pc)
def main():
 proof=json.loads((OUT/'original-words.json').read_text())
 words={r['pc']:r['word'] for b in proof['blocks'] for r in b['instructions']}
 ram_path=ROOT/'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin'
 ram=ram_path.read_bytes();ram_sha=hashlib.sha256(ram).hexdigest()
 assert ram_sha=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
 sentinel=ram[0x4435a0:0x4435b0];assert sentinel==struct.pack('<4f',0,0,0,1)
 (OUT/'zero-point-sentinel.bin').write_bytes(sentinel)
 rng=random.Random(0x48a297390);coverage=set();branches={};steps=0;records=[]
 def vec(scale=1):return [f32(rng.uniform(-scale,scale)) for _ in range(3)]
 def matrix(i):
  if i%4==0:return [1,0,0,0,0,1,0,0,0,0,1,0]+vec(100)+[1]
  angle=rng.uniform(-3,3);c=f32(math.cos(angle));s=f32(math.sin(angle))
  m=[c,0,-s,0,0,1,0,0,s,0,c,0]+vec(100)+[1]
  if i%4==3:
   for j in (0,1,2,4,5,6,8,9,10):m[j]=f32(m[j]+rng.uniform(-.1,.1))
  return m
 for i in range(1024):
  n=vec();length=math.sqrt(sum(x*x for x in n));n=[f32(x/length) for x in n]
  point=vec(100);velocity=vec(100)
  if i%16==0:point=[0,0,0]
  if i%16==1:n=[0,1,0];velocity=[0,f32(rng.uniform(-20,20)),0]
  if i%16 in (2,3,4):n=[0,1,0];velocity=[fp(0x41200000+i%16-3),0,0]
  coefficient=f32(rng.uniform(0,1));torque=f32(rng.uniform(-2,2))
  if i%16==5:coefficient=0
  custom=vec(5);center=vec(5);flags=[i&1,(i>>1)&1];mass=f32(rng.uniform(0,2000))
  pm=matrix(i);om=matrix(i+1);pl=vec(100)+vec(50);ol=vec(100)+vec(50);other=(i>>2)&1
  inputs=n+point+velocity+[coefficient,torque]+custom+center+flags+[mass]+pm+om+pl+ol+[other]
  assert len(inputs)==65
  o=SpringOriginal(words=words)
  spring=0x10000;body=0x11000;second=0x12000;scene=0x13000;scene2=0x14000;normal=0x15000;contact=0x15010
  o.write(spring+0x30,body);o.write(spring+0x70,flags[0]);o.write(spring+0x6c,flags[1])
  o.vector(spring+0x100,custom+[1]);o.vector(spring+0x120,velocity+[1]);o.storef(spring+0xc4,torque)
  for b,s,m,l in ((body,scene,pm,pl),(second,scene2,om,ol)):
   o.write(b+0x288,s);o.vector(s+0x100,m);o.vector(b+0x180,l[:3]+[1]);o.vector(b+0x190,l[3:]+[1])
  o.vector(body+0x1e0,center+[1]);o.storef(body+0x26c,mass)
  o.vector(normal,n+[1]);o.vector(contact,point+[1]);o.vector(0x4435a0,[0,0,0,1])
  o.g[4][0]=spring;o.g[5][0]=normal;o.g[6][0]=contact;o.g[7][0]=second if other else 0;o.f[12]=coefficient
  o.run(0x297390)
  result=o.vec(0x1ff0000-0x100+0x90)+o.vec(body+0x180)+o.vec(body+0x190)
  result+=(o.vec(second+0x180)+o.vec(second+0x190)) if other else [0]*6
  assert all(math.isfinite(x) for x in result)
  assert not o.source_reads # Every data byte explicitly initialized or written by original instructions.
  records.append([bits(x) for x in inputs+result]);coverage.update(o.coverage);steps+=o.steps
  for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
 raw=struct.pack('<II',len(records),80)+b''.join(struct.pack('<80I',*r) for r in records)
 (OUT/'friction-fixtures.bin').write_bytes(raw)
 report=dict(cases=len(records),words_per_case=80,steps=steps,fixture_sha256=hashlib.sha256(raw).hexdigest(),
  ram_sha256=ram_sha,sentinel_address='004435A0',sentinel_sha256=hashlib.sha256(sentinel).hexdigest(),
  original_words_sha256=hashlib.sha256((OUT/'original-words.json').read_bytes()).hexdigest(),
  model='Original instructions; individually rounded IEEE binary32 ordinary finite arithmetic, not physical EE exceptional modes',
  source_reads=0,coverage=[f'{pc:08X}' for pc in sorted(coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())})
 (OUT/'friction-fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
