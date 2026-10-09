#!/usr/bin/env python3
"""Supplemental hint-validation negative controls; final2 is read-only input.

Fresh serial builds establish exact current source/object provenance. The final2
suite and its13 controls are left unchanged, and these results are counted
separately. No existing temporary object is trusted or reused.
"""
import argparse
import json
import os
from pathlib import Path
import resource
import shutil
import signal
import subprocess
import sys
import tempfile
import time

sys.dont_write_bytecode = True
from group_shape_native48t import FLAGS, ROOT, sha


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--baseline', default='evidence/groupShape48T/native-final2')
    parser.add_argument('--output', default='evidence/groupShape48T/hint-sensitivity')
    args = parser.parse_args()
    os.chdir(ROOT)
    os.sched_setaffinity(0, {min(os.sched_getaffinity(0))})
    baseline = ROOT / args.baseline
    frozen_files = {p.name: sha(p) for p in baseline.iterdir() if p.is_file()}
    expected = json.loads((baseline / 'input-hashes.json').read_text())
    prior = json.loads((baseline / 'summary.json').read_text())
    assert prior['all_passed'] and len(prior['mutants']) == 13
    assert sha(ROOT / 'tools/gameplay/group_shape_native48t.py') == prior['runner_sha256']
    assert expected == {p: sha(ROOT / p) for p in expected}, 'Final2 source/fixture hashes changed'
    out = ROOT / args.output
    out.mkdir(parents=True, exist_ok=False)
    build = Path(tempfile.mkdtemp(prefix='rr48t-hint-sensitivity-'))
    assert ROOT not in build.parents
    (out / 'input-hashes.json').write_text(json.dumps(expected, indent=2) + '\n')
    (out / 'baseline-file-hashes.json').write_text(json.dumps(frozen_files, indent=2) + '\n')
    jobs, objects = [], []

    def run(name, command, wanted=0):
        command = list(map(str, command))
        start = time.monotonic()
        with (out / (name + '.stdout')).open('w') as stdout, (out / (name + '.stderr')).open('w') as stderr:
            process = subprocess.Popen(command, stdout=stdout, stderr=stderr, start_new_session=True)
            while True:
                pid, status, usage = os.wait4(process.pid, os.WNOHANG)
                if pid:
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                if time.monotonic() - start > 180:
                    os.killpg(process.pid, signal.SIGKILL)
                    os.wait4(process.pid, 0)
                    raise RuntimeError(name + ': command bound exceeded')
                time.sleep(.05)
        measurement = dict(wall_seconds=time.monotonic()-start, max_rss_kib=usage.ru_maxrss,
                           user_seconds=usage.ru_utime, system_seconds=usage.ru_stime)
        job = dict(name=name, argv=command, returncode=process.returncode, resource=measurement)
        jobs.append(job)
        (out / 'jobs.json').write_text(json.dumps(jobs, indent=2) + '\n')
        (out / (name + '.time.json')).write_text(json.dumps(measurement, indent=2) + '\n')
        if process.returncode != wanted:
            raise RuntimeError(f'{name}: got{process.returncode}, expected{wanted}: ' + (out / (name + '.stderr')).read_text()[:1200])

    def compile_one(name, source):
        obj = build / (name + '.o')
        source_hash = sha(source)
        run(name + '-compile', ['g++', *FLAGS, '-c', source, '-o', obj])
        assert sha(source) == source_hash
        objects.append(dict(name=name, source=str(source.relative_to(ROOT)), source_sha256=source_hash,
                            object_path=str(obj), object_sha256=sha(obj), reused=False))
        (out / 'object-provenance.json').write_text(json.dumps(objects, indent=2) + '\n')
        stack = obj.with_suffix('.su')
        if stack.exists():
            shutil.copyfile(stack, out / stack.name)
        return obj

    def test(name, collection_obj, wanted):
        binary = build / name
        run(name + '-link', ['g++', *FLAGS, test_obj, shape_obj, bounds_obj, collection_obj, '-o', binary])
        report = out / (name + '-report.json')
        run(name + '-run', [binary, ROOT / 'evidence/groupShape48T/collector-fixtures.json', report], wanted)
        result = json.loads(report.read_text())
        assert result['observed_allocations'] == 0
        if wanted:
            assert result['failures'] > 0, 'Normal assertion failures are required'
        else:
            assert result['failures'] == 0
            assert result['checks'] == prior['release']['collector']['checks']
        return result

    run('compiler-version', ['g++', '--version'])
    test_obj = compile_one('collector-test', ROOT / 'tests/native/candidate_collection48t.cpp')
    shape_obj = compile_one('shape-runtime', ROOT / 'src/game/group_shape48t.cpp')
    bounds_obj = compile_one('bounds-runtime', ROOT / 'src/game/collection_bounds48s.cpp')
    original_path = ROOT / 'src/game/candidate_collection48s.cpp'
    baseline_obj = compile_one('baseline-collection', original_path)
    baseline_result = test('baseline', baseline_obj, 0)
    edits = [
        ('identity-strict-order-removed',
         'if(identity<=previous)return Status::InvalidInput;',
         'if(false&&identity<=previous)return Status::InvalidInput;'),
        ('block-strict-order-removed',
         'if(start<previousStart||(start==previousStart&&r<=previousRef))return Status::InvalidInput;',
         'if(false&&(start<previousStart||(start==previousStart&&r<=previousRef)))return Status::InvalidInput;'),
        ('hinted-interval-disjointness-removed',
         'if(begin<previousEnd)return Status::InvalidInput;',
         'if(false&&begin<previousEnd)return Status::InvalidInput;'),
    ]
    results = []
    original = original_path.read_text()
    for name, before, after in edits:
        assert original.count(before) == 1
        source = out / ('mutant-' + name + '.cpp')
        source.write_text(original.replace(before, after, 1))
        obj = compile_one('mutant-' + name, source)
        result = test('mutant-' + name, obj, 1)
        results.append(dict(name=name, failures=result['failures'], normal_exit_code=1,
                            observed_allocations=result['observed_allocations'], source_sha256=sha(source)))

    assert expected == {p: sha(ROOT / p) for p in expected}, 'Current final2 inputs changed'
    assert frozen_files == {p.name: sha(p) for p in baseline.iterdir() if p.is_file()}, 'Final2 evidence changed'
    summary = dict(all_passed=True, baseline_summary_sha256=frozen_files['summary.json'],
                   baseline_inputs_unchanged=True, baseline_evidence_unchanged=True,
                   baseline_original_mutant_count=13, supplemental_mutant_count=len(results),
                   baseline=baseline_result, supplemental_mutants=results,
                   object_provenance=objects, reused_objects=False,
                   max_child_rss_kib=resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss,
                   cpu_affinity=sorted(os.sched_getaffinity(0)), parallelism=1,
                   runner_sha256=sha(Path(__file__)),
                   scope='Additional normal-assertion sensitivity for exact current validation hints; no runtime edits or new sanitizer claim')
    (out / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    hashes = {p.name: sha(p) for p in out.iterdir() if p.is_file() and p.name != 'file-hashes.json'}
    (out / 'file-hashes.json').write_text(json.dumps(hashes, indent=2) + '\n')
    print(json.dumps(dict(output=str(out.relative_to(ROOT)), supplemental_mutants=len(results), passed=True)))


if __name__ == '__main__':
    main()
