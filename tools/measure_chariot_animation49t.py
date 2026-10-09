#!/usr/bin/env python3
"""Bounded49T strict object footprint and source-checked serial host hot cost."""
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
from visual.run_chariot49t import local_vertices, SCENES

BASE = ROOT / 'evidence/chariotAnimation49T/budget'
NEW = ['game/source_acos49t', 'game/model_motion49t', 'game/chariot_ik49t', 'game/vehicle_animation49t']
SUPPORT = ['game/scene_graph49p', 'assets/model_geometry49o', 'assets/model_bindings49o',
    'game/vehicle_visual49r', 'game/visual_container49r', 'game/visual_response49r',
    'game/gradient_noise49m', 'game/body_scene48g', 'game/model_palette49s', 'game/rigid_vertices49s']
MODES = ['animation', 'animation_visual_palette', 'animation_visual_palette_rigid']
HARNESS = ROOT / 'tests/native/bench_chariot_animation49t.cpp'
MOT_NAMES = ['50-englt1.mot','45-engrt1.mot','54-01_brklt.mot','55-01_brkrt.mot','56-01_forwd.mot','57-01_back.mot']
ASSET_NAMES = MOT_NAMES + ['48-englt1.psg','43-engrt1.psg','52-chariot.psg','49-englt1.vut','44-engrt1.vut','53-chariot.vut']
ASSET_DIR = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures'
VISUAL_FLOATS = 'frameFactor cachedBodySpeed34 rawB5C objectD34_10 engine68 engine6C engine70 engine74 aeroE4 aeroE8 damage7D0 damage7D8 damage7D4 damage7DC'.split()
PARAMS = 'impact9F0 groundImpact9F4 separation9F8 thrust9FC liftA00 dragA04 springA08 dampingA0C binderSpringA10 yawA14 slideYawA18 slidePitchA1C pitchA20 rollA24 angleReturnA28 angularDampingA2C binderTorqueA30 minTimeA34 maxTimeA38 minAmplitudeA44 maxAmplitudeA48 effectiveSpeedSquaredA54 damageTranslationA58 damageOrientationA5C orientationScaleA64 effectiveInputSpeedA68 verticalLimit7EC'.split()


def dump(path, value): path.write_text(json.dumps(value, indent=2) + '\n')
def pack(words): return struct.pack('<'+'I'*len(words), *[int(x)&0xffffffff for x in words])
def fnv(data):
    h=1469598103934665603
    for x in data: h=((h^x)*1099511628211)&0xffffffffffffffff
    return h

def graph_words(graph):
    out=[]
    for i,n in enumerate(graph):
        assert n['index']==i
        out += n['local_bits']+n['world_bits']+[n['flags190'],n['parent180'],n['first_child184'],n['next_sibling188']]
    return out

def live_words(row):
    s=row['animation_state']; h=row['animation_shared']; v=row['visual_state']; n=row['shared']
    out=sum([s[k+'_filter_bits'] for k in ('left','right','pose','blend')],[])
    out += [s[k] for k in ('leftB04','rightB0C','poseB1C','blendB14','chariot_model18','left_model18','right_model18')]
    out += [h[k] for k in ('enable_guard3D60F4','enable434748','interpolation_value4332F0','pair_value433478','interpolation_guard3D5D8C','pair_guard3D5DC4')]
    out += h['ik_sentinel443450']+h['ik_sentinel443458']+[h['libm'][k] for k in ('mode','errno','default_matherr','cold_nan_record')]
    out += v['upC20']+v['publishedUpAF0']
    for side in ('right','left'):
        for k in ('position','velocity','omega','quaternion'): out += v[side+'_'+k]
    out += [v[k] for k in ('phase8B0','phase900','phase950','phase9A0')]
    out += sum(n['lazy_globals'],[])+n['orientation_sentinel']+n['zero434630']
    out += n['permutation']+n['gradient_bits']+[n['noise_scratch_cell'],n['noise_initialized'],n['random_state']]
    return out

def frame_words(row):
    a=row['animation_input'];v=row['visual_input_bits']
    out=[a[k] for k in ('phase','animation_delta23C_bits','aeroE4_bits','aeroE8_bits','chariot191','chariot1B0','left191','right191','body340_present','mid_records_absent')]+a['body_accelerationC0_bits']
    out += [v[k] for k in ('phase','vehicle834','vehicle844','vehicle40','applicationByte58','probe108','aeroD8','aeroDC','vehicleCFC','vehicleD00')]
    out += [v[k] for k in VISUAL_FLOATS]+v['probeVector1D0']+v['impactBA0']
    return out

