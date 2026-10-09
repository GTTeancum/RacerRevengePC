"""Clone accepted IOP; cache only launch-time diagnostic settings, with off mode."""
from pathlib import Path
import hashlib,json,re,shutil
root=Path(__file__).resolve().parents[1];original=root/'build/iop47J2';out=root/'build/iop47Q'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
out.mkdir(exist_ok=True);records=[];changed=[]
for d in ['src','include','tests']:
    for p in (original/d).rglob('*'):
        if not p.is_file():continue
        rel=p.relative_to(original);dest=out/rel;dest.parent.mkdir(parents=True,exist_ok=True)
        text=p.read_text();keys=re.findall(r'std::getenv\("(RR_(?:TRACE|CAPTURE)_[A-Z0-9_]+)"\)',text)
        if keys:
            text=re.sub(r'std::getenv\("(RR_(?:TRACE|CAPTURE)_[A-Z0-9_]+)"\)',r'RR47Q_DIAGNOSTIC_ENV("\1")',text)
            text='#include "launch_env47q.h"\n'+text
            changed.append(dict(path=rel.as_posix(),sites=len(keys),settings=keys))
        dest.write_text(text);records.append(dict(path=rel.as_posix(),original_sha256=sha(p),candidate_sha256=sha(dest)))
for name in ['CMakeLists.txt','differential47j2.cpp','memory_reference47j.cpp']:
    shutil.copyfile(original/name,out/name)
    records.append(dict(path=name,original_sha256=sha(original/name),candidate_sha256=sha(out/name)))
(out/'provenance.json').write_text(json.dumps(dict(original=str(original),changes=changed,files=records,scope='IOP only; diagnostic launch values cached. Game settings, guest instructions, scheduling and tracing payloads unchanged.'),indent=2))
print('IOP diagnostic getenv sites:',sum(x['sites'] for x in changed),'in',len(changed),'files')
# Reuse the accepted probe/GS/registry/leaf wiring, changing only the IOP objects.
cmake=(root/'tools/GsProfile47O.cmake').read_text()
begin=cmake.index('add_executable(');cmake=cmake[begin:]
cmake=cmake.replace('racer_gsprofile47o_fixed_probe','racer_launch47q_fixed_probe').replace('rr_iop47j2','rr_iop47q')
(out/'Probe.cmake').write_text(cmake)
runner=(root/'tools/Run-Native47O.ps1').read_text()
runner=runner.replace('    [switch]$DisableGsProfile,','    [switch]$DisableGsProfile,\n    [ValidateSet(\'0\',\'1\')][string]$DiagnosticCache=\'1\',')
runner=runner.replace("$Mode=([int](-not $DisableGsProfile.IsPresent)).ToString()+", "$env:RR_CACHE_DIAGNOSTICS47Q=$DiagnosticCache\n$Mode=$DiagnosticCache+'-'+([int](-not $DisableGsProfile.IsPresent)).ToString()+")
runner=runner.replace('native-run47o-','native-run47q-').replace('racer_gsprofile47o_fixed_probe.exe','racer_launch47q_fixed_probe.exe').replace('evidence/47O/build-gate.json','evidence/47Q/build-gate.json')
runner=runner.replace('@{gs_profile47o=', '@{diagnostic_cache47q=$DiagnosticCache;gs_profile47o=').replace('release-opt-47o-renderer-call-profile','release-opt-47q-iop-diagnostic-cache')
(root/'tools/Run-Native47Q.ps1').write_text(runner)
