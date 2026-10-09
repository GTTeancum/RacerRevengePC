#!/usr/bin/env python3
"""Rebuild the bounded advancing49W view and independently verify every vertex.

All compiler products are temporary. --output-dir creates new evidence only.
--verify-frozen also executes every native binary with no arguments and proves
its frozen evidence is unchanged. No game, RAM, installation, or renderer runs.
"""
import argparse
import concurrent.futures
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
E = ROOT / 'evidence/frameBridge49W/connected'

def module(name, relative):
    spec = importlib.util.spec_from_file_location(name, ROOT / relative)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result

OLD = module('retained_visual49v', 'tools/visual/run_chariot_lifecycle49v.py')
FRAME = module('retained_frame49w', 'tools/gameplay/vehicle_frame_recover49w.py')
SOURCES = list(dict.fromkeys(['tools/visual/export_vehicle_frame49w.cpp', *FRAME.SOURCES,
    'src/game/authored_parameters49n.cpp', 'src/game/control_presets49d.cpp',
    'src/game/device_held48y.cpp', 'src/game/event_dispatch48z.cpp',
    'src/game/player_events49a.cpp', 'src/game/body_advancement49k.cpp',
    'src/game/coupled_body_advancement49m.cpp', 'src/assets/import_decode48c.cpp']))
BASE = ROOT / OLD.BASE
SCENES = [ROOT / p for p in OLD.SCENES]
COMPONENTS = [('chariot', 0, 2884), ('left_engine', 2884, 4295), ('right_engine', 4295, 5755)]
CORE_FILES = ('trace.json', 'exact-goldens49w.txt', 'export-report.native.json')

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def dump(path, data):
    Path(path).write_text(json.dumps(data, indent=2, sort_keys=True) + '\n')

def run(command, output, name, env=None):
    result = subprocess.run([str(x) for x in command], cwd=ROOT, text=True,
                            capture_output=True, env=env)
    (output / (name + '.stdout.log')).write_text(result.stdout)
    (output / (name + '.stderr.log')).write_text(result.stderr)
    if result.returncode:
        raise RuntimeError(name + ': ' + result.stdout[-4000:] + result.stderr[-8000:])
    return result

class Reader(OLD.Reader):
    pass

def semantic_scene(path):
    r = Reader(path.read_bytes())
    assert r.take(8) == b'RRSCENE1' and r.u() == 1
    world, identity, nn, nm, nl, total_vertices, total_surfaces = r.read('7I')
    assert world == 0 and identity == 1 and nn in (14, 18) and nl == 4
    r.take(56)
    nodes = []
    for _ in range(nn):
        name, parent = r.s(), r.u()
        local = r.read('16d'); r.take(23 * 8)
        nodes.append({'name': name, 'parent': parent, 'local': local})
    materials = [r.s() for _ in range(nm)]
    vertices, indices, surfaces = [], [], []
    counted_vertices = counted_surfaces = 0
    for lod in range(nl):
        r.take(8); ns = r.u(); counted_surfaces += ns
        for surface in range(ns):
            material, colors = r.read('2I'); r.take(8); nv, ni = r.read('2I')
            assert material < len(materials) and colors in (0, 1)
            first_vertex = len(vertices); first_index = len(indices)
            for _ in range(nv):
                xyz = r.read('3d'); r.take(24); uv = r.read('2d')
                rgba = r.take(4); bone = r.read('H')[0]; assert bone < nn
                if lod == 0:
                    vertices.append((xyz, bone, struct.pack('<2f', *uv) + (rgba if colors else b'\xff' * 4)))
            local_indices = r.read(str(ni) + 'I'); assert all(i < nv for i in local_indices)
            if lod == 0:
                indices.extend(first_vertex + i for i in local_indices)
                surfaces.append({'surface': surface, 'material_index': material,
                    'material': materials[material], 'has_original_colors': bool(colors),
                    'vertex_start': first_vertex, 'vertex_count': nv,
                    'index_start': first_index, 'index_count': ni})
            counted_vertices += nv
    assert (counted_vertices, counted_surfaces) == (total_vertices, total_surfaces)
    ns = r.u(); assert ns == len(surfaces)
    for surface in surfaces:
        assert r.read('6I') == (surface['material_index'], surface['vertex_start'],
            surface['vertex_count'], surface['index_start'], surface['index_count'],
            int(surface['has_original_colors']))
    neutral = r.take(r.u()); assert r.p == len(r.b)
    assert neutral[:8] == b'RRMESH01'
    return {'vertices': vertices, 'indices': indices, 'surfaces': surfaces, 'nodes': nodes}

