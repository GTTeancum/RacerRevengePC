#!/usr/bin/env python3
"""Build and verify full21A000 ->21C120 ->21A660 ->all-three49S diagnostics."""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
SOURCES = [
    'tools/visual/export_chariot_root49u.cpp',
    'src/game/engine_animation49q.cpp', 'src/game/model_motion49q.cpp',
    'src/game/vehicle_animation49t.cpp', 'src/game/model_motion49t.cpp',
    'src/game/chariot_ik49t.cpp', 'src/game/source_acos49t.cpp',
    'src/game/scene_graph49p.cpp', 'src/assets/model_geometry49o.cpp',
    'src/assets/model_bindings49o.cpp', 'src/assets/pod_physics49n.cpp',
    'src/game/control_binding48w.cpp', 'src/game/control_shaping48v.cpp',
    'src/game/control_filter48b.cpp', 'src/game/vehicle_visual49r.cpp',
    'src/game/visual_container49r.cpp', 'src/game/visual_response49r.cpp',
    'src/assets/vehicle_visual_authoring49r.cpp', 'src/game/gradient_noise49m.cpp',
    'src/game/body_scene48g.cpp', 'src/game/frame_timing49i.cpp',
    'src/game/model_palette49s.cpp', 'src/game/rigid_vertices49s.cpp',
    'src/assets/scene_asset48d.cpp', 'src/assets/native_asset48b.cpp',
    'src/game/chariot_root49u.cpp', 'src/game/chariot_scene49u.cpp',
    'src/assets/chariot_root_authoring49u.cpp', 'src/game/pod_model_geometry49o.cpp',
    'src/game/model_motion49o.cpp', 'src/game/model_bounds49o.cpp',
    'src/game/authored_geometry49o.cpp', 'src/game/pod_sphere_constraint49l.cpp',
]
FLAGS = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
         '-Wno-misleading-indentation', '-Wno-unused-function',
         '-fno-fast-math', '-frounding-math', '-ffp-contract=off',
         '-I' + str(ROOT / 'src/game'), '-DRR_VISUAL49R_ENABLE_STAGE_TRACE=1',
         '-include', str(ROOT / 'evidence/vehicleVisual49R/source/stage_trace.h'),
         '-include', str(ROOT / 'evidence/chariotAnimation49T/flow/stage_trace.h'),
         '-DRR_CHARIOT_ROOT49U_ENABLE_STAGE_TRACE=1', '-include',
         str(ROOT / 'evidence/chariotRoot49U/root/source/root_stage_trace.h')]
BASE = 'evidence/visual49G/export-placed-normal/pod01-placed.rrmesh'
KEYS = ['cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5',
        '210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3',
        'f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60']
SCENES = ['evidence/visual49G/cacheobjects/' + k + '.rrscene' for k in KEYS]

def sha(path): return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def dump(path, value): path.write_text(json.dumps(value, indent=2, sort_keys=True) + '\n')
def run(command, output, label, env=None):
    p = subprocess.run([str(x) for x in command], cwd=ROOT, text=True,
                       capture_output=True, env=env)
    (output / (label + '.stdout.log')).write_text(p.stdout)
    (output / (label + '.stderr.log')).write_text(p.stderr)
    if p.returncode: raise RuntimeError(label + ': ' + p.stdout + p.stderr)
    return p

class Reader:
    def __init__(self, b): self.b, self.p = b, 0
    def take(self, n):
        r = self.b[self.p:self.p+n]; assert len(r) == n
        self.p += n; return r
    def read(self, f): return struct.unpack('<' + f, self.take(struct.calcsize('<' + f)))
    def u(self): return self.read('I')[0]
    def s(self): return self.take(self.u()).decode()

