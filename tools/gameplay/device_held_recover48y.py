#!/usr/bin/env python3
"""Freeze 48Y held query, direct context gate, packet body and event writer.

Reads only the hash-verified original ELF and this recovery's frozen outputs.
No RAM, translations, resources, production helpers or earlier oracle outputs.
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
BLOCKS = {'query': (0x143600, 0x1436f4),
          'gate': (0x143e30, 0x143ef4),
          'packet': (0x237ee0, 0x2381d0),
          'queue': (0x127ff0, 0x12807c)}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, word):
    op, rs, rt, rd, sa, fn = word >> 26, (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31, (word >> 6) & 31, word & 63
    immediate = struct.unpack('<h', struct.pack('<H', word & 65535))[0]
    if not word:
        return 'nop'
    if op in (4, 5, 0x14, 0x15):
        return f'{ {4:"beq", 5:"bne", 0x14:"beql", 0x15:"bnel"}[op]} {REG[rs]}, {REG[rt]}, {pc + 4 + 4 * immediate:#x}'
    if op == 1 and rt == 0:
        return f'bltz {REG[rs]}, {pc + 4 + 4 * immediate:#x}'
    if op == 3:
        return f'jal {((pc + 4) & 0xf0000000) | ((word & 0x3ffffff) << 2):#x}'
    if op in (9, 0x19, 0xa, 0xb):
        return f'{ {9:"addiu", 0x19:"daddiu", 0xa:"slti", 0xb:"sltiu"}[op]} {REG[rt]}, {REG[rs]}, {immediate:#x}'
    if op in (0xc, 0xd, 0xe):
        return f'{ {0xc:"andi", 0xd:"ori", 0xe:"xori"}[op]} {REG[rt]}, {REG[rs]}, {word & 65535:#x}'
    if op == 0xf:
        return f'lui {REG[rt]}, {word & 65535:#x}'
    if op in (0x23, 0x24, 0x25, 0x28, 0x2b, 0x31, 0x39, 0x1e, 0x1f):
        name = {0x23:'lw', 0x24:'lbu', 0x25:'lhu', 0x28:'sb', 0x2b:'sw', 0x31:'lwc1', 0x39:'swc1', 0x1e:'lq', 0x1f:'sq'}[op]
        return f'{name} {"f"+str(rt) if op in (0x31, 0x39) else REG[rt]}, {immediate:#x}({REG[rs]})'
    if op == 0:
        if fn in (0, 2, 0x3c, 0x3e):
            return f'{ {0:"sll", 2:"srl", 0x3c:"dsll32", 0x3e:"dsrl32"}[fn]} {REG[rd]}, {REG[rt]}, {sa}'
        if fn in (0x21, 0x24, 0x25, 0x26, 0x2a, 0x2b):
            return f'{ {0x21:"addu", 0x24:"and", 0x25:"or", 0x26:"xor", 0x2a:"slt", 0x2b:"sltu"}[fn]} {REG[rd]}, {REG[rs]}, {REG[rt]}'
        if fn == 8:
            return f'jr {REG[rs]}'
    if op == 0x1c and fn == 0x28 and sa == 0x18:
        return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
    if op == 0x11:
        if rs == 4:
            return f'mtc1 {REG[rt]}, f{rd}'
        if rs == 20 and fn == 0x20:
            return f'cvt.s.w f{sa}, f{rd}'
        if rs == 16 and fn in (0, 1, 2, 3):
            return f'{ {0:"add.s", 1:"sub.s", 2:"mul.s", 3:"div.s"}[fn]} f{sa}, f{rd}, f{rt}'
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
              '// Test-only original words, frozen by device_held_recover48y.py.']
    listing = []
    manifest = {
        'elf_sha256': ELF_SHA256,
        'source': 'work/native/project/disc/SLUS_202.68',
        'ram_reads': 0,
        'generated_source_reads': 0,
        'resource_reads': 0,
        'blocks': {},
        'external_input_boundaries': {
            'sdk_packet_read': {
                'target': '0x313350',
                'arguments': 'a0/a1 are synthetic device port/slot; a2 is packet destination',
                'hook': 'Copy exactly 32 fixture packet bytes to a2; v0 supplies explicit readSucceeded; return to original ra.'
            },
            'axis_update': {
                'target': '0x1438a0',
                'hook': 'Record a1 axis ID, f12 numerator and f13 denominator; no effect on held records; return to original ra.',
                'limitation': 'Axis normalization is outside 48Y and independently recovered by 48X. This is a declared trace-only boundary.'
            },
            'direct_gate': 'Original jal 0x143e30 executes frozen words, including its return and delay slot; never stubbed.',
            'event_writer': 'Original jal 0x127ff0 executes frozen words, all five event stores and queue-count update; never stubbed.'
        },
        'fixture_layout': {
            'device': 'count+0x4, descriptor pointer+0x14, deviceID+0x320, 32 six-word records+0x324; descriptor stride12 flags+0 offset+4 mask+8',
            'context': 'root+0x4 -> manager; manager+0x34 optional context; root byte+0x228; context words+0x518/+0x1898',
            'queue': 'manager+0x38 count initially0; manager+0x3c ordered20-byte entries, at most32',
            'axis_packet_offsets': [6, 7, 4, 0, 0, 5]
        },
        'admitted_domain_unreachable_instructions': {
            '0x143ec0': 'Redundant ANDI skipped by the preceding unconditional branch or targeted branch destination0x143ec4.',
            '0x237f94': 'Duplicate LUI skipped by the unconditional branch or targeted branch destination0x237f98.',
            '0x238020': 'Duplicate LUI skipped by the unconditional branch or targeted branch destination0x238024.',
            '0x237ffc..0x238014_inclusive': 'Seven instructions in negative conversion path after BLTZ; source is LBU and always0..255. The actual BLTZ delay slot0x237fec does execute.'
        },
        'interval_notes': {
            'query': 'Full original prologue, direct context gate, six-word conditional copy and epilogue, returns to a synthetic sentinel. Allowed index domain0..31 is checked only if original flow reaches record dereference.',
            'packet': 'Starts after readiness gate with s0=device, s3=stack+0x70. Success stops0x2381d0 before rumble. Read failure or nonzero packet[0] stops original branch target0x23830c after six neutral axis calls, skips transitions.',
            'queue': 'Complete leaf writer through jr ra and delay-slot count store.',
            'likely_slots': 'BEQL and BNEL execute their delay instruction only when taken. Low-pressure BNEL0x2380c8 targets0x238114, with descriptor increment in its taken delay slot.',
            'floating_point_domain': 'Host binary32 instruction-step arithmetic and signed CVT.S.W for bounded byte fixtures; not a full PS2 exceptional-value emulator.',
            'limitations': 'No readiness state machine, live SDK/device access, device discovery, rumble, full update epilogue, message consumers, player reader changes, RAM, installation or PC runtime validation.'
        }
    }
    for name, (start, end) in BLOCKS.items():
        raw = extract(start, end)
        words = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
        manifest['blocks'][name] = {'start': hex(start), 'end_exclusive': hex(end),
                                    'words': len(words), 'sha256': hashlib.sha256(raw).hexdigest()}
        header += [f'inline constexpr std::uint32_t rr_{name}_48y_start = 0x{start:08x}u;',
                   f'inline constexpr std::uint32_t rr_{name}_48y_end = 0x{end:08x}u;',
                   f'inline constexpr std::uint32_t rr_{name}_48y[] = {{']
        for index, word in enumerate(words):
            pc = start + 4 * index
            instruction = decode(pc, word)
            header.append(f'    0x{word:08x}u, // {pc:08x} {instruction}')
            listing.append(f'{pc:08x} {word:08x} {instruction}')
        header.append('};')
    files = {
        'tools/gameplay/device_held_words48y.h': '\n'.join(header) + '\n',
        'evidence/deviceHeld48Y/original-disassembly.txt': '\n'.join(listing) + '\n',
        'evidence/deviceHeld48Y/recovery-manifest.json': json.dumps(manifest, indent=2) + '\n'
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
