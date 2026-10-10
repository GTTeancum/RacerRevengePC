#!/usr/bin/env python3
"""Read-only 49T release gates; only explicit --seal writes a checkpoint seal.

No compiler, C++ diagnostic, renderer, PC/RAM/install, held collision-capacity,
or 49C ownership test is executed. Negative controls alter decoded data only.
Every inherited archive member is checked and every added release file frozen.
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
E = ROOT / 'evidence/chariotAnimation49T'
BASELINE_SHA = 'a8cc6b0b4d607a40840bcf1201e31f9f9567e4293439e983927ebc8ea885f645'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
ACOS_SHA = 'a159dd9bbad9f5016411ee30c69119ced603ce1ee23c9a897efa9c6f69ec4411'
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
ALLOWED = {'RacerRevengePC/README.md', 'RacerRevengePC/CMakeLists.txt'}
NEW_UNITS = {'src/game/' + n + '49t.cpp' for n in ('source_acos', 'model_motion', 'chariot_ik', 'vehicle_animation')}
NEW_SUITES = {'model_motion49t', 'chariot_ik49t', 'vehicle_animation49t', 'chariot_export49t'}
FRAME_KEYS = {(i, 'full-motion') for i in (0, 31, 63, 95)} | {(95, 'frozen-chariot-locals'), (95, 'ik-omitted')}
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}

class GateError(ValueError):
    pass

def require(value, message):
    if not value:
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
                not any(p in ('', '.', '..') for p in name.split('/')), 'Unsafe path: ' + repr(name))

def current_path(name):
    require(isinstance(name, str), 'Non-string file path')
    if Path(name).is_absolute():
        require(name.count('/RacerRevengePC/') == 1, 'Absolute pin lacks unique project root')
        name = name.split('/RacerRevengePC/', 1)[1]
    safe_names([name])
    path = ROOT / name
    require(path.is_file() and not path.is_symlink() and path.resolve().is_relative_to(ROOT.resolve()), 'Missing or unsafe input: ' + name)
    return path

def pins(rows):
    if isinstance(rows, list):
        require(len(rows) == len({r['path'] for r in rows}), 'Duplicate file pins')
        rows = {r['path']: r for r in rows}
    require(isinstance(rows, dict) and rows, 'Missing file pins')
    for name, value in rows.items():
        row = value if isinstance(value, dict) else {'sha256': value}
        path = current_path(name)
        require(sha(path) == row['sha256'], 'File pin: ' + name)
        if 'bytes' in row:
            require(path.stat().st_size == row['bytes'], 'File size: ' + name)

def local_pins(prefix, rows):
    safe_names(list(rows))
    pins({f'evidence/chariotAnimation49T/{prefix}/{name}': value for name, value in rows.items()})

def strict_command(command, sanitized, standard, traced=None):
    require(isinstance(command, list) and command and not BAD_FLAGS.intersection(command), 'Missing command or unsafe compiler flags')
    require('-fno-fast-math' in command and '-ffp-contract=off' in command and standard in command, 'Strict FP/language flags missing')
    require(('-fsanitize=address,undefined' in command) is sanitized, 'Wrong sanitizer flags')
    if traced is False:
        require('-include' not in command and not any('ENABLE_STAGE_TRACE' in x for x in command), 'Aggregate/plain tracing unexpectedly enabled')
    elif traced:
        require(any(command[i] == '-include' and Path(command[i + 1]).name == traced for i in range(len(command) - 1)), 'Ordered trace include missing')

def integer(value):
    return int(value, 16) if isinstance(value, str) else value

def branch_kind(word):
    op, rs, rt = word >> 26, (word >> 21) & 31, (word >> 16) & 31
    if op in (4, 5, 6, 7):
        return 'integer'
    if op in (20, 21, 22, 23):
        return 'integer_likely'
    if op == 1 and rt in (0, 1, 16, 17):
        return 'regimm'
    if op == 1 and rt in (2, 3, 18, 19):
        return 'regimm_likely'
    if op in (16, 17, 18) and rs == 8 and rt in (0, 1, 2, 3):
        return 'cop' + str(op - 16)
    return None

def static_branch_outcome(word):
    op, rs, rt = word >> 26, (word >> 21) & 31, (word >> 16) & 31
    if op in (4, 5, 20, 21) and rs == rt:
        return op in (4, 20)
    if rs == 0 and op in (6, 7, 22, 23):
        return op in (6, 22)
    if rs == 0 and op == 1 and rt in (0, 1, 2, 3, 16, 17, 18, 19):
        return rt in (1, 3, 17, 19)
    return None


def original_sources():
    for op in (4, 5, 6, 7, 20, 21, 22, 23):
        require(branch_kind(op << 26) is not None, 'Scalar/likely branch decoder self-check')
    for rt in (0, 1, 2, 3, 16, 17, 18, 19):
        require(branch_kind((1 << 26) | (rt << 16)) is not None, 'REGIMM/link branch decoder self-check')
    for cop in (16, 17, 18):
        for rt in (0, 1, 2, 3):
            require(branch_kind((cop << 26) | (8 << 21) | (rt << 16)) == 'cop' + str(cop - 16), 'Coprocessor branch decoder self-check')
    require(branch_kind(0) is None and branch_kind(3 << 26) is None, 'Nonbranch decoder self-check')
    raw = current_path('work/native/project/disc/SLUS_202.68').read_bytes()
    require(digest(raw) == ELF_SHA and raw[:6] == b'\x7fELF\x01\x01', 'Original ELF pin')
    off = struct.unpack_from('<I', raw, 28)[0]
    size, count = struct.unpack_from('<HH', raw, 42)
    segments = [struct.unpack_from('<8I', raw, off + size * i) for i in range(count)]
    def take(start, end):
        matches = [raw[o + start - v:o + end - v] for k, o, v, _, n, *_ in segments if k == 1 and v <= start < end <= v + n]
        require(len(matches) == 1 and len(matches[0]) == end - start, 'Original range must be uniquely file backed')
        return matches[0]
    sources = {}
    specs = [('acos/source-authentication.json', 'acos/original_words.h', 1510),
             ('motion/source-manifest.json', None, 1538),
             ('ik/source/source-manifest.json', 'ik/source/ik_words.h', 4174),
             ('flow/source/source-manifest.json', 'flow/source/flow_words.h', 1862)]
    for filename, header, total in specs:
        manifest = load(filename); require(manifest['elf_sha256'] == ELF_SHA, 'Manifest ELF pin')
        words = {}; arrays = {}
        if header:
            arrays = {n: bytes().join(struct.pack('<I', int(w, 16)) for w in re.findall(r'0x([0-9a-fA-F]+)u', text))
                      for n, text in re.findall(r'uint32_t\s+(\w+)\[\]\s*=\s*\{(.*?)\};', (E / header).read_text(), re.S)}
        index = 0
        for row in manifest['blocks']:
            start, end = integer(row['start']), integer(row['end_exclusive'])
            data = take(start, end)
            require(len(data) == row['words'] * 4 and digest(data) == row['sha256'], 'Original block hash/size: ' + filename)
            if 'index' in row:
                require(row['index'] == index, 'Original block index')
            if 'elf_offset' in row:
                at = integer(row['elf_offset']); require(raw[at:at + len(data)] == data, 'Original ELF offset')
            if header:
                require(arrays[row['name']] == data, 'Compiled original words: ' + row['name'])
            if filename.startswith(('ik/', 'flow/')):
                require((E / Path(filename).parent / (row['name'] + '.bin')).read_bytes() == data, 'Original sidecar bytes')
            for i, word in enumerate(struct.unpack('<' + str(row['words']) + 'I', data)):
                require(start + i * 4 not in words, 'Overlapping original blocks')
                words[start + i * 4] = word
            index += row['words']
        require(index == len(words) == total, 'Original selected word count')
        if header:
            text = (E / header).read_text()
            bounds = [(name, int(address, 0), array, int(count), int(offset) if offset else None) for name, address, array, count, offset in
                      re.findall(r'\{\s*"(\w+)",\s*(0x[0-9a-fA-F]+|[0-9]+)u,\s*(\w+),\s*([0-9]+)(?:,\s*([0-9]+))?\s*\}', text)]
            expected_bounds = []; cursor = 0
            for row in manifest['blocks']:
                expected_bounds.append((row['name'], integer(row['start']), row['name'], row['words'], None if filename.startswith('flow/') else cursor))
                cursor += row['words']
            require(bounds == expected_bounds, 'Compiled source address table')
        else:
            bounds = [(int(a, 16), int(b, 16)) for a, b in re.findall(r'\{0x([0-9a-fA-F]+),0x([0-9a-fA-F]+)\}', (E / 'motion/blocks.h').read_text())]
            require(bounds == [(integer(row['start']), integer(row['end_exclusive'])) for row in manifest['blocks']], 'Compiled motion source boundaries')
        data_words = {}
        for row in manifest.get('source_data', manifest.get('data', [])):
            start = integer(row.get('start', row.get('address')))
            end = integer(row['end_exclusive']) if 'end_exclusive' in row else start + 4
            data = take(start, end)
            require(digest(data) == row['sha256'], 'Original data hash')
            for offset in range(0, len(data), 4):
                require(start + offset not in data_words, 'Overlapping original static words')
                data_words[start + offset] = struct.unpack_from('<I', data, offset)[0]
            if 'bytes' in row:
                require(bytes.fromhex(row['bytes']) == data, 'Original embedded data bytes')
            if 'word' in row:
                require(struct.unpack('<I', data)[0] == integer(row['word']), 'Original data word')
        if filename.startswith(('acos/', 'ik/')):
            data_header = E / ('acos/original_data.h' if filename.startswith('acos/') else 'ik/source/ik_data.h')
            pairs = [(int(a, 16), int(b, 16)) for a, b in re.findall(r'\{\s*0x([0-9a-fA-F]+)u,\s*0x([0-9a-fA-F]+)u\}', data_header.read_text())]
            require(len(pairs) == len(dict(pairs)) and dict(pairs) == data_words, 'Compiled original static data words')
        if 'retained_inputs' in manifest:
            pins(manifest['retained_inputs']); pins([manifest['fixture']])
        sources[filename.split('/')[0]] = words
    require(sha(ROOT / 'src/game/source_acos49t.cpp') == ACOS_SHA, 'Final portable source acos implementation pin')
    return sources

def coverage_gate(rows, words, label, explained=True):
    require(len(rows) == len(words) and len({integer(r['pc']) for r in rows}) == len(rows), label + ': incomplete/duplicate PC map')
    require({integer(r['pc']) for r in rows} == set(words), label + ': unknown source PC')
    kinds = {}; reached = both = one = selected = instructions = 0
    conditional = conditional_both = conditional_one = conditional_zero = constants = 0
    for row in rows:
        pc = integer(row['pc']); word = words[pc]
        if 'word' in row:
            require(integer(row['word']) == word, label + ': source instruction differs')
        hits, taken, not_taken = (row[k] for k in ('hits', 'taken', 'not_taken'))
        require(all(type(v) is int and v >= 0 for v in (hits, taken, not_taken)), label + ': invalid counters')
        kind = branch_kind(word)
        require(taken + not_taken == hits if kind else taken == not_taken == 0, label + ': original conditional accounting')
        if 'conditional_branch' in row:
            require(row['conditional_branch'] is bool(kind), label + ': incorrect conditional decode')
        if 'branch_kind' in row:
            require(row['branch_kind'] == kind, label + ': incorrect branch family')
        if kind:
            outcome = static_branch_outcome(word)
            if outcome is None:
                conditional += 1; conditional_both += bool(taken and not_taken)
                conditional_one += bool(hits and not (taken and not_taken)); conditional_zero += not hits
            else:
                constants += 1
                require(not_taken == 0 if outcome else taken == 0, label + ': impossible instruction-constant branch direction')
            selected += 1; kinds[kind] = kinds.get(kind, 0) + 1
            reached += bool(hits); both += bool(taken and not_taken); one += bool(hits and not (taken and not_taken))
            if hits and not (taken and not_taken) and explained:
                require(bool(row.get('one_sided_reason') or row.get('single_direction_reason')), label + ': unexplained branch direction')
        if not hits and explained:
            require(bool(row.get('unreached_reason') or row.get('reason')), label + ': unexplained unreached source word')
        instructions += hits
    return {'selected_words': len(words), 'reached_words': sum(r['hits'] > 0 for r in rows), 'dynamic_instructions': instructions,
            'selected_branch_instructions': selected, 'reached_branch_instructions': reached, 'both_directions': both,
            'one_direction_only': one, 'unexecuted_branch_instructions': selected - reached, 'branch_families': kinds,
            'true_conditional_branches': conditional, 'instruction_constant_branches': constants,
            'conditional_both_directions': conditional_both, 'conditional_one_direction': conditional_one, 'conditional_unexecuted': conditional_zero}

def mutation_gate(rows, names, label, diagnostic='diagnostic', prefix='FAIL '):
    require(len(rows) == len(names) and {r['name'] for r in rows} == set(names), label + ': mutation membership')
    for row in rows:
        require(isinstance(row.get(diagnostic), str) and row[diagnostic].startswith(prefix) and
                not any(x in row[diagnostic].lower() for x in ('segmentation', 'addresssanitizer', 'runtime error:', 'compile failed')), label + ': mutation is not a behavioral assertion')
        for flag in ('detected', 'killed'):
            if flag in row:
                require(row[flag] is True, label + ': undetected mutation')
        if 'returncode' in row:
            require(row['returncode'] == 1, label + ': mutation process failure')

ACOS_COUNTS = {'status': 'pass', 'checks': 146223, 'actual_arm_arguments': 1250, 'unique_synthetic_and_actual_inputs': 13822, 'full_chains': 16608, 'primitive_cases': 16384, 'standalone_conversion_cases': 8704, 'exact_sqrt_cases': 512, 'independent_exact_cases': 2060, 'poisoned_chains': 1536, 'poison_patterns': 256, 'original_instructions': 191427455, 'original_distinct_pcs': 1345}
CONNECTED_COUNTS = {'49R_complete_calls': 96, 'checks': 1982364, 'frames': 96, 'full21A000_calls': 96, 'graph_nodes': 52, 'later_physics_pose_copies': 0, 'moving_chariot_local_node_events': 1027, 'moving_engine_local_node_events': 850, 'moving_engine_root_frames': 96, 'native_hot_allocations': 0, 'native_ik_calls': 192, 'palette_calls': 294, 'palette_entries': 46, 'passed': True, 'persistent_ik_initializations': 1, 'rigid_calls': 18, 'root21A660_calls': 0, 'source21A000_instructions': 4016776, 'source48W_instructions': 45744, 'source49R_instructions': 1224784, 'source_ik_calls': 192, 'source_noise_calls': 1152, 'source_palette_instructions': 338396, 'source_rng_calls': 1791, 'source_substitutions': 0}
CONSUMER_CONTROLS = {'wrong_bind_baked_input': 23020, 'wrong_double_root': 23020, 'wrong_float32_semantic_input': 1, 'wrong_palette_swap': 23020, 'wrong_stale_frame0_pose': 11258, 'wrong_whole_model_transform': 15440}
IK_COUNTS = {'runtime_cases': 805, 'initializers': 13, 'checks': 574344, 'original_instructions': 13602235, 'domain_error_calls': 314, 'lower_clamps': 67, 'native_allocations': 0, 'all_match': True}
FLOW_COUNTS = {'cases': 524, 'checks': 2996899, 'initializer_cases': 16, 'native_calls': 524, 'native_hot_allocations': 0, 'real_returns': 524, 'rejections': 13, 'source_ik_calls': 819, 'source_instructions': 16074526, 'source_substitutions': 0, 'stage_trace': True}
MOTION_ACCOUNT = {'frozen_words': 1538, 'reached_words': 1058, 'unreached_words_with_specific_reason': 480, 'reached_branches': 83, 'both_directions': 38, 'one_sided_with_specific_reason': 45, 'conditional_decode_includes_BGTZ': True, 'selected_BGTZ_words': 0, 'unaccounted_words_or_branch_directions': 0}
FLOW_ACCOUNT = {'frozen_words': 407, 'reached_words': 332, 'unreached_words_with_specific_reason': 75, 'branch_pcs': 23, 'both_directions': 20, 'one_sided_with_specific_reason': 3, 'unaccounted_words_or_branches': 0}
ACOS_MUTATIONS = ['domain_propagates_inner_nan', 'domain_errno_wrong', 'success_resets_errno', 'admits_wrong_mode', 'ignores_handler_contract', 'float_nan_host_marker', 'double_pack_round_down', 'float_pack_round_down', 'polynomial_coefficient_truncated', 'positive_df_keeps_low_word', 'sqrt_omits_final_rounding', 'negative_branch_uses_subtraction', 'narrow_loses_sticky_bit']
MOTION_MUTATIONS = ['omit_single_model18_zero', 'single_time_add', 'omit_pair_model18_zero', 'omit_pair_lazy_value', 'omit_interpolation_lazy_value', 'omit_pair_lazy_guard', 'omit_interpolation_lazy_guard', 'rotation_omit_fourth_zero', 'position_omit_z', 'rotation_ready_gate_inverted', 'position_ready_gate_inverted', 'ancestor_or_to_and', 'skip_sibling_invalidation', 'pair_position_subtract_to_add', 'reverse_pair_rotation', 'pair_endpoint_shortcut', 'pair_position_sign', 'omit_model18_zero', 'refresh_world_cache', 'reverse_track_channels', 'overwrite_translation_fourth', 'drop_scene_effects', 'warm_interpolation_clobber', 'warm_pair_clobber', 'pair_time_from_second', 'position_final_copy']
IK_MUTATIONS = ['lower_clamp_omitted', 'acos_domain_errno_store_omitted', 'projected_cosine_add_instead_of_multiply', 'rest_angle_subtracted', 'target_direction_normalization_omitted', 'shoulder_sqrt_replaced_with_move', 'target_orientation_up_reads_forward', 'constructor_reciprocal_replaced_with_product']
FLOW_MUTATIONS = ['omit_left_filter_value', 'skip_chariot_brakes', 'use_body_velocity', 'pose_filter_max_step', 'omit_pair', 'omit_left_ik', 'omit_right_ik', 'wrong_left_hand_target', 'omit_brake_samples', 'ignore_chariot_suffix_gate', 'filter_clamp_feedback', 'body_local_instead_of_cached_world', 'swap_pair_time_weight', 'omit_ik', 'reset_shared_orientation', 'wrong_B18_initial_seed', 'engines_before_chariot_brakes']
REVIEW_MUTATIONS = ['acos_domain', 'acos_cold_record_gate', 'acos_float_tie', 'acos_sqrt_round', 'filter_seed', 'filter_history_clamped', 'body_direction_basis', 'wrist_orientation_omitted', 'warm_shared_clobbered', 'sampler_eager_worlds', 'pair_uses_second_duration', 'sampler_late_publish', 'ik_late_publish']

def acos_gate(report):
    require(report['authentication'] == {'elf_sha256': ELF_SHA, 'source_words': 1510, 'data_words': 77, 'actual_arguments': 1250}, 'Acos authentication boundary')
    require(report['normal'] == ACOS_COUNTS and report['sanitizers'] == {'address': True, 'undefined': True, 'leak': False, 'result': ACOS_COUNTS}, 'Acos modes/counts/leak boundary')
    mutation_gate(report['controls'], ACOS_MUTATIONS, 'Acos', 'assertion')
    require(all(r['status'] == 'detected' for r in report['controls']), 'Acos mutation status')

def motion_gate(report, coverage, mode, words):
    traced = mode != 'plain'
    equal_fields(report, {'status': 'pass', 'ordered_tracing': traced, 'elf_sha256': ELF_SHA, 'source_words_verified': 1538, 'native_allocations': 0}, 'Motion result')
    require(report['mode'] == ('ASan+UBSan (LSan disabled)' if mode == 'sanitized' else 'normal'), 'Motion sanitizer boundary')
    strict_command(report['command'], mode == 'sanitized', '-std=c++17', 'flag_trace.h' if traced else False)
    expected = {'ordered_store_tracing': traced, 'cases': 1402, 'checks': 7295049 if traced else 7104963,
                'native_calls': 1439, 'original_instructions': 6853463, 'flag_stores_compared': 54776 if traced else 0, 'native_allocations': 0}
    equal_fields(coverage, expected, 'Motion coverage')
    account = coverage_gate(coverage['pcs'], words, 'Motion')
    require(account['reached_words'] == 1058 and account['dynamic_instructions'] == 6853463 and account['reached_branch_instructions'] == 83 and account['both_directions'] == 38, 'Motion source accounting')
    require(coverage['accounting'] == report['original_instruction_accounting'] == MOTION_ACCOUNT, 'Motion frozen accounting')
    names = MOTION_MUTATIONS if mode == 'normal' else []
    mutation_gate(report['sensitivity'], names, 'Motion', prefix='FAIL ModelMotion49T: ')
    require(report['source_mutations_detected'] == (14 if mode == 'normal' else 0) and report['native_mutations_detected'] == (12 if mode == 'normal' else 0), 'Motion sensitivity mode boundary')
    return account

def ik_gate(report, coverage, controls, words):
    equal_fields(report, {'status': 'passed', 'production_language': 'C++17', 'test_language': 'C++20', 'normal': IK_COUNTS,
                         'sanitized': IK_COUNTS, 'leak_sanitizer': 'disabled, not claimed', 'mutations': 8, 'source_elf_sha256': ELF_SHA}, 'IK result')
    expected = dict(IK_COUNTS, checks=720638)
    require(report['instrumented'] == expected and report['instrumented_flags'] == ['-DRR_CHARIOT_IK49T_TRACE', '-include', 'evidence/chariotAnimation49T/ik/source/flag_trace.h'], 'IK ordered store proof')
    rows = [r for b in coverage['blocks'] for r in b['pcs']]
    account = coverage_gate(rows, words, 'IK')
    equal_fields(coverage, {'frozen_words': 4174, 'executed_unique_words': 3059, 'dynamic_instructions': 13602235,
                           'unknown_executed_words': 0, 'all_conditional_branch_opcodes_accounted': True}, 'IK coverage')
    require(account['branch_families'] == coverage['branch_families'] and account['reached_words'] == 3059 and account['dynamic_instructions'] == 13602235, 'IK independently decoded accounting')
    classes = {}
    for r in rows:
        if not r['hits']:
            classes[r['coverage_class']] = classes.get(r['coverage_class'], 0) + 1
    require(classes == coverage['uncovered_classes'], 'IK exclusions arithmetic')
    equal_fields(controls, {'actual_mismatches': 8, 'setup_failures_counted': 0, 'working_tree_production_mutated': False}, 'IK mutation boundary')
    mutation_gate(controls['mutations'], IK_MUTATIONS, 'IK', 'actual_mismatch', '')
    for r in controls['mutations']:
        require('mismatch native=' in r['actual_mismatch'] and words[integer(r['pc'])] == integer(r['original']) and r['original'] != r['replacement'], 'IK mutation source/behavior')
    return account

def flow_gate(report, coverage, mode, words):
    traced = mode != 'plain'; expected = dict(FLOW_COUNTS, checks=2996899 if traced else 2992431, stage_trace=traced)
    equal_fields(report, {'status': 'pass', 'stage_trace': traced, 'result': expected,
        'source': {'elf_sha256': ELF_SHA, 'verified_flow_and_ik_words': 6036, 'assets_verified': 15},
        'production_standard': 'C++17', 'original_test_standard': 'C++20'}, 'Full flow result')
    require(report['mode'] == ('ASan+UBSan; LSan disabled' if mode == 'sanitized' else 'normal'), 'Full flow mode boundary')
    selected = {p: w for p, w in words.items() if 0x21a000 <= p < 0x21a65c}
    account = coverage_gate(coverage['pcs'], selected, 'Full flow')
    require(account['reached_words'] == 332 and account['reached_branch_instructions'] == 23 and account['both_directions'] == 20 and
            coverage['accounting'] == report['instruction_accounting'] == FLOW_ACCOUNT, 'Full flow source accounting')
    names = FLOW_MUTATIONS if traced else [n for n in FLOW_MUTATIONS if n != 'engines_before_chariot_brakes']
    mutation_gate(report['controls'], names, 'Full flow', prefix='FAIL vehicle_animation49t case=')
    require(report['source_controls_detected'] == 8 and report['native_controls_detected'] == (9 if traced else 8), 'Full flow mutation accounting')
    for row in report['controls']:
        require('expected=' in row['diagnostic'] or 'stage' in row['diagnostic'], 'Full flow behavioral diagnostic')
        if row['kind'] == 'original word':
            require(integer(row['pc']) in selected and selected[integer(row['pc'])] != integer(row['replacement']), 'Full flow source mutation')
    return account

def component_gates(sources):
    acos = load('acos/verification.json'); acos_gate(acos); pins(acos['inputs']); pins(load('acos/frozen-inputs.json'))
    rows = []
    for line in (E / 'acos/original-coverage.tsv').read_text().splitlines():
        pc, word, hits, taken, not_taken = line.split()
        rows.append(dict(pc=int(pc, 16), word=int(word, 16), hits=int(hits), taken=int(taken), not_taken=int(not_taken)))
    require(len(rows) == 1345 and len({r['pc'] for r in rows}) == 1345 and all(r['hits'] > 0 for r in rows), 'Acos TSV contains reached PCs exactly once')
    seen = {r['pc'] for r in rows}
    rows += [dict(pc=pc, word=word, hits=0, taken=0, not_taken=0) for pc, word in sources['acos'].items() if pc not in seen]
    acos_account = coverage_gate(rows, sources['acos'], 'Acos', False)
    require(acos_account['reached_words'] == 1345 and acos_account['dynamic_instructions'] == ACOS_COUNTS['original_instructions'], 'Acos raw source execution counts')
    manifest = load('acos/source-authentication.json')
    require(sha(E / 'acos/actual-arguments.tsv') == manifest['actual_arguments']['tsv_sha256'] and
            sha(E / 'ik/fixtures/research/assessment.json') == manifest['actual_arguments']['assessment_sha256'], 'Actual arm acos argument provenance')
    vectors = load('acos/exact-vectors.json')
    require(vectors == {'method': 'Fraction exact arithmetic, nearest-even integer rounding, integer isqrt', 'arithmetic': 1036, 'widen': 256, 'narrow': 256, 'sqrt': 512, 'sha256': sha(E / 'acos/exact_vectors.h')}, 'Independent exact numeric vectors')
    accounts = {'acos': acos_account}; motion_rows = []; flow_rows = []
    for mode in ('normal', 'sanitized', 'plain'):
        report, coverage = load(f'motion/{mode}/native-test-result.json'), load(f'motion/{mode}/coverage.json')
        accounts['motion'] = motion_gate(report, coverage, mode, sources['motion']); pins(report['artifacts']); motion_rows.append(coverage['pcs'])
        require((E / f'motion/{mode}/model_motion49t.stdout').read_text().strip() == report['native_result'] and not (E / f'motion/{mode}/model_motion49t.stderr').read_bytes(), 'Motion raw stdout/stderr')
        for r in report['sensitivity']:
            prefix = 'source_' if r['kind'] == 'original-word substitution' else 'native_'
            require((E / f'motion/{mode}/{prefix}{r["name"]}.stderr').read_text().strip() == r['diagnostic'], 'Motion raw sensitivity')
        report, coverage = load(f'flow/{mode}/result.json'), load(f'flow/{mode}/coverage.json')
        accounts['flow'] = flow_gate(report, coverage, mode, sources['flow']); flow_rows.append(coverage['pcs'])
        require(json.loads((E / f'flow/{mode}/test.stdout').read_text()) == report['result'] and not (E / f'flow/{mode}/test.stderr').read_bytes(), 'Full flow raw stdout/stderr')
        for r in report['controls']:
            prefix = 'source_' if r['kind'] == 'original word' else 'native_'
            require((E / f'flow/{mode}/{prefix}{r["name"]}.stderr').read_text().strip() == r['diagnostic'], 'Full flow raw sensitivity')
    require(motion_rows[0] == motion_rows[1] == motion_rows[2] and flow_rows[0] == flow_rows[1] == flow_rows[2], 'Source coverage diverges by mode')
    ik, coverage, controls = load('ik/validation/validation.json'), load('ik/validation/coverage-accounting.json'), load('ik/validation/mutation-controls.json')
    accounts['ik'] = ik_gate(ik, coverage, controls, sources['ik']); pins(ik['files']); local_pins('ik/validation', ik['outputs'])
    for filename, result in [('normal.stdout', IK_COUNTS), ('asan-ubsan.stdout', IK_COUNTS), ('ordered-flag-stores.stdout', ik['instrumented'])]:
        require(json.loads((E / 'ik/validation' / filename).read_text()) == result and not (E / 'ik/validation' / filename.replace('.stdout', '.stderr')).read_bytes(), 'IK raw runtime proof')
    raw_ik = {int(p, 16): (int(h), int(t), int(n)) for p, h, t, n in (line.split() for line in (E / 'ik/validation/coverage.tsv').read_text().splitlines())}
    require(raw_ik == {integer(r['pc']): (r['hits'], r['taken'], r['not_taken']) for b in coverage['blocks'] for r in b['pcs']}, 'IK raw coverage differs')
    for r in controls['mutations']:
        require((E / f'ik/validation/mutation-{r["name"]}.stderr').read_text().strip() == r['actual_mismatch'], 'IK raw mutation mismatch')
    flow_manifest = load('flow/proof-manifest.json'); pins(flow_manifest['files'])
    equal_fields(flow_manifest, {'status': 'pass', 'elf_sha256': ELF_SHA, 'final_acos_sha256': ACOS_SHA, 'native_hot_allocations': 0, 'inherited_file_edits': 0}, 'Full-flow source seal')
    for mode in ('normal', 'sanitized', 'plain'):
        report = load(f'flow/{mode}/result.json')
        require(flow_manifest['modes'][mode] == {'result': report['result'], 'controls_detected': len(report['controls']), 'instruction_accounting': report['instruction_accounting']}, 'Full-flow sealed mode')
    accounts['acos_exclusions'] = acos_exclusion_gate(sources['acos'], rows, acos_account)
    return accounts

def connected_gate(report, export):
    require(report['passed'] is True and report['leak_sanitizer_used'] is False and set(report['runs']) == {'normal', 'asan-ubsan', 'plain'}, 'Connected modes/status')
    equal_fields(export, {'passed': True, 'schema': 'chariot49t.export.v1', 'summary': CONNECTED_COUNTS, 'consumer_controls': CONSUMER_CONTROLS,
                         'engine_mesh_bytes_match49S': {str(i): True for i in (0, 31, 63, 95)}}, 'Connected proof')
    for mode, run in report['runs'].items():
        equal_fields(run, {'summary': CONNECTED_COUNTS, 'consumer_controls': CONSUMER_CONTROLS, 'engine_mesh_bytes_match49S': export['engine_mesh_bytes_match49S'],
                          'read_only_native_cli': mode == 'plain', 'trace_sha256': export['trace_sha256'], 'exact_goldens_sha256': export['exact_goldens_sha256']}, 'Connected run')
        strict_command(run['flags'], mode == 'asan-ubsan', '-std=c++20', False if mode == 'plain' else 'stage_trace.h')
        require(run['mesh_sha256'] == {r['mesh_path']: r['mesh_sha256'] for r in export['frames']}, 'Connected mode meshes diverge')
    rows = export['frames']
    require(len(rows) == 6 and {(r['frame'], r['label']) for r in rows} == FRAME_KEYS, 'Connected frame membership')
    for row in rows:
        equal_fields(row, {'vertices': 5755, 'indices': 10713, 'component_vertices': {'chariot': 2884, 'left_engine': 1411, 'right_engine': 1460}}, 'Connected vertex counts')
        require(math.isfinite(row['elapsed_seconds']) and 0 < row['elapsed_seconds'] < 10, 'Connected elapsed fixture time')
        require(row['pose_source'] == ('actual21A000-internal-animation-49R' if row['label'] == 'full-motion' else 'counterfactual-display-setters'), 'Connected pose label')
        require(row['changed_vertices_vs_full'] == {'chariot': {'full-motion': 0, 'frozen-chariot-locals': 832, 'ik-omitted': 149}[row['label']], 'left_engine': 0, 'right_engine': 0}, 'Connected control effects')
    mutations = [dict(row, name=name) for name, row in report['sensitivity'].items()]
    mutation_gate(mutations, ['native_omit_both_ik', 'native_reverse_engine_order', 'source_omit_lateral_negation', 'source_pose_filter_add_instead_of_multiply'], 'Connected', prefix='FAIL chariot49T frame=')

def mesh_gate(data, baseline):
    require(len(data) == len(baseline) == 180988 and data[:16] == baseline[:16] == b'RRMESH01' + struct.pack('<II', 5755, 10713), 'Mesh header/length')
    require(data[16 + 5755 * 24:] == baseline[16 + 5755 * 24:], 'Mesh index bytes changed')
    changes = {'chariot': 0, 'left_engine': 0, 'right_engine': 0}
    for i in range(5755):
        at = 16 + i * 24
        require(data[at + 12:at + 24] == baseline[at + 12:at + 24], 'Mesh UV/color bytes changed')
        require(all(math.isfinite(x) for x in struct.unpack_from('<3f', data, at)), 'Nonfinite mesh positions')
        changes['chariot' if i < 2884 else 'left_engine' if i < 4295 else 'right_engine'] += data[at:at + 12] != baseline[at:at + 12]
    return changes

def connected_gates():
    report, export = load('connected/verification.json'), load('connected/export-report.json'); connected_gate(report, export)
    for value in (report, export):
        pins(value['source_sha256']); pins(value['input_sha256'])
    require(report['source_sha256'] == export['source_sha256'] and report['input_sha256'] == export['input_sha256'], 'Connected provenance divergence')
    trace = load('connected/trace.json')
    require(sha(E / 'connected/trace.json') == export['trace_sha256'] and trace['summary'] == CONNECTED_COUNTS and
            len(trace['frames']) == 96 and [r['frame'] for r in trace['frames']] == list(range(96)) and len(trace['display_controls']) == 2, 'Connected raw trace')
    for row in trace['frames'] + trace['display_controls']:
        for key, count in [('chariot_palette', 18), ('left_palette', 14), ('right_palette', 14)]:
            require(row[key]['entry_count'] == len(row[key]['node_world_bits']) == count and all(len(r) == 16 for r in row[key]['node_world_bits']), 'Connected complete source palette')
    require(sha(E / 'connected/exact-goldens49t.txt') == export['exact_goldens_sha256'], 'Connected exact goldens')
    require(load('connected/connected-sensitivity.json') == {'passed': True, 'mutations': report['sensitivity']}, 'Connected sensitivity artifact')
    for mode in ('normal', 'asan-ubsan', 'plain'):
        require(json.loads((E / f'connected/{mode}-run.stdout.log').read_text()) == CONNECTED_COUNTS and not (E / f'connected/{mode}-run.stderr.log').read_bytes(), 'Connected raw mode output')
    base = current_path(export['baseline_mesh_path']).read_bytes(); full = {}
    for row in export['frames']:
        data = (E / 'connected' / row['mesh_path']).read_bytes()
        require(digest(data) == row['mesh_sha256'] and mesh_gate(data, base) == row['changed_vertices_vs49H'], 'Connected mesh pin/counts')
        if row['label'] == 'full-motion':
            full[row['frame']] = data
            inherited = ROOT / f'evidence/modelPalette49S/connected/meshes/frame-{row["frame"]:03d}-full-motion.rrmesh'
            require(data[16 + 2884 * 24:] == inherited.read_bytes()[16 + 2884 * 24:], 'Inherited engine mesh bytes differ')
        else:
            require(mesh_gate(data, full[95]) == row['changed_vertices_vs_full'], 'Connected counterfactual mesh isolation')
    for name in ['trace.json', 'exact-goldens49t.txt', 'export-report.native.json'] + [r['mesh_path'] for r in export['frames']]:
        require((E / 'connected' / name).read_bytes() == (E / 'connected/asan-ubsan' / name).read_bytes(), 'Connected sanitizer byte replay: ' + name)
    return export

def independent_gate(report):
    expected = {'acos': {'status': 'passed', 'checks': 879125, 'normal_arithmetic_vectors': 40000, 'monotone_positive_inputs': 65537, 'source_oracle_used': False},
                'animation': {'status': 'passed', 'checks': 379, 'flow_calls': 13, 'late_atomic_failures': 4, 'source_oracle_used': False}}
    equal_fields(report, {'status': 'passed', 'normal': expected, 'sanitized': expected, 'LSan': False, 'mutation_count': 13,
                         'authored_corpus': {'poses_verified': 625, 'asset_members_verified': 15, 'sampler_cross_product_not_control_reachability': True}}, 'Independent review')
    strict_command(report['flags'], False, '-std=c++17', False)
    mutation_gate(report['mutations'], REVIEW_MUTATIONS, 'Independent review', 'assertion', 'FAIL after ')

def independent_gates():
    report = load('independent-review/results/verification.json'); independent_gate(report)
    pins(report['input_hashes']); local_pins('independent-review', report['review_harness_hashes'])
    expected = {'acos/source-authentication.json': 6348, 'motion/source-manifest.json': 6152, 'ik/source/source-manifest.json': 17000, 'flow/source/source-manifest.json': 7448}
    require(len(report['source_authentication']) == len(expected), 'Independent source authentication membership')
    for row in report['source_authentication']:
        require(row['file_backed_bytes_checked'] == expected[row['manifest']] and row['sha256'] == sha(E / row['manifest']), 'Independent source authentication pin')
    for mode in ('normal', 'sanitized'):
        for suite in ('acos', 'animation'):
            require(json.loads((E / f'independent-review/results/{suite}-{mode}.stdout').read_text()) == report[mode][suite] and not (E / f'independent-review/results/{suite}-{mode}.stderr').read_bytes(), 'Independent raw result')
    for row in report['mutations']:
        require((E / f'independent-review/results/mutation-{row["name"]}.stderr').read_text().strip() == row['assertion'], 'Independent raw mutation')
    return {'acos_checks_each': 879125, 'animation_checks_each': 379, 'semantic_mutations': 13}


def plan_gate(plan, export, frame, profile):
    require(plan['schema'] == 'chariot49t.visual-plan.v1' and plan['diagnostic_only'] is True and
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
    require(plan['profile']['native_chariot_and_engine_animation'] is (profile == 'full-motion') and
            plan['profile']['counterfactual_display_control'] is (profile != 'full-motion') and
            plan['profile']['original_material_programs'] is False and plan['profile']['original_lighting_or_visibility'] is False,
            'Render interpretation boundary')
    require(plan['uploaded_vehicle_geometry']['chariot_vertices'] == 2884 and
            plan['uploaded_vehicle_geometry']['uv_color_indices_unchanged'] is True and
            plan['uploaded_vehicle_geometry']['engine_vertices'] == 2871, 'Render geometry preservation')
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
    require(plan['renderer49T_sha256'] == sha(ROOT / 'tools/visual/render_chariot49t.py'), 'Render current script pin')
    payload = dict(plan); expected_hash = payload.pop('plan_sha256')
    require(digest(json.dumps(payload, sort_keys=True, separators=(',', ':'), allow_nan=False).encode()) == expected_hash, 'Render canonical plan hash')


def frame_gate(report, plan):
    equal_fields(report, {'status': 'rendered_diagnostic_only', 'schema': 'chariot49t.visual-report.v1',
        'actual_draw_calls': 360, 'actual_submitted_triangles': 73409, 'actual_texture_uploads': 26,
        'actual_gl_buffer_uploads': 4, 'shared_mesh_sets': 2, 'uniform_bits_round_trip': True,
        'framebuffer_unchanged_by_caption': True, 'opaque_output_alpha': True}, 'Rendered result')
    require(report['plan'] == plan and len(report['images']) == 2 and len(report['texture_state_checks']) == 26 and
            all(r['parameters_and_level_zero_verified'] is True for r in report['texture_state_checks']), 'Rendered plan/texture verification')
    require(report['runtime']['cpu_affinity'] == [0] and report['runtime']['software_environment']['LP_NUM_THREADS'] == '1',
            'Renderer process constraints')
    require(report['pixel_rgba8_sha256'] == report['images'][0]['rgba8_sha256'], 'Rendered pixel hash marker')


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
    for i, profile in enumerate(['full-motion', 'frozen-chariot-locals', 'ik-omitted']):
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

def png_gate(row):
    from PIL import Image
    pins([row]); path = current_path(row['path'])
    with Image.open(path) as source:
        rgba = source.convert('RGBA')
        require(digest(rgba.tobytes()) == row['rgba8_sha256'], 'Decoded PNG RGBA pin')
        if 'dimensions' in row:
            require(list(rgba.size) == row['dimensions'], 'PNG dimensions')
        return rgba.copy()

def render_gates(export):
    passes = ['pass-1', 'pass-2']; canonical = {}; pixels = {}; records = []
    for directory in passes:
        for frame, profile in sorted(FRAME_KEYS):
            stem = f'frame-{frame:03d}-{profile}'
            report = load(f'frames/{directory}/{stem}.json'); plan = report['plan']
            plan_gate(plan, export, frame, profile); frame_gate(report, plan)
            images = [png_gate(row) for row in report['images']]
            require([row['kind'] for row in report['images']] == ['raw framebuffer', 'captioned diagnostic'], 'Render image type/order')
            require(images[0].size == (640, 480) and images[1].size == (640, 552) and
                    images[1].crop((0, 0, 640, 480)).tobytes() == images[0].tobytes(), 'Caption changes framebuffer pixels')
            key = (frame, profile)
            pair = (plan['plan_sha256'], tuple(row['sha256'] for row in report['images']), tuple(image.tobytes() for image in images))
            if key in canonical:
                require(canonical[key] == pair, 'Repeated render plan/PNG/RGBA differs')
            else:
                canonical[key] = pair; pixels[stem] = images[0].tobytes()
            records.append({'pass': directory, 'frame': frame, 'profile': profile, 'plan_sha256': plan['plan_sha256'], 'pixel_rgba8_sha256': report['pixel_rgba8_sha256']})
    require(len(set(pixels.values())) == 6, 'Native animation/control images have no distinct framebuffer effect')
    reference = pixels['frame-095-full-motion']; comparisons = []
    order = [f'frame-{i:03d}-full-motion' for i in (0, 31, 63, 95)] + ['frame-095-frozen-chariot-locals', 'frame-095-ik-omitted']
    for name in order:
        changed = [i for i in range(640 * 480) if pixels[name][4 * i:4 * i + 4] != reference[4 * i:4 * i + 4]]
        bbox = [min(i % 640 for i in changed), min(i // 640 for i in changed), 1 + max(i % 640 for i in changed), 1 + max(i // 640 for i in changed)] if changed else None
        comparisons.append({'name': name, 'pixels_changed_vs95': len(changed), 'bbox': bbox})
    require(load('frames/pixel-verification.json') == {'status': 'passed', 'repeated_raw_and_caption_png_pairs': 12,
            'caption_framebuffer_pixels_preserved': True, 'comparisons': comparisons}, 'Raw pixel comparison report differs')
    require(comparisons[-2]['pixels_changed_vs95'] == 914 and comparisons[-1]['pixels_changed_vs95'] == 180, 'Displayed Chariot/IK controls lost pixel sensitivity')
    return {'authoritative_passes': passes, 'frames': records, 'pixel_comparisons': comparisons, 'repeat_plan_png_rgba_exact': True}

def detail_presentation_gate():
    from PIL import Image
    report = load('frames/presentation/detail-presentation.json')
    equal_fields(report, {'status': 'passed', 'crop_box': [328, 240, 440, 312], 'nearest_integer_scale': 4, 'draws_performed': 0}, 'Close-up presentation')
    expected = [f'evidence/chariotAnimation49T/frames/pass-1/frame-095-{profile}.raw.png' for profile in ('full-motion', 'frozen-chariot-locals', 'ik-omitted')]
    require(set(report['source_pngs']) == set(expected) and len(report['images']) == 1, 'Close-up source membership')
    pins(report['source_pngs']); output = png_gate(report['images'][0])
    require(output.size == (1344, 342), 'Close-up canvas dimensions')
    for i, name in enumerate(expected):
        with Image.open(ROOT / name) as image:
            crop = image.convert('RGBA').crop(tuple(report['crop_box'])).resize((448, 288), Image.Resampling.NEAREST)
            require(output.crop((i * 448, 54, (i + 1) * 448, 342)).tobytes() == crop.tobytes(), 'Close-up exact nearest-pixel copy')
    return {'four_times_control_crops_source_pixel_exact': True}


def check_count(text):
    try:
        return int(json.loads(text)['checks'])
    except (ValueError, KeyError):
        match = re.search(r'checks=(\d+)', text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks', text)
        require(match is not None, 'Missing stdout check count')
        return int(match.group(1))

def previous_report():
    return json.loads((ROOT / 'evidence/modelPalette49S/final-normal/regression-report.json').read_text())

def report_gate(report, mode):
    sanitized = mode == 'sanitized'; old = previous_report()
    flags = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-fno-fast-math', '-frounding-math', '-ffp-contract=off']
    flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-sanitize-recover=all'] if sanitized else ['-O2']
    require(type(report['stack_usage_requested']) is bool, 'Aggregate stack request type')
    if report['stack_usage_requested']:
        flags += ['-fstack-usage']
    require(bool(report.get('stack_usage', [])) is report['stack_usage_requested'], 'Aggregate stack evidence/request mismatch')
    for row in report.get('stack_usage', []):
        require(digest(row['text'].encode()) == row['sha256'] and len(row['text'].encode()) == row['bytes'], 'Aggregate embedded stack evidence')
    require(report['status'] == 'passed' and not report['errors'] and not report['changed_inputs'] and report['scope'] == 'all_chariot-animation_checks' and report['serial'] is True, 'Aggregate status/scope')
    require(report['compile_flags'] == flags and report['link_flags'] == (['-fsanitize=address,undefined'] if sanitized else []) and
            report['sanitizers'] is sanitized and report['leak_detection'] is False, 'Aggregate flags/LSan boundary')
    suites = set(old['selected_tests']) | NEW_SUITES
    require(len(report['tests']) == len(report['selected_tests']) == 56 and {r['name'] for r in report['tests']} == set(report['selected_tests']) == suites, 'Aggregate suite membership')
    require(len(report['build']) == 50 and {r['source'] for r in report['build']} == {r['source'] for r in old['build']} | NEW_UNITS, 'Aggregate production units')
    registered = re.search(r'set\(rr_chariot_animation_tests\s+([^)]*)\)', (ROOT / 'cmake/NativeChariotAnimation49T.cmake').read_text())
    require(registered and set(registered.group(1).split()) == suites, 'CMake aggregate suite registration')
    expected = set(old['input_hashes']) | set(load('connected/export-report.json')['input_sha256']) | set(load('ik/validation/validation.json')['files'])
    expected.update({'tools/test_native_chariot_animation49t.py', 'cmake/NativeChariotAnimation49T.cmake', 'tools/visual/export_chariot49t.cpp'})
    expected.update('evidence/chariotAnimation49T/connected/' + name for name in ['export-report.json', 'export-report.native.json', 'trace.json', 'exact-goldens49t.txt'])
    expected.update('evidence/chariotAnimation49T/connected/' + r['mesh_path'] for r in load('connected/export-report.json')['frames'])
    expected.update(NEW_UNITS)
    expected.update({'src/game/model_motion49t.h', 'src/game/vehicle_animation49t.h', 'evidence/chariotAnimation49T/motion/blocks.h', 'evidence/chariotAnimation49T/motion/motion_machine.h', 'evidence/chariotAnimation49T/flow/source/flow_words.h', 'evidence/chariotAnimation49T/flow/flow_fixture.h'})
    expected.update('tests/native/' + n + '.cpp' for n in NEW_SUITES if n != 'chariot_export49t')
    require(set(report['input_hashes']) == expected, 'Aggregate source/input membership')
    for name, value in old['input_hashes'].items():
        if name != 'CMakeLists.txt':
            require(report['input_hashes'][name] == value, 'Inherited aggregate input changed: ' + name)
    if sanitized:
        require('detect_leaks=0' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['UBSAN_OPTIONS'], 'Aggregate sanitizer environment')
    for build in report['build'] + [r['build'] for r in report['tests']]:
        require(build['status'] == 'passed' and build['returncode'] == 0 and build['command'][1:1 + len(flags)] == flags, 'Aggregate compile status/flags')
        strict_command(build['command'], sanitized, '-std=c++20', False)
        require(build['stdout']['bytes'] == 0 and build['stdout']['sha256'] == EMPTY_SHA, 'Aggregate build stdout')
        if build.get('source') in NEW_UNITS:
            require(build['stderr']['bytes'] == 0 and build['stderr']['sha256'] == EMPTY_SHA, 'New production compiler warning')
    for test in report['tests']:
        run = test['run']
        require(test['status'] == run['status'] == 'passed' and run['returncode'] == 0 and run['command'][0] == test['executable']['path'], 'Aggregate runtime status')
        require(run['stderr']['bytes'] == 0 and run['stderr']['sha256'] == EMPTY_SHA, 'Aggregate runtime stderr')

def regression_gates():
    summary = load('regression-summary.json')
    equal_fields(summary, {'status': 'passed', 'all_mode_outputs_identical': True, 'semantic_trace_identical_to_focused': True,
        'connected_replay_exact_bytes': True, 'all_52_inherited_suite_outputs_identical': True, 'production_units_each': 50, 'checks_each': 166851429,
        'trace_checks': {'aggregate': CONNECTED_COUNTS['checks'], 'focused': CONNECTED_COUNTS['checks']}}, 'Aggregate summary')
    old = json.loads((ROOT / 'evidence/modelPalette49S/regression-summary.json').read_text())['runs']['normal']['tests']
    # Default aggregate CLIs omit one optional coverage-file assertion in motion and flow.
    reports = {}; output_checks = {'model_motion49t': 7104962, 'chariot_ik49t': 574344, 'vehicle_animation49t': 2992430, 'chariot_export49t': 1982364}
    for mode in ('normal', 'sanitized'):
        row = summary['runs'][mode]; report = load(f'final-{mode}/regression-report.json'); reports[mode] = report
        report_gate(report, mode); pins(report['input_hashes'])
        equal_fields(row, {'suites': 56, 'inputs_unchanged': True, 'tracked_inputs': len(report['input_hashes'])}, 'Aggregate mode summary')
        require(row['report_sha256'] == sha(E / f'final-{mode}/regression-report.json') and len(row['tests']) == 56, 'Aggregate report pin/test count')
        require([r for r in row['tests'] if r['name'] not in NEW_SUITES] == old, 'All 52 inherited stdout/check summaries differ')
        total = 0
        for test in row['tests']:
            actual = next(r for r in report['tests'] if r['name'] == test['name'])['run']
            data = (E / f'final-{mode}/{test["name"]}.stdout').read_bytes()
            require(digest(data) == test['stdout_sha256'] == actual['stdout']['sha256'] and len(data) == actual['stdout']['bytes'] and
                    test['stderr_bytes'] == 0 and check_count(data.decode()) == test['checks'], 'Aggregate raw stdout bytes/checks')
            if test['name'] in NEW_SUITES:
                require(test['checks'] == output_checks[test['name']], 'Aggregate uninstrumented new-suite checks')
            else:
                require(data == (ROOT / f'evidence/modelPalette49S/final-normal/{test["name"]}.stdout').read_bytes(), 'Inherited stdout bytes changed')
            total += test['checks']
        require(row['checks'] == summary['checks_each'] == total, 'Aggregate checks arithmetic')
        names = []
        for process in report['build'] + [t[k] for t in report['tests'] for k in ('build', 'run')]:
            for stream in ('stdout', 'stderr'):
                meta = process[stream]; name = Path(meta['path']).name; path = E / f'final-{mode}/logs/{name}'
                require(sha(path) == meta['sha256'] and path.stat().st_size == meta['bytes'] and row['process_logs'][name] == {'bytes': meta['bytes'], 'sha256': meta['sha256']}, 'Aggregate durable process log')
                names.append(name)
        require(len(names) == len(set(names)) == 324 and set(names) == set(row['process_logs']), 'Aggregate process log membership')
        expected = {'chariot/' + n for n in ['trace.json', 'exact-goldens49t.txt', 'export-report.native.json']} | {'chariot/' + r['mesh_path'] for r in load('connected/export-report.json')['frames']}
        require(set(row['replayed_files']) == expected, 'Aggregate connected replay membership')
        for name, pin in row['replayed_files'].items():
            data = (E / f'final-{mode}' / name).read_bytes()
            require(digest(data) == pin and data == (E / 'connected' / name[len('chariot/'):]).read_bytes(), 'Aggregate connected replay bytes')
        require(json.loads((E / f'final-{mode}/chariot_export49t-output.stdout').read_text()) == CONNECTED_COUNTS, 'Aggregate connected replay counters')
    require(summary['runs']['normal']['tests'] == summary['runs']['sanitized']['tests'] and summary['runs']['normal']['replayed_files'] == summary['runs']['sanitized']['replayed_files'] and reports['normal']['input_hashes'] == reports['sanitized']['input_hashes'], 'Aggregate normal/sanitizer divergence')
    return summary

def baseline_gate(path):
    require(path.is_file() and not path.is_symlink() and sha(path) == BASELINE_SHA, 'Expected exact verified 49S baseline archive')
    changed = []
    with zipfile.ZipFile(path) as archive:
        names = [row.filename for row in archive.infolist() if not row.is_dir()]
        safe_names(names); require(archive.testzip() is None, 'Baseline ZIP CRC failure')
        for name in names:
            file = ROOT.parent / name
            require(file.is_file() and not file.is_symlink() and file.resolve().is_relative_to(ROOT.parent.resolve()), 'Missing or unsafe baseline member: ' + name)
            if sha(file) != digest(archive.read(name)):
                changed.append(name)
    require(set(changed) == ALLOWED, 'Unexpected inherited file changes: ' + repr(changed))
    baseline_names = set(names)
    for file in ROOT.parent.rglob('*'):
        if file.is_file() and not file.is_relative_to(ROOT):
            require(file.relative_to(ROOT.parent).as_posix() in baseline_names, 'New file outside sealed project tree')
    return names, sorted(changed)

def seal_hashes(baseline_names):
    baseline_names = set(baseline_names); result = {}
    for file in sorted(ROOT.rglob('*')):
        if file.is_symlink():
            require((ROOT.name + '/' + file.relative_to(ROOT).as_posix()) in baseline_names, 'New symlink cannot be sealed')
        if not file.is_file() or file == E / 'checkpoint-validation.json':
            continue
        name = file.relative_to(ROOT).as_posix(); member = ROOT.name + '/' + name
        if member not in baseline_names or member in ALLOWED:
            safe_names([name]); require(not file.is_symlink(), 'New symlink cannot be sealed')
            result[name] = sha(file)
    return result

def gate_controls(sources, export, aggregate=True, resources=None):
    rows = []
    def control(name, original, mutate, gate):
        value = copy.deepcopy(original); gate(value); mutate(value)
        try:
            gate(value)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Negative release control escaped: ' + name)
    acos = load('acos/verification.json')
    control('acos_false_leak_claim', acos, lambda x: x['sanitizers'].update(leak=True), acos_gate)
    control('acos_missing_poison_cases', acos, lambda x: x['normal'].update(poisoned_chains=0), acos_gate)
    control('acos_compile_failure_as_mutation', acos, lambda x: x['controls'][0].update(assertion='compile failed'), acos_gate)
    control('acos_missing_mutation', acos, lambda x: x['controls'].pop(), acos_gate)
    acos_rows = []
    for line in (E / 'acos/original-coverage.tsv').read_text().splitlines():
        pc, word, hits, taken, not_taken = line.split()
        acos_rows.append(dict(pc=int(pc, 16), word=int(word, 16), hits=int(hits), taken=int(taken), not_taken=int(not_taken)))
    seen = {r['pc'] for r in acos_rows}
    acos_rows += [dict(pc=pc, word=word, hits=0, taken=0, not_taken=0) for pc, word in sources['acos'].items() if pc not in seen]
    bgtz = next(i for i, r in enumerate(acos_rows) if r['word'] >> 26 == 7 and r['hits'])
    control('acos_original_BGTZ_direction_lost', acos_rows, lambda x: x[bgtz].update(taken=0, not_taken=0), lambda x: coverage_gate(x, sources['acos'], 'Acos', False))
    constant = next(i for i, r in enumerate(acos_rows) if static_branch_outcome(r['word']) is True and r['taken'])
    control('acos_impossible_constant_branch', acos_rows, lambda x: x[constant].update(taken=0, not_taken=x[constant]['hits']), lambda x: coverage_gate(x, sources['acos'], 'Acos', False))

    for mode in ('normal', 'sanitized', 'plain'):
        original = [load(f'motion/{mode}/native-test-result.json'), load(f'motion/{mode}/coverage.json')]
        gate = lambda x, m=mode: motion_gate(*x, m, sources['motion'])
        control(mode + '_motion_hot_allocation', original, lambda x: x[1].update(native_allocations=1), gate)
        control(mode + '_motion_source_word_changed', original, lambda x: x[1]['pcs'][0].update(word='00000000'), gate)
        branch = next(i for i, r in enumerate(original[1]['pcs']) if r['taken'] or r['not_taken'])
        control(mode + '_motion_branch_lost', original, lambda x, i=branch: x[1]['pcs'][i].update(taken=0, not_taken=0), gate)
        original = [load(f'flow/{mode}/result.json'), load(f'flow/{mode}/coverage.json')]
        gate = lambda x, m=mode: flow_gate(*x, m, sources['flow'])
        control(mode + '_flow_fake_full_return', original, lambda x: x[0]['result'].update(real_returns=0), gate)
        control(mode + '_flow_source_substitution', original, lambda x: x[0]['result'].update(source_substitutions=1), gate)
        control(mode + '_flow_missing_control', original, lambda x: x[0]['controls'].pop(), gate)
    ik = [load('ik/validation/validation.json'), load('ik/validation/coverage-accounting.json'), load('ik/validation/mutation-controls.json')]
    gate = lambda x: ik_gate(*x, sources['ik'])
    control('ik_store_tracing_missing', ik, lambda x: x[0]['instrumented'].update(checks=574344), gate)
    control('ik_setup_failure_as_parity', ik, lambda x: x[2].update(setup_failures_counted=1), gate)
    control('ik_missing_original_pc', ik, lambda x: x[1]['blocks'][0]['pcs'].pop(), gate)
    connected = [load('connected/verification.json'), export]
    gate = lambda x: connected_gate(*x)
    control('connected_source_substitution', connected, lambda x: x[1]['summary'].update(source_substitutions=1), gate)
    control('connected_missing_display_control', connected, lambda x: x[1]['frames'].pop(), gate)
    control('connected_control_mislabeled', connected, lambda x: x[1]['frames'][-1].update(pose_source='actual21A000-internal-animation-49R'), gate)
    control('connected_branch_cli_trace_leak', connected, lambda x: x[0]['runs']['plain']['flags'].extend(['-include', 'stage_trace.h']), gate)
    control('connected_mutant_crash_counted', connected, lambda x: next(iter(x[0]['sensitivity'].values())).update(returncode=-11), gate)
    review = load('independent-review/results/verification.json')
    control('review_source_oracle_used', review, lambda x: x['normal']['acos'].update(source_oracle_used=True), independent_gate)
    control('review_missing_semantic_mutation', review, lambda x: x['mutations'].pop(), independent_gate)
    baseline = current_path(export['baseline_mesh_path']).read_bytes()
    mesh = bytearray((E / 'connected' / export['frames'][0]['mesh_path']).read_bytes())
    for name, at in [('chariot_uv', 16 + 12), ('engine_color', 16 + 2884 * 24 + 20), ('mesh_index', 16 + 5755 * 24)]:
        control(name + '_changed', mesh, lambda x, a=at: x.__setitem__(a, x[a] ^ 1), lambda x: mesh_gate(x, baseline))
    rendered = load('frames/pass-2/frame-095-full-motion.json'); plan = rendered['plan']
    gate = lambda x: plan_gate(x, export, 95, 'full-motion')
    control('render_missing_draw', plan, lambda x: x.update(total_draw_calls=359), gate)
    control('render_stale_export', plan, lambda x: x['native_export'].update(sha256='0' * 64), gate)
    control('render_missing_source_pin', plan, lambda x: x['native_export']['verified_pins'].pop(next(iter(x['native_export']['verified_pins']))), gate)
    control('render_original_lighting_claim', plan, lambda x: x['profile'].update(original_lighting_or_visibility=True), gate)
    control('render_caption_pixel_change', rendered, lambda x: x.update(framebuffer_unchanged_by_caption=False), lambda x: frame_gate(x, plan))
    control('render_false_pixel_hash', rendered, lambda x: x.update(pixel_rgba8_sha256='0' * 64), lambda x: frame_gate(x, plan))
    if aggregate:
        for mode in ('normal', 'sanitized'):
            report = load(f'final-{mode}/regression-report.json'); gate = lambda x, m=mode: report_gate(x, m)
            control(mode + '_aggregate_hidden_failed_return', report, lambda x: x['tests'][0]['run'].update(returncode=1), gate)
            control(mode + '_aggregate_missing_suite', report, lambda x: x['tests'].pop(), gate)
            control(mode + '_aggregate_fast_math', report, lambda x: x['build'][0]['command'].append('-ffast-math'), gate)
            control(mode + '_aggregate_trace_hook', report, lambda x: x['build'][0]['command'].extend(['-include', 'stage_trace.h']), gate)
            control(mode + '_aggregate_changed_input', report, lambda x: x['changed_inputs'].append('src/game/source_acos49t.cpp'), gate)
            control(mode + '_aggregate_runtime_stderr', report, lambda x: x['tests'][0]['run']['stderr'].update(bytes=1), gate)
    if resources:
        resource_controls(resources, control)
    for name, values in [('duplicate_path', ['a', 'a']), ('parent_traversal', ['../a']), ('absolute_path', ['/a']), ('windows_path', ['a\\b'])]:
        try:
            safe_names(values)
        except GateError as error:
            rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else:
            raise GateError('Negative path control escaped')
    return {'status': 'passed', 'method': 'Decoded reports and mesh bytes mutated only in memory; no numerical diagnostic reruns or production writes.',
            'control_count': len(rows), 'all_rejected': True, 'controls': rows}

RESOURCE_MODES = ('animation', 'animation_visual_palette', 'animation_visual_palette_rigid')
RESOURCE_TOTALS = {'gnu_size_text_including_readonly_and_unwind_bytes': 60127, 'executable_text_bytes': 47355,
                   'readonly_data_bytes': 10796, 'data_bytes': 0, 'bss_bytes': 0}

def percentile(values, p):
    values = sorted(values); position = (len(values) - 1) * p; low = int(position)
    return values[low] + (values[min(low + 1, len(values) - 1)] - values[low]) * (position - low)

def resource_gate(footprint, benchmark):
    equal_fields(footprint, {'status': 'measured', 'compiler_target': 'x86_64-linux-gnu', 'incremental49T_totals': RESOURCE_TOTALS,
        'incremental49T_caller_total_bytes': 28636, 'combined_caller_payload_bytes_excluding_motion_bytes_and_padding': 349423,
        'retained_motion_byte_payload': 15624, 'maximum_individual49T_compiler_frame_bytes': 1008}, 'Footprint')
    units = {'game/' + name + '49t' for name in ('source_acos', 'model_motion', 'chariot_ik', 'vehicle_animation')}
    require(set(footprint['incremental49T_units']) == units and len(footprint['objects']) == 14, 'Footprint unit membership')
    for command in footprint['compile_commands']:
        strict_command(command, False, '-std=c++17', False)
        require(not any(flag.startswith('-DRR_') or flag == '-flto' for flag in command), 'Production footprint test hooks/LTO')
    for key, value in RESOURCE_TOTALS.items():
        require(sum(footprint['objects'][unit][key] for unit in units) == value, 'Footprint section arithmetic')
    for unit in units:
        row = footprint['objects'][unit]
        require(not row['direct_allocator_references'] and row['source_sha256'] == sha(ROOT / ('src/' + unit + '.cpp')), 'Production allocator/source footprint')
    require(sum(footprint['incremental49T_caller_records'].values()) == 28636 and
            sum(footprint['combined_caller_records'].values()) == 349423, 'Caller storage arithmetic')
    workspace = footprint['workspace_composition']
    require(workspace['VehicleAnimationWorkspace49T'] == sum(v for k, v in workspace.items() if k.startswith('included_')) == 28152 and
            footprint['record_bytes']['VehicleAnimationWorkspace49T'] == 28152, 'Nested workspace double counting')
    for key, value in footprint['combined_caller_records'].items():
        require(footprint['record_bytes'][key] == value, 'Caller record sizeof correspondence')
    require(max(r['bytes'] for r in footprint['frames'] if r['unit'] in units) == 1008, 'Individual compiler frame arithmetic')
    require(footprint['recursive_invalidate'] == {'maximum_simultaneous_invocations': 64, 'individual_frame_bytes': 32}, 'Recursive frame estimate boundary')
    chains = footprint['selected_additive_compiler_frame_estimates']
    require(len(chains) == 4, 'Selected compiler call-chain estimate membership')
    for chain in chains:
        total = 0
        for step in chain['chain']:
            if step == 'invalidate x <=64':
                total += 64 * 32
            else:
                unit, fragment = step.split(':', 1)
                matches = [row['bytes'] for row in footprint['frames'] if row['unit'] == unit and fragment in row['function'] and '<lambda' not in row['function']]
                require(len(matches) == 1, 'Compiler call-chain function ambiguous')
                total += matches[0]
        require(total == chain['sum_compiler_frame_bytes'], 'Selected additive compiler frame arithmetic')
    require(len(footprint['limits']) >= 6 and any('not stack high-water' in x for x in footprint['limits']) and
            any('no Cmalloc/aligned' in x for x in footprint['limits']), 'Resource interpretation limits')
    equal_fields(benchmark, {'status': 'measured', 'host_machine': 'x86_64', 'cpu_affinity': [0],
        'normal_virtual_address_limit_bytes': 67108864, 'samples_per_mode': 15,
        'cpu_seconds_limit_per_process': 30, 'wall_seconds_timeout_per_process': 40}, 'Benchmark scope')
    strict_command(benchmark['flags'], False, '-std=c++17', False)
    require(benchmark['source_hashes'] == footprint['source_hashes'] and benchmark['compiler'] == footprint['compiler'], 'Benchmark/footprint build provenance')
    require(set(benchmark['statistics']) == set(RESOURCE_MODES) and set(benchmark['histories_per_sample']) == set(RESOURCE_MODES) and
            len(benchmark['results']) == 45 and len(benchmark['calibrations']) == len(benchmark['untimed_ordinary_new_probes']) == 3, 'Benchmark sample membership')
    expected = footprint['fixture']['expected_final_hashes']
    final = {key: expected[source] for key, source in [('final_graph', 'palette_graph'), ('final_records', 'records'), ('final_palettes', 'palettes'), ('final_positions', 'positions')]}
    for row in benchmark['results'] + benchmark['calibrations'] + benchmark['untimed_ordinary_new_probes']:
        equal_fields(row, final, 'Benchmark expected source history')
        require(row['mode'] in RESOURCE_MODES and type(row['histories']) is int and row['histories'] >= 2 and row['frames'] == row['histories'] * 96 and row['retained_motion_bytes'] == 15624, 'Benchmark history accounting')
        checksum = (final['final_graph'] ^ final['final_records'] ^ final['final_palettes'] ^ (final['final_positions'] if row['mode'].endswith('_rigid') else 0)) * row['histories'] & 0xffffffffffffffff
        require(row['checksum'] == checksum, 'Benchmark continuation checksum arithmetic')
    for mode in RESOURCE_MODES:
        values = [r for r in benchmark['results'] if r['mode'] == mode]
        probes = [r for r in benchmark['untimed_ordinary_new_probes'] if r['mode'] == mode]
        calibration = [r for r in benchmark['calibrations'] if r['mode'] == mode]
        require(len(values) == 15 and {r['sample'] for r in values} == set(range(15)) and len(probes) == len(calibration) == 1, 'Benchmark missing mode/sample')
        require(probes[0]['cpp_new_allocations'] == probes[0]['nanoseconds'] == 0 and probes[0]['allocation_counter_enabled'] == 1, 'Enabled ordinary-new probe failed')
        require(probes[0]['histories'] == benchmark['histories_per_sample'][mode], 'Ordinary-new probe history count')
        nanos = []
        for row in values:
            require(row['allocation_counter_enabled'] == 0 and row['cpp_new_allocations'] == 0 and row['nanoseconds'] > 0 and
                    row['histories'] == benchmark['histories_per_sample'][mode] and row['checksum'] == probes[0]['checksum'], 'Timed sample instrumentation/counts')
            require(row['nanoseconds_per_frame'] == row['nanoseconds'] / row['frames'], 'Timing denominator')
            nanos.append(row['nanoseconds_per_frame'])
        stats = {'minimum_ns': min(nanos), 'p05_ns': percentile(nanos, .05), 'p25_ns': percentile(nanos, .25),
                 'median_ns': statistics.median(nanos), 'p75_ns': percentile(nanos, .75), 'p95_ns': percentile(nanos, .95),
                 'maximum_ns': max(nanos), 'mean_ns': statistics.mean(nanos),
                 'sample_duration_ns_range': [min(r['nanoseconds'] for r in values), max(r['nanoseconds'] for r in values)]}
        require(benchmark['statistics'][mode] == stats, 'Benchmark statistics arithmetic')
    require([(r['sample'], r['mode']) for r in benchmark['results']] == [(sample, mode) for sample in range(15) for mode in (RESOURCE_MODES if sample % 2 == 0 else RESOURCE_MODES[::-1])], 'Benchmark alternating run order')
    require(len(benchmark['measurement_limits']) >= 6 and any('No game FPS' in x for x in benchmark['measurement_limits']) and
            any('not a frame tail-latency' in x for x in benchmark['measurement_limits']), 'Benchmark interpretation boundary')

def resource_gates():
    footprint, benchmark, fixture = load('budget/footprint.json'), load('budget/benchmark.json'), load('budget/fixture.json')
    resource_gate(footprint, benchmark)
    pins(footprint['source_hashes']); pins(benchmark['source_hashes']); pins(fixture['source_assets'])
    require(footprint['fixture'] == fixture and benchmark['footprint_sha256'] == sha(E / 'budget/footprint.json'), 'Resource fixture/footprint pins')
    require(fixture['trace_sha256'] == sha(current_path(fixture['trace_path'])) and fixture['fixture_sha256'] == sha(E / 'budget/actual-history96.bin') and
            fixture['fixture_bytes'] == (E / 'budget/actual-history96.bin').stat().st_size == 201172, 'Benchmark retained fixture bytes')
    require(fixture['frames'] == len(fixture['all_frame_expected_hashes']) == 96 and fixture['graph_nodes'] == 52 and fixture['palette_entries'] == [18, 14, 14] and fixture['rigid_vertices'] == [2884, 1411, 1460], 'Benchmark fixture membership')
    trace = load('connected/trace.json')
    for row, hashes in zip(trace['frames'], fixture['all_frame_expected_hashes']):
        require(len(hashes) == 6 and hashes[:2] == [int(row['after21A000_graph_fnv64'], 16), int(row['after49R_graph_fnv64'], 16)], 'Benchmark original stage trace hashes')
    require([fixture['expected_final_hashes'][key] for key in ('animation_graph', 'visual_graph', 'palette_graph', 'records', 'palettes', 'positions')] == fixture['all_frame_expected_hashes'][-1], 'Benchmark final fixture hash row')
    baseline = footprint['baseline49S_manifest']
    require(sha(ROOT.parent / Path(baseline['path']).name) == baseline['sha256'] and all(baseline['reused_units_identical'].values()) and all(baseline['all_retained_dependencies_identical'].values()), 'Resource inherited source boundary')
    records = [line.split() for line in (E / 'budget/record-sizes.stdout').read_text().splitlines()]
    require({name: int(size) for name, size, _ in records} == footprint['record_bytes'] and {name: int(alignment) for name, _, alignment in records} == footprint['record_alignments'], 'Raw record sizes/alignment')
    for unit, row in footprint['objects'].items():
        stem = unit.split('/')[-1]
        size = (E / f'budget/{stem}-size.txt').read_text().splitlines()[1].split()
        require([int(x) for x in size[:3]] == [row['gnu_size_text_including_readonly_and_unwind_bytes'], row['data_bytes'], row['bss_bytes']], 'Raw GNU object size')
        sections = {name: int(size) for name, size, _ in re.findall(r'^(\S+)\s+(\d+)\s+(\d+)\s*$', (E / f'budget/{stem}-sections.txt').read_text(), re.M)}
        require(sections == row['sections'], 'Raw object section accounting')
        raw_frames = []
        for line in (E / f'budget/{stem}.su').read_text().splitlines():
            function, size, kind = line.split('\t')
            raw_frames.append({'unit': unit, 'function': function, 'bytes': int(size), 'kind': kind})
        require(raw_frames == [r for r in footprint['frames'] if r['unit'] == unit], 'Raw compiler frame estimates')
        require(not any(token in (E / f'budget/{stem}-undefined.txt').read_text() for token in ('_Znwm', '_Znam', 'operator new', 'malloc', 'calloc', 'realloc', 'aligned_alloc')), 'Unexpected linked allocator reference')
    symbols = (E / 'budget/timed-executable-symbols.txt').read_text()
    require(not any(word in symbols for word in ('Machine', 'original::', 'oracle', 'trace::', 'stage_trace', 'countAllocations')), 'Timed binary contains source oracle/tracing/allocation hooks')
    size = (E / 'budget/timed-executable-size.txt').read_text().splitlines()[1].split()
    executable = footprint['timed_executable_sections']
    require([int(x) for x in size[:3]] == [executable['gnu_size_text_including_readonly_and_unwind_bytes'], executable['data_bytes'], executable['bss_bytes']], 'Raw timed executable size')
    sections = {name: int(size) for name, size, _ in re.findall(r'^(\S+)\s+(\d+)\s+(\d+)\s*$', (E / 'budget/timed-executable-sections.txt').read_text(), re.M)}
    require(sections == executable['sections'], 'Raw timed executable sections')
    for filename, key in [('samples.stdout', 'results'), ('calibration.stdout', 'calibrations'), ('ordinary-new-probe.stdout', 'untimed_ordinary_new_probes')]:
        raw_rows = []
        for line in (E / 'budget' / filename).read_text().splitlines():
            pairs = [part.split('=', 1) for part in line.split()]
            require(all(len(p) == 2 for p in pairs) and len({p[0] for p in pairs}) == len(pairs), 'Malformed raw timing sample')
            raw_rows.append({key: value if key == 'mode' else int(value) for key, value in pairs})
        require(len(raw_rows) == len(benchmark[key]), 'Raw timing sample count')
        for raw, row in zip(raw_rows, benchmark[key]):
            equal_fields(row, raw, 'Raw timing sample')
    return footprint, benchmark

def resource_controls(resources, control):
    gate = lambda x: resource_gate(*x)
    control('resource_nested_scratch_double_count', resources, lambda x: x[0].update(incremental49T_caller_total_bytes=38136), gate)
    control('resource_allocator_reference', resources, lambda x: x[0]['objects']['game/vehicle_animation49t']['direct_allocator_references'].append('malloc'), gate)
    control('timing_missing_sample', resources, lambda x: x[1]['results'].pop(), gate)
    control('timing_incorrect_median', resources, lambda x: x[1]['statistics']['animation'].update(median_ns=0), gate)
    control('timing_counter_disabled_during_probe', resources, lambda x: x[1]['untimed_ordinary_new_probes'][0].update(allocation_counter_enabled=0), gate)
    control('timing_successful_hot_allocation', resources, lambda x: x[1]['untimed_ordinary_new_probes'][0].update(cpp_new_allocations=1), gate)
    control('timing_missing_history_checksum', resources, lambda x: x[1]['results'][0].update(checksum=0), gate)


def independent_connected_gates(renders):
    review = load('independent-review/results/connected-review.json'); manifest = load('independent-review/review-manifest.json')
    equal_fields(manifest, {'status': 'passed', 'production_edits_by_reviewer': 0, 'inherited_file_edits_by_reviewer': 0,
        'residuals_are_source_result_not_native_mismatch': True}, 'Independent review separation')
    local_pins('independent-review', manifest['files']); pins(review['files'])
    require(set(manifest['files']) == {p.relative_to(E / 'independent-review').as_posix() for p in (E / 'independent-review').rglob('*') if p.is_file() and p.name != 'review-manifest.json'}, 'Independent review sealed file membership')
    equal_fields(review, {'status': 'passed', 'source_input_pins': 133, 'source_code_pins': 89, 'trace_frames': 96,
        'independent_filter_recurrences': 384, 'graph_palette_row_matches': 4416, 'cross_build_exact_outputs': ['normal', 'plain', 'asan-ubsan'],
        'expected_repeat_images': 6, 'image_review_complete': True,
        'presentation_checks': {'four_original_tiles_preserved': True, 'control_crops_exact_nearest2x': True,
        'four_lossless_loop_frames_preserved': True, 'presentation_draws': 0, 'hand_closeup_exact_nearest4x': True}}, 'Independent connected review')
    require(len(review['verified_repeat_images']) == 6 and len({r['path'] for r in review['verified_repeat_images']}) == 6, 'Independent image membership')
    for row in review['verified_repeat_images']:
        require(row['repeat_pixels_exact'] is True and row['caption_keeps_raw_pixels'] is True, 'Independent image consistency')
        png_gate(dict(row, rgba8_sha256=row['rgba_sha256']))
    comparisons = {r['name']: r for r in renders['pixel_comparisons']}
    require(len(review['pixel_comparisons']) == len(comparisons), 'Independent pixel comparison membership')
    for row in review['pixel_comparisons']:
        original = comparisons[row['name'].removesuffix('.raw.png')]; box = original['bbox']
        require(row['changed_pixels_vs_frame95'] == original['pixels_changed_vs95'] and row['bbox_inclusive'] == ([box[0], box[1], box[2] - 1, box[3] - 1] if box else None), 'Independent pixel comparison arithmetic')
    return {'current_separate_review_manifest_sha256': sha(E / 'independent-review/review-manifest.json'), 'connected_source_and_image_review_passed': True}


def acos_exclusion_gate(words, raw_rows, account):
    report = load('acos/coverage-accounting.json'); local_pins('acos', report['inputs'])
    expected = {'words': 1510, 'reached_words': 1345, 'unreached_words': 165,
        'unreached_word_classifications': {'proved_exclusion': 91, 'remaining_coverage_limitation': 74},
        'branch_instructions': 158, 'conditional_branches': 120, 'instruction_constant_branches': 38,
        'conditional_both_directions': 67, 'conditional_one_direction': 42, 'conditional_unreached': 11,
        'missing_conditional_directions': 64, 'missing_direction_classifications': {'proved_exclusion': 27, 'remaining_coverage_limitation': 37},
        'observed_instructions': 191427455}
    require(report['summary'] == expected, 'Acos honest conditional/exclusion accounting')
    require(account['true_conditional_branches'] == 120 and account['instruction_constant_branches'] == 38 and
            account['conditional_both_directions'] == 67 and account['conditional_one_direction'] == 42 and account['conditional_unexecuted'] == 11, 'Independent acos conditional decode')
    raw = {r['pc']: r for r in raw_rows}; classes = {}; missing = {}
    require(len(report['words']) == 1510 and {integer(r['pc']) for r in report['words']} == set(words), 'Acos exclusion word membership')
    for row in report['words']:
        pc = integer(row['pc']); reason = report['reasons'][row['reason']]
        require(integer(row['word']) == words[pc] and row['hits'] == raw[pc]['hits'] and
                row['classification'] == reason['classification'] and reason['explanation'], 'Acos exclusion source/counter/reason')
        if not row['hits']:
            classes[row['classification']] = classes.get(row['classification'], 0) + 1
    branches = report['branches']
    require(len(branches) == 158 and {integer(r['pc']) for r in branches} == {p for p, w in words.items() if branch_kind(w)}, 'Acos branch artifact membership')
    for row in branches:
        pc = integer(row['pc']); word = words[pc]; outcome = static_branch_outcome(word)
        require(integer(row['word']) == word and row['static_outcome'] is outcome and row['conditional'] is (outcome is None), 'Acos instruction constant classification')
        equal_fields(row, {key: raw[pc][key] for key in ('hits', 'taken', 'not_taken')}, 'Acos raw branch counters')
        absent = {key for key in ('taken', 'not_taken') if not row[key]}
        require(len(row['missing_directions']) == len(absent) and {r['direction'] for r in row['missing_directions']} == absent, 'Acos missing direction completeness')
        for item in row['missing_directions']:
            reason = report['reasons'][item['reason']]
            require(item['classification'] == reason['classification'] and bool(reason['explanation']), 'Acos direction reason')
            if outcome is None:
                missing[item['classification']] = missing.get(item['classification'], 0) + 1
    require(classes == expected['unreached_word_classifications'] and missing == expected['missing_direction_classifications'], 'Acos exclusion arithmetic')
    return expected


def release_addendum_gate():
    report = load('independent-review/results/release-addendum.json')
    equal_fields(report, {'schema': 'chariot49t.independent-release-addendum.v1', 'status': 'passed',
        'aggregate_registered_suites': 56, 'aggregate_production_units': 50, 'flow_root_argument_corrected': True,
        'cmake_executed': False, 'cxx_or_renderer_executed_by_addendum': False, 'retained_compiler_frame_rows': 83,
        'selected_stack_chains_recomputed': 4, 'objects_without_direct_allocator_references': 14,
        'timed_symbol_exclusions_verified': True, 'timing_modes': 3, 'retained_samples_each': 15,
        'separate_ordinary_new_probes': 3, 'incremental_caller_bytes': 28636, 'workspace_bytes': 28152,
        'combined_caller_bytes': 349423, 'source_coverage_remaining_words': 74, 'source_coverage_remaining_outcomes': 37}, 'Independent release addendum')
    require(report.get('findings') == [], 'Independent release addendum has unresolved findings')
    pins(report['input_hashes'])
    require(report['input_hashes'].get('tools/validate_chariot_animation49t.py') == sha(Path(__file__)), 'Independent review must cover this exact validator')
    return {'release_addendum_sha256': sha(E / 'independent-review/results/release-addendum.json'), 'release_addendum_passed': True}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, help='Exact verified 49S ZIP; mandatory for complete release validation')
    parser.add_argument('--seal', action='store_true', help='Explicitly write checkpoint-validation.json after all gates pass')
    parser.add_argument('--check-only', action='store_true', help='Require and independently verify the existing seal, without writes')
    parser.add_argument('--components-only', action='store_true', help='Read-only source, focused, connected, rendering, presentation and independent-review gates only')
    args = parser.parse_args()
    require(not (args.seal and (args.check_only or args.components_only)), 'Seal cannot be combined with read-only partial/check modes')
    require(not (args.check_only and args.components_only), 'Choose complete seal verification or partial components')
    require(args.components_only or args.baseline is not None, '--baseline required for complete release validation')
    sources = original_sources(); components = component_gates(sources); export = connected_gates(); review = independent_gates()
    renders = render_gates(export); presentation = presentation_gate(['pass-1', 'pass-2']); presentation.update(detail_presentation_gate())
    review.update(independent_connected_gates(renders))
    if args.components_only:
        controls = gate_controls(sources, export, False)
        print(json.dumps({'status': 'passed', 'scope': 'components, connected, renders, presentation, independent review only',
                          'release_sealed': False, 'source_accounting': components, 'gate_controls_rejected': controls['control_count']}))
        return
    resources = resource_gates(); aggregate = regression_gates(); review.update(release_addendum_gate())
    names, changed = baseline_gate(args.baseline)
    controls = gate_controls(sources, export, True, resources)
    result = {'checkpoint': '49T-ChariotAnimation', 'status': 'passed', 'baseline_sha256': BASELINE_SHA,
        'baseline_members': len(names), 'changed_baseline_members': changed, 'regression_suites_each': 56,
        'regression_checks_each': aggregate['checks_each'], 'tracked_inputs_each': aggregate['runs']['normal']['tracked_inputs'],
        'production_units_each': 50, 'new_production_units': 4, 'normal_sanitizer_outputs_identical': True,
        'inherited49S_stdout_exact': True, 'source_accounting': components, 'independent_review': review,
        'connected_checks': CONNECTED_COUNTS['checks'], 'connected_frames': 96, 'authoritative_render_passes': ['pass-1', 'pass-2'],
        'rendered_frames_each': 6, 'deterministic_plan_png_pixel_repeats': True, 'renders': renders, 'presentation': presentation,
        'gate_controls_rejected': controls['control_count'], 'gate_controls': controls,
        'resource_incremental_totals': resources[0]['incremental49T_totals'], 'resource_caller_bytes': resources[0]['incremental49T_caller_total_bytes'],
        'host_subsystem_statistics': resources[1]['statistics'], 'ordinary_new_success_probes_each': 3,
        'cmake_executed': False, 'focused_store_tracing': True, 'aggregate_store_tracing': False,
        'limits': 'Bounded original internal21A000 animation and authored type1/debug0 hand IK with supplied stationary roots/body/control context. Separate21A660 root producer, authentic spawn, physics/contacts, original material/VU/visibility, and full game remain open. Source numeric coverage limitations are retained explicitly. Host subsystem measurements are not gameplay FPS, process RAM or console performance. ASan+UBSan only; no LeakSanitizer claim. No PC/RAM/install or held collision-capacity/49C ownership test is run by this validator.',
        'source_hashes': seal_hashes(names)}
    if args.seal:
        result['verified_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        (E / 'checkpoint-validation.json').write_text(json.dumps(result, indent=2) + '\n')
    elif args.check_only or (E / 'checkpoint-validation.json').exists():
        existing = load('checkpoint-validation.json')
        require({k: v for k, v in existing.items() if k != 'verified_utc'} == result and isinstance(existing.get('verified_utc'), str), 'Existing release seal differs from independently revalidated evidence')
    print(json.dumps({'status': 'passed', 'seal_written': args.seal, 'baseline_members': len(names), 'regression_suites_each': 56,
                      'checks_each': aggregate['checks_each'], 'tracked_inputs_each': result['tracked_inputs_each'],
                      'gate_controls_rejected': controls['control_count'], 'frozen_new_or_allowed_files': len(result['source_hashes'])}))

if __name__ == '__main__':
    try:
        main()
    except (GateError, KeyError, TypeError, OSError, ValueError, zipfile.BadZipFile) as error:
        print('49T release validation failed: ' + str(error), file=sys.stderr)
        raise SystemExit(1)
