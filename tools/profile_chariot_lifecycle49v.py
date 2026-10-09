#!/usr/bin/env python3
"""Bounded49V production footprint and frozen240-frame serial host budget."""
import argparse
import copy
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
from measure_chariot_root49u import ROOT_PARAMS, SUPPORT as PRIOR_SUPPORT, PHY, MESH, percentile
from visual.run_chariot_root49u import local_vertices, SCENES

BASE = ROOT/'evidence/chariotLifecycle49V/budget'
CONNECTED = ROOT/'evidence/chariotLifecycle49V/connected'
FROZEN = CONNECTED/'frozen-proof.json'
FROZEN_SHA = '790bc71d3dae278dadbcd419cc6de8e3e205a6b54a172fb86503322d7330c100'
HARNESS = ROOT/'tools/bench_chariot_lifecycle49v.cpp'
NEW = ['game/secondary_activation49v','game/chariot_blend49v','game/chariot_lifecycle49v']
SUPPORT = ['game/chariot_scene49u','assets/chariot_root_authoring49u'] + PRIOR_SUPPORT
MODES = ['lifecycle','getter','animation_visual_lifecycle_palette',
    'animation_visual_lifecycle_palette_getter','animation_visual_lifecycle_palette_rigid_getter']
EXTRA_FLAGS = {'game/body_scene48g':['-Wno-misleading-indentation']}
KEYS = ['gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes',
    'readonly_data_bytes','relocated_readonly_data_bytes','mutable_data_bytes','data_bytes','bss_bytes']
# Represented source-body words, independent of native padding/layout.
BODY_VECTOR_RANGES = [(0x60,3),(0x70,3),(0x80,3),(0x90,3),(0xa0,3),(0xb0,3),
    (0xc0,3),(0xd0,3),(0x180,3),(0x190,3),(0x1a0,3),(0x1b0,3),(0x1c0,3),
    (0x1d0,3),(0x1f0,4),(0x2c4,3),(0x2dc,3),(0x314,3),(0x2e8,4),(0x2f8,4)]
BODY_WORDS = [a+4*i for a,n in BODY_VECTOR_RANGES for i in range(n)]
BODY_WORDS += [0x238,0x34,0xe0,0xe4,0xe8,0x20c,0x7c,0xbc,0x18c,0x19c,
    0x8c,0x9c,0xcc,0xdc,0x2b8,0x2bc,0x2c0,0x2d0,0x2d4,0x2d8,0x308,0x30c,0x310]


def dump(path,value): path.write_text(json.dumps(value,indent=2)+'\n')

def secondary_words(s):
    return ([s['source_words'][format(k,'x')] for k in BODY_WORDS]+s['cached_world_bits']
        +[s[k] for k in ('scene_flags190','phase864_bits','phase','scene_present','enabled')])

def lifecycle_words(row):
    s,h=row['root_state'],row['root_shared']
    return ([s[k] for k in ('vehicle838','vehicle83C','vehicle840','duration848_bits','elapsed84C_bits','previous85C_bits')]
        +sum([s[k+'_bits'] for k in ('position870','linearVelocity880','angularVelocity890','orientation8A0')],[])
        +h['speed434750']+h['yaw434758']+h['up434650_bits'])

def live_words(row,secondary): return retained_live_words(row)+lifecycle_words(row)+secondary_words(secondary)

def frame_words(row):
    r=row['root_input_bits']
    return (retained_frame_words(row)+[r[k] for k in ('phase','vehicle40','vehicle58','aeroD8','vehicleCFC','vehicleD00','probe10C',
        'frameFactor_bits','cachedBodySpeed34_bits','bodyAngularVelocityYB4_bits','objectD34_10_bits')]
        +r['bodyLocalVelocity80_bits']+r['impactBA0_bits']+r['probeVector1D0_bits'])

def verify_freeze():
    assert digest(FROZEN)==FROZEN_SHA,'Connected frozen-proof pin changed'
    frozen=json.loads(FROZEN.read_text());assert frozen['passed'] and frozen['native_modes']==['normal','plain','asan-ubsan']
    for group,base in [('source_sha256',ROOT),('input_sha256',ROOT),('artifact_sha256',CONNECTED)]:
        for name,h in frozen[group].items(): assert digest(base/name)==h,(group,name)
    return frozen

