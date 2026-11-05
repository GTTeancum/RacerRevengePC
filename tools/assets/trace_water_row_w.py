"""Bounded W-lane knownness analysis for a new Water row output allocation.

This is static dataflow over pinned instructions, not VU execution or proof of
unknown/exceptional floating-point self-subtraction. Unknown stays unknown.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys
sys.dont_write_bytecode = True
from trace_water_topology import EXPECTED_SHA256, ROOT, WATER_VECTOR_WORDS, file_bytes_at

MAIN = (0x299618, 0x29969C)
TAILS = ((0x2996C4, 0x299720), (0x299728, 0x299780))
ADD_ZERO_W = 0x4A202180
SUB_SELF_W = 0x4A2631AC


def block(words, registers, memory):
    for word in words:
        op, base, reg = word >> 26, (word >> 21) & 31, (word >> 16) & 31
        if op in (0x36, 0x3E):
            if word & 65535:
                raise ValueError('unexpected vector memory offset')
            role = {3: 'scratch', 6: 'normal', 7: 'output'}.get(base)
            if role is None:
                raise ValueError('unexpected vector memory base')
            if op == 0x36:
                if reg not in (4, 5):
                    raise ValueError('unexpected vector load register')
                registers[reg] = memory[role]
            else:
                if reg != 6 or role == 'normal':
                    raise ValueError('unexpected vector store')
                memory[role] = registers[6]
        elif op == 0x12:
            if word == ADD_ZERO_W:
                #VF0.x is architectural+0. Only known finite0/1 are represented.
                registers[6] = registers[4]
            elif word == SUB_SELF_W:
                registers[6] = 0 if registers[6] is not None else None
            elif word not in WATER_VECTOR_WORDS:
                raise ValueError(f'unsupported vector word {word:08x}')
            #All other whitelisted operations have no W destination.
        elif word != 0x27A30020: #ADDIU v1,sp,0x20; symbolic scratch address.
            raise ValueError(f'unsupported instruction {word:08x}')


def analyze(count, main, tails):
    if not 2 <= count <= 16384:
        raise ValueError('bounded row count must be2..16384')
    registers = {4: None, 5: None, 6: None}
    memory = {'normal': None, 'scratch': 1, 'output': 0}
    output = []
    for _ in range(count - 2):
        memory['output'] = 0 #Distinct new calloc output slot, not previous row.
        block(main, registers, memory)
        output.append(memory['output'])
    for tail in tails:
        memory['output'] = 0
        block(tail, registers, memory)
        output.append(memory['output'])
    return output


def inspect(data):
    if hashlib.sha256(data).hexdigest() != EXPECTED_SHA256:
        raise ValueError('original ELF SHA256 differs from source evidence')
    #Pin stack initialization, scalar XYZ-only stores, and count-2 loop test.
    expected = {0x299434: 0xF8600000, 0x299568: 0xE4E00000,
                0x299574: 0xE4E00004, 0x299584: 0xE4E00008,
                0x2996B4: 0x2463FFFE, 0x2996B8: 0x0083182A,
                0x2996BC: 0x1460FF84}
    for pc, word in expected.items():
        if int.from_bytes(file_bytes_at(data, pc, 4), 'little') != word:
            raise ValueError(f'instruction mismatch at {pc:08x}')
    def words(bounds):
        raw = file_bytes_at(data, bounds[0], bounds[1] - bounds[0])
        return [int.from_bytes(raw[i:i+4], 'little') for i in range(0, len(raw), 4)]
    main, tails = words(MAIN), [words(bounds) for bounds in TAILS]
    cases = []
    for count in (2, 3, 4, 17, 16384):
        outputs = analyze(count, main, tails)
        cases.append(dict(count=count, knownZero=sum(v == 0 for v in outputs),
                          unknown=sum(v is None for v in outputs), first=outputs[0], tails=outputs[-2:]))
    return dict(elfSha256=EXPECTED_SHA256,
        conditions=['new16*N output storage initially zero', '2<=N<=16384',
                    'scalar branches modify only outputXYZ', 'VF6.W unknown at function entry',
                    'normalW unknown; never substituted with padding zeros'],
        mainWords=[hex(w) for w in main], tailWords=[[hex(w) for w in t] for t in tails], cases=cases,
        conclusion='ForN>=3 every final outputW is established zero; forN=2 both outputW lanes remain unknown under this conservative model.',
        scope='Static bounded knownness only. No unknown/exceptional arithmetic, silicon flag, pre-existing-output arm, loader return or runtime claim.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    output = args.output.resolve();evidence = (ROOT / 'evidence').resolve()
    if output == evidence or not output.is_relative_to(evidence):
        parser.error('output must be a fresh repository evidence subdirectory')
    report = inspect(args.elf.read_bytes())
    output.mkdir(parents=True, exist_ok=False)
    (output / 'w-knownness.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(report['conclusion'])
    print(output / 'w-knownness.json')


if __name__ == '__main__':
    main()
