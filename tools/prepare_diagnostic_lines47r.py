from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/diagnostic47R';out.mkdir(exist_ok=True)
original=root/'build/memory47N/memory-probe.cpp';text=original.read_text()
needle='int main(int argc,char** argv) {';assert text.count(needle)==1
text='#include "diagnostic_lines47r.h"\n'+text.replace(needle,needle+'\n    rr47r::Scope diagnosticLines;')
assert text.count('return runStatus;')==1;text=text.replace('return runStatus;','if(!diagnosticLines.finish())return 13;\n        return runStatus;')
(out/'probe.cpp').write_text(text)
(out/'provenance.json').write_text(json.dumps(dict(original=str(original.relative_to(root)),original_sha256=hashlib.sha256(original.read_bytes()).hexdigest(),candidate_sha256=hashlib.sha256((out/'probe.cpp').read_bytes()).hexdigest(),scope='Diagnostic stderr line batching only; no guest/runtime/renderer changes.'),indent=2))
t=(root/'build/iop47Q/Probe.cmake').read_text().replace('racer_launch47q_fixed_probe','racer_diagnostic47r_fixed_probe').replace('build/memory47N/memory-probe.cpp','build/diagnostic47R/probe.cpp')
(out/'Probe.cmake').write_text(t)
t=(root/'tools/Run-Native47Q.ps1').read_text().replace("    [switch]$DisableGsProfile,","    [switch]$DisableGsProfile,\n    [ValidateSet('0','1')][string]$DiagnosticLines='1',")
t=t.replace("$Mode=$DiagnosticCache+", "$env:RR_DIAGNOSTIC_LINES47R=$DiagnosticLines\n$Mode=$DiagnosticLines+'-'+$DiagnosticCache+")
t=t.replace('native-run47q-','native-run47r-').replace('racer_launch47q_fixed_probe.exe','racer_diagnostic47r_fixed_probe.exe').replace('evidence/47Q/build-gate.json','evidence/47R/build-gate.json').replace('@{diagnostic_cache47q=', '@{diagnostic_lines47r=$DiagnosticLines;diagnostic_cache47q=')
(root/'tools/Run-Native47R.ps1').write_text(t)
print('Prepared bounded diagnostic line buffering, switchable in one native binary.')
