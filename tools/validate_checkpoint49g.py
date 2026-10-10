#!/usr/bin/env python3
"""Freeze passed 49G evidence after rechecking baseline and artifact hashes."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / 'evidence/visual49G'
BASELINE_SHA = '8c9a74ba2c9e5acd53f1e1e0b313a9e3a36c93981ed964e6b1ccf05e10dbf47e'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    if not __debug__:
        raise RuntimeError('Validation requires assertions; do not use Python -O')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    args = parser.parse_args()
    assert digest(args.baseline) == BASELINE_SHA, 'Expected exact verified49F archive'
    changes = []
    with zipfile.ZipFile(args.baseline) as archive:
        old_names = {i.filename for i in archive.infolist() if not i.is_dir()}
        for name in sorted(old_names):
            path = ROOT.parent / name
            assert path.is_file(), ('Missing inherited file', name)
            if hashlib.sha256(archive.read(name)).hexdigest() != digest(path):
                changes.append(name)
    assert changes == ['RacerRevengePC/README.md'], changes
    load = lambda name: json.loads((EVIDENCE / name).read_text())
    selection = load('asset-selection.json')
    for row in selection['objects']:
        path = EVIDENCE / row['path']
        assert path.stat().st_size == row['bytes'] and digest(path) == row['sha256'], row['path']
    independent = load('independent-review.json')
    assert independent['status'] == 'PASS_SCOPED_DIAGNOSTIC_WITH_LIMITATIONS'
    assert not independent['blocking_findings']
    for row in independent['current_file_hashes']:
        assert digest(ROOT / row['path']) == row['sha256'], row['path']
    for row in independent['fresh_reproduction']['artifact_comparisons']:
        assert row['exact_bytes_equal'] and digest(ROOT / row['artifact']) == row['sha256'], row['artifact']
    packaging = load('packaging-review.json')
    assert packaging['status'] in ('passed', 'PASS_WITH_INHERITED_WARNINGS'), 'Packaging review not passed'
    assert not packaging['blocking_findings']
    for row in packaging['reviewed_files']:
        assert digest(ROOT / row['path']) == row['sha256'], row['path']
    build = load('build-validation/result.json')
    assert build['status'] == 'passed'
    assert digest(ROOT / 'tools/build_visual49g.py') == build['source_sha256']
    for mode in ('normal', 'sanitized'):
        assert build[mode]['mesh_and_export_report_identical'] and build[mode]['camera_json_identical']
    regression = load('regression-summary.json')
    assert regression['status'] == 'passed' and regression['all_74_tracked_inputs_verified_unchanged_after_both_runs']
    for run in regression['runs'].values():
        assert run['checks'] == 7334097 and run['suites_passed'] == 10
        assert not run['errors'] and not run['changed_inputs']
    assert load('asset-selection-validation.json')['status'] == 'PASS'
    assert load('renderer-validation/validation-manifest.json')['status'] == 'passed'
    frames = [load('frames/ta-pod01-neutral.json'), load('frames/ta-pod01-wireframe.json')]
    for frame, calls, triangles in zip(frames, (2, 4), (73409, 146818)):
        assert frame['status'] == 'rendered_diagnostic_only'
        assert frame['actual_draw_calls'] == calls and frame['actual_submitted_triangles'] == triangles
        assert frame['uniform_bits_round_trip'] and frame['runtime']['cpu_affinity'] == [0]
        for image in frame['images']:
            assert digest(EVIDENCE / 'frames' / Path(image['path']).name) == image['sha256']
    source_hashes = {}
    for path in sorted(ROOT.rglob('*')):
        if not path.is_file() or path == EVIDENCE / 'validation.json':
            continue
        relative = path.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/' + relative not in old_names or relative == 'README.md':
            source_hashes[relative] = digest(path)
    result = {
        'checkpoint': '49G-VisualDiagnostic', 'status': 'passed',
        'validated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'baseline': {'archive': args.baseline.name, 'sha256': BASELINE_SHA,
                     'bytes': args.baseline.stat().st_size, 'members_preserved': len(old_names),
                     'changed_baseline_files': changes,
                     'library_file_id': 'libfile_e09edc73f1e881919cf8207ab4d21c16'},
        'scope': 'Authentic TA static geometry and three Pod01 bind parts, explicit supplied stationary pose, unchanged49F core camera matrices, external one-CPU unlit Mesa diagnostic pixels.',
        'assets': {'native_files': len(selection['objects']),
                   'bytes': sum(row['bytes'] for row in selection['objects']),
                   'ta_drawable_instances': 284, 'ta_non_drawn_instances': 34,
                   'saved_roots_replaced_once': 42, 'unique_scenes': 231},
        'draws': {'solid': 2, 'solid_triangles': 73409, 'with_wire': 4,
                  'with_wire_triangle_submissions': 146818, 'viewport': [640, 480]},
        'normal_and_asan_ubsan': {'related_suites_each': 10, 'checks_each': 7334097,
                                'tracked_inputs_unchanged': 74, 'runtime_stderr_empty': True,
                                'fresh_export_camera_bytes_equal': True, 'leak_check': 'unavailable: ptrace'},
        'independent_review': 'passed scoped diagnostic; no blocking findings; exact recipes reproduced',
        'inherited_parser_warnings': 'retained in build-validation; no inherited source repair',
        'limitations': ['No original materials, textures, lighting, visibility or animation are applied.',
                       'Explicit diagnostic Pod placement is not a race grid or physics pose.',
                       'No game-loop, native-renderer parity, FPS or Xbox budget claim.',
                       'Offline exporter and external renderer allocate; unchanged camera hot path does not.',
                       'Separate49C work excluded and unreleased; collision/action-reset contracts unresolved.'],
        'images': [row for frame in frames for row in frame['images']],
        'source_hashes': source_hashes,
    }
    (EVIDENCE / 'validation.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'status': 'passed', 'baseline_members': len(old_names),
                      'changed_baseline_files': changes, 'new_or_changed_files_hashed': len(source_hashes)}))


if __name__ == '__main__':
    main()
