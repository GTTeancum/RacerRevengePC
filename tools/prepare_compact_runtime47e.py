"""Prepare a compact Windows runtime adapter without editing the reference."""
from pathlib import Path
import hashlib
import json
import struct

root=Path(__file__).resolve().parents[1]
source=root/'build/registry47D'
out=root/'build/compact47E'
assert not out.exists(), 'Preserve previous candidates'
out.mkdir(parents=True)
report=json.loads((source/'report.json').read_text())
metadata='#pragma once\n#include <stdint.h>\nnamespace rr47e {\n'
for name,key in [('Base','base'),('Slots','slots'),('Pages','pages'),('Patterns','patterns'),('Owners','owner_ids'),('Symbols','symbols')]:
    metadata+=f'constexpr uint32_t k{name}={report[key]}u;\n'
(out/'registry_metadata47e.h').write_text(metadata+'}\n')
lines=['#include "ps2_runtime.h"','#include "compact_registry.h"',
       '#ifdef RR_COMPACT_DEFINE_TABLE_METADATA',
       f'extern const uint32_t g_ps2RecompiledFunctionTableBase={report["base"]}u;',
       f'extern const uint32_t g_ps2RecompiledFunctionTableEnd={report["end_exclusive"]}u;',
       f'extern const uint32_t g_ps2RecompiledFunctionTableSlotCount={report["slots"]}u;',
       '#endif','extern const rr::RegistryPage rr47e_pages[]={']
lines += ['{%du,%du,%du},'%p for p in struct.iter_unpack('<IHH',(source/'pages.bin').read_bytes())]
lines+=['};','extern const uint8_t rr47e_patterns[]={']
data=(source/'patterns.bin').read_bytes()
lines += [','.join(str(n) for n in data[i:i+64])+',' for i in range(0,len(data),64)]
lines+=['};','extern const uint16_t rr47e_owners[]={']
owners=[p[0] for p in struct.iter_unpack('<H',(source/'owners.bin').read_bytes())]
lines += [','.join(str(n) for n in owners[i:i+32])+',' for i in range(0,len(owners),32)]
lines+=['};','']
bindings=(source/'bindings.cpp').read_text().replace('PS2Runtime::RecompiledFunction registry47d_bindings[]',
    'extern PS2Runtime::RecompiledFunction const rr47e_bindings[]')
lines+=[bindings]
(out/'registry_data47e.cpp').write_text('\n'.join(lines),encoding='utf-8',newline='\n')
runtime=root/'work/native/sources/src/PS2Recomp/ps2xRuntime/src/lib/ps2_runtime.cpp'
text=runtime.read_text()
for before,after in [
    ('#include "ps2_runtime.h"','#include "ps2_runtime.h"\n#include "registry_adapter47e.h"'),
    ('    g_ps2RecompiledFunctionTable[slot] = func;\n    return true;',
     '    return rr47e::replace(address,func);'),
    ('return generatedFunctionTableSlot(address, slot) && g_ps2RecompiledFunctionTable[slot] != nullptr;',
     'return generatedFunctionTableSlot(address, slot) && rr47e::lookup(address) != nullptr;'),
    ('RecompiledFunction fn = g_ps2RecompiledFunctionTable[slot];',
     'RecompiledFunction fn = rr47e::lookup(address);')]:
    assert text.count(before)==1,before
    text=text.replace(before,after)
(out/'ps2_runtime.cpp').write_text(text,encoding='utf-8',newline='\n')
probe=root/'work/native/project/src/probe.cpp'
text=probe.read_text().replace('#include "ps2_runtime.h"','#include "ps2_runtime.h"\n#include "registry_adapter47e.h"')
before='        rr_observe::report();'
assert text.count(before)==1
text=text.replace(before,'''        std::cerr<<"[rr:compact-registry] pages="<<rr47e::allocatedPages()
                 <<" directory_bytes="<<rr47e::directoryBytes()
                 <<" replacement_bytes="<<rr47e::replacementBytes()<<"\\n";
'''+before)
(out/'probe.cpp').write_text(text,encoding='utf-8',newline='\n')
paths=[runtime,probe,source/'report.json']
manifest={'original_sources_modified':False,'native_integration':'prepared; not yet built or accepted',
    'inputs':[{'path':p.relative_to(root).as_posix(),'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in paths],
    'outputs':[{'path':p.name,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in sorted(out.iterdir())]}
(out/'provenance.json').write_text(json.dumps(manifest,indent=2))
print(out)
