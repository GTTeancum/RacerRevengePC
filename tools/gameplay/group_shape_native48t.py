#!/usr/bin/env python3
"""Bounded serial48T native differential/sanitizer/sensitivity evidence.

Build products stay outside the source tree. Evidence retains input hashes,
commands, timings, reports, stack usage, mutant source and normal failure logs.
This runner never starts the game, downloads, installs, or reads a RAM image.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import resource
import shutil
import signal
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[2]
SOURCES = [
    'tests/native/group_shape48t.cpp',
    'tests/native/group_shape_fixture48t.h',
    'tests/native/candidate_collection48t.cpp',
    'tests/native/candidate_collection_fixture48t.h',
    'tests/native/candidate_collection_fixture48s.h',
    'src/game/group_shape48t.cpp', 'src/game/group_shape48t.h',
    'src/game/candidate_collection48s.cpp', 'src/game/candidate_collection48s.h',
    'src/game/collection_bounds48s.cpp', 'src/game/collection_bounds48s.h',
    'evidence/groupShape48T/helper-fixtures.json',
    'evidence/groupShape48T/collector-fixtures.json',
    'evidence/candidateCollection48S/native-fixtures.json',
]
FLAGS = ['-std=c++17', '-O2', '-ffp-contract=off', '-fno-fast-math',
         '-frounding-math', '-Wall', '-Wextra', '-Werror', '-pedantic',
         '-fstack-usage', '-I', str(ROOT / 'src/game'),
         '-I', str(ROOT / 'third_party/native-json')]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', default='evidence/groupShape48T/native-final')
    parser.add_argument('--helper-sha', required=True)
    parser.add_argument('--collector-sha', required=True)
    parser.add_argument('--skip-sanitizer', action='store_true')
    parser.add_argument('--skip-mutants', action='store_true')
    args = parser.parse_args()
    os.chdir(ROOT)
    if not hasattr(os, 'sched_getaffinity'):
        raise SystemExit('This serial measurement runner requires Linux CPU affinity')
    os.sched_setaffinity(0, {min(os.sched_getaffinity(0))})
    out = ROOT / args.output
    out.mkdir(parents=True, exist_ok=False)
    build = Path(tempfile.mkdtemp(prefix='rr48t-native-'))
    assert ROOT not in build.parents, 'Build products must be outside source tree'
    hashes = {p: sha(ROOT / p) for p in SOURCES}
    fixture = {'helper': ROOT / 'evidence/groupShape48T/helper-fixtures.json',
               'collector': ROOT / 'evidence/groupShape48T/collector-fixtures.json'}
    assert sha(fixture['helper']) == args.helper_sha, 'Helper proof fingerprint changed'
    assert sha(fixture['collector']) == args.collector_sha, 'Collector proof fingerprint changed'
    (out / 'input-hashes.json').write_text(json.dumps(hashes, indent=2) + '\n')
    jobs = []

    def run(name, argv, expected=0, env=None):
        started = time.monotonic()
        command = list(map(str, argv))
        with (out / (name + '.stdout')).open('w') as stdout, (out / (name + '.stderr')).open('w') as stderr:
            process = subprocess.Popen(command, stdout=stdout, stderr=stderr, env=env, start_new_session=True)
            while True:
                pid, status, usage = os.wait4(process.pid, os.WNOHANG)
                if pid:
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                if time.monotonic() - started > 180:
                    os.killpg(process.pid, signal.SIGKILL)
                    _, status, usage = os.wait4(process.pid, 0)
                    process.returncode = os.waitstatus_to_exitcode(status)
                    raise RuntimeError(name + ': exceeded180second command bound')
                time.sleep(.05)
        measured = dict(wall_seconds=time.monotonic()-started,
                        max_rss_kib=usage.ru_maxrss,
                        user_seconds=usage.ru_utime, system_seconds=usage.ru_stime)
        (out / (name + '.time.json')).write_text(json.dumps(measured, indent=2) + '\n')
        jobs.append(dict(name=name, argv=command, returncode=process.returncode, resource=measured))
        (out / 'jobs.json').write_text(json.dumps(jobs, indent=2) + '\n')
        if process.returncode != expected:
            detail = (out / (name + '.stderr')).read_text()[:1500]
            raise RuntimeError(f'{name}: return{process.returncode}, expected{expected}: {detail}')

    def compile_one(name, source, flags):
        obj = build / (name + '.o')
        run(name + '-compile', ['g++', *flags, '-c', source, '-o', obj])
        stack = obj.with_suffix('.su')
        if stack.exists():
            shutil.copyfile(stack, out / stack.name)
        return obj

    def link(name, objects, flags):
        binary = build / name
        run(name + '-link', ['g++', *flags, *objects, '-o', binary])
        return binary

    run('compiler-version', ['g++', '--version'])
    runtime = dict(shape=ROOT / 'src/game/group_shape48t.cpp',
                   collection=ROOT / 'src/game/candidate_collection48s.cpp',
                   bounds=ROOT / 'src/game/collection_bounds48s.cpp')
    tests = dict(helper=ROOT / 'tests/native/group_shape48t.cpp',
                 collector=ROOT / 'tests/native/candidate_collection48t.cpp')
    release_objects = {name: compile_one('release-' + name, source, FLAGS)
                       for name, source in {**runtime, **tests}.items()}
    reports = {}
    for test in tests:
        used = [release_objects[test], release_objects['shape']]
        if test == 'collector':
            used += [release_objects['collection'], release_objects['bounds']]
        binary = link('release-' + test, used, FLAGS)
        report = out / ('release-' + test + '-report.json')
        run('release-' + test + '-run', [binary, fixture[test], report])
        reports[test] = json.loads(report.read_text())
        assert reports[test]['failures'] == reports[test]['observed_allocations'] == 0

    sanitizer_reports = {}
    if not args.skip_sanitizer:
        sanflags = [x for x in FLAGS if x != '-O2'] + ['-O1', '-g', '-fno-omit-frame-pointer', '-fsanitize=address,undefined']
        sanobjects = {name: compile_one('sanitized-' + name, source, sanflags)
                      for name, source in {**runtime, **tests}.items()}
        env = os.environ.copy()
        env['ASAN_OPTIONS'] = 'detect_leaks=0:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
        for test in tests:
            used = [sanobjects[test], sanobjects['shape']]
            if test == 'collector':
                used += [sanobjects['collection'], sanobjects['bounds']]
            binary = link('sanitized-' + test, used, sanflags)
            report = out / ('sanitized-' + test + '-report.json')
            run('sanitized-' + test + '-run-no-lsan', [binary, fixture[test], report], env=env)
            sanitizer_reports[test] = json.loads(report.read_text())
            assert sanitizer_reports[test]['failures'] == sanitizer_reports[test]['observed_allocations'] == 0

    mutants = []
    if not args.skip_mutants:
        edits = [
            ('previous-copies-old-translation', 'shape', 'helper',
             'std::memcpy(&result.previousC0,&result.world80,sizeof(result.previousC0));',
             'std::memcpy(&result.previousC0,&initial.world80,sizeof(result.previousC0));'),
            ('translation-w-forced-one', 'shape', 'helper',
             'std::memcpy(&result.previousC0,&result.world80,sizeof(result.previousC0));',
             'result.world80.v[15]=1.f;std::memcpy(&result.previousC0,&result.world80,sizeof(result.previousC0));'),
            ('composition-drops-fourth-term', 'shape', 'helper',
             'result.composed00.v[4*column+lane]=math.add(value,math.mul(result.world80.v[12+lane],source.local40.v[4*column+3]));',
             'result.composed00.v[4*column+lane]=value;'),
            ('composition-w-lanes-zero', 'shape', 'helper',
             'result.composed00.v[4*column+lane]=math.add(value,math.mul(result.world80.v[12+lane],source.local40.v[4*column+3]));',
             'result.composed00.v[4*column+lane]=lane==3?0.f:math.add(value,math.mul(result.world80.v[12+lane],source.local40.v[4*column+3]));'),
            ('center-uses-composed-matrix', 'shape', 'helper',
             'float value=math.mul(result.world80.v[axis],geometry.localCenter.v[0]);',
             'float value=math.mul(result.composed00.v[axis],geometry.localCenter.v[0]);'),
            ('sphere-w-not-one', 'shape', 'helper',
             'resultSphere.center.v[3]=1.f;', 'resultSphere.center.v[3]=result.world80.v[15];'),
            ('radius-scaled', 'shape', 'helper',
             'resultSphere.radius=math.root(math.add(xy,math.mul(geometry.localHalf.v[2],geometry.localHalf.v[2])));',
             'resultSphere.radius=math.mul(2.f,math.root(math.add(xy,math.mul(geometry.localHalf.v[2],geometry.localHalf.v[2]))));'),
            ('center-association-changed', 'shape', 'helper',
             'value=math.add(value,math.mul(result.world80.v[8+axis],geometry.localCenter.v[2]));\n        resultSphere.center.v[axis]=math.add(value,result.world80.v[12+axis]);',
             'resultSphere.center.v[axis]=math.add(value,math.add(math.mul(result.world80.v[8+axis],geometry.localCenter.v[2]),result.world80.v[12+axis]));'),
            ('member-sphere-not-written', 'collection', 'collector',
             'b.shape0States[def.shape0Ref-1]=shape;s.cachedCenter=sphere.center;s.cachedRadius=sphere.radius;',
             'b.shape0States[def.shape0Ref-1]=shape;'),
            ('shared-shape-not-updated', 'collection', 'collector',
             'b.shape0States[def.shape0Ref-1]=shape;s.cachedCenter=sphere.center;s.cachedRadius=sphere.radius;',
             's.cachedCenter=sphere.center;s.cachedRadius=sphere.radius;'),
            ('shape-destination-not-committed', 'collection', 'collector',
             'copy(destination.shape0States,b.shape0States,in.shape0Count);', ''),
            ('query-shape-authority-overridden', 'collection', 'collector',
             'const auto shapeTag=member==in.query?in.bounds.shapeTag:def.shapeTag;',
             'const auto shapeTag=def.shapeTag;'),
            ('pool-shared-tail-guard-removed', 'collection', 'collector',
             'if(last&&last==unusedLast)return fail(Status::InvalidTopology);',
             'if(false&&last&&last==unusedLast)return fail(Status::InvalidTopology);'),
        ]
        for name, unit, test, before, after in edits:
            original = runtime[unit].read_text()
            assert original.count(before) == 1, (name, original.count(before))
            source = out / ('mutant-' + name + '.cpp')
            source.write_text(original.replace(before, after, 1))
            obj = compile_one('mutant-' + name, source, FLAGS)
            objects = [release_objects[test], obj]
            if unit != 'shape':
                objects.append(release_objects['shape'])
            if test == 'collector':
                objects.append(release_objects['bounds'])
                if unit != 'collection':
                    objects.append(release_objects['collection'])
            binary = link('mutant-' + name, objects, FLAGS)
            report = out / ('mutant-' + name + '-report.json')
            # Full normal assertions, including semantic safety tests; a crash,
            # harness exception or compiler failure is never a killed mutant.
            run('mutant-' + name + '-run', [binary, fixture[test], report], expected=1)
            result = json.loads(report.read_text())
            assert result['failures'] > 0 and result['observed_allocations'] == 0
            mutants.append(dict(name=name, source_sha256=sha(source), failures=result['failures'], normal_exit_code=1))

    assert hashes == {p: sha(ROOT / p) for p in SOURCES}, 'Inputs changed during serial run'
    summary = dict(input_hashes=hashes, cpu_affinity=sorted(os.sched_getaffinity(0)), parallelism=1,
                   release=reports, sanitized=sanitizer_reports, mutants=mutants,
                   sanitizer='skipped' if args.skip_sanitizer else 'ASan+UBSan passed; LeakSanitizer explicitly disabled (unsupported executor ptrace route)',
                   max_child_rss_kib=resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss,
                   runner_sha256=sha(Path(__file__)), all_passed=True,
                   note='Synthetic bounded serial native Linux evidence only; no game/Xbox FPS, live PC, RAM or install claim')
    (out / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    files = {str(p.relative_to(out)): sha(p) for p in sorted(out.iterdir()) if p.is_file() and p.name != 'file-hashes.json'}
    (out / 'file-hashes.json').write_text(json.dumps(files, indent=2) + '\n')
    print(json.dumps(dict(output=str(out.relative_to(ROOT)), checks=sum(r['checks'] for r in reports.values()), mutants=len(mutants), passed=True)))


if __name__ == '__main__':
    main()
