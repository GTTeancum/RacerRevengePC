#!/usr/bin/env python3
"""Synthetic injected-stage mainbody diagnostic, using unchanged bounded 49H draw code.

A non-drawing plan pins the passed native trace, original assets, camera, and exact
CPU display transform. Rendering requires the plan hash. The supplied post-player,
world/contact/no-hit stages are NOT a recovered freeflight/full-vehicle loop.
"""
from __future__ import annotations
import argparse
import copy
import hashlib
import json
import math
import os
from pathlib import Path
import resource
import struct
import subprocess
import sys
import tempfile
import time
import render_textured49h as old

base = old.base
demand = old.demand
ROOT = Path(__file__).resolve().parents[2]
VISUAL = ROOT / 'evidence/bodyAdvancement49K/visual'
RECIPE = ROOT / 'evidence/visual49H/frames/recipe-textured-final.json'
LABEL = 'Synthetic injected-stage mainbody diagnostic'
ANCHOR = [2903.56103515625, 1292.14404296875, 7649.8837890625]
PHYSICAL_ORIGIN = [0.0, 10.0, 0.0]
OFFSET = [a - b for a, b in zip(ANCHOR, PHYSICAL_ORIGIN)]
MAX_TRACE_BYTES = 2 * 1024 * 1024
MAX_FRAMES = 180
LABEL_HEIGHT = 72
PINNED = {
    '49H_renderer': (Path(old.__file__), '4ee39e6e7e9786f221e352299f14286a9bfcdf9143f85162951706acc29a3598'),
    '49G_base': (Path(base.__file__), '4bd13edc8c741f40f68ec965f9559dfe3d13a3163d6bb43200ce91ca4204f1cc'),
    '49H_recipe': (RECIPE, '7bf0a0ff562b88d26099da4411349640d7f748d48875ff39af7fef0101b7daa3'),
    '49H_bindings': (ROOT / 'evidence/visual49H/surface-bindings-final.json',
                     'f035fd96815ab2b84fd342afdf999fd3167997eaff13c0dff664247ef9420549'),
}


def finite_f32_array(value, count, label):
    demand(isinstance(value, list) and len(value) == count, label + ' extent')
    demand(all(type(x) in (int, float) and math.isfinite(x) for x in value), label + ' finite numbers')
    return [base.f32(x) for x in value]


def scene_values(step):
    scene = finite_f32_array(step['scene'], 16, 'scene16')
    demand([scene[i] for i in (3, 7, 11, 15)] == [0.0, 0.0, 0.0, 1.0], 'scene16 must be affine row-vector')
    if 'scene_bits' in step:
        words = step['scene_bits']
        demand(isinstance(words, list) and len(words) == 16 and all(
            type(x) is int and 0 <= x <= 0xffffffff for x in words), 'scene bit words')
        demand(words == base.f32_bits(scene), 'scene decimal/word disagreement')
        scene = list(struct.unpack('<16f', struct.pack('<16I', *words)))
    return scene


def validate_trace(value):
    demand(isinstance(value, dict) and set(value) == {'kind', 'external_stages', 'frames'}, 'trace schema')
    demand(value['kind'] == 'synthetic bounded main-body diagnostic', 'trace diagnostic scope')
    demand(isinstance(value['external_stages'], str) and all(
        part in value['external_stages'] for part in ('post-player', 'post-base', 'no-hit219120')), 'trace external stages')
    frames = value['frames']
    demand(isinstance(frames, list) and 1 <= len(frames) <= MAX_FRAMES, 'trace frame extent')
    for index, frame in enumerate(frames):
        demand(isinstance(frame, dict) and frame.get('frame') == index and type(frame['frame']) is int,
               'consecutive integer frame IDs')
        old.integer(frame['milliseconds'], 'trace timestamp', 0xffffffff)
        old.integer(frame['synthetic_count'], 'supplied counter', 0xffff)
        old.integer(frame['supplied_pending_overflows'], 'supplied overflow', 0xffffffff)
        old.integer(frame['count'], 'substep count', 4, 1)
        factor = finite_f32_array([frame['factor']], 1, 'frame factor')[0]
        demand(0 <= factor <= base.f32(.06), 'frame factor bound')
        demand(isinstance(frame['steps'], list) and len(frame['steps']) == frame['count'], 'exact substep extent')
        for substep, step in enumerate(frame['steps']):
            demand(type(step['step']) is int and step['step'] == substep, 'consecutive substep IDs')
            demand(0 <= finite_f32_array([step['factor']], 1, 'substep factor')[0] <= base.f32(.06), 'substep factor bound')
            demand('scene_bits' in step, 'Exact native scene_bits required for trace')
            scene_values(step)
            finite_f32_array(step['velocity'], 3, 'velocity')
            finite_f32_array(step['orientation'], 4, 'orientation')
    return frames


