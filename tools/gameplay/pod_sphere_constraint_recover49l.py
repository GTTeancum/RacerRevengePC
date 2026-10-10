#!/usr/bin/env python3
"""Freeze exact sphere-constraint words from the hash-verified original ELF.
No production formulas, process memory, game execution, reconstruction, PC launch,
contact blocks, ownership, allocation-failure or query/reset diagnostics are used.
"""
import argparse, hashlib, json, struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
DEST=Path('evidence/sphereConstraint49L/numeric-source')
ELF_SHA256='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={
 'constructor':(0x1b6200,0x1b6468),
 'setup_a':(0x1b64d0,0x1b6668),
 'setup_b':(0x1b6670,0x1b66c4),
 'prepare':(0x1b66d0,0x1b6b98),
 'correct':(0x1b6ba0,0x1b6fe4),
 'apply':(0x1b6ff0,0x1b7138),
 'matrix_vector':(0x127790,0x1277d0),
 'matrix_subtract':(0x14d8c0,0x14da3c),
 'matrix_inverse':(0x14e910,0x14eadc),
 'vector_constructor':(0x106790,0x1067ac),
 'skew':(0x14da40,0x14dae4),
 'diagonal':(0x14e150,0x14e214),
 'matrix_zero':(0x14d7f0,0x14d834),
 'scene_rotate':(0x28b9b0,0x28baa8),
 'scene_point':(0x28bab0,0x28bbb0),
 'scene_inverse_rotate':(0x28bbb0,0x28bd48),
 'scene_inverse_point':(0x28bd50,0x28bee4),
 'static_vectors':(0x3c9cf0,0x3c9d70),
}
REG='zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()
def decode(pc,w):
 op,rs,rt,rd,fd,fn=w>>26,(w>>21)&31,(w>>16)&31,(w>>11)&31,(w>>6)&31,w&63
 imm=(w&65535)-(65536 if w&32768 else 0)
 if not w:return 'nop'
 if op in (4,5):return f'{"beq" if op==4 else "bne"} {REG[rs]}, {REG[rt]}, {pc+4+4*imm:#x}'
 if op==7:return f'bgtz {REG[rs]}, {pc+4+4*imm:#x}'
 if op==3:return f'jal {((pc+4)&0xf0000000)|((w&0x3ffffff)<<2):#x}'
 if op==9:return f'addiu {REG[rt]}, {REG[rs]}, {imm:#x}'
 if op in (12,13):return f'{"andi" if op==12 else "ori"} {REG[rt]}, {REG[rs]}, {w&65535:#x}'
 if op==15:return f'lui {REG[rt]}, {w&65535:#x}'
 if op in (30,31,35,36,43,49,54,57,62):
  name={30:'lq',31:'sq',35:'lw',36:'lbu',43:'sw',49:'lwc1',54:'lqc2',57:'swc1',62:'sqc2'}[op]
  reg=('f'+str(rt)) if op in (49,57) else ('vf'+str(rt)) if op in (54,62) else REG[rt]
  return f'{name} {reg}, {imm:#x}({REG[rs]})'
 if op==0 and fn==8:return f'jr {REG[rs]}'
 if op==28 and (w&0x7ff)==0x628:return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
 if op==17:
  if rs==4:return f'mtc1 {REG[rt]}, f{rd}'
  if rs==8:return f'{"bc1t" if rt&1 else "bc1f"} {pc+4+imm*4:#x}'
  if rs==16:
   names={0:'add.s',1:'sub.s',2:'mul.s',3:'div.s',4:'sqrt.s',5:'abs.s',6:'mov.s',7:'neg.s',0x18:'adda.s',0x19:'suba.s',0x1a:'mula.s',0x1c:'madd.s',0x1d:'msub.s',0x28:'max.s',0x32:'c.eq.s',0x34:'c.lt.s',0x36:'c.le.s'}
   if fn in names:return f'{names[fn]} f{fd}, f{rd}, f{rt}'
 if op==18:
  if rs==2:return f'qmfc2 {REG[rt]}, vf{rd}'
  if rs>=16:
   mask=''.join(c for i,c in enumerate('xyzw') if w&(1<<(24-i)))
   if fn<4:return f'vadd{"xyzw"[fn]}.{mask} vf{fd},vf{rd},vf{rt}'
   if fn<8:return f'vsub{"xyzw"[fn&3]}.{mask} vf{fd},vf{rd},vf{rt}'
   if fn<12:return f'vmadd{"xyzw"[fn&3]}.{mask} vf{fd},vf{rd},vf{rt}'
   if fn<16:return f'vmsub{"xyzw"[fn&3]}.{mask} vf{fd},vf{rd},vf{rt}'
   if fn in (0x18,0x19,0x1a,0x1b):return f'vmul{"xyzw"[fn&3]}.{mask} vf{fd},vf{rd},vf{rt}'
   if fn in (0x28,0x2a,0x2c,0x2e):return f'{ {0x28:"vadd",0x2a:"vmul",0x2c:"vsub",0x2e:"vopmsub"}[fn]}.{mask} vf{fd},vf{rd},vf{rt}'
   return f'vu-special.{mask} 0x{w&0x7ff:03x},vf{rd},vf{rt}'
 return f'.word 0x{w:08x}'
