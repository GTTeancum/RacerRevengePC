#!/usr/bin/env python3
"""49W type0 late consumer: exact authenticated source words, native differential.

No PC/RAM/game execution, collision traversal, registration, heap/capacity
diagnostic or allocation-failure experiment. Default products are temporary;
--output-dir explicitly retains reports. All inherited evidence stays read-only.
"""
import argparse
import copy
import hashlib
import json
import math
import os
from pathlib import Path
import random
import struct
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
EVIDENCE = ROOT / 'evidence/frameBridge49W/shape'
sys.path.insert(0, str(ROOT / 'tools/assets'))
from decode_collision48g import decode
from group_shape_fixtures48t import GroupOriginal, helper_case, put, get, audit, MEMBER, SHAPE, GEOMETRY, MATRIX
from candidate_collection_fixtures48s import RETURN, STACK
from query_oracle48h import bits, fp

ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
ASSET_SHA = '579f7dfc733a0faabd04c1b9be2dc37d6ceec8638a8f5b0103c4b32d7af9b4e7'
SQRT_REFERENCE = ROOT / 'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h'
SQRT_SHA = '1c479c20842c7334e1112adebfb1903d182b943f9c1bc3ff6aa9e3ed973c3071'
EXECUTED = ('late_consumer', 'type0_dispatch', 'type0_transform', 'transform_return', 'registration_guard')
FLAGS = ['-std=c++17', '-ffp-contract=off', '-fno-fast-math', '-Wall', '-Wextra', '-Werror', '-Wno-misleading-indentation']
PRODUCTION = ROOT / 'src/game/pod_shape49w.cpp'
TEST = ROOT / 'tests/native/pod_shape49w.cpp'

def sha(raw): return hashlib.sha256(raw).hexdigest()


def authenticate():
    manifest = json.loads((EVIDENCE / 'source-manifest.json').read_text())
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    assert manifest['elf_sha256'] == sha(elf) == ELF_SHA
    assert sha(SQRT_REFERENCE.read_bytes()) == SQRT_SHA
    off = struct.unpack_from('<I', elf, 28)[0]
    size, count = struct.unpack_from('<HH', elf, 42)
    segments = [struct.unpack_from('<8I', elf, off + i * size) for i in range(count)]
    words, dispatch = {}, {}
    for name, block in manifest['blocks'].items():
        start, end = block['start'], block['end_exclusive']
        for typ, offset, address, _, length, *_ in segments:
            if typ == 1 and address <= start < end <= address + length:
                raw = elf[offset + start - address:offset + end - address]
                break
        else: raise ValueError(name)
        assert sha(raw) == block['sha256']
        assert raw == (EVIDENCE / 'source' / (name + '.bin')).read_bytes()
        rows = (EVIDENCE / 'source' / (name + '.txt')).read_text().splitlines()
        pairs = [(start + i * 4, w) for i, (w,) in enumerate(struct.iter_unpack('<I', raw))]
        assert [(int(s.split()[0], 16), int(s.split()[1], 16)) for s in rows] == pairs
        if name in EXECUTED: words.update(pairs)
        elif name == 'shape_jump_table': dispatch.update(pairs)
    assert dispatch[0x388a00] == 0x11f6c8
    # This authenticates object+88 as the file's type, and explicitly separates
    # constructor argument5 ->11B570 flags from that type assignment.
    loader = {int(s.split()[0], 16): int(s.split()[1], 16) for s in
              (EVIDENCE / 'source/type_loader.txt').read_text().splitlines()}
    assert loader[0x124fac] == 0x26460088 and loader[0x124fb8] == 0x0c04becc
    assert loader[0x12500c] == 0x0c049624
    asset = (EVIDENCE / 'podboxf.col').read_bytes()
    assert sha(asset) == ASSET_SHA and len(asset) == 1760
    container = (ROOT / 'work/native/project/disc/PODS/POD01/POD01.RES').read_bytes()
    assert sha(container) == manifest['asset']['container_sha256'] and len(container) == 349110
    decoded = decode(asset)
    assert [len(decoded[k]) for k in ('vertices', 'nodes', 'faces')] == [16, 27, 28]
    return manifest, words, dispatch, decoded


