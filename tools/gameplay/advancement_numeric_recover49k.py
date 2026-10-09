#!/usr/bin/env python3
"""Freeze selected 49K source words directly from the hash-verified original ELF.
No RAM, reconstructed sources, legacy recovery invocation, installs, or runtime.
"""
import argparse, hashlib, json, struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
DEST=ROOT/'evidence/bodyAdvancement49K/numeric-source'
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={
 'scheduler':(0x1a8384,0x1a83b4),
 'gravity_incoming':(0x21990c,0x219964),
 'gravity_numeric':(0x2199c0,0x219a80),
 'gravity_setter':(0x1143e0,0x1143e8),
 'engine_flags':(0x219a90,0x219ad4),
 'probe_prepare_entry':(0x1b1800,0x1b1898),
 'probe_prepare_second_null':(0x1b18b4,0x1b18c0),
 'probe_prepare_disabled':(0x1b1904,0x1b1910),
 'probe_prepare_return':(0x1b1a14,0x1b1a2c),
 'probe_response_clear':(0x1b1aac,0x1b1adc),
 'probe_response_unhit':(0x1b1b4c,0x1b1b70),
 'probe_response_pending':(0x1b1e28,0x1b1e38),
 'probe_response_inactive':(0x1b1ebc,0x1b1ec8),
 'probe_response_final_gate':(0x1b2330,0x1b233c),
 'probe_response_airborne':(0x1b23c0,0x1b2478),
 'main_post':(0x165420,0x165428),
 'late_feedback':(0x219f90,0x219fd0),
 'vehicle380_feedback':(0x219ea4,0x219ec0),
}
CONTEXT={
 'gravity_initializer_and_scene_boundary':(0x219964,0x2199c0),
 'gravity_dispatch':(0x219a80,0x219a90),
 'probe_response_ready_scene_boundary':(0x1b1a30,0x1b1aac),
 'airborne_effective_global_initializer':(0x1b239c,0x1b23c0),
 'engine_application_selector':(0x219c98,0x219cc8),
 'impact_accumulator_gate':(0x219d1c,0x219da0),
}
REG='zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()
def decode(pc,w):
 op=w>>26; rs=(w>>21)&31;rt=(w>>16)&31;rd=(w>>11)&31;sa=(w>>6)&31;fn=w&63;imm=(w&65535)-(65536 if w&32768 else 0)
 if not w:return 'nop'
 if op in (4,5):return f'{"beq" if op==4 else "bne"} {REG[rs]}, {REG[rt]}, {pc+4+imm*4:#x}'
 if op in (9,25):return f'{"addiu" if op==9 else "daddiu"} {REG[rt]}, {REG[rs]}, {imm}'
 if op==15:return f'lui {REG[rt]}, {w&65535:#x}'
 if op in (12,13):return f'{"andi" if op==12 else "ori"} {REG[rt]}, {REG[rs]}, {w&65535:#x}'
 if op in (0x20,0x23,0x24,0x28,0x2b,0x1e,0x1f,0x31,0x39,0x36,0x3e):
  name={0x20:'lb',0x23:'lw',0x24:'lbu',0x28:'sb',0x2b:'sw',0x1e:'lq',0x1f:'sq',0x31:'lwc1',0x39:'swc1',0x36:'lqc2',0x3e:'sqc2'}[op]
  reg=('f'+str(rt)) if op in(0x31,0x39) else ('vf'+str(rt)) if op in(0x36,0x3e) else REG[rt]
  return f'{name} {reg}, {imm:#x}({REG[rs]})'
 if op==3:return f'jal {(w&0x3ffffff)*4:#x}'
 if op==0 and fn in(8,9):return f'{"jr" if fn==8 else "jalr"} {REG[rs]}'
 if op==0 and fn in(60,62):return f'{"dsll32" if fn==60 else "dsrl32"} {REG[rd]}, {REG[rt]}, {sa}'
 if (w&0xfc0007ff)==0x70000628:return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
 if op==17:
  if rs in(0,4):return f'{"mfc1" if rs==0 else "mtc1"} {REG[rt]}, f{rd}'
  if rs==8:return f'bc1{"t" if rt&1 else "f"} {pc+4+imm*4:#x}'
  if rs==20:return f'cvt.s.w f{sa}, f{rd}'
  names={0:'add.s',1:'sub.s',2:'mul.s',3:'div.s',6:'mov.s',36:'cvt.w.s',50:'c.eq.s',52:'c.lt.s',54:'c.le.s'}
  if fn in names:return f'{names[fn]} f{sa}, f{rd}, f{rt}'
 if op==18:
  return {0x4a20025b:'vmulw.w vf9,vf0,vf0',0x4bc821bc:'vmulax.xyz ACC,vf4,vf8',0x4bc828bd:'vmadday.xyz ACC,vf5,vf8',0x4bc8324a:'vmaddz.xyz vf9,vf6,vf8',0x4bc74a68:'vadd.xyz vf9,vf9,vf7'}.get(w,f'VU raw {w:08x}')
 return f'raw {w:08x}'
