#!/usr/bin/env python3
"""Authenticate and prove the49V numeric blend against retained original words.

No runtime installation, native-output oracle or call-target substitution.
Default evidence/build products are temporary; --output-dir opts in to saving.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PRODUCTION = ROOT / 'src/game/chariot_blend49v.cpp'
TEST = ROOT / 'tests/native/chariot_blend49v.cpp'
EVIDENCE = ROOT / 'evidence/chariotLifecycle49V/blend'
TRACE = EVIDENCE / 'row_trace49v.h'
FLAGS = ['-ffp-contract=off', '-fno-fast-math', '-Wall', '-Wextra', '-Werror', '-Wno-misleading-indentation']
SOURCE_MUTANTS = [
    ('physical_reads_local_instead_of_cached_world', 0x21ba5c, 0x244500c0),
    ('first_virtual_row_is_forward', 0x21ba0c, 0x24060002),
    ('first_physical_row_is_forward', 0x21ba48, 0x24060002),
    ('interpolation_division_reversed', 0x21babc, 0x46020003),
    ('translation_difference_reversed', 0x21baf4, 0x46041841),
    ('translation_multiply_omitted', 0x21bb00, 0),
    ('second_cross_omitted', 0x21bde8, 0),
    ('up_normalization_omitted', 0x21be1c, 0),
    ('forward_normalization_omitted', 0x21be3c, 0),
    ('row0_construction_omitted', 0x21be44, 0),
    ('translation_x_publish_omitted', 0x21be74, 0),
    ('elapsed_update_omitted', 0x21be94, 0),
    ('expiry_greater_or_equal', 0x21bea0, 0x46000834),
    ('expired_virtual_clear_omitted', 0x21beb0, 0),
    ('rowgetter_position_x_reads_y', 0x14ec50, 0xc4a00034),
]
NATIVE_MUTANTS = [
    ('reverse_interpolation', 'f.sub(last, first)', 'f.sub(first, last)'),
    ('fused_lerp', 'f.add(first, f.mul(alpha, difference))', 'std::fma(alpha, difference, first)'),
    ('clamp_negative_alpha_wrongly', 'if (alpha <= 0.0f) return first;', 'if (alpha < -1.0f) return first;'),
    ('endpoint_matrix_copy', '    Math f;\n', '    if (oldElapsed84C <= 0.0f) { output = virtualLocal; return true; }\n    if (oldElapsed84C >= positiveDuration848) { output = physicalCachedWorld; return true; }\n    Math f;\n'),
    ('skip_up_rebuild', 'const V rebuiltUp = cross(forward, x, f);', 'const V rebuiltUp = up; (void)x;'),
    ('reverse_cross_handedness', 'const V x = cross(up, forward, f);', 'const V x = cross(forward, up, f);'),
    ('skip_forward_normalization', 'const V normalizedForward = f.normalize(forward);', 'const V normalizedForward = forward;'),
    ('extra_row0_normalization', 'const V right = cross(normalizedUp, normalizedForward, f);', 'const V right = f.normalize(cross(normalizedUp, normalizedForward, f));'),
    ('zero_norm_invented_identity', 'if (norm == 0.0f) q =', 'if (norm == 0.0f) return V{{1,0,0,0}};\n        if (norm == 0.0f) q ='),
    ('erase_signed_zero', 'output = result;', 'for (float& lane : result.v) if (lane == 0.0f) lane = 0.0f;\n    output = result;'),
    ('position_w_zero', 'result.v[15] = 1.0f;', 'result.v[15] = 0.0f;'),
    ('duration_accepts_negative', 'positiveDuration848 <= 0.0f', 'positiveDuration848 == 0.0f'),
    ('upper_endpoint_skips_delay_slot_subtraction', '    const float difference = f.sub(last, first);', '    if (alpha >= 1.0f) return last;\n    const float difference = f.sub(last, first);'),
]

def sha(data):
    return hashlib.sha256(data).hexdigest()

def authenticate():
    path = ROOT / 'tools/gameplay/chariot_root_recover49u.py'
    spec = importlib.util.spec_from_file_location('retained_root49u_provenance', path)
    module = importlib.util.module_from_spec(spec)
    previous_bytecode_mode = sys.dont_write_bytecode
    try:
        sys.dont_write_bytecode = True
        spec.loader.exec_module(module)
    finally:
        sys.dont_write_bytecode = previous_bytecode_mode
    manifest, words = module.verify()
    # These are the original ordinary blend and publication/timer words. The
    # interpreter and original-word tables stay byte-identical to inherited49U.
    required = {0x21ba54: 0x8c420288, 0x21ba5c: 0x24450100,
        0x21ba74: 0x8c420288, 0x21ba7c: 0x24450100,
        0x21ba94: 0x8c420288, 0x21ba9c: 0x24450100,
        0x21babc: 0x46001003, 0x21bdc0: 0x20070001,
        0x21be84: 0x0c0a2d24, 0x21be94: 0x46000840,
        0x21bea0: 0x46000836, 0x21beb0: 0xae00083c}
    assert all(words[pc] == word for pc, word in required.items())
    return manifest, words

def coverage(path, words):
    counts = {int(pc, 16): (int(h), int(t), int(n)) for pc, h, t, n in
        (line.split() for line in path.read_text().splitlines())}
    assert counts.keys() == words.keys()
    rows = []
    missing = []
    for pc in range(0x21ba04, 0x21beb4, 4):
        h, t, n = counts[pc]
        row = dict(pc=f'{pc:08x}', word=f'{words[pc]:08x}', hits=h, taken=t, not_taken=n)
        if not h:
            assert 0x21bdf4 <= pc < 0x21be00, f'uncovered reachable blend word {pc:08x}'
            row.update(coverage_class='source_constant_unreachable', reason=
                '21BDC0 writes a3=1, matching t0=1; the alternate cross branch cannot execute in this construction.')
            missing.append(pc)
        rows.append(row)
    getters = [dict(pc=f'{pc:08x}', word=f'{words[pc]:08x}', hits=counts[pc][0],
        taken=counts[pc][1], not_taken=counts[pc][2]) for pc in range(0x14ebb0, 0x14ecfc, 4)]
    return dict(blend_words=len(rows), blend_reached=sum(r['hits'] > 0 for r in rows),
        source_constant_unreachable=len(missing), unaccounted=0, blend=rows,
        row_getter=getters, row_getter_scope='Only rows3,2,1 are admitted. Row0, invalid-index diagnostics and trailing padding are retained but not executed.',
        remaining_helpers='Original28B490,28C4F0 execute for output publication. Full retained49U blocks are authenticated; this leaf proof does not claim all their routes.')

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output-dir', type=Path)
    parser.add_argument('--skip-sanitized', action='store_true')
    parser.add_argument('--skip-mutations', action='store_true')
    args = parser.parse_args()
    manifest, words = authenticate()
    with tempfile.TemporaryDirectory(prefix='chariot-blend49v-') as tmp:
        base = Path(tmp)
        out = args.output_dir.resolve() if args.output_dir else base / 'evidence'
        out.mkdir(parents=True, exist_ok=True)
        build = base / 'build'
        build.mkdir()
        def save(name, data):
            (out / name).write_text(json.dumps(data, indent=2) + '\n')
        def run(command, name, env=None, success=True):
            result = subprocess.run([str(v) for v in command], cwd=ROOT, capture_output=True, text=True, env=env)
            (out / (name + '.stdout')).write_text(result.stdout)
            (out / (name + '.stderr')).write_text(result.stderr)
            if success:
                assert result.returncode == 0, f'{name}: {result.stdout}\n{result.stderr}'
            return result
        cxx = os.environ.get('CXX', 'c++')
        compiler = run([cxx, '--version'], 'compiler').stdout.splitlines()[0]
        def compile_set(tag, traced=True, sanitizer=False):
            flags = FLAGS + (['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie']
                if sanitizer else ['-O2', '-g'])
            if traced:
                flags += ['-include', TRACE]
            obj = build / f'blend-{tag}.o'
            run([cxx, '-std=c++17', *flags, '-fstack-usage', '-c', PRODUCTION, '-o', obj], 'build-' + tag)
            exe = build / ('blend-' + tag)
            test_obj = build / ('test-' + tag + '.o')
            run([cxx, '-std=c++20', *flags, '-c', TEST, '-o', test_obj], 'build-test-' + tag)
            run([cxx, *flags, *(['-no-pie'] if sanitizer else []), test_obj, obj, '-o', exe], 'link-' + tag)
            if not sanitizer:
                (out / ('stack-' + tag + '.su')).write_text(obj.with_suffix('.su').read_text())
            return exe, obj, test_obj, flags
        clean = dict(os.environ)
        clean.pop('RR_CHARIOT_BLEND49V_COVERAGE', None)
        exe, obj, test_obj, flags = compile_set('normal')
        env = dict(clean, RR_CHARIOT_BLEND49V_COVERAGE=str(out / 'coverage.tsv'))
        normal = json.loads(run([exe], 'normal', env).stdout)
        assert normal['passed'] and normal['native_allocations'] == 0
        save('normal.json', normal)
        save('coverage-accounting.json', coverage(out / 'coverage.tsv', words))
        original_coverage_sha = sha((out / 'coverage.tsv').read_bytes())
        readonly = json.loads(run([exe], 'default-read-only', clean).stdout)
        assert readonly == normal and sha((out / 'coverage.tsv').read_bytes()) == original_coverage_sha
        save('read-only-default.json', dict(passed=True, evidence_unchanged=True, explicit_output_only=True))
        plain_exe, plain_obj, _, _ = compile_set('plain', False)
        plain = json.loads(run([plain_exe], 'plain', clean).stdout)
        assert {k:v for k,v in plain.items() if k != 'checks'} == {k:v for k,v in normal.items() if k != 'checks'}
        save('plain.json', plain)
        undefined = run(['nm', '-u', plain_obj], 'native-undefined').stdout
        assert not any(symbol in undefined for symbol in ['_Znwm', '_Znam', 'malloc', 'calloc', 'realloc', 'pthread'])
        mutants = []
        if not args.skip_mutations:
            for name, pc, word in SOURCE_MUTANTS:
                result = run([exe, f'{pc:x}', f'{word:x}'], 'mutant-' + name, clean, False)
                assert result.returncode != 0 and 'mismatch' in result.stderr, (name, result.stderr)
                mutants.append(dict(name=name, kind='original_word', pc=f'{pc:08x}', word=f'{word:08x}', mismatch=result.stderr.strip()))
            source = PRODUCTION.read_text()
            for name, before, after in NATIVE_MUTANTS:
                assert source.count(before) == 1, (name, source.count(before))
                file = build / (name + '.cpp')
                file.write_text(source.replace(before, after))
                mutant_obj = build / (name + '.o')
                run([cxx, '-std=c++17', *flags, '-I', ROOT / 'src/game', '-c', file, '-o', mutant_obj], 'mutant-build-' + name)
                binary = build / name
                run([cxx, *flags, test_obj, mutant_obj, '-o', binary], 'mutant-link-' + name)
                result = run([binary], 'mutant-' + name, clean, False)
                assert result.returncode != 0 and 'mismatch' in result.stderr, (name, result.stderr)
                mutants.append(dict(name=name, kind='isolated_native_change', before=before, after=after, mismatch=result.stderr.strip()))
        save('mutations.json', dict(count=len(mutants), detected=mutants, production_edited=False, setup_failures_counted=0))
        sanitized = None
        if not args.skip_sanitized:
            san_exe, _, _, _ = compile_set('sanitized', True, True)
            san_env = dict(clean, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
            sanitized = json.loads(run([san_exe], 'asan-ubsan', san_env).stdout)
            assert sanitized == normal
            save('sanitized.json', sanitized)
        inputs = [Path(__file__), PRODUCTION, PRODUCTION.with_suffix('.h'), TEST, TRACE,
            ROOT / 'tools/gameplay/chariot_root_recover49u.py',
            ROOT / 'evidence/chariotRoot49U/root/source/root_machine.h',
            ROOT / 'evidence/chariotRoot49U/root/source/root_words.h',
            ROOT / 'evidence/chariotRoot49U/root/source/source-manifest.json']
        summary = dict(passed=True, scope='Pure21BA04..21BE7C blended matrix, observed through original28B490 publication and21BEB0 strict timer expiry. No complete lifecycle claim from this leaf alone.',
            elf_sha256=manifest['elf_sha256'], compiler=compiler, production_language='C++17', test_language='C++20',
            flags=FLAGS, normal=normal, plain=plain, sanitized=sanitized,
            leak_sanitizer='disabled; no LeakSanitizer claim', mutations=len(mutants), original_call_target_substitutions=0,
            inherited_files_edited=False, native_storage='Fixed automatic scalars/vectors/matrix; no allocation, graph access, callbacks or I/O.',
            hashes={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in inputs})
        save('validation.json', summary)
        print(json.dumps(dict(passed=True, cases=normal['cases'], mutations=len(mutants), sanitized=sanitized is not None,
            output=str(out) if args.output_dir else 'temporary')))

if __name__ == '__main__':
    main()
