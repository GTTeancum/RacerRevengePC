#!/usr/bin/env python3
"""Read-only evidence gates and explicit release sealing for bounded 49S.

No C++ diagnostic is run here. Negative controls change decoded values in memory
only. Gates use explicit exceptions and remain active under Python -O.
"""
import argparse
import copy
import datetime
import hashlib
import json
import math
from pathlib import Path, PurePosixPath
import re
import statistics
import struct
import sys
import zipfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/modelPalette49S'
BASELINE_SHA = '2c2b646cc273e048bc3ac49677cb5cd6ad92ad0e1c6d52f4b7543af8d187fc45'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
ALLOWED = {'RacerRevengePC/CMakeLists.txt', 'RacerRevengePC/README.md'}
NEW_UNITS = {'src/game/model_palette49s.cpp', 'src/game/rigid_vertices49s.cpp'}
REUSED_UNITS = {'src/assets/scene_asset48d.cpp', 'src/assets/native_asset48b.cpp'}
NEW_SUITES = {'model_palette49s', 'rigid_vertices49s', 'animated_export49s'}
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}
FRAME_KEYS = {(0, 'full-motion'), (31, 'full-motion'), (63, 'full-motion'),
              (95, 'full-motion'), (95, 'frozen-nodes'), (95, 'frozen-roots')}
PALETTE_COUNTS = {'fixtures': 542, 'operations': 1077, 'checks': 2575914,
    'instructions': 1359068, 'getter_calls': 21060, 'ordered_trace': True,
    'flag_stores': 10860, 'hot_allocations': 0}
PALETTE_ACCOUNTING = {'source_words': 178, 'reached_words': 174, 'excluded_words': 4,
    'reached_branch_pcs': 12, 'both_directions': 8, 'single_direction_with_reason': 4,
    'unaccounted_words_or_branches': 0}
RIGID_COUNTS = {'passed': True, 'checks': 371680, 'calls': 38, 'fixture_cases': 12,
    'fixture_vertices': 8619, 'input_record_bytes': 32, 'output_record_bytes': 12}
CONNECTED_COUNTS = {'49Q_calls': 96, '49R_complete_calls': 96, 'checks': 1110835,
    'engine_palette_entries': 28, 'frames': 96, 'graph_nodes': 52,
    'later_physics_pose_copies': 0, 'moving_brake_fin_nodes': 850, 'moving_root_frames': 96,
    'native_chariot_animation_ik_calls': 0, 'native_hot_allocations': 0,
    'palette_calls': 196, 'passed': True, 'rigid_calls': 12,
    'source48W_instructions': 45744, 'source49Q_instructions': 1160795,
    'source49Q_samples_including_chariot_brake_prefix': 384, 'source49R_instructions': 1224864,
    'source_noise_calls': 1152, 'source_palette_instructions': 250112,
    'source_rng_calls': 1791, 'source_substitutions': 0}
CONSUMER_CONTROLS = {'wrong_bind_baked_input': 11484, 'wrong_double_root': 11484,
    'wrong_float32_semantic_input': 1, 'wrong_palette_swap': 11484,
    'wrong_whole_engine_transform': 3904}
REVIEW_COUNTS = {'passed': True, 'checks': 23752, 'palette_calls': 13,
    'vertex_calls': 40, 'fixture_cases': 7, 'fixture_vertices': 3904}


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


def equal_fields(actual, expected, label):
    for key, value in expected.items():
        require(key in actual and type(actual[key]) is type(value) and actual[key] == value, label + ': ' + key)


def safe_names(names):
    require(len(names) == len(set(names)), 'Duplicate paths')
    for name in names:
        require(isinstance(name, str) and name and not PurePosixPath(name).is_absolute() and
            '\\' not in name and ':' not in name and '\x00' not in name and
            not any(x in ('', '.', '..') for x in name.split('/')), 'Unsafe path: ' + repr(name))


def current_path(name):
    """Relocate recorded absolute paths only by their explicit project-root suffix."""
    name = str(name)
    if Path(name).is_absolute():
        marker = '/RacerRevengePC/'
        require(name.count(marker) == 1, 'Absolute pin lacks unique repository root')
        name = name.split(marker, 1)[1]
    safe_names([name])
    path = ROOT / name
    require(path.is_file() and not path.is_symlink(), 'Missing regular input: ' + name)
    require(path.resolve().is_relative_to(ROOT.resolve()), 'Input resolves outside repository')
    return path


def pins(rows):
    if isinstance(rows, list):
        require(len(rows) == len({r['path'] for r in rows}), 'Duplicate file pin')
        rows = {r['path']: r for r in rows}
    require(isinstance(rows, dict) and bool(rows), 'Missing file pins')
    for name, value in rows.items():
        record = value if isinstance(value, dict) else {'sha256': value}
        path = current_path(name)
        require(sha(path) == record['sha256'], 'File pin: ' + name)
        if 'bytes' in record:
            require(path.stat().st_size == record['bytes'], 'File size: ' + name)


def strict_command(command, sanitized, standard, traced=None):
    require(isinstance(command, list) and command, 'Missing compile command')
    require(not BAD_FLAGS.intersection(command), 'Unsafe floating-point/sanitizer flags')
    require('-fno-fast-math' in command and '-ffp-contract=off' in command and standard in command,
            'Missing strict FP/language flags')
    require(('-fsanitize=address,undefined' in command) is sanitized, 'Wrong sanitizer flags')
    if traced is False:
        require('-include' not in command and not any('ENABLE_STAGE_TRACE' in x for x in command), 'Unexpected aggregate tracing')
    elif isinstance(traced, str):
        require('-include' in command, 'Missing ordered trace include')
        index = command.index('-include')
        require(index + 1 < len(command) and Path(command[index + 1]).name == traced, 'Wrong ordered trace include')


def original_sources():
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    require(digest(elf) == ELF_SHA and elf[:6] == b'\x7fELF\x01\x01', 'Original ELF pin')
    off = struct.unpack_from('<I', elf, 28)[0]
    size, count = struct.unpack_from('<HH', elf, 42)
    segments = [struct.unpack_from('<8I', elf, off + i * size) for i in range(count)]
    def original(start, length):
        found = [elf[o + start - v:o + start - v + length] for k, o, v, _, n, *_ in segments
                 if k == 1 and v <= start and start + length <= v + n]
        require(len(found) == 1 and len(found[0]) == length, 'Original range is not uniquely file-backed')
        return found[0]
    manifest = load('source/source-manifest.json')
    require(manifest['elf_sha256'] == ELF_SHA and len(manifest['blocks']) == 4 and manifest['word_count'] == 178,
            'Original selected source manifest')
    pins([manifest['oracle_parent']])
    source_check = load('proof/source-check.json')
    require(source_check['blocks'] == manifest['blocks'] and source_check['elf_sha256'] == ELF_SHA,
            'Original source-check mismatch')
    header = (E / 'source/palette_words49s.h').read_text()
    arrays = dict(re.findall(r'\b(\w+)\[\]\s*=\s*\{([^}]+)\}', header, re.S))
    expected_bounds, words, index = [], {}, 0
    for row in manifest['blocks']:
        start, end = int(row['start'], 16), int(row['end_exclusive'], 16)
        raw = original(start, end - start)
        require(len(raw) == row['words'] * 4 and digest(raw) == row['sha256'] and row['index'] == index,
                'Original selected range hash/size/index')
        offset = int(row['elf_offset'], 16)
        require(elf[offset:offset + len(raw)] == raw, 'Original selected offset')
        require((E / 'source' / (row['name'] + '.bin')).read_bytes() == raw, 'Selected source sidecar')
        packed = b''.join(struct.pack('<I', int(x, 16)) for x in re.findall(r'0x([0-9a-fA-F]+)u?', arrays[row['name']]))
        require(packed == raw, 'Compiled original words: ' + row['name'])
        expected_bounds.append((row['name'], start, row['name'], row['words'], index))
        index += row['words']
        for i, word in enumerate(struct.unpack('<' + str(row['words']) + 'I', raw)):
            require(start + i * 4 not in words, 'Overlapping original words')
            words[start + i * 4] = word
    bounds = [(a, int(b, 16), c, len(re.findall(r'0x[0-9a-fA-F]+u', arrays[c])), int(e)) for a, b, c, d, e in
        re.findall(r'\{"(\w+)",0x([0-9a-fA-F]+)u,(\w+),sizeof\((\w+)\)/4,(\d+)\}', header) if c == d]
    require(bounds == expected_bounds, 'Compiled source address table')
    assessment = load('assessment/source-manifest.json')
    require(assessment['elf_sha256'] == ELF_SHA, 'Assessment ELF pin')
    for name, row in assessment['blocks'].items():
        raw = original(int(row['start'], 16), row['words'] * 4)
        require(digest(raw) == row['sha256'] and (E / 'assessment' / (name + '.bin')).read_bytes() == raw,
                'Assessment original range: ' + name)
    for row in assessment['vtable_slots']:
        require(struct.unpack('<I', original(int(row['address'], 16), 4))[0] == int(row['target'], 16),
                'Original vtable dispatch target')
    return words