def generate():
 data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert hashlib.sha256(data).hexdigest()==SHA
 phoff=struct.unpack_from('<I',data,28)[0];ps,pn=struct.unpack_from('<HH',data,42)
 segments=[struct.unpack_from('<8I',data,phoff+i*ps) for i in range(pn)]
 def extract(a,z):
  for typ,off,va,_,size,_,_,_ in segments:
   if typ==1 and va<=a<z<=va+size:return data[off+a-va:off+z-va],off+a-va
  raise ValueError(f'unmapped {a:x}')
 manifest={'original_elf_sha256':SHA,'source':'work/native/project/disc/SLUS_202.68','ram_reads':0,'generated_source_reads':0,'legacy_recovery_invocations':0,'selected':{},'context_only':{},'scope':'Finite ordinary binary32 leaf execution, supplied effective globals and ready root scene. Does not resolve/query world, detectors, 219120, scene hierarchy or whole vehicle runtime.',
 'source_boundaries':['Gravity interpreter stops at219964 and resumes2199C0 with supplied effective434730/434738 and ready upY; known setter1143E0 is independently executed.', 'Probe preparation executes original reached path with linked128 present,120/124null,100zero,ready scene. No original callback occurs on this path.', 'Probe response begins at1B1AAC after scene-axis preparation (unused on disabled path), stops1B239C and resumes1B23C0 with effective4342C8; lazy process-global initialization is outside numeric API. Original scalar/body stores execute in order.', 'Scheduler liveScale is supplied; factor0..bits3D75C28F and resultcount1..4. EE CVT.W.S is truncation toward zero.', 'Latefeedback executes after actual body and supplied external boundary effects; APIs do not stand in for missing stages.'],
 'defaults':{'effective434730':'43c13334','effective434738':'3f34fdf4','effective4342C8':'3e99999a'},
 'domain':'All consumed floats and intermediates finite. Scalar strict binary32/no contraction. No exceptional EE/VU mode equivalence claim. Effective434260 and434290 are explicitly supplied rather than read from snapshots.',
 'coverage':'Only selected reached words are credited. Branch outcomes excluded by admitted linked-body/null-probe/disabled/zero-hit domain are not fabricated.'}
 out={};lines=[];words=[]
 for name,ranges in [('selected',BLOCKS),('context_only',CONTEXT)]:
  for label,(a,z) in ranges.items():
   raw,off=extract(a,z);out[label+'.bin']=raw
   manifest[name][label]={'start':hex(a),'end_exclusive':hex(z),'words':len(raw)//4,'elf_file_offset':hex(off),'sha256':hashlib.sha256(raw).hexdigest()}
   lines.append(f'# {name} {label}')
   for i in range(0,len(raw),4):
    w=struct.unpack_from('<I',raw,i)[0];pc=a+i;lines.append(f'{pc:08x} {w:08x} {decode(pc,w)}')
    if name=='selected':words.append((pc,w))
 header=['#pragma once','#include <cstdint>','// Original ELF words only; generated by advancement_numeric_recover49k.py.','namespace advancement49k_source {','struct Word {std::uint32_t pc,bits;};','inline constexpr Word words[] = {']
 for pc,w in sorted(words):header.append(f'    {{0x{pc:08x}u,0x{w:08x}u}}, // {decode(pc,w)}')
 header+=['};','}'];out['original_words49k.h']=('\n'.join(header)+'\n').encode();out['original-disassembly.txt']=('\n'.join(lines)+'\n').encode();out['recovery-manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode();return out
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');args=p.parse_args();out=generate()
 for name,data in out.items():
  target=DEST/name
  if args.check:assert target.read_bytes()==data,name
  else:
   target.parent.mkdir(parents=True,exist_ok=True)
   if target.exists():assert target.read_bytes()==data,'Refusing to replace different source '+name
   target.write_bytes(data)
 print(json.dumps({'status':'verified' if args.check else 'frozen','files':len(out),'elf_sha256':SHA}))
