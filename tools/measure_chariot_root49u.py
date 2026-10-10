#!/usr/bin/env python3
"""Bounded49U strict production footprint and connected-history serial host cost."""
import argparse
import datetime
import json
import math
import os
import platform
import resource
import statistics
import struct
import subprocess
import sys
from pathlib import Path
sys.dont_write_bytecode = True
from measure_scene_projection49p import (ROOT, FLAGS, run, digest, dependency_paths,
    snapshot_sources, check_stable_sources, sections)
from measure_chariot_animation49t import (graph_words, live_words as retained_live_words,
    frame_words as retained_frame_words, palette_words, position_bytes, pack, fnv,
    MOT_NAMES, ASSET_NAMES, ASSET_DIR, PARAMS)
from visual.run_chariot_root49u import local_vertices, SCENES

BASE = ROOT / 'evidence/chariotRoot49U/budget'
NEW = ['game/chariot_root49u', 'game/chariot_scene49u', 'assets/chariot_root_authoring49u']
ANIMATION = ['game/source_acos49t', 'game/model_motion49t', 'game/chariot_ik49t', 'game/vehicle_animation49t']
SUPPORT = ANIMATION + ['game/scene_graph49p', 'assets/model_geometry49o', 'assets/model_bindings49o',
    'game/vehicle_visual49r', 'game/visual_container49r', 'game/visual_response49r',
    'game/gradient_noise49m', 'game/body_scene48g', 'game/model_palette49s', 'game/rigid_vertices49s',
    'assets/pod_physics49n', 'game/pod_model_geometry49o', 'game/model_motion49o',
    'game/model_bounds49o', 'game/authored_geometry49o', 'game/pod_sphere_constraint49l']
MODES = ['root', 'animation_visual_root_palette', 'animation_visual_root_palette_rigid']
HARNESS = ROOT / 'tests/native/bench_chariot_root49u.cpp'
PHY = ROOT / 'evidence/authoredInitialization49N/authoring-source/Pod01.phy'
MESH = ROOT / 'evidence/visual49G/export-placed-normal/pod01-placed.rrmesh'
CONNECTED_FREEZE = ROOT / 'evidence/chariotRoot49U/connected/frozen-connected-manifest.json'
ROOT_PARAMS = 'minTimeA3C maxTimeA40 minAmplitudeA4C maxAmplitudeA50 effectiveSpeedSquaredA54 orientationScaleA60 cableForceA6C radiusForceA70 restoringA74 slideRestoringA78 returnGainA7C linearDampingA80 angularDampingA84 yawInputRangeA88 yawAngleA8C orientationGainA94 collisionBoundA98 groundBoundA9C collisionXYAA0 collisionZAA4 groundGainAA8 groundTransitionGainAAC restLength854'.split()
EXTRA_FLAGS = {'game/body_scene48g': ['-Wno-misleading-indentation']}


def dump(path, value): path.write_text(json.dumps(value, indent=2) + '\n')


def root_words(row):
    s, h = row['root_state'], row['root_shared']
    return ([s[k] for k in ('vehicle838','vehicle83C','vehicle840','previous85C_bits')]
        + sum([s[k+'_bits'] for k in ('position870','linearVelocity880','angularVelocity890','orientation8A0')], [])
        + h['speed434750'] + h['yaw434758'] + h['up434650_bits'] + [row['phase864_bits']])


def live_words(row): return retained_live_words(row) + root_words(row)


def frame_words(row):
    r = row['root_input_bits']
    return (retained_frame_words(row)
        + [r[k] for k in ('phase','vehicle40','vehicle58','aeroD8','vehicleCFC','vehicleD00','probe10C',
                         'frameFactor_bits','cachedBodySpeed34_bits','bodyAngularVelocityYB4_bits','objectD34_10_bits')]
        + r['bodyLocalVelocity80_bits'] + r['impactBA0_bits'] + r['probeVector1D0_bits'])


