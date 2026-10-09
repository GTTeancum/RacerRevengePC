#!/usr/bin/env python3
"""Freeze verified 49I timing/state sources and preserved baseline evidence."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/frameTiming49I'
BASELINE_SHA = '9f4bb6720cd4a73abd80a81b6ce7d1c424f06aa15eea6244d8537db030eec04c'
ALLOWED = {'RacerRevengePC/CMakeLists.txt', 'RacerRevengePC/README.md'}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    if not __debug__:
        raise RuntimeError('Validation requires assertions; do not use Python -O')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    args = parser.parse_args()
    assert digest(args.baseline) == BASELINE_SHA
    changed = []
    with zipfile.ZipFile(args.baseline) as archive:
        old_names = {i.filename for i in archive.infolist() if not i.is_dir()}
        for name in sorted(old_names):
            path = ROOT.parent / name
            assert path.is_file(), ('Missing inherited file', name)
            if hashlib.sha256(archive.read(name)).hexdigest() != digest(path):
                changed.append(name)
    assert set(changed) == ALLOWED, changed
    load = lambda name: json.loads((E / name).read_text())
    check = subprocess.run([sys.executable, '-B', 'tools/gameplay/frame_timing_recover49i.py', '--check'],
                           cwd=ROOT, capture_output=True, text=True)
    assert check.returncode == 0, check.stderr
    source_check = json.loads(check.stdout)
    assert source_check['status'] == 'verified' and source_check['selected_words'] == 69
    assert source_check['unreachable_selected_words'] == 1 and not source_check['context_executed']
    review = load('independent-review-final.json')
    assert review['status'] == 'passed' and review['source_unchanged_during_review']
    for name, sha in review['input_sha256'].items():
        assert digest(ROOT / name) == sha, name
    assert review['summary'] == {'meaningful_wrong_variants': 28, 'meaningful_wrong_variants_detected': 28,
                                 'equivalent_variants_passed': 4, 'all_expectations_met': True}
    assert all(row['expectation_met'] for row in review['mutations'])
    frozen = review['frozen_initial_review']
    assert frozen['preserved_unchanged']
    assert digest(ROOT / frozen['report']) == frozen['report_sha256']
    assert digest(ROOT / frozen['helper']) == frozen['helper_sha256']
    packaging = load('packaging-review.json')
    assert packaging['status'] == 'passed' and not packaging['blocking_findings']
    for name, sha in packaging['reviewed_file_hashes'].items():
        assert digest(ROOT / name) == sha, name
    regression = load('regression-summary.json')
    assert regression['status'] == 'passed' and regression['test_stdout_and_coverage_identical']
    for mode in ('normal', 'sanitized'):
        run = regression['runs'][mode]
        assert run['checks'] == 8061710 and run['suites'] == 11
        assert run['tracked_inputs'] == 85 and run['input_hashes_unchanged']
        assert digest(E / f'final-{mode}/regression-report.json') == run['report_sha256']
        assert digest(E / f'final-{mode}/original-word-coverage.json') == run['coverage_sha256']
        report = load(f'final-{mode}/regression-report.json')
        assert report['status'] == 'passed' and not report['errors'] and not report['changed_inputs']
        for name, row in report['input_hashes'].items():
            assert digest(ROOT / name) == row['sha256'], name
        for test in run['tests']:
            assert digest(E / f'final-{mode}' / (test['name'] + '.stdout')) == test['stdout_sha256']
            assert test['stderr_bytes'] == 0
    normal_coverage = load('final-normal/original-word-coverage.json')
    assert normal_coverage == load('final-sanitized/original-word-coverage.json')
    assert normal_coverage['selected_words'] == 69 and normal_coverage['executed_words'] == 68
    assert normal_coverage['reachable_words'] == 68 and normal_coverage['all_decision_arms_covered']
    assert normal_coverage['context_words_executed'] == 0
    assert normal_coverage['unreachable_duplicate_shift'] == '0x140c04'
    assert sum(row['executions'] for row in normal_coverage['words']) == 1472881
    unexecuted = [row['pc'] for row in normal_coverage['words'] if not row['executions']]
    assert unexecuted == ['0x140c04']
    footprint = load('footprint/summary.json')
    assert footprint['source_sha256'] == digest(ROOT / 'src/game/frame_timing49i.cpp')
    assert (footprint['state_bytes'], footprint['output_bytes'], footprint['gnu_size_text_bytes']) == (28, 8, 569)
    source_hashes = {}
    for path in sorted(ROOT.rglob('*')):
        if not path.is_file() or path == E / 'validation.json':
            continue
        name = path.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/' + name not in old_names or 'RacerRevengePC/' + name in ALLOWED:
            source_hashes[name] = digest(path)
    result = {
        'checkpoint': '49I-FrameTiming', 'status': 'passed',
        'validated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'baseline': {'archive': args.baseline.name, 'sha256': BASELINE_SHA,
                     'bytes': args.baseline.stat().st_size, 'members_preserved': len(old_names),
                     'changed_baseline_files': changed,
                     'library_file_id': 'libfile_76c472a4f5248191bc8298e3272b91a8'},
        'scope': 'Native supplied original quantized uint32 millisecond timestamp -> staged elapsed/state producer; no hardware/OS timer, opaque callback or full scheduler implementation.',
        'native_state_bytes': 28, 'elapsed_output_bytes': 8,
        'normal_and_asan_ubsan': {'suites_each': 11, 'checks_each': 8061710,
                                'new_checks_each': 727613, 'coverage_export_checks_each': 727616,
                                'tracked_inputs_unchanged_each': 85, 'zero_hot_allocations': True,
                                'leak_check': 'unavailable under ptrace; disabled'},
        'original_word_proof': {'selected_words': 69, 'reachable_executed': 68,
                               'instruction_executions': 1472881,
                               'conditional_branches_both_arms': 7,
                               'unexecuted_dead_duplicate': '0x140c04', 'context_execution': False,
                               'model': 'Independent integer-significand nearest-even COP1 steps; not universal R5900 silicon equivalence'},
        'connected_input': {'frames': 3855, 'original_input_instructions': 1042670},
        'staged_trajectory_frames': 8192,
        'independent_mutations': review['summary'],
        'source_preservation': 'All inherited files except CMakeLists.txt/README.md remain exact49H bytes.',
        'remaining_boundaries': ['Original timer MMIO/overflow sampling and unsigned64/soft-float timestamp provider',
                                 'Real input-manager, gated pre-dispatch, world and platform callback effects',
                                 'Live object eligibility/topology and live substep scale; no pause/focus or catch-up policy invented',
                                 '49C remains excluded/unreleased; collision/action-reset contract unresolved'],
        'source_hashes': source_hashes,
    }
    (E / 'validation.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'status': 'passed', 'baseline_members': len(old_names),
                      'changed_baseline_files': changed, 'new_or_changed_files_hashed': len(source_hashes)}))


if __name__ == '__main__':
    main()
