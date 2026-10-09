"""Original-word raw copy-only fixtures for the canonical frame consumer join."""
from pathlib import Path
import hashlib,json,struct
from impact_consumers_recover49z import ConsumerOriginal,OFFSETS,ELF_SHA
from query_oracle48h import fp
ROOT=Path(__file__).resolve().parents[2];out=ROOT/'evidence/frameImpact49Y'
data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert hashlib.sha256(data).hexdigest()==ELF_SHA
frozen=json.loads((ROOT/'work/local49Z/impact-consumers/original-words.json').read_text());words={}
phoff=struct.unpack_from('<I',data,28)[0];size,count=struct.unpack_from('<HH',data,42);segments=[struct.unpack_from('<8I',data,phoff+j*size) for j in range(count)]
for block in frozen['blocks']:
 for row in block['instructions']:
  pc,w=row['pc'],row['word'];segment=next(s for s in segments if s[0]==1 and s[2]<=pc<pc+4<=s[2]+s[4]);assert struct.unpack_from('<I',data,segment[1]+pc-segment[2])[0]==w;words[pc]=w
class TracedConsumerOriginal(ConsumerOriginal):
 def __init__(self,words):
  super().__init__(words);self.execution=[];self.exit_targets=[]
 def execute(self,pc):
  target=super().execute(pc);self.execution.append(dict(pc=pc,word=words[pc],target=target,annul=self.annul))
  if target in (0x228b40,0x2288bc):self.exit_targets.append(target)
  return target
traces=[]
records=[];coverage=set();patterns=[0x7f800001,0xff800001,0x7fc12345,0xffc54321,0x7f800000,0xff800000,1,0x80000001,0x007fffff,0x807fffff,0,0x80000000]
for pattern in patterns:
 for gate in range(2):
  ins=[0 if gate==0 else 2,gate,0,0,0,0,0,15,2,0,0,pattern,0x3f800000,0];state=[pattern]*29
  o=TracedConsumerOriginal(words);vehicle=0x10000;partner=0x12000;game=0x14000;manager=0x15000;tuning=0x16000;profile=0x30000
  for off,x in zip(OFFSETS,state):o.write(vehicle+off,x)
  for off,x in zip([0xcd8,0x3c,0x40,0x58,0x64,0x5c],ins[1:7]):o.write(vehicle+off,x,1 if off==0xcd8 else 4)
  for j in range(4):o.write(vehicle+0x1b8+j*4,1)
  o.write(vehicle+0xd34,partner);o.write(0x382b80,game);o.write(game+0x28c,manager);o.write(manager+0xc0,tuning);o.write(manager+0xb0,0x19000);o.write(manager+0xb4,0x17000);o.write(0x17058,ins[0],1)
  o.write(game+0x2dc,profile);o.write(profile+0x34,2);o.write(profile+0x10fac,0);o.write(profile+0x11050,0,1)
  o.write(tuning+0x64,ins[12]);o.write(tuning+0x68,ins[13]);o.storef(tuning+0x48,7.)
  o.g[16][0]=vehicle;o.f[20]=fp(ins[11]);status=o.run_consumer(0);assert status==1 and not o.source_reads and not o.events
  result=[o.read(vehicle+off) for off in OFFSETS];assert result==state
  assert o.exit_targets==[0x228b40 if gate==0 else 0x2288bc]
  traces.append(dict(pattern=hex(pattern),managerFlags58=ins[0],flagsCD8=ins[1],sourceExit=hex(o.exit_targets[0]),source223B20Called=0x222b20 in o.coverage,execution=o.execution,input_words=ins,state_words=state,output_words=result))
  records.append(ins+state+[status]+result+[0]+[0]*12);coverage.update(o.coverage)
raw=struct.pack('<II',len(records),86)+b''.join(struct.pack('<86I',*r) for r in records);(out/'consumer-copy-only.bin').write_bytes(raw)
(out/'consumer-copy-only.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,cases=len(records),fixture_sha256=hashlib.sha256(raw).hexdigest(),coverage=[hex(p) for p in sorted(coverage)],source_reads=0,scope='Actual original manager/CD8 gates; all unconsumed state words are arbitrary raw payloads. No native output oracle.'),indent=2)+'\n')
print(len(records),'original raw-copy cases',hashlib.sha256(raw).hexdigest())

(out/'consumer-dispatch-exits.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,cases=traces,scope='One original memory/register context per case. Actual2288A8 branch versus222B20 return, including executed delay slots. No substituted gameplay call.'),indent=2)+'\n')
(out/'consumer_dispatch49z.h').write_text('#pragma once\n#include <cstdint>\nnamespace consumer_dispatch49z {\ninline constexpr std::uint32_t managerSkipExit=0x%08xu;\ninline constexpr std::uint32_t leafSkipExit=0x%08xu;\n}\n'%(int(traces[0]['sourceExit'],16),int(traces[1]['sourceExit'],16)))
