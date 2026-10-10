"""Create and reopen source/evidence/toolchain recovery archives for milestone 47A."""
from pathlib import Path
import datetime
import hashlib
import json
import subprocess
import zipfile

root = Path(__file__).resolve().parents[1]
out = root / 'outputs'
tag = 'RacerRevenge-Checkpoint47A-WindowsXboxSharedCore'
assert not list(out.glob(tag + '*.zip')), 'Checkpoint archives must be immutable'

def sha(path):
    with path.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()

sdk = root / 'toolchains/nxdk'
sdk_archives = root / 'evidence/47A/sdk-source'
sdk_archives.mkdir(exist_ok=True)
sdk_records = []
modules = ['', 'lib/pdclib', 'lib/libcxx', 'lib/net/lwip', 'lib/usb/libusbohci',
           'lib/zlib/zlib', 'lib/libpng/libpng', 'lib/libjpeg/libjpeg-turbo']
for module in modules:
    directory = sdk / module
    rev = subprocess.check_output(['git', '-C', str(directory), 'rev-parse', 'HEAD'], text=True).strip()
    tar = sdk_archives / ((module.replace('/', '-') or 'nxdk') + '.tar')
    assert not tar.exists()
    subprocess.run(['git', '-C', str(directory), 'archive', '--format=tar', '-o', str(tar), 'HEAD'], check=True)
    sdk_records.append({'module': module, 'revision': rev, 'archive': tar.relative_to(root).as_posix(), 'sha256': sha(tar)})
(sdk_archives / 'restore-map.json').write_text(json.dumps(sdk_records, indent=2), encoding='utf-8')

source = [root / p for p in ['.gitignore', '.gitattributes', 'README.md', 'CMakeLists.txt']]
for directory in ['src', 'tests', 'platform', 'tools', 'docs']:
    source.extend(p for p in (root / directory).rglob('*') if p.is_file() and
                  (p.suffix in {'.h', '.cpp', '.py', '.ps1', '.md', '.json'} or p.name == 'Makefile'))
source += list(sdk_archives.iterdir())
source.append(out / (tag + '.md'))

reference = root / 'work/native-run45z-20260930-112950-077'
assert (reference / 'native.exit').read_text().strip() == '0'
evidence = [p for p in (root / 'evidence').rglob('*') if p.is_file() and sdk_archives not in p.parents]
evidence += [p for p in reference.rglob('*') if p.is_file()]
evidence += [root / p for p in ['build/windows-x86/racer_core_check.exe',
    'build/windows-x86/racer_xbox_objects_check.exe', 'build/windows-x64/racer_core_check.exe',
    'build/xbox-core/default.xbe', 'platform/xbox/main.exe', 'platform/xbox/main.obj',
    'src/core/cycle_ledger.obj', 'tests/core_suite.obj']]
toolchain = [root / 'toolchains/msys2-base-x86_64.tar.xz']
toolchain += [p for p in (root / 'toolchains/msys64/var/cache/pacman/pkg').iterdir() if p.is_file()]

manifest = {'created_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'goal_status': 'active/incomplete', 'sdk': sdk_records,
    'core_check_count_per_build': 149366, 'core_check_digest': '947da74c',
    'verified_execution': ['Windows MSVC x86', 'Windows MSVC x64', 'nxdk core objects linked to Windows x86 runner'],
    'xbox_xbe_execution_verified': False, 'gameplay_integration': False,
    'legacy_reference': {'checkpoint': '46G plus its transitive recovery chain', 'native45z_terminal_exit': 0}}
for label, files in [('Source', source), ('Evidence', evidence), ('Toolchain', toolchain)]:
    archive = out / (tag + '-' + label + '.zip')
    records = []
    with zipfile.ZipFile(archive, 'x', zipfile.ZIP_DEFLATED, compresslevel=1) as z:
        for path in sorted(set(files)):
            assert path.is_file(), path
            relative = path.relative_to(root).as_posix()
            digest = sha(path)
            z.write(path, relative, compress_type=zipfile.ZIP_STORED if label == 'Toolchain' else zipfile.ZIP_DEFLATED)
            records.append({'path': relative, 'bytes': path.stat().st_size, 'sha256': digest})
    with zipfile.ZipFile(archive) as z:
        assert len(z.namelist()) == len(records)
        for record in records:
            with z.open(record['path']) as f:
                assert hashlib.file_digest(f, 'sha256').hexdigest() == record['sha256'], record['path']
    manifest[label] = {'path': archive.name, 'bytes': archive.stat().st_size, 'sha256': sha(archive), 'files': records}
    print(label, manifest[label]['sha256'], len(records), flush=True)
(out / (tag + '-Manifest.json')).write_text(json.dumps(manifest, indent=2), encoding='utf-8')
print('Checkpoint archives reopened and every member verified.', flush=True)
