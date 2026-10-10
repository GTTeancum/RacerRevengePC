"""Original spring detector numeric preparation, with initialized objects only.

No RAM, backing data image, instruction patches, helper interception, or native
expected values. Prepared scenes and dirty parentless scenes return naturally.
The original detector runs to its real registration/query dispatch boundary.
"""
import hashlib
import json
import math
import random
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
from query_oracle48h import bits, fp, signed
from spring_fixture48j import SpringOriginal

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/springDetector48O'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
RANGES = [('detector', 0x295630, 0x295a40),
          ('world_point', 0x28bab0, 0x28bbb0),
          ('spring_reset', 0x296490, 0x2964b8),
          ('construct_vec4', 0x106790, 0x1067ac),
          ('stage_type3', 0x11ca80, 0x11cb20),
          ('registration_prefix', 0x11fe50, 0x11ff74)]
RANGE_HASHES = {
    'detector': '652e36380071b7276894c259d44b0935fc08e6953407e4114395a8064d8ad9f5',
    'world_point': 'c2ab755092a6832cab606e4b528f75cefc9b147084cbd0e3343ebb6256907ffb',
    'spring_reset': '72b410005b683604e41678e1ee01b7f49bebac96a8422d6452145d895466da71',
    'construct_vec4': 'e0d8c70d997a0996626a12d2184e4cbbfd221c249fea4f54d3c5e505dec51dfd',
    'stage_type3': '05fb87232185b927f953359af1071bb6d40073247878e0bf64c7bad8185bbbd3',
    'registration_prefix': 'a2473ea4c6846493afc8d5d1744af0c89c0d54f64bf70093f48c2421a0d7845b',
}
D, S, RECORDS, PAIRS, Q, SHAPE, STAGED, AUX = (0x10000, 0x11000, 0x12000, 0x13000,
                                              0x14000, 0x15000, 0x16000, 0x17000)
SCENES = [0x20000 + i * 0x1000 for i in range(4)]
GP, RETURN = 0x3dd970, 0xfffffff0
DISPATCH = {0x388a2c: 0x11ff4c}
STATE_OFFSETS = (0x38, 0x3c, 0x40, 0x44, 0x74, 0x78, 0x130, 0x7c, 0x84, 0x88, 0xd4)
IDENTITY = [1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.]
CONTROL_NAMES = ['spring_present', 'pairs_present', 'provider_present',
                 'spring_provider_enabled', 'query_present', 'query_enabled',
                 'reset', 'explicit_list', 'shape_present', 'shape_count',
                 'initial_query_type']
INPUT_WORDS, OUTPUT_WORDS = 250, 160
BOUNDARIES = {0x11ff74: 2, 0x295a08: 3, 0x295a1c: 4,
              0x295740: 5, 0x28baf4: 6}


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def floats(values):
    return [bits(x) for x in values]


def put(m, address, values):
    for index, value in enumerate(values):
        m.write(address + 4 * index, value)


def get(m, address, count):
    return [m.read(address + 4 * index) for index in range(count)]


