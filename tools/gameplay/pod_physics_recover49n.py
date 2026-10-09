#!/usr/bin/env python3
"""Independent original-word short-decimal proof. No production parser/math reads."""
import sys
sys.dont_write_bytecode=True
import argparse, hashlib, json, struct, random, re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
DEST=ROOT/'evidence/authoredInitialization49N/authoring-source'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
PHY_SHA='47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b'
BLOCKS={
'decimal_wrapper':(0x302e10,0x302e2c),'decimal_reentrant_wrapper':(0x30ab18,0x30ab44),
'decimal_short':(0x309c18,0x30a1b4),'decimal_finish':(0x30aac4,0x30ab18),
'int_to_double':(0x3101e8,0x3102a0),'double_pack':(0x30f7a0,0x30f8cc),
'double_unpack':(0x30f8d0,0x30f96c),'double_add':(0x30f970,0x30fc70),
'double_divide':(0x30ff18,0x310080),'narrow':(0x310440,0x310494),
'float_pack':(0x310498,0x3105a4),'float_pack_call':(0x311008,0x311034),
}
SOURCE={ 'section_lookup':(0x157af0,0x157d00),'section_read':(0x157d00,0x158240),
'key_lookup':(0x158240,0x15891c),'integer_reader':(0x158920,0x158a70),
'scalar_reader':(0x158a70,0x158ac8),'vector_reader':(0x158c00,0x158cf8),
'line_tokenizer':(0x29ff50,0x2a0094),'text_normalization':(0x159440,0x159578),
'ascii_case_compare':(0x308f00,0x308fb4),'integer_parser':(0x30ac38,0x30aea4)}
MASK=(1<<64)-1
def si(x,n=64):return (x&((1<<n)-1))-(1<<n) if x&(1<<(n-1)) else x&((1<<n)-1)
def sx(x,n=32):return si(x,n)&MASK
class Original:
 def __init__(self,elf):
  self.elf=elf;self.hits={};self.total=0
  o=struct.unpack_from('<I',elf,28)[0];s,n=struct.unpack_from('<HH',elf,42)
  self.segs=[struct.unpack_from('<8I',elf,o+i*s) for i in range(n)]
 def raw(self,p,n):
  for kind,o,v,_,size,_,_,_ in self.segs:
   if kind==1 and v<=p and p+n<=v+size:return self.elf[o+p-v:o+p-v+n]
  raise ValueError(f'Unmapped static source {p:x}+{n}')
 def get(self,p,n):
  if p>=0x700000:
   if not (0x700000<=p and p+n<=0x710000 or 0x7fe000<=p and p+n<=0x800000):raise ValueError(f'read bounds {p:x}')
   return sum(self.mem.get(p+i,0xa5)<<(8*i) for i in range(n))
  return int.from_bytes(self.raw(p,n),'little')
 def put(self,p,n,v):
  if not (0x700000<=p and p+n<=0x710000 or 0x7fe000<=p and p+n<=0x800000):raise ValueError(f'write bounds {p:x}')
  for i in range(n):self.mem[p+i]=(v>>(i*8))&255
 def step(self,pc,delay=False):
  if not any(a<=pc<z for a,z in BLOCKS.values()):raise ValueError(f'escaped closed original code at{pc:x}')
  w=int.from_bytes(self.raw(pc,4),'little');op=w>>26;rs=w>>21&31;rt=w>>16&31;rd=w>>11&31;sa=w>>6&31;fn=w&63;im=si(w&65535,16)
  self.hits[pc]=self.hits.get(pc,0)+1;self.steps+=1;self.total+=1
  if self.steps>20000:raise ValueError('instruction budget')
  r=self.r;a=r[rs];b=r[rt];control=False;likely=False;taken=False;target=pc+8
  def wr(k,v):
   if k:r[k]=v&MASK
  def branch(t,lk=False):
   nonlocal control,likely,taken,target
   control=True;likely=lk;taken=t;target=pc+4+im*4
  addr=(a+im)&MASK
  if op==0:
   if fn==0:wr(rd,sx(b<<sa))
   elif fn==2:wr(rd,sx((b&0xffffffff)>>sa))
   elif fn==3:wr(rd,sx(si(b,32)>>sa))
   elif fn==4:wr(rd,sx(b<<(a&31)))
   elif fn==6:wr(rd,sx((b&0xffffffff)>>(a&31)))
   elif fn==7:wr(rd,sx(si(b,32)>>(a&31)))
   elif fn==8:control=taken=True;target=a&0xffffffff
   elif fn==10:
    if not b:wr(rd,a)
   elif fn==11:
    if b:wr(rd,a)
   elif fn==18:wr(rd,self.lo)
   elif fn==19:self.lo=a
   elif fn==22:wr(rd,b>>(a&63))
   elif fn==24:
    p=si(a,32)*si(b,32);self.lo=sx(p);self.hi=sx(p>>32);wr(rd,self.lo)
   elif fn==33:wr(rd,sx(a+b))
   elif fn==35:wr(rd,sx(a-b))
   elif fn==36:wr(rd,a&b)
   elif fn==37:wr(rd,a|b)
   elif fn==38:wr(rd,a^b)
   elif fn==42:wr(rd,si(a)<si(b))
   elif fn==43:wr(rd,a<b)
   elif fn==45:wr(rd,a+b)
   elif fn==47:wr(rd,a-b)
   elif fn==56:wr(rd,b<<sa)
   elif fn==58:wr(rd,b>>sa)
   elif fn==59:wr(rd,si(b)>>sa)
   elif fn==60:wr(rd,b<<(sa+32))
   elif fn==62:wr(rd,b>>(sa+32))
   elif fn==63:wr(rd,si(b)>>(sa+32))
   else:raise ValueError(f'SPECIAL {pc:x}: {w:x}')
  elif op==1:
   if rt not in (0,1,2,3):raise ValueError('REGIMM')
   branch(si(a)<0 if rt in (0,2) else si(a)>=0,rt in (2,3))
  elif op in (2,3):
   if op==3:wr(31,pc+8)
   control=taken=True;target=((pc+4)&0xf0000000)|((w&0x3ffffff)<<2)
  elif op in (4,5,20,21):branch(a==b if op in (4,20) else a!=b,op>=20)
  elif op in (6,7,22,23):branch(si(a)<=0 if op in (6,22) else si(a)>0,op>=22)
  elif op==9:wr(rt,sx(a+im))
  elif op==10:wr(rt,si(a)<im)
  elif op==11:wr(rt,a<(im&MASK))
  elif op==12:wr(rt,a&(w&65535))
  elif op==13:wr(rt,a|(w&65535))
  elif op==14:wr(rt,a^(w&65535))
  elif op==15:wr(rt,sx((w&65535)<<16))
  elif op==17:
   if rs!=4:raise ValueError('COP1')
   self.f[rd]=b&0xffffffff
  elif op==25:wr(rt,a+im)
  elif op==28 and fn==0:
   p=si(a,32)*si(b,32)+si(self.lo,32)+(si(self.hi,32)<<32);self.lo=sx(p);self.hi=sx(p>>32);wr(rd,self.lo)
  elif op==32:wr(rt,sx(self.get(addr,1),8))
  elif op==35:wr(rt,sx(self.get(addr,4)))
  elif op==36:wr(rt,self.get(addr,1))
  elif op==40:self.put(addr,1,b)
  elif op==43:self.put(addr,4,b)
  elif op==55:wr(rt,self.get(addr,8))
  elif op==63:self.put(addr,8,b)
  elif op in (26,27):
   aligned=addr&~7;k=addr&7;v=self.get(aligned,8)
   if op==26:
    sh=(7-k)*8;mask=MASK>>((k+1)*8) if k<7 else 0;wr(rt,(b&mask)|(v<<sh))
   else:
    sh=k*8;mask=(MASK<<(64-sh))&MASK if sh else 0;wr(rt,(b&mask)|(v>>sh))
  elif op in (44,45):
   aligned=addr&~7;k=addr&7;v=self.get(aligned,8)
   if op==44:
    sh=(7-k)*8;mask=(MASK<<((k+1)*8))&MASK;self.put(aligned,8,(v&mask)|(b>>sh))
   else:
    sh=k*8;mask=(1<<sh)-1;self.put(aligned,8,(v&mask)|(b<<sh))
  else:raise ValueError(f'opcode {pc:x}: {w:x}')
  if control:
   if delay:raise ValueError('control in delay')
   if not likely or taken:self.step(pc+4,True)
   return target if taken else pc+8
  return pc+4
 def run(self,entry,args):
  self.r=[0]*32;self.f=[0]*32;self.steps=0;self.lo=self.hi=0
  self.r[29]=0x800000;self.r[31]=0xfffffff0
  for i,v in enumerate(args):self.r[4+i]=v&MASK
  pc=entry
  while pc!=0xfffffff0:pc=self.step(pc)
  if self.r[29]!=0x800000 or self.r[31]!=0xfffffff0:raise ValueError('ABI restoration')
 def convert(self,s):
  self.mem={};self.put(0x700000,len(s)+1,int.from_bytes(s.encode()+b'\0','little'))
  self.run(0x302e10,[0x700000]);double=self.r[2];a=self.steps
  self.run(0x310440,[double]);return double,self.f[0],a+self.steps

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--write',action='store_true');ap.add_argument('--token');args=ap.parse_args()
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert hashlib.sha256(elf).hexdigest()==ELF_SHA
 cpu=Original(elf)
 # Prove required original10^k constants are exact integers without host floats.
 for k in range(7):
  bits=int.from_bytes(cpu.raw(0x3c6558+8*k,8),'little');exp=((bits>>52)&2047)-1023-52
  sig=(1<<52)|(bits&((1<<52)-1))
  assert bits>>63==0 and (sig<<exp if exp>=0 else sig//(1<<-exp))==10**k
  assert exp>=0 or sig%(1<<-exp)==0
 if args.token is not None:
  d,f,n=cpu.convert(args.token);print(json.dumps({'token':args.token,'binary64':f'{d:016x}','binary32':f'{f:08x}','instructions':n}));return
 phy=(DEST/'Pod01.phy').read_bytes();assert hashlib.sha256(phy).hexdigest()==PHY_SHA
 section='';records=[];offset=0;tokens=set()
 for raw in phy.splitlines(keepends=True):
  line=raw.strip()
  if line.startswith(b'['):section=line[1:-1].decode()
  elif line and not line.startswith(b';') and b'=' in line:
   k,v=line.split(b'=',1);k=k.strip();v=v.strip();ko=offset+raw.index(k);vo=offset+raw.index(b'=')+1
   while phy[vo] in (32,9):vo+=1
   record={'section':section,'key':k.decode(),'key_offset':ko,'value_offset':vo,'value_bytes':len(v),'token':v.decode()}
   if re.fullmatch(rb'-?\d+(?:\.\d+)?(?:,\s*-?\d+(?:\.\d+)?){0,2}',v):
    ts=[x.strip().decode() for x in v.split(b',')];tokens.update(ts)
    values=[cpu.convert(t) for t in ts];record['binary64']=[f'{x[0]:016x}' for x in values];record['binary32']=[f'{x[1]:08x}' for x in values]
   records.append(record)
  offset+=len(raw)
 rng=random.Random(0x49a)
 tokens.update(['0','-0','+0','0.0','-0.0','1','-1','999999999','-999999999','0.000001','-0.000001','16777217','16777219'])
 for _ in range(2500):
  m=rng.randrange(1000000000);p=rng.randrange(7);s=str(m).zfill(p+1)
  if p:s=s[:-p]+'.'+s[-p:]
  # Nine total digits (leading zero included) keeps the native grammar simple.
  if sum(c.isdigit() for c in s)>9:continue
  tokens.add(('-' if rng.randrange(2) else '')+s)
 vectors=[]
 for s in sorted(tokens):
  d,f,n=cpu.convert(s);vectors.append({'token':s,'binary64':f'{d:016x}','binary32':f'{f:08x}','instructions':n})
 manifest={'elf_sha256':ELF_SHA,'phy_sha256':PHY_SHA,'phy_bytes':len(phy),'resource_sha256':'08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3','resource_member':'Pods\\Pod01\\Pod01.phy','resource_logical_offset':1130496,'records':records,'numeric_cases':len(vectors),'instructions':cpu.total,'visited_original_pcs':len(cpu.hits),'source_blocks':{}}
 files={}
 for name,(a,z) in {**SOURCE,**BLOCKS,'power10_table':(0x3c6558,0x3c6610),'decimal_jump_table':(0x3c6670,0x3c6728),'ascii_classification_table':(0x3c6391,0x3c6411),'decimal_reentrancy_pointer':(0x38544c,0x385450)}.items():
  raw=cpu.raw(a,z-a);files[name+'.bin']=raw;manifest['source_blocks'][name]={'start':hex(a),'end_exclusive':hex(z),'sha256':hashlib.sha256(raw).hexdigest()}
 manifest['scope'] = {
  'text_boundary': 'Source-recovered text section/line/key semantics, with explicit file service boundary. The original whole-file reader is not executed. Source fixture279 keys in19 sections is byte exact; offsets index original decoded PHY bytes.',
  'lookup': '157AF0 text mode traverses sections in source order and calls308F00 ASCII case-insensitive comparison.157D00 reads each chosen line through159440, which removes space/tab/CR/LF outside quotes and bracketed names. It skips empty and whole-line semicolon comments.158240 text mode traverses in order, splits on= then newline via29FFA0, compares case-insensitively, strips enclosing quotes, and copies found text. Duplicate first match and wider original forms are outside native admission.',
  'scalar': '158A70 calls158240 with maximum=-1; on false skips destination SWC1 and returns0. On success calls302E10->30AB18->309C18 then310440 and stores f0, returning1. Loader callers generally ignore success; native complete-key rejection is narrower admission, not source missing-key/default behavior.',
  'vector': '158C00 lookup limit128; if absent copies caller default xyz. If present,30AB70 comma tokens x/y then newline token z, each scalar conversion, stores sequentially. Native requires both vectors and exactly3 complete bounded numeric components; no default or partial write.',
  'integers': '158920 recognizes first-letter T/F/Y/N and ON/OFF, otherwise30AE70->30AC38 base0. Native restricts tuning flags to single ASCII0/1; this source-reviewed integer subset is not part of decimal machine execution statistics.',
  'conversion': 'Original-word execution from302E10 through309C18 short digits,3101E8 integer-to-double,30FF18 software division and30F7A0 pack,30FC08 software0-minus-positive for minus sign,310440 software narrow. No host floating arithmetic or math hooks. Code entry into non-short strtod path is rejected by oracle.',
  'native_numeric_domain': 'Optional sign; >=1 integer digit; optional dot then>=1 fractional digit; <=9 total digits, <=6 fractional digits. No whitespace/exponent/suffix/locale/NaN/Inf. All magnitudes stay normal binary32 except zero. Negative zero produces positive zero in original software subtraction. Native exact integer rational nearest-even binary64 followed by original normal binary32 narrow.',
  'native_restrictions': '32KiB input,255-byte lines,32 sections,512 keys, ASCII, complete61 scalars+2 vectors, no duplicate sections/keys even unconsumed, optional Tuning presence only, atomic output/no heap. These are explicit narrower native restrictions; not general original INI compatibility or a complete original file/parser execution claim.'
 }
 projection=[]
 groups=[('Physical Properties','Weight RollInertiaScale PitchInertiaScale YawInertiaScale Gravity AirborneGravity'),('Engine','MaxThrustA MaxThrustB MaxThrustC MinMaxThrust MaxMaxThrust TheoreticalTopSpeedA TheoreticalTopSpeedB TheoreticalTopSpeedC MinTheoreticalTopSpeed MaxTheoreticalTopSpeed BoostThrust RepairThrust RampUpFilter RampDownFilter RampDownSpeed ActualTopSpeed BoostSpeed RepairSpeed'),('Engine Aerodynamics','DefaultAngleOfAttack Lift BasicDrag PitchMoment PitchRatePitchMoment SideSlipSideForce RollRateRollMoment SideSlipYawMoment YawRateYawMoment SpoilerDrag SpoilerBrakeDrag SpoilerYawMomentScale SpoilerSideForce PowerslideSideForce PowerslideSpoilerYawMoment PowerslideThrust InvertedSideSlipSideForce InvertedSpoilerSideForce InvertedSideSlipYawMoment SpoilerYawMomentA SpoilerYawMomentB SpoilerYawMomentC MinSpoilerYawMoment MaxSpoilerYawMoment SpoilerYawMomentOffset')]
 names=[(section,key,key) for section,keys in groups for key in keys.split()]
 names += [(scope+'Abilities',key,scope+'Ability'+key) for key in ['MaxThrust','TheoreticalTopSpeed','SpoilerYawMoment'] for scope in ['Base','Max']]
 names += [('Joystick',key,key) for key in 'SteerSpeed1 SteerScale1 SteerSpeed2 SteerScale2 XAxisRateLimit YAxisRateLimit'.split()]
 for section,key,enum in names:
  found=[r for r in records if r['section']==section and r['key']==key];assert len(found)==1
  projection.append(dict(found[0],native_key=enum))
 manifest['native_scalar_projection']=projection
 files['source-manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode()
 expected=['#pragma once','namespace pod_phy49n_fixture {','struct Field { rr::game::AuthoredKey49N key; std::uint32_t keyOffset,valueOffset,valueBytes,word; };','inline constexpr Field fields[] = {']
 for r in projection:
  expected.append('  {rr::game::AuthoredKey49N::'+r['native_key']+','+str(r['key_offset'])+','+str(r['value_offset'])+','+str(r['value_bytes'])+',0x'+r['binary32'][0]+'u},')
 expected+=['};','}'];files['pod01_projection49n.h']=('\n'.join(expected)+'\n').encode()
 sys.path.insert(0,str(ROOT/'tools/gameplay'))
 from tv_camera_recover49e import decode
 listing=[]
 for name,(a,z) in {**SOURCE,**BLOCKS}.items():
  listing.append('# '+name)
  for pc in range(a,z,4):
   w=int.from_bytes(cpu.raw(pc,4),'little')
   try: decoded=decode(pc,w)
   except ValueError: decoded=f'.word0x{w:08x}'
   listing.append(f'{pc:08x} {w:08x} {decoded}')
 files['original-disassembly.txt']=('\n'.join(listing)+'\n').encode()
 files['conversion-cases.json']=(json.dumps(vectors,indent=2)+'\n').encode()
 files['conversion-visits.json']=(json.dumps({f'{p:08x}':n for p,n in sorted(cpu.hits.items())},indent=2)+'\n').encode()
 hdr=['#pragma once','#include <cstdint>','namespace pod_phy49n_original {','struct Case { const char* token; std::uint64_t binary64; std::uint32_t binary32; };','inline constexpr Case cases[] = {']
 hdr += ['  {"'+x['token']+'",0x'+x['binary64']+'ull,0x'+x['binary32']+'u},' for x in vectors]
 hdr += ['};','}'];files['conversion_cases49n.h']=('\n'.join(hdr)+'\n').encode()
 if args.write:
  for name,data in files.items():(DEST/name).write_bytes(data)
 else:
  for name,data in files.items():assert (DEST/name).read_bytes()==data,name
 print(json.dumps({'cases':len(vectors),'instructions':cpu.total,'visited_pcs':len(cpu.hits),'verified_files':len(files),'mode':'write' if args.write else 'verify'}))
if __name__=='__main__':main()