def scalar_position(position, scene):
    """Ordered binary64, no FMA; round once to binary32 at the display upload."""
    local = [float(position[i]) - ANCHOR[i] for i in range(3)]
    result = []
    for column in range(3):
        x = local[0] * scene[column]
        x = x + local[1] * scene[4 + column]
        x = x + local[2] * scene[8 + column]
        x = x + scene[12 + column]
        x = x + OFFSET[column]
        result.append(base.f32(x))
    return result


def display_transform(mesh, scene, np):
    source = mesh['vertex_data']
    target = source.copy()
    # Work one axis at a time, deliberately preserving scalar operation order.
    for column in range(3):
        value = (source['position'][:, 0].astype(np.float64) - ANCHOR[0]) * scene[column]
        value = value + (source['position'][:, 1].astype(np.float64) - ANCHOR[1]) * scene[4 + column]
        value = value + (source['position'][:, 2].astype(np.float64) - ANCHOR[2]) * scene[8 + column]
        value = value + scene[12 + column]
        value = value + OFFSET[column]
        target['position'][:, column] = value.astype(np.float32)
    demand(np.isfinite(target['position']).all(), 'transformed position finite')
    demand(source['uv'].tobytes() == target['uv'].tobytes(), 'UV bytes must remain exact')
    demand(source['color'].tobytes() == target['color'].tobytes(), 'color bytes must remain exact')
    # An independent scalar implementation checks every position, not only bounds.
    for i, point in enumerate(source['position']):
        demand(base.f32_bits(scalar_position(point, scene)) == base.f32_bits(target['position'][i]),
               'scalar/vector display transform disagreement')
    result = dict(mesh, vertex_data=target)
    return result