class PodShapeOriginal(GroupOriginal):
    """Local SQRT.S semantic correction; inherited48T/query engines unchanged.

    The retained EE interface flushes exponent-zero operands, unlike the old
    generic query engine. Only the numeric opcode result is corrected, using
    its actual Ft input. Caller/callee words, flow and all tracing stay genuine.
    The C++ test independently checks this boundary against ee_sqrt14.h itself.
    """
    def execute(self, pc):
        word = self.words[pc]
        scalar_root = word >> 26 == 0x11 and (word >> 21) & 31 == 16 and word & 63 == 4
        if scalar_root: operand = self.f[(word >> 16) & 31]
        target = super().execute(pc)
        if scalar_root:
            assert math.isfinite(operand) and operand >= 0
            raw = bits(operand)
            result = fp(raw & 0x80000000) if raw & 0x7f800000 == 0 else fp(bits(math.sqrt(operand)))
            self.f[(word >> 6) & 31] = result
        return target


def execute(words, dispatch, case):
    """Original11FF90 returns through both genuine callees; no call substitution."""
    m = PodShapeOriginal(words)
    m.identities = {}
    for address, values in ((MEMBER, case['member_words']), (SHAPE, case['shape_words']),
                            (GEOMETRY, case['shape_geometry_words']), (MATRIX, case['input_matrix'])):
        put(m, address, values)
    for address, value in dispatch.items(): m.write(address, value)
    m.watch = [(MEMBER, MEMBER + 256), (SHAPE, SHAPE + 288), (GEOMETRY, GEOMETRY + 36)]
    # Finite nonzero incoming scratch does not become an invented source input.
    for reg in range(1, 32): m.v[reg] = [fp(bits((reg + lane + 1) * .125)) for lane in range(4)]
    m.g[4][0], m.g[5][0], m.g[29][0], m.g[31][0] = MEMBER, MATRIX, STACK, RETURN
    initial = set(m.overlay)
    pc = 0x11ff90
    while pc != RETURN:
        if pc not in words: raise RuntimeError(f'unmapped source dependency {pc:08x}')
        if m.steps > 2000: raise RuntimeError('source failed to return')
        target = m.execute(pc)
        if target is not None:
            if not m.annul: assert m.execute(pc + 4) is None
            pc = target
        else: pc += 4
    assert m.g[29][0] == STACK
    result = dict(member_words=get(m, MEMBER, 64), shape_words=get(m, SHAPE, 72),
                  shape_geometry_words=get(m, GEOMETRY, 9))
    assert result['shape_geometry_words'] == case['shape_geometry_words']
    result['byte_audit'] = audit(m, initial)
    result['calls'] = m.calls
    result['steps'] = m.steps
    return result, m


