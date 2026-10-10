"""Compare the complete x86 IOP against existing x64 evidence; audit its objects."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import struct
import subprocess
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--evidence',type=Path,required=True);args=p.parse_args()
out=args.evidence.resolve()/'checks';out.mkdir(exist_ok=False)
build=root/'build/iop47J2-x86'
records=[]
env=os.environ.copy()
for key in ('RR_IOP_IDLE_STATS','RR_IOP_READY_STATS47G','RR_IOP_IMPORT_STATS47I'):
    env.pop(key,None)
def executable(suffix):
    exe=build/f'racer_iop47j2_{suffix}.exe';raw=exe.read_bytes()
    at=struct.unpack_from('<I',raw,0x3c)[0]
    assert raw[at:at+4]==b'PE\0\0' and struct.unpack_from('<H',raw,at+4)[0]==0x14c, 'Not x86 PE'
    return exe,hashlib.sha256(raw).hexdigest()
def normalized(text):
    return re.sub(r'cache_bytes=\d+','cache_bytes=HOST_LAYOUT',text).splitlines()
for suffix,reference in [('memory','memory-final'),('scheduler','scheduler-final'),
                         ('import_cache','import-cache'),('differential','differential')]:
    exe,digest=executable(suffix)
    result=subprocess.run([str(exe)],env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=180)
    (out/(suffix+'.log')).write_bytes(result.stdout)
    (out/(suffix+'.exit')).write_text(str(result.returncode))
    expected=(root/f'evidence/47J2/{reference}.log').read_text(encoding='utf-8-sig')
    match=normalized(result.stdout.decode('utf-8-sig'))==normalized(expected)
    record=dict(test=suffix,exit=result.returncode,exe_sha256=digest,matches_x64_except_layout_size=match)
    records.append(record);print(json.dumps(record),flush=True)
for mode in ('0','1'):
    for suffix in ('emulator','import','import_version','compatibility'):
        env.update(RR_IOP_IDLE_BATCH='1',RR_IOP_READY_CACHE47G='1',RR_IOP_IMPORT_CACHE47I=mode)
        exe,digest=executable(suffix)
        result=subprocess.run([str(exe)],env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=180)
        name=f'ps2_iop_{suffix}_tests-{mode}'
        (out/(name+'.log')).write_bytes(result.stdout)
        (out/(name+'.exit')).write_text(str(result.returncode))
        frozen=root/f'evidence/47B/iop-regressions-b2/{name}'
        expected_exit=int(frozen.with_suffix('.exit').read_text(encoding='utf-8-sig'))
        record=dict(test=name,exit=result.returncode,expected_exit=expected_exit,exe_sha256=digest,
                    matches_frozen45Y_via47B2=result.stdout==frozen.with_suffix('.log').read_bytes())
        records.append(record);print(json.dumps(record),flush=True)
# Audit only project IOP implementation objects, not CRT/DLL dispatch routines.
objdump=root/'toolchains/msys64/mingw64/bin/llvm-objdump.exe'
objects=sorted((build/'CMakeFiles/rr_iop47j2.dir').rglob('*.obj'))
assert len(objects)==24,len(objects)
sse1=set('movaps movups movss movlps movhps movhlps movlhps movntps movmskps addps addss subps subss mulps mulss divps divss minps minss maxps maxss sqrtps sqrtss rsqrtps rsqrtss rcpps rcpss andps andnps orps xorps shufps unpckhps unpcklps comiss ucomiss cvtsi2ss cvtss2si cvttss2si cvtpi2ps cvtps2pi cvttps2pi'.split())
newer=set('lfence mfence clflush lzcnt tzcnt popcnt bextr andn movbe crc32 rdrand rdseed pdep pext mulx sarx shrx shlx bzhi rorx fisttp'.split())
audit=[];forbidden=[];instructions=0
for obj in objects:
    result=subprocess.run([str(objdump),'-d','--no-show-raw-insn','--x86-asm-syntax=intel',str(obj)],
                          stdout=subprocess.PIPE,stderr=subprocess.PIPE,check=True)
    log=out/(obj.name+'.disasm.txt');log.write_bytes(result.stdout)
    n=0;xmm=0
    for line in result.stdout.decode(errors='replace').splitlines():
        m=re.match(r'^\s*[0-9a-f]+:\s+([a-z][a-z0-9]*)\s*(.*)$',line)
        if not m:continue
        op,operands=m.groups();n+=1
        has_xmm=bool(re.search(r'\bxmm\d+\b',operands));xmm+=has_xmm
        if ((has_xmm and op not in sse1) or op in newer or (op.startswith('v') and op not in ('verr','verw'))
            or re.search(r'\b[yz]mm\d+\b',operands)):
            forbidden.append(dict(object=str(obj.relative_to(root)),instruction=line.strip()))
    assert n,obj
    instructions+=n
    audit.append(dict(path=str(obj.relative_to(root)),sha256=hashlib.sha256(obj.read_bytes()).hexdigest(),
                      instructions=n,xmm_instructions=xmm,disassembly=log.name))
report=dict(tests=records,objects=audit,instructions=instructions,forbidden_instructions=forbidden,
    scope='24 project IOP objects, x86 PE test binaries. Excludes CRT and DLLs; not an Xbox build/boot or performance test.')
(out/'report.json').write_text(json.dumps(report,indent=2))
ok=not forbidden and all((r['exit']==0 and r['matches_x64_except_layout_size']) if 'matches_x64_except_layout_size' in r
    else (r['exit']==r['expected_exit'] and r['matches_frozen45Y_via47B2']) for r in records)
print(json.dumps(dict(success=ok,objects=len(objects),instructions=instructions,forbidden=len(forbidden))),flush=True)
raise SystemExit(0 if ok else 1)
