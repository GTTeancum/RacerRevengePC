#!/usr/bin/env python3
"""Bounded native-only49W staged144-frame runtime and host footprint.

Prepare first; measure separately only in an approved quiet window. Cold
initialization reuses the unchanged verified exporter. No game, PC, RAM,
ownership/collision-capacity diagnostic, installation or historical edit.
"""
import argparse, datetime, hashlib, importlib.util, json, math, os, platform
import resource, statistics, subprocess, sys, types
from pathlib import Path
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[2]
BASE=ROOT/'evidence/frameBridge49W/budget'
CONNECTED=ROOT/'evidence/frameBridge49W/connected'
sys.path.insert(0,str(ROOT/'tools'))
from measure_scene_projection49p import sections, dependency_paths, digest, snapshot_sources, check_stable_sources
SPEC=importlib.util.spec_from_file_location('approved49w',ROOT/'tools/visual/run_vehicle_frame49w.py')
APPROVED=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(APPROVED)
SOURCES=APPROVED.SOURCES[1:]
NEW=['src/game/vehicle_frame49w.cpp','src/game/graph_body49w.cpp','src/game/pod_shape49w.cpp','src/assets/pod_shape49w.cpp']
FLAGS=['-std=c++17','-O2','-Wall','-Wextra','-Wpedantic','-Werror','-Wno-misleading-indentation','-fno-fast-math','-frounding-math','-ffp-contract=off','-fstack-usage']
SCENARIOS=['synthetic-support','no-support']
KEYS=['gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','relocated_readonly_data_bytes','mutable_data_bytes','data_bytes','bss_bytes']
LIMITS=[
 'Host ABI, sizeof/alignment, object sections and serial subsystem timing only; not process RAM, console footprint, gameplay FPS, authentic race/spawn or historical speedup.',
 'Exact144-frame native input/graphics/body advance, including native49I factor preparation, explicit preserving player/base/children/world cuts, declared synthetic354 support or zero support, supplied no-hit query, and late type0shape. These are not recovered terrain contacts or a complete original frame.',
 'One canonical mutable state and one disjoint transaction workspace persist within each history. Immutable authored descriptors, MOT bytes, IK coefficients and retained diagnostic parameters are bound once. Only complete144-frame histories restart, outside timing.',
 'Palette/5755vertex projection is a separate disposable-snapshot diagnostic and excluded. Initial parsing, source interpreter, trace serialization/validation, fixture reads, reset, comparison, hashing and printing are excluded from timed intervals.',
 'Cold fixture stores host-native ABI images, not a portable wire format. GCC clear_padding changes only unobserved padding. All serialized logical fields at the initial/before/graphics/after cuts are compared to approved JSON. Full-state images additionally check native replay consistency for owner fields omitted from that projection; those extra fields are not independently present in JSON. Endian/sizes/source/compiler identity are pinned. No state field normalization or invented initialization.',
 'Compiler.su records are static per-function frames, not measured stack high-water or an exhaustive ABI bound. Selected sums are partial nested-call examples. Stack is additional to caller payload; recursion and retained runtime/library stack must be considered separately. Cold authoring/exporter and renderer process RSS are excluded.',
 'Ordinary scalar/array C++new is counted in a separate untimed executable using identical production objects. Every linked production object is also audited for direct allocation references. No allocation-failure, Cmalloc/aligned interception, ownership or capacity-alias experiment.',
 'Cold PODshape decoder is a new production unit with permitted load-time allocations; it is separately itemized and excluded from the staged runtime dependency closure.'
]
def dump(path,value):path.write_text(json.dumps(value,indent=2,sort_keys=True)+'\n')
def run(command,name,timeout=None):
    r=subprocess.run([str(x)for x in command],cwd=ROOT,text=True,capture_output=True,timeout=timeout)
    (BASE/(name+'.stdout')).write_text(r.stdout);(BASE/(name+'.stderr')).write_text(r.stderr)
    if r.returncode:raise RuntimeError(name+': '+r.stdout[-1500:]+r.stderr[-10000:])
    return r.stdout
def stem(path):return path.replace('/','_').replace('.cpp','')
def symbols(obj,defined):
    result=subprocess.check_output(['nm','-g','--defined-only' if defined else '-u',str(obj)],text=True)
    return {line.split()[-1] for line in result.splitlines() if line.split()}
