#!/usr/bin/env python3
"""Authenticate loader bindings; execute original software decimal words."""
import sys
sys.dont_write_bytecode=True
import hashlib,json,pathlib,random,struct
from pod_physics_recover49n import Original,BLOCKS,ELF_SHA
R=pathlib.Path(__file__).resolve().parents[2]
D=R/'work/impactAuthoring49Y';D.mkdir(parents=True,exist_ok=True)
elf=(R/'work/native/project/disc/SLUS_202.68').read_bytes()
assert hashlib.sha256(elf).hexdigest()==ELF_SHA
cpu=Original(elf)
source=R/'work/local49Y/static-impact/provenance'
manifest=json.loads((source/'manifest.json').read_text())
for name,block in manifest['source_blocks'].items():
 a,b=int(block['start'],16),int(block['end_exclusive'],16)
 raw=cpu.raw(a,b-a);assert hashlib.sha256(raw).hexdigest()==block['sha256']
 (D/(name+'.words.txt')).write_text(''.join(f'{p:08x} {int.from_bytes(cpu.raw(p,4),"little"):08x}\n' for p in range(a,b,4)))
a,b=0x215cc8,0x215d88
raw=cpu.raw(a,b-a)
manifest['source_blocks']['defense-section-and-routes']={'start':hex(a),'end_exclusive':hex(b),'sha256':hashlib.sha256(raw).hexdigest()}
(D/'defense-section-and-routes.words.txt').write_text(''.join(f'{p:08x} {int.from_bytes(cpu.raw(p,4),"little"):08x}\n' for p in range(a,b,4)))
a,b=0x1aa788,0x1aa938
raw=cpu.raw(a,b-a)
manifest['source_blocks']['pod-damage-loader']={'start':hex(a),'end_exclusive':hex(b),'sha256':hashlib.sha256(raw).hexdigest()}
(D/'pod-damage-loader.words.txt').write_text(''.join(f'{p:08x} {int.from_bytes(cpu.raw(p,4),"little"):08x}\n' for p in range(a,b,4)))
manifest['strings'].update({'0x3aae30':'PodDamage','0x3aaee0':'FinalDamageThreshold','0x3aaf00':'FinalDamageIncrement'})
assert int.from_bytes(cpu.raw(0x1aa918,4),'little')==0x0c05629c
assert int.from_bytes(cpu.raw(0x1aa91c,4),'little')==0x24660064
assert int.from_bytes(cpu.raw(0x1aa930,4),'little')==0x0c05629c
assert int.from_bytes(cpu.raw(0x1aa934,4),'little')==0x24660068
for addr,s in manifest['strings'].items():assert cpu.raw(int(addr,16),len(s)+1)==s.encode()+b'\0'
def word(p):return int.from_bytes(cpu.raw(p,4),'little')
bindings=[]
for i,(key,address) in enumerate([(k,int(a,16)) for a,k in manifest['strings'].items() if k in ['hitXL','hitL','hitM','hitS','hitXS','pointsXL','pointsL','pointsM','pointsS','pointsXS']]):
 call=0x1aa6a8+i*24
 assert word(call)==0x0c05629c
 if i==0:assert word(0x1aa69c)==0x8e0600c0 and word(call+4)==0x24450000+(address&65535)
 else:assert word(call+4)==0x24660000+i*4 and word(call-4)==0x24450000+(address&65535)
 bindings.append(dict(section='EnvDamage',key=key,call=hex(call),destination='tuning+'+hex(i*4),key_address=hex(address)))
for key,keypc,call,offset in [('MinDefense',0x215dd0,0x215e08,0x480),('MaxDefense',0x215e14,0x215e1c,0x484),('Defense',0x215e88,0x215e90,0x488),('Defense',0x215ed4,0x215edc,0x48c)]:
 assert word(call)==0x0c05629c and word(keypc+4)==0x27a60000+offset
 address=0x3b0000+(word(keypc)&65535);assert cpu.raw(address,len(key)+1)==key.encode()+b'\0'
 bindings.append(dict(section='DamageRepair' if offset<0x488 else 'BaseAbilities' if offset==0x488 else 'MaxAbilities',key=key,call=hex(call),destination='loader_stack+'+hex(offset),key_address=hex(address)))