def prepare_fixture(trace_path):
    trace=json.loads(trace_path.read_text()); summary=trace['summary']
    assert len(trace['frames'])==96 and summary['graph_nodes']==53
    assert summary['passed'] and summary['source_substitutions']==0
    assert summary['physical_entry_calls']==1 and summary['persistent_virtual_calls']==95
    assert summary['source49U_noise_calls']==570 and summary['source_noise_calls']==1722
    initial=trace['initial'];vertices=[local_vertices(ROOT/p) for p in SCENES]
    assert [len(v) for v in vertices]==[2884,1411,1460]
    out=bytearray(b'RR49UB01')+pack([96])+pack(graph_words(initial['pre_motion_constructor_graph']))
    out+=struct.pack('<Q',fnv(pack(graph_words(initial['post_constructor_graph']))))
    out+=pack(graph_words(initial['graph']))+pack(live_words(initial))
    for side in ('left','right'):
        r=initial['persistent_ik_records'][side]
        out+=pack(r['joint_indices']+[r['mode10']]+r['coefficient_bits']+r['saved_elbow_local30']+[r['debug40']]+r['axis50'])
    p=initial['visual_parameters_bits'];out+=pack([p[k] for k in PARAMS]+p['rightBase2C0']+p['leftBase2B0'])
    out+=pack([initial['root_parameters_bits'][k] for k in ROOT_PARAMS]+[initial['unconsumedA90_bits']])
    for vs in vertices:
        for xyz,bone in vs:out+=struct.pack('<3dI',*xyz,bone)
    expected=[];base_mesh=MESH.read_bytes()
    for row in trace['frames']:
        assert (row['root_state_before']['vehicle838']!=0)==(row['frame']==0)
        positions=position_bytes(row,vertices);mesh=bytearray(base_mesh)
        for n in range(5755):mesh[16+n*24:28+n*24]=positions[n*12:(n+1)*12]
        assert fnv(mesh)==int(row['mesh_fnv64'],16),'Connected full mesh digest differs'
        hashes=[int(row[k],16) for k in ('after21A000_graph_fnv64','after49R_graph_fnv64','after49U_graph_fnv64')]
        hashes += [fnv(pack(graph_words(row['graph']))),fnv(pack(live_words(row))),fnv(pack(palette_words(row))),fnv(positions),int(row['after49R_noise_fnv64'],16)]
        expected.append(hashes);out+=pack(frame_words(row))+struct.pack('<8Q',*hashes)
    path=BASE/'actual-history96.bin';path.write_bytes(out)
    result={'trace_path':str(trace_path.relative_to(ROOT)),'trace_sha256':digest(trace_path),
        'fixture_bytes':len(out),'fixture_sha256':digest(path),'frames':96,'graph_nodes':53,
        'physical_entry_calls':1,'persistent_virtual_calls':95,'root_noise_calls':570,'all_noise_calls':1722,
        'palette_entries':[18,14,14],'rigid_vertices':[2884,1411,1460],
        'source_assets':[{'path':str(p.relative_to(ROOT)),'bytes':p.stat().st_size,'sha256':digest(p)} for p in [PHY,MESH,*[ROOT/s for s in SCENES],*[ASSET_DIR/n for n in ASSET_NAMES]]],
        'expected_final_hashes':dict(zip(['animation_graph','visual_graph','root_graph','palette_graph','records','palettes','positions','noise_after49R'],expected[-1])),
        'all_frame_expected_hashes':expected,'source_full_mesh_digests_verified':96,
        'boundary':'Exact source-validated53-node96-frame history, actual PHY/MOT and native fresh geometry binding. Initial physical entry followed by95 virtual calls;49U consumes the same canonical SecondaryBodyState49M.time864 and persistent noise/RNG after49R. Controls, clock outputs, current main/secondary body snapshots and component/probe state are supplied. No later physics, contacts, reset lifecycle, authentic spawn,FrameReady or rendering execution.'}
    dump(BASE/'fixture.json',result);return result


