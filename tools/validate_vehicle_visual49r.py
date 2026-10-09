#!/usr/bin/env python3
"""Validate frozen 49R proofs and the release seal without running diagnostics.

All gates remain active under Python -O. Controls alter decoded JSON in memory
only; neither production files nor numerical tests are mutated or rerun.
"""
import argparse
import copy
import datetime
import hashlib
import json
import re
import statistics
import struct
import sys
import zipfile
from collections import Counter
from pathlib import Path, PurePosixPath

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/vehicleVisual49R'
BASELINE_SHA = '9e218d3a35579ce5bdf9c7216ab43c083b26ad4152184b938d8e091d3a996b17'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
GOLDEN_SHA = '8fb432a675cbaa29e326045ba22abac1eb85fc84738d8ba7f05f8863db29c10a'
TRACE_SHA = '90ac2f9c277a2df98a9544e5456dc8d982f935347c090f3f85d324f75f102210'
PLAIN_TRACE_SHA = '162f3aa73b305a92ea0cc9743807fa87c26fccc60322c18524a47dc927d8be91'
CONNECTED_STDOUT_SHA = '50f5819601fcdebf9edb23e0c2fc38bcbed937cfc44fd90b2c0204231f8c795e'
TOTAL_CHECKS = 150183835
PLAIN_CHECKS = {'visual_container49r': 3678715, 'visual_response49r': 171,
    'vehicle_visual_authoring49r': 30618, 'vehicle_visual49r': 1991837,
    'vehicle_visual_diagnostic49r': 9489663}
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
ALLOWED = {'RacerRevengePC/CMakeLists.txt', 'RacerRevengePC/README.md'}
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}
NEW_UNITS = {'src/game/vehicle_visual49r.cpp', 'src/game/visual_container49r.cpp',
             'src/game/visual_response49r.cpp', 'src/assets/vehicle_visual_authoring49r.cpp'}
NEW_SUITES = {'visual_container49r', 'visual_response49r', 'vehicle_visual_authoring49r',
              'vehicle_visual49r', 'vehicle_visual_diagnostic49r'}
ADDED_INPUTS = {
    'cmake/NativeVehicleVisual49R.cmake', 'tools/test_native_vehicle_visual49r.py',
    'evidence/vehicleVisual49R/authoring/authoring_oracle49r.h',
    'evidence/vehicleVisual49R/authoring/authoring_words49r.h',
    'evidence/vehicleVisual49R/authoring/pod01_visual_fixture49r.h',
    'evidence/vehicleVisual49R/connected/exact-goldens49r.txt',
    'evidence/vehicleVisual49R/connected/input-manifest.json',
    'evidence/vehicleVisual49R/container/source/container_oracle.h',
    'evidence/vehicleVisual49R/container/source/container_words.h',
    'evidence/vehicleVisual49R/source/stage_trace.h',
    'evidence/vehicleVisual49R/source/visual_fixture.h',
    'evidence/vehicleVisual49R/source/visual_machine.h',
    'evidence/vehicleVisual49R/source/visual_words.h',
    'src/assets/vehicle_visual_authoring49r.h', 'src/game/vehicle_visual49r.h',
    'src/game/vehicle_visual49r_internal.h',
} | NEW_UNITS | {'tests/native/' + n + '.cpp' for n in NEW_SUITES}
CONTAINER_ACCOUNTING = {'frozen_words': 1048, 'reached_words': 626,
    'unreached_words_with_specific_reason': 422, 'reached_branch_pcs': 42,
    'both_directions': 23, 'single_direction_with_specific_reason': 19,
    'unknown_executed_pcs': 0, 'unaccounted_words_or_directions': 0}
AUTHORING_RESULT = {'passed': True, 'checks': 30619, 'native_calls': 475,
    'native_allocations': 0, 'binder_cases': 302, 'roundtrips': 1,
    'sensitivity_cases': 29, 'rejections': 139, 'world_getter_calls': 605,
    'original_instructions': 182649, 'original_covered_words': 500,
    'A54': '48a80dd5', 'A68': '4392aa7f', '7EC': '40a00000'}
COMPLETE_RESULT = {'cases': 810, 'checks': 2000747, 'native_calls': 810,
    'native_allocations': 0, 'original_instructions': 9020719, 'noise_calls': 8136,
    'rng_calls': 12537, 'normal_source_substitutions': 0, 'all_match': True}
CONNECTED_RESULT = {'passed': True, 'frames': 576,
    'serial_vehicles_sharing_one_noise_object': 6, 'graph_nodes': 52,
    'state3_4_base_skips': 24, 'full49R_calls': 552, 'moving_root_frames': 552,
    'moving_real_brake_fin_nodes': 3815, 'scene_calls': 3744,
    'later_container_invalidations': 1296, 'later_retained_engine_roots': 1296,
    'wrong_animation_clock_changed_frames': 550, 'wrong_current_as_smoothed_changed_frames': 311,
    'wrong_postphysics_order_changed_frames': 531, 'wrong_body_copy_changed_frames': 552,
    'wrong_stale_main_changed_frames': 534, 'source49R_instructions': 3282292,
    'source49Q_instructions': 6382059, 'source48W_instructions': 254663,
    'source49P_instructions': 408036, 'source_noise_calls': 6216,
    'source_rng_calls': 1791, 'source_substitutions': 0, 'native_hot_allocations': 0}


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


def safe_names(names):
    require(len(names) == len(set(names)), 'Duplicate archive paths')
    for name in names:
        p = PurePosixPath(name)
        require(name and not p.is_absolute() and '\\' not in name and ':' not in name and
                '\x00' not in name and not any(x in ('', '.', '..') for x in name.split('/')),
                'Unsafe archive path: ' + name)


def pins(rows):
    if isinstance(rows, list):
        require(len(rows) == len({r['path'] for r in rows}), 'Duplicate file pin')
        rows = {r['path']: r for r in rows}
    safe_names(list(rows))
    for name, value in rows.items():
        record = value if isinstance(value, dict) else {'sha256': value}
        path = ROOT / name
        require(path.is_file() and not path.is_symlink() and sha(path) == record['sha256'], 'File pin: ' + name)
        if 'bytes' in record:
            require(path.stat().st_size == record['bytes'], 'File size: ' + name)


def equal_fields(actual, expected, label):
    for key, value in expected.items():
        require(key in actual and type(actual[key]) is type(value) and actual[key] == value, label + ': ' + key)


