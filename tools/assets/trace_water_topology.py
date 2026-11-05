"""Extract private, hash-pinned Water topology evidence from the original ELF.

Outputs stay in a fresh repository evidence directory. This is an inspection
tool, not an emulator or proof that the corresponding source calls returned.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

# Inspection must not create dependency caches outside the repository.
sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[2]
EXPECTED_SHA256 = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RANGES = {
    'topology_reader': (0x2BAB10, 0x2BAE2C),
    'malloc_wrapper': (0x305700, 0x305730),
    'malloc_prefix': (0x305980, 0x305AE0),
    'topology_element_constructor': (0x133F80, 0x133FA0),
    'topology_array_constructor': (0x2E8990, 0x2E8ADC),
    'topology_inner_constructor': (0x299790, 0x2997A4),
    'topology_inner_loader': (0x2998D0, 0x299A94),
    'topology_inner_finalize': (0x299420, 0x299790),
    'topology_inner_zero_allocator': (0x302E58, 0x302F48),
    'file_read_items': (0x12FB30, 0x12FF68),
}
SOURCES = (
    'tests/native/ra_cold_aggregate50la.h',
    'tests/native/original_source_world_water_pool_topology_count50la.inl',
    'tests/native/original_source_world_water_pool_data50la.inl',
    'tests/native/original_source_world_water_pool_material50la.inl',
    'src/assets/original_water_pool_source_file50la.inl',
)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def file_bytes_at(data, address, size):
    """Resolve only file-backed ELF32 little-endian MIPS PT_LOAD bytes."""
    if len(data) < 52 or data[:7] != b'\x7fELF\x01\x01\x01':
        raise ValueError('expected ELF32 little-endian version 1')
    if struct.unpack_from('<H', data, 18)[0] != 8:
        raise ValueError('expected MIPS ELF')
    phoff = struct.unpack_from('<I', data, 28)[0]
    phsize, phcount = struct.unpack_from('<HH', data, 42)
    if phsize < 32 or phoff + phsize * phcount > len(data):
        raise ValueError('invalid program-header table')
    if address < 0 or size <= 0 or address + size > 0x100000000:
        raise ValueError('invalid address range')
    matches = []
    for i in range(phcount):
        kind, offset, va, _, filesz, memsz, _, _ = struct.unpack_from(
            '<8I', data, phoff + i * phsize)
        if kind != 1:
            continue
        if filesz > memsz or offset + filesz > len(data):
            raise ValueError('invalid load segment')
        if va <= address and address + size <= va + filesz:
            start = offset + address - va
            matches.append(data[start:start + size])
    if len(matches) != 1:
        raise ValueError('range must belong to exactly one file-backed load segment')
    return matches[0]


REG = ('zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 '
       's0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra').split()

# Exact words already recovered in tools/gameplay/camera_view_oracle49f.h.
# This is a disassembly whitelist, not an interpreter or hardware parity claim.
WATER_VECTOR_WORDS = {
    0x4BC4216A: 'vmul.xyz vf5, vf4, vf4',
    0x4B000183: 'vaddw.x vf6, vf0, vf0',
    0x4B05283D: 'vadday.x ACC, vf5, vf5',
    0x4B05314A: 'vmaddz.x vf5, vf6, vf5',
    0x4A6503BE: 'vrsqrt Q, vf0.w, vf5.x',
    0x4A202180: 'vaddx.w vf6, vf4, vf0',
    0x4A0003BF: 'vwaitq',
    0x4BC0219C: 'vmulq.xyz vf6, vf4, Q',
    0x4A2631AC: 'vsub.w vf6, vf6, vf6',
    0x4BC522FE: 'vopmula.xyz ACC, vf4, vf5',
    0x4BC429AE: 'vopmsub.xyz vf6, vf5, vf4',
}


def ee_instruction(word):
    """Override incompatible modern-MIPS interpretations of EE instructions."""
    op, rs, rt, rd = word >> 26, (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
    imm = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
    if op == 0 and (word & 63) in (0x18, 0x19):
        #EE MULT/MULTU also write the sign-extended low word to rd. Modern
        #MIPS DSP disassembly incorrectly renders rd as an accumulator number.
        return f'{"mult" if (word & 63) == 0x18 else "multu"} ${REG[rd]}, ${REG[rs]}, ${REG[rt]}'
    if op in (0x1E, 0x1F):
        return f'{"lq" if op == 0x1E else "sq"} ${REG[rt]}, {imm:#x}(${REG[rs]})'
    if op in (0x36, 0x3E):
        return f'{"lqc2" if op == 0x36 else "sqc2"} $vf{rt}, {imm:#x}(${REG[rs]})'
    if op == 0x12:
        return WATER_VECTOR_WORDS.get(word, 'EE COP2 (not decoded)')
    if op == 0x1C:
        # EE MMI3/POR. Other MMI words are deliberately left undecoded.
        if word & 0x7FF == 0x628:
            return f'por ${REG[rd]}, ${REG[rs]}, ${REG[rt]}'
        return 'EE MMI (not decoded)'
    return None


def inspect(data):
    if digest(data) != EXPECTED_SHA256:
        raise ValueError('original ELF SHA256 differs from the source evidence')
    import capstone
    decoder = capstone.Cs(capstone.CS_ARCH_MIPS,
                         capstone.CS_MODE_MIPS64 | capstone.CS_MODE_LITTLE_ENDIAN)
    ranges = {}
    for name, (begin, end) in RANGES.items():
        raw = file_bytes_at(data, begin, end - begin)
        instructions = []
        for offset in range(0, len(raw), 4):
            va = begin + offset
            encoded = raw[offset:offset + 4]
            word = int.from_bytes(encoded, 'little')
            rendered = ee_instruction(word)
            if rendered is None:
                decoded = list(decoder.disasm(encoded, va, count=1))
                rendered = (decoded[0].mnemonic + ' ' + decoded[0].op_str).strip() if decoded else 'not decoded'
            item = dict(va=f'{va:08x}', word=f'{word:08x}', instruction=rendered)
            if word >> 26 == 3:
                item['jal_target'] = f'{((va + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2):08x}'
                item['delay_slot_va'] = f'{va + 4:08x}'
            instructions.append(item)
        ranges[name] = dict(begin=f'{begin:08x}', endExclusive=f'{end:08x}',
                            sha256=digest(raw), instructions=instructions)
    # Pin the handoff boundary, subsequent two allocations and real return.
    for va, word in {0x2BAB54: 0x0C0C15C0, 0x2BAB58: 0x00022080,
                     0x2BAB5C: 0xAE620028, 0x2BABE8: 0x0C0C15C0,
                     0x2BABF0: 0xAE620020, 0x2BAE24: 0x03E00008,
                     0x2BAE28: 0x27BD0060}.items():
        if int.from_bytes(file_bytes_at(data, va, 4), 'little') != word:
            raise ValueError(f'boundary instruction mismatch at {va:08x}')
    return dict(elfSha256=digest(data), decoder=f'capstone {capstone.__version__} with explicit EE overrides',
                scope='static original bytes; no runtime, allocator return or loader completion claimed',
                ranges=ranges)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    if not output.is_relative_to((ROOT / 'evidence').resolve()) or output == (ROOT / 'evidence').resolve():
        parser.error('output must be a fresh subdirectory of repository evidence')
    report = inspect(args.elf.read_bytes())
    report['sourceHashes'] = {name: digest((ROOT / name).read_bytes()) for name in SOURCES}
    output.mkdir(parents=True, exist_ok=False)
    (output / 'trace.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    lines = ['Water topology original-byte trace', report['scope'], '']
    for name, entry in report['ranges'].items():
        lines.extend([name, *[f"{i['va']}  {i['word']}  {i['instruction']}" for i in entry['instructions']], ''])
    (output / 'trace.txt').write_text('\n'.join(lines), encoding='utf-8')
    print(output / 'trace.json')


if __name__ == '__main__':
    main()
