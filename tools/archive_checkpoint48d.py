"""Freeze 48D native scene/cache, texture recovery and vehicle force evidence."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

if not __debug__:
    raise RuntimeError('Archive verification requires normal Python execution')
ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'outputs'
TAG = 'RacerRevenge-Checkpoint48D-NativeSceneCacheAndForces'
BUILD = ROOT/'evidence/nativePc48D-build-20261003-134330-366'


def sha(p):
    with p.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def js(p):
    return json.loads(p.read_text(encoding='utf-8-sig'))


def main():
    assert not list(OUT.glob(TAG+'-*.zip')), 'Immutable checkpoint already exists'
    dependencies = []
    for label, digest in [('Source','e620cec2ab68d428e1fff28305d856acdeab5c7974dfe3dbbc2f643e08140398'),
                          ('Evidence','375e40ae8a10f6308aa5d0588cd9c8ea393be5ad0cac1ff2eb64c712f0eb3073')]:
        p = OUT/f'RacerRevenge-Checkpoint48C-ImporterSceneAndPhysics-{label}.zip'
        assert sha(p) == digest
        dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=digest))
    for arch in ('x64','x86'):
        assert (BUILD/(arch+'.exit')).read_text().strip() == '0'
        log = (BUILD/(arch+'-tests.log')).read_text(encoding='utf-8-sig')
        for phrase in ['checks=414 failures=0','130349 checks passed','checks=3250 failures=0',
                       '299019 checks passed','checks=7384 failures=0','121260 scene cache checks passed','423693 checks passed']:
            assert phrase in log, phrase
        for record in js(BUILD/(arch+'-source-hashes.json')):
            assert sha(Path(record['Path'])) == record['Hash'].lower(), record['Path']
        for record in js(BUILD/(arch+'-hashes.json')):
            assert sha(BUILD/arch/Path(record['Path']).name) == record['Hash'].lower()
    for name in ['import48D-x64-jjjc79ng','import48D-x86-1tk5i74g']:
        check = js(ROOT/'evidence'/name/'checks.json')
        assert check['passed'] and check['native_invocations'] == 15 and check['original_meshes_byte_identical'] == 228
    for record in js(ROOT/'evidence/gameplay48D/checkpoint-manifest.json')['files']:
        assert sha(ROOT/record['path']) == record['sha256'], record['path']
    for record in js(ROOT/'evidence/textures48D/manifest.json')['files']:
        assert sha(ROOT/record['path']) == record['sha256'], record['path']
    audit = js(ROOT/'evidence/native48D-parent/audit.json')
    assert len(audit['builds']) == 2 and len(audit['runs']) == 4
    full = js(ROOT/'evidence/importFull48D-20261003/result.json')
    assert full['success'] and full['converted'] + full['reused'] == 5558
    verification = js(ROOT/'evidence/importFull48D-20261003/independent-verification.json')
    assert verification['passed'] and verification['imported_members_byte_identical'] == 5558
    assert verification['current_generation'] == full['generation']
    source = [ROOT/n for n in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
    allowed = {'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
    for name in ['src','tests','platform','tools','docs','cmake']:
        source.extend(p for p in (ROOT/name).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile'))
    for name in ['third_party/native-zlib','third_party/native-json']:
        source.extend(p for p in (ROOT/name).rglob('*') if p.is_file())
    source += [ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h', OUT/(TAG+'.md')]
    evidence = [OUT/'RacerRevenge-Checkpoint48C-ImporterSceneAndPhysics-Manifest.json']
    for directory in (ROOT/'evidence').iterdir():
        if directory.is_dir() and '48d' in directory.name.lower():
            evidence.extend(p for p in directory.rglob('*') if p.is_file())
    manifest = dict(client_checkpoint_date='2026-10-03',host_created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        dependencies=dependencies,goal_status='active',native_gameplay_complete=False,native_racing_fps_measured=False,
        xbox_execution_verified=False,scope='native scene converter/cache/render input, recovered textures and partial vehicle forces',
        final_build=BUILD.relative_to(ROOT).as_posix(),audit=audit,
        full_import_summary=full,independent_full_import_summary={k:v for k,v in verification.items() if k!='meshes'},
        blockers=['Original material shaders, texture conversion in app, runtime scene placement/animation/sky remain incomplete.',
                  'Aerodynamics, repulsor/collision, scene synchronization, cached speed, live timestep/action mapping remain incomplete.',
                  'No complete native race/progression/audio/gameplay benchmark or Xbox execution acceptance.'],
        all_members_reopened_sha256_verified=False)
    for label, files in [('Source',source),('Evidence',evidence)]:
        archive = OUT/(TAG+'-'+label+'.zip'); records = []
        with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
            for p in sorted(set(files)):
                name = p.relative_to(ROOT).as_posix(); digest = sha(p)
                z.write(p,name); assert sha(p) == digest, 'Changed during archive: '+name
                records.append(dict(path=name,bytes=p.stat().st_size,sha256=digest))
        with zipfile.ZipFile(archive) as z:
            assert len(z.namelist()) == len(records)
            for record in records:
                with z.open(record['path']) as stream:
                    assert hashlib.file_digest(stream,'sha256').hexdigest() == record['sha256'], record['path']
        manifest[label] = dict(path=archive.name,bytes=archive.stat().st_size,sha256=sha(archive),files=records)
        print(label,manifest[label]['sha256'],len(records),flush=True)
    manifest['all_members_reopened_sha256_verified'] = True
    manifest['host_verified_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (OUT/(TAG+'-Manifest.json')).write_text(json.dumps(manifest,indent=2)+'\n')
    print('Both archives reopened and verified; full goal remains active.',flush=True)


if __name__ == '__main__':
    main()
