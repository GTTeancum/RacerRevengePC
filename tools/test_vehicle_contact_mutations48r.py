#!/usr/bin/env python3
"""Serial, test-only source sensitivity checks for the 48R native handler.

Mutated copies and binaries stay in a new external output directory. Original
source and fixture hashes must remain unchanged. A mutation is killed only by
normal test failure (exit1), never a build error, timeout or crash.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import time
sys.dont_write_bytecode = True


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo-root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--cxx', default='g++')
    args = parser.parse_args()
    root, out = args.repo_root.resolve(), args.output_dir.resolve()
    if out == root or root in out.parents:
        parser.error('Output must be outside repository')
    if out.exists() and any(out.iterdir()):
        parser.error('Output directory must be empty')
    out.mkdir(parents=True, exist_ok=True)
    source = root / 'src/game/vehicle_contact48r.cpp'
    test = root / 'tests/native/vehicle_contact48r.cpp'
    fixtures = root / 'evidence/vehicleContact48R/native-fixtures.json'
    originals = {str(p.relative_to(root)): sha(p) for p in (source, test, fixtures)}
    text = source.read_text()
    mutations = [
        ('cached-candidate-radius', 'const float radius=math.mul(fromBits(result.statics.words[0]),math.root(math.square(half)));', 'const float radius=candidate.cachedRadius;'),
        ('nonstrict-overlap-tie', 'if(!(distance<threshold))continue;', 'if(!(distance<=threshold))continue;'),
        ('lane-division-normalization', 'result.v[i]=mul(a.v[i],q);', 'result.v[i]=d==0.f?mul(a.v[i],q):div(a.v[i],root(d));'),
        ('force-half-static-scale', 'math.mul(fromBits(result.statics.words[0]),math.root(math.square(half)))', 'math.mul(0.5f,math.root(math.square(half)))'),
        ('bounds-center-instead-cached-center', 'math.sub(position.v[i],candidate.cachedCenter.v[i])', 'math.sub(position.v[i],candidate.localCenter.v[i])'),
        ('reset-body-count-in-handler', 'auto& primary=scratch.bodies[input.primaryBodyIndex];', 'auto& primary=scratch.bodies[input.primaryBodyIndex]; primary.contacts.count20C=0;'),
        ('reset-spring-count-in-handler', 'VehicleContactResult48R result{input.spring,input.statics};', 'VehicleContactResult48R result{input.spring,input.statics}; result.spring.generatedCount84=0;'),
        ('skip-callbacks-when-saturated', 'auto& primary=scratch.bodies[input.primaryBodyIndex];', 'if(result.spring.generatedCount84>=result.spring.generatedCapacity88)continue; auto& primary=scratch.bodies[input.primaryBodyIndex];'),
        ('reverse-candidate-order', 'source.candidateOrder[static_cast<std::size_t>(index)]', 'source.candidateOrder[static_cast<std::size_t>(input.candidateCount-1-index)]'),
        ('conflate-position-local-scenes', 'auto& localScene=scratch.scenes[input.localSceneIndex];', 'auto& localScene=scratch.scenes[input.positionSceneIndex];'),
        ('invent-unit-zero-normal', 'result.v[i]=mul(a.v[i],q);', 'result.v[i]=d==0.f?(i==0?1.f:0.f):mul(a.v[i],q);'),
        ('reassociate-dot-products', 'return add(add(x,y),mul(one,z));', 'return add(x,add(y,mul(one,z)));'),
        ('normalize-consumed-up-axis', 'Arithmetic math;', 'Arithmetic math; up=math.normal(up);'),
        ('drop-up-projection-to-y-only', 'Vec up{{positionScene.world.v[4],positionScene.world.v[5],positionScene.world.v[6],1}};', 'Vec up{{0,1,0,1}};'),
        ('clear-stale-record-margin', 'copy(scratch.records,source.records,source.recordCount);', 'copy(scratch.records,source.records,source.recordCount); for(std::size_t i=0;i<source.recordCount;++i)scratch.records[i].margin48=0;'),
    ]
    flags = ['-std=c++17', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-fno-fast-math', '-frounding-math', '-ffp-contract=off', '-I'+str(root/'src/game'), '-I'+str(root/'third_party/native-json')]
    commands = []
    def run(command, name, timeout=120):
        start = time.monotonic()
        r = subprocess.run(command, cwd=root, capture_output=True, timeout=timeout)
        (out/(name+'.stdout.log')).write_bytes(r.stdout)
        (out/(name+'.stderr.log')).write_bytes(r.stderr)
        commands.append(dict(name=name, command=[str(x) for x in command], returncode=r.returncode, elapsed_seconds=round(time.monotonic()-start,6)))
        return r
    objects = []
    for rel, name in [('src/game/body_contact_list48r.cpp','body'), ('src/game/spring_records48n.cpp','records'), ('tests/native/vehicle_contact48r.cpp','harness')]:
        obj = out/(name+'.o')
        r = run([args.cxx,*flags,'-c',str(root/rel),'-o',str(obj)],'compile-'+name)
        if r.returncode:
            print(r.stderr.decode(), file=sys.stderr);return 2
        objects.append(obj)
    results = []
    for name, old, new in [('unmodified',None,None), *mutations]:
        current = text
        if old is not None:
            if current.count(old) != 1:
                raise RuntimeError(name+': mutation anchor not unique')
            current = current.replace(old,new)
        cpp, obj, binary = out/(name+'.cpp'), out/(name+'.o'), out/name
        cpp.write_text(current)
        r = run([args.cxx,*flags,'-c',str(cpp),'-o',str(obj)],name+'-compile')
        if r.returncode:
            results.append(dict(name=name,status='build-error',returncode=r.returncode));continue
        r = run([args.cxx,*map(str,objects),str(obj),'-o',str(binary)],name+'-link')
        if r.returncode:
            results.append(dict(name=name,status='link-error',returncode=r.returncode));continue
        r = run([str(binary),str(root)],name+'-run')
        status = ('passed' if r.returncode==0 else 'failed') if name=='unmodified' else ('killed' if r.returncode==1 else 'survived' if r.returncode==0 else 'crashed-or-error')
        results.append(dict(name=name,status=status,returncode=r.returncode,source_sha256=sha(cpp),test_stdout=r.stdout.decode(),first_failures=r.stderr.decode().splitlines()[:6]))
        print(name+': '+status,flush=True)
    after = {rel:sha(root/rel) for rel in originals}
    ok = originals==after and results[0]['status']=='passed' and len(results)==1+len(mutations) and all(r['status']=='killed' for r in results[1:])
    report = dict(schema=1,status='passed' if ok else 'failed',serial=True,compile_flags=flags,original_hashes=originals,after_hashes=after,source_unchanged=originals==after,mutations=results,commands=commands)
    (out/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    return 0 if ok else 1


if __name__=='__main__':
    sys.exit(main())
