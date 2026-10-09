"""Freeze full attribution and capture-off startup, excluding active race/build."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs';tag='RacerRevenge-Checkpoint47V-EnvironmentCallerEvidence'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
assert (root/'evidence/47U/native.exit').read_text(encoding='utf-8-sig').strip()=='0'
v='evidence/captureOff47V-20260930-180114-129'
assert (root/v/'500-state.exit').read_text(encoding='utf-8-sig').strip()=='0'
for p in ['evidence/47U/7000-state.json',v+'/500-state.json']:
    s=js(p);assert s['saved_state_equivalent'] and s['files']['mixed.s16le']['equal'] and s['files']['final.ppm']['equal']
dependencies=[]
for label,digest in [('Source','8af54533dd0d3d620bc231f277c7a8678e0631541ef3565bb64ec0ea35b33598'),('Evidence','d840698d74600f3e96fc367563145c850ec15896315c11e578c31506567550ab')]:
    p=out/f'RacerRevenge-Checkpoint47U-VerifiedSchedulerGain-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/attribution47v.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source+=files('build/capture47V')+[out/(tag+'.md')]
evidence=files('evidence/47U')+files('work/native-run47u-1-1-1-0-1-1-1-1-20260930-175329-326')
evidence+=files('work/native-run47v-1-1-1-1-0-1-1-1-1-20260930-180114-394')+list((root/v).glob('500-*'))
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',
    attribution_full_saved_state_and_media_verified=True,total_calls=68347425,overflow=0,capture_off_startup_verified=True,
    capture_off_race_verified=False,memory_vif_vu_cache_built=False,active_runs_excluded=True,xbox_execution_verified=False)
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
