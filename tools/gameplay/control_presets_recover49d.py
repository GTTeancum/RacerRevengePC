#!/usr/bin/env python3
"""Freeze controller presets and their local defaults/getter from verified ELF only.

No production, reconstructed source, RAM, resources, devices or unrelated families
are inputs. --check regenerates in memory and verifies every frozen output.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS = {
    'select': (0x188880, 0x188c04),
    'defaults': (0x1864f8, 0x18658c),
    'getter': (0x188c10, 0x188d10),
}
DATA = {'jump_table': (0x3a8da0, 0x3a8dc0)}
BOUNDARIES = {'setter_end_padding_getter_entry': (0x188bfc, 0x188c14),
              'getter_return_next_entry': (0x188d08, 0x188d18),
              'default_loop_exit_continuation': (0x186584, 0x186598)}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, w):
    op, rs, rt, rd, sa, fn = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31, (w >> 6) & 31, w & 63
    imm = (w & 0xffff) - (0x10000 if w & 0x8000 else 0)
    if not w:
        return 'nop'
    if op in (4, 5):
        return f'{"beq" if op == 4 else "bne"} {REG[rs]}, {REG[rt]}, {pc + 4 + 4 * imm:#x}'
    if op == 3:
        return f'jal {((pc + 4) & 0xf0000000) | ((w & 0x3ffffff) << 2):#x}'
    if op in (9, 10, 11):
        return f'{ {9:"addiu", 10:"slti", 11:"sltiu"}[op]} {REG[rt]}, {REG[rs]}, {imm:#x}'
    if op == 13:
        return f'ori {REG[rt]}, {REG[rs]}, {w & 65535:#x}'
    if op == 15:
        return f'lui {REG[rt]}, {w & 65535:#x}'
    if op in (0x23, 0x2b, 0x31, 0x39, 0x1e, 0x1f):
        name = {0x23:'lw', 0x2b:'sw', 0x31:'lwc1', 0x39:'swc1', 0x1e:'lq', 0x1f:'sq'}[op]
        return f'{name} {"f" + str(rt) if op in (0x31, 0x39) else REG[rt]}, {imm:#x}({REG[rs]})'
    if op == 0:
        if fn == 0:
            return f'sll {REG[rd]}, {REG[rt]}, {sa}'
        if fn == 0x21:
            return f'addu {REG[rd]}, {REG[rs]}, {REG[rt]}'
        if fn == 8:
            return f'jr {REG[rs]}'
    if op == 0x1c and fn == 0x28 and sa == 0x18:
        return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
    raise ValueError(f'Unrecognized selected original instruction {pc:#x}: {w:08x}')


def generate():
    image = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    if hashlib.sha256(image).hexdigest() != ELF_SHA256:
        raise ValueError('Original ELF SHA256 mismatch')
    if image[:6] != b'\x7fELF\x01\x01':
        raise ValueError('Expected little-endian ELF32')
    phoff = struct.unpack_from('<I', image, 28)[0]
    phsize, phnum = struct.unpack_from('<HH', image, 42)
    segments = [struct.unpack_from('<8I', image, phoff + i * phsize) for i in range(phnum)]

    def extract(start, end):
        for kind, fileoff, address, _, filesz, _, _, _ in segments:
            if kind == 1 and address <= start < end <= address + filesz:
                return image[fileoff + start - address:fileoff + end - address], fileoff + start - address
        raise ValueError(f'Unmapped original extent {start:#x}..{end:#x}')

    manifest = {
        'elf_sha256': ELF_SHA256,
        'source': 'work/native/project/disc/SLUS_202.68',
        'ram_reads': 0, 'generated_source_reads': 0, 'production_source_reads': 0,
        'resource_reads': 0,
        'blocks': {}, 'data_blocks': {}, 'boundary_provenance_only': {},
        'contract': {
            'select_arguments': 'a0=config, a1=slot, a2=raw32 preset; no meaningful return value is invented.',
            'slot_domain': 'Original setter contains no slot guard. Native adapter separately admits slots0..1, the exact local default loop range. Sparse/synthetic mapped execution is not proof of arbitrary real-object capacity.',
            'invalid_preset': 'First store raw preset at config+0x11048+4*slot; then SLTIU a2,8. Raw32 presets outside0..7 return without mode/code writes. Negative inputs are invalid. No rollback or normalization.',
            'row_layout': 'Ten32-bit words at config+40*slot+offset; ascending offsets690,694,698,69C,6A0,6A4,6A8,6AC,6B0,6B4.',
            'store_order': ['preset11048', 'mode11040', '694', '690', '6A8', '698', '69C', '6AC', '6A0', '6A4', '6B0', '6B4'],
            'modes': 'Presets0..3 store mode0; presets4..7 store mode1.',
            'preset_rows_ascending_offsets': [
                [3, 2, 5, 4, 9, 0, 1, 8, 6, 7],
                [0, 1, 7, 6, 9, 3, 2, 8, 4, 5],
                [1, 2, 5, 4, 9, 0, 3, 8, 6, 7],
                [3, 2, 7, 5, 9, 0, 1, 8, 6, 4],
                [-1, -1, 5, 4, 9, 0, 1, 8, 6, 7],
                [-1, -1, 7, 6, 9, 3, 2, 8, 4, 5],
                [-1, -1, 5, 4, 9, 0, 3, 8, 6, 7],
                [-1, -1, 7, 5, 9, 0, 1, 8, 6, 4]],
            'default_scope': 'Complete local loop0x1864f8..0x18658c, not the enclosing full initializer. s2 supplies config; original words set s0=0,s1=config and execute exactly slots0 then1. Requested interior0x186538..0x186588 alone would omit side effects and loop setup.',
            'default_store_order_per_slot': ['10F94=7', '10F9C=7', '10FA4=1', '11040=0', '11048=0', 'call select(slot,0), including its redundant preset/mode stores and all10 codes', '11030=5', '11038=0 in taken/not-taken BNE delay slot'],
            'getter_arguments': 'a0=40-byte caller output, a1=config, a2=requested preset. Complete function0x188c10..0x188d10, including128-bit saves/restores and return delay slot.',
            'getter_effects': 'Save slot0 preset only; call select(0,requested); snapshot10 slot0 row words to local stack; call select(0,saved); copy snapshot to output in ascending row order. Mode/custom row are not saved. Saved-valid reconstructs the saved preset canonical row and mode. Saved-invalid restores only its raw index, retaining requested-valid row/mode; requested-invalid reads original row unchanged until restoring saved. Slot1 is untouched.',
            'getter_copy': 'LWC1/SWC1 copy arbitrary32-bit payloads; no floating-point arithmetic, conversion, canonicalization, or host float is used by oracle.',
            'alias_policy': 'Only bounded slots0/1 execute. Fixture output/config/stack are disjoint and fixed; invalid native slot behavior and arbitrary pointer aliases are not executed or declared recovered.',
            'scope_exclusions': 'No physical control names, device transport, ownership/collision-family diagnostics, allocation-failure tests, PC/RAM runs, installers, renderer, gameplay scheduler or arbitrary ELF execution.'
        },
        'oracle': {
            'kind': 'Test-only exact-word integer interpreter with128-bit register copies,32/64-bit signedness, exact delay slots and bounded fixed-size mapped memory/traces; no production code dependency.',
            'coverage': 'All selected words are reachable across presets0..7, invalid presets, getter and defaults; no reachable-instruction exclusions.',
            'external_calls': 'None. Both getter and default loop execute original selected setter directly; jump table is frozen original data.',
            'stop_boundaries': 'Setter/getter stop on synthetic return sentinel after executing JR delay slot. Defaults stop at actual local-loop fallthrough0x18658c after final BNE delay slot.'
        }
    }
    files = {}
    listing = []
    header = ['#pragma once', '#include <cstdint>', '// Test-only original words; generated by control_presets_recover49d.py.']
    for category, blocks in [('blocks', BLOCKS), ('data_blocks', DATA), ('boundary_provenance_only', BOUNDARIES)]:
        for name, (start, end) in blocks.items():
            raw, offset = extract(start, end)
            words = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
            manifest[category][name] = {'start': hex(start), 'end_exclusive': hex(end), 'words': len(words), 'elf_file_offset': hex(offset), 'sha256': hashlib.sha256(raw).hexdigest()}
            files[f'evidence/controlPresets49D/{name}.bin'] = raw
            listing.append(f'# {name}: {category}; [{start:#x}, {end:#x})')
            if category != 'boundary_provenance_only':
                header += [f'inline constexpr std::uint32_t rr_{name}_49d_start = 0x{start:08x}u;', f'inline constexpr std::uint32_t rr_{name}_49d_end = 0x{end:08x}u;', f'inline constexpr std::uint32_t rr_{name}_49d[] = {{']
            for i, word in enumerate(words):
                pc = start + 4 * i
                text = f'.word 0x{word:08x}' if category == 'data_blocks' else decode(pc, word)
                listing.append(f'{pc:08x} {word:08x} {text}')
                if category != 'boundary_provenance_only':
                    header.append(f'    0x{word:08x}u, // {pc:08x} {text}')
            if category != 'boundary_provenance_only':
                header.append('};')
    files['tools/gameplay/control_presets_words49d.h'] = ('\n'.join(header) + '\n').encode()
    files['evidence/controlPresets49D/original-disassembly.txt'] = ('\n'.join(listing) + '\n').encode()
    files['evidence/controlPresets49D/recovery-manifest.json'] = (json.dumps(manifest, indent=2) + '\n').encode()
    return files, manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    files, manifest = generate()
    for name, data in files.items():
        path = ROOT / name
        if args.check:
            if path.read_bytes() != data:
                raise ValueError('Frozen output mismatch: ' + name)
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            if path.exists() and path.read_bytes() != data:
                raise ValueError('Refusing to replace different frozen output: ' + name)
            path.write_bytes(data)
    print(json.dumps({'status': 'verified' if args.check else 'frozen', 'files': len(files), 'elf_sha256': ELF_SHA256, 'blocks': manifest['blocks']}))


if __name__ == '__main__':
    main()
