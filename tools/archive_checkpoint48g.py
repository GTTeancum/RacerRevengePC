"""Freeze native collision and terrain progress, with reopened member hashes."""
from pathlib import Path
import argparse,datetime,hashlib,json,zipfile
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs'
TAG='RacerRevenge-Checkpoint48G-NativeCollisionAndTerrain'
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
    assert __debug__
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);a=ap.parse_args();build=a.build.resolve()
    assert not list(OUT.glob(TAG+'-*.zip')),'Immutable checkpoint already exists'
    audit=js(ROOT/'evidence/native48G-parent/audit.json');assert audit['passed'] and audit['build']==build.relative_to(ROOT).as_posix()
    dependencies=[]
    for label,digest in [('Source','fe69a5b5064ee9bb066445d79c5ba5ad7cf9cdfcc2dfea6402e87311f3d8e015'),('Evidence','60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c')]:
        p=OUT/f'RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-{label}.zip';assert sha(p)==digest
        dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=digest))
    for arch in ('x64','x86'):
        assert (build/(arch+'.exit')).read_text().strip()=='0'
        assert (build/(arch+'-tests.log')).read_text(encoding='utf-8-sig').count('Test Passed.')==13
        for r in js(build/(arch+'-source-hashes.json')):assert sha(Path(r['Path']))==r['Hash'].lower(),r['Path']
        for r in js(build/(arch+'-hashes.json')):assert sha(build/arch/Path(r['Path']).name)==r['Hash'].lower()
    for relative in ['evidence/gameplay48G/checkpoint-manifest.json','evidence/importCollision48G/manifest.json']:
        for r in js(ROOT/relative)['files']:assert sha(ROOT/r['path'])==r['sha256'],r['path']
    source=[ROOT/p for p in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
    allowed={'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
    for d in ['src','tests','platform','tools','docs','cmake']:
        source.extend(p for p in (ROOT/d).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile'))
    for d in ['third_party/native-zlib','third_party/native-json']:source.extend(p for p in (ROOT/d).rglob('*') if p.is_file())
    source.extend([ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h',OUT/(TAG+'.md'),OUT/'RacerRevenge-Checkpoint48G-InProgress.md'])
    evidence=[OUT/'RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Manifest.json']
    for d in (ROOT/'evidence').iterdir():
        if d.is_dir() and '48g' in d.name.lower():evidence.extend(p for p in d.rglob('*') if p.is_file())
    manifest=dict(client_checkpoint_date='2026-10-03',created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        dependencies=dependencies,goal_status='active',native_gameplay_complete=False,native_racing_fps_measured=False,xbox_execution_verified=False,
        scope='Original collision conversion/native world retention, terrain response and orientation helpers',audit=audit,
        blockers=['Original collision query traversal/filtering and spring/impulse/friction response remain incomplete.',
                  'Full body scene wrapper, second body, live timestep/input scheduling and a connected vehicle remain incomplete.',
                  'Original render programs/sky/visibility, AI/camera/audio/progression, complete native races and60FPS benchmark remain incomplete.'],
        all_members_reopened_sha256_verified=False)
    for label,files in [('Source',source),('Evidence',evidence)]:
        p=OUT/(TAG+'-'+label+'.zip');records=[]
        with zipfile.ZipFile(p,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
            for item in sorted(set(files)):
                name=item.relative_to(ROOT).as_posix();digest=sha(item);z.write(item,name);assert sha(item)==digest
                records.append(dict(path=name,bytes=item.stat().st_size,sha256=digest))
        with zipfile.ZipFile(p) as z:
            assert len(z.namelist())==len(records)
            for r in records:
                with z.open(r['path']) as stream:assert hashlib.file_digest(stream,'sha256').hexdigest()==r['sha256'],r['path']
        manifest[label]=dict(path=p.name,bytes=p.stat().st_size,sha256=sha(p),files=records)
        print(label,manifest[label]['sha256'],len(records),flush=True)
    manifest['all_members_reopened_sha256_verified']=True
    (OUT/(TAG+'-Manifest.json')).write_text(json.dumps(manifest,indent=2)+'\n')
    print('All archive members reopened and verified. Full native game goal remains active.',flush=True)
if __name__=='__main__':main()
