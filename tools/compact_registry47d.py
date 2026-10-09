"""Encode the existing exact-PC registry, retaining original symbol ownership.

This prepares an offline candidate. It does not modify or replace the game.
"""
from pathlib import Path
import hashlib
import json
import re
import struct

root = Path(__file__).resolve().parents[1]
out = root / 'build/registry47D'
assert not out.exists(), 'Keep prior generated candidates immutable'
out.mkdir(parents=True)
project = root / 'work/native/project'
paths = [project / p for p in ['generated/register_functions.cpp',
    'src/resume-registration.inc', 'src/recovered10/register.cpp',
    'src/recovered14/register.cpp']]
texts = [p.read_text() for p in paths]
base = int(re.search(r'g_ps2RecompiledFunctionTableBase = (0x[0-9a-f]+)',texts[0])[1],16)
end = int(re.search(r'g_ps2RecompiledFunctionTableEnd = (0x[0-9a-f]+)',texts[0])[1],16)
count = int(re.search(r'g_ps2RecompiledFunctionTableSlotCount = (\d+)',texts[0])[1])
assert base + count*4 == end
table = [None] * count
assign = re.compile(r'g_ps2RecompiledFunctionTable\[(\d+)\]\s*=\s*(\w+)\s*;\s*//\s*(0x[0-9a-fA-F]+)')
layers = []
for number, text in enumerate(texts):
    matches = assign.findall(text)
    added = 0
    for slot_s, name, pc_s in matches:
        slot, pc = int(slot_s), int(pc_s,16)
        assert 0 <= slot < count and base+slot*4 == pc
        assert re.fullmatch(r'[A-Za-z_]\w*',name)
        if number == 1 and table[slot] is not None:
            continue  # Existing resume initializer explicitly fills only holes.
        assert table[slot] is None, f'Unexpected initializer collision: {slot}'
        table[slot] = name
        added += 1
    assert matches
    layers.append({'source':paths[number].relative_to(root).as_posix(),
        'sha256':hashlib.sha256(paths[number].read_bytes()).hexdigest(),
        'assignments':len(matches),'added':added})
symbols = sorted({name for name in table if name is not None})
assert len(symbols) < 65536, 'Widen global IDs explicitly if required'
ids = {name:n+1 for n,name in enumerate(symbols)}
dense = [ids.get(name,0) for name in table]

# A page's local byte codes refer to its small list of global symbol IDs.
# Repeated byte patterns share storage even when their symbol owners differ.
patterns, pattern_ids, owners, pages = [], {}, [], []
for start in range(0,count,64):
    slots = dense[start:start+64]
    slots += [0]*(64-len(slots))
    local = [0] + list(dict.fromkeys(n for n in slots if n))
    codes = bytes(local.index(n) for n in slots)
    if codes not in pattern_ids:
        pattern_ids[codes] = len(patterns)
        patterns.append(codes)
    pages.append((len(owners),pattern_ids[codes],len(local)))
    owners.extend(local)
assert len(patterns) < 65536
page_bytes = b''.join(struct.pack('<IHH',*p) for p in pages)
pattern_bytes = b''.join(patterns)
owner_bytes = struct.pack('<'+'H'*len(owners),*owners)
dense_bytes = struct.pack('<'+'H'*count,*dense)
for slot, expected in enumerate(dense):
    owner_base,pattern_id,owner_count = pages[slot//64]
    local_id = patterns[pattern_id][slot%64]
    assert local_id < owner_count
    assert owners[owner_base+local_id] == expected, hex(base+slot*4)
for name,data in [('pages.bin',page_bytes),('patterns.bin',pattern_bytes),
                  ('owners.bin',owner_bytes),('expected.bin',dense_bytes)]:
    (out / name).write_bytes(data)
(out / 'symbols.json').write_text(json.dumps(symbols,indent=2))

# Bind IDs to actual original function symbols for an independent linked check
# against the canonical table initialized by the original legacy translation units.
binding = ['// Generated symbol bindings; no guest function implementations.',
           '#include "ps2_runtime.h"']
binding += ['extern void '+s+'(uint8_t*,R5900Context*,PS2Runtime*);' for s in symbols]
binding += ['PS2Runtime::RecompiledFunction registry47d_bindings[] = {nullptr,']
binding += [s+',' for s in symbols]
binding += ['};','']
(out / 'bindings.cpp').write_text('\n'.join(binding),encoding='utf-8',newline='\n')
index_bytes = len(page_bytes)+len(pattern_bytes)+len(owner_bytes)
report = {'status':'offline encoder verified at every slot; linked native comparison pending',
    'base':base,'end_exclusive':end,'slots':count,'registered_slots':sum(bool(n) for n in dense),
    'symbols':len(symbols),'pages':len(pages),'patterns':len(patterns),'owner_ids':len(owners),
    'layers':layers,'page_bytes':len(page_bytes),'pattern_bytes':len(pattern_bytes),
    'owner_bytes':len(owner_bytes),'index_bytes':index_bytes,
    'x86_binding_bytes':4*(len(symbols)+1),'x86_dense_pointer_bytes':4*count,
    'x86_compact_total_bytes':index_bytes+4*(len(symbols)+1),
    'verification_slots':count,'native_game_integration':False,
    'warning':'Read-only registry only. Runtime replacements need a separate overlay; no machine code reduction or FPS improvement claimed.',
    'outputs':[{'path':p.name,'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()}
               for p in sorted(out.iterdir()) if p.is_file()]}
(out / 'report.json').write_text(json.dumps(report,indent=2))
print(json.dumps({k:v for k,v in report.items() if k not in ('outputs','layers')},indent=2))
