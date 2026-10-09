"""Audit the actual final binaries, source hashes and native collision captures."""
from pathlib import Path
import argparse,hashlib,json
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
    assert __debug__
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);ap.add_argument('--run',type=Path,action='append',required=True);ap.add_argument('--full',type=Path,required=True);a=ap.parse_args()
    build=a.build.resolve();out=ROOT/'evidence/native48G-parent';out.mkdir(exist_ok=True)
    builds=[];runs=[]
    for arch in ('x64','x86'):
        assert (build/(arch+'.exit')).read_text().strip()=='0'
        log=(build/(arch+'-tests.log')).read_text(encoding='utf-8-sig');assert log.count('Test Passed.')==13
        assert '398925 body scene checks passed' in log and '241921 checks passed; 5900 original-word calls' in log
        for r in js(build/(arch+'-source-hashes.json')):assert sha(Path(r['Path']))==r['Hash'].lower(),r['Path']
        for r in js(build/(arch+'-hashes.json')):assert sha(build/arch/Path(r['Path']).name)==r['Hash'].lower()
        collision=js(build/(arch+'-collisions.json'));assert collision['accepted']==38 and collision['checks']>=88052 and collision['failures']==0
        for r in collision['collisions']:assert sha(build/(arch+'-collision-fixtures')/Path(r['cache_fixture']).name)==r['rrcol_sha256']
        app=build/arch/'racer_native_pc.exe';mapping=(build/(arch+'-native.map')).read_text(errors='replace').lower()
        assert 'ps2runtime' not in mapping and 'dispatchguest' not in mapping and 'pcsx2' not in mapping
        builds.append(dict(architecture=arch,executable_sha256=sha(app),ctests=13,collision_checks=collision['checks'],orientation_checks=398925,terrain_checks=241921,
            source_hash_records=len(js(build/(arch+'-source-hashes.json')))))
    assert len(a.run)==2
    for run in a.run:
        run=run.resolve();c=js(run/'checks.json');r=js(run/'world.json')
        assert c['passed'] and len(c['collisions'])==38 and c['native_invocations']==7
        assert c['executable_sha256']==next(b['executable_sha256'] for b in builds if b['architecture']==c['architecture'])
        assert r['success'] and r['last_frame_completed_readback'] and r['native_collision_world_loaded']
        assert (r['collision_placed_instances'],r['collision_unique_assets'],r['collision_absent_original_members'])==(37,36,281)
        assert not any('ps2runtime' in m.lower() or 'pcsx2' in m.lower() for m in r['loaded_modules'])
        assert sha(run/'world.ppm')=='dcf6b6c4929180b54aef63ea192344dafe2394ef6654e9700c33d6d1d0489063'
        with Image.open(run/'world.ppm') as im:
            im.save(run/'world.png')
            with Image.open(run/'world.png') as png:assert im.mode==png.mode and im.size==png.size and im.tobytes()==png.tobytes()
        runs.append(dict(path=run.relative_to(ROOT).as_posix(),architecture=c['architecture'],executable_sha256=c['executable_sha256'],
            capture_sha256=sha(run/'world.ppm'),png_sha256=sha(run/'world.png'),collision_payload_bytes=r['collision_retained_payload_bytes']))
    full=js(a.full/'checks.json');assert full['passed']
    assert full['executable_sha256']==next(b['executable_sha256'] for b in builds if b['architecture']=='x64')
    audit=dict(passed=True,build=build.relative_to(ROOT).as_posix(),builds=builds,runs=runs,full_import=full,
        scope='Actual binaries/source hashes, CTests, converter cache and native world readback. Maps/modules checked; not an OS file-open trace.',
        native_races_verified=False,native_racing_fps_measured=False,xbox_execution_verified=False)
    (out/'audit.json').write_text(json.dumps(audit,indent=2)+'\n');print(json.dumps(audit))
if __name__=='__main__':main()
