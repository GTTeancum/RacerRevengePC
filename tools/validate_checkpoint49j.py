#!/usr/bin/env python3
"""Freeze verified 49J clock sources, word proofs, connected traces and baseline."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/clockQuantization49J'
BASELINE_SHA = '1f4f28ad598bd8569eeba4b9e572c05e521b2bf981a7e38d87cf9c169317da0c'
ALLOWED = {'RacerRevengePC/CMakeLists.txt', 'RacerRevengePC/README.md'}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    if not __debug__:
        raise RuntimeError('Validation requires assertions; do not use Python -O')
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--baseline', type=Path, required=True)
    args = ap.parse_args()
    assert digest(args.baseline) == BASELINE_SHA
    changes = []
    with zipfile.ZipFile(args.baseline) as old:
        old_names = {i.filename for i in old.infolist() if not i.is_dir()}
        for name in sorted(old_names):
            p = ROOT.parent / name
            assert p.is_file(), name
            if digest(p) != hashlib.sha256(old.read(name)).hexdigest(): changes.append(name)
    assert set(changes) == ALLOWED, changes
    load = lambda name: json.loads((E / name).read_text())
    for script, count_key, count in [('conversion-source/extract_conversion49j.py', 'instruction_words', 772),
                                    ('sampler-source/extract_sampler_words49j.py', 'words', 37)]:
        r = subprocess.run([sys.executable, '-B', str(E / script), '--check'], cwd=ROOT,
                           capture_output=True, text=True)
        assert r.returncode == 0, r.stderr
        j = json.loads(r.stdout)
        assert j['status'] == 'verified' and j[count_key] == count
    for name in ('conversion-artifact-hashes.json', 'sampler-artifact-hashes.json'):
        for relative, row in load(name)['files'].items():
            p = E / relative
            assert p.stat().st_size == row['bytes'] and digest(p) == row['sha256'], relative
    review = load('independent-review.json')
    assert review['status'] == 'passed' and not review['blocking_findings']
    for name, sha in review['frozen_sources_sha256'].items(): assert digest(ROOT / name) == sha, name
    mutations = review['tests']['safe_scalar_state_mutations']
    assert mutations['count'] == mutations['detected'] == 18
    packaging = load('packaging-review.json')
    assert packaging['status'] == 'passed' and not packaging['blocking_findings']
    for name, sha in packaging['reviewed_file_hashes'].items(): assert digest(ROOT / name) == sha, name
    for name, cases, instructions in [('quantization-proof-results.json', 940635, 1778057976),
                                      ('sampler-pipeline-proof-results.json', 13739, 26479135)]:
        j = load(name)
        assert j['status'] == 'passed' and j['cases'] == cases and j['model_mismatches'] == 0
        assert j['instructions'] == instructions
    for name, cases in [('quantization-proof-sanitized-results.json', 28835),
                         ('sampler-pipeline-proof-sanitized-results.json', 13739)]:
        j = load(name)
        assert j['status'] == 'passed' and j['cases'] == cases and j['model_mismatches'] == 0
    regression = load('regression-summary.json')
    assert regression['status'] == 'passed' and regression['all_mode_outputs_identical']
    for mode in ('normal', 'sanitized'):
        row = regression['runs'][mode]
        assert row['suites'] == 13 and row['checks'] == 9193802 and row['tracked_inputs'] == 97
        assert row['inputs_unchanged']
        assert digest(E / f'final-{mode}/regression-report.json') == row['report_sha256']
        report = load(f'final-{mode}/regression-report.json')
        assert report['status'] == 'passed' and not report['errors'] and not report['changed_inputs']
        for name, record in report['input_hashes'].items(): assert digest(ROOT / name) == record['sha256'], name
        for test in row['tests']:
            assert digest(E / f'final-{mode}' / (test['name']+'.stdout')) == test['stdout_sha256']
            assert test['stderr_bytes'] == 0
        coverage = load(f'final-{mode}/clock-coverage.json')
        assert coverage['status'] == 'passed' and coverage['frozen_original_words'] == 809
        assert coverage['visited_original_words'] == 682 and coverage['hot_call_allocations'] == 0
        assert coverage['first_saturated_counter'] == 2473901031424
        for suffix, key in [('trace.tsv', 'connected_trace_sha256'), ('summary.json', 'connected_summary_sha256')]:
            p = E / f'final-{mode}/connected.{suffix}'
            assert digest(p) == row[key]
            assert p.read_bytes() == (E / f'connected/native.{suffix}').read_bytes()
    assert (E / 'final-normal/clock-coverage.json').read_bytes() == (E / 'final-sanitized/clock-coverage.json').read_bytes()
    connected = load('connected/native.summary.json')
    assert len(connected['scenarios']) == 13 and sum(r['frames'] for r in connected['scenarios']) == 915
    assert connected['repeatable'] and connected['native_hot_call_allocations'] == 0
    footprint = load('footprint/summary.json')
    assert footprint['source_sha256'] == digest(ROOT / 'src/game/clock_quantization49j.cpp')
    assert footprint['object_text_bytes'] == 1234 and footprint['data_bytes'] == footprint['bss_bytes'] == 0
    hashes = {}
    for p in sorted(ROOT.rglob('*')):
        if not p.is_file() or p == E / 'validation.json': continue
        name = p.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/'+name not in old_names or 'RacerRevengePC/'+name in ALLOWED: hashes[name] = digest(p)
    result = {
        'checkpoint': '49J-ClockQuantization', 'status': 'passed',
        'validated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'baseline': {'archive': args.baseline.name, 'sha256': BASELINE_SHA,
                     'bytes': args.baseline.stat().st_size, 'members_preserved': len(old_names),
                     'changed_baseline_files': changes,
                     'library_file_id': 'libfile_1065980bac8881919c17da093d43c79c'},
        'scope': 'Supplied timer count/pending word -> original numeric accumulator/quantization ->49I elapsed preparation; no OS/hardware acquisition, IRQ atomicity or opaque callbacks.',
        'normal_and_asan_ubsan': {'suites_each': 13, 'checks_each': 9193802,
                                'tracked_inputs_unchanged_each': 97,
                                'producer_noarg_checks': 856514, 'connected_no_output_checks': 275578,
                                'zero_hot_allocations': True, 'leak_check': 'unavailable under ptrace; disabled'},
        'word_proof': {'frozen': 809, 'reachable_visited': 682, 'classified_exclusions': 127,
                       'full_quantization_cases': 940635, 'full_quantization_instructions': 1778057976,
                       'sampler_cases': 13739, 'sampler_instructions': 26479135},
        'connected': {'scenarios': 13, 'paired_frames': 915, 'rows': 915, 'columns': 181,
                      'normal_sanitized_traces_identical': True,
                      'trace_sha256': digest(E/'final-normal/connected.trace.tsv')},
        'independent_mutations': {'meaningful_wrong_variants': 18, 'detected': 18},
        'first_saturated_counter': 2473901031424,
        'source_counterexample': {'counter': 9007200865353727, 'original_float_bits': '5a000002',
                                  'direct_RNE_bits': '5a000001', 'final_timestamp_already_saturated': True},
        'remaining_boundaries': ['Actual COUNT/overflow acquisition and IRQ interleavings/atomicity',
                                 'Live manager/world/platform effects, scene/mode scheduling and complete gameplay',
                                 'Universal PS2 silicon/FPU flags/cycles and native Xbox/full-game memory/FPS',
                                 '49C remains excluded/unreleased; collision/action-reset contracts unresolved'],
        'source_hashes': hashes,
    }
    (E/'validation.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({'status':'passed','baseline_members':len(old_names),
                      'changed_baseline_files':changes,'new_or_changed_files_hashed':len(hashes)}))


if __name__ == '__main__': main()
