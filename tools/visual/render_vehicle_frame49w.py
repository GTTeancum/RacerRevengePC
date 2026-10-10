#!/usr/bin/env python3
"""Render verified 49W advancing native meshes in the preserved 49H inspection view.

The native exporter supplies posed positions. This consumer checks their verified
capture pins, mesh invariants and explicit fixture identity before any GL draw.
It reuses the unchanged unlit first-pass diagnostic, not original VU shading.
"""
from __future__ import annotations
import argparse
import copy
import json
import math
import os
from pathlib import Path
import resource
import subprocess
import sys
import tempfile
import time
import render_textured49h as old

sys.dont_write_bytecode = True
base, demand = old.base, old.demand
ROOT = Path(__file__).resolve().parents[2]
EVIDENCE = ROOT / 'evidence/frameBridge49W'
RECIPE = ROOT / 'evidence/visual49H/frames/recipe-textured-final.json'
LABEL = 'Native advancing vehicle | staged diagnostic'
CAPTION_HEIGHT = 88
MAX_REPORT_BYTES = 2 * 1024 * 1024
MAX_PIN_BYTES = 64 * 1024 * 1024
FULL_ROWS = {(i, 'full-motion') for i in (0, 31, 95, 143)}
EXPECTED_ROWS = FULL_ROWS | {(31, 'frozen-roots'), (31, 'physical-copy-only')}
PINNED = {
    '49H renderer': (Path(old.__file__), '4ee39e6e7e9786f221e352299f14286a9bfcdf9143f85162951706acc29a3598'),
    '49G base': (Path(base.__file__), '4bd13edc8c741f40f68ec965f9559dfe3d13a3163d6bb43200ce91ca4204f1cc'),
    '49H recipe': (RECIPE, '7bf0a0ff562b88d26099da4411349640d7f748d48875ff39af7fef0101b7daa3'),
    '49H bindings': (ROOT / 'evidence/visual49H/surface-bindings-final.json',
                     'f035fd96815ab2b84fd342afdf999fd3167997eaff13c0dff664247ef9420549'),
}


def digest(value):
    demand(isinstance(value, str) and len(value) == 64 and
           all(c in '0123456789abcdef' for c in value), 'Expected lowercase SHA256')
    return value


def relative_file(directory, text):
    demand(isinstance(text, str) and text and not Path(text).is_absolute()
           and '..' not in Path(text).parts and '\\' not in text, 'Expected bounded relative file path')
    path = directory / text
    demand(not path.is_symlink(), 'Symlink input is outside this fixture')
    resolved = path.resolve(strict=True)
    demand(resolved.is_relative_to(directory.resolve()), 'Input escaped its fixture directory')
    demand(resolved.is_file(), 'Input is not a regular file')
    return resolved


def report_rows(report):
    demand(isinstance(report, dict) and report.get('schema') == 'advancing49w.export.v1'
           and report.get('passed') is True, 'Passing native advancing49w export required')
    demand(report.get('scenario') in ('synthetic-support','no-support'), 'Unknown native scenario')
    rows = report.get('frames')
    demand(isinstance(rows, list) and len(rows) == (6 if report.get('scenario') == 'synthetic-support' else 4), 'Expected four native frames and two controls')
    keys = []
    for row in rows:
        demand(isinstance(row, dict), 'Frame row must be an object')
        frame = old.integer(row.get('frame'), 'native frame', 239)
        label = row.get('label')
        demand(label in ('full-motion', 'frozen-roots', 'physical-copy-only'), 'Unknown frame profile')
        keys.append((frame, label))
        elapsed = row.get('elapsed_seconds')
        demand(type(elapsed) in (int, float) and math.isfinite(elapsed)
               and 0 <= elapsed <= 10, 'Bounded supplied elapsed time required')
        demand((row.get('vertices'), row.get('indices')) == (5755, 10713),
               'Native mesh counts differ from selected Pod01')
        expected = 'source-ordered-native-graphics-and-body-frame' if label == 'full-motion' else 'counterfactual-display-setters'
        demand(row.get('pose_source') == expected, 'Pose-source label is inconsistent')
        for field in ('changed_vertices_vs49H','changed_vertices_vs_full'):
            values=row.get(field)
            demand(isinstance(values,dict) and set(values)=={'chariot','left_engine','right_engine'}, 'Exact component count keys required')
            for component,maximum in (('chariot',2884),('left_engine',1411),('right_engine',1460)):
                old.integer(values[component], component+' change count', maximum)
        demand(row.get('component_vertices')=={'chariot':2884,'left_engine':1411,'right_engine':1460}, 'Component vertex counts differ')
        counts=row['changed_vertices_vs_full']
        expected_changes = ({'chariot':0,'left_engine':0,'right_engine':0} if label=='full-motion' else
                            {'chariot':2884,'left_engine':0,'right_engine':0} if label=='physical-copy-only' else
                            {'chariot':2884,'left_engine':1411,'right_engine':1460})
        demand(counts == expected_changes, 'Profile-specific full/control component isolation differs')
        digest(row.get('mesh_sha256'))
    demand(len(set(keys)) == len(keys) and set(keys) == (EXPECTED_ROWS if report.get('scenario') == 'synthetic-support' else FULL_ROWS), 'Frame/profile set differs')
    return rows


