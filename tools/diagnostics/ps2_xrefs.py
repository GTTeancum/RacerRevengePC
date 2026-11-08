"""List direct JAL/J call sites to target addresses in a PS2 ELF.

Usage: python -B ps2_xrefs.py <elf> <target_hex> [<target_hex> ...]
"""
import struct
import sys


def main():
    data = open(sys.argv[1], 'rb').read()
    phoff = struct.unpack_from('<I', data, 0x1C)[0]
    phentsize, phnum = struct.unpack_from('<HH', data, 0x2A)
    targets = {int(t, 16) for t in sys.argv[2:]}
    for i in range(phnum):
        t, off, va, _pa, fs, _ms, _fl, _al = struct.unpack_from('<8I', data, phoff + i * phentsize)
        if t != 1 or not fs:
            continue
        words = struct.unpack_from(f'<{fs // 4}I', data, off)
        for k, w in enumerate(words):
            op = w >> 26
            if op in (2, 3):
                pc = va + 4 * k
                dest = ((pc + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
                if dest in targets:
                    print(f'{pc:08x}: {"jal" if op == 3 else "j"} {dest:#x}')


if __name__ == '__main__':
    main()