def palette_words(row):
    out=[]
    for side in ('chariot','left','right'):
        p=row[side+'_palette'];out+=p['root_world_bits']+sum(p['node_world_bits'],[])+[p['entry_count']]
    return out

def position_bytes(row, vertices):
    out=bytearray()
    for side,vs in zip(('chariot','left','right'), vertices):
        matrices=[struct.unpack('<16f',pack(m)) for m in row[side+'_palette']['node_world_bits']]
        for xyz,bone in vs:
            m=matrices[bone];p=[]
            for axis in range(3):
                total=0.0
                for k in range(3): total=total+xyz[k]*m[k*4+axis]
                p.append(total+m[12+axis])
            out.extend(struct.pack('<3f',*p))
    return out

def prepare_fixture(trace_path):
    trace=json.loads(trace_path.read_text());assert len(trace['frames'])==96
    assert trace['summary']['passed'] and trace['summary']['source_substitutions']==0
    initial=trace['initial']; vertices=[local_vertices(ROOT/p) for p in SCENES]
    assert [len(v) for v in vertices]==[2884,1411,1460]
    out=bytearray(b'RR49TB01')+pack([96])+pack(graph_words(initial['pre_motion_constructor_graph']))
    out+=struct.pack('<Q',fnv(pack(graph_words(initial['post_constructor_graph']))))
    out+=pack(graph_words(initial['graph']))+pack(live_words(initial))
    for side in ('left','right'):
        r=initial['persistent_ik_records'][side]
        out+=pack(r['joint_indices']+[r['mode10']]+r['coefficient_bits']+r['saved_elbow_local30']+[r['debug40']]+r['axis50'])
    p=initial['visual_parameters_bits'];out+=pack([p[k] for k in PARAMS]+p['rightBase2C0']+p['leftBase2B0'])
    for vs in vertices:
        for xyz,bone in vs: out+=struct.pack('<3dI',*xyz,bone)
    expected=[]
    for row in trace['frames']:
        hashes=[int(row['after21A000_graph_fnv64'],16),int(row['after49R_graph_fnv64'],16),fnv(pack(graph_words(row['graph']))),fnv(pack(live_words(row))),fnv(pack(palette_words(row))),fnv(position_bytes(row,vertices))]
        expected.append(hashes);out+=pack(frame_words(row))+struct.pack('<6Q',*hashes)
    path=BASE/'actual-history96.bin';path.write_bytes(out)
    result={'trace_path':str(trace_path.relative_to(ROOT)),'trace_sha256':digest(trace_path),
        'fixture_bytes':len(out),'fixture_sha256':digest(path),'frames':96,'graph_nodes':52,
        'palette_entries':[18,14,14],'rigid_vertices':[2884,1411,1460],
        'source_assets':[{'path':str(p.relative_to(ROOT)),'bytes':p.stat().st_size,'sha256':digest(p)} for p in [*[ROOT/s for s in SCENES],*[ASSET_DIR/n for n in ASSET_NAMES]]],
        'expected_final_hashes':dict(zip(['animation_graph','visual_graph','palette_graph','records','palettes','positions'],expected[-1])),
        'all_frame_expected_hashes':expected,
        'boundary':'Exact retained96-frame source-validated inputs and initial persistent records. Native constructor separately verified on pre-motion authored graph. Current body acceleration, stationary main/Chariot roots, component/probe state and control results are supplied; no controls, clock,21A660, body physics, contacts, reset lifecycle, authentic spawn,FrameReady or rendering execution.'}
    dump(BASE/'fixture.json',result);return result


