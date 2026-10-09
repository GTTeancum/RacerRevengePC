"""Explicit diagnostic-off runner; reference capture behavior remains available."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/capture47V';out.mkdir(exist_ok=True)
src=root/'tools/Run-Native47T.ps1';t=src.read_text()
t=t.replace('    [switch]$DisableGsProfile,', '    [switch]$DisableGsProfile,\n    [switch]$DisableVuCapture,')
t=t.replace('native-run47t-','native-run47v-')
t=t.replace("$Mode=$DiagnosticLines+", "$Mode=([int]$DisableVuCapture.IsPresent).ToString()+'-'+$DiagnosticLines+")
needle="$env:RR_CAPTURE_VU=Join-Path $run 'vu'";assert t.count(needle)==1
t=t.replace(needle,"if($DisableVuCapture){Remove-Item Env:RR_CAPTURE_VU -ErrorAction SilentlyContinue}else{"+needle+"}")
t=t.replace('@{diagnostic_lines47r=', '@{vu_capture_disabled=$DisableVuCapture.IsPresent;diagnostic_lines47r=')
(root/'tools/Run-Native47V.ps1').write_text(t)
(out/'provenance.json').write_text(json.dumps(dict(original=str(src.relative_to(root)),original_sha256=hashlib.sha256(src.read_bytes()).hexdigest(),candidate_sha256=hashlib.sha256(t.encode()).hexdigest(),scope='No executable changes. Unset RR_CAPTURE_VU only with explicit DisableVuCapture switch. Zero COUNT retains legacy unique-program capture semantics.'),indent=2))