def cache_gate():
    report = load('cache-assessment/cache-assessment.json')
    pins(report['input_hashes'])
    pins([report['resource']])
    require(len(report['parts']) == 3 and {r['setup_field'] for r in report['parts']} == {'LeftEngine', 'RightEngine', 'Chariot'},
            'Cache admission part membership')
    for row in report['parts']:
        require(row['verified_resource_member_bytes_equal'] is True and row['all_cached_semantic_values_equal_original'] is True and
                row['neutral_export_byte_equal'] is True and row['world_space'] == 0 and row['lods'] == 4,
                'Cache semantic/source admission')
        pins({row['source_fixture']: {'sha256': row['source_sha256'], 'bytes': row['source_bytes']},
              row['scene_path']: {'sha256': row['scene_sha256'], 'bytes': row['scene_bytes']}})
        palette = row['matrix_palette']; source = current_path(row['source_fixture']).read_bytes()
        require(source[palette['offset']:palette['offset'] + len(bytes.fromhex(row['raw_palette_hex']))] ==
                bytes.fromhex(row['raw_palette_hex']), 'Original PSG palette bytes')
        require(palette['ids'] == list(range(palette['node_count'])), 'Selected source palette identity')
    preserved = load('cache-assessment/preservation-manifest.json')
    require(preserved['historical49R_research_preserved_byte_identical'] is True, 'Preserved research marker')
    pins({r['preserved_path']: r for r in preserved['files'].values()})


def palette_gate(report, coverage, mode):
    require(report['status'] == 'pass' and report['elf_sha256'] == ELF_SHA and report['source_words'] == 178, 'Palette proof source/status')
    require(report['mode'] == ('normal' if mode == 'normal' else 'ASan+UBSan; LeakSanitizer disabled'), 'Palette mode label')
    strict_command(report['command'], mode == 'sanitized', '-std=c++17', 'trace49s.h')
    equal_fields(coverage, PALETTE_COUNTS, 'Palette counters')
    require(report['coverage'] == coverage and coverage['accounting'] == PALETTE_ACCOUNTING, 'Palette coverage accounting')
    rows = coverage['pcs']
    require(len(rows) == len({r['pc'] for r in rows}) == 178 and sum(r['hits'] for r in rows) == 1359068,
            'Palette source coverage totals')
    require(sum(r['hits'] > 0 for r in rows) == 174 and
        all(r.get('unreached_reason') for r in rows if not r['hits']), 'Palette unexplained unreached words')
    branches = [r for r in rows if r['taken'] or r['not_taken']]
    require(len(branches) == 12 and sum(bool(r['taken'] and r['not_taken']) for r in branches) == 8 and
        all(r.get('single_direction_reason') for r in branches if not (r['taken'] and r['not_taken'])), 'Palette unexplained branch direction')
    controls = report['mutations']
    require(len(controls) == len({r['name'] for r in controls}) == 13, 'Palette control membership')
    require(sum(r['kind'] == 'original-word substitution' for r in controls) == 6 and
            sum(r['kind'] == 'native semantic control' for r in controls) == 7, 'Palette control split')
    for row in controls:
        require(row['detected'] is True and row['diagnostic'].startswith('FAIL ModelPalette49S: '), 'Palette control missing assertion')
        if row['name'] == 'palette_source_index_shift':
            require(row['diagnostic'] == 'FAIL ModelPalette49S: oracle read outside fixture: 37891 pc=2364752',
                    'Palette source-index guard evidence changed')
        else:
            require('mismatch' in row['diagnostic'], 'Palette control lacks behavioral mismatch')


def rigid_gate(normal, sanitized, validation):
    require(validation['passed'] is True and validation['normal'] == validation['sanitized'] == RIGID_COUNTS,
            'Rigid normal/sanitized counters')
    require(validation['normal_sanitized_fixture_byte_equal'] is True and validation['normal_sanitized_stdout_byte_equal'] is True and
            validation['leak_sanitizer'] == 'disabled, not tested' and validation['runtime_heap_bytes'] == 0, 'Rigid boundaries')
    for mode, report in [('normal', normal), ('sanitized', sanitized)]:
        require(report['passed'] is True and report['result'] == RIGID_COUNTS and report['sanitizers'] is (mode == 'sanitized') and
                report['consumed_assets_unchanged'] is True, 'Rigid proof status')
        strict_command(report['commands'][0]['command'], mode == 'sanitized', '-std=c++17', False)
        require(report['commands'][0]['exit_code'] == report['commands'][1]['exit_code'] == 0, 'Rigid compile/run status')
        require(report['fixture']['actual_unique_engine_vertices'] == 2871 and report['fixture']['actual_unique_engine_triangles'] == 1905,
                'Rigid actual asset counts')
    controls = validation['mutants']
    require(len(controls) == len({r['name'] for r in controls}) == 7 and controls == normal['mutants'], 'Rigid control membership')
    for row in controls:
        require(row['killed'] is True and row['exit_code'] == 1 and row['reason'] == 'independent rational oracle mismatch',
                'Rigid crash/setup failure counted as parity mismatch')


def connected_gate(report, export):
    require(report['passed'] is True and report['leak_sanitizer_used'] is False and set(report['runs']) == {'normal', 'asan-ubsan', 'plain'},
            'Connected status/modes')
    require(export['passed'] is True and export['schema'] == 'animated49s.export.v1' and export['summary'] == CONNECTED_COUNTS and
            export['consumer_controls'] == CONSUMER_CONTROLS, 'Connected export counters')
    for mode, run in report['runs'].items():
        require(run['summary'] == CONNECTED_COUNTS and run['consumer_controls'] == CONSUMER_CONTROLS and
                run['read_only_native_cli'] is (mode == 'plain'), 'Connected run counters/CLI mode')
        strict_command(run['flags'], mode == 'asan-ubsan', '-std=c++20', False if mode == 'plain' else 'stage_trace.h')
        require(run['trace_sha256'] == export['trace_sha256'] and run['exact_goldens_sha256'] == export['exact_goldens_sha256'],
                'Connected mode trace/golden divergence')
        require(run['mesh_sha256'] == {r['mesh_path']: r['mesh_sha256'] for r in export['frames']}, 'Connected mode mesh divergence')
    rows = export['frames']
    require(len(rows) == 6 and {(r['frame'], r['label']) for r in rows} == FRAME_KEYS, 'Connected frame/control membership')
    for row in rows:
        equal_fields(row, {'vertices': 5755, 'indices': 10713, 'static_chariot_vertices': 2884, 'changed_engine_vertices': 2871}, 'Connected mesh counts')
        require(math.isfinite(row['elapsed_seconds']) and 0 < row['elapsed_seconds'] < 10, 'Connected elapsed time')
        require(row['pose_source'] == ('actual49Q49R' if row['label'] == 'full-motion' else 'counterfactual-diagnostic-setters'), 'Connected pose-source label')
        require(row['changed_engine_vertices_vs_full'] == {'full-motion': 0, 'frozen-nodes': 640, 'frozen-roots': 2871}[row['label']], 'Connected counterfactual effect')


