"""Disassemble address ranges of the original PS2 executable (SLUS_202.68).

Usage: python -B ps2_disasm.py <elf> <start_hex> <end_hex> [<start_hex> <end_hex> ...]
Reads the ELF program headers to map virtual addresses to file offsets and
prints MIPS (little-endian, 64-bit mode) disassembly. R5900-specific MMI
instructions that Capstone does not know are printed as raw words.
"""
import struct
import sys

import capstone


def segments(data):
    assert data[:4] == b'\x7fELF', 'not an ELF file'
    phoff = struct.unpack_from('<I', data, 0x1C)[0]
    phentsize, phnum = struct.unpack_from('<HH', data, 0x2A)
    out = []
    for i in range(phnum):
        p_type, p_offset, p_vaddr, _p_paddr, p_filesz, _p_memsz, _flags, _align = \
            struct.unpack_from('<8I', data, phoff + i * phentsize)
        if p_type == 1:
            out.append((p_vaddr, p_offset, p_filesz))
    return out


def read(data, segs, start, end):
    for vaddr, offset, size in segs:
        if vaddr <= start and end <= vaddr + size:
            return data[offset + start - vaddr: offset + end - vaddr]
    raise ValueError(f'range {start:#x}-{end:#x} not in a loaded segment')


def main():
    elf = sys.argv[1]
    data = open(elf, 'rb').read()
    segs = segments(data)
    md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS64 + capstone.CS_MODE_LITTLE_ENDIAN)
    args = sys.argv[2:]
    for i in range(0, len(args), 2):
        start, end = int(args[i], 16), int(args[i + 1], 16)
        code = read(data, segs, start, end)
        print(f'; ---- {start:#x}..{end:#x}')
        pc = start
        while pc < end:
            word = code[pc - start: pc - start + 4]
            insns = list(md.disasm(word, pc, 1))
            if insns:
                ins = insns[0]
                print(f'{pc:08x}:  {ins.mnemonic:8} {ins.op_str}')
            else:
                print(f'{pc:08x}:  .word    {struct.unpack("<I", word)[0]:#010x}')
            pc += 4


if __name__ == '__main__':
    main()
