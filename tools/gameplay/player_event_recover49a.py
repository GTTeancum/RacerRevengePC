#!/usr/bin/env python3
"""Freeze 49A complete player press handler, held selector, and state gate.

Read only the verified ELF and this script's own outputs. No helper internals,
RAM, game resources, generated translations, or production implementation reads.
"""
import sys
sys.dont_write_bytecode = True
import argparse
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS = {'player': (0x1a16f0, 0x1a1a20),
          'manager': (0x128520, 0x128600),
          'gate': (0x1b0b80, 0x1b0bb0)}
PROVENANCE = {'player_constructor_prefix': (0x1a02c0, 0x1a02e4),
              'manager_constructor': (0x127ea0, 0x127ef8),
              'psx_manager_constructor': (0x237300, 0x237338),
              'device_constructor_prefix': (0x143390, 0x1433d8),
              'psx_device_constructor_prefix': (0x237c10, 0x237c30),
              'player_vtable': (0x3ced50, 0x3cede8),
              'manager_vtable': (0x3cce10, 0x3cce34),
              'psx_manager_vtable': (0x3d1ca0, 0x3d1cc4),
              'device_vtable': (0x3cd350, 0x3cd370),
              'psx_device_vtable': (0x3d1d80, 0x3d1da0)}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, word, strict=True):
    op, rs, rt, rd, sa, fn = word >> 26, (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31, (word >> 6) & 31, word & 63
    imm = struct.unpack('<h', struct.pack('<H', word & 65535))[0]
    if not word:
        return 'nop'
    if op in (4, 5, 0x14):
        return f'{ {4:"beq", 5:"bne", 0x14:"beql"}[op]} {REG[rs]}, {REG[rt]}, {pc + 4 + 4 * imm:#x}'
    if op == 3:
        return f'jal {((pc + 4) & 0xf0000000) | ((word & 0x3ffffff) << 2):#x}'
    if op in (9, 0xa):
        return f'{"addiu" if op == 9 else "slti"} {REG[rt]}, {REG[rs]}, {imm:#x}'
    if op in (0xc, 0xd, 0xe):
        return f'{ {0xc:"andi", 0xd:"ori", 0xe:"xori"}[op]} {REG[rt]}, {REG[rs]}, {word & 65535:#x}'
    if op == 0xf:
        return f'lui {REG[rt]}, {word & 65535:#x}'
    if op in (0x20, 0x23, 0x24, 0x28, 0x2b, 0x31, 0x39, 0x1e, 0x1f):
        name = {0x20:'lb', 0x23:'lw', 0x24:'lbu', 0x28:'sb', 0x2b:'sw', 0x31:'lwc1', 0x39:'swc1', 0x1e:'lq', 0x1f:'sq'}[op]
        return f'{name} {"f"+str(rt) if op in (0x31, 0x39) else REG[rt]}, {imm:#x}({REG[rs]})'
    if op == 0:
        if fn in (0, 0x3c, 0x3e):
            return f'{ {0:"sll", 0x3c:"dsll32", 0x3e:"dsrl32"}[fn]} {REG[rd]}, {REG[rt]}, {sa}'
        if fn in (0x21, 0x23, 0x2b):
            return f'{ {0x21:"addu", 0x23:"subu", 0x2b:"sltu"}[fn]} {REG[rd]}, {REG[rs]}, {REG[rt]}'
        if fn == 8:
            return f'jr {REG[rs]}'
        if fn == 9:
            return f'jalr {REG[rd]}, {REG[rs]}'
    if op == 0x1c and fn == 0x28 and sa == 0x18:
        return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
    if op == 0x11:
        if rs == 4:
            return f'mtc1 {REG[rt]}, f{rd}'
        if rs == 8 and rt in (0, 1):
            return f'{"bc1f" if rt == 0 else "bc1t"} {pc + 4 + 4 * imm:#x}'
        if rs == 16 and fn == 1:
            return f'sub.s f{sa}, f{rd}, f{rt}'
        if rs == 16 and fn == 0x34:
            return f'c.lt.s f{rd}, f{rt}'
    if strict:
        raise ValueError(f'Unrecognized selected instruction {pc:#x} {word:08x}')
    return f'.word 0x{word:08x}'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    data = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    if hashlib.sha256(data).hexdigest() != ELF_SHA256:
        raise ValueError('Original ELF hash mismatch')
    if data[:6] != b'\x7fELF\x01\x01':
        raise ValueError('Expected little-endian ELF32')
    offset = struct.unpack_from('<I', data, 28)[0]
    size, count = struct.unpack_from('<HH', data, 42)
    segments = [struct.unpack_from('<8I', data, offset + i * size) for i in range(count)]

    def extract(start, end):
        for kind, file_offset, address, _, file_size, _, _, _ in segments:
            if kind == 1 and address <= start < end <= address + file_size:
                return data[file_offset + start - address:file_offset + end - address]
        raise ValueError('Unmapped original ELF extent')

    header = ['#pragma once', '#include <cstdint>',
              '// Test-only original words, frozen by player_event_recover49a.py.']
    listing = []
    manifest = {
        'elf_sha256': ELF_SHA256,
        'source': 'work/native/project/disc/SLUS_202.68',
        'ram_reads': 0, 'generated_source_reads': 0, 'resource_reads': 0,
        'blocks': {}, 'provenance_only_blocks': {},
        'provenance_notes': {
            'player': 'Constructor prefix stores vtable0x3ced50 at player+8. Slot+0x60=0x1422a0 (unchanged release router);+0x64=0x1a16f0 (this press handler);+0x88=0x2261b0,+0x8c=0x226750,+0x90=0x1a3930,+0x94=0x1a3970 are supplied effect boundaries, not executed helper bodies.',
            'manager': 'Base and PSX constructors store0x3cce10/0x3d1ca0 at manager+0xcc0. Both vtable+0x10 entries are0x128520, the complete selector executed here.',
            'device': 'Base and PSX constructor prefixes store0x3cd350/0x3d1d80 at device+0x18. Both source query slot+0x10 entries are0x143600. That implementation remains an external supplied source boundary here.',
            'scope': 'These bytes establish object and virtual-slot identity only; none count toward instruction execution coverage. Context and auxiliary concrete provider implementation identities remain unresolved explicit boundaries.'
        },
        'external_input_boundaries': {
            'source_queries': 'Original manager dispatch executes in full. Category0 selects context+0x34; category1 auxiliary+0x30; category2 selects device pointer+0x10+4*source. Nonnull sources call source vtable+0x18 slot+0x18 for categories0/1 or slot+0x10 for category2. a1=code, a2=mask, a3=0. Callback supplies raw32-bit v0; original words normalize nonzero to1. Missing source and unknown category return0 without callback.',
            'player_helpers': 'Explicit synchronous supplied boundaries: 0x2255b0, player vtable+0x88/+0x8c/+0x90/+0x94, 0x22b170, 0x22b370, 0x22b570, 0x22b760, 0x22ba60. No implementation effects are invented. Full live fixture is decoded before, mutated by hook, encoded afterward, then exact original words resume. Helper v0 is recorded but not used by the original player continuation.',
            'tail_router': '0x142350 is an explicit supplied routing boundary. a0=player, a1=incoming20-byte Event identity, a2=incoming24-byte Record identity. The handler forwards these references only here. Its raw v0 survives the full player epilogue.',
            'gate': 'Original direct call0x1b0b80 executes all12 frozen gate words, including BEQL annulment and return delay slot.'
        },
        'fixture_layout': {
            'player': 'Words+0x40,+0x5c,+0xffc; binary32 bit-preserving words+0x1098,+0x1094,+0x1090,+0xcb4,+0xcb0,+0xcb8; word+0xcd0. Player+8 points to fixture vtable. Callee-saved128-bit registers and full stack return frame are checked.',
            'application': 'Original0x382b80 singleton points to fixture application. Application+4=manager;+0x28c=flag owner;+0x2dc=config. Flag owner byte+0x29 and gate pointer+0xb4->byte+0x58 are separate live fields.',
            'configuration': 'Eight declared action rows at config+40*index+0x6ac, index0..7; selector byte+0x1125f; sixteen tuning rows with stride0xe4, float word+0x102f0 and integer word+0x10388. Every field is re-encoded after hooks. Device pointer indices0..7 only; no source-table alias or history.',
            'lazy_static': 'Signed lazy-init byte at gp-0x7a0c (instruction offset0x85f4) and word0x4340a0 are independent mutable fixture state. Fresh initialization writes word2 then byte1.',
            'query_arguments': 'Handler always invokes three manager queries in order: configured action/category2/mask0x3f/source+0x5c; code0x1a/category0/mask0x80/source+0x5c; code0x12/category0/mask0x80/source+0x5c. Query and later helper mutations are visible only at original reloads.'
        },
        'admitted_domain_unreachable_instructions': {
            '0x1a1840': 'Dead duplicate addiu after unconditional branch0x1a1838 and its0x1a183c delay slot; branch0x1a1820 targets0x1a1844 directly.',
            '0x1a1864': 'Dead duplicate addiu after unconditional branch0x1a185c and its0x1a1860 delay slot; branch0x1a1844 targets0x1a1868 directly.',
            '0x12854c': 'Dead duplicate shift after unconditional branch0x128544 and its0x128548 delay slot; category2 branch targets0x128550, with actual shift in0x128540 delay slot.',
            '0x1285f0': 'Dead zero-result duplicate after unconditional branch0x1285e8 and its0x1285ec delay slot; unknown-category branch goes directly to0x1285f4.'
        },
        'interval_notes': {
            'player': 'Complete204-word function0x1a16f0..0x1a1a20, including save/restore and return delay slot. No event-field checks invented at entry. Application flag bit2 blocks first action; byte gate bit1 and not bit0 is required; player+0x1098 comparison is strictly less than zero. Source0 or1 permits tuning action after third query.',
            'manager': 'Complete56-word held selector0x128520..0x128600. Categories other than0/1/2 return0. Only category2 dereferences source index; its admitted native range is0..7. No restriction on code or mask.',
            'floating_point_domain': 'Finite IEEE binary32 fixtures under FE_TONEAREST/FE_DOWNWARD/FE_UPWARD/FE_TOWARDZERO, one volatile binary32 SUB.S step, compiled without contraction/reassociation and with rounding-math. Copy/load/store preserve exact bits, including signed zero. This is not PS2 exceptional-value FPU emulation.',
            'scope': 'No player/helper bodies, collision-family addresses, resources, RAM, PC gameplay validation, install work, or device read/reset/prepass. All external hooks and mutable effects are explicitly supplied.'
        }
    }
    for provenance, blocks in ((False, BLOCKS), (True, PROVENANCE)):
        for name, (start, end) in blocks.items():
            raw = extract(start, end)
            words = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
            manifest['provenance_only_blocks' if provenance else 'blocks'][name] = {
                'start': hex(start), 'end_exclusive': hex(end),
                'words': len(words), 'sha256': hashlib.sha256(raw).hexdigest()}
            listing.append(f'# {name}; {"provenance only, not executed" if provenance else "complete executed original function"}')
            header += [f'inline constexpr std::uint32_t rr_{name}_49a_start = 0x{start:08x}u;',
                       f'inline constexpr std::uint32_t rr_{name}_49a_end = 0x{end:08x}u;',
                       f'inline constexpr std::uint32_t rr_{name}_49a[] = {{']
            for index, word in enumerate(words):
                pc = start + 4 * index
                instruction = f'.word 0x{word:08x}' if 'vtable' in name else decode(pc, word, not provenance)
                header.append(f'    0x{word:08x}u, // {pc:08x} {instruction}')
                listing.append(f'{pc:08x} {word:08x} {instruction}')
            header.append('};')
    files = {
        'tools/gameplay/player_event_words49a.h': '\n'.join(header) + '\n',
        'evidence/playerEvent49A/original-disassembly.txt': '\n'.join(listing) + '\n',
        'evidence/playerEvent49A/recovery-manifest.json': json.dumps(manifest, indent=2) + '\n'
    }
    for name, text in files.items():
        path = ROOT / name
        if args.check:
            if path.read_text() != text:
                raise ValueError('Frozen output mismatch: ' + name)
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            if path.exists() and path.read_text() != text:
                raise ValueError('Refusing to replace different frozen evidence: ' + name)
            path.write_text(text)
    print(json.dumps({'status': 'verified' if args.check else 'frozen', **manifest}))


if __name__ == '__main__':
    main()
