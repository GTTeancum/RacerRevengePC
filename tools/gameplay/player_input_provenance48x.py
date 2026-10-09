#!/usr/bin/env python3
"""Freeze bounded constructor/device provenance; never infer missing action names."""
import argparse,hashlib,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={
 'player_create':(0x1a0570,0x1a0894,'02ab8da9d88f7add1e712f5b7fdc56758e4e5a5161638c880c2ed4102a9d873b'),
 'axis_constructor':(0x127bb0,0x127c94,'71933fe8b265799cca09f9e3965d9a0952935052e4da873c180e1663ecbdd1be'),
 'axis_registration':(0x143700,0x143800,'13f51676c53888aaf53fedf85e12880ccb566f7f4512dc61348ed5baf0aeb5e8'),
 'update_attached_axes':(0x1438a0,0x1439cc,'922398f912d95690ac03119a696ef571a263e745054768fa3eb17edad52e756d'),
 'packet_axis_update':(0x237f20,0x238080,'5df95e6a3aec6c6b02aa9342b9236fcb315f32e6f809092959b2ef43e87d7475'),
 'device_type_configuration':(0x239250,0x239c08,'45208bbb6f202f7c2e2d8bf599ab816c8e29a125e9ed21e65ea0f54317ef9730'),
 'flipped_key_storage':(0x1434c4,0x143500,'848c25bb54cd363519cd6682ce74714da95c5c19c9e39cd249aa94989a63de57')}
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--check',action='store_true');a=ap.parse_args()
 data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 if hashlib.sha256(data).hexdigest()!=SHA:raise ValueError('ELF hash mismatch')
 po=struct.unpack_from('<I',data,28)[0];size,n=struct.unpack_from('<HH',data,42)
 segments=[struct.unpack_from('<8I',data,po+i*size) for i in range(n)]
 def extract(start,end):
  for t,o,v,_,length,_,_,_ in segments:
   if t==1 and v<=start<end<=v+length:return data[o+start-v:o+end-v]
  raise ValueError('Unmapped provenance extent')
 result={'elf_sha256':SHA,'blocks':{},'strings':{}}
 for name,(start,end,digest) in BLOCKS.items():
  b=extract(start,end)
  if hashlib.sha256(b).hexdigest()!=digest:raise ValueError('Independent block hash mismatch: '+name)
  result['blocks'][name]={'start':hex(start),'end_exclusive':hex(end),'sha256':digest,'hex_bytes':b.hex()}
 for name,start in [('axis_assertion',0x388d90),('axis_update_assertion',0x3a43f0),('dualshock',0x3b3d60),('inversion_key',0x3a4390)]:
  b=extract(start,start+128).split(b'\0',1)[0]
  result['strings'][name]={'address':hex(start),'value':b.decode('ascii'),'hex_bytes':b.hex()}
 text=json.dumps(result,indent=2)+'\n';path=ROOT/'evidence/playerInput48X/provenance-blocks.json'
 if a.check:
  if path.read_text()!=text:raise ValueError('Provenance file mismatch')
 else:
  if path.exists() and path.read_text()!=text:raise ValueError('Refusing historical overwrite')
  path.write_text(text)
 print(json.dumps({'status':'verified' if a.check else 'frozen','blocks':len(BLOCKS),'elf_sha256':SHA}))
if __name__=='__main__':main()