def local_vertices(path):
    r = Reader(path.read_bytes()); assert r.take(8) == b'RRSCENE1'; assert r.u() == 1
    world, palette, nodes, materials, lods, _, _ = r.read('7I')
    assert world == 0 and palette == 1 and nodes in (14, 18) and lods == 4
    r.take(56)
    for _ in range(nodes): r.s(); r.take(4 + 39 * 8)
    for _ in range(materials): r.s()
    out = []
    for li in range(lods):
        r.take(8); ns = r.u()
        for _ in range(ns):
            r.read('2I'); r.take(8); nv, ni = r.read('2I')
            for _ in range(nv):
                xyz = r.read('3d'); r.take(3 * 8 + 2 * 8 + 4); bone = r.read('H')[0]
                if li == 0: out.append((xyz, bone))
            r.take(ni * 4)
    return out

def f32(word): return struct.unpack('<f', struct.pack('<I', word))[0]
def position(xyz, matrix):
    result = []
    for axis in range(3):
        total = 0.0
        for k in range(3): total = total + xyz[k] * matrix[k * 4 + axis]
        result.append(total + matrix[12 + axis])
    return struct.pack('<3f', *result)

def validate(output):
    report = json.loads((output / 'export-report.native.json').read_text())
    trace = json.loads((output / 'trace.json').read_text())
    assert report['passed'] and trace['summary']['passed']
    assert len(trace['frames']) == 96
    base = (ROOT / BASE).read_bytes(); assert base[:8] == b'RRMESH01'
    assert struct.unpack_from('<II', base, 8) == (5755, 10713)
    locals_ = [local_vertices(ROOT / p) for p in SCENES]
    assert [len(v) for v in locals_] == [2884, 1411, 1460]
    def fnv(data):
        h = 1469598103934665603
        for byte in data: h = ((h ^ byte) * 1099511628211) & 0xffffffffffffffff
        return '%016x' % h
    # Independently rebuild every position for all96 frames from the semantic
    # binary64 locals and current binary32 palette, then verify native mesh bits.
    for row in trace['frames']:
        expected = bytearray(base)
        for side, name in enumerate(('chariot_palette', 'left_palette', 'right_palette')):
            matrices = [[f32(w) for w in m] for m in row[name]['node_world_bits']]
            for vi, (xyz, bone) in enumerate(locals_[side]):
                at = 16 + ((0, 2884, 4295)[side] + vi) * 24
                expected[at:at+12] = position(xyz, matrices[bone])
        assert row['vertices'] == 5755 and row['mesh_fnv64'] == fnv(expected)
    report['independent_all96_frame_vertex_count'] = 96 * 5755
    report['independent_all96_frame_mesh_digests_match'] = True
    control_rows = {x['label']: x for x in trace['display_controls']}
    controls = {'wrong_whole_model_transform': 0, 'wrong_palette_swap': 0,
                'wrong_double_root': 0, 'wrong_bind_baked_input': 0,
                'wrong_float32_semantic_input': 0, 'wrong_stale_frame0_pose': 0}
    for export in report['frames']:
        actual = (output / export['mesh_path']).read_bytes(); expected = bytearray(base)
        assert len(actual) == len(base)
        assert actual[:16] == base[:16]
        assert actual[16 + 5755 * 24:] == base[16 + 5755 * 24:]
        row = trace['frames'][export['frame']] if export['label'] == 'full-motion' else control_rows[export['label']]
        for side, name in enumerate(('chariot_palette', 'left_palette', 'right_palette')):
            pal = row[name]; assert pal['entry_count'] == (18 if side == 0 else 14)
            matrices = [[f32(w) for w in m] for m in pal['node_world_bits']]
            root = [f32(w) for w in pal['root_world_bits']]
            for vi, (xyz, bone) in enumerate(locals_[side]):
                vertex = (0, 2884, 4295)[side] + vi; at = 16 + vertex * 24
                projected = position(xyz, matrices[bone]); expected[at:at+12] = projected
                assert actual[at+12:at+24] == base[at+12:at+24]
                if export['label'] == 'full-motion':
                    controls['wrong_whole_model_transform'] += position(xyz, root) != projected
                    controls['wrong_palette_swap'] += position(xyz, matrices[(bone+1) % pal['entry_count']]) != projected
                    controls['wrong_double_root'] += position(struct.unpack('<3f', projected), root) != projected
                    controls['wrong_bind_baked_input'] += position(struct.unpack_from('<3f', base, at), matrices[bone]) != projected
                    rounded = struct.unpack('<3f', struct.pack('<3f', *xyz))
                    controls['wrong_float32_semantic_input'] += position(rounded, matrices[bone]) != projected
                    initial_matrix = [f32(w) for w in trace['frames'][0][name]['node_world_bits'][bone]]
                    controls['wrong_stale_frame0_pose'] += position(xyz, initial_matrix) != projected
        assert actual == expected, 'independent semantic/local palette deformation differs'
        export['mesh_sha256'] = sha(output / export['mesh_path'])
    assert all(value for key, value in controls.items() if key != 'wrong_float32_semantic_input'), controls
    report['consumer_control_limits'] = 'The float32-first local-coordinate counterfactual is byte-insensitive in this high-speed/large-anchor capture; zero differences are reported without a detection claim.'
    def changed(a, b):
        return {name: sum(a[16+i*24:28+i*24] != b[16+i*24:28+i*24]
                         for i in range(lo, hi))
                for name, lo, hi in [('chariot',0,2884),('left_engine',2884,4295),('right_engine',4295,5755)]}
    full = {e['frame']: (output / e['mesh_path']).read_bytes()
            for e in report['frames'] if e['label'] == 'full-motion'}
    for e in report['frames']:
        mesh = (output / e['mesh_path']).read_bytes()
        assert e['changed_vertices_vs49H'] == changed(base, mesh)
        assert e['changed_vertices_vs_full'] == changed(full[e['frame']], mesh)
        if e['label'] != 'full-motion':
            assert e['changed_vertices_vs_full'] == control_rows[e['label']]['changed_vertices_vs_full']
            assert e['changed_vertices_vs_full']['chariot'] > 0
            assert e['changed_vertices_vs_full']['left_engine'] == 0
            assert e['changed_vertices_vs_full']['right_engine'] == 0
    report['engine_comparison_scope'] = 'Root-only display controls preserve full frame95 engine positions; no49T/49S byteidentity claim after cached speed changes from0 to200.'
    # All96 frames have full graph and palette words, not just renderedframes.
    for row in trace['frames']:
        assert len(row['graph']) == len(row['pre_ik_graph']) == 53
        assert [row[k]['entry_count'] for k in ('chariot_palette','left_palette','right_palette')] == [18,14,14]
        assert row['application_factor_bits'] == row['animation_input']['animation_delta23C_bits'] == row['visual_input_bits']['frameFactor'] == row['root_input_bits']['frameFactor_bits']
        assert row['control_cached_speed34_bits'] == row['visual_input_bits']['cachedBodySpeed34'] == row['root_input_bits']['cachedBodySpeed34_bits'] == 1128792064
        assert row['root_state']['vehicle838'] == 0 and row['root_state']['vehicle83C'] != 0
        assert row['root_input_bits']['vehicle58'] == row['root_input_bits']['vehicle40'] == 0
        assert row['shared']['permutation'] == row['shared_after49R']['permutation']
        assert row['shared']['gradient_bits'] == row['shared_after49R']['gradient_bits']
        if row['frame']:
            assert row['root_state_before']['vehicle838'] == 0 and row['root_state_before']['vehicle83C'] != 0
            assert row['phase864_before_bits'] == trace['frames'][row['frame']-1]['phase864_bits']
            assert f32(row['phase864_bits']) > f32(row['phase864_before_bits'])
        else:
            assert row['root_state_before']['vehicle838'] != 0 and row['root_state_before']['vehicle83C'] == 0
            assert row['phase864_bits'] == row['phase864_before_bits'] == 0
    assert trace['summary']['root21A660_calls'] == 96
    assert trace['summary']['rigid_calls'] == 294
    assert trace['summary']['native_vertices_all96_frames'] == 96 * 5755
    assert trace['summary']['source49U_noise_calls'] == 95 * 6
    assert trace['summary']['physical_entry_calls'] == 1
    assert trace['summary']['persistent_virtual_calls'] == 95
    assert trace['initial']['graph'][52]['flags190'] == 0x80
    assert trace['initial']['graph'][52]['world_bits'] == [0] * 16
    assert {x['label'] for x in report['frames']} == {'full-motion','frozen-chariot-root','physical-copy-only'}
    report['consumer_controls'] = controls
    report['trace_sha256'] = sha(output / 'trace.json')
    report['exact_goldens_sha256'] = sha(output / 'exact-goldens49u.txt')
    return report

