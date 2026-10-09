"""Focused actual-source/provenance/bounds checks for 50FC; no game launch."""
from pathlib import Path
import json,struct,sys
sys.dont_write_bytecode=True
from decode_source_textures50fc import ROOT,load_ta,parse_resource,parse_pst,FormatError,sha,decode_indexed,ipu_rgba,verify_elf
checks=0
def need(value,why):
 global checks
 checks+=1
 if not value:raise AssertionError(why)
def rejects(name,b):
 try:parse_resource(name,b)
 except (FormatError,struct.error):return
 raise AssertionError('accepted malformed source')
p=ROOT/'evidence/textures50FC';report=json.loads((p/'manifest.json').read_text());sources=load_ta(ROOT/'work/native/project/disc/TRACKS/TA.RES');records=[]
for e in report['textures']:
 b=sources[e['member']];need(sha(b)==e['source_sha256'],'source SHA');frames=parse_resource(e['member'],b);need(len(frames)==e['frame_count'],'frame count')
 cutoffs={0,1,3,4,7,8,11,len(b)-1}
 for f in frames:
  cutoffs.update([f['start'],f['start']+11,f['end']-1])
  for mip in f['mips']:cutoffs.update([mip['header_offset'],mip['header_offset']+15,mip['data_offset'],mip['data_offset']+len(mip['data'])-1])
 for cut in sorted(c for c in cutoffs if c<len(b)):
  rejects(e['member'],b[:cut]);checks+=1
 rejects(e['member'],b+b'\0');checks+=1
 for at in ((12,) if e['member'].endswith('.atm') else (0,)):
  wrong=bytearray(b);struct.pack_into('<I',wrong,at,2);rejects(e['member'],wrong);checks+=1
 for record in e['mips']:
  data=(p/record['output']['path']).read_bytes();w,h=struct.unpack_from('<II',data,8)
  need(data[:8]==b'RRTEX001' and (w,h)==(record['width'],record['height']) and len(data)==16+w*h*4,'native frame/extent')
  need(sha(data)==record['output']['sha256'] and sha(data[16:])==record['rgba_sha256'],'native hash')
  source=frames[record['frame']];mip=source['mips'][record['level']]
  need(sha(mip['data'])==record['encoded_sha256'],'mip source SHA')
  if source['kind']==1:need(data[16:]==decode_indexed(mip['data'],source['palette']),'indexed source texels')
  else:
   need(all(v%8==0 for i,v in enumerate(data[16:]) if i%4<3),'RGB16 expansion')
   need(all(data[16+i*4+3]==(255 if any(data[16+i*4:19+i*4]) else 0) for i in range(w*h)),'TEXA AEM')
 records.append(dict(member=e['member'],frames=len(frames),mips_checked=len(e['mips']),base_sha256=e['selected_output']['sha256']))
for y,expected in [(16,bytes([0,0,0,0])),(235,bytes([248,248,248,255]))]:
 data=bytes([y])*(16*16)+bytes([128])*(16*16//2);rgba=ipu_rgba(data,16,16)
 need(rgba==expected*(16*16),'known color neutral endpoint')
need(len(verify_elf(ROOT/'work/native/project/disc/SLUS_202.68'))==15,'ELF static anchors')
result=dict(status='PASS',checks=checks,actual_resources=records,scope='Actual source identity, complete frame/mip bounds, selected decoded output hashes, indexed RGBA equality, RGB16/TEXA invariants. Hardware IDCT differential is not claimed.')
(p/'checks.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(dict(status='PASS',checks=checks,resources=len(records))))
