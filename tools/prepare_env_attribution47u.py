"""Add a diagnostic-only getenv import shim to the combined executable."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/env47U';out.mkdir(exist_ok=True)
src=root/'build/combined47T/Probe.cmake'
t=src.read_text().replace('racer_combined47t_fixed_probe','racer_env47u_fixed_probe')
t=t.replace('add_executable(racer_env47u_fixed_probe ', 'add_executable(racer_env47u_fixed_probe "${RR_SHARED_ROOT}/src/legacy/env_attribution47u.cpp" ')
t+='\ntarget_link_options(racer_env47u_fixed_probe PRIVATE "/MAP:${RR_SHARED_ROOT}/build/env47U/native.map")\n'
t+='add_executable(racer_env47u_test "${RR_SHARED_ROOT}/tests/env_attribution47u.cpp" "${RR_SHARED_ROOT}/src/legacy/env_attribution47u.cpp")\n'
t+='target_compile_features(racer_env47u_test PRIVATE cxx_std_20)\n'
(out/'Probe.cmake').write_text(t)
runner=(root/'tools/Run-Native47T.ps1').read_text()
runner=runner.replace('native-run47t-','native-run47u-').replace('racer_combined47t_fixed_probe.exe','racer_env47u_fixed_probe.exe').replace('evidence/47T/','evidence/47U/')
runner=runner.replace("New-Item -ItemType Directory -Path $run | Out-Null", "New-Item -ItemType Directory -Path $run | Out-Null\n$env:RR_ENV_ATTRIBUTION47U=Join-Path $run 'env-sites'")
runner=runner.replace('release-opt-47t-event-gate-and-diagnostic-lines','diagnostic-47u-executable-getenv-caller-counts')
(root/'tools/Run-Native47U.ps1').write_text(runner)
inputs=['build/combined47T/Probe.cmake','src/legacy/env_attribution47u.cpp','tests/env_attribution47u.cpp','tools/Run-Native47T.ps1']
(out/'provenance.json').write_text(json.dumps(dict(inputs=[dict(path=p,sha256=hashlib.sha256((root/p).read_bytes()).hexdigest()) for p in inputs],scope='Executable getenv import attribution only; original result forwarded; names and caller RVAs only; instrumented timing not a benchmark.'),indent=2))
