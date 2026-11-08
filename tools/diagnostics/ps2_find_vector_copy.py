"""Find 128-bit copies (R5900 LQ then SQ) between given field offsets.

Usage: python -B ps2_find_vector_copy.py <elf> <src_off_hex> <dst_off_hex> [...pairs]
Reports addresses where `lq rt, src(base)` is followed within 12 instructions
by `sq rt, dst(base2)` with the same rt register.
"""
import struct
import sys


def main():
    data = open(sys.argv[1], 'rb').read()
    phoff = struct.unpack_from('<I', data, 0x1C)[0]
    phentsize, phnum = struct.unpack_from('<HH', data, 0x2A)
    segs = []
    for i in range(phnum):
        t, off, va, _pa, fs, _ms, _fl, _al = struct.unpack_from('<8I', data, phoff + i * phentsize)
        if t == 1 and fs:
            segs.append((va, off, fs))
    pairs = [(int(sys.argv[i], 16), int(sys.argv[i + 1], 16)) for i in range(2, len(sys.argv), 2)]
    for va, off, size in segs:
        words = struct.unpack_from(f'<{size // 4}I', data, off)
        for i, w in enumerate(words):
            if (w >> 26) != 0x1E:  # LQ
                continue
            base, rt, imm = (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
            for src, dst in pairs:
                if imm != src:
                    continue
                for j in range(i + 1, min(i + 13, len(words))):
                    w2 = words[j]
                    if (w2 >> 26) == 0x1F and ((w2 >> 16) & 31) == rt and (w2 & 0xFFFF) == dst:
                        print(f'{va + 4 * i:08x}: lq r{rt},{src:#x}(r{base}) -> {va + 4 * j:08x}: sq r{rt},{dst:#x}(r{(w2 >> 21) & 31})')
                        break


if __name__ == '__main__':
    main()
