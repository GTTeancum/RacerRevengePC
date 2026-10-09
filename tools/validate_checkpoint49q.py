#!/usr/bin/env python3
"""Validate frozen 49Q evidence without executing or rewriting numerical proofs.

All report gates are ordinary Python checks (also active under -O). Negative gate
controls mutate decoded reports in memory only. Final validation seals every new
file, including the gates and review, for archive_checkpoint49q.py.
"""
import argparse
import copy
import datetime
import hashlib
import json
import re
import statistics
import struct
import zipfile
from pathlib import Path, PurePosixPath

ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/engineAnimation49Q'
BASELINE_SHA = 'c240d9ff1ac9ef5443e224700a00bda295c63e0832bee1220123452e4c0e9e6b'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
TRACE_SHA = '04872b9ad46eec311e42898f5b82d4766f83bbbe5eb5da69328132ecf6f94202'
GOLDEN_SHA = '9924eeb2c49ccb87824b62c13d17b842aa853d45d87b531ad1967f071ec768bc'
CONTROL_SHA = '677d7da88a23da6f9d860de0afcd02e8e247840c3ee8988150b0387d6f9a2db5'
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
ALLOWED = {'RacerRevengePC/CMakeLists.txt', 'RacerRevengePC/README.md'}
TOTAL_CHECKS = 134992831
EXPECTED_SUITES = set('model_motion49q engine_animation49q engine_animation_diagnostic49q scene_graph_api49p scene_graph49p chariot_projection49p scene_frame_diagnostic49p model_geometry49o model_bindings49o model_motion49o model_bounds49o authored_geometry49o pod_model_geometry49o authored_body_diagnostic49o pod_physics49n authored_parameters_api49n authored_parameters49n authored_body_diagnostic49n gradient_noise49m secondary_body_api49m secondary_wrapper49m coupled_body_advancement49m pod_sphere_constraint_api49l pod_sphere_constraint49l advancement_selectors49k body_advancement49k physics48c physics48d physics48e body_scene48g body_wrapper_nearest49k clock_quantization49j clock_timing_diagnostics49j frame_timing49i camera_frame49f tv_camera49e control_presets49d player_events49a event_dispatch48z device_held48y player_input48x control_binding48w control_shaping48v control_filter48b'.split())
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}
ADDED_INPUTS = {
    'cmake/NativeEngineAnimation49Q.cmake',
    'evidence/engineAnimation49Q/connected/exact-goldens49q.txt',
    'evidence/engineAnimation49Q/connected/input-manifest.json',
    'evidence/engineAnimation49Q/connected/supplied-controls49q.bin',
    'evidence/engineAnimation49Q/flow/flow_fixture.h',
    'evidence/engineAnimation49Q/single-motion/blocks.h',
    'evidence/engineAnimation49Q/single-motion/motion_machine.h',
    'src/game/engine_animation49q.cpp', 'src/game/engine_animation49q.h',
    'src/game/model_motion49q.cpp', 'src/game/model_motion49q.h',
    'tests/native/engine_animation49q.cpp', 'tests/native/engine_animation_diagnostic49q.cpp',
    'tests/native/model_motion49q.cpp', 'tools/test_native_engine_animation49q.py',
}


