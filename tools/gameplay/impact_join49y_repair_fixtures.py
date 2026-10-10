"""Source-executed raw-copy and timer cases for frame repair50A."""
from pathlib import Path
import hashlib,json,struct
from spring_fixture48j import SpringOriginal
from query_oracle48h import fp
ROOT=Path(__file__).resolve().parents[2];out=ROOT/'evidence/frameImpact49Y'
data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();sha=hashlib.sha256(data).hexdigest();assert sha=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
manifest=json.loads((ROOT/'work/local50A/impact-aftermath/original-words.json').read_text());words={}
for b in manifest['blocks']:
 for r in b['instructions']:
  pc,w=r['pc'],r['word'];assert struct.unpack_from('<I',data,pc-0x100000+0x80)[0]==w;words[pc]=w
patterns=[0x7f800001,0xff800001,0x7fc12345,0xffc12345,0x7f800000,0xff800000,1,0x80000001,0x007fffff,0x807fffff,0,0x80000000]
records=[];coverage=set()
for w in patterns:
 for timed in range(2):
  ins=[0x82 if timed else 0x80,15,0x3c888889 if timed else w,w];state=[w]*7
  if timed:state[4]=0x3f000000
  o=SpringOriginal(words=words);v=0x10000;o.write(v+0xb9c,ins[0],1)
  for j in range(4):o.write(v+0x1b8+4*j,1)
  o.write(v+0xcf4,ins[3]);o.f[12]=fp(ins[2]);o.g[4][0]=v
  offsets=[0x7d0,0x7d4,0x7d8,0x7dc,0x7e4,0xd38,0xd3c]
  for off,word in zip(offsets,state):o.write(v+off,word)
  o.run(0x224370);assert not o.source_reads;result=[o.read(v+off) for off in offsets]
  # Only the real timer subtract may touch the raw-copy cases.
  for j in [0,1,2,3,5,6]:assert result[j]==state[j]
  if not timed:assert result==state
  records.append(ins+state+[0 if timed else 1]+result);coverage.update(o.coverage)
raw=struct.pack('<II',len(records),19)+b''.join(struct.pack('<19I',*r) for r in records);(out/'repair-copy-only.bin').write_bytes(raw)
(out/'repair-copy-only.json').write_text(json.dumps(dict(elf_sha256=sha,cases=len(records),sha256=hashlib.sha256(raw).hexdigest(),coverage=[hex(p) for p in sorted(coverage)],source_reads=0),indent=2)+'\n');print(len(records),hashlib.sha256(raw).hexdigest())