assert word(0x215ccc)==0x244529d8 and word(0x215cd0)==0x0c055f40
assert word(0x215e78)==0x244520d8 and word(0x215e7c)==0x0c055f40
assert word(0x215ec4)==0x24452108 and word(0x215ec8)==0x0c055f40
for key,call,offset in [('FinalDamageThreshold',0x1aa918,0x64),('FinalDamageIncrement',0x1aa930,0x68)]:
 address=0x3b0000+((word(call-4)&65535)-65536)
 assert cpu.raw(address,len(key)+1)==key.encode()+b'\0'
 bindings.append(dict(section='PodDamage',key=key,call=hex(call),destination='tuning+'+hex(offset),key_address=hex(address)))
files={'ta.tun':(source/'ta.tun','6cba8a2ea570e6985fc73a7f0399e07360b96315f5a190886e4d3edbe793c243'),'Pod01.phy':(R/'evidence/authoredInitialization49N/authoring-source/Pod01.phy','47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b')}
selected={'EnvDamage':['hitXL','hitL','hitM','hitS','hitXS','pointsXL','pointsL','pointsM','pointsS','pointsXS'],'BaseAbilities':['Defense'],'MaxAbilities':['Defense'],'DamageRepair':['MinDefense','MaxDefense'],'PodDamage':['FinalDamageThreshold','FinalDamageIncrement']}
records=[];tokens=set()
for name,(path,sha) in files.items():
 raw=path.read_bytes();assert hashlib.sha256(raw).hexdigest()==sha;(D/name).write_bytes(raw)
 section='';offset=0
 for line in raw.splitlines(keepends=True):
  stripped=line.strip()
  if stripped.startswith(b'['):section=stripped[1:-1].decode()
  elif b'=' in stripped and not stripped.startswith(b';'):
   key,value=(x.strip() for x in stripped.split(b'=',1));key=key.decode()
   if key in selected.get(section,[]):
    token=value.decode();d,f,n=cpu.convert(token);tokens.add(token)
    vo=offset+line.index(b'=')+1
    while raw[vo] in (32,9):vo+=1
    records.append(dict(file=name,section=section,key=key,key_offset=offset+line.index(key.encode()),value_offset=vo,value_bytes=len(value),token=token,binary64=f'{d:016x}',binary32=f'{f:08x}'))
  offset+=len(line)
assert len(records)==16
rng=random.Random(0x49a017)
for token in ['.5','+.5','-.5','.0','+.0','-.0','.000001','-.999999','+.999999','0','-0','+0','999999999','-999999999']:
 tokens.add(token)
for _ in range(1024):
 digits=rng.randrange(1,7);v=rng.randrange(10**digits);tokens.add(rng.choice(['','+','-'])+'.'+str(v).zfill(digits))
rows=[]
for token in sorted(tokens):
 d,f,n=cpu.convert(token)
 sign=1 if token[0] in '+-' else 0
 if token[sign]=='.':
  normalized=token[:sign]+'0'+token[sign:];nd,nf,_=cpu.convert(normalized);assert (d,f)==(nd,nf)
 rows.append(f'{token}\t{d:016x}\t{f:08x}\n')
(D/'decimal.tsv').write_text(''.join(rows))
blocks={name:{'start':hex(a),'end':hex(b),'sha256':hashlib.sha256(cpu.raw(a,b-a)).hexdigest()} for name,(a,b) in BLOCKS.items()}
report=dict(elf_sha256=ELF_SHA,loader_source=manifest,bindings=bindings,decimal_source=blocks,authored=records,decimal_cases=len(rows),executed_instructions=cpu.total,executed_pcs=len(cpu.hits),scope='Original integer/software-double and narrowing words; no Python floating point conversion. Source key/write mappings retained. No profile selection or native CF0 resolution.')
(D/'proof.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({'cases':len(rows),'instructions':cpu.total,'authored':len(records)}))