def fnv(data):
    value = 1469598103934665603
    for byte in data:
        value = ((value ^ byte) * 1099511628211) & 0xffffffffffffffff
    return f'{value:016x}'

def changed(a, b):
    return {name: sum(a[16 + i * 24:28 + i * 24] != b[16 + i * 24:28 + i * 24]
                     for i in range(lo, hi)) for name, lo, hi in COMPONENTS}

def evidence_hashes(path):
    names = list(CORE_FILES) + sorted(p.name for p in path.glob('*.rrmesh'))
    return {name: sha(path / name) for name in names}

def validate(output, no_support=False):
    report = json.loads((output / 'export-report.native.json').read_text())
    trace = json.loads((output / 'trace.json').read_text())
    assert report['passed'] and trace['summary']['passed']
    assert report['summary'] == trace['summary']
    frame_count = report['summary']['frames']
    assert frame_count > 0 and len(trace['frames']) == frame_count
    base = BASE.read_bytes(); assert len(base) == 180988
    assert base[:16] == b'RRMESH01' + struct.pack('<II', 5755, 10713)
    scenes = [semantic_scene(p) for p in SCENES]
    assert [len(s['vertices']) for s in scenes] == [2884, 1411, 1460]
    assert [len(s['indices']) for s in scenes] == [4998, 2835, 2880]
    ordered_indices = []
    for scene, (_, lo, hi) in zip(scenes, COMPONENTS):
        for i, (_, _, attrs) in enumerate(scene['vertices']):
            assert attrs == base[16 + (lo + i) * 24 + 12:16 + (lo + i) * 24 + 24]
        ordered_indices.extend(lo + index for index in scene['indices'])
    assert struct.pack('<10713I', *ordered_indices) == base[16 + 5755 * 24:]
    material_bindings = json.loads((ROOT / 'evidence/visual49H/surface-bindings-final.json').read_text())
    spans = [s for s in material_bindings['spans'] if s['mesh_id'] == 'POD01']
    assert len(spans) == sum(len(s['surfaces']) for s in scenes)
    cursor = 0; index_base = 0
    for side, scene in enumerate(scenes):
        vertex_base = COMPONENTS[side][1]
        for surface in scene['surfaces']:
            span = spans[cursor]; cursor += 1
            for key in ('material_index', 'has_original_colors', 'vertex_count', 'index_count', 'surface_index'):
                assert span[key] == surface['surface' if key == 'surface_index' else key]
            assert span['scene_key'] == OLD.KEYS[side]
            assert span['vertex_start'] == vertex_base + surface['vertex_start']
            assert span['index_start'] == index_base + surface['index_start']
        index_base += len(scene['indices'])
    def reconstruct(palettes):
        expected = bytearray(base)
        assert [p['entry_count'] for p in palettes] == [18, 14, 14]
        for side, scene in enumerate(scenes):
            matrices = [[OLD.f32(w) for w in m] for m in palettes[side]['node_world_bits']]
            assert len(matrices) == palettes[side]['entry_count']
            for i, (xyz, bone, _) in enumerate(scene['vertices']):
                at = 16 + (COMPONENTS[side][1] + i) * 24
                expected[at:at + 12] = OLD.position(xyz, matrices[bone])
        return bytes(expected)
    golden_lines = (output / 'exact-goldens49w.txt').read_text().splitlines()
    assert len(golden_lines) == frame_count
    reconstructed = {}
    position_frames = orientation_frames = 0
    previous = {k: v for k, v in trace['initial'].items() if k in trace['frames'][0]['before']}
    elapsed = 0.0; substeps = 0; physical_substeps = 0
    for i, row in enumerate(trace['frames']):
        assert row['frame'] == i and row['before'] == previous, 'canonical frame persistence'
        before, graphics, after = row['before'], row['after_graphics_before_physics'], row['after']
        assert all(len(s['graph']) == 53 for s in (before, graphics, after))
        assert all(s['shape']['type'] == 0 and s['shape']['registration'] == 0 for s in (before, graphics, after))
        assert after['shape']['current'] == after['graph'][50]['world_bits']
        assert after['phase'] == before['phase'] == 0
        assert after['main']['root_words'] != before['main']['root_words']
        for root in (0, 15, 30):
            assert graphics['graph'][root]['local_bits'] == after['graph'][root]['local_bits']
        position_frames += before['graph'][50]['world_bits'][12:15] != after['graph'][50]['world_bits'][12:15]
        orientation_frames += before['main']['orientation1F0'] != after['main']['orientation1F0']
        support = row['supplied354_support_forces']; substeps += len(support)
        assert len(support) == after['substeps']
        for j, supplied in enumerate(support):
            assert supplied['frame'] == i and supplied['substep'] == j
            assert OLD.f32(supplied['main_support_y_bits']) == 0 if no_support else OLD.f32(supplied['main_support_y_bits']) > 0
            if supplied['physical838']:
                assert OLD.f32(supplied['secondary_support_y_bits']) == 0 if no_support else OLD.f32(supplied['secondary_support_y_bits']) > 0
                physical_substeps += 1
            else:
                assert supplied['secondary_support_y_bits'] == 0
        elapsed += OLD.f32(after['frame_factor'])
        assert elapsed == row['elapsed_seconds']
        expected_packet = [128, 80, 128, 48] if i < 32 else [136, 128, 120, 128] if i < 48 else [128, 255, 128, 255]
        assert row['packet_bytes'] == expected_packet
        mesh = reconstruct(row['palettes']); assert row['mesh_fnv64'] == fnv(mesh)
        graph_words = []
        for node in after['graph']:
            graph_words.extend(node['local_bits'] + node['world_bits'])
            graph_words.extend(node[k] & 0xffffffff for k in ('flags190', 'parent180', 'first_child184', 'next_sibling188'))
        noise = after['visual_shared']
        noise_words = [int(noise['noise_initialized']), noise['random_state'], noise['noise_scratch_cell'] & 0xffffffff]
        for perm, gradient in zip(noise['permutation'], noise['gradient_bits']):
            noise_words.extend((perm, gradient))
        graph_hash = int(fnv(struct.pack('<%dI' % len(graph_words), *graph_words)), 16)
        noise_hash = int(fnv(struct.pack('<%dI' % len(noise_words), *noise_words)), 16)
        assert golden_lines[i] == f'{i} {row["mesh_fnv64"]} {graph_hash} {noise_hash}'
        reconstructed[i] = mesh; previous = after
    summary = trace['summary']
    assert summary['native_vertices_all_frames'] == frame_count * 5755
    assert summary['substeps'] == substeps
    assert summary['position_changed_frames'] == position_frames == frame_count
    assert summary['orientation_state_changed_frames'] == orientation_frames
    assert summary['native_hot_allocations'] == summary['native_postphysics_graphics_pose_copies'] == 0
    for key in ('input_original_comparisons', 'graphics_original_comparisons'):
        assert summary[key] == frame_count
    assert summary['body_cache_original_comparisons'] == frame_count * 2
    for key in ('tail_original_comparisons', 'main_force_original_comparisons', 'shape_original_comparisons'):
        assert summary[key] == substeps, key
    assert summary['secondary_original_comparisons'] == physical_substeps
    assert summary['body_original_comparisons'] == substeps + physical_substeps
    controls = {k: 0 for k in ('whole_model_transform', 'palette_swap', 'double_root',
        'bind_baked_input', 'float32_semantic_input', 'stale_frame0_pose')}
    exports = []
    for entry in report['frames']:
        frame, label = entry['frame'], entry['label']
        assert label in ('full-motion', 'physical-copy-only', 'frozen-roots')
        actual = (output / entry['mesh_path']).read_bytes()
        expected = reconstruct(entry['palettes'])
        assert actual == expected and entry['mesh_fnv64'] == fnv(actual)
        assert entry['changed_vertices_vs49H'] == changed(base, actual)
        assert entry['changed_vertices_vs_full'] == changed(reconstructed[frame], actual)
        assert entry['vertices'] == 5755 and entry['indices'] == 10713
        if label == 'full-motion':
            assert actual == reconstructed[frame]
            for side, scene in enumerate(scenes):
                p = entry['palettes'][side]
                matrix = [[OLD.f32(w) for w in m] for m in p['node_world_bits']]
                root = [OLD.f32(w) for w in p['root_world_bits']]
                for i, (xyz, bone, _) in enumerate(scene['vertices']):
                    at = 16 + (COMPONENTS[side][1] + i) * 24
                    projected = actual[at:at + 12]
                    controls['whole_model_transform'] += OLD.position(xyz, root) != projected
                    controls['palette_swap'] += OLD.position(xyz, matrix[(bone + 1) % len(matrix)]) != projected
                    controls['double_root'] += OLD.position(struct.unpack('<3f', projected), root) != projected
                    controls['bind_baked_input'] += OLD.position(struct.unpack_from('<3f', base, at), matrix[bone]) != projected
                    controls['float32_semantic_input'] += OLD.position(struct.unpack('<3f', struct.pack('<3f', *xyz)), matrix[bone]) != projected
                    initial_matrix = [OLD.f32(w) for w in trace['frames'][0]['palettes'][side]['node_world_bits'][bone]]
                    controls['stale_frame0_pose'] += OLD.position(xyz, initial_matrix) != projected
        elif label == 'physical-copy-only':
            assert entry['changed_vertices_vs_full']['chariot'] > 0
            assert entry['changed_vertices_vs_full']['left_engine'] == 0
            assert entry['changed_vertices_vs_full']['right_engine'] == 0
        else:
            assert all(entry['changed_vertices_vs_full'].values())
        exports.append({**entry, 'mesh_sha256': sha(output / entry['mesh_path'])})
    assert len([e for e in exports if e['label'] == 'full-motion']) == 4
    assert {e['label'] for e in exports} == ({'full-motion'} if no_support else {'full-motion', 'physical-copy-only', 'frozen-roots'})
    assert all(controls.values()), controls
    return {'passed': True, 'schema': 'advancing49w.independent-verification.v1',
        'summary': summary, 'frames': exports, 'consumer_controls_changed_positions': controls,
        'all_frame_mesh_fnv64_match': True, 'frame_count': frame_count,
        'independent_reconstructed_positions': frame_count * 5755,
        'all_exported_mesh_bytes_match': True, 'all_exact_golden_mesh_graph_noise_hashes_match': True, 'semantic_uv_color_and_ordered_indices_match': True,
        'material_surface_order_match': True, 'semantic_scene_sha256': {str(p.relative_to(ROOT)): sha(p) for p in SCENES},
        'canonical_state_persistence': True, 'graphics_roots_preserved_during_body_advance': True,
        'elapsed_seconds': elapsed, 'physical_secondary_substeps': physical_substeps,
        'trace_sha256': sha(output / 'trace.json'), 'exact_goldens_sha256': sha(output / 'exact-goldens49w.txt'),
        'native_capture_sha256': evidence_hashes(output), 'scope': trace['initial']['scope'],
        'initialization': trace['initial']['authoring'], 'palette_observation': trace['initial']['palette_projection'],
        'limits': ('No added support forces; supplied no-hit query; natural unsupported fallthrough control. ' if no_support else 'Declared counter-gravity354 force inputs and no-hit query; ') + 'Ordinary initialized vehicle only. No recovered terrain/contact producer, spawn, full frame, performance/FPS or material-program claim.'}

