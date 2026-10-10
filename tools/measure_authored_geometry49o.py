#!/usr/bin/env python3
"""Reproduce host object/record/stack costs of the native 49O load-time path."""
import argparse
import hashlib
import json
import shlex
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / 'evidence/authoredGeometry49O/footprint'
SOURCES = ['assets/model_geometry49o', 'assets/model_bindings49o',
           'game/model_motion49o', 'game/model_bounds49o',
           'game/authored_geometry49o', 'game/pod_model_geometry49o']
FLAGS = ['-std=c++17', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
         '-fno-fast-math', '-frounding-math', '-ffp-contract=off', '-fstack-usage']
TYPES = ['GeometryDiagnostic49O', 'PsgHierarchyView49O', 'PsgNode49O',
         'MotTrack49O', 'MotionView49O', 'BezierKey49O', 'ByteAxisKey49O',
         'MotionNodeBinding49O', 'MotionNodeBindings49O', 'MotionBindingDiagnostic49O',
         'ModelLocalMatrix49O', 'ModelMotionDiagnostic49O', 'ModelBoundsNode49O',
         'ModelBoundsContext49O', 'ModelBoundsWorkspace49O', 'ModelBoundsResult49O',
         'AuthoredGeometryInputs49O', 'AuthoredGeometryBinding49O',
         'FreshPodGeometrySources49O', 'FreshPodGeometryWorkspace49O',
         'FreshPodGeometryResult49O', 'FreshPodGeometryDiagnostic49O',
         'AuthoredParameters49N', 'AuthoredBinding49N', 'CoupledBodyParameters49M',
         'CoupledBodyState49M', 'CoupledNoiseContext49M', 'SecondaryInputs49M',
         'PodSphereBody49L', 'PodSphereJoint49L']


def run(command):
    p = subprocess.run(command, cwd=ROOT, text=True, capture_output=True)
    if p.returncode or p.stderr:
        raise RuntimeError(shlex.join(map(str, command)) + '\n' + p.stdout + p.stderr)
    return p.stdout


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def dependency_paths(dep):
    text = dep.read_text().replace('\\\n', ' ')
    return [Path(p).resolve() for p in shlex.split(text.split(':', 1)[1])]


def snapshot_sources():
    return {p.relative_to(ROOT).as_posix(): digest(p) for p in (ROOT / 'src').rglob('*')
            if p.is_file() and p.suffix in ('.cpp', '.h', '.inc')}


