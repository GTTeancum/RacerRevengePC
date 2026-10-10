"""Freeze verified startup/GS differential evidence before the long race sweep."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs'
tag='RacerRevenge-Checkpoint47L-RawGifCandidateStartup'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
for name in ('build','build-fixture-v3','differential-v3'):
    assert (root/f'evidence/47L/{name}.exit').read_text(encoding='utf-8-sig').strip()=='0'
assert (root/'evidence/47L/startup/all.exit').read_text(encoding='utf-8-sig').strip()=='0'
report=js('evidence/47L/differential-v3/report.json');assert report['success'] and len(report['tests'])==8
runs=js('evidence/47L/startup/runs.json');assert len(runs)==3
for r in runs:
    assert js(f"evidence/47L/startup/mode-{r['mode']}.json")['saved_state_equivalent']
    assert (Path(r['run'])/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
provenance=js('build/gs47L/provenance.json')
assert sha(root/provenance['original'])==provenance['original_sha256']
assert sha(root/'build/gs47L/gs_frontend.cpp')==provenance['candidate_sha256']
binary=root/'work/native/ninja-build-release-opt/racer_gs47l_fixed_probe.exe'
for r in runs:assert sha(Path(r['run'])/'racer_native_probe.exe')==sha(binary)
dependencies=[]
for label,digest in [('Source','6cb89e7ba1e5d2c5646f41db0ee13870bffc9d16473d4e3dfce1b621129e1af6'),
                     ('Evidence','d6b93ed755c846146ef61b4f6f32c9164c6eec53e23204cafcec1a0cf805badf')]:
    p=out/f'RacerRevenge-Checkpoint47K-Pcsx2ScalarMmiAndRaceProfile-{label}.zip'
    assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
doc=f'''# Checkpoint 47L — raw-GIF candidate, startup verified

2026-09-30. `D:/Programming/GitHub/RacerRevengePC`. Goal active/incomplete.
All native runs in this archive are terminal. A longer three-way race sweep is
prepared but is not included or claimed complete by this checkpoint.

## Change and acceptance

The isolated GS frontend avoids constructing structured draw batches that the
current PCSX2 raw-GIF backend ignores. The original preferred-display predicate
is retained using the same values directly, and vertex queue progression and
history remain unchanged. Mode 2 separately disables native duplicate history.
The canonical GS source and actual PCSX2 renderer are unchanged.

Eight original/candidate configurations pass, each using 42 deterministic GS
fixtures and 504 intermediate preferred-display observations. Raw mode delivers
2,016 unchanged original packets. The batch candidate eliminates 334 redundant
submissions, with full snapshots, history, test pixels, VRAM and synchronization
counts matching. Mode 2 intentionally omits history. Non-raw backends preserve
all submissions and output. This is a mock-backend differential of real GS code,
not a claim of real GPU correctness by itself.

All three modes also complete native VBlank 500 and match the accepted 47J2 run:
32 MiB EE RAM, 2 MiB IOP RAM, PCM, final image, reported terminal state, exact EE
cycle and unchanged memory cards. The actual fixed-stop marker appears once.
The frozen candidate binary SHA-256 is `{sha(binary)}`.
Exact artifact hashes and source provenance are in the evidence and manifest.

The first two fixture attempts are retained. The first used an incompatible
2-pixel mock row stride; the second failed to enable history, which defaults to
paused. The corrected fixture uses the frontend's 640-pixel stride and enables
history explicitly. Their source/logs and captures, plus a batch-path launch
error, are preserved. No acceptance rests on the rejected fixture runs.

## Performance status

No new race performance result is accepted here. The established 47H gain remains
1.705227x in its controlled race window, as documented by 47J2/47K. The new mode
0/1/2 sweep uses one frozen binary and the same input, bridge, VBlank target and
capture settings; host profiling is off. Mode 1 retains full history. Mode 2's
additional effect is capture-diagnostic reduction and must be reported separately.

Native history is ordinarily paused, but the capture-enabled probe turns it on.
This means history reduction is not a new CPU/JIT speedup or an Xbox FPS forecast.
The altered source remains opt-in and the long race acceptance remains open.

## Recovery and next step

Restore 47K and its documented 47J2 recovery chain, then overlay this Source
archive. It includes the full current shared core/MMI snapshot, test/build/run
scripts, the isolated GS candidate and documentation. Original game/reference
dependencies remain pinned by the earlier checkpoints. The Evidence archive
contains all completed startup runs, their executable/DLL/resource copies,
the final test executables, fixture data, logs, state reports and old failed
fixture sources. Archives are reopened and each member is SHA-256 verified.

`tools/Build-Gs47L.ps1` creates the isolated native targets; it refuses to replace
the existing evidence directory, so use a fresh attempt log when rebuilding in
an already restored workspace. `Run-Native47L.ps1` accepts `-RawMode 0`, `1` or
`2` and an optional frozen `-BinarySource`. `Run-NativeSweep47L.ps1` performs the
sequential full race comparison; inspect its real process/session and reports
before declaring it terminal. Test source and the full experiment rationale are
in `docs/graphics-experiment47l.md`.

Open work: large CPU/graphics performance gains, compact EE integration, full
64 MiB fit, game-specific VU/NV2A rendering, sky/surface correctness, a complete
lap, and actual Xbox boot/performance. These are not proven by this checkpoint.
'''
(out/(tag+'.md')).write_text(doc,encoding='utf-8')
source=[root/p for p in ('CMakeLists.txt','.gitignore','.gitattributes','README.md')]
for d in ('src','tests','platform','tools','docs'):
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += files('third_party/pcsx2-mmi47k')+files('build/gs47L')+[out/(tag+'.md')]
evidence=files('evidence/47L')
for r in runs:evidence += [p for p in Path(r['run']).rglob('*') if p.is_file()]
evidence += list((root/'work/native/ninja-build-release-opt').glob('racer_gs47l_*.exe'))
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),goal_status='active/incomplete',
    dependencies=dependencies,closed_native_runs=runs,binary_sha256=sha(binary),verified_vblank=500,
    gs_configurations=8,fixtures_per_configuration=42,preferred_state_observations_per_configuration=504,
    full_race47L_verified=False,new_speedup_verified=False,xbox_execution_verified=False)
for label,paths in (('Source',source),('Evidence',evidence)):
    archive=out/(tag+'-'+label+'.zip');records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            name=p.relative_to(root).as_posix();digest=sha(p)
            z.write(p,name);assert sha(p)==digest,p
            records.append(dict(path=name,bytes=p.stat().st_size,sha256=digest))
    with zipfile.ZipFile(archive) as z:
        assert len(z.namelist())==len(records)
        for r in records:
            with z.open(r['path']) as f:assert hashlib.file_digest(f,'sha256').hexdigest()==r['sha256'],r['path']
    manifest[label]=dict(path=archive.name,bytes=archive.stat().st_size,sha256=sha(archive),files=records)
    print(label,manifest[label]['sha256'],len(records),flush=True)
manifest['verified_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
(out/(tag+'-Manifest.json')).write_text(json.dumps(manifest,indent=2),encoding='utf-8')
print('All archive members reopened and verified.',flush=True)