def sensitivity(compiler, output, temporary, objects, env, jobs):
    """Kill altered original words and production ordering using the same live scene."""
    trials = [
        ('source_omit_phase864_store', 0, None, ['-DRR_CHARIOT_ROOT49U_SOURCE_MUTATION_PC=0x21a93cu', '-DRR_CHARIOT_ROOT49U_SOURCE_MUTATION_WORD=0u'], 'mismatch'),
        ('source_subtract_noise_term', 0, None, ['-DRR_CHARIOT_ROOT49U_SOURCE_MUTATION_PC=0x21b7d8u', '-DRR_CHARIOT_ROOT49U_SOURCE_MUTATION_WORD=0x46020841u'], 'mismatch'),
        ('native_omit_root_call', 0, None, ['-DRR_CHARIOT_ROOT49U_NATIVE_OMIT=1'], 'mismatch'),
        ('native_root_uses_pre49R_graph', 0, None, ['-DRR_CHARIOT_ROOT49U_NATIVE_REORDER=1'], 'mismatch'),
        ('native_reset_shared_noise', 0, None, ['-DRR_CHARIOT_ROOT49U_NATIVE_RESET_NOISE=1'], 'mismatch'),
    ]
    def trial(spec):
        name, index, source, extra, expected = spec
        path = ROOT / SOURCES[index]
        if source is not None:
            path = temporary / (name+'.cpp'); path.write_text(source)
        obj = temporary / (name+'.o')
        run([compiler, *FLAGS, '-O2', *extra, '-c', path, '-o', obj], output, name+'-compile')
        altered = list(objects); altered[index] = obj
        binary = temporary / name
        run([compiler, '-O2', *altered, '-o', binary], output, name+'-link')
        result = subprocess.run([str(binary), str(temporary / (name+'-capture'))], cwd=ROOT, text=True, capture_output=True, env=env)
        (output/(name+'.stdout.log')).write_text(result.stdout)
        (output/(name+'.stderr.log')).write_text(result.stderr)
        assert result.returncode != 0 and expected in result.stderr, (name,result.returncode,result.stderr)
        return name, {'killed': True, 'returncode': result.returncode, 'diagnostic': result.stderr.strip(), 'mutation_target': SOURCES[index], 'compile_defines': extra}
    with concurrent.futures.ThreadPoolExecutor(max_workers=min(jobs, 2)) as pool:
        result = dict(pool.map(trial, trials))
    dump(output / 'connected-sensitivity.json', {'passed': True, 'mutations': result})
    return result

