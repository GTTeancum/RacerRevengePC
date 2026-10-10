"""Inventory emitted guest instruction duplication; never edits generated code."""
from pathlib import Path
from collections import Counter
import argparse
import hashlib
import json
import re

parser = argparse.ArgumentParser()
parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
generated = args.root / 'work/native/project/generated'
instruction = re.compile(rb'// 0x([0-9a-fA-F]+): 0x([0-9a-fA-F]+)\s')
counts = Counter()
opcodes = {}
conflicts = []
files = []
for path in sorted(generated.glob('*.cpp')):
    if path.name == 'register_functions.cpp':
        continue
    data = path.read_bytes()
    occurrences = instruction.findall(data)
    if not occurrences:
        continue
    files.append({'path': path.relative_to(args.root).as_posix(), 'bytes': len(data),
                  'sha256': hashlib.sha256(data).hexdigest(), 'instruction_comments': len(occurrences)})
    for address, opcode in occurrences:
        pc, word = int(address, 16), int(opcode, 16)
        counts[pc] += 1
        if pc in opcodes and opcodes[pc] != word:
            conflicts.append({'pc': hex(pc), 'old': hex(opcodes[pc]), 'new': hex(word), 'path': files[-1]['path']})
        opcodes[pc] = word
report = {
    'scope': 'Base generated .cpp instruction comments only; not recovered14 code or dynamic execution',
    'warning': 'Static duplication inventory, not a safe deletion list or an FPS forecast. Resume ownership, call boundaries and timing require separate validation.',
    'files': files, 'unique_instruction_addresses': len(counts),
    'emitted_instruction_comments': sum(counts.values()),
    'address_replication_ratio': sum(counts.values()) / max(1, len(counts)),
    'opcode_conflicts': conflicts,
    'most_repeated_addresses': [{'pc': hex(pc), 'copies': count} for pc, count in counts.most_common(25)],
}
args.output.parent.mkdir(parents=True, exist_ok=True)
with args.output.open('x', encoding='utf-8') as f:
    json.dump(report, f, indent=2)
print(json.dumps({k: v for k, v in report.items() if k not in ('files', 'most_repeated_addresses', 'opcode_conflicts')}))
print(f'Opcode conflicts: {len(conflicts)}; report: {args.output}')