def review_gate(normal, sanitized):
    for mode, report in [('normal', normal), ('sanitized', sanitized)]:
        require(report['status'] == 'pass' and report['test'] == REVIEW_COUNTS and report['fixture_cases'] == 7 and
                report['fixture_vertices'] == 3904, 'Independent review counters')
        strict_command(report['commands']['flags'], mode == 'sanitized', '-std=c++17', 'trace.h')
        rows = report['mutants']
        require(len(rows) == len({r['name'] for r in rows}) == 13, 'Independent review control membership')
        allowed = {'conditional root and ordered duplicate getters', 'ready root must skip getter only',
            'analytic graph matrix/cache mismatch', 'source table/palette order mismatch',
            'rational binary64/binary32 bit mismatch', 'root multiplied twice or hierarchy missing',
            'vertex rejection accepted', 'late rejection partially committed vertices'}
        require(all(r['detected'] is True and r['diagnostic'] in allowed for r in rows), 'Independent review lacks behavioral assertion')
    require(normal['test'] == sanitized['test'] and normal['mutants'] == sanitized['mutants'], 'Independent review mode divergence')


def mesh_gate(data, baseline):
    require(len(data) == len(baseline) == 180988 and data[:16] == baseline[:16] == b'RRMESH01' + struct.pack('<II', 5755, 10713), 'Mesh header/size')
    require(data[:16 + 2884 * 24] == baseline[:16 + 2884 * 24], 'Static Chariot changed')
    require(data[16 + 5755 * 24:] == baseline[16 + 5755 * 24:], 'Index bytes changed')
    changed = 0
    for i in range(5755):
        at = 16 + i * 24
        require(data[at + 12:at + 24] == baseline[at + 12:at + 24], 'UV/color bytes changed')
        require(all(math.isfinite(x) for x in struct.unpack_from('<3f', data, at)), 'Nonfinite mesh position')
        changed += data[at:at + 12] != baseline[at:at + 12]
    return changed


def evidence_gates(words):
    component = load('validation.json'); pins(component['input_sha256'])
    require(component['status'] == 'pass' and component['new_production_units'] == 1 and component['source_words'] == 178,
            'Palette component validation boundary')
    palette_reports = []
    for mode in ['normal', 'sanitized']:
        report, coverage = load(f'proof/{mode}/verification.json'), load(f'proof/{mode}/coverage.json')
        palette_gate(report, coverage, mode); palette_reports.append(report)
        require({r['pc']: r['word'] for r in coverage['pcs']} == words, 'Executed coverage differs from original source map')
        require((E / f'proof/{mode}/test.stdout').read_text().strip() == report['result'] and
                (E / f'proof/{mode}/test.stderr').read_bytes() == b'', 'Palette raw runtime evidence')
        for row in report['mutations']:
            prefix = 'source-control-' if row['kind'] == 'original-word substitution' else 'native-control-'
            require((E / f'proof/{mode}' / (prefix + row['name'] + '.stderr')).read_text().strip() == row['diagnostic'],
                    'Palette raw mutation diagnostic')
        pins({'evidence/authoredGeometry49O/asset-reader/fixtures/' + k: v for k, v in report['psg_inputs'].items()})
    require(palette_reports[0]['coverage'] == palette_reports[1]['coverage'] and palette_reports[0]['mutations'] == palette_reports[1]['mutations'], 'Palette mode divergence')
    require('2530979 checks' in (E / 'proof/uninstrumented-normal.txt').read_text() and
            'ordered tracing off' in (E / 'proof/uninstrumented-normal.txt').read_text(), 'Uninstrumented palette evidence')
    rigid = [load('rigid/normal/report.json'), load('rigid/sanitized/report.json'), load('rigid/validation.json')]
    rigid_gate(*rigid)
    for mode, report in zip(['normal', 'sanitized'], rigid):
        pins(report['production_source_sha256']); pins(report['fixture']['input_hashes'])
        require(json.loads((E / f'rigid/{mode}/test.stdout').read_text()) == RIGID_COUNTS and
                (E / f'rigid/{mode}/test.stderr').read_bytes() == b'', 'Rigid raw runtime evidence')
    reviews = [load('independent-review/normal-results.json'), load('independent-review/sanitized-results.json')]
    review_gate(*reviews)
    for mode, report in zip(['normal', 'sanitized'], reviews):
        pins(report['pins']); pins(report['source_pins']); pins([report['numeric_fixture']])
        for row in report['mutants']:
            require((E / 'independent-review' / (mode + '-' + row['name'] + '.stderr')).read_text().strip() == row['diagnostic'], 'Independent raw assertion')
    connected, export = load('connected/verification.json'), load('connected/export-report.json')
    connected_gate(connected, export)
    for report in [connected, export]:
        pins(report['source_sha256']); pins(report['input_sha256'])
    baseline = current_path(export['baseline_mesh_path']).read_bytes()
    trace = load('connected/trace.json')
    require(sha(E / 'connected/trace.json') == export['trace_sha256'] and trace['summary'] == CONNECTED_COUNTS and
            len(trace['frames']) == 96 and [r['frame'] for r in trace['frames']] == list(range(96)) and
            len(trace['display_controls']) == 2, 'Connected raw trace')
    for row in trace['frames'] + trace['display_controls']:
        for side in ['left_palette', 'right_palette']:
            require(row[side]['entry_count'] == len(row[side]['node_world_bits']) == 14 and
                    all(len(x) == 16 for x in row[side]['node_world_bits']), 'Connected full 14-entry palette')
    require(sha(E / 'connected/exact-goldens49s.txt') == export['exact_goldens_sha256'], 'Connected exact golden pin')
    for mode in ['normal', 'asan-ubsan', 'plain']:
        require(json.loads((E / f'connected/{mode}-run.stdout.log').read_text()) == CONNECTED_COUNTS and
                (E / f'connected/{mode}-run.stderr.log').read_bytes() == b'', 'Connected raw run counters/stderr')
    for name in ['trace.json', 'exact-goldens49s.txt', 'export-report.native.json']:
        require((E / 'connected' / name).read_bytes() == (E / 'connected/asan-ubsan' / name).read_bytes(), 'Connected sanitizer replay differs: ' + name)
    for row in export['frames']:
        data = (E / 'connected' / row['mesh_path']).read_bytes()
        require(digest(data) == row['mesh_sha256'] and mesh_gate(data, baseline) == 2871, 'Connected actual mesh pin/change count')
    return export


