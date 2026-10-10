#!/usr/bin/env python3
"""Freeze source provenance and execute the straight-line DualShock descriptor setup."""
import argparse,hashlib,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={
 'base_constructor':(0x143390,0x143500,'e99a722054bb256923687c1c523f27a2aad9a6f756407863e38f4775babc1171'),
 'separate_virtual_query':(0x1439d0,0x143d68,'511452b3c384246bbc28a33d073ff9aa84f715e9de427355293aca84a1ea599a'),
 'manager_update':(0x128080,0x128450,'743b2a41a0fb58407ea945c8719f7af63a98487f4c684734cfe96a838cd71c40'),
 'psx_initialization':(0x237d00,0x237ddc,'0c1f20be5da61ce68379d546acd142dbbff8eac741057daaab5aa00bd2d1b6b0'),
 'device_update_prefix':(0x237df0,0x2381d0,'47d9ff3b04dbf81a7e9e694875fb5c6cc29d8b39e1ec83963c512c62b1bed6dc'),
 'separate_history_sampler':(0x238330,0x238724,'af6056b8684557dcb1398904459ed82962f39a30c2fd81178a8a6ca48911bb36'),
 'dualshock_profile':(0x2395b8,0x23984c,'1e8bc09da6f93e2e241432e567f07c51251f4ec657853f29a18f88434fa774fc')}
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--check',action='store_true');a=ap.parse_args()
 data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 if hashlib.sha256(data).hexdigest()!=SHA:raise ValueError('ELF hash mismatch')
 po=struct.unpack_from('<I',data,28)[0];size,n=struct.unpack_from('<HH',data,42);segments=[struct.unpack_from('<8I',data,po+i*size) for i in range(n)]
 def extract(start,end):
  for t,o,v,_,length,_,_,_ in segments:
   if t==1 and v<=start<end<=v+length:return data[o+start-v:o+end-v]
  raise ValueError('Unmapped provenance bytes')
 result={'elf_sha256':SHA,'blocks':{}}
 for name,(start,end,digest) in BLOCKS.items():
  b=extract(start,end)
  if hashlib.sha256(b).hexdigest()!=digest:raise ValueError('Independent provenance hash mismatch: '+name)
  result['blocks'][name]={'start':hex(start),'end_exclusive':hex(end),'sha256':digest,'hex_bytes':b.hex()}
 # Original setup after diagnostic call, before axis-map stores. Only integer
 # instructions execute; declared starting fixture is a zeroed16-descriptor table.
 g=[0]*32;memory=bytearray(0x3000);g[17]=0x1000;struct.pack_into('<I',memory,0x1014,0x2000)
 def address(rs,imm):
  x=(g[rs]+imm)&0xffffffff
  if not 0<=x<len(memory)-4:raise ValueError('Profile fixture address')
  return x
 count=0
 for pc in range(0x2395c4,0x239834,4):
  w=struct.unpack('<I',extract(pc,pc+4))[0];op=w>>26;rs=(w>>21)&31;rt=(w>>16)&31;rd=(w>>11)&31;sa=(w>>6)&31;fn=w&63;im=struct.unpack('<h',struct.pack('<H',w&65535))[0]
  if op in (9,0x19):g[rt]=(g[rs]+im)&0xffffffff
  elif op==12:g[rt]=g[rs]&(w&65535)
  elif op==13:g[rt]=g[rs]|(w&65535)
  elif op==0x24:g[rt]=memory[address(rs,im)]
  elif op==0x23:g[rt]=struct.unpack_from('<I',memory,address(rs,im))[0]
  elif op==0x28:memory[address(rs,im)]=g[rt]&255
  elif op==0x29:struct.pack_into('<H',memory,address(rs,im),g[rt]&65535)
  elif op==0x2b:struct.pack_into('<I',memory,address(rs,im),g[rt]&0xffffffff)
  elif op==0 and fn==0:g[rd]=(g[rt]<<sa)&0xffffffff
  elif op==0 and fn==0x24:g[rd]=g[rs]&g[rt]
  elif op==0 and fn==0x25:g[rd]=g[rs]|g[rt]
  else:raise ValueError(f'Unexpected profile word {pc:x}: {w:08x}')
  g[0]=0;count+=1
 rows=[]
 for i in range(16):
  p=0x2000+12*i;rows.append([memory[p],struct.unpack_from('<I',memory,p+4)[0],struct.unpack_from('<H',memory,p+8)[0]])
 result['profile_execution']={'start':'0x2395c4','end_exclusive':'0x239834','instructions':count,'fixture':'zeroed descriptor table; device+14 resolved; no SDK or RAM','rows_flags_offset_mask':rows}
 header='#pragma once\n#include <cstdint>\n// Test-only profile derived by original straight-line instructions.\ninline constexpr std::uint32_t rr_device_profile48y[16][3]={\n'+''.join('    {'+','.join(str(v)+'u' for v in row)+'},\n' for row in rows)+'};\n'
 files={'evidence/deviceHeld48Y/provenance-blocks.json':json.dumps(result,indent=2)+'\n','tools/gameplay/device_held_profile48y.h':header}
 for name,text in files.items():
  p=ROOT/name
  if a.check:
   if p.read_text()!=text:raise ValueError('Frozen provenance mismatch: '+name)
  else:
   if p.exists() and p.read_text()!=text:raise ValueError('Refusing historical overwrite')
   p.write_text(text)
 print(json.dumps({'status':'verified' if a.check else 'frozen','blocks':len(BLOCKS),'profile_instructions':count,'profile':rows}))
if __name__=='__main__':main()