def check_stable_sources(before, used):
    changed = [name for name, hash_value in used.items() if name.startswith('src/')
               and (before.get(name) != hash_value or digest(ROOT / name) != hash_value)]
    if changed:
        raise RuntimeError('Production sources changed while measuring; rerun: ' + ', '.join(changed))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    args = parser.parse_args()
    build = args.build_dir.resolve()
    build.mkdir(parents=True, exist_ok=False)
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    before = snapshot_sources()
    objects, frames, hashes, commands = {}, [], {}, []
    all_size = []
    for name in SOURCES:
        source = ROOT / 'src' / (name + '.cpp')
        stem = Path(name).name
        obj, dep = build / (stem + '.o'), build / (stem + '.d')
        command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-c', str(source), '-o', str(obj)]
        commands.append(command)
        run(command)
        for path in dependency_paths(dep):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
        summary = run(['size', '-B', str(obj)])
        all_size.append(summary)
        row = summary.splitlines()[-1].split()
        detailed = run(['size', '-A', str(obj)])
        (EVIDENCE / (stem + '-sections.txt')).write_text(detailed)
        sections = {}
        for line in detailed.splitlines():
            cols = line.split()
            if len(cols) == 3 and cols[0].startswith('.'):
                sections[cols[0]] = int(cols[1])
        nm = run(['nm', '-S', '--size-sort', '--radix=d', '-C', str(obj)])
        (EVIDENCE / (stem + '-symbols.txt')).write_text(nm)
        undefined = run(['nm', '-u', '-C', str(obj)])
        (EVIDENCE / (stem + '-undefined.txt')).write_text(undefined)
        disassembly = run(['objdump', '-drC', str(obj)])
        (EVIDENCE / (stem + '-host-disassembly.txt')).write_text(disassembly)
        tables = []
        for line in nm.splitlines():
            cols = line.split(maxsplit=3)
            if len(cols) == 4 and any(t in cols[3] for t in ('sineWords', 'cosineWords', 'acosWords')):
                tables.append({'name': cols[3], 'bytes': int(cols[1]), 'symbol_type': cols[2]})
        objects[name] = {
            'source_sha256': digest(source), 'object_sha256': digest(obj),
            'gnu_size_text_including_readonly_and_unwind_bytes': int(row[0]),
            'data_bytes': int(row[1]), 'bss_bytes': int(row[2]),
            'executable_text_bytes': sum(v for k, v in sections.items() if k.startswith('.text')),
            'readonly_data_bytes': sum(v for k, v in sections.items() if k.startswith('.rodata')),
            'sections': sections, 'readonly_table_symbols': tables,
            'direct_allocator_references': [s for s in undefined.splitlines()
                if any(t in s for t in ('operator new', 'malloc', 'calloc', 'realloc', 'aligned_alloc'))]
        }
        su = (build / (stem + '.su')).read_text()
        (EVIDENCE / (stem + '.su')).write_text(su)
        for line in su.splitlines():
            fields = line.split('\t')
            frames.append({'function': fields[0], 'bytes': int(fields[1]), 'kind': fields[2]})
    code = '#include "game/pod_model_geometry49o.h"\n#include "assets/pod_physics49n.h"\n#include <cstdio>\nusing namespace rr::assets; using namespace rr::game;\nint main(){\n'
    code += ''.join(f' std::printf("{t} %zu %zu\\n", sizeof({t}), alignof({t}));\n' for t in TYPES)
    code += '}\n'
    (build / 'record-sizes.cpp').write_text(code)
    record_dep = build / 'record-sizes.d'
    run(['g++', *FLAGS, '-MMD', '-MF', str(record_dep), '-I', str(ROOT / 'src'), str(build / 'record-sizes.cpp'), '-o', str(build / 'record-sizes')])
    for path in dependency_paths(record_dep):
        if path.is_relative_to(ROOT):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    stdout = run([str(build / 'record-sizes')])
    records = {r.split()[0]: int(r.split()[1]) for r in stdout.splitlines()}
    alignments = {r.split()[0]: int(r.split()[2]) for r in stdout.splitlines()}
    (EVIDENCE / 'record-sizes.cpp').write_text(code)
    (EVIDENCE / 'record-sizes.stdout').write_text(stdout)
    (EVIDENCE / 'gnu-size.txt').write_text(''.join(all_size))
    baseline_path = ROOT / 'evidence/authoredInitialization49N/footprint/summary.json'
    baseline = json.loads(baseline_path.read_text())
    comparison = {t: {'49N_bytes': baseline['record_bytes'][t], '49O_bytes': records[t],
                      'delta_bytes': records[t] - baseline['record_bytes'][t]}
                  for t in records if t in baseline['record_bytes']}
    fixture_dir = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures'
    fixtures = ['52-chariot.psg', '43-engrt1.psg', '53-chariot.vut', '56-01_forwd.mot', '57-01_back.mot']
    retained = sum((fixture_dir / n).stat().st_size for n in fixtures)
    manifest = json.loads((fixture_dir.parent / 'asset-source-manifest.json').read_text())
    hashes['tools/measure_authored_geometry49o.py'] = digest(Path(__file__))
    check_stable_sources(before, hashes)
    def frame(name):
        matches = [f['bytes'] for f in frames if name in f['function']]
        if len(matches) != 1:
            raise RuntimeError('Ambiguous/missing compiler frame: ' + name)
        return matches[0]
    chains = [
        ['readMotionNodeBindings49O(', 'readPsgNode49O('],
        ['buildFreshPodGeometry49O(', 'sampleModelMotionBlend49O(', 'Eval::position(', 'readPositionKey49O('],
        ['buildFreshPodGeometry49O(', 'sampleModelMotionBlend49O(', 'Eval::rotation(', 'Eval::blend(', 'Eval::sine(']
    ]
    result = {
        'status': 'measured', 'compiler': run(['g++', '--version']).splitlines()[0],
        'compiler_target': run(['g++', '-dumpmachine']).strip(),
        'flags': FLAGS, 'compile_commands': commands, 'source_hashes': hashes,
        'objects': objects, 'record_bytes': records, 'record_alignments': alignments,
        'totals': {key: sum(o[key] for o in objects.values()) for key in
            ['gnu_size_text_including_readonly_and_unwind_bytes', 'executable_text_bytes',
             'readonly_data_bytes', 'data_bytes', 'bss_bytes']},
        'readonly_trig_table_bytes': sum(t['bytes'] for o in objects.values() for t in o['readonly_table_symbols']),
        'maximum_individual_compiler_frame_bytes': max(f['bytes'] for f in frames),
        'frames': frames,
        'selected_nested_compiler_frame_sums': [
            {'chain': chain, 'individual_bytes': [frame(name) for name in chain],
             'sum_bytes_excluding_caller_and_external_runtime': sum(frame(name) for name in chain)}
            for chain in chains],
        'caller_owned_load_records_bytes': sum(records[t] for t in
            ['FreshPodGeometrySources49O', 'FreshPodGeometryWorkspace49O', 'FreshPodGeometryResult49O', 'FreshPodGeometryDiagnostic49O']),
        'retained_fixture_input_bytes': retained,
        'benchmark_records_plus_input_bytes': retained + sum(records[t] for t in
            ['FreshPodGeometrySources49O', 'FreshPodGeometryWorkspace49O', 'FreshPodGeometryResult49O', 'FreshPodGeometryDiagnostic49O']),
        'resource_container_bytes': manifest['container']['bytes'],
        'resource_decoded_bytes': manifest['container']['decoded_bytes'],
        'baseline49N': {'path': str(baseline_path.relative_to(ROOT)), 'sha256': digest(baseline_path), 'record_comparison': comparison},
        'limits': [
            'These are GCC host object sections and sizeof records, not console or whole-game memory.',
            'GNU size text includes read-only data and unwind metadata. Executable .text and .rodata are separately reported; trig tables are not free.',
            'Six 49O modules are incremental costs; existing 49L link dependencies and C/C++ runtimes are not included in incremental totals.',
            'Individual .su frames are compiler estimates. Nested calls add simultaneously live stack; the largest frame is not a total-stack bound.',
            'Selected call-chain sums are additive compiler-frame estimates, checked against retained host disassembly; not measured stack high-water or a universal ABI/runtime upper bound.',
            'Named record/input totals exclude local metadata, allocator bookkeeping, ABI alignment, transitive stack, runtime, existing dependencies and executable sections; they are not a process-footprint claim.',
            'Selected fixture bytes remain alive with views. Workspace is caller-owned temporary load storage, not heap-free memory savings.',
            'Resource file I/O and 349110-byte compressed / 2826240-byte decoded container lifetime are outside this numeric core; no production decoder-residency reduction is demonstrated.',
            '49O adds geometry functionality. Existing coupled parameters/state/noise sizes are unchanged from 49N; no state-size reduction or FPS claim.',
            'Zero direct allocator references is a static object audit, not an allocation-failure experiment.'
        ]
    }
    assert not any(o['direct_allocator_references'] for o in objects.values())
    (EVIDENCE / 'summary.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
