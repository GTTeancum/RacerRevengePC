"""Freeze exact original-word placement/composition fixtures for native collision."""
from pathlib import Path
import hashlib
import json
import random
import re
import struct
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/gameplay'))
from query_oracle48h import Original, bits, f32

OUT = ROOT / 'evidence/collisionMesh48I'
def sha(data): return hashlib.sha256(data).hexdigest()

def main():
    OUT.mkdir(exist_ok=True)
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    assert sha(elf) == 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    ranges = [(0x27b1fc, 0x27b26c), (0x11f730, 0x11f7a0)]
    words = {pc: struct.unpack_from('<I', elf, pc - 0x100000 + 0x80)[0] for lo, hi in ranges for pc in range(lo, hi, 4)}
    sources = ['entry_0027b1fc_0x27b1fc.cpp', 'entry_0011f708_0x11f708.cpp']
    anchored = {}
    for name in sources:
        data = (ROOT / 'work/native/project/generated' / name).read_text()
        for pc, word, text in re.findall(r'// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+([^\r\n]+)', data):
            address = int(pc, 16)
            if address in words:
                assert int(word, 16) == words[address]
                anchored[address] = text
    assert len(anchored) == len(words)
    raw = b''.join(struct.pack('<I', w) for w in words.values())
    (OUT / 'placement-words.bin').write_bytes(raw)
    (OUT / 'placement-words.txt').write_text('\n'.join(f'{pc:08X} {w:08X} {anchored[pc]}' for pc, w in words.items()) + '\n')
    world = json.loads((ROOT / 'evidence/worldScene48E/world.json').read_text())
    locals_ = [json.loads(p.read_text())['matrix'] for p in sorted((ROOT / 'evidence/importCollision48G').glob('*.reference.json'))]
    inputs = [(i['position'], i['look'], i['up']) for i in world['instances']]
    real = len(inputs)
    rng = random.Random(0x48_49)
    for _ in range(240):
        inputs.append(([f32(rng.uniform(-20000, 20000)) for _ in range(3)],
                       [f32(rng.uniform(-4, 4)) for _ in range(3)],
                       [f32(rng.uniform(-4, 4)) for _ in range(3)]))
    records = []
    for i, (position, look, up) in enumerate(inputs):
        machine = Original(bytes(0x2000), words)
        machine.v[2] = [*up, 0.0]; machine.v[3] = [*look, 0.0]; machine.v[4] = [*position, 1.0]
        machine.g[3][0] = 0x1000
        for pc in range(*ranges[0], 4):
            assert machine.execute(pc) is None
        placement = machine.vec(0x1000, 16)
        local = locals_[i % len(locals_)].copy()
        if i >= real:
            # Exercise nonidentity translation and basis beyond authored COL fixtures.
            local = [f32(rng.uniform(-2, 2)) if j % 4 != 3 else float(j == 15) for j in range(16)]
        machine.vector(0x1100, local); machine.vector(0x1200, placement)
        machine.g[4][0] = 0x1100; machine.g[18][0] = 0x1200; machine.g[3][0] = 0x1300
        for pc in range(*ranges[1], 4):
            assert machine.execute(pc) is None
        records.append([*position, *look, *up, *local, *placement, *machine.vec(0x1300, 16)])
    header = '#pragma once\n#include <cstdint>\nstatic constexpr std::uint32_t placement_fixtures48i[][57] = {\n'
    header += '\n'.join('    {' + ','.join(f'0x{bits(v):08x}u' for v in r) + '},' for r in records)
    header += '\n};\n'
    path = ROOT / 'tools/assets/collision_placement_fixtures48i.h'
    path.write_text(header)
    proof = dict(elf_sha256=sha(elf), instruction_words=len(words), original_world_instances=real,
                 generated_cases=len(records)-real, fixture_cases=len(records), words_sha256=sha(raw),
                 fixture_sha256=sha(path.read_bytes()), oracle='48H offline instruction executor, ordinary binary32 round-to-nearest; not all physical exceptional modes',
                 input_sources=[dict(path=p.relative_to(ROOT).as_posix(), sha256=sha(p.read_bytes())) for p in
                               [ROOT/'evidence/worldScene48E/world.json', *sorted((ROOT/'evidence/importCollision48G').glob('*.reference.json'))]])
    (OUT / 'placement-recovery.json').write_text(json.dumps(proof, indent=2) + '\n')
    print(json.dumps({k: proof[k] for k in ['instruction_words','original_world_instances','fixture_cases','words_sha256']}))

if __name__ == '__main__': main()