def authenticate():
    source = FRAME.authenticate()
    body = module('retained_graph_body49w', 'tools/gameplay/graph_body_recover49w.py')
    body_manifest, body_words = body.authenticate()
    source['body_source_words'] = len(body_words)
    source['body_source_manifest'] = body_manifest
    sys.path.insert(0, str(ROOT / 'tools/gameplay'))
    shape = module('retained_pod_shape49w', 'tools/gameplay/pod_shape_recover49w.py')
    manifest, words, dispatch, _ = shape.authenticate()
    header = (E / 'shape_original49w.h').read_text()
    for name in shape.EXECUTED:
        array = re.search(r'\b' + name + r'\[\]=\{(.*?)\};', header, re.S)
        assert array, name
        raw = struct.pack('<%dI' % len(re.findall(r'0x([0-9a-f]+)u', array[1])),
            *[int(w, 16) for w in re.findall(r'0x([0-9a-f]+)u', array[1])])
        assert raw == (shape.EVIDENCE / 'source' / (name + '.bin')).read_bytes()
    body_machine = (ROOT / 'evidence/frameBridge49W/body/source/body_machine.h').read_text()
    copied = body_machine[body_machine.index('namespace graph49w_original {'):]
    copied = copied.replace('graph49w_original', 'shape49w_original').replace('graph49w_words', 'shape49w_words')
    assert copied in header, 'shape ISA decoder must equal retained body decoder'
    assert '#include "../../../work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h"' in header
    source['shape_source_words'] = len(words)
    source['shape_source_manifest'] = manifest
    source['shape_decoder_matches_retained_body_decoder'] = True
    source['shape_sqrt_reference_sha256'] = shape.SQRT_SHA
    source['source_call_substitutions'] = 0
    return source