def prepare_fixture(trace_path):
    trace=json.loads(trace_path.read_text());summary=trace['summary'];initial=trace['initial']
    assert len(trace['frames'])==240 and summary['passed'] and summary['source_substitutions']==0
    expected_counts={'physical_entry_calls':36,'persistent_virtual_calls':82,'blend_calls':122,'reverse_calls':2,
        'independent_later_getter_calls':157,'dirty_later_getter_calls':2,'source49V_noise_calls':1224,'source_noise_calls':4104}
    assert all(summary[k]==v for k,v in expected_counts.items())
    vertices=[local_vertices(ROOT/p) for p in SCENES];assert [len(v) for v in vertices]==[2884,1411,1460]
    out=bytearray(b'RR49VB01')+pack([240])+pack(graph_words(initial['pre_motion_constructor_graph']))
    out+=struct.pack('<Q',fnv(pack(graph_words(initial['post_constructor_graph']))))
    out+=pack(graph_words(initial['graph']))+pack(live_words(initial,initial['secondary_body']))
    for side in ('left','right'):
        r=initial['persistent_ik_records'][side]
        out+=pack(r['joint_indices']+[r['mode10']]+r['coefficient_bits']+r['saved_elbow_local30']+[r['debug40']]+r['axis50'])
    p=initial['visual_parameters_bits'];out+=pack([p[k] for k in PARAMS]+p['rightBase2C0']+p['leftBase2B0'])
    out+=pack([initial['root_parameters_bits'][k] for k in ROOT_PARAMS]+[initial['unconsumedA90_bits']])
    for vs in vertices:
        for xyz,bone in vs: out+=struct.pack('<3dI',*xyz,bone)
    expected=[];base_mesh=MESH.read_bytes();previous=initial['secondary_body'];reverses=[];getters=0
    for row in trace['frames']:
        assert secondary_words(row['secondary_before'])==secondary_words(previous),'Secondary state replay/discontinuity'
        assert row['phase864_before_bits']==previous['phase864_bits']
        positions=position_bytes(row,vertices);mesh=bytearray(base_mesh)
        for n in range(5755):mesh[16+n*24:28+n*24]=positions[n*12:(n+1)*12]
        assert fnv(mesh)==int(row['mesh_fnv64'],16),'Connected full mesh digest differs'
        before=row['root_state_before'];after=row['secondary_after_root'];obs=row['later_getter_observation']
        assert after['phase864_bits']==row['phase864_bits']
        assert set(after['source_words'])=={format(x,'x') for x in BODY_WORDS}|{'8','200','288'}
        assert after['source_words']['8']==0x3ce000 and after['source_words']['288']==156160
        assert after['source_words']['200']==after['enabled']
        post_graph=copy.deepcopy(row['graph']);post=copy.deepcopy(after)
        if obs:
            getters+=1;assert row['root_state']['vehicle838'] and obs['source_callsite']==0x218c60 and obs['source_entry']==0x28b1a0
            assert post_graph[49]['local_bits']==obs['physical_local_before_bits'] and post_graph[49]['world_bits']==obs['physical_cached_world_before_bits']
            post_graph[49]['world_bits']=obs['cached_world_bits'];post_graph[49]['flags190']=obs['scene_flags190']&255
            post['cached_world_bits']=obs['cached_world_bits'];post['scene_flags190']=obs['scene_flags190']
        else: assert not row['root_state']['vehicle838']
        previous=post
        hashes=[int(row[k],16) for k in ('after21A000_graph_fnv64','after49R_graph_fnv64','after49V_graph_fnv64')]
        assert hashes[2]==fnv(pack(graph_words(row['root_return_graph'])))
        hashes += [fnv(pack(graph_words(row['graph']))),fnv(pack(live_words(row,after))),fnv(pack(palette_words(row))),fnv(positions),int(row['after49R_noise_fnv64'],16),fnv(pack(graph_words(post_graph))),fnv(pack(live_words(row,post))),fnv(pack(secondary_words(row['secondary_before'])))]
        expected.append(hashes);out+=pack(frame_words(row))+struct.pack('<11Q',*hashes)
        out+=pack([bool(obs),before['vehicle838'],before['vehicle83C'],row['lifecycle_audit']['reversed']]+(obs['relative_position_bits'] if obs else [0]*4))
        if row['lifecycle_audit']['reversed']:reverses.append(row['frame'])
    assert getters==157 and reverses==[64,160]
    path=BASE/'actual-history240.bin';path.write_bytes(out)
    names=['animation_graph','visual_graph','lifecycle_return_graph','palette_graph','return_records','palettes','positions','noise_after49R','post_getter_graph','post_getter_records','secondary_before']
    result={'trace_path':str(trace_path.relative_to(ROOT)),'trace_sha256':digest(trace_path),'fixture_bytes':len(out),'fixture_sha256':digest(path),
        'frames':240,'graph_nodes':53,**expected_counts,'reverse_frames':reverses,'mode_transitions':trace['mode_transitions'],
        'palette_entries':[18,14,14],'rigid_vertices':[2884,1411,1460],'source_full_mesh_digests_verified':240,
        'source_assets':[{'path':str(p.relative_to(ROOT)),'bytes':p.stat().st_size,'sha256':digest(p)} for p in [PHY,MESH,*[ROOT/s for s in SCENES],*[ASSET_DIR/n for n in ASSET_NAMES]]],
        'expected_final_hashes':dict(zip(names,expected[-1])),'all_frame_expected_hash_names':names,'all_frame_expected_hashes':expected,
        'secondary_source_word_order_hex':[format(x,'x') for x in BODY_WORDS],
        'boundary':'Exact frozen53-node240-frame supplied schedule, actual PHY/MOT/fresh geometry. Canonical838/secondary864/history persist; two reverse callbacks reset before full70/B0 copies; blend sees old elapsed and cached world. Complete21A660 return is checked before palettes/vertices and independent218C60 getter. Main/control/clock/acceleration inputs supplied; no physics, contacts, observer, authentic spawn or FrameReady execution.'}
    dump(BASE/'fixture.json',result);return result


