"""Bounded cold conversion of the eight missing TA texture resources.

Source identities and every container/frame/mip extent are checked. Kind2 is
original IPUM intra MPEG video, decoded by installed FFmpeg to YUV420P. Color
conversion follows the original RGB16+dither command and documented IPU integer
CSC. This is genuine source decoding, not pixel-exact PS2 IDCT certification.
No guest code, emulator, source replacement, upload swizzle or row flip is used.
"""
from pathlib import Path
import argparse, hashlib, json, shutil, struct, subprocess, sys, zlib
sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
TA_SHA = 'b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
SOURCE_SHA = {
 'track.pst':'e27d767a7d2aaa595d97b86b7208658404389540d2265d1bfbcaf2bd09ccbfef',
 'trackdark.pst':'df48a11d8ca7ad3a4c53e0e2840c4c0df57ce01504751dd5ea62248bfcb6c0c2',
 'trackside.pst':'4db3e6270ec85d8ddd16af7445befd43bad7e03f61cc9399d9559095ae9d5d02',
 'trackwheel.pst':'de447b0debdad77b00d3f95d0919c0ec9d2a169229a18a61a5df9c722e240265',
 'cord.pst':'91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1',
 'crawlera.pst':'0130a604d965a47948f36d02e1b6b64aec3ed4cebcffeb36a389a88454aee7e4',
 'arrowred.atm':'dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220',
 'crowd.atm':'76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031',
}
class FormatError(ValueError): pass
def need(ok, why):
 if not ok: raise FormatError(why)
def sha(b): return hashlib.sha256(b).hexdigest()
def unpack(fmt,b,at):
 need(0<=at<=len(b)-struct.calcsize(fmt),f'truncated {fmt} at {at}')
 return struct.unpack_from(fmt,b,at)
def piece(b,at,n):
 need(0<=n and 0<=at<=len(b)-n,f'extent {at}+{n} outside {len(b)}')
 return b[at:at+n]
