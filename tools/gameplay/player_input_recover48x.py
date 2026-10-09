#!/usr/bin/env python3
"""Freeze 48X axis arithmetic, player reader, and state gate from the original ELF.

This standalone recovery reads only the verified ELF and its own frozen outputs.
It does not read RAM, generated translations, game resources, or 48V/48W outputs.
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
BLOCKS = {'axis': (0x127da4, 0x127e38),
          'reader': (0x1a1a20, 0x1a1dcc),
          'gate': (0x1b0b80, 0x1b0bb0)}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, word):
    op, rs, rt, rd, sa, fn = word >> 26, (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31, (word >> 6) & 31, word & 63
    immediate = struct.unpack('<h', struct.pack('<H', word & 65535))[0]
    if not word:
        return 'nop'
    if op in (4, 5, 0x14):
        return f'{ {4:"beq", 5:"bne", 0x14:"beql"}[op]} {REG[rs]}, {REG[rt]}, {pc + 4 + 4 * immediate:#x}'
    if op == 3:
        return f'jal {((pc + 4) & 0xf0000000) | ((word & 0x3ffffff) << 2):#x}'
    if op in (9, 0x19):
        return f'{"addiu" if op == 9 else "daddiu"} {REG[rt]}, {REG[rs]}, {immediate:#x}'
    if op in (0xc, 0xd, 0xe):
        return f'{ {0xc:"andi", 0xd:"ori", 0xe:"xori"}[op]} {REG[rt]}, {REG[rs]}, {word & 65535:#x}'
    if op == 0xf:
        return f'lui {REG[rt]}, {word & 65535:#x}'
    if op in (0x23, 0x24, 0x28, 0x2b, 0x31, 0x39, 0x1f):
        name = {0x23:'lw', 0x24:'lbu', 0x28:'sb', 0x2b:'sw', 0x31:'lwc1', 0x39:'swc1', 0x1f:'sq'}[op]
        return f'{name} {"f"+str(rt) if op in (0x31, 0x39) else REG[rt]}, {immediate:#x}({REG[rs]})'
    if op == 0:
        if fn in (0, 0x3c, 0x3e):
            return f'{ {0:"sll", 0x3c:"dsll32", 0x3e:"dsrl32"}[fn]} {REG[rd]}, {REG[rt]}, {sa}'
        if fn in (0x21, 0x24, 0x25, 0x2b):
            return f'{ {0x21:"addu", 0x24:"and", 0x25:"or", 0x2b:"sltu"}[fn]} {REG[rd]}, {REG[rs]}, {REG[rt]}'
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
            return f'{"bc1f" if rt == 0 else "bc1t"} {pc + 4 + 4 * immediate:#x}'
        if rs == 16:
            if fn in (0, 1, 2, 3):
                return f'{ {0:"add.s", 1:"sub.s", 2:"mul.s", 3:"div.s"}[fn]} f{sa}, f{rd}, f{rt}'
            if fn in (0x34, 0x36):
                return f'{"c.lt.s" if fn == 0x34 else "c.le.s"} f{rd}, f{rt}'
    raise ValueError(f'Unrecognized selected instruction {pc:#x} {word:08x}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='Verify frozen outputs without writing')
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
              '// Test-only original words, frozen by player_input_recover48x.py.']
    listing = []
    manifest = {
        'elf_sha256': ELF_SHA256,
        'source': 'work/native/project/disc/SLUS_202.68',
        'ram_reads': 0,
        'generated_source_reads': 0,
        'resource_reads': 0,
        'blocks': {},
        'external_input_boundary': {
            'kind': 'test-only synthetic virtual-query target',
            'dispatch': 'player+0xff0 -> device+0x18 -> virtual slot+0x10',
            'arguments': 'a1=signed code, a2=mask 0x3f, a3=caller scratch',
            'result': 'callback outcome supplies whether original v0 is nonzero',
            'limitations': 'The original device implementation and event system are not executed. Scratch results are unused by the frozen reader.',
            'direct_state_gate_call': 'Original jal 0x1b0b80 executes the frozen gate block; never stubbed.'
        },
        'interval_notes': {
            'axis': 'Numeric body only: s0 points to synthetic axis state; f21/f20 supplied by fixture; stops after result store.',
            'reader': 'Stops at 0x1a1dcc before stack-restoration epilogue.',
            'gate': 'Includes jr ra and its delay slot; BEQL not-taken delay slot is annulled.',
            'floating_point_domain': 'Host binary32 instruction-step arithmetic for finite fixtures; not a complete PS2 exceptional-value FPU emulator.'
        }
    }
    for name, (start, end) in BLOCKS.items():
        raw = extract(start, end)
        words = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
        manifest['blocks'][name] = {'start': hex(start), 'end_exclusive': hex(end),
                                    'words': len(words), 'sha256': hashlib.sha256(raw).hexdigest()}
        header += [f'inline constexpr std::uint32_t rr_{name}_48x_start = 0x{start:08x}u;',
                   f'inline constexpr std::uint32_t rr_{name}_48x_end = 0x{end:08x}u;',
                   f'inline constexpr std::uint32_t rr_{name}_48x[] = {{']
        for index, word in enumerate(words):
            pc = start + 4 * index
            instruction = decode(pc, word)
            header.append(f'    0x{word:08x}u, // {pc:08x} {instruction}')
            listing.append(f'{pc:08x} {word:08x} {instruction}')
        header.append('};')
    files = {
        'tools/gameplay/player_input_words48x.h': '\n'.join(header) + '\n',
        'evidence/playerInput48X/original-disassembly.txt': '\n'.join(listing) + '\n',
        'evidence/playerInput48X/recovery-manifest.json': json.dumps(manifest, indent=2) + '\n'
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
