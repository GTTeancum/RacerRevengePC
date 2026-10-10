"""Preserve the original face-normal packing loop and PCSX2 conversion policy."""
from pathlib import Path
import hashlib,json,struct
ROOT=Path(__file__).resolve().parents[2];out=ROOT/'evidence/collisionRuntime48H';out.mkdir(exist_ok=True)
elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
assert hashlib.sha256(elf).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
start,end=0x126320,0x126378;data=elf[start-0x100000+0x80:end-0x100000+0x80]
proof=json.loads((ROOT/'evidence/importCollision48G/original-loader-proof.json').read_text())
instructions={int(i['address'],16):i for g in proof['groups'] for i in g['instructions'] if start<=int(i['address'],16)<end}
for a,i in instructions.items():assert int(i['word'],16)==struct.unpack_from('<I',data,a-start)[0]
(out/'normal-pack.bin').write_bytes(data)
(out/'normal-pack.txt').write_text('\n'.join(f'{a:08x} {i["word"]} {i["disassembly"]}' for a,i in sorted(instructions.items()))+'\n')
words=struct.unpack('<'+'I'*(len(data)//4),data)
(ROOT/'tools/assets/collision_runtime_words48h.h').write_text('#pragma once\n#include <cstdint>\ninline constexpr std::uint32_t rr_normal_pack48h[]={'+','.join(hex(x)+'u' for x in words)+'};\n')
p=ROOT/'work/reference/pcsx2-master/pcsx2/FPU.cpp';text=p.read_text();excerpt=text[text.index('void CVT_W()'):text.index('void DIV_S()')]
(out/'pcsx2-cvt-w.txt').write_text(excerpt)
(out/'recovery.json').write_text(json.dumps(dict(start=hex(start),end_exclusive=hex(end),sha256=hashlib.sha256(data).hexdigest(),
    words=len(words),matched_original_comments=len(instructions),pcsx2_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),
    scope='Face normals only: optional vertex normals are read as float3 at125FC4..126008; no native query or loader integration claimed.'),indent=2)+'\n')
