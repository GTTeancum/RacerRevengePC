"""Preserve original spring contact impulse/friction instructions and anchors."""
from pathlib import Path
import hashlib,json,re,struct
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/spring48J'
RANGES=[('friction',0x297390,0x297610),('impulse',0x297610,0x2983b4),
 ('point_force',0x296c10,0x296f10),('body_linear_correction',0x165580,0x165740),
 ('body_angular_correction',0x165740,0x165900),('matrix_vector',0x127790,0x1277d0),
 ('world_direction',0x28b9b0,0x28baa8),('local_direction',0x28bbb0,0x28bd48),
 ('scene_world_refresh',0x2897b0,0x289940),('scene_world_compose',0x289940,0x2899b8),
 ('dot',0x106e70,0x106e98),('make_vector',0x106790,0x1067ac)]
def main():
 OUT.mkdir(exist_ok=True)
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 assert hashlib.sha256(elf).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
 comments={};sources=[]
 for p in (ROOT/'work/native/project/generated').glob('*_0x*.cpp'):
  start=int(p.stem.split('_0x')[-1],16)
  if not any(lo<=start<hi for _,lo,hi in RANGES):continue
  data=p.read_bytes()
  for line,text in enumerate(data.decode().splitlines(),1):
   m=re.search(r'// 0x([0-9a-f]+): 0x([0-9a-f]+)\s+(.*)',text)
   if m and any(lo<=int(m[1],16)<hi for _,lo,hi in RANGES):
    pc=int(m[1],16);w=int(m[2],16)
    assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==w
    comments[pc]=dict(pc=pc,word=w,assembly=m[3],source=p.relative_to(ROOT).as_posix(),line=line)
  sources.append(dict(path=p.relative_to(ROOT).as_posix(),sha256=hashlib.sha256(data).hexdigest()))
 proof=[]
 for name,lo,hi in RANGES:
  raw=elf[lo-0x100000+0x80:hi-0x100000+0x80]
  (OUT/(name+'.bin')).write_bytes(raw)
  rows=[comments.get(pc,dict(pc=pc,word=struct.unpack_from('<I',raw,pc-lo)[0],assembly='[no annotation]')) for pc in range(lo,hi,4)]
  (OUT/(name+'.txt')).write_text('\n'.join(f"{r['pc']:08X} {r['word']:08X} {r['assembly']}" for r in rows)+'\n')
  proof.append(dict(name=name,start=lo,end=hi,sha256=hashlib.sha256(raw).hexdigest(),instructions=rows))
 (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(elf).hexdigest(),blocks=proof,sources=sources),indent=2)+'\n')
 print(json.dumps(dict(words=sum((b-a)//4 for _,a,b in RANGES),calls=sorted(set(r['assembly'] for r in comments.values() if r['assembly'].startswith(('jal','jalr')))))))
if __name__=='__main__':main()
