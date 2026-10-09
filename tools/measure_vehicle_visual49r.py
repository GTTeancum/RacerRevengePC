#!/usr/bin/env python3
"""49R incremental object sections, caller records, and compiler stack estimates."""
import argparse
import json
import sys
from pathlib import Path
sys.dont_write_bytecode = True
from measure_scene_projection49p import (ROOT, FLAGS, run, digest, dependency_paths,
    snapshot_sources, check_stable_sources, sections)

EVIDENCE = ROOT / 'evidence/vehicleVisual49R/footprint'
SOURCES = ['game/vehicle_visual49r', 'game/visual_container49r',
           'game/visual_response49r', 'assets/vehicle_visual_authoring49r']
SUPPORT = ['game/scene_graph49p', 'game/gradient_noise49m', 'game/body_scene48g',
           'assets/pod_physics49n', 'assets/model_geometry49o']
UNIT_EXTRA_FLAGS = {'game/body_scene48g':['-Wno-misleading-indentation']}
TYPES = ['SceneNode49P', 'SceneNodes32', 'SceneNodes64', 'SceneGraph49P',
         'VisualVector49R', 'VisualResponse49R', 'VehicleVisualState49R',
         'VisualLazyWord49R', 'VisualShared49R', 'VehicleVisualParameters49R',
         'VehicleVisualInput49R', 'VisualSceneBindings49R', 'VehicleVisualWorkspace49R',
         'VehicleVisualDiagnostic49R', 'detail::VisualFrame49R',
         'GradientNoiseState49M', 'CoupledNoiseContext49M',
         'VisualAuthoredValues49R', 'VisualAuthoredProvenance49R',
         'VisualAuthoringDiagnostic49R', 'PsgHierarchyView49O', 'PsgViews2',
         'PsgNode49O', 'BodyMatrix48G', 'ReaderSections32', 'ReaderKeys512',
         'ReaderPresent27', 'RefreshChain64', 'RefreshWorlds64']


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    args = parser.parse_args()
    build = args.build_dir.resolve(); build.mkdir(parents=True, exist_ok=False)
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    before = snapshot_sources(); objects = {}; hashes = {}; commands = []; frames = []; reports = []
    for name in [*SOURCES, *SUPPORT]:
        stem = Path(name).name; source = ROOT / 'src' / (name + '.cpp')
        obj, dep = build / (stem + '.o'), build / (stem + '.d')
        command = ['g++', *FLAGS, *UNIT_EXTRA_FLAGS.get(name,[]), '-MMD', '-MF', str(dep), '-c', str(source), '-o', str(obj)]
        commands.append(command); run(command)
        for path in dependency_paths(dep): hashes[path.relative_to(ROOT).as_posix()] = digest(path)
        measured, summary, detail = sections(obj); reports.append(summary)
        (EVIDENCE / (stem + '-sections.txt')).write_text(detail)
        symbols = run(['nm', '-S', '--size-sort', '--radix=d', '-C', str(obj)])
        undefined = run(['nm', '-u', '-C', str(obj)])
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
    code = '''#include "game/vehicle_visual49r_internal.h"
#include "assets/vehicle_visual_authoring49r.h"
#include <cstdio>
using namespace rr::assets; using namespace rr::game;
using SceneNodes32 = SceneNode49P[32];
using SceneNodes64 = SceneNode49P[maxSceneNodes49P];
using PsgViews2 = PsgHierarchyView49O[2];
struct ReaderRange { std::uint16_t offset, bytes; };
struct ReaderSeen { ReaderRange key; std::uint16_t section; };
using ReaderSections32 = ReaderRange[maxPodPhysicsSections49N];
using ReaderKeys512 = ReaderSeen[maxPodPhysicsKeys49N];
using ReaderPresent27 = bool[visualAuthoredKeyCount49R];
using RefreshChain64 = SceneNodeIndex49P[maxSceneNodes49P];
using RefreshWorlds64 = BodyMatrix48G[maxSceneNodes49P];
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
    def frame(fragment):
        rows = [f for f in frames if fragment in f['function']]
        if len(rows) != 1: raise RuntimeError('Missing or ambiguous frame: ' + fragment)
        return rows[0]['bytes']
    wrapper = frame('updateVehicleVisual49R(')
    container = frame('prepareVisualContainer49R(')
    responses = frame('updateVisualResponses49R(')
    recursion = frame('void rr::game::{anonymous}::invalidate(')
    estimates = []
    for stage, stage_frame in [('prepareVisualContainer49R',container),('updateVisualResponses49R',responses)]:
        estimates.append({'chain':['updateVehicleVisual49R',stage,'refreshSceneWorld49P','validateSceneGraph49P'],
            'sum_compiler_frame_bytes':wrapper+stage_frame+frame('refreshSceneWorld49P(')+frame('validateSceneGraph49P(')})
    emit = frame('emitRoot(')
    estimates.append({'chain':['updateVehicleVisual49R','updateVisualResponses49R','emitRoot','setLocalMatrix49P','afterLocalWrite','invalidate x <=64'],
        'sum_compiler_frame_bytes':wrapper+responses+emit+frame('setLocalMatrix49P(')+frame('void rr::game::{anonymous}::afterLocalWrite(')+64*recursion})
    estimates.append({'chain':['readVehicleVisualAuthoring49R','parsePodPhysicsNumber49N'],
        'sum_compiler_frame_bytes':frame('readVehicleVisualAuthoring49R(')+frame('parsePodPhysicsNumber49N(')})
    caller = ['SceneNodes32','SceneGraph49P','VehicleVisualState49R','VisualShared49R',
              'CoupledNoiseContext49M','VehicleVisualParameters49R','VehicleVisualInput49R',
              'VisualSceneBindings49R','VehicleVisualWorkspace49R','VehicleVisualDiagnostic49R']
    scratch = ['ReaderSections32','ReaderKeys512','ReaderPresent27','VisualAuthoredValues49R','VisualAuthoredProvenance49R']
    hashes['tools/measure_vehicle_visual49r.py'] = digest(Path(__file__))
    hashes['tools/measure_scene_projection49p.py'] = digest(ROOT / 'tools/measure_scene_projection49p.py')
    check_stable_sources(before, hashes)
    result = {'status':'measured','compiler':run(['g++','--version']).splitlines()[0],
        'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':UNIT_EXTRA_FLAGS,'compile_commands':commands,
        'source_hashes':hashes,'objects':objects,'record_bytes':records,'record_alignments':alignments,
        'incremental49R_units':SOURCES,'totals':{k:sum(objects[n][k] for n in SOURCES) for k in keys},
        'runtime49R_units':SOURCES[:3],'runtime49R_totals':{k:sum(objects[n][k] for n in SOURCES[:3]) for k in keys},
        'authoring49R_units':SOURCES[3:],'authoring49R_totals':{k:sum(objects[n][k] for n in SOURCES[3:]) for k in keys},
        'support_units_not_incremental49R':SUPPORT,
        'caller_payload_records':{n:records[n] for n in caller},
        'caller_payload_total_bytes_excluding_assets_loader_and_frame_padding':sum(records[n] for n in caller),
        'workspace_composition':{n:records[n] for n in ['SceneNodes64','VehicleVisualState49R','VisualShared49R','CoupledNoiseContext49M']},
        'workspace_total_bytes':records['VehicleVisualWorkspace49R'],
        'shared_noise_accounting':'One caller-owned existing49M noise/RNG record reused by serial consumers; one transaction copy already inside VehicleVisualWorkspace49R. Neither is allocated per frame. Caller sum includes each exactly once.',
        'authoring_reader_local_scratch_payload':{n:records[n] for n in scratch},
        'authoring_reader_local_scratch_payload_sum_bytes':sum(records[n] for n in scratch),
        'authoring_reader_compiler_frame_bytes':frame('readVehicleVisualAuthoring49R('),
        'frames':frames,'maximum_individual49R_compiler_frame_bytes':max(f['bytes'] for f in frames if f['unit'] in SOURCES),
        'recursive_invalidate':{'maximum_source_invocations_on_stack':64,'individual_compiler_frame_bytes':recursion},
        'selected_additive_compiler_frame_estimates':estimates,
        'limits':[
            'Host GCC14 strict C++17 sizeof/alignment, relocatable object sections and compiler stack estimates only. No FPS, Xbox or full-game memory claim.',
            'Only the four new49R units count toward incremental totals. Existing49M noise,48G quaternion helpers,49P graph and49N/49O readers are reused code, reported separately.',
            'Reused body_scene48g.cpp has inherited formatting warnings; only its misleading-indentation diagnostic is disabled. Production bytes are unmodified and strict arithmetic flags remain identical.',
            'GNU size text includes read-only data and unwind metadata; executable .text, .rodata, .data and .bss are split explicitly. Relocation-backed constant descriptors can appear as data.',
            'Caller payload is component sizeof arithmetic, not an ABI aggregate, live allocation total or stack high-water. Loader assets/runtime/frame padding are additional.',
            'The64-node transaction workspace already contains a complete scene scratch, visual state, shared graphics and shared49M noise/RNG copy. Components must not be added again.',
            'Reader local arrays and result/provenance scratch are already included in its compiler frame; the scratch sum is explanatory, not an extra charge.',
            'Static .su values and selected additive chains are not measured stack high-water or exhaustive transitive bounds. x86_64 red-zone usage can extend beyond reported stack-pointer frames; runtime/libm and benchmark caller frames are excluded.',
            'Selected chains conservatively count frames even if tail-optimized. A64-node graph gives at most64 recursive invalidate invocations.',
            'No allocation-failure, ownership, reset/contact, historical diagnostic, install or push experiment is performed. Direct allocator symbol audit describes ordinary production linkage.']}
    (EVIDENCE / 'summary.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'status':result['status'],'totals':result['totals'],'caller_bytes':result['caller_payload_total_bytes_excluding_assets_loader_and_frame_padding'],'workspace_bytes':result['workspace_total_bytes']}))

if __name__ == '__main__': main()