class GateError(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise GateError(message)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def sha(path):
    return digest(path.read_bytes())


def load(name):
    return json.loads((E / name).read_text())


def pins(rows):
    for name, value in rows.items():
        record = value if isinstance(value, dict) else {'sha256': value}
        path = ROOT / name
        require(path.is_file() and sha(path) == record['sha256'], 'File pin: ' + name)
        if 'bytes' in record:
            require(path.stat().st_size == record['bytes'], 'File size: ' + name)


def safe_names(names):
    require(len(names) == len(set(names)), 'Duplicate archive paths')
    for name in names:
        path = PurePosixPath(name)
        require(name and not path.is_absolute() and '\\' not in name and
                not any(p in ('', '.', '..') for p in name.split('/')) and
                ':' not in name and '\x00' not in name, 'Unsafe archive path: ' + name)


def strict_command(command, sanitized, standard=None, traced=False):
    require(isinstance(command, list) and bool(command), 'Missing command')
    require(not BAD_FLAGS.intersection(command), 'Unsafe floating-point/sanitizer command')
    require(all(x in command for x in ['-fno-fast-math', '-ffp-contract=off']), 'Missing strict FP flags')
    if standard:
        require(standard in command, 'Wrong language standard')
    require(('-fsanitize=address,undefined' in command) == sanitized, 'Wrong sanitizer command')
    if traced:
        require('-include' in command and Path(command[command.index('-include') + 1]).name == 'flag_trace.h',
                'Missing ordered-store instrumentation')


def sampler_gate(report, coverage, mode):
    require(report['status'] == 'pass' and report['elf_sha256'] == ELF_SHA, 'Sampler failed/source changed')
    require(report['source_words_verified'] == 1246 and report['native_allocations'] == 0, 'Sampler words/allocations')
    require(report['mode'] == ('normal' if mode == 'normal' else 'ASan+UBSan (LSan disabled)'), 'Sampler mode')
    require(report['native_result'] == 'PASS ModelMotion49Q: 1042 cases, 5615735 checks, 1063 native calls, 3769236 original instructions, 40163 ordered flag stores, zero native allocations', 'Sampler result')
    strict_command(report['command'], mode == 'sanitized', '-std=c++17', True)
    expected = {'frozen_words': 1246, 'reached_words': 892, 'unreached_words_with_specific_reason': 354,
                'reached_branches': 76, 'both_directions': 35, 'one_sided_with_specific_reason': 41,
                'unaccounted_words_or_branch_directions': 0}
    require(report['original_instruction_accounting'] == coverage['accounting'] == expected, 'Sampler accounting')
    require(coverage['ordered_store_tracing'] is True and coverage['flag_stores_compared'] == 40163, 'Focused flag tracing')
    for key, value in {'cases': 1042, 'checks': 5615735, 'native_calls': 1063,
                       'original_instructions': 3769236, 'native_allocations': 0}.items():
        require(coverage[key] == value, 'Sampler coverage ' + key)
    rows = report['sensitivity']
    require(len(rows) == (16 if mode == 'normal' else 9), 'Sampler mutation count')
    require(report['source_mutations_detected'] == 9 and report['native_mutations_detected'] == (7 if mode == 'normal' else 0), 'Sampler mutation split')
    require(len({(r['kind'], r['name']) for r in rows}) == len(rows), 'Duplicate sampler controls')
    for row in rows:
        require(row['detected'] is True and ('mismatch' in row['diagnostic'] or 'native missing flag store' in row['diagnostic']), 'Sampler control did not expose mismatch')


def flow_gate(validation, controls, coverage):
    expected = {'cases': 459, 'checks': 953546, 'native_calls': 476, 'native_allocations': 0,
                'original_instructions': 3927396, 'original_single_motion_calls': 1436,
                'real_chariot_prefix_effect_cases': 332, 'normal_source_substitutions': 0, 'all_match': True}
    require(validation['status'] == 'passed' and validation['normal'] == validation['asan_ubsan'] == expected, 'Flow modes/counters')
    require(validation['production_language'] == 'C++17' and validation['test_language'] == 'C++20', 'Flow language')
    require(validation['leak_sanitizer'] == 'disabled, not claimed', 'Flow sanitizer boundary')
    require(all(x in validation['flags'] for x in ['-ffp-contract=off', '-fno-fast-math', '-Werror']), 'Flow flags')
    represented = [r for r in controls['mutations'] if not r.get('scope_control')]
    outside = [r for r in controls['mutations'] if r.get('scope_control')]
    require(len(represented) == 11 and len(outside) == 1 and len({r['name'] for r in controls['mutations']}) == 12, 'Flow mutation count/scope')
    require(outside[0]['name'] == 'Chariot_left_brake_reads_right_mirror' and 'correctly unchanged' in outside[0]['result'], 'Chariot scope control')
    require(controls['invalid_context_failures_counted'] == 0 and controls['all_represented_mutants_detected_by_actual_state_or_trace_mismatch'] is True, 'Invalid flow control accounting')
    for row in represented:
        require(row['detected'] is True and any(s in row['actual_mismatch'] for s in ['mismatch', 'source sample count', 'source left/right/prefix call order', 'filter store order']), 'Flow lacks actual mismatch')
    require(coverage['total_source_words'] == 408 and coverage['executed_unique_words'] == 181 and coverage['executed_dynamic_instructions_in21A000'] == 71093, 'Flow coverage counts')
    require(not coverage['admitted_unexecuted_words'] and coverage['every_dynamic_gate_both_directions'], 'Flow uncovered words')
    require(sum(x['words'] for x in coverage['exclusions']) == 227, 'Flow exclusions')
    dynamic = {0x21a018, 0x21a038, 0x21a0dc, 0x21a0ec, 0x21a0f8, 0x21a104, 0x21a224, 0x21a230, 0x21a240, 0x21a2a4, 0x21a2b0, 0x21a2c0}
    require({int(r['pc'], 16) for r in coverage['branches'] if r['taken'] > 0 and r['not_taken'] > 0} == dynamic, 'Flow branch directions')


def independent_gate(review, results, controls):
    require(review['status'] == 'pass' and review['normal_and_sanitized_match'] is True, 'Independent proof status')
    require((review['independent_cases'], review['independent_checks'], review['executed_original_filter_instructions'], review['semantic_controls']) == (1421, 1892448, 51756, 11), 'Independent proof counters')
    require(results['normal'] == results['asan_ubsan_no_lsan'] == 'PASS independent49Q: 1421 cases; 1892448 checks; 51756 original filter instructions; zero production allocations', 'Independent result')
    require(controls['count'] == len(controls['controls']) == 11 and controls['production_edited'] is False, 'Independent controls')
    require(len({r['name'] for r in controls['controls']}) == 11, 'Duplicate independent control')
    for row in controls['controls']:
        require(row['exit_code'] == 1 and row['result'] == 'wrong result detected' and row['output'].startswith('FAIL independent49Q:') and 'case=' in row['output'], 'Independent control crash/non-result')
        require(row['production_sha256'] != row['mutant_sha256'], 'Identical independent mutant')


def connected_gate(summary, validation, trace, manifest):
    expected = {'frames': 576, 'scenarios': 6, 'checks': 4063640, 'native_hot_allocations': 0,
                'native_hot_calls': 2760, 'original_animation_instructions': 6439636,
                'original_control_instructions': 254663, 'original_scene_instructions': 44964,
                'engine_nodes_compared': 30, 'state3_4_base_skips': 24, 'source_order_checks': 552,
                'real_brake_changes': 1504, 'real_fin_changes': 2256, 'world_cache_retentions': 576}
    require(summary['passed'] is True and trace['summary'] == summary, 'Connected summary/trace disagreement')
    for key, value in expected.items():
        require(summary[key] == value, 'Connected ' + key)
    for key in ['bind_reset_changed_frames', 'clamped_feedback_changed_frames', 'paired_shortcut_changed_frames', 'wrong_channel_changed_frames', 'wrong_dt_changed_frames']:
        require(summary[key] > 0, 'Connected insensitive alternative ' + key)
    require(validation['passed'] and validation['read_only_fixture'] and validation['leak_sanitizer_used'] is False, 'Connected proof mode')
    require(set(validation['runs']) == {'normal', 'asan-ubsan'}, 'Connected missing mode')
    for mode, row in validation['runs'].items():
        strict_command(row['build_command'], mode == 'asan-ubsan', '-std=c++17', True)
        require(len(row['repeats']) == 3, 'Connected repeat count')
        for repeat in row['repeats']:
            require(repeat['summary'] == summary and repeat['trace_sha256'] == TRACE_SHA and repeat['stdout_sha256'] == '055f4847d004d6b2a6e8328369736d0c8de5d5d907f4faa3933098e92560c9d4', 'Connected repeat diverged')
    require(set(validation['mutations']) == {'clamped-filter-feedback', 'swap-engine-call-order', 'wrong-control-channel'}, 'Connected controls missing')
    for row in validation['mutations'].values():
        require(row['rejected'] is True and row['returncode'] == 1 and row['diagnostic'].startswith('FAIL EngineAnimationDiagnostic49Q'), 'Connected mutant not numerical failure')
    require(manifest['sha256'] == CONTROL_SHA and manifest['bytes'] == 39276 and len(manifest['scenarios']) == 6 and all(r['frames'] == 96 for r in manifest['scenarios']), 'Connected supplied controls')


def report_gate(report, mode):
    sanitized = mode == 'sanitized'
    flags = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-fno-fast-math', '-frounding-math', '-ffp-contract=off']
    flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-sanitize-recover=all'] if sanitized else ['-O2']
    require(report['status'] == 'passed' and not report['errors'] and not report['changed_inputs'], 'Regression not passed/inputs changed')
    require(report['scope'] == 'all_engine-animation_checks' and report['serial'] is True, 'Regression scope')
    require(report['compile_flags'] == flags and report['link_flags'] == (['-fsanitize=address,undefined'] if sanitized else []), 'Regression flags')
    require(report['sanitizers'] is sanitized and report['leak_detection'] is False, 'Regression sanitizer mode')
    require(len(report['selected_tests']) == 44 and set(report['selected_tests']) == EXPECTED_SUITES, 'Regression suite selection')
    require(len(report['tests']) == 44 and {t['name'] for t in report['tests']} == EXPECTED_SUITES, 'Regression executed suites')
    require(len(report['input_hashes']) == 258 and len(report['build']) == 38 and len({b['source'] for b in report['build']}) == 38, 'Regression input/unit counts')
    baseline_report = json.loads((ROOT / 'evidence/sceneProjection49P/final-normal/regression-report.json').read_text())
    require(set(report['input_hashes']) == set(baseline_report['input_hashes']) | ADDED_INPUTS, 'Regression input membership')
    expected_units = {b['source'] for b in baseline_report['build']} | {'src/game/model_motion49q.cpp', 'src/game/engine_animation49q.cpp'}
    require({b['source'] for b in report['build']} == expected_units, 'Regression production unit membership')
    for name, pin in baseline_report['input_hashes'].items():
        if name != 'CMakeLists.txt': require(report['input_hashes'][name] == pin, 'Historical tracked input changed')
    if sanitized:
        require('detect_leaks=0' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['UBSAN_OPTIONS'], 'Sanitizer environment')
    for build in report['build'] + [t['build'] for t in report['tests']]:
        require(build['status'] == 'passed' and build['returncode'] == 0, 'Regression build failed')
        command = build['command']
        require(command[1:1 + len(flags)] == flags and '-include' not in command, 'Aggregate build flags/instrumentation')
        strict_command(command, sanitized, '-std=c++20')
        require(build['stdout']['bytes'] == 0 and build['stdout']['sha256'] == EMPTY_SHA, 'Nonempty build stdout')
        if build.get('source') in ['src/game/model_motion49q.cpp', 'src/game/engine_animation49q.cpp']:
            require(build['stderr']['bytes'] == 0 and build['stderr']['sha256'] == EMPTY_SHA, 'New production compiler warning')
    for test in report['tests']:
        run = test['run']
        require(test['status'] == run['status'] == 'passed' and run['returncode'] == 0, 'Regression run failed')
        require(run['command'][0] == test['executable']['path'], 'Run executable provenance')
        require(run['stderr']['bytes'] == 0 and run['stderr']['sha256'] == EMPTY_SHA, 'Regression stderr')


def check_count(text):
    try:
        return int(json.loads(text)['checks'])
    except (ValueError, KeyError):
        match = re.search(r'checks=(\d+)', text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks', text)
        require(match is not None, 'Missing check count')
        return int(match.group(1))


def evidence_gates():
    for mode in ['normal', 'sanitized']:
        report = load(f'single-motion/{mode}/native-test-result.json')
        coverage = load(f'single-motion/{mode}/coverage.json')
        sampler_gate(report, coverage, mode)
        pins(report['verified_final_inputs'])
        require((E / f'single-motion/{mode}/model_motion49q.stdout').read_text().strip() == report['native_result'], 'Sampler raw stdout')
        for stream in ['model_motion49q.stderr', 'compile_model_motion49q.stdout', 'compile_model_motion49q.stderr']:
            require((E / f'single-motion/{mode}' / stream).read_bytes() == b'', 'Sampler raw build/stderr')
        for row in report['sensitivity']:
            prefix = 'source_' if row['kind'] == 'original-word substitution' else 'native_'
            require((E / f'single-motion/{mode}' / (prefix + row['name'] + '.stderr')).read_text().strip() == row['diagnostic'], 'Sampler raw mutation diagnostic')
    pins(load('single-motion/single-motion-proof-manifest.json')['artifacts'])
    validation = load('flow/validation.json')
    flow_gate(validation, load('flow/mutation-controls.json'), load('flow/coverage-accounting.json'))
    pins(validation['files'])
    for mode, log in [('normal', 'normal.log'), ('asan_ubsan', 'asan-ubsan.log')]:
        require(json.loads((E / 'flow' / log).read_text()) == validation[mode], 'Flow raw output')
    readonly = load('flow/read-only-default.json')
    require(readonly['default_run_passed'] and readonly['coverage_unchanged'], 'Flow read-only invocation')
    review = load('independent-review/review-manifest.json')
    results = load('independent-review/results.json')
    controls = load('independent-review/controls.json')
    independent_gate(review, results, controls)
    pins(review['production_sources']); pins(results['source_sha256']); pins(review['artifacts'])
    for key, log in [('normal', 'normal.log'), ('asan_ubsan_no_lsan', 'asan-ubsan.log')]:
        require((E / 'independent-review' / log).read_text().strip() == results[key], 'Independent raw output')
    for row in controls['controls']:
        require(sha(ROOT / row['target']) == row['production_sha256'], 'Independent stale production')
        require(sha(E / 'independent-review/controls' / (row['name'] + '.cpp')) == row['mutant_sha256'], 'Independent stale mutant')
        require((E / 'independent-review/controls' / (row['name'] + '.log')).read_text().strip() == row['output'], 'Independent raw mutation output')
    summary = load('connected/summary.json'); validation = load('connected/validation.json')
    connected_gate(summary, validation, load('connected/control-driven-engine-frames49q.json'), load('connected/input-manifest.json'))
    pins(validation['input_sha256'])
    for name, expected in [('control-driven-engine-frames49q.json', TRACE_SHA), ('exact-goldens49q.txt', GOLDEN_SHA), ('supplied-controls49q.bin', CONTROL_SHA)]:
        require(sha(E / 'connected' / name) == expected, 'Connected artifact pin ' + name)
    for name in ['normal.log', 'asan-ubsan.log']:
        require(sha(E / 'connected' / name) == '055f4847d004d6b2a6e8328369736d0c8de5d5d907f4faa3933098e92560c9d4', 'Connected raw stdout')


def original_sources():
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    require(digest(elf) == ELF_SHA and elf[:6] == b'\x7fELF\x01\x01', 'Original ELF')
    offset = struct.unpack_from('<I', elf, 28)[0]; size, count = struct.unpack_from('<HH', elf, 42)
    segments = [struct.unpack_from('<8I', elf, offset + i * size) for i in range(count)]
    def original(start, length):
        rows = [elf[o + start - v:o + start - v + length] for k, o, v, _, n, *_ in segments if k == 1 and v <= start and start + length <= v + n]
        require(len(rows) == 1, 'Original range not uniquely file-backed')
        return rows[0]
    single = load('single-motion/source-manifest.json'); flow = load('flow/source-manifest.json')
    independent = load('independent-review/source-and-assets.json'); words = load('flow/source/source-word-manifest.json')
    for manifest in [single, flow, independent, words]:
        require(manifest['elf_sha256'] == ELF_SHA, 'Manifest ELF pin')
    for row in single['blocks'] + flow['source_ranges'] + independent['source_ranges_verified'] + words['slices']:
        start, end = int(row['start'], 16), int(row['end_exclusive'], 16)
        require(end - start == 4 * row.get('words', row.get('word_count')), 'Original word range count')
        require(digest(original(start, end - start)) == row['sha256'], 'Original range hash: ' + row['name'])
        sidecar = E / 'flow/source' / (row['name'] + '.bin')
        if sidecar.exists(): require(sidecar.read_bytes() == original(start, end - start), 'Source binary sidecar')
    for address, row in words['literal_strings'].items():
        data = bytes.fromhex(row['bytes_hex'])
        require(original(int(address, 16), len(data)) == data and digest(data) == row['sha256'], 'Original selection literal')
    pins(single['retained_inputs'])
    for row in [single['oracle_base'], single['fixture'], flow['fixture'], *flow['frozen_inputs']]:
        pins({row['path']: row})
    for row in flow['assets']:
        pins({'evidence/authoredGeometry49O/asset-reader/fixtures/' + row['file']: row})
    for row in independent['authentic_members']:
        pins({'evidence/authoredGeometry49O/asset-reader/' + row['file']: row})
    require(independent['fixed_words_verified'] == 24 and independent['filter_words_executed_by_harness'] == 38, 'Independent source coverage')
    require(len(independent['authentic_members']) == 15 and len(single['blocks']) == 13 and single['word_count'] == 1246, 'Original member/range counts')
    fixture = (ROOT / single['fixture']['path']).read_bytes()
    expected_words = {}
    for block in single['blocks']:
        start, end = int(block['start'], 16), int(block['end_exclusive'], 16)
        raw = original(start, end - start)
        require(fixture[start:end] == raw, 'Sampler fixture words differ from ELF')
        for i, word in enumerate(struct.unpack('<' + str(block['words']) + 'I', raw)):
            require(start + 4 * i not in expected_words, 'Overlapping sampler source blocks')
            expected_words[start + 4 * i] = word
    listing_rows = (E / 'single-motion/source-disassembly.txt').read_text().splitlines()
    listing = {int(line.split()[0], 16): int(line.split()[1], 16) for line in listing_rows}
    require(len(listing_rows) == 1246 and listing == expected_words, 'Sampler disassembly word mismatch')
    bounds = [(int(a, 16), int(z, 16)) for a, z in re.findall(r'\{0x([a-f0-9]+),0x([a-f0-9]+)\}', (E / 'single-motion/blocks.h').read_text())]
    require(bounds == [(int(b['start'], 16), int(b['end_exclusive'], 16)) for b in single['blocks']], 'Sampler compiled source bounds')
    for mode in ['normal', 'sanitized']:
        coverage = load(f'single-motion/{mode}/coverage.json'); rows = coverage['pcs']
        require(len(rows) == 1246 and {r['pc'] for r in rows} == set(expected_words), 'Sampler coverage PC membership')
        require(sum(r['hits'] for r in rows) == 3769236 and sum(r['hits'] > 0 for r in rows) == 892, 'Sampler dynamic instruction accounting')
        for row in rows:
            require(int(row['word'], 16) == expected_words[row['pc']], 'Sampler coverage altered word')
            if not row['hits']: require(bool(row.get('unreached_reason')), 'Unexplained unreached word')
            if row['taken'] or row['not_taken']:
                require(row['taken'] + row['not_taken'] == row['hits'], 'Sampler branch accounting')
                if not (row['taken'] and row['not_taken']): require(bool(row.get('one_sided_reason')), 'Unexplained one-sided branch')
    flow_rows = [tuple(int(v, 16 if i == 0 else 10) for i, v in enumerate(line.split())) for line in (E / 'flow/coverage.tsv').read_text().splitlines()]
    require(len(flow_rows) == 408 and {r[0] for r in flow_rows} == set(range(0x21a000, 0x21a660, 4)), 'Flow coverage PC membership')
    require(sum(r[1] for r in flow_rows) == 71093 and sum(r[1] > 0 for r in flow_rows) == 181, 'Flow coverage raw counts')
    branches = [{'pc': f'{pc:08x}', 'taken': taken, 'not_taken': not_taken} for pc, hits, taken, not_taken in flow_rows if taken + not_taken]
    require(branches == load('flow/coverage-accounting.json')['branches'], 'Flow raw branch coverage')
    # Rebuild the fixed filter-word header read-only, directly from the pinned ELF.
    filter_data = original(0x21a040, 0x98)
    expected = 'static constexpr unsigned filterWords[]={\n' + ','.join(f'0x{w:08x}u' for w in struct.unpack('<38I', filter_data)) + '\n};\n'
    require((E / 'independent-review/filter_words.h').read_text() == expected, 'Filter header differs from ELF')
    # Reauthenticate literal MCF selections and actual per-track MOT endpoints.
    from configparser import ConfigParser
    names = {}; tracks = {}
    for row in independent['authentic_members']:
        path = ROOT / 'evidence/authoredGeometry49O/asset-reader' / row['file']; data = path.read_bytes()
        if path.suffix == '.mcf':
            cp = ConfigParser(); cp.read_string(data.decode('ascii'))
            for section in [s for s in cp if s.startswith('MOTION ')]:
                name = cp[section]['NAME'].lower(); require(name not in names, 'Duplicate MCF motion')
                names[name] = cp[section]['VUE'].replace('\\', '/').split('/')[-1].rsplit('.', 1)[0] + '.mot'
        if path.suffix == '.mot':
            version, frames, duration, rotation, position, positions, rotations = struct.unpack_from('<7I', data)
            require((version, frames, duration, rotation, position) == (0xfffffffe, 21, 0x3faaaaab, 0, 1), 'Actual MOT header')
            offset = 28; rows = []
            for channel, total, stride in [('position', positions, 40), ('rotation', rotations, 8)]:
                for _ in range(total):
                    keys, node = struct.unpack_from('<II', data, offset); offset += 8
                    require(keys == 21 and struct.unpack_from('<I', data, offset)[0] == 0 and struct.unpack_from('<I', data, offset + (keys - 1) * stride)[0] == duration, 'Actual MOT endpoints')
                    rows.append({'channel': channel, 'row': node, 'keys': keys}); offset += keys * stride
            require(offset == len(data), 'Trailing/truncated MOT records'); tracks[path.name] = rows
    require(names == independent['selected_names'] and tracks == independent['motion_tracks'], 'Authentic motion selection/tracks')
    require(set(names) == {'01_engineright', '01_engineleft', '01_brakeleft', '01_brakeright', '01_forward', '01_back'}, 'Engine Mid/foreign record admitted')


def regression_gates():
    summary = load('regression-summary.json')
    require(summary['status'] == 'passed' and summary['all_mode_outputs_identical'] is True and summary['checks_each'] == TOTAL_CHECKS, 'Aggregate summary')
    require(summary['semantic_trace_identical_to_focused'] is True and summary['trace_checks'] == {'aggregate': 3976868, 'focused': 4063640}, 'Aggregate/focused summary distinction')
    reports = {}
    for mode in ['normal', 'sanitized']:
        row = summary['runs'][mode]; report = load(f'final-{mode}/regression-report.json'); reports[mode] = report
        report_gate(report, mode)
        require(row['suites'] == 44 and row['tracked_inputs'] == 258 and row['inputs_unchanged'] and row['checks'] == TOTAL_CHECKS, 'Aggregate summary counters')
        require(sha(E / f'final-{mode}/regression-report.json') == row['report_sha256'], 'Aggregate report pin')
        require(len(row['tests']) == 44 and {r['name'] for r in row['tests']} == EXPECTED_SUITES, 'Summary tests')
        total = 0
        for test in row['tests']:
            path = E / f'final-{mode}' / (test['name'] + '.stdout'); raw = path.read_bytes()
            actual = next(r for r in report['tests'] if r['name'] == test['name'])['run']
            require(digest(raw) == test['stdout_sha256'] == actual['stdout']['sha256'] and len(raw) == actual['stdout']['bytes'], 'Actual test stdout pin')
            checks = check_count(raw.decode()); total += checks
            require(checks == test['checks'] and test['stderr_bytes'] == 0, 'Actual test count/stderr')
        require(total == TOTAL_CHECKS, 'Total checks')
        prior = json.loads((ROOT / 'evidence/sceneProjection49P/regression-summary.json').read_text())['runs']['normal']['tests']
        require([r for r in row['tests'] if r['name'] not in {'model_motion49q', 'engine_animation49q', 'engine_animation_diagnostic49q'}] == prior, 'Historical 41-suite exact outputs changed')
        pins(report['input_hashes'])
        for name, value in row['replayed_files'].items():
            require(sha(E / f'final-{mode}' / name) == value, 'Aggregate replay pin')
        require(set(row['replayed_files']) == {'motion-coverage.json', 'engine-coverage.tsv', 'trace.json'}, 'Aggregate replay membership')
        for binary, expected in [('model_motion49q', 5526543), ('engine_animation49q', 953546), ('engine_animation_diagnostic49q', 3976868)]:
            path = E / f'final-{mode}' / (binary + '-output.stdout')
            require(check_count(path.read_text()) == expected, 'Replay output assertion count')
            require(path.read_bytes() == (E / 'final-normal' / path.name).read_bytes(), 'Replay stdout differs by mode')
        coverage = load(f'final-{mode}/motion-coverage.json')
        require(coverage['ordered_store_tracing'] is False and coverage['flag_stores_compared'] == 0 and coverage['checks'] == 5526543, 'Aggregate hook state')
        trace = load(f'final-{mode}/trace.json'); focused = load('connected/control-driven-engine-frames49q.json')
        require(trace['summary']['checks'] == 3976868 and focused['summary']['checks'] == 4063640, 'Connected instrumented check distinction')
        del trace['summary']['checks']; del focused['summary']['checks']
        require(trace == focused, 'Aggregate/focused semantic trace mismatch')
        require((E / f'final-{mode}/engine-coverage.tsv').read_bytes() == (E / 'flow/coverage.tsv').read_bytes(), 'Aggregate original flow coverage differs')
        # Inspect durable actual compiler/linker/test bytes, including retained
        # historical compiler warnings; only runtime stderr must be empty.
        process_rows = report['build'] + [x[k] for x in report['tests'] for k in ['build', 'run']]
        require(len(row['process_logs']) == 252, 'Missing durable process logs')
        log_names = set()
        for process in process_rows:
            for stream in ['stdout', 'stderr']:
                record = process[stream]; path = E / f'final-{mode}/logs' / Path(record['path']).name
                require(path.is_file() and sha(path) == record['sha256'] and path.stat().st_size == record['bytes'], 'Preserved process log mismatch')
                require(row['process_logs'][path.name] == {'bytes': record['bytes'], 'sha256': record['sha256']}, 'Process log summary pin')
                log_names.add(path.name)
        require(log_names == set(row['process_logs']), 'Process log membership')
    require(summary['runs']['normal']['tests'] == summary['runs']['sanitized']['tests'] and summary['runs']['normal']['replayed_files'] == summary['runs']['sanitized']['replayed_files'], 'Mode output divergence')
    require(reports['normal']['input_hashes'] == reports['sanitized']['input_hashes'], 'Different final inputs by mode')
    return summary


def resource_gates():
    footprint = load('footprint/summary.json'); benchmark = load('benchmark/verification.json')
    for report in [footprint, benchmark]:
        require(report['status'] == 'measured' and report['compiler_target'] == 'x86_64-linux-gnu', 'Resource measurement target')
        pins(report['source_hashes'])
        for command in report['compile_commands']:
            strict_command(command, False, '-std=c++17')
    require(footprint['totals'] == {'gnu_size_text_including_readonly_and_unwind_bytes': 22534, 'executable_text_bytes': 11842, 'readonly_data_bytes': 10388, 'data_bytes': 0, 'bss_bytes': 0}, 'Footprint totals')
    require(footprint['record_bytes']['EngineAnimationWorkspace49Q'] == 18432 and footprint['record_bytes']['ModelMotionWorkspace49Q'] == 9216 and footprint['caller_payload_total_bytes_excluding_assets_loader_and_frame_padding'] == 33004, 'Caller payload')
    require(sum(footprint['caller_payload_records'].values()) == 33004 and footprint['immutable_tables']['additional49Q_duplicate_payload_bytes'] == 10244, 'Footprint accounting')
    require(footprint['maximum_individual49Q_compiler_frame_bytes'] == 832, 'Compiler frame pin')
    require(benchmark['cpu_affinity'] == [0] and benchmark['normal_virtual_address_limit_bytes'] == 67108864 and benchmark['samples_per_mode'] == 15, 'Benchmark constraints')
    for row in benchmark['fixture']['members']:
        pins({row['path']: row})
    modes = {'both_engines_enabled', 'shared_enable_gate_disabled'}
    require(len(benchmark['results']) == 30 and set(r['mode'] for r in benchmark['results']) == modes, 'Benchmark samples')
    for mode in modes:
        rows = [r for r in benchmark['results'] if r['mode'] == mode]
        require(len(rows) == 15 and {r['sample'] for r in rows} == set(range(15)), 'Benchmark missing sample')
        values = []
        for row in rows:
            require(row['iterations'] == benchmark['iterations_per_sample_by_mode'][mode] and row['nanoseconds'] > 0, 'Benchmark iterations/time')
            require(row['nanoseconds_per_iteration'] == row['nanoseconds'] / row['iterations'], 'Benchmark normalization')
            require(row['workspace_bytes'] == 18432 and row['caller_payload_bytes'] == 33004 and row['input_bytes'] == 131048 and row['nodes'] == 31, 'Benchmark fixture size')
            values.append(row['nanoseconds_per_iteration'])
        require(statistics.median(values) == benchmark['statistics'][mode]['median_ns'], 'Benchmark median')
    require(len(benchmark['separate_untimed_ordinary_new_probes']) == 2 and all(r['cpp_new_allocations'] == 0 for r in benchmark['separate_untimed_ordinary_new_probes']), 'Separate ordinary-new probes')
    selection = load('benchmark/run-selection.json'); pins(selection['final_source_hashes_rechecked'])
    earlier = load('benchmark/earlier-run-01-verification.json')
    require(selection['selected_run'] == 'verification.json' and selection['same_production_hashes'] and selection['same_timed_executable_sha256'] and earlier['timed_executable_sha256'] == benchmark['timed_executable_sha256'], 'Benchmark run selection')


def baseline_gate(path):
    require(path.stat().st_size == 133996038 and sha(path) == BASELINE_SHA, 'Wrong verified baseline archive')
    changes = []
    with zipfile.ZipFile(path) as archive:
        names = [r.filename for r in archive.infolist() if not r.is_dir()]; safe_names(names)
        require(len(names) == 18454 and archive.testzip() is None, 'Baseline ZIP count/CRC')
        for name in names:
            current = ROOT.parent / name
            require(current.is_file(), 'Missing baseline file: ' + name)
            if sha(current) != digest(archive.read(name)): changes.append(name)
    require(set(changes) == ALLOWED, 'Unexpected changed baseline members: ' + repr(changes))
    return names, sorted(changes)


def gate_controls():
    """Semantic failures, not malformed-JSON errors; no source writes or reruns."""
    rows = []
    def control(name, original, mutate, gate):
        sample = copy.deepcopy(original); gate(sample); mutate(sample)
        try:
            gate(sample)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Gate control escaped: ' + name)
    for mode in ['normal', 'sanitized']:
        pair = [load(f'single-motion/{mode}/native-test-result.json'), load(f'single-motion/{mode}/coverage.json')]
        gate = lambda x, m=mode: sampler_gate(x[0], x[1], m)
        control(mode + '_focused_trace_off', pair, lambda x: x[1].update(ordered_store_tracing=False), gate)
        control(mode + '_missing_trace_include', pair, lambda x: x[0]['command'].remove('-include'), gate)
        control(mode + '_mutation_without_mismatch', pair, lambda x: x[0]['sensitivity'][0].update(diagnostic='process crashed'), gate)
        control(mode + '_unaccounted_source_word', pair, lambda x: x[1]['accounting'].update(unaccounted_words_or_branch_directions=1), gate)
    flow = [load('flow/validation.json'), load('flow/mutation-controls.json'), load('flow/coverage-accounting.json')]
    control('flow_scope_control_counted_as_detection', flow, lambda x: x[1]['mutations'][-1].update(scope_control=False), lambda x: flow_gate(*x))
    control('flow_mutation_without_actual_mismatch', flow, lambda x: x[1]['mutations'][0].update(actual_mismatch='invalid context'), lambda x: flow_gate(*x))
    review = [load('independent-review/review-manifest.json'), load('independent-review/results.json'), load('independent-review/controls.json')]
    control('independent_crash_not_mismatch', review, lambda x: x[2]['controls'][0].update(exit_code=-11), lambda x: independent_gate(*x))
    connected = [load('connected/summary.json'), load('connected/validation.json'), load('connected/control-driven-engine-frames49q.json'), load('connected/input-manifest.json')]
    control('connected_wrong_trace_hash', connected, lambda x: x[1]['runs']['normal']['repeats'][0].update(trace_sha256='0' * 64), lambda x: connected_gate(*x))
    control('connected_leak_sanitizer_claim', connected, lambda x: x[1].update(leak_sanitizer_used=True), lambda x: connected_gate(*x))
    artifact_pin = {'evidence/engineAnimation49Q/connected/exact-goldens49q.txt': GOLDEN_SHA}
    control('changed_exact_golden_pin', artifact_pin, lambda x: x.update({'evidence/engineAnimation49Q/connected/exact-goldens49q.txt': '0' * 64}), pins)
    for mode in ['normal', 'sanitized']:
        report = load(f'final-{mode}/regression-report.json'); gate = lambda x, m=mode: report_gate(x, m)
        control(mode + '_failed_returncode_hidden_by_status', report, lambda x: x['tests'][0]['run'].update(returncode=1), gate)
        control(mode + '_missing_suite', report, lambda x: x['tests'].pop(), gate)
        control(mode + '_fast_math_command', report, lambda x: x['build'][0]['command'].append('-ffast-math'), gate)
        control(mode + '_aggregate_trace_hook', report, lambda x: x['build'][0]['command'].extend(['-include', 'flag_trace.h']), gate)
        control(mode + '_changed_source_input', report, lambda x: x['changed_inputs'].append('src/game/engine_animation49q.cpp'), gate)
        control(mode + '_nonempty_stderr', report, lambda x: x['tests'][0]['run']['stderr'].update(bytes=1), gate)
    for name, bad in [('duplicate_zip_path', ['a', 'a']), ('zip_parent_traversal', ['../a']), ('zip_absolute_path', ['/a']), ('zip_windows_separator', ['a\\b'])]:
        try: safe_names(bad)
        except GateError as error: rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else: raise GateError('Archive path control escaped')
    return {'status': 'passed', 'method': 'Decoded-report copies mutated in memory; no production, proof, or source input changed; no numerical diagnostic rerun.',
            'controls': rows, 'control_count': len(rows), 'all_rejected': True}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--check-only', action='store_true', help='Read-only validation; do not write the final seal')
    parser.add_argument('--write-gate-controls', action='store_true', help='Write only the in-memory gate-control evidence')
    args = parser.parse_args()
    names, changes = baseline_gate(args.baseline)
    original_sources(); evidence_gates(); regression_gates(); resource_gates()
    controls = gate_controls()
    if args.write_gate_controls:
        (E / 'packaging-gate-controls.json').write_text(json.dumps(controls, indent=2) + '\n')
        print(json.dumps({'status': 'passed', 'gate_controls': controls['control_count']})); return
    require(load('packaging-gate-controls.json') == controls, 'Stale packaging gate controls')
    review = load('packaging-review.json')
    require(review['status'] == 'passed' and not review['blocking_findings'], 'Packaging review failed')
    pins(review['reviewed_file_hashes'])
    result = {'checkpoint': '49Q-EngineAnimation', 'status': 'passed',
              'verified_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
              'baseline_members': len(names), 'changed_baseline_members': changes,
              'regression_suites_each': 44, 'regression_checks_each': TOTAL_CHECKS,
              'tracked_inputs_each': 258, 'production_units_each': 38,
              'normal_sanitizer_outputs_identical': True, 'gate_controls_rejected': controls['control_count'],
              'focused_connected_checks': 4063640, 'aggregate_connected_checks': 3976867,
              'focused_sampler_checks': 5615735, 'aggregate_sampler_checks': 5526542,
              'file_output_replay_checks': {'sampler': 5526543, 'connected': 3976868},
              'focused_ordered_store_tracing': True, 'aggregate_ordered_store_tracing': False,
              '49c_status': 'excluded and unreleased', 'collision_contract': 'unresolved and unchanged',
              'limits': 'Bounded authentic Pod01 engine projection and supplied numeric controls. No complete visual system, spawn, Chariot IK, renderer, physical hardware, whole-game RAM, or full-frame performance claim.'}
    if not args.check_only:
        baseline_names = set(names)
        result['source_hashes'] = {p.relative_to(ROOT).as_posix(): sha(p) for p in sorted(ROOT.rglob('*'))
                                   if p.is_file() and p != E / 'validation.json' and
                                   ('RacerRevengePC/' + p.relative_to(ROOT).as_posix() not in baseline_names or
                                    'RacerRevengePC/' + p.relative_to(ROOT).as_posix() in ALLOWED)}
        (E / 'validation.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'source_hashes'}))


if __name__ == '__main__':
    main()
