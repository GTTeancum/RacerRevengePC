#!/usr/bin/env python3
"""Freeze verified ELF words for projected1-D gradient noise, never RAM."""
import argparse,hashlib,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
DEST=Path('evidence/secondaryWrapper49M/noise-source')
ELF_SHA256='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={'noise':(0x14eec0,0x14efc4),'initialize':(0x14efd0,0x14f2f0),'random':(0x308030,0x308060),'seed':(0x308020,0x308030)}
REG='zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()
EXCLUSIONS=[(0x14f0b0,0x14f0d8,'2-D squared norm and float/double/sqrt calls; outputs feed excluded2-D writes only'),(0x14f0e0,0x14f0f8,'2-D normalization loads/divides/stores'),(0x14f148,0x14f17c,'3-D squared norm and float/double/sqrt calls; outputs feed excluded3-D writes only'),(0x14f188,0x14f18c,'3-D first normalized divide'),(0x14f194,0x14f1b4,'3-D normalized stores and remaining loads/divides'),(0x14f228,0x14f2c8,'Mirrors p/g1/g2/g3 at offset256;1-D read indices are masked255')]
def decode(pc,w):
 op,rs,rt,rd,fd,fn=w>>26,(w>>21)&31,(w>>16)&31,(w>>11)&31,(w>>6)&31,w&63
 im=(w&65535)-(65536 if w&32768 else 0)
 if not w:return 'nop'
 if op in(4,5):return f'{"beq" if op==4 else "bne"} {REG[rs]}, {REG[rt]}, {pc+4+4*im:#x}'
 if op==1 and rt==1:return f'bgez {REG[rs]}, {pc+4+4*im:#x}'
 if op==3:return f'jal {(w&0x3ffffff)*4:#x}'
 if op in(9,10):return f'{"addiu" if op==9 else "slti"} {REG[rt]}, {REG[rs]}, {im:#x}'
 if op in(12,13):return f'{"andi" if op==12 else "ori"} {REG[rt]}, {REG[rs]}, {w&65535:#x}'
 if op==15:return f'lui {REG[rt]}, {w&65535:#x}'
 if op in(30,31,35,36,43,49,57):
  name={30:'lq',31:'sq',35:'lw',36:'lbu',43:'sw',49:'lwc1',57:'swc1'}[op]
  return f'{name} {"f"+str(rt) if op in(49,57) else REG[rt]}, {im:#x}({REG[rs]})'
 if op==0:
  if fn==0:return f'sll {REG[rd]}, {REG[rt]}, {fd}'
  if fn==8:return f'jr {REG[rs]}'
  if fn in(0x18,0x21,0x24):return f'{ {0x18:"mult",0x21:"addu",0x24:"and"}[fn]} {REG[rd]}, {REG[rs]}, {REG[rt]}'
 if op==28 and fn==0x28 and fd==0x18:return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
 if op==17:
  if rs in(0,4):return f'{"mfc1" if rs==0 else "mtc1"} {REG[rt]}, f{rd}'
  if rs==20 and fn==0x20:return f'cvt.s.w f{fd}, f{rd}'
  if rs==16 and fn in(0,1,2,3,0x18,0x1a,0x1c,0x24):return f'{ {0:"add.s",1:"sub.s",2:"mul.s",3:"div.s",0x18:"adda.s",0x1a:"mula.s",0x1c:"madd.s",0x24:"cvt.w.s"}[fn]} f{fd}, f{rd}, f{rt}'
 raise ValueError(f'Unknown selected word{pc:08x}:{w:08x}')