def input_paths():
    paths = set(OLD.input_paths())
    paths.update({'evidence/frameBridge49W/shape/source-manifest.json', 'evidence/frameBridge49W/shape/podboxf.col',
        'evidence/frameBridge49W/shape/verification/fixtures.json',
        'evidence/frameBridge49W/body/source/source-manifest.json',
        'evidence/frameBridge49W/independent-review/source-authentication.json',
        'work/native/project/disc/SLUS_202.68', 'work/native/project/disc/PODS/POD01/POD01.RES'})
    for directory in ('evidence/frameBridge49W/shape/source', 'evidence/frameBridge49W/body/source',
        'evidence/authoredInitialization49N/authoring-source', 'evidence/authoredGeometry49O/initialization',
        'evidence/secondaryWrapper49M/numeric-source'):
        paths.update(str(p.relative_to(ROOT)) for p in (ROOT / directory).rglob('*')
                     if p.is_file() and p.suffix in ('.bin', '.json', '.txt', '.phy', '.col'))
    return paths

def source_paths(compiler):
    paths = set(SOURCES) | {'tools/visual/run_vehicle_frame49w.py',
        'tools/visual/run_chariot_lifecycle49v.py', 'tools/gameplay/vehicle_frame_recover49w.py',
        'tools/gameplay/pod_shape_recover49w.py', 'tools/gameplay/graph_body_recover49w.py',
        'tools/gameplay/group_shape_fixtures48t.py',
        'tools/gameplay/candidate_collection_fixtures48s.py', 'tools/gameplay/query_oracle48h.py',
        'tools/assets/decode_collision48g.py',
        'evidence/frameBridge49W/connected/shape-adapter-crosscheck.cpp'}
    for source in SOURCES:
        dep = subprocess.run([compiler, *OLD.FLAGS, '-MM', str(ROOT / source)], cwd=ROOT,
                             text=True, capture_output=True, check=True).stdout
        for item in dep.replace('\\\n', ' ').split()[1:]:
            path = Path(item); path = path if path.is_absolute() else ROOT / path
            if path.is_file(): paths.add(str(path.resolve().relative_to(ROOT)))
    for imported in tuple(sys.modules.values()):
        filename = getattr(imported, '__file__', None)
        if filename:
            path = Path(filename).resolve()
            if path.is_file() and path.is_relative_to(ROOT):
                paths.add(str(path.relative_to(ROOT)))
    return paths

