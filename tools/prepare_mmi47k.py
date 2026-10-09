"""Reproduce the context-based scalar MMI port from the user-supplied PCSX2.

Instruction bodies stay unchanged. Helpers/handlers receive an explicit CPU
reference. The test reference compiles the byte-identical upstream MMI.cpp with
small type/bit-count shims; it retains upstream global-state calling conventions.
"""
from pathlib import Path
import hashlib
import json
import re
import shutil

root = Path(__file__).resolve().parents[1]
upstream = root / 'work/reference/pcsx2-master'
vendor = root / 'third_party/pcsx2-mmi47k'
vendor.mkdir(parents=True, exist_ok=True)
for source in ('pcsx2/MMI.cpp', 'pcsx2/R5900OpcodeTables.cpp', 'common/BitUtils.h', 'COPYING.GPLv3'):
    target = vendor / Path(source).name
    data = (upstream / source).read_bytes()
    if target.exists():
        assert target.read_bytes() == data, 'Refuse to replace a different upstream snapshot'
    else:
        target.write_bytes(data)
text = (vendor / 'MMI.cpp').read_text()
definitions = list(re.finditer(r'\bvoid\s+(\w+)\s*\(([^)]*)\)\s*\{', text))
names = [m[1] for m in definitions]
assert len(names) == len(set(names))
handlers = [m[1] for m in definitions if not m[2].strip()]
helpers = [n for n in names if n not in handlers and n != 'PMFHL_CLAMP']
contexts = set(handlers + helpers)
tables = {}
optext = (vendor / 'R5900OpcodeTables.cpp').read_text()
for name, size in [('MMI', 64), ('MMI0', 32), ('MMI1', 32), ('MMI2', 32), ('MMI3', 32)]:
    body = re.search(r'tbl_' + name + r'\[' + str(size) + r'\]\s*=\s*\{(.*?)\};', optext, re.S)[1]
    values = re.findall(r'\b\w+\b', body)
    assert len(values) == size
    tables[name] = values
mapped = set(sum(tables.values(), [])) - {'MMI0', 'MMI1', 'MMI2', 'MMI3', 'MMI_Unknown'}
assert mapped == set(handlers), (mapped - set(handlers), set(handlers) - mapped)

compat = '''// Adaptation glue; scalar equivalent of PCSX2 CountLeadingSignBits.
#pragma once
#include "mmi.h"
using u8=uint8_t; using s8=int8_t; using u16=uint16_t; using s16=int16_t;
using u32=uint32_t; using s32=int32_t; using u64=uint64_t; using s64=int64_t;
using GPR_reg=rr::MmiRegister;
#define __fi inline
#define _Rs_ ((cpuRegs.code >> 21) & 31u)
#define _Rt_ ((cpuRegs.code >> 16) & 31u)
#define _Rd_ ((cpuRegs.code >> 11) & 31u)
#define _Sa_ ((cpuRegs.code >> 6) & 31u)
namespace Common {
inline u32 CountLeadingSignBits(s32 n) {
    u32 word=static_cast<u32>(n); if(n<0) word=~word;
    u32 count=0; for(u32 mask=0x80000000u; mask && !(word&mask); mask>>=1) ++count;
    return count;
}
}
'''
(vendor / 'mmi_compat.h').write_text(compat)
shim = vendor / 'reference_shim'
(shim / 'common').mkdir(parents=True, exist_ok=True)
(shim / 'Common.h').write_text('#pragma once\n#include "../mmi_compat.h"\nextern rr::MmiState cpuRegs;\n')
(shim / 'common/BitUtils.h').write_text('// CountLeadingSignBits is supplied by the scalar compatibility shim.\n')

