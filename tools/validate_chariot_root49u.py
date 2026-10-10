#!/usr/bin/env python3
"""Read-only 49U release gates; only explicit --seal writes checkpoint-validation.json.
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
E = ROOT / 'evidence/chariotRoot49U'
BASELINE_SHA = '1573cef7bc7227faeb5a0a2bbc7893e26a5da0abfea4b288cfe7387324f68af1'
BASELINE_MEMBERS = 24694
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
ALLOWED = {'RacerRevengePC/README.md', 'RacerRevengePC/CMakeLists.txt'}
NEW_UNITS = {'src/game/chariot_root49u.cpp', 'src/game/chariot_scene49u.cpp', 'src/assets/chariot_root_authoring49u.cpp'}
NEW_SUITES = {'chariot_scene49u', 'chariot_root49u', 'chariot_root_authoring49u', 'chariot_root_export49u'}
FRAME_KEYS = {(i, 'full-motion') for i in (0, 31, 63, 95)} | {(95, 'frozen-chariot-root'), (95, 'physical-copy-only')}
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}
class GateError(ValueError):
    pass

def load(name):
    return json.loads((E / name).read_text())

def local_pins(prefix, rows):
    safe_names(list(rows))
    pins({f'evidence/chariotRoot49U/{prefix}/{name}': value for name, value in rows.items()})

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
    require(plan['schema'] == 'chariotroot49u.visual-plan.v1' and plan['diagnostic_only'] is True and
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
    require(plan['profile']['native_chariot_and_engine_root_updates'] is (profile == 'full-motion') and
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
    require(plan['renderer49U_sha256'] == sha(ROOT / 'tools/visual/render_chariot_root49u.py'), 'Render current script pin')
    payload = dict(plan); expected_hash = payload.pop('plan_sha256')
    require(digest(json.dumps(payload, sort_keys=True, separators=(',', ':'), allow_nan=False).encode()) == expected_hash, 'Render canonical plan hash')

def frame_gate(report, plan):
    equal_fields(report, {'status': 'rendered_diagnostic_only', 'schema': 'chariotroot49u.visual-report.v1',
        'actual_draw_calls': 360, 'actual_submitted_triangles': 73409, 'actual_texture_uploads': 26,
        'actual_gl_buffer_uploads': 4, 'shared_mesh_sets': 2, 'uniform_bits_round_trip': True,
        'framebuffer_unchanged_by_caption': True, 'opaque_output_alpha': True}, 'Rendered result')
    require(report['plan'] == plan and len(report['images']) == 2 and len(report['texture_state_checks']) == 26 and
            all(r['parameters_and_level_zero_verified'] is True for r in report['texture_state_checks']), 'Rendered plan/texture verification')
    require(report['runtime']['cpu_affinity'] == [0] and report['runtime']['software_environment']['LP_NUM_THREADS'] == '1',
            'Renderer process constraints')
    require(report['pixel_rgba8_sha256'] == report['images'][0]['rgba8_sha256'], 'Rendered pixel hash marker')

def presentation_gate(passes, directory='presentation'):
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
    for i, frame in enumerate([0, 31, 63, 95]):
        with Image.open(E / f'frames/{passes[0]}/frame-{frame:03d}-full-motion.png') as image:
            rgba = image.convert('RGBA'); sources.append(rgba.copy())
            x, y = (i % 2) * 640, 40 + (i // 2) * 552
            require(images[0].crop((x, y, x + 640, y + 552)).tobytes() == rgba.tobytes(), 'Presentation tile source pixels')
    require(report['control_crop_box'] == ([288, 224, 608, 444] if directory == 'presentation-final' else [128, 120, 448, 340]), 'Presentation control crop boundary')
    for i, profile in enumerate(['full-motion', 'frozen-chariot-root', 'physical-copy-only']):
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
    order = [f'frame-{i:03d}-full-motion' for i in (0, 31, 63, 95)] + ['frame-095-frozen-chariot-root', 'frame-095-physical-copy-only']
    for name in order:
        changed = [i for i in range(640 * 480) if pixels[name][4 * i:4 * i + 4] != reference[4 * i:4 * i + 4]]
        bbox = [min(i % 640 for i in changed), min(i // 640 for i in changed), 1 + max(i % 640 for i in changed), 1 + max(i // 640 for i in changed)] if changed else None
        comparisons.append({'name': name, 'pixels_changed_vs95': len(changed), 'bbox': bbox})
    require(load('frames/pixel-verification.json') == {'status': 'passed', 'repeated_raw_and_caption_png_pairs': 12,
            'caption_framebuffer_pixels_preserved': True, 'comparisons': comparisons}, 'Raw pixel comparison report differs')
    require(comparisons[-2]['pixels_changed_vs95'] > 0 and comparisons[-1]['pixels_changed_vs95'] > 0, 'Displayed Chariot-root controls lost pixel sensitivity')
    return {'authoritative_passes': passes, 'frames': records, 'pixel_comparisons': comparisons, 'repeat_plan_png_rgba_exact': True}

def check_count(text):
    try:
        return int(json.loads(text)['checks'])
    except (ValueError, KeyError):
        match = re.search(r'checks=(\d+)', text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks', text)
        require(match is not None, 'Missing stdout check count')
        return int(match.group(1))

def baseline_gate(path):
    require(path.is_file() and not path.is_symlink() and sha(path) == BASELINE_SHA, 'Expected exact verified 49T baseline archive')
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

# The release validator independently decodes the authenticated ELF and compiled
# source arrays. It does not execute the fixture producer or its assertions.
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
    require(size >= 32 and off + size * count <= len(raw), 'ELF program headers')
    segments = [struct.unpack_from('<8I', raw, off + size * i) for i in range(count)]
    def take(start, end):
        matches = [raw[o + start - v:o + end - v] for k, o, v, _, n, *_ in segments
                   if k == 1 and v <= start < end <= v + n]
        require(len(matches) == 1 and len(matches[0]) == end - start, 'Original range must be uniquely file backed')
        return matches[0]
    result = {}
    for key, manifest_name, header_name, total in [
        ('scene', 'scene/source/manifest.json', 'scene/source/scene_words49u.h', 1228),
        ('root', 'root/source/source-manifest.json', 'root/source/root_words.h', 4897),
        ('authoring', 'authoring/source-manifest.json', 'authoring/authoring_words49u.h', 443)]:
        manifest = load(manifest_name)
        require(manifest['elf_sha256'] == ELF_SHA, 'Manifest ELF pin')
        text = (E / header_name).read_text()
        arrays = {n: b''.join(struct.pack('<I', int(w, 16)) for w in re.findall(r'0x([0-9a-fA-F]+)u', body))
                  for n, body in re.findall(r'uint32_t\s+(\w+)\[\]\s*=\s*\{(.*?)\};', text, re.S)}
        require(len(arrays) == len(manifest['blocks']), 'Compiled source array membership')
        words = {}; expected_bounds = []; cursor = 0
        for row in manifest['blocks']:
            start, end = integer(row['start']), integer(row['end_exclusive'])
            data = take(start, end); word_count = (end - start) // 4
            require(start % 4 == end % 4 == 0 and row.get('words', word_count) == word_count,
                    'Original block bounds/count')
            require(digest(data) == row['sha256'] and arrays[row['name']] == data, 'Original compiled block: ' + row['name'])
            require((E / Path(manifest_name).parent / (row['name'] + '.bin')).read_bytes() == data, 'Original sidecar: ' + row['name'])
            if 'elf_offset' in row:
                at = integer(row['elf_offset']); require(raw[at:at + len(data)] == data, 'Original ELF offset')
            expected_bounds.append((row['name'], start, row['name'], word_count, cursor)); cursor += word_count
            for i, word in enumerate(struct.unpack('<' + str(word_count) + 'I', data)):
                require(start + 4 * i not in words, 'Overlapping source blocks')
                words[start + 4 * i] = word
        if key == 'authoring':
            bounds = [(name, int(address, 16), name, int(n), int(index)) for address, n, index, name in
                      re.findall(r'\{0x([0-9a-fA-F]+)u,(\d+),(\d+),(\w+)\}', text)]
        else:
            bounds = [(name, int(address, 16), array, int(n), int(index)) for name, address, array, n, index in
                      re.findall(r'\{"(\w+)",0x([0-9a-fA-F]+)u,(\w+),(\d+),(\d+)\}', text)]
        require(bounds == expected_bounds and cursor == len(words) == total, 'Compiled source address/index table')
        byte_arrays = {n: bytes(int(w) for w in body.split(',') if w.strip()) for n, body in
                       re.findall(r'uint8_t\s+(\w+)\[\]\s*=\s*\{(.*?)\};', text, re.S)}
        for row in manifest.get('data', []):
            data = take(integer(row['start']), integer(row['end_exclusive']))
            require(digest(data) == row['sha256'] and byte_arrays[row['name']] == data and
                    (E / Path(manifest_name).parent / (row['name'] + '.bin')).read_bytes() == data, 'Original compiled static data')
        for field in ('retained_provenance', 'assets', 'inherited_proof_inputs'):
            if field in manifest: pins(manifest[field])
        for field in ('decoder', 'authored_scene', 'retained_machine'):
            if field in manifest: pins([manifest[field]])
        if key == 'root':
            equal_fields(manifest, {'words': 4897, 'root_words': 1710, 'root_return': '0021c110', 'root_delay': '0021c114'}, 'Complete root boundary')
            require(words[0x21c110] == 0x03e00008, 'Root real return instruction')
        result[key] = words
    return result

SCENE_COUNTS = {'fixtures': 523, 'operations': 5994, 'checks': 546108, 'instructions': 1076939,
    'ordered_flag_tracing': True, 'flag_stores_compared': 4548, 'native_hot_allocations': 0}
SCENE_ACCOUNT = {'original_words': 1228, 'reached_words': 1186, 'excluded_or_source_unreachable_words': 42,
    'reached_branch_pcs': 76, 'both_directions': 53, 'single_direction_with_specific_reason': 23,
    'unexercised_pcs': [], 'unknown_executed_pcs': 0, 'unaccounted_words_or_directions': 0}
SCENE_MUTATIONS = ['axes_swap_forward_up', 'relative_position_final_add_to_subtract', 'direction_drop_fourth_lane_increment',
    'inverse_direction_drop_fourth_lane_increment', 'relative_matrix_omit_parent_conversion', 'copy_local_drop_final_pair',
    'rotation_final_add_to_subtract', 'matrix_quaternion_reverse_tie', 'sincos_replace_cosine_multiply_with_add',
    'relative_world_instead_of_its_parent', 'refresh_relative_before_target_same_terminal_state']
ROOT_COUNTS = {'passed': True, 'cases': 432, 'checks': 382457, 'native_calls': 432, 'full_returns': 432,
    'source_instructions': 2909645, 'noise_calls': 1920, 'rng_calls': 3582, 'native_allocations': 0}
AUTHOR_COUNTS = {'passed': True, 'checks': 35365, 'native_calls': 429, 'native_allocations': 0,
    'binder_cases': 285, 'rejections': 115, 'sensitivity_cases': 24, 'original_instructions': 126432,
    'geometry_original_instructions': 946, 'original_covered_words': 443, 'length854': '41a64687',
    'orientationGainA94': 'c3960000', 'production_cxx17_compile': True}
ROOT_MUTATIONS = ['cold_speed_word', 'phase_store_omitted', 'previous_error_store_omitted', 'velocity_x_store_omitted',
    'position_x_store_omitted', 'omega_x_store_omitted', 'persistent_normalization_omitted', 'translation_noise_offset_2',
    'hidden_root_setter_omitted', 'display_root_setter_omitted', 'transition_hidden_seed_omitted',
    'transition_wrong_relative_matrix', 'transition_mode_store_omitted', 'final_probe_store_omitted',
    'cold_rng_increment_changed', 'derivative_sign', 'no_position_integration', 'phase_reset_each_call', 'zero_amplitude_skips_noise']
AUTHOR_MUTATIONS = ['wrong_scalar_conversion', 'wrong_degree_constant', 'wrong_authored_orientation_gain', 'wrong_geometric_radius',
    'wrong_denominator_reconversion', 'wrong_constructor_zero_sign', 'wrong_constructor_velocity_W', 'wrong_constructor_phase', 'wrong_unused_A90_conversion']

def scene_gate(report, coverage, ordinary, mode, words):
    equal_fields(report, {'status': 'pass', 'mode': 'normal' if mode == 'normal' else 'ASan+UBSan', 'elf_sha256': ELF_SHA, 'source_word_ranges_verified': 15,
        'source_words_verified': 1228, 'accounting': SCENE_ACCOUNT}, 'Scene focused proof')
    equal_fields(coverage, SCENE_COUNTS, 'Scene ordered stores')
    equal_fields(ordinary, dict(SCENE_COUNTS, checks=523970, ordered_flag_tracing=False, flag_stores_compared=0), 'Scene ordinary proof')
    require(coverage['accounting'] == SCENE_ACCOUNT, 'Scene accounting divergence')
    account = coverage_gate(coverage['pcs'], words, 'Scene')
    require(account['reached_words'] == 1186 and account['dynamic_instructions'] == 1076939 and
            account['reached_branch_instructions'] == 76 and account['both_directions'] == 53 and account['one_direction_only'] == 23,
            'Scene independently decoded source counts')
    require([{k: r[k] for k in ('pc', 'word', 'hits', 'taken', 'not_taken')} for r in coverage['pcs']] == ordinary['pcs'], 'Scene ordinary coverage divergence')
    strict_command(report['command'], mode == 'sanitizers', '-std=c++20', 'flag_trace.h')
    mutation_gate(report['sensitivity'], SCENE_MUTATIONS, 'Scene', prefix='FAIL ChariotScene49U: ')
    require('surviving VU scratch' in report['scope'] and 'VF12.W' in report['scope'] and 'arbitrary poisoned VU state is excluded' in report['scope'], 'Scene finite VU boundary omitted')
    return account

def root_gate(report, coverage, mutations, words):
    equal_fields(report, {'passed': True, 'elf_sha256': ELF_SHA, 'production_language': 'C++17', 'test_language': 'C++20',
        'normal': ROOT_COUNTS, 'sanitized': ROOT_COUNTS, 'plain': dict(ROOT_COUNTS, checks=377945),
        'leak_sanitizer': 'disabled; no LeakSanitizer claim', 'mutations': 19, 'source_substitutions': 0}, 'Root focused proof')
    require('-fno-fast-math' in report['flags'] and '-ffp-contract=off' in report['flags'] and not BAD_FLAGS.intersection(report['flags']), 'Root strict FP')
    equal_fields(coverage, {'frozen_words': 4897, 'reached_words': 3175, 'root_words': 1710, 'root_reached': 1316,
        'root_unaccounted': 0, 'uncovered_classes': {'excluded_root_mode': 394, 'provenance_only': 44, 'unexercised_helper_branch': 1284}}, 'Root coverage limits')
    rows = [r for b in coverage['blocks'] for r in b['pcs']]
    account = coverage_gate(rows, words, 'Root', False)
    require(account['dynamic_instructions'] == ROOT_COUNTS['source_instructions'] and account['reached_words'] == 3175, 'Root decoded source execution')
    classes = {}; reached_root = 0
    for r in rows:
        pc = integer(r['pc']); reached_root += bool(r['hits'] and 0x21a660 <= pc < 0x21c118)
        if not r['hits']:
            require(bool(r.get('reason')), 'Root missing coverage explanation')
            classes[r['coverage_class']] = classes.get(r['coverage_class'], 0) + 1
            if 0x21a660 <= pc < 0x21c118:
                require(r['coverage_class'] == 'excluded_root_mode' and any(a <= pc < z for a, z in
                    [(0x21ba04, 0x21beb4), (0x21beec, 0x21bf4c), (0x21bfdc, 0x21c0f4)]), 'Unexecuted admitted root instruction')
        if branch_kind(words[pc]) and r['hits']:
            require(r['branch_directions'] == ('both' if r['taken'] and r['not_taken'] else 'single') and
                    'single does not mean universal unreachability' in r['branch_scope'], 'Root branch observation boundary')
    require(classes == coverage['uncovered_classes'] and reached_root == 1316, 'Root uncovered arithmetic')
    require(next(r for r in rows if integer(r['pc']) == 0x21c110)['hits'] == 432 and
            next(r for r in rows if integer(r['pc']) == 0x21c114)['hits'] == 432, 'Root return/delay execution')
    equal_fields(mutations, {'count': 19, 'production_edited': False, 'setup_failures_counted': 0}, 'Root sensitivity scope')
    mutation_gate([dict(r, diagnostic=r['mismatch']) for r in mutations['detected']], ROOT_MUTATIONS, 'Root', prefix='mismatch ')
    return account

def authoring_gate(report, normal, sanitized):
    require(report['passed'] is True and report['normal'] == normal and report['sanitized'] == sanitized, 'Authoring report divergence')
    for mode, row in [('normal', normal), ('sanitized', sanitized)]:
        equal_fields(row, AUTHOR_COUNTS, 'Authoring actual fixture and constructors')
        strict_command(row['command_flags'], mode == 'sanitized', '-std=c++20', False)
        require(row['sanitizers'] == ('none' if mode == 'normal' else 'ASan+UBSan; leak detection disabled'), 'Authoring sanitizer boundary')
    mutation_gate([dict(r, name=n) for n, r in normal['mutations'].items()], AUTHOR_MUTATIONS,
                  'Authoring', prefix='chariot_root_authoring49u: ')

def component_gates(sources):
    accounts = {}; mode_rows = []
    for mode in ('normal', 'sanitizers'):
        report, coverage, ordinary = (load(f'scene/{mode}/' + n) for n in ('native-test-result.json', 'coverage.json', 'ordinary-coverage.json'))
        accounts['scene'] = scene_gate(report, coverage, ordinary, mode, sources['scene']); mode_rows.append(coverage)
        for stem, key in [('native', 'native_result'), ('ordinary', 'ordinary_result')]:
            require((E / f'scene/{mode}/{stem}.stdout').read_text().strip() == report[key] and
                    not (E / f'scene/{mode}/{stem}.stderr').read_bytes(), 'Scene raw runtime output')
        for r in report['sensitivity']:
            require((E / f'scene/{mode}/control_{r["name"]}.stderr').read_text().strip() == r['diagnostic'], 'Scene raw behavioral mismatch')
    require(mode_rows[0] == mode_rows[1], 'Scene normal/sanitizer coverage differs')
    report, coverage, mutations = (load('root/proof/' + n) for n in ('validation.json', 'coverage-accounting.json', 'mutations.json'))
    accounts['root'] = root_gate(report, coverage, mutations, sources['root']); pins(report['files'])
    for stem, key in [('normal', 'normal'), ('asan-ubsan', 'sanitized'), ('plain', 'plain')]:
        require(json.loads((E / f'root/proof/{stem}.stdout').read_text()) == report[key] and
                not (E / f'root/proof/{stem}.stderr').read_bytes(), 'Root raw runtime output')
        require(load('root/proof/' + key + '.json') == report[key], 'Root mode artifact divergence')
    raw = {int(p, 16): (int(h), int(t), int(n)) for p, h, t, n in
           (line.split() for line in (E / 'root/proof/coverage.tsv').read_text().splitlines())}
    require(raw == {integer(r['pc']): (r['hits'], r['taken'], r['not_taken']) for b in coverage['blocks'] for r in b['pcs']}, 'Root raw coverage differs')
    for r in mutations['detected']:
        require((E / f'root/proof/mutant-{r["name"]}.stderr').read_text().strip() == r['mismatch'], 'Root raw behavioral mismatch')
    equal_fields(load('root/proof/read-only-default.json'), {'passed': True, 'evidence_unchanged': True, 'explicit_output_only': True}, 'Root default read-only diagnostic')
    report, normal, sanitized = (load('authoring/' + n) for n in ('validation.json', 'normal-result.json', 'sanitized-result.json'))
    authoring_gate(report, normal, sanitized); pins(report['files'])
    counters = []
    for mode in ('normal', 'sanitized'):
        data = json.loads((E / f'authoring/{mode}.stdout').read_text()); equal_fields(data, {k: v for k, v in AUTHOR_COUNTS.items() if k != 'production_cxx17_compile'}, 'Authoring raw stdout')
        require(not (E / f'authoring/{mode}.stderr').read_bytes(), 'Authoring runtime stderr')
        coverage = load(f'authoring/{mode}-coverage.json'); seen = {}; total = 0
        for block in coverage['blocks']:
            for i, hits in enumerate(block['hits']):
                pc = block['start'] + 4 * i
                require(pc in sources['authoring'] and pc not in seen and type(hits) is int and hits > 0, 'Authoring source coverage')
                seen[pc] = hits; total += hits
        require(set(seen) == set(sources['authoring']) and total == 126432, 'Authoring coverage instruction arithmetic'); counters.append(seen)
    require(counters[0] == counters[1], 'Authoring normal/sanitizer coverage differs')
    accounts['authoring'] = {'compiled_original_words': 443, 'reached_words': 443, 'original_instructions': 126432, 'checks_each': 35365}
    return accounts

CONNECTED_COUNTS = {'49R_complete_calls': 96, 'checks': 8587902, 'frames': 96, 'full21A000_calls': 96,
    'graph_nodes': 53, 'later_physics_pose_copies': 0, 'moving_chariot_local_node_events': 1026,
    'moving_chariot_root_frames': 95, 'moving_engine_local_node_events': 850, 'moving_engine_root_frames': 96,
    'native_hot_allocations': 0, 'native_ik_calls': 192, 'native_vertices_all96_frames': 552480,
    'palette_calls': 294, 'palette_entries': 46, 'passed': True, 'persistent_ik_initializations': 1,
    'persistent_virtual_calls': 95, 'physical_entry_calls': 1, 'rigid_calls': 294, 'root21A660_calls': 96,
    'source21A000_instructions': 4007158, 'source48W_instructions': 45744, 'source49R_instructions': 1238560,
    'source49R_noise_calls': 1152, 'source49U_instructions': 420899, 'source49U_noise_calls': 570,
    'source_ik_calls': 192, 'source_noise_calls': 1722, 'source_palette_instructions': 392168,
    'source_rng_calls': 1791, 'source_substitutions': 0}
CONSUMER_CONTROLS = {'wrong_bind_baked_input': 23020, 'wrong_double_root': 23020, 'wrong_float32_semantic_input': 0,
    'wrong_palette_swap': 23020, 'wrong_stale_frame0_pose': 17265, 'wrong_whole_model_transform': 15440}
CONNECTED_MUTATIONS = ['native_omit_root_call', 'native_root_uses_pre49R_graph', 'native_reset_shared_noise', 'source_subtract_noise_term', 'source_omit_phase864_store']


def connected_gate(report, export):
    require(report['passed'] is True and report['leak_sanitizer_used'] is False and set(report['runs']) == {'normal', 'plain', 'asan-ubsan'}, 'Connected modes/status')
    equal_fields(export, {'passed': True, 'schema': 'chariotroot49u.export.v1', 'summary': CONNECTED_COUNTS,
        'consumer_controls': CONSUMER_CONTROLS, 'independent_all96_frame_mesh_digests_match': True,
        'independent_all96_frame_vertex_count': 552480}, 'Connected full-call proof')
    require('byte-insensitive' in export['consumer_control_limits'] and 'zero differences' in export['consumer_control_limits'], 'Insensitive float32-first control must be disclosed')
    require('not original VU vertex execution' in export['arithmetic'] and 'supplied' in export['boundary'].lower() and
            '49M' in export['boundary'] and 'Later physics is omitted' in export['boundary'], 'Connected interpretation boundary')
    for mode, run in report['runs'].items():
        equal_fields(run, {'summary': CONNECTED_COUNTS, 'consumer_controls': CONSUMER_CONTROLS,
            'read_only_native_cli': mode == 'plain', 'trace_sha256': export['trace_sha256'],
            'exact_goldens_sha256': export['exact_goldens_sha256'], 'independent_all96_frame_vertex_count': 552480,
            'independent_all96_frame_mesh_digests_match': True}, 'Connected exact replay')
        strict_command(run['flags'], mode == 'asan-ubsan', '-std=c++20', False if mode == 'plain' else 'root_stage_trace.h')
        require(run['mesh_sha256'] == {r['mesh_path']: r['mesh_sha256'] for r in export['frames']}, 'Connected mesh replay divergence')
    require(len(export['frames']) == 6 and {(r['frame'], r['label']) for r in export['frames']} == FRAME_KEYS, 'Connected frame/control membership')
    for row in export['frames']:
        equal_fields(row, {'vertices': 5755, 'indices': 10713, 'component_vertices': {'chariot': 2884, 'left_engine': 1411, 'right_engine': 1460}}, 'Connected mesh dimensions')
        require(math.isfinite(row['elapsed_seconds']) and 0 < row['elapsed_seconds'] < 10, 'Connected elapsed time')
        require(row['pose_source'] == ('actual21A000-21C120-21A660' if row['label'] == 'full-motion' else 'counterfactual-display-setters'), 'Connected control label')
        require(row['changed_vertices_vs_full'] == {'chariot': 0 if row['label'] == 'full-motion' else 2884, 'left_engine': 0, 'right_engine': 0}, 'Root control vertex isolation')
    mutation_gate([dict(r, name=n) for n, r in report['sensitivity'].items()], CONNECTED_MUTATIONS, 'Connected', prefix='FAIL chariot49U frame=')

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

def trace_gate(trace):
    require(trace['schema'] == 'chariotroot49u.trace.v1' and trace['summary'] == CONNECTED_COUNTS and
            len(trace['frames']) == 96 and [r['frame'] for r in trace['frames']] == list(range(96)), 'Connected all96 trace membership')
    initial = trace['initial']
    require(initial['graph'][52]['flags190'] == 0x80 and initial['graph'][52]['world_bits'] == [0] * 16, 'Cold hidden source node')
    require(initial['cable_bindings']['left1E0'] == 35 and initial['cable_bindings']['right1E4'] == 36,
            'Actual Chariot cable nodes')
    require('secondary.time864' in initial['phase_owner'] and 'No duplicate' in initial['phase_owner'] and
            'serially after49R into49U' in initial['noise_owner'], 'Caller-owned shared phase/noise')
    require('supplied' in initial['body_snapshot'].lower() and 'not integrated body history' in initial['body_snapshot'], 'Body snapshot disclosure')
    controls = trace['display_controls']
    require(len(controls) == 2 and {r['label'] for r in controls} == {'frozen-chariot-root', 'physical-copy-only'}, 'Trace display controls')
    for row in trace['frames'] + controls:
        graph = row['graph']
        require(len(graph) == 53 and [n['index'] for n in graph] == list(range(53)), 'Complete graph rows')
        for node in graph:
            require(len(node['local_bits']) == len(node['world_bits']) == 16, 'Complete graph matrix lanes')
            for key in ('parent180', 'first_child184', 'next_sibling188'):
                require(type(node[key]) is int and -1 <= node[key] < 53, 'Graph link index')
        for key, count, offset, root in [('chariot_palette', 18, 31, 30), ('left_palette', 14, 1, 0), ('right_palette', 14, 16, 15)]:
            pal = row[key]
            require(pal['entry_count'] == len(pal['node_world_bits']) == count and all(len(m) == 16 for m in pal['node_world_bits']), 'Complete palette matrices')
            require(pal['node_world_bits'] == [n['world_bits'] for n in graph[offset:offset + count]] and
                    pal['root_world_bits'] == graph[root]['world_bits'], 'Palette rows must equal current graph')
        require(graph[50]['local_bits'] == initial['main_pose_bits'] and graph[49]['local_bits'] == initial['secondary_pose_bits'], 'Held supplied body poses')
    for i, row in enumerate(trace['frames']):
        require(len(row['pre_ik_graph']) == len(row['after21A000_graph']) == 53, 'Original full animation graph snapshots')
        require(row['application_factor_bits'] == row['animation_input']['animation_delta23C_bits'] ==
                row['visual_input_bits']['frameFactor'] == row['root_input_bits']['frameFactor_bits'], 'Single application factor')
        require(row['control_cached_speed34_bits'] == row['visual_input_bits']['cachedBodySpeed34'] ==
                row['root_input_bits']['cachedBodySpeed34_bits'] == 1128792064, 'Shared supplied high-speed cache')
        require(row['root_state']['vehicle838'] == 0 and row['root_state']['vehicle83C'] != 0 and
                row['root_input_bits']['vehicle58'] == row['root_input_bits']['vehicle40'] == 0, 'Admitted full root mode')
        require(row['shared']['permutation'] == row['shared_after49R']['permutation'] and
                row['shared']['gradient_bits'] == row['shared_after49R']['gradient_bits'] and
                row['shared']['random_state'] == row['shared_after49R']['random_state'], 'Persistent shared noise/RNG after49R')
        if i:
            require(row['root_state_before'] == trace['frames'][i - 1]['root_state'], 'Persistent root state reset')
            require(row['phase864_before_bits'] == trace['frames'][i - 1]['phase864_bits'] and
                    f32(row['phase864_bits']) > f32(row['phase864_before_bits']), 'Canonical phase continuation')
        else:
            require(row['root_state_before']['vehicle838'] != 0 and row['root_state_before']['vehicle83C'] == 0 and
                    row['phase864_bits'] == row['phase864_before_bits'] == 0, 'Complete physical entry initial state')
    final = trace['frames'][95]
    for row in controls:
        require(row['counterfactual'] is True and row['frame'] == 95, 'Display-only control marker')
        expected = trace['frames'][31]['graph'][30]['local_bits'] if row['label'] == 'frozen-chariot-root' else final['graph'][49]['local_bits']
        require(row['graph'][30]['local_bits'] == expected and
                [i for i in range(53) if row['graph'][i]['local_bits'] != final['graph'][i]['local_bits']] == [30], 'Control must restore exact selected root only')
        require(row['left_palette'] == final['left_palette'] and row['right_palette'] == final['right_palette'], 'Root controls changed engine palettes')
        require(('frame31' if row['label'] == 'frozen-chariot-root' else 'secondary19C') in row['restored_from'], 'Control source disclosure')

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
            require(row['vertices'] == 5755 and fnv(expected) == row['mesh_fnv64'], 'Independent all96 complete mesh digest')
        key = row['frame'], label
        if key in outputs:
            meta = outputs[key]; actual = (E / 'connected' / meta['mesh_path']).read_bytes()
            require(actual == expected and digest(actual) == meta['mesh_sha256'] and mesh_gate(actual, baseline) == meta['changed_vertices_vs49H'], 'Independent exact exported vertex bytes')
            reconstructed[key] = actual
    require(controls == CONSUMER_CONTROLS, 'Independently recomputed consumer sensitivities')
    for key, actual in reconstructed.items():
        require(mesh_gate(actual, reconstructed[(key[0], 'full-motion')]) == outputs[key]['changed_vertices_vs_full'], 'Independent control vertex isolation')
    return {'independently_reconstructed_full_frame_vertices': 552480, 'exact_exported_meshes': 6,
            'consumer_controls': controls, 'float32_first_control': 'insensitive in this fixture; not killed'}

def connected_gates():
    report, export = load('connected/verification.json'), load('connected/export-report.json'); connected_gate(report, export)
    for value in (report, export): pins(value['source_sha256']); pins(value['input_sha256'])
    require(report['source_sha256'] == export['source_sha256'] and report['input_sha256'] == export['input_sha256'], 'Connected provenance divergence')
    frozen = load('connected/frozen-connected-manifest.json')
    equal_fields(frozen, {'passed': True, 'source_immutable_after_validation': True,
        'normal_plain_sanitizer_identical': True, 'native_no_arguments_read_only': True}, 'Connected source freeze')
    require(frozen['source_sha256'] == export['source_sha256'] and frozen['input_sha256'] == export['input_sha256'], 'Connected frozen provenance')
    pins(frozen['artifact_sha256'])
    trace = load('connected/trace.json'); trace_gate(trace)
    require(sha(E / 'connected/trace.json') == export['trace_sha256'] and
            sha(E / 'connected/exact-goldens49u.txt') == export['exact_goldens_sha256'], 'Connected trace/golden pins')
    require(load('connected/connected-sensitivity.json') == {'passed': True, 'mutations': report['sensitivity']}, 'Connected sensitivity artifact')
    for mode in ('normal', 'plain', 'asan-ubsan'):
        require(json.loads((E / f'connected/{mode}-run.stdout.log').read_text()) == CONNECTED_COUNTS and
                not (E / f'connected/{mode}-run.stderr.log').read_bytes(), 'Connected raw output')
    for name in ['trace.json', 'exact-goldens49u.txt', 'export-report.native.json'] + [r['mesh_path'] for r in export['frames']]:
        require((E / 'connected' / name).read_bytes() == (E / 'connected/asan-ubsan' / name).read_bytes(), 'Connected sanitizer byte replay: ' + name)
    independent = reconstruct_vertices(trace, export)
    return export, independent

def previous_report():
    return json.loads((ROOT / 'evidence/chariotAnimation49T/final-normal/regression-report.json').read_text())

def aggregate_expected_inputs(old):
    expected = set(old['input_hashes']) | set(load('connected/export-report.json')['input_sha256']) | set(load('authoring/validation.json')['files'])
    expected.update({'tools/test_native_chariot_root49u.py', 'cmake/NativeChariotRoot49U.cmake', 'tools/visual/export_chariot_root49u.cpp'})
    expected.update('evidence/chariotRoot49U/connected/' + n for n in ['export-report.json', 'export-report.native.json', 'trace.json', 'exact-goldens49u.txt'])
    expected.update('evidence/chariotRoot49U/connected/' + r['mesh_path'] for r in load('connected/export-report.json')['frames'])
    expected.update(NEW_UNITS)
    expected.update({'src/game/chariot_root49u.h', 'src/game/chariot_scene49u.h',
        'evidence/chariotRoot49U/root/source/root_words.h', 'evidence/chariotRoot49U/root/source/root_machine.h',
        'evidence/chariotRoot49U/root/source/root_fixture.h', 'evidence/chariotRoot49U/root/source/root_stage_trace.h', 'evidence/chariotRoot49U/scene/source/scene_words49u.h',
        'evidence/chariotRoot49U/scene/source/scene_machine49u.h'})
    expected.update('tests/native/' + n + '.cpp' for n in NEW_SUITES if n != 'chariot_root_export49u')
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
            report['scope'] == 'all_chariot-root_checks' and report['serial'] is True, 'Aggregate status/scope')
    require(report['compile_flags'] == flags and report['link_flags'] == (['-fsanitize=address,undefined'] if sanitized else []) and
            report['sanitizers'] is sanitized and report['leak_detection'] is False, 'Aggregate strict flags/LSan boundary')
    suites = set(old['selected_tests']) | NEW_SUITES
    require(len(report['tests']) == len(report['selected_tests']) == 60 and
            {r['name'] for r in report['tests']} == set(report['selected_tests']) == suites, 'Aggregate suite membership')
    require(len(report['build']) == 53 and {r['source'] for r in report['build']} == {r['source'] for r in old['build']} | NEW_UNITS, 'Aggregate production units')
    registered = re.search(r'set\(rr_chariot_root_tests\s+([^)]*)\)', (ROOT / 'cmake/NativeChariotRoot49U.cmake').read_text())
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
    require(len(objects) == 53, 'Aggregate unique ordinary objects')
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
        'connected_replay_exact_bytes': True, 'all_56_inherited_suite_outputs_identical': True, 'production_units_each': 53,
        'trace_checks': {'aggregate': CONNECTED_COUNTS['checks'], 'focused': CONNECTED_COUNTS['checks']},
        'aggregate_store_tracing': 'off; separately required and verified in focused normal/sanitized source proof'}, 'Aggregate summary')
    old = json.loads((ROOT / 'evidence/chariotAnimation49T/regression-summary.json').read_text())['runs']['normal']['tests']
    output_checks = {'chariot_scene49u': 523969, 'chariot_root49u': 377945, 'chariot_root_authoring49u': 35364,
                     'chariot_root_export49u': CONNECTED_COUNTS['checks']}
    reports = {}
    for mode in ('normal', 'sanitized'):
        row = summary['runs'][mode]; report = load(f'final-{mode}/regression-report.json'); reports[mode] = report
        report_gate(report, mode); pins(report['input_hashes'])
        equal_fields(row, {'suites': 60, 'inputs_unchanged': True, 'tracked_inputs': len(report['input_hashes'])}, 'Aggregate mode')
        require(row['report_sha256'] == sha(E / f'final-{mode}/regression-report.json') and len(row['tests']) == 60, 'Aggregate report pin/count')
        require([r for r in row['tests'] if r['name'] not in NEW_SUITES] == old, 'All56 inherited check/stdout summaries')
        total = 0
        for test in row['tests']:
            actual = next(r for r in report['tests'] if r['name'] == test['name'])['run']; data = (E / f'final-{mode}/{test["name"]}.stdout').read_bytes()
            require(digest(data) == test['stdout_sha256'] == actual['stdout']['sha256'] and len(data) == actual['stdout']['bytes'] and
                    test['stderr_bytes'] == 0 and check_count(data.decode()) == test['checks'], 'Aggregate raw stdout/checks')
            if test['name'] in NEW_SUITES:
                require(test['checks'] == output_checks[test['name']], 'Aggregate ordinary new suite: ' + test['name'])
            else:
                require(data == (ROOT / f'evidence/chariotAnimation49T/final-normal/{test["name"]}.stdout').read_bytes(), 'Inherited stdout bytes changed')
            total += test['checks']
        require(row['checks'] == summary['checks_each'] == total, 'Aggregate check sum')
        names = []
        for process in report['build'] + [t[k] for t in report['tests'] for k in ('build', 'run')]:
            for stream in ('stdout', 'stderr'):
                meta = process[stream]; name = Path(meta['path']).name; path = E / f'final-{mode}/logs/{name}'
                safe_names([name]); require(sha(path) == meta['sha256'] and path.stat().st_size == meta['bytes'] and
                    row['process_logs'][name] == {'bytes': meta['bytes'], 'sha256': meta['sha256']}, 'Durable aggregate process log')
                names.append(name)
        require(len(names) == len(set(names)) == 346 and set(names) == set(row['process_logs']), 'Aggregate process log membership')
        expected = {'chariot/' + n for n in ['trace.json', 'exact-goldens49u.txt', 'export-report.native.json']} | {'chariot/' + r['mesh_path'] for r in load('connected/export-report.json')['frames']}
        require(set(row['replayed_files']) == expected, 'Aggregate connected replay membership')
        for name, pin in row['replayed_files'].items():
            data = (E / f'final-{mode}' / name).read_bytes()
            require(digest(data) == pin and data == (E / 'connected' / name[len('chariot/'):]).read_bytes(), 'Aggregate connected replay exact bytes')
        require(json.loads((E / f'final-{mode}/chariot_root_export49u-output.stdout').read_text()) == CONNECTED_COUNTS, 'Aggregate connected replay counters')
    require(summary['runs']['normal']['tests'] == summary['runs']['sanitized']['tests'] and
            summary['runs']['normal']['replayed_files'] == summary['runs']['sanitized']['replayed_files'] and
            reports['normal']['input_hashes'] == reports['sanitized']['input_hashes'], 'Aggregate normal/sanitizer divergence')
    return summary

RESOURCE_MODES = ('root', 'animation_visual_root_palette', 'animation_visual_root_palette_rigid')
RESOURCE_TOTALS = {'gnu_size_text_including_readonly_and_unwind_bytes': 33049, 'executable_text_bytes': 30809,
    'readonly_data_bytes': 920, 'relocated_readonly_data_bytes': 504, 'mutable_data_bytes': 0, 'data_bytes': 504, 'bss_bytes': 0}
ROOT_RECORDS = {'ChariotRootState49U': 80, 'ChariotRootShared49U': 32, 'ChariotRootInput49U': 92,
    'ChariotRootParameters49U': 92, 'ChariotRootBindings49U': 36, 'ChariotRootWorkspace49U': 11392,
    'ChariotRootDiagnostic49U': 8, 'SceneNode49P': 144}


def footprint_gate(footprint):
    require(footprint['status'] == 'measured' and footprint['compiler_target'] == 'x86_64-linux-gnu', 'Footprint host scope')
    strict_command(footprint['flags'], False, '-std=c++17', False)
    new = ['game/chariot_root49u', 'game/chariot_scene49u', 'assets/chariot_root_authoring49u']
    require(footprint['incremental49U_units'] == new and footprint['incremental49U_totals'] == RESOURCE_TOTALS,
            'Footprint incremental object boundary')
    require(len(footprint['objects']) == 23 and len(footprint['compile_commands']) == 25 and
            set(footprint['objects']) == set(new) | set(footprint['reused49T_or_earlier_support']), 'Footprint linked unit membership')
    for command in footprint['compile_commands'][:23]:
        strict_command(command, False, '-std=c++17', False)
        require('-c' in command and '-flto' not in command and not any(x.startswith('-D') for x in command), 'Footprint ordinary C++17 objects')
        unit = next((u for u in footprint['objects'] if any(x.endswith('/src/' + u + '.cpp') for x in command)), None)
        require(unit is not None, 'Footprint unknown compiled unit')
        if unit in new: require(not any(x.startswith('-Wno') for x in command), 'New production warning suppression')
    object_paths = {command[command.index('-o') + 1] for command in footprint['compile_commands'][:23]}
    for index, command in enumerate(footprint['compile_commands'][23:]):
        strict_command(command, False, '-std=c++17', False)
        require(object_paths <= set(command) and ('-DPROBE49U' in command) is (index == 1), 'Timed/probe ordinary object linkage')
        require(not any(x.startswith('-D') and x != '-DPROBE49U' for x in command), 'Timed/probe unexpected test defines')
    for units, field in [(new, 'incremental49U_totals'), (new[:2], 'incremental49U_runtime_units_totals'), (new[2:], 'incremental49U_loadtime_unit_totals')]:
        require(footprint[field] == {k: sum(footprint['objects'][u][k] for u in units) for k in RESOURCE_TOTALS}, 'Footprint object subtotal arithmetic')
    for unit, row in footprint['objects'].items():
        require(row['direct_allocator_references'] == [] and row['source_sha256'] == footprint['source_hashes']['src/' + unit + '.cpp'], 'Production allocator/source audit')
        sections = row['sections']
        require(row['executable_text_bytes'] == sum(v for k, v in sections.items() if k.startswith('.text')) and
                row['readonly_data_bytes'] == sum(v for k, v in sections.items() if k.startswith('.rodata')) and
                row['relocated_readonly_data_bytes'] == sum(v for k, v in sections.items() if k.startswith('.data.rel.ro')) and
                row['mutable_data_bytes'] == row['data_bytes'] - row['relocated_readonly_data_bytes'], 'Object section classifications')
    require(footprint['incremental49U_caller_records'] == ROOT_RECORDS and footprint['incremental49U_caller_total_bytes'] == sum(ROOT_RECORDS.values()) == 11876,
            'Root complete caller payload')
    records = footprint['record_bytes']; combined = footprint['combined_caller_records']
    for name, value in dict(ROOT_RECORDS, **combined).items(): require(records[name] == value, 'Caller record sizeof: ' + name)
    expected_combined = dict(ROOT_RECORDS); del expected_combined['SceneNode49P']
    old = json.loads((ROOT / 'evidence/chariotAnimation49T/budget/footprint.json').read_text())
    expected_combined.update(old['combined_caller_records']); expected_combined.pop('SceneNodes52')
    expected_combined.update(SceneNodes53=7632, SecondaryBodyState49M=372)
    require(combined == expected_combined and sum(combined.values()) == footprint['combined_caller_payload_bytes_excluding_motion_bytes_and_padding'] == 361671,
            'Combined53-node complete caller payload')
    require(records['SceneNodes53'] == 53 * records['SceneNode49P'] and footprint['incremental_scene198_bytes'] == 144 and
            footprint['existing_canonical_secondary_owner_bytes'] == 372 and footprint['canonical_time864_member_bytes'] == 4 and
            footprint['retained_motion_byte_payload'] == 15624, 'Canonical clock owner and separate motion bytes')
    composition = footprint['workspace_composition']
    require(composition == {'ChariotRootWorkspace49U': 11392, 'included_nodes64': 9216, 'included_state': 80,
        'included_shared': 32, 'included_noise': 2060, 'included_phase864': 4} and
        sum(v for k, v in composition.items() if k.startswith('included_')) == composition['ChariotRootWorkspace49U'], 'Nested workspace counted once')
    frames = footprint['frames']
    require(len(frames) == 137 and all(type(r['bytes']) is int and r['bytes'] >= 0 and r['unit'] in footprint['objects'] for r in frames), 'Compiler frame membership')
    require(max(r['bytes'] for r in frames if r['unit'] in new) == footprint['maximum_individual49U_compiler_frame_bytes'] == 4000 and
            max(r['bytes'] for r in frames if r['unit'] in new[:2]) == footprint['maximum_individual49U_runtime_compiler_frame_bytes'] == 1072, 'Compiler frame maxima')
    for chain in footprint['selected_additive_compiler_frame_estimates']:
        total = 0
        for item in chain['chain']:
            unit, fragment = item.split(':', 1); matches = [r['bytes'] for r in frames if r['unit'] == unit and fragment in r['function']]
            require(len(matches) == 1, 'Stack example ambiguous member'); total += matches[0]
        require(total == chain['sum_compiler_frame_bytes'], 'Compiler frame chain arithmetic')
    require(footprint['recursive_invalidate'] == {'maximum_simultaneous_invocations': 64, 'individual_frame_bytes': 32, 'compiler_frame_product_bytes': 2048}, 'Recursive frame estimate')
    require(len(footprint['limits']) >= 6 and any('not measured stack high-water' in x for x in footprint['limits']) and
            any('no Cmalloc/aligned' in x for x in footprint['limits']), 'Footprint measurement limits')


def resource_gate(footprint, benchmark):
    footprint_gate(footprint)
    equal_fields(benchmark, {'status': 'measured', 'host_machine': 'x86_64', 'cpu_affinity': [0],
        'normal_virtual_address_limit_bytes': 67108864, 'samples_per_mode': 15,
        'cpu_seconds_limit_per_process': 30, 'wall_seconds_timeout_per_process': 40}, 'Benchmark scope')
    strict_command(benchmark['flags'], False, '-std=c++17', False)
    require(benchmark['source_hashes'] == footprint['source_hashes'] and benchmark['compiler'] == footprint['compiler'], 'Benchmark build provenance')
    require(set(benchmark['statistics']) == set(RESOURCE_MODES) and set(benchmark['histories_per_sample']) == set(RESOURCE_MODES) and
            len(benchmark['results']) == 45 and len(benchmark['calibrations']) == len(benchmark['untimed_ordinary_new_probes']) == 3, 'Benchmark sample membership')
    expected = footprint['fixture']['expected_final_hashes']
    final = {key: expected[source] for key, source in [('final_graph', 'palette_graph'), ('final_records', 'records'), ('final_palettes', 'palettes'), ('final_positions', 'positions')]}
    for row in benchmark['results'] + benchmark['calibrations'] + benchmark['untimed_ordinary_new_probes']:
        equal_fields(row, dict(final, validated_physical_calls=1, validated_virtual_calls=95), 'Benchmark expected source history')
        require(row['mode'] in RESOURCE_MODES and type(row['histories']) is int and row['histories'] >= 2 and
                row['frames'] == row['histories'] * 96 and row['retained_motion_bytes'] == 15624, 'Benchmark history accounting')
        checksum = (final['final_graph'] ^ final['final_records'] ^ final['final_palettes'] ^
                    (final['final_positions'] if row['mode'].endswith('_rigid') else 0)) * row['histories'] & 0xffffffffffffffff
        require(row['checksum'] == checksum, 'Benchmark continuation checksum')
    for mode in RESOURCE_MODES:
        values = [r for r in benchmark['results'] if r['mode'] == mode]
        probes = [r for r in benchmark['untimed_ordinary_new_probes'] if r['mode'] == mode]
        calibrations = [r for r in benchmark['calibrations'] if r['mode'] == mode]
        require(len(values) == 15 and {r['sample'] for r in values} == set(range(15)) and len(probes) == len(calibrations) == 1, 'Benchmark missing sample/mode')
        require(probes[0]['cpp_new_allocations'] == probes[0]['nanoseconds'] == 0 and probes[0]['allocation_counter_enabled'] == 1 and
                probes[0]['histories'] == benchmark['histories_per_sample'][mode], 'Enabled ordinary-new probe')
        nanos = []
        for row in values:
            require(row['allocation_counter_enabled'] == 0 and row['cpp_new_allocations'] == 0 and row['nanoseconds'] > 0 and
                    row['histories'] == benchmark['histories_per_sample'][mode] and row['checksum'] == probes[0]['checksum'], 'Timed sample instrumentation/count')
            require(row['nanoseconds_per_frame'] == row['nanoseconds'] / row['frames'], 'Timing denominator'); nanos.append(row['nanoseconds_per_frame'])
        stats = {'minimum_ns': min(nanos), 'p05_ns': percentile(nanos, .05), 'p25_ns': percentile(nanos, .25),
            'median_ns': statistics.median(nanos), 'p75_ns': percentile(nanos, .75), 'p95_ns': percentile(nanos, .95),
            'maximum_ns': max(nanos), 'mean_ns': statistics.mean(nanos),
            'sample_duration_ns_range': [min(r['nanoseconds'] for r in values), max(r['nanoseconds'] for r in values)]}
        require(benchmark['statistics'][mode] == stats, 'Benchmark statistics arithmetic')
    require([(r['sample'], r['mode']) for r in benchmark['results']] == [(sample, mode) for sample in range(15)
        for mode in (RESOURCE_MODES if sample % 2 == 0 else RESOURCE_MODES[::-1])], 'Alternating timing mode order')
    require(len(benchmark['measurement_limits']) >= 6 and any('No game FPS' in x for x in benchmark['measurement_limits']) and
            any('not a frame tail-latency' in x for x in benchmark['measurement_limits']), 'Benchmark interpretation limits')


def resource_gates():
    footprint, benchmark, fixture = load('budget/footprint.json'), load('budget/benchmark.json'), load('budget/fixture.json')
    resource_gate(footprint, benchmark); pins(footprint['source_hashes']); pins(fixture['source_assets']); pins([footprint['connected_freeze']])
    require(footprint['fixture'] == fixture and benchmark['footprint_sha256'] == sha(E / 'budget/footprint.json'), 'Resource report/fixture pins')
    require(fixture['trace_sha256'] == sha(current_path(fixture['trace_path'])) and fixture['fixture_sha256'] == sha(E / 'budget/actual-history96.bin') and
            fixture['fixture_bytes'] == (E / 'budget/actual-history96.bin').stat().st_size == 212040, 'Budget fixture bytes')
    equal_fields(fixture, {'frames': 96, 'graph_nodes': 53, 'physical_entry_calls': 1, 'persistent_virtual_calls': 95,
        'root_noise_calls': 570, 'all_noise_calls': 1722, 'palette_entries': [18, 14, 14], 'rigid_vertices': [2884, 1411, 1460],
        'source_full_mesh_digests_verified': 96}, 'Budget connected history')
    require(len(fixture['all_frame_expected_hashes']) == 96, 'Budget all-frame membership')
    trace = load('connected/trace.json')
    for row, hashes in zip(trace['frames'], fixture['all_frame_expected_hashes']):
        require(len(hashes) == 8 and hashes[:3] == [int(row[k], 16) for k in ('after21A000_graph_fnv64', 'after49R_graph_fnv64', 'after49U_graph_fnv64')] and
                hashes[-1] == int(row['after49R_noise_fnv64'], 16), 'Budget original stage hashes')
    require([fixture['expected_final_hashes'][k] for k in ('animation_graph', 'visual_graph', 'root_graph', 'palette_graph', 'records', 'palettes', 'positions', 'noise_after49R')] == fixture['all_frame_expected_hashes'][-1], 'Budget final hash row')
    baseline = footprint['baseline49T_manifest']
    require(sha(ROOT.parent / Path(baseline['path']).name) == baseline['sha256'] and all(baseline['reused_units_identical'].values()) and
            all(baseline['all_retained_dependencies_identical'].values()), 'Budget inherited source boundary')
    rows = [line.split() for line in (E / 'budget/record-sizes.stdout').read_text().splitlines()]
    require({name: int(size) for name, size, _ in rows} == footprint['record_bytes'] and
            {name: int(alignment) for name, _, alignment in rows} == footprint['record_alignments'], 'Raw record sizes/alignments')
    for unit, row in footprint['objects'].items():
        stem = unit.split('/')[-1]
        size = (E / f'budget/{stem}-size.txt').read_text().splitlines()[1].split()
        require([int(x) for x in size[:3]] == [row['gnu_size_text_including_readonly_and_unwind_bytes'], row['data_bytes'], row['bss_bytes']], 'Raw GNU object size')
        sections = {name: int(size) for name, size, _ in re.findall(r'^(\S+)\s+(\d+)\s+(\d+)\s*$', (E / f'budget/{stem}-sections.txt').read_text(), re.M)}
        require(sections == row['sections'], 'Raw object sections')
        frames = []
        for line in (E / f'budget/{stem}.su').read_text().splitlines():
            function, size, kind = line.split('\t'); frames.append({'unit': unit, 'function': function, 'bytes': int(size), 'kind': kind})
        require(frames == [r for r in footprint['frames'] if r['unit'] == unit], 'Raw compiler frame estimates')
        require(not any(token in (E / f'budget/{stem}-undefined.txt').read_text() for token in
            ('_Znwm', '_Znam', 'operator new', 'malloc', 'calloc', 'realloc', 'aligned_alloc')), 'Production allocator reference')
    symbols = (E / 'budget/timed-executable-symbols.txt').read_text()
    require(not any(word in symbols for word in ('Machine', 'original::', 'oracle', 'trace::', 'stage_trace', 'countAllocations')), 'Timed binary source oracle/test hooks')
    size = (E / 'budget/timed-executable-size.txt').read_text().splitlines()[1].split(); executable = footprint['timed_executable_sections']
    require([int(x) for x in size[:3]] == [executable['gnu_size_text_including_readonly_and_unwind_bytes'], executable['data_bytes'], executable['bss_bytes']], 'Raw timed executable size')
    sections = {name: int(size) for name, size, _ in re.findall(r'^(\S+)\s+(\d+)\s+(\d+)\s*$', (E / 'budget/timed-executable-sections.txt').read_text(), re.M)}
    require(sections == executable['sections'], 'Raw timed executable sections')
    for filename, key in [('samples.stdout', 'results'), ('calibration.stdout', 'calibrations'), ('ordinary-new-probe.stdout', 'untimed_ordinary_new_probes')]:
        raw_rows = []
        for line in (E / 'budget' / filename).read_text().splitlines():
            pairs = [part.split('=', 1) for part in line.split()]
            require(all(len(p) == 2 for p in pairs) and len({p[0] for p in pairs}) == len(pairs), 'Malformed raw timing row')
            raw_rows.append({k: value if k == 'mode' else int(value) for k, value in pairs})
        require(len(raw_rows) == len(benchmark[key]), 'Raw timing sample count')
        for raw, row in zip(raw_rows, benchmark[key]): equal_fields(row, raw, 'Raw timing sample')
    return footprint, benchmark

REVIEW_ANALYTIC = {'passed': True, 'analytic_groups': 8, 'native_calls': 43, 'checks': 142}
REVIEW_POISON = {'passed': True, 'full_original_poison_calls': 104, 'checks': 91468, 'poison_words': 8, 'mode_cache_aero_combinations': 8}
REVIEW_MUTATIONS = ['unnormalized_body80', 'recomputed_speed34', 'wrong_quaternion_reference', 'explicit_old_velocity',
    'repeated_landing_gain', 'missing_probe_publication', 'wrong_noise_offset', 'omitted_cable_roll']


def independent_gate(report, normal, sanitized, mutations):
    require(report.get('passed') is True or (report.get('passed') is False and report.get('status') == 'pending_release_addendum' and bool(report.get('pending_reviews'))), 'Independent component/release status')
    equal_fields(report, {'production_input_hashes_current': True, 'normal_and_sanitized_identical': True,
        'analytic': REVIEW_ANALYTIC, 'original_poison': REVIEW_POISON, 'all_8_semantic_mutants_detected': True,
        'connected_audit_passed': True, 'pixel_audit_passed': True, 'open_findings': []}, 'Independent review')
    require(normal['analytic'] == sanitized['analytic'] == REVIEW_ANALYTIC and normal['poison'] == sanitized['poison'] == REVIEW_POISON,
            'Independent normal/sanitizer proof')
    require(normal['sanitizers'] == 'none' and sanitized['sanitizers'] == 'ASan+UBSan, leak detection disabled', 'Independent sanitizer boundary')
    require(normal['source'] == sanitized['source'] and normal['input_sha256'] == sanitized['input_sha256'] and
            normal['test_sha256'] == sanitized['test_sha256'], 'Independent provenance divergence')
    equal_fields(normal['source'], {'elf_sha256': ELF_SHA, 'original_blocks': 47, 'compiled_original_words': 4897}, 'Independent ELF authentication')
    require(mutations['passed'] is True and mutations['production_unchanged'] is True, 'Independent mutation isolation')
    mutation_gate([dict(r, name=n) for n, r in mutations['semantic_mutants'].items()], REVIEW_MUTATIONS,
                  'Independent semantic controls', prefix='independent analytic control: ')


def independent_gates(sources, export, renders, release=False):
    report = load('independent-review/final-review.json')
    normal, sanitized, mutations = (load('independent-review/' + n) for n in ('normal/result.json', 'sanitized/result.json', 'mutations/result.json'))
    independent_gate(report, normal, sanitized, mutations)
    pins(normal['input_sha256']); local_pins('independent-review', normal['test_sha256']); local_pins('independent-review', report['review_files'])
    for row in normal['source']['independent_spot_checks'].values():
        require(sources['root'][integer(row['pc'])] == integer(row['word']), 'Independent source spot word')
    for mode in ('normal', 'sanitized'):
        for name, expected in [('analytic', REVIEW_ANALYTIC), ('poison', REVIEW_POISON)]:
            require(json.loads((E / f'independent-review/{mode}/{name}.stdout').read_text()) == expected and
                    not (E / f'independent-review/{mode}/{name}.stderr').read_bytes(), 'Independent raw runtime result')
        commands = load(f'independent-review/{mode}/commands.json')
        require(len(commands) == 5, 'Independent build/run provenance')
        for command in (commands[0], commands[1], commands[3]): strict_command(command, mode == 'sanitized', '-std=c++20', False)
    for name, row in mutations['semantic_mutants'].items():
        require((E / f'independent-review/mutations/{name}.stderr').read_text().strip() == row['diagnostic'] and
                sha(E / f'independent-review/mutations/{name}.cpp') == row['source_sha256'], 'Independent raw mutation assertion/source')
    connected = load('independent-review/connected-audit.json')
    equal_fields(connected, {'passed': True, 'continuous_frames': 96, 'all_frame_palette_entries': 4416,
        'initial198_separately_initialized': True, 'first_physical_source_transition': True, 'persistent_virtual_calls': 95,
        'shared_noise_order_records': True, 'mode838_83C_840_continuity': True, 'frozen_control_source_frame': 31,
        'source_pins_verified': len(export['source_sha256']) + len(export['input_sha256']),
        'trace_sha256': export['trace_sha256'], 'export_report_sha256': sha(E / 'connected/export-report.json')}, 'Independent connected audit')
    require(connected['meshes'] == [{'path': r['mesh_path'], 'sha256': r['mesh_sha256'], 'changed_vs_full': r['changed_vertices_vs_full']} for r in export['frames']], 'Independent exact meshes')
    pixels = load('independent-review/pixel-audit.json'); pins(pixels['image_report_sha256'])
    equal_fields(pixels, {'passed': True, 'read_only_no_GL': True, 'six_raw_images_distinct': True}, 'Independent pixel audit')
    require(len(pixels['images']) == 6 and {r['name'] for r in pixels['images']} == {f'frame-{f:03d}-{p}' for f, p in FRAME_KEYS}, 'Independent six image membership')
    for row in pixels['images']:
        require(row['both_passes_identical'] is True and row['caption_preserves_framebuffer'] is True and
                row['raw_rgba8_sha256'] == load('frames/pass-1/' + row['name'] + '.json')['pixel_rgba8_sha256'], 'Independent image digest')
    for name, row in pixels['controls_vs_full_frame95'].items():
        found = next(r for r in renders['pixel_comparisons'] if r['name'] == name); box = found['bbox']
        require(row['changed_raw_pixels'] == found['pixels_changed_vs95'] and row['difference_box_xyxy'] ==
                [box[0], box[1], box[2] - 1, box[3] - 1], 'Independent control pixel arithmetic')
    presentation = load('independent-review/presentation-audit.json')
    equal_fields(presentation, {'passed': True, 'delivery_directory': 'evidence/chariotRoot49U/frames/presentation-final',
        'crop_box': [288, 224, 608, 444], 'nearest_scale': 2, 'nearest_rgba_pixels_checked': 844800,
        'unscaled_sheet_tiles_checked': 4, 'lossless_loop_frames_checked': 4,
        'sheet_and_loop_pixels_unchanged_from_provisional': True, 'whole_chariot_poses_visually_inspected': True}, 'Independent final presentation')
    pins(presentation['artifacts_sha256']); pins({presentation['composer_path']: presentation['composer_sha256']})
    if release:
        require(report['passed'] is True and not report.get('pending_reviews', []), 'Independent release addendum is incomplete')
        require(report['presentation_final_audit_passed'] is True and report['validator_sha256'] == sha(Path(__file__)), 'Independent final validator/presentation review')
        pins(report['release_input_sha256'])
        for name in ('aggregate_review', 'budget_review'):
            value = report[name]
            if isinstance(value, str): value = load('independent-review/' + value)
            require(isinstance(value, dict) and value['passed'] is True, 'Independent ' + name)
            if 'input_sha256' in value: pins(value['input_sha256'])
            if name == 'aggregate_review':
                equal_fields(value, {'all_tracked_source_inputs_current': True}, 'Independent aggregate source pins')
                pins({value['summary_path']: value['summary_sha256']})
                require(set(value['runs']) == {'normal', 'sanitized'}, 'Independent aggregate modes')
                for mode, row in value['runs'].items():
                    equal_fields(row, {'suites': 60, 'production_units': 53, 'raw_process_logs': 346,
                        'checks': 176376609, 'report_sha256': sha(E / f'final-{mode}/regression-report.json')}, 'Independent aggregate audit')
            else:
                equal_fields(value, {'read_only_no_measurements_run': True, 'all_source_inputs_current': True,
                    'all_samples_retained_and_statistics_recomputed': True, 'new_production_object_totals': RESOURCE_TOTALS,
                    'caller_payload_bytes': 11876, 'combined_caller_payload_bytes': 361671,
                    'maximum_runtime_individual_compiler_frame_bytes': 1072,
                    'ordinary_scalar_array_new_probes_zero': True}, 'Independent budget audit')
                pins(value['artifacts_sha256'])
                statistics_ = load('budget/benchmark.json')['statistics']
                require(value['mode_statistics'] == {mode: {'samples': 15, 'median_ns_per_frame': row['median_ns'],
                    'p95_ns_per_frame': row['p95_ns'], 'range_ns_per_frame': [row['minimum_ns'], row['maximum_ns']]}
                    for mode, row in statistics_.items()}, 'Independent timing arithmetic')
    return {'final_review_sha256': sha(E / 'independent-review/final-review.json'), 'analytic_checks_each': 142,
            'original_poison_checks_each': 91468, 'semantic_mutants': 8, 'source_and_pixels_reviewed': True,
            'authoritative_presentation': 'frames/presentation-final', 'release_addendum_required': release}


def gate_controls(sources, export, aggregate=True, resources=None):
    rows = []
    def control(name, original, mutate, gate):
        value = copy.deepcopy(original); gate(value); mutate(value)
        try: gate(value)
        except GateError as error: rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else: raise GateError('Negative release control escaped: ' + name)
    for mode in ('normal', 'sanitizers'):
        original = [load(f'scene/{mode}/' + n) for n in ('native-test-result.json', 'coverage.json', 'ordinary-coverage.json')]
        gate = lambda x, m=mode: scene_gate(*x, m, sources['scene'])
        control(mode + '_scene_lost_ordered_stores', original, lambda x: x[1].update(flag_stores_compared=0), gate)
        control(mode + '_scene_source_word_changed', original, lambda x: x[1]['pcs'][0].update(word=0), gate)
        index = next(i for i, r in enumerate(original[1]['pcs']) if r['word'] >> 26 == 7 and r['hits'])
        control(mode + '_scene_BGTZ_account_lost', original, lambda x, i=index: x[1]['pcs'][i].update(taken=0, not_taken=0), gate)
        control(mode + '_scene_mutation_crash', original, lambda x: x[0]['sensitivity'][0].update(diagnostic='AddressSanitizer crash'), gate)
    root = [load('root/proof/' + n) for n in ('validation.json', 'coverage-accounting.json', 'mutations.json')]
    gate = lambda x: root_gate(*x, sources['root'])
    control('root_fake_full_return', root, lambda x: x[0]['normal'].update(full_returns=0), gate)
    control('root_native_hot_allocation', root, lambda x: x[0]['plain'].update(native_allocations=1), gate)
    control('root_source_substitution', root, lambda x: x[0].update(source_substitutions=1), gate)
    control('root_coverage_word_removed', root, lambda x: x[1]['blocks'][0]['pcs'].pop(), gate)
    control('root_mutant_setup_failure_counted', root, lambda x: x[2].update(setup_failures_counted=1), gate)
    control('root_helper_gaps_hidden', root, lambda x: x[1]['uncovered_classes'].update(unexercised_helper_branch=0), gate)
    author = [load('authoring/' + n) for n in ('validation.json', 'normal-result.json', 'sanitized-result.json')]
    control('authoring_wrong_gain', author, lambda x: x[1].update(orientationGainA94='00000000'), lambda x: authoring_gate(*x))
    control('authoring_missing_control', author, lambda x: x[1]['mutations'].pop(next(iter(x[1]['mutations']))), lambda x: authoring_gate(*x))
    connected = [load('connected/verification.json'), export]; gate = lambda x: connected_gate(*x)
    control('connected_source_substitution', connected, lambda x: x[1]['summary'].update(source_substitutions=1), gate)
    control('connected_lost_virtual_history', connected, lambda x: x[1]['summary'].update(persistent_virtual_calls=1), gate)
    control('connected_missing_control_mesh', connected, lambda x: x[1]['frames'].pop(), gate)
    control('connected_engine_control_leak', connected, lambda x: x[1]['frames'][-1]['changed_vertices_vs_full'].update(left_engine=1), gate)
    control('connected_claims_insensitive_float32_killed', connected, lambda x: x[1]['consumer_controls'].update(wrong_float32_semantic_input=1), gate)
    control('connected_plain_trace_leak', connected, lambda x: x[0]['runs']['plain']['flags'].extend(['-include', 'root_stage_trace.h']), gate)
    control('connected_mutant_crash_counted', connected, lambda x: next(iter(x[0]['sensitivity'].values())).update(returncode=-11), gate)
    trace = load('connected/trace.json')
    control('trace_duplicate_phase_reset', trace, lambda x: x['frames'][10].update(phase864_before_bits=0), trace_gate)
    control('trace_stale_source_palette', trace, lambda x: x['frames'][10]['chariot_palette']['node_world_bits'][0].__setitem__(12, 0), trace_gate)
    control('trace_incorrect_frozen_frame0', trace, lambda x: x['display_controls'][0]['graph'][30].update(local_bits=x['frames'][0]['graph'][30]['local_bits']), trace_gate)
    control('trace_missing_graph_node', trace, lambda x: x['frames'][10]['graph'].pop(), trace_gate)
    baseline = current_path(export['baseline_mesh_path']).read_bytes(); mesh = bytearray((E / 'connected' / export['frames'][0]['mesh_path']).read_bytes())
    for name, at in [('chariot_uv', 16 + 12), ('engine_color', 16 + 2884 * 24 + 20), ('index', 16 + 5755 * 24)]:
        control(name + '_bytes_changed', mesh, lambda x, a=at: x.__setitem__(a, x[a] ^ 1), lambda x: mesh_gate(x, baseline))
    rendered = load('frames/pass-2/frame-095-full-motion.json'); plan = rendered['plan']
    gate = lambda x: plan_gate(x, export, 95, 'full-motion')
    control('render_missing_draw', plan, lambda x: x.update(total_draw_calls=359), gate)
    control('render_stale_export', plan, lambda x: x['native_export'].update(sha256='0' * 64), gate)
    control('render_original_lighting_claim', plan, lambda x: x['profile'].update(original_lighting_or_visibility=True), gate)
    control('render_caption_changes_framebuffer', rendered, lambda x: x.update(framebuffer_unchanged_by_caption=False), lambda x: frame_gate(x, plan))
    review = [load('independent-review/' + n) for n in ('final-review.json', 'normal/result.json', 'sanitized/result.json', 'mutations/result.json')]
    gate = lambda x: independent_gate(*x)
    control('independent_open_finding_hidden', review, lambda x: x[0]['open_findings'].append('unresolved'), gate)
    control('independent_poison_calls_omitted', review, lambda x: x[1]['poison'].update(full_original_poison_calls=0), gate)
    control('independent_missing_semantic_mutant', review, lambda x: x[3]['semantic_mutants'].pop(next(iter(x[3]['semantic_mutants']))), gate)
    if aggregate:
        for mode in ('normal', 'sanitized'):
            report = load(f'final-{mode}/regression-report.json'); gate = lambda x, m=mode: report_gate(x, m)
            control(mode + '_aggregate_hidden_failed_return', report, lambda x: x['tests'][0]['run'].update(returncode=1), gate)
            control(mode + '_aggregate_missing_suite', report, lambda x: x['tests'].pop(), gate)
            control(mode + '_aggregate_fast_math', report, lambda x: x['build'][0]['command'].append('-ffast-math'), gate)
            control(mode + '_aggregate_traced_object', report, lambda x: x['build'][0]['command'].extend(['-include', 'root_stage_trace.h']), gate)
            control(mode + '_aggregate_changed_inputs', report, lambda x: x['changed_inputs'].append('src/game/chariot_root49u.cpp'), gate)
            control(mode + '_aggregate_runtime_stderr', report, lambda x: x['tests'][0]['run']['stderr'].update(bytes=1), gate)
    if resources:
        gate = lambda x: resource_gate(*x)
        control('budget_nested_scratch_double_count', resources, lambda x: x[0].update(incremental49U_caller_total_bytes=21092), gate)
        control('budget_canonical_clock_owner_omitted', resources, lambda x: x[0]['combined_caller_records'].pop('SecondaryBodyState49M'), gate)
        control('budget_relocated_readonly_hidden', resources, lambda x: x[0]['incremental49U_totals'].update(data_bytes=0), gate)
        control('budget_allocator_reference', resources, lambda x: x[0]['objects']['game/chariot_root49u']['direct_allocator_references'].append('malloc'), gate)
        control('timing_missing_sample', resources, lambda x: x[1]['results'].pop(), gate)
        control('timing_incorrect_median', resources, lambda x: x[1]['statistics']['root'].update(median_ns=0), gate)
        control('timing_probe_counter_disabled', resources, lambda x: x[1]['untimed_ordinary_new_probes'][0].update(allocation_counter_enabled=0), gate)
        control('timing_successful_hot_allocation', resources, lambda x: x[1]['untimed_ordinary_new_probes'][0].update(cpp_new_allocations=1), gate)
        control('timing_missing_history_checksum', resources, lambda x: x[1]['results'][0].update(checksum=0), gate)
    for name, value in [('duplicate_path', ['a', 'a']), ('parent_traversal', ['../a']), ('absolute_path', ['/a']), ('windows_path', ['a\\b']), ('empty_component', ['a//b'])]:
        try: safe_names(value)
        except GateError as error: rows.append({'name': name, 'rejected': True, 'diagnostic': str(error)})
        else: raise GateError('Negative path control escaped: ' + name)
    return {'status': 'passed', 'method': 'Decoded reports, trace and mesh bytes mutated only in memory; no C++ diagnostic reruns or production writes.',
            'control_count': len(rows), 'all_rejected': True, 'controls': rows}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, help='Exact saved verified49T ZIP; mandatory for complete validation')
    parser.add_argument('--seal', action='store_true', help='Explicitly write checkpoint-validation.json after every gate passes')
    parser.add_argument('--check-only', action='store_true', help='Require and independently verify an existing seal without writes')
    parser.add_argument('--components-only', action='store_true', help='Read-only focused, connected, render/presentation and independent review gates')
    args = parser.parse_args()
    require(not (args.seal and (args.check_only or args.components_only)), 'Seal cannot be combined with read-only partial/check modes')
    require(not (args.check_only and args.components_only), 'Choose complete verification or components')
    require(args.components_only or args.baseline is not None, '--baseline is required for complete release validation')
    sources = original_sources(); components = component_gates(sources); export, consumer = connected_gates()
    renders = render_gates(export)
    presentation = {'authoritative_directory': 'frames/presentation-final',
        'final': presentation_gate(['pass-1', 'pass-2'], 'presentation-final'),
        'provisional_preserved': presentation_gate(['pass-1', 'pass-2'])}
    review = independent_gates(sources, export, renders, not args.components_only)
    if args.components_only:
        controls = gate_controls(sources, export, False)
        print(json.dumps({'status': 'passed', 'scope': 'focused/connected/independent/render/presentation components only',
            'release_sealed': False, 'source_accounting': components, 'consumer': consumer, 'gate_controls_rejected': controls['control_count']}))
        return
    resources = resource_gates(); aggregate = regression_gates(); names, changed = baseline_gate(args.baseline)
    controls = gate_controls(sources, export, True, resources)
    result = {'checkpoint': '49U-ChariotRoot', 'status': 'passed', 'baseline_sha256': BASELINE_SHA,
        'baseline_members': len(names), 'changed_baseline_members': changed, 'regression_suites_each': 60,
        'regression_checks_each': aggregate['checks_each'], 'tracked_inputs_each': aggregate['runs']['normal']['tracked_inputs'],
        'production_units_each': 53, 'new_production_units': 3, 'normal_sanitizer_outputs_identical': True,
        'inherited49T_stdout_exact': True, 'source_accounting': components, 'independent_review': review,
        'connected_checks': CONNECTED_COUNTS['checks'], 'connected_frames': 96, 'consumer': consumer,
        'authoritative_render_passes': ['pass-1', 'pass-2'], 'rendered_frames_each': 6,
        'deterministic_plan_png_pixel_repeats': True, 'renders': renders, 'presentation': presentation,
        'gate_controls_rejected': controls['control_count'], 'gate_controls': controls,
        'resource_incremental_totals': resources[0]['incremental49U_totals'],
        'resource_incremental_caller_bytes': resources[0]['incremental49U_caller_total_bytes'],
        'resource_combined_caller_payload_bytes': resources[0]['combined_caller_payload_bytes_excluding_motion_bytes_and_padding'],
        'retained_motion_bytes_separate': resources[0]['retained_motion_byte_payload'],
        'host_subsystem_statistics': resources[1]['statistics'], 'ordinary_new_success_probes_each': 3,
        'cmake_executed': False, 'focused_store_tracing': True, 'aggregate_store_tracing': False,
        'limits': 'Complete admitted high-speed physical-to-virtual entry and stable virtual21A660 with full source return, after full21A000 and21C120. No reverse transition/blend/vehicle58 route admission. Surviving VU scratch operands used by self-subtraction must be finite; arbitrary poisoned VU state excluded. Current main/secondary poses and pre-base body/component/probe snapshots are supplied; actual caller-owned secondary.time864 and shared49M noise continue serially. Later physics/contacts, live reset/spawn/217440/FrameReady, original material/VU/visibility and whole game remain open. Root helper coverage gaps are explicit. Float32-before-double consumer control is insensitive in this fixture. Host sizeof, object sections, compiler frame estimates and finite-history subsystem timing are not process RAM, gameplay FPS or console performance. ASan+UBSan only; no LeakSanitizer claim. No PC/RAM/install, held collision-capacity or49C ownership operation is run by this validator.',
        'source_hashes': seal_hashes(names)}
    if args.seal:
        result['verified_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        (E / 'checkpoint-validation.json').write_text(json.dumps(result, indent=2) + '\n')
    elif args.check_only or (E / 'checkpoint-validation.json').exists():
        existing = load('checkpoint-validation.json')
        require({k: v for k, v in existing.items() if k != 'verified_utc'} == result and isinstance(existing.get('verified_utc'), str),
                'Existing seal differs from independently revalidated evidence')
    print(json.dumps({'status': 'passed', 'seal_written': args.seal, 'baseline_members': len(names),
        'regression_suites_each': 60, 'checks_each': aggregate['checks_each'], 'tracked_inputs_each': result['tracked_inputs_each'],
        'gate_controls_rejected': controls['control_count'], 'frozen_new_or_allowed_files': len(result['source_hashes'])}))

if __name__ == '__main__':
    try: main()
    except (GateError, KeyError, TypeError, OSError, ValueError, IndexError, StopIteration, zipfile.BadZipFile) as error:
        print('49U release validation failed: ' + str(error), file=sys.stderr)
        raise SystemExit(1)
