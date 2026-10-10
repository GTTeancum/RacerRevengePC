"""Archive terminal Q/R/S/S2/T evidence; exclude the active T race pair."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs'
tag='RacerRevenge-Checkpoint47T-CombinedSchedulerStartup'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
for v in ['47R','47S2','47T']:
    assert (root/f'evidence/{v}/startup.exit').read_text(encoding='utf-8-sig').strip()=='0'
for d in ['launchEnv-race47Q-20260930-164842-029','launchEnv-repeat47Q-20260930-171243-658']:
    assert (root/f'evidence/{d}/race.exit').read_text(encoding='utf-8-sig').strip()=='0'
dependencies=[]
for label,digest in [('Source','85eee99b7a3050d7d51d194b5bd106c6c67d0f419c393b6cb8333fbec4410a28'),('Evidence','9678a513c1368c93b0eca654fbd7402ddadd45ca23b0069b4208269a88a33764')]:
    p=out/f'RacerRevenge-Checkpoint47R-DiagnosticBufferAndDispatchBudget-{label}.zip'
    assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
(out/(tag+'.md')).write_text((root/'docs/combined47t.md').read_text())
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
for d in ['build/event47S','build/event47S2','build/combined47T']:source+=files(d)
source += [out/(tag+'.md')]
evidence=[]
dirs=['evidence/47R','evidence/47S','evidence/47S2','evidence/47T',
      'evidence/event47S-20260930-171011-533','evidence/event47S2-20260930-172059-194',
      'evidence/event47S2-20260930-172614-039','evidence/combined47T-20260930-172845-390',
      'evidence/launchEnv-race47Q-20260930-164842-029','evidence/launchEnv-repeat47Q-20260930-171243-658',
      'evidence/observer47Q-cpu-1','evidence/observer47Q-sample-1','evidence/observer47S-clean-control',
      'work/native-run47q-1-0-1-1-1-1-20260930-170018-199','work/native-run47q-0-0-1-1-1-1-20260930-171244-067']
for v in ['47R','47S2','47T']:
    for rec in js(f'evidence/{v}/startup-runs.json'):
        run=Path(rec['run']);assert (run/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
        dirs.append(str(run.relative_to(root)))
for d in dirs:
    assert (root/d).exists(),d
    evidence += files(d)
    for suffix in ['.log','.exit']:
        p=root/(d+suffix)
        if p.exists():evidence.append(p)
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,
    goal_status='active/incomplete',combined_build=js('evidence/47T/build-gate.json'),
    combined_startup_verified=True,combined_race_verified=False,large_gain_verified=False,
    cache_full_race_state_verified=True,cache_clean_pair_gain_percent=1.90026,
    zero_alarm_counterexample_reproduced=False,failed_test_expectation_preserved=True,
    active_runs_excluded=True,xbox_fit_verified=False,xbox_execution_verified=False)
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
print('Every member reopened and hash verified; active full races excluded.',flush=True)
