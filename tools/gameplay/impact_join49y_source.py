"""Authenticate joined caller/impact/body/post original bytes; no native output oracle."""
from pathlib import Path
import hashlib,json,re,struct
ROOT=Path(__file__).resolve().parents[2]
out=ROOT/'evidence/frameImpact49Y'
data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
sha=hashlib.sha256(data).hexdigest()
assert sha=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
phoff=struct.unpack_from('<I',data,28)[0]; phsize,phcount=struct.unpack_from('<HH',data,42)
segments=[struct.unpack_from('<8I',data,phoff+i*phsize) for i in range(phcount)]
base=ROOT/'evidence/frameBridge49W/body/source/body_words.h'
text=base.read_text()
blocks=[(name,int(start,16),int(count)) for name,start,count in re.findall(r'\{"([^"]+)",0x([0-9a-f]+)u,(\d+),',text)]
blocks += [('impact_dispatch',0x219d1c,(0x219ea4-0x219d1c)//4),('impact',0x223030,(0x2234dc-0x223030)//4),('static_helper',0x222770,(0x2228c8-0x222770)//4),('post_clear',0x165420,3)]
lines=['#pragma once','#include <cstdint>','namespace impact49y_words {']
meta=[]; rows=[]; index=0
for name,start,count in blocks:
 end=start+count*4
 seg=next(s for s in segments if s[0]==1 and s[2]<=start<end<=s[2]+s[4])
 raw=data[seg[1]+start-seg[2]:seg[1]+end-seg[2]]
 words=struct.unpack('<%dI'%count,raw)
 inherited=re.search(r'uint32_t '+name+r'\[\]=\{(.*?)\};',text,re.S)
 if inherited: assert list(words)==[int(x,16) for x in re.findall(r'0x([0-9a-f]+)u',inherited.group(1))]
 lines.append('inline constexpr std::uint32_t '+name+'[]={')
 lines.extend(' '+','.join('0x%08xu'%w for w in words[i:i+8])+',' for i in range(0,count,8));lines.append('};')
 rows.append('{"%s",0x%xu,%d,%d,%s},'%(name,start,count,index,name));index+=count
 meta.append(dict(name=name,start=start,end=end,sha256=hashlib.sha256(raw).hexdigest()))
lines+=['struct Block {const char* name;std::uint32_t start,count,index;const std::uint32_t* words;};','inline constexpr Block blocks[]={']+rows+['};','inline constexpr std::uint32_t total_words=%d;'%index,'}']
(out/'impact_join49y_words.h').write_text('\n'.join(lines)+'\n')
machine=ROOT/'evidence/frameBridge49W/body/source/body_machine.h'
s=machine.read_text().replace('"body_words.h"','"impact_join49y_words.h"').replace('../../../../work/','../../work/').replace('graph49w_original','impact49y_original').replace('graph49w_words','impact49y_words')
(out/'impact_join49y_machine.h').write_text(s)
(out/'source.json').write_text(json.dumps(dict(elf_sha256=sha,blocks=meta,inherited_machine_sha256=hashlib.sha256(machine.read_bytes()).hexdigest(),inherited_words_sha256=hashlib.sha256(base.read_bytes()).hexdigest(),machine_changes='Only include paths and namespace identifiers; decoder semantics unchanged.'),indent=2)+'\n')
print('Authenticated',index,'original words')

