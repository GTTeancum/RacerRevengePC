"""Isolate memory/VIF/VU units; cache measured diagnostic literal settings."""
from pathlib import Path
import hashlib,json,re,shutil
root=Path(__file__).resolve().parents[1];original=root/'work/native/sources/src/PS2Recomp/ps2xRuntime/src/lib';out=root/'build/memory47W';out.mkdir(exist_ok=True)
targets={'ps2_memory.cpp','ps2_vif1_interpreter.cpp','ipu/rr_ipu_memory25.inc','ipu/rr_ipu_unit24.h','vu/rr_vu_capture14.h'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
records=[];changes=[]
for p in original.rglob('*'):
    if not p.is_file():continue
    rel=p.relative_to(original).as_posix();dest=out/'lib'/rel;dest.parent.mkdir(parents=True,exist_ok=True)
    if rel in targets:
        t=p.read_text();keys=re.findall(r'std::getenv\("(RR_(?:TRACE|CAPTURE)_[A-Z0-9_]+)"\)',t);assert keys,rel
        t=re.sub(r'std::getenv\("(RR_(?:TRACE|CAPTURE)_[A-Z0-9_]+)"\)',r'RR47W_MEMORY_ENV("\1")',t)
        dest.write_text('#include "memory_env47w.h"\n'+t)
        changes.append(dict(path=rel,sites=len(keys),names=keys))
    else:shutil.copyfile(p,dest)
    records.append(dict(path=rel,original_sha256=sha(p),candidate_sha256=sha(dest)))
t=(root/'build/combined47T/Probe.cmake').read_text().replace('racer_combined47t_fixed_probe','racer_memoryenv47w_fixed_probe')
t=t.replace('add_executable(racer_memoryenv47w_fixed_probe ', 'add_executable(racer_memoryenv47w_fixed_probe "${RR_SHARED_ROOT}/build/memory47W/lib/ps2_memory.cpp" "${RR_SHARED_ROOT}/build/memory47W/lib/ps2_vif1_interpreter.cpp" "${RR_SHARED_ROOT}/build/memory47W/lib/vu/ps2_vu1_core.cpp" ')
t+='\ntarget_link_options(racer_memoryenv47w_fixed_probe PRIVATE "/MAP:${RR_SHARED_ROOT}/build/memory47W/native.map")\n'
t+='add_custom_command(TARGET racer_memoryenv47w_fixed_probe POST_BUILD COMMAND "C:/Users/LRPC/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe" "${RR_SHARED_ROOT}/tools/check_memory_link47w.py" "${RR_SHARED_ROOT}/build/memory47W/native.map" "${RR_SHARED_ROOT}/build/memory47W/link-check.json" VERBATIM)\n'
t+='add_executable(racer_memoryenv47w_test "${RR_SHARED_ROOT}/tests/memory_env47w.cpp")\n'
t+='target_include_directories(racer_memoryenv47w_test PRIVATE "${RR_SHARED_ROOT}/src/legacy")\ntarget_compile_features(racer_memoryenv47w_test PRIVATE cxx_std_20)\n'
(out/'Probe.cmake').write_text(t)
runner=(root/'tools/Run-Native47V.ps1').read_text().replace('    [switch]$DisableGsProfile,',"    [switch]$DisableGsProfile,\n    [ValidateSet('0','1')][string]$MemoryDiagnostics='1',")
runner=runner.replace('$Mode=([int]$DisableVuCapture.IsPresent)',"$env:RR_CACHE_MEMORY_DIAGNOSTICS47W=$MemoryDiagnostics\n$Mode=$MemoryDiagnostics+'-'+([int]$DisableVuCapture.IsPresent)")
runner=runner.replace('native-run47v-','native-run47w-').replace('racer_combined47t_fixed_probe.exe','racer_memoryenv47w_fixed_probe.exe').replace('evidence/47T/','evidence/47W/').replace('@{vu_capture_disabled=', '@{memory_diagnostics47w=$MemoryDiagnostics;vu_capture_disabled=')
runner=runner.replace('release-opt-47t-event-gate-and-diagnostic-lines','release-opt-47w-memory-ipu-diagnostic-cache')
(root/'tools/Run-Native47W.ps1').write_text(runner)
(out/'provenance.json').write_text(json.dumps(dict(original=str(original),changes=changes,files=records,scope='Only diagnostic getenv literals in memory/VIF implementation and included IPU/VU capture headers; VU implementation recompiled unchanged. No guest instructions, timers, DMA or decoder behavior edited.'),indent=2))
print(json.dumps(changes,indent=2))
