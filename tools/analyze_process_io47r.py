"""Counter deltas only; IO counts do not identify destination or exclusive cost."""
from pathlib import Path
import argparse,json,hashlib
p=argparse.ArgumentParser();p.add_argument('evidence',type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
path=a.evidence/'samples.jsonl';rows=[json.loads(s) for s in path.read_text().splitlines()]
assert (a.evidence/'end.json').exists() and all(r.get('io') for r in rows)
def interval(rows):
    first,last=rows[0],rows[-1];seconds=(last['elapsed_ns']-first['elapsed_ns'])/1e9
    delta={k:last['io'][k]-first['io'][k] for k in first['io']};assert all(v>=0 for v in delta.values())
    return dict(first_utc=first['utc'],last_utc=last['utc'],wall_seconds=seconds,first_frame=first['frame'],last_frame=last['frame'],io=delta,write_operations_per_second=delta['write_operations']/seconds)
race=[r for r in rows if r['frame'] and 6000<=r['frame']['vsync']<=6900]
report=dict(samples_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),full=interval(rows),capture_bounded_race=interval(race) if len(race)>1 else None,
    caveats=['All process IO destinations and threads are included.','Counts do not attribute file paths, blocked time or exclusive CPU cost.','Latest capture markers lag observations; boundary is not exactly VBlank 6000–6900.','Control 47Q-0 timing is potentially contaminated by the documented source search.'])
with a.output.open('x') as f:json.dump(report,f,indent=2)
print(json.dumps(report,indent=2))
