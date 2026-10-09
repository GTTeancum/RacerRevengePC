#!/usr/bin/env python3
"""Freeze 49H after checking baseline preservation and final evidence hashes."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import zipfile
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/visual49H'
BASELINE_SHA = '23e772638cb870677a46991d9a8085ca648dd34089980e79df7c69206f235f2c'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    if not __debug__:
        raise RuntimeError('Validation requires assertions; do not use Python -O')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    args = parser.parse_args()
    assert digest(args.baseline) == BASELINE_SHA
    changes = []
    with zipfile.ZipFile(args.baseline) as archive:
        old_names = {i.filename for i in archive.infolist() if not i.is_dir()}
        for name in sorted(old_names):
            path = ROOT.parent / name
            assert path.is_file(), ('Missing inherited file', name)
            if hashlib.sha256(archive.read(name)).hexdigest() != digest(path):
                changes.append(name)
    assert changes == ['RacerRevengePC/README.md'], changes
    load = lambda name: json.loads((E / name).read_text())
    review = load('independent-review.json')
    assert review['status'] == 'PASS_SCOPED_DIAGNOSTIC' and not review['blocking_findings']
    for row in review['artifacts']:
        assert digest(ROOT / row['path']) == row['sha256'], row['path']
    packaging = load('packaging-review.json')
    assert packaging['status'] == 'PASS' and not packaging['blocking_findings']
    for row in packaging['reviewed_files']:
        assert digest(ROOT / row['path']) == row['sha256'], row['path']
    selection = json.loads((ROOT / 'evidence/visual49G/asset-selection.json').read_text())
    for row in selection['objects']:
        path = ROOT / 'evidence/visual49G' / row['path']
        assert path.stat().st_size == row['bytes'] and digest(path) == row['sha256']
    assert len(selection['objects']) == 859
    for mode in ('normal', 'sanitized'):
        report = load(f'binding-tests/{mode}-final/report.json')
        assert report['status'] == 'PASS' and report['checks'] == 33
        assert all(row['status'] == 'PASS' for row in report['cases'])
        assert digest(E / f'surface-bindings-rebuilt-{mode}.json') == digest(E / 'surface-bindings-final.json')
    builds = load('build-validation/result.json')
    assert builds['status'] == 'passed'
    assert builds['build_script_sha256'] == digest(ROOT / 'tools/build_visual49h.py')
    assert builds['source_sha256'] == digest(ROOT / 'tools/visual/export_surface_bindings49h.cpp')
    assert all(row['stderr_empty'] and row['manifest_exact_bytes_equal'] for row in builds['runs'])
    regressions = load('regression-summary.json')
    assert regressions['status'] == 'passed' and regressions['normal_sanitized_stdout_identical']
    for run in regressions['runs'].values():
        assert run['checks'] == 7334097 and run['suites_passed'] == 10 and run['input_count'] == 74
        assert run['input_hashes_unchanged']
    probe = load('renderer-positive-tests/final-source/probe.json')
    assert probe['status'] == 'rendered_diagnostic_only'
    assert len(probe['positive_pixel_controls']) == 12 and all(row['passed'] for row in probe['positive_pixel_controls'])
    controls = load('renderer-positive-tests/positive-controls-summary.json')
    assert controls['status'] == 'PASS'
    assert controls['renderer_script_sha256'] == digest(ROOT / 'tools/visual/render_textured49h.py')
    assert controls['base49g_script_sha256'] == digest(ROOT / 'tools/visual/render_neutral49g.py')
    assert len(controls['semantic_sensitivity']) == 11 and all(row['distinguished'] for row in controls['semantic_sensitivity'])
    for name in ('independent-binding-review-final.json', 'independent-frame-review-final.json',
                 'independent-reproduction.json', 'independent-renderer-scalar-checks.json'):
        assert load(name)['status'] == 'PASS', name
    reproduction = load('independent-reproduction.json')
    assert reproduction['approved_plans_reproduced_exactly']
    assert len(reproduction['outputs']) == 2 and all(row['png_byte_identical'] for row in reproduction['outputs'])
    frames = []
    for stem, uploads in [('ta-pod01-textured-final', 26), ('ta-pod01-neutral-control-final', 0)]:
        frame = load('frames/' + stem + '.json')
        assert frame['status'] == 'rendered_diagnostic_only'
        assert frame['actual_draw_calls'] == 360 and frame['actual_submitted_triangles'] == 73409
        assert frame['actual_texture_uploads'] == uploads and frame['actual_gl_buffer_uploads'] == 4
        assert frame['uniform_bits_round_trip'] and frame['opaque_output_alpha']
        assert frame['runtime']['cpu_affinity'] == [0]
        assert frame['plan']['renderer_script_sha256'] == digest(ROOT / 'tools/visual/render_textured49h.py')
        assert frame['plan']['base49g_script_sha256'] == digest(ROOT / 'tools/visual/render_neutral49g.py')
        assert frame['plan']['bindings_sha256'] == digest(E / 'surface-bindings-final.json')
        for image in frame['images']:
            assert digest(E / 'frames' / Path(image['path']).name) == image['sha256']
        frames.append(frame)
    pixels = lambda p: Image.open(p).convert('RGBA').tobytes()
    neutral = pixels(E / 'frames/ta-pod01-neutral-control-final.png')
    assert len(neutral) == 1228800 and neutral == pixels(ROOT / 'evidence/visual49G/frames/ta-pod01-neutral.png')
    assert pixels(E / 'frames/ta-pod01-textured-final.png') == pixels(E / 'frames/ta-pod01-textured.png')
    source_hashes = {}
    for path in sorted(ROOT.rglob('*')):
        if not path.is_file() or path == E / 'validation.json':
            continue
        relative = path.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/' + relative not in old_names or relative == 'README.md':
            source_hashes[relative] = digest(path)
    result = {
        'checkpoint': '49H-TexturedDiagnostic', 'status': 'passed',
        'validated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'baseline': {'archive': args.baseline.name, 'sha256': BASELINE_SHA,
                     'bytes': args.baseline.stat().st_size, 'members_preserved': len(old_names),
                     'changed_baseline_files': changes,
                     'library_file_id': 'libfile_65b4e30a810481918eed1369e9c9924d'},
        'scope': 'Same49G camera/geometry/pose; original first-pass/slot-one cached textures and UVs; external diagnostic RGB times vertex RGB, opaque alpha, linear repeat largest mip; original material programs unexecuted.',
        'coverage': {'surfaces': 360, 'textured': 286, 'fallback': 74,
                     'textured_triangles': 67967, 'fallback_triangles': 5442,
                     'total_triangles': 73409, 'textures': 26, 'selected_asset_files': 859},
        'payload_bytes': {'geometry': 3663732, 'textures': 3940352, 'sum': 7604084},
        'normal_and_asan_ubsan': {'exporter_checks_each': 33, 'related_suites_each': 10,
                                'checks_each': 7334097, 'unchanged_inputs_each': 74,
                                'fresh_manifest_bytes_equal': True, 'leak_check': 'unavailable: ptrace'},
        'pixel_controls': {'actual_GL_probes': 12, 'CPU_checks': 7,
                           'numeric_sensitivity_predictions': 11, 'metadata_sensitivity_cases': 5,
                           'scalar_rejections': 14, 'neutral_all_pixels_equal49G': True,
                           'independent_final_PNG_bytes_reproduced': True},
        'host_peak_RSS_bytes': {'textured': frames[0]['runtime']['maximum_resident_set_bytes'],
                                'neutral_control': frames[1]['runtime']['maximum_resident_set_bytes']},
        'images': [row for frame in frames for row in frame['images']],
        'limitations': ['No original material microprograms, extra maps, alpha blending/testing, lighting, visibility or animation.',
                       'Explicit diagnostic Pod placement is not a race grid or physics pose.',
                       'Host offline/staging tools allocate; no game-FPS or Xbox budget claim.',
                       '49C remains excluded/unreleased and collision/action-reset contracts remain unresolved.'],
        'source_hashes': source_hashes,
    }
    (E / 'validation.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'status': 'passed', 'baseline_members': len(old_names),
                      'changed_baseline_files': changes, 'new_or_changed_files_hashed': len(source_hashes)}))


if __name__ == '__main__':
    main()
