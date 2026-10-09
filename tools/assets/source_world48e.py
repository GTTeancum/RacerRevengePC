"""Freeze scene-loader evidence, checking generated instruction words against ELF."""
from pathlib import Path
import hashlib
import json
import re
import struct

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/worldScene48E'
RANGES = [
    ('original_binary_parameter_reader', 0x1578f8, 0x157a94),
    ('original_decimal_vector_to_float32_reader', 0x158c00, 0x158cf8),
    ('zone_static_model_count', 0x275f40, 0x275f74),
    ('static_parameters_base_reader', 0x27a270, 0x27a358),
    ('static_parameters_concrete_reader', 0x27a550, 0x27a5c8),
    ('static_model_root_setter_calls', 0x27adc4, 0x27adf4),
    ('zone_model_header_and_asset_lookup', 0x27b7c0, 0x27b8a0),
    ('root_translation_replacement', 0x28ab90, 0x28abc8),
    ('root_orientation_replacement', 0x289ec0, 0x28a03c),
    ('geometry_hierarchy_reader_call', 0x26ed14, 0x26ed50),
    ('root_node_is_geometry_object_and_reads_original_matrix', 0x28cac0, 0x28cb90),
]
CARS = [
    ('third_party/cars-tools-reference/CarsTools/CARSRESOURCEBUILDER/ParameterBlocks.cpp',
     [(9, 16), (510, 588), (1846, 2190)]),
    ('third_party/cars-tools-reference/CarsTools/MAX Plugins/LevelBastard/LevelBastard.cpp',
     [(3377, 3432)]),
]
STRINGS = [(0x3a62a8, 'BINARY'), (0x3b9b70, 'SceneInfo'),
           (0x3ba5c8, 'Zone%d'), (0x3ba650, 'NumberOfStaticModels'),
           (0x3b9ff8, 'Position'), (0x3bab20, 'LookVector'),
           (0x3bb168, 'UpVector'), (0x3bb318, '%s-Model%d')]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    if not __debug__:
        raise RuntimeError('Run without -O: source evidence assertions must execute')
    elf_path = ROOT / 'work/native/project/disc/SLUS_202.68'
    elf = elf_path.read_bytes()
    assert elf[:6] == b'\x7fELF\x01\x01'
    phoff = struct.unpack_from('<I', elf, 28)[0]
    phsize, phcount = struct.unpack_from('<HH', elf, 42)
    segments = [struct.unpack_from('<8I', elf, phoff + phsize*i) for i in range(phcount)]

    def at(va, count):
        for kind, off, base, _, size, _, _, _ in segments:
            if kind == 1 and base <= va and va + count <= base + size:
                return off + va - base
        raise ValueError('Unmapped original address ' + hex(va))

    paths = []
    for path in (ROOT / 'work/native/project/generated').glob('*_0x*.cpp'):
        match = re.search(r'_0x([0-9a-f]+)\.cpp$', path.name)
        start = int(match[1], 16)
        if any(low - 0x1000 <= start <= high for _, low, high in RANGES):
            paths.append(path)
    words = {}
    source_hashes = {}
    for path in sorted(paths):
        data = path.read_bytes()
        selected = False
        for line_number, line in enumerate(data.decode('utf-8').splitlines(), 1):
            match = re.match(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+(.*)', line)
            if not match:
                continue
            address, word = int(match[1], 16), int(match[2], 16)
            if not any(low <= address < high for _, low, high in RANGES):
                continue
            offset = at(address, 4)
            original = struct.unpack_from('<I', elf, offset)[0]
            assert original == word, (path, line_number, hex(address), hex(word), hex(original))
            record = words.setdefault(address, dict(address=hex(address), file_offset=offset,
                word=f'{word:08x}', original_word_verified=True, disassembly=match[3], sources=[]))
            assert record['word'] == f'{word:08x}'
            record['sources'].append(dict(path=path.relative_to(ROOT).as_posix(), line=line_number))
            selected = True
        if selected:
            source_hashes[path.relative_to(ROOT).as_posix()] = sha(data)
    commented_instruction_count = len(words)
    omitted = []
    groups = []
    for label, low, high in RANGES:
        for address in range(low, high, 4):
            if address not in words:
                offset = at(address, 4)
                word = struct.unpack_from('<I', elf, offset)[0]
                # Generated code omits some zero/NOP delay-slot comments.
                # Retain original bytes rather than invent disassembly.
                omitted.append(dict(address=hex(address),word=f'{word:08x}'))
                words[address] = dict(address=hex(address),file_offset=offset,word=f'{word:08x}',
                    original_word_verified=True,disassembly='nop' if word==0 else None,sources=[])
        groups.append(dict(label=label, start=hex(low), end_exclusive=hex(high),
                           instructions=[words[a] for a in range(low, high, 4)]))
    strings = []
    for address, expected in STRINGS:
        offset = at(address, len(expected)+1)
        assert elf[offset:offset+len(expected)+1] == expected.encode('ascii')+b'\0', hex(address)
        strings.append(dict(address=hex(address), file_offset=offset, original_text=expected))
    excerpts = []
    for name, ranges in CARS:
        data = (ROOT/name).read_bytes()
        source_hashes[name] = sha(data)
        lines = data.decode('latin1').splitlines()
        excerpts.append(dict(path=name, excerpts=[dict(first_line=a,last_line=b,
            lines=[f'{i}: {lines[i-1]}' for i in range(a,b+1)]) for a,b in ranges]))
    report = dict(schema='original-world-source-proof48E-v1',
        original_elf=dict(path=elf_path.relative_to(ROOT).as_posix(),sha256=sha(elf)),
        instruction_count=len(words),generated_comments_verified=commented_instruction_count,
        instructions_without_generated_comments=omitted,strings=strings, groups=groups,
        source_sha256=source_hashes,cars_reference_excerpts=excerpts,
        limitations=['Instruction words and string bytes verified against original ELF; generated disassembly text is retained evidence, not separately re-disassembled here.',
            'Later Cars exporter corroborates format intent; it is not used to manufacture missing Racer Revenge placement data.',
            'Original operations establish transform semantics; scalar binary64 export does not reproduce VU RSQRT rounding.'])
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT/'original-loader-proof.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(instruction_count=len(words),generated_comments_verified=commented_instruction_count,
        instructions_without_generated_comments=omitted,groups=len(groups),strings=len(strings),
        elf_sha256=sha(elf),source_files=len(source_hashes))))


if __name__ == '__main__':
    main()
