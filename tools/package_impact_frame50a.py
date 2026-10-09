"""Seal a cumulative source overlay and evidence without replacing any archive."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT/'work/handoff49w/sealed49w/RacerRevengePC'
BASE_SHA = 'cc9da601541bcc4e81b074edda18411ee2f28a83102093797c99b8294a83d61f'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def files(folder):
    return {p for p in folder.rglob('*') if p.is_file() and '__pycache__' not in p.parts}


def archive(destination, paths, description):
    entries = []
    with zipfile.ZipFile(destination, 'x', zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        for path in sorted(paths):
            relative = path.relative_to(ROOT).as_posix()
            data = path.read_bytes()
            old = BASE/relative
            entries.append({'path': relative, 'bytes': len(data), 'sha256': sha(data),
                            'base_sha256': sha(old.read_bytes()) if old.is_file() else None})
            z.writestr(relative, data)
        z.writestr('IMPACT-FRAME50A-MANIFEST.json', json.dumps({
            'description': description, 'base_archive_sha256': BASE_SHA,
            'entries': entries}, indent=2))
    with zipfile.ZipFile(destination) as z:
        if z.testzip() is not None:
            raise RuntimeError('Archive CRC verification failed')
        for row in entries:
            data = z.read(row['path'])
            if len(data) != row['bytes'] or sha(data) != row['sha256']:
                raise RuntimeError(f'Archive member verification failed: {row["path"]}')
    return {'path': destination.relative_to(ROOT).as_posix(),
            'bytes': destination.stat().st_size, 'sha256': sha(destination.read_bytes()),
            'verified_members': len(entries)+1}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--validation', type=Path, required=True)
    parser.add_argument('--checkpoint', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    validation, checkpoint, output = ((ROOT/p).resolve() for p in
                                      (args.validation, args.checkpoint, args.output))
    if any(not p.is_relative_to(ROOT) for p in (validation, checkpoint, output)):
        raise RuntimeError('All package paths must stay in this repository')
    summary = json.loads((validation/'summary.json').read_text(encoding='utf-8-sig'))
    if summary.get('status') != 'passed' or sorted(r['architecture'] for r in summary['results']) != ['x64', 'x86']:
        raise RuntimeError('Both final native architectures must pass before sealing')
    source = set()
    input_sets = []
    for arch in ('x64', 'x86'):
        recorded = json.loads((validation/f'{arch}-source-hashes.json').read_text(encoding='utf-8-sig'))
        comparable = {}
        for row in recorded:
            path = Path(row['Path'])
            if not path.is_relative_to(ROOT) or sha(path.read_bytes()).upper() != row['Hash'].upper():
                raise RuntimeError(f'Validated input changed: {path}')
            relative = path.relative_to(ROOT)
            if relative.parts[0] == 'build':
                continue  # architecture-specific generated command metadata is in evidence
            comparable[relative.as_posix()] = row['Hash']
            baseline = BASE/relative
            if not baseline.is_file() or sha(path.read_bytes()) != sha(baseline.read_bytes()):
                source.add(path)
        input_sets.append(comparable)
    if input_sets[0] != input_sets[1]:
        raise RuntimeError('Final architectures did not validate identical project inputs')

    # Retain all earlier Windows corrections, even if a file is outside today's
    # compiled target. This overlay applies directly to the sealed49W base.
    prior = ROOT/'outputs/windows49w-20261007/RacerRevenge-Windows49W-SourceOverlay.zip'
    with zipfile.ZipFile(prior) as z:
        previous = json.loads(z.read('WINDOWS49W-MANIFEST.json'))
        source.update(ROOT/row['path'] for row in previous['entries'])
    for p in (ROOT/'tools/gameplay').iterdir():
        if p.is_file() and p.suffix in ('.py', '.ps1') and any(token in p.name for token in
            ('static_impact', 'impact_join49y', 'impact_consumers', 'impact_aftermath', 'impact_authoring49y')):
            if not any(token in p.name.lower() for token in ('50b', '50c')):
                source.add(p)
    source.update((Path(__file__).resolve(), checkpoint))

    evidence = files(validation)
    # Retain this integration's interrupted aggregate as well as the final run.
    for folder in (ROOT/'evidence').glob('nativeFrame49W-Windows-*'):
        if folder.name >= 'nativeFrame49W-Windows-20261007-090631-683':
            evidence.update(files(folder))
    for folder in ('work/local49Y', 'work/local49Z', 'work/local50A',
                   'work/impactAuthoring49Y', 'work/repairAuthoring50A', 'evidence/frameImpact49Y'):
        evidence.update(files(ROOT/folder))
    # Original numeric failures and successful Windows baseline remain intact.
    evidence.add(ROOT/'outputs/windows49w-20261007/RacerRevenge-Windows49W-Evidence.zip')
    for arch in ('x64', 'x86'):
        evidence.add(ROOT/f'build/native-pc48K-{arch}/racer_native_zlib.lib')
        evidence.add(ROOT/f'build/native-frame49W-{arch}/racer_native_vehicle_frame.lib')
    # Avoid mutable diagnostics subsequently entering this sealed package.
    evidence = {p for p in evidence if not any(token in p.name.lower() for token in ('50b','50c'))}
    output.mkdir(parents=True, exist_ok=False)
    receipts = [
        archive(output/'RacerRevenge-ImpactFrame50A-SourceOverlay.zip', source,
                'Cumulative complete-file overlay over authenticated49W. No deletions. '
                'Contains Windows portability fixes and joined impact/damage/repair. '
                'Requires base49W; no full-game or FPS claim.'),
        archive(output/'RacerRevenge-ImpactFrame50A-Evidence.zip', evidence,
                'Source recovery proofs, fixtures, reviews, successful and unsuccessful '
                'targeted runs, final native x64/x86 validation and earlier Windows evidence.'),
    ]
    (output/'archive-receipts.json').write_text(json.dumps(receipts, indent=2), encoding='utf-8')
    lines = ['# Impact frame50A recovery', '',
             'Extract the authenticated49W base into a separate directory, then apply the '
             'source overlay and evidence archive at their recorded relative paths. '
             'Verify archive SHA-256 and embedded member manifests before applying. '
             'The overlay is cumulative: the previous Windows source overlay is not required.', '',
             f'Base archive SHA-256: `{BASE_SHA}`.', '',
             '| Archive | Bytes | SHA-256 |', '|---|---:|---|']
    lines.extend(f'| `{r["path"]}` | {r["bytes"]} | `{r["sha256"]}` |' for r in receipts)
    lines.extend(['', 'Run `tools/Build-NativeFrame49W.ps1 -Architecture both` with the '
                  'documented MSVC/SDK/CMake/Ninja dependencies. Compiler runtimes are not '
                  'redistributed in these packages. No deletion, Git push or public release '
                  'is needed for recovery.', '',
                  'The full-game goal remains active. Effects/transitions, live contact '
                  'integration, complete races and representative FPS are still unfinished.'])
    (output/'RECOVERY.md').write_text('\n'.join(lines)+'\n', encoding='utf-8')
    print(json.dumps(receipts, indent=2))


if __name__ == '__main__':
    main()