def input_paths():
    paths = {BASE, 'evidence/visual49H/surface-bindings-final.json', *SCENES,
             'evidence/engineAnimation49Q/connected/supplied-controls49q.bin',
             'evidence/authoredInitialization49N/authoring-source/Pod01.phy',
             'evidence/authoredGeometry49O/motion/fixture.bin'}
    for directory in ('evidence/engineAnimation49Q/flow/source',
                      'evidence/chariotAnimation49T/flow/source',
                      'evidence/chariotAnimation49T/ik/source',
                      'evidence/authoredGeometry49O/asset-reader/fixtures',
                      'evidence/authoredGeometry49O/motion/trig',
                      'evidence/vehicleVisual49R/source',
                      'evidence/chariotRoot49U/root/source',
                      'evidence/chariotRoot49U/authoring'):
        paths.update(str(x.relative_to(ROOT)) for x in (ROOT / directory).iterdir()
                     if x.suffix in ('.bin', '.psg', '.vut', '.mot'))
    return paths

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output-dir', type=Path, required=True)
    p.add_argument('--skip-sanitizers', action='store_true')
    p.add_argument('--compiler', default='g++')
    p.add_argument('--jobs', type=int, default=4)
    args = p.parse_args(); output = args.output_dir.resolve(); output.mkdir(parents=True, exist_ok=False)
    compiler = shutil.which(args.compiler); assert compiler
    source_paths = set(SOURCES) | {'tools/visual/run_chariot_root49u.py'}
    for source in SOURCES:
        dep = subprocess.run([compiler, *FLAGS, '-MM', str(ROOT / source)], cwd=ROOT,
                             text=True, capture_output=True, check=True).stdout
        for item in dep.replace('\\\n', ' ').split()[1:]:
            f = Path(item); f = f if f.is_absolute() else ROOT / f
            if f.is_file(): source_paths.add(str(f.resolve().relative_to(ROOT)))
    pins = {path: sha(ROOT / path) for path in sorted(input_paths())}
    source_pins = {path: sha(ROOT / path) for path in sorted(source_paths)}
    env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',
               UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    verification = {'passed': False, 'input_sha256': pins, 'source_sha256': source_pins,
                    'leak_sanitizer_used': False, 'runs': {}}
    with tempfile.TemporaryDirectory(prefix='rr_chariot49u_') as temporary:
        temporary = Path(temporary)
        for mode in ['normal', 'plain'] + ([] if args.skip_sanitizers else ['asan-ubsan']):
            flags = ['-O2'] if mode in ('normal', 'plain') else ['-O1', '-g', '-fno-omit-frame-pointer',
                    '-fno-pie', '-no-pie', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
            mode_flags = FLAGS[:11] if mode == 'plain' else FLAGS
            objects = [temporary / (mode + '-' + str(i) + '.o') for i in range(len(SOURCES))]
            def compile_one(index):
                run([compiler, *mode_flags, *flags, '-c', ROOT / SOURCES[index], '-o', objects[index]],
                    output, mode + '-compile-' + str(index))
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
                list(pool.map(compile_one, range(len(SOURCES))))
            binary = temporary / mode
            run([compiler, *flags, *objects, '-o', binary], output, mode + '-link')
            destination = output if mode in ('normal', 'plain') else output / mode
            if mode == 'plain':
                frozen = ROOT / 'evidence/chariotRoot49U/connected'
                frozen_paths = ['trace.json', 'exact-goldens49u.txt', 'export-report.native.json'] + sorted(str(x.relative_to(frozen)) for x in (frozen / 'meshes').glob('*.rrmesh'))
                before = {name: sha(frozen / name) for name in frozen_paths}
                run([binary], output, mode + '-run', env)
                assert before == {name: sha(frozen / name) for name in before}
            else:
                run([binary, destination], output, mode + '-run', env)
            result = validate(destination)
            result['input_sha256'], result['source_sha256'] = pins, source_pins
            dump(destination / 'export-report.json', result)
            verification['runs'][mode] = {'summary': result['summary'], 'flags': mode_flags + flags,
                'read_only_native_cli': mode == 'plain',
                'trace_sha256': result['trace_sha256'], 'exact_goldens_sha256': result['exact_goldens_sha256'],
                'mesh_sha256': {r['mesh_path']: r['mesh_sha256'] for r in result['frames']},
                'consumer_controls': result['consumer_controls'],
                'independent_all96_frame_vertex_count': result['independent_all96_frame_vertex_count'],
                'independent_all96_frame_mesh_digests_match': True}
            if mode != 'normal':
                assert verification['runs']['normal']['trace_sha256'] == result['trace_sha256']
                assert verification['runs']['normal']['exact_goldens_sha256'] == result['exact_goldens_sha256']
                assert verification['runs']['normal']['mesh_sha256'] == verification['runs'][mode]['mesh_sha256']
            if mode == 'normal':
                verification['sensitivity'] = sensitivity(compiler, output, temporary, objects, env, args.jobs)
            print(mode + ': ' + json.dumps(result['summary'], sort_keys=True), flush=True)
    assert pins == {path: sha(ROOT / path) for path in pins}
    assert source_pins == {path: sha(ROOT / path) for path in source_pins}
    verification['passed'] = True
    dump(output / 'verification.json', verification)
    print('PASS chariot49U connected export and independent vertex controls', flush=True)

if __name__ == '__main__': main()
