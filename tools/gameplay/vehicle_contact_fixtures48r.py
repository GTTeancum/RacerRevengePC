"""Natural original219420 vehicle-contact proof with explicit tiny byte objects.

No guest RAM, native expected values, patched words, intercepted helper returns,
or skipped effects. Unsupported paths stop at the actual reached instruction.
The normal dispatch binds only the independently frozen165430 body callback.
"""
import copy
import hashlib
import json
import math
import random
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits, fp, signed, f32

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/vehicleContact48R'
MAP = OUT / 'map'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
MAP_SHA = '717452d4d1f70808eda3cf6cba3391da96005c1ef8afe3c1b5d2151e6282d99c'
P, SPRING, RECORDS, RADIUS = 0x10000, 0x12000, 0x13000, 0x14000
SCENES = [0x15000 + i * 0x1000 for i in range(3)]
BODIES = {1: 0x18000, 2: 0x19000, 3: 0x1a000}
CANDIDATES = [0x20000 + i * 0x1000 for i in range(4)]
SHAPES = [0x30000 + i * 0x1000 for i in range(4)]
GP, STACK, RETURN = 0x3dd970, 0x1ff0000, 0xfffffff0
FRAME = STACK - 0x190
CALLBACK = 0x165430
STATE_OFFSETS = (0x38, 0x3c, 0x40, 0x44, 0x74, 0x78, 0x130, 0x7c, 0x84, 0x88, 0xd4)
IDENTITY = [1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.]
BODY_IDENTITIES = {a: i for i, a in BODIES.items()}
BOUNDARY_PCS = {0x28b1fc: 'position-dirty-parent',
                0x28a77c: 'axes-dirty-parent',
                0x28bd94: 'inverse-point-dirty-parent'}
UNSUPPORTED_BOUNDS_TARGETS = {0x120378, 0x12044c, 0x1205ec, 0x1209f8, 0x1208e4}


def sha(raw): return hashlib.sha256(raw).hexdigest()
def floats(values): return [bits(v) for v in values]
def put(m, address, values):
    for i, value in enumerate(values): m.write(address + 4 * i, value)
def get(m, address, count): return [m.read(address + 4 * i) for i in range(count)]
def original_identity(value): return BODIES.get(value, value)
def identity(value): return BODY_IDENTITIES.get(value, value)


def load_words():
    raw = (MAP / 'original-words.json').read_bytes()
    assert sha(raw) == MAP_SHA, 'Frozen original map changed'
    proof = json.loads(raw)
    assert proof['elf']['sha256'] == ELF_SHA
    words = {}
    for block in proof['blocks']:
        rows = block['instructions']
        assert [r['pc'] for r in rows] == list(range(block['start'], block['end_exclusive'], 4))
        raw = b''.join(struct.pack('<I', r['word']) for r in rows)
        assert sha(raw) == block['sha256']
        assert raw == (MAP / (block['name'] + '.bin')).read_bytes()
        for r in rows:
            assert r['pc'] not in words or words[r['pc']] == r['word']
            words[r['pc']] = r['word']
    data = json.loads((MAP / 'dispatch-data.json').read_text())
    assert data['elf_sha256'] == ELF_SHA
    dispatch = {}
    for a in data['arrays']:
        raw = struct.pack('<' + str(len(a['words'])) + 'I', *a['words'])
        assert sha(raw) == a['sha256'] and raw == (MAP / (a['name'] + '.bin')).read_bytes()
        if a['name'] in ('bounds_switch', 'body_vtable_slot98'):
            dispatch.update((a['va'] + 4 * i, w) for i, w in enumerate(a['words']))
    assert dispatch[0x3ce098] == CALLBACK
    if '--frozen' not in sys.argv:
        elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
        assert sha(elf) == ELF_SHA
        for pc, word in list(words.items()) + list(dispatch.items()):
            assert struct.unpack_from('<I', elf, pc - 0x100000 + 0x80)[0] == word, hex(pc)
    return words, dispatch