def freeze():
    path = OUT / 'original-words.json'
    if '--frozen' in sys.argv:
        proof = json.loads(path.read_text())
        assert proof['elf_sha256'] == ELF_SHA
    else:
        elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
        assert sha(elf) == ELF_SHA
        for address, word in DISPATCH.items():
            assert struct.unpack_from('<I', elf, address - 0x100000 + 0x80)[0] == word
        blocks = []
        for name, start, end in RANGES:
            raw = elf[start - 0x100000 + 0x80:end - 0x100000 + 0x80]
            assert sha(raw) == RANGE_HASHES[name]
            rows = [dict(pc=pc, word=struct.unpack_from('<I', raw, pc-start)[0])
                    for pc in range(start, end, 4)]
            blocks.append(dict(name=name, start=start, end=end,
                               sha256=sha(raw), instructions=rows))
            (OUT / (name + '.bin')).write_bytes(raw)
        proof = dict(elf_sha256=ELF_SHA,
                     mapping='ELF file offset = VA - 0x100000 + 0x80', blocks=blocks,
                     dispatch=[dict(address=a, word=w) for a, w in DISPATCH.items()])
        path.write_text(json.dumps(proof, indent=2) + '\n')
    words = {}
    assert {row['address']: row['word'] for row in proof['dispatch']} == DISPATCH
    assert len(proof['blocks']) == len(RANGES)
    for block, (name, start, end) in zip(proof['blocks'], RANGES):
        assert (block['name'], block['start'], block['end']) == (name, start, end)
        rows = block['instructions']
        assert [r['pc'] for r in rows] == list(range(start, end, 4))
        raw = b''.join(struct.pack('<I', r['word']) for r in rows)
        assert sha(raw) == block['sha256']
        assert block['sha256'] == RANGE_HASHES[name]
        assert raw == (OUT / (name + '.bin')).read_bytes()
        words.update((r['pc'], r['word']) for r in rows)
    return words, proof


class Guarded(SpringOriginal):
    """Read/write observation plus an explicit scalar MIN/MAX comparison.

    Signed-word ordering matches contact_oracle48g and PCSX2 FPU.cpp fp_min /
    fp_max. SpringOriginal now uses the same corrected rule. The optional old
    model is pinned explicitly below to report historical signed-zero differences,
    never to generate expected fixtures.
    """
    def __init__(self, words, word_minmax=True):
        super().__init__(data=b'', words=words)
        self.allowed_source_addresses = set()
        self.executing = False
        self.read_bytes = set()
        self.written_bytes = set()
        self.word_minmax = word_minmax
        self.minmax_deltas = []

    def execute(self, pc):
        word = self.words[pc]
        if word >> 26 == 0x11 and (word >> 21) & 31 == 16 and word & 63 in (0x28, 0x29):
            self.pc = pc
            self.steps += 1
            self.coverage.add(pc)
            self.annul = False
            x = self.f[(word >> 11) & 31]
            y = self.f[(word >> 16) & 31]
            a, b = signed(bits(x)), signed(bits(y))
            want_max = word & 63 == 0x28
            # Negative float encodings descend as their numerical values rise.
            integer_max = want_max != (a < 0 and b < 0)
            selected = max(a, b) if integer_max else min(a, b)
            previous = bits((x if x > y else y) if want_max else (x if x < y else y))
            result = selected & 0xffffffff
            if self.word_minmax and result != previous:
                self.minmax_deltas.append(dict(pc=f'{pc:08X}', first=bits(x), second=bits(y),
                                              word_order=result, legacy_float_comparison=previous))
            self.f[(word >> 6) & 31] = fp(result if self.word_minmax else previous)
            return None
        return super().execute(pc)

    def read(self, address, size=4):
        if self.executing:
            self.read_bytes.update(range(address, address + size))
        return super().read(address, size)

    def write(self, address, value, size=4):
        if self.executing:
            self.written_bytes.update(range(address, address + size))
        super().write(address, value, size)


def make_case(name):
    points = [[-3., 2., 7., -9.], [5., -6., 1., 12.],
              [1., 8., -4., .25], [7., 3., 2., 99.]]
    records = []
    for i, local in enumerate(points):
        world = IDENTITY.copy()
        world[12:15] = [3. * i, -2. * i, 5. * i]
        root_local = IDENTITY.copy()
        root_local[12:15] = [-7. * i, 4. * i, -3. * i]
        records.append(dict(local=floats(local), world=floats(world),
                            scene_local=floats(root_local), flags=1))
    return dict(name=name, controls=[1, 1, 0, 0, 1, 1, 1, 1, 1, 3, 0x76543210],
                detector=floats([101., 102., 103., -11., 201., 202., 203., -12.]),
                state=[0x11111111, 0x22222222, 0x33333333, 0x44444444,
                       0x55555555, 0x66666666, 0x77777777, 3, 17, 23, 0x88888888],
                records=records, pairs=floats([300. + i for i in range(32)]),
                staged=floats([400. + i for i in range(32)]),
                aux=[0x12340000 + i for i in range(8)])