def verify_mesh(original, posed, np):
    a, b = original['vertex_data'], posed['vertex_data']
    demand(len(a) == len(b) == 5755 and a.dtype == b.dtype and a.dtype.itemsize == 24,
           'Exact native Pod vertex layout required')
    demand(len(original['indices']) == len(posed['indices']) == 10713 and
           original['indices'].tobytes() == posed['indices'].tobytes(), 'Triangle/index bytes changed')
    demand(a['uv'].tobytes() == b['uv'].tobytes(), 'UV bytes changed')
    demand(a['color'].tobytes() == b['color'].tobytes(), 'Vertex color bytes changed')
    demand(np.isfinite(b['position']).all(), 'Posed position is nonfinite')
    changed = np.any(a['position'].view(np.uint32).reshape(-1,3) !=
                     b['position'].view(np.uint32).reshape(-1,3),axis=1)
    return {name:int(np.count_nonzero(changed[first:last])) for name,first,last in
            (('chariot',0,2884),('left_engine',2884,4295),('right_engine',4295,5755))}



def load_plan(args, cpu, np):
    for label, (path, expected) in PINNED.items():
        old.digest_match(base.read_bounded(path, MAX_REPORT_BYTES), expected, label)
    demand(args.export_report and args.export_sha256, 'Native export report and exact SHA256 required')
    report_path = args.export_report.resolve(strict=True)
    demand(report_path.is_relative_to(EVIDENCE.resolve()), 'Export must be in this 49W evidence tree')
    raw = base.read_bounded(report_path, MAX_REPORT_BYTES)
    old.digest_match(raw, digest(args.export_sha256), 'native export report')
    wrapper = json.loads(raw)
    demand(wrapper.get('schema') == 'advancing49w.independent-verification.v1' and wrapper.get('passed') is True,
           'Passing independent native wrapper required')
    for flag in ('all_exact_golden_mesh_graph_noise_hashes_match','all_exported_mesh_bytes_match',
                 'all_frame_mesh_fnv64_match','canonical_state_persistence','graphics_roots_preserved_during_body_advance',
                 'material_surface_order_match','semantic_uv_color_and_ordered_indices_match'):
        demand(wrapper.get(flag) is True, 'Failed native validation: ' + flag)
    pins = {}
    for field, directory in (('native_capture_sha256', report_path.parent), ('semantic_scene_sha256', ROOT)):
        values = wrapper.get(field)
        demand(isinstance(values, dict) and 1 <= len(values) <= 512, 'Bounded native capture/input pins required')
        for name, expected in values.items():
            path = relative_file(directory, name)
            data = base.read_bounded(path, MAX_PIN_BYTES)
            old.digest_match(data, digest(expected), 'native pin ' + name)
            pins[path.relative_to(ROOT).as_posix()] = {'sha256': expected, 'bytes': len(data)}
    native_path = relative_file(report_path.parent, 'export-report.native.json')
    demand('export-report.native.json' in wrapper['native_capture_sha256'], 'Native export is unpinned')
    report = json.loads(base.read_bounded(native_path, MAX_REPORT_BYTES))
    wrapper_rows = wrapper.get('frames')
    demand(isinstance(wrapper_rows, list) and
           [{k:v for k,v in row.items() if k!='mesh_sha256'} for row in wrapper_rows] == report.get('frames')
           and report['summary'] == wrapper.get('summary'), 'Native/wrapper rows differ')
    report['frames'] = wrapper_rows
    rows = report_rows(report)
    demand(all(wrapper['native_capture_sha256'].get(row['mesh_path']) == row['mesh_sha256'] for row in rows), 'Mesh row/capture pins differ')
    demand(report['summary'].get('native_postphysics_graphics_pose_copies') == 0, 'Canonical graphics were postphysics overwritten')
    meshes, textures, camera, inherited = old.load_plan(RECIPE, cpu, False, np)
    selected = [r for r in rows if r['frame'] == args.frame and r['label'] == args.profile]
    demand(len(selected) == 1, 'Selected native frame/profile is absent')
    row = selected[0]
    mesh_path = relative_file(report_path.parent, row['mesh_path'])
    posed = base.load_mesh(mesh_path, np)
    old.digest_match(posed['raw'], row['mesh_sha256'], 'posed Pod mesh')
    changed = verify_mesh(meshes[1], posed, np)
    demand(changed == row['changed_vertices_vs49H'],
           'Component baseline vertex accounting differs')
    control_changed = None
    if row['label'] != 'full-motion':
        reference = next(r for r in rows if r['frame'] == row['frame'] and r['label'] == 'full-motion')
        full = base.load_mesh(relative_file(report_path.parent, reference['mesh_path']), np)
        old.digest_match(full['raw'], reference['mesh_sha256'], 'control full-motion reference')
        control_changed = verify_mesh(full, posed, np)
        demand(control_changed == row['changed_vertices_vs_full'],
               'Counterfactual component vertex accounting differs')
        demand(sum(control_changed.values())>0,'Counterfactual display control has no vertex effect')
    posed['id'], posed['fallback_rgba'] = 'POD01', old.FALLBACKS['POD01']
    meshes = [meshes[0], posed]
    plan = copy.deepcopy(inherited)
    inherited_hash = plan.pop('plan_sha256')
    plan.update({'schema': 'advancing49w.visual-plan.v1', 'label': LABEL,
                 'inherited49H_plan_sha256': inherited_hash,
                 'renderer49W_sha256': base.sha256(Path(__file__).read_bytes()),
                 'native_export': {'path': str(report_path), 'sha256': args.export_sha256,
                                   'bytes': len(raw), 'verified_pins': pins, 'selected': row},
                 'scenario': report['scenario'], 'fixture_profile': 'advancing49w-' + report['scenario'] + '-' + args.profile})
    demand(len({(span['instance'],span['material_index']) for span in inherited['spans'] if span['mesh_id']=='POD01'}) == 7, 'Pod01 must retain seven materials')
    plan['meshes'][1] = dict(posed['summary'], id='POD01', fallback_rgba=old.FALLBACKS['POD01'])
    positions = posed['vertex_data']['position']
    plan['uploaded_vehicle_geometry'] = {
        'vertex_sha256': base.sha256(posed['vertex_data'].tobytes()),
        'position_sha256': base.sha256(positions.tobytes()),
        'index_sha256': base.sha256(posed['indices'].tobytes()),
        'chariot_vertices':2884, 'engine_vertices':2871,
        'changed_component_vertices_from49H':changed,
        'control_changed_component_vertices_vs_full':control_changed,
        'uv_color_indices_unchanged': True,
        'bounds_min': positions.min(axis=0).astype(float).tolist(),
        'bounds_max': positions.max(axis=0).astype(float).tolist()}
    plan['profile'].pop('animation_lighting_or_visibility')
    plan['profile'].update({'native_advancing_frame': args.profile == 'full-motion',
                            'counterfactual_display_control': args.profile != 'full-motion',
                            'original_lighting_or_visibility': False,
                            'chariot': 'original input, graphics and ordinary Chariot lifecycle before native body/force/query-tail advance; canonical caches persist',
                            'vertex_transform': 'native49S rigid cache/display arithmetic; original VU vertex program not implemented'})
    plan['cpu_clip_controls'] = [base.cpu_control('posed Pod vertex ' + str(i),
        [*positions[i], 1.0], camera['combined'], np) for i in (0, 2883, 2884, 4294, 4295, 5754)]
    plan['budget'].update({'additional_posed_mesh_read_bytes': len(posed['raw']),
        'native_report_bytes': len(raw), 'native_pinned_file_bytes_verified': sum(p['bytes'] for p in pins.values()),
        'captioned_rgba_bytes': 640 * (480 + CAPTION_HEIGHT) * 4,
        'note49W': 'Native deformation already completed; Python uploads supplied float32 positions. Additional temporary source validation and caption/PNG overhead is measured in RSS.'})
    description = ('Source-ordered native input, graphics, caches, body, force and supplied-no-hit query tail' if args.profile == 'full-motion' else
                   'Counterfactual display control freezes every component root' if args.profile == 'frozen-roots' else
                   'Counterfactual display control replaces only the Chariot root with its physical cached-world root')
    scenario = ('Synthetic upward support at admitted354 balances authored gravity; it is not terrain contact' if report['scenario']=='synthetic-support' else
                'No-support fallthrough: identical initial state, controls and timing, with no added support force')
    plan['interpretation'] = (description + '. ' + scenario + '. Initial pose, velocities and history are supplied once; later state advances natively. '
        'Graphics roots remain prephysics. Render palettes are evaluated on a disposable copy after the frame without canonical cache side effects. '
        'The scene and 49H camera are unchanged. Original spawn/contact generation, full original frame/gameplay, original lighting/material programs and FPS are outside this staged diagnostic.')
    plan['caption'] = {'height': CAPTION_HEIGHT, 'outside_framebuffer': True}
    plan['plan_sha256'] = base.sha256(base.json_bytes(plan))
    return meshes, textures, camera, plan