def prepare(build,trace):
    BASE.mkdir(parents=True,exist_ok=True);build.mkdir(parents=True,exist_ok=False)
    frozen=verify_freeze();before=snapshot_sources();hashes={};objects={};frames=[];commands=[]
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
    for binary,extra in [('benchmark',[]),('ordinary-new-probe',['-DPROBE49V'])]:
        dep=build/(binary+'.d');command=['g++',*FLAGS,*extra,'-I',str(ROOT/'src'),'-MMD','-MF',str(dep),str(HARNESS),*[str(build/(Path(n).name+'.o')) for n in NEW+SUPPORT],'-o',str(build/binary)]
        commands.append(command);run(command)
        for path in dependency_paths(dep):
            if path.is_relative_to(ROOT):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    for path in build.glob('*bench_chariot_lifecycle49v.su'):(BASE/path.name).write_text(path.read_text())
    stdout=run([str(build/'benchmark'),'--sizes']);(BASE/'record-sizes.stdout').write_text(stdout)
    records={r.split()[0]:int(r.split()[1]) for r in stdout.splitlines()};alignments={r.split()[0]:int(r.split()[2]) for r in stdout.splitlines()}
    baseline=ROOT.parent/'CLOUD-MILESTONE-49U-CHARIOTROOTRESPONSE-MANIFEST.json';listed=json.loads(baseline.read_text())['files'];prefix='RacerRevengePC/'
    prior={f['path'][len(prefix):]:f['sha256'] for f in listed if f['path'].startswith(prefix+'src/')}
    retained={n:digest(ROOT/n)==h for n,h in prior.items()};assert all(retained.values()),'Retained production changed versus49U'
    assert all('src/'+n+'.cpp' not in prior for n in NEW)
    baseline_foot=ROOT/'evidence/chariotRoot49U/budget/footprint.json';old=json.loads(baseline_foot.read_text())
    # New API records replace49U state/input/workspace/diagnostic in this path.
    # Shared/parameters/bindings already exist; canonical838/864 are inside49M.
    incremental=['ChariotLifecycleState49V','ChariotLifecycleInput49V','ChariotLifecycleWorkspace49V','ChariotLifecycleDiagnostic49V','SecondaryActivationSupplement49V','SecondaryActivationGlobals49V']
    replaced=['ChariotRootState49U','ChariotRootInput49U','ChariotRootWorkspace49U','ChariotRootDiagnostic49U']
    retained_records=[n for n in old['combined_caller_records'] if n not in replaced+['SecondaryBodyState49M']]
    getter_records=['LaterGetterPosition49V','detail::SceneVectorContext49U']
    combined=incremental+['CoupledBodyState49M']+retained_records+getter_records
    minimal=incremental+['MainBodyState49K','MainSphereLanes49M','SecondaryBodyState49M']+retained_records+getter_records
    # Minimal disjoint canonical owner accounting needs a separate actual838.
    newframes=[f for f in frames if f['unit'] in NEW]
    def frame(unit,fragment):
        found=[f['bytes'] for f in frames if f['unit']==unit and fragment in f['function'] and '<lambda' not in f['function']]
        assert len(found)==1,(unit,fragment,found);return found[0]
    chains=[[(NEW[2],'updateChariotLifecycle49V('),('game/chariot_scene49u','position49U('),('game/chariot_scene49u','::refresh('),('game/scene_graph49p','refreshSceneWorld49P('),('game/scene_graph49p','validateSceneGraph49P(')],
        [('game/model_palette49s','projectModelPalette49S('),('game/scene_graph49p','refreshSceneWorld49P('),('game/scene_graph49p','validateSceneGraph49P(')]]
    additive=[{'chain':[u+':'+f for u,f in chain],'sum_compiler_frame_bytes':sum(frame(u,f) for u,f in chain)} for chain in chains]
    invalidate=frame('game/scene_graph49p','::invalidate(')
    fixture=prepare_fixture(trace)
    input_paths=[HARNESS,Path(__file__),ROOT/'tools/measure_chariot_root49u.py',ROOT/'tools/measure_chariot_animation49t.py',ROOT/'tools/measure_scene_projection49p.py',ROOT/'tools/visual/run_chariot_root49u.py',ROOT/'tools/visual/run_chariot49t.py',trace,BASE/'actual-history240.bin',FROZEN,PHY,MESH,*[ROOT/s for s in SCENES],*[ASSET_DIR/n for n in ASSET_NAMES]]
    hashes.update({str(p.relative_to(ROOT)):digest(p) for p in input_paths});check_stable_sources(before,hashes)
    symbols=run(['nm','-C',str(build/'benchmark')]);(BASE/'timed-executable-symbols.txt').write_text(symbols)
    for forbidden in ['Machine','original::','oracle','trace::','stage_trace','countAllocations','chariot49v_test']: assert forbidden not in symbols,forbidden
    measured,size,detail=sections(build/'benchmark');(BASE/'timed-executable-size.txt').write_text(size);(BASE/'timed-executable-sections.txt').write_text(detail)
    preflight=[]
    for binary in ('benchmark','ordinary-new-probe'):
        preflight.append(run([str(build/binary),str(BASE/'actual-history240.bin'),str(ROOT),'0',MODES[-1]]))
    (BASE/'preflight.stdout').write_text(''.join(preflight))
    result={'status':'measured','compiler':run(['g++','--version']).splitlines()[0],'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':EXTRA_FLAGS,
        'compile_commands':commands,'source_hashes':hashes,'objects':objects,'frames':frames,'incremental49V_units':NEW,'reused49U_or_earlier_support':SUPPORT,
        'incremental49V_totals':{k:sum(objects[n][k] for n in NEW) for k in KEYS},'linked_production_totals':{k:sum(objects[n][k] for n in NEW+SUPPORT) for k in KEYS},
        'connected_freeze':{'path':str(FROZEN.relative_to(ROOT)),'sha256':digest(FROZEN),'validated_groups':{k:len(frozen[k]) for k in ('source_sha256','input_sha256','artifact_sha256')}},
        'baseline49U_manifest':{'path':str(baseline),'sha256':digest(baseline),'all_retained_production_files_identical':retained},
        'baseline49U_budget':{'path':str(baseline_foot.relative_to(ROOT)),'sha256':digest(baseline_foot),'original_combined_caller_bytes':old['combined_caller_payload_bytes_excluding_motion_bytes_and_padding']},
        'record_bytes':records,'record_alignments':alignments,'incremental49V_caller_records':{n:records[n] for n in incremental},'incremental49V_caller_total_bytes':sum(records[n] for n in incremental),
        'replaced49U_caller_records':{n:records[n] for n in replaced},'replaced49U_caller_total_bytes':sum(records[n] for n in replaced),
        'net49V_caller_api_delta_bytes':sum(records[n] for n in incremental)-sum(records[n] for n in replaced),
        'combined_caller_records':{n:records[n] for n in combined},'combined_caller_payload_bytes_excluding_motion_bytes_and_padding':sum(records[n] for n in combined),
        'minimum_disjoint_canonical_caller_records':{**{n:records[n] for n in minimal},'actualVehicle838':4},'minimum_disjoint_canonical_caller_payload_bytes':sum(records[n] for n in minimal)+4,
        'separate_later_getter_caller_records':{n:records[n] for n in getter_records},'existing_canonical_coupled_owner_bytes':records['CoupledBodyState49M'],'canonical_vehicle838_member_bytes':4,'canonical_time864_member_bytes':4,
        'retained_motion_byte_payload':sum((ASSET_DIR/n).stat().st_size for n in MOT_NAMES),
        'workspace_composition':{'ChariotLifecycleWorkspace49V':records['ChariotLifecycleWorkspace49V'],'included_nodes64':records['SceneNodes64'],'included_state':records['ChariotLifecycleState49V'],'included_shared':records['ChariotRootShared49U'],'included_secondary':records['SecondaryBodyState49M'],'included_supplement':records['SecondaryActivationSupplement49V'],'included_noise':records['CoupledNoiseContext49M'],'included_vehicle838':4},
        'maximum_individual49V_compiler_frame_bytes':max(f['bytes'] for f in newframes),'public49V_compiler_frames':[f for f in newframes if any('::'+n+'(' in f['function'] for n in ('updateChariotLifecycle49V','blendChariotMatrix49V','resetSecondaryActivation49V','copySecondaryMainVelocities49V','initializedSecondaryActivationGlobals49V'))],
        'selected_additive_compiler_frame_estimates':additive,'recursive_invalidate':{'maximum_simultaneous_invocations':64,'individual_frame_bytes':invalidate,'compiler_frame_product_bytes':64*invalidate},
        'timed_executable_sections':measured,'timed_executable_sha256':digest(build/'benchmark'),'probe_executable_sha256':digest(build/'ordinary-new-probe'),'fixture':fixture,
        'preflight_sha256':digest(BASE/'preflight.stdout'),
        'limits':['New object delta is three49V production units; retained49U sources and baseline evidence stay unchanged.49U bounded root implementation remains available but is not linked into this complete49V path. No old object subtraction or speedup claim.',
            'Host sizeof/alignment and object sections only; not process RAM, game memory, console footprint or FPS. GNU text includes read-only/unwind metadata; executable text, readonly, relocated readonly, mutable data and BSS are separate.',
            'New caller records total includes new API state/input/workspace/diagnostic plus supplement and activation globals. Net API delta subtracts replaced49U state/input/workspace/diagnostic only. Shared/parameters/bindings and53-node graph are retained; actual838/864 are canonical49M members.',
            'Full combined caller payload includes the complete pre-existing CoupledBodyState49M owner used by this harness, without separately counting its main/secondary/sphere/mode subobjects. Disjoint canonical alternative is itemized separately, including actual838. Neither claims the whole physics pipeline executes.',
            'Nested workspace members are already included; component totals are explanatory, not extra charges. Payload excludes fixture history, loader vectors, constructor-only scratch, system/runtime and inter-record padding; immutable six-motion bytes are separate.',
            'Compiler.su frames are individual static estimates, not measured stack high-water or an exhaustive ABI bound. Selected additive chains are partial examples; source recursion may add up to64 invalidate frames. No red-zone/runtime estimate is invented.',
            'Direct allocator audit covers every linked production object. Separate successful ordinary scalar/array C++new probe counts full240-frame continuation. No Cmalloc/aligned interception, ownership, allocation-failure or historical capacity-alias experiment.']}
    dump(BASE/'footprint.json',result)
    print(json.dumps({'stage':'prepared','new_code':result['incremental49V_totals'],'new_records':result['incremental49V_caller_total_bytes'],'net_api_delta':result['net49V_caller_api_delta_bytes'],'combined':result['combined_caller_payload_bytes_excluding_motion_bytes_and_padding']}),flush=True)
    return result


