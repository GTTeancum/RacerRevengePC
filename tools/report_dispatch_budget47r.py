"""Arithmetic design budget from measured dispatch counts; not an Xbox forecast."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
p=root/'evidence/47K/profile/race-window.json';r=json.loads(p.read_text())
loops=r['end']['loops']-r['start']['loops'];wall=(r['end']['wall_ns']-r['start']['wall_ns'])/1e9
# 47K endpoints are near, rather than exactly at, VBlank 6000 and 6900.
# Use the documented nominal 900-event window only as an approximate budget.
dispatches_per_vblank=loops/900
out=dict(source=str(p.relative_to(root)),source_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),dispatch_iterations=loops,
    measured_wall_seconds=wall,mean_host_ns_per_dispatch_including_all_work=wall*1e9/loops,
    nominal_vblank_span=900,approximate_dispatches_per_vblank=dispatches_per_vblank,
    target_cpu_hz=733000000,target_budgets=[dict(vblanks_per_second=rate,whole_cpu_cycles_per_vblank=733000000/rate,whole_cpu_cycles_per_dispatch_at_same_loop_density=733000000/rate/dispatches_per_vblank) for rate in [30,60]],
    caveats=['Approximate design constraint because source profile endpoints are near the named boundaries.','Budget assigns every target CPU cycle to these dispatch intervals, including all guest, IOP, VU, rendering and platform work; it is not an available per-function allowance.','No host-cycle scaling or Xbox FPS forecast is implied.','Goal requires reducing dispatch density or total work as well as code size; diagnostic cleanup alone does not establish feasibility.'])
dest=root/'evidence/dispatch-budget47R.json'
with dest.open('x') as f:json.dump(out,f,indent=2)
print(json.dumps(out,indent=2))
