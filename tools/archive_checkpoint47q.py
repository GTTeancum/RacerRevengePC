"""Archive validated cache startup; never promote an unfinished race."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs';tag='RacerRevenge-Checkpoint47Q-DiagnosticCacheStartup'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
gate=js('evidence/47Q/build-gate.json');build=Path(gate['build_evidence'])
assert (root/'evidence/47Q/startup.exit').read_text(encoding='utf-8-sig').strip()=='0'
for p in ['build.exit','differential-0.exit','differential-1.exit']:assert (build/p).read_text(encoding='utf-8-sig').strip()=='0'
runs=js('evidence/47Q/startup-runs.json');assert len(runs)==2
for r in runs:
    assert js('evidence/47Q/'+r['mode']+'-startup-state.json')['saved_state_equivalent']
    assert sha(Path(r['run'])/'racer_native_probe.exe')==gate['binary_sha256']
provenance=js('build/iop47Q/provenance.json')
for entry in provenance['files']:
    assert sha(root/'build/iop47J2'/entry['path'])==entry['original_sha256']
    assert sha(root/'build/iop47Q'/entry['path'])==entry['candidate_sha256']
dependencies=[]
for label,digest in [('Source','164ea352bc32dcc18728e9026be147c8c9a68562ea409e709d4bd897ec6b00e8'),('Evidence','0b13db617b3dc5c8fe560d9e514bbfb1441cf12785130bdd61ca2885084d184a')]:
    p=out/f'RacerRevenge-Checkpoint47P-VerifiedRacesAndCpuAttribution-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/diagnostic-cache47q.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += [p for p in files('build/iop47Q') if p.suffix in {'.cpp','.h','.cmake','.txt','.json'}]
source += [out/(tag+'.md')]
evidence=files('evidence/47Q')+[p for p in build.rglob('*') if p.is_file()]
for r in runs:evidence += [p for p in Path(r['run']).rglob('*') if p.is_file()]
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',build_gate=gate,startups=runs,
    startup_state_verified=True,full_race_verified=False,throughput_gain_verified=False,active_race='evidence/launchEnv-race47Q-20260930-164842-029',active_race_excluded=True,
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
print('Every archived member reopened and hash verified; active races excluded.',flush=True)
