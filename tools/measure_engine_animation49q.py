#!/usr/bin/env python3
"""Native49Q incremental object, caller record and static-stack measurements."""
import argparse
import json
import re
import sys
from pathlib import Path
sys.dont_write_bytecode = True
from measure_scene_projection49p import (ROOT, FLAGS, run, digest, dependency_paths,
    snapshot_sources, check_stable_sources, sections)

EVIDENCE = ROOT / 'evidence/engineAnimation49Q/footprint'
SOURCES = ['game/model_motion49q', 'game/engine_animation49q']
SUPPORT = ['game/scene_graph49p', 'assets/model_geometry49o', 'assets/model_bindings49o']
TYPES = ['SceneNode49P', 'SceneGraph49P', 'SceneNodes31', 'SceneNodes64',
         'ModelMotionState49Q', 'ModelMotionWorkspace49Q', 'ModelMotionDiagnostic49Q',
         'AnimationFilter49Q', 'EngineAnimationState49Q', 'EngineAnimationInput49Q',
         'EngineMotionModel49Q', 'EngineAnimationModels49Q', 'EngineAnimationWorkspace49Q',
         'EngineAnimationDiagnostic49Q', 'MotionView49O', 'MotionViews2',
         'MotionNodeBindings49O', 'MotionBindings2', 'PsgHierarchyView49O',
         'PsgViews2', 'ByteAxisKey49O', 'BezierKey49O', 'BodyMatrix48G']


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    args = parser.parse_args()
    build = args.build_dir.resolve(); build.mkdir(parents=True, exist_ok=False)
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    before = snapshot_sources(); objects = {}; hashes = {}; commands = []; frames = []; reports = []
    # Old sampler is compiled only to measure duplicated immutable table payload.
    for name in [*SOURCES, *SUPPORT, 'game/model_motion49o']:
        stem = Path(name).name; source = ROOT / 'src' / (name + '.cpp')
        obj, dep = build / (stem + '.o'), build / (stem + '.d')
        command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-c', str(source), '-o', str(obj)]
        commands.append(command); run(command)
        for path in dependency_paths(dep): hashes[path.relative_to(ROOT).as_posix()] = digest(path)
        measured, summary, detail = sections(obj); reports.append(summary)
        (EVIDENCE / (stem + '-sections.txt')).write_text(detail)
        symbols = run(['nm', '-S', '--size-sort', '--radix=d', '-C', str(obj)])
        undefined = run(['nm', '-u', '-C', str(obj)])
        measured['readonly_table_symbols'] = []
        for line in symbols.splitlines():
            fields = line.split(maxsplit=3)
            if len(fields) == 4 and fields[3].endswith(('::sineWords', '::cosineWords', '::acosWords')):
                measured['readonly_table_symbols'].append({'name':fields[3], 'bytes':int(fields[1]), 'symbol_type':fields[2]})
        measured['direct_allocator_references'] = [s for s in undefined.splitlines() if
            any(t in s for t in ('operator new', 'malloc', 'calloc', 'realloc', 'aligned_alloc'))]
        assert not measured['direct_allocator_references']
        for suffix, content in [('symbols', symbols), ('undefined', undefined),
                                ('host-disassembly', run(['objdump', '-drC', str(obj)]))]:
            (EVIDENCE / (stem + '-' + suffix + '.txt')).write_text(content)
        measured.update(source_sha256=digest(source), object_sha256=digest(obj)); objects[name] = measured
        su = (build / (stem + '.su')).read_text(); (EVIDENCE / (stem + '.su')).write_text(su)
        for line in su.splitlines():
            f = line.split('\t'); frames.append({'unit':name, 'function':f[0], 'bytes':int(f[1]), 'kind':f[2]})
    code = '''#include "game/engine_animation49q.h"
#include <cstdio>
using namespace rr::assets; using namespace rr::game;
using SceneNodes31 = SceneNode49P[31];
using SceneNodes64 = SceneNode49P[maxSceneNodes49P];
using MotionViews2 = MotionView49O[2];
using MotionBindings2 = MotionNodeBindings49O[2];
using PsgViews2 = PsgHierarchyView49O[2];
int main() {
'''
    code += ''.join(f' std::printf("{t} %zu %zu\\n", sizeof({t}), alignof({t}));\n' for t in TYPES) + '}\n'
    (build / 'record-sizes.cpp').write_text(code); dep = build / 'record-sizes.d'
    command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-I', str(ROOT / 'src'), str(build / 'record-sizes.cpp'), '-o', str(build / 'record-sizes')]
    commands.append(command); run(command)
    for path in dependency_paths(dep):
        if path.is_relative_to(ROOT): hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    stdout = run([str(build / 'record-sizes')]); (EVIDENCE / 'record-sizes.cpp').write_text(code)
    (EVIDENCE / 'record-sizes.stdout').write_text(stdout); (EVIDENCE / 'gnu-size.txt').write_text(''.join(reports))
    records = {r.split()[0]:int(r.split()[1]) for r in stdout.splitlines()}
    alignments = {r.split()[0]:int(r.split()[2]) for r in stdout.splitlines()}
    keys = ['gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','data_bytes','bss_bytes']
    new_tables = objects['game/model_motion49q']['readonly_table_symbols']
    old_tables = objects['game/model_motion49o']['readonly_table_symbols']
    assert new_tables == old_tables and len(new_tables) == 3
    table_bytes = sum(r['bytes'] for r in new_tables)
    def frame(fragment):
        rows = [f for f in frames if fragment in f['function']]
        if len(rows) != 1: raise RuntimeError('Missing or ambiguous frame: ' + fragment)
        return rows[0]['bytes']
    common = frame('advanceEngineAnimation49Q(') + frame('sampleModelMotion49Q(')
    recursion = frame('void rr::game::{anonymous}::invalidate(')
    estimates = [
        {'chain':['advanceEngineAnimation49Q','sampleModelMotion49Q','setLocalMatrix49P','afterLocalWrite','invalidate x <=64'],
         'sum_compiler_frame_bytes': common + frame('setLocalMatrix49P(') + frame('void rr::game::{anonymous}::afterLocalWrite(') + 64 * recursion},
        {'chain':['advanceEngineAnimation49Q','sampleModelMotion49Q','setLocalMatrix49P','validateSceneGraph49P'],
         'sum_compiler_frame_bytes': common + frame('setLocalMatrix49P(') + frame('validateSceneGraph49P(')}]
    baseline_path = ROOT / 'evidence/sceneProjection49P/footprint/summary.json'
    baseline = json.loads(baseline_path.read_text())
    comparison = {t:{'49P_bytes':baseline['record_bytes'][t], '49Q_bytes':records[t], 'delta_bytes':records[t]-baseline['record_bytes'][t]} for t in records if t in baseline['record_bytes']}
    assert comparison and all(v['delta_bytes']==0 for v in comparison.values())
    caller = ['SceneNodes31','SceneGraph49P','EngineAnimationState49Q','EngineAnimationInput49Q','EngineAnimationModels49Q','EngineAnimationWorkspace49Q','EngineAnimationDiagnostic49Q','MotionViews2','MotionBindings2']
    hashes['tools/measure_engine_animation49q.py'] = digest(Path(__file__))
    hashes['tools/measure_scene_projection49p.py'] = digest(ROOT / 'tools/measure_scene_projection49p.py')
    check_stable_sources(before, hashes)
    result = {'status':'measured','compiler':run(['g++','--version']).splitlines()[0],
        'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'compile_commands':commands,
        'source_hashes':hashes,'objects':objects,'record_bytes':records,'record_alignments':alignments,
        'incremental49Q_units':SOURCES,'totals':{k:sum(objects[n][k] for n in SOURCES) for k in keys},
        'support_units_not_incremental49Q':SUPPORT,
        'caller_payload_records':{n:records[n] for n in caller},
        'caller_payload_total_bytes_excluding_assets_loader_and_frame_padding':sum(records[n] for n in caller),
        'workspace_composition':{'outer_scene_transaction64_bytes':records['SceneNodes64'],
             'nested_single_motion_transaction64_bytes':records['ModelMotionWorkspace49Q'],
             'both_already_in_EngineAnimationWorkspace49Q_bytes':records['EngineAnimationWorkspace49Q']},
        'immutable_tables':{'one_sampler_payload_bytes':table_bytes,'old49O_sampler_payload_bytes':table_bytes,
             'additional49Q_duplicate_payload_bytes':table_bytes,'combined_if_both_objects_linked_bytes':2*table_bytes,
             'link_note':'Internal-linkage arrays appear in both separately compiled objects; this measures object payload, not a full-game link or linker-folding result.'},
        'frames':frames,'maximum_individual49Q_compiler_frame_bytes':max(f['bytes'] for f in frames if f['unit'] in SOURCES),
        'recursive_invalidate':{'maximum_source_invocations_on_stack':64,'individual_compiler_frame_bytes':recursion},
        'selected_additive_compiler_frame_estimates':estimates,
        'baseline49P':{'path':str(baseline_path.relative_to(ROOT)),'sha256':digest(baseline_path),'record_comparison':comparison},
        'limits':[
            'Host GCC14.2 C++17 sizeof/alignment, relocatable object sections and compiler stack estimates only. No console, PC game RAM or full-frame claim.',
            'Only the two new49Q production units count toward incremental totals. Existing dependencies and loader/runtime are reported separately or excluded.',
            'GNU size text includes read-only data and unwind metadata; executable .text, .rodata, .data and .bss are split explicitly.',
            'Zero mutable object data/BSS does not remove caller memory. Caller owns current scene/state, asset views, retained MOT bytes and two64-node transaction arrays.',
            'Caller payload sum is component sizeof arithmetic, not an ABI aggregate, live allocation total or stack high-water. Loader fixtures and process runtime are additional.',
            'Old49O and new49Q each contain10244 immutable trig-table bytes. New49Q duplicates this payload when both sampler objects are retained; no memory saving is claimed.',
            'Static .su estimates are not measured stack high-water or deepest-touched stack-address bounds. x86_64 red-zone usage can extend beyond reported stack-pointer frames.',
            'Selected additive chains include conservative64-node invalidate recursion, count frames even when tail-optimized, and omit external runtime/fegetround/memcpy and benchmark callers.',
            'Maximum individual frame and selected chains do not constitute an exhaustive transitive stack bound. Source recursion and caller-owned workspace are separate costs.',
            'Direct allocator symbol audit is not allocation-failure, ownership or capacity testing.']}
    (EVIDENCE / 'summary.json').write_text(json.dumps(result,indent=2)+'\n'); print(json.dumps(result))

if __name__ == '__main__': main()