def encode_input(case):
    result = case['controls'] + case['detector'] + case['state']
    for record in case['records']:
        result += record['local'] + record['world'] + record['scene_local'] + [record['flags']]
    result += case['pairs'] + case['staged'] + case['aux']
    assert len(result) == INPUT_WORDS
    return result


def execute_case(words, case, word_minmax=True):
    m = Guarded(words, word_minmax)
    c = dict(zip(CONTROL_NAMES, case['controls']))
    for offset, value in ((0x30, S if c['spring_present'] else 0),
                          (0x34, 0x18000 if c['provider_present'] else 0),
                          (0x38, Q if c['query_present'] else 0),
                          (0x60, PAIRS if c['pairs_present'] else 0),
                          (0x68, c['query_enabled']), (0x6c, c['reset']),
                          (0x70, 0x19000),
                          (0x74, 0x1a000 if c['explicit_list'] else 0)):
        m.write(D + offset, value)
    put(m, D + 0x40, case['detector'])
    put(m, S + 0x48, [c['spring_provider_enabled']])
    for offset, value in zip(STATE_OFFSETS, case['state']):
        m.write(S + offset, value)
    m.write(S + 0x80, RECORDS)
    m.write(Q + 0x88, c['initial_query_type'])
    m.write(Q + 0x8c, SHAPE if c['shape_present'] else 0)
    m.write(Q + 0xe0, 0x1b000)  # Opaque registration identity; never dereferenced.
    put(m, SHAPE, [c['shape_count'], AUX, STAGED])
    put(m, PAIRS, case['pairs'])
    put(m, STAGED, case['staged'])
    put(m, AUX, case['aux'])
    m.write(0x19000, 7)  # Explicit candidate-count cell, consumed before query call.
    m.write(GP - 0x7360, case.get('static_flags', [1, 1])[0], 1)
    m.write(GP - 0x735c, case.get('static_flags', [1, 1])[1], 1)
    m.write(0x443590, 0x13579bdf)
    m.write(0x443598, 0x2468ace0)
    for address, word in DISPATCH.items():
        m.write(address, word)
    initial_records = []
    for index, record in enumerate(case['records']):
        address = RECORDS + index * 0x60
        raw = [0x42000000 + 100 * index + j for j in range(24)]
        raw[4:8] = record['local']
        raw[16] = SCENES[index]
        initial_records.append(raw)
        put(m, address, raw)
        put(m, SCENES[index] + 0x100, record['world'])
        put(m, SCENES[index] + 0xc0, record['scene_local'])
        m.write(SCENES[index] + 0x190, record['flags'], 1)
        m.write(SCENES[index] + 0x180, record.get('parent', 0))
    m.g[4][0] = D
    m.g[28][0] = GP
    m.g[29][0] = 0x1ff0000
    m.g[31][0] = RETURN

    def snapshot():
        return dict(detector=get(m, D + 0x40, 8),
                    state=[m.read(S + off) for off in STATE_OFFSETS],
                    pairs=get(m, PAIRS, 32), query_type=m.read(Q + 0x88),
                    staged=get(m, STAGED, 32), aux=get(m, AUX, 8),
                    scenes=[dict(world=get(m, scene + 0x100, 16),
                                 flags=m.read(scene + 0x190, 1)) for scene in SCENES])

    pc = 0x295630
    stages = []
    while pc not in BOUNDARIES and pc != RETURN:
        if pc in (0x295944, 0x2959d4, 0x2959e4):
            stages.append(dict(pc=f'{pc:08X}', **snapshot()))
        m.executing = True
        target = m.execute(pc)
        if target is not None:
            if not m.annul:
                assert m.execute(pc + 4) is None
            pc = target
        else:
            pc += 4
        m.executing = False
    final = snapshot()
    out = final['detector'] + final['state'] + final['pairs'] + [final['query_type']]
    out += final['staged'] + final['aux']
    for scene in final['scenes']:
        out += scene['world'] + [scene['flags']]
    assert len(out) == OUTPUT_WORDS
    status = BOUNDARIES.get(pc, 0 if not c['spring_present'] else 1)
    assert m.data == b'' and not m.source_reads
    assert not ({0x120230, 0x122660, 0x121920, 0x2e83d0, 0x2897b0} & m.coverage)
    full_records = [get(m, RECORDS + i * 0x60, 24) for i in range(4)]
    if status != 5:
        assert full_records == initial_records
    assert [get(m, scene + 0xc0, 16) for scene in SCENES] == [r['scene_local'] for r in case['records']]
    if not c['spring_present']:
        assert not any(S <= address < S + 0x140 for address in m.read_bytes)
        assert not any(D <= address < D + 0x80 for address in m.written_bytes)
    assert not any(Q + 0xe0 <= address < Q + 0xe4 for address in m.read_bytes)
    if status < 5 and c['spring_present'] and c['pairs_present'] and 0 < case['state'][7] <= 4:
        for index in range(case['state'][7]):
            assert final['pairs'][index * 8 + 7] == bits(1.)
        assert final['detector'][3] == bits(1.) and final['detector'][7] == bits(1.)
    report = dict(name=case['name'], input_words=encode_input(case), status=status,
                  output_words=out, stop_pc=f'{pc:08X}', return_word=m.g[2][0] if pc == RETURN else None,
                  instructions=m.steps, source_byte_reads=0, stages=stages,
                  initial_records=initial_records, final_records=full_records,
                  scene_local_after=[get(m, scene + 0xc0, 16) for scene in SCENES],
                  static_flags=[m.read(GP - 0x7360, 1), m.read(GP - 0x735c, 1)],
                  static_words=[m.read(0x443590), m.read(0x443598)],
                  executed_pcs=[f'{x:08X}' for x in sorted(m.coverage)],
                  original_read_bytes=len(m.read_bytes), original_written_bytes=len(m.written_bytes))
    report['minmax_model_deltas'] = m.minmax_deltas
    return report, m


