#!/usr/bin/env python3
"""Freeze independent 48Z supplied-event manager and complete GameObject routers.

Only this file's frozen outputs and the hash-verified original ELF are read.
No RAM, generated translations, production source, or earlier oracle evidence.
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
BLOCKS = {'manager': (0x12815c, 0x128430),
          'release_router': (0x1422a0, 0x142344),
          'press_router': (0x142350, 0x1423f4)}
PROVENANCE = {'game_release': (0x140ea0, 0x140ee8),
              'game_press': (0x140ef0, 0x140fc4),
              'psxgame_press': (0x2376f0, 0x237850),
              'podgame_press': (0x17cbc0, 0x17ccf0),
              'game_vtable': (0x3cd1a0, 0x3cd1e4),
              'psxgame_vtable': (0x3d1cd0, 0x3d1d14),
              'podgame_vtable': (0x3ce480, 0x3ce4c4),
              'gameobject_vtable': (0x3cd240, 0x3cd2a8)}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, word, strict=True):
    op, rs, rt, rd, sa, fn = word >> 26, (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31, (word >> 6) & 31, word & 63
    imm = struct.unpack('<h', struct.pack('<H', word & 65535))[0]
    if word == 0:
        return 'nop'
    if op in (4, 5, 0x14, 0x15):
        return f'{ {4:"beq", 5:"bne", 0x14:"beql", 0x15:"bnel"}[op]} {REG[rs]}, {REG[rt]}, {pc + 4 + 4 * imm:#x}'
    if op == 1 and rt in (0, 1):
        return f'{"bltz" if rt == 0 else "bgez"} {REG[rs]}, {pc + 4 + 4 * imm:#x}'
    if op in (2, 3):
        return f'{"j" if op == 2 else "jal"} {((pc + 4) & 0xf0000000) | ((word & 0x3ffffff) << 2):#x}'
    if op in (9, 0x19, 0xa, 0xb):
        return f'{ {9:"addiu", 0x19:"daddiu", 0xa:"slti", 0xb:"sltiu"}[op]} {REG[rt]}, {REG[rs]}, {imm:#x}'
    if op in (0xc, 0xd, 0xe):
        return f'{ {0xc:"andi", 0xd:"ori", 0xe:"xori"}[op]} {REG[rt]}, {REG[rs]}, {word & 65535:#x}'
    if op == 0xf:
        return f'lui {REG[rt]}, {word & 65535:#x}'
    if op in (0x20, 0x23, 0x24, 0x25, 0x28, 0x2b, 0x1e, 0x1f):
        name = {0x20:'lb', 0x23:'lw', 0x24:'lbu', 0x25:'lhu', 0x28:'sb', 0x2b:'sw', 0x1e:'lq', 0x1f:'sq'}[op]
        return f'{name} {REG[rt]}, {imm:#x}({REG[rs]})'
    if op == 0:
        if fn in (0, 2, 3, 0x3c, 0x3e):
            return f'{ {0:"sll", 2:"srl", 3:"sra", 0x3c:"dsll32", 0x3e:"dsrl32"}[fn]} {REG[rd]}, {REG[rt]}, {sa}'
        if fn in (0x21, 0x23, 0x24, 0x25, 0x26, 0x2a, 0x2b, 0x2d):
            return f'{ {0x21:"addu", 0x23:"subu", 0x24:"and", 0x25:"or", 0x26:"xor", 0x2a:"slt", 0x2b:"sltu", 0x2d:"daddu"}[fn]} {REG[rd]}, {REG[rs]}, {REG[rt]}'
        if fn == 0xd:
            return f'break {(word >> 6) & 0xfffff:#x}'
        if fn == 8:
            return f'jr {REG[rs]}'
        if fn == 9:
            return f'jalr {REG[rd]}, {REG[rs]}'
    if op == 0x1c and fn == 0x28 and sa == 0x18:
        return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
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
              '// Test-only original words, frozen by event_dispatch_recover48z.py.']
    listing = []
    manifest = {
        'elf_sha256': ELF_SHA256,
        'source': 'work/native/project/disc/SLUS_202.68',
        'ram_reads': 0, 'generated_source_reads': 0, 'resource_reads': 0,
        'blocks': {}, 'provenance_only_blocks': {},
        'external_input_boundaries': {
            'manager_virtuals': 'Application vtable+0x3c/+0x40 are explicit supplied callbacks. a1 and a2 are the original live event and record pointers. Decode state before each hook, mutate supplied state, encode it afterwards, and return explicit callback v0. Manager ignores v0.',
            'router_child_virtuals': 'Each admitted child vtable+0x60/+0x64 is a supplied opaque child binding. The callback can mutate event, record, flags and links and supplies explicit v0. Nonzero stops traversal. No recursive tree or concrete child behavior is invented.',
            'application_provenance': 'Wrapper and vtable bytes are frozen only as provenance. Their instructions are not executed by this oracle. A separate production bridge must declare its own qualified class/flag/category domain.'
        },
        'fixture_layout': {
            'manager': 's0=0, s2=synthetic manager at entry0x12815c. manager+0x38 live signed count; manager+0x3c is 64 ordered20-byte supplied events. manager+0x34 optional context; manager+0x30 optional auxiliary; manager+0x10+4*deviceID is one of8 optional device pointers.',
            'event': 'Five words: code, category, device index, transition, context tag. Category0..2 admitted. Transition is any32-bit value. Signed count may be nonpositive (no iterations) and must not exceed64; code0..31 and device0..7 are checked only when original pointer access requires them.',
            'record': 'Six live mutable words,24-byte stride. Category0=context+0x20; category1=auxiliary+0x1c; category2=device[event.deviceID]+0x324.',
            'context': 'Current pointer presence and context+0x1898 tag are re-encoded after every supplied callback.',
            'router': 'parent aggregateMask+0x24; firstChild+0x18; child next+0x14, flags byte+0x29, vtable+8.32 explicit nodes, noChild maps to null. Mutable event and record have stable addresses.'
        },
        'admitted_domain_unreachable_instructions': {
            '0x1281b8..0x1281bc_inclusive': 'Unknown-category branch and its LB delay slot; the preceding comparisons route every admitted category0..2 elsewhere.',
            '0x1283b0..0x128414_inclusive': 'Unknown-category debug-print/assert body, including dead fallthrough0x1283b0 and post-BREAK padding. No admitted category reaches it.'
        },
        'interval_notes': {
            'manager': 'Starts at initial branch0x12815c with live count loaded in delay slot. Ends before manager epilogue0x128430. Count is reloaded at each iteration. Per-event category is selected before callbacks; event transition and ID/source/object pointers are reloaded for a possible second callback. Category outside0..2 is excluded because it enters original debug-print path.',
            'release_router': 'Full original function0x1422a0..0x142344, including stack saves, restores, jr ra and its delay slot. Parent mask0x200 is checked once. Child bit0 required, bit3 excluded; bit2 and individual child masks are not tested.',
            'press_router': 'Full original function0x142350..0x1423f4, same contract with parent mask0x400 and child virtual slot0x64.',
            'mutations': 'Pointers identify bank records at each hook, so changing event ID/category/device inside a callback does not redirect its existing Record reference. Changes affect later original reloads. Router next is read after the callback.',
            'limits': 'Bounded initialized fixture pointers, at most64 events and32 records per bank,8 devices,32 router nodes. Cyclic or unbounded callback-driven traversal is outside domain and terminates with an instruction-budget exception. No RAM, live device prepass/reset/read, actual application object, concrete child effects, game installation or PC runtime validation.'
        }
    }
    for provenance, blocks in ((False, BLOCKS), (True, PROVENANCE)):
        for name, (start, end) in blocks.items():
            raw = extract(start, end)
            words = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
            manifest['provenance_only_blocks' if provenance else 'blocks'][name] = {
                'start': hex(start), 'end_exclusive': hex(end), 'words': len(words),
                'sha256': hashlib.sha256(raw).hexdigest()}
            listing.append(f'# {name}; {"provenance only, not executed" if provenance else "executed oracle interval"}')
            header += [f'inline constexpr std::uint32_t rr_{name}_48z_start = 0x{start:08x}u;',
                       f'inline constexpr std::uint32_t rr_{name}_48z_end = 0x{end:08x}u;',
                       f'inline constexpr std::uint32_t rr_{name}_48z[] = {{']
            for index, word in enumerate(words):
                pc = start + 4 * index
                instruction = f'.word 0x{word:08x}' if 'vtable' in name else decode(pc, word, not provenance)
                header.append(f'    0x{word:08x}u, // {pc:08x} {instruction}')
                listing.append(f'{pc:08x} {word:08x} {instruction}')
            header.append('};')
    files = {
        'tools/gameplay/event_dispatch_words48z.h': '\n'.join(header) + '\n',
        'evidence/eventDispatch48Z/original-disassembly.txt': '\n'.join(listing) + '\n',
        'evidence/eventDispatch48Z/recovery-manifest.json': json.dumps(manifest, indent=2) + '\n'
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
