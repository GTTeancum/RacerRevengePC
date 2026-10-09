"""Freeze completed control and bounded stderr candidate; active work excluded."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs';tag='RacerRevenge-Checkpoint47R-DiagnosticBufferAndDispatchBudget'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
gate=js('evidence/47R/build-gate.json');build=Path(gate['build_evidence']);race=root/'evidence/launchEnv-race47Q-20260930-164842-029'
for name in ['build.exit','test.exit']:assert (build/name).read_text(encoding='utf-8-sig').strip()=='0'
assert js(str((race/'state-0.json').relative_to(root)))['saved_state_equivalent']
run=root/'work/native-run47q-0-0-1-1-1-1-20260930-164842-716'
assert (run/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
for d in ['observer47Q-cpu-0','observer47Q-sample-0']:assert (root/'evidence'/d/'end.json').exists()
provenance=js('build/diagnostic47R/provenance.json');assert sha(root/provenance['original'])==provenance['original_sha256']
assert sha(root/'build/diagnostic47R/probe.cpp')==provenance['candidate_sha256']
dependencies=[]
for label,digest in [('Source','1a1f7df9e6a204d4d2eb198a2453d612f9a9a6da5691d21094f67b41a4772e64'),('Evidence','2233a3e91bbed60ef3fab22855a92a7d2e9afe5329eac4033174cce3c4b2ee7e')]:
    p=out/f'RacerRevenge-Checkpoint47Q-DiagnosticCacheStartup-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/diagnostic-lines47r.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source+=files('build/diagnostic47R')+[out/(tag+'.md')]
evidence=[p for p in build.rglob('*') if p.is_file()]+[root/'evidence/47R/build-gate.json',root/'evidence/dispatch-budget47R.json']
evidence += [p for p in run.rglob('*') if p.is_file()]
evidence += [race/name for name in ['queued.json','state-0.json','state-0.log','state-0.exit','timing-contamination.json']]
for d in ['observer47Q-cpu-0','observer47Q-sample-0']:
    evidence+=files('evidence/'+d)
    for suffix in ['.log','.exit']:
        p=root/'evidence'/(d+suffix)
        if p.exists():evidence.append(p)
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',build_gate=gate,
    closed_control_run=str(run),control_state_verified=True,control_timing_contaminated=True,buffer_unit_checks_passed=True,
    buffer_native_startup_verified=False,buffer_native_race_verified=False,throughput_gain_verified=False,active_runs_excluded=True,xbox_fit_verified=False,xbox_execution_verified=False)
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
print('Every member reopened and hash verified; unfinished runs excluded.',flush=True)
