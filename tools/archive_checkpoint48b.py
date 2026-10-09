"""Freeze native viewer, bounded conversion and control recovery with verified ZIPs."""
from pathlib import Path
import datetime
import hashlib
import json
import re
import zipfile

if not __debug__:
    raise RuntimeError('Archive verification requires normal Python execution')

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'outputs'
TAG = 'RacerRevenge-Checkpoint48B-NativeAssetsAndControl'


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def read_json(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))


def files(directory):
    return [p for p in directory.rglob('*') if p.is_file()]


def main():
    assert not list(OUT.glob(TAG + '-*.zip')), 'Immutable archive already exists'
    assert not (OUT / (TAG + '-Manifest.json')).exists()
    dependencies = []
    for label, expected in [
        ('Source', 'ed990ac49bc5f4effba7ba4cd8faed2d022b0c5a18e39a0aa9a8406181177702'),
        ('Evidence', '2c6ea9d605ad98a68b4048f6f80b98e9f66385f3a37ea59d9751a8e3369c47f9')]:
        path = OUT / ('RacerRevenge-Checkpoint48A-NativePcPlan-' + label + '.zip')
        assert sha(path) == expected, path
        dependencies.append(dict(path=path.name, bytes=path.stat().st_size, sha256=expected))

    verification = read_json(ROOT / 'evidence/native48B-parent/verification.json')
    for item in verification['frozen_agent_manifests']:
        manifest = ROOT / item['manifest']
        assert sha(manifest) == item['sha256']
        records = read_json(manifest)['files']
        assert len(records) == item['files_verified']
        for record in records:
            assert sha(ROOT / record['path']) == record['sha256'], record['path']

    build = ROOT / 'evidence/nativePc48B-build-20261003-123513-121'
    audit = read_json(ROOT / 'evidence/native48B-parent/audit.json')
    assert all(audit[key] for key in ['source_allowlist_passed', 'link_map_and_import_name_audit_passed',
        'loaded_module_name_audit_passed', 'input_hashes_verified', 'gpu_readbacks_nonempty'])
    assert len(audit['builds']) == 2 and len(audit['runs']) == 7
    for arch in ('x64', 'x86'):
        log = (build / (arch + '.log')).read_text(encoding='utf-8-sig')
        assert 'checks=414 failures=0' in log and '130349 checks passed' in log
        for record in read_json(build / (arch + '-source-hashes.json')):
            assert sha(Path(record['Path'])) == record['Hash'].lower()
        for record in read_json(build / (arch + '-hashes.json')):
            assert sha(build / arch / Path(record['Path']).name) == record['Hash'].lower()
    for run in audit['runs']:
        directory = Path(run['path'])
        assert directory.resolve().is_relative_to((ROOT / 'evidence').resolve())
        assert sha(directory / 'capture.ppm') == run['capture_sha256']
        for record in run['input_files']:
            assert sha(directory / record['name']) == record['sha256']
        for record in read_json(directory / 'output-hashes.json'):
            assert sha(directory / Path(record['Path']).name) == record['Hash'].lower()

    source = [ROOT / n for n in ['README.md', 'CMakeLists.txt', '.gitignore', '.gitattributes']]
    allowed = {'.cpp', '.c', '.h', '.hpp', '.inl', '.py', '.ps1', '.cmake', '.md', '.json', '.toml', '.s', '.asm'}
    for directory in ['src', 'tests', 'platform', 'tools', 'docs', 'cmake']:
        source += [p for p in files(ROOT / directory) if p.suffix.lower() in allowed or p.name == 'Makefile']
    source.append(OUT / (TAG + '.md'))
    evidence = []
    for directory in sorted((ROOT / 'evidence').iterdir()):
        if directory.is_dir() and re.search('48b', directory.name, re.I):
            evidence += files(directory)
    evidence.append(OUT / 'RacerRevenge-Checkpoint48A-NativePcPlan-Manifest.json')

    manifest = dict(client_checkpoint_date='2026-10-03',
        host_created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        scope='native static geometry viewer, bounded original-data conversion, native control helper',
        dependencies=dependencies, native_asset_viewer_built_x64_and_x86=True,
        native_control_filter_tested_x64_and_x86=True, native_gameplay_accepted=False,
        native_racing_fps_measured=False, xbox_execution_verified=False,
        built_in_converter_required=True, built_in_converter_ui_implemented=False,
        texture_palette_and_material_fidelity_accepted=False,
        full_track_assembly_accepted=False, all_members_reopened_sha256_verified=False,
        final_build=build.relative_to(ROOT).as_posix(), audit=audit,
        workstream_manifest_verification=verification,
        blockers=['Scene placement/hierarchies and unsupported PSG variants need recovery.',
                  'PST palette/swizzle and original materials/sky remain unverified.',
                  'Full vehicle simulation, live timing, action mapping and collision are missing.',
                  'Built-in import service/UI remains to implement around validated decoders.',
                  'No native gameplay or Xbox performance measurement is available.'])
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
    print('Source and evidence archives reopened and verified.', flush=True)


if __name__ == '__main__':
    main()
