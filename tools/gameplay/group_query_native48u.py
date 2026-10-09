#!/usr/bin/env python3
"""Serial48U native original-fixture differential, sanitizer and semantic mutants.

All build products are outside the source tree. No game, PC, RAM, installation,
network, publication or native-generated expected observations are involved.
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
FIXTURE = 'evidence/groupQuery48U/native-fixtures.json'
RUNTIME = [
    'src/game/group_query48u.cpp', 'src/game/group_shape48t.cpp',
    'src/game/type3_query48p.cpp', 'src/game/type3_prepare48p.cpp',
    'src/game/collision_type3_leaf48p.cpp', 'src/game/collision_type3_node48p.cpp',
    'src/game/spring_type3_callback48p.cpp', 'src/game/spring_callback48n.cpp',
    'src/game/spring_records48n.cpp',
]
TEST = 'tests/native/group_query48u.cpp'
SOURCES = RUNTIME + [p.replace('.cpp', '.h') for p in RUNTIME] + [
    TEST, 'tests/native/group_query_fixture48u.h', 'src/game/type3_state48p.h',
    'src/game/type3_query_internal48u.h', 'src/game/scalar_minmax48o.h',
    'src/game/terrain_query48j.h', 'src/game/collision_tree48i.h',
    'src/game/body_scene48g.h', 'src/game/body_integration48c.h',
    'src/game/vehicle_contact48g.h', 'src/assets/collision_mesh48i.h',
    'third_party/native-json/json.hpp', FIXTURE,
]
FLAGS = ['-std=c++17', '-O2', '-ffp-contract=off', '-fno-fast-math',
         '-frounding-math', '-Wall', '-Wextra', '-Werror', '-pedantic',
         '-fstack-usage', '-I', str(ROOT / 'src/game'), '-I', str(ROOT / 'src'),
         '-I', str(ROOT / 'third_party/native-json')]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def mutations():
    # Every replacement is anchored exactly once in its named production unit.
    # A mutant counts only when the unchanged normal harness exits1 with failed
    # assertions. Compile errors, crashes, sanitizer failures and harness2 do not.
    path = 'src/game/group_query48u.cpp'
    edits = [
        ('group-ignore-bit0', '!(result.queryFlags&1u)&&c.typeTag==input.groupTypeTag', 'c.typeTag==input.groupTypeTag'),
        ('ordinary-tag-loses-precedence', 'if(c.typeTag==input.collisionTypeTag){', 'if(c.typeTag==input.collisionTypeTag&&c.typeTag!=input.groupTypeTag){'),
        ('group-coarse-equality-misses', 'if(!(gap<=0.f)||g.memberCount<=0)', 'if(!(gap<0.f)||g.memberCount<=0)'),
        ('group-coarse-includes-y', 'm.root(m.add(m.mul(dx,dx),m.mul(dz,dz)))', 'm.root(m.add(m.add(m.mul(dx,dx),m.mul(dz,dz)),m.mul(m.sub(result.state.queryCenter.v[1],m.mul(static_cast<float>(object.position[1]),groups.worldPositionScale)),m.sub(result.state.queryCenter.v[1],m.mul(static_cast<float>(object.position[1]),groups.worldPositionScale)))))'),
        ('group-single-member-skipped', 'if(!(gap<=0.f)||g.memberCount<=0)', 'if(!(gap<=0.f)||g.memberCount<=1)'),
        ('member-ordinary-filter-applied', 'current(r);return dispatch(r);', 'current(r);if(!detail::eligibleType3Borrowed48U(input.filter,candidate.collision.filter,b.objects[r-1].flagsF8))return Status::Complete;return dispatch(r);'),
        ('member-inactive-skips-setter', 'if(!object.hasShape)return Status::UnsupportedScope;', 'if(!object.hasShape)return Status::UnsupportedScope;\n        if(!(candidate.collision.filter.flags31&1u))return Status::Complete;'),
        ('member-position-not-owner', 'const auto& outer=groups.objects[owner-1];const auto& object=groups.objects[r-1];', 'const auto& outer=groups.objects[(owner? r:owner)-1];const auto& object=groups.objects[r-1];'),
        ('member-sphere-stale', 'b.shapes[index]=next;b.objects[r-1].center=bound.center;b.objects[r-1].radius=bound.radius;', 'b.shapes[index]=next;'),
        ('shared-shape-state-stale', 'b.shapes[index]=next;b.objects[r-1].center=bound.center;b.objects[r-1].radius=bound.radius;', 'b.objects[r-1].center=bound.center;b.objects[r-1].radius=bound.radius;'),
        ('shared-shape-refreshes-unvisited-spheres', 'b.shapes[index]=next;b.objects[r-1].center=bound.center;b.objects[r-1].radius=bound.radius;', 'b.shapes[index]=next;for(std::size_t k=0;k<groups.objectCount;++k)if(groups.objects[k].shape0==object.shape0){b.objects[k].center=bound.center;b.objects[k].radius=bound.radius;}'),
        ('shape45-scales-swapped', 'b.shapes45[object.shape45-1]={translation,scale18C,scale180};', 'b.shapes45[object.shape45-1]={translation,scale180,scale18C};'),
        ('shape45-translation-omitted', 'b.shapes45[object.shape45-1]={translation,scale18C,scale180};', 'b.shapes45[object.shape45-1].scale18C=scale18C;b.shapes45[object.shape45-1].scale180=scale180;'),
        ('dispatch-ignores-shared-matrix', 'mesh=groups.shapes[object.shape0-1].mesh;matrix=&b.shapes[object.shape0-1].composed00;', 'mesh=groups.shapes[object.shape0-1].mesh;'),
        ('dispatch-cached-sphere-ignored', 'candidate,sphere(state),mesh,*matrix,object.hasShape,', 'candidate,c.sphere,mesh,*matrix,object.hasShape,'),
        ('direct-cached-sphere-ignored', 'detail::outerSphereType3Borrowed48U(result.state,sphere(b.objects[r-1]),hit)', 'detail::outerSphereType3Borrowed48U(result.state,c.sphere,hit)'),
        ('reached-ratio-errors-ignored', 'if(!m.valid)return Status::InvalidInput;\n        if(candidate.shapeType==0){', 'if(candidate.shapeType==0){'),
        ('canonical45-tag-validation-ignored', 'if(groups.shape45Tags[object.shape45-1]!=candidate.shapeType)return Status::UnsupportedScope;', 'if(groups.shape45Tags[object.shape45-1]!=candidate.shapeType){}'),
        ('member-dispatch-skipped', 'current(r);return dispatch(r);', 'current(r);return Status::Complete;'),
        ('member-current-not-updated', 'result.currentCandidate=groups.objects[r-1].candidate->collision.filter.identity;', 'result.currentCandidate=input.initialQueryCurrent;'),
        ('member-copied-fields-not-updated', 'state.copied40=input.copied40;state.copied50=input.copied50;', 'state.copied40=c.copied40;state.copied50=c.copied50;'),
        ('query-self-flags-stale', 'q.flagsF8=result.queryFlags;q.copied40=input.copied40;q.copied50=input.copied50;', 'q.copied40=input.copied40;q.copied50=input.copied50;'),
        ('member-response-callback-ignored', 'if(c.hasResponseCallbackE8)return Status::UnsupportedScope;', 'if(c.hasResponseCallbackE8)return Status::Complete;'),
        ('callback-template-flag-overrides-identity', 'if(groups.queryCallback==GroupQueryCallback48U::None)return Status::Complete;', 'if(!input.callbackEnabled)return Status::Complete;'),
        ('callback-member-identity-replaced-query', 'context.bodyScene,context.bodyScenePresent,c.filter.identity,', 'context.bodyScene,context.bodyScenePresent,input.filter.identity,'),
        ('identity-hint-nonstrict', 'if(identity<=previous)return Status::InvalidInput;', 'if(identity<previous)return Status::InvalidInput;'),
        ('failed-query-commits-result', 'const auto status=run.run();if(status!=Status::Complete)return status;', 'const auto status=run.run();if(status!=Status::Complete){output=run.result;return status;}'),
        ('shape-destination-not-committed', 'copy(destination.shapes,b.shapes,groups.shapeCount);copy(destination.shapes45,b.shapes45,groups.shape45Count);', 'copy(destination.shapes45,b.shapes45,groups.shape45Count);'),
        ('shape45-destination-not-committed', 'copy(destination.shapes,b.shapes,groups.shapeCount);copy(destination.shapes45,b.shapes45,groups.shape45Count);', 'copy(destination.shapes,b.shapes,groups.shapeCount);'),
    ]
    return [(name, path, before, after) for name, before, after in edits]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--output', default='evidence/groupQuery48U/native-final')
    ap.add_argument('--fixture-sha', required=True)
    ap.add_argument('--skip-sanitizer', action='store_true')
    ap.add_argument('--skip-mutants', action='store_true')
    ap.add_argument('--resume', action='store_true', help='Reuse successful exact commands only with unchanged native inputs and surviving external build products')
    args = ap.parse_args()
    os.chdir(ROOT)
    if not hasattr(os, 'sched_getaffinity'):
        raise SystemExit('This bounded serial measurement runner requires Linux CPU affinity')
    os.sched_setaffinity(0, {min(os.sched_getaffinity(0))})
    out = ROOT / args.output
    out.mkdir(parents=True, exist_ok=args.resume)
    jobs = json.loads((out / 'jobs.json').read_text()) if args.resume else []
    if args.resume:
        compiles = [j for j in jobs if j['name'].endswith('-compile')]
        build = Path(compiles[0]['argv'][-1]).parent
        assert build.is_dir(), 'Prior external build workspace is unavailable'
    else:
        build = Path(tempfile.mkdtemp(prefix='rr48u-native-'))
    assert ROOT not in build.parents
    hashes = {p: sha(ROOT / p) for p in SOURCES}
    if args.resume:
        assert json.loads((out / 'input-hashes.json').read_text()) == hashes, 'Resume native input fingerprints changed'
    fixture = ROOT / FIXTURE
    assert hashes[FIXTURE] == args.fixture_sha, 'Original source-executed fixture fingerprint changed'
    (out / 'input-hashes.json').write_text(json.dumps(hashes, indent=2) + '\n')

    def run(name, argv, expected=0, env=None):
        start = time.monotonic()
        command = list(map(str, argv))
        if args.resume and any(j['name']==name and j['argv']==command and j['returncode']==expected for j in jobs):
            if ('-o' not in command or Path(command[command.index('-o')+1]).exists()) and (not name.endswith('-run') or Path(command[2]).exists()):
                return
        with (out / (name + '.stdout')).open('w') as stdout, (out / (name + '.stderr')).open('w') as stderr:
            process = subprocess.Popen(command, stdout=stdout, stderr=stderr, env=env, start_new_session=True)
            while True:
                pid, status, usage = os.wait4(process.pid, os.WNOHANG)
                if pid:
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                if time.monotonic() - start > 240:
                    os.killpg(process.pid, signal.SIGKILL)
                    _, status, usage = os.wait4(process.pid, 0)
                    process.returncode = os.waitstatus_to_exitcode(status)
                    raise RuntimeError(name + ': exceeded240second command bound')
                time.sleep(.05)
        measured = dict(wall_seconds=time.monotonic()-start, max_rss_kib=usage.ru_maxrss,
                        user_seconds=usage.ru_utime, system_seconds=usage.ru_stime)
        (out / (name + '.time.json')).write_text(json.dumps(measured, indent=2) + '\n')
        jobs.append(dict(name=name, argv=command, returncode=process.returncode, resource=measured))
        (out / 'jobs.json').write_text(json.dumps(jobs, indent=2) + '\n')
        if process.returncode != expected:
            raise RuntimeError(f'{name}: return{process.returncode}, expected{expected}: ' + (out / (name + '.stderr')).read_text()[:3000])

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

    def build_all(name, flags):
        return {p: compile_one(name+'-'+('test-' if p==TEST else 'runtime-')+Path(p).stem, ROOT/p, flags) for p in RUNTIME+[TEST]}

    def execute(name, binary, env=None):
        report = out / (name + '-report.json')
        run(name + '-run', [binary, fixture, report], env=env)
        result = json.loads(report.read_text())
        assert result['failures'] == result['observed_allocations'] == 0
        return result

    run('compiler-version', ['g++', '--version'])
    objects = build_all('release', FLAGS)
    release = execute('release', link('release', list(objects.values()), FLAGS))
    sanitized = None
    if not args.skip_sanitizer:
        sanflags = [x for x in FLAGS if x != '-O2'] + ['-O1', '-g', '-fno-omit-frame-pointer', '-fsanitize=address,undefined']
        sanobjects = build_all('sanitized', sanflags)
        env = os.environ.copy()
        env['ASAN_OPTIONS'] = 'detect_leaks=0:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
        sanitized = execute('sanitized-no-lsan', link('sanitized', list(sanobjects.values()), sanflags), env)
    killed = []
    if not args.skip_mutants:
        for name, path, before, after in mutations():
            original = (ROOT / path).read_text()
            assert original.count(before) == 1, (name, original.count(before))
            source = out / ('mutant-' + name + '.cpp')
            source.write_text(original.replace(before, after, 1))
            obj = compile_one('mutant-' + name, source, FLAGS)
            mutant_objects = [obj if p == path else o for p, o in objects.items()]
            binary = link('mutant-' + name, mutant_objects, FLAGS)
            report = out / ('mutant-' + name + '-report.json')
            run('mutant-' + name + '-run', [binary, fixture, report], expected=1)
            result = json.loads(report.read_text())
            assert result['failures'] > 0 and result['observed_allocations'] == 0
            killed.append(dict(name=name, source_sha256=sha(source), failures=result['failures'], normal_exit_code=1))
    assert hashes == {p: sha(ROOT / p) for p in SOURCES}, 'Inputs changed during serial evidence run'
    summary = dict(input_hashes=hashes, cpu_affinity=sorted(os.sched_getaffinity(0)), parallelism=1,
                   release=release, sanitized=sanitized, mutants=killed,
                   sanitizer='skipped' if args.skip_sanitizer else 'ASan+UBSan passed; LeakSanitizer explicitly disabled because executor ptrace route is unsupported',
                   max_child_rss_kib=resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss,
                   runner_sha256=sha(Path(__file__)), all_passed=True,
                   note='Synthetic bounded serial native Linux evidence only; no game/Xbox FPS, live PC, RAM or install claim')
    (out / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    files = {str(p.relative_to(out)): sha(p) for p in sorted(out.iterdir()) if p.is_file() and p.name != 'file-hashes.json'}
    (out / 'file-hashes.json').write_text(json.dumps(files, indent=2) + '\n')
    print(json.dumps(dict(output=str(out.relative_to(ROOT)), checks=release['checks'], mutants=len(killed), passed=True)))


if __name__ == '__main__':
    main()
