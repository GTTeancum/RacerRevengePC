"""Freeze importer, scene recovery and native physics with reopened member hashes."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile

if not __debug__:
    raise RuntimeError('Archive verification requires normal Python execution')
ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'outputs'
TAG = 'RacerRevenge-Checkpoint48C-ImporterSceneAndPhysics'


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def js(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))


def main():
    assert not list(OUT.glob(TAG+'-*.zip')), 'Immutable archive already exists'
    assert not (OUT/(TAG+'-Manifest.json')).exists()
    dependencies = []
    for label, expected in [
        ('Source','ae470b6b00d1ee533c26fd4cc3bec42a08b9a879ad25f2f8c1be1ababcbae1a0'),
        ('Evidence','09bde2b03eb1ef1e1b221533de028b39ade3669fdd7efd12044f4fb97964c1b4')]:
        p=OUT/f'RacerRevenge-Checkpoint48B-NativeAssetsAndControl-{label}.zip'
        assert sha(p)==expected
        dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=expected))
    for name in ['evidence/assets48C/manifest.json','evidence/gameplay48C/checkpoint-manifest.json',
                 'evidence/importDecode48C/source-review.json','evidence/native48C-parent/dependency-provenance.json']:
        for record in js(ROOT/name)['files']:
            assert sha(ROOT/record['path'])==record['sha256'],record['path']
    build=ROOT/'evidence/nativePc48C-build-20261003-131135-609'
    for arch in ('x64','x86'):
        assert (build/(arch+'.exit')).read_text().strip()=='0'
        log=(build/(arch+'-tests.log')).read_text(encoding='utf-8-sig')
        for phrase in ['checks=414 failures=0','130349 checks passed','checks=3250 failures=0','299019 checks passed']:
            assert phrase in log,phrase
        for item in js(build/(arch+'-source-hashes.json')):
            assert sha(Path(item['Path']))==item['Hash'].lower(),item['Path']
        for item in js(build/(arch+'-hashes.json')):
            assert sha(build/arch/Path(item['Path']).name)==item['Hash'].lower()
    verification=js(ROOT/'evidence/native48C-parent/verification.json')
    for record in verification['final_importer_checks']:
        p=ROOT/record['checks']; assert sha(p)==record['sha256']
        assert js(p)['passed'] and js(p)['native_invocations']==14
    full=js(ROOT/verification['full_import_independent_verification'])
    assert full['passed'] and full['imported_members_byte_identical']==2184
    final_import=js(ROOT/'evidence/importFull48C-20261003/final-result.json')
    assert final_import['success'] and final_import['generation']==full['current_generation']
    assert final_import['converted']==0 and final_import['reused']==2184
    audit=js(ROOT/'evidence/native48C-parent/audit.json')
    assert len(audit['builds'])==2 and len(audit['runs'])==4
    for b in audit['builds']:
        assert sha(build/b['architecture']/'racer_native_pc.exe')==b['executable_sha256']
    for run in audit['runs']:
        d=Path(run['path']); assert d.resolve().is_relative_to((ROOT/'evidence').resolve())
        assert sha(d/'capture.ppm')==run['capture_sha256']
        for item in run['input_files']:
            assert sha(d/item['name'])==item['sha256']

    source=[ROOT/n for n in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
    allowed={'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
    for name in ['src','tests','platform','tools','docs','cmake']:
        source += [p for p in (ROOT/name).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile')]
    for name in ['third_party/native-zlib','third_party/native-json']:
        source += [p for p in (ROOT/name).rglob('*') if p.is_file()]
    source += [ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h', OUT/(TAG+'.md')]
    evidence=[]
    for directory in (ROOT/'evidence').iterdir():
        if directory.is_dir() and '48c' in directory.name.lower():
            evidence += [p for p in directory.rglob('*') if p.is_file()]
    evidence.append(OUT/'RacerRevenge-Checkpoint48B-NativeAssetsAndControl-Manifest.json')
    manifest=dict(client_checkpoint_date='2026-10-03',
        host_created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        dependencies=dependencies, scope='built-in partial native importer, expanded scene recovery, native body numerical core',
        goal_status='active', native_gameplay_complete=False, native_racing_fps_measured=False,
        xbox_execution_verified=False, builtin_importer_partial_geometry_verified=True,
        expanded_scene_grammar_in_builtin_importer=False, original_material_sky_fidelity_verified=False,
        final_build=build.relative_to(ROOT).as_posix(), audit=audit, verification=verification,
        full_import_independent_verification_summary={k:v for k,v in full.items() if k!='meshes'},
        blockers=['Expanded scene grammar must be integrated into the native converter.',
                  'Original textures/materials/sky, scene instances and animation need recovery.',
                  'Vehicle force producers, collision, scene writeback and live timestep/input remain missing.',
                  'No native full race, progression, gameplay performance or Xbox acceptance exists.'],
        all_members_reopened_sha256_verified=False)
    for label,paths in [('Source',source),('Evidence',evidence)]:
        archive=OUT/(TAG+'-'+label+'.zip'); records=[]
        with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
            for p in sorted(set(paths)):
                name=p.relative_to(ROOT).as_posix(); digest=sha(p)
                z.write(p,name); assert sha(p)==digest,'File changed during archive: '+name
                records.append(dict(path=name,bytes=p.stat().st_size,sha256=digest))
        with zipfile.ZipFile(archive) as z:
            assert len(z.namelist())==len(records)
            for record in records:
                with z.open(record['path']) as stream:
                    assert hashlib.file_digest(stream,'sha256').hexdigest()==record['sha256'],record['path']
        manifest[label]=dict(path=archive.name,bytes=archive.stat().st_size,sha256=sha(archive),files=records)
        print(label,manifest[label]['sha256'],len(records),flush=True)
    manifest['all_members_reopened_sha256_verified']=True
    manifest['host_verified_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
    (OUT/(TAG+'-Manifest.json')).write_text(json.dumps(manifest,indent=2)+'\n')
    print('Source and evidence archives reopened and verified; full goal remains active.',flush=True)


if __name__=='__main__':
    main()
