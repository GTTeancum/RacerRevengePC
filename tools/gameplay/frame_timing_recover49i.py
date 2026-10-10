#!/usr/bin/env python3
"""Freeze 49I post-clock words and source context directly from the original ELF.

No production code, reconstructed guest code, RAM, resources, hardware sampler,
soft-float timestamp provider or opaque callback is executed/read by this tool.
Run with python3 -B; the old recover48b tool is never imported or executed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
from tv_camera_recover49e import decode as common_decode

ROOT = Path(__file__).resolve().parents[2]
DEST = ROOT / 'evidence/frameTiming49I/numeric-source'
ELF_SHA256 = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
SELECTED = {
    'bootstrap_selector': (0x140b10, 0x140b2c),
    'supplied_sample_store': (0x140be0, 0x140be4),
    'elapsed_numeric': (0x140be4, 0x140c8c),
    'late_flag_gate': (0x140c98, 0x140cac),
    'enter_phase_stores': (0x140cdc, 0x140ce8),
    'leave_phase_store': (0x140cfc, 0x140d00),
    'live_frame_tail': (0x140d2c, 0x140d4c),
    'reset_timestamp_stores': (0x140e38, 0x140e40),
}
CONTEXT = {
    'clock_and_call_order_context': (0x140b10, 0x140d64),
    'timestamp_reset_context': (0x140d70, 0x140e54),
}


def decode(pc, word):
    op, rs, rt = word >> 26, word >> 21 & 31, word >> 16 & 31
    if op in (37, 41):
        reg = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()
        imm = (word & 65535) - (65536 if word & 32768 else 0)
        return f'{"lhu" if op == 37 else "sh"} {reg[rt]}, {imm:#x}({reg[rs]})'
    return common_decode(pc, word)


def generate():
    source = ROOT / 'work/native/project/disc/SLUS_202.68'
    data = source.read_bytes()
    if hashlib.sha256(data).hexdigest() != ELF_SHA256:
        raise ValueError('Original ELF SHA256 mismatch')
    if data[:6] != b'\x7fELF\x01\x01':
        raise ValueError('Expected little-endian ELF32')
    phoff = struct.unpack_from('<I', data, 28)[0]
    phsize, phnum = struct.unpack_from('<HH', data, 42)
    segments = [struct.unpack_from('<8I', data, phoff+i*phsize) for i in range(phnum)]
    def extract(a, z):
        for kind, off, va, _, count, _, _, _ in segments:
            if kind == 1 and va <= a < z <= va + count:
                start = off+a-va
                return data[start:start+z-a], start
        raise ValueError(f'Unmapped source {a:#x}..{z:#x}')
    manifest = {
        'elf_sha256': ELF_SHA256,
        'source': 'work/native/project/disc/SLUS_202.68',
        'scope': 'Test-only exact selected original words with caller-owned synthetic fixture state and supplied already-quantized/saturating uint32 millisecond timestamp. Not a RAM reconstruction, game runtime or hardware-clock equivalence proof.',
        'ram_reads': 0, 'production_source_reads': 0,
        'generated_guest_source_reads': 0, 'resource_reads': 0,
        'selected_blocks': {}, 'context_only_blocks': {},
        'excluded_selected_word': {'pc':'0x140c04', 'reason':'Duplicate SRL is unreachable from selected elapsed entry: BLTZ taken goes to 140C08 after delay slot 140BF4; not-taken path branches from 140BFC to 140C20 after 140C00. It is frozen but never credited as executed.'},
        'execution_boundaries': [
            'Bootstrap selector executes through JAL 140D70 plus its delay slot, but stops at that external-provider boundary. Supplied converted uint32 stamp is placed in v0, only 140E38/140E3C stores execute, then caller resumes at 140B24.',
            'Nonzero-previous selector stops at 140B30. Hardware MMIO sampler, overflow bookkeeping, 64-bit/soft-float conversion and timestamp quantization/saturation are not executed. Supplied converted uint32 stamp resumes at original store 140BE0.',
            'Elapsed numeric block stops before 140C8C/140C90 manager dispatch. Post-manager flags are supplied separately to exact late gate 140C98..140CA8.',
            'Enter executes only original phase setup/store words 140CDC..140CE4. Optional pre-dispatch manager action and root virtual callback remain external. Leave executes only original store 140CFC after caller says the actual callback returned.',
            'Tail executes only 140D2C..140D48 after opaque 267BE0 stage. Live current/pending fields are supplied by caller; callback effects are neither invented nor executed.',
        ],
        'source_facts': {
            'delta': 'Low32 current230 - previous234, wrapping modulo 2^32.',
            'conversion': 'Signed nonnegative CVT.S.W, or high-bit sticky-half CVT.S.W followed by separately rounded ADD.S doubling.',
            'seconds_multiplier_bits':'0x3a83126f',
            'ordinary_upper_cap_bits':'0x3d75c28f',
            'secondary_upper_cap_bits':'0x3d08850a',
            'secondary_enable': 'Every nonzero original word at 423ED8 enables a second upper cap; this is not a fixed step.',
            'raw_store': '140C50 is the BC1T delay-slot SWC1. raw250 is always written before ordinary cap MOV.S and optional cap.',
            'late_skip': 'Only bit3 of the LIVE low byte at caller+228 controls the gate, after manager128080.',
            'reset_order': ['previous234', 'current230'],
            'enter_order': ['phase22C=2', 'lastDispatched23C=elapsedBits'],
            'leave': 'phase22C=0',
            'tail_order': ['previous234=LIVE current230', 'if LIVE pending120 != 0: active11C=1', 'pending120=0'],
        },
        'coverage_policy':'Only reached selected PCs count. Context-only source is never credited. Tests must execute all 68 reachable selected words; 69 selected words are frozen including unreachable 140C04.',
        'arithmetic_model':'Test-only integer binary32 round-to-nearest-even instruction operations. It does not call production arithmetic or host float conversion/multiplication.'
    }
    outputs = {}
    lines = []
    header = ['#pragma once', '#include <cstdint>', '// Generated only from hash-verified original ELF by frame_timing_recover49i.py.', 'namespace frame49i_source {', 'struct Word { std::uint32_t pc; std::uint32_t bits; };', 'inline constexpr Word words[] = {']
    for group, blocks in [('selected_blocks', SELECTED), ('context_only_blocks', CONTEXT)]:
        for name, (a, z) in blocks.items():
            raw, off = extract(a, z)
            words = struct.unpack('<'+'I'*(len(raw)//4), raw)
            manifest[group][name] = {'start':hex(a), 'end_exclusive':hex(z), 'words':len(words), 'elf_file_offset':hex(off), 'sha256':hashlib.sha256(raw).hexdigest()}
            outputs[name+'.bin'] = raw
            lines.append(f'# {group}: {name} [{a:#x}, {z:#x})')
            for i, word in enumerate(words):
                pc = a + i*4
                mnemonic = decode(pc, word)
                lines.append(f'{pc:08x} {word:08x} {mnemonic}')
                if group == 'selected_blocks':
                    header.append(f'  {{0x{pc:08x}u, 0x{word:08x}u}}, // {mnemonic}')
    header += ['};', 'inline constexpr std::uint32_t unreachableDuplicateShift = 0x140c04u;', '}']
    outputs['original_words49i.h'] = ('\n'.join(header)+'\n').encode()
    outputs['original-disassembly.txt'] = ('\n'.join(lines)+'\n').encode()
    outputs['recovery-manifest.json'] = (json.dumps(manifest, indent=2)+'\n').encode()
    return outputs, manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    files, manifest = generate()
    for name, data in files.items():
        path = DEST / name
        if args.check:
            if path.read_bytes() != data:
                raise ValueError('Frozen original source mismatch: '+str(path))
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            if path.exists() and path.read_bytes() != data:
                raise ValueError('Refusing to replace different frozen source: '+str(path))
            path.write_bytes(data)
    print(json.dumps({'status':'verified' if args.check else 'frozen', 'files':len(files), 'elf_sha256':ELF_SHA256, 'selected_words':sum(b['words'] for b in manifest['selected_blocks'].values()), 'unreachable_selected_words':1, 'context_executed':False}))

if __name__ == '__main__':
    main()