def fixtures(words, dispatch, decoded):
    cases, coverage, branches = [], {}, {}
    def base(name):
        c = helper_case(name, 'late_consumer')
        c['member_words'][56] = 0
        c['shape_words'][16:32] = [bits(x) for x in decoded['matrix']]
        c['shape_geometry_words'][3:9] = [bits(x) for x in decoded['nodes'][0]['center'] + decoded['nodes'][0]['half']]
        return c
    def add(c):
        c['expected'], machine = execute(words, dispatch, c)
        assert machine.calls == [{'pc': 0x11ff9c, 'target': 0x11f680}, {'pc': 0x11ffa4, 'target': 0x120230}]
        # Exact source write order: previous64 bytes precede current64 bytes.
        writes = machine.write_trace
        assert [w['address'] for w in writes[:32]] == list(range(SHAPE + 0xc0, SHAPE + 0x100, 4)) + list(range(SHAPE + 0x80, SHAPE + 0xc0, 4))
        for pc in machine.coverage: coverage[pc] = coverage.get(pc, 0) + 1
        for pc, values in machine.branches.items(): branches.setdefault(pc, set()).update(values)
        cases.append(c)
    add(base('actual-POD01-root'))
    for kind, matrix in [
        ('scaled', [2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 13, -17, 19, 1]),
        ('rotated', [0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 13, -17, 19, 1]),
        ('full-W', [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]),
        ('signed-zero', [-0., 0., -0., -0., 0., -0., 0., -0., -0., 0., -0., -0., -0., -0., -0., -0.])]:
        for identity in (False, True):
            c = base(kind + ('-authored-local' if identity else '-nonidentity-local'))
            c['input_matrix'] = [bits(x) for x in matrix]
            if not identity: c['shape_words'][16:32] = [bits(x) for x in [.5, -1, 2, -0., 0, .25, -.5, 3, 1, 2, -3, 4, -2, .125, 7, -0.]]
            add(c)
    for half in ([0., -0., 0.], [-1., -2., -3.], [.125, .25, .5], [1000., .001, 17.]):
        c = base('ordered-radius-' + str(half)); c['shape_geometry_words'][6:9] = [bits(x) for x in half]; add(c)
    for word in (0, 0x80000000, 0x1a000000, 0x1b000000, 0x1e3ce508, 0x1ffffffe, 0x1fffffff, 0x20000000, 0x20000001):
        for axis in range(3):
            c = base(f'scalar-root-boundary-{word:08x}-axis{axis}')
            c['shape_geometry_words'][6:9] = [0, 0, 0]
            c['shape_geometry_words'][6 + axis] = word
            add(c)
    for half in ([0x1fb504f3, 0x1fb504f3, 0], [0x1fb504f3, 0x1fb504f3, 0x1a000000], [0x1f000000] * 3):
        c = base('scalar-root-accumulation-' + '-'.join(f'{x:08x}' for x in half))
        c['shape_geometry_words'][6:9] = half; add(c)
    c = base('raw-history-NaNs-not-arithmetic')
    c['shape_words'][0:16] = [0x7fa12345] * 16
    c['shape_words'][32:64] = [0xffa43210 + i for i in range(32)]
    add(c)
    rng = random.Random(0x49f11ff90)
    for i in range(192):
        c = base('finite-random-' + str(i))
        for sl in (slice(16, 32), slice(32, 48), slice(48, 64)):
            c['shape_words'][sl] = [bits(rng.uniform(-8, 8)) for _ in range(16)]
        c['input_matrix'] = [bits(rng.uniform(-8, 8)) for _ in range(16)]
        c['shape_geometry_words'][3:9] = [bits(rng.uniform(-5, 5)) for _ in range(6)]
        add(c)
    for sequence in range(2):
        previous = None
        for i in range(64):
            c = base(f'persistent-{sequence}-{i}')
            c['continue_state'] = i != 0
            if sequence: c['shape_words'][16:32] = [bits(x) for x in [.5, 1, 2, .25, 0, .25, -.5, 3, 1, 2, -3, 4, -2, .125, 7, 1]]
            if previous:
                c['shape_words'] = copy.deepcopy(previous['expected']['shape_words'])
                c['member_words'] = copy.deepcopy(previous['expected']['member_words'])
            c['input_matrix'] = [bits(rng.uniform(-3, 3)) for _ in range(16)]
            add(c); previous = c
    required = set(words)
    excluded = set(range(0x120248, 0x120274, 4))
    assert required - set(coverage) == excluded
    assert branches[0x120240] == {True}
    rows = [dict(pc=f'{pc:08x}', word=f'{word:08x}', cases_reached=coverage.get(pc, 0),
                 outcomes=sorted(branches.get(pc, set())), classification='executed' if pc in coverage else 'current_E0_nonnull_excluded')
            for pc, word in sorted(words.items())]
    return dict(format='pod-shape49w-v1', cases=cases), dict(compiled_words=len(words), reached_words=len(coverage),
        excluded_nonnull_registration_words=len(excluded), unaccounted_words=0, rows=rows)


