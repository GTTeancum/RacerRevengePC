#!/usr/bin/env python3
"""CPU0 native actual Pod01 engine animation-stage cost, with a disabled-gate control."""
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
from measure_engine_animation49q import (ROOT, SOURCES, SUPPORT, FLAGS, run, digest,
    dependency_paths, snapshot_sources, check_stable_sources, sections)
EVIDENCE = ROOT / 'evidence/engineAnimation49Q/benchmark'
FIXTURES = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures'
MEMBERS = ['48-englt1.psg','49-englt1.vut','50-englt1.mot','43-engrt1.psg','44-engrt1.vut','45-engrt1.mot']
MODES = ['both_engines_enabled','shared_enable_gate_disabled']


def percentile(values, fraction):
    ordered = sorted(values); position = (len(ordered)-1)*fraction
    lower = math.floor(position); upper = math.ceil(position)
    return ordered[lower] + (ordered[upper]-ordered[lower])*(position-lower)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir',type=Path,required=True)
    parser.add_argument('--samples',type=int,default=15)
    parser.add_argument('--iterations',type=int,default=0,help='0 calibrates each mode to150ms with minimum32768, rounded to64-call periods')
    args = parser.parse_args()
    if args.samples < 5 or args.iterations < 0 or args.iterations % 64:
        parser.error('Use >=5 samples and zero or a positive multiple of64 iterations.')
    build = args.build_dir.resolve(); build.mkdir(parents=True,exist_ok=False)
    EVIDENCE.mkdir(parents=True,exist_ok=True)
    source = EVIDENCE / 'engine_benchmark49q.cpp'; before = snapshot_sources()
    objects = []; hashes = {}; commands = []
    for name in [*SOURCES,*SUPPORT]:
        obj,dep = build/(Path(name).name+'.o'),build/(Path(name).name+'.d')
        command=['g++',*FLAGS,'-MMD','-MF',str(dep),'-c',str(ROOT/'src'/(name+'.cpp')),'-o',str(obj)]
        commands.append(command);run(command);objects.append(str(obj))
        for path in dependency_paths(dep):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    exe,probe=build/'benchmark',build/'ordinary-new-probe'
    for output,extra in [(exe,[]),(probe,['-DALLOCATION_PROBE49Q'])]:
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
        command=[str(binary),str(FIXTURES),str(count),mode]
        p=subprocess.Popen(command,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,preexec_fn=limits)
        _,status,usage=os.wait4(p.pid,0);p.returncode=os.waitstatus_to_exitcode(status)
        stdout,stderr=p.communicate()
        if p.returncode or stderr:raise RuntimeError(f'{command}: exit={p.returncode}\n{stdout}{stderr}')
        row=dict(part.split('=',1) for part in stdout.split())
        row={k:v if k=='mode' else int(v) for k,v in row.items()}
        row.update(peak_rss_kib=usage.ru_maxrss,user_cpu_seconds=usage.ru_utime,system_cpu_seconds=usage.ru_stime)
        assert row['nodes']==31 and row['tracks_per_enabled_iteration']==20
        assert row['input_bytes']==sum((FIXTURES/n).stat().st_size for n in MEMBERS)
        assert row['caller_payload_bytes']==33004 and row['workspace_bytes']==18432
        return row,stdout
    calibration=[];raw_calibration=[];iterations={}
    for mode in MODES:
        row,stdout=execute(exe,mode,2048);calibration.append(row);raw_calibration.append(stdout)
        assert row['nanoseconds']>0
        iterations[mode]=args.iterations or max(32768,64*math.ceil(150000000*2048/row['nanoseconds']/64))
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
        stats[mode]={'minimum_ns':min(values),'p05_ns':percentile(values,.05),'p25_ns':percentile(values,.25),
                     'median_ns':statistics.median(values),'p75_ns':percentile(values,.75),'p95_ns':percentile(values,.95),
                     'maximum_ns':max(values),'mean_ns':statistics.mean(values),
                     'sample_duration_ns_range':[min(r['nanoseconds'] for r in rows),max(r['nanoseconds'] for r in rows)]}
        for field in ['peak_rss_kib','post_exec_vm_rss_kib','post_exec_vm_hwm_kib']:
            stats[mode][field+'_range']=[min(r[field] for r in rows),max(r[field] for r in rows)]
    delta=[]
    for sample in range(args.samples):
        pair={r['mode']:r['nanoseconds_per_iteration'] for r in results if r['sample']==sample}
        delta.append(pair[MODES[0]]-pair[MODES[1]])
    for path in [source,Path(__file__),ROOT/'tools/measure_engine_animation49q.py',ROOT/'tools/measure_scene_projection49p.py',*[FIXTURES/n for n in MEMBERS]]:
        hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    check_stable_sources(before,hashes)
    measured,size,detail=sections(exe);symbols=run(['nm','-C',str(exe)])
    assert 'countAllocations' not in symbols and 'operator new(' not in symbols
    for forbidden in ['Machine','original::','scene49p_oracle','sampleModelMotionPair49O','sampleModelLocalMatrices49O']:
        assert forbidden not in symbols
    (EVIDENCE/'timed-executable-symbols.txt').write_text(symbols)
    (EVIDENCE/'timed-executable-size.txt').write_text(size)
    (EVIDENCE/'timed-executable-sections.txt').write_text(detail)
    result={'status':'measured','compiler':run(['g++','--version']).splitlines()[0],
        'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'compile_commands':commands,'source_hashes':hashes,
        'timed_executable_sha256':digest(exe),'ordinary_new_probe_executable_sha256':digest(probe),
        'timed_executable_sections':measured,'cpu_affinity':[0],'available_cpu_affinity':cpus,
        'normal_virtual_address_limit_bytes':cap,'rounding':'FE_TONEAREST checked each process',
        'samples_per_mode':args.samples,'iterations_per_sample_by_mode':iterations,'warmup_enabled_iterations':256,
        'iteration_justification':'One2048-call calibration per mode,150ms target with minimum32768,rounded upward to complete64-call input periods. Explicit--iterations overrides calibration-derived count.',
        'calibration':calibration,'statistics':stats,'percentile_method':'Linear interpolation at sorted index(n-1)*p;15 sample distributions,not tail-latency guarantees.',
        'paired_enabled_minus_disabled_gate_cost':{'median_ns':statistics.median(delta),'p05_ns':percentile(delta,.05),'p95_ns':percentile(delta,.95),'samples_ns':delta,
            'meaning':'Additional enabled animation-stage cost relative to the same API early disabled shared gate,not a49P/49O game baseline or an optimization speedup.'},
        'results':results,'separate_untimed_ordinary_new_probes':probes,
        'fixture':{'meaning':'Actual left/right Pod01 MOT,PSG,VUT bytes with explicit separate supplied model roots and native current filter/scene history.',
            'nodes':31,'left_model_root':0,'left_psg_nodes':list(range(1,15)),'right_model_root':15,'right_psg_nodes':list(range(16,30)),
            'separate_chariot_root':30,'psg_nodes_per_engine':14,'motions_per_enabled_iteration':2,'tracks_per_enabled_iteration':20,
            'track_execution_order':'Left5 rotations,then5 positions;right5 rotations,then5 positions. Original per-channel row order retained.',
            'motion_frames_per_asset':21,'motion_keys_per_track':21,'retained_fixture_bytes':sum((FIXTURES/n).stat().st_size for n in MEMBERS),
            'retained_hot_mot_bytes':sum((FIXTURES/n).stat().st_size for n in MEMBERS if n.endswith('.mot')),
            'caller_payload_bytes_excluding_loader_and_frame_padding':33004,'engine_workspace_bytes':18432,
            'inputs':'dt=nearest binary32(1/60);left targets.1/.9,right.85/.25,switched every32 calls;initial filter values.23/.71,time.2;256 continuous enabled warmup calls precede timing.',
            'current_state':'Loaded once;nullcg translation explicitly zeroed once;then current locals/worlds/flags/model18 and filter values survive all warmup and measured calls. No bindpose/current-pose reset per iteration. Whole64-call periods return to the same settled state,so equal initial/final checksum is expected;the per-call observable sum retains the changing poses/filter values.',
            'cache_state':'Warm serial animation-only stage:affected nodes remain naturally dirty after warmup.No rendering/world-cache refresh occurs between timed calls;untargeted ready cache words remain unchanged.',
            'boundary':'Phase explicitly baseAnimation21A000;shared enable guard already initialized;both engine motion/model/scene gates active;engine Mid records absent;Chariot model and both brake gates supplied true for mirror clamps only.',
            'members':[{'path':str((FIXTURES/n).relative_to(ROOT)),'bytes':(FIXTURES/n).stat().st_size,'sha256':digest(FIXTURES/n)} for n in MEMBERS]},
        'limits':[
            'Native-only host stage timing over two actual14-node Pod01 engine motions. No interpreter/oracle,paired49O sampler,renderer,physics,root-placement producer,Chariot motion/IK or full frame is linked into timed work.',
            'Both modes include public API context/rounding/topology admission,input-fixture selection,success branches and three small observable word sinks. Enabled mode additionally filters and executes all20 source-ordered tracks and transactional graph copies.',
            'Disabled shared gate is a labeled API cost control,not a historical performance baseline. There is no valid direct timing comparison to prior49O load-time geometry or49P different scene operations.',
            'File I/O,PSG/MOT/VUT parsing,binding remap,initial graph supply,continuous warmup,full checksums and/proc memory reads are outside timing.',
            'No allocation counter or interpreter instrumentation is compiled into timed executable. Separate untimed normal probes count ordinary C++ new/new[] for the same iteration count;they do not globally instrument malloc/aligned allocations.',
            'Source/object allocator audit plus separate zero ordinary-new count supports bounded allocation-free hot path;not allocation-failure,ownership or capacity diagnostics.',
            'Caller payload33004 bytes includes31 scene nodes,views/bindings,state/input/models/diagnostic and18432-byte workspace. Six retained source fixtures add131048 bytes;only10296 MOT bytes are consumed by the hot sampler. Loader/runtime/frame costs are additional.',
            'wait4 peak RSS includes pre-exec Python child/startup high-water and cannot be attributed to49Q. Post-exec VmRSS/VmHWM includes loader/runtime,all retained fixtures,caller records and stack.',
            'CPU0 pinning and64MiB RLIMIT_AS are normal host process constraints,not a console-memory simulation or target hardware budget.',
            'Measurements characterize this hot serial dirty-cache animation-stage fixture only.No FPS,Xbox/console,whole-game RAM,fullvisual,FrameReady or memory-reduction claim.']}
    (EVIDENCE/'samples.stdout').write_text(''.join(raw))
    (EVIDENCE/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))

if __name__=='__main__':main()
