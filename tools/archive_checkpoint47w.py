"""Freeze capture-off full race and cache startup; exclude active cache races."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs';tag='RacerRevenge-Checkpoint47W-DiagnosticCacheStartup'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
v='evidence/captureOff47V-20260930-180114-129'
for p in [v+'/native.exit','evidence/47W/500-pair.exit']:assert (root/p).read_text(encoding='utf-8-sig').strip()=='0'
for p in [v+'/7000-state.json','evidence/47W/500-0-state.json','evidence/47W/500-1-state.json']:
    s=js(p);assert s['saved_state_equivalent'] and s['files']['mixed.s16le']['equal'] and s['files']['final.ppm']['equal']
gate=js('evidence/47W/build-gate.json');build=Path(gate['build_evidence'])
for n in ['build.exit','test-0.exit','test-1.exit']:assert (build/n).read_text(encoding='utf-8-sig').strip()=='0'
link=js('build/memory47W/link-check.json');assert len(link['objects'])==3 and all(x['original_archive_member_absent'] for x in link['objects'])
dependencies=[]
for label,digest in [('Source','d0873ec11f1eab48e60c1bd170cf09a6f8064365220875934f5905fb47e05bbf'),('Evidence','2401304b2985ec1d54852fadc8a66e3bdf427dc6148948394e02c0c73560a7bf')]:
    p=out/f'RacerRevenge-Checkpoint47V-EnvironmentCallerEvidence-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/memory-env47w.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += [p for p in files('build/memory47W') if p.suffix!='.map']+[out/(tag+'.md')]
evidence=files(v)+files('work/native-run47v-1-1-1-1-0-1-1-1-1-20260930-180133-410')
evidence += [p for p in build.rglob('*') if p.is_file()]
evidence += [root/'evidence/47W/build-gate.json',root/'evidence/47W/500-pair.exit']+list((root/'evidence/47W').glob('500-*-state.*'))+list((root/'evidence/47W').glob('500-*-run.log'))
startups=[r for r in js('evidence/47W/runs.json') if r['stop']==500];assert len(startups)==2
for r in startups:evidence+=files(r['run'])
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',
    capture_off_saved_state_and_media_verified=True,capture_off_timing=js(v+'/timing.json'),build_gate=gate,link_validation=link,startups=startups,
    cache_startup_verified=True,cache_race_verified=False,active_runs_excluded=True,xbox_execution_verified=False,xbox_memory_fit_verified=False)
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