def compare_scenarios(supported, unsupported):
    a = json.loads((supported / 'trace.json').read_text())
    b = json.loads((unsupported / 'trace.json').read_text())
    assert a['initial']['authoring'] == b['initial']['authoring']
    assert a['frames'][0]['before'] == b['frames'][0]['before']
    assert len(a['frames']) == len(b['frames'])
    differences = []
    for left, right in zip(a['frames'], b['frames']):
        for field in ('frame', 'packet_bytes', 'elapsed_seconds'):
            assert left[field] == right[field]
        assert left['after']['frame_factor'] == right['after']['frame_factor']
        differences.append({
            'frame': left['frame'],
            'supported_main_position': [OLD.f32(w) for w in left['after']['graph'][50]['world_bits'][12:15]],
            'unsupported_main_position': [OLD.f32(w) for w in right['after']['graph'][50]['world_bits'][12:15]],
            'mesh_differs': left['mesh_fnv64'] != right['mesh_fnv64']})
    assert any(x['mesh_differs'] for x in differences)
    return {'passed': True, 'same_initial_canonical_state': True,
        'same_authored_parameters_and_supplied_packets_timing': True,
        'difference': 'Only synthetic354 support forces are enabled/disabled; later trajectories follow persistent native results.',
        'contact_control_supported': a['initial']['contact_control'],
        'contact_control_unsupported': b['initial']['contact_control'],
        'frames': differences}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--verify-frozen', action='store_true')
    parser.add_argument('--freeze', action='store_true', help='Install newly validated normal captures; refuses existing frozen outputs.')
    parser.add_argument('--skip-sanitizers', action='store_true')
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--jobs', type=int, default=4)
    args = parser.parse_args()
    assert 1 <= args.jobs <= 4
    assert not args.freeze or args.verify_frozen, '--freeze requires --verify-frozen'
    if args.freeze:
        assert not any((E / name).exists() for name in CORE_FILES), 'frozen outputs already exist'
        assert not any((E / 'no-support' / name).exists() for name in CORE_FILES), 'frozen no-support outputs already exist'
    output = args.output_dir.resolve(); output.mkdir(parents=True, exist_ok=False)
    compiler = shutil.which(args.compiler); assert compiler
    authentication = authenticate(); dump(output / 'source-authentication.json', authentication)
    pins = {p: sha(ROOT / p) for p in sorted(input_paths())}
    sources = {p: sha(ROOT / p) for p in sorted(source_paths(compiler))}
    dump(output / 'input-sha256.json', pins); dump(output / 'source-sha256.json', sources)
    compiler_version = run([compiler, '--version'], output, 'compiler').stdout.splitlines()[0]
    env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',
               UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    env.pop('RR_VEHICLE_FRAME49W_COVERAGE', None)
    verification = {'passed': False, 'compiler': compiler_version, 'production_standard': 'C++17',
        'fixture_standard': 'C++20', 'input_sha256': pins, 'source_sha256': sources,
        'runs': {}, 'leak_sanitizer_used': False, 'frozen_read_only_requested': args.verify_frozen,
        'authentication': authentication, 'all_mode_capture_bytes_identical': False}
    with tempfile.TemporaryDirectory(prefix='rr_vehicle49w_') as directory:
        build = Path(directory)
        shape_binary = build / 'shape-adapter-crosscheck'
        run([compiler, '-std=c++20', '-O2', '-ffp-contract=off', '-fno-fast-math',
             '-Wall', '-Wextra', '-Werror', '-Wno-misleading-indentation',
             E / 'shape-adapter-crosscheck.cpp', '-o', shape_binary], output, 'shape-adapter-compile')
        shape_result = run([shape_binary, ROOT / 'evidence/frameBridge49W/shape/verification/fixtures.json'],
                           output, 'shape-adapter-crosscheck').stdout.strip()
        assert shape_result == 'PASS all original shape cases 364'
        verification['shape_adapter_crosscheck'] = {'passed': True, 'cases': 364,
            'reference': 'Retained original-word fixtures, including exponent-zero SQRT boundaries.'}
        for mode in ['normal', 'plain'] + ([] if args.skip_sanitizers else ['asan-ubsan']):
            optimize = ['-O2'] if mode != 'asan-ubsan' else ['-O1', '-g', '-fno-omit-frame-pointer',
                '-fno-pie', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
            common = OLD.FLAGS[1:11] if mode == 'plain' else OLD.FLAGS[1:]
            objects = [build / (mode + '-' + str(i) + '.o') for i in range(len(SOURCES))]
            def compile_one(i):
                standard = '-std=c++20' if i == 0 else '-std=c++17'
                run([compiler, standard, *common, *optimize, '-c', ROOT / SOURCES[i], '-o', objects[i]],
                    output, mode + '-compile-' + str(i))
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
                list(pool.map(compile_one, range(len(SOURCES))))
            binary = build / mode
            run([compiler, *optimize, *(['-no-pie'] if mode == 'asan-ubsan' else []), *objects, '-lz', '-o', binary],
                output, mode + '-link')
            destination = output / mode; destination.mkdir()
            run([binary, destination], output, mode + '-capture', env)
            result = validate(destination); dump(destination / 'reconstruction.json', result)
            unsupported = destination / 'no-support'; unsupported.mkdir()
            run([binary, unsupported, '--no-support'], output, mode + '-no-support-capture', env)
            control_result = validate(unsupported, True)
            dump(unsupported / 'reconstruction.json', control_result)
            dump(destination / 'support-comparison.json', compare_scenarios(destination, unsupported))
            mode_result = {'capture': result['native_capture_sha256'], 'summary': result['summary'],
                'independent_reconstructed_positions': result['independent_reconstructed_positions'],
                'no_support_capture': control_result['native_capture_sha256'],
                'no_support_summary': control_result['summary'],
                'flags': common + optimize, 'link_libraries': ['z'], 'read_only_native_cli': False}
            if args.freeze and mode == 'normal':
                (E / 'no-support').mkdir(exist_ok=True)
                for src, dst, data in ((destination, E, result), (unsupported, E / 'no-support', control_result)):
                    for name in data['native_capture_sha256']:
                        shutil.copyfile(src / name, dst / name)
                    dump(dst / 'reconstruction.json', data)
                    dump(dst / 'export-report.json', data)
                shutil.copyfile(destination / 'support-comparison.json', E / 'support-comparison.json')
                print('Validated normal support/no-support captures installed for cross-mode and read-only verification.', flush=True)
            if args.verify_frozen:
                frozen_before = evidence_hashes(E)
                unsupported_before = evidence_hashes(E / 'no-support')
                assert mode_result['capture'] == frozen_before, 'capture differs from frozen evidence'
                assert mode_result['no_support_capture'] == unsupported_before, 'unsupported capture differs'
                run([binary], output, mode + '-noargs-readonly', env)
                run([binary, '--verify-no-support'], output, mode + '-no-support-readonly', env)
                assert frozen_before == evidence_hashes(E), 'native noargs altered frozen evidence'
                assert unsupported_before == evidence_hashes(E / 'no-support'), 'native no-support altered frozen evidence'
                mode_result['read_only_native_cli'] = True
                mode_result['frozen_sha256_before_after'] = frozen_before
                mode_result['no_support_frozen_sha256_before_after'] = unsupported_before
            if verification['runs']:
                assert mode_result['capture'] == verification['runs']['normal']['capture'], mode + ' differs'
                assert mode_result['no_support_capture'] == verification['runs']['normal']['no_support_capture'], mode + ' no-support differs'
            verification['runs'][mode] = mode_result
            dump(output / 'verification.in-progress.json', verification)
            print(mode + ': ' + json.dumps(result['summary'], sort_keys=True), flush=True)
    assert pins == {p: sha(ROOT / p) for p in pins}, 'input changed during verification'
    assert sources == {p: sha(ROOT / p) for p in sources}, 'source changed during verification'
    verification['passed'] = verification['all_mode_capture_bytes_identical'] = True
    dump(output / 'verification.json', verification)
    if args.freeze:
        dump(E / 'freeze.json', {'passed': True, 'verification_path': str((output / 'verification.json').relative_to(ROOT)),
            'verification_sha256': sha(output / 'verification.json'), 'source_sha256': sources, 'input_sha256': pins,
            'support_native_sha256': evidence_hashes(E), 'no_support_native_sha256': evidence_hashes(E / 'no-support'),
            'normal_plain_sanitized_bytes_identical': not args.skip_sanitizers,
            'all_native_read_only_paths_verified': all(v['read_only_native_cli'] for v in verification['runs'].values())})
    (output / 'verification.in-progress.json').unlink()
    print('PASS advancing49W source/native persistence and all-frame semantic reconstruction', flush=True)

if __name__ == '__main__':
    main()
