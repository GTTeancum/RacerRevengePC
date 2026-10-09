"""Cold exact-source Pod01 cable PST1/kind0 texture export and provenance."""
from pathlib import Path
import hashlib,json,struct
ROOT=Path(__file__).resolve().parents[2]
def sha(b):return hashlib.sha256(b).hexdigest()
def need(ok,message):
 if not ok:raise ValueError(message)
def main():
 source_path=ROOT/'evidence/podCables50FD/source/textures_cable.pst';source=source_path.read_bytes()
 need(len(source)==2140 and sha(source)=='e6b0abd320de50c784816a4e15dc6bfa056c8241a173eb26481243ff84dc301f','Exact original cable source required')
 need(struct.unpack_from('<III',source,0)==(1,0,1),'PST1 kind0 single mip')
 need(struct.unpack_from('<4I',source,76)==(64,64,0,0),'Exact mip header')
 palette=[source[12+4*i:16+4*i] for i in range(16)]
 rgba=b''.join(palette[index] for packed in source[92:] for index in (packed&15,packed>>4))
 native=b'RRTEX001'+struct.pack('<II',64,64)+rgba
 need(sha(native)=='10329f55fd0edd413b71c74915b6694700beee922c36255c410099eccd903d64','Pinned native output')
 # Hardware physical-address table facts. A raw8x2RGBA upload followed by
 # CSM1/CPSM32/CSA0 T32I4 lookup composes to identity for all16 palette entries.
 uploaded_addresses=[0,1,4,5,8,9,12,13,2,3,6,7,10,11,14,15]
 clut_addresses=[0,1,4,5,8,9,12,13,2,3,6,7,10,11,14,15]
 physical={address:index for index,address in enumerate(uploaded_addresses)}
 mapping=[physical[address] for address in clut_addresses]
 need(mapping==list(range(16)),'Original palette upload/CLUT composition')
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 need(sha(elf)=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30','Original ELF identity')
 ranges=[(0x11a518,0x11a544,'palette count16 and64byte allocation'),(0x11a580,0x11a59c,'read raw palette words'),
 (0x24eda4,0x24edf4,'kind0 reads width*height*4/8 and skips kind1 upload conversion'),
 (0x250dec,0x250e30,'PSMT4 upload original source pointer and palette pointer'),
 (0x257ca8,0x257ce0,'kind0 raw palette upload8x2'),(0x257d1c,0x257d7c,'palette transfer dimensions and raw pointer'),
 (0x242298,0x2422c0,'PSMT4 CPSM32 CSM1 CSA0 CLD1 binding')]
 anchors=[]
 for start,end,label in ranges:
  data=elf[start-0x100000+128:end-0x100000+128]
  anchors.append(dict(start=hex(start),end=hex(end),meaning=label,sha256=sha(data),words=[f'{v:08x}'for v in struct.unpack('<%dI'%(len(data)//4),data)]))
 out=ROOT/'evidence/cableTexture50FC';out.mkdir(parents=True,exist_ok=True);(out/'cable.rrtex').write_bytes(native)
 report=dict(schema='RR_CABLE_TEXTURE_50FC_1',source_member='textures\\cable.pst',source_container='PODS/POD01/POD01.RES',
 source_container_sha256='08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3',source_sha256=sha(source),
 source_bytes=len(source),revision=1,kind=0,mip_count=1,width=64,height=64,palette_offset=12,palette_bytes=64,mip_header_offset=76,packed_indices_offset=92,
 nibble_order='low then high, in unchanged linear row order',palette_mapping=mapping,color_policy='All original RGBA bytes preserved without scaling, quantization, row flip or palette permutation.',
 output=dict(path='cable.rrtex',bytes=len(native),sha256=sha(native),rgba_sha256=sha(rgba)),elf_sha256=sha(elf),source_evidence=anchors,
 reference_urls=['https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/GS/GSTables.cpp',
 'https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/GS/GSLocalMemoryMultiISA.cpp',
 'https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/GS/GSLocalMemory.h'],
 scope='Exact retained Pod01 cable only. Native decoder rejects every other input identity. Original shader, filtering and cable geometry are separate.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(dict(status='PASS',source_texels=4096,output_sha256=sha(native))))
if __name__=='__main__':main()
