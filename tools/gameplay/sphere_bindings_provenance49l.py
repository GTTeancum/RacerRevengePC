#!/usr/bin/env python3
"""Freeze narrow static body/constraint binding provenance from the original ELF.
No generated guest source, RAM, runtime, allocation or query execution.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
from tv_camera_recover49e import decode
ROOT=Path(__file__).resolve().parents[2]
DEST=ROOT/'evidence/sphereConstraint49L/binding-source'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={
 'rigid_body_table_install':(0x167920,0x167950),
 'main_body_binding':(0x211f74,0x211fd0),
 'optional_chariot_gate':(0x2124dc,0x2124f0),
 'secondary_body_and_constraint_binding':(0x212538,0x212720),
 'absent_secondary_route':(0x212940,0x21295c),
 'constraint_table_install':(0x1b6200,0x1b6224),
 'optional_simulation_dispatch':(0x219cfc,0x219d1c),
 'constraint_contact_integration_order':(0x2190a0,0x219104),
 'physical_chariot_render_selection':(0x21b9a4,0x21b9d0),
 'rigid_body_mass_setter':(0x165300,0x165340),
}
DATA={
 'rigid_body_type_name':(0x3a76f0,0x3a7701),
 'rigid_body_typeinfo':(0x3a77a0,0x3a77a8),
 'rigid_body_vtable':(0x3ce000,0x3ce0b0),
 'constraint_type_name':(0x3ac420,0x3ac434),
 'constraint_typeinfo':(0x3ac4d0,0x3ac4d8),
 'constraint_vtable':(0x3cf290,0x3cf310),
 'physical_properties_chariot_weight_strings':(0x3b1a90,0x3b1abd),
}
def sha(b):return hashlib.sha256(b).hexdigest()
def generate():
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(raw)==ELF_SHA
 phoff=struct.unpack_from('<I',raw,28)[0];size,count=struct.unpack_from('<HH',raw,42)
 segments=[struct.unpack_from('<8I',raw,phoff+i*size) for i in range(count)]
 def take(a,z):
  for kind,off,va,_,filesz,_,_,_ in segments:
   if kind==1 and va<=a<z<=va+filesz:return raw[off+a-va:off+z-va]
  raise ValueError('Unmapped original range')
 manifest={'source':'work/native/project/disc/SLUS_202.68','elf_sha256':ELF_SHA,'status':'static-source-only','scope':'Body/constraint type, construction links, optional update and render dispatch. No guest, allocation, contact/query or scene-cache traversal execution.','blocks':{},'data':{}}
 outputs={};lines=[]
 for group,ranges in [('blocks',BLOCKS),('data',DATA)]:
  for name,(a,z) in ranges.items():
   b=take(a,z);outputs[name+'.bin']=b;manifest[group][name]={'start':hex(a),'end_exclusive':hex(z),'bytes':len(b),'sha256':sha(b)}
   if group=='blocks':
    lines.append(f'# {name} [{a:#x},{z:#x})')
    for n in range(0,len(b),4):
     w=struct.unpack_from('<I',b,n)[0]
     try:meaning=decode(a+n,w)
     except ValueError:meaning='original word (mnemonic not decoded here)'
     lines.append(f'{a+n:08x} {w:08x} {meaning}')
 outputs['original-disassembly.txt']=('\n'.join(lines)+'\n').encode()
 outputs['manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode()
 return outputs

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');args=p.parse_args()
 files=generate()
 for name,data in files.items():
  path=DEST/name
  if args.check:assert path.read_bytes()==data,name
  else:
   path.parent.mkdir(parents=True,exist_ok=True)
   if path.exists():assert path.read_bytes()==data,'Refusing to replace differing frozen bytes: '+name
   else:path.write_bytes(data)
 print(json.dumps({'status':'verified' if args.check else 'frozen','files':len(files),'elf_sha256':ELF_SHA,'execution':False}))
if __name__=='__main__':main()
