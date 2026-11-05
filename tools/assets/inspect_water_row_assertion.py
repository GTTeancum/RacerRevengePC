"""Inspect the pinned Water row-calculation assertion pair without executing it.

The scan reports direct instruction encodings only. It does not discover all
indirect aliases, prove control flow, or authorize arbitrary process writers.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.dont_write_bytecode = True
from trace_water_topology import EXPECTED_SHA256, ROOT, file_bytes_at

GP = 0x3DD970
GUARD = GP - 0x732C
ASSERTION = 0x443638


def inspect(data):
    if hashlib.sha256(data).hexdigest() != EXPECTED_SHA256:
        raise ValueError('original ELF SHA256 differs from source evidence')
    # Validate ELF identity and segment structure through the maintained reader.
    file_bytes_at(data, 0x299420, 4)
    phoff = struct.unpack_from('<I', data, 28)[0]
    phsize, phcount = struct.unpack_from('<HH', data, 42)
    segments = []
    for i in range(phcount):
        kind, offset, va, _, filesz, memsz, flags, _ = struct.unpack_from('<8I', data, phoff + i * phsize)
        if kind == 1:
            segments.append((offset, va, filesz, memsz, flags))
    globals_ = []
    for address, size in ((GUARD, 1), (ASSERTION, 4)):
        matches = [s for s in segments if s[1] <= address and address + size <= s[1] + s[3]]
        if len(matches) != 1:
            raise ValueError('global must belong to exactly one load segment')
        _, va, filesz, memsz, _ = matches[0]
        if address < va + filesz:
            raise ValueError('expected global wholly in ELF zero-fill suffix')
        globals_.append(dict(address=hex(address), size=size, segmentStart=hex(va),
                             fileBackedEnd=hex(va + filesz), memoryEnd=hex(va + memsz),
                             initialization='ELF zero-fill'))
    expected = {0x299454: 0x83838CD4, 0x299460: 0x24030001,
                0x299464: 0x3C010044, 0x299468: 0xAC233638,
                0x29946C: 0xA3838CD4, 0x299470: 0x3C010044,
                0x299474: 0x8C233638, 0x29948C: 0x0C04CD98}
    for pc, word in expected.items():
        if int.from_bytes(file_bytes_at(data, pc, 4), 'little') != word:
            raise ValueError(f'assertion instruction mismatch at {pc:08x}')
    candidates = []
    for offset, va, filesz, _, flags in segments:
        if not flags & 1:
            continue
        #The original load segment contains data as well as instructions. Report
        #candidates, not an inferred complete code map or reachable-writer set.
        for at in range(0, filesz - 3, 4):
            word = struct.unpack_from('<I', data, offset + at)[0]
            op, rs, imm = word >> 26, (word >> 21) & 31, word & 65535
            guard = rs == 28 and imm == 0x8CD4 and op >= 0x20
            assertion = imm == 0x3638 and op >= 0x20
            if guard or assertion:
                begin = max(0, at - 16)
                end = min(filesz & ~3, at + 8)
                candidates.append(dict(pc=hex(va + at), word=hex(word),
                    kind='GP-relative guard candidate' if guard else 'assertion low-half candidate',
                    context=[dict(pc=hex(va + j), word=hex(struct.unpack_from('<I', data, offset + j)[0]))
                             for j in range(begin, end, 4)]))
    raw = file_bytes_at(data, 0x299420, 0x9C)
    helper = file_bytes_at(data, 0x133660, 0x54)
    return dict(elfSha256=EXPECTED_SHA256, gp=hex(GP), globals=globals_,
        initializationOrder=['299468: assertion word = 1', '29946C: guard byte = 1'],
        calculationPrefixSha256=hashlib.sha256(raw).hexdigest(),
        assertionHelperSha256=hashlib.sha256(helper).hexdigest(),
        expectedWords={hex(pc): hex(word) for pc, word in expected.items()},
        directEncodingCandidates=candidates,
        scope='Pinned static bytes and ELF initialization only. No indirect-writer completeness, live process state, allocator return or loader completion claim.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    output = args.output.resolve()
    evidence = (ROOT / 'evidence').resolve()
    if output == evidence or not output.is_relative_to(evidence):
        parser.error('output must be a fresh subdirectory of repository evidence')
    report = inspect(args.elf.read_bytes())
    output.mkdir(parents=True, exist_ok=False)
    destination = output / 'assertion-state.json'
    destination.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(destination)
    print(f"Direct encoding candidates: {len(report['directEncodingCandidates'])}")


if __name__ == '__main__':
    main()