def render(meshes, textures, camera, plan, output, np):
    from PIL import Image, ImageDraw, PngImagePlugin
    with tempfile.TemporaryDirectory(prefix='raw49h-', dir=output.parent) as directory:
        result = old.render(meshes, textures, camera, plan, Path(directory) / 'raw', np)
        with Image.open(result['images'][0]['path']) as im:
            pixels = im.convert('RGBA')
        demand(base.sha256(pixels.tobytes()) == result['pixel_rgba8_sha256'], 'Framebuffer round trip differs')
        row = plan['native_export']['selected']
        captioned = Image.new('RGBA', (640, 480 + CAPTION_HEIGHT), (18, 22, 30, 255))
        captioned.paste(pixels, (0, 0))
        draw = ImageDraw.Draw(captioned)
        scenario = plan['scenario']
        profile_line = ('Native input / graphics / body advance; prephysics graphics roots.' if row['label'] == 'full-motion' else
                        'Control: all component roots frozen; current internal motion.' if row['label'] == 'frozen-roots' else
                        'Control: Chariot physical root only; engines unchanged.')
        scenario_line = ('SYNTHETIC SUPPORT: upward force at354 balances gravity.' if scenario == 'synthetic-support' else
                         'NO-SUPPORT FALLTHROUGH: no support force; supplied no-hit query.')
        lines = [LABEL, f"Frame {row['frame']} | {row['elapsed_seconds']:.3f} supplied seconds | {row['label']}",
                 scenario_line, profile_line,
                 'Fixed49H camera; diagnostic materials. No authentic contact/gameplay/FPS.']
        for i, line in enumerate(lines):
            draw.text((8, 484 + 16*i), line, fill=(240, 240, 240, 255))
        metadata = PngImagePlugin.PngInfo()
        for name, value in {'Title': LABEL, 'Description': plan['interpretation'],
            'Plan SHA256': plan['plan_sha256'], 'Native Export SHA256': plan['native_export']['sha256'],
            'Framebuffer RGBA8 SHA256': result['pixel_rgba8_sha256']}.items():
            metadata.add_text(name, value)
        images = []
        for image, suffix, kind in ((pixels, '.raw.png', 'raw framebuffer'),
                                    (captioned, '.png', 'captioned diagnostic')):
            path = output.parent / (output.name + suffix)
            with path.open('xb') as handle:
                image.save(handle, format='PNG', pnginfo=metadata)
            images.append({'path': str(path), 'bytes': path.stat().st_size,
                'sha256': base.sha256(base.read_bounded(path, 4*1024*1024)),
                'rgba8_sha256': base.sha256(image.tobytes()), 'kind': kind,
                'frame': row['frame'], 'profile': row['label']})
        result['images'] = images
        result['interpretation'] = plan['interpretation']
        result['framebuffer_unchanged_by_caption'] = True
        return result