class ContactOriginal(SpringOriginal):
    """Passive trace only; instruction semantics are inherited unchanged."""
    def __init__(self, words):
        super().__init__(data=b'', words=words)
        self.allowed_source_addresses = set()
        self.executing = False
        self.read_bytes, self.written_bytes = set(), set()
        self.events, self.calls, self.callback_calls, self.append_calls = [], [], [], []
        self.last_v0_writer = None
        self.active_candidate = None

    def read(self, address, size=4):
        if self.executing: self.read_bytes.update(range(address, address + size))
        return super().read(address, size)

    def write(self, address, value, size=4):
        if self.executing: self.written_bytes.update(range(address, address + size))
        return super().write(address, value, size)

    def observe(self, pc):
        if pc == 0x2194f0:
            self.events.append(dict(event='scene-prologue', position=get(self, FRAME + 0xf0, 4),
                                    axis_z=get(self, FRAME + 0x80, 4), axis_y=get(self, FRAME + 0x90, 4)))
        elif pc == 0x21951c:
            self.active_candidate = CANDIDATES.index(self.g[17][0])
        elif pc == 0x21952c:
            self.events.append(dict(event='bounds', candidate=self.active_candidate,
                                    center=get(self, FRAME + 0x60, 4), half=get(self, FRAME + 0x70, 4)))
        elif pc == 0x2195bc and self.position_axis_probe:
            self.events.append(dict(event='projection-dot', candidate=self.active_candidate,
                                    delta=get(self, FRAME + 0xa0, 4), up=get(self, FRAME + 0x90, 4),
                                    projection=bits(self.f[0])))
        elif pc == 0x219650:
            self.events.append(dict(event='threshold', candidate=self.active_candidate,
                                    lateral=get(self, FRAME + 0xa0, 4), distance=bits(self.f[1]),
                                    other_radius=bits(self.f[20]), self_radius=bits(self.f[21]),
                                    threshold=bits(self.f[0]), accepted=bool(self.condition)))
        elif pc == 0x219778:
            self.events.append(dict(event='contact-vectors', candidate=self.active_candidate,
                                    local_center=get(self, FRAME + 0xb0, 4),
                                    local_surface=get(self, FRAME + 0xc0, 4),
                                    normal=get(self, FRAME + 0xd0, 4),
                                    other_surface=get(self, FRAME + 0xe0, 4)))
        elif pc == CALLBACK:
            self.callback_calls.append(dict(candidate=self.active_candidate,
                body=identity(self.g[4][0]), other=identity(self.g[5][0]),
                count=self.read(self.g[4][0] + 0x20c),
                contacts=[identity(x) for x in get(self, self.g[4][0] + 0x210, 10)]))
        elif pc == 0x2963e0:
            self.append_calls.append(dict(candidate=self.active_candidate,
                count=self.read(SPRING + 0x84), capacity=self.read(SPRING + 0x88),
                other=identity(self.g[9][0]), scene=self.g[10][0], material=self.g[11][0]))

    def execute(self, pc):
        self.executing = False
        self.observe(pc)
        w = self.words[pc]
        if w >> 26 == 3:
            self.calls.append(dict(pc=pc, target=(w & 0x3ffffff) * 4))
        elif w >> 26 == 0 and w & 63 == 9:
            self.calls.append(dict(pc=pc, target=self.g[(w >> 21) & 31][0]))
        before = self.g[2].copy()
        self.executing = True
        target = super().execute(pc)
        self.executing = False
        if self.g[2] != before: self.last_v0_writer = pc
        return target


def initialize(words, dispatch, c):
    m = ContactOriginal(words)
    m.position_axis_probe = c.get('position_axis_probe', False)
    m.g[28][0] = GP
    m.g[2][0] = c.get('incoming_v0', 0xdeadbeef)
    m.f[20], m.f[21] = fp(0x41234567), fp(0xc2345678)
    for address, word in dispatch.items(): m.write(address, word)
    for address, value in zip((GP - 0x7894, GP - 0x7890), c['static_flags']): m.write(address, value, 1)
    for address, value in zip((0x434718, 0x434720), c['static_words']): m.write(address, value)
    m.write(0x3b14b0, c['category_mask'])
    for off, value in ((0x1b4, SCENES[c['position_scene']]), (0x190, SCENES[c['local_scene']]),
                       (0x340, BODIES[1]), (0x354, SPRING), (0x364, RADIUS), (0x58c, c['count'])):
        m.write(P + off, value)
    put(m, P + 0x590, [CANDIDATES[i] for i in c['order']])
    m.write(RADIUS + 0x80, c['radius'])
    for address, scene in zip(SCENES, c['scenes']):
        put(m, address + 0xc0, scene['local']); put(m, address + 0x100, scene['world'])
        m.write(address + 0x190, scene['flags'], 1); m.write(address + 0x180, scene['parent'])
    for b in c['bodies']:
        address = BODIES[b['id']]
        vtable = 0x3ce000 if b['callback'] == CALLBACK else 0x40000 + 0x100 * b['id']
        m.write(address + 8, vtable)
        if b['callback'] != CALLBACK: m.write(vtable + 0x98, b['callback'])
        m.write(address + 0x20c, b['count'])
        put(m, address + 0x210, [original_identity(v) for v in b['contacts']])
        # These bytes are deliberately distinct and must never be used instead
        # of the player's explicit scene bindings by this handler.
        m.write(address + 0x288, SCENES[b['scene']])
    for i, candidate in enumerate(c['candidates']):
        address, shape = CANDIDATES[i], SHAPES[i]
        for off, value in ((0x88, candidate['shape_tag']), (0x8c, shape),
                           (0xa4, candidate['category']), (0xa0, original_identity(candidate['body'])),
                           (0x80, candidate['cached_radius'])): m.write(address + off, value)
        put(m, address + 0x60, candidate['cached_center'])
        if candidate['shape_tag'] == 1:
            put(m, shape, candidate['local_center']); put(m, shape + 0x10, candidate['local_half'])
            put(m, shape + 0x80, candidate['shape_world'])
        elif candidate['shape_tag'] == 6:
            put(m, shape, candidate['shape_world']); put(m, shape + 0x40, candidate['local_center'])
            put(m, shape + 0x50, candidate['local_half'])
        # Unsupported shape contents remain uninitialized. Their dispatch must
        # stop before any attempted field load, even with category/count gates.
    for off, value in zip(STATE_OFFSETS, c['spring_state']): m.write(SPRING + off, value)
    m.write(SPRING + 0x8c, RECORDS)
    for i, row in enumerate(c['initial_records']):
        raw = row.copy(); raw[17] = original_identity(raw[17]); put(m, RECORDS + i * 96, raw)
    return m


