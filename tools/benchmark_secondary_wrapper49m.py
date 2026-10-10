#!/usr/bin/env python3
"""Bounded single-CPU native-only secondary fixture measurement; not game FPS."""
import argparse,hashlib,json,os,resource,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SOURCES=['secondary_body49m','gradient_noise49m','pod_sphere_constraint49l','body_wrapper48h','body_scene48g','body_integration48c','vehicle_aero48e','vehicle_forces48d']
def main():
    p=argparse.ArgumentParser();p.add_argument('--build-dir',type=Path,required=True);a=p.parse_args()
    b=a.build_dir.resolve();assert not b.exists();b.mkdir(parents=True)
    evidence=ROOT/'evidence/secondaryWrapper49M/benchmark'
    source=evidence/'secondary_benchmark49m.cpp';exe=b/'benchmark'
    command=['g++','-std=c++17','-O2','-I',str(ROOT/'src/game'),'-fno-fast-math','-frounding-math','-ffp-contract=off',str(source),*[str(ROOT/'src/game'/f'{s}.cpp') for s in SOURCES],'-o',str(exe)]
    compile=subprocess.run(command,cwd=ROOT,text=True,capture_output=True);assert compile.returncode==0,compile.stderr
    assert not compile.stderr,compile.stderr
    cpu=min(os.sched_getaffinity(0))
    def limits():
        os.sched_setaffinity(0,{cpu});resource.setrlimit(resource.RLIMIT_AS,(67108864,67108864))
    results=[]
    for mode in (0,1):
        for repeat in range(5):
            process=subprocess.Popen([str(exe),'100000',str(mode)],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,preexec_fn=limits)
            _,status,usage=os.wait4(process.pid,0);process.returncode=os.waitstatus_to_exitcode(status)
            stdout,stderr=process.communicate();assert process.returncode==0 and not stderr,stderr
            row=dict(part.split('=',1) for part in stdout.split() if '=' in part)
            row={k:int(v) for k,v in row.items()};row['peak_rss_kib']=usage.ru_maxrss;row['repeat']=repeat
            assert row['allocations']==0 and row['iterations']==100000 and row['state']==mode
            row['nanoseconds_per_iteration']=row['nanoseconds']/row['iterations'];results.append(row)
    assert len({r['checksum'] for r in results if r['state']==0})==len({r['checksum'] for r in results if r['state']==1})==1
    files=[source,ROOT/'tests/native/secondary_body_api49m.cpp',ROOT/'tools/benchmark_secondary_wrapper49m.py']
    for name in SOURCES:files.extend([ROOT/'src/game'/f'{name}.h',ROOT/'src/game'/f'{name}.cpp'])
    result={'status':'measured','command':command,'cpu_affinity':[cpu],'normal_virtual_address_limit_bytes':67108864,'iterations_per_sample':100000,'samples_per_mode':5,'source_hashes':{f.relative_to(ROOT).as_posix():hashlib.sha256(f.read_bytes()).hexdigest() for f in files},'results':results,'limits':['Native-only reset synthetic fixture: prefix,49L,identity supplied368 state,secondary48H.','Noise initialized outside timing; shared noise state and body/joint fixture copied inside loop.','Includes caller-owned record copies; no world, renderer, device input or original instruction oracle in timed loop.','Host measurements only; not target console performance or gameplay FPS.']}
    (evidence/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
