"""Immutable archive of compact-registry validation, excluding active pilots."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

root=Path(__file__).resolve().parents[1]
out=root/'outputs'
tag='RacerRevenge-Checkpoint47D-CompactRegistry'
assert not list(out.glob(tag+'*.zip'))
def sha(p):
    with p.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def files(d): return [p for p in (root/d).rglob('*') if p.is_file()]
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += files('build/registry47D')
source += [root/'work/native/project'/p for p in ['generated/register_functions.cpp',
    'src/resume-registration.inc','src/recovered10/register.cpp','src/recovered14/register.cpp']]
source += [out/(tag+'.md')]
evidence=files('evidence/47D')
for d in ['evidence/windows-20260930-125102-728','evidence/xbox-20260930-125058-735']:
    evidence += files(d)
evidence += [root/p for p in ['build/windows-x86/racer_core_check.exe',
    'build/windows-x86/racer_xbox_objects_check.exe','build/windows-x64/racer_core_check.exe',
    'build/xbox-core/default.xbe','src/core/cycle_ledger.obj','src/core/compact_registry.obj',
    'tests/core_suite.obj','platform/xbox/main.obj','platform/xbox/main.exe',
    'work/native/ninja-build-release-opt/racer_registry47d_check.exe']]
dependencies=[]
for name in ['RacerRevenge-Checkpoint47B-IdleDeadlineCandidate-Source.zip',
             'RacerRevenge-Checkpoint47B-IdleDeadlineCandidate-Evidence.zip',
             'RacerRevenge-Checkpoint47A-WindowsXboxSharedCore-Source.zip',
             'RacerRevenge-Checkpoint47A-WindowsXboxSharedCore-Toolchain.zip']:
    p=out/name
    dependencies.append({'path':name,'sha256':sha(p),'bytes':p.stat().st_size})
manifest={'created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'goal_status':'active/incomplete','dependencies':dependencies,
    'legacy_dependency':'46G plus its documented transitive recovery chain',
    'linked_original_registry_slots':733427,'linked_registered_slots':154796,
    'native_pointer_mismatches':0,'portable_checks_per_build':149391,
    'compact_game_integration':False,'xbox_execution':False,'native47C_pair':'pending'}
for label,paths in [('Source',source),('Evidence',evidence)]:
    archive=out/(tag+'-'+label+'.zip')
    records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            assert p.is_file(),p
            name=p.relative_to(root).as_posix()
            digest=sha(p)
            z.write(p,name)
            assert sha(p)==digest,p
            records.append({'path':name,'bytes':p.stat().st_size,'sha256':digest})
    with zipfile.ZipFile(archive) as z:
        assert len(z.namelist())==len(records)
        for r in records:
            with z.open(r['path']) as f: assert hashlib.file_digest(f,'sha256').hexdigest()==r['sha256'],r['path']
    manifest[label]={'path':archive.name,'bytes':archive.stat().st_size,'sha256':sha(archive),'files':records}
    print(label,manifest[label]['sha256'],len(records),flush=True)
(out/(tag+'-Manifest.json')).write_text(json.dumps(manifest,indent=2),encoding='utf-8')
print('All archive members reopened and SHA-256 verified.',flush=True)
