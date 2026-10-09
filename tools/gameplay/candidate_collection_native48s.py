#!/usr/bin/env python3
"""Serial, offline collector48S release/sanitizer/mutation evidence runner.

Only this48S test packet/output directory is written. It never runs the game,
installs software, pushes, uploads, accesses a PC or imports a RAM snapshot.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import resource
import signal
import subprocess
import time

ROOT = Path(__file__).resolve().parents[2]
SOURCES = [
    'tests/native/candidate_collection48s.cpp',
    'tests/native/candidate_collection_fixture48s.h',
    'src/game/candidate_collection48s.cpp',
    'src/game/candidate_collection48s.h',
    'src/game/group_shape48t.cpp',
    'src/game/group_shape48t.h',
    'src/game/collection_bounds48s.cpp',
    'src/game/collection_bounds48s.h',
    'evidence/candidateCollection48S/native-fixtures.json',
]
FLAGS = ['-std=c++17', '-O2', '-ffp-contract=off', '-fno-fast-math',
         '-Wall', '-Wextra', '-Werror', '-pedantic', '-fstack-usage',
         '-I', str(ROOT / 'src/game'), '-I', str(ROOT / 'third_party/native-json')]

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--output', default='evidence/candidateCollection48S/native-final')
    ap.add_argument('--fixture-sha', required=True)
    ap.add_argument('--skip-sanitizer', action='store_true')
    ap.add_argument('--skip-mutants', action='store_true')
    ap.add_argument('--resume', action='store_true', help='Reuse already successful exact commands with unchanged input hashes')
    args = ap.parse_args()
    os.chdir(ROOT)
    if hasattr(os, 'sched_getaffinity'):
        affinity = sorted(os.sched_getaffinity(0))
        os.sched_setaffinity(0, {affinity[0]})
    else:
        raise SystemExit('This bounded runner requires Linux CPU affinity support')
    out = ROOT / args.output
    out.mkdir(parents=True, exist_ok=args.resume)
    hashes = {p: sha(ROOT / p) for p in SOURCES}
    fixture = ROOT / SOURCES[-1]
    assert hashes[SOURCES[-1]] == args.fixture_sha, 'Unexpected source-executed fixture fingerprint'
    if args.resume:
        assert json.loads((out / 'input-hashes.json').read_text()) == hashes, 'Resume inputs changed'
    (out / 'input-hashes.json').write_text(json.dumps(hashes, indent=2) + '\n')
    jobs = json.loads((out / 'jobs.json').read_text()) if args.resume else []
    def run(name, argv, expected=0, env=None):
        if args.resume:
            prior = [j for j in jobs if j['name'] == name and j['argv'] == list(map(str, argv)) and j['returncode'] == expected]
            if prior:
                return prior[-1]
        started = time.monotonic()
        stdout, stderr = out / (name + '.stdout'), out / (name + '.stderr')
        timing = out / (name + '.time.json')
        command = list(map(str, argv))
        with stdout.open('w') as so, stderr.open('w') as se:
            process = subprocess.Popen(command, stdout=so, stderr=se, env=env, start_new_session=True)
            while True:
                pid, status, usage = os.wait4(process.pid, os.WNOHANG)
                if pid:
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                if time.monotonic()-started > 180:
                    os.killpg(process.pid, signal.SIGKILL)
                    _, status, usage = os.wait4(process.pid, 0)
                    process.returncode = os.waitstatus_to_exitcode(status)
                    raise RuntimeError(f'{name}: timed out after180seconds')
                time.sleep(.05)
        measured = dict(wall_seconds=time.monotonic()-started, max_rss_kib=usage.ru_maxrss,
                        user_seconds=usage.ru_utime, system_seconds=usage.ru_stime)
        timing.write_text(json.dumps(measured, indent=2)+'\n')
        job = dict(name=name, argv=list(map(str, argv)), returncode=process.returncode,
                   elapsed_seconds=time.monotonic()-started, resource=measured)
        jobs.append(job)
        (out / 'jobs.json').write_text(json.dumps(jobs, indent=2) + '\n')
        if process.returncode != expected:
            raise RuntimeError(f'{name}: return {process.returncode}, expected {expected}: {stderr.read_text()[:500]}')
        return job
    run('compiler-version', ['g++', '--version'])
    def build(kind, flags, runtime=None):
        paths = [('test', ROOT / 'tests/native/candidate_collection48s.cpp'),
                 ('bounds', ROOT / 'src/game/collection_bounds48s.cpp'),
                 ('group_shape', ROOT / 'src/game/group_shape48t.cpp'),
                 ('runtime', runtime or ROOT / 'src/game/candidate_collection48s.cpp')]
        objects = []
        for name, source in paths:
            obj = out / f'{kind}-{name}.o'
            run(f'{kind}-compile-{name}', ['g++', *flags, '-c', source, '-o', obj])
            objects.append(obj)
        binary = out / kind
        run(f'{kind}-link', ['g++', *flags, *objects, '-o', binary])
        return binary
    release = build('release', FLAGS)
    run('release-run', [release, fixture, out / 'release-report.json'])
    release_report = json.loads((out / 'release-report.json').read_text())
    assert release_report['failures'] == 0 and release_report['observed_allocations'] == 0
    if not args.skip_sanitizer:
        sanflags = [x for x in FLAGS if x != '-O2'] + ['-O1', '-g', '-fno-omit-frame-pointer', '-fsanitize=address,undefined']
        sanitizer = build('sanitized', sanflags)
        env = os.environ.copy()
        env['ASAN_OPTIONS'] = 'detect_leaks=0:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
        run('sanitized-run-no-lsan', [sanitizer, fixture, out / 'sanitized-report-no-lsan.json'], env=env)
        report = json.loads((out / 'sanitized-report-no-lsan.json').read_text())
        assert report['failures'] == 0 and report['observed_allocations'] == 0
    mutants = []
    if not args.skip_mutants:
        original = (ROOT / 'src/game/candidate_collection48s.cpp').read_text()
        edits = [
            ('capacity-before-next', 'while(status==Status::Complete){\n            const Ref o=next();', 'while(status==Status::Complete){\n            if(!(result.count<in.nominalCapacity))break;\n            const Ref o=next();'),
            ('group-emits-member', 'if(!append(object.identity))return false;', 'if(!append(def.identity))return false;'),
            ('group-inner-limit', 'for(std::int32_t j=0;j<g.memberCount;++j){', 'for(std::int32_t j=0;j<g.memberCount&&result.count<in.nominalCapacity;++j){'),
            ('stamp-dedup-disabled', 'if(state.epoch==s.epoch)continue;', 'if(false&&state.epoch==s.epoch)continue;'),
            ('cached-block-cleared', 's.currentHeadA8=s.savedHeadA4;s.slotAC=0;', 's.currentHeadA8=s.savedHeadA4;s.currentBlockB0=0;s.slotAC=0;'),
            ('children-reversed', 'const unsigned child=f.nextChild++;', 'const unsigned child=3u-f.nextChild++;'),
            ('direct-flags-not-set', 'result.currentIdentity=object.identity;b.objects[o-1].flagsF8=static_cast<std::uint8_t>(b.objects[o-1].flagsF8|0x10u);', 'result.currentIdentity=object.identity;'),
            ('group-flags-not-set', 'result.currentIdentity=def.identity;s.flagsF8=static_cast<std::uint8_t>(s.flagsF8|0x10u);', 'result.currentIdentity=def.identity;'),
            ('tree-epoch-not-incremented', 's.auxiliaryB4=0;++s.epoch;return true;', 's.auxiliaryB4=0;return true;'),
            ('strict-sphere-touch', 'return numeric(m)&&gap<=0.f;', 'return numeric(m)&&gap<0.f;'),
        ]
        for name, before, after in edits:
            assert original.count(before) == 1, (name, original.count(before))
            source = out / f'mutant-{name}.cpp'
            source.write_text(original.replace(before, after, 1))
            obj = out / f'mutant-{name}.o'
            binary = out / f'mutant-{name}'
            run(f'mutant-{name}-compile', ['g++', *FLAGS, '-c', source, '-o', obj])
            run(f'mutant-{name}-link', ['g++', *FLAGS, out / 'release-test.o', out / 'release-bounds.o', out / 'release-group_shape.o', obj, '-o', binary])
            run(f'mutant-{name}-run', [binary, fixture, out / f'mutant-{name}-report.json', '--differential-only'], expected=1)
            report = json.loads((out / f'mutant-{name}-report.json').read_text())
            assert report['failures'] > 0
            mutants.append(dict(name=name, failures=report['failures'], allocation_calls=report['observed_allocations'], source_sha256=sha(source)))
    assert hashes == {p: sha(ROOT / p) for p in SOURCES}, 'Inputs changed during serial proof run'
    summary = dict(input_hashes=hashes, cpu_affinity=sorted(os.sched_getaffinity(0)), parallelism=1,
                   release=release_report, mutants=mutants,
                   child_max_rss_kib=max([resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss]+[j['resource']['max_rss_kib'] for j in jobs]),
                   runner_sha256=sha(Path(__file__)),
                   sanitizer='skipped' if args.skip_sanitizer else 'ASan+UBSan passed; LeakSanitizer disabled because ptrace is unsupported in this executor' ,
                   all_passed=True, note='Native Linux synthetic microbenchmarks only; no game or Xbox FPS inference')
    (out / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    files = {str(p.relative_to(out)):sha(p) for p in sorted(out.iterdir()) if p.is_file() and p.name != 'file-hashes.json'}
    (out / 'file-hashes.json').write_text(json.dumps(files, indent=2) + '\n')
    print(json.dumps(dict(output=str(out.relative_to(ROOT)), checks=release_report['checks'], mutants=len(mutants), passed=True)))

if __name__ == '__main__':
    main()