def generate():
 data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 if hashlib.sha256(data).hexdigest()!=ELF_SHA256:raise ValueError('ELF mismatch')
 ph=struct.unpack_from('<I',data,28)[0];size,count=struct.unpack_from('<HH',data,42)
 segs=[struct.unpack_from('<8I',data,ph+i*size) for i in range(count)]
 def extract(a,z):
  for kind,o,v,_,n,_,_,_ in segs:
   if kind==1 and v<=a<z<=v+n:return data[o+a-v:o+z-v]
  raise ValueError('No original file-backed bytes')
 files={};listing=[];header=['#pragma once','#include <cstdint>','namespace gradient49m_words {'];manifest={'elf_sha256':ELF_SHA256,'source':'work/native/project/disc/SLUS_202.68','RAM_reads':0,'generated_translation_reads':0,'blocks':{},'boundary':'Dependency-sliced1-D noise only. No claim of full2-D/3-D shared initialization or physical EE edge semantics.'}
 for name,(a,z) in BLOCKS.items():
  raw=extract(a,z);words=struct.unpack('<'+'I'*(len(raw)//4),raw)
  files[DEST/(name+'.bin')]=raw
  manifest['blocks'][name]={'start':hex(a),'end_exclusive':hex(z),'words':len(words),'sha256':hashlib.sha256(raw).hexdigest()}
  header.extend([f'inline constexpr std::uint32_t {name}_start=0x{a:x}u;',f'inline constexpr std::uint32_t {name}[]={{'])
  listing.append(f'# {name}')
  for i,w in enumerate(words):
   pc=a+4*i;d=decode(pc,w);listing.append(f'{pc:08x} {w:08x} {d}');header.append(f' 0x{w:08x}u, // {pc:08x} {d}')
  header.append('};')
 header.append('}')
 files[DEST/'gradient_noise_words49m.h']=('\n'.join(header)+'\n').encode()
 files[DEST/'original-disassembly.txt']=('\n'.join(listing)+'\n').encode()
 manifest['excluded_oracle_spans']=[{'start':hex(a),'end_exclusive':hex(z),'words':(z-a)//4,'reason':s} for a,z,s in EXCLUSIONS]
 manifest['retained_integer_control_amid_exclusions']=['14F0D8','14F0DC','14F17C','14F180','14F184','14F18C','14F190','14F1B4','14F1B8']
 manifest['globals']={'382B40':{'initial_elf_word':hex(struct.unpack('<I',extract(0x382b40,0x382b44))[0]),'write':'14EEE0 clears before initializer; no reset inferred'},'38544C':{'initial_elf_pointer':hex(struct.unpack('<I',extract(0x38544c,0x385450))[0]),'purpose':'libc reentrancy pointer; raw RNG at pointed object+58'},'3851B8':{'initial_elf_seed':hex(struct.unpack('<I',extract(0x3851b8,0x3851bc))[0]),'boundary':'ELF startup value1 is provenance, never implicit API seed'},'42FA60':'permutation514 uint32; first256 retained','430270':'3-D gradient514x3 float, excluded','431A90':'2-D gradient514x2 float, excluded','432AA0':'1-D gradient514 float; first256 retained','4332A8':'signed conversion scratch word, retained'}
 for name,a,z in [('startup_zero_words',0x100008,0x100034),('lazy_flag',0x382b40,0x382b44),('rng_pointer',0x38544c,0x385450),('elf_initial_rng',0x3851b8,0x3851bc)]:
  raw=extract(a,z);files[DEST/(name+'.bin')]=raw;manifest.setdefault('data_provenance',{})[name]={'start':hex(a),'end_exclusive':hex(z),'sha256':hashlib.sha256(raw).hexdigest()}
 files[DEST/'recovery-manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode()
 return files,manifest
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--check',action='store_true');args=ap.parse_args();files,manifest=generate()
 for relative,data in files.items():
  path=ROOT/relative
  if args.check:
   if path.read_bytes()!=data:raise ValueError('Frozen evidence differs: '+str(relative))
  else:
   path.parent.mkdir(parents=True,exist_ok=True)
   if path.exists() and path.read_bytes()!=data:raise ValueError('Refusing to replace evidence')
   path.write_bytes(data)
 print(json.dumps({'status':'verified' if args.check else 'frozen','files':len(files),'words':sum(x['words'] for x in manifest['blocks'].values()),'elf_sha256':ELF_SHA256}))
if __name__=='__main__':main()
