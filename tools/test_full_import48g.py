"""Migrate the complete verified 48F cache; preserve every earlier object byte."""
from pathlib import Path
import argparse,hashlib,json,shutil,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--resume-dir',type=Path);a=ap.parse_args()
    out=a.resume_dir.resolve() if a.resume_dir else Path(tempfile.mkdtemp(prefix='importFull48G-',dir=ROOT/'evidence'));cache=out/'native-cache'
    previous=ROOT/'evidence/importFull48F-20261003/native-cache'
    if a.resume_dir:
        assert out.parent==ROOT/'evidence' and out.name.startswith('importFull48G-') and (cache/'current.json').is_file()
        prior_manifest=js(ROOT/'outputs/RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Manifest.json')
        prefix='evidence/importFull48F-20261003/native-cache/objects/'
        objects={Path(r['path']).name:r['sha256'] for r in prior_manifest['Evidence']['files'] if r['path'].startswith(prefix)}
        assert len(objects)>20000
        attempt=1
        while (out/f'attempt-{attempt:02d}').exists():attempt+=1
        saved=out/f'attempt-{attempt:02d}';saved.mkdir()
        for name in ['launch.json','stdout.log','stderr.log','exit.txt','result.json','checks.json']:
            if (out/name).exists():shutil.copyfile(out/name,saved/name)
    else:
        objects={p.name:sha(p) for p in (previous/'objects').iterdir()}
        shutil.copytree(previous,cache)
    exe=ROOT/'build/native-pc48G-x64/racer_native_pc.exe'
    args=[str(exe),'--import','--source',str(ROOT/'work/native/project/disc'),'--cache',str(cache)]
    (out/'launch.json').write_text(json.dumps(dict(command=args,executable_sha256=sha(exe),old_object_count=len(objects)),indent=2)+'\n')
    p=subprocess.run(args,capture_output=True,text=True,timeout=600)
    (out/'stdout.log').write_text(p.stdout);(out/'stderr.log').write_text(p.stderr);(out/'exit.txt').write_text(str(p.returncode)+'\n')
    result=json.loads(p.stdout);(out/'result.json').write_text(json.dumps(result,indent=2)+'\n')
    unchanged=all(sha(cache/'objects'/name)==digest for name,digest in objects.items())
    assert unchanged,'Existing content changed during migration'
    assert p.returncode==0 and result['success'],(str(out),p.returncode,result)
    manifest=js(Path(result['manifest_path']));assert manifest['converter']=='rr-native-content-48G-v1'
    from collections import Counter
    counts=dict(Counter(m['status'] for m in manifest['members']))
    collisions=[m for m in manifest['members'] if m['status']=='converted_collision']
    assert len(collisions)>=38 and result['containers']==103 and result['reused']>=15527
    for m in collisions:
        native=m['native'];assert sha(cache/'objects'/(native['key']+'.rrcol'))==native['collision_sha256']
    (out/'checks.json').write_text(json.dumps(dict(passed=True,counts=counts,old_objects_unchanged=len(objects),
        collision_member_count=len(collisions),collision_containers=sorted(set(m['container'] for m in collisions)),
        generation=result['generation'],executable_sha256=sha(exe)),indent=2)+'\n')
    print(json.dumps(dict(output=str(out),passed=True,counts=counts)),flush=True)
if __name__=='__main__':main()
