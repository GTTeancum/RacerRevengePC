#!/usr/bin/env python3
"""Freeze only input-shaping/limiter and its cosine helper from the verified ELF.
No RAM, generated translation, resources, emulator or install is used.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[2]
ELF_SHA256 = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS = {'input': (0x217de0, 0x2181c4), 'cosine': (0x13bba0, 0x13bca0)}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, w):
    op, rs, rt, rd, fd, fn = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31, (w >> 6) & 31, w & 63
    im = struct.unpack('<h', struct.pack('<H', w & 65535))[0]
    if not w: return 'nop'
    if op in (4, 5): return f'{"beq" if op == 4 else "bne"} {REG[rs]}, {REG[rt]}, {pc + 4 + 4*im:#x}'
    if op == 3: return f'jal {(w & 0x3ffffff)*4:#x}'
    if op == 9: return f'addiu {REG[rt]}, {REG[rs]}, {im:#x}'
    if op == 13: return f'ori {REG[rt]}, {REG[rs]}, {w & 65535:#x}'
    if op == 15: return f'lui {REG[rt]}, {w & 65535:#x}'
    if op in (0x20, 0x23, 0x28, 0x2b, 0x31, 0x39, 0x1e, 0x1f):
        name = {32:'lb', 35:'lw', 40:'sb', 43:'sw', 49:'lwc1', 57:'swc1', 30:'lq', 31:'sq'}[op]
        return f'{name} {"f"+str(rt) if op in (49,57) else REG[rt]}, {im:#x}({REG[rs]})'
    if op == 0 and fn == 8: return f'jr {REG[rs]}'
    if op == 0x1c and fn == 0x28 and fd == 0x18: return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
    if op == 0x11:
        if rs in (0,4): return f'{"mfc1" if rs == 0 else "mtc1"} {REG[rt]}, f{rd}'
        if rs == 8 and rt in (0,1): return f'{"bc1f" if rt == 0 else "bc1t"} {pc + 4 + 4*im:#x}'
        if rs == 20 and fn == 0x20: return f'cvt.s.w f{fd}, f{rd}'
        if rs == 16:
            names = {0:'add.s', 1:'sub.s', 2:'mul.s', 5:'abs.s', 6:'mov.s', 7:'neg.s', 0x18:'adda.s', 0x1a:'mula.s', 0x1c:'madd.s', 0x1e:'madda.s', 0x24:'cvt.w.s', 0x28:'max.s', 0x29:'min.s', 0x32:'c.eq.s', 0x34:'c.lt.s', 0x36:'c.le.s'}
            if fn in names: return f'{names[fn]} f{fd}, f{rd}, f{rt}'
    raise ValueError(f'Unrecognized selected instruction {pc:#x} {w:08x}')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--check', action='store_true', help='Verify frozen files without writing')
    args = ap.parse_args()
    data = (ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
    if hashlib.sha256(data).hexdigest() != ELF_SHA256: raise ValueError('Original ELF hash mismatch')
    if data[:6] != b'\x7fELF\x01\x01': raise ValueError('Expected little-endian ELF32')
    offset = struct.unpack_from('<I', data, 28)[0]
    size, count = struct.unpack_from('<HH', data, 42)
    segments = [struct.unpack_from('<8I', data, offset+i*size) for i in range(count)]
    def extract(start, end):
        for kind, off, va, _, filesz, _, _, _ in segments:
            if kind == 1 and va <= start < end <= va+filesz:
                return data[off+start-va:off+end-va]
        raise ValueError('Unmapped ELF extent')
    lines = ['#pragma once', '// Test-only original words, frozen by input_shaping_recover48v.py.', '#include <cstdint>']
    listing = []
    manifest = {'elf_sha256':ELF_SHA256, 'source':'work/native/project/disc/SLUS_202.68', 'ram_reads':0, 'generated_source_reads':0, 'blocks':{}}
    for name, (start, end) in BLOCKS.items():
        raw = extract(start,end)
        words = struct.unpack('<'+'I'*(len(raw)//4), raw)
        manifest['blocks'][name] = {'start':hex(start),'end_exclusive':hex(end),'words':len(words),'sha256':hashlib.sha256(raw).hexdigest()}
        lines += [f'inline constexpr std::uint32_t rr_{name}_48v_start = 0x{start:08x}u;', f'inline constexpr std::uint32_t rr_{name}_48v[] = {{']
        for i, word in enumerate(words):
            pc = start+4*i
            instruction = decode(pc,word)
            lines += [f'    0x{word:08x}u, // {pc:08x} {instruction}']
            listing += [f'{pc:08x} {word:08x} {instruction}']
        lines += ['};']
    files = {'tools/gameplay/input_shaping_words48v.h':'\n'.join(lines)+'\n',
             'evidence/inputShaping48V/original-disassembly.txt':'\n'.join(listing)+'\n',
             'evidence/inputShaping48V/recovery-manifest.json':json.dumps(manifest,indent=2)+'\n'}
    for name, text in files.items():
        path = ROOT/name
        if args.check:
            if path.read_text()!=text: raise ValueError('Frozen evidence differs: '+name)
        else:
            if path.exists() and path.read_text()!=text: raise ValueError('Refusing to overwrite different evidence: '+name)
            path.write_text(text)
    print(json.dumps({'status':'verified' if args.check else 'frozen',**manifest}))

if __name__ == '__main__': main()