def resource_gate(footprint, benchmark):
    for report in [footprint, benchmark]:
        require(report['status'] == 'measured' and report['compiler_target'] == 'x86_64-linux-gnu', 'Resource target/status')
        for command in report['compile_commands']:
            strict_command(command, False, '-std=c++17')
    require(set(footprint['incremental49S_units']) == {'game/model_palette49s', 'game/rigid_vertices49s'}, 'Incremental resource unit membership')
    require(footprint['totals'] == {'gnu_size_text_including_readonly_and_unwind_bytes': 2030,
        'executable_text_bytes': 1710, 'readonly_data_bytes': 64, 'data_bytes': 0, 'bss_bytes': 0}, 'Incremental object totals')
    for key, value in footprint['totals'].items():
        require(sum(footprint['objects'][n][key] for n in footprint['incremental49S_units']) == value, 'Object section arithmetic')
    require(footprint['workspace_total_bytes'] == sum(footprint['workspace_composition'].values()) == 13380 and
        footprint['record_bytes']['ModelPaletteWorkspace49S'] == 13380 and footprint['rigid_projection_workspace_bytes'] == 0,
        'Resource workspace accounting')
    require(footprint['caller_payload_total_bytes_excluding_loader_and_frame_padding'] == sum(footprint['caller_payload_records'].values()) == 155821,
            'Resource caller payload arithmetic')
    require(footprint['actual_active_palette_bytes'] == 1928 and footprint['actual_palette_storage_bytes'] == 8328 and
            footprint['maximum_individual49S_compiler_frame_bytes'] == 112, 'Resource palette/stack boundaries')
    require(all(not footprint['objects'][n]['direct_allocator_references'] for n in footprint['incremental49S_units']), 'New production allocator reference')
    require(benchmark['cpu_affinity'] == [0] and benchmark['normal_virtual_address_limit_bytes'] == 67108864 and
            benchmark['samples_per_mode'] == 15, 'Benchmark constraints')
    modes = {'palette_ready', 'palette_root_setters', 'rigid_ready_palette', 'combined_ready', 'combined_root_setters'}
    require(len(benchmark['results']) == 75 and len(benchmark['calibration']) == 5 and set(benchmark['statistics']) == modes and
            {r['mode'] for r in benchmark['results']} == modes, 'Benchmark sample membership')
    expected_checksums = benchmark['fixture']['expected_checksums']
    for mode in modes:
        rows = [r for r in benchmark['results'] if r['mode'] == mode]
        require(len(rows) == 15 and {r['sample'] for r in rows} == set(range(15)), 'Benchmark missing sample')
        values = []
        for row in rows:
            equal_fields(row, expected_checksums, 'Benchmark fixture persistence')
            require(row['iterations'] == benchmark['iterations_per_sample_by_mode'][mode] and row['nanoseconds'] > 0 and
                    row['nanoseconds_per_iteration'] == row['nanoseconds'] / row['iterations'], 'Benchmark timing arithmetic')
            equal_fields(row, {'nodes': 52, 'left_vertices': 1411, 'right_vertices': 1460,
                'palette_entries_per_engine': 14, 'workspace_bytes': 13380, 'fixture_payload_bytes': 133812}, 'Benchmark fixture counts')
            equal_fields(row, {'palette_calls_per_iteration': 0 if mode == 'rigid_ready_palette' else 2,
                'rigid_calls_per_iteration': 0 if mode.startswith('palette_') else 2,
                'root_setters_per_iteration': 2 if mode.endswith('root_setters') else 0}, 'Benchmark stage counts')
            values.append(row['nanoseconds_per_iteration'])
        stats = benchmark['statistics'][mode]
        require(stats['median_ns'] == statistics.median(values) and stats['minimum_ns'] == min(values) and
                stats['maximum_ns'] == max(values) and stats['mean_ns'] == statistics.mean(values), 'Benchmark statistics')
    probes = benchmark['separate_untimed_success_probes']
    require(len(probes) == 5 and {r['mode'] for r in probes} == modes, 'Benchmark untimed probe membership')
    for row in probes:
        require(row['cpp_new_allocations'] == row['nanoseconds'] == 0 and row['getter_order_verified'] == 1,
                'Benchmark successful-call allocation/getter probe')
        expected = 0 if row['mode'] == 'rigid_ready_palette' else 30 if row['mode'].endswith('root_setters') else 28
        require(row['getter_calls'] == expected * row['iterations'], 'Benchmark getter accounting')
    for unit, pin in benchmark['reused_footprint_objects'].items():
        require(footprint['objects'][unit]['object_sha256'] == pin, 'Benchmark reused object provenance')


def resource_gates():
    footprint, benchmark = load('footprint/summary.json'), load('benchmark/verification.json')
    resource_gate(footprint, benchmark)
    pins(footprint['source_hashes']); pins(benchmark['source_hashes']); pins(benchmark['fixture']['scene_sources'])
    require(benchmark['footprint_summary_sha256'] == sha(E / 'footprint/summary.json') and
        benchmark['fixture']['trace_sha256'] == sha(E / benchmark['fixture']['trace_path'].split('evidence/modelPalette49S/', 1)[-1]), 'Benchmark fixture/source summary pins')
    rows = [line.split() for line in (E / 'footprint/record-sizes.stdout').read_text().splitlines()]
    require({n: int(s) for n, s, a in rows} == footprint['record_bytes'] and
            {n: int(a) for n, s, a in rows} == footprint['record_alignments'], 'Raw record size/alignment evidence')
    for filename, key in [('samples.stdout', 'results'), ('calibration.stdout', 'calibration'),
                          ('ordinary-new-getter-probe.stdout', 'separate_untimed_success_probes')]:
        raw_rows = []
        for line in (E / 'benchmark' / filename).read_text().splitlines():
            pairs = [x.split('=', 1) for x in line.split()]
            require(all(len(x) == 2 for x in pairs) and len({x[0] for x in pairs}) == len(pairs), 'Malformed benchmark sample')
            raw_rows.append({k: v if k == 'mode' else int(v) for k, v in pairs})
        require(len(raw_rows) == len(benchmark[key]), 'Benchmark raw sample count')
        for raw, row in zip(raw_rows, benchmark[key]):
            equal_fields(row, raw, 'Benchmark raw sample')


def plan_gate(plan, export, frame, profile):
    require(plan['schema'] == 'animated49s.visual-plan.v1' and plan['diagnostic_only'] is True and
            plan['total_draw_calls'] == plan['solid_draw_calls'] == 360 and plan['wire_draw_calls'] == 0 and
            plan['submitted_triangles'] == plan['unique_input_triangles'] == 73409 and
            plan['planned_texture_uploads'] == len(plan['textures']) == 26 and plan['planned_gl_buffer_uploads'] == 4,
            'Render plan draw/triangle/texture counts')
    require(plan['budget']['gl_geometry_and_texture_payload_bytes'] == 7604084 and
            plan['budget']['additional_posed_mesh_read_bytes'] == 180988 and
            plan['budget']['gl_buffer_bytes'] + plan['budget']['gl_texture_bytes'] == 7604084, 'Render payload arithmetic')
    selected = next(r for r in export['frames'] if (r['frame'], r['label']) == (frame, profile))
    require(plan['native_export']['selected'] == selected and plan['native_export']['sha256'] == sha(E / 'connected/export-report.json') and
            plan['native_export']['bytes'] == (E / 'connected/export-report.json').stat().st_size, 'Render native export provenance')
    expected = dict(export['input_sha256']); expected.update(export['source_sha256'])
    require({k: r['sha256'] for k, r in plan['native_export']['verified_pins'].items()} == expected, 'Render source pin membership')
    pins(plan['native_export']['verified_pins'])
    require(plan['budget']['native_pinned_file_bytes_verified'] == sum(r['bytes'] for r in plan['native_export']['verified_pins'].values()), 'Render validated input byte accounting')
    require(plan['profile']['native_engine_animation'] is (profile == 'full-motion') and
            plan['profile']['counterfactual_display_control'] is (profile != 'full-motion') and
            plan['profile']['original_material_programs'] is False and plan['profile']['original_lighting_or_visibility'] is False,
            'Render interpretation boundary')
    require(plan['uploaded_engine_geometry']['unchanged_chariot_vertices'] == 2884 and
            plan['uploaded_engine_geometry']['uv_color_indices_unchanged'] is True and
            plan['uploaded_engine_geometry']['engine_vertices'] == 2871, 'Render geometry preservation')
    require(plan['caption'] == {'height': 72, 'outside_framebuffer': True} and
            plan['framebuffer']['width'] == 640 and plan['framebuffer']['height'] == 480, 'Render framebuffer/caption boundary')
    require(len(plan['spans']) == 360 and [r['draw_order'] for r in plan['spans']] == list(range(360)) and
            sum(r['triangle_count'] for r in plan['spans']) == 73409, 'Render ordered surface partition')
    for mesh in plan['meshes']:
        rows = [r for r in plan['spans'] if r['mesh_id'] == mesh['id']]
        at = 0
        for row in rows:
            require(row['index_start'] == at and row['index_byte_offset'] == at * 4 and row['index_count'] == row['triangle_count'] * 3,
                    'Render index partition')
            at += row['index_count']
        require(at == mesh['index_count'], 'Render incomplete mesh partition')
        pins({mesh['path']: {'sha256': mesh['sha256'], 'bytes': mesh['file_bytes']}})
    for row in plan['textures']:
        pins({row['path']: {'sha256': row['sha256'], 'bytes': row['file_bytes']}})
        require(row['rgba_bytes'] == row['width'] * row['height'] * 4 and row['gl_level'] == 0, 'Render texture upload payload')
    for pathkey, hashkey in [('recipe_path', 'recipe_sha256'), ('bindings_path', 'bindings_sha256')]:
        pins({plan[pathkey]: plan[hashkey]})
    require(plan['renderer49S_sha256'] == sha(ROOT / 'tools/visual/render_animated49s.py'), 'Render current script pin')
    payload = dict(plan); expected_hash = payload.pop('plan_sha256')
    require(digest(json.dumps(payload, sort_keys=True, separators=(',', ':'), allow_nan=False).encode()) == expected_hash, 'Render canonical plan hash')