def strict_command(command, sanitized, standard=None, traced=None):
    require(isinstance(command, list) and command, 'Missing command')
    require(not BAD_FLAGS.intersection(command), 'Unsafe floating-point/sanitizer flags')
    require(all(x in command for x in ['-fno-fast-math', '-ffp-contract=off']), 'Missing strict FP flags')
    if standard:
        require(standard in command, 'Wrong language standard')
    require(('-fsanitize=address,undefined' in command) is sanitized, 'Wrong sanitizer flags')
    if traced is False:
        require('-include' not in command, 'Aggregate trace instrumentation')
    elif isinstance(traced, str):
        require('-include' in command, 'Missing trace instrumentation')
        i = command.index('-include')
        require(i + 1 < len(command) and Path(command[i + 1]).name == traced, 'Wrong trace instrumentation')


def container_gate(report, coverage, mode):
    require(report['status'] == 'pass' and report['elf_sha256'] == ELF_SHA, 'Container proof/source')
    require(report['source_words_verified'] == 1048 and report['original_word_ranges_verified'] == 12,
            'Container word/range counts')
    require(report['native_hot_allocations'] == 0, 'Container allocations')
    require(report['mode'] == ('normal' if mode == 'normal' else 'ASan+UBSan; leak detection disabled'), 'Container mode')
    strict_command(report['command'], mode != 'normal', '-std=c++20', 'flag_trace.h')
    require(report['original_instruction_accounting'] == coverage['accounting'] == CONTAINER_ACCOUNTING,
            'Container accounting')
    equal_fields(coverage, {'fixtures': 406, 'operations': 1833, 'checks': 3696475,
        'instructions': 1221365, 'ordered_flag_store_tracing': True,
        'flag_stores_compared': 7963, 'hot_allocations': 0}, 'Container counters')
    rows = report['sensitivity']
    require(len(rows) == 21 and len({r['name'] for r in rows}) == 21, 'Container mutation count')
    require(Counter(r['kind'] for r in rows) == {'original-word substitution': 20, 'native arithmetic control': 1},
            'Container mutation split')
    for row in rows:
        require(row['detected'] is True and row['diagnostic'].startswith('FAIL Container49R:') and
                'mismatch' in row['diagnostic'], 'Container control lacks actual mismatch')


def authoring_gate(normal, sanitized, validation, coverage):
    for mode, report in [('normal', normal), ('sanitized', sanitized)]:
        equal_fields(report, AUTHORING_RESULT, 'Authoring result')
        strict_command(report['command_flags'], mode == 'sanitized', '-std=c++17')
    require(normal['sanitizers'] == 'none' and sanitized['sanitizers'] == 'ASan+UBSan; leak detection disabled',
            'Authoring sanitizer boundary')
    require(validation['passed'] is True and validation['normal_and_sanitized_counts_match'] is True and
            validation['native_allocations'] == 0 and validation['five_semantic_mutants_detected'] is True,
            'Authoring validation')
    require(len(normal['mutations']) == 5, 'Authoring control count')
    for row in normal['mutations'].values():
        require(row['detected'] is True and row['diagnostic'].startswith('vehicle_visual_authoring49r:'),
                'Authoring control lacks behavioral assertion')
    require(len(coverage['blocks']) == 12 and sum(sum(b['hits']) for b in coverage['blocks']) == 182649 and
            sum(sum(h > 0 for h in b['hits']) for b in coverage['blocks']) == 500, 'Authoring coverage totals')
    for b in coverage['blocks']:
        require(len(b['hits']) * 4 == b['end_exclusive'] - b['start'] and all(type(x) is int and x >= 0 for x in b['hits']),
                'Authoring coverage word count')


def response_gate(report):
    expected = {'suite': 'visual_response49r', 'cases': 37, 'checks': 171, 'hot_allocations': 0, 'status': 'pass'}
    require(report['status'] == 'pass' and report['normal'] == report['sanitizers']['result'] == expected,
            'Response modes/counters')
    require(report['sanitizers']['address'] is True and report['sanitizers']['undefined'] is True and
            report['sanitizers']['leak'] is False, 'Response sanitizer boundary')
    rows = report['semantic_controls']
    require(len(rows) == 10 and len({r['name'] for r in rows}) == 10, 'Response mutation count')
    for row in rows:
        require(row['status'] == 'detected' and row['assertion'].startswith('FAIL '), 'Response control assertion')
    witness = report['double_normalization_witness']
    require(witness == load('response/double-normalization-witness.json') and
            witness['persistent_words'] == ['3ec031a0', '3e52cb13', '3f5c17cc', '3e8e986f'] and
            len(witness['twice_normalized_matrix_words']) == 16, 'Response original normalization witness')


def complete_gate(report, controls, coverage):
    require(report['status'] == 'passed' and report['normal'] == report['sanitized'] == COMPLETE_RESULT,
            'Complete source proof modes/counters')
    require(report['source_elf_sha256'] == ELF_SHA and report['native_no_allocation'] is True and
            report['production_language'] == 'C++17' and report['test_language'] == 'C++20' and
            report['leak_sanitizer'] == 'disabled, not claimed', 'Complete proof source/language/boundary')
    require(all(x in report['flags'] for x in ['-fno-fast-math', '-ffp-contract=off', '-Werror']) and
            not BAD_FLAGS.intersection(report['flags']), 'Complete proof flags')
    require(report['mutations'] == len(controls['mutations']) == controls['actual_mismatches'] == 14 and
            controls['setup_failures_counted'] == 0 and controls['working_tree_production_mutated'] is False,
            'Complete proof control accounting')
    require(len({r['name'] for r in controls['mutations']}) == 14, 'Duplicate complete control')
    for row in controls['mutations']:
        require('mismatch' in row['actual_mismatch'] and 'case=' in row['actual_mismatch'], 'Complete control lacks state mismatch')
    equal_fields(coverage, {'frozen_words': 7017, 'executed_unique_words': 4746, 'dynamic_instructions': 9020719,
        'complete_root_unaccounted_words': 0, 'unknown_executed_words': 0,
        'uncovered_classes': {'excluded_or_unreachable': 2022, 'provenance_only': 41, 'unexercised_helper_branch': 208}},
        'Complete coverage accounting')


def independent_gate(review):
    result = {'checks': 160, 'native_calls': 24, 'native_allocations': 0, 'all_match': True}
    require(review['status'] == 'pass' and review['production_edits'] is False and
            review['normal'] == review['asan-ubsan-no-lsan'] == result, 'Independent proof modes/counters')
    require(review['sanitizers'] == 'ASan+UBSan, LeakSanitizer disabled' and
            review['source_audit']['elf_sha256'] == ELF_SHA and
            review['source_audit']['normal_native_expectation_substitutions'] == 0, 'Independent proof boundary')
    rows = review['mutations']
    require(len(rows) == 10 and len({r['name'] for r in rows}) == 10, 'Independent controls')
    for row in rows:
        require(row['status'] == 'detected_behavioral_assertion' and row['returncode'] == 1 and
                row['assertion'].startswith('FAIL '), 'Independent crash/setup failure counted as detection')


