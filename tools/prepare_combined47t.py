"""Compose validated mechanisms without editing either implementation."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/combined47T';out.mkdir(exist_ok=True)
inputs=['build/event47S2/Probe.cmake','build/event47S2/EeScheduler.cpp','build/diagnostic47R/probe.cpp','src/legacy/diagnostic_lines47r.h']
records=[dict(path=p,sha256=hashlib.sha256((root/p).read_bytes()).hexdigest()) for p in inputs]
t=(root/inputs[0]).read_text().replace('racer_event47s2_fixed_probe','racer_combined47t_fixed_probe').replace('build/memory47N/memory-probe.cpp','build/diagnostic47R/probe.cpp')
assert 'build/diagnostic47R/probe.cpp' in t and 'build/event47S2/EeScheduler.cpp' in t
(out/'Probe.cmake').write_text(t)
t=(root/'tools/Run-Native47S2.ps1').read_text().replace('    [switch]$DisableGsProfile,',"    [switch]$DisableGsProfile,\n    [ValidateSet('0','1')][string]$DiagnosticLines='1',")
t=t.replace('$Mode=$EventGate+',"$env:RR_DIAGNOSTIC_LINES47R=$DiagnosticLines\n$Mode=$DiagnosticLines+'-'+$EventGate+")
t=t.replace('native-run47s2-','native-run47t-').replace('racer_event47s2_fixed_probe.exe','racer_combined47t_fixed_probe.exe').replace('evidence/47S2/','evidence/47T/').replace('@{event_gate47s=', '@{diagnostic_lines47r=$DiagnosticLines;event_gate47s=')
t=t.replace('release-opt-47s2-zero-safe-event-gate','release-opt-47t-event-gate-and-diagnostic-lines')
(root/'tools/Run-Native47T.ps1').write_text(t)
(out/'provenance.json').write_text(json.dumps(dict(inputs=records,scope='Combine unchanged conservative event gate and unchanged diagnostic line buffer; both independently switchable in one executable.'),indent=2))
print('Prepared combined candidate with independent event/log switches.')