def parse(text):return {k:v if k=='scenario' else int(v)for k,v in (x.split('=',1)for x in text.split())}
def traces():return [CONNECTED/'trace.json',CONNECTED/'no-support/trace.json']
def inputs():
    imported=[];pending=[APPROVED,sys.modules['measure_scene_projection49p']];seen=set()
    while pending:
        module=pending.pop()
        if id(module) in seen:continue
        seen.add(id(module));name=getattr(module,'__file__',None)
        if not name:continue
        path=Path(name).resolve()
        if not path.is_relative_to(ROOT):continue
        imported.append(path)
        pending.extend(value for value in vars(module).values()if isinstance(value,types.ModuleType))
    return [*imported,*traces(),CONNECTED/'exact-goldens49w.txt',CONNECTED/'no-support/exact-goldens49w.txt',CONNECTED/'authored_inputs49w.h',ROOT/'tools/visual/export_vehicle_frame49w.cpp',ROOT/'evidence/frameBridge49W/frame/frame_fixture.h',*sorted((ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures').glob('*')),ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy',ROOT/'evidence/frameBridge49W/shape/podboxf.col',*sorted((ROOT/'tools/budget').glob('*49w*'))]
def prepare(build):
    BASE.mkdir(parents=True,exist_ok=True);build.mkdir(parents=True,exist_ok=False)
    before=snapshot_sources();hashes={str(p.relative_to(ROOT)):digest(p) for p in inputs() if p.is_file()};commands=[];objects={};paths={};frames=[]
    for name in SOURCES:
        path=ROOT/name;obj=build/(stem(name)+'.o');dep=obj.with_suffix('.d');paths[name]=obj
        cmd=['g++',*FLAGS,'-MMD','-MF',dep,'-c',path,'-o',obj];commands.append([str(x)for x in cmd]);run(cmd,'compile-'+stem(name))
        for p in dependency_paths(dep):hashes[str(p.relative_to(ROOT))]=digest(p)
        measured,summary,detail=sections(obj);undefined=run(['nm','-u','-C',obj],stem(name)+'-undefined')
        measured['relocated_readonly_data_bytes']=sum(v for k,v in measured['sections'].items() if k.startswith('.data.rel.ro'))
        measured['mutable_data_bytes']=sum(v for k,v in measured['sections'].items() if (k=='.data' or k.startswith('.data.')) and not k.startswith('.data.rel.ro'))
        measured['direct_allocator_references']=[s for s in undefined.splitlines()if any(x in s for x in ('operator new','malloc','calloc','realloc','aligned_alloc'))]
        measured.update(source_sha256=digest(path),object_sha256=digest(obj));objects[name]=measured
        (BASE/(stem(name)+'-sections.txt')).write_text(detail);(BASE/(stem(name)+'-size.txt')).write_text(summary)
        run(['nm','-S','--size-sort','--radix=d','-C',obj],stem(name)+'-symbols')
        su=obj.with_suffix('.su').read_text();(BASE/(stem(name)+'.su')).write_text(su)
        for row in su.splitlines():f=row.split('\t');frames.append({'unit':name,'function':f[0],'bytes':int(f[1]),'kind':f[2]})
    harness={}
    for name,source,extra in [('cold','prepare_frame49w.cpp',['-std=c++20','-Wno-unused-function']),('benchmark','bench_frame49w.cpp',[]),('ordinary-new-probe','bench_frame49w.cpp',['-DPROBE49W'])]:
        obj=build/(name+'.o');dep=obj.with_suffix('.d');harness[name]=obj
        cmd=['g++',*FLAGS,*extra,'-I',ROOT/'src','-MMD','-MF',dep,'-c',ROOT/'tools/budget'/source,'-o',obj];commands.append([str(x)for x in cmd]);run(cmd,'compile-'+name)
        for p in dependency_paths(dep):hashes[str(p.relative_to(ROOT))]=digest(p)
        su=obj.with_suffix('.su').read_text();(BASE/(name+'.su')).write_text(su)
    cmd=['g++',harness['cold'],*paths.values(),'-lz','-o',build/'cold'];commands.append([str(x)for x in cmd]);run(cmd,'link-cold')
    run([build/'cold',BASE],'cold-fixture',300)
    fixture=json.loads((BASE/'fixture-validation.json').read_text());assert fixture['passed']
    for scenario,path in zip(fixture['scenarios'],traces()):
        goldens=(path.parent/'exact-goldens49w.txt').read_text().splitlines();assert len(goldens)==144
        for row,line in zip(scenario['hashes'],goldens):
            frame,mesh,graph,noise=line.split();assert int(frame)==row['frame']
            assert int(graph)==row['graph_fnv64'] and int(noise)==row['noise_fnv64'],'approved exact graph/noise hash'
        scenario['approved_graph_noise_hashes_verified']=144
    # Resolve the actual native-only link closure from the harness. Cold-only
    # asset decoders and source interpreter translation units cannot enter it.
    defs={name:symbols(obj,True)for name,obj in paths.items()};refs={name:symbols(obj,False)for name,obj in paths.items()}
    needed=symbols(harness['benchmark'],False);provided=symbols(harness['benchmark'],True);linked=[]
    while True:
        added=[]
        for symbol in sorted(needed-provided):
            candidates=[n for n in SOURCES if n not in linked and n not in added and symbol in defs[n]]
            if candidates:added.append(candidates[0]);provided|=defs[candidates[0]]
        if not added:break
        for n in added:linked.append(n);needed|=refs[n]
    assert all(n in linked for n in NEW[:3]);assert NEW[3] not in linked
    assert not any(objects[n]['direct_allocator_references']for n in linked),'native production allocation reference'
    for name in ['benchmark','ordinary-new-probe']:
        cmd=['g++',harness[name],*[paths[n]for n in linked],'-o',build/name];commands.append([str(x)for x in cmd]);run(cmd,'link-'+name)
    native_symbols=run(['nm','-C',build/'benchmark'],'timed-executable-symbols')
    for prohibited in ('original::','original49','oracle','Machine','frame49w_test','nlohmann','chariotlifecycle49v::','counting','allocated'):
        assert prohibited not in native_symbols,prohibited
    preflights=[]
    for binary in ['benchmark','ordinary-new-probe']:
        for scenario in SCENARIOS:
            # No timed histories during preparation. The separate probe runs
            # one whole history, without a clock, to establish zero hot new.
            h=1 if binary=='ordinary-new-probe' else 0
            r=parse(run([build/binary,BASE/'actual-history144.bin',h,scenario],'preflight-'+binary+'-'+scenario,60));assert r['cpp_new_allocations']==0;preflights.append(r)
    raw=run([build/'benchmark','--sizes'],'record-sizes');records={r.split()[0]:int(r.split()[1])for r in raw.splitlines()};align={r.split()[0]:int(r.split()[2])for r in raw.splitlines()}
    harness_frames=[]
    for row in (BASE/'benchmark.su').read_text().splitlines():
        f=row.split('\t');harness_frames.append({'unit':'native-benchmark-harness','function':f[0],'bytes':int(f[1]),'kind':f[2]})
    def frame(unit,fragment):
        found=[f for f in frames+harness_frames if f['unit']==unit and fragment in f['function'] and '<lambda' not in f['function']]
        assert len(found)==1,(unit,fragment,found);return found[0]
    native='src/game/vehicle_frame49w.cpp';scene='src/game/scene_graph49p.cpp';body='src/game/graph_body49w.cpp';chariot='src/game/chariot_scene49u.cpp';harness_name='native-benchmark-harness'
    chains=[[(harness_name,'budget49w::substep('),(native,'integrateVehicleMain49W('),(scene,'refreshSceneWorld49P('),(scene,'validateSceneGraph49P(')],
      [(harness_name,'budget49w::substep('),(native,'integrateVehicleMain49W('),(body,'advanceGraphBody49W('),(body,'pointGraph49W('),(scene,'refreshSceneWorld49P('),(scene,'validateSceneGraph49P(')],
      [(harness_name,'budget49w::graphics('),(native,'graphicsVehicleFrame49W('),('src/game/chariot_lifecycle49v.cpp','updateChariotLifecycle49V('),(chariot,'position49U('),(chariot,'::refresh('),(scene,'refreshSceneWorld49P('),(scene,'validateSceneGraph49P(')]]
    additive=[]
    for chain in chains:
        entries=[frame(u,f)for u,f in chain];additive.append({'members':entries,'sum_compiler_frame_bytes':sum(e['bytes']for e in entries),'scope':'Selected potentially nested path, not an exhaustive stack bound; outer harness main and system/library frames excluded.'})
    recursion=frame(scene,'::invalidate(')
    hash_paths=[BASE/'actual-history144.bin',BASE/'fixture-validation.json'];hashes.update({str(p.relative_to(ROOT)):digest(p)for p in hash_paths});check_stable_sources(before,hashes)
    for name,h in hashes.items():assert digest(ROOT/name)==h,name
    measure,summary,detail=sections(build/'benchmark');(BASE/'timed-executable-sections.txt').write_text(detail);(BASE/'timed-executable-size.txt').write_text(summary)
    disjoint={'canonical_state':records['VehicleFrameState49W'],'transaction_workspace':records['VehicleFrameWorkspace49W'],'diagnostic':records['VehicleFrameDiagnostic49W'],'native_timing_state':records['FrameTimingState49I'],'sampled_timestamp':4,'immutable_descriptors_MOT_rows_IK':records['b::Immutable'],'six_native_MOT_views':6*records['MotionView49O'],'immutable_MOT_byte_payload':preflights[0]['retained_motion_bytes']}
    state_parts={'nodes53':53*records['SceneNode49P']}
    for name in ['CoupledBodyState49M','SecondaryActivationSupplement49V','GraphBodyExtraLanes49W','VehicleAnimationState49T','VehicleAnimationShared49T','VehicleVisualState49R','VisualShared49R','ChariotLifecycleState49V','ChariotLifecycleShared49V','CoupledNoiseContext49M','PodShapeState49W','VehicleFrameFields49W','VehicleFramePhase49W']:
        state_parts[name]=(2 if name in ['SecondaryActivationSupplement49V','GraphBodyExtraLanes49W']else 1)*records[name]
    result={'status':'prepared','compiler':run(['g++','--version'],'compiler').splitlines()[0],'target':run(['g++','-dumpmachine'],'target').strip(),'flags':FLAGS,'commands':commands,'source_hashes':hashes,'production_sources_unchanged':True,'objects':objects,'linked_native_production_units':linked,'new49W_units':NEW,'cold_only_production_units':[n for n in SOURCES if n not in linked],
      'new49W_object_totals':{k:sum(objects[n][k]for n in NEW)for k in KEYS},'new49W_runtime_object_totals':{k:sum(objects[n][k]for n in NEW[:3])for k in KEYS},'native_linked_object_totals':{k:sum(objects[n][k]for n in linked)for k in KEYS},
      'record_bytes':records,'record_alignments':align,'disjoint_runtime_records':disjoint,'disjoint_runtime_payload_bytes':sum(disjoint.values()),'actual_mutable_Running_allocation_bytes':records['b::Running'],
      'state_composition_included_not_added':state_parts,'state_inter_member_padding_bytes':records['VehicleFrameState49W']-sum(state_parts.values()),
      'workspace_composition_included_not_added':{n:records[n]for n in ['VehicleFrameState49W','VehicleAnimationWorkspace49T','VehicleVisualWorkspace49R','ChariotLifecycleWorkspace49V','GraphBodyWorkspace49W']},
      'harness_only_memory':{'immutable_restart_state_bytes':records['VehicleFrameState49W'],'expected_two_scenario_phase_snapshots_bytes':2*144*records['b::Expected'],'fixture_owner_bytes_excludes_vector_allocations':records['Fixture'],'six_vector_descriptor_bytes_included_in_fixture':6*records['std::vector<std::uint8_t>'],'binary_fixture_disk_bytes':(BASE/'actual-history144.bin').stat().st_size},
      'production_stack_frames':frames,'harness_stack_frames':harness_frames,'selected_additive_compiler_frame_estimates':additive,'recursive_scene_invalidation':{'node_capacity_bound':64,'current_graph_nodes':53,'individual_frame_bytes':recursion['bytes'],'capacity_frame_product_bytes':64*recursion['bytes'],'scope':'Conservative capacity recursion product alone, not an additive whole-program stack bound.'},'maximum_native_individual_compiler_frame_bytes':max(f['bytes']for f in frames if f['unit']in linked),'preflight':preflights,'fixture_validation':fixture,'timed_executable_sections':measure,'timed_executable_sha256':digest(build/'benchmark'),'probe_executable_sha256':digest(build/'ordinary-new-probe'),'limits':LIMITS}
    dump(BASE/'footprint.json',result);print(json.dumps({'stage':'prepared','native_units':len(linked),'runtime_payload_bytes':result['disjoint_runtime_payload_bytes'],'new_runtime_text':result['new49W_runtime_object_totals']['executable_text_bytes']}),flush=True);return result
def percentile(v,p):
    s=sorted(v);x=(len(s)-1)*p;a=int(x);return s[a]+(s[min(a+1,len(s)-1)]-s[a])*(x-a)
def measure(build,args,foot):
    hashes=foot['source_hashes'];assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Input changed after preparation'
    for name,key in [('benchmark','timed_executable_sha256'),('ordinary-new-probe','probe_executable_sha256')]:assert digest(build/name)==foot[key]
    cpus=sorted(os.sched_getaffinity(0));cpu=args.cpu if args.cpu is not None else cpus[0];assert cpu in cpus
    def limits():os.sched_setaffinity(0,{cpu});resource.setrlimit(resource.RLIMIT_AS,(64*1024*1024,64*1024*1024));resource.setrlimit(resource.RLIMIT_CPU,(30,30))
    def execute(binary,scenario,histories,name):
        cmd=[str(build/binary),str(BASE/'actual-history144.bin'),str(histories),scenario]
        r=subprocess.run(cmd,cwd=ROOT,text=True,capture_output=True,preexec_fn=limits,timeout=40)
        (BASE/(name+'.stdout')).write_text(r.stdout);(BASE/(name+'.stderr')).write_text(r.stderr)
        if r.returncode or r.stderr:raise RuntimeError(name+': '+r.stdout+r.stderr)
        row=parse(r.stdout);assert row['final_state']==foot['fixture_validation']['scenarios'][SCENARIOS.index(scenario)]['final_state_hash'];return row
    calibrations=[];counts={};probes=[]
    for scenario in SCENARIOS:
        row=execute('benchmark',scenario,2,'calibration-'+scenario);calibrations.append(row);counts[scenario]=args.histories or min(1000,max(2,math.ceil(300000000/row['nanoseconds'])))
        probe=execute('ordinary-new-probe',scenario,counts[scenario],'allocation-probe-'+scenario);assert probe['allocation_counter_enabled']==1 and probe['cpp_new_allocations']==probe['nanoseconds']==0;probes.append(probe)
    rows=[]
    for sample in range(args.samples):
        for scenario in SCENARIOS if sample%2==0 else SCENARIOS[::-1]:
            row=execute('benchmark',scenario,counts[scenario],f'sample-{sample:02}-{scenario}');assert row['allocation_counter_enabled']==0;row['sample']=sample;row['nanoseconds_per_frame']=row['nanoseconds']/row['frames'];rows.append(row)
    stats={}
    for scenario in SCENARIOS:
        selected=[r for r in rows if r['scenario']==scenario];v=[r['nanoseconds_per_frame']for r in selected]
        assert {r['checksum']for r in selected}=={r['checksum']for r in probes if r['scenario']==scenario}
        stats[scenario]={'minimum_ns':min(v),'p05_ns':percentile(v,.05),'median_ns':statistics.median(v),'p95_ns':percentile(v,.95),'maximum_ns':max(v),'mean_ns':statistics.mean(v),'sample_duration_ns_range':[min(r['nanoseconds']for r in selected),max(r['nanoseconds']for r in selected)]}
    assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Input changed during measurement'
    result={'status':'measured','created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'host_machine':platform.machine(),'host_cpu_model':next((l.split(':',1)[1].strip()for l in Path('/proc/cpuinfo').read_text().splitlines()if l.startswith('model name')),'unknown'),'cpu_affinity':[cpu],'available_cpu_affinity':cpus,'address_space_limit_bytes':64*1024*1024,'cpu_seconds_limit':30,'wall_timeout_seconds':40,'samples_per_scenario':args.samples,'histories_per_sample':counts,'calibrations':calibrations,'untimed_allocation_probes':probes,'statistics':stats,'samples':rows,'footprint_sha256':digest(BASE/'footprint.json'),'source_hashes':hashes,'timing_scope':'One timer pair per144-frame history. All native stage validation/copying, graph/animation/body/shape work, prepared timing, input packets, explicit cuts and synthetic-support arithmetic are included. Reset and final checks excluded. No per-frame state replay.','limits':LIMITS+['All samples retained in alternating scenario order. Affinity does not prevent scheduler/frequency/load variance. Percentiles describe history averages, not per-frame tail latency. Calibration targets150ms/sample with1000histories maximum. No overhead subtraction.']}
    dump(BASE/'benchmark.json',result);print(json.dumps({'stage':'measured','histories':counts,'statistics':stats}),flush=True)
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build-dir',type=Path,required=True);p.add_argument('--prepare-only',action='store_true');p.add_argument('--benchmark-only',action='store_true');p.add_argument('--samples',type=int,default=15);p.add_argument('--histories',type=int,default=0);p.add_argument('--cpu',type=int);a=p.parse_args();assert not(a.prepare_only and a.benchmark_only) and a.samples>=15 and a.histories>=0
    build=a.build_dir.resolve();foot=json.loads((BASE/'footprint.json').read_text())if a.benchmark_only else prepare(build)
    if not a.prepare_only:measure(build,a,foot)
if __name__=='__main__':main()