def connected_gate(report, manifest):
    require(report['passed'] is True and report['read_only_by_default'] is True and
            report['leak_sanitizer_used'] is False, 'Connected proof boundary')
    require(set(report['runs']) == {'normal', 'asan-ubsan', 'plain'}, 'Connected mode membership')
    for mode, run in report['runs'].items():
        strict_command(run['flags'], mode == 'asan-ubsan', '-std=c++20', False if mode == 'plain' else 'stage_trace.h')
        require(len(run['repeats']) == 3, 'Connected repeat count')
        for r in run['repeats']:
            equal_fields(r['summary'], CONNECTED_RESULT, 'Connected counters')
            require(r['summary']['checks'] == 9489664, 'Connected file-output check count')
            require(r['summary']['native_stage_hooks'] is (mode != 'plain'), 'Connected hook marker')
            require(r['trace_sha256'] == (PLAIN_TRACE_SHA if mode == 'plain' else TRACE_SHA) and
                    r['stdout_sha256'] == CONNECTED_STDOUT_SHA, 'Connected repeats diverged')
    expected = {'left_before_right', 'left_uses_right_clock', 'omit_absent_engine_clock950',
                'recompute_binder_after_right', 'skip_zero_amplitude_noise', 'wrong_root_W'}
    require(set(report['mutations']) == expected, 'Connected semantic controls')
    for name, r in report['mutations'].items():
        require(r['detected'] is True and r['diagnostic'].startswith('FAIL VehicleVisualDiagnostic49R') and
                (('mismatch' in r['diagnostic'] and 'case=' in r['diagnostic']) or
                 (name == 'left_before_right' and r['diagnostic'] ==
                  'FAIL VehicleVisualDiagnostic49R scenario=0 frame=0 stage=read: 49R source/native right-before-left order')),
                'Connected control lacks actual mismatch')
    require(manifest['frames'] == 576 and manifest['frames_per_vehicle'] == 96 and manifest['vehicles'] == 6,
            'Connected input shape')


