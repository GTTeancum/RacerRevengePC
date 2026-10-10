"""Extract non-trapping PCSX2 scalar handlers for compact real-game leaves."""
from pathlib import Path
import hashlib
import json
import re
root=Path(__file__).resolve().parents[1]
upstream=root/'work/reference/pcsx2-master/pcsx2/R5900OpcodeImpl.cpp'
vendor=root/'third_party/pcsx2-scalar47m';vendor.mkdir(exist_ok=True)
target=vendor/'R5900OpcodeImpl.cpp'
if target.exists():assert target.read_bytes()==upstream.read_bytes()
else:target.write_bytes(upstream.read_bytes())
source=target.read_text()
names='ADDIU DADDIU ANDI ORI XORI SLTI SLTIU ADDU DADDU SUBU DSUBU AND OR XOR NOR SLT SLTU DIV DIVU MULT MULTU LUI MFHI MFLO MTHI MTLO SRA SRL SLL DSLL DSLL32 DSRA DSRA32 DSRL DSRL32 SLLV SRAV SRLV DSLLV DSRAV DSRLV MOVZ MOVN MFSA MTSA MTSAB MTSAH'.split()
def extract(name):
    match=re.search(r'\bvoid\s+'+name+r'\s*\(\s*\)\s*\{',source);assert match,name
    start=match.start();at=match.end();depth=1
    while depth:
        if source[at]=='{':depth+=1
        elif source[at]=='}':depth-=1
        at+=1
    return source[start:at]
bodies={name:extract(name) for name in names}
tables=(root/'third_party/pcsx2-mmi47k/R5900OpcodeTables.cpp').read_text()
def table(name,size):
    values=re.findall(r'\b\w+\b',re.search(r'tbl_'+name+r'\['+str(size)+r'\]\s*=\s*\{(.*?)\};',tables,re.S)[1])
    assert len(values)==size
    return values
standard=table('Standard',64);special=table('Special',64);regimm=table('RegImm',32)
assert set(names).issubset(set(standard+special+regimm))
notice='// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team\n// SPDX-License-Identifier: GPL-3.0+\n'
glue='#include "../../third_party/pcsx2-mmi47k/mmi_compat.h"\n#include "scalar47m.h"\n#define _Imm_ static_cast<s16>(cpuRegs.code)\n#define _ImmU_ static_cast<u16>(cpuRegs.code)\n'
ported=notice+glue+'namespace rr_scalar47m {\n'
for name,body in bodies.items():
    ported+=re.sub(r'void\s+'+name+r'\s*\(\s*\)',f'void {name}(rr::MmiState& cpuRegs)',body)+'\n'
ported+='}\n'
def route(reference=False):
    namespace='rr_scalar_reference47m' if reference else 'rr_scalar47m'
    arg='' if reference else 'cpuRegs'
    lines=['    switch(instruction>>26) {']
    def call(name):return f'{namespace}::{name}({arg}); return true;'
    for major,name in enumerate(standard):
        if name in names:lines.append(f'    case {major}: '+call(name))
        elif name in ('SPECIAL','REGIMM'):
            values=special if name=='SPECIAL' else regimm
            mask='instruction&63u' if name=='SPECIAL' else '(instruction>>16)&31u'
            lines.append(f'    case {major}: switch({mask}) {{')
            for sub,op in enumerate(values):
                if op in names:lines.append(f'        case {sub}: '+call(op))
            lines.append('        default: return false; }')
    lines+=['    default: return false;', '    }']
    return '\n'.join(lines)
# Probe supported status on an immutable opcode table before changing code.
valid=re.sub(r'rr_scalar47m::\w+\(cpuRegs\); return true;', 'return true;', route())
ported+='static bool supported(uint32_t instruction) {\n'+valid+'\n}\n'
ported+='bool rr::executeScalar47m(MmiState& cpuRegs,uint32_t instruction) noexcept {\n    if(!supported(instruction)) return false;\n    cpuRegs.code=instruction;\n'+route()+'\n}\n'
(root/'src/core/scalar47m.cpp').write_text(ported)
# The reference keeps each extracted upstream function body byte-for-byte;
# it changes only their enclosing namespace and supplies the original global CPU.
reference=notice+glue.replace('../../','../')+'namespace rr_scalar_reference47m {\nrr::MmiState cpuRegs{};\n'+'\n'.join(bodies.values())+'\n}\n'
reference+='bool referenceScalar47m(rr::MmiState& state,uint32_t instruction) noexcept {\n    rr_scalar_reference47m::cpuRegs=state;\n    rr_scalar_reference47m::cpuRegs.code=instruction;\n    auto run=[&]() {\n'+route(True)+'\n    };\n    const bool ok=run();if(ok) state=rr_scalar_reference47m::cpuRegs;return ok;\n}\n'
(root/'tests/scalar_reference47m.cpp').write_text(reference)
(vendor/'extracted-original.cpp.txt').write_text(notice+'\n'.join(bodies.values())+'\n')
(vendor/'provenance.json').write_text(json.dumps(dict(
    origin=str(upstream.relative_to(root)),upstream_sha256=hashlib.sha256(target.read_bytes()).hexdigest(),
    handlers=names,license='GPL-3.0-or-later; COPYING.GPLv3 preserved in ../pcsx2-mmi47k',
    extraction='Selected function bodies verbatim. Production adds explicit CPU state; reference retains original global-state calls.'),indent=2))
print('Prepared',len(names),'scalar handlers from the supplied PCSX2 source.')