def measure(build,args,foot):
    hashes=foot['source_hashes'];assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Inputs changed since prepare'
    verify_freeze();assert digest(build/'benchmark')==foot['timed_executable_sha256'] and digest(build/'ordinary-new-probe')==foot['probe_executable_sha256']
    cpus=sorted(os.sched_getaffinity(0));cpu=args.cpu if args.cpu is not None else cpus[0];assert cpu in cpus
    cap=64*1024*1024
    def limits():
        os.sched_setaffinity(0,{cpu});resource.setrlimit(resource.RLIMIT_AS,(cap,cap));resource.setrlimit(resource.RLIMIT_CPU,(30,30))
    def execute(binary,mode,histories):
        command=[str(build/binary),str(BASE/'actual-history240.bin'),str(ROOT),str(histories),mode]
        p=subprocess.run(command,cwd=ROOT,text=True,capture_output=True,preexec_fn=limits,timeout=40)
        if p.returncode or p.stderr:raise RuntimeError(f'{command}: exit={p.returncode}\n{p.stdout}{p.stderr}')
        row=dict(part.split('=',1) for part in p.stdout.split());row={k:v if k=='mode' else int(v) for k,v in row.items()}
        e=foot['fixture']['expected_final_hashes']
        assert [row[k] for k in ('final_graph','final_records','final_palettes','final_positions')]==[e[k] for k in ('post_getter_graph','post_getter_records','palettes','positions')]
        assert row['retained_motion_bytes']==foot['retained_motion_byte_payload']
        assert [row['validated_'+k] for k in ('physical_calls','virtual_calls','blend_calls','reverse_calls','getter_calls','dirty_getter_calls','transitions')]==[36,82,122,2,157,2,6]
        return row,p.stdout
    calibrations=[];counts={};calibration_stdout=[]
    for mode in MODES:
        row,stdout=execute('benchmark',mode,2);calibrations.append(row);calibration_stdout.append(stdout)
        # Getter is sparse and tiny; cap full continuations to remain under the
        # same30-second process bound while retaining all157 actual observations.
        counts[mode]=args.histories or min(128 if mode=='getter' else 1000,max(2,math.ceil(150000000*2/row['nanoseconds'])))
    (BASE/'calibration.stdout').write_text(''.join(calibration_stdout))
    probes=[];probe_stdout=[]
    for mode in MODES:
        row,stdout=execute('ordinary-new-probe',mode,counts[mode]);assert row['allocation_counter_enabled']==1 and row['cpp_new_allocations']==0 and row['nanoseconds']==0
        probes.append(row);probe_stdout.append(stdout)
    (BASE/'ordinary-new-probe.stdout').write_text(''.join(probe_stdout))
    rows=[];stdout=[]
    for sample in range(args.samples):
        for mode in MODES if sample%2==0 else MODES[::-1]:
            row,out=execute('benchmark',mode,counts[mode]);assert row['allocation_counter_enabled']==0
            row['sample']=sample;row['nanoseconds_per_frame']=row['nanoseconds']/row['frames']
            if mode=='getter':row['nanoseconds_per_active_getter']=row['nanoseconds']/(row['histories']*157)
            rows.append(row);stdout.append(out)
    stats={}
    for mode in MODES:
        selected=[r for r in rows if r['mode']==mode];values=[r['nanoseconds_per_frame'] for r in selected]
        assert {r['checksum'] for r in selected}=={p['checksum'] for p in probes if p['mode']==mode}
        stats[mode]={'minimum_ns':min(values),'p05_ns':percentile(values,.05),'p25_ns':percentile(values,.25),'median_ns':statistics.median(values),'p75_ns':percentile(values,.75),'p95_ns':percentile(values,.95),'maximum_ns':max(values),'mean_ns':statistics.mean(values),'sample_duration_ns_range':[min(r['nanoseconds'] for r in selected),max(r['nanoseconds'] for r in selected)]}
        if mode=='getter':stats[mode]['median_ns_per_active_getter']=statistics.median([r['nanoseconds_per_active_getter'] for r in selected])
    assert all(digest(ROOT/n)==h for n,h in hashes.items()),'Inputs changed during measurement';verify_freeze()
    (BASE/'samples.stdout').write_text(''.join(stdout))
    result={'status':'measured','created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'host_machine':platform.machine(),'host_cpu_model':next((line.split(':',1)[1].strip() for line in Path('/proc/cpuinfo').read_text().splitlines() if line.startswith('model name')),'unreported'),
        'footprint_sha256':digest(BASE/'footprint.json'),'source_hashes':hashes,'cpu_affinity':[cpu],'available_cpu_affinity':cpus,'normal_virtual_address_limit_bytes':cap,'cpu_seconds_limit_per_process':30,'wall_seconds_timeout_per_process':40,'rounding':'FE_TONEAREST at startup and production admission','compiler':foot['compiler'],'flags':FLAGS,
        'samples_per_mode':args.samples,'histories_per_sample':counts,'calibrations':calibrations,'statistics':stats,'results':rows,'untimed_ordinary_new_probes':probes,
        'validation':'Frozen-proof source/input/artifact hashes authenticated before compilation and timing. Both binaries preflight actual PHY/fresh geometry, original pre-motion constructor graph/IK, root constructor, all240 animation/visual/exact49V-return/palette/post-getter graph hashes, full persistent secondary source words/history/clock/modes/noise, all46 palette entries, all5755 rigid positions,157 getter results and immutable main. Source instruction interpreter and test hooks are absent from timing. Every completed timed history rechecks final graph/records/palettes/selected vertices.',
        'timed_stages':{'lifecycle':'Complete49V call only, one timer pair/frame. All other stages, including later getter, execute in order outside that interval.','getter':'Only the157 active independent218C60->28B1A0 observations plus canonical scene/flag mirroring, one timer pair/active observation. Per-frame average is total getter time/240; active-getter median also reported.','animation_visual_lifecycle_palette':'Supplied main snapshot assignment,49T->49R->canonical main scene projection->49V->three49S palettes, one timer pair/frame; separately ordered later getter runs outside timer.','animation_visual_lifecycle_palette_getter':'Same plus the declared later getter seam, one timer pair/240-frame history.','animation_visual_lifecycle_palette_rigid_getter':'Same plus all5755 diagnostic rigid positions/frame before the later getter, one timer pair/240-frame history.'},
        'measurement_limits':['Serial host subsystem budget only, no gameplay FPS, process RAM, console benchmark or historical speedup inference.',
            'Finite240-frame supplied schedule restarts outside timing. No within-history reset/replay of canonical secondary838/864, histories or noise. Natural cold lazy/noise setup occurs once per history.',
            'Getter is explicitly separate from21A660 and from omitted intervening/later physics.157 active getters, including2 dirty refreshes, are required to reproduce this diagnostic history. Full-path labels state whether their cost is included.',
            'Main/control/clock/acceleration outputs supplied; constructor, parsing, asset admission, validation hashes and printing excluded. Production validation/copies, graph refresh and rigid validation/commit remain inside selected intervals.',
            'Isolated lifecycle/getter and getter-excluded full path include timer overhead per selected interval. Aggregate modes use one timer pair/history. Loop branches/status accumulation included; no subtractive overhead correction.',
            'All samples retained in alternating mode order. Affinity cannot remove host scheduling/frequency/load variation. Percentiles are linear descriptive percentiles of history averages, not frame tail-latency guarantees.',
            'Calibration targets150ms selected time/sample but caps128 histories for the sparse getter (1000 otherwise) to bound continuation wall time; all raw counts/durations retained.64MiB virtual-address and30-second CPU caps bound this experiment, not target hardware.',
            'Ordinary-new probe is a separate executable using identical production objects. Counts successful scalar/array new across full continuations; loader vectors allocate beforehand. No Cmalloc/aligned interception or failure diagnostics.']}
    dump(BASE/'benchmark.json',result)
    print(json.dumps({'stage':'measured','histories':counts,'statistics':stats}),flush=True)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build-dir',type=Path,required=True)
    p.add_argument('--prepare-only',action='store_true');p.add_argument('--benchmark-only',action='store_true')
    p.add_argument('--trace',type=Path,default=CONNECTED/'trace.json');p.add_argument('--samples',type=int,default=15)
    p.add_argument('--histories',type=int,default=0);p.add_argument('--cpu',type=int)
    args=p.parse_args();assert args.samples>=15 and args.histories>=0 and not(args.prepare_only and args.benchmark_only)
    build=args.build_dir.resolve();foot=json.loads((BASE/'footprint.json').read_text()) if args.benchmark_only else prepare(build,args.trace.resolve())
    if not args.prepare_only:measure(build,args,foot)
if __name__=='__main__':main()
