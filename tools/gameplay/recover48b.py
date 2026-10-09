"""Bounded, offline gameplay recovery from the supplied Racer Revenge ELF/data.

Nothing here loads or executes game binaries. Outputs are evidence, not replacement
assets or a playable simulation. Paths from resource records never control writes.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import math
from pathlib import Path
import random
import re
import struct
import zlib

ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RAM_SHA = '38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
RES_SHA = '8361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8'
RAM_PATH = 'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin'
PROJECT = 'work/native/project'
BLOCK = 24576


def sha(data):
    return hashlib.sha256(data).hexdigest()


def require(ok, message):
    if not ok:
        raise ValueError(message)


def unpack_resource(blob):
    """Same independently documented v3 UI container, with bounded zlib output."""
    require(28 <= len(blob) <= 64 * 1024 * 1024, 'container size')
    def u32(off):
        require(0 <= off <= len(blob) - 4, 'truncated integer')
        return struct.unpack_from('<I', blob, off)[0]
    require((u32(0), u32(8), u32(12)) == (3, 0x340, 0), 'container header')
    meta, table_bytes = u32(4), u32(16)
    require(2 <= table_bytes <= 16384 and table_bytes % 2 == 0, 'size table')
    require(20 + table_bytes == 8 + meta <= len(blob), 'metadata bounds')
    sizes = struct.unpack_from('<' + 'H' * (table_bytes // 2), blob, 20)
    require(sizes[-1] == 0 and all(sizes[:-1]), 'block terminator')
    require(len(sizes) * BLOCK <= 128 * 1024 * 1024, 'virtual size')
    off = 8 + meta
    count, index_size = u32(off), u32(off + 4)
    off += 8
    end = off + index_size
    require(count <= 10000 and end <= len(blob), 'index extent')
    records = []
    for _ in range(count):
        n = u32(off)
        off += 4
        require(0 < n <= 1024 and off + n + 8 <= end, 'name extent')
        name = blob[off:off + n].decode('ascii')
        require('\0' not in name, 'name terminator')
        off += n
        start, length = u32(off), u32(off + 4)
        off += 8
        records.append({'name': name, 'logical_offset': start, 'bytes': length})
    require(off == end, 'index trailing bytes')
    decoded, blocks = [], []
    for index, size in enumerate(sizes[:-1]):
        require(off + size < len(blob) and blob[off + size] == 255, 'block boundary')
        d = zlib.decompressobj()
        data = d.decompress(blob[off:off + size], BLOCK + 1)
        require(d.eof and not d.unused_data and not d.unconsumed_tail and len(data) == BLOCK,
                'decoded block extent')
        blocks.append({'index': index, 'compressed_offset': off, 'compressed_bytes': size,
                       'decoded_sha256': sha(data)})
        decoded.append(data)
        off += size + 1
    require(off == len(blob), 'container trailing bytes')
    data = b''.join(decoded)
    require(all(r['logical_offset'] + r['bytes'] <= len(data) for r in records), 'logical extent')
    return records, blocks, data


def parse_phy(data):
    """Preserve keys/order/duplicates and original values; no runtime INI semantics implied."""
    text = data.decode('ascii')
    require('\0' not in text, 'NUL in PHY text')
    section, entries, sections, ignored = None, [], [], []
    for number, line in enumerate(text.splitlines(), 1):
        stripped = line.strip()
        if not stripped or stripped.startswith(';'):
            ignored.append(number)
            continue
        m = re.fullmatch(r'\[([^\[\]]+)\]', stripped)
        if m:
            section = m[1]
            sections.append({'name': section, 'line': number})
            continue
        require(section is not None and '=' in stripped, f'unrecognized PHY line {number}')
        key, raw = (s.strip() for s in stripped.split('=', 1))
        require(bool(key), f'empty key at {number}')
        entries.append({'section': section, 'key': key, 'raw_value': raw, 'line': number})
    require(bool(entries), 'empty PHY')
    return {'sections': sections, 'entries': entries, 'ignored_blank_or_comment_lines': ignored,
            'semantics': 'Lexical inventory only; original reader defaults, units and tuning precedence require separate proof.'}


class Elf:
    def __init__(self, data):
        require(sha(data) == ELF_SHA, 'unexpected original ELF hash')
        require(data[:6] == b'\x7fELF\x01\x01', 'ELF32 little endian')
        self.data = data
        off = struct.unpack_from('<I', data, 28)[0]
        size, count = struct.unpack_from('<HH', data, 42)
        self.segments = [struct.unpack_from('<8I', data, off + i * size) for i in range(count)]

    def file_offset(self, address, length=1):
        for kind, off, va, _, filesz, _, _, _ in self.segments:
            if kind == 1 and va <= address and address + length <= va + filesz:
                return off + address - va
        raise ValueError(f'unmapped original extent {address:#x}+{length}')

    def bytes(self, address, length):
        off = self.file_offset(address, length)
        return self.data[off:off + length]

    def word(self, address):
        return struct.unpack('<I', self.bytes(address, 4))[0]

    def string(self, address):
        off = self.file_offset(address)
        end = self.data.find(b'\0', off, off + 512)
        require(end >= 0, 'unterminated ELF string')
        return self.data[off:end].decode('ascii')


# Audited bounded source slices: verify their emitted original instruction comments
# against ELF bytes before exposing them as evidence. A slice is not a whole function.
SLICES = [
    ('entry_001a8060_0x1a8060.cpp', 0x1a8330, 0x1a8478, 'substep'),
    ('FUN_00219810_0x219810.cpp', 0x219810, 0x21a000, 'run_simulation'),
    ('FUN_001a0570_0x1a0570.cpp', 0x1a05a0, 0x1a05f8, 'player_input_binding'),
    ('FUN_001a1a20_0x1a1a20.cpp', 0x1a1a20, 0x1a1abc, 'player_input_reads'),
    ('FUN_00217de0_0x217de0.cpp', 0x217de0, 0x2181c4, 'input_shaping_and_rate_limit'),
    ('FUN_00217de0_0x217de0.cpp', 0x2184c0, 0x218588, 'steering_scale'),
    ('FUN_002146c0_0x2146c0.cpp', 0x2146c0, 0x214880, 'physical_tuning_loader'),
    ('entry_00214f6c_0x214f6c.cpp', 0x2150ec, 0x21532c, 'engine_tuning_loader'),
    ('entry_00214f6c_0x214f6c.cpp', 0x215a70, 0x215af8, 'joystick_tuning_loader'),
    ('FUN_00158a70_0x158a70.cpp', 0x158a70, 0x158ab0, 'float_reader'),
]


def source_evidence(root, elf, output):
    rows, dependencies = [], []
    rx = re.compile(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+(.*)', re.I)
    for filename, lo, hi, label in SLICES:
        path = root / PROJECT / 'generated' / filename
        data = path.read_bytes()
        lines = data.decode('utf-8').splitlines()
        records, seen = [], set()
        for line, text in enumerate(lines, 1):
            m = rx.fullmatch(text)
            if not m:
                continue
            address, word = int(m[1], 16), int(m[2], 16)
            if not lo <= address < hi:
                continue
            require(word == elf.word(address), f'generated instruction mismatch {address:#x}')
            if address in seen:
                continue
            seen.add(address)
            records.append({'pc': f'0x{address:08x}', 'word': f'0x{word:08x}',
                            'source_line': line, 'instruction': m[3]})
            if word >> 26 == 3:
                dependencies.append({'call_pc': f'0x{address:08x}',
                                     'target': f'0x{(word & 0x3ffffff) * 4:08x}',
                                     'slice': label})
        # Some generator slices omit harmless NOPs; nothing else may be absent.
        omitted = [a for a in range(lo, hi, 4) if a not in seen]
        require(all(elf.word(a) == 0 for a in omitted), f'non-NOP gap in {label}')
        p = output / 'source' / (label + '.json')
        write_json(p, {'file': str(path.relative_to(root)).replace('\\', '/'), 'sha256': sha(data),
                       'begin': hex(lo), 'end_exclusive': hex(hi),
                       'original_bytes_sha256': sha(elf.bytes(lo, hi - lo)),
                       'omitted_original_nops': [hex(a) for a in omitted], 'instructions': records})
        rows.append({'label': label, 'file': str(p.relative_to(output)).replace('\\', '/'),
                     'instructions_verified': len(records), 'original_sha256': sha(elf.bytes(lo, hi - lo))})
    return rows, dependencies


def float32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def fbits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def frombits(value):
    return struct.unpack('<f', struct.pack('<I', value))[0]


def cvt_bits(bits):
    e, negative = (bits >> 23) & 255, bool(bits >> 31)
    if e < 127:
        return 0
    if e >= 158:
        return -2147483648 if negative else 2147483647
    m = (bits & 0x7fffff) | 0x800000
    magnitude = m << (e - 150) if e >= 150 else m >> (150 - e)
    return -magnitude if negative else magnitude


def substep_formula(delta):
    scaled = float32(delta * 60.0)
    count = cvt_bits(fbits(scaled)) + 1
    require(0 < count <= 1000000, 'outside offline accepted count domain')
    return count, float32(delta / float32(count))


def execute_substep_words(elf, delta):
    """Independent bounded straight-line ELF-word diagnostic, never a game runtime.

    Only known operations in this 13-word interval are supported. FPU conversion
    uses Python finite truncation, separate from the recovered bit-field helper.
    Float operations use explicitly rounded binary32 under nearest-even host math.
    """
    g, f = [0] * 32, [0.0] * 32
    f[20] = delta
    for pc in range(0x1a8380, 0x1a83b4, 4):
        w = elf.word(pc)
        op, rs, rt, rd, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31, w & 65535
        si = imm if imm < 32768 else imm - 65536
        if w == 0:
            continue
        if op == 15:
            g[rt] = imm << 16
        elif op == 0x31:
            require((g[rs] + si) == 0x434188, 'unexpected substep load')
            f[rt] = 60.0
        elif op == 9:
            g[rt] = g[rs] + si
        elif w == 0x7000ae28:  # paddub s5,zero,zero
            g[21] = 0
        elif op == 0x11:
            fs, ft, fd, fun = rd, rt, (w >> 6) & 31, w & 63
            if rs == 0:
                g[rt] = fbits(f[fs])
                if g[rt] & 0x80000000:
                    g[rt] -= 1 << 32
            elif rs == 4:
                f[fs] = frombits(g[rt] & 0xffffffff)
            elif rs == 16 and fun == 2:
                f[fd] = float32(f[fs] * f[ft])
            elif rs == 16 and fun == 0x24:
                require(math.isfinite(f[fs]) and -2147483648 <= f[fs] < 2147483648, 'conversion domain')
                f[fd] = frombits(int(f[fs]) & 0xffffffff)
            elif rs == 20 and fun == 0x20:
                v = fbits(f[fs])
                f[fd] = float32(v if v < 0x80000000 else v - 0x100000000)
            elif rs == 16 and fun == 3:
                f[fd] = float32(f[fs] / f[ft])
            else:
                raise ValueError(f'unsupported offline FPU opcode at {pc:#x}')
        else:
            raise ValueError(f'unsupported offline opcode at {pc:#x}')
    return g[18], f[20]


def substep_fixtures(elf):
    cases = {0, 1, 0x80000000}
    for v in (1 / 240, 1 / 120, 1 / 60, 1 / 30, 1 / 20, 1 / 15, .25, .5, 1.0):
        b = fbits(v)
        cases.update(range(b - 16, b + 17))
    rng = random.Random(0x48b)
    cases.update(rng.randrange(0, fbits(1.0) + 1) for _ in range(4096))
    records = []
    for bits in sorted(cases):
        value = frombits(bits)
        expected = substep_formula(value)
        observed = execute_substep_words(elf, value)
        require((expected[0], fbits(expected[1])) == (observed[0], fbits(observed[1])), 'substep differential mismatch')
        records.append({'input_bits': f'{bits:08x}', 'input_float': value,
                        'count': expected[0], 'step_bits': f'{fbits(expected[1]):08x}'})
    return {'checks': len(records), 'records': records,
            'scope': 'Offline original-word diagnostic versus recovered formula; nearest-even binary32 arithmetic. Synthetic finite nonnegative cases around common intervals and 0..1; not an observed gameplay time domain or proof of PS2 FPU flags.',
            'float_conversion': 'CVT.W.S truncates toward zero; bit helper independently supports signed saturation. Host rounding mode does not affect its integer conversion.',
            'important': '1/60 rounded to binary32 produces count2, not count1. Preserve original arithmetic before choosing a native outer timestep.'}


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, allow_nan=False) + '\n', encoding='utf-8')


def inspect_snapshot(root, elf):
    path = root / RAM_PATH
    b = path.read_bytes()
    require(sha(b) == RAM_SHA and len(b) == 32 * 1024 * 1024, 'unexpected reference snapshot')
    def u(a):
        require(0 <= a <= len(b) - 4, 'RAM integer bounds')
        return struct.unpack_from('<I', b, a)[0]
    def number(a):
        value = frombits(u(a))
        return {'bits': f'{u(a):08x}', 'float': value if math.isfinite(value) else None}
    tuning = struct.pack('<6f', 250, 1, 450, 1, 4, 3)
    off, objects = 0, []
    while True:
        off = b.find(tuning, off)
        if off < 0:
            break
        a = off - 0xb84
        off += 1
        if not 0 <= a < len(b) - 0x10a8:
            continue
        vt = u(a + 8)
        if vt not in (0x3ce1e0, 0x3ced50):
            continue
        require(b[vt:vt + 0x60] == elf.bytes(vt, 0x60), 'mutated vtable')
        typeinfo = u(vt)
        require(u(typeinfo) == elf.word(typeinfo), 'mutated type name pointer')
        name = elf.string(u(typeinfo))
        o = {'address': hex(a), 'vtable': hex(vt), 'typeinfo': hex(typeinfo), 'class_name': name,
             'pointer_fields': {hex(k): hex(u(a + k)) for k in (0x190, 0x340, 0x344, 0x348, 0x34c, 0x350, 0x354, 0x358)},
             'axis_candidates': {hex(k): number(a + k) for k in range(0xb5c, 0xb9c, 4)},
             'gravity_scaled': {hex(k): number(a + k) for k in (0x378, 0x37c)},
             'object_c70_vector_candidate': [number(a + 0xc70 + i * 4) for i in range(4)]}
        body = u(a + 0x340)
        o['body_34_scalar_candidate'] = number(body + 0x34)
        o['body_70_vector_candidate'] = [number(body + 0x70 + i * 4) for i in range(4)]
        if name == 'PodPlayerVehicle':
            o['input_object'] = hex(u(a + 0xff0))
            o['input_index'] = u(a + 0xffc)
            o['input_source_components'] = {hex(k): {'address': hex(u(a + k)), 'value_at_24': number(u(a + k) + 0x24)}
                                            for k in (0xfe0, 0xfe4, 0xfe8, 0xfec)}
        objects.append(o)
    require(len(objects) == 8 and sum(o['class_name'] == 'PodPlayerVehicle' for o in objects) == 1, 'snapshot identification changed')
    return {'path': RAM_PATH, 'sha256': RAM_SHA, 'matching_objects': objects,
            'substep_scale': number(0x434188), 'substep_reciprocal': number(0x434190),
            'scope': 'Stopped VBlank7000 reference snapshot. Exact PHY tuning-pattern search plus original ELF vtable/type-name chain. This does not enumerate all possible vehicles, establish object allocation size, prove dynamic writers, or supply a before/after tick.'}


def tuning_field_proof(elf):
    rows = []
    for index, key in enumerate(('SteerSpeed1', 'SteerScale1', 'SteerSpeed2', 'SteerScale2', 'XAxisRateLimit', 'YAxisRateLimit')):
        pc, string, field = 0x215a80 + index * 20, 0x3b2918 + index * 16, 0xb84 + index * 4
        require(elf.string(string) == key, 'tuning key changed')
        expected = (0x3c02003b, 0x24450000 | (string & 65535), 0x26a60000 | field, 0x0c05629c, 0x72802628)
        require(tuple(elf.word(pc + i * 4) for i in range(5)) == expected, 'tuning call pattern changed')
        rows.append({'section': 'Joystick', 'key': key, 'string_va': hex(string),
                     'string_file_offset': hex(elf.file_offset(string)), 'call_pc': hex(pc + 12),
                     'float_reader': '0x158a70', 'destination': f'entry_a0+0x{field:x}',
                     'source_owner': 'FUN_002146c0', 'scope': 'Original reader destination proven; type-reader conversion/default path remains part of original helper.'})
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[2])
    ap.add_argument('--output', type=Path)
    args = ap.parse_args()
    root = args.root.resolve()
    output = (args.output or root / 'evidence/gameplay48B').resolve()
    require(output.is_relative_to(root / 'evidence/gameplay48B'), 'output must remain in owned evidence area')
    elf_path, res_path = root / PROJECT / 'disc/SLUS_202.68', root / PROJECT / 'disc/UI/PODPHYS.RES'
    elf = Elf(elf_path.read_bytes())
    blob = res_path.read_bytes()
    require(sha(blob) == RES_SHA, 'unexpected PODPHYS.RES hash')
    records, blocks, decoded = unpack_resource(blob)
    resources = []
    for i, r in enumerate(records):
        require(re.fullmatch(r'pods\\pod\d{2}\\pod\d{2}\.phy', r['name'], re.I), 'unexpected PHY record name')
        data = decoded[r['logical_offset']:r['logical_offset'] + r['bytes']]
        # Synthetic ordinal destination: no resource-provided path is traversed.
        name = f'pod-{i + 1:02d}.phy'
        p = output / 'phy' / name
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_bytes(data)
        parsed = parse_phy(data)
        write_json(p.with_suffix('.json'), parsed)
        resources.append({**r, 'extracted_file': 'phy/' + name, 'sha256': sha(data),
                          'section_count': len(parsed['sections']), 'entry_count': len(parsed['entries'])})
    require(len(resources) == 23, 'unexpected PHY count')
    write_json(output / 'phy-index.json', {'container': str(res_path.relative_to(root)).replace('\\', '/'),
               'sha256': RES_SHA, 'blocks': blocks, 'records': resources,
               'decoder_reference': PROJECT + '/scripts/validate_ui_resources07.py'})
    slices, calls = source_evidence(root, elf, output)
    write_json(output / 'source-index.json', {'elf_sha256': ELF_SHA, 'slices': slices, 'direct_call_dependencies': calls,
               'scope': 'Selected original-code slices only. Indirect-call targets and hidden memory dependencies need live capture.'})
    write_json(output / 'tuning-fields.json', tuning_field_proof(elf))
    fixture = substep_fixtures(elf)
    write_json(output / 'substep-fixtures.json', fixture)
    snapshot = inspect_snapshot(root, elf)
    write_json(output / 'snapshot-identification.json', snapshot)
    program = elf.bytes(0x2180ac, 0x2181c4 - 0x2180ac)
    words = struct.unpack('<' + str(len(program) // 4) + 'I', program)
    # The test-only program is generated from the same hash-checked original ELF.
    # It is not linked into the native game helper.
    header = ['#pragma once', '// Test-only original ELF words; do not link into a game runtime.',
              '// ELF SHA256 ' + ELF_SHA, '// Interval 0x2180AC..0x2181C4 SHA256 ' + sha(program),
              '#include <cstdint>', 'inline constexpr std::uint32_t rrControlFilterWords48B[] = {']
    header += ['    ' + ', '.join(f'0x{w:08x}u' for w in words[i:i + 6]) + ',' for i in range(0, len(words), 6)]
    header += ['};', '']
    (root / 'tools/gameplay/control_filter_words48b.h').write_text('\n'.join(header), encoding='utf-8')
    write_json(output / 'rate-filter-program.json', {'begin': '0x2180ac', 'end_exclusive': '0x2181c4',
               'sha256': sha(program), 'words': [f'{w:08x}' for w in words],
               'scope': 'Test-only original instruction fixture for independent native helper validation.'})
    products = []
    for p in sorted(output.rglob('*')):
        if p.is_file() and p.name not in ('manifest.json', 'recovery.log', 'tests.log'):
            products.append({'path': str(p.relative_to(output)).replace('\\', '/'), 'bytes': p.stat().st_size,
                             'sha256': sha(p.read_bytes())})
    manifest = {'original_elf_sha256': ELF_SHA, 'original_container_sha256': RES_SHA,
                'reference_ram_sha256': RAM_SHA, 'tool_sha256': sha(Path(__file__).read_bytes()),
                'products': products, 'scope': 'Offline recovery evidence only; no game or reference runtime mutation.'}
    write_json(output / 'manifest.json', manifest)
    # Reopen output evidence, including raw resources, rather than trusting writes.
    for item in products:
        require(sha((output / item['path']).read_bytes()) == item['sha256'], 'output readback mismatch')
    print(json.dumps({'phy_resources': len(resources), 'source_slices': len(slices),
                      'substep_differential_cases': fixture['checks'], 'snapshot_typed_objects': len(snapshot['matching_objects']),
                      'products_hash_verified': len(products), 'output': str(output)}, indent=2))


if __name__ == '__main__':
    main()