def generate():
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 if hashlib.sha256(elf).hexdigest()!=ELF_SHA256:raise ValueError('Original ELF SHA256 mismatch')
 if elf[:6]!=b'\x7fELF\x01\x01':raise ValueError('Expected little endian ELF32')
 off=struct.unpack_from('<I',elf,28)[0];size,count=struct.unpack_from('<HH',elf,42)
 segs=[struct.unpack_from('<8I',elf,off+i*size) for i in range(count)]
 def extract(a,z):
  for kind,o,v,_,n,_,_,_ in segs:
   if kind==1 and v<=a<z<=v+n:return elf[o+a-v:o+z-v],o+a-v
  raise ValueError(f'Unmapped source extent {a:#x}..{z:#x}')
 files={};listing=[];header=['#pragma once','#include <cstdint>','#include <cstddef>','// Exact ELF words; generated independently of native implementation.','namespace sphere49l_words {']
 manifest={'elf_sha256':ELF_SHA256,'source':'work/native/project/disc/SLUS_202.68','production_source_reads':0,'RAM_reads':0,'instructions_are_original_words':True,'blocks':{},'boundaries':[
  'constructor numeric-default execution begins at 0x1B6214 after inherited base initializer; its first five words are provenance only. Synthetic stack saves are initialized explicitly. No base ownership or lifecycle execution.',
  'Scene inputs have bit0 of byte190 set and initialized matrix100. Dirty-scene branches and callees2897B0/289940 are not executed or modeled.',
  'Only initialized bounded synthetic bodies/joints/scenes/stack. Native domain requires finite nonzero separation squared; the source has no zero-distance guard.',
  'Scalar/vector normal finite binary32 operations with explicit operation ordering; sqrt from inherited exact restored ee_sqrt14.h. This is not full EE/VU silicon equivalence or hardware execution.',
  'Full source helper ranges are preserved; coverage explicitly lists all unexecuted PCs and branch outcomes.',
  'No contact/secondary-integration blocks, 49C domain, collision capacity, ownership, allocator failures, reset or query diagnostics.' ]}
 total=0
 for name,(a,z) in BLOCKS.items():
  raw,o=extract(a,z);words=struct.unpack('<'+'I'*(len(raw)//4),raw)
  manifest['blocks'][name]={'start':hex(a),'end_exclusive':hex(z),'word_count':len(words),'index':total,'elf_offset':hex(o),'sha256':hashlib.sha256(raw).hexdigest()};total+=len(words)
  files[DEST/(name+'.bin')]=raw;listing.append(f'# {name} [{a:#x},{z:#x})')
  header.extend([f'inline constexpr std::uint32_t {name}_start=0x{a:x}u;',f'inline constexpr std::uint32_t {name}[]={{'])
  for i,w in enumerate(words):
   d=decode(a+4*i,w);line=f'{a+4*i:08x} {w:08x} {d}';listing.append(line);header.append(f'  0x{w:08x}u, // {a+4*i:08x} {d}')
  header.append('};')
 header.extend(['struct Block { const char* name; std::uint32_t start; const std::uint32_t* words; std::size_t count; std::size_t index; };','inline constexpr Block blocks[]={'])
 for name,m in manifest['blocks'].items():header.append(f' {{"{name}",{name}_start,{name},sizeof({name})/4,{m["index"]}}},')
 header.extend(['};',f'inline constexpr std::size_t total_words={total};','}'])
 files[DEST/'sphere_words49l.h']=('\n'.join(header)+'\n').encode();files[DEST/'original-disassembly.txt']=('\n'.join(listing)+'\n').encode();files[DEST/'recovery-manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode()
 calls=[]
 starts={a:name for name,(a,z) in BLOCKS.items()}
 for name,(a,z) in BLOCKS.items():
  raw=files[DEST/(name+'.bin')]
  for i,w in enumerate(struct.unpack('<'+'I'*(len(raw)//4),raw)):
   if w>>26!=3:continue
   pc=a+4*i;target=((pc+4)&0xf0000000)|((w&0x3ffffff)<<2)
   if target in starts:scope='included exact original callee: '+starts[target]
   elif target==0x1276e0:scope='excluded inherited constructor prefix'
   elif target in (0x2897b0,0x289940):scope='excluded dirty-scene hierarchy path'
   else:raise ValueError(f'Unaccounted original call {pc:#x} -> {target:#x}')
   calls.append({'caller_block':name,'pc':hex(pc),'target':hex(target),'scope':scope})
 files[DEST/'call-targets.json']=(json.dumps({'elf_sha256':ELF_SHA256,'calls':calls},indent=2)+'\n').encode()

 return files,manifest
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--check',action='store_true');args=ap.parse_args();files,m=generate()
 for name,data in files.items():
  path=ROOT/name
  if args.check:
   if path.read_bytes()!=data:raise ValueError('Frozen source mismatch: '+str(name))
  else:
   path.parent.mkdir(parents=True,exist_ok=True)
   if path.exists() and path.read_bytes()!=data:raise ValueError('Refusing to overwrite different frozen source: '+str(name))
   path.write_bytes(data)
 print(json.dumps({'status':'verified' if args.check else 'frozen','files':len(files),'original_words':sum(x['word_count'] for x in m['blocks'].values()),'elf_sha256':ELF_SHA256}))
if __name__=='__main__':main()