ported = text.replace('#include "Common.h"\n#include "common/BitUtils.h"', '#include "../../third_party/pcsx2-mmi47k/mmi_compat.h"')
ported = ported.replace('namespace R5900 {', 'namespace rr_mmi47k {')
# Insert a context into definitions first; then into calls, excluding the just
# modified definitions. No instruction expression is otherwise rewritten.
for name in sorted(contexts, key=len, reverse=True):
    ported = re.sub(r'(\bvoid\s+' + name + r'\s*\()([^)]*)\)',
                    lambda m: m[1] + 'rr::MmiState& cpuRegs' + (', ' + m[2] if m[2].strip() else '') + ')', ported)
    ported = re.sub(r'\b' + name + r'\((?!rr::MmiState&)([^)]*)\)',
                    lambda m: name + '(cpuRegs' + (', ' + m[1] if m[1].strip() else '') + ')', ported)

def routes(reference):
    lines = []
    prefix = 'R5900' if reference else 'rr_mmi47k'
    outer = set(handlers[:handlers.index('PLZCW')])
    def handler(name):
        namespace = prefix + '::Interpreter::OpcodeImpl::' + ('' if name in outer else 'MMI::')
        return namespace + name + ('(); return true;' if reference else '(cpuRegs); return true;')
    lines.append('    switch (instruction & 63u) {')
    for op, name in enumerate(tables['MMI']):
        if name == 'MMI_Unknown': continue
        if name in tables:
            lines.append(f'    case {op}: switch ((instruction >> 6) & 31u) {{')
            for sub, subname in enumerate(tables[name]):
                if subname != 'MMI_Unknown': lines.append(f'        case {sub}: ' + handler(subname))
            lines.append('        default: return false; }')
        else:
            lines.append(f'    case {op}: ' + handler(name))
    lines += ['    default: return false;', '    }']
    return '\n'.join(lines)

# Validate before assigning code so all reserved encodings leave the entire
# state intact. No speculative handler runs on a rejected opcode.
valid = ['static bool validMmi(uint32_t instruction) {', '    if ((instruction >> 26) != 0x1cu) return false;', '    switch (instruction & 63u) {']
for op, name in enumerate(tables['MMI']):
    if name == 'MMI_Unknown': continue
    if name in tables:
        mask = sum(1 << i for i, h in enumerate(tables[name]) if h != 'MMI_Unknown')
        valid.append(f'    case {op}: return ((0x{mask:08x}u >> ((instruction >> 6) & 31u)) & 1u) != 0;')
    else: valid.append(f'    case {op}: return true;')
valid += ['    default: return false;', '    }', '}']
ported += '\n' + '\n'.join(valid) + '\n'
ported += 'bool rr::executeMmi(MmiState& cpuRegs, uint32_t instruction) noexcept {\n    if (!validMmi(instruction)) return false;\n    cpuRegs.code=instruction;\n' + routes(False) + '\n}\n'
(root / 'src/core/mmi47k.cpp').write_text(ported)

ref = '#include "../third_party/pcsx2-mmi47k/MMI.cpp"\nrr::MmiState cpuRegs{};\n'
ref += 'bool referenceMmi47k(rr::MmiState& state, uint32_t instruction) noexcept {\n    if((instruction >> 26)!=0x1cu) return false;\n    cpuRegs=state; cpuRegs.code=instruction;\n    auto run=[&]() {\n' + routes(True) + '\n    };\n    bool handled=run(); if(handled) state=cpuRegs; return handled;\n}\n'
(root / 'tests/mmi_reference47k.cpp').write_text(ref)
records = []
for p in sorted(vendor.rglob('*')):
    if p.is_file() and p.name != 'provenance.json':
        records.append(dict(path=str(p.relative_to(root)).replace('\\','/'), bytes=p.stat().st_size,
                            sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
(vendor / 'provenance.json').write_text(json.dumps(dict(handlers=len(handlers), helpers=len(helpers),
    origin='User-supplied work/reference/pcsx2-master; no remote revision inferred',
    license='GPL-3.0-or-later; upstream notices retained', files=records), indent=2))
print(f'Prepared {len(handlers)} PCSX2 scalar MMI handlers and {len(helpers)} context helpers.')