def state(m, c):
    records = [get(m, RECORDS + i * 96, 24) for i in range(len(c['initial_records']))]
    for row in records: row[17] = identity(row[17])
    return dict(static_flags=[m.read(a, 1) for a in (GP - 0x7894, GP - 0x7890)],
                static_words=[m.read(a) for a in (0x434718, 0x434720)],
                scenes=[dict(world=get(m, a + 0x100, 16), local=get(m, a + 0xc0, 16),
                             flags=m.read(a + 0x190, 1), parent=m.read(a + 0x180)) for a in SCENES],
                bodies=[dict(id=b['id'], count=m.read(BODIES[b['id']] + 0x20c),
                             contacts=[identity(v) for v in get(m, BODIES[b['id']] + 0x210, 10)]) for b in c['bodies']],
                spring_state=[m.read(SPRING + off) for off in STATE_OFFSETS], records=records,
                candidates=[dict(cached_center=get(m, a + 0x60, 4), cached_radius=m.read(a + 0x80))
                            for a in CANDIDATES[:len(c['candidates'])]])


def execute_case(words, dispatch, c):
    m = initialize(words, dispatch, c)
    initialized = set(m.overlay)
    initial = state(m, c)
    m.g[4][0], m.g[29][0], m.g[31][0] = P, STACK, RETURN
    pc, boundary = 0x219420, None
    while pc != RETURN:
        if pc in BOUNDARY_PCS:
            boundary = BOUNDARY_PCS[pc]; break
        # Execute the authentic range gate and immutable jump table before
        # stopping at the selected unsupported branch's first instruction.
        if pc in UNSUPPORTED_BOUNDS_TARGETS or (pc == 0x120c4c and m.read(m.g[19][0] + 0x88) not in (1, 6)):
            boundary = 'unsupported-bounds-tag'; break
        if pc in (0x219788, 0x21979c) and m.g[25][0] != CALLBACK:
            boundary = 'primary-virtual' if pc == 0x219788 else 'other-virtual'; break
        if pc == CALLBACK and signed(m.read(m.g[4][0] + 0x20c)) < 0:
            boundary = 'negative-body-contact-count'; break
        if pc == 0x2963e0 and signed(m.read(SPRING + 0x84)) < 0:
            boundary = 'negative-record-count'; break
        target = m.execute(pc)
        if target is not None:
            if not m.annul: assert m.execute(pc + 4) is None
            pc = target
        else: pc += 4
    assert m.data == b'' and m.allowed_source_addresses == set() and not m.source_reads
    assert not ({0x2897b0, 0x289940} & m.coverage)
    assert not any(BODIES[b['id']] + 0x288 + i in m.read_bytes for b in c['bodies'] for i in range(4))
    assert not any(a + 0x80 + i in m.read_bytes for a in CANDIDATES for i in range(4))
    result = state(m, c)
    result.update(boundary=boundary, stop_pc=pc,
                  residual_v0=m.g[2][0] if pc == RETURN else None,
                  last_v0_change_pc=m.last_v0_writer,
                  events=m.events, calls=m.calls, callback_calls=m.callback_calls, append_calls=m.append_calls,
                  byte_audit=dict(initialized_bytes=len(initialized), execution_read_bytes=len(m.read_bytes),
                                  execution_written_bytes=len(m.written_bytes), source_reads=0),
                  steps=m.steps)
    # No hidden reset; skipped/capacity paths preserve all stale spring state.
    if not m.append_calls:
        assert result['spring_state'] == initial['spring_state']
        assert result['records'] == initial['records']
    assert [v for i, v in enumerate(result['spring_state']) if i not in (8, 10)] == [
        v for i, v in enumerate(initial['spring_state']) if i not in (8, 10)]
    for before, after in zip(initial['records'], result['records']):
        assert [before[i] for i in (18, 20, 21, 22, 23)] == [after[i] for i in (18, 20, 21, 22, 23)]
    assert result['candidates'] == initial['candidates']
    if pc == RETURN:
        assert m.g[29][0] == STACK
        assert bits(m.f[20]) == 0x41234567 and bits(m.f[21]) == 0xc2345678
    return result, m


