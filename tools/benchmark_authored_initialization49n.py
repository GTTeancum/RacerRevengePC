#!/usr/bin/env python3
"""Bounded single-CPU native-only authored parameter fixture; not game FPS."""
import argparse,hashlib,json,os,resource,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SOURCES=['assets/pod_physics49n','game/authored_parameters49n']
def main():
    p=argparse.ArgumentParser();p.add_argument('--build-dir',type=Path,required=True);a=p.parse_args()
    b=a.build_dir.resolve();assert not b.exists();b.mkdir(parents=True)
    evidence=ROOT/'evidence/authoredInitialization49N/benchmark'
    source=evidence/'authored_benchmark49n.cpp';exe=b/'benchmark'
    command=['g++','-std=c++17','-O2','-I',str(ROOT/'src/assets'),'-fno-fast-math','-frounding-math','-ffp-contract=off',str(source),*[str(ROOT/'src'/f'{s}.cpp') for s in SOURCES],'-o',str(exe)]
    compile=subprocess.run(command,cwd=ROOT,text=True,capture_output=True);assert compile.returncode==0,compile.stderr
    assert not compile.stderr,compile.stderr
    cpu=min(os.sched_getaffinity(0))
    def limits():
        os.sched_setaffinity(0,{cpu});resource.setrlimit(resource.RLIMIT_AS,(67108864,67108864))
    results=[]
    for mode in (0,):
        for repeat in range(5):
            process=subprocess.Popen([str(exe),str(ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy'),'5000'],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,preexec_fn=limits)
            _,status,usage=os.wait4(process.pid,0);process.returncode=os.waitstatus_to_exitcode(status)
            stdout,stderr=process.communicate();assert process.returncode==0 and not stderr,stderr
            row=dict(part.split('=',1) for part in stdout.split() if '=' in part)
            row={k:int(v) for k,v in row.items()};row['peak_rss_kib']=usage.ru_maxrss;row['repeat']=repeat
            assert row['allocations']==0 and row['iterations']==5000
            row['nanoseconds_per_iteration']=row['nanoseconds']/row['iterations'];results.append(row)
    assert len({r['checksum'] for r in results})==1
    files=[source,ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy',ROOT/'tools/benchmark_authored_initialization49n.py']
    for name in SOURCES:files.extend([ROOT/'src'/f'{name}.h',ROOT/'src'/f'{name}.cpp'])
    result={'status':'measured','command':command,'cpu_affinity':[cpu],'normal_virtual_address_limit_bytes':67108864,'iterations_per_sample':5000,'samples':5,'source_hashes':{f.relative_to(ROOT).as_posix():hashlib.sha256(f.read_bytes()).hexdigest() for f in files},'results':results,'limits':['Exact Pod01 PHY read, source-ordered numeric binding and49M projection, with declared synthetic selection/profile/prior context.','Input read outside timing; caller-owned records reused, including full parsing each iteration.','No physics tick, world, renderer, transport or original instruction oracle in timed loop.','wait4 peak RSS includes launch overhead; fixed records are measured separately.','Host measurements only; not target console performance or gameplay FPS.']}
    (evidence/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
