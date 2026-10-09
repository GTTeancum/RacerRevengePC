#!/usr/bin/env python3
"""Read-only 49V release gates; only explicit --seal writes checkpoint-validation.json.
No compiler, C++ diagnostic, renderer, PC/RAM/install, held collision-capacity,
or 49C ownership test is executed. Negative controls mutate decoded data only.
All baseline members are checked; every new or allowed release file is frozen.
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
E = ROOT / 'evidence/chariotLifecycle49V'
BASELINE_SHA = '293a70924f0182707ad31d130f74c2d280744266d62ec354532e3ec933dbda4c'
BASELINE_BYTES = 213780826
BASELINE_MEMBERS = 26467
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
ALLOWED = {'RacerRevengePC/README.md', 'RacerRevengePC/CMakeLists.txt'}
NEW_UNITS = {'src/game/chariot_lifecycle49v.cpp', 'src/game/chariot_blend49v.cpp', 'src/game/secondary_activation49v.cpp'}
NEW_SUITES = {'chariot_lifecycle49v', 'chariot_blend49v', 'secondary_activation49v', 'chariot_lifecycle_export49v'}
FRAME_KEYS = {(i, 'full-motion') for i in (15, 47, 94, 135)} | {(94, 'virtual-only'), (94, 'physical-copy-only')}
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}
class GateError(ValueError):
    pass

def load(name):
    return json.loads((E / name).read_text())

def local_pins(prefix, rows):
    safe_names(list(rows))
    pins({f'evidence/chariotLifecycle49V/{prefix}/{name}': value for name, value in rows.items()})

def require(value, message):
    if not value:
        raise GateError(message)

def digest(data):
    return hashlib.sha256(data).hexdigest()

def sha(path):
    return digest(path.read_bytes())

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
                require(bool(row.get('one_sided_reason') or row.get('single_direction_reason') or row.get('unreached_direction_reason')), label + ': unexplained branch direction')
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

def plan_gate(plan, export, frame, profile):
    require(plan['schema'] == 'chariotlife49v.visual-plan.v1' and plan['diagnostic_only'] is True and
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
    require(plan['profile']['native_chariot_mode_lifecycle'] is (profile == 'full-motion') and
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
    require(plan['renderer49V_sha256'] == sha(ROOT / 'tools/visual/render_chariot_lifecycle49v.py'), 'Render current script pin')
    payload = dict(plan); expected_hash = payload.pop('plan_sha256')
    require(digest(json.dumps(payload, sort_keys=True, separators=(',', ':'), allow_nan=False).encode()) == expected_hash, 'Render canonical plan hash')

def frame_gate(report, plan):
    equal_fields(report, {'status': 'rendered_diagnostic_only', 'schema': 'chariotlife49v.visual-report.v1',
        'actual_draw_calls': 360, 'actual_submitted_triangles': 73409, 'actual_texture_uploads': 26,
        'actual_gl_buffer_uploads': 4, 'shared_mesh_sets': 2, 'uniform_bits_round_trip': True,
        'framebuffer_unchanged_by_caption': True, 'opaque_output_alpha': True}, 'Rendered result')
    require(report['plan'] == plan and len(report['images']) == 2 and len(report['texture_state_checks']) == 26 and
            all(r['parameters_and_level_zero_verified'] is True for r in report['texture_state_checks']), 'Rendered plan/texture verification')
    require(report['runtime']['cpu_affinity'] == [0] and report['runtime']['software_environment']['LP_NUM_THREADS'] == '1',
            'Renderer process constraints')
    require(report['pixel_rgba8_sha256'] == report['images'][0]['rgba8_sha256'], 'Rendered pixel hash marker')

def png_gate(row):
    from PIL import Image
    pins([row]); path = current_path(row['path'])
    with Image.open(path) as source:
        rgba = source.convert('RGBA')
        require(digest(rgba.tobytes()) == row['rgba8_sha256'], 'Decoded PNG RGBA pin')
        if 'dimensions' in row:
            require(list(rgba.size) == row['dimensions'], 'PNG dimensions')
        return rgba.copy()

def check_count(text):
    try:
        return int(json.loads(text)['checks'])
    except (ValueError, KeyError):
        match = re.search(r'checks=(\d+)', text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks', text)
        require(match is not None, 'Missing stdout check count')
        return int(match.group(1))

def baseline_gate(path):
    require(path.is_file() and not path.is_symlink() and path.stat().st_size == BASELINE_BYTES and sha(path) == BASELINE_SHA, 'Expected exact verified 49U baseline archive')
    changed = []
    with zipfile.ZipFile(path) as archive:
        names = [row.filename for row in archive.infolist() if not row.is_dir()]
        safe_names(names); require(len(names) == BASELINE_MEMBERS, 'Baseline member count'); require(archive.testzip() is None, 'Baseline ZIP CRC failure')
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

def percentile(values, p):
    values = sorted(values); position = (len(values) - 1) * p; low = int(position)
    return values[low] + (values[min(low + 1, len(values) - 1)] - values[low]) * (position - low)

class Reader:
    def __init__(self, data): self.data, self.at = data, 0
    def take(self, n):
        require(type(n) is int and n >= 0 and self.at + n <= len(self.data), 'Semantic cache bounds')
        result = self.data[self.at:self.at + n]; self.at += n; return result
    def read(self, form): return struct.unpack('<' + form, self.take(struct.calcsize('<' + form)))
    def u(self): return self.read('I')[0]
    def string(self): return self.take(self.u()).decode()

def local_vertices(path):
    r = Reader(path.read_bytes()); require(r.take(8) == b'RRSCENE1' and r.u() == 1, 'Semantic cache header')
    world, palette, nodes, materials, lods, _, _ = r.read('7I')
    require(world == 0 and palette == 1 and nodes in (14, 18) and lods == 4, 'Semantic cache scope')
    r.take(56)
    for _ in range(nodes): r.string(); r.take(4 + 39 * 8)
    for _ in range(materials): r.string()
    result = []
    for lod in range(lods):
        r.take(8)
        for _ in range(r.u()):
            r.read('2I'); r.take(8); vertices, indices = r.read('2I')
            for _ in range(vertices):
                xyz = r.read('3d'); r.take(3 * 8 + 2 * 8 + 4); bone = r.read('H')[0]
                require(bone < nodes and all(math.isfinite(x) for x in xyz), 'Semantic local coordinates/palette ID')
                if lod == 0: result.append((xyz, bone))
            r.take(indices * 4)
    r.take(r.u() * 24)  # LOD0 compatibility spans.
    r.take(r.u())  # Frozen neutral compatibility mesh.
    require(r.at == len(r.data), 'Semantic cache trailing bytes')
    return result

def f32(word):
    require(type(word) is int and 0 <= word <= 0xffffffff, 'Binary32 word')
    value = struct.unpack('<f', struct.pack('<I', word))[0]
    require(math.isfinite(value), 'Nonfinite consumed binary32')
    return value

def position(xyz, matrix):
    result = []
    for axis in range(3):
        total = 0.0
        for k in range(3): total = total + xyz[k] * matrix[k * 4 + axis]
        result.append(total + matrix[12 + axis])
    require(all(math.isfinite(x) for x in result), 'Nonfinite diagnostic projection')
    return struct.pack('<3f', *result)

def fnv(data):
    h = 1469598103934665603
    for byte in data: h = ((h ^ byte) * 1099511628211) & 0xffffffffffffffff
    return '%016x' % h

def reconstruct_vertices(trace, export):
    keys = ['cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5',
            '210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3',
            'f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60']
    locals_ = [local_vertices(current_path('evidence/visual49G/cacheobjects/' + k + '.rrscene')) for k in keys]
    require([len(v) for v in locals_] == [2884, 1411, 1460], 'All semantic local vertices')
    baseline = current_path(export['baseline_mesh_path']).read_bytes()
    outputs = {(r['frame'], r['label']): r for r in export['frames']}; controls = dict.fromkeys(CONSUMER_CONTROLS, 0); reconstructed = {}
    for row in trace['frames'] + trace['display_controls']:
        label = row.get('label', 'full-motion'); expected = bytearray(baseline)
        for side, name in enumerate(('chariot_palette', 'left_palette', 'right_palette')):
            pal = row[name]; matrices = [[f32(w) for w in m] for m in pal['node_world_bits']]; root = [f32(w) for w in pal['root_world_bits']]
            for i, (xyz, bone) in enumerate(locals_[side]):
                at = 16 + ((0, 2884, 4295)[side] + i) * 24
                projected = position(xyz, matrices[bone]); expected[at:at + 12] = projected
                if (row['frame'], label) in outputs and label == 'full-motion':
                    controls['wrong_whole_model_transform'] += position(xyz, root) != projected
                    controls['wrong_palette_swap'] += position(xyz, matrices[(bone + 1) % pal['entry_count']]) != projected
                    controls['wrong_double_root'] += position(struct.unpack('<3f', projected), root) != projected
                    controls['wrong_bind_baked_input'] += position(struct.unpack_from('<3f', baseline, at), matrices[bone]) != projected
                    controls['wrong_float32_semantic_input'] += position(struct.unpack('<3f', struct.pack('<3f', *xyz)), matrices[bone]) != projected
                    controls['wrong_stale_frame0_pose'] += position(xyz, [f32(w) for w in trace['frames'][0][name]['node_world_bits'][bone]]) != projected
        if label == 'full-motion':
            require(row['vertices'] == 5755 and fnv(expected) == row['mesh_fnv64'], 'Independent all240 complete mesh digest')
        key = row['frame'], label
        if key in outputs:
            meta = outputs[key]; actual = (E / 'connected' / meta['mesh_path']).read_bytes()
            require(actual == expected and digest(actual) == meta['mesh_sha256'] and mesh_gate(actual, baseline) == meta['changed_vertices_vs49H'], 'Independent exact exported vertex bytes')
            reconstructed[key] = actual
    require(controls == CONSUMER_CONTROLS, 'Independently recomputed consumer sensitivities')
    for key, actual in reconstructed.items():
        require(mesh_gate(actual, reconstructed[(key[0], 'full-motion')]) == outputs[key]['changed_vertices_vs_full'], 'Independent control vertex isolation')
    return {'independently_reconstructed_full_frame_vertices': 1381200, 'exact_exported_meshes': 6,
            'consumer_controls': controls, 'float32_first_control': '2 position witnesses only'}


def original_sources():
    raw = current_path('work/native/project/disc/SLUS_202.68').read_bytes()
    require(digest(raw) == ELF_SHA and raw[:6] == b'\x7fELF\x01\x01', 'Original ELF pin')
    off = struct.unpack_from('<I', raw, 28)[0]; size, count = struct.unpack_from('<HH', raw, 42)
    require(size >= 32 and off + size * count <= len(raw), 'ELF program headers')
    segments = [struct.unpack_from('<8I', raw, off + size * i) for i in range(count)]
    def take(start, end):
        matches = [raw[o + start - v:o + end - v] for k, o, v, _, n, *_ in segments if k == 1 and v <= start < end <= v + n]
        require(len(matches) == 1 and len(matches[0]) == end - start, 'Original range uniquely file backed'); return matches[0]
    sources = {}
    for name, header, total in [('lifecycle', 'lifecycle_words.h', 5017), ('secondary', 'secondary_words49v.h', 277)]:
        manifest = load(name + '/source/source-manifest.json'); text = (E / name / 'source' / header).read_text()
        require(manifest['elf_sha256'] == ELF_SHA, 'Source manifest ELF pin')
        blocks = manifest['blocks']; blocks = [dict(row, name=k) for k, row in blocks.items()] if isinstance(blocks, dict) else blocks
        arrays = {n: b''.join(struct.pack('<I', int(w, 16)) for w in re.findall(r'0x([0-9a-fA-F]+)u', body)) for n, body in re.findall(r'uint32_t\s+(\w+)\[\]\s*=\s*\{(.*?)\};', text, re.S)}
        require(set(arrays) == {r['name'] for r in blocks}, 'Original array membership')
        words = {}; expected_bounds = []; cursor = 0
        for row in blocks:
            start, end = integer(row['start']), integer(row['end_exclusive']); data = take(start, end); n = (end - start) // 4
            require(start % 4 == end % 4 == 0 and row.get('words', row.get('word_count')) == n, 'Original source bounds')
            require(digest(data) == row['sha256'] and arrays[row['name']] == data and (E / name / 'source' / (row['name'] + '.bin')).read_bytes() == data, 'Original source bytes: ' + row['name'])
            at = integer(row['elf_offset']); require(raw[at:at + len(data)] == data, 'Original explicit ELF offset')
            expected_bounds.append((row['name'], start, row['name'], n, cursor)); cursor += n
            for i, w in enumerate(struct.unpack('<' + str(n) + 'I', data)):
                require(start + 4 * i not in words, 'Overlapping source blocks'); words[start + 4 * i] = w
        if name == 'lifecycle':
            actual = [(n,int(a,16),v,int(c),int(i)) for n,a,v,c,i in re.findall(r'\{"(\w+)",0x([0-9a-fA-F]+)u,(\w+),(\d+),(\d+)\}',text)]
            equal_fields(manifest, {'words':5017,'root_words':1710,'root_return':'0021c110','root_delay':'0021c114'}, 'Complete lifecycle boundary')
            pins([manifest['retained_machine']]); require(sha(ROOT/'evidence/chariotRoot49U/root/source/root_machine.h') == manifest['retained49U_machine_sha256'], 'Retained root machine pin')
        else:
            starts = {n:int(a,16) for n,a in re.findall(r'uint32_t\s+(\w+)_start=0x([0-9a-fA-F]+)u',text)}
            actual = [(n,starts[a],v,len(arrays[v])//4,int(i)) for n,a,v,q,i in re.findall(r'\{"(\w+)",(\w+)_start,(\w+),sizeof\((\w+)\)/4,(\d+)\}',text)]
            require(manifest['RAM_reads'] == manifest['production_source_reads'] == 0, 'Secondary source-only provenance')
        require(actual == expected_bounds and cursor == len(words) == total, 'Original compiled source index table')
        sources[name] = words
    require(sources['lifecycle'][0x21c110] == 0x03e00008 and sources['lifecycle'][0x21c114] == 0x27bd05d0, 'Full original return and delay')
    getter = load('connected/getter-boundary-proof.json')
    require(getter['passed'] is True and getter['original_elf_sha256'] == ELF_SHA and getter['source_callsite'] == '0x218C60' and
        getter['callee'] == '0x28B1A0(physical19C,main190,out)' and getter['original_callsite_words'] == list(struct.unpack('<4I',take(0x218c58,0x218c68))) and
        getter['source_getter_sha256'] == digest(take(0x28b1a0,0x28b2b0)), 'Isolated later getter original callsite')
    return sources

LIFECYCLE_COUNTS = {'passed':True,'cases':560,'checks':1681153,'native_calls':560,'full_returns':560,'source_instructions':4252456,'noise_calls':2568,'rng_calls':5373,'reverse_calls':16,'blend_calls':84,'body_bytes_compared':913920,'native_allocations':0}
BLEND_COUNTS = {'passed':True,'cases':2612,'checks':433688,'rejections':32,'original_instructions':1273512,'source_row_calls':15672,'duration_equal':58,'duration_expired':920,'endpoint_changed_zero_signs':647,'native_allocations':0}


def focused_gate(name, report, coverage, mutations, words):
    counts = LIFECYCLE_COUNTS if name == 'lifecycle' else BLEND_COUNTS
    plain = dict(counts, checks=1674877 if name == 'lifecycle' else 399732)
    equal_fields(report, {'passed':True,'elf_sha256':ELF_SHA,'production_language':'C++17','test_language':'C++20',
        'normal':counts,'sanitized':counts,'plain':plain,'leak_sanitizer':'disabled; no LeakSanitizer claim',
        'mutations':44 if name == 'lifecycle' else 28}, name + ' focused proof')
    require(report['source_substitutions' if name == 'lifecycle' else 'original_call_target_substitutions'] == 0, name + ' source substitution')
    require('-fno-fast-math' in report['flags'] and '-ffp-contract=off' in report['flags'] and not BAD_FLAGS.intersection(report['flags']), name + ' strict FP')
    require(mutations['count'] == report['mutations'] == len(mutations['detected']) and mutations['production_edited'] is False and mutations['setup_failures_counted'] == 0, name + ' behavioral mutation scope')
    expected_mutations = LIFECYCLE_MUTATIONS if name == 'lifecycle' else BLEND_MUTATIONS
    require([(r['name'],r['kind']) for r in mutations['detected']] == expected_mutations, name + ' mutation identity and kind')
    for row in mutations['detected']:
        require(row['mismatch'].startswith('mismatch ') and not any(x in row['mismatch'].lower() for x in ('sanitizer','segmentation','runtime error','compile failed')), name + ' mutation assertion')
        if row['kind'] == 'original_word': require(integer(row['pc']) in words and integer(row['word']) != words[integer(row['pc'])], name + ' actual source mutation')
        else: require(row['before'] != row['after'], name + ' actual native mutation')
    if name == 'lifecycle':
        equal_fields(coverage, {'frozen_words':5017,'reached_words':3704,'root_words':1710,'root_reached':1637,'root_unaccounted':0,
            'uncovered_classes':{'excluded_root_mode_or_proven_fixed_branch':73,'provenance_only':44,'unexercised_helper_branch':1196}}, 'Lifecycle coverage limits')
        rows = [r for b in coverage['blocks'] for r in b['pcs']]; result = coverage_gate(rows, words, name, False)
        require(result['dynamic_instructions'] == counts['source_instructions'] and result['reached_words'] == coverage['reached_words'], 'Lifecycle dynamic coverage arithmetic')
        classes = {}
        for row in rows:
            pc = integer(row['pc'])
            if not row['hits']:
                require(bool(row.get('reason')), 'Lifecycle unexecuted word explanation')
                classes[row['coverage_class']] = classes.get(row['coverage_class'],0) + 1
                if 0x21a660 <= pc < 0x21c118:
                    require(pc in (0x21bdf4,0x21bdf8,0x21bdfc) or 0x21bfdc <= pc < 0x21c0f4, 'Unexecuted admitted lifecycle word')
            if branch_kind(words[pc]) and row['hits']:
                require(row['branch_directions'] == ('both' if row['taken'] and row['not_taken'] else 'single') and 'single does not mean universal unreachability' in row['branch_scope'], 'Lifecycle branch interpretation')
        require(classes == coverage['uncovered_classes'], 'Lifecycle uncovered arithmetic')
        lookup = {integer(r['pc']):r for r in rows}
        require(lookup[0x21c110]['hits'] == lookup[0x21c114]['hits'] == counts['full_returns'], 'All lifecycle real returns/delays')
        require(all(lookup[p]['hits'] for a,z in [(0x167a50,0x167ac4),(0x167ad0,0x167b7c)] for p in range(a,z,4)), 'Every reset callback word reached')
    else:
        equal_fields(coverage, {'blend_words':300,'blend_reached':297,'source_constant_unreachable':3,'unaccounted':0}, 'Blend coverage limits')
        selected = {p:w for p,w in words.items() if 0x21ba04 <= p < 0x21beb4}
        result = coverage_gate(coverage['blend'], selected, 'Blend', False)
        require(result['reached_words'] == 297 and {integer(r['pc']) for r in coverage['blend'] if not r['hits']} == {0x21bdf4,0x21bdf8,0x21bdfc}, 'Blend known fixed unreachable branch')
        coverage_gate(coverage['row_getter'], {p:w for p,w in words.items() if 0x14ebb0 <= p < 0x14ecfc}, 'Blend source row getter', False)
        require(next(r['hits'] for r in coverage['row_getter'] if integer(r['pc']) == 0x14ebb0) == counts['source_row_calls'] == counts['cases'] * 6, 'All six original row calls per blend')
    return result


def component_gates(sources):
    results = {}
    for name,directory in [('lifecycle','lifecycle/validation'),('blend','blend/proof')]:
        report,coverage,mutations = [load(directory+'/'+n) for n in ('validation.json','coverage-accounting.json','mutations.json')]
        results[name] = focused_gate(name,report,coverage,mutations,sources['lifecycle']); pins(report['files' if name == 'lifecycle' else 'hashes'])
        for mode,stem in [('normal','normal'),('sanitized','asan-ubsan'),('plain','plain')]:
            require(load(directory+'/'+mode+'.json') == report[mode] and json.loads((E/directory/(stem+'.stdout')).read_text()) == report[mode] and not (E/directory/(stem+'.stderr')).read_bytes(), name+' raw mode outputs')
        equal_fields(load(directory+'/read-only-default.json'), {'passed':True,'evidence_unchanged':True,'explicit_output_only':True}, name+' read-only default')
        if name == 'lifecycle':
            raw = {int(p,16):(int(h),int(t),int(n)) for p,h,t,n in (s.split() for s in (E/directory/'coverage.tsv').read_text().splitlines())}
            require(raw == {integer(r['pc']):(r['hits'],r['taken'],r['not_taken']) for b in coverage['blocks'] for r in b['pcs']}, 'Lifecycle durable raw coverage')
        for row in mutations['detected']:
            # The historical producer reuses one filename for a source and a
            # native reverse_callback_omitted control. Its final native log is
            # checked; both independently identified report rows are required.
            if row['name'] == 'reverse_callback_omitted' and row['kind'] == 'original_word': continue
            require((E/directory/('mutant-'+row['name']+'.stderr')).read_text().strip() == row['mismatch'], name+' raw mutation log')
    contract = load('lifecycle/validation/body-scene-contract.json')
    equal_fields(contract, {'passed':True,'body_source_bytes_compared':913920,'reverse_calls':16,'blend_calls':84}, 'Canonical body contract')
    require(any('stale cached world' in x for x in contract['produced']) and any('upper24bits' in x for x in contract['produced']) and any('no eager world refresh' in x for x in contract['excluded']), 'Body dirty cache and upper-flag scope')
    secondary = load('secondary/verification/verification.json'); equal_fields(secondary,{'passed':True,'normal_and_asan_ubsan':True,'native_mutations_detected':10},'Secondary focused proof')
    require(secondary['leak_sanitizer'] == 'Disabled because the executor uses ptrace; native operator-new allocation counts remain checked.', 'Secondary LSan boundary')
    pins(load('secondary/artifact-hashes.json'))
    records = secondary['records']; require(len(records) == 24 and len({r['name'] for r in records}) == 24, 'Secondary compile/run membership')
    for row in records:
        mutant = row['name'].startswith('test-mutation-'); require(row['passed'] is True and row['returncode'] == row['expected'] == int(mutant), 'Secondary process status')
        if row['name'].startswith('build-'): strict_command(row['command'], row['name']=='build-sanitized','-std=c++17',False)
        stderr = (E/'secondary/verification'/(row['name']+'.stderr')).read_text()
        require(stderr.startswith('check failed: ') if mutant else not stderr, 'Secondary behavioral mutation/runtime stderr')
    normal = (E/'secondary/verification/test-normal.stdout').read_bytes()
    require(normal == (E/'secondary/verification/test-sanitized.stdout').read_bytes() and 'PASS 144 cases, 384196 checks, 0 allocations, 117504 full-object bytes compared' in normal.decode(), 'Secondary exact raw output')
    results['secondary'] = {'cases':144,'checks':384196,'body_bytes_compared':117504,'mutations':10}
    return results

LIFECYCLE_MUTATIONS = [('cold_speed_word', 'original_word'), ('phase_store_omitted', 'original_word'), ('previous_error_store_omitted', 'original_word'), ('velocity_x_store_omitted', 'original_word'), ('position_x_store_omitted', 'original_word'), ('omega_x_store_omitted', 'original_word'), ('persistent_normalization_omitted', 'original_word'), ('translation_noise_offset_2', 'original_word'), ('hidden_root_setter_omitted', 'original_word'), ('display_root_setter_omitted', 'original_word'), ('transition_hidden_seed_omitted', 'original_word'), ('transition_wrong_relative_matrix', 'original_word'), ('transition_mode_store_omitted', 'original_word'), ('final_probe_store_omitted', 'original_word'), ('cold_rng_increment_changed', 'original_word'), ('reverse_callback_omitted', 'original_word'), ('reverse_main70_store_omitted', 'original_word'), ('reverse_mainB0_store_omitted', 'original_word'), ('reverse_timer_reset_omitted', 'original_word'), ('reverse_physical_mode_omitted', 'original_word'), ('reset_speed_clear_omitted', 'original_word'), ('reset_local80_omitted', 'original_word'), ('blend_physical_local_instead_world', 'original_word'), ('blend_timer_store_omitted', 'original_word'), ('blend_expiry_equality_changed', 'original_word'), ('blend_virtual_clear_omitted', 'original_word'), ('speed_equality_changed', 'original_word'), ('reverse_callback_omitted', 'isolated_native_change'), ('reverse_clears_virtual', 'isolated_native_change'), ('reverse_recomputes_speed', 'isolated_native_change'), ('reverse_forces_velocity_w', 'isolated_native_change'), ('blend_uses_local', 'isolated_native_change'), ('blend_uses_next_timer', 'isolated_native_change'), ('blend_expires_at_equality', 'isolated_native_change'), ('high_speed_aborts_blend', 'isolated_native_change'), ('secondary_projects_local', 'isolated_native_change'), ('secondary_discards_flag_high_bytes', 'isolated_native_change'), ('reverse_eager_world_refresh', 'isolated_native_change'), ('reverse_copy_before_reset', 'isolated_native_change'), ('speed_equality_low', 'isolated_native_change'), ('derivative_sign', 'isolated_native_change'), ('no_position_integration', 'isolated_native_change'), ('phase_reset_each_call', 'isolated_native_change'), ('zero_amplitude_skips_noise', 'isolated_native_change')]

BLEND_MUTATIONS = [('physical_reads_local_instead_of_cached_world', 'original_word'), ('first_virtual_row_is_forward', 'original_word'), ('first_physical_row_is_forward', 'original_word'), ('interpolation_division_reversed', 'original_word'), ('translation_difference_reversed', 'original_word'), ('translation_multiply_omitted', 'original_word'), ('second_cross_omitted', 'original_word'), ('up_normalization_omitted', 'original_word'), ('forward_normalization_omitted', 'original_word'), ('row0_construction_omitted', 'original_word'), ('translation_x_publish_omitted', 'original_word'), ('elapsed_update_omitted', 'original_word'), ('expiry_greater_or_equal', 'original_word'), ('expired_virtual_clear_omitted', 'original_word'), ('rowgetter_position_x_reads_y', 'original_word'), ('reverse_interpolation', 'isolated_native_change'), ('fused_lerp', 'isolated_native_change'), ('clamp_negative_alpha_wrongly', 'isolated_native_change'), ('endpoint_matrix_copy', 'isolated_native_change'), ('skip_up_rebuild', 'isolated_native_change'), ('reverse_cross_handedness', 'isolated_native_change'), ('skip_forward_normalization', 'isolated_native_change'), ('extra_row0_normalization', 'isolated_native_change'), ('zero_norm_invented_identity', 'isolated_native_change'), ('erase_signed_zero', 'isolated_native_change'), ('position_w_zero', 'isolated_native_change'), ('duration_accepts_negative', 'isolated_native_change'), ('upper_endpoint_skips_delay_slot_subtraction', 'isolated_native_change')]

CONNECTED_COUNTS = {'49R_complete_calls': 240, 'blend_calls': 122, 'checks': 21988191, 'dirty_later_getter_calls': 2, 'discarded_cache_witness_calls': 2, 'frames': 240, 'full21A000_calls': 240, 'graph_nodes': 53, 'independent_later_getter_calls': 157, 'later_physics_pose_copies': 0, 'moving_chariot_local_node_events': 2402, 'moving_chariot_root_frames': 205, 'moving_engine_local_node_events': 2290, 'moving_engine_root_frames': 240, 'native_hot_allocations': 0, 'native_ik_calls': 480, 'native_vertices_all240_frames': 1381200, 'palette_calls': 726, 'palette_entries': 46, 'passed': True, 'persistent_ik_initializations': 1, 'persistent_virtual_calls': 82, 'physical_entry_calls': 36, 'reverse_calls': 2, 'rigid_calls': 726, 'root21A660_calls': 240, 'schedule_integer_ms': 4000, 'source21A000_instructions': 10397643, 'source48W_instructions': 114360, 'source49R_instructions': 1938064, 'source49R_noise_calls': 2880, 'source49V_instructions': 981665, 'source49V_noise_calls': 1224, 'source_ik_calls': 480, 'source_noise_calls': 4104, 'source_palette_instructions': 975980, 'source_rng_calls': 1791, 'source_substitutions': 0}
CONSUMER_CONTROLS = {'wrong_bind_baked_input': 23020, 'wrong_double_root': 23020, 'wrong_float32_semantic_input': 2, 'wrong_palette_swap': 23020, 'wrong_stale_frame0_pose': 20819, 'wrong_whole_model_transform': 15440}
CONNECTED_MUTATIONS = ['native_drop_previous_position_history', 'native_early_high_speed_blend_cancel', 'native_wrong_physical_local_blend', 'native_wrong_reset_order', 'source_drop_previous_velocity_history']

def rounded(value): return struct.unpack('<f', struct.pack('<f',value))[0]
def bits32(value): return struct.unpack('<I',struct.pack('<f',value))[0]

def blended_matrix(virtual, physical, elapsed, duration):
    a = rounded(f32(elapsed)/f32(duration)); virtual=[f32(x) for x in virtual]; physical=[f32(x) for x in physical]
    def lerp(x,y): return x if a <= 0 else y if a >= 1 else rounded(x+rounded(a*rounded(y-x)))
    def cross(x,y): return [rounded(rounded(x[(i+1)%3]*y[(i+2)%3])-rounded(y[(i+1)%3]*x[(i+2)%3])) for i in range(3)]
    def norm(x):
        sq=[rounded(v*v) for v in x]; n=rounded(rounded(sq[0]+sq[1])+sq[2]); q=rounded(1.0/rounded(math.sqrt(n))) if n else f32(0x7f7fffff)
        return [rounded(v*q) for v in x]
    pos=[lerp(virtual[12+i],physical[12+i]) for i in range(3)]
    forward=[lerp(virtual[8+i],physical[8+i]) for i in range(3)]
    up=[lerp(virtual[4+i],physical[4+i]) for i in range(3)]
    rebuilt=norm(cross(forward,cross(up,forward))); forward=norm(forward); right=cross(rebuilt,forward)
    return [bits32(v) for row in (right,rebuilt,forward,pos) for v in row+[1.0 if row is pos else 0.0]]


def trace_gate(trace):
    require(trace['schema']=='chariotlife49v.trace.v1' and trace['summary']==CONNECTED_COUNTS and len(trace['frames'])==240 and [r['frame']for r in trace['frames']]==list(range(240)), 'Complete all240 trace membership')
    initial=trace['initial']; require(initial['invalid_mode00_rejected_atomically'] is True, 'Invalid empty lifecycle mode rejected')
    require(initial['cable_bindings']['left1E0']==35 and initial['cable_bindings']['right1E4']==36, 'Actual Chariot cable bindings')
    require(initial['body_scene_source_alias']=={'main190_node':50,'main288_source_word':156672,'secondary19C_node':49,'secondary288_source_word':156160}, 'Canonical body scene source binding')
    require('secondary.time864' in initial['phase_owner'] and 'no integrated body history' in initial['body_snapshot'].lower(), 'Persistent canonical phase/body disclosure')
    controls=trace['display_controls']; require(len(controls)==2 and {r['label']for r in controls}=={'virtual-only','physical-copy-only'}, 'Two midblend display controls')
    for row in trace['frames']+controls:
        graph=row['graph']; require(len(graph)==53 and [n['index']for n in graph]==list(range(53)), 'All53 graph nodes')
        for n in graph:
            require(len(n['local_bits'])==len(n['world_bits'])==16 and all(type(n[k]) is int and -1<=n[k]<53 for k in ('parent180','first_child184','next_sibling188')), 'Complete graph lanes/links')
        for name,count,offset,root in [('chariot_palette',18,31,30),('left_palette',14,1,0),('right_palette',14,16,15)]:
            p=row[name]; require(p['entry_count']==len(p['node_world_bits'])==count and p['node_world_bits']==[n['world_bits']for n in graph[offset:offset+count]] and p['root_world_bits']==graph[root]['world_bits'], 'Current graph source palette')
    reverse=[]; blend=[]; getter=[]; dirty=[]; mode_changes=[]; total_ms=0
    reset_zero={0x34,*range(0x2b8,0x320,4)}
    reset_vectors={a+i*4 for a in (0x180,0x190,0xc0,0xd0,0x70,0x80,0x90,0xb0) for i in range(4)}
    changed_allowed=reset_zero|reset_vectors
    for i,r in enumerate(trace['frames']):
        state,before=r['root_state'],r['root_state_before']; pre,body=r['secondary_before'],r['secondary_after_root']; audit=r['lifecycle_audit']; graph=r['root_return_graph']
        require(len(graph)==len(r['pre_ik_graph'])==len(r['after21A000_graph'])==53, 'Captured pre/post source graphs')
        require(r['application_factor_bits']==r['animation_input']['animation_delta23C_bits']==r['visual_input_bits']['frameFactor']==r['root_input_bits']['frameFactor_bits'], 'One application factor')
        require(r['control_cached_speed34_bits']==r['visual_input_bits']['cachedBodySpeed34']==r['root_input_bits']['cachedBodySpeed34_bits'] and r['root_input_bits']['vehicle58']==r['root_input_bits']['vehicle40']==0, 'Shared supplied speed and ordinary admission')
        require(r['timestamp_delta_ms']==(16,17,17)[i%3] and r['control_fixture_frame']==i%96, 'Supplied native timestamp/control schedule'); total_ms+=r['timestamp_delta_ms']
        require(r['quantized_timestamp']==1000+total_ms, 'Timestamp cumulative accounting')
        require(r['shared']['permutation']==r['shared_after49R']['permutation'] and r['shared']['gradient_bits']==r['shared_after49R']['gradient_bits'] and r['shared']['random_state']==r['shared_after49R']['random_state'], 'Shared serial noise/RNG ownership')
        require(body['source_words']['288']==pre['source_words']['288']==156160 and body['cached_world_bits']==graph[49]['world_bits'] and (body['scene_flags190']&255)==graph[49]['flags190'] and (body['scene_flags190']&0xffffff00)==(pre['scene_flags190']&0xffffff00), 'Canonical physical scene/upper flags')
        require(body['phase864_bits']==r['phase864_bits'] and pre['phase864_bits']==r['phase864_before_bits'], 'Canonical phase owner')
        require(r['main_cached_scene_before49V_bits']==graph[50]['world_bits'] and (r['main_scene_flags_before49V']&255)==graph[50]['flags190'], 'Immutable main scene projection')
        if i:
            prev=trace['frames'][i-1]; require(before==prev['root_state'] and pre['source_words']==prev['secondary_after_root']['source_words'] and r['phase864_before_bits']==prev['phase864_bits'], 'Persistent lifecycle/body histories')
            cache=prev['later_getter_observation'] or prev['secondary_after_root']
            require(pre['cached_world_bits']==cache['cached_world_bits'] and pre['scene_flags190']==cache['scene_flags190'], 'Only separately observed getter cache carries forward')
        else: require(before==initial['root_state'], 'Initial lifecycle state')
        if [before[k] for k in ('vehicle838','vehicle83C')] != [state[k] for k in ('vehicle838','vehicle83C')]:mode_changes.append(i)
        if audit['reversed']:
            reverse.append(i); require(r['entry_mode']=='virtual' and r['exit_mode']=='blend' and state['vehicle838']==state['vehicle83C']==1 and state['elapsed84C_bits']==0, 'Reverse lifecycle mode/timer')
            equal_fields(audit,{'source_calls167A50':1,'source_hits167AD0':1,'source_zero_helpers':6,'body_write_bytes':236,'dirty_local_and_stale_world_preserved':True},'Reverse exact callback')
            stores=audit['ordered_store_indices']; require(len(stores)==7 and stores==sorted(set(stores)), 'Reverse ordered reset/copy stores')
            require(body['cached_world_bits']==pre['cached_world_bits']==graph[49]['world_bits'] and graph[49]['local_bits']!=graph[49]['world_bits'] and not graph[49]['flags190']&1, 'Reverse dirty local and stale world retained')
            words=body['source_words']; require(all(words[f'{p:x}']==0 for p in reset_zero), 'Reset scalar and all previous/scratch histories')
            for a in (0x180,0x190,0xc0,0xd0,0x80,0x90): require([words[f'{a+j*4:x}']for j in range(4)]==[0,0,0,0x3f800000], 'Reset exact vector W lanes')
            require([words[f'{0x70+j*4:x}']for j in range(4)]==[0,0,r['control_cached_speed34_bits'],0x80000000] and [words[f'{0xb0+j*4:x}']for j in range(4)]==[0x3d000000,0,0xbd800000,0x3f400000], 'Main70/B0 copies follow reset including W')
            require(all(v==pre['source_words'][k] for k,v in words.items() if int(k,16) not in changed_allowed), 'Unrelated secondary words preserved')
        else:
            require(audit['body_write_bytes']==audit['source_calls167A50']==audit['source_hits167AD0']==audit['source_zero_helpers']==0 and body['source_words']==pre['source_words'], 'No reactivation outside reverse')
        if r['entry_mode']=='blend':
            blend.append(i); require(audit['source_blend_rows']==6, 'Six source blend row observations')
            require(graph[30]['local_bits']==blended_matrix(graph[52]['local_bits'],graph[49]['world_bits'],before['elapsed84C_bits'],before['duration848_bits']), 'Independently reconstructed full blend basis/translation')
            elapsed=rounded(f32(before['elapsed84C_bits'])+f32(r['application_factor_bits'])); require(state['elapsed84C_bits']==bits32(elapsed) and state['vehicle83C']==int(elapsed<=f32(before['duration848_bits']) or f32(r['control_cached_speed34_bits'])>=f32(r['root_shared']['speed434750'][1])), 'Old timer blend and strict expiry')
        else: require(audit['source_blend_rows']==0, 'No blend rows outside blend entry')
        observation=r['later_getter_observation']
        require(bool(observation)==bool(state['vehicle838']), 'Later getter only after actual physical mode')
        if observation:
            getter.append(i); require(observation['source_callsite']==0x218c60 and observation['source_entry']==0x28b1a0 and observation['source_instructions']>0 and observation['physical_local_before_bits']==graph[49]['local_bits'] and observation['physical_cached_world_before_bits']==graph[49]['world_bits'] and observation['scene_flags_before']==graph[49]['flags190'], 'Getter strictly after captured root return')
            require(observation['cached_world_bits']==graph[49]['local_bits'] and (observation['scene_flags190']&255)==(graph[49]['flags190']|1) and observation['scene_flags190']&0xffffff00==body['scene_flags190']&0xffffff00, 'Isolated getter cache refresh and flags')
            if not observation['scene_flags_before']&1:dirty.append(i)
    require(total_ms==4000 and mode_changes==[16,64,125,144,160,221] and [x['frame']for x in trace['mode_transitions']]==mode_changes, 'Mode transitions and schedule')
    for t in trace['mode_transitions']:
        r=trace['frames'][t['frame']]; require(t['before']==r['root_state_before'] and t['after']==r['root_state'] and t['speed_bits']==r['control_cached_speed34_bits'], 'Exact transition snapshots')
    require(reverse==dirty==[64,160] and len(blend)==122 and len(getter)==157, 'Reverse/blend/getter exact coverage')
    require([x['frame']for x in trace['reverse_audits']]==reverse and [x['frame']for x in trace['discarded_cache_witnesses']]==reverse, 'Reverse and discarded cache witness membership')
    for row in trace['reverse_audits']:
        r=trace['frames'][row['frame']]; require(row['audit']==r['lifecycle_audit'] and row['body_before']==r['secondary_before'] and row['body_after']==r['secondary_after_root'] and row['physical_local_bits']==r['root_return_graph'][49]['local_bits'] and row['physical_stale_world_bits']==r['root_return_graph'][49]['world_bits'], 'Reverse audit exact return snapshot')
    for row in trace['discarded_cache_witnesses']:
        r=trace['frames'][row['frame']]; require(row['audit']['source_blend_rows']==6 and row['audit']['body_write_bytes']==0 and row['supplied_elapsed_bits']==0x3f000000 and row['physical_local_bits']!=row['physical_cached_world_bits'], 'Discarded cache distinction fork')
        require(row['physical_local_bits']==r['root_return_graph'][49]['local_bits'] and row['physical_cached_world_bits']==r['root_return_graph'][49]['world_bits'] and len(row['result1B4_bits'])==16 and row['result1B4_bits'] not in (row['physical_local_bits'],row['physical_cached_world_bits']) and all(math.isfinite(f32(x))for x in row['result1B4_bits']), 'Discarded stale-cache witness exact fork inputs and distinct result')
    full=trace['frames'][94]
    for row in controls:
        require(row['counterfactual'] is True and row['frame']==94 and row['left_palette']==full['left_palette'] and row['right_palette']==full['right_palette'], 'Counterfactual display/engine isolation')
        require([i for i in range(53)if row['graph'][i]['local_bits']!=full['graph'][i]['local_bits']]==[30] and row['graph'][30]['local_bits']==full['graph'][52 if row['label']=='virtual-only' else 49]['local_bits'], 'Exact selected control root only')
    return {'frames':240,'reverse_frames':reverse,'blend_frames':len(blend),'isolated_later_getters':len(getter),'dirty_getters':dirty,'independently_rebuilt_blend_matrices':len(blend)}


def connected_gate(report,export):
    require(report['passed'] is True and report['leak_sanitizer_used'] is False and set(report['runs'])=={'normal','plain','asan-ubsan'}, 'Connected modes/status')
    require(export['passed'] is True and export['schema']=='chariotlife49v.export.v1' and export['summary']==CONNECTED_COUNTS and export['consumer_controls']==CONSUMER_CONTROLS, 'Connected counters/consumer controls')
    require(export['independent_all240_frame_mesh_digests_match'] is True and export['independent_all240_frame_vertex_count']==1381200 and export['mode_transition_frames']==[16,64,125,144,160,221], 'Connected independent mesh/transition counts')
    require(len(export['frames'])==6 and {(r['frame'],r['label'])for r in export['frames']}==FRAME_KEYS, 'Connected six mesh membership')
    mesh_pins={r['mesh_path']:r['mesh_sha256']for r in export['frames']}
    for mode,row in report['runs'].items():
        strict_command(row['flags'],mode=='asan-ubsan','-std=c++20',False if mode=='plain' else 'lifecycle_stage_trace.h')
        require(row['summary']==CONNECTED_COUNTS and row['consumer_controls']==CONSUMER_CONTROLS and row['mesh_sha256']==mesh_pins and row['trace_sha256']==export['trace_sha256'] and row['exact_goldens_sha256']==export['exact_goldens_sha256'] and row['read_only_native_cli'] is (mode=='plain'), 'Connected exact mode replay')
    mutation_gate([dict(row,name=name)for name,row in report['sensitivity'].items()],CONNECTED_MUTATIONS,'Connected lifecycle')
    require(report['getter_boundary']==load('connected/getter-boundary-proof.json'), 'Connected isolated getter proof link')


def connected_gates():
    report,export=load('connected/verification.json'),load('connected/export-report.json'); connected_gate(report,export)
    frozen=load('connected/frozen-proof.json'); require(frozen['passed'] is True and frozen['native_modes']==['normal','plain','asan-ubsan'], 'Connected freeze')
    for field in ('source_sha256','input_sha256'):
        require(report[field]==export[field]==frozen[field], 'Connected frozen source membership'); pins(report[field])
    local_pins('connected',frozen['artifact_sha256'])
    trace=load('connected/trace.json'); trace_result=trace_gate(trace)
    require(sha(E/'connected/trace.json')==export['trace_sha256'] and sha(E/'connected/exact-goldens49v.txt')==export['exact_goldens_sha256'], 'Connected durable trace/goldens')
    require(load('connected/connected-sensitivity.json')=={'passed':True,'mutations':report['sensitivity']}, 'Connected mutation report')
    for mode in ('normal','plain','asan-ubsan'):
        require(json.loads((E/'connected'/f'{mode}-run.stdout.log').read_text())==CONNECTED_COUNTS and not (E/'connected'/f'{mode}-run.stderr.log').read_bytes(), 'Connected raw runtime output')
    for name,row in report['sensitivity'].items(): require((E/'connected'/(name+'.stderr.log')).read_text().strip()==row['diagnostic'], 'Connected raw mutation assertion')
    for name in ['trace.json','exact-goldens49v.txt','export-report.native.json']+[r['mesh_path']for r in export['frames']]: require((E/'connected'/name).read_bytes()==(E/'connected/asan-ubsan'/name).read_bytes(), 'Connected sanitizer exact bytes')
    return export,dict(reconstruct_vertices(trace,export),**trace_result)

def presentation_gate(passes, directory='presentation-final'):
    from PIL import Image
    report = load(f'frames/{directory}/presentation.json')
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
    for i, frame in enumerate([15, 47, 94, 135]):
        with Image.open(E / f'frames/{passes[0]}/frame{frame:03d}-full-motion.png') as image:
            rgba = image.convert('RGBA'); sources.append(rgba.copy())
            x, y = (i % 2) * 640, 40 + (i // 2) * 552
            require(images[0].crop((x, y, x + 640, y + 552)).tobytes() == rgba.tobytes(), 'Presentation tile source pixels')
    require(report['control_crop_box'] == ([288, 224, 608, 444] if directory == 'presentation-final' else [128, 120, 448, 340]), 'Presentation control crop boundary')
    for i, profile in enumerate(['full-motion', 'virtual-only', 'physical-copy-only']):
        with Image.open(E / f'frames/{passes[0]}/frame094-{profile}.raw.png') as image:
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


def render_gates(export):
    from PIL import ImageChops
    summary=load('render-validation.json'); equal_fields(summary,{'passed':True,'rendered_views':6,'independent_passes':2,'all_raw_and_caption_pixels_identical':True},'Authoritative render summary')
    pins(summary['image_and_capture_files']); canonical={}; raw_images={}; records=[]; expected_pins=set()
    for directory in ('pass1','pass2'):
        for frame,profile in sorted(FRAME_KEYS):
            stem=f'frame{frame:03d}-{profile}'; report=load(f'frames/{directory}/{stem}.json'); plan=report['plan']
            plan_gate(plan,export,frame,profile); frame_gate(report,plan)
            require(load(f'frames/{directory}/{stem}.plan.json')==plan,'Durable identical render plan')
            require(json.loads((E/f'frames/{directory}/{stem}.render.stdout').read_text())=={k:report[k]for k in ('status','images','runtime')} and not (E/f'frames/{directory}/{stem}.render.stderr').read_bytes(),'Durable render stdout/stderr')
            images=[png_gate(row)for row in report['images']]
            require([r['kind']for r in report['images']]==['raw framebuffer','captioned diagnostic'] and images[0].size==(640,480) and images[1].size==(640,552) and images[1].crop((0,0,640,480)).tobytes()==images[0].tobytes(),'Caption preserves every raw pixel')
            key=frame,profile; pair=(plan['plan_sha256'],tuple(r['sha256']for r in report['images']),tuple(i.tobytes()for i in images))
            if key in canonical:require(pair==canonical[key],'Repeated plans PNG and RGBA exact identity')
            else:canonical[key]=pair;raw_images[key]=images[0]
            view=next(r for r in summary['views']if (r['frame'],r['profile'])==key)
            equal_fields(view,{'plan_sha256':plan['plan_sha256'],'pixel_rgba8_sha256':report['pixel_rgba8_sha256'],'draw_calls':360,'triangles':73409,'gpu_payload_bytes':7604084,'two_pass_raw_and_caption_pixels_identical':True},'Render summary agrees with raw reports')
            expected_pins.update(f'evidence/chariotLifecycle49V/frames/{directory}/{stem}{suffix}'for suffix in ('.raw.png','.png','.json','.plan.json','.render.stdout','.render.stderr'))
            records.append({'pass':directory,'frame':frame,'profile':profile,'plan_sha256':plan['plan_sha256'],'pixel_rgba8_sha256':report['pixel_rgba8_sha256']})
    require(set(summary['image_and_capture_files'])==expected_pins and len(summary['views'])==6 and len({i.tobytes()for i in raw_images.values()})==6,'Exactly six distinct capture views and all durable pins')
    controls={}; full=raw_images[(94,'full-motion')]
    for name in ('virtual-only','physical-copy-only'):
        other=raw_images[(94,name)]; a,b=full.tobytes(),other.tobytes(); changed=[i for i in range(640*480)if a[4*i:4*i+4]!=b[4*i:4*i+4]]
        bounds=[min(i%640 for i in changed),min(i//640 for i in changed),max(i%640 for i in changed)+1,max(i//640 for i in changed)+1]
        controls[name]={'different_framebuffer_pixels':len(changed),'difference_bounds_xyxy':bounds,'changed_chariot_vertices':2884,'changed_engine_vertices':0}
    require(summary['controls']==controls,'Independently counted control framebuffer differences')
    return {'passes':['pass1','pass2'],'records':records,'controls':controls,'presentation':presentation_gate(['pass1','pass2'])}

def previous_report():
    return json.loads((ROOT/'evidence/chariotRoot49U/final-normal/regression-report.json').read_text())

def aggregate_expected_inputs(old):
    export=load('connected/export-report.json')
    expected=set(old['input_hashes'])|set(export['input_sha256'])|NEW_UNITS
    expected.update({'tools/test_native_chariot_lifecycle49v.py','cmake/NativeChariotLifecycle49V.cmake','tools/visual/export_chariot_lifecycle49v.cpp'})
    expected.update('evidence/chariotLifecycle49V/connected/'+n for n in ['export-report.json','export-report.native.json','trace.json','exact-goldens49v.txt'])
    expected.update('evidence/chariotLifecycle49V/connected/'+r['mesh_path']for r in export['frames'])
    expected.update('src/game/'+n+'.h'for n in NEW_SUITES if n!='chariot_lifecycle_export49v')
    expected.update('tests/native/'+n+'.cpp'for n in NEW_SUITES if n!='chariot_lifecycle_export49v')
    expected.update('evidence/chariotLifecycle49V/lifecycle/source/'+n for n in ['lifecycle_stage_trace.h','lifecycle_machine.h','lifecycle_fixture.h','lifecycle_words.h'])
    expected.add('evidence/chariotLifecycle49V/secondary/source/secondary_words49v.h')
    return expected

def report_gate(report, mode):
    sanitized = mode == 'sanitized'; old = previous_report()
    flags = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-fno-fast-math', '-frounding-math', '-ffp-contract=off']
    flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-sanitize-recover=all'] if sanitized else ['-O2']
    require(type(report['stack_usage_requested']) is bool, 'Aggregate stack request type')
    if report['stack_usage_requested']: flags += ['-fstack-usage']
    require(bool(report.get('stack_usage', [])) is report['stack_usage_requested'], 'Aggregate stack request/evidence')
    for row in report.get('stack_usage', []):
        require(digest(row['text'].encode()) == row['sha256'] and len(row['text'].encode()) == row['bytes'], 'Aggregate stack evidence')
    require(report['status'] == 'passed' and not report['errors'] and not report['changed_inputs'] and
            report['scope'] == 'all_chariot-lifecycle_checks' and report['serial'] is True, 'Aggregate status/scope')
    require(report['compile_flags'] == flags and report['link_flags'] == (['-fsanitize=address,undefined'] if sanitized else []) and
            report['sanitizers'] is sanitized and report['leak_detection'] is False, 'Aggregate strict flags/LSan boundary')
    suites = set(old['selected_tests']) | NEW_SUITES
    require(len(report['tests']) == len(report['selected_tests']) == 64 and
            {r['name'] for r in report['tests']} == set(report['selected_tests']) == suites, 'Aggregate suite membership')
    require(len(report['build']) == 56 and {r['source'] for r in report['build']} == {r['source'] for r in old['build']} | NEW_UNITS, 'Aggregate production units')
    registered = re.search(r'set\(rr_chariot_lifecycle_tests\s+([^)]*)\)', (ROOT / 'cmake/NativeChariotLifecycle49V.cmake').read_text())
    require(registered and set(registered.group(1).split()) == suites, 'CMake aggregate suite registration')
    expected = aggregate_expected_inputs(old)
    require(set(report['input_hashes']) == expected, 'Aggregate source/input membership: missing=' + repr(sorted(expected - set(report['input_hashes']))) + '; extra=' + repr(sorted(set(report['input_hashes']) - expected)))
    for name, value in old['input_hashes'].items():
        if name != 'CMakeLists.txt': require(report['input_hashes'][name] == value, 'Inherited aggregate input changed: ' + name)
    if sanitized:
        require('detect_leaks=0' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['ASAN_OPTIONS'] and
                'halt_on_error=1' in report['environment']['UBSAN_OPTIONS'], 'Aggregate sanitizer environment')
    objects = set()
    for build in report['build'] + [r['build'] for r in report['tests']]:
        require(build['status'] == 'passed' and build['returncode'] == 0 and build['command'][1:1 + len(flags)] == flags, 'Aggregate compiler status/flags')
        strict_command(build['command'], sanitized, '-std=c++20', False)
        require(build['stdout']['bytes'] == 0 and build['stdout']['sha256'] == EMPTY_SHA, 'Aggregate build stdout')
        if build.get('source') in NEW_UNITS:
            require(build['stderr']['bytes'] == 0 and build['stderr']['sha256'] == EMPTY_SHA, 'New production compiler warnings')
        if build in report['build']:
            command = build['command']; require('-c' in command and '-o' in command, 'Aggregate production object compile')
            objects.add(command[command.index('-o') + 1])
    require(len(objects) == 56, 'Aggregate unique ordinary objects')
    for test in report['tests']:
        command = test['build']['command']
        require(objects <= set(command), 'Aggregate executable must link ordinary production objects')
        run = test['run']
        require(test['status'] == run['status'] == 'passed' and run['returncode'] == 0 and
                run['command'][0] == test['executable']['path'], 'Aggregate runtime status')
        require(run['stderr']['bytes'] == 0 and run['stderr']['sha256'] == EMPTY_SHA, 'Aggregate runtime stderr')

def regression_gates():
    summary = load('regression-summary.json')
    equal_fields(summary, {'status': 'passed', 'all_mode_outputs_identical': True, 'semantic_trace_identical_to_focused': True,
        'connected_replay_exact_bytes': True, 'all_60_inherited_suite_outputs_identical': True, 'production_units_each': 56,
        'trace_checks': {'aggregate': CONNECTED_COUNTS['checks'], 'focused': CONNECTED_COUNTS['checks']},
        'aggregate_store_tracing': 'off; separately required and verified in focused normal/sanitized source proof'}, 'Aggregate summary')
    old = json.loads((ROOT / 'evidence/chariotRoot49U/regression-summary.json').read_text())['runs']['normal']['tests']
    output_checks = {'secondary_activation49v': 384196, 'chariot_blend49v': 399732, 'chariot_lifecycle49v': 1674877, 'chariot_lifecycle_export49v': CONNECTED_COUNTS['checks']}
    reports = {}
    for mode in ('normal', 'sanitized'):
        row = summary['runs'][mode]; report = load(f'final-{mode}/regression-report.json'); reports[mode] = report
        report_gate(report, mode); pins(report['input_hashes'])
        equal_fields(row, {'suites': 64, 'inputs_unchanged': True, 'tracked_inputs': len(report['input_hashes'])}, 'Aggregate mode')
        require(row['report_sha256'] == sha(E / f'final-{mode}/regression-report.json') and len(row['tests']) == 64, 'Aggregate report pin/count')
        require([r for r in row['tests'] if r['name'] not in NEW_SUITES] == old, 'All60 inherited check/stdout summaries')
        total = 0
        for test in row['tests']:
            actual = next(r for r in report['tests'] if r['name'] == test['name'])['run']; data = (E / f'final-{mode}/{test["name"]}.stdout').read_bytes()
            require(digest(data) == test['stdout_sha256'] == actual['stdout']['sha256'] and len(data) == actual['stdout']['bytes'] and
                    test['stderr_bytes'] == 0 and check_count(data.decode()) == test['checks'], 'Aggregate raw stdout/checks')
            if test['name'] in NEW_SUITES:
                require(test['checks'] == output_checks[test['name']], 'Aggregate ordinary new suite: ' + test['name'])
            else:
                require(data == (ROOT / f'evidence/chariotRoot49U/final-normal/{test["name"]}.stdout').read_bytes(), 'Inherited stdout bytes changed')
            total += test['checks']
        require(row['checks'] == summary['checks_each'] == total, 'Aggregate check sum')
        names = []
        for process in report['build'] + [t[k] for t in report['tests'] for k in ('build', 'run')]:
            for stream in ('stdout', 'stderr'):
                meta = process[stream]; name = Path(meta['path']).name; path = E / f'final-{mode}/logs/{name}'
                safe_names([name]); require(sha(path) == meta['sha256'] and path.stat().st_size == meta['bytes'] and
                    row['process_logs'][name] == {'bytes': meta['bytes'], 'sha256': meta['sha256']}, 'Durable aggregate process log')
                names.append(name)
        require(len(names) == len(set(names)) == 368 and set(names) == set(row['process_logs']), 'Aggregate process log membership')
        expected = {'chariot/' + n for n in ['trace.json', 'exact-goldens49v.txt', 'export-report.native.json']} | {'chariot/' + r['mesh_path'] for r in load('connected/export-report.json')['frames']}
        require(set(row['replayed_files']) == expected, 'Aggregate connected replay membership')
        for name, pin in row['replayed_files'].items():
            data = (E / f'final-{mode}' / name).read_bytes()
            require(digest(data) == pin and data == (E / 'connected' / name[len('chariot/'):]).read_bytes(), 'Aggregate connected replay exact bytes')
        require(json.loads((E / f'final-{mode}/chariot_lifecycle_export49v-output.stdout').read_text()) == CONNECTED_COUNTS, 'Aggregate connected replay counters')
    require(summary['runs']['normal']['tests'] == summary['runs']['sanitized']['tests'] and
            summary['runs']['normal']['replayed_files'] == summary['runs']['sanitized']['replayed_files'] and
            reports['normal']['input_hashes'] == reports['sanitized']['input_hashes'], 'Aggregate normal/sanitizer divergence')
    return summary


RESOURCE_MODES=('lifecycle','getter','animation_visual_lifecycle_palette','animation_visual_lifecycle_palette_getter','animation_visual_lifecycle_palette_rigid_getter')
RESOURCE_FIELDS=('gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','relocated_readonly_data_bytes','mutable_data_bytes','data_bytes','bss_bytes')
RESOURCE_RECORDS={'ChariotLifecycleState49V':84,'ChariotLifecycleInput49V':44,'ChariotLifecycleWorkspace49V':11820,'ChariotLifecycleDiagnostic49V':8,'SecondaryActivationSupplement49V':52,'SecondaryActivationGlobals49V':32}

def footprint_gate(f):
    require(f['status']=='measured' and f['compiler_target']=='x86_64-linux-gnu','Footprint host scope');strict_command(f['flags'],False,'-std=c++17',False)
    units=[n[4:-4]for n in sorted(NEW_UNITS)];require(set(f['incremental49V_units'])==set(units) and len(f['objects'])==25 and set(f['objects'])==set(units)|set(f['reused49U_or_earlier_support']),'Budget production object membership')
    require(len(f['compile_commands'])==len(f['objects'])+2,'Budget object and harness command membership');objects=set()
    for command in f['compile_commands'][:-2]:
        strict_command(command,False,'-std=c++17',False);require('-c'in command and '-flto'not in command and not any(x.startswith('-D')for x in command),'Ordinary budget production compile')
        objects.add(command[command.index('-o')+1]);unit=next((n for n in f['objects']if any(x.endswith('/src/'+n+'.cpp')for x in command)),None);require(unit is not None,'Budget compiled object identity')
        if unit in units:require(not any(x.startswith('-Wno')for x in command),'New production warning suppression')
    for i,command in enumerate(f['compile_commands'][-2:]):
        strict_command(command,False,'-std=c++17',False);require(objects<=set(command) and ('-DPROBE49V'in command)is bool(i) and not any(x.startswith('-D')and x!='-DPROBE49V'for x in command),'Budget identical ordinary object linkage')
    for unit,row in f['objects'].items():
        sec=row['sections'];require(row['direct_allocator_references']==[] and row['source_sha256']==f['source_hashes']['src/'+unit+'.cpp'],'Object allocator/source audit')
        require(row['executable_text_bytes']==sum(v for k,v in sec.items()if k.startswith('.text')) and row['readonly_data_bytes']==sum(v for k,v in sec.items()if k.startswith('.rodata')) and row['relocated_readonly_data_bytes']==sum(v for k,v in sec.items()if k.startswith('.data.rel.ro')) and row['mutable_data_bytes']==row['data_bytes']-row['relocated_readonly_data_bytes'],'Object section classification')
    for field,selected in [('incremental49V_totals',units),('linked_production_totals',list(f['objects']))]:require(f[field]=={k:sum(f['objects'][u][k]for u in selected)for k in RESOURCE_FIELDS},'Object subtotal arithmetic')
    require(f['incremental49V_totals']['mutable_data_bytes']==f['incremental49V_totals']['bss_bytes']==0,'New production mutable static bytes')
    records=f['record_bytes']; require(f['incremental49V_caller_records']==RESOURCE_RECORDS and f['incremental49V_caller_total_bytes']==sum(RESOURCE_RECORDS.values()),'New caller payload complete')
    for name,value in RESOURCE_RECORDS.items():require(records[name]==value,'New caller sizeof')
    replaced={k:records[k]for k in ('ChariotRootState49U','ChariotRootInput49U','ChariotRootWorkspace49U','ChariotRootDiagnostic49U')}
    require(f['replaced49U_caller_records']==replaced and f['replaced49U_caller_total_bytes']==sum(replaced.values()) and f['net49V_caller_api_delta_bytes']==sum(RESOURCE_RECORDS.values())-sum(replaced.values()),'Caller API replacement delta')
    old=json.loads((ROOT/'evidence/chariotRoot49U/budget/footprint.json').read_text()); combined={k:v for k,v in old['combined_caller_records'].items()if k not in replaced and k!='SecondaryBodyState49M'}
    combined.update(RESOURCE_RECORDS);combined.update(CoupledBodyState49M=records['CoupledBodyState49M'],LaterGetterPosition49V=16);combined['detail::SceneVectorContext49U']=4
    require(f['combined_caller_records']==combined and f['combined_caller_payload_bytes_excluding_motion_bytes_and_padding']==sum(combined.values()),'Canonical Coupled owner counted once')
    disjoint=dict(combined);disjoint.pop('CoupledBodyState49M');disjoint.update({k:records[k]for k in ('MainBodyState49K','MainSphereLanes49M','SecondaryBodyState49M')});disjoint['actualVehicle838']=4
    require(f['minimum_disjoint_canonical_caller_records']==disjoint and f['minimum_disjoint_canonical_caller_payload_bytes']==sum(disjoint.values()),'Disjoint canonical owner alternative')
    require(f['separate_later_getter_caller_records']=={'LaterGetterPosition49V':16,'detail::SceneVectorContext49U':4} and f['existing_canonical_coupled_owner_bytes']==records['CoupledBodyState49M']==1580 and f['canonical_vehicle838_member_bytes']==f['canonical_time864_member_bytes']==4 and f['retained_motion_byte_payload']==15624,'Separate getter, canonical modes/clock, immutable motion')
    c=f['workspace_composition']; require(c=={'ChariotLifecycleWorkspace49V':11820,'included_nodes64':9216,'included_state':84,'included_shared':32,'included_secondary':372,'included_supplement':52,'included_noise':2060,'included_vehicle838':4} and sum(v for k,v in c.items()if k.startswith('included_'))==c['ChariotLifecycleWorkspace49V'],'Nested scratch counted once')
    frames=f['frames'];require(frames and all(type(r['bytes'])is int and r['bytes']>=0 and r['unit']in f['objects']for r in frames),'Compiler frame membership')
    require(max(r['bytes']for r in frames if r['unit']in units)==f['maximum_individual49V_compiler_frame_bytes'],'Compiler frame maximum')
    for row in f['public49V_compiler_frames']:require(row in frames and row['unit']in units,'Public API frame evidence')
    for chain in f['selected_additive_compiler_frame_estimates']:
        total=0
        for item in chain['chain']:
            unit,fragment=item.split(':',1);matches=[r['bytes']for r in frames if r['unit']==unit and fragment in r['function']and '<lambda'not in r['function']];require(len(matches)==1,'Unambiguous frame-chain member');total+=matches[0]
        require(total==chain['sum_compiler_frame_bytes'],'Selected compiler-frame sum')
    require(f['recursive_invalidate']=={'maximum_simultaneous_invocations':64,'individual_frame_bytes':32,'compiler_frame_product_bytes':2048},'Recursive frame bound')
    require(any('not measured stack high-water'in x for x in f['limits'])and any('No Cmalloc/aligned'in x for x in f['limits']),'Budget measurement limitations')


def resource_gate(f,b):
    footprint_gate(f); equal_fields(b,{'status':'measured','host_machine':'x86_64','normal_virtual_address_limit_bytes':67108864,'cpu_seconds_limit_per_process':30,'wall_seconds_timeout_per_process':40},'Benchmark bounded host scope')
    strict_command(b['flags'],False,'-std=c++17',False)
    require(b['source_hashes']==f['source_hashes'] and b['compiler']==f['compiler'] and len(b['cpu_affinity'])==1 and b['cpu_affinity'][0]in b['available_cpu_affinity'],'Benchmark source/compiler/CPU provenance')
    require(type(b['samples_per_mode'])is int and b['samples_per_mode']>=15 and set(b['statistics'])==set(b['histories_per_sample'])==set(b['timed_stages'])==set(RESOURCE_MODES),'All benchmark modes/samples')
    require(len(b['results'])==b['samples_per_mode']*len(RESOURCE_MODES) and len(b['calibrations'])==len(b['untimed_ordinary_new_probes'])==len(RESOURCE_MODES),'Benchmark sample cardinality')
    expected=f['fixture']['expected_final_hashes'];final={k:expected[n]for k,n in [('final_graph','post_getter_graph'),('final_records','post_getter_records'),('final_palettes','palettes'),('final_positions','positions')]}
    for row in b['results']+b['calibrations']+b['untimed_ordinary_new_probes']:
        equal_fields(row,dict(final,validated_physical_calls=36,validated_virtual_calls=82,validated_blend_calls=122,validated_reverse_calls=2,validated_getter_calls=157,validated_dirty_getter_calls=2,validated_transitions=6),'All timed histories final source state')
        require(row['mode']in RESOURCE_MODES and type(row['histories'])is int and row['histories']>=2 and row['frames']==240*row['histories'] and row['retained_motion_bytes']==15624,'Benchmark history accounting')
        checksum=(final['final_graph']^final['final_records']^final['final_palettes']^(final['final_positions']if '_rigid_'in row['mode']else 0))*row['histories']&0xffffffffffffffff
        require(row['checksum']==checksum,'Benchmark continuation checksum')
    for mode in RESOURCE_MODES:
        values=[r for r in b['results']if r['mode']==mode];probes=[r for r in b['untimed_ordinary_new_probes']if r['mode']==mode];cal=[r for r in b['calibrations']if r['mode']==mode]
        require(len(values)==b['samples_per_mode'] and {r['sample']for r in values}==set(range(b['samples_per_mode'])) and len(probes)==len(cal)==1,'Complete per-mode samples')
        probe=probes[0];require(probe['allocation_counter_enabled']==1 and probe['cpp_new_allocations']==probe['nanoseconds']==0 and probe['histories']==b['histories_per_sample'][mode],'Successful enabled ordinary-new probe')
        ns=[]
        for row in values:
            require(row['allocation_counter_enabled']==row['cpp_new_allocations']==0 and type(row['nanoseconds'])is int and row['nanoseconds']>0 and row['histories']==b['histories_per_sample'][mode] and row['checksum']==probe['checksum'],'Uninstrumented timing/continuation')
            require(row['nanoseconds_per_frame']==row['nanoseconds']/row['frames'],'Per-frame denominator');ns.append(row['nanoseconds_per_frame'])
            if mode=='getter':require(row['nanoseconds_per_active_getter']==row['nanoseconds']/(row['histories']*157),'Active getter denominator')
        stats={'minimum_ns':min(ns),'p05_ns':percentile(ns,.05),'p25_ns':percentile(ns,.25),'median_ns':statistics.median(ns),'p75_ns':percentile(ns,.75),'p95_ns':percentile(ns,.95),'maximum_ns':max(ns),'mean_ns':statistics.mean(ns),'sample_duration_ns_range':[min(r['nanoseconds']for r in values),max(r['nanoseconds']for r in values)]}
        if mode=='getter':stats['median_ns_per_active_getter']=statistics.median([r['nanoseconds_per_active_getter']for r in values])
        require(b['statistics'][mode]==stats,'Independently recomputed timing statistics')
    require(any('All samples retained'in x for x in b['measurement_limits']) and any('No Cmalloc/aligned'in x for x in b['measurement_limits']),'Benchmark interpretive limits')


def resource_gates():
    f,b,fixture=load('budget/footprint.json'),load('budget/benchmark.json'),load('budget/fixture.json');resource_gate(f,b)
    pins(f['source_hashes']);pins(fixture['source_assets']);pins([f['connected_freeze'],f['baseline49U_budget']])
    require(f['fixture']==fixture and b['footprint_sha256']==sha(E/'budget/footprint.json'),'Budget report/fixture binding')
    require(fixture['trace_sha256']==sha(current_path(fixture['trace_path'])) and fixture['fixture_sha256']==sha(E/'budget/actual-history240.bin') and fixture['fixture_bytes']==(E/'budget/actual-history240.bin').stat().st_size,'Budget frozen input bytes')
    for key in ('frames','graph_nodes','physical_entry_calls','persistent_virtual_calls','blend_calls','reverse_calls','independent_later_getter_calls','dirty_later_getter_calls','source49V_noise_calls','source_noise_calls'):require(fixture[key]==CONNECTED_COUNTS[key],'Budget connected source counts')
    require(fixture['source_full_mesh_digests_verified']==240 and fixture['palette_entries']==[18,14,14] and fixture['rigid_vertices']==[2884,1411,1460] and len(fixture['all_frame_expected_hashes'])==240,'Budget full connected geometry')
    trace=load('connected/trace.json'); require(fixture['mode_transitions']==trace['mode_transitions'] and fixture['reverse_frames']==[64,160],'Budget complete lifecycle transitions')
    for row,hashes in zip(trace['frames'],fixture['all_frame_expected_hashes']):require(len(hashes)==11 and hashes[:3]==[int(row[k],16)for k in ('after21A000_graph_fnv64','after49R_graph_fnv64','after49V_graph_fnv64')] and hashes[7]==int(row['after49R_noise_fnv64'],16),'Budget original source stage hashes')
    require([fixture['expected_final_hashes'][k]for k in fixture['all_frame_expected_hash_names']]==fixture['all_frame_expected_hashes'][-1],'Budget final source row')
    baseline=f['baseline49U_manifest'];path=ROOT.parent/Path(baseline['path']).name;require(sha(path)==baseline['sha256'] and all(baseline['all_retained_production_files_identical'].values()),'Budget unchanged baseline production claim')
    listed=json.loads(path.read_text())['files'];old={r['path'][len('RacerRevengePC/'):]:r['sha256']for r in listed if r['path'].startswith('RacerRevengePC/src/')}
    require(set(baseline['all_retained_production_files_identical'])==set(old),'Budget baseline production membership');pins(old)
    sizes=[line.split()for line in (E/'budget/record-sizes.stdout').read_text().splitlines()];require({n:int(z)for n,z,a in sizes}==f['record_bytes'] and {n:int(a)for n,z,a in sizes}==f['record_alignments'],'Raw host sizes and alignments')
    for unit,row in f['objects'].items():
        stem=unit.split('/')[-1];size=(E/f'budget/{stem}-size.txt').read_text().splitlines()[1].split();require([int(x)for x in size[:3]]==[row[k]for k in ('gnu_size_text_including_readonly_and_unwind_bytes','data_bytes','bss_bytes')],'Raw object GNU size')
        sections={n:int(z)for n,z,a in re.findall(r'^(\S+)\s+(\d+)\s+(\d+)\s*$',(E/f'budget/{stem}-sections.txt').read_text(),re.M)};require(sections==row['sections'],'Raw object sections')
        frames=[]
        for line in (E/f'budget/{stem}.su').read_text().splitlines():
            function,size,kind=line.split('\t');frames.append({'unit':unit,'function':function,'bytes':int(size),'kind':kind})
        require(frames==[r for r in f['frames']if r['unit']==unit],'Raw compiler frames')
        require(not any(t in (E/f'budget/{stem}-undefined.txt').read_text()for t in ('_Znwm','_Znam','operator new','malloc','calloc','realloc','aligned_alloc')),'Raw production allocator symbols')
    symbols=(E/'budget/timed-executable-symbols.txt').read_text();require(not any(t in symbols for t in ('Machine','original::','oracle','trace::','stage_trace','countAllocations','chariot49v_test')),'No oracle/test hooks in timed binary')
    size=(E/'budget/timed-executable-size.txt').read_text().splitlines()[1].split();exe=f['timed_executable_sections'];require([int(x)for x in size[:3]]==[exe[k]for k in ('gnu_size_text_including_readonly_and_unwind_bytes','data_bytes','bss_bytes')],'Raw timed executable size')
    sections={n:int(z)for n,z,a in re.findall(r'^(\S+)\s+(\d+)\s+(\d+)\s*$',(E/'budget/timed-executable-sections.txt').read_text(),re.M)};require(sections==exe['sections'],'Raw timed executable sections')
    def parse_rows(filename):
        rows=[]
        for line in (E/'budget'/filename).read_text().splitlines():
            pairs=[s.split('=',1)for s in line.split()];require(all(len(p)==2 for p in pairs)and len({p[0]for p in pairs})==len(pairs),'Raw timing row fields');rows.append({k:v if k=='mode'else int(v)for k,v in pairs})
        return rows
    for filename,key in [('samples.stdout','results'),('calibration.stdout','calibrations'),('ordinary-new-probe.stdout','untimed_ordinary_new_probes')]:
        rows=parse_rows(filename);require(len(rows)==len(b[key]),'Raw timing sample count')
        for raw,row in zip(rows,b[key]):equal_fields(row,raw,'Raw timing sample exact')
    require(sha(E/'budget/preflight.stdout')==f['preflight_sha256'],'Raw preflight pin');preflight=parse_rows('preflight.stdout');require(len(preflight)==2,'Both timed/probe preflights')
    for i,row in enumerate(preflight):require(row['histories']==row['frames']==row['nanoseconds']==row['cpp_new_allocations']==0 and row['allocation_counter_enabled']==i and row['validated_reverse_calls']==2 and row['validated_getter_calls']==157,'Preflight source checks outside timing')
    frozen=load('budget/artifact-manifest.json');require(frozen['schema']=='chariotlife49v.budget.freeze.v1' and frozen['passed'] is True and frozen['arithmetic_recomputed'] is True and frozen['raw_samples_checked']==len(b['results']) and frozen['ordinary_new_modes_checked']==len(RESOURCE_MODES) and frozen['connected_frozen_proof_sha256']==sha(E/'connected/frozen-proof.json') and frozen['source_sha256']=={k:f['source_hashes'][k]for k in ('tools/bench_chariot_lifecycle49v.cpp','tools/profile_chariot_lifecycle49v.py')},'Budget final artifact freeze')
    pins(frozen['source_sha256']);local_pins('budget',frozen['artifact_sha256'])
    return f,b

REVIEW_ANALYTIC={'passed':True,'analytic_checks':273,'lifecycle_calls':9,'source_fixture_dependency':False,'same_owner_canonical_subobjects':True}
REVIEW_SOURCE={'passed':True,'source_calls':240,'source_checks':1126772,'full_returns':240,'source_instructions':7020822,'reverse_calls':18,'blend_calls':150,'independent_same_owner_calls':240,'independent_extra_checks':960,'oracle':'retained full-source machine and compare adapter; independently authored fixtures'}
REVIEW_MUTATIONS=['refresh_physical_world','reset_whole_body','omit_supplement_buffer','copy_world_angular_A0','skip_upper_delay_subtraction','replace_endpoint_with_matrix','publish_using_updated_elapsed','expire_at_equality','high_speed_cancels_blend','collapse_flag_word']

def independent_gate(summary,mutations):
    equal_fields(summary,{'passed':True,'analytic':REVIEW_ANALYTIC,'source_spots':REVIEW_SOURCE,'asan_ubsan':True,'leak_sanitizer':False,'semantic_mutations_killed':10,'production_language':'C++17','test_language':'C++20'},'Independent semantic proof')
    equal_fields(mutations,{'passed':True,'killed':10,'production_edited':False,'setup_failures_counted':0},'Independent mutation scope')
    require(len(mutations['mutants'])==10 and [r['name']for r in mutations['mutants']]==REVIEW_MUTATIONS,'Independent mutation membership')
    for row in mutations['mutants']:require(row['detected'] is True and row['kind']=='isolated_native_semantic_mutation' and row['failure'] and not any(s in row['failure'].lower()for s in ('sanitizer','segmentation','compile failed','runtime error')),'Independent mutation semantic failure')


def final_review_gate(report):
    require(report['schema']=='chariotlife49v.independent-final.v1' and report['passed'] is True and report['unresolved_findings']==[] and set(report['reviewed_components'])=={'source','connected','render','presentation','aggregate','budget','release'},'Independent complete final review')
    require(all(v is True or isinstance(v,dict)and v.get('passed')is True for v in report['reviewed_components'].values()),'Independent each release component passed')
    required={'tools/validate_chariot_lifecycle49v.py','tools/archive_checkpoint49v.py'}
    required.update('evidence/chariotLifecycle49V/'+n for n in ['regression-summary.json','final-normal/regression-report.json','final-sanitized/regression-report.json','budget/footprint.json','budget/benchmark.json','budget/fixture.json','lifecycle/validation/validation.json','blend/proof/validation.json','secondary/verification/verification.json','connected/export-report.json','connected/verification.json','connected/frozen-proof.json','connected/getter-boundary-proof.json','render-validation.json','frames/presentation-final/presentation.json'])
    required.update('evidence/chariotLifecycle49V/independent-review/'+n for n in ['summary.json','source-authentication.json','connected-review.json','connected-freeze-review.json','render-review.json','process-provenance-review.json','build-wiring-review.json','aggregate-review.json','budget-review.json'])
    require(required<=set(report['input_sha256']),'Independent final release pin membership');pins(report['input_sha256'])


def independent_gates(release=False):
    summary,mutations=load('independent-review/summary.json'),load('independent-review/mutations.json');independent_gate(summary,mutations);pins(summary['hashes'])
    for mode in ('normal','sanitized'):
        for stem,expected in [('analytic',REVIEW_ANALYTIC),('source-spots',REVIEW_SOURCE)]:
            require(load(f'independent-review/{stem}-{mode}.json')==expected and json.loads((E/f'independent-review/{stem}-{mode}.stdout').read_text())==expected and not (E/f'independent-review/{stem}-{mode}.stderr').read_bytes(),'Independent exact raw normal/sanitizer result')
    for r in mutations['mutants']:require((E/'independent-review'/(r['name']+'.stderr')).read_text().strip()==r['failure'],'Independent raw semantic diagnostic')
    source=load('independent-review/source-authentication.json');equal_fields(source,{'passed':True,'source_authentication_words':5017,'elf_sha256':ELF_SHA,'response_prefix_mechanically_unchanged':True},'Independent source authentication');pins(source['retained_guards_and_helpers']);require(all(r['unchanged'] is True for r in source['retained_guards_and_helpers'].values()),'Independent retained guard boundary')
    connected=load('independent-review/connected-review.json');equal_fields(connected,{'passed':True,'frames':240,'reverse_frames':[64,160],'blend_frames':122,'exact_body288_binding':'0x26200'},'Independent canonical connected audit');pins(connected['inputs'])
    frozen=load('independent-review/connected-freeze-review.json');require(frozen['passed'] is True and frozen['frozen_proof_sha256']==sha(E/'connected/frozen-proof.json') and frozen['trace_sha256']==sha(E/'connected/trace.json') and frozen['verification_sha256']==sha(E/'connected/verification.json'),'Independent freeze pins')
    image=load('independent-review/render-review.json');equal_fields(image,{'passed':True,'views':6,'passes':2},'Independent render review');pins(image['input_sha256'])
    for name,row in load('render-validation.json')['controls'].items():require(image['control_differences'][name]=={'pixels':row['different_framebuffer_pixels'],'bounds':row['difference_bounds_xyxy']},'Independent control image arithmetic')
    wiring=load('independent-review/build-wiring-review.json');equal_fields(wiring,{'passed':True,'production_units':56,'suites':64,'cmake_direct_runner_sets_exact':True,'all_sources_exist':True},'Independent build wiring');pins(wiring['input_sha256'])
    provenance=load('independent-review/process-provenance-review.json');equal_fields(provenance,{'passed':True,'process_count':52,'log_count':104,'all_logged_stderr_empty_except_expected_semantic_mutants':True},'Independent raw process provenance')
    require('original resolved argv/return-code records were not retained' in provenance['provenance_kind'] and 'reconstructed' in provenance['provenance_kind'] and provenance['runner_sha256']==sha(E/'independent-review/run_review.py') and provenance['summary_sha256']==sha(E/'independent-review/summary.json'),'Independent honest reconstructed argv boundary')
    require(len(provenance['processes'])==52 and len({r['label']for r in provenance['processes']})==52,'Independent process membership')
    log_names=[]
    for r in provenance['processes']:
        pins(list(r['logs'].values()));log_names += [v['path']for v in r['logs'].values()]
        mutated=r['label'] in REVIEW_MUTATIONS;require(r['established_return_code']==int(mutated),'Independent inferred assertion status')
        if 'compile'in r['kind'] or 'link'in r['kind']:
            cmd=r['reproducible_argv_template'];strict_command(cmd,'sanitized'in r['label'],'-std=c++17'if r['kind']in ('production_compile','isolated_mutation_compile')else '-std=c++20',False)
        if not mutated:require(r['logs']['stderr']['bytes']==0 and r['logs']['stderr']['sha256']==EMPTY_SHA,'Independent successful process raw stderr')
    require(len(set(log_names))==104,'Independent unique raw process logs')
    if release:
        final=load('independent-review/final-review.json');final_review_gate(final)
        for name in ('aggregate-review.json','budget-review.json'):
            review=load('independent-review/'+name);require(review['passed']is True,'Independent '+name)
            for field in ('input_sha256','artifact_sha256','artifacts_sha256'):
                if field in review:pins(review[field])
        return {'final_review_sha256':sha(E/'independent-review/final-review.json'),'analytic_checks_each':273,'source_calls_each':240,'source_checks_each':1126772,'semantic_mutations':10,'raw_process_logs':104,'argv_provenance':'Reconstructed templates, explicitly labeled; original argv was not retained.'}
    return {'analytic_checks_each':273,'source_calls_each':240,'source_checks_each':1126772,'semantic_mutations':10,'release_addendum_required':True}


def gate_controls(sources,export,complete=False,resources=None,review_controls=True):
    rows=[]
    def control(name,original,mutate,gate):
        gate(original);value=copy.deepcopy(original);mutate(value)
        try:gate(value)
        except GateError as error:rows.append({'name':name,'rejected':True,'diagnostic':str(error)})
        else:raise GateError('Negative release control escaped: '+name)
    for name,directory in [('lifecycle','lifecycle/validation'),('blend','blend/proof')]:
        original=[load(directory+'/'+n)for n in ('validation.json','coverage-accounting.json','mutations.json')]
        gate=lambda x,n=name:focused_gate(n,*x,sources['lifecycle'])
        control(name+'_fake_full_source_count',original,lambda x:x[0]['normal'].update(cases=0),gate)
        control(name+'_successful_hot_allocation',original,lambda x:x[0]['plain'].update(native_allocations=1),gate)
        control(name+'_source_substitution',original,lambda x,n=name:x[0].update({('source_substitutions'if n=='lifecycle'else 'original_call_target_substitutions'):1}),gate)
        control(name+'_missing_mutation',original,lambda x:x[2]['detected'].pop(),gate)
        control(name+'_mutation_crash',original,lambda x:x[2]['detected'][0].update(mismatch='AddressSanitizer crash'),gate)
        control(name+'_fast_math',original,lambda x:x[0]['flags'].append('-ffast-math'),gate)
        if name=='lifecycle':
            control('lifecycle_lost_return',original,lambda x:next(r for b in x[1]['blocks']for r in b['pcs']if integer(r['pc'])==0x21c110).update(hits=0),gate)
            control('lifecycle_coverage_word_removed',original,lambda x:x[1]['blocks'][0]['pcs'].pop(),gate)
            control('lifecycle_helper_gaps_hidden',original,lambda x:x[1]['uncovered_classes'].update(unexercised_helper_branch=0),gate)
            control('lifecycle_duplicate_mutant_kind',original,lambda x:x[2]['detected'][27].update(kind='original_word'),gate)
        else:
            control('blend_coverage_word_changed',original,lambda x:x[1]['blend'][0].update(word='00000000'),gate)
            control('blend_lost_row_calls',original,lambda x:x[1]['row_getter'][0].update(hits=0),gate)
    connected=[load('connected/verification.json'),export];gate=lambda x:connected_gate(*x)
    control('connected_source_substitution',connected,lambda x:x[1]['summary'].update(source_substitutions=1),gate)
    control('connected_lost_persistent_reverse',connected,lambda x:x[1]['summary'].update(reverse_calls=0),gate)
    control('connected_missing_display_control',connected,lambda x:x[1]['frames'].pop(),gate)
    control('connected_claims_broad_float32_sensitivity',connected,lambda x:x[1]['consumer_controls'].update(wrong_float32_semantic_input=5755),gate)
    control('connected_plain_trace_leak',connected,lambda x:x[0]['runs']['plain']['flags'].extend(['-include','lifecycle_stage_trace.h']),gate)
    control('connected_mutation_crash',connected,lambda x:next(iter(x[0]['sensitivity'].values())).update(returncode=-11),gate)
    trace=load('connected/trace.json')
    trace_controls=[
        ('trace_duplicate_phase_reset',lambda x:x['frames'][20].update(phase864_before_bits=0)),
        ('trace_missing_graph_node',lambda x:x['frames'][10]['graph'].pop()),
        ('trace_stale_palette',lambda x:x['frames'][10]['chariot_palette']['node_world_bits'][0].__setitem__(12,0)),
        ('trace_lost_body_history',lambda x:x['frames'][65]['secondary_before']['source_words'].__setitem__('70',1)),
        ('trace_discard_main70_W',lambda x:x['frames'][64]['secondary_after_root']['source_words'].__setitem__('7c',0)),
        ('trace_invent_quaternion_identity',lambda x:x['frames'][64]['secondary_after_root']['source_words'].__setitem__('2e8',0x3f800000)),
        ('trace_reset_wrong_order',lambda x:x['frames'][64]['lifecycle_audit']['ordered_store_indices'].reverse()),
        ('trace_eager_reverse_refresh',lambda x:x['frames'][64]['secondary_after_root'].update(cached_world_bits=x['frames'][64]['root_return_graph'][49]['local_bits'])),
        ('trace_collapse_flag_high_bytes',lambda x:x['frames'][64]['secondary_after_root'].update(scene_flags190=132)),
        ('trace_old_elapsed_basis_changed',lambda x:x['frames'][94]['root_return_graph'][30]['local_bits'].__setitem__(4,0)),
        ('trace_expiry_at_equality',lambda x:x['frames'][124]['root_state'].update(vehicle83C=0)),
        ('trace_high_speed_cancels_blend',lambda x:x['frames'][176]['root_state'].update(vehicle838=0)),
        ('trace_later_getter_before_return',lambda x:x['frames'][64]['later_getter_observation'].update(physical_cached_world_before_bits=x['frames'][64]['later_getter_observation']['cached_world_bits'])),
        ('trace_getter_missing',lambda x:x['frames'][64].update(later_getter_observation=None)),
        ('trace_unstated_source_callsite',lambda x:x['frames'][64]['later_getter_observation'].update(source_callsite=0)),
        ('trace_missing_discarded_fork',lambda x:x['discarded_cache_witnesses'].pop()),
        ('trace_control_engine_leak',lambda x:x['display_controls'][0]['left_palette']['node_world_bits'][0].__setitem__(12,0))]
    for name,mutate in trace_controls:control(name,trace,mutate,trace_gate)
    baseline=current_path(export['baseline_mesh_path']).read_bytes();mesh=bytearray((E/'connected'/export['frames'][0]['mesh_path']).read_bytes())
    for name,at in [('chariot_uv',28),('engine_color',16+2884*24+20),('index',16+5755*24)]:control(name+'_bytes_changed',mesh,lambda x,a=at:x.__setitem__(a,x[a]^1),lambda x:mesh_gate(x,baseline))
    rendered=load('frames/pass2/frame094-full-motion.json');plan=rendered['plan'];gate=lambda x:plan_gate(x,export,94,'full-motion')
    control('render_missing_draw',plan,lambda x:x.update(total_draw_calls=359),gate)
    control('render_stale_export',plan,lambda x:x['native_export'].update(sha256='0'*64),gate)
    control('render_original_lighting_claim',plan,lambda x:x['profile'].update(original_lighting_or_visibility=True),gate)
    control('render_caption_changes_pixels',rendered,lambda x:x.update(framebuffer_unchanged_by_caption=False),lambda x:frame_gate(x,plan))
    review=[load('independent-review/summary.json'),load('independent-review/mutations.json')];gate=lambda x:independent_gate(*x)
    control('independent_missing_full_returns',review,lambda x:x[0]['source_spots'].update(full_returns=0),gate)
    control('independent_missing_semantic_mutant',review,lambda x:x[1]['mutants'].pop(),gate)
    if complete:
        for mode in ('normal','sanitized'):
            report=load(f'final-{mode}/regression-report.json');gate=lambda x,m=mode:report_gate(x,m)
            control(mode+'_aggregate_hidden_failed_return',report,lambda x:x['tests'][0]['run'].update(returncode=1),gate)
            control(mode+'_aggregate_missing_suite',report,lambda x:x['tests'].pop(),gate)
            control(mode+'_aggregate_missing_object',report,lambda x:x['build'].pop(),gate)
            control(mode+'_aggregate_fast_math',report,lambda x:x['build'][0]['command'].append('-ffast-math'),gate)
            control(mode+'_aggregate_traced_object',report,lambda x:x['build'][0]['command'].extend(['-include','lifecycle_stage_trace.h']),gate)
            control(mode+'_aggregate_changed_input',report,lambda x:x['changed_inputs'].append('src/game/chariot_lifecycle49v.cpp'),gate)
            control(mode+'_aggregate_missing_input',report,lambda x:x['input_hashes'].pop('src/game/chariot_lifecycle49v.cpp'),gate)
            control(mode+'_aggregate_runtime_stderr',report,lambda x:x['tests'][0]['run']['stderr'].update(bytes=1),gate)
    if complete and review_controls:
        final=load('independent-review/final-review.json')
        control('independent_open_finding',final,lambda x:x['unresolved_findings'].append('unresolved'),final_review_gate)
        control('independent_missing_budget_review',final,lambda x:x['reviewed_components'].pop('budget'),final_review_gate)
        control('independent_stale_validator_pin',final,lambda x:x['input_sha256'].__setitem__('tools/validate_chariot_lifecycle49v.py','0'*64),final_review_gate)
    if resources:
        gate=lambda x:resource_gate(*x)
        control('budget_nested_scratch_double_count',resources,lambda x:x[0].update(incremental49V_caller_total_bytes=99999),gate)
        control('budget_canonical_owner_omitted',resources,lambda x:x[0]['combined_caller_records'].pop('CoupledBodyState49M'),gate)
        control('budget_nested_secondary_double_count',resources,lambda x:x[0]['combined_caller_records'].update(SecondaryBodyState49M=372),gate)
        control('budget_missing_getter_cost',resources,lambda x:x[0]['combined_caller_records'].pop('LaterGetterPosition49V'),gate)
        control('budget_allocator_reference',resources,lambda x:x[0]['objects']['game/chariot_lifecycle49v']['direct_allocator_references'].append('malloc'),gate)
        control('budget_wrong_section_sum',resources,lambda x:x[0]['incremental49V_totals'].update(executable_text_bytes=0),gate)
        control('budget_wrong_frame_chain',resources,lambda x:x[0]['selected_additive_compiler_frame_estimates'][0].update(sum_compiler_frame_bytes=0),gate)
        control('timing_missing_sample',resources,lambda x:x[1]['results'].pop(),gate)
        control('timing_wrong_median',resources,lambda x:x[1]['statistics']['lifecycle'].update(median_ns=0),gate)
        control('timing_probe_disabled',resources,lambda x:x[1]['untimed_ordinary_new_probes'][0].update(allocation_counter_enabled=0),gate)
        control('timing_hot_allocation',resources,lambda x:x[1]['untimed_ordinary_new_probes'][0].update(cpp_new_allocations=1),gate)
        control('timing_missing_getter_continuation',resources,lambda x:x[1]['results'][0].update(validated_getter_calls=0),gate)
        control('timing_wrong_checksum',resources,lambda x:x[1]['results'][0].update(checksum=0),gate)
        control('timing_wrong_getter_denominator',resources,lambda x:next(r for r in x[1]['results']if r['mode']=='getter').update(nanoseconds_per_active_getter=0),gate)
    for name,value in [('duplicate_path',['a','a']),('parent_traversal',['../a']),('absolute_path',['/a']),('windows_path',['a\\b']),('empty_component',['a//b'])]:
        try:safe_names(value)
        except GateError as error:rows.append({'name':name,'rejected':True,'diagnostic':str(error)})
        else:raise GateError('Negative path control escaped: '+name)
    return {'status':'passed','method':'Decoded evidence/trace/mesh mutations in memory only. No compiler, native diagnostic, renderer, process RAM, install, capacity or ownership operation.','control_count':len(rows),'all_rejected':True,'controls':rows}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline',type=Path,help='Exact verified49U ZIP; mandatory for complete release validation')
    parser.add_argument('--seal',action='store_true',help='Explicitly write checkpoint-validation.json only after every gate passes')
    parser.add_argument('--check-only',action='store_true',help='Require and independently verify existing seal without writes')
    parser.add_argument('--components-only',action='store_true',help='Read-only source, focused, connected, render, presentation, independent component gates')
    args=parser.parse_args();require(not(args.seal and(args.check_only or args.components_only)) and not(args.check_only and args.components_only),'Choose one seal/check/components mode')
    require(args.components_only or args.baseline is not None,'--baseline is required for full validation')
    sources=original_sources();components=component_gates(sources);export,consumer=connected_gates();renders=render_gates(export);review=independent_gates(not args.components_only)
    if args.components_only:
        controls=gate_controls(sources,export)
        print(json.dumps({'status':'passed','scope':'components only; release unsealed','source_accounting':components,'consumer':consumer,'rendered_views_each':6,'gate_controls_rejected':controls['control_count']}));return
    resources=resource_gates();aggregate=regression_gates();names,changed=baseline_gate(args.baseline);controls=gate_controls(sources,export,True,resources)
    result={'checkpoint':'49V-ChariotLifecycle','status':'passed','baseline_sha256':BASELINE_SHA,'baseline_bytes':BASELINE_BYTES,'baseline_members':len(names),'changed_baseline_members':changed,
        'regression_suites_each':64,'regression_checks_each':aggregate['checks_each'],'tracked_inputs_each':aggregate['runs']['normal']['tracked_inputs'],'production_units_each':56,'new_production_units':3,
        'normal_sanitizer_outputs_identical':True,'inherited49U_stdout_exact':True,'source_accounting':components,'independent_review':review,'connected_checks':CONNECTED_COUNTS['checks'],'connected_frames':240,'consumer':consumer,
        'authoritative_render_passes':['pass1','pass2'],'rendered_frames_each':6,'deterministic_plan_png_pixel_repeats':True,'renders':renders,'authoritative_presentation':'frames/presentation-final',
        'gate_controls_rejected':controls['control_count'],'gate_controls':controls,'resource_incremental_totals':resources[0]['incremental49V_totals'],'resource_incremental_caller_bytes':resources[0]['incremental49V_caller_total_bytes'],
        'resource_net_caller_api_delta_bytes':resources[0]['net49V_caller_api_delta_bytes'],'resource_combined_caller_payload_bytes':resources[0]['combined_caller_payload_bytes_excluding_motion_bytes_and_padding'],'retained_motion_bytes_separate':resources[0]['retained_motion_byte_payload'],
        'host_subsystem_statistics':resources[1]['statistics'],'ordinary_new_success_probes_each':len(RESOURCE_MODES),'cmake_executed':False,'focused_store_tracing':True,'aggregate_store_tracing':False,
        'limits':'Complete admitted ordinary21A660 physical/virtual/reverse/blend lifecycle and exact callback reset, with dirty/stale return preserved. Isolated later218C60 getter is separately declared and source-compared; no intervening force/cache/physics/observer execution. Canonical actual838, secondary864/history and shared noise persist. Current main/control/acceleration/clock schedule is supplied. Full49M bridge, contacts, integrated body history, live reset/spawn/FrameReady, original VU/material/lighting/visibility and whole game remain open. Float32-first consumer sensitivity is only two vertex positions. Host sizeof/sections/compiler frames and bounded subsystem timing are not process RAM, gameplay FPS or console performance. ASan+UBSan only; no LeakSanitizer claim. Independent review command templates are explicitly reconstructed; original resolved argv/return-code records were not retained. No PC/RAM/install, excluded historical capacity or49C ownership operation is executed by this read-only validator.',
        'source_hashes':seal_hashes(names)}
    if args.seal:
        result['verified_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();(E/'checkpoint-validation.json').write_text(json.dumps(result,indent=2)+'\n')
    elif args.check_only or (E/'checkpoint-validation.json').exists():
        existing=load('checkpoint-validation.json');require({k:v for k,v in existing.items()if k!='verified_utc'}==result and isinstance(existing.get('verified_utc'),str),'Existing seal differs from independently validated evidence')
    print(json.dumps({'status':'passed','seal_written':args.seal,'baseline_members':len(names),'regression_suites_each':64,'checks_each':aggregate['checks_each'],'tracked_inputs_each':result['tracked_inputs_each'],'gate_controls_rejected':controls['control_count'],'frozen_new_or_allowed_files':len(result['source_hashes'])}))

if __name__=='__main__':
    try:main()
    except (GateError,KeyError,TypeError,OSError,ValueError,IndexError,StopIteration,zipfile.BadZipFile)as error:
        print('49V release validation failed: '+str(error),file=sys.stderr);raise SystemExit(1)
