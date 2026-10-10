"""Bounded offline proof of terrain setup, query handoff and physical inputs."""
from pathlib import Path
import hashlib,json,re,struct
from recover48g import ROOT,ELF_SHA,RAM_SHA,RAM_PATH
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();ram=(ROOT/RAM_PATH).read_bytes()
 assert sha(elf)==ELF_SHA and sha(ram)==RAM_SHA
 def word(a):return struct.unpack_from('<I',elf,a-0x100000+0x80)[0]
 def u(a):return struct.unpack_from('<I',ram,a)[0]
 def f(a):return struct.unpack_from('<f',ram,a)[0]
 def vec(a):return {'bits':[f'{u(a+i*4):08x}' for i in range(3)],'value':[f(a+i*4) for i in range(3)]}
 def literal(a):
  o=a-0x100000+0x80;return elf[o:elf.index(0,o)].decode('ascii')
 out=ROOT/'evidence/gameplay48G';gen=ROOT/'work/native/project/generated'
 paths=[gen/n for n in ['FUN_001b0ee0_0x1b0ee0.cpp','FUN_001b11b0_0x1b11b0.cpp','FUN_001b1240_0x1b1240.cpp',
 'FUN_001b1560_0x1b1560.cpp','FUN_002146c0_0x2146c0.cpp','FUN_002208a0_0x2208a0.cpp','FUN_0011c3f0_0x11c3f0.cpp',
 'entry_0011c490_0x11c490.cpp','entry_001b0e94_0x1b0e94.cpp','entry_001b0eb0_0x1b0eb0.cpp','entry_001b0ecc_0x1b0ecc.cpp']]
 sources=[]
 for p in paths:
  seen=set();lines=[]
  for n,line in enumerate(p.read_text().splitlines(),1):
   m=re.match(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+(.*)',line)
   if not m:continue
   a,w=int(m[1],16),int(m[2],16);assert word(a)==w
   if a not in seen:seen.add(a);lines.append(f'{n}: {line.strip()}')
  (out/(p.name+'.instructions.txt')).write_text('\n'.join(lines)+'\n')
  sources.append({'path':p.relative_to(ROOT).as_posix(),'sha256':sha(p.read_bytes()),'verified_words':len(seen)})
 keys=[(0x3b1b30,'Repulsor'),(0x3b1b40,'MaxLiftHeight'),(0x3b1b50,'Spring'),(0x3b1b58,'Damping'),
       (0x3b1b60,'SideForceCoefficient'),(0x3b1b80,'OrientationSpring')]
 for a,text in keys:assert literal(a)==text
 # Complete argument/destination route for OrientationSpring, not naming by proximity.
 expected={0x2148fc:0x24451b80,0x214900:0x27a603ec,0x214904:0x0c05629c,
 0x214928:0xc7af03ec,0x21494c:0x0c06c490,0x1b12ac:0xc440026c,0x1b12b0:0x46007802,
 0x1b12b4:0x46000802,0x1b12b8:0x46000205,0x1b12cc:0xe6000204,
 0x1b0fa8:0x3c023f1c,0x1b0fac:0x344461a9,0x1b0fbc:0x3c0340a0,
 0x1b10ec:0xac440208,0x1b10f4:0xac43020c,0x1b0eac:0xace30108,0x1b0ec8:0xace3012c}
 for a,w in expected.items():assert word(a)==w,(hex(a),hex(word(a)),hex(w))
 terrain=0xd113f0;body=0xd110a0;scene=0xcfd8f0;record=u(terrain+0x90)
 assert u(terrain+0x128)==body and u(body+0x288)==scene
 assert ram[record+0x20:record+0x2c]==ram[terrain+0x170:terrain+0x17c]
 assert ram[record+0x30:record+0x3c]==ram[terrain+0x150:terrain+0x15c]
 data={'elf_sha256':ELF_SHA,'ram_sha256':RAM_SHA,'sources':sources,
  'literal_keys':[{'address':hex(a),'file_offset':hex(a-0x100000+0x80),'text':text} for a,text in keys],
  'orientation_spring':{'parser_call':'0x214904','stack_field':'0x3ec','argument_register':'f15 at0x214928',
   'setter_call':'0x21494c','destination':'component+0x204 at0x1b12cc',
   'formula':'abs(float32(float32(OrientationSpring * bodyMass) * float32_bits(0x4200cccd)))',
   'saved_field204_bits':f'{u(terrain+0x204):08x}','saved_field204_value':f(terrain+0x204)},
  'constructor_defaults':{'field208':{'bits':'3f1c61a9','value':f(terrain+0x208),'setter':'0x1b10ec'},
   'field20c':{'bits':'40a00000','value':f(terrain+0x20c),'setter':'0x1b10f4'}},
  'saved_runtime_contract':{'scope':'Stopped snapshot only; no fresh per-tick query result is implied.',
   'terrain':hex(terrain),'body':hex(body),'scene':hex(scene),'surface_record':hex(record),
   'state':{hex(o):u(terrain+o) for o in [0x100,0x104,0x108,0x10c,0x110,0x114,0x118,0x11c,0x12c]},
   'vectors':{hex(o):vec(terrain+o) for o in [0x130,0x140,0x150,0x170,0x180,0x190,0x1c0,0x1d0]},
   'query_mode88':u(terrain+0x88),'query_structure8c':hex(u(terrain+0x8c)),
   'optional_queries':{'120':hex(u(terrain+0x120)),'124':hex(u(terrain+0x124))},
   'material_masks':{hex(a):hex(u(a)) for a in [0x4342b0,0x3e0718,0x3e0720]}},
  'query_dependency':{'initializer':'0x11c3f0(type/count1,points=&terrain130,mode1,bodyScene)',
   'dispatch':'0x122660 limited object list or0x121920 generic query after0x11ff90 transform',
   'selection':'Query hit bit5 of terrain+F8; terrain+12C must be zero; point/normal copied from record+30/+20 at0x1b19fc/0x1b1a08.',
   'callback':'0x1b0e50 reads collision object+A4. Mask128 sets terrain+108. Mask4 copies object+9C into terrain+12C. Mask8 appends object to16-entry bounded list.',
   'unclosed':'Shape/traversal, nearest-hit/face-side and boundary policy; fresh native query equivalence and material/object filtering not implemented by terrain response.'},
  'remaining_collision':{'component354':hex(u(0xc5b0b0+0x354)),'method':'0x2964c0',
   'saved_flags':{hex(o):u(0xd3c570+o) for o in [0x48,0x4c,0x50,0x54,0x58,0x60,0x64,0x68]},
   'unimplemented':'Normal impulses297610, friction297390 and collision detection; terrain response is not a replacement.'},
  'checked_words':{hex(a):f'{w:08x}' for a,w in expected.items()}}
 (out/'dependency-contract.json').write_text(json.dumps(data,indent=2)+'\n')
 print(json.dumps({'verified_source_files':len(sources),'checked_words':len(expected),'matched_surface_record':hex(record),'native_query_closed':False}))
if __name__=='__main__':main()
