"""Freeze 48F native world cache, original placement and renderer evidence."""
from pathlib import Path
import datetime
import argparse
import hashlib
import json
import zipfile

if not __debug__:
    raise RuntimeError('Archive verification requires normal Python execution')
ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'outputs'
TAG = 'RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering'
BUILD = None


def sha(p):
    with p.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def js(p):
    return json.loads(p.read_text(encoding='utf-8-sig'))


def main():
    global BUILD
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);ap.add_argument('--run',type=Path,action='append',required=True);args=ap.parse_args()
    BUILD=args.build.resolve()

    assert not list(OUT.glob(TAG+'-*.zip')), 'Immutable checkpoint already exists'
    dependencies = []
    for label, digest in [('Source','7dd134b6aecf5de7bb98cf5cf3afc741b875184c4053d3db701f77fb92d4d92d'),
                          ('Evidence','b1fc6667061a2d74a9fd8f1ab03c86e4c91772a69f02a1fd77abed4b7ec94885')]:
        p = OUT/f'RacerRevenge-Checkpoint48E-NativeMaterialsWorldAndAero-{label}.zip'
        assert sha(p) == digest
        dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=digest))
    for arch in ('x64','x86'):
        assert (BUILD/(arch+'.exit')).read_text().strip() == '0'
        log = (BUILD/(arch+'-tests.log')).read_text(encoding='utf-8-sig')
        assert log.count('Test Passed.') == 10, 'Expected all ten native CTests'
        world_report=js(BUILD/(arch+'-worlds.json'))
        assert world_report['checks']==416524 and world_report['failures']==0 and world_report['accepted']==12
        for phrase in ['checks=414 failures=0','130349 checks passed','checks=3250 failures=0',
                       '299019 checks passed','checks=7384 failures=0','121260 scene cache checks passed','423693 checks passed',
                       '"checks":256233','354461 checks passed']:
            assert phrase in log, phrase
        for record in js(BUILD/(arch+'-source-hashes.json')):
            assert sha(Path(record['Path'])) == record['Hash'].lower(), record['Path']
        for record in js(BUILD/(arch+'-hashes.json')):
            assert sha(BUILD/arch/Path(record['Path']).name) == record['Hash'].lower()
    for run in args.run:
        check = js(run/'checks.json')
        assert check['passed'] and check['native_invocations'] >= 16 and check['counts']['converted_world'] == 1
        assert js(run/'world.json')['world_placed_mesh_sha256'] == '4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7'
    for f in js(ROOT/'evidence/importWorld48F/manifest.json')['files']:
        assert sha(ROOT/f['path'])==f['sha256'],f['path']
    binding=js(ROOT/'evidence/native48F-parent/full-world-binding-check.json')
    assert binding['passed'] and len(binding['runs'])==2
    audit = js(ROOT/'evidence/native48F-parent/audit.json')
    assert len(audit['builds']) == 2 and len(audit['runs']) == 2
    for r in binding['runs']:
        assert r['executable_sha256']==next(b['executable_sha256'] for b in audit['builds'] if b['architecture']==r['architecture'])
    full = js(ROOT/'evidence/importFull48F-20261003/result.json')
    assert full['success'] and full['converted'] + full['reused'] == 15527
    verification = js(ROOT/'evidence/importFull48F-20261003/world-verification.json')
    assert verification['passed'] and verification['accepted_worlds'] == 12
    assert verification['generation'] == full['generation']
    source = [ROOT/n for n in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
    allowed = {'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
    for name in ['src','tests','platform','tools','docs','cmake']:
        source.extend(p for p in (ROOT/name).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile'))
    for name in ['third_party/native-zlib','third_party/native-json']:
        source.extend(p for p in (ROOT/name).rglob('*') if p.is_file())
    source += [ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h', OUT/(TAG+'.md')]
    evidence = [OUT/'RacerRevenge-Checkpoint48E-NativeMaterialsWorldAndAero-Manifest.json']
    for directory in (ROOT/'evidence').iterdir():
        if directory.is_dir() and '48f' in directory.name.lower():
            evidence.extend(p for p in directory.rglob('*') if p.is_file())
    manifest = dict(client_checkpoint_date='2026-10-03',host_created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        dependencies=dependencies,goal_status='active',native_gameplay_complete=False,native_racing_fps_measured=False,
        xbox_execution_verified=False,scope='native SCB world conversion/cache, root replacement and textured static world rendering',
        final_build=BUILD.relative_to(ROOT).as_posix(),audit=audit,
        full_import_summary=full,independent_full_import_summary={k:v for k,v in verification.items() if k!='meshes'},
        
        blockers=['Original shaders, runtime visibility/animation/sky, MC duplicate parameters and unzoned END worlds remain incomplete.',
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
