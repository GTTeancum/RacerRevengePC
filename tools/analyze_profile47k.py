"""Summarize sampled elapsed time without treating nested costs as exclusive."""
from pathlib import Path
import argparse
import csv
import hashlib
import io
import json
import re

p=argparse.ArgumentParser()
p.add_argument('profile',type=Path)
p.add_argument('--subtract',type=Path)
p.add_argument('--output',type=Path,required=True)
a=p.parse_args()
def read(path):
    data=path.read_bytes(); text=data.decode()
    header=re.search(r'# loops=(\d+) sampled=(\d+) wall_ns=(\d+)',text)
    assert header
    rows={}
    for row in csv.DictReader(io.StringIO('\n'.join(line for line in text.splitlines() if not line.startswith('#')))):
        rows[(row['kind'],row['key'])]={key:int(row[key]) for key in ('samples','elapsed_ns','max_ns')}
    return dict(path=str(path),sha256=hashlib.sha256(data).hexdigest(),
                loops=int(header[1]),sampled=int(header[2]),wall_ns=int(header[3])),rows
meta,end=read(a.profile)
startmeta,start=read(a.subtract) if a.subtract else (None,{})
delta=[]
for (kind,key),row in end.items():
    before=start.get((kind,key),{'samples':0,'elapsed_ns':0})
    count=row['samples']-before['samples']; ns=row['elapsed_ns']-before['elapsed_ns']
    assert count>=0 and ns>=0
    if count: delta.append(dict(kind=kind,key=key,samples=count,elapsed_ns=ns))
denominator=sum(row['elapsed_ns'] for row in delta if row['kind']=='phase')
for row in delta: row['percent_sampled_phase_time']=100*row['elapsed_ns']/denominator
report=dict(end=meta,start=startmeta,sampled_phase_ns=denominator,
    phases=[r for r in delta if r['kind']=='phase'],nested=[r for r in delta if r['kind']=='nested'],
    top_guest_entries=sorted([r for r in delta if r['kind']=='guest'],key=lambda r:r['elapsed_ns'],reverse=True)[:30],
    caveats=['Host elapsed samples include preemption, profiler overhead and nested work.',
             'Nested categories overlap; do not add them or subtract all of them for an exclusive CPU cost.',
             'Guest keys are scheduler entry PCs, not isolated function timings.',
             'GS worker/GPU time is not separately measured; capture boundaries are approximate.',
             'This profiled run is not a matched FPS benchmark or an Xbox forecast.'])
a.output.parent.mkdir(parents=True,exist_ok=True)
a.output.write_text(json.dumps(report,indent=2))
print(json.dumps(dict(phases=report['phases'],nested=report['nested'],top_guest_entries=report['top_guest_entries'][:5]),indent=2))
