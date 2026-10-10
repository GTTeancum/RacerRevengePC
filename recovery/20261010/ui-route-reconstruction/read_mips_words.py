# Static byte-to-text decoder only. Does not execute any guest instruction.
import struct,sys
from pathlib import Path
path=Path('/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68')
data=path.read_bytes()
r='zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()
def decode(pc,w):
 o=w>>26; rs=w>>21&31; rt=w>>16&31; rd=w>>11&31; sa=w>>6&31; fn=w&63; u=w&65535; s=u if u<32768 else u-65536
 if not w:return 'nop'
 if o==0:
  if fn in {0,2,3,56,58,59,60,62,63}:return f'{dict(zip([0,2,3,56,58,59,60,62,63], ["sll","srl","sra","dsll","dsrl","dsra","dsll32","dsrl32","dsra32"]))[fn]} {r[rd]},{r[rt]},{sa}'
  if fn==8:return f'jr {r[rs]}'
  if fn==9:return f'jalr {r[rd]},{r[rs]}'
  if fn in {16,18}:return f'{"mfhi" if fn==16 else "mflo"} {r[rd]}'
  if fn in {17,19}:return f'{"mthi" if fn==17 else "mtlo"} {r[rs]}'
  names={4:'sllv',6:'srlv',7:'srav',10:'movz',11:'movn',24:'mult',25:'multu',26:'div',27:'divu',32:'add',33:'addu',34:'sub',35:'subu',36:'and',37:'or',38:'xor',39:'nor',42:'slt',43:'sltu',44:'dadd',45:'daddu',46:'dsub',47:'dsubu'}
  if fn in names:return f'{names[fn]} {r[rd]},{r[rs]},{r[rt]}'
 if o in (2,3):return f'{"j" if o==2 else "jal"} 0x{((pc+4)&0xf0000000)|((w&0x3ffffff)<<2):08x}'
 if o==1:
  name={0:'bltz',1:'bgez',2:'bltzl',3:'bgezl',16:'bltzal',17:'bgezal'}.get(rt,f'regimm{rt}')
  return f'{name} {r[rs]},0x{pc+4+4*s:08x}'
 if o in (4,5,6,7,20,21,22,23):
  return f'{ {4:"beq",5:"bne",6:"blez",7:"bgtz",20:"beql",21:"bnel",22:"blezl",23:"bgtzl"}[o]} {r[rs]},{r[rt]},0x{pc+4+4*s:08x}'
 if o in (8,9,10,11,12,13,14,24,25):return f'{ {8:"addi",9:"addiu",10:"slti",11:"sltiu",12:"andi",13:"ori",14:"xori",24:"daddi",25:"daddiu"}[o]} {r[rt]},{r[rs]},{s if o not in (12,13,14) else hex(u)}'
 if o==15:return f'lui {r[rt]},0x{u:04x}'
 names={26:'ldl',27:'ldr',30:'lq',31:'sq',32:'lb',33:'lh',34:'lwl',35:'lw',36:'lbu',37:'lhu',38:'lwr',39:'lwu',40:'sb',41:'sh',42:'swl',43:'sw',44:'sdl',45:'sdr',46:'swr',49:'lwc1',55:'ld',57:'swc1',63:'sd'}
 if o in names:return f'{names[o]} {"f"+str(rt) if o in (49,57) else r[rt]},{s:#x}({r[rs]})'
 if o==28 and fn==40 and sa==24:return f'por {r[rd]},{r[rs]},{r[rt]}'
 if o==17:
  if rs==0:return f'mfc1 {r[rt]},f{rd}'
  if rs==4:return f'mtc1 {r[rt]},f{rd}'
  if rs==8:return f'bc1{"t" if rt&1 else "f"}{"l" if rt&2 else ""} 0x{pc+4+4*s:08x}'
  names={0:'add',1:'sub',2:'mul',3:'div',4:'sqrt',5:'abs',6:'mov',7:'neg',12:'round.w',13:'trunc.w',14:'ceil.w',15:'floor.w',32:'cvt.s',36:'cvt.w',48:'c.f',50:'c.eq',60:'c.lt',62:'c.le'}
  if fn in names:return f'{names[fn]}.{ {16:"s",17:"d",20:"w"}.get(rs,str(rs))} f{sa},f{rd},f{rt}'
 return '.word UNKNOWN'
def string(va):
 off=va-0x100000+0x80
 if 0<=off<len(data):
  e=data.find(b'\0',off,off+300)
  b=data[off:e] if e>=off else b''
  if b and all(c in (9,10,13) or 32<=c<127 for c in b):return repr(b.decode('ascii'))
 return None
start=int(sys.argv[1],16);end=int(sys.argv[2],16)
for pc in range(start,end,4):
 w=struct.unpack_from('<I',data,pc-0x100000+0x80)[0]
 print(f'{pc:08X} {w:08X} {decode(pc,w)}')