def prepare(build,trace):
    BASE.mkdir(parents=True,exist_ok=True);build.mkdir(parents=True,exist_ok=False)
    before=snapshot_sources();hashes={};objects={};frames=[];commands=[]
    frozen=json.loads(CONNECTED_FREEZE.read_text())
    assert frozen['passed'] and frozen['normal_plain_sanitizer_identical']
    for group in ('source_sha256','input_sha256','artifact_sha256'):
        assert all(digest(ROOT/n)==h for n,h in frozen[group].items()),'Connected freeze changed: '+group
    assert frozen['artifact_sha256'][str(trace.relative_to(ROOT))]==digest(trace)
    for name in NEW+SUPPORT:
        stem=Path(name).name;source=ROOT/'src'/(name+'.cpp');obj=build/(stem+'.o');dep=build/(stem+'.d')
        command=['g++',*FLAGS,*EXTRA_FLAGS.get(name,[]),'-MMD','-MF',str(dep),'-c',str(source),'-o',str(obj)]
        commands.append(command);run(command)
        for path in dependency_paths(dep):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
        measured,summary,detail=sections(obj);undefined=run(['nm','-u','-C',str(obj)])
        measured['relocated_readonly_data_bytes']=sum(v for k,v in measured['sections'].items() if k.startswith('.data.rel.ro'))
        measured['mutable_data_bytes']=sum(v for k,v in measured['sections'].items() if (k=='.data' or k.startswith('.data.')) and not k.startswith('.data.rel.ro'))
        measured['direct_allocator_references']=[s for s in undefined.splitlines() if any(t in s for t in ('operator new','malloc','calloc','realloc','aligned_alloc'))]
        assert not measured['direct_allocator_references'],name
        measured.update(source_sha256=digest(source),object_sha256=digest(obj));objects[name]=measured
        for suffix,content in [('sections',detail),('size',summary),('undefined',undefined),('symbols',run(['nm','-S','--size-sort','--radix=d','-C',str(obj)]))]:
            (BASE/(stem+'-'+suffix+'.txt')).write_text(content)
        su=(build/(stem+'.su')).read_text();(BASE/(stem+'.su')).write_text(su)
        for line in su.splitlines():
            f=line.split('\t');frames.append({'unit':name,'function':f[0],'bytes':int(f[1]),'kind':f[2]})
    for binary,extra in [('benchmark',[]),('ordinary-new-probe',['-DPROBE49U'])]:
        dep=build/(binary+'.d');command=['g++',*FLAGS,*extra,'-I',str(ROOT/'src'),'-MMD','-MF',str(dep),str(HARNESS),*[str(build/(Path(n).name+'.o')) for n in NEW+SUPPORT],'-o',str(build/binary)]
        commands.append(command);run(command)
        for path in dependency_paths(dep):
            if path.is_relative_to(ROOT):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    for path in build.glob('*bench_chariot_root49u.su'):(BASE/path.name).write_text(path.read_text())
    stdout=run([str(build/'benchmark'),'--sizes']);(BASE/'record-sizes.stdout').write_text(stdout)
    records={r.split()[0]:int(r.split()[1]) for r in stdout.splitlines()};alignments={r.split()[0]:int(r.split()[2]) for r in stdout.splitlines()}
    baseline=ROOT.parent/'CLOUD-MILESTONE-49T-CHARIOTANIMATION-MANIFEST.json'
    listed=json.loads(baseline.read_text())['files'];prefix='RacerRevengePC/'
    prior={f['path'][len(prefix):]:f['sha256'] for f in listed if f['path'].startswith(prefix+'src/')}
    prior_units={n:prior['src/'+n+'.cpp']==digest(ROOT/'src'/(n+'.cpp')) for n in SUPPORT}
    assert all(prior_units.values()),'Reused production source differs from49T'
    prior_dependencies={n:digest(ROOT/n)==prior[n] for n in hashes if n.startswith('src/') and n in prior}
    assert all(prior_dependencies.values())
    assert all('src/'+n+'.cpp' not in prior for n in NEW)
    keys=['gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','relocated_readonly_data_bytes','mutable_data_bytes','data_bytes','bss_bytes']
    incremental=['ChariotRootState49U','ChariotRootShared49U','ChariotRootInput49U','ChariotRootParameters49U','ChariotRootBindings49U','ChariotRootWorkspace49U','ChariotRootDiagnostic49U','SceneNode49P']
    inherited=['VehicleAnimationState49T','VehicleAnimationShared49T','VehicleAnimationInput49T','VehicleAnimationBindings49T','ChariotIkPair49T','VehicleAnimationWorkspace49T','VehicleAnimationDiagnostic49T',
        'SceneGraph49P','VehicleVisualState49R','VehicleVisualParameters49R','VehicleVisualInput49R','VisualSceneBindings49R','VisualShared49R','CoupledNoiseContext49M','VehicleVisualWorkspace49R','VehicleVisualDiagnostic49R','MotionViews6','MotionBindings3','Palettes3','PaletteInputs3','PaletteNodes46','PaletteIds46','ModelPaletteWorkspace49S','ModelPaletteDiagnostic49S','RigidVertices5755','RigidPositions5755','RigidVertexDiagnostic49S']
    combined=incremental[:-1]+['SceneNodes53','SecondaryBodyState49M']+inherited
    loadtime=['a::ChariotRootAuthoredValues49U','a::ChariotRootAuthoredProvenance49U','a::ChariotRootAuthoringDiagnostic49U','FreshPodGeometryWorkspace49O','FreshPodGeometryResult49O']
    newframes=[f for f in frames if f['unit'] in NEW]
    runtimeframes=[f for f in frames if f['unit'] in NEW[:2]]
    def frame(unit,fragment):
        found=[f['bytes'] for f in frames if f['unit']==unit and fragment in f['function'] and '<lambda' not in f['function']]
        assert len(found)==1,(unit,fragment,found)
        return found[0]
    invalidate=frame('game/scene_graph49p','::invalidate(')
    additive_chains=[
        [('game/chariot_root49u','updateChariotRoot49U('),('game/chariot_scene49u','position49U('),('game/chariot_scene49u','::refresh('),('game/scene_graph49p','refreshSceneWorld49P('),('game/scene_graph49p','validateSceneGraph49P(')],
        [('game/model_palette49s','projectModelPalette49S('),('game/scene_graph49p','refreshSceneWorld49P('),('game/scene_graph49p','validateSceneGraph49P(')]]
    additive=[{'chain':[u+':'+f for u,f in chain],'sum_compiler_frame_bytes':sum(frame(u,f) for u,f in chain)} for chain in additive_chains]
    fixture=prepare_fixture(trace)
    input_paths=[HARNESS,Path(__file__),ROOT/'tools/measure_chariot_animation49t.py',ROOT/'tools/measure_scene_projection49p.py',ROOT/'tools/visual/run_chariot_root49u.py',ROOT/'tools/visual/run_chariot49t.py',trace,BASE/'actual-history96.bin',CONNECTED_FREEZE,PHY,MESH,*[ROOT/s for s in SCENES],*[ASSET_DIR/n for n in ASSET_NAMES]]
    hashes.update({str(p.relative_to(ROOT)):digest(p) for p in input_paths});check_stable_sources(before,hashes)
    symbols=run(['nm','-C',str(build/'benchmark')]);(BASE/'timed-executable-symbols.txt').write_text(symbols)
    for forbidden in ['Machine','original::','oracle','trace::','stage_trace','countAllocations']:assert forbidden not in symbols,forbidden
    measured,size,detail=sections(build/'benchmark');(BASE/'timed-executable-size.txt').write_text(size);(BASE/'timed-executable-sections.txt').write_text(detail)
    result={'status':'measured','compiler':run(['g++','--version']).splitlines()[0],'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':EXTRA_FLAGS,'compile_commands':commands,'source_hashes':hashes,'objects':objects,'frames':frames,
        'connected_freeze':{'path':str(CONNECTED_FREEZE.relative_to(ROOT)),'sha256':digest(CONNECTED_FREEZE),'validated_groups':{k:len(frozen[k]) for k in ('source_sha256','input_sha256','artifact_sha256')}},
        'incremental49U_units':NEW,'reused49T_or_earlier_support':SUPPORT,
        'incremental49U_totals':{k:sum(objects[n][k] for n in NEW) for k in keys},
        'incremental49U_runtime_units_totals':{k:sum(objects[n][k] for n in NEW[:2]) for k in keys},
        'incremental49U_loadtime_unit_totals':{k:objects[NEW[2]][k] for k in keys},
        'baseline49T_manifest':{'path':str(baseline),'sha256':digest(baseline),'reused_units_identical':prior_units,'all_retained_dependencies_identical':prior_dependencies},
        'record_bytes':records,'record_alignments':alignments,
        'incremental49U_caller_records':{n:records[n] for n in incremental},'incremental49U_caller_total_bytes':sum(records[n] for n in incremental),
        'incremental_scene198_bytes':records['SceneNode49P'],
        'combined_caller_records':{n:records[n] for n in combined},'combined_caller_payload_bytes_excluding_motion_bytes_and_padding':sum(records[n] for n in combined),
        'existing_canonical_secondary_owner_bytes':records['SecondaryBodyState49M'],'canonical_time864_member_bytes':4,
        'loadtime_only_record_bytes':{n:records[n] for n in loadtime},
        'retained_motion_byte_payload':sum((ASSET_DIR/n).stat().st_size for n in MOT_NAMES),
        'workspace_composition':{'ChariotRootWorkspace49U':records['ChariotRootWorkspace49U'],'included_nodes64':records['SceneNodes64'],'included_state':records['ChariotRootState49U'],'included_shared':records['ChariotRootShared49U'],'included_noise':records['CoupledNoiseContext49M'],'included_phase864':4},
        'maximum_individual49U_compiler_frame_bytes':max(f['bytes'] for f in newframes),'maximum_individual49U_runtime_compiler_frame_bytes':max(f['bytes'] for f in runtimeframes),
        'selected_additive_compiler_frame_estimates':additive,'recursive_invalidate':{'maximum_simultaneous_invocations':64,'individual_frame_bytes':invalidate,'compiler_frame_product_bytes':64*invalidate},
        'public49U_compiler_frames':[f for f in newframes if any(('::'+name+'(') in f['function'] for name in ('updateChariotRoot49U','readChariotRootAuthoring49U','bindFreshChariotRootParameters49U','initializeChariotRootConstructorRecords49U','initializeChariotRootSharedColdRecords49U'))],
        'scene49U_leaf_compiler_frames':[f for f in newframes if any(('::'+name+'(') in f['function'] for name in ('axes49U','position49U','direction49U','inverseDirection49U','relativeMatrix49U','copyLocal49U','rotateVector49U','matrixQuaternion49U'))],
        'timed_executable_sections':measured,'timed_executable_sha256':digest(build/'benchmark'),'probe_executable_sha256':digest(build/'ordinary-new-probe'),'fixture':fixture,
        'limits':['Object delta versus49T is the three new production units only, with runtime and load-time subsets separate; retained source/header dependencies are hash-identical to49T. No old object is subtracted or replaced. StrictC++17, no test defines, LTO or interpreter linkage. New49U units have no warning suppressions; existing body_scene48g alone uses its documented misleading-indentation suppression.',
            'Host sizeof/alignment and object sections only; no game-memory, process-RAM, FPS, Xbox or whole-game speedup claim. GNU size text includes read-only/unwind metadata; executable text, read-only, GNU data and BSS are separate. The authoring const lookup pointer table occupies504 bytes in.data.rel.ro.local; this relocated read-only data is reported separately from mutable.data. GNU data remains504 bytes, not zero.',
            'Incremental caller payload includes root state/shared/input/parameters/bindings/workspace/diagnostic and one new198 scene node. Existing shared zero/noise and canonical secondary clock owner are not new49U state. Nested workspace fields are already included and must not be counted twice.',
            'Combined caller payload itemizes this53-node,three-palette,5755-vertex arrangement including the full existing SecondaryBodyState49M owner whose time864 alone is consumed. It excludes loader vectors, fixture history/expected hashes, constructor-only workspaces, runtime and inter-record padding. Retained immutable six-motion bytes are separate.',
            'Compiler.su frames are static individual estimates, not measured stack high-water or an exhaustive ABI/runtime stack bound. Selected additive examples are partial paths, not maxima; source recursion may add <=64 invalidate frames. Nested scratch fields are caller storage, not extra stack charges. Host red zones/runtime functions are not inferred from.su.',
            'Direct allocator symbol audit covers all linked production objects. Separate successful-call probes count ordinary scalar/array C++new during complete96-frame continuations; no Cmalloc/aligned allocator interception, ownership or allocation-failure experiment.']}
    dump(BASE/'footprint.json',result)
    print(json.dumps({'stage':'prepared','new_code':result['incremental49U_totals'],'incremental_caller':result['incremental49U_caller_total_bytes'],'workspace':records['ChariotRootWorkspace49U']}),flush=True)
    return result


