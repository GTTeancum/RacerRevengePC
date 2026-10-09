#!/usr/bin/env python3
"""CPU0 complete admitted21C120 host cost over authored Pod01 data and persistent state."""
import argparse
import json
import math
import os
import resource
import statistics
import subprocess
import sys
from pathlib import Path
sys.dont_write_bytecode = True
from measure_vehicle_visual49r import (ROOT, SOURCES, SUPPORT, FLAGS, UNIT_EXTRA_FLAGS,
    run, digest, dependency_paths, snapshot_sources, check_stable_sources, sections)
EVIDENCE = ROOT / 'evidence/vehicleVisual49R/benchmark'
MEMBERS = ['evidence/authoredInitialization49N/authoring-source/Pod01.phy',
    'evidence/authoredGeometry49O/asset-reader/fixtures/43-engrt1.psg',
    'evidence/authoredGeometry49O/asset-reader/fixtures/48-englt1.psg']
MODES = ['warm_noise_zero_dt','noise_disabled_zero_dt','warm_noise_bounded_steps',
         'noise_disabled_bounded_steps','cold_noise_first_call','cold_disabled_first_call']


def percentile(values, fraction):
    ordered = sorted(values); position = (len(ordered)-1)*fraction
    lower = math.floor(position); upper = math.ceil(position)
    return ordered[lower] + (ordered[upper]-ordered[lower])*(position-lower)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir',type=Path,required=True)
    parser.add_argument('--object-dir',type=Path,help='Reuse footprint objects only after verifying every recorded source and object hash.')
    parser.add_argument('--samples',type=int,default=15)
    parser.add_argument('--iterations',type=int,default=0,help='Override zero-dt iterations; 0 calibrates to150ms, minimum32768 and multiple64.')
    args = parser.parse_args()
    if args.samples < 5 or args.iterations < 0 or args.iterations % 64:
        parser.error('Use >=5 samples and zero or a positive multiple of64 iterations.')
    build = args.build_dir.resolve(); build.mkdir(parents=True,exist_ok=False)
    EVIDENCE.mkdir(parents=True,exist_ok=True)
    source = EVIDENCE / 'vehicle_visual_benchmark49r.cpp'; before = snapshot_sources()
    objects = []; hashes = {}; commands = []; reused = []
    footprint_path = ROOT/'evidence/vehicleVisual49R/footprint/summary.json'
    footprint = json.loads(footprint_path.read_text())
    if args.object_dir:
        for name,h in footprint['source_hashes'].items():
            if digest(ROOT/name)!=h: raise RuntimeError('Footprint input changed: '+name)
    for name in [*SOURCES,*SUPPORT]:
        obj=(args.object_dir.resolve() if args.object_dir else build)/(Path(name).name+'.o')
        if args.object_dir:
            if digest(obj)!=footprint['objects'][name]['object_sha256']:
                raise RuntimeError('Footprint object changed: '+str(obj))
            reused.append({'unit':name,'object':str(obj),'sha256':digest(obj)})
            hashes.update({n:h for n,h in footprint['source_hashes'].items() if n.startswith('src/')})
        else:
            dep=build/(Path(name).name+'.d')
            command=['g++',*FLAGS,*UNIT_EXTRA_FLAGS.get(name,[]),'-MMD','-MF',str(dep),'-c',str(ROOT/'src'/(name+'.cpp')),'-o',str(obj)]
            commands.append(command);run(command)
            for path in dependency_paths(dep):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
        objects.append(str(obj))
    exe,probe=build/'benchmark',build/'ordinary-new-probe'
    for output,extra in [(exe,[]),(probe,['-DALLOCATION_PROBE49R'])]:
        dep=build/(output.name+'.d')
        command=['g++',*FLAGS,*extra,'-MMD','-MF',str(dep),'-I',str(ROOT/'src'),str(source),*objects,'-o',str(output)]
        commands.append(command);run(command)
        for path in dependency_paths(dep):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    for path in build.glob('*.su'):(EVIDENCE/path.name).write_text(path.read_text())
    cpus=sorted(os.sched_getaffinity(0))
    if 0 not in cpus:raise RuntimeError('Requested CPU0 unavailable: '+str(cpus))
    cap=64*1024*1024
    def limits():
        os.sched_setaffinity(0,{0});resource.setrlimit(resource.RLIMIT_AS,(cap,cap))
    def execute(binary,mode,count):
        command=[str(binary),str(ROOT),str(count),mode]
        p=subprocess.run(command,cwd=ROOT,text=True,capture_output=True,preexec_fn=limits)
        if p.returncode or p.stderr:raise RuntimeError(f'{command}: exit={p.returncode}\n{p.stdout}{p.stderr}')
        row=dict(part.split('=',1) for part in p.stdout.split())
        row={k:v if k=='mode' else int(v) for k,v in row.items()}
        assert row['nodes']==32 and row['expected_noise_calls_per_iteration']==(0 if 'disabled' in mode else 12)
        assert row['retained_asset_bytes']==sum((ROOT/n).stat().st_size for n in MEMBERS)
        assert row['caller_payload_bytes']==footprint['caller_payload_total_bytes_excluding_assets_loader_and_frame_padding']
        assert row['workspace_bytes']==footprint['workspace_total_bytes']
        return row,p.stdout
    calibration=[];raw_calibration=[];iterations={}
    for mode in MODES:
        count=1 if 'cold' in mode else 2048 if 'zero_dt' in mode else 64
        row,stdout=execute(exe,mode,count);calibration.append(row);raw_calibration.append(stdout)
        assert row['nanoseconds']>0
        target=math.ceil(150000000*count/row['nanoseconds'])
        if 'cold' in mode:iterations[mode]=1
        elif 'zero_dt' in mode:iterations[mode]=args.iterations or max(32768,64*math.ceil(target/64))
        else:iterations[mode]=min(240,max(64,target))
    (EVIDENCE/'calibration.stdout').write_text(''.join(raw_calibration))
    probes=[];raw_probes=[]
    for mode in MODES:
        row,stdout=execute(probe,mode,iterations[mode])
        assert row['cpp_new_allocations']==0 and row['nanoseconds']==0
        probes.append(row);raw_probes.append(stdout)
    (EVIDENCE/'ordinary-new-probe.stdout').write_text(''.join(raw_probes))
    results=[];raw=[]
    for sample in range(args.samples):
        for mode in MODES if sample%2==0 else MODES[::-1]:
            row,stdout=execute(exe,mode,iterations[mode]);row['sample']=sample
            row['nanoseconds_per_iteration']=row['nanoseconds']/row['iterations']
            results.append(row);raw.append(stdout)
    stats={}
    for mode in MODES:
        rows=[r for r in results if r['mode']==mode];values=[r['nanoseconds_per_iteration'] for r in rows]
        probe_row=next(r for r in probes if r['mode']==mode)
        assert len({r['checksum'] for r in rows})==1
        assert {r['result_checksum'] for r in rows}=={probe_row['result_checksum']}
        assert {r['initial_checksum'] for r in rows}=={probe_row['initial_checksum']}
        stats[mode]={'minimum_ns':min(values),'p05_ns':percentile(values,.05),'p25_ns':percentile(values,.25),
                     'median_ns':statistics.median(values),'p75_ns':percentile(values,.75),'p95_ns':percentile(values,.95),
                     'maximum_ns':max(values),'mean_ns':statistics.mean(values),
                     'sample_duration_ns_range':[min(r['nanoseconds'] for r in rows),max(r['nanoseconds'] for r in rows)]}
    for path in [source,Path(__file__),ROOT/'tools/measure_vehicle_visual49r.py',ROOT/'tools/measure_scene_projection49p.py',footprint_path,*[ROOT/n for n in MEMBERS]]:
        hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    check_stable_sources(before,hashes)
    measured,size,detail=sections(exe);symbols=run(['nm','-C',str(exe)])
    assert 'countAllocations' not in symbols and 'operator new(' not in symbols
    for forbidden in ['Machine','original::','oracle','advanceCoupledSecondary49M','advanceEngineAnimation49Q']:
        assert forbidden not in symbols
    (EVIDENCE/'timed-executable-symbols.txt').write_text(symbols)
    (EVIDENCE/'timed-executable-size.txt').write_text(size)
    (EVIDENCE/'timed-executable-sections.txt').write_text(detail)
    result={'status':'measured','compiler':run(['g++','--version']).splitlines()[0],
        'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':UNIT_EXTRA_FLAGS,
        'compile_commands':commands,'source_hashes':hashes,'reused_verified_footprint_objects':reused,
        'timed_executable_sha256':digest(exe),'ordinary_new_probe_executable_sha256':digest(probe),
        'timed_executable_sections':measured,'cpu_affinity':[0],'available_cpu_affinity':cpus,
        'normal_virtual_address_limit_bytes':cap,'rounding':'FE_TONEAREST checked each process',
        'samples_per_mode':args.samples,'iterations_per_sample_by_mode':iterations,
        'iteration_justification':'One calibration per mode. Zero-dt:2048 calibration calls,150ms target,minimum32768,rounded to64; override applies only here. Positive-dt:64 calibration calls,150ms target capped at240 calls to preserve a4-second measured history. Cold:one fresh first call. Short bounded/cold samples include proportionally greater clock/scheduling noise.',
        'calibration':calibration,'statistics':stats,'percentile_method':'Linear interpolation at sorted index(n-1)*p. All15 samples per mode retained,including noisy runs; not tail-latency guarantees.',
        'results':results,'separate_untimed_ordinary_new_probes':probes,
        'fixture':{'meaning':'Actual original Pod01 PHY scalars and right/left PSG nullcg base offsets,canonical main->194 and separate parentless engine roots with actual authored descendants.',
            'nodes':32,'main190':0,'container194':1,'right_model_root':2,'right_psg_nodes':list(range(3,17)),
            'left_model_root':17,'left_psg_nodes':list(range(18,32)),
            'psg_nodes_per_engine':14,'retained_asset_bytes':sum((ROOT/n).stat().st_size for n in MEMBERS),
            'hot_required_asset_bytes':0,'asset_retention_note':'Three source buffers remain retained in this benchmark; the hot49R call reads copied parameters and scene records and requires no PHY/PSG backing bytes. No MOT/VUT or model animation is loaded.',
            'caller_payload_bytes_excluding_loader_and_frame_padding':probes[0]['caller_payload_bytes'],
            'workspace_bytes':probes[0]['workspace_bytes'],'loader_record_bytes':probes[0]['loader_record_bytes'],
            'initial_records':'Explicit source constructor response/up records and cold graphics lazy globals; one caller-owned cold shared49M noise/RNG with explicitly supplied0x12345678 seed. PSG offsets captured in fresh identity-root context; nullcg XYZ cleared once in scene construction.',
            'inputs':'vehicle834=0,applicationByte58=2,probe108=0,vehicle40=0,no optional engine roots,B54=0. Stationary upright main at(4,3,-2),194 local translation(0,.5,0). Cached speed80,nonzero impact/probe and damage. Aero/engine/slide inputs alternate every32 calls in a64-call period.',
            'warmup':'64 continuous1/60 calls on the same enabled/disabled844 source path before warm zero-dt and bounded measurements. No warmup before cold first-call modes.',
            'zero_dt':'After warmup,only frameFactor changes to0; all existing response,clock,graph,shared/noise/RNG history persists for the calibrated loop. This source-admitted workload controls long-run pose growth without adding clamps; it is not advancing game time.',
            'bounded_steps':'64 warmup plus up to240 measured calls atnearest-binary32(1/60),same persistent records; each process is one new authored fixture. No per-iteration seed,pose or constructor reset.',
            'noise_source_paths':'844=0 executes12 gradient-noise evaluations per complete call,including atzero dt. 844!=0 skips those calls and also applies the original downward-force term,so it is a distinct source workload,not a pure noise-overhead subtraction.',
            'cold_cost':'Cold first call includes graphics lazy initialization,constructor-history response and,for844=0,the49M cold table/RNG initialization. Loader/authoring/scene setup is outside timing. One-call times include clock overhead.',
            'cache_state':'All32 worlds are refreshed once outside timing. First root write invalidates engine descendants; warm calls retain naturally dirty engine-root/descendant caches. No renderer/world-cache refresh is inserted between calls.',
            'boundary':'Complete admitted normal21C120 through its real return,after engine animation and before Chariot/physical stages. Previous animation is supplied phase context,not executed or timed.',
            'members':[{'path':n,'bytes':(ROOT/n).stat().st_size,'sha256':digest(ROOT/n)} for n in MEMBERS]},
        'limits':[
            'Native host complete admitted49R graphics-stage cost only. No interpreter/oracle or allocation instrumentation is linked into timed loops. No FPS,Xbox,PC RAM or full-game memory claim.',
            'All modes include API rounding/topology/record admission,64-node transaction workspace use,container orientation/world work,both response integrations and root-local writes,tail lazy guard,input selection and three small observable sums.',
            'The noise-disabled source path also changes forceY. Timings cannot isolate a pure12-call noise cost or establish a historical speedup.',
            'Warm serial engine-root caches stay dirty. This is not a full animation/render/physics frame,world-cache consumer,contact query or reset experiment.',
            'Positive-dt samples deliberately stop after a4-second measured interval; long throughput uses labelled zero dt rather than synthetic stabilizing clamps or per-call resets.',
            'File I/O,PHY/PSG parsing,parameter binding,scene construction,full checksums and64 warmup calls are outside timing. Authoring reader cost is not benchmarked; its local scratch and static frame are in the footprint report.',
            'Source/object allocator audit plus zero ordinary-new counts from separate untimed probes supports a bounded allocation-free hot path. malloc/aligned allocations are not globally instrumented; this is not allocation-failure or ownership testing.',
            'Existing49M/48G support code is reused,not newly allocated per frame. Shared noise and its transaction copy are already charged in caller payload/workspace.',
            'Timed executable sections include the harness,loaders and linked support objects and are not incremental49R code/data. Incremental object totals are in the footprint report.',
            'CPU0 and64MiB RLIMIT_AS are ordinary host process constraints,not a target hardware or memory-budget simulation. No process RAM observations are taken.',
            'Compiler .su files are static frame estimates,not measured stack high-water. Short cold/bounded samples are noisier than calibrated long zero-dt samples.']}
    (EVIDENCE/'samples.stdout').write_text(''.join(raw))
    (EVIDENCE/'verification.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'status':result['status'],'iterations':iterations,'statistics':stats}))

if __name__=='__main__':main()