def prepare(build,trace):
    BASE.mkdir(parents=True,exist_ok=True);build.mkdir(parents=True,exist_ok=False)
    before=snapshot_sources(); hashes={};objects={};frames=[];commands=[]
    for name in NEW+SUPPORT:
        stem=Path(name).name;source=ROOT/'src'/(name+'.cpp');obj=build/(stem+'.o');dep=build/(stem+'.d')
        command=['g++',*FLAGS,*(['-Wno-misleading-indentation'] if name=='game/body_scene48g' else []),'-MMD','-MF',str(dep),'-c',str(source),'-o',str(obj)];commands.append(command);run(command)
        for path in dependency_paths(dep):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
        measured,summary,detail=sections(obj);undefined=run(['nm','-u','-C',str(obj)])
        measured['direct_allocator_references']=[s for s in undefined.splitlines() if any(t in s for t in ('operator new','malloc','calloc','realloc','aligned_alloc'))]
        assert not measured['direct_allocator_references'],name
        measured.update(source_sha256=digest(source),object_sha256=digest(obj));objects[name]=measured
        for suffix,content in [('sections',detail),('size',summary),('undefined',undefined),('symbols',run(['nm','-S','--size-sort','--radix=d','-C',str(obj)]))]:
            (BASE/(stem+'-'+suffix+'.txt')).write_text(content)
        su=(build/(stem+'.su')).read_text();(BASE/(stem+'.su')).write_text(su)
        for line in su.splitlines():
            f=line.split('\t');frames.append({'unit':name,'function':f[0],'bytes':int(f[1]),'kind':f[2]})
    for binary,extra in [('benchmark',[]),('ordinary-new-probe',['-DPROBE49T'])]:
        dep=build/(binary+'.d');command=['g++',*FLAGS,*extra,'-I',str(ROOT/'src'),'-MMD','-MF',str(dep),str(HARNESS),*[str(build/(Path(n).name+'.o')) for n in NEW+SUPPORT],'-o',str(build/binary)]
        commands.append(command);run(command)
        for path in dependency_paths(dep):
            if path.is_relative_to(ROOT):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    for path in build.glob('*bench_chariot_animation49t.su'):(BASE/path.name).write_text(path.read_text())
    stdout=run([str(build/'benchmark'),'--sizes']);(BASE/'record-sizes.stdout').write_text(stdout)
    records={r.split()[0]:int(r.split()[1]) for r in stdout.splitlines()};alignments={r.split()[0]:int(r.split()[2]) for r in stdout.splitlines()}
    baseline=ROOT.parent/'CLOUD-MILESTONE-49S-ANIMATEDENGINEVIEW-MANIFEST.json'
    listed=json.loads(baseline.read_text())['files'];prefix='RacerRevengePC/'
    prior={f['path'][len(prefix):]:f['sha256'] for f in listed if f['path'].startswith(prefix+'src/')}
    prior_units={n:prior['src/'+n+'.cpp']==digest(ROOT/'src'/(n+'.cpp')) for n in SUPPORT}
    assert all(prior_units.values()),'Reused production source differs from49S'
    prior_dependencies={n:digest(ROOT/n)==h for n,h in hashes.items() if n.startswith('src/') and n in prior}
    assert all(prior_dependencies.values())
    assert all('src/'+n+'.cpp' not in prior for n in NEW)
    keys=['gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','data_bytes','bss_bytes']
    incremental=['VehicleAnimationState49T','VehicleAnimationShared49T','VehicleAnimationInput49T','VehicleAnimationBindings49T','ChariotIkPair49T','VehicleAnimationWorkspace49T','VehicleAnimationDiagnostic49T']
    all_caller=incremental+['SceneNodes52','SceneGraph49P','VehicleVisualState49R','VehicleVisualParameters49R','VehicleVisualInput49R','VisualSceneBindings49R','VisualShared49R','CoupledNoiseContext49M','VehicleVisualWorkspace49R','VehicleVisualDiagnostic49R','MotionViews6','MotionBindings3','Palettes3','PaletteInputs3','PaletteNodes46','PaletteIds46','ModelPaletteWorkspace49S','ModelPaletteDiagnostic49S','RigidVertices5755','RigidPositions5755','RigidVertexDiagnostic49S']
    newframes=[f for f in frames if f['unit'] in NEW]
    def frame(unit,fragment):
        found=[f['bytes'] for f in frames if f['unit']==unit and fragment in f['function'] and '<lambda' not in f['function']]
        assert len(found)==1,(unit,fragment,found)
        return found[0]
    additive_chains=[
        [('game/vehicle_animation49t','advanceVehicleAnimation49T('),('game/chariot_ik49t','solveChariotHands49T('),('game/chariot_ik49t','::solve('),('game/chariot_ik49t','::inverseVector('),('game/scene_graph49p','refreshSceneWorld49P('),('game/scene_graph49p','validateSceneGraph49P(')],
        [('game/vehicle_animation49t','advanceVehicleAnimation49T('),('game/model_motion49t','sampleModelMotionBlend49T('),('game/model_motion49t','::sample('),('game/model_motion49t','Eval::position('),('assets/model_geometry49o','readPositionKey49O(')],
        [('game/model_palette49s','projectModelPalette49S('),('game/scene_graph49p','refreshSceneWorld49P('),('game/scene_graph49p','validateSceneGraph49P(')]]
    additive=[{'chain':[u+':'+f for u,f in chain],'sum_compiler_frame_bytes':sum(frame(u,f) for u,f in chain)} for chain in additive_chains]
    invalidate=frame('game/scene_graph49p','::invalidate(')
    setter_chain=[('game/vehicle_animation49t','advanceVehicleAnimation49T('),('game/chariot_ik49t','solveChariotHands49T('),('game/chariot_ik49t','::solve('),('game/scene_graph49p','setLocalMatrix49P('),('game/scene_graph49p','::afterLocalWrite(')]
    additive.append({'chain':[u+':'+f for u,f in setter_chain]+['invalidate x <=64'],'sum_compiler_frame_bytes':sum(frame(u,f) for u,f in setter_chain)+64*invalidate})
    fixture=prepare_fixture(trace)
    hashes.update({str(p.relative_to(ROOT)):digest(p) for p in [HARNESS,Path(__file__),ROOT/'tools/measure_scene_projection49p.py',ROOT/'tools/visual/run_chariot49t.py',trace,BASE/'actual-history96.bin',*[ROOT/s for s in SCENES],*[ASSET_DIR/n for n in ASSET_NAMES]]})
    check_stable_sources(before,hashes)
    symbols=run(['nm','-C',str(build/'benchmark')]);(BASE/'timed-executable-symbols.txt').write_text(symbols)
    for forbidden in ['Machine','original::','oracle','trace::','stage_trace','countAllocations']:
        assert forbidden not in symbols,forbidden
    measured,size,detail=sections(build/'benchmark');(BASE/'timed-executable-size.txt').write_text(size);(BASE/'timed-executable-sections.txt').write_text(detail)
    result={'status':'measured','compiler':run(['g++','--version']).splitlines()[0],'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':{'game/body_scene48g':['-Wno-misleading-indentation']},'compile_commands':commands,'source_hashes':hashes,'objects':objects,'frames':frames,
        'incremental49T_units':NEW,'reused49S_or_earlier_support':SUPPORT,'incremental49T_totals':{k:sum(objects[n][k] for n in NEW) for k in keys},
        'baseline49S_manifest':{'path':str(baseline),'sha256':digest(baseline),'reused_units_identical':prior_units,'all_retained_dependencies_identical':prior_dependencies},
        'record_bytes':records,'record_alignments':alignments,'incremental49T_caller_records':{n:records[n] for n in incremental},'incremental49T_caller_total_bytes':sum(records[n] for n in incremental),
        'combined_caller_records':{n:records[n] for n in all_caller},'combined_caller_payload_bytes_excluding_motion_bytes_and_padding':sum(records[n] for n in all_caller),
        'retained_motion_byte_payload':sum((ASSET_DIR/n).stat().st_size for n in MOT_NAMES),
        'workspace_composition':{'VehicleAnimationWorkspace49T':records['VehicleAnimationWorkspace49T'],'included_nodes64':records['SceneNodes64'],'included_state':records['VehicleAnimationState49T'],'included_shared':records['VehicleAnimationShared49T'],'included_visual_shared':records['VisualShared49R'],'included_motion_workspace':records['ModelMotionWorkspace49T'],'included_ik_workspace':records['ChariotIkWorkspace49T']},
        'maximum_individual49T_compiler_frame_bytes':max(f['bytes'] for f in newframes),'selected_additive_compiler_frame_estimates':additive,'recursive_invalidate':{'maximum_simultaneous_invocations':64,'individual_frame_bytes':invalidate},
        'public49T_compiler_frames':[f for f in newframes if '<lambda' not in f['function'] and any('rr::game::'+n+'(' in f['function'] for n in ['sourceAcos49T','initializeChariotIk49T','solveChariotHands49T','sampleModelMotion49T','sampleModelMotionBlend49T','advanceVehicleAnimation49T','initializeVehicleAnimationFilters49T'])],
        'timed_executable_sections':measured,'timed_executable_sha256':digest(build/'benchmark'),'probe_executable_sha256':digest(build/'ordinary-new-probe'),'fixture':fixture,
        'limits':['Object delta versus49S is the four new production units only; retained source/header dependencies are hash-identical to49S. No old object is subtracted or replaced. Ordinary strictC++17 with no test defines, LTO or interpreter linkage. All four new49T units have no warning suppressions; existing body_scene48g alone uses its required misleading-indentation suppression.',
            'Host sizeof/alignment and object sections only; no game-memory,process-RAM,FPS,Xbox or whole-game speedup claim. GNU size text includes read-only/unwind metadata; executable text,read-only,data,BSS are separate.',
            'Incremental caller payload includes new49T state/shared/input/bindings/persistentIK/workspace/diagnostic. Existing graph, shared49R state, retained immutable asset views/bytes and later palette/rigid records are separately itemized. Nested workspace components must not be counted twice.',
            'Combined caller payload describes this52-node,three-palette,5755-vertex arrangement. It excludes loader vectors, fixture history/expected hashes, constructor-only workspace, runtime, frame padding, and process memory. Immutable six motion byte payload is separate. Extra Chariot mesh/palette storage is integration payload, not allocated by49T.',
            'Compiler.su frames are static individual function estimates, not stack high-water or an exhaustive ABI/runtime stack bound. Nested scratch fields are caller storage; they are not extra stack charges. Host red zones and runtime library calls are not inferred from.su.',
            'Direct allocator symbol audit covers linked production objects. Separate probes count ordinary scalar/array C++new during successful hot calls and constructor; no Cmalloc/aligned allocator interception, ownership or allocation-failure experiment.']}
    dump(BASE/'footprint.json',result)
    print(json.dumps({'stage':'prepared','new_code':result['incremental49T_totals'],'incremental_caller':result['incremental49T_caller_total_bytes'],'workspace':records['VehicleAnimationWorkspace49T']}),flush=True)
    return result