def self_test(np):
    dtype=np.dtype([('position','<f4',(3,)),('uv','<f4',(2,)),('color','u1',(4,))])
    vertices=np.zeros(5755,dtype=dtype)
    source={'vertex_data':vertices,'indices':np.zeros(10713,dtype='<u4')}
    posed={'vertex_data':vertices.copy(),'indices':source['indices'].copy()}
    demand(verify_mesh(source,posed,np)==dict(chariot=0,left_engine=0,right_engine=0),'Identical mesh control')
    for i in (0,2884,4295):posed['vertex_data']['position'][i,0]=1
    demand(verify_mesh(source,posed,np)==dict(chariot=1,left_engine=1,right_engine=1),'All three components can animate')
    checks=2
    for change in ('uv','color','index','nonfinite'):
        bad={'vertex_data':posed['vertex_data'].copy(),'indices':posed['indices'].copy()}
        if change=='uv':bad['vertex_data']['uv'][0,0]=1
        elif change=='color':bad['vertex_data']['color'][0,0]=1
        elif change=='index':bad['indices'][0]=1
        else:bad['vertex_data']['position'][0,0]=math.inf
        try:verify_mesh(source,bad,np)
        except ValueError:checks+=1
        else:raise ValueError('Invariant control was not rejected: '+change)
    return {'status':'passed','checks':checks,'GL_context_created':False,'draw_calls':0}


