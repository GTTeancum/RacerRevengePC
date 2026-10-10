"""Saved-state original normal-response replay; recovery evidence, not native code."""
from pathlib import Path
import hashlib,json,struct,itertools
from spring_fixture48j import SpringOriginal,ROOT,OUT
from query_oracle48h import bits
FIELDS={'worldVelocity':0x70,'localAngularVelocity':0xb0,'force':0x180,'torque':0x190,
 'positionCorrection':0x1a0,'rotationCorrection':0x1b0,'velocityChange':0x1c0,'angularVelocityChange':0x1d0}
def main():
 ram=(ROOT/'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin').read_bytes()
 ram_sha=hashlib.sha256(ram).hexdigest();assert ram_sha=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
 words={r['pc']:r['word'] for b in json.loads((OUT/'original-words.json').read_text())['blocks'] for r in b['instructions']}
 rows=[];reads=set()
 for other in (0,0xd117b0):
  for penetration,approaching in itertools.product((0.,.5),(False,True)):
   o=SpringOriginal(ram,words)
   normal=0x1fe0000;point=normal+16
   for i,w in enumerate((0x3bcfb961,0x3f785cd8,0xbe782c43,0x3f800000,0x459b9b53,0x45048912,0x4647428c,0x3f800000)):o.write(normal+i*4,w)
   o.g[4][0]=0x987cd0;o.g[5][0]=normal;o.g[6][0]=point;o.g[7][0]=other;o.f[12]=penetration
   if approaching:o.vector(0x987cd0+0x120,[0,-25,0,1])
   before={f'{b:08X}':{n:list(struct.unpack_from('<3I',ram,b+off)) for n,off in FIELDS.items()} for b in (0xd110a0,0xd117b0)}
   result=o.run(0x297610)
   after={f'{b:08X}':{n:[o.read(b+off+4*i) for i in range(3)] for n,off in FIELDS.items()} for b in (0xd110a0,0xd117b0)}
   reads.update(o.source_reads)
   rows.append(dict(other=other,penetration_bits=bits(penetration),approaching_velocity_override=approaching,return_value=result,steps=o.steps,before=before,after=after,
    coverage=[f'{pc:08X}' for pc in sorted(o.coverage)],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(o.branches.items())}))
 slices=[]
 for address in sorted(reads):
  if not slices or address!=slices[-1][-1]+1:slices.append([address])
  else:slices[-1].append(address)
 payload=b'';layout=[]
 for chunk in slices:
  start=chunk[0];data=ram[start:start+len(chunk)];layout.append(dict(address=start,offset=len(payload),size=len(data)));payload+=data
 (OUT/'impulse-saved-data.bin').write_bytes(payload)
 # Independently rerun with only preserved source bytes permitted.
 sparse=bytearray(0x2000000)
 for r in layout:sparse[r['address']:r['address']+r['size']]=payload[r['offset']:r['offset']+r['size']]
 for row in rows:
  o=SpringOriginal(bytes(sparse),words);o.allowed_source_addresses=reads
  for i,w in enumerate((0x3bcfb961,0x3f785cd8,0xbe782c43,0x3f800000,0x459b9b53,0x45048912,0x4647428c,0x3f800000)):o.write(0x1fe0000+i*4,w)
  o.g[4][0]=0x987cd0;o.g[5][0]=0x1fe0000;o.g[6][0]=0x1fe0010;o.g[7][0]=row['other'];o.f[12]=struct.unpack('<f',struct.pack('<I',row['penetration_bits']))[0]
  if row['approaching_velocity_override']:o.vector(0x987cd0+0x120,[0,-25,0,1])
  assert o.run(0x297610)==row['return_value'] and o.steps==row['steps']
  for b,fields in row['after'].items():
   for name,expected in fields.items():assert [o.read(int(b,16)+FIELDS[name]+i*4) for i in range(3)]==expected
 report=dict(ram_sha256=ram_sha,original_words_sha256=hashlib.sha256((OUT/'original-words.json').read_bytes()).hexdigest(),
  sparse_sha256=hashlib.sha256(payload).hexdigest(),sparse_bytes=len(payload),slices=layout,cases=rows,sparse_replay_passed=True,
  scope='Original297610 and reachable helper recovery over saved body state; contact point/normal supplied from48I floor query, four cases override contact velocity to (0,-25,0). No claim caller chose these parameters or native impulse implemented.')
 (OUT/'impulse-saved-replay.json').write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps(dict(cases=len(rows),steps=[r['steps'] for r in rows],sparse_bytes=len(payload),slices=len(layout),sparse_replay_passed=True)))
if __name__=='__main__':main()
