"""Freeze 48E native content/cache, original world placement and aerodynamic evidence."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

if not __debug__:
    raise RuntimeError('Archive verification requires normal Python execution')
ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'outputs'
TAG = 'RacerRevenge-Checkpoint48E-NativeMaterialsWorldAndAero'
BUILD = ROOT/'evidence/nativePc48E-build-20261003-141953-482'


def sha(p):
    with p.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def js(p):
    return json.loads(p.read_text(encoding='utf-8-sig'))


def main():
    assert not list(OUT.glob(TAG+'-*.zip')), 'Immutable checkpoint already exists'
    dependencies = []
    for label, digest in [('Source','efe4506bc1772ff5e673bc7125cc1455993668748fbb06d4f7c2a56002fb3edd'),
                          ('Evidence','1e4349392e4959ded5b2049f4253d92c2edac597061d1961edea518c8d0b51af')]:
        p = OUT/f'RacerRevenge-Checkpoint48D-NativeSceneCacheAndForces-{label}.zip'
        assert sha(p) == digest
        dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=digest))
    for arch in ('x64','x86'):
        assert (BUILD/(arch+'.exit')).read_text().strip() == '0'
        log = (BUILD/(arch+'-tests.log')).read_text(encoding='utf-8-sig')
        for phrase in ['checks=414 failures=0','130349 checks passed','checks=3250 failures=0',
                       '299019 checks passed','checks=7384 failures=0','121260 scene cache checks passed','423693 checks passed',
                       '"checks":256233','354461 checks passed']:
            assert phrase in log, phrase
        for record in js(BUILD/(arch+'-source-hashes.json')):
            assert sha(Path(record['Path'])) == record['Hash'].lower(), record['Path']
        for record in js(BUILD/(arch+'-hashes.json')):
            assert sha(BUILD/arch/Path(record['Path']).name) == record['Hash'].lower()
    for name in ['import48E-x64-2ugjf7ni','import48E-x86-ln7996qv']:
        check = js(ROOT/'evidence'/name/'checks.json')
        assert check['passed'] and check['native_invocations'] == 9 and check['counts']['converted_geometry'] == 241
    for record in js(ROOT/'evidence/gameplay48E/checkpoint-manifest.json')['files']:
        assert sha(ROOT/record['path']) == record['sha256'], record['path']
    for record in js(ROOT/'evidence/importTexture48E/manifest.json')['files']:
        assert sha(ROOT/record['path']) == record['sha256'], record['path']
    for record in js(ROOT/'evidence/worldScene48E/hashes.json')['files']:
        assert sha(ROOT/record['path']) == record['sha256'], record['path']
    audit = js(ROOT/'evidence/native48E-parent/audit.json')
    assert len(audit['builds']) == 2 and len(audit['runs']) == 2
    full = js(ROOT/'evidence/importFull48E-20261003/result.json')
    assert full['success'] and full['converted'] + full['reused'] == 15515
    verification = js(ROOT/'evidence/importFull48E-20261003/independent-verification.json')
    assert verification['passed'] and verification['imported_members_byte_identical'] == 5558
    assert verification['current_generation'] == full['generation']
    content = js(ROOT/'evidence/importFull48E-20261003/content-verification.json')
    assert content['passed'] and content['generation'] == full['generation']
    assert content['counts']['textures'] == 3850 and content['counts']['materials'] == 6107
    source = [ROOT/n for n in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
    allowed = {'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
    for name in ['src','tests','platform','tools','docs','cmake']:
        source.extend(p for p in (ROOT/name).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile'))
    for name in ['third_party/native-zlib','third_party/native-json']:
        source.extend(p for p in (ROOT/name).rglob('*') if p.is_file())
    source += [ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h', OUT/(TAG+'.md')]
    evidence = [OUT/'RacerRevenge-Checkpoint48D-NativeSceneCacheAndForces-Manifest.json']
    for directory in (ROOT/'evidence').iterdir():
        if directory.is_dir() and '48e' in directory.name.lower():
            evidence.extend(p for p in directory.rglob('*') if p.is_file())
    manifest = dict(client_checkpoint_date='2026-10-03',host_created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        dependencies=dependencies,goal_status='active',native_gameplay_complete=False,native_racing_fps_measured=False,
        xbox_execution_verified=False,scope='native texture/material cache and rendering, original world placement, recovered aerodynamic forces',
        final_build=BUILD.relative_to(ROOT).as_posix(),audit=audit,
        full_import_summary=full,independent_full_import_summary={k:v for k,v in verification.items() if k!='meshes'},
        native_content_verification={k:v for k,v in content.items() if k!='members'},
        blockers=['Original material shaders, native world cache assembly, runtime visibility/animation/sky remain incomplete.',
                  'Repulsor/collision, scene synchronization, cached speed, live timestep/action mapping remain incomplete.',
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