def owned_output(path):
    path = path.resolve()
    demand(path.is_relative_to(EVIDENCE.resolve()), 'Output must stay in this49W evidence tree')
    demand(path.parent.is_dir(), 'Output parent must already exist')
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    operation = parser.add_mutually_exclusive_group()
    operation.add_argument('--self-test', action='store_true')
    operation.add_argument('--plan', action='store_true')
    operation.add_argument('--render', action='store_true')
    parser.add_argument('--export-report', type=Path)
    parser.add_argument('--export-sha256')
    parser.add_argument('--frame', type=int, default=0)
    parser.add_argument('--profile', choices=('full-motion', 'frozen-roots', 'physical-copy-only'), default='full-motion')
    parser.add_argument('--cpu', type=int)
    parser.add_argument('--plan-output', type=Path)
    parser.add_argument('--output-prefix', type=Path)
    parser.add_argument('--draw-ack')
    parser.add_argument('--_worker', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args()
    os.environ.update(base.SOFTWARE_ENV)
    allowed = os.sched_getaffinity(0)
    cpu = min(allowed) if args.cpu is None else args.cpu
    demand(cpu in allowed, 'CPU outside permitted affinity')
    os.sched_setaffinity(0, {cpu})
    if args._worker:
        demand(args.render, 'Worker must render')
        resource.setrlimit(resource.RLIMIT_AS, (base.ADDRESS_SPACE_BYTES, base.ADDRESS_SPACE_BYTES))
        resource.setrlimit(resource.RLIMIT_CPU, (base.CPU_SECONDS, base.CPU_SECONDS+1))
        resource.setrlimit(resource.RLIMIT_FSIZE, (4*1024*1024, 4*1024*1024))
    import numpy as np
    demand(sys.byteorder == 'little', 'Little-endian native arrays required')
    if args.self_test:
        print(json.dumps(self_test(np)))
        return 0
    start = time.monotonic()
    meshes, textures, camera, plan = load_plan(args, cpu, np)
    if not args.render:
        demand(not args.draw_ack and not args.output_prefix, 'Drawing arguments require --render')
        if args.plan_output: base.write_json_new(owned_output(args.plan_output), plan)
        print(json.dumps({'status': 'planned_no_draw', 'plan_sha256': plan['plan_sha256'],
            'frame': args.frame, 'profile': args.profile, 'draws': plan['total_draw_calls'],
            'triangles': plan['submitted_triangles'], 'gpu_payload_bytes': plan['budget']['gl_geometry_and_texture_payload_bytes']}))
        return 0
    demand(not args.plan_output, 'Separate planning output from drawing')
    demand(args.draw_ack == plan['plan_sha256'], 'Exact current drawing plan required')
    demand(args.output_prefix is not None, 'Output prefix required')
    output = owned_output(args.output_prefix)
    demand(all(not (output.parent/(output.name+suffix)).exists() for suffix in ('.png','.raw.png','.json')), 'No output overwrite')
    if not args._worker:
        del meshes, textures, camera
        try:
            return subprocess.run([sys.executable, '-B', str(Path(__file__).resolve()), *sys.argv[1:], '--_worker'],
                                  env=dict(os.environ), timeout=base.WALL_SECONDS, check=False).returncode
        except subprocess.TimeoutExpired:
            print('Chariot49W rendering reached its90-second wall limit', file=sys.stderr)
            return 124
    result = render(meshes, textures, camera, plan, output, np)
    usage = resource.getrusage(resource.RUSAGE_SELF)
    result.update({'schema': 'advancing49w.visual-report.v1', 'plan': plan,
        'runtime': {'cpu_affinity': sorted(os.sched_getaffinity(0)), 'software_environment': base.SOFTWARE_ENV,
                    'maximum_resident_set_bytes': usage.ru_maxrss*1024,
                    'user_cpu_seconds': usage.ru_utime, 'system_cpu_seconds': usage.ru_stime,
                    'validation_and_render_wall_seconds': time.monotonic()-start}})
    base.write_json_new(output.with_suffix('.json'), result)
    print(json.dumps({'status': result['status'], 'images': result['images'], 'runtime': result['runtime']}))
    return 0

if __name__ == '__main__':
    try: sys.exit(main())
    except (OSError, ValueError, RuntimeError, AssertionError, KeyError, TypeError, OverflowError) as error:
        print(f'Chariot49W: {type(error).__name__}: {error}', file=sys.stderr)
        sys.exit(1)