def make_case(name):
    position = IDENTITY.copy(); position[12:15] = [.5, 0., 0.]
    scenes = [dict(world=floats(position), local=floats(position), flags=1, parent=0),
              dict(world=floats(IDENTITY), local=floats(IDENTITY), flags=1, parent=0),
              dict(world=floats(IDENTITY[:12] + [17., -23., 31., 1.]), local=floats(IDENTITY), flags=1, parent=0)]
    candidates = [dict(id=11+i, shape_tag=1 if i % 2 == 0 else 6, shape_world=floats(IDENTITY),
                       local_center=floats([13. + i, 17., 19., -7.]), local_half=floats([1., 9., 1., -8.]),
                       cached_center=floats([0., 0., 0., 9. + i]), cached_radius=bits(117. + i),
                       category=2, body=2 + i % 2) for i in range(4)]
    rows = [[(0x41000000 + i * 100 + j) for j in range(24)] for i in range(12)]
    for row in rows: row[17] = 3
    return dict(name=name, static_flags=[1, 1], static_words=[bits(.5), bits(1.)], category_mask=2,
                count=1, order=[0], radius=bits(1.), position_scene=0, local_scene=1,
                scenes=scenes, candidates=candidates,
                bodies=[dict(id=i, callback=CALLBACK, count=0,
                             contacts=[0xabc00000 + i * 100 + j for j in range(10)], scene=2) for i in (1, 2, 3)],
                spring_state=[0x12340000 + i for i in range(11)], initial_records=rows)