def load_plan(args, cpu, np):
    pinned = {}
    for label, (path, expected) in PINNED.items():
        raw = base.read_bounded(path, 2 * 1024 * 1024)
        old.digest_match(raw, expected, label)
        pinned[label] = {'path': str(path), 'sha256': expected, 'bytes': len(raw)}
    meshes, textures, camera, inherited = old.load_plan(RECIPE, cpu, False, np)
    demand(args.trace and args.trace_sha256 and args.native_pass_log and args.native_pass_sha256,
           'Final trace and native pass log paths + SHA256 values are mandatory')
    trace_path = args.trace.resolve(strict=True)
    raw = base.read_bounded(trace_path, MAX_TRACE_BYTES)
    old.digest_match(raw, args.trace_sha256, 'final native trace')
    trace = json.loads(raw)
    frames = validate_trace(trace)
    pass_path = args.native_pass_log.resolve(strict=True)
    passed = base.read_bounded(pass_path, 128 * 1024)
    old.digest_match(passed, args.native_pass_sha256, 'native passing run log')
    passed_text = passed.decode('utf-8')
    demand('body_advancement49k:' in passed_text and
           'persistent synthetic frames' in passed_text and 'retained original-word instructions' in passed_text,
           'Native pass summary required (caller must supply successful run log)')
    selected_index = old.integer(args.frame, 'selected frame', len(frames) - 1)
    frame = frames[selected_index]
    step = frame['steps'][-1]
    scene = scene_values(step)
    transformed = display_transform(meshes[1], scene, np)
    meshes = [meshes[0], transformed]
    plan = copy.deepcopy(inherited)
    inherited_sha = plan.pop('plan_sha256')
    plan.update({'schema': 'bodyAdvancement49K.visual-plan.v1', 'label': LABEL,
                 'diagnostic_only': True, 'inherited49H_plan_sha256': inherited_sha,
                 '49K_renderer_script_sha256': base.sha256(Path(__file__).read_bytes()), 'pinned_inputs': pinned})
    positions = transformed['vertex_data']['position']
    source = transformed['raw']
    frame_positions = [scene_values(f['steps'][-1])[12:15] for f in frames]
    plan['native_trace'] = {'path': str(trace_path), 'sha256': args.trace_sha256, 'bytes': len(raw),
        'pass_log_path': str(pass_path), 'pass_log_sha256': args.native_pass_sha256,
        'frame_count': len(frames), 'substep_count': sum(f['count'] for f in frames),
        'external_stages': trace['external_stages'], 'selected_frame': frame['frame'],
        'selected_last_substep': step['step'], 'milliseconds': frame['milliseconds'],
        'frame_factor': frame['factor'], 'substep_factor': step['factor'],
        'synthetic_count': frame['synthetic_count'], 'supplied_pending_overflows': frame['supplied_pending_overflows'],
        'scene_f32_bits': base.f32_bits(scene), 'scene': scene,
        'scene_word_source': 'explicit native scene_bits uint32 words checked against decimal',
        'frame_end_translation_bounds': {'min': [min(p[i] for p in frame_positions) for i in range(3)],
                                         'max': [max(p[i] for p in frame_positions) for i in range(3)]},
        'final_frame_translation': frame_positions[-1]}
    plan['display_transform'] = {'mesh_id': 'POD01', 'source_placement': ANCHOR,
        'initial_physical_origin': PHYSICAL_ORIGIN, 'display_offset': OFFSET,
        'formula': 'local=p49G-anchor; display[j]=f32((((local[0]*scene[j]+local[1]*scene[4+j])+local[2]*scene[8+j])+scene[12+j])+display_offset[j])',
        'arithmetic': 'ordered binary64 products and additions, no FMA; one final binary32 conversion',
        'convention': 'row-major scene16, row-vector local position, translation indices12..14',
        'scope': 'Display placement only; not original race grid or original physics position. Re-centering retains prior49G binary32 position rounding.',
        'ground_behavior': 'No ground clamp/contact, camera following, rescale, auto-fit or visibility adjustment. Supplied no-hit query allows fallthrough.',
        'uploaded_vertex_sha256': base.sha256(transformed['vertex_data'].tobytes()),
        'uploaded_position_sha256': base.sha256(positions.tobytes()),
        'uploaded_index_sha256': base.sha256(transformed['indices'].tobytes()),
        'uv_sha256': base.sha256(transformed['vertex_data']['uv'].tobytes()),
        'color_sha256': base.sha256(transformed['vertex_data']['color'].tobytes()),
        'source_geometry_sha256': base.sha256(source), 'all_vertices_scalar_checked': len(positions),
        'bounds_min': positions.min(axis=0).astype(float).tolist(),
        'bounds_max': positions.max(axis=0).astype(float).tolist()}
    plan['profile'].update({'auto_fit_or_reposition': 'Only explicit49K Pod model display transform above',
                            'animation_lighting_or_visibility': False})
    plan['cpu_clip_controls'] = [base.cpu_control('transformed POD01 vertex ' + str(i), [*positions[i], 1.0], camera['combined'], np)
                                 for i in (0, len(positions)//2, len(positions)-1)]
    plan['caption'] = {'outside_framebuffer': True, 'width': 640, 'height': LABEL_HEIGHT,
                       'output_dimensions': [640, 480 + LABEL_HEIGHT], 'label': LABEL}
    plan['budget'].update({'pod_vertex_copy_bytes': transformed['vertex_data'].nbytes,
        'per_axis_float64_array_bytes': len(positions) * 8,
        'transform_working_array_upper_bound_bytes': len(positions) * 8 * 4,
        'trace_file_bytes': len(raw), 'native_pass_log_bytes': len(passed),
        'captioned_RGBA_bytes': 640 * (480 + LABEL_HEIGHT) * 4,
        'loaded_framebuffer_RGBA_bytes': 640 * 480 * 4,
        'captioning_note': 'Sequential to GL cleanup; label canvas/PNG encoder/Pillow allocations are additional to inherited payload arithmetic. RSS measured.'})
    plan['interpretation'] = ('Synthetic injected-stage mainbody diagnostic; original-word-compared native scene evolution with synthetic tuning and supplied external stages. '
                              'No actual freeflight, full vehicle, world collision, original lighting or gameplay/FPS claim.')
    plan['plan_sha256'] = base.sha256(base.json_bytes(plan))
    return meshes, textures, camera, plan


def render(meshes, textures, camera, plan, output, np):
    from PIL import Image, ImageDraw, PngImagePlugin
    # Reuse the exact49H GL route. Its temporary file is never presented as a49K view.
    with tempfile.TemporaryDirectory(prefix='raw49h-', dir=VISUAL) as directory:
        result = old.render(meshes, textures, camera, plan, Path(directory) / 'raw', np)
        with Image.open(result['images'][0]['path']) as im:
            pixels = im.convert('RGBA')
        demand(base.sha256(pixels.tobytes()) == result['pixel_rgba8_sha256'], 'raw framebuffer pixel round trip')
        captioned = Image.new('RGBA', (640, 480 + LABEL_HEIGHT), (18, 22, 30, 255))
        captioned.paste(pixels, (0, 0))
        draw = ImageDraw.Draw(captioned)
        frame = plan['native_trace']
        lines = [LABEL,
                 f"Frame {frame['selected_frame']}, last substep {frame['selected_last_substep']}; supplied-counter timestamp {frame['milliseconds']} ms",
                 'Synthetic tuning + supplied post-player/world/contact/no-hit states.',
                 'Fixed recovered camera. No ground clamp; no-hit can fall through terrain.']
        for i, line in enumerate(lines):
            draw.text((8, 484 + 16*i), line, fill=(240, 240, 240, 255))
        metadata = PngImagePlugin.PngInfo()
        metadata.add_text('Title', LABEL)
        metadata.add_text('Description', plan['interpretation'])
        metadata.add_text('Plan SHA256', plan['plan_sha256'])
        metadata.add_text('Trace SHA256', frame['sha256'])
        metadata.add_text('Framebuffer RGBA8 SHA256', result['pixel_rgba8_sha256'])
        raw_path = output.parent / (output.name + '.raw.png')
        with raw_path.open('xb') as handle:
            pixels.save(handle, format='PNG', pnginfo=metadata)
        raw_png = base.read_bounded(raw_path, base.MAX_PIXELS * 8 + 65536)
        path = output.with_suffix('.png')
        with path.open('xb') as handle:
            captioned.save(handle, format='PNG', pnginfo=metadata)
        png = base.read_bounded(path, 640 * (480 + LABEL_HEIGHT) * 8 + 65536)
        result['images'] = [{'path': str(path), 'bytes': len(png), 'sha256': base.sha256(png),
                             'kind': LABEL, 'frame': frame['selected_frame'],
                             'captioned_rgba8_sha256': base.sha256(captioned.tobytes())},
                            {'path': str(raw_path), 'bytes': len(raw_png), 'sha256': base.sha256(raw_png),
                             'kind': LABEL + ' (raw framebuffer; same PNG scope metadata)',
                             'frame': frame['selected_frame'], 'rgba8_sha256': result['pixel_rgba8_sha256']}]
        result['interpretation'] = plan['interpretation']
        result['framebuffer_content_unchanged_by_caption'] = True
        return result


def self_test(np):
    checks = []
    dtype = np.dtype([('position', '<f4', (3,)), ('uv', '<f4', (2,)), ('color', 'u1', (4,))])
    vertices = np.zeros(3, dtype=dtype)
    vertices['position'] = [ANCHOR, [ANCHOR[i]+(i+1) for i in range(3)], [ANCHOR[i]-(i+1) for i in range(3)]]
    vertices['uv'] = [[-.5, 1.5], [.25, .75], [2, -2]]
    vertices['color'] = [[1, 2, 3, 4], [255, 127, 63, 0], [2, 4, 6, 8]]
    mesh = {'vertex_data': vertices, 'indices': np.asarray([0, 1, 2], dtype='<u4')}
    identity = [1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,10.,0.,1.]
    got = display_transform(mesh, identity, np)
    demand(got['vertex_data'].tobytes() == vertices.tobytes(), 'identity+initial origin restores exact supplied mesh')
    demand(got['indices'] is mesh['indices'], 'index object must be reused')
    checks.append('identity basis + initial physical origin restores supplied mesh exactly')
    nonsymmetric = [0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,3.,4.,5.,1.]
    expected = [base.f32(ANCHOR[0]+1), base.f32(ANCHOR[1]-5), base.f32(ANCHOR[2]+8)]
    demand(scalar_position(vertices['position'][1], nonsymmetric) == expected, 'nonsymmetric row-vector transform')
    display_transform(mesh, nonsymmetric, np)
    checks += ['nonsymmetric basis, translation and offset use row-vector convention', 'UV/color bytes and index identity preserved',
               'all scalar transformed positions equal vectorized float32 output']
    scene_values({'scene': identity, 'scene_bits': base.f32_bits(identity)})
    for altered in ([1.] * 15, identity[:3] + [1.] + identity[4:], [math.nan] + identity[1:]):
        try:
            scene_values({'scene': altered})
        except ValueError:
            pass
        else:
            raise ValueError('invalid scalar/schema accepted')
    checks += ['finite affine scene16 schema and optional word validation', 'malformed/nonfinite/nonaffine scalar schema rejected']
    positive_trace = {'kind': 'synthetic bounded main-body diagnostic',
                      'external_stages': 'post-player, post-base, no-hit219120',
                      'frames': [{'frame': 0, 'milliseconds': 0, 'synthetic_count': 0,
                                  'supplied_pending_overflows': 0, 'count': 1, 'factor': 0.,
                                  'steps': [{'step': 0, 'factor': 0., 'scene': identity,
                                             'scene_bits': base.f32_bits(identity),
                                             'velocity': [0., 0., 0.], 'orientation': [1., 0., 0., 0.]}]}]}
    validate_trace(positive_trace)
    negative_trace = copy.deepcopy(positive_trace)
    negative_trace['frames'][0]['steps'][0]['scene_bits'][0] += 1
    try:
        validate_trace(negative_trace)
    except ValueError:
        pass
    else:
        raise ValueError('decimal/word disagreement accepted')
    checks.append('trace frame/substep schema and decimal/native-word agreement')
    one = display_transform(mesh, nonsymmetric, np)['vertex_data'].tobytes()
    two = display_transform(mesh, nonsymmetric, np)['vertex_data'].tobytes()
    demand(one == two, 'CPU transform determinism')
    checks.append('repeated CPU transform bytes identical')
    return {'status': 'passed', 'checks': checks, 'draw_calls': 0, 'GL_context_created': False}


def owned_output(path):
    path = path.resolve()
    demand(path.is_relative_to(VISUAL.resolve()), 'Outputs must stay in scoped49K visual evidence')
    demand(path.parent.is_dir(), 'Output parent must exist')
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    op = parser.add_mutually_exclusive_group()
    op.add_argument('--self-test', action='store_true')
    op.add_argument('--plan', action='store_true')
    op.add_argument('--render', action='store_true')
    parser.add_argument('--trace', type=Path)
    parser.add_argument('--trace-sha256')
    parser.add_argument('--native-pass-log', type=Path)
    parser.add_argument('--native-pass-sha256')
    parser.add_argument('--frame', type=int, default=0)
    parser.add_argument('--cpu', type=int)
    parser.add_argument('--plan-output', type=Path)
    parser.add_argument('--output-prefix', type=Path)
    parser.add_argument('--draw-ack')
    parser.add_argument('--_worker', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args()
    os.environ.update(base.SOFTWARE_ENV)
    allowed = os.sched_getaffinity(0)
    cpu = min(allowed) if args.cpu is None else args.cpu
    demand(cpu in allowed, 'CPU must be in allowed affinity')
    os.sched_setaffinity(0, {cpu})
    if args._worker:
        demand(args.render, 'Worker requires render')
        resource.setrlimit(resource.RLIMIT_AS, (base.ADDRESS_SPACE_BYTES, base.ADDRESS_SPACE_BYTES))
        resource.setrlimit(resource.RLIMIT_CPU, (base.CPU_SECONDS, base.CPU_SECONDS+1))
        resource.setrlimit(resource.RLIMIT_FSIZE, (3*1024*1024, 3*1024*1024))
    import numpy as np
    demand(sys.byteorder == 'little', 'Little-endian arrays required')
    if args.self_test:
        result = self_test(np)
        if args.plan_output:
            base.write_json_new(owned_output(args.plan_output), result)
        print(json.dumps(result, indent=2))
        return 0
    start = time.monotonic()
    meshes, textures, camera, plan = load_plan(args, cpu, np)
    if not args.render:
        demand(not args.draw_ack and not args.output_prefix, 'Rendering arguments require --render')
        if args.plan_output:
            base.write_json_new(owned_output(args.plan_output), plan)
        print(json.dumps({'status': 'planned_no_draw', 'plan_sha256': plan['plan_sha256'],
                          'frame': args.frame, 'draws': plan['total_draw_calls'], 'triangles': plan['submitted_triangles']}, indent=2))
        return 0
    demand(not args.plan_output, '--plan-output is for non-drawing plans')
    demand(args.draw_ack == plan['plan_sha256'], 'Exact current plan acknowledgement required')
    demand(args.output_prefix is not None, 'Render output prefix required')
    output = owned_output(args.output_prefix)
    demand(not output.with_suffix('.png').exists() and not output.with_suffix('.json').exists()
           and not (output.parent / (output.name + '.raw.png')).exists(), 'No output overwrite')
    if not args._worker:
        command = [sys.executable, '-B', str(Path(__file__).resolve()), *sys.argv[1:], '--_worker']
        del meshes, textures, camera
        try:
            return subprocess.run(command, env=dict(os.environ), timeout=base.WALL_SECONDS, check=False).returncode
        except subprocess.TimeoutExpired:
            print('49K render reached inherited90s wall ceiling', file=sys.stderr)
            return 124
    result = render(meshes, textures, camera, plan, output, np)
    usage = resource.getrusage(resource.RUSAGE_SELF)
    result.update({'schema': 'bodyAdvancement49K.visual-report.v1', 'plan': plan,
                   'runtime': {'cpu_affinity': sorted(os.sched_getaffinity(0)), 'software_environment': base.SOFTWARE_ENV,
                               'maximum_resident_set_bytes': usage.ru_maxrss*1024,
                               'user_cpu_seconds': usage.ru_utime, 'system_cpu_seconds': usage.ru_stime,
                               'validation_and_render_wall_seconds': time.monotonic()-start}})
    base.write_json_new(output.with_suffix('.json'), result)
    print(json.dumps({'status': result['status'], 'frame': args.frame, 'plan_sha256': plan['plan_sha256'],
                      'images': result['images'], 'runtime': result['runtime']}, indent=2))
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, AssertionError, KeyError, TypeError, OverflowError) as error:
        print(f'bodyAdvancement49K visual: {type(error).__name__}: {error}', file=sys.stderr)
        sys.exit(1)
