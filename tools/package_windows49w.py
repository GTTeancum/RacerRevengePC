"""Create a recoverable 49W Windows overlay and evidence archive; never delete."""
import hashlib
import json
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT / 'work/handoff49w/sealed49w/RacerRevengePC'
BASE_HASH = 'cc9da601541bcc4e81b074edda18411ee2f28a83102093797c99b8294a83d61f'
OUT = ROOT / 'outputs/windows49w-20261007'
OUT.mkdir(exist_ok=True)

def digest(data):
    return hashlib.sha256(data).hexdigest()

def archive(name, paths, description):
    destination = OUT / name
    if destination.exists():
        raise RuntimeError(f'Refusing to overwrite sealed package: {destination}')
    entries = []
    with zipfile.ZipFile(destination, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        for path in sorted(set(paths)):
            data = path.read_bytes()
            relative = path.relative_to(ROOT).as_posix()
            old = BASE / relative
            entries.append(dict(path=relative, size=len(data), sha256=digest(data),
                                base_sha256=digest(old.read_bytes()) if old.is_file() else None))
            z.writestr(relative, data)
        manifest = dict(description=description, base_archive_sha256=BASE_HASH, entries=entries)
        z.writestr('WINDOWS49W-MANIFEST.json', json.dumps(manifest, indent=2))
    with zipfile.ZipFile(destination) as z:
        assert z.testzip() is None
        for entry in entries:
            data = z.read(entry['path'])
            assert len(data) == entry['size'] and digest(data) == entry['sha256']
    return dict(path=destination.relative_to(ROOT).as_posix(), size=destination.stat().st_size,
                sha256=digest(destination.read_bytes()), verified_members=len(entries)+1)

source = []
for folder in ('src', 'tests/native', 'cmake', 'tools', 'evidence/frameBridge49W'):
    for p in (ROOT / folder).rglob('*'):
        if not p.is_file() or p.suffix.lower() not in ('.h', '.hpp', '.cpp', '.cmake', '.py', '.ps1'):
            continue
        if '49y' in p.name.lower():
            continue  # separately packaged, not integrated into this baseline
        baseline = BASE / p.relative_to(ROOT)
        if not baseline.exists() or digest(p.read_bytes()) != digest(baseline.read_bytes()):
            source.append(p)
source.append(ROOT / 'outputs/RacerRevenge-Windows49W-20261007.md')
evidence = []
for folder in list((ROOT / 'evidence').glob('nativeFrame49W-Windows-*')) + [ROOT / 'work/windowsNumeric49W']:
    evidence.extend(p for p in folder.rglob('*') if p.is_file())
for arch in ('x64', 'x86'):
    evidence.append(ROOT / f'build/native-pc48K-{arch}/racer_native_zlib.lib')
for name in ('receipt49w.json', 'manifest-verification49w.json', 'receipt49x.json',
             'windows-string-includes.json', 'integration49w/plan.json',
             'integration49w/result.json', 'integration49w/previous-overwritten-files.zip'):
    evidence.append(ROOT / 'work/handoff49w' / name)
receipts = [archive('RacerRevenge-Windows49W-SourceOverlay.zip', source,
    'Apply complete replacement files over the authenticated 49W archive. No deletions. '
    'Requires that base archive; not a standalone source release. Excludes unintegrated49Y.'),
    archive('RacerRevenge-Windows49W-Evidence.zip', evidence,
    'Local Windows build/test logs including unsuccessful attempts, hashes, and restore preimages.')]
(OUT / 'archive-receipts.json').write_text(json.dumps(receipts, indent=2))
print(json.dumps(receipts, indent=2))
