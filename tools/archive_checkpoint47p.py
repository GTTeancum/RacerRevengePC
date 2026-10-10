"""Freeze completed registry/renderer races and process-local attribution."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs'
tag='RacerRevenge-Checkpoint47P-VerifiedRacesAndCpuAttribution'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
races=['evidence/memory-race47N-20260930-160118-731','evidence/gsProfile-race47O-20260930-161223-107']
runs=[]
for race in races:
    assert (root/race/'race.exit').read_text(encoding='utf-8-sig').strip()=='0'
    assert js(race+'/state.json')['saved_state_equivalent']
    run=Path(js(race+'/run.json')['run']);assert (run/'native.exit').read_text(encoding='utf-8-sig').strip()=='0';runs.append(run)
profile=js(races[1]+'/profile.json');assert profile['terminal_vsync_reconciled'] and profile['final_vsync_calls']==7000
for obs in ['threadCpu47P-registry','threadCpu47P-gsProfile','processSample47P-pilot']:assert (root/'evidence'/obs/'end.json').exists()
sample=js('evidence/processSample47P-pilot/report.json');assert sample['map_validation']['identical_virtual_section_layout']
dependencies=[]
for label,digest in [('Source','8e067cb7c686a88cd2f2bba26170f526f855775f940d43d72e13591d054dbcd4'),('Evidence','5fec2641850746b72f23f87f361b2af43bec262b2bb375434678b88ae11a50bb')]:
    p=out/f'RacerRevenge-Checkpoint47O-LeafRaceAndRendererProfile-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/performance-profile47p.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += [out/(tag+'.md')]
evidence=[]
for race in races:evidence += [p for p in (root/race).iterdir() if p.is_file()]
for run in runs:evidence += [p for p in run.rglob('*') if p.is_file()]
for d in ['threadCpu47P-registry','threadCpu47P-gsProfile','processSample47P-pilot','etw47P-pilot','map47P-20260930-163843-932']:
    evidence+=files('evidence/'+d)
evidence += [p for p in (root/'evidence').glob('*47P*.log') if p.is_file()]
evidence += [p for p in (root/'evidence').glob('*47P*.exit') if p.is_file()]
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',closed_runs=[str(p) for p in runs],
    full_registry_race_verified=True,full_race_renderer_profile_verified=True,profile=profile,map_validation=sample['map_validation'],xbox_fit_verified=False,xbox_execution_verified=False,
    observer_overlap='PSS 20 seconds and per-thread CPU observer overlap the renderer profile. No matched throughput claim.')
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
print('All archive members reopened and SHA-256 verified.',flush=True)
