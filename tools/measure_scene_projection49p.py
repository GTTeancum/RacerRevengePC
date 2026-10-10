#!/usr/bin/env python3
"""Measure the bounded 49P host graph/projection footprint; never game memory."""
import argparse
import hashlib
import json
import shlex
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / 'evidence/sceneProjection49P/footprint'
SOURCES = ['game/scene_graph49p', 'game/chariot_projection49p']
FLAGS = ['-std=c++17', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
         '-fno-fast-math', '-frounding-math', '-ffp-contract=off', '-fstack-usage']
TYPES = ['SceneNodeIndex49P', 'SceneNode49P', 'SceneGraph49P', 'SceneDisplacement49P',
         'ChariotProjectionInput49P', 'BodyMatrix48G', 'SceneNodes22', 'SceneNodes64',
         'RefreshChain64', 'RefreshWorlds64', 'ValidationLinked64', 'TranslationScratch3',
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
    return [Path(p).resolve() for p in shlex.split(dep.read_text().replace('\\\n', ' ').split(':', 1)[1])]


def snapshot_sources():
    return {p.relative_to(ROOT).as_posix(): digest(p) for p in (ROOT / 'src').rglob('*')
            if p.is_file() and p.suffix in ('.cpp', '.h', '.inc')}


def check_stable_sources(before, used):
    changed = [n for n, h in used.items() if n.startswith('src/') and
               (before.get(n) != h or digest(ROOT / n) != h)]
    if changed:
        raise RuntimeError('Production sources changed; rerun: ' + ', '.join(changed))


def sections(path):
    summary = run(['size', '-B', str(path)])
    detail = run(['size', '-A', str(path)])
    row = summary.splitlines()[-1].split()
    values = {}
    for line in detail.splitlines():
        fields = line.split()
        if len(fields) == 3 and fields[0].startswith('.'):
            values[fields[0]] = int(fields[1])
    return {'gnu_size_text_including_readonly_and_unwind_bytes': int(row[0]),
            'data_bytes': int(row[1]), 'bss_bytes': int(row[2]),
            'executable_text_bytes': sum(v for k, v in values.items() if k.startswith('.text')),
            'readonly_data_bytes': sum(v for k, v in values.items() if k.startswith('.rodata')),
            'sections': values}, summary, detail


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    args = parser.parse_args()
    build = args.build_dir.resolve()
    build.mkdir(parents=True, exist_ok=False)
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    before = snapshot_sources()
    objects, frames, hashes, commands, size_reports = {}, [], {}, [], []
    for name in SOURCES:
        stem = Path(name).name
        source = ROOT / 'src' / (name + '.cpp')
        obj, dep = build / (stem + '.o'), build / (stem + '.d')
        command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-c', str(source), '-o', str(obj)]
        commands.append(command); run(command)
        for path in dependency_paths(dep):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
        measured, summary, detail = sections(obj)
        size_reports.append(summary)
        (EVIDENCE / (stem + '-sections.txt')).write_text(detail)
        for suffix, command in [('symbols', ['nm', '-S', '--size-sort', '--radix=d', '-C']),
                                ('undefined', ['nm', '-u', '-C']),
                                ('host-disassembly', ['objdump', '-drC'])]:
            output = run([*command, str(obj)])
            (EVIDENCE / (stem + '-' + suffix + '.txt')).write_text(output)
            if suffix == 'undefined':
                measured['direct_allocator_references'] = [s for s in output.splitlines() if
                    any(t in s for t in ('operator new', 'malloc', 'calloc', 'realloc', 'aligned_alloc'))]
        measured.update(source_sha256=digest(source), object_sha256=digest(obj))
        objects[name] = measured
        su = (build / (stem + '.su')).read_text()
        (EVIDENCE / (stem + '.su')).write_text(su)
        for line in su.splitlines():
            fields = line.split('\t')
            frames.append({'function': fields[0], 'bytes': int(fields[1]), 'kind': fields[2]})
    code = '''#include "game/chariot_projection49p.h"
#include "game/pod_model_geometry49o.h"
#include "assets/pod_physics49n.h"
#include <cstdio>
using namespace rr::assets; using namespace rr::game;
using SceneNodes22 = SceneNode49P[22];
using SceneNodes64 = SceneNode49P[maxSceneNodes49P];
using RefreshChain64 = SceneNodeIndex49P[maxSceneNodes49P];
using RefreshWorlds64 = BodyMatrix48G[maxSceneNodes49P];
using ValidationLinked64 = bool[maxSceneNodes49P];
using TranslationScratch3 = float[3];
int main() {
'''
    code += ''.join(f' std::printf("{t} %zu %zu\\n", sizeof({t}), alignof({t}));\n' for t in TYPES)
    code += '}\n'
    (build / 'record-sizes.cpp').write_text(code)
    dep = build / 'record-sizes.d'
    command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-I', str(ROOT / 'src'),
               str(build / 'record-sizes.cpp'), '-o', str(build / 'record-sizes')]
    commands.append(command); run(command)
    for path in dependency_paths(dep):
        if path.is_relative_to(ROOT):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    stdout = run([str(build / 'record-sizes')])
    records = {r.split()[0]: int(r.split()[1]) for r in stdout.splitlines()}
    alignments = {r.split()[0]: int(r.split()[2]) for r in stdout.splitlines()}
    (EVIDENCE / 'record-sizes.cpp').write_text(code)
    (EVIDENCE / 'record-sizes.stdout').write_text(stdout)
    (EVIDENCE / 'gnu-size.txt').write_text(''.join(size_reports))
    baseline_path = ROOT / 'evidence/authoredGeometry49O/footprint/summary.json'
    baseline = json.loads(baseline_path.read_text())
    comparisons = {t: {'49O_bytes': baseline['record_bytes'][t], '49P_bytes': records[t],
                       'delta_bytes': records[t] - baseline['record_bytes'][t]}
                   for t in records if t in baseline['record_bytes']}
    assert comparisons and all(v['delta_bytes'] == 0 for v in comparisons.values())
    assert not any(o['direct_allocator_references'] for o in objects.values())
    def frame(name):
        matches = [f['bytes'] for f in frames if name in f['function']]
        if len(matches) != 1:
            raise RuntimeError('Missing/ambiguous compiler frame: ' + name)
        return matches[0]
    # Every recursive invalidate visit is a distinct node in the admitted
    # forest. <=64 simultaneously live invocations is conservative even when
    # the host compiler optimizes the child tail call. Caller frames and source
    # recursion are not the same thing as a measured stack high-water mark.
    recursion_frame = frame('void rr::game::{anonymous}::invalidate(')
    setters = ['setParentlessWorldMatrix49P(', 'addParentlessTranslation49P(', 'setLocalMatrix49P(']
    estimates = [{'chain': [n, 'afterLocalWrite(', 'invalidate x <=64'],
                  'sum_compiler_frame_bytes': frame(n) + frame('void rr::game::{anonymous}::afterLocalWrite(') + 64 * recursion_frame} for n in setters]
    estimates += [{'chain': ['projectChariotPhysicalBranch49P(', 'setLocalMatrix49P(', 'afterLocalWrite(', 'invalidate x <=64'],
                   'sum_compiler_frame_bytes': frame('projectChariotPhysicalBranch49P(') + frame('setLocalMatrix49P(') + frame('void rr::game::{anonymous}::afterLocalWrite(') + 64 * recursion_frame},
                  {'chain': ['invalidateSceneReady49P(', 'invalidate x <=64'],
                   'sum_compiler_frame_bytes': frame('invalidateSceneReady49P(') + 64 * recursion_frame},
                  {'chain': ['refreshSceneWorld49P(', 'validateSceneGraph49P('],
                   'sum_compiler_frame_bytes': frame('refreshSceneWorld49P(') + frame('validateSceneGraph49P(')}]
    hashes['tools/measure_scene_projection49p.py'] = digest(Path(__file__))
    check_stable_sources(before, hashes)
    result = {
        'status': 'measured', 'compiler': run(['g++', '--version']).splitlines()[0],
        'compiler_target': run(['g++', '-dumpmachine']).strip(), 'flags': FLAGS,
        'compile_commands': commands, 'source_hashes': hashes, 'objects': objects,
        'record_bytes': records, 'record_alignments': alignments,
        'totals': {k: sum(o[k] for o in objects.values()) for k in
                   ['gnu_size_text_including_readonly_and_unwind_bytes', 'executable_text_bytes',
                    'readonly_data_bytes', 'data_bytes', 'bss_bytes']},
        'caller_owned_22_node_storage_bytes': records['SceneNodes22'],
        'caller_owned_maximum_64_node_storage_bytes': records['SceneNodes64'],
        'graph_view_and_projection_input_bytes': records['SceneGraph49P'] + records['ChariotProjectionInput49P'],
        'refresh_explicit_local_array_payload_bytes': records['RefreshChain64'] + records['RefreshWorlds64'],
        'frames': frames, 'maximum_individual_compiler_frame_bytes': max(f['bytes'] for f in frames),
        'recursive_invalidate': {'maximum_source_invocations_on_stack': 64,
                                'individual_compiler_frame_bytes': recursion_frame,
                                'compiler_frame_product_bytes': 64 * recursion_frame},
        'selected_additive_compiler_frame_estimates': estimates,
        'baseline49O': {'path': str(baseline_path.relative_to(ROOT)), 'sha256': digest(baseline_path),
                        'record_comparison': comparisons},
        'limits': [
            'Host GCC sizeof/alignment, relocatable object sections and compiler stack estimates only; no console or whole-game memory claim.',
            'GNU size text includes read-only data and unwind metadata; executable .text, .rodata, .data and .bss are reported separately.',
            'Object totals include the two incremental 49P units only; benchmark loader, C/C++ runtime and existing production dependencies are excluded.',
            'Caller owns all graph nodes. The graph is only a pointer/count view; zero .bss does not mean zero memory or reduced existing state.',
            'Scratch payload sizes describe the source arrays, not extra independent heap records; they are already included in the function compiler stack estimate.',
            'Stack .su values and <=64 recursive invalidate frame products are static compiler estimates, not measured stack high-water or universal runtime/ABI bounds.',
            'Host disassembly shows leaf validation uses the x86_64 red zone; compiler .su stack-pointer frames are not an exact deepest-touched stack-address bound.',
            'Selected additive estimates conservatively count caller frames even when a call is tail-optimized; external fegetround/memmove/runtime and the benchmark caller are excluded.',
            'Each recursive invalidation visits a distinct admitted graph node; a 64-node forest gives a conservative maximum of 64 simultaneous invalidate invocations.',
            'Unchanged 49O body/state/parameter sizes are comparisons, not state-size reductions.',
            'Direct allocator symbol audit is not an allocation-failure or ownership experiment.'
        ]}
    (EVIDENCE / 'summary.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
