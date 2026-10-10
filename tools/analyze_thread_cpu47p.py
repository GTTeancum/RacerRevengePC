"""Summarize OS CPU counters without equating host cycles to Xbox performance."""
from pathlib import Path
import argparse,json,hashlib
p=argparse.ArgumentParser();p.add_argument('evidence',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
path=a.evidence/'samples.jsonl';samples=[json.loads(x) for x in path.read_text().splitlines() if x.strip()]
assert len(samples)>=2
assert all(x['elapsed_ns']<y['elapsed_ns'] for x,y in zip(samples,samples[1:]))
def total(t):return t['user_100ns']+t['kernel_100ns']
def key(t):return t['tid'],t['created_100ns']
def summarize(rows):
    first,last=rows[0],rows[-1];wall=(last['elapsed_ns']-first['elapsed_ns'])/1e9
    before={key(t):t for t in first['threads']};after={key(t):t for t in last['threads']};result=[]
    for identity in before.keys()&after.keys():
        b,e=before[identity],after[identity]
        cpu=(total(e)-total(b))/1e7;user=(e['user_100ns']-b['user_100ns'])/1e7;kernel=(e['kernel_100ns']-b['kernel_100ns'])/1e7
        assert cpu>=0 and user>=0 and kernel>=0
        cycles=e['cycles']-b['cycles'] if e['cycles'] is not None and b['cycles'] is not None else None
        result.append(dict(tid=e['tid'],created_100ns=e['created_100ns'],name=e['name'],cpu_seconds=cpu,user_seconds=user,kernel_seconds=kernel,logical_core_equivalents=cpu/wall,host_thread_cycles=cycles))
    result.sort(key=lambda r:r['cpu_seconds'],reverse=True)
    proc=((last['process_user_100ns']+last['process_kernel_100ns'])-(first['process_user_100ns']+first['process_kernel_100ns']))/1e7
    accounted=sum(r['cpu_seconds'] for r in result)
    for r in result:r['share_of_process_cpu_percent']=100*r['cpu_seconds']/proc if proc else None
    return dict(samples=len(rows),wall_seconds=wall,first_utc=first['utc'],last_utc=last['utc'],first_latest_capture=first['frame'],last_latest_capture=last['frame'],
        process_cpu_seconds=proc,process_logical_core_equivalents=proc/wall,persistent_threads_cpu_seconds=accounted,process_minus_persistent_cpu_seconds=proc-accounted,
        persistent_threads=result,threads_missing_at_one_boundary=len(before.keys()^after.keys()),counter_read_errors=sum(len(r['thread_errors']) for r in rows))
report=dict(observer=json.loads((a.evidence/'observer.json').read_text()),samples_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),full_observed_interval=summarize(samples),race_capture_subset=None,
    caveats=['Logical-core equivalents are CPU seconds divided by wall seconds, not percent of all machine cores.','Only threads present at both interval boundaries are attributed; process total includes other threads. Snapshots are sequential rather than atomic.','Latest capture VSync values can lag the CPU observation by the capture interval; these are not exact 6000/6900 timing markers.','Host thread cycles cannot be scaled directly to Pentium III or converted into Xbox FPS.','No routine, module, or thread role is inferred from an empty thread description.'])
subset=[r for r in samples if r['frame'] and 6000<=r['frame']['vsync']<=6900]
if len(subset)>=2:report['race_capture_subset']=summarize(subset)
end=a.evidence/'end.json'
if end.exists():report['observer_terminal']=json.loads(end.read_text())
with a.output.open('x') as f:json.dump(report,f,indent=2)
compact=report['race_capture_subset'] or report['full_observed_interval']
print(json.dumps({k:compact[k] for k in ['samples','wall_seconds','first_latest_capture','last_latest_capture','process_cpu_seconds','process_logical_core_equivalents','counter_read_errors']},indent=2))
print(json.dumps(compact['persistent_threads'][:5],indent=2))