def load_ta(path):
 b=path.read_bytes(); need(sha(b)==TA_SHA,'TA.RES identity mismatch')
 ver,meta,codec,flags,table=unpack('<5I',b,0)
 need((ver,codec,flags)==(3,0x340,0) and table>=2 and table%2==0 and 20+table==8+meta,'RES header')
 sizes=unpack('<'+'H'*(table//2),b,20);need(sizes[-1]==0 and all(sizes[:-1]),'RES block table')
 at=20+table; count,index_size=unpack('<2I',b,at);at+=8;end=at+index_size
 need(count<=10000 and len(sizes)*24576<=128*1024*1024,'RES caps');rows=[]
 for _ in range(count):
  n,=unpack('<I',b,at);at+=4;need(0<n<=1024,'RES name cap');name=piece(b,at,n).decode('ascii');at+=n
  need('\0' not in name,'RES NUL');off,n=unpack('<2I',b,at);at+=8;need(at<=end,'RES index overrun');rows.append((name,off,n))
 need(at==end,'RES index suffix');flat=bytearray()
 for n in sizes[:-1]:
  compressed=piece(b,at,n);need(piece(b,at+n,1)==b'\xff','RES boundary');d=zlib.decompressobj();raw=d.decompress(compressed,24577)
  need(len(raw)==24576 and d.eof and not d.unused_data and not d.unconsumed_tail,'RES zlib block');flat.extend(raw);at+=n+1
 need(at==len(b),'RES suffix');out={}
 for name,off,n in rows:
  raw=piece(flat,off,n);key=name.replace('\\','/').lower()
  if key.startswith('textures/') and key[9:] in SOURCE_SHA:
   need(key not in out,'duplicate texture name');need(sha(raw)==SOURCE_SHA[key[9:]],'source texture hash');out[key]=bytes(raw)
 need(len(out)==len(SOURCE_SHA),'missing source texture');return out

def parse_pst(b,at=0,*,allow_kind0=False):
 start=at;version,kind,count=unpack('<III',b,at);at+=12
 need(version==1 and (kind in (1,2) or (allow_kind0 and kind==0)) and 1<=count<=7,'unsupported PST version/kind/mips')
 palette_bytes=64 if kind==0 else 1024 if kind==1 else 0
 palette=piece(b,at,palette_bytes) if palette_bytes else None
 at+=palette_bytes
 mips=[];last=None
 for level in range(count):
  header=at;h,w,a,c=unpack('<4I',b,at);at+=16
  need(16<=w<=1024 and 16<=h<=1024 and not(w&(w-1)) and not(h&(h-1)) and a==c==0,'PST mip header')
  need(last is None or (w,h)==(last[0]*2,last[1]*2),'PST mip order');last=(w,h)
  if kind==2:n,=unpack('<I',b,at);at+=4
  else:
   need(kind!=0 or (w*h)%2==0,'kind0 requires complete packed texel pairs')
   n=w*h//2 if kind==0 else w*h
  data=piece(b,at,n)
  if kind==2:
   magic,size,iw,ih,frames=unpack('<4sIHHI',data,0)
   need(magic==b'ipum' and size==n-8 and (iw,ih)==(w,h) and frames==1,'IPUM header')
   need(data[-8:]==b'\0\0\1\xb0\0\0\1\xb1' and data[16]==0,'IPUM supported frame flags/terminators')
  mips.append(dict(level=level,width=w,height=h,header_offset=header,data_offset=at,data=data));at+=n
 return dict(start=start,end=at,kind=kind,mips=mips,palette=palette),at

def parse_resource(name,b,*,allow_kind0=False):
 need(len(b)<=8*1024*1024,'source cap')
 if name.endswith('.atm'):
  version,count,reserved=unpack('<3I',b,0)
  need(version==1 and 1<=count<=64 and reserved==0,'ATM header');at=12
 else:count=1;at=0
 frames=[]
 for i in range(count):
  frame,at=parse_pst(b,at,allow_kind0=allow_kind0);frames.append(frame)
 need(at==len(b),'PST/ATM trailing bytes')
 return frames

# BT.601 integer coefficients and fixed 4x4 IPU dither values are hardware
# format facts. Independently expressed scalar math; no emulator code imported.
DITHER=((-4,0,-3,1),(2,-2,3,-1),(-3,1,-4,0),(3,-1,2,-2))
def ipu_rgba(yuv,w,h):
 need(len(yuv)==w*h*3//2,'YUV420P extent');n=w*h;out=bytearray(n*4)
 for y in range(h):
  for x in range(w):
   p=y*w+x;uv=(y//2)*(w//2)+x//2;u=yuv[n+uv]-128;v=yuv[n+n//4+uv]-128
   yy=(149*max(0,yuv[p]-16))//64
   channels=((yy+(204*v)//64+1)//2,(yy+(-104*v)//64+(-50*u)//64+1)//2,(yy+(258*u)//64+1)//2)
   rgb=[(max(0,min(255,max(0,min(255,c))+DITHER[y%4][x%4]))//8)*8 for c in channels]
   # Original kind2 TEXA selects TA0=TA1=255 and AEM=1 (2421EC..2421FC).
   # IPU SETTH is zero: packed alpha bit is0; black texels become transparent
   # under AEM. RGB5 components expand into the high5 bits, as on GS sampling.
   out[p*4:p*4+4]=bytes(rgb+[255 if any(rgb) else 0])
 return bytes(out)

def decode_ipu(data,w,h,ffmpeg):
 # The final B1 sequence terminator is container framing, not a second image.
 # Feed the exact first frame through B0, preventing FFmpeg's raw parser from
 # reporting a bogus empty second packet. No coefficient/pixel bytes change.
 cmd=[ffmpeg,'-hide_banner','-loglevel','error','-nostdin','-cpuflags','0','-flags','+bitexact','-idct','simple','-threads','1',
      '-err_detect','explode','-f','ipu','-i','pipe:0','-frames:v','1','-pix_fmt','yuv420p','-f','rawvideo','pipe:1']
 proc=subprocess.run(cmd,input=data[:-4],capture_output=True,timeout=30)
 need(proc.returncode==0 and not proc.stderr,'FFmpeg IPU decode failed: '+proc.stderr.decode(errors='replace'))
 need(len(proc.stdout)==w*h*3//2,'FFmpeg decoded frame extent')
 return ipu_rgba(proc.stdout,w,h),sha(proc.stdout)

def decode_indexed(data,palette,kind=1):
 if kind==0:
  # Source-equivalent to import_texture48e.cpp:184-195: PSMT4 low nibble
  # first, identity 16-entry palette, all four original bytes including alpha.
  # No kind1 CLUT permutation, alpha normalization, row flip or padding.
  need(palette is not None and len(palette)==64,'kind0 palette extent')
  out=bytearray()
  for packed in data:
   for index in (packed&15,packed>>4):out.extend(palette[4*index:4*index+4])
  return bytes(out)
 need(kind==1,'unsupported indexed kind')
 out=bytearray()
 for index in data:
  p=((index&~24)|((index&8)<<1)|((index&16)>>1))*4;out.extend(palette[p:p+4])
 return bytes(out)

def verify_elf(path):
 b=path.read_bytes();need(sha(b)==ELF_SHA,'SLUS identity mismatch')
 anchors={0x24e99c:0x10820028,0x24f158:0x0c04becc,0x24f198:0x0c04becc,0x24f1b8:0x24420010,
  0x257604:0x3c030800,0x25760c:0x3c050400,0x257624:0x3c070001,0x257634:0xac650000,
  0x2f56c8:0x3c039000,0x2f56d4:0xac430000,0x2421e8:0x240300ff,0x2421ec:0xa2230015,
  0x2421f0:0xa2230016,0x2421f4:0x24030001,0x2421fc:0xa2230017}
 for va,word in anchors.items():need(unpack('<I',b,va-0x100000+128)[0]==word,'SLUS anchor mismatch')
 return [{'address':hex(va),'word':hex(word)} for va,word in anchors.items()]

def write(path,data):
 path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data);need(path.read_bytes()==data,'write readback')
 return dict(path=path.name,bytes=len(data),sha256=sha(data))
def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--disc',type=Path,default=ROOT/'work/native/project/disc')
 p.add_argument('--output',type=Path,default=ROOT/'evidence/textures50FC')
 p.add_argument('--ffmpeg',default='ffmpeg');p.add_argument('--all-atm-frames',action='store_true')
 p.add_argument('--index-header',type=Path)
 args=p.parse_args();ffmpeg=shutil.which(args.ffmpeg);need(ffmpeg is not None,'FFmpeg not available')
 anchors=verify_elf(args.disc/'SLUS_202.68');sources=load_ta(args.disc/'TRACKS/TA.RES');entries=[];total_mips=0
 for name,b in sorted(sources.items()):
  frames=parse_resource(name,b);all_mips=[];stem=Path(name).stem;selection=None
  for fi,frame in enumerate(frames if args.all_atm_frames else frames[:1]):
   for mip in frame['mips']:
    if frame['kind']==2:rgba,yuv_sha=decode_ipu(mip['data'],mip['width'],mip['height'],ffmpeg)
    else:rgba=decode_indexed(mip['data'],frame['palette']);yuv_sha=None
    native=struct.pack('<8sII',b'RRTEX001',mip['width'],mip['height'])+rgba
    filename=f'{stem}-frame{fi}-mip{mip["level"]}.rrtex';written=write(args.output/filename,native)
    item={k:v for k,v in mip.items() if k!='data'};item.update(frame=fi,kind=frame['kind'],encoded_bytes=len(mip['data']),encoded_sha256=sha(mip['data']),yuv420p_sha256=yuv_sha,rgba_sha256=sha(rgba),output=written)
    all_mips.append(item);total_mips+=1
    if fi==0:selection=item
  entries.append(dict(member=name,source_sha256=sha(b),source_bytes=len(b),frame_count=len(frames),
   frame_extents=[dict(index=i,start=f['start'],end=f['end'],kind=f['kind'],mips=len(f['mips'])) for i,f in enumerate(frames)],
   selected_frame=0,selected_mip=selection['level'],selected_output=selection['output'],width=selection['width'],height=selection['height'],mips=all_mips))
 report=dict(schema='RR_SOURCE_TEXTURES_50FC_1',archive_sha256=TA_SHA,elf_sha256=ELF_SHA,elf_verified_anchors=anchors,
  ffmpeg_version=subprocess.run([ffmpeg,'-version'],capture_output=True,text=True,check=True).stdout.splitlines()[0],
  ffmpeg_sha256=sha(Path(ffmpeg).read_bytes()),converter_sha256=sha(Path(__file__).read_bytes()),
  decoder_policy='FFmpeg native IPU decoder; fixed scalar simple IDCT; exact original frame bytes excluding final sequence terminator; documented integer IPU CSC; original enabled RGB16 dither; GS high5-bit expansion and TEXA255/AEM1; no row flip',
  limitations=['PS2 hardware IDCT bit-exactness is unmeasured. FFmpeg inverse-DCT rounding may differ by small amounts before RGB16 quantization.',
   'Dither clamp/rounding follows documented independent reference; not validated against an original framebuffer capture.',
   'ATM frame0 is a genuine first stored frame; playback timing and animation are not implemented by the callback.',
   'Original shaders, filtering, mip selection and material blend behavior remain renderer responsibilities.'],
  reference_urls=['https://ffmpeg.org/doxygen/7.1/ipudec_8c_source.html','https://ffmpeg.org/doxygen/7.1/mpeg12dec_8c_source.html',
   'https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/IPU/yuv2rgb.cpp','https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/IPU/IPUdither.cpp'],
  textures=entries,mips_exported=total_mips)
 manifest=(json.dumps(report,indent=2)+'\n').encode();write(args.output/'manifest.json',manifest)
 if args.index_header:
  lines=['// Generated from actual source conversion; output hashes pin each native asset.']
  for e in entries:
   lines.append(' {"%s", "%s", "%s", "%s", %du, %du, %du, %du},'%(e['member'].upper(),e['source_sha256'],e['selected_output']['path'],e['selected_output']['sha256'],e['source_bytes'],e['width'],e['height'],e['selected_mip']))
  write(args.index_header,('\n'.join(lines)+'\n').encode())
 print(json.dumps(dict(status='PASS',textures=len(entries),mips=total_mips,rgba_bytes=sum(e['width']*e['height']*4 for e in entries),manifest_sha256=sha(manifest))))
if __name__=='__main__':main()
