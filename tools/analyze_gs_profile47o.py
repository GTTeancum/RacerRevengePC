"""Validate cumulative GS call measurements and calculate bounded windows."""
from pathlib import Path
import argparse,csv,json,hashlib
p=argparse.ArgumentParser();p.add_argument('run',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--from-vsync',type=int,default=6000);p.add_argument('--to-vsync',type=int,default=6900);a=p.parse_args()
path=a.run/'gs-profile.csv';rows=list(csv.DictReader(path.open(newline='')))
assert rows
metrics={'packet','vsync','flush','fifo','snapshot','vram','read','reset','open'}
groups={};previous={};last_host=0
for row in rows:
    assert row['metric'] in metrics
    for key in row:
        if key not in ('stage','metric'):row[key]=int(row[key]);assert row[key]>=0
    assert row['host_ns']>=last_host;last_host=row['host_ns']
    before=previous.get(row['metric'])
    if before:
        for key in ('vsync_calls','host_ns','calls','elapsed_ns','packet_bytes','p16','p64','p256','p4096','p65536','plarge'):assert row[key]>=before[key],(key,row)
    previous[row['metric']]=row
    key=(row['stage'],row['vsync_calls'],row['host_ns']);group=groups.setdefault(key,{})
    assert row['metric'] not in group;group[row['metric']]=row
for group in groups.values():
    assert set(group)==metrics
    assert group['packet']['calls']==sum(group['packet'][k] for k in ('p16','p64','p256','p4096','p65536','plarge'))
    assert group['vsync']['calls']==group['vsync']['vsync_calls']
def describe(start,end):
    host=end['vsync']['host_ns']-(start['vsync']['host_ns'] if start else 0)
    result=[]
    for name in sorted(metrics):
        b=end[name];before=start[name] if start else {'calls':0,'elapsed_ns':0}
        calls=b['calls']-before['calls'];ns=b['elapsed_ns']-before['elapsed_ns']
        result.append(dict(metric=name,calls=calls,elapsed_ns=ns,mean_ns=ns/calls if calls else None,percent_host_elapsed=100*ns/host if host else None))
    assert sum(r['elapsed_ns'] for r in result)<=host,'Serialized disjoint DLL call time exceeds host interval'
    return dict(host_elapsed_ns=host,metrics=result,measured_dll_call_percent=sum(r['percent_host_elapsed'] or 0 for r in result))
final=list(groups.values())[-1]
report=dict(run=str(a.run),csv_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),snapshots=len(groups),final_stage=final['vsync']['stage'],final_vsync_calls=final['vsync']['vsync_calls'],full_run=describe(None,final),window=None,
    caveats=['Host elapsed includes DLL producer work, GS-thread work, blocking waits and preemption.','Does not separate exclusive CPU, queue wait or GPU time.','Profile overhead makes this a diagnostic run, not a matched throughput benchmark.','Backend VSync count must be reconciled with scheduler fixed-stop count before calling the requested window a game-VBlank window.'])
matches={count:group for (stage,count,_),group in groups.items() if stage=='vsync'}
if a.from_vsync in matches and a.to_vsync in matches:
    report['window']=dict(from_backend_vsync=a.from_vsync,to_backend_vsync=a.to_vsync,**describe(matches[a.from_vsync],matches[a.to_vsync]))
stop=a.run/'fixed-stop-validation.json'
if stop.exists():
    validation=json.loads(stop.read_text(encoding='utf-8-sig'))
    report['terminal_vsync_reconciled']=bool(validation['verified'] and validation['expected_vblank']==report['final_vsync_calls'] and report['final_stage']=='close')
a.output.parent.mkdir(exist_ok=True,parents=True)
with a.output.open('x') as f:json.dump(report,f,indent=2)
print(json.dumps(report,indent=2))