def frame_gate(report, plan):
    equal_fields(report, {'status': 'rendered_diagnostic_only', 'schema': 'animated49s.visual-report.v1',
        'actual_draw_calls': 360, 'actual_submitted_triangles': 73409, 'actual_texture_uploads': 26,
        'actual_gl_buffer_uploads': 4, 'shared_mesh_sets': 2, 'uniform_bits_round_trip': True,
        'framebuffer_unchanged_by_caption': True, 'opaque_output_alpha': True}, 'Rendered result')
    require(report['plan'] == plan and len(report['images']) == 2 and len(report['texture_state_checks']) == 26 and
            all(r['parameters_and_level_zero_verified'] is True for r in report['texture_state_checks']), 'Rendered plan/texture verification')
    require(report['runtime']['cpu_affinity'] == [0] and report['runtime']['software_environment']['LP_NUM_THREADS'] == '1',
            'Renderer process constraints')
    require(report['pixel_rgba8_sha256'] == report['images'][0]['rgba8_sha256'], 'Rendered pixel hash marker')


def render_gates(export, passes):
    from PIL import Image
    require(len(passes) == 2 and len(set(passes)) == 2 and all(p not in {'pass-1', 'pass-2', 'pass-3'} for p in passes), 'Authoritative final render pass pair')
    canonical = {}; pixels = {}; records = []
    for directory in passes:
        safe_names([directory])
        for frame, profile in sorted(FRAME_KEYS):
            stem = f'frame-{frame:03d}-{profile}'
            plan = load(f'frames/{directory}/{stem}.plan.json')
            report = load(f'frames/{directory}/{stem}.json')
            plan_gate(plan, export, frame, profile); frame_gate(report, plan)
            images = []
            for row in report['images']:
                path = current_path(row['path']); pins([row])
                with Image.open(path) as image:
                    rgba = image.convert('RGBA')
                    require(digest(rgba.tobytes()) == row['rgba8_sha256'], 'Decoded PNG pixel pin')
                    images.append(rgba.copy())
            require(images[0].size == (640, 480) and images[1].size == (640, 552) and
                    images[1].crop((0, 0, 640, 480)).tobytes() == images[0].tobytes(), 'Caption altered framebuffer pixels')
            key = (frame, profile)
            pair = (plan['plan_sha256'], images[0].tobytes())
            if key in canonical:
                require(canonical[key] == pair, 'Final repeat plan/pixels differ')
            else:
                canonical[key] = pair; pixels[stem] = images[0]
            records.append({'pass': directory, 'frame': frame, 'profile': profile,
                'plan_sha256': plan['plan_sha256'], 'pixel_rgba8_sha256': report['pixel_rgba8_sha256']})
    require(len({r['pixel_rgba8_sha256'] for r in records}) == 6, 'Final animation/control images do not differ')
    full = pixels['frame-095-full-motion'].tobytes(); differences = []
    for stem, image in sorted(pixels.items()):
        if stem == 'frame-095-full-motion':
            continue
        data = image.tobytes()
        changed = [i for i in range(640 * 480) if data[4 * i:4 * i + 4] != full[4 * i:4 * i + 4]]
        require(bool(changed), 'Final counterfactual has no pixel effect')
        bbox = [min(i % 640 for i in changed), min(i // 640 for i in changed), max(i % 640 for i in changed), max(i // 640 for i in changed)]
        differences.append({'image': stem, 'pixels_different_from_frame95_full': len(changed), 'bbox': bbox})
    require(differences == load('independent-review/pixel-differences.json'), 'Independent raw pixel difference mismatch')
    integration = load('independent-review/integration-results.json')
    require(integration['status'] == 'pass' and integration['export_report_sha256'] == sha(E / 'connected/export-report.json') and
            integration['trace_sha256'] == export['trace_sha256'] and integration['original_elf_sha256'] == ELF_SHA,
            'Independent integration current provenance')
    equal_fields(integration, {'graph_states_checked': 98, 'all14_palettes_checked': 196, 'exact_rational_positions_checked': 17226}, 'Independent integration counts')
    review = load('independent-review/renderer-results.json')
    require(review['status'] == 'pass' and review['export_report_sha256'] == sha(E / 'connected/export-report.json') and
            review['renderer_sha256'] == sha(ROOT / 'tools/visual/render_animated49s.py') and
            review['frame_directory'] == 'evidence/modelPalette49S/frames/' + passes[-1], 'Independent renderer current provenance')
    require(len(review['plans']) == len(review['rendered_frames']) == 6 and len(review['metadata_negative_controls']) == 6 and
            review['renderer_mesh_controls'] == {'status': 'passed', 'checks': 7, 'GL_context_created': False, 'draw_calls': 0}, 'Independent renderer controls')
    for row in review['rendered_frames']:
        require(row['pixel_rgba8_sha256'] == digest(canonical[(row['frame'], row['profile'])][1]) and
                row['caption_preserves_framebuffer'] is True, 'Independent reviewed image pin')
    return {'authoritative_passes': passes, 'frames': records, 'pixel_differences': differences,
            'preliminary_passes': ['pass-1', 'pass-2', 'pass-3'], 'plans_and_pixels_repeat_identical': True}


def presentation_gate(passes):
    from PIL import Image
    report = load('frames/presentation/presentation.json')
    require(report['status'] == 'passed' and report['source_pass'] == passes[0] and report['draws_performed'] == 0,
            'Presentation source/draw boundary')
    pins(report['source_pngs']); pins(report['images']); pins([report['animation']])
    images = []
    for row in report['images']:
        with Image.open(current_path(row['path'])) as image:
            rgba = image.convert('RGBA')
            require(list(rgba.size) == row['dimensions'] and digest(rgba.tobytes()) == row['rgba8_sha256'], 'Presentation decoded image pin')
            images.append(rgba.copy())
    sources = []
    for i, frame in enumerate([0, 31, 63, 95]):
        with Image.open(E / f'frames/{passes[0]}/frame-{frame:03d}-full-motion.png') as image:
            rgba = image.convert('RGBA'); sources.append(rgba.copy())
            x, y = (i % 2) * 640, 40 + (i // 2) * 552
            require(images[0].crop((x, y, x + 640, y + 552)).tobytes() == rgba.tobytes(), 'Presentation tile source pixels')
    require(report['control_crop_box'] == [128, 120, 448, 340], 'Presentation control crop boundary')
    for i, profile in enumerate(['full-motion', 'frozen-nodes', 'frozen-roots']):
        with Image.open(E / f'frames/{passes[0]}/frame-095-{profile}.raw.png') as image:
            crop = image.convert('RGBA').crop(tuple(report['control_crop_box'])).resize((640, 440), Image.Resampling.NEAREST)
            require(images[1].crop((i * 640, 54, (i + 1) * 640, 494)).tobytes() == crop.tobytes(), 'Presentation control nearest-pixel copy')
    with Image.open(current_path(report['animation']['path'])) as animation:
        require(animation.n_frames == 4 and animation.info['loop'] == 0 and report['animation']['presentation_frame_duration_ms'] == 600,
                'Presentation four-sample loop boundary')
        for i, source in enumerate(sources):
            animation.seek(i)
            require(animation.convert('RGBA').tobytes() == source.tobytes(), 'Presentation lossless animation source pixels')
    return {'source_pass': passes[0], 'four_frame_tiles_source_pixel_exact': True,
            'counterfactual_crops_nearest_pixel_exact': True, 'lossless_loop_source_pixel_exact': True,
            'loop_is_presentation_only': True}


def check_count(text):
    try:
        return int(json.loads(text)['checks'])
    except (ValueError, KeyError):
        match = re.search(r'checks=(\d+)', text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks', text)
        require(match is not None, 'Missing stdout check count')
        return int(match.group(1))


def previous_report():
    return json.loads((ROOT / 'evidence/vehicleVisual49R/final-normal/regression-report.json').read_text())


def expected_inputs():
    expected = set(previous_report()['input_hashes'])
    export = load('connected/export-report.json')
    expected.update(export['input_sha256'])
    expected.update('evidence/modelPalette49S/connected/' + r['mesh_path'] for r in export['frames'])
    expected.update({
        'tools/test_native_animated_view49s.py', 'cmake/NativeAnimatedView49S.cmake',
        'tools/visual/export_animated49s.cpp', 'tests/native/model_palette49s.cpp', 'tests/native/rigid_vertices49s.cpp',
        'src/game/model_palette49s.cpp', 'src/game/model_palette49s.h', 'src/game/rigid_vertices49s.cpp', 'src/game/rigid_vertices49s.h',
        'src/assets/scene_asset48d.cpp', 'src/assets/scene_asset48d.h', 'src/assets/native_asset48b.cpp', 'src/assets/native_asset48b.h',
        'src/assets/import_decode48c.h', 'src/assets/import_scene48d.h',
        'evidence/modelPalette49S/source/palette_oracle49s.h', 'evidence/modelPalette49S/source/palette_words49s.h',
        'evidence/modelPalette49S/rigid/normal/fixture.bin', 'evidence/modelPalette49S/rigid/normal/fixture-manifest.json',
        'evidence/modelPalette49S/connected/export-report.json', 'evidence/modelPalette49S/connected/export-report.native.json',
        'evidence/modelPalette49S/connected/trace.json', 'evidence/modelPalette49S/connected/exact-goldens49s.txt'})
    require(len(expected) == 362, 'Expected aggregate input accounting')
    return expected


def report_gate(report, mode):
    sanitized = mode == 'sanitized'
    flags = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-fno-fast-math', '-frounding-math', '-ffp-contract=off']
    flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-sanitize-recover=all'] if sanitized else ['-O2']
    old = previous_report(); suites = set(old['selected_tests']) | NEW_SUITES
    cmake = (ROOT / 'cmake/NativeAnimatedView49S.cmake').read_text()
    registered = re.search(r'set\(rr_animated_view_tests\s+([^)]*)\)', cmake)
    require(registered is not None and set(registered.group(1).split()) == suites, 'CMake/runner suite membership')
    require(report['status'] == 'passed' and not report['errors'] and not report['changed_inputs'] and
            report['scope'] == 'all_animated-view_checks' and report['serial'] is True, 'Aggregate status/scope')
    require(report['compile_flags'] == flags and report['link_flags'] == (['-fsanitize=address,undefined'] if sanitized else []) and
            report['sanitizers'] is sanitized and report['leak_detection'] is False, 'Aggregate language/flags/sanitizer boundary')
    require(len(report['selected_tests']) == len(report['tests']) == 52 and set(report['selected_tests']) == suites and
            {r['name'] for r in report['tests']} == suites, 'Aggregate suite membership')
    require(len(report['build']) == 46 and {r['source'] for r in report['build']} == {r['source'] for r in old['build']} | NEW_UNITS | REUSED_UNITS,
            'Aggregate production unit membership')
    require(set(report['input_hashes']) == expected_inputs(), 'Aggregate complete input membership')
    for name, value in old['input_hashes'].items():
        if name != 'CMakeLists.txt':
            require(report['input_hashes'][name] == value, 'Inherited aggregate input changed: ' + name)
    if sanitized:
        require('detect_leaks=0' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['ASAN_OPTIONS'] and
                'halt_on_error=1' in report['environment']['UBSAN_OPTIONS'], 'Aggregate sanitizer environment')
    for build in report['build'] + [r['build'] for r in report['tests']]:
        require(build['status'] == 'passed' and build['returncode'] == 0 and build['command'][1:1 + len(flags)] == flags,
                'Aggregate compile status/flags')
        strict_command(build['command'], sanitized, '-std=c++20', False)
        require(build['stdout']['bytes'] == 0 and build['stdout']['sha256'] == EMPTY_SHA, 'Aggregate compile stdout')
        if build.get('source') in NEW_UNITS:
            require(build['stderr']['bytes'] == 0 and build['stderr']['sha256'] == EMPTY_SHA, 'New production compiler warning')
    for test in report['tests']:
        run = test['run']
        require(test['status'] == run['status'] == 'passed' and run['returncode'] == 0 and
                run['command'][0] == test['executable']['path'], 'Aggregate run status/executable')
        require(run['stderr']['bytes'] == 0 and run['stderr']['sha256'] == EMPTY_SHA, 'Aggregate runtime stderr')


def regression_gates():
    summary = load('regression-summary.json')
    equal_fields(summary, {'status': 'passed', 'all_mode_outputs_identical': True,
        'semantic_trace_identical_to_focused': True, 'connected_replay_exact_bytes': True,
        'production_units_each': 46, 'checks_each': 154197329,
        'trace_checks': {'aggregate': 1110835, 'focused': 1110835}}, 'Aggregate summary')
    old = json.loads((ROOT / 'evidence/vehicleVisual49R/regression-summary.json').read_text())['runs']['normal']['tests']
    reports = {}
    for mode in ['normal', 'sanitized']:
        row, report = summary['runs'][mode], load(f'final-{mode}/regression-report.json'); reports[mode] = report
        report_gate(report, mode); pins(report['input_hashes'])
        equal_fields(row, {'suites': 52, 'checks': 154197329, 'tracked_inputs': 362, 'inputs_unchanged': True}, 'Aggregate per-mode summary')
        require(row['report_sha256'] == sha(E / f'final-{mode}/regression-report.json'), 'Aggregate report pin')
        require(len(row['tests']) == 52 and [r for r in row['tests'] if r['name'] not in NEW_SUITES] == old,
                'Inherited 49-suite stdout/check summaries changed')
        total = 0
        for test in row['tests']:
            actual = next(r for r in report['tests'] if r['name'] == test['name'])['run']
            raw = (E / f'final-{mode}' / (test['name'] + '.stdout')).read_bytes()
            require(digest(raw) == test['stdout_sha256'] == actual['stdout']['sha256'] and len(raw) == actual['stdout']['bytes'] and
                    test['stderr_bytes'] == 0 and check_count(raw.decode()) == test['checks'], 'Aggregate raw stdout counts/pins')
            if test['name'] not in NEW_SUITES:
                require(raw == (ROOT / 'evidence/vehicleVisual49R/final-normal' / (test['name'] + '.stdout')).read_bytes(), 'Inherited stdout bytes changed')
            else:
                require(test['checks'] == {'model_palette49s': 2530979, 'rigid_vertices49s': 371680, 'animated_export49s': 1110835}[test['name']], 'New uninstrumented aggregate count')
            total += test['checks']
        require(total == 154197329, 'Aggregate total check arithmetic')
        processes = report['build'] + [x[k] for x in report['tests'] for k in ['build', 'run']]
        require(len(row['process_logs']) == 300, 'Durable aggregate process log count')
        names = []
        for process in processes:
            for stream in ['stdout', 'stderr']:
                meta = process[stream]; path = E / f'final-{mode}/logs' / Path(meta['path']).name
                require(path.is_file() and sha(path) == meta['sha256'] and path.stat().st_size == meta['bytes'] and
                        row['process_logs'][path.name] == {'bytes': meta['bytes'], 'sha256': meta['sha256']}, 'Durable aggregate process log pin')
                names.append(path.name)
        require(len(names) == len(set(names)) and set(names) == set(row['process_logs']), 'Aggregate process log membership')
        expected_replays = {'palette-coverage.json', 'animated/export-report.native.json', 'animated/trace.json', 'animated/exact-goldens49s.txt'} | {
            'animated/' + r['mesh_path'] for r in load('connected/export-report.json')['frames']}
        require(set(row['replayed_files']) == expected_replays, 'Aggregate replay membership')
        for name, expected in row['replayed_files'].items():
            path = E / f'final-{mode}' / name
            require(sha(path) == expected, 'Aggregate replay pin')
            if name.startswith('animated/'):
                require(path.read_bytes() == (E / 'connected' / name[len('animated/'):]).read_bytes(), 'Aggregate connected replay changed')
        require(check_count((E / f'final-{mode}/model_palette49s-output.stdout').read_text()) == 2530980 and
                json.loads((E / f'final-{mode}/animated_export49s-output.stdout').read_text()) == CONNECTED_COUNTS, 'Aggregate replay stdout counts')
        coverage = load(f'final-{mode}/palette-coverage.json'); focused = load('proof/normal/coverage.json')
        require(coverage['ordered_trace'] is False and coverage['flag_stores'] == 0 and coverage['checks'] == 2530980, 'Aggregate instrumentation boundary')
        for key in ['ordered_trace', 'flag_stores', 'checks']:
            del coverage[key]; del focused[key]
        del focused['accounting']
        focused['pcs'] = [{k: r[k] for k in ['pc', 'word', 'hits', 'taken', 'not_taken']} for r in focused['pcs']]
        require(coverage == focused, 'Aggregate original coverage differs from focused proof')
    require(summary['runs']['normal']['tests'] == summary['runs']['sanitized']['tests'] and
            summary['runs']['normal']['replayed_files'] == summary['runs']['sanitized']['replayed_files'] and
            reports['normal']['input_hashes'] == reports['sanitized']['input_hashes'], 'Aggregate mode divergence')
    return summary


def baseline_gate(path):
    require(path.stat().st_size == 140185476 and sha(path) == BASELINE_SHA, 'Wrong verified 49R archive')
    changed = []
    with zipfile.ZipFile(path) as archive:
        names = [r.filename for r in archive.infolist() if not r.is_dir()]
        safe_names(names)
        require(len(names) == 20696 and archive.testzip() is None, '49R baseline ZIP count/CRC')
        for name in names:
            path = ROOT.parent / name
            require(path.is_file() and not path.is_symlink(), 'Missing baseline member: ' + name)
            if sha(path) != digest(archive.read(name)):
                changed.append(name)
    require(set(changed) == ALLOWED, 'Unexpected inherited file changes: ' + repr(changed))
    return names, sorted(changed)


def gate_controls(export, passes, aggregate):
    rows = []
    def control(name, original, mutate, gate):
        sample = copy.deepcopy(original); gate(sample); mutate(sample)
        try:
            gate(sample)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Release gate control escaped: ' + name)
    for mode in ['normal', 'sanitized']:
        data = [load(f'proof/{mode}/verification.json'), load(f'proof/{mode}/coverage.json')]
        gate = lambda x, m=mode: palette_gate(*x, m)
        control(mode + '_palette_trace_disabled', data, lambda x: x[1].update(ordered_trace=False), gate)
        control(mode + '_palette_false_detection', data, lambda x: x[0]['mutations'][1].update(diagnostic='process crashed'), gate)
        control(mode + '_palette_branch_omitted', data, lambda x: x[1]['accounting'].update(reached_branch_pcs=10), gate)
        control(mode + '_palette_trace_include_missing', data, lambda x: x[0]['command'].remove('-include'), gate)
    rigid = [load('rigid/normal/report.json'), load('rigid/sanitized/report.json'), load('rigid/validation.json')]
    control('rigid_crash_counted_as_detection', rigid, lambda x: x[2]['mutants'][0].update(exit_code=-11), lambda x: rigid_gate(*x))
    control('rigid_wrong_count', rigid, lambda x: x[0]['result'].update(checks=0), lambda x: rigid_gate(*x))
    connected = [load('connected/verification.json'), export]
    control('connected_native_source_substitution', connected, lambda x: x[0]['runs']['normal']['summary'].update(source_substitutions=1), lambda x: connected_gate(*x))
    control('connected_missing_control_frame', connected, lambda x: x[1]['frames'].pop(), lambda x: connected_gate(*x))
    control('connected_mislabeled_control', connected, lambda x: x[1]['frames'][-1].update(pose_source='actual49Q49R'), lambda x: connected_gate(*x))
    control('connected_float32_sensitivity_lost', connected, lambda x: x[1]['consumer_controls'].update(wrong_float32_semantic_input=0), lambda x: connected_gate(*x))
    control('connected_false_leak_claim', connected, lambda x: x[0].update(leak_sanitizer_used=True), lambda x: connected_gate(*x))
    reviews = [load('independent-review/normal-results.json'), load('independent-review/sanitized-results.json')]
    control('independent_setup_error_as_assertion', reviews, lambda x: x[0]['mutants'][0].update(diagnostic='compile failed'), lambda x: review_gate(*x))
    resources = [load('footprint/summary.json'), load('benchmark/verification.json')]
    control('resource_workspace_double_count', resources, lambda x: x[0].update(workspace_total_bytes=26760), lambda x: resource_gate(*x))
    control('benchmark_missing_sample', resources, lambda x: x[1]['results'].pop(), lambda x: resource_gate(*x))
    control('benchmark_wrong_median', resources, lambda x: x[1]['statistics']['palette_ready'].update(median_ns=0), lambda x: resource_gate(*x))
    control('benchmark_allocation', resources, lambda x: x[1]['separate_untimed_success_probes'][0].update(cpp_new_allocations=1), lambda x: resource_gate(*x))
    baseline = current_path(export['baseline_mesh_path']).read_bytes()
    mesh = bytearray((E / 'connected' / export['frames'][0]['mesh_path']).read_bytes())
    for name, at in [('static_chariot_changed', 16), ('engine_uv_changed', 16 + 2884 * 24 + 12), ('engine_color_changed', 16 + 2884 * 24 + 20), ('index_changed', 16 + 5755 * 24)]:
        control(name, mesh, lambda x, a=at: x.__setitem__(a, x[a] ^ 1), lambda x: mesh_gate(x, baseline))
    plan = load(f'frames/{passes[-1]}/frame-095-full-motion.plan.json')
    gate = lambda x: plan_gate(x, export, 95, 'full-motion')
    control('render_wrong_draw_count', plan, lambda x: x.update(total_draw_calls=359), gate)
    control('render_stale_export_pin', plan, lambda x: x['native_export'].update(sha256='0' * 64), gate)
    control('render_missing_source_pin', plan, lambda x: x['native_export']['verified_pins'].pop(next(iter(x['native_export']['verified_pins']))), gate)
    control('render_missing_span', plan, lambda x: x['spans'].pop(), gate)
    result = load(f'frames/{passes[-1]}/frame-095-full-motion.json')
    control('render_false_caption_preservation', result, lambda x: x.update(framebuffer_unchanged_by_caption=False), lambda x: frame_gate(x, plan))
    control('render_false_pixel_pin', result, lambda x: x.update(pixel_rgba8_sha256='0' * 64), lambda x: frame_gate(x, plan))
    if aggregate:
        for mode in ['normal', 'sanitized']:
            report = load(f'final-{mode}/regression-report.json'); gate = lambda x, m=mode: report_gate(x, m)
            control(mode + '_aggregate_hidden_failed_returncode', report, lambda x: x['tests'][0]['run'].update(returncode=1), gate)
            control(mode + '_aggregate_missing_suite', report, lambda x: x['tests'].pop(), gate)
            control(mode + '_aggregate_fast_math', report, lambda x: x['build'][0]['command'].append('-ffast-math'), gate)
            control(mode + '_aggregate_trace_hook', report, lambda x: x['build'][0]['command'].extend(['-include', 'trace49s.h']), gate)
            control(mode + '_aggregate_source_changed', report, lambda x: x['changed_inputs'].append('src/game/model_palette49s.cpp'), gate)
            control(mode + '_aggregate_nonempty_stderr', report, lambda x: x['tests'][0]['run']['stderr'].update(bytes=1), gate)
    for name, bad in [('duplicate_path', ['a', 'a']), ('parent_traversal', ['../a']), ('absolute_path', ['/a']), ('windows_separator', ['a\\b'])]:
        try:
            safe_names(bad)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Archive gate control escaped: ' + name)
    return {'status': 'passed', 'method': 'Decoded report/mesh mutations in memory only; no production writes or numerical diagnostic reruns.',
        'control_count': len(rows), 'all_rejected': True, 'controls': rows}


def seal_hashes(baseline_names):
    baseline_names = set(baseline_names); result = {}
    for path in sorted(ROOT.rglob('*')):
        if not path.is_file() or path == E / 'checkpoint-validation.json':
            continue
        name = path.relative_to(ROOT).as_posix(); member = ROOT.name + '/' + name
        if member not in baseline_names or member in ALLOWED:
            require(not path.is_symlink(), 'Cannot seal a new symlink')
            result[name] = sha(path)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--render-passes', nargs=2, default=['pass-4', 'pass-5'])
    parser.add_argument('--focused-only', action='store_true')
    parser.add_argument('--prepare-review', action='store_true', help='Write release-review files only, not the seal')
    parser.add_argument('--check-only', action='store_true', help='Verify existing checkpoint-validation.json without any writes')
    args = parser.parse_args()
    require(sum([args.focused_only, args.prepare_review, args.check_only]) <= 1, 'Choose one validation mode')
    if not args.focused_only:
        require(args.baseline is not None, '--baseline required for complete release validation')
    words = original_sources(); cache_gate(); export = evidence_gates(words); resource_gates()
    renders = render_gates(export, args.render_passes)
    presentation = presentation_gate(args.render_passes)
    if args.focused_only:
        controls = gate_controls(export, args.render_passes, False)
        print(json.dumps({'status': 'passed', 'scope': 'source/cache/focused/resource/final-render gates', 'controls_rejected': controls['control_count']}))
        return
    names, changed = baseline_gate(args.baseline)
    aggregate = regression_gates(); controls = gate_controls(export, args.render_passes, True)
    review = {'status': 'passed', 'blocking_findings': [], 'baseline_sha256': BASELINE_SHA,
        'baseline_members': len(names), 'changed_baseline_members': changed,
        'original_words': 178, 'palette_branches': {'reached': 12, 'both_directions': 8, 'single_direction_explained': 4},
        'palette_controls': {'behavioral_mismatches': 12, 'source_address_guard_rejections': 1},
        'rigid_semantic_controls': 7, 'independent_semantic_controls': 13,
        'connected_consumer_controls': CONSUMER_CONTROLS, 'renders': renders, 'presentation': presentation,
        'aggregate_suites_each': 52, 'aggregate_checks_each': 154197329,
        'production_units_each': 46, 'tracked_inputs_each': 362,
        'build_boundary': 'CMake unavailable; focused component builds use strict C++17, inherited aggregate harness uses strict C++20. ASan+UBSan, no LeakSanitizer claim.',
        'reviewed_file_hashes': {p.relative_to(ROOT).as_posix(): sha(p) for p in [
            Path(__file__), E / 'validation.json', E / 'proof/normal/verification.json', E / 'proof/sanitized/verification.json',
            E / 'connected/export-report.json', E / 'connected/verification.json', E / 'independent-review/integration-results.json',
            E / 'independent-review/renderer-results.json', E / 'footprint/summary.json', E / 'benchmark/verification.json', E / 'regression-summary.json']}}
    if args.prepare_review:
        (E / 'release-review').mkdir(exist_ok=True)
        (E / 'release-review/gate-controls.json').write_text(json.dumps(controls, indent=2) + '\n')
        (E / 'release-review/review.json').write_text(json.dumps(review, indent=2) + '\n')
        print(json.dumps({'status': 'passed', 'review_prepared': True, 'seal_written': False, 'controls_rejected': controls['control_count']}))
        return
    require(load('release-review/gate-controls.json') == controls and load('release-review/review.json') == review,
            'Release review artifacts absent or stale; run --prepare-review before sealing')
    result = {'checkpoint': '49S-AnimatedEngineView', 'status': 'passed',
        'baseline_sha256': BASELINE_SHA, 'baseline_members': len(names), 'changed_baseline_members': changed,
        'regression_suites_each': 52, 'regression_checks_each': aggregate['checks_each'], 'tracked_inputs_each': 362,
        'production_units_each': 46, 'new_production_units': 2, 'reused_additional_production_units': 2,
        'normal_sanitizer_outputs_identical': True, 'inherited49R_stdout_exact': True,
        'gate_controls_rejected': controls['control_count'], 'focused_palette_checks': 2575914,
        'aggregate_palette_checks': 2530979, 'aggregate_palette_coverage_output_checks': 2530980, 'rigid_checks': 371680, 'independent_review_checks': 23752,
        'connected_checks': 1110835, 'authoritative_render_passes': args.render_passes,
        'rendered_frames_each': 6, 'deterministic_plan_pixel_repeats': True,
        'focused_ordered_store_tracing': True, 'aggregate_ordered_store_tracing': False,
        'cmake_executed': False, 'focused_component_language': 'strict C++17', 'aggregate_language': 'strict C++20',
        'limits': 'Diagnostic actual engine motion/root/palette/rigid positions with stationary supplied main pose and static Chariot. Chariot IK, spawn, contacts, original VU/material/visibility remain open. No PC/RAM/install or held collision-capacity/49C ownership/copy-assignment/allocation-failure diagnostic was run by this validator.',
        'source_hashes': seal_hashes(names)}
    if args.check_only:
        existing = load('checkpoint-validation.json')
        require({k: v for k, v in existing.items() if k != 'verified_utc'} == result and isinstance(existing.get('verified_utc'), str),
                'Release seal differs from independently revalidated evidence')
    else:
        result['verified_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        (E / 'checkpoint-validation.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'source_hashes'}))


if __name__ == '__main__':
    main()
