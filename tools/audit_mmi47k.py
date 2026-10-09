"""Audit project MMI COFF objects and retain complete disassembly/section sizes."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
import subprocess

root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('output',type=Path);a=p.parse_args()
a.output.mkdir(parents=True,exist_ok=False)
objects=[root/'src/core/mmi47k.obj',root/'tests/mmi_reference47k.obj',root/'tests/mmi_suite47k.obj',
    root/'build/mmi47K-x86/CMakeFiles/racer_mmi47k.dir/src/core/mmi47k.cpp.obj']
sse1=set('movaps movups movss movlps movhps movhlps movlhps movntps movmskps addps addss subps subss mulps mulss divps divss minps minss maxps maxss sqrtps sqrtss rsqrtps rsqrtss rcpps rcpss andps andnps orps xorps shufps unpckhps unpcklps comiss ucomiss cvtsi2ss cvtss2si cvttss2si cvtpi2ps cvtps2pi cvttps2pi'.split())
newer=set('lfence mfence clflush lzcnt tzcnt popcnt bextr andn movbe crc32 rdrand rdseed pdep pext mulx sarx shrx shlx bzhi rorx fisttp'.split())
records=[];forbidden=[]
for index,obj in enumerate(objects):
    raw=obj.read_bytes(); machine,count=struct.unpack_from('<HH',raw)
    assert machine==0x14c, 'Expected x86 COFF'
    optional=struct.unpack_from('<H',raw,16)[0]
    sections=[]
    for n in range(count):
        at=20+optional+n*40
        name=raw[at:at+8].split(b'\0')[0].decode()
        size=struct.unpack_from('<I',raw,at+16)[0]
        flags=struct.unpack_from('<I',raw,at+36)[0]
        sections.append(dict(name=name,bytes=size,code=bool(flags&0x20)))
    result=subprocess.run([str(root/'toolchains/msys64/mingw64/bin/llvm-objdump.exe'),'-d','--no-show-raw-insn','--x86-asm-syntax=intel',str(obj)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,check=True)
    log=f'{index}-{obj.name}.disasm.txt';(a.output/log).write_bytes(result.stdout)
    instructions=0
    for line in result.stdout.decode().splitlines():
        match=re.match(r'^\s*[0-9a-f]+:\s+([a-z][a-z0-9]*)\s*(.*)$',line)
        if not match: continue
        op,args=match.groups();instructions+=1
        if ((re.search(r'\bxmm\d+\b',args) and op not in sse1) or op in newer
            or (op.startswith('v') and op not in ('verr','verw')) or re.search(r'\b[yz]mm\d+\b',args)):
            forbidden.append(dict(object=str(obj.relative_to(root)),instruction=line.strip()))
    assert instructions
    records.append(dict(path=str(obj.relative_to(root)),sha256=hashlib.sha256(raw).hexdigest(),
        bytes=len(raw),code_bytes=sum(s['bytes'] for s in sections if s['code']),sections=sections,
        instructions=instructions,disassembly=log))
report=dict(objects=records,forbidden_instructions=forbidden,
    scope='Project MMI nxdk objects and MSVC x86 production object only; external helpers/CRT/Xbox execution are not validated by this disassembly scan.')
(a.output/'report.json').write_text(json.dumps(report,indent=2))
print(json.dumps(dict(objects=[{k:r[k] for k in ('path','code_bytes','instructions')} for r in records],forbidden=len(forbidden)),indent=2))
raise SystemExit(bool(forbidden))