def original_sources():
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    require(digest(elf) == ELF_SHA and elf[:6] == b'\x7fELF\x01\x01', 'Original ELF pin')
    offset = struct.unpack_from('<I', elf, 28)[0]
    size, count = struct.unpack_from('<HH', elf, 42)
    segments = [struct.unpack_from('<8I', elf, offset + i * size) for i in range(count)]
    def original(start, length):
        rows = [elf[o + start - v:o + start - v + length] for k, o, v, _, n, *_ in segments
                if k == 1 and v <= start and start + length <= v + n]
        require(len(rows) == 1 and len(rows[0]) == length, 'Original range not uniquely file-backed')
        return rows[0]
    manifests = [('source/source-manifest.json', 'source/visual_words.h'),
                 ('container/source/container-manifest.json', 'container/source/container_words.h'),
                 ('authoring/source-manifest.json', 'authoring/authoring_words49r.h')]
    maps = {}
    for manifest_name, header_name in manifests:
        manifest = load(manifest_name)
        require(manifest['elf_sha256'] == ELF_SHA, 'Manifest ELF pin')
        text = (E / header_name).read_text()
        arrays = dict(re.findall(r'\b(\w+)\[\]\s*=\s*\{([^}]+)\}', text, re.S))
        words = {}
        expected_bounds = []
        index = 0
        for r in manifest['blocks'] + manifest.get('data', []):
            start, end = int(r['start'], 16), int(r['end_exclusive'], 16)
            raw = original(start, end - start)
            require(digest(raw) == r['sha256'], 'Original range hash: ' + r['name'])
            if 'elf_offset' in r:
                off = int(r['elf_offset'], 16)
                require(elf[off:off + len(raw)] == raw, 'Original ELF offset')
            for sidecar in [E / Path(manifest_name).parent / (r['name'] + '.bin')]:
                if sidecar.exists():
                    require(sidecar.read_bytes() == raw, 'Original binary sidecar: ' + r['name'])
            if r in manifest['blocks']:
                require(len(raw) % 4 == 0 and len(raw) // 4 == r.get('words', r.get('word_count', len(raw) // 4)),
                        'Original word count')
                require(r['name'] in arrays, 'Compiled source array missing: ' + r['name'])
                packed = b''.join(struct.pack('<I', int(x, 16)) for x in re.findall(r'0x([0-9a-fA-F]+)u?', arrays[r['name']]))
                require(packed == raw, 'Compiled original words: ' + r['name'])
                expected_bounds.append((start, len(raw) // 4, index, r['name']))
                index += len(raw) // 4
                for i, word in enumerate(struct.unpack('<' + str(len(raw) // 4) + 'I', raw)):
                    require(start + 4 * i not in words, 'Overlapping source blocks')
                    words[start + 4 * i] = word
            elif r['name'] in arrays:
                require(bytes(int(x.strip()) for x in arrays[r['name']].split(',') if x.strip()) == raw,
                        'Compiled original data: ' + r['name'])
        if header_name == 'source/visual_words.h':
            bounds = [(int(start, 16), int(count), int(index), name) for label, start, name, count, index in
                re.findall(r'\{"(\w+)",0x([0-9a-fA-F]+)u,(\w+),(\d+),(\d+)\}', text) if label == name]
        else:
            bounds = [(int(start, 16), int(count), int(index), name) for start, count, index, name in
                re.findall(r'\{0x([0-9a-fA-F]+)u,(\d+)u?,(\d+)u?,(\w+)\}', text)]
        require(bounds == expected_bounds, 'Compiled original source address/count/index table')
        maps[manifest_name] = words
    require(len(maps[manifests[0][0]]) == 7017 and len(maps[manifests[1][0]]) == 1048,
            'Original source map totals')
    container = load('container/source/container-manifest.json')
    pins({r['path']: r for r in [container['reader_source'], container['sqrt_source']]})
    authoring = load('authoring/source-manifest.json')
    pins(authoring['assets'])
    phy = ROOT / 'evidence/authoredInitialization49N/authoring-source/Pod01.phy'
    require(sha(phy) == authoring['phy_sha256'], 'Authoring PHY provenance')
    raw_phy = phy.read_bytes()
    for r in authoring['records']:
        require(raw_phy[r['value_offset']:r['value_offset'] + r['value_bytes']].decode() == r['token'],
                'Authoring literal token provenance')
    # Recompute all coverage word membership and instruction/branch sums.
    for mode in ['normal', 'sanitizers']:
        c = load(f'container/{mode}/coverage.json'); rows = c['pcs']; expected = maps[manifests[1][0]]
        require(len(rows) == len(expected) and {r['pc'] for r in rows} == set(expected), 'Container coverage PC membership')
        require(sum(r['hits'] for r in rows) == 1221365 and sum(r['hits'] > 0 for r in rows) == 626,
                'Container dynamic coverage')
        for r in rows:
            require(r['word'] == expected[r['pc']] and type(r['hits']) is int and r['hits'] >= 0, 'Container coverage word/hit')
            if not r['hits']:
                require(bool(r.get('unreached_reason')), 'Unexplained container word')
            if r['taken'] or r['not_taken']:
                require(r['taken'] + r['not_taken'] == r['hits'], 'Container branch accounting')
                if not (r['taken'] and r['not_taken']):
                    require(bool(r.get('restricted_branch_reason')), 'Unexplained container branch')
    coverage = load('complete-proof/coverage-accounting.json')
    rows = [r for b in coverage['blocks'] for r in b['pcs']]; expected = maps[manifests[0][0]]
    raw_rows = [tuple(int(v, 16 if i == 0 else 10) for i, v in enumerate(line.split()))
                for line in (E / 'complete-proof/coverage.tsv').read_text().splitlines()]
    require(raw_rows == [(int(r['pc'], 16), r['hits'], r['taken'], r['not_taken']) for r in rows],
            'Complete raw coverage accounting')
    require(len(rows) == 7017 and {int(r['pc'], 16) for r in rows} == set(expected), 'Complete coverage PC membership')
    require(sum(r['hits'] for r in rows) == 9020719 and sum(r['hits'] > 0 for r in rows) == 4746,
            'Complete dynamic coverage')
    require(Counter(r['coverage_class'] for r in rows if not r['hits']) == coverage['uncovered_classes'],
            'Complete uncovered-class accounting')
    for r in rows:
        require(int(r['word'], 16) == expected[int(r['pc'], 16)] and type(r['hits']) is int and r['hits'] >= 0,
                'Complete coverage word/hit')
        if not r['hits']:
            require(bool(r.get('reason')), 'Unexplained complete source word')
        if r['taken'] or r['not_taken']:
            require(r['taken'] + r['not_taken'] == r['hits'], 'Complete branch accounting')
            require(r['branch_directions'] == ('both' if r['taken'] and r['not_taken'] else 'single'), 'Complete branch direction label')
            if not (r['taken'] and r['not_taken']):
                require(bool(r.get('single_direction_context')), 'Unexplained complete branch direction')


def evidence_gates():
    pins(load('assessment/preservation.json')['files'])
    for mode in ['normal', 'sanitizers']:
        report = load(f'container/{mode}/native-test-result.json')
        container_gate(report, load(f'container/{mode}/coverage.json'), mode)
        pins(report['file_sha256'])
        require((E / f'container/{mode}/visual_container49r.stdout').read_text().strip() == report['native_result'],
                'Container raw stdout')
        for name in ['visual_container49r.stderr', 'compile.stdout', 'compile.stderr', 'compile_direct_rsqrt.stdout', 'compile_direct_rsqrt.stderr']:
            require((E / f'container/{mode}' / name).read_bytes() == b'', 'Container raw build/stderr')
    av = load('authoring/validation.json')
    authoring_gate(load('authoring/normal-result.json'), load('authoring/sanitized-result.json'), av, load('authoring/coverage.json'))
    pins(av['files'])
    for mode in ['normal', 'sanitized']:
        require(load(f'authoring/{mode}.stdout') == AUTHORING_RESULT and
                (E / f'authoring/{mode}.stderr').read_bytes() == b'', 'Authoring raw output')
    response = load('response/validation.json'); response_gate(response); pins(response['inputs'])
    full = load('complete-proof/validation.json'); controls = load('complete-proof/mutation-controls.json')
    complete_gate(full, controls, load('complete-proof/coverage-accounting.json')); pins(full['files'])
    for mode, filename in [('normal', 'normal'), ('sanitized', 'asan-ubsan')]:
        require(load(f'complete-proof/{mode}.json') == full[mode] == load(f'complete-proof/{filename}.stdout'), 'Complete raw output')
        require((E / f'complete-proof/{filename}.stderr').read_bytes() == b'', 'Complete runtime stderr')
    readonly = load('complete-proof/read-only-default.json')
    require(readonly['default_run_passed'] is True and readonly['coverage_unchanged'] is True, 'Complete read-only default')
    for r in controls['mutations']:
        require(r['actual_mismatch'] in (E / 'complete-proof' / ('mutation-' + r['name'] + '.stderr')).read_text(),
                'Complete raw mutation diagnostic')
    review = load('independent-review/review-manifest.json'); independent_gate(review)
    pins(review['reviewed_source_hashes']); pins(review['test_source_hashes']); pins(load('independent-review/inherited-contract-hashes.json'))
    for mode in ['normal', 'asan-ubsan-no-lsan']:
        require(load(f'independent-review/{mode}.stdout') == review[mode], 'Independent raw output')
        for tail in ['.stderr', '-compile.stdout', '-compile.stderr']:
            require((E / ('independent-review/' + mode + tail)).read_bytes() == b'', 'Independent build/runtime stderr')
    for row in review['mutations']:
        require((E / ('independent-review/' + row['name'] + '.stderr')).read_text().strip() == row['assertion'],
                'Independent raw mutation assertion')
    connected = load('connected/verified/report.json'); manifest = load('connected/input-manifest.json')
    connected_gate(connected, manifest); pins(connected['source_sha256']); pins(manifest['input_sha256'])
    require(sha(E / 'connected/exact-goldens49r.txt') == GOLDEN_SHA, 'Connected exact golden pin')
    for mode, run in connected['runs'].items():
        for i, row in enumerate(run['repeats']):
            path = E / f'connected/verified/{mode}-trace-{i}.json'
            require(sha(path) == row['trace_sha256'], 'Connected raw trace pin')
            trace = json.loads(path.read_text())
            equal_fields(trace['summary'], CONNECTED_RESULT, 'Connected trace counters')
            require(trace['summary']['checks'] == 9489664 and trace['summary']['native_stage_hooks'] is (mode != 'plain'),
                    'Connected trace hook/check boundary')
            require(semantic_trace(trace) == semantic_trace(load('connected/verified/normal-trace-0.json')), 'Connected plain/focused semantic divergence')
            require(len(trace['frames']) == 576 and Counter(f['scenario'] for f in trace['frames']) == dict.fromkeys(range(6), 96),
                    'Connected frame membership')
            raw = E / f'connected/verified/{mode}-run-{i}.stdout.log'
            require(sha(raw) == row['stdout_sha256'] and check_count(raw.read_text()) == 9489664, 'Connected raw stdout pin/count')
            require((E / f'connected/verified/{mode}-run-{i}.stderr.log').read_bytes() == b'', 'Connected runtime stderr')
    for name, row in connected['mutations'].items():
        require((E / f'connected/verified/{name}-run.stderr.log').read_text().strip() == row['diagnostic'],
                'Connected raw mutation diagnostic')


def prior_report():
    return json.loads((ROOT / 'evidence/engineAnimation49Q/final-normal/regression-report.json').read_text())


def check_count(text):
    try:
        return int(json.loads(text)['checks'])
    except (ValueError, KeyError):
        match = re.search(r'checks=(\d+)', text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks', text)
        require(match is not None, 'Missing check count')
        return int(match.group(1))


def report_gate(report, mode):
    sanitized = mode == 'sanitized'
    flags = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-fno-fast-math', '-frounding-math', '-ffp-contract=off']
    flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-sanitize-recover=all'] if sanitized else ['-O2']
    previous = prior_report(); suites = set(previous['selected_tests']) | NEW_SUITES
    cmake = (ROOT / 'cmake/NativeVehicleVisual49R.cmake').read_text()
    registered = re.search(r'set\(rr_vehicle_visual_tests\s+([^)]*)\)', cmake)
    require(registered is not None and set(registered.group(1).split()) == suites, 'CMake/runner suite membership')
    require(report['status'] == 'passed' and not report['errors'] and not report['changed_inputs'], 'Regression status/inputs')
    require(report['scope'] == 'all_vehicle-visual_checks' and report['serial'] is True, 'Regression scope')
    require(report['compile_flags'] == flags and report['link_flags'] == (['-fsanitize=address,undefined'] if sanitized else []), 'Regression flags')
    require(report['sanitizers'] is sanitized and report['leak_detection'] is False, 'Regression sanitizer mode')
    require(len(report['selected_tests']) == 49 and set(report['selected_tests']) == suites and
            len(report['tests']) == 49 and {t['name'] for t in report['tests']} == suites, 'Regression suite membership')
    require(len(report['input_hashes']) == 283 and set(report['input_hashes']) == set(previous['input_hashes']) | ADDED_INPUTS,
            'Regression input membership')
    require(len(report['build']) == 42 and {r['source'] for r in report['build']} == {r['source'] for r in previous['build']} | NEW_UNITS,
            'Regression production units')
    for name, pin in previous['input_hashes'].items():
        if name != 'CMakeLists.txt':
            require(report['input_hashes'][name] == pin, 'Historical tracked input changed')
    if sanitized:
        require('detect_leaks=0' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['ASAN_OPTIONS'] and
                'halt_on_error=1' in report['environment']['UBSAN_OPTIONS'], 'Sanitizer environment')
    for build in report['build'] + [t['build'] for t in report['tests']]:
        require(build['status'] == 'passed' and build['returncode'] == 0, 'Regression build status/returncode')
        require(build['command'][1:1 + len(flags)] == flags, 'Regression process flags')
        strict_command(build['command'], sanitized, '-std=c++20', False)
        require(build['stdout']['bytes'] == 0 and build['stdout']['sha256'] == EMPTY_SHA, 'Regression build stdout')
        if build.get('source') in NEW_UNITS:
            require(build['stderr']['bytes'] == 0 and build['stderr']['sha256'] == EMPTY_SHA, 'New production compiler warning')
    for test in report['tests']:
        run = test['run']
        require(test['status'] == run['status'] == 'passed' and run['returncode'] == 0, 'Regression run failed')
        require(run['command'][0] == test['executable']['path'], 'Regression executable provenance')
        require(run['stderr']['bytes'] == 0 and run['stderr']['sha256'] == EMPTY_SHA, 'Regression runtime stderr')


def semantic_trace(trace):
    trace = copy.deepcopy(trace)
    del trace['summary']['checks']; del trace['summary']['native_stage_hooks']
    return trace


def regression_gates():
    summary = load('regression-summary.json')
    require(summary['status'] == 'passed' and summary['all_mode_outputs_identical'] is True and
            summary['semantic_trace_identical_to_focused'] is True and summary['production_units_each'] == 42 and
            summary['checks_each'] == TOTAL_CHECKS,
            'Aggregate summary status')
    reports = {}
    for mode in ['normal', 'sanitized']:
        row = summary['runs'][mode]; report = load(f'final-{mode}/regression-report.json'); reports[mode] = report
        report_gate(report, mode); pins(report['input_hashes'])
        require(row['suites'] == 49 and row['tracked_inputs'] == 283 and row['inputs_unchanged'] is True and
                row['checks'] == summary['checks_each'], 'Aggregate summary counts')
        require(sha(E / f'final-{mode}/regression-report.json') == row['report_sha256'], 'Aggregate report pin')
        require(len(row['tests']) == 49 and {r['name'] for r in row['tests']} == {r['name'] for r in report['tests']}, 'Summary tests')
        total = 0
        for test in row['tests']:
            raw = (E / f'final-{mode}' / (test['name'] + '.stdout')).read_bytes()
            actual = next(r for r in report['tests'] if r['name'] == test['name'])['run']
            require(digest(raw) == test['stdout_sha256'] == actual['stdout']['sha256'] and len(raw) == actual['stdout']['bytes'], 'Actual test stdout pin')
            checks = check_count(raw.decode()); total += checks
            require(checks == test['checks'] and test['stderr_bytes'] == 0, 'Actual test count/stderr')
            if test['name'] in PLAIN_CHECKS:
                require(checks == PLAIN_CHECKS[test['name']], 'Plain aggregate check count')
        require(total == summary['checks_each'], 'Total checks')
        prior = json.loads((ROOT / 'evidence/engineAnimation49Q/regression-summary.json').read_text())['runs']['normal']['tests']
        require([r for r in row['tests'] if r['name'] not in NEW_SUITES] == prior, 'Historical 44-suite outputs changed')
        require(set(row['replayed_files']) == {'container-coverage.json', 'visual-coverage.tsv', 'authoring-coverage.json', 'trace.json'}, 'Aggregate replay membership')
        for name, expected in row['replayed_files'].items():
            require(sha(E / f'final-{mode}' / name) == expected, 'Aggregate replay pin')
        for name in ['visual_container49r', 'vehicle_visual49r', 'vehicle_visual_authoring49r', 'vehicle_visual_diagnostic49r']:
            path = E / f'final-{mode}' / (name + '-output.stdout')
            require(path.read_bytes() == (E / 'final-normal' / path.name).read_bytes(), 'Replay stdout differs by mode')
            expected = PLAIN_CHECKS[name] + (0 if name == 'vehicle_visual49r' else 1)
            require(check_count(path.read_text()) == expected, 'Replay check count')
        coverage = load(f'final-{mode}/container-coverage.json'); focused = load('container/normal/coverage.json')
        require(coverage['ordered_flag_store_tracing'] is False and coverage['flag_stores_compared'] == 0, 'Aggregate container trace hook state')
        for key in ['ordered_flag_store_tracing', 'flag_stores_compared', 'checks']:
            del coverage[key]; del focused[key]
        # Focused collector adds human coverage reasons; compare raw executed
        # words and branch counts after independently validating those reasons.
        del focused['accounting']
        focused['pcs'] = [{k: r[k] for k in ['pc', 'word', 'hits', 'taken', 'not_taken']} for r in focused['pcs']]
        require(coverage == focused, 'Aggregate container original coverage changed')
        require(load(f'final-{mode}/authoring-coverage.json') == load('authoring/coverage.json'), 'Aggregate authoring coverage changed')
        require((E / f'final-{mode}/visual-coverage.tsv').read_bytes() == (E / 'complete-proof/coverage.tsv').read_bytes(), 'Aggregate complete original coverage changed')
        trace = load(f'final-{mode}/trace.json'); focused = load('connected/verified/normal-trace-0.json')
        require(trace['summary']['native_stage_hooks'] is False and focused['summary']['native_stage_hooks'] is True and
                summary['trace_checks'] == {'aggregate': trace['summary']['checks'], 'focused': focused['summary']['checks']} and
                trace['summary']['checks'] == focused['summary']['checks'] == 9489664, 'Aggregate connected hook/check distinction')
        require(semantic_trace(trace) == semantic_trace(focused), 'Aggregate/focused semantic trace mismatch')
        processes = report['build'] + [x[k] for x in report['tests'] for k in ['build', 'run']]
        require(len(row['process_logs']) == 280, 'Missing durable process logs')
        names = []
        for process in processes:
            for stream in ['stdout', 'stderr']:
                r = process[stream]; path = E / f'final-{mode}/logs' / Path(r['path']).name
                require(path.is_file() and sha(path) == r['sha256'] and path.stat().st_size == r['bytes'], 'Preserved process log mismatch')
                require(row['process_logs'][path.name] == {'bytes': r['bytes'], 'sha256': r['sha256']}, 'Process log summary pin')
                names.append(path.name)
        require(len(names) == len(set(names)) and set(names) == set(row['process_logs']), 'Process log membership')
    require(summary['runs']['normal']['tests'] == summary['runs']['sanitized']['tests'] and
            summary['runs']['normal']['replayed_files'] == summary['runs']['sanitized']['replayed_files'] and
            reports['normal']['input_hashes'] == reports['sanitized']['input_hashes'], 'Aggregate mode divergence')
    return summary


def resource_gate(footprint, benchmark):
    for report in [footprint, benchmark]:
        require(report['status'] == 'measured' and report['compiler_target'] == 'x86_64-linux-gnu', 'Resource measurement target')
        for command in report['compile_commands']:
            strict_command(command, False, '-std=c++17', False)
    require(footprint['totals'] == {'gnu_size_text_including_readonly_and_unwind_bytes': 36665,
        'executable_text_bytes': 34751, 'readonly_data_bytes': 818, 'data_bytes': 648, 'bss_bytes': 0}, 'Footprint totals')
    for group, key in [('incremental49R_units', 'totals'), ('runtime49R_units', 'runtime49R_totals'), ('authoring49R_units', 'authoring49R_totals')]:
        require(all(sum(footprint['objects'][n][k] for n in footprint[group]) == v for k, v in footprint[key].items()), 'Footprint unit accounting')
    require(footprint['workspace_total_bytes'] == sum(footprint['workspace_composition'].values()) == 11548 and
            footprint['record_bytes']['VehicleVisualWorkspace49R'] == 11548 and
            footprint['caller_payload_total_bytes_excluding_assets_loader_and_frame_padding'] == sum(footprint['caller_payload_records'].values()) == 18804,
            'Caller/workspace payload accounting')
    require(footprint['maximum_individual49R_compiler_frame_bytes'] == 4112 and footprint['authoring_reader_compiler_frame_bytes'] == 4112,
            'Compiler frame bound')
    for name in footprint['incremental49R_units']:
        require(not footprint['objects'][name]['direct_allocator_references'], 'New object allocator reference')
    require(benchmark['cpu_affinity'] == [0] and benchmark['normal_virtual_address_limit_bytes'] == 67108864 and
            benchmark['samples_per_mode'] == 15, 'Benchmark constraints')
    modes = {'warm_noise_zero_dt', 'noise_disabled_zero_dt', 'warm_noise_bounded_steps',
             'noise_disabled_bounded_steps', 'cold_noise_first_call', 'cold_disabled_first_call'}
    require(len(benchmark['results']) == 90 and set(r['mode'] for r in benchmark['results']) == modes and
            set(benchmark['statistics']) == modes and len(benchmark['calibration']) == 6, 'Benchmark sample membership')
    for mode in modes:
        rows = [r for r in benchmark['results'] if r['mode'] == mode]
        require(len(rows) == 15 and {r['sample'] for r in rows} == set(range(15)), 'Benchmark missing sample')
        values = []
        for row in rows:
            require(row['iterations'] == benchmark['iterations_per_sample_by_mode'][mode] and row['nanoseconds'] > 0 and
                    row['nanoseconds_per_iteration'] == row['nanoseconds'] / row['iterations'], 'Benchmark iteration/time')
            require(row['workspace_bytes'] == 11548 and row['caller_payload_bytes'] == 18804 and row['nodes'] == 32 and
                    row['retained_asset_bytes'] == 126760 and row['hot_required_asset_bytes'] == 0, 'Benchmark fixture accounting')
            enabled = mode in {'warm_noise_zero_dt', 'warm_noise_bounded_steps', 'cold_noise_first_call'}
            require(row['expected_noise_calls_per_iteration'] == (12 if enabled else 0), 'Benchmark noise path')
            if mode.endswith('zero_dt'):
                require(row['frame_factor_bits'] == 0 and row['warmup_iterations'] == 64 and
                        row['phase_before_bits'] == row['phase_after_bits'] and row['rng_before'] == row['rng_after'] and
                        row['result_checksum'] == row['initial_checksum'], 'Zero-dt benchmark persistence')
            elif mode.startswith('cold_'):
                require(row['iterations'] == 1 and row['warmup_iterations'] == 0 and
                        row['initial_checksum'] == row['constructor_checksum'] and row['phase_before_bits'] == 0,
                        'Cold benchmark not fresh')
            else:
                require(row['iterations'] == 240 and row['warmup_iterations'] == 64 and row['frame_factor_bits'] == 1015580809 and
                        row['phase_before_bits'] != row['phase_after_bits'], 'Bounded advancing benchmark')
            values.append(row['nanoseconds_per_iteration'])
        stats = benchmark['statistics'][mode]
        require(statistics.median(values) == stats['median_ns'] and min(values) == stats['minimum_ns'] and
                max(values) == stats['maximum_ns'] and statistics.mean(values) == stats['mean_ns'], 'Benchmark summary statistics')
    probes = benchmark['separate_untimed_ordinary_new_probes']
    require(len(probes) == 6 and {r['mode'] for r in probes} == modes and
            all(r['cpp_new_allocations'] == 0 and r['nanoseconds'] == 0 for r in probes), 'Separate ordinary-new probes')
    for row in benchmark['reused_verified_footprint_objects']:
        require(footprint['objects'][row['unit']]['object_sha256'] == row['sha256'], 'Benchmark reused object provenance')


def resource_gates():
    footprint = load('footprint/summary.json'); benchmark = load('benchmark/verification.json')
    resource_gate(footprint, benchmark); pins(footprint['source_hashes']); pins(benchmark['source_hashes'])
    pins(benchmark['fixture']['members'])
    record_rows = [line.split() for line in (E / 'footprint/record-sizes.stdout').read_text().splitlines()]
    require({name: int(size) for name, size, align in record_rows} == footprint['record_bytes'] and
            {name: int(align) for name, size, align in record_rows} == footprint['record_alignments'],
            'Raw host record sizes/alignments')
    for unit, record in footprint['objects'].items():
        source = ROOT / ('src/' + unit + '.cpp')
        if source.exists():
            require(sha(source) == record['source_sha256'], 'Object source pin')
    for file, key in [('samples.stdout', 'results'), ('calibration.stdout', 'calibration'), ('ordinary-new-probe.stdout', 'separate_untimed_ordinary_new_probes')]:
        rows = []
        for line in (E / 'benchmark' / file).read_text().splitlines():
            fields = [token.split('=', 1) for token in line.split()]
            require(all(len(p) == 2 for p in fields) and len({p[0] for p in fields}) == len(fields), 'Malformed benchmark raw row')
            rows.append({k: v if k == 'mode' else int(v) for k, v in fields})
        require(len(rows) == len(benchmark[key]), 'Benchmark raw row count')
        for raw, record in zip(rows, benchmark[key]):
            equal_fields(record, raw, 'Benchmark raw sample')


def baseline_gate(path):
    require(path.stat().st_size == 136523547 and sha(path) == BASELINE_SHA, 'Wrong verified baseline archive')
    changes = []
    with zipfile.ZipFile(path) as archive:
        names = [r.filename for r in archive.infolist() if not r.is_dir()]
        safe_names(names)
        require(len(names) == 19386 and archive.testzip() is None, 'Baseline ZIP count/CRC')
        for name in names:
            current = ROOT.parent / name
            require(current.is_file() and not current.is_symlink(), 'Missing baseline file: ' + name)
            if sha(current) != digest(archive.read(name)):
                changes.append(name)
    require(set(changes) == ALLOWED, 'Unexpected changed baseline members: ' + repr(changes))
    return names, sorted(changes)


def gate_controls(include_regression=True):
    """Negative semantic gates; expected GateError only, not incidental exceptions."""
    rows = []
    def control(name, original, mutate, gate):
        sample = copy.deepcopy(original); gate(sample); mutate(sample)
        try:
            gate(sample)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Gate control escaped: ' + name)
    for mode in ['normal', 'sanitizers']:
        data = [load(f'container/{mode}/native-test-result.json'), load(f'container/{mode}/coverage.json')]
        gate = lambda x, m=mode: container_gate(*x, m)
        control(mode + '_container_hook_off', data, lambda x: x[1].update(ordered_flag_store_tracing=False), gate)
        control(mode + '_container_missing_trace_include', data, lambda x: x[0]['command'].remove('-include'), gate)
        control(mode + '_container_false_detection', data, lambda x: x[0]['sensitivity'][0].update(diagnostic='process crashed'), gate)
        control(mode + '_container_unaccounted_word', data, lambda x: x[1]['accounting'].update(unaccounted_words_or_directions=1), gate)
    authoring = [load('authoring/normal-result.json'), load('authoring/sanitized-result.json'), load('authoring/validation.json'), load('authoring/coverage.json')]
    control('authoring_wrong_scalar_bits', authoring, lambda x: x[0].update(A54='00000000'), lambda x: authoring_gate(*x))
    control('authoring_mutant_not_detected', authoring, lambda x: next(iter(x[0]['mutations'].values())).update(detected=False), lambda x: authoring_gate(*x))
    response = load('response/validation.json')
    control('response_leak_claim', response, lambda x: x['sanitizers'].update(leak=True), response_gate)
    control('response_missing_control', response, lambda x: x['semantic_controls'].pop(), response_gate)
    full = [load('complete-proof/validation.json'), load('complete-proof/mutation-controls.json'), load('complete-proof/coverage-accounting.json')]
    control('full_native_expected_substitution', full, lambda x: x[0]['normal'].update(normal_source_substitutions=1), lambda x: complete_gate(*x))
    control('full_setup_failure_counted', full, lambda x: x[1].update(setup_failures_counted=1), lambda x: complete_gate(*x))
    control('full_control_without_mismatch', full, lambda x: x[1]['mutations'][0].update(actual_mismatch='invalid context'), lambda x: complete_gate(*x))
    control('full_unaccounted_word', full, lambda x: x[2].update(complete_root_unaccounted_words=1), lambda x: complete_gate(*x))
    review = load('independent-review/review-manifest.json')
    control('independent_crash_not_assertion', review, lambda x: x['mutations'][0].update(returncode=-11), independent_gate)
    control('independent_production_edited', review, lambda x: x.update(production_edits=True), independent_gate)
    connected = [load('connected/verified/report.json'), load('connected/input-manifest.json')]
    control('connected_false_detection', connected, lambda x: next(iter(x[0]['mutations'].values())).update(diagnostic='process crashed'), lambda x: connected_gate(*x))
    control('connected_missing_repeat', connected, lambda x: x[0]['runs']['normal']['repeats'].pop(), lambda x: connected_gate(*x))
    control('connected_rng_reset', connected, lambda x: x[0]['runs']['normal']['repeats'][0]['summary'].update(source_rng_calls=3582), lambda x: connected_gate(*x))
    control('connected_leak_claim', connected, lambda x: x[0].update(leak_sanitizer_used=True), lambda x: connected_gate(*x))
    resources = [load('footprint/summary.json'), load('benchmark/verification.json')]
    control('resource_workspace_double_count', resources, lambda x: x[0].update(workspace_total_bytes=23096), lambda x: resource_gate(*x))
    control('benchmark_missing_sample', resources, lambda x: x[1]['results'].pop(), lambda x: resource_gate(*x))
    control('benchmark_wrong_median', resources, lambda x: x[1]['statistics']['warm_noise_zero_dt'].update(median_ns=0), lambda x: resource_gate(*x))
    control('benchmark_allocation', resources, lambda x: x[1]['separate_untimed_ordinary_new_probes'][0].update(cpp_new_allocations=1), lambda x: resource_gate(*x))
    artifact = {'evidence/vehicleVisual49R/connected/exact-goldens49r.txt': sha(E / 'connected/exact-goldens49r.txt')}
    control('changed_exact_golden_pin', artifact, lambda x: x.update({next(iter(x)): '0' * 64}), pins)
    if include_regression:
        for mode in ['normal', 'sanitized']:
            report = load(f'final-{mode}/regression-report.json'); gate = lambda x, m=mode: report_gate(x, m)
            control(mode + '_failed_returncode_hidden_by_status', report, lambda x: x['tests'][0]['run'].update(returncode=1), gate)
            control(mode + '_missing_suite', report, lambda x: x['tests'].pop(), gate)
            control(mode + '_fast_math_command', report, lambda x: x['build'][0]['command'].append('-ffast-math'), gate)
            control(mode + '_aggregate_trace_hook', report, lambda x: x['build'][0]['command'].extend(['-include', 'stage_trace.h']), gate)
            control(mode + '_changed_source_input', report, lambda x: x['changed_inputs'].append('src/game/vehicle_visual49r.cpp'), gate)
            control(mode + '_nonempty_stderr', report, lambda x: x['tests'][0]['run']['stderr'].update(bytes=1), gate)
    for name, bad in [('duplicate_zip_path', ['a', 'a']), ('zip_parent_traversal', ['../a']), ('zip_absolute_path', ['/a']), ('zip_windows_separator', ['a\\b'])]:
        try:
            safe_names(bad)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Archive path control escaped')
    return {'status': 'passed', 'method': 'Decoded reports mutated in memory only. No production writes or numerical diagnostic reruns.',
            'controls': rows, 'control_count': len(rows), 'all_rejected': True}


def seal_hashes(baseline_names):
    baseline_names = set(baseline_names)
    result = {}
    for p in sorted(ROOT.rglob('*')):
        if not p.is_file() or p == E / 'validation.json':
            continue
        name = p.relative_to(ROOT).as_posix(); archive_name = ROOT.name + '/' + name
        if archive_name not in baseline_names or archive_name in ALLOWED:
            require(not p.is_symlink(), 'New symlink cannot be sealed')
            result[name] = sha(p)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--focused-only', action='store_true', help='Read-only focused/source/resource gates; aggregate may not exist yet')
    parser.add_argument('--check-only', action='store_true', help='Require and verify the existing release seal without writing')
    parser.add_argument('--write-gate-controls', action='store_true', help='Write only release-review/gate-controls.json; no final seal')
    args = parser.parse_args()
    if args.focused_only and (args.check_only or args.write_gate_controls):
        parser.error('--focused-only cannot write controls or check a final seal')
    if not args.focused_only and not args.baseline:
        parser.error('--baseline is required for final validation')
    original_sources(); evidence_gates(); resource_gates()
    if args.focused_only:
        controls = gate_controls(False)
        print(json.dumps({'status': 'passed', 'scope': 'focused/source/resource gates', 'gate_controls': controls['control_count']}))
        return
    names, changes = baseline_gate(args.baseline)
    summary = regression_gates(); controls = gate_controls()
    if args.write_gate_controls:
        (E / 'release-review').mkdir(exist_ok=True)
        (E / 'release-review/gate-controls.json').write_text(json.dumps(controls, indent=2) + '\n')
        print(json.dumps({'status': 'passed', 'gate_controls': controls['control_count']}))
        return
    require(load('release-review/gate-controls.json') == controls, 'Stale release gate controls')
    review = load('release-review/review.json')
    require(review['status'] == 'passed' and not review['blocking_findings'], 'Release review failed')
    pins(review['reviewed_file_hashes'])
    result = {'checkpoint': '49R-EngineRootResponse', 'status': 'passed',
        'baseline_members': len(names), 'changed_baseline_members': changes,
        'regression_suites_each': 49, 'regression_checks_each': summary['checks_each'],
        'tracked_inputs_each': 283, 'production_units_each': 42,
        'normal_sanitizer_outputs_identical': True, 'gate_controls_rejected': controls['control_count'],
        'focused_container_checks': 3696475, 'focused_complete_checks': 2000747,
        'authoring_checks': 30619, 'response_semantic_checks': 171, 'independent_review_checks': 160,
        'connected_trace_checks': summary['trace_checks'],
        'focused_ordered_store_tracing': True, 'aggregate_ordered_store_tracing': False,
        '49c_status': 'excluded and unreleased', 'collision_contract': 'unresolved and unchanged',
        'limits': 'Bounded normal two-engine original21C120 return with explicit B54=0, supplied stable phase inputs, serial shared noise/RNG and authored Pod01 parameters. No complete Create/spawn/FrameReady, Chariot IK, physical wrapper widening, renderer, hardware, whole-game memory or frame-rate claim.',
        'source_hashes': seal_hashes(names)}
    if args.check_only:
        existing = load('validation.json')
        require({k: v for k, v in existing.items() if k != 'verified_utc'} == result, 'Release seal differs from revalidated evidence')
        require(isinstance(existing.get('verified_utc'), str), 'Release seal missing time')
    else:
        result['verified_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        (E / 'validation.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'source_hashes'}))


if __name__ == '__main__':
    main()
