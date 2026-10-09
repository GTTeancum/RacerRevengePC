"""Freeze 47B's source and closed evidence; exclude mutable game-run captures."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

root = Path(__file__).resolve().parents[1]
out = root / 'outputs'
tag = 'RacerRevenge-Checkpoint47B-IdleDeadlineCandidate'
assert not list(out.glob(tag + '*.zip')), 'Never overwrite a checkpoint'

def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def files(directory):
    return [p for p in (root / directory).rglob('*') if p.is_file()]

source = [root / p for p in ['.gitattributes', '.gitignore', 'README.md', 'CMakeLists.txt']]
for directory in ['src', 'tests', 'platform', 'tools', 'docs']:
    source += [p for p in files(directory) if p.suffix in {'.cpp','.h','.py','.ps1','.cmake','.md','.json'} or p.name == 'Makefile']
for directory in ['build/legacy-iop47B', 'build/legacy-iop47B2',
                  'work/native/sources/src/PS2Recomp/ps2xIOP']:
    source += files(directory)
source += [root / 'work/run-native45z.ps1', out / (tag + '.md')]

evidence = files('evidence/47B')
evidence += [root / p for p in ['evidence/47A/generated-audit.json', 'evidence/47A/generated-audit.log']]
for directory in ['evidence/windows-20260930-122840-158', 'evidence/windows-20260930-123833-502',
                  'evidence/windows-20260930-123948-017', 'evidence/xbox-20260930-123804-340']:
    evidence += files(directory)
for directory in ['build/windows-x86', 'build/windows-x64']:
    evidence += [p for p in (root / directory).glob('*.exe') if p.is_file()]
evidence += [root / p for p in ['build/reference46D/racer_native_probe.exe',
    'build/xbox-core/default.xbe', 'platform/xbox/main.exe', 'platform/xbox/main.obj',
    'src/core/cycle_ledger.obj', 'tests/core_suite.obj', 'work/diagnostic45y/ps2_iop.lib',
    'work/native/ninja-build-release-opt/racer_native_probe.exe',
    'work/native/ninja-build-release-opt/racer_desktop.exe',
    'work/native/ninja-build-release-opt/CMakeCache.txt',
    'evidence/native-pair47C-20260930-124035-308/pair.json']]
run = root / 'work/native-run47c-baseline-20260930-124035-421'
for name in ['launch.json','process-launch.json','binary-hash.json','bridge-hash.json','buttons.txt','cards-before.json']:
    evidence.append(run / name)

dependencies = []
for name in ['RacerRevenge-Checkpoint47A-WindowsXboxSharedCore-Source.zip',
             'RacerRevenge-Checkpoint47A-WindowsXboxSharedCore-Toolchain.zip',
             'RacerRevenge-Checkpoint47A-WindowsXboxSharedCore-Evidence.zip']:
    path = out / name
    dependencies.append({'path':name, 'sha256':sha(path), 'bytes':path.stat().st_size})
manifest = {'created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'goal_status':'active/incomplete', 'dependencies':dependencies,
    'legacy_dependency':'checkpoint46G and its documented transitive source chain',
    'game_pair':'in progress; only stable launch artifacts included',
    'native_game_speedup_verified':False, 'xbox_execution_verified':False,
    'core_checks_per_build':149373, 'core_digest':'947da74c',
    'differential_checkpoints':372, 'legacy_known_failures_preserved':True}
for label, paths in [('Source',source),('Evidence',evidence)]:
    archive = out / (tag + '-' + label + '.zip')
    records = []
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for path in sorted(set(paths)):
            assert path.is_file(), path
            relative = path.relative_to(root).as_posix()
            digest = sha(path)
            size = path.stat().st_size
            z.write(path,relative)
            assert sha(path) == digest, f'File changed while archiving: {path}'
            records.append({'path':relative,'bytes':size,'sha256':digest})
    with zipfile.ZipFile(archive) as z:
        assert len(z.namelist()) == len(records)
        for record in records:
            with z.open(record['path']) as stream:
                assert hashlib.file_digest(stream,'sha256').hexdigest() == record['sha256'], record['path']
    manifest[label] = {'path':archive.name,'bytes':archive.stat().st_size,'sha256':sha(archive),'files':records}
    print(label,manifest[label]['sha256'],len(records),flush=True)
(out / (tag+'-Manifest.json')).write_text(json.dumps(manifest,indent=2),encoding='utf-8')
print('Archives reopened; every member hash verified.',flush=True)
