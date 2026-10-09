#!/usr/bin/env python3
"""Freeze 49N numeric/selection instructions directly from the verified original ELF.
No generated production source, process memory, game execution or original callbacks.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from pod_sphere_constraint_recover49l import decode as base_decode
ROOT=Path(__file__).resolve().parents[2]
DEST=ROOT/'evidence/authoredInitialization49N/numeric-source'
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={
 'physical_loader':(0x214720,0x214894),'class_selection':(0x214954,0x214aa4),
 'tuning_aero_engine':(0x214aa4,0x2154d8),'joystick':(0x215a70,0x215af8),
 'profile_gate_difference':(0x215fdc,0x216010),'profile_numeric':(0x216040,0x216350),
 'mass_setter':(0x165300,0x16533c),'inertia_setter':(0x165340,0x1653c0),
 'matrix_zero':(0x14d7f0,0x14d834),'matrix_inverse':(0x14e910,0x14eadc),
 'aero_zero':(0x100430,0x1004a4),'aero_assign':(0x1004b0,0x1005a0),
 'engine_setter':(0x2e7680,0x2e76dc),
 'strcmp_unaligned_gate':(0x3090e8,0x3090f8),'strcmp_scalar':(0x3091f8,0x30922c),
}
DATA={'reader_key_strings':(0x3b1a78,0x3b2978)}
REG='zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()
def decode(p,w):
 op,rs,rt,rd,sa,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
 if op==0 and fn in (0,3,0x21,0x23,0x25,0x18):return f'{ {0:"sll",3:"sra",0x21:"addu",0x23:"subu",0x25:"or",0x18:"mult"}[fn]} {REG[rd]},{REG[rs]},{REG[rt]},sa={sa}'
 if op==0 and fn==9:return f'jalr {REG[rd]},{REG[rs]}'
 if op==0x14 or op==0x15:return f'{"beql" if op==0x14 else "bnel"} {REG[rs]},{REG[rt]},{p+4+4*struct.unpack("<h",struct.pack("<H",w&65535))[0]:#x}'
 if op==0x20:return f'lb {REG[rt]},{struct.unpack("<h",struct.pack("<H",w&65535))[0]:#x}({REG[rs]})'
 return base_decode(p,w)
def generate():
 data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert hashlib.sha256(data).hexdigest()==SHA
 off=struct.unpack_from('<I',data,28)[0];size,count=struct.unpack_from('<HH',data,42)
 segs=[struct.unpack_from('<8I',data,off+i*size) for i in range(count)]
 def take(a,z):
  for kind,o,v,_,n,_,_,_ in segs:
   if kind==1 and v<=a<z<=v+n:return data[o+a-v:o+z-v],o+a-v
  raise ValueError('unmapped original address')
 manifest={'elf_sha256':SHA,'source':'work/native/project/disc/SLUS_202.68','production_source_reads':0,'RAM_reads':0,'original_game_callbacks_executed':0,'blocks':{},'data':{},'boundaries':[
 'Bounded initialized synthetic vehicle/body/engine/aero/manager/reader/stack only; every unmapped or uninitialized load fails. Memory is not an observed process image.',
 'Physical loader begins214720 after section selection. Caller supplies reader context Physical Properties and s5 vehicle. Matrix zero constructor and mass/inertia virtual targets execute their verified original words; table slots are explicit selected-type links.',
 '157D00 section lookup,158A70 scalar,158920 integer and158C00 vector are explicit returned parsed-value boundaries. The harness supplies section presence, lookup result and parsed words; no text conversion or opaque game parser execution is claimed. Required native keys reject missing, independently of original readers skipping writes.',
 '187330 returns only the explicitly supplied track-name pointer. Track selection runs original byte loops and original strcmp scalar words, using an unaligned initialized string to select that valid scalar route. No manager or track discovery is modeled.',
 '214894..214954 repulsor work is excluded; only its reader-section and live s4/s5/SP dependencies are supplied. It has no represented main mass/inertia/engine/aero/joystick output in these slices.',
 '2154D8..215A70 animation/collision consumers,215AF8..215FDC wreckage/binder/camera/damage/callback work are excluded. No opaque callback is claimed. Their effects on represented inputs must remain caller supplied/preserved; the adapter is a numeric projection, not a whole loader.',
 'Profile216040..216350 executes original engine/aero prefix with explicit selected manager100C0/vehicle60/profile values. Remaining216350 onward damage/repair work is outside projection. Continuation restores the source saved-frame registers before the original215FFC..21600C final speed difference; it never runs or fabricates the omitted work.',
 'Original28-float aero bulk is executed, but native API projects only documented consumer coefficients. Other authoring keys receive explicit independent fixture values, never inferred missing-key defaults. Aero130 damping is unmodified and must be supplied.',
 'Finite ordinary binary32 with separate operations/no contraction, FE_TONEAREST; not exceptional EE/VU silicon equivalence. All 16 inertia and inverse lanes are compared. No pose/cache/history/secondary initialization is inferred.',
 ]}
 files={};header=['#pragma once','#include <cstdint>','#include <cstddef>','namespace authored49n_words {'];listing=[];total=0;calls=[]
 known={a:name for name,(a,z) in BLOCKS.items()};external={0x157d00:'supplied section-presence boundary',0x158a70:'supplied parsed scalar boundary',0x158920:'supplied parsed integer boundary',0x158c00:'supplied parsed vector boundary',0x187330:'supplied track-string accessor boundary'}
 for name,(a,z) in BLOCKS.items():
  raw,fileoff=take(a,z);ws=struct.unpack('<'+'I'*(len(raw)//4),raw);files[name+'.bin']=raw
  manifest['blocks'][name]={'start':hex(a),'end_exclusive':hex(z),'word_count':len(ws),'index':total,'elf_offset':hex(fileoff),'sha256':hashlib.sha256(raw).hexdigest()};total+=len(ws)
  header.append(f'inline constexpr std::uint32_t {name}[]={{');listing.append('# '+name)
  for i,w in enumerate(ws):
   p=a+4*i;d=decode(p,w);header.append(f' 0x{w:08x}u, // {p:08x} {d}');listing.append(f'{p:08x} {w:08x} {d}')
   if w>>26==3:
    target=(w&0x3ffffff)*4
    if target not in known and target not in external:raise ValueError(f'unaccounted call {p:x}->{target:x}')
    calls.append({'pc':hex(p),'target':hex(target),'scope':external.get(target,'included original '+known.get(target,''))})
   if w>>26==0 and w&63==9:calls.append({'pc':hex(p),'scope':'explicit main-body selected virtual slot78 mass165300 / slot7C inertia165340'})
  header.append('};')
 header+=['struct Block {const char* name;std::uint32_t start;const std::uint32_t* words;std::size_t count,index;};','inline constexpr Block blocks[]={']
 for n,m in manifest['blocks'].items():header.append(f' {{"{n}",{m["start"]}u,{n},sizeof({n})/4,{m["index"]}}},')
 header+=['};',f'inline constexpr std::size_t total_words={total};']
 for name,(a,z) in DATA.items():
  raw,fileoff=take(a,z);files[name+'.bin']=raw;manifest['data'][name]={'start':hex(a),'end_exclusive':hex(z),'elf_offset':hex(fileoff),'sha256':hashlib.sha256(raw).hexdigest()}
  header.append(f'inline constexpr std::uint32_t {name}_start=0x{a:x};');header.append(f'inline constexpr unsigned char {name}[]={{')
  for i in range(0,len(raw),24):header.append(','.join(str(v) for v in raw[i:i+24])+',')
  header.append('};')
 header.append('}')
 files['authored_words49n.h']=('\n'.join(header)+'\n').encode();files['original-disassembly.txt']=('\n'.join(listing)+'\n').encode();files['recovery-manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode();files['call-targets.json']=(json.dumps(calls,indent=2)+'\n').encode();return files
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('--check',action='store_true');args=ap.parse_args();files=generate()
 for n,b in files.items():
  p=DEST/n
  if args.check:assert p.read_bytes()==b,n
  else:
   p.parent.mkdir(parents=True,exist_ok=True)
   if p.exists():assert p.read_bytes()==b,'refuse changed frozen file '+n
   else:p.write_bytes(b)
 print(json.dumps({'status':'verified' if args.check else 'frozen','files':len(files),'elf_sha256':SHA}))
