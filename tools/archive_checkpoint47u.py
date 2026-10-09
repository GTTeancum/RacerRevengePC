"""Freeze completed combined pair and attribution startup; exclude active U/V."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs';tag='RacerRevenge-Checkpoint47U-VerifiedSchedulerGain'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
race='evidence/combined-race47T-20260930-173405-793'
assert (root/race/'race.exit').read_text(encoding='utf-8-sig').strip()=='0'
for mode in ['0','1']:
    state=js(f'{race}/state-{mode}.json');assert state['saved_state_equivalent']
    assert all(state['files'][n]['equal'] for n in ['mixed.s16le','final.ppm'])
assert (root/'evidence/47U/500-state.exit').read_text(encoding='utf-8-sig').strip()=='0'
gate=js('evidence/47U/build-gate.json');build=Path(gate['build_evidence'])
for n in ['build.exit','test.exit','count-check.exit']:assert (build/n).read_text(encoding='utf-8-sig').strip()=='0'
dependencies=[]
for label,digest in [('Source','edfb10483fb12b705894f70bc3b22b57b36029c54efeaaab8f527918c4f81c2e'),('Evidence','5ec98f565cf8740ec55252f5af1701008402f8f7c9221f9b5b9b1bb2cbd13656')]:
    p=out/f'RacerRevenge-Checkpoint47T-CombinedSchedulerStartup-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/combined-race47u.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += [p for p in files('build/env47U') if p.suffix!='.map']+[out/(tag+'.md')]
evidence=files(race)+list(p for p in build.rglob('*') if p.is_file())
for rec in js(race+'/runs.json'):evidence+=files(rec['run'])
for mode in ['0','1']:
    d='evidence/observer47T-cpu-'+mode;assert (root/d/'end.json').exists()
    evidence+=files(d)
    for suffix in ['.log','.exit']:
        p=root/(d+suffix)
        if p.exists():evidence.append(p)
startup=root/'work/native-run47u-1-1-1-0-1-1-1-1-20260930-175309-083'
evidence += [p for p in startup.rglob('*') if p.is_file()]
evidence += [root/'evidence/47U/build-gate.json']+list((root/'evidence/47U').glob('500-*'))
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,
    goal_status='active/incomplete',combined_race_saved_state_and_media_verified=True,
    combined_race_timing=js(race+'/timing.json'),attribution_startup_verified=True,
    attribution_full_race_verified=False,capture_off_verified=False,active_runs_excluded=True,
    xbox_fit_verified=False,xbox_execution_verified=False)
for label,paths in [('Source',source),('Evidence',evidence)]:
    archive=out/(tag+'-'+label+'.zip');records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            name=p.relative_to(root).as_posix();digest=sha(p);z.write(p,name);assert sha(p)==digest,p
            records.append(dict(path=name,bytes=p.stat().st_size,sha256=digest))
    with zipfile.ZipFile(archive) as z:
        assert len(z.namelist())==len(records)
        for rec in records:
            with z.open(rec['path']) as f:assert hashlib.file_digest(f,'sha256').hexdigest()==rec['sha256'],rec['path']
    manifest[label]=dict(path=archive.name,bytes=archive.stat().st_size,sha256=sha(archive),files=records)
    print(label,manifest[label]['sha256'],len(records),flush=True)
manifest['verified_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
(out/(tag+'-Manifest.json')).write_text(json.dumps(manifest,indent=2))
print('Every member reopened and hash verified; active native runs excluded.',flush=True)
