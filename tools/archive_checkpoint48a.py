"""Freeze the PC-native plan, original-data findings and completed 47W evidence."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'outputs'
TAG = 'RacerRevenge-Checkpoint48A-NativePcPlan'


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def read_json(path):
    return json.loads((ROOT / path).read_text(encoding='utf-8-sig'))


def files(path):
    return [p for p in (ROOT / path).rglob('*') if p.is_file()]


def main():
    assert not list(OUT.glob(TAG + '-*.zip')), 'Immutable archive already exists'
    assert not (OUT / (TAG + '-Manifest.json')).exists()
    dependencies = []
    for name, expected in [
        ('RacerRevenge-Checkpoint47W-DiagnosticCacheStartup-Source.zip',
         '77a3b36a2c101728c7665fa8a60227b4a1b6508e9e3c8f14e9cff4580baaeb62'),
        ('RacerRevenge-Checkpoint47W-DiagnosticCacheStartup-Evidence.zip',
         '2ed33081ba95d62307bb779d976e3dbf0203a9d1563fee2ca5904b685e5d652b'),
        ('RacerRevenge-CarsTools-KnowledgeIntake-20261003.zip',
         'cae3f279f8ccb662694ca5025700ebd28132590b64b53e5e01011da09b0afa5b')]:
        path = OUT / name
        assert sha(path) == expected, name
        dependencies.append(dict(path=name, bytes=path.stat().st_size, sha256=expected))
    evidence = files('evidence/native-plan48A')
    verification = []
    for mode in (0, 1):
        report = read_json(f'evidence/47W/7000-{mode}-state.json')
        assert report['saved_state_equivalent']
        run = Path(report['candidate']['path'])
        assert run.resolve().is_relative_to((ROOT / 'work').resolve())
        assert (run / 'native.exit').read_text(encoding='utf-8-sig').strip() == '0'
        checked = []
        for name, item in report['files'].items():
            assert item['equal'], name
            assert sha(run / name) == item['candidate_sha256'], name
            checked.append(dict(path=name, sha256=item['candidate_sha256']))
        binary_hash = sha(run / 'racer_native_probe.exe')
        assert binary_hash == 'f6da2e716339a2e3a7bfb2563f389c6efa9b9c97c12d803da966ef84c4eb6724'
        verification.append(dict(mode=mode, run=run.relative_to(ROOT).as_posix(),
                                 checked_files=checked, executable_sha256=binary_hash))
        evidence += [p for p in run.rglob('*') if p.is_file()]
        observer = ROOT / f'evidence/observer47W-cpu-{mode}'
        assert json.loads((observer / 'end.json').read_text())['process_exit'] == 0
        evidence += [p for p in observer.rglob('*') if p.is_file()]
        evidence += [ROOT / f'evidence/observer47W-cpu-{mode}.{ext}' for ext in ['exit', 'log']]
    evidence += [p for p in (ROOT / 'evidence/47W').iterdir() if p.is_file()]
    for name in ['RacerRevenge-Checkpoint47W-DiagnosticCacheStartup-Manifest.json',
                 'RacerRevenge-CarsTools-KnowledgeIntake-20261003-Manifest.json']:
        evidence.append(OUT / name)
    assessment = read_json('evidence/native-plan48A/assessment.json')
    for anchor in assessment['source_anchors']:
        path = ROOT / anchor['path']
        assert sha(path) == anchor['sha256']
        evidence.append(path)
    decoder = ROOT / assessment['decoder']
    assert sha(decoder) == assessment['decoder_sha256']
    evidence.append(decoder)
    source = [ROOT / n for n in ['README.md', 'CMakeLists.txt', '.gitignore', '.gitattributes']]
    allowed = {'.cpp', '.c', '.h', '.hpp', '.inl', '.py', '.ps1', '.cmake', '.md', '.json', '.toml', '.s', '.asm'}
    for directory in ['src', 'tests', 'platform', 'tools', 'docs']:
        source += [p for p in files(directory) if p.suffix.lower() in allowed or p.name == 'Makefile']
    source.append(OUT / (TAG + '.md'))
    timing = read_json('evidence/47W/timing.json')
    assert timing['candidate']['latest']['vsync'] == 6877
    assert 'window' not in timing['candidate']
    manifest = dict(client_checkpoint_date='2026-10-03',
        host_created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        scope='native PC planning, static/read-only assessment, and completed reference evidence preservation',
        goal_service_status_observed='usageLimited', dependencies=dependencies,
        native_game_built=False, native_gameplay_accepted=False, native_fps_measured=False,
        xbox_execution_verified=False, reference47w_saved_state_and_media_verified=True,
        reference47w_fixed_window_speed_ratio_accepted=False,
        timing_blocker='Candidate last capture VBlank 6877 does not bracket requested endpoint 6900.',
        reference_files_rehashed=verification, all_members_reopened_sha256_verified=False)
    for label, paths in [('Source', source), ('Evidence', evidence)]:
        archive = OUT / (TAG + '-' + label + '.zip')
        records = []
        with zipfile.ZipFile(archive, 'x', zipfile.ZIP_DEFLATED, compresslevel=1) as z:
            for path in sorted(set(paths)):
                name = path.relative_to(ROOT).as_posix()
                digest = sha(path)
                z.write(path, name)
                assert sha(path) == digest, 'File changed during archive: ' + name
                records.append(dict(path=name, bytes=path.stat().st_size, sha256=digest))
        with zipfile.ZipFile(archive) as z:
            assert len(z.namelist()) == len(records)
            for record in records:
                with z.open(record['path']) as stream:
                    assert hashlib.file_digest(stream, 'sha256').hexdigest() == record['sha256'], record['path']
        manifest[label] = dict(path=archive.name, bytes=archive.stat().st_size,
                               sha256=sha(archive), files=records)
        print(label, manifest[label]['sha256'], len(records), flush=True)
    manifest['all_members_reopened_sha256_verified'] = True
    manifest['host_verified_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (OUT / (TAG + '-Manifest.json')).write_text(json.dumps(manifest, indent=2) + '\n')
    print('Source and evidence archives verified; no game build or new performance claim.', flush=True)


if __name__ == '__main__':
    main()