SOURCE_MUTANTS = [
    ('omit_transform_call', 0x11ff9c, 0),
    ('history_reads_composed', 0x11f6d0, 0x24670000),
    ('current_written_over_previous', 0x11f704, 0x246700c0),
    ('compose_uses_current_as_local', 0x11f72c, 0x24640080),
    ('force_local_W_to_one', 0x11f754, 0x4be0224b),
    ('center_reads_half_x', 0x11f7b0, 0xc4600018),
    ('radius_omits_z_square', 0x11f808, 0x46000006),
    ('center_W_zero', 0x11f834, 0x4a200258),
    ('center_omits_translation', 0x11f844, 0),
]
NATIVE_MUTANTS = [
    ('history_after_current', '&state.world80, sizeof(next.previousC0)', '&mainCachedWorld100, sizeof(next.previousC0)'),
    ('incoming_world_ignored', '&mainCachedWorld100, sizeof(next.world80)', '(static_cast<void>(mainCachedWorld100), &state.world80), sizeof(next.world80)'),
    ('local_ignored', 'source.local40.v[4 * row]', 'world.v[4 * row]'),
    ('omit_W_term', 'source.local40.v[4 * row + 3]', '0.0f'),
    ('center_uses_composed', 'f.mul(world.v[lane], source.rootCenter.v[0])', 'f.mul(next.composed00.v[lane], source.rootCenter.v[0])'),
    ('radius_scaled', 'next.radius80 = f.root(', 'next.radius80 = world.v[0] * f.root('),
    ('radius_omits_z', 'f.mul(source.rootHalf.v[2], source.rootHalf.v[2])', '0.0f'),
    ('center_W_zero', 'next.center60.v[3] = 1.0f;', 'next.center60.v[3] = 0.0f;'),
    ('fused_composition', 'f.add(value, f.mul(world.v[4 + lane], source.local40.v[4 * row + 1]))', 'std::fma(world.v[4 + lane], source.local40.v[4 * row + 1], value)'),
    ('identity_shortcut', '    //11F7FC..11F81C:', '    { BodyMatrix48G identity{}; identity.v[0]=identity.v[5]=identity.v[10]=identity.v[15]=1; if (!std::memcmp(&source.local40, &identity, sizeof(identity))) next.composed00=world; }\n    //11F7FC..11F81C:'),
    ('accept_nonnull_E0', 'state.registrationE0 != 0', 'false'),
    ('accept_other_type', 'state.type88 != 0', 'false'),
    ('rounding_guard_omitted', 'std::fegetround() != FE_TONEAREST', 'false'),
    ('nonfinite_publishes', 'if (!f.valid) return false;', 'if (!f.valid) { state = next; return false; }'),
    ('scalar_root_subnormal_not_flushed', 'if ((word & 0x7f800000u) == 0)', 'if (false)'),
]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output-dir', type=Path)
    parser.add_argument('--skip-mutations', action='store_true')
    parser.add_argument('--skip-sanitized', action='store_true')
    args = parser.parse_args()
    manifest, words, dispatch, decoded = authenticate()
    packet, coverage = fixtures(words, dispatch, decoded)
    with tempfile.TemporaryDirectory(prefix='pod-shape49w-') as temporary:
        build = Path(temporary)
        out = args.output_dir.resolve() if args.output_dir else build / 'reports'
        out.mkdir(parents=True, exist_ok=True)
        def save(name, value): (out / name).write_text(json.dumps(value, indent=2) + '\n')
        save('source-coverage.json', coverage)
        save('fixtures.json', packet)
        save('asset-decoded.json', decoded)
        save('authentication.json', dict(elf_sha256=ELF_SHA, asset_sha256=ASSET_SHA,
            authenticated_blocks=len(manifest['blocks']), executed_words=len(words), actual_type=decoded['type'],
            scalar_sqrt_reference_sha256=SQRT_SHA,
            loader_scope='Static original type assignment and complete native bounded decoder; original allocation/file-I/O loader not executed.'))
        inputs = [Path(__file__).resolve(), PRODUCTION, ROOT / 'src/game/pod_shape49w.h', TEST,
                  ROOT / 'src/assets/pod_shape49w.h', ROOT / 'src/assets/pod_shape49w.cpp', SQRT_REFERENCE]
        inputs += [ROOT / rel for rel in ('tools/gameplay/group_shape_fixtures48t.py',
            'tools/gameplay/candidate_collection_fixtures48s.py', 'tools/gameplay/spring_fixture48j.py',
            'tools/gameplay/query_oracle48h.py', 'tools/assets/decode_collision48g.py',
            'src/assets/import_collision48g.cpp', 'src/assets/import_decode48c.cpp')]
        save('proof-inputs.json', {path.relative_to(ROOT).as_posix(): sha(path.read_bytes()) for path in inputs})
        commands = []
        def run(command, name, env=None, must_pass=True):
            result = subprocess.run([str(x) for x in command], cwd=ROOT, capture_output=True, text=True, env=env)
            commands.append(dict(name=name, argv=[str(x) for x in command], returncode=result.returncode))
            (out / (name + '.stdout')).write_text(result.stdout)
            (out / (name + '.stderr')).write_text(result.stderr)
            if must_pass: assert result.returncode == 0, f'{name}: {result.stdout}\n{result.stderr}'
            return result
        compiler = os.environ.get('CXX', 'c++')
        run([compiler, '--version'], 'compiler')
        includes = ['-I', ROOT / 'src/game', '-I', ROOT / 'src/assets', '-I', ROOT / 'third_party/native-json']
        def compile_set(name, optimize, sanitize=False):
            flags = FLAGS + includes + optimize
            if sanitize: flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie']
            objects = []
            for stem, path in [('leaf', PRODUCTION), ('adapter', ROOT / 'src/assets/pod_shape49w.cpp'),
                               ('decoder', ROOT / 'src/assets/import_collision48g.cpp'),
                               ('archive', ROOT / 'src/assets/import_decode48c.cpp'), ('test', TEST)]:
                obj = build / (name + '-' + stem + '.o')
                run([compiler, *flags, *(['-std=c++20'] if stem == 'test' else []), '-fstack-usage', '-c', path, '-o', obj], name + '-build-' + stem)
                objects.append(obj)
                if stem == 'leaf': (out / (name + '-stack.su')).write_text(obj.with_suffix('.su').read_text())
            exe = build / name
            run([compiler, *flags, *(['-no-pie'] if sanitize else []), *objects, '-lz', '-o', exe], name + '-link')
            return exe, objects, flags
        clean = dict(os.environ)
        exe, objects, flags = compile_set('normal', ['-O2', '-g'])
        invocation = [out / 'fixtures.json', EVIDENCE / 'podboxf.col', ROOT / 'work/native/project/disc/PODS/POD01/POD01.RES']
        normal = json.loads(run([exe, *invocation], 'normal-run', clean).stdout)
        assert normal['passed'] and normal['native_allocations'] == 0
        save('normal.json', normal)
        plain, _, _ = compile_set('plain', ['-O0'])
        plain_result = json.loads(run([plain, *invocation], 'plain-run', clean).stdout)
        assert plain_result == normal
        save('plain.json', plain_result)
        undefined = run(['nm', '-u', objects[0]], 'leaf-undefined').stdout
        assert not any(symbol in undefined for symbol in ['_Znwm', '_Znam', 'malloc', 'calloc', 'realloc', 'pthread'])
        mutants = []
        if not args.skip_mutations:
            for name, pc, word in SOURCE_MUTANTS:
                changed = dict(words); changed[pc] = word
                witness = None
                for case in packet['cases']:
                    got, _ = execute(changed, dispatch, case)
                    if any(got[field] != case['expected'][field] for field in ('member_words', 'shape_words', 'shape_geometry_words')):
                        witness = case['name']; break
                assert witness is not None, name
                mutants.append(dict(name=name, kind='original_word', pc=f'{pc:08x}', word=f'{word:08x}', witness=witness))
            source = PRODUCTION.read_text()
            for name, before, after in NATIVE_MUTANTS:
                assert source.count(before) == 1, name
                file = build / (name + '.cpp'); file.write_text(source.replace(before, after))
                obj = build / (name + '.o')
                run([compiler, *flags, '-c', file, '-o', obj], name + '-build')
                binary = build / name
                run([compiler, *flags, obj, *objects[1:], '-lz', '-o', binary], name + '-link')
                result = run([binary, *invocation], name + '-run', clean, False)
                assert result.returncode == 1 and 'mismatch' in result.stderr, (name, result.stderr)
                mutants.append(dict(name=name, kind='isolated_native_change', witness=result.stderr.strip().splitlines()[0]))
        save('mutations.json', dict(count=len(mutants), detected=mutants, setup_failures_counted=0, production_edited=False))
        sanitized = None
        if not args.skip_sanitized:
            exe, _, _ = compile_set('sanitized', ['-O1', '-g'], True)
            env = dict(clean, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
            sanitized = json.loads(run([exe, *invocation], 'sanitized-run', env).stdout)
            assert sanitized == normal
            save('sanitized.json', sanitized)
        save('commands.json', commands)
        summary = dict(passed=True, differential_cases=len(packet['cases']), source_words=len(words),
            original_instructions=sum(c['expected']['steps'] for c in packet['cases']), source_coverage=coverage['reached_words'],
            native=normal, mutation_count=len(mutants), sanitized=sanitized is not None,
            scope='Actual authored POD01 type0 transform/history/center/radius, current E0-null. No collision traversal or spatial registration.')
        save('summary.json', summary)
        print(json.dumps(summary))


if __name__ == '__main__': main()
