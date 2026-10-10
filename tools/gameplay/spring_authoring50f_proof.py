"""Bounded original setter proof; no game/RAM or native-derived expectations."""
from pathlib import Path
import struct,hashlib,json,random,sys
sys.dont_write_bytecode=True
from query_oracle48h import Original,bits,fp,f32,signed
class SetterOriginal(Original):
 def execute(self,pc):
  w=self.words[pc]
  if w>>26==1 and ((w>>16)&31)==0:
   self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
   taken=signed(self.g[(w>>21)&31][0])<0
   self.branches.setdefault(pc,set()).add(taken)
   immediate=w&65535;immediate-=65536 if immediate&32768 else 0
   return pc+4+immediate*4 if taken else pc+8
  return super().execute(pc)
ROOT=Path(__file__).resolve().parents[2]
elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
assert hashlib.sha256(elf).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
words={pc:struct.unpack_from('<I',elf,pc-0x100000+0x80)[0] for pc in range(0x295c90,0x295df8,4)}
keys=[(0x3b2828,'MaxCompression',0x21590c),(0x3b1b50,'Spring',0x215920),(0x3b1b58,'Damping',0x215934),(0x3b2838,'ImpulseSpring',0x215948),(0x3b2848,'ImpulseDamping',0x21595c),(0x3b2858,'Friction',0x215970),(0x3b2870,'WallRestitution',0x215984),(0x3b2880,'PodRestitution',0x215998),(0x3b2890,'MomentScale',0x2159b0),(0x3b28a0,'AngularImpulseScale',0x2159c8),(0x3b28c0,'FrictionTorqueScale',0x2159e0)]
for address,key,pc in keys:
 off=address-0x100000+0x80;assert elf[off:elf.index(0,off)].decode()==key
 assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==0x0c05629c
rng=random.Random(0x50f);cases=[];coverage=set();steps=0
for mass in [0.,-1.,.001,1.,300.,1000.]:
 for index in range(12):
  values=[f32(rng.uniform(-2,3)) for _ in range(11)]
  if index==0:values=[1,0,0,1,f32(.3),f32(.1),0,0,0,0,0]
  if index%3==1:values[1]=values[3]=0.
  machine=SetterOriginal(words=words);spring,body=0x10000,0x20000
  for off in range(0,0x140,4):machine.write(spring+off,0x3f000000+off)
  machine.write(spring+0x30,body);machine.storef(body+0x26c,mass)
  machine.g[4][0]=spring
  for reg,value in zip(range(12,20),[values[0],values[1],values[2],values[3],values[4],values[6],values[6],values[7]]):machine.f[reg]=value
  machine.run(0x295c90)
  machine.g[4][0]=spring;machine.g[5][0]=0;machine.f[12]=values[5];machine.run(0x295dd0)
  expected={str(off):machine.read(spring+off) for off in [0x90,0x9c,0xa0,0xa4,0xa8,0xb8,0xbc,0xc0,0xd8]}
  cases.append({'mass':bits(mass),'values':list(map(bits,values)),'expected':expected});coverage.update(machine.coverage);steps+=machine.steps
out=ROOT/'evidence/springAuthoring50F';out.mkdir(exist_ok=True)
(out/'setter-fixtures.json').write_text(json.dumps({'cases':cases,'executed_words':steps,'source_words':{hex(pc):hex(words[pc]) for pc in sorted(coverage)},'keys':[{'address':hex(a),'key':k,'reader_call':hex(pc)}for a,k,pc in keys]},indent=2)+'\n')
print(len(cases),steps,len(coverage))