def percentile(v,p):
    v=sorted(v);i=(len(v)-1)*p;a=int(i);return v[a]+(v[min(a+1,len(v)-1)]-v[a])*(i-a)

def measure(build,args,foot):
    hashes=foot['source_hashes'];assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Inputs changed since prepare; rerun'
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
        assert row['retained_motion_bytes']==foot['retained_motion_byte_payload'];return row,p.stdout
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
        'validation':'Before any measurement each executable completes all96frames and checks every stage graph, persistent record, active palette and5755 rigid positions against source-validated trace hashes. Native pre-motion initializer coefficients and graph checked separately. End-of-history graph/record/palette and selected rigid hashes checked after every timed history.',
        'timed_stages':{'animation':'Complete admitted49T perframe; one timer pair perframe.49R and all-three palettes execute outside that frame interval to preserve exactly the connected subsequent-state history. Probe also counts these untimed continuation calls.','animation_visual_palette':'Complete49T -> complete49R -> allthree18/14/14-entry49S palettes, one timer pair per96-frame history.','animation_visual_palette_rigid':'Same plus all5755 actual local vertices projected everyframe, one timer pair per96-frame history.'},
        'measurement_limits':['Host serial subsystem samples only. No game FPS, Xbox benchmark, whole-game CPU/memory, process RAM or historical speedup claim.','Exact96-frame histories restart from retained initial state outside timing; there is no reset within a history. Cold noise/lazy setup therefore occurs once per96-frame history where naturally reached. No continuous game-loop or unbounded playback claim.','Source-validated control and clock outputs are supplied. Constructor, fixture parsing, asset admission, warmup verification, full hashes and printing are outside timing. Production validation, transactional copies, dirty graph refresh and deformation validating/committing passes remain inside selected intervals.','The animation-only timer includes clock-call overhead perframe, unlike aggregate modes. Timers include loop branches/boolean status accumulation; there is no subtractive overhead correction. All samples retained in alternating mode order; CPU pinning does not remove host scheduling/frequency/load effects.','Separate successful-call ordinary-new probe uses the same production objects and counts scalar/array C++new across the complete96-frame continuation. Timed executable contains no allocation counter or test hooks. Loader vectors allocate before measurement.','Linear descriptive percentiles use sorted index(n-1)*p; p95 is not a frame tail-latency guarantee. Calibration targets150ms timed work per sample; each datum is a finite-history average. Virtual-address/CPU caps bound this experiment and do not simulate console hardware.']}
    dump(BASE/'benchmark.json',result)
    print(json.dumps({'stage':'measured','histories':counts,'statistics':stats}),flush=True)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build-dir',type=Path,required=True)
    p.add_argument('--prepare-only',action='store_true');p.add_argument('--benchmark-only',action='store_true')
    p.add_argument('--trace',type=Path,default=ROOT/'evidence/chariotAnimation49T/connected/trace.json')
    p.add_argument('--samples',type=int,default=15);p.add_argument('--histories',type=int,default=0);p.add_argument('--cpu',type=int)
    args=p.parse_args();assert args.samples>=5 and args.histories>=0
    assert not(args.prepare_only and args.benchmark_only)
    build=args.build_dir.resolve();foot=json.loads((BASE/'footprint.json').read_text()) if args.benchmark_only else prepare(build,args.trace.resolve())
    if not args.prepare_only:measure(build,args,foot)
if __name__=='__main__':main()