def cases():
    result = []
    def add(name, change=None):
        case = make_case(name)
        if change:
            change(case)
        result.append(case)

    add('prepared-scenes-three-points')
    for linked in (0, 1):
        for pairs in (0, 1):
            for query in (0, 1):
                for enabled in (0, 1):
                    for reset in (0, 1):
                        def change(c, linked=linked, pairs=pairs, query=query, enabled=enabled, reset=reset):
                            c['controls'][0:2] = [linked, pairs]
                            c['controls'][4:7] = [query, enabled, reset]
                            if not pairs:
                                c['controls'][10] = 3
                        add(f'gates-spring{linked}-pairs{pairs}-query{query}-enable{enabled}-reset{reset}', change)
    for point_count in range(1, 5):
        for shape_count in range(5):
            for explicit in (0, 1):
                def change(c, point_count=point_count, shape_count=shape_count, explicit=explicit):
                    c['state'][7] = point_count
                    c['controls'][7] = explicit
                    c['controls'][9] = shape_count
                add(f'points{point_count}-shape{shape_count}-list{explicit}', change)
    for shape in (0, 1):
        for provider, provider_enabled in ((0, 0), (1, 0), (0, 1)):
            def change(c, shape=shape, provider=provider, provider_enabled=provider_enabled):
                c['controls'][2:4] = [provider, provider_enabled]
                c['controls'][8] = shape
            add(f'shape{shape}-provider{provider}-providerEnabled{provider_enabled}', change)
    for count in (0, 4, 0xffffffff, 0x80000000):
        for provider, provider_enabled in ((0, 0), (1, 0), (0, 1)):
            def change(c, count=count, provider=provider, provider_enabled=provider_enabled):
                c['state'][7] = count
                c['controls'][1:4] = [0, provider, provider_enabled]
                c['controls'][10] = 3
            add(f'no-pairs-count{count:08X}-provider{provider}-enabled{provider_enabled}', change)
    for scene_flag in (0, 1, 2, 3, 0x80, 0x81, 0xfe, 0xff):
        add(f'scene-flags-{scene_flag}', lambda c, value=scene_flag: [r.update(flags=value) for r in c['records']])
    matrices = {
        'identity': IDENTITY,
        'quarter-turn': [0., 0., -1., 8., 0., 1., 0., 9., 1., 0., 0., 10., 10., -20., 30., -11.],
        'nonorthogonal': [2., .25, -.5, -1., 1., 3., .75, -2., -.125, .5, .25, -3., -10., 20., -30., -4.],
        'zero-basis': [0.] * 12 + [10., 20., 30., 1.],
        'signed-zero': [-0., 0., -0., 0., 0., 1., -0., 0., -0., 0., 1., 0., -0., 0., -0., 1.],
    }
    locals_ = {
        'repeat': [[2., -3., 4., -12.]] * 4,
        'origin': [[0., 0., 0., 0.]] * 4,
        'signed-zero': [[-0., 0., -0., -0.], [0., -0., 0., 0.]] * 2,
        'tiny': [[2**-30, -2**-28, 2**-26, -19.], [-2**-30, 2**-28, -2**-26, 19.]] * 2,
        'varied': [[-3., 2., 7., -9.], [5., -6., 1., 12.], [1., 8., -4., .25], [7., 3., 2., 99.]],
    }
    for matrix_name, matrix in matrices.items():
        for local_name, local in locals_.items():
            for dirty in (0, 1):
                def change(c, matrix=matrix, local=local, dirty=dirty):
                    c['state'][7] = 4
                    c['controls'][9] = 4
                    for r, point in zip(c['records'], local):
                        r.update(local=floats(point), flags=0 if dirty else 1)
                        r['scene_local' if dirty else 'world'] = floats(matrix)
                add(f'{matrix_name}-{local_name}-dirty{dirty}', change)
    for word in (0x3f7fffff, 0x3f800000, 0x3f800001):
        def change(c, word=word):
            c['records'][0]['local'][0] = word
        add(f'adjacent-local-{word:08X}', change)
    for first_negative, second_negative in ((1, 0), (0, 1), (1, 1), (0, 0)):
        for enabled in (0, 1):
            def change(c, first_negative=first_negative, second_negative=second_negative, enabled=enabled):
                c['state'][7] = 2
                c['controls'][5] = enabled
                c['controls'][9] = 2
                for record, negative in zip(c['records'], (first_negative, second_negative, 0, 0)):
                    record.update(local=floats([1., 1., 1., 123.]), flags=1,
                                  world=[0x80000000 if negative else 0] * 16)
            add(f'mixed-zero-first{first_negative}-second{second_negative}-enabled{enabled}', change)
    rng = random.Random(0x48a295630)
    for index in range(128):
        def change(c):
            c['state'][7] = rng.randrange(1, 5)
            c['controls'][9] = rng.randrange(5)
            c['controls'][0] = int(rng.randrange(16) != 0)
            c['controls'][1] = int(rng.randrange(8) != 0)
            if not c['controls'][1]:
                c['controls'][10] = 3
            c['controls'][4:9] = [int(rng.randrange(8) != 0), int(rng.randrange(8) != 0),
                                  rng.randrange(2), rng.randrange(2), int(rng.randrange(8) != 0)]
            for j in (0, 1, 2, 3, 4, 5, 6, 8, 9, 10):
                c['state'][j] = rng.getrandbits(32)
            for r in c['records']:
                r['local'] = floats([rng.uniform(-100., 100.) for _ in range(4)])
                r['flags'] = rng.randrange(256)
                for field in ('world', 'scene_local'):
                    r[field] = floats([rng.uniform(-2., 2.) for _ in range(12)] +
                                      [rng.uniform(-15000., 15000.) for _ in range(3)] + [rng.uniform(-2., 2.)])
        add(f'deterministic-{index}', change)
    return result


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    words, original = freeze()
    fixture_cases = cases()
    reports, coverage, branches, instructions = [], set(), {}, 0
    model_deltas = []
    for case in fixture_cases:
        report, m = execute_case(words, case)
        assert report['status'] < 5
        assert all(math.isfinite(fp(w)) for w in report['output_words'][:8])
        reports.append(report)
        coverage.update(m.coverage)
        instructions += m.steps
        for pc, values in m.branches.items():
            branches.setdefault(pc, set()).update(values)
        if m.minmax_deltas:
            old, old_machine = execute_case(words, case, word_minmax=False)
            assert old['status'] == report['status'] and old_machine.coverage == m.coverage
            differences = [dict(index=i, word_order=a, legacy_float_comparison=b)
                           for i, (a, b) in enumerate(zip(report['output_words'], old['output_words'])) if a != b]
            model_deltas.append(dict(name=case['name'], instruction_deltas=m.minmax_deltas,
                                     output_deltas=differences, legacy_source_byte_reads=0))

    probes = []
    for count in (0, 0xffffffff, 0x80000000):
        case = make_case(f'nonpositive-active-count-{count:08X}')
        case['state'][7] = count
        case['controls'][9] = 0
        report, _ = execute_case(words, case)
        assert report['output_words'][:8] == [0, 0, 0, bits(1.), 0xff800000, 0xff800000, 0xff800000, bits(1.)]
        report['scope'] = 'Original raw-byte outcome under existing IEEE binary32 model; excluded from ordinary-finite native seam.'
        probes.append(report)
    for name, change in [
            ('provider-boundary', lambda c: c['controls'].__setitem__(slice(2, 4), [1, 1])),
            ('no-pairs-provider-boundary', lambda c: c['controls'].__setitem__(slice(1, 4), [0, 1, 1])),
            ('parent-scene-refresh-boundary', lambda c: c['records'][0].update(flags=0, parent=0x28000)),
            ('prepared-scene-opaque-parent', lambda c: c['records'][0].update(flags=1, parent=0x28000)),
            ('natural-static-initializers', lambda c: c.update(static_flags=[0, 0]))]:
        case = make_case(name)
        change(case)
        report, _ = execute_case(words, case)
        probes.append(report)

    for pc in (0x295648, 0x295674, 0x295684, 0x295710, 0x295968, 0x29597c,
               0x295988, 0x295994, 0x2959f8, 0x28bae0, 0x11ca90, 0x11cb10):
        assert branches[pc] == {False, True}, f'missing branch outcome at {pc:08X}'

    packed = [0x4f383452, 1, len(reports), INPUT_WORDS, OUTPUT_WORDS]
    for report in reports:
        packed += report['input_words'] + [report['status']] + report['output_words']
    binary = struct.pack('<' + 'I' * len(packed), *packed)
    (OUT / 'fixtures.bin').write_bytes(binary)
    header = ['// Original-instruction numeric detector fixtures; test-only.', '#pragma once', '#include <cstdint>',
              'static const std::uint32_t springDetectorFixtures48O[]={']
    header += [','.join(f'0x{v:08x}u' for v in packed[i:i+12]) + ',' for i in range(0, len(packed), 12)]
    header += ['};']
    (OUT / 'fixtures.h').write_text('\n'.join(header) + '\n')
    layout = dict(input_words=INPUT_WORDS, output_words=OUTPUT_WORDS,
                  controls=CONTROL_NAMES,
                  input='controls11, detectorCenterHalf8, springState11, 4*(local4,sceneWorld16,sceneLocal16,sceneFlags), pairs32, staged32, auxiliary8',
                  output='detectorCenterHalf8, springState11, pairs32, queryType, staged32, auxiliary8, 4*(sceneWorld16,sceneFlags)',
                  spring_state_offsets=[f'{off:X}' for off in STATE_OFFSETS],
                  status={'0':'absent spring, natural return1', '1':'query absent or disabled, natural return1',
                          '2':'before11FF74 registration call', '3':'before295A08 explicit-list query call',
                          '4':'before295A1C query dispatcher call'},
                  scene_scope='Per-record independent scene: world prepared or dirty parentless local-to-world copy; original world-point transform returns naturally.')
    layout['registration_scope'] = 'No-buffer active queries have initial type3. Authentic388A2C=11FF4C skips type-specific scene/sphere update; type2 needs the separate queryPrepare48O schema.'
    (OUT / 'fixtures.json').write_text(json.dumps(dict(layout=layout, cases=reports), indent=2) + '\n')
    (OUT / 'boundary-probes.json').write_text(json.dumps(dict(cases=probes), indent=2) + '\n')
    delta_proof = dict(scope='Corrected scalar MIN/MAX signed-word ordering versus the explicitly pinned historical float comparator. Historical source archived in evidence/minmaxAudit48O; historical fixture bytes unchanged.',
                       reference='https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp',
                       reference_checked='2026-10-06', reference_lines='83-90 and265-276',
                       local_reference='tools/gameplay/contact_oracle48g.h',
                       affected_cases=len(model_deltas),
                       cases_with_final_output_change=sum(bool(r['output_deltas']) for r in model_deltas),
                       cases=model_deltas)
    assert delta_proof['affected_cases'] >= 4 and delta_proof['cases_with_final_output_change'] >= 2
    (OUT / 'minmax-model-delta.json').write_text(json.dumps(delta_proof, indent=2) + '\n')
    proof = dict(scope='Original295630 numeric detector through reset, pair common-endpoint and11CA80 staging, stopping before actual query or spatial registration dispatch.',
                 oracle='Original/SpringOriginal with corrected scalar MIN/MAX signed-word ordering and an explicit legacy-comparison mode; empty data backing; every data byte explicitly initialized or produced by original instructions; historical fixture bytes unchanged.',
                 exclusions='No provider execution, parented dirty scene refresh, registration, query, RAM reconstruction, live tick, or native expected-value generation.',
                 numeric_model='Individually rounded host IEEE binary32 finite-domain behavior; not a claim about all physical EE/VU exceptional modes.',
                 elf_sha256=ELF_SHA, cases=len(reports), boundary_probes=len(probes), instructions=instructions,
                 source_byte_reads=0, unique_words=len(coverage), no_instruction_patches=True,
                 minmax_delta_cases=delta_proof['affected_cases'], minmax_final_delta_cases=delta_proof['cases_with_final_output_change'],
                 status_counts={str(k):sum(r['status'] == k for r in reports) for k in range(5)},
                 boundaries=layout['status'],
                 branches={f'{pc:08X}':sorted(values) for pc, values in sorted(branches.items())},
                 executed_pcs=[f'{pc:08X}' for pc in sorted(coverage)],
                 original_ranges=[{k:b[k] for k in ('name', 'start', 'end', 'sha256')} for b in original['blocks']],
                 source_hashes={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in
                                [Path(__file__), ROOT/'tools/gameplay/query_oracle48h.py', ROOT/'tools/gameplay/spring_fixture48j.py', ROOT/'tools/gameplay/contact_oracle48g.h']},
                 fixture_hashes={name:sha((OUT/name).read_bytes()) for name in
                                 ['fixtures.bin','fixtures.h','fixtures.json','original-words.json','boundary-probes.json','minmax-model-delta.json']})
    (OUT / 'proof.json').write_text(json.dumps(proof, indent=2) + '\n')
    print(json.dumps({k:proof[k] for k in ('cases','boundary_probes','instructions','unique_words','source_byte_reads','status_counts')}))


if __name__ == '__main__':
    main()