def make_cases():
    cases = []
    def add(name, edit=None):
        c = make_case(name); c['spring_state'][8:10] = [0, 12]
        if edit: edit(c)
        cases.append(c); return c
    add('basic-hit')
    add('shape6-hit', lambda c: c.update(order=[1]))
    for count in (0, 0xffffffff, 0x80000000):
        add('signed-count-' + str(count), lambda c, count=count: c.update(count=count, order=[]))
    for mask, category in ((0, 2), (1, 2), (2, 0), (0x80000000, 0x80000000), (0xffffffff, 0x80000000)):
        add(f'category-mask{mask}-category{category}', lambda c, m=mask, k=category:
            (c.update(category_mask=m), c['candidates'][0].update(category=k)))
    for flag in (0, 1, 2, 0x7f, 0x80, 0xff):
        for which in (0, 1, 2):
            add(f'static-flags-{flag}-{which}', lambda c, f=flag, w=which:
                c.update(static_flags=[f if w != 1 else 1, f if w != 0 else 1], static_words=floats([.25, -19.])))
    for order in ([0, 1], [1, 0], [0, 0, 0], [0, 1, 2, 3], [3, 2, 1, 0]):
        add('ordered-' + ''.join(map(str, order)), lambda c, o=order: c.update(order=o, count=len(o)))
    for n in (0, 1, 3, 10, 12):
        for capacity in (0, 1, 3, 12, 0xffffffff, 0x80000000):
            add(f'record-count{n}-capacity{capacity}', lambda c, n=n, k=capacity:
                (c.update(order=[0, 1, 0], count=3), c['spring_state'].__setitem__(slice(8, 10), [n, k])))
    for count in (0, 1, 3, 9, 10, 11, 0x7fffffff):
        for duplicate in (False, True):
            for side in (0, 1):
                def lists(c, n=count, dup=duplicate, side=side):
                    b = c['bodies'][side]; b['count'] = n
                    if dup and n: b['contacts'][min(n - 1, 9)] = 2 if side == 0 else 1
                add(f'body-count{count}-duplicate{int(duplicate)}-side{side}', lists)
    add('self-body-alias', lambda c: c['candidates'][0].update(body=1))
    add('same-body-repeated', lambda c: (c.update(order=[0, 2], count=2)))
    for flag in (0, 1, 2, 3, 0x80, 0x81, 0xfe, 0xff):
        for alias in (False, True):
            def scenes(c, flag=flag, alias=alias):
                c['local_scene'] = 0 if alias else 1
                for s in c['scenes'][:2]: s['flags'] = flag
                c['scenes'][0]['world'][12:15] = floats([20., 0., 0.])
            add(f'scene-flags{flag}-alias{int(alias)}', scenes)
    for which in ('position', 'local'):
        add('prepared-opaque-parent-' + which, lambda c, w=which:
            c['scenes'][c[w + '_scene']].update(parent=0x456700))
        add('boundary-dirty-parent-' + which, lambda c, w=which:
            c['scenes'][c[w + '_scene']].update(flags=0, parent=0x456700))
    # Contact radius=.5 * horizontal extent length. Set half=(2,9,0) so
    # threshold is exactly2, then exercise adjacent binary32 comparisons.
    for word in (bits(-2.)-1, bits(-2.), bits(-2.)+1, 0x80000000, 0, bits(2.)-1, bits(2.), bits(2.)+1):
        def threshold(c, word=word):
            c['scenes'][0]['world'][12] = word
            c['candidates'][0]['local_half'] = floats([2., 9., 0., -8.])
        add('strict-threshold-' + f'{word:08x}', threshold)
    for radius in (-3., -1., -0., 0., 1., 3.):
        add('signed-radius-' + str(radius), lambda c, v=radius: c.update(radius=bits(v)))
    for scale in (-1., -0., 0., .25, 1., 2.):
        add('runtime-scale-' + str(scale), lambda c, v=scale: c['static_words'].__setitem__(0, bits(v)))
    add('zero-lateral', lambda c: c['scenes'][0]['world'].__setitem__(12, 0))
    add('vertical-offset-projects-away', lambda c: c['scenes'][0]['world'].__setitem__(13, bits(100.)))
    add('cached-center-distinct-from-bounds', lambda c: c['candidates'][0].update(
        local_center=floats([-999., 400., 250., 41.]), cached_center=floats([.25, 80., .125, -99.])))
    for word in (0, 0x80000000, 0x7fc12345, 0x7f800000, 0xff800000, 0x7f812345):
        add('ignored-cached-radius-' + f'{word:08x}', lambda c, w=word:
            c['candidates'][0].update(cached_radius=w))
    add('ignored-static-second-word-nan', lambda c: c['static_words'].__setitem__(1, 0x7fc12345))
    add('initialized-statics-replace-poison', lambda c:
        c.update(static_flags=[0, 0], static_words=[0x7fc12345, 0xff800000]))
    add('opaque-stale-records', lambda c:
        c.update(initial_records=[[0x7f812345 if j == 18 else 0x7fc12345 if j >= 20 else x
                                  for j, x in enumerate(row)] for row in c['initial_records']] ))
    add('unread-body-scenes', lambda c: [b.update(scene=b['id'] - 1) for b in c['bodies']])
    for gate in ('empty', 'mask-skip', 'forward-axis', 'unused-inverse-scene'):
        def lazy_poison(c, gate=gate):
            c['copy_only_nonfinite'] = True
            if gate == 'forward-axis':
                c['scenes'][0]['world'][8:11] = [0x7fc12345, 0x7f800000, 0xff800000]
            elif gate == 'unused-inverse-scene':
                c['scenes'][1]['world'] = [0x7fc12345] * 16
                c['scenes'][1].update(flags=0, parent=0x456700)
                c['scenes'][0]['world'][12] = bits(99.)
            else:
                c['scenes'][0]['world'][4:7] = [0x7fc12345, 0x7f800000, 0xff800000]
                c['scenes'][0]['world'][12:15] = [0x7fc12345, 0x7f800000, 0xff800000]
                c['radius'] = 0x7fc12345; c['static_words'][0] = 0x7fc12345
                if gate == 'empty': c.update(count=0, order=[])
                else: c['candidates'][0]['category'] = 0
        add('lazy-nonfinite-' + gate, lazy_poison)
    matrices = {
        'rotate-z': [0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'rotate-y': [0.,0.,-1.,0.,0.,1.,0.,0.,1.,0.,0.,0.,3.,-2.,5.,1.],
        'rotate37': [.8,.6,0.,0.,-.6,.8,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'reflection': [-1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,3.,-2.,5.,1.],
        'scale': [2.,0.,0.,0.,0.,.5,0.,0.,0.,0.,1.5,0.,3.,-2.,5.,1.],
        'shear': [1.,0.,0.,0.,.5,1.,.25,0.,.125,0.,1.,0.,3.,-2.,5.,1.],
        'signed-zero': [-0.,0.,-0.,0.,-0.,1.,0.,0.,0.,-0.,1.,0.,-0.,0.,-0.,1.]}
    for family, matrix in matrices.items():
        for tag in (1, 6):
            for dirty in (False, True):
                def transform(c, matrix=matrix, tag=tag, dirty=dirty):
                    c['candidates'][0]['shape_tag'] = tag
                    c['candidates'][0]['shape_world'] = floats(matrix)
                    c['scenes'][1]['world' if not dirty else 'local'] = floats(matrix)
                    c['scenes'][1]['flags'] = 0 if dirty else 1
                    c['radius'] = bits(10.)
                add(f'transform-{family}-tag{tag}-dirty{int(dirty)}', transform)
    # Unknown callbacks are truly reached only after numeric acceptance and
    # transforms. The other-side boundary preserves the first callback effect.
    for side in (0, 1):
        add('boundary-virtual-side' + str(side), lambda c, side=side:
            c['bodies'][side].update(callback=0x234560))
        add('boundary-negative-body-count-side' + str(side), lambda c, side=side:
            c['bodies'][side].update(count=0xffffffff))
        add('lazy-unknown-callback-miss-side' + str(side), lambda c, side=side:
            (c['bodies'][side].update(callback=0x234560), c['scenes'][0]['world'].__setitem__(12, bits(99.))))
    add('boundary-negative-record-count', lambda c: c['spring_state'].__setitem__(8, 0xffffffff))
    for tag in (0, 2, 3, 4, 5, 7, 0xffffffff):
        add('boundary-shape-tag' + str(tag), lambda c, t=tag: c['candidates'][0].update(shape_tag=t))
        add('lazy-shape-tag' + str(tag), lambda c, t=tag: c['candidates'][0].update(shape_tag=t, category=0))
    for poison in (0, 0x12345678, 0xffffffff):
        add('incoming-return-register-' + str(poison), lambda c, v=poison:
            c.update(count=0, order=[], incoming_v0=v))
    # Reproducible ordinary finite variations; expected values still come only
    # from the same frozen instructions, never from host contact formulas.
    rng = random.Random(0x48a219420)
    for i in range(96):
        def random_case(c, i=i):
            c['order'] = [rng.randrange(4) for _ in range(1 + i % 4)]; c['count'] = len(c['order'])
            c['radius'] = bits(rng.uniform(.05, 5.))
            c['spring_state'][8:10] = [i % 4, 4 + i % 9]
            for s in c['scenes'][:2]:
                matrix = IDENTITY.copy(); matrix[12:15] = [rng.uniform(-3., 3.) for _ in range(3)]
                s['world'] = floats(matrix)
            for candidate in c['candidates']:
                candidate['cached_center'] = floats([rng.uniform(-3., 3.) for _ in range(3)] + [rng.uniform(-10., 10.)])
                candidate['local_half'] = floats([rng.uniform(-5., 5.) for _ in range(3)] + [rng.uniform(-10., 10.)])
                candidate['local_center'] = floats([rng.uniform(-20., 20.) for _ in range(4)])
            c['bodies'][0]['count'] = i % 11
        add('finite-random-' + str(i), random_case)
    # Append-only expansion: preserve the exact original298 case objects.
    # The first packet varied inverse scenes, but all consumed position-scene
    # up axes were(0,1,0). These genuine-entry additions exercise all XYZ dot
    # lanes, nonunit up vectors, both acceptance outcomes, cache refresh and
    # aliasing. In particular mixed-cancellation distinguishes(X+Y)+Z from
    # X+(Y+Z) without overflow, and nonunit-mixed's miss would become a hit if
    # the supplied up vector were incorrectly normalized.
    up_families = [
        ('tilted-unit', [.36, .48, .8], [1.125, -2.375, .625], [3., .125]),
        ('nonunit-mixed', [2., 1., -.5], [.3, -.6, .2], [1., .01]),
        ('nonunit-vertical', [0., 2., 0.], [.2, 1., .3], [5., 1.]),
        ('mixed-cancellation', [.5, .5, .5], [67108864., -67108864., 2.], [1e8, 1.]),
        ('negative-nonunit', [-.75, .125, 1.5], [1.05, -.6, 1.2], [3., .125]),
    ]
    for family, up, delta, radii in up_families:
        for dirty in (False, True):
            for alias in (False, True):
                for hit, radius in zip((True, False), radii):
                    def position_axis(c, up=up, delta=delta, dirty=dirty, alias=alias, radius=radius):
                        c['position_axis_probe'] = True
                        c['radius'] = bits(radius); c['local_scene'] = 0 if alias else 1
                        matrix = IDENTITY.copy(); matrix[4:7] = up; matrix[12:15] = delta
                        c['scenes'][0]['world' if not dirty else 'local'] = floats(matrix)
                        if dirty:
                            c['scenes'][0]['flags'] = 0x80
                            c['scenes'][0]['world'] = floats(IDENTITY[:12] + [99., -88., 77., 1.])
                    add(f'position-up-{family}-dirty{int(dirty)}-alias{int(alias)}-' + ('hit' if hit else 'miss'), position_axis)
    return cases


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    words, dispatch = load_words(); cases = make_cases()
    coverage, branches, steps = set(), {}, 0
    for c in cases:
        try: result, m = execute_case(words, dispatch, c)
        except Exception:
            print('FAILED CASE:', c['name'], file=sys.stderr); raise
        c['expected'] = result
        c['numeric_scope'] = 'nonfinite-copy-only' if c.pop('copy_only_nonfinite', False) else 'zero-normalization-finite-model' if any(
            e['event'] == 'threshold' and e['accepted'] and e['distance'] & 0x7fffffff == 0
            for e in result['events']) else 'ordinary-finite'
        coverage.update(m.coverage); steps += m.steps
        for pc, outcomes in m.branches.items(): branches.setdefault(pc, set()).update(outcomes)
    by = {c['name']: c for c in cases}
    assert by['basic-hit']['expected']['spring_state'][8] == 1
    assert len(by['basic-hit']['expected']['callback_calls']) == 2
    assert by['boundary-virtual-side1']['expected']['bodies'][0]['count'] == 1
    assert by['boundary-virtual-side0']['expected']['bodies'][0]['count'] == 0
    assert by['self-body-alias']['expected']['bodies'][0]['count'] == 1
    assert by['strict-threshold-40000000']['expected']['spring_state'][8] == 0
    assert by['strict-threshold-3fffffff']['expected']['spring_state'][8] == 1
    assert by['strict-threshold-40000001']['expected']['spring_state'][8] == 0
    assert all(by['incoming-return-register-' + str(v)]['expected']['residual_v0'] == FRAME + 0xe0
               for v in (0, 0x12345678, 0xffffffff))
    archived_raw = (OUT / 'archive298/native-fixtures.json').read_bytes()
    assert sha(archived_raw) == '685a4bd8ee8906ff65f1102bd290ccfea0bc1f0025c6cc59c5c73d431ae2ac87'
    archived = json.loads(archived_raw)
    assert len(archived['cases']) == 298 and cases[:298] == archived['cases'], 'Changed a preserved298-case object'
    position_axis_cases = [c for c in cases if c.get('position_axis_probe')]
    assert len(position_axis_cases) == 40
    axis_diagnostics = []
    for c in position_axis_cases:
        e = next(e for e in c['expected']['events'] if e['event'] == 'projection-dot')
        threshold = next(e for e in c['expected']['events'] if e['event'] == 'threshold')
        assert threshold['accepted'] == c['name'].endswith('-hit')
        delta, up = [fp(v) for v in e['delta'][:3]], [fp(v) for v in e['up'][:3]]
        products = [f32(a * b) for a, b in zip(delta, up)]
        reassociated = bits(f32(products[0] + f32(products[1] + products[2])))
        norm = f32((f32(up[0] * up[0]) + f32(up[1] * up[1])))
        norm = f32(norm + f32(up[2] * up[2]))
        # Diagnostics never feed the genuine execution or expected fixture.
        inverse_length = f32(1. / f32(math.sqrt(norm)))
        normalized_up = [f32(v * inverse_length) for v in up]
        normalized_products = [f32(a * b) for a, b in zip(delta, normalized_up)]
        normalized_projection = f32(f32(normalized_products[0] + normalized_products[1]) + normalized_products[2])
        normalized_lateral = [bits(f32(a - f32(normalized_projection * b))) for a, b in zip(delta, normalized_up)]
        normalized_squares = [f32(fp(v) * fp(v)) for v in normalized_lateral]
        normalized_distance = f32(math.sqrt(f32(f32(normalized_squares[0] + normalized_squares[1]) + normalized_squares[2])))
        y_only_lateral = [bits(delta[0]), bits(f32(delta[1] - f32(f32(delta[1] * up[1]) * up[1]))), bits(delta[2])]
        axis_diagnostics.append(dict(case=c['name'], actual_up=e['up'][:3], actual_delta=e['delta'][:3],
            original_projection=e['projection'], reassociated_projection=reassociated,
            original_lateral=threshold['lateral'][:3], normalized_up_lateral=normalized_lateral,
            normalized_up=floats(normalized_up), normalized_up_distance=bits(normalized_distance),
            normalized_up_accepted=normalized_distance < fp(threshold['threshold']),
            y_only_projection_lateral=y_only_lateral, accepted=threshold['accepted']))
    assert any(r['original_projection'] != r['reassociated_projection'] for r in axis_diagnostics)
    assert any(r['original_lateral'] != r['normalized_up_lateral'] for r in axis_diagnostics)
    assert any(r['accepted'] != r['normalized_up_accepted'] for r in axis_diagnostics)
    assert any(r['original_lateral'] != r['y_only_projection_lateral'] for r in axis_diagnostics)
    (OUT / 'position-axis-diagnostics.json').write_text(json.dumps(dict(
        purpose='Append-only genuine-entry up-axis coverage and alternate-route diagnostics; alternatives never supply expected values',
        preserved_cases=298, new_cases=len(position_axis_cases), all_original_case_objects_preserved=True,
        no_original_words_patched=True, no_native_values=True, cases=axis_diagnostics), indent=2) + '\n')
    normalization_differences = []
    for c in cases:
        threshold = None
        for e in c['expected']['events']:
            if e['event'] == 'threshold': threshold = e
            elif e['event'] == 'contact-vectors' and threshold and threshold['distance'] & 0x7fffffff:
                alternative = [bits(f32(fp(v) / fp(threshold['distance']))) for v in threshold['lateral'][:3]]
                if alternative != e['normal'][:3]:
                    normalization_differences.append(dict(case=c['name'], candidate=e['candidate'],
                        original_normal=e['normal'][:3], alternate_per_lane_division=alternative,
                        original_lateral=threshold['lateral'][:3], original_scalar_distance=threshold['distance']))
    assert normalization_differences, 'Corpus must distinguish reciprocal multiplication from lane division'
    (OUT / 'normalization-route-diagnostics.json').write_text(json.dumps(dict(
        purpose='Sensitivity diagnostic only; these alternatives never supply an expected result',
        no_original_words_patched=True, no_native_values=True,
        original_route='VU recomputes norm squared, rounded reciprocal of rounded square root, then lane multiplication',
        differences=normalization_differences), indent=2) + '\n')
    for pc in (0x219444, 0x219464, 0x219514, 0x219650, 0x2197d8, 0x120354, 0x28b1e8, 0x28b1f4,
               0x28bd80, 0x28bd8c, 0x165438, 0x165454, 0x165474, 0x16547c, 0x165488, 0x296420):
        assert branches[pc] == {False, True}, f'missing branch outcome {pc:08X}'
    fixture = dict(schema=1, state_offsets=list(STATE_OFFSETS), records=12, body_slots=10,
                   supported_shape_tags=[1, 6], supported_callback=CALLBACK, cases=cases)
    raw = (json.dumps(fixture, separators=(',', ':')) + '\n').encode()
    (OUT / 'native-fixtures.json').write_bytes(raw)
    (OUT / 'executed-original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,
        words=[dict(pc=pc, word=words[pc]) for pc in sorted(coverage)]), indent=2) + '\n')
    source_paths = ['tools/gameplay/vehicle_contact_fixtures48r.py', 'tools/gameplay/spring_fixture48j.py',
                    'tools/gameplay/query_oracle48h.py', 'evidence/vehicleContact48R/map/original-words.json',
                    'evidence/vehicleContact48R/map/dispatch-data.json',
                    'evidence/vehicleContact48R/archive298/native-fixtures.json']
    report = dict(cases=len(cases), natural_returns=sum(not c['expected']['boundary'] for c in cases),
                  accepted_contacts=sum(len(c['expected']['append_calls']) for c in cases),
                  callback_calls=sum(len(c['expected']['callback_calls']) for c in cases),
                  position_axis_probe_cases=len(position_axis_cases), preserved_previous_case_objects=298,
                  zero_normalization_model_cases=sum(c['numeric_scope'] == 'zero-normalization-finite-model' for c in cases),
                  nonfinite_copy_only_cases=sum(c['numeric_scope'] == 'nonfinite-copy-only' for c in cases),
                  negative_capacity_native_guard_cases=[c['name'] for c in cases if signed(c['spring_state'][9]) < 0 and c['expected']['append_calls']],
                  reciprocal_vs_lane_division_distinguishing_contacts=len(normalization_differences),
                  boundary_cases={c['name']: c['expected']['boundary'] for c in cases if c['expected']['boundary']},
                  source_reads=0, steps=steps, unique_words=len(coverage), entry=0x219420,
                  no_RAM_reads_or_reconstruction=True, no_native_expected_values=True,
                  no_original_words_patched=True, no_calls_stubbed=True, no_uninitialized_read_fallback=True,
                  elf_sha256=ELF_SHA, fixture_sha256=sha(raw), fixture_bytes=len(raw),
                  numeric_domain='Individually rounded binary32 finite arithmetic; zero VU normalization uses inherited finite maximum reciprocal model, explicitly labeled; no physical EE/VU exceptional-mode equivalence',
                  original_return='No explicit result assignment; residual v0 is observed only and is not a boolean/native completion contract',
                  body_scene_contract='Vehicle+1B4 and vehicle+190 bindings used directly; body+288 stays unread',
                  sources=[dict(path=p, sha256=sha((ROOT / p).read_bytes())) for p in source_paths],
                  coverage=[f'{pc:08X}' for pc in sorted(coverage)],
                  branches={f'{pc:08X}': sorted(v) for pc, v in sorted(branches.items())})
    (OUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    (OUT / 'return-semantics.json').write_text(json.dumps(dict(
        contract=report['original_return'],
        handler_epilogue='2197E0..219800 restores callee-saved registers and stack but never writes v0',
        cases=[dict(name=c['name'], residual_v0=c['expected']['residual_v0'],
                    last_v0_change_pc=c['expected']['last_v0_change_pc'], boundary=c['expected']['boundary'])
               for c in cases if c['name'].startswith(('incoming-return-register', 'basic-hit', 'strict-threshold', 'shape6-hit'))]), indent=2) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k not in ('sources', 'coverage', 'branches')}, indent=2))


if __name__ == '__main__': main()
