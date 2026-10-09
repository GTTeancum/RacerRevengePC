"""Native compact collision runtime, converter migration, reuse, corruption and unchanged visuals."""
from pathlib import Path
import argparse,hashlib,json,shutil,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
assert __debug__, 'Verification requires assertions'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--architecture',choices=['x64','x86'],required=True);args=ap.parse_args()
    exe=ROOT/f'build/native-pc48I-{args.architecture}/racer_native_pc.exe'
    out=Path(tempfile.mkdtemp(prefix=f'import48I-{args.architecture}-',dir=ROOT/'evidence'))
    source=out/'original-fixture';cache=out/'native-cache'
    for name in ['SLUS_202.68','TRACKS/TA.RES']:
        target=source/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(ROOT/'work/native/project/disc'/name,target)
    originals={p.relative_to(source).as_posix():sha(p) for p in source.rglob('*') if p.is_file()}
    shutil.copytree(ROOT/'evidence/import48F-x64-jr57ewco/native-cache',cache)
    old_current=(cache/'current.json').read_bytes();old_objects={p.name:sha(p) for p in (cache/'objects').iterdir()}
    runs=[]
    def run(label,arguments,expected=0):
        command=[str(exe),*map(str,arguments)];p=subprocess.run(command,capture_output=True,encoding='utf-8',timeout=180)
        (out/(label+'.stdout')).write_text(p.stdout);(out/(label+'.stderr')).write_text(p.stderr)
        runs.append(dict(label=label,command=command,exit=p.returncode))
        assert p.returncode==expected,(label,p.returncode,p.stdout,p.stderr)
        return json.loads(p.stdout) if p.stdout.startswith('{') else None
    base=['--import','--source',source,'--cache',cache]
    cancelled=run('01-cancel-migration',base+['--cancel-after','3'],130)
    assert cancelled['cancelled'] and (cache/'current.json').read_bytes()==old_current
    imported=run('02-resume-migration',base);assert imported['success'] and imported['reused']>=228
    manifest=js(Path(imported['manifest_path']));current=(cache/'current.json').read_bytes()
    assert manifest['converter']=='rr-native-content-48G-v1'
    collisions=[m for m in manifest['members'] if m['status']=='converted_collision'];assert len(collisions)==38
    assert imported['converted']+cancelled['converted']==len(set(m['source_sha256'] for m in collisions))==37
    assert all(m['container']=='TRACKS/TA.RES' for m in collisions)
    for name,digest in old_objects.items():assert sha(cache/'objects'/name)==digest
    collision_hashes={m['member']:dict(source_sha256=m['source_sha256'],native_sha256=sha(cache/'objects'/(m['native']['key']+'.rrcol')),
        vertices=m['native']['vertices'],nodes=m['native']['nodes'],faces=m['native']['faces']) for m in collisions}
    checker=js(ROOT/f'build/native-pc48I-{args.architecture}/collision-report48g.json')
    expected={r['member']:r for r in checker['collisions']};assert len(expected)==38 and checker['status']=='PASS'
    for member,r in collision_hashes.items():
        assert r['source_sha256']==expected[member]['source_sha256'] and r['native_sha256']==expected[member]['rrcol_sha256']
        assert all(r[k]==expected[member][k] for k in ('vertices','nodes','faces'))
    reused=run('03-reuse',base);assert reused['converted']==0 and (cache/'current.json').read_bytes()==current
    victim=next(m['native'] for m in collisions if m['member'].replace('\\','/').lower()=='geometry/zone9_col_floor.col')
    path=cache/'objects'/(victim['key']+'.rrcol');original=path.read_bytes()
    bad=bytearray(original);bad[-1]^=1;path.write_bytes(bad)
    world_args=['--world-collision','--cache',cache,'--container','TRACKS/TA.RES','--member',r'tracks\ta.scb']
    run('04-reject-corrupt-collision',world_args,1)
    repaired=run('04-repair-collision-bytes',base);assert repaired['converted']==1 and path.read_bytes()==original
    meta=cache/'objects'/(victim['key']+'.json');original_meta=meta.read_bytes();bad=js(meta);bad['faces']+=1;meta.write_text(json.dumps(bad))
    repaired=run('05-repair-collision-metadata',base);assert repaired['converted']==1 and meta.read_bytes()==original_meta
    assert (cache/'current.json').read_bytes()==current
    # Migration adds collision objects; existing original visual world is identical.
    run('06-native-world',world_args+['--frames','3','--capture',out/'world.ppm','--report',out/'world.json'])
    report=js(out/'world.json');assert report['success'] and report['last_frame_completed_readback']
    assert report['native_collision_world_loaded'] and report['collision_placed_instances']==37 and report['collision_unique_assets']==36
    assert report['collision_absent_original_members']==281
    assert report['collision_runtime_format']=='native48I compact records'
    assert report['collision_placement_arithmetic']=='original-order binary32'
    assert report['collision_retained_payload_bytes']<1000000
    assert report['world_placed_mesh_sha256']=='4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7'
    assert sha(out/'world.ppm')=='dcf6b6c4929180b54aef63ea192344dafe2394ef6654e9700c33d6d1d0489063'
    assert originals=={p.relative_to(source).as_posix():sha(p) for p in source.rglob('*') if p.is_file()}
    (out/'checks.json').write_text(json.dumps(dict(passed=True,architecture=args.architecture,executable_sha256=sha(exe),
        native_invocations=len(runs),collisions=collision_hashes,old_objects_unchanged=len(old_objects),generation=imported['generation'],
        originals=originals,world_capture_sha256=sha(out/'world.ppm'),runs=runs),indent=2)+'\n')
    print(json.dumps(dict(output=str(out),passed=True,collision_count=len(collisions),native_invocations=len(runs))))
if __name__=='__main__':main()

