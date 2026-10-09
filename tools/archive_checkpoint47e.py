"""Archive the compact adapter and completed baseline; exclude the live partner."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

root=Path(__file__).resolve().parents[1]
out=root/'outputs'
tag='RacerRevenge-Checkpoint47E-CompactNativeAdapter'
assert not list(out.glob(tag+'*.zip'))
def sha(p):
    with p.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def files(d): return [p for p in (root/d).rglob('*') if p.is_file()]
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += files('build/compact47E')
source += [out/(tag+'.md')]
evidence=files('evidence/47E')
for d in ['evidence/windows-20260930-131429-273','evidence/xbox-20260930-131419-426']:
    evidence += files(d)
reference='work/native-run47c-baseline-20260930-124035-421'
assert (root/reference/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
evidence += files(reference)
evidence += [root/p for p in ['build/windows-x86/racer_core_check.exe',
    'build/windows-x86/racer_xbox_objects_check.exe','build/windows-x64/racer_core_check.exe',
    'build/xbox-core/default.xbe','src/core/cycle_ledger.obj','src/core/compact_registry.obj',
    'tests/core_suite.obj','platform/xbox/main.obj','platform/xbox/main.exe',
    'work/native/ninja-build-release-opt/racer_compact47e_probe.exe',
    'work/native/ninja-build-release-opt/racer_compact47e_contract.exe',
    'evidence/47C-progress-1306.json','evidence/47C-baseline-preview.png','evidence/47C-baseline-later-preview.png',
    'evidence/native-pair47C-20260930-124035-308/pair.json']]
partner=root/'work/native-run47c-batched-20260930-131039-775'
for name in ['process-launch.json','launch.json','binary-hash.json','bridge-hash.json','buttons.txt','cards-before.json']:
    evidence.append(partner/name)
dependencies=[]
for name in ['RacerRevenge-Checkpoint47D-CompactRegistry-Source.zip',
             'RacerRevenge-Checkpoint47D-CompactRegistry-Evidence.zip',
             'RacerRevenge-Checkpoint47B-IdleDeadlineCandidate-Source.zip']:
    p=out/name
    dependencies.append({'path':name,'sha256':sha(p),'bytes':p.stat().st_size})
manifest={'created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'goal_status':'active/incomplete','dependencies':dependencies,
    'legacy_dependency':'46G and transitive chain; nxdk/MSYS2 via 47A',
    'runtime_registry_contract':'all 733427 slots before and after per-address replacement checks; zero mismatches',
    'portable_checks_per_build':149413,'compact_native_game_validation':'not yet run',
    'baseline47C_exit':0,'batched47C_status':'running; stable launch artifacts only',
    'xbox_execution_verified':False,'game_speedup_verified':False}
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
print('All archive members reopened and verified.',flush=True)
