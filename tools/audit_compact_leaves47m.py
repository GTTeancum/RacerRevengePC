"""Find actual game leaf bodies eligible for initial compact CPU integration.

This is eligibility evidence, not code-size savings or instruction correctness.
Every accepted instruction word must also match the original game ELF bytes.
"""
from pathlib import Path
import hashlib
import json
import re
import struct
import os
import datetime
from concurrent.futures import ThreadPoolExecutor
root=Path(__file__).resolve().parents[1]
out=root/'evidence/47M';out.mkdir(exist_ok=True)
elfpath=root/'work/native/project/disc/SLUS_202.68';elf=elfpath.read_bytes()
assert elf[:6]==b'\x7fELF\x01\x01'
phoff=struct.unpack_from('<I',elf,28)[0];phsize,phcount=struct.unpack_from('<HH',elf,42)
segments=[]
for i in range(phcount):
    kind,offset,address,_,size=struct.unpack_from('<IIIII',elf,phoff+i*phsize)
    if kind==1:segments.append((address,address+size,offset))
def word(pc):
    for begin,end,offset in segments:
        if begin<=pc and pc+4<=end:return struct.unpack_from('<I',elf,offset+pc-begin)[0]
    return None
tables=(root/'third_party/pcsx2-mmi47k/R5900OpcodeTables.cpp').read_text()
allowed=set('ADDIU DADDIU ANDI ORI XORI SLTI SLTIU ADDU DADDU SUBU DSUBU AND OR XOR NOR SLT SLTU DIV DIVU MULT MULTU LUI MFHI MFLO MTHI MTLO SRA SRL SLL DSLL DSLL32 DSRA DSRA32 DSRL DSRL32 SLLV SRAV SRLV DSLLV DSRAV DSRLV MOVZ MOVN MFSA MTSA MTSAB MTSAH'.split())
def table(name,size):
    body=re.search(r'tbl_'+name+r'\['+str(size)+r'\]\s*=\s*\{(.*?)\};',tables,re.S)[1]
    values=re.findall(r'\b\w+\b',body);assert len(values)==size
    return values
decode={'standard':table('Standard',64),'special':table('Special',64),'regimm':table('RegImm',32),'mmi':table('MMI',64)}
for i in range(4):decode['MMI'+str(i)]=table('MMI'+str(i),32)
def operation(code):
    name=decode['standard'][code>>26]
    if name=='SPECIAL':name=decode['special'][code&63]
    elif name=='REGIMM':name=decode['regimm'][(code>>16)&31]
    elif name=='MMI':
        name=decode['mmi'][code&63]
        if name in ('MMI0','MMI1','MMI2','MMI3'):name=decode[name][(code>>6)&31]
        if name not in ('MMI_Unknown','QFSRV'):return name
        return None # Native SA convention requires an explicit later adapter audit.
    return name if name in allowed else None
def examine(path):
    file=Path(path);raw=file.read_bytes();text=raw.decode()
    if any(token in text for token in ('READ8(', 'READ16(', 'READ32(', 'READ64(', 'READ128(',
        'WRITE8(', 'WRITE16(', 'WRITE32(', 'WRITE64(', 'WRITE128(', 'runtime->eeCheckpointDue(',
        'GuestBranchKind::DirectCall', 'GuestBranchKind::IndirectCall')):return None,0
    symbol=re.search(r'void (\w+_0x([0-9a-f]+))\(',text)
    if not symbol:return None,0
    entry=int(symbol[2],16)
    pairs=[(int(pc,16),int(code,16)) for pc,code in re.findall(r'// (0x[0-9a-f]+): (0x[0-9a-f]+)\s',text)]
    if not 2<=len(pairs)<=128 or len(set(pc for pc,_ in pairs))!=len(pairs):return None,0
    pairs.sort()
    if [pc for pc,_ in pairs]!=list(range(entry,entry+4*len(pairs),4)):return None,0
    if pairs[-2][1]!=0x03e00008:return None,0
    ops=[operation(code) for _,code in pairs[:-2]+pairs[-1:]]
    if any(op is None or op in ('MFSA','MTSA','MTSAB','MTSAH') for op in ops):return None,0
    if any(word(pc)!=code for pc,code in pairs):return None,1
    # Only entry and final JR are allowed: no memory, calls, backedges, traps,
    # syscalls or coprocessor work can hide in this first compact integration.
    return dict(path=str(file.relative_to(root)),symbol=symbol[1],entry=entry,
        end=entry+len(pairs)*4,source_sha256=hashlib.sha256(raw).hexdigest(),
        words=[code for _,code in pairs],operations=ops,
        delay_is_nop=pairs[-1][1]==0,uses_mmi=any(code>>26==0x1c for _,code in pairs),
        original_sets_delay_state='ctx->in_delay_slot = true;' in text),0
started=datetime.datetime.now(datetime.timezone.utc).isoformat()
with os.scandir(root/'work/native/project/generated') as entries:
    paths=sorted(entry.path for entry in entries if entry.name.endswith('.cpp') and entry.stat().st_size<=32768)
accepted=[];rejected_elf=0;seen=len(paths)
with ThreadPoolExecutor(max_workers=4) as pool:
    for index,(record,rejected) in enumerate(pool.map(examine,paths)):
        if record:accepted.append(record)
        rejected_elf+=rejected
        if index%5000==0:print('Examined',index,'of',seen,'eligible',len(accepted),flush=True)
report=dict(started_utc=started,finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
    elf_sha256=hashlib.sha256(elf).hexdigest(),small_generated_files_examined=seen,
    count=len(accepted),instruction_words=sum(len(a['words']) for a in accepted),
    mmi_functions=sum(a['uses_mmi'] for a in accepted),elf_mismatches_rejected=rejected_elf,
    functions=accepted,caveat='Not a complete CPU backend, removed machine-code measurement or FPS claim; candidate bodies require direct original-function and native-game validation.')
(out/'leaf-eligibility.json').write_text(json.dumps(report,indent=2))
print(json.dumps({k:v for k,v in report.items() if k!='functions'},indent=2))
for a in accepted[:8]:print(a['symbol'],len(a['words']),a['operations'])