def percentile(v,p):
    v=sorted(v);i=(len(v)-1)*p;a=int(i);return v[a]+(v[min(a+1,len(v)-1)]-v[a])*(i-a)

def measure(build,args,foot):
    hashes=foot['source_hashes'];assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Inputs changed since prepare; rerun'
    assert digest(build/'benchmark')==foot['timed_executable_sha256'] and digest(build/'ordinary-new-probe')==foot['probe_executable_sha256'],'Executable changed since prepare'
    cpus=sorted(os.sched_getaffinity(0));cpu=args.cpu if args.cpu is not None else (0 if 0 in cpus else cpus[0]);assert cpu in cpus
    cap=64*1024*1024
    def limits():
        os.sched_setaffinity(0,{cpu});resource.setrlimit(resource.RLIMIT_AS,(cap,cap));resource.setrlimit(resource.RLIMIT_CPU,(30,30))
    def execute(binary,mode,histories):
        command=[str(build/binary),str(BASE/'actual-history96.bin'),str(ROOT),str(histories),mode]
        p=subprocess.run(command,cwd=ROOT,text=True,capture_output=True,preexec_fn=limits,timeout=40)
        if p.returncode or p.stderr:raise RuntimeError(f'{command}: exit={p.returncode}\n{p.stdout}{p.stderr}')
        row=dict(part.split('=',1) for part in p.stdout.split());row={k:v if k=='mode' else int(v) for k,v in row.items()}
        e=foot['fixture']['expected_final_hashes']
        assert [row[k] for k in ('final_graph','final_records','final_palettes','final_positions')]==[e[k] for k in ('palette_graph','records','palettes','positions')]
        assert row['retained_motion_bytes']==foot['retained_motion_byte_payload']
        assert row['validated_physical_calls']==1 and row['validated_virtual_calls']==95
        return row,p.stdout
    calibrations=[];counts={};calibration_stdout=[]
    for mode in MODES:
        row,stdout=execute('benchmark',mode,2);calibrations.append(row);calibration_stdout.append(stdout)
        counts[mode]=args.histories or max(2,math.ceil(150000000*2/row['nanoseconds']))
    (BASE/'calibration.stdout').write_text(''.join(calibration_stdout))
    probes=[];probe_stdout=[]
    for mode in MODES:
        row,stdout=execute('ordinary-new-probe',mode,counts[mode]);assert row['allocation_counter_enabled']==1 and row['cpp_new_allocations']==0 and row['nanoseconds']==0
        probes.append(row);probe_stdout.append(stdout)
    (BASE/'ordinary-new-probe.stdout').write_text(''.join(probe_stdout))
    rows=[];stdout=[]
    for sample in range(args.samples):
        for mode in MODES if sample%2==0 else MODES[::-1]:
            row,out=execute('benchmark',mode,counts[mode]);assert row['allocation_counter_enabled']==0;row['sample']=sample;row['nanoseconds_per_frame']=row['nanoseconds']/row['frames'];rows.append(row);stdout.append(out)
    stats={}
    for mode in MODES:
        selected=[r for r in rows if r['mode']==mode];values=[r['nanoseconds_per_frame'] for r in selected]
        assert {r['checksum'] for r in selected}=={p['checksum'] for p in probes if p['mode']==mode}
        stats[mode]={'minimum_ns':min(values),'p05_ns':percentile(values,.05),'p25_ns':percentile(values,.25),'median_ns':statistics.median(values),'p75_ns':percentile(values,.75),'p95_ns':percentile(values,.95),'maximum_ns':max(values),'mean_ns':statistics.mean(values),'sample_duration_ns_range':[min(r['nanoseconds'] for r in selected),max(r['nanoseconds'] for r in selected)]}
    assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Inputs changed during measurement; rerun'
    (BASE/'samples.stdout').write_text(''.join(stdout))
    result={'status':'measured','created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'host_machine':platform.machine(),'host_cpu_model':next((line.split(':',1)[1].strip() for line in Path('/proc/cpuinfo').read_text().splitlines() if line.startswith('model name')),'unreported'),'footprint_sha256':digest(BASE/'footprint.json'),'source_hashes':hashes,'cpu_affinity':[cpu],'available_cpu_affinity':cpus,'normal_virtual_address_limit_bytes':cap,'cpu_seconds_limit_per_process':30,'wall_seconds_timeout_per_process':40,'rounding':'FE_TONEAREST checked at startup and by production calls','compiler':foot['compiler'],'flags':FLAGS,'samples_per_mode':args.samples,'histories_per_sample':counts,'calibrations':calibrations,'statistics':stats,'results':rows,'untimed_ordinary_new_probes':probes,
        'validation':'Before any measurement each executable completes all96frames and checks every stage graph, persistent record, active palette and5755 rigid positions against source-validated trace hashes. Native pre-motion initializer coefficients/graph, root constructor and actual PHY/MOT fresh binding checked separately. Every frame also checks the shared noise state after49R and root graph after49U; each history is one physical entry plus95 persistent virtual calls. All96 expected vertex digests are tied to connected full-mesh FNV before fixture generation. End-of-history graph/record/palette and selected rigid hashes checked after every timed history.',
        'timed_stages':{'root':'Complete admitted49U perframe; one timer pair perframe.49T and49R execute before, and all-three palettes after, outside that frame interval to preserve the exact connected graph, shared-noise and canonical864 history. Probe also counts untimed continuation calls.','animation_visual_root_palette':'Complete49T -> complete49R -> complete49U -> allthree18/14/14-entry49S palettes, one timer pair per96-frame history.','animation_visual_root_palette_rigid':'Same plus all5755 actual local vertices projected everyframe, one timer pair per96-frame history.'},
        'measurement_limits':['Host serial subsystem samples only. No game FPS, Xbox benchmark, whole-game CPU/memory, process RAM or historical speedup claim.','Exact96-frame histories restart from retained initial state outside timing; there is no reset within a history. Cold noise/lazy setup therefore occurs once per96-frame history where naturally reached. No continuous game-loop or unbounded playback claim.','Source-validated control and clock outputs are supplied. Constructor, fixture parsing, asset admission, warmup verification, full hashes and printing are outside timing. Production validation, transactional copies, dirty graph refresh and deformation validating/committing passes remain inside selected intervals.','The root-only timer includes clock-call overhead perframe, unlike aggregate modes. Timers include loop branches/boolean status accumulation; there is no subtractive overhead correction. All samples retained in alternating mode order; CPU pinning does not remove host scheduling/frequency/load effects.','Separate successful-call ordinary-new probe uses the same production objects and counts scalar/array C++new across the complete96-frame continuation. Timed executable contains no allocation counter or test hooks. Loader vectors allocate before measurement.','Linear descriptive percentiles use sorted index(n-1)*p; p95 is not a frame tail-latency guarantee. Calibration targets150ms timed work per sample; each datum is a finite-history average. Virtual-address/CPU caps bound this experiment and do not simulate console hardware.']}
    dump(BASE/'benchmark.json',result)
    print(json.dumps({'stage':'measured','histories':counts,'statistics':stats}),flush=True)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build-dir',type=Path,required=True)
    p.add_argument('--prepare-only',action='store_true');p.add_argument('--benchmark-only',action='store_true')
    p.add_argument('--trace',type=Path,default=ROOT/'evidence/chariotRoot49U/connected/trace.json')
    p.add_argument('--samples',type=int,default=15);p.add_argument('--histories',type=int,default=0);p.add_argument('--cpu',type=int)
    args=p.parse_args();assert args.samples>=15 and args.histories>=0
    assert not(args.prepare_only and args.benchmark_only)
    build=args.build_dir.resolve();foot=json.loads((BASE/'footprint.json').read_text()) if args.benchmark_only else prepare(build,args.trace.resolve())
    if not args.prepare_only:measure(build,args,foot)
if __name__=='__main__':main()
