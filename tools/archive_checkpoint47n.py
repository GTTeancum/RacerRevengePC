"""Archive closed registry startup and complete GS sweep evidence."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs'
tag='RacerRevenge-Checkpoint47N-IntegratedCompactRegistry'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
build='evidence/memory47N-20260930-155547-464';sweep='evidence/native-sweep47L-20260930-152035-712'
for p in ['evidence/47N/startup.exit','evidence/47N/memory-report.exit',build+'/build.exit',sweep+'/sweep.exit']:
    assert (root/p).read_text(encoding='utf-8-sig').strip()=='0'
memory=js('evidence/47N/memory-report.json');runs=js('evidence/47N/startup-runs.json')
for run in runs:
    assert js(f"evidence/47N/{run['variant']}-startup-state.json")['saved_state_equivalent']
    assert (Path(run['run'])/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
gsruns=js(sweep+'/runs.json');assert len(gsruns)==3
for run in gsruns:
    assert js(sweep+f"/state-{run['mode']}.json")['saved_state_equivalent']
    assert (Path(run['run'])/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
dependencies=[]
for label,digest in [('Source','827047823dc8899b188e05c50d51ece4921cb977c3c989720b65a0bfaf2acefc'),('Evidence','3c6033d99de8f217e00125a796127b2157482a5994c7dc233f991c7da0d3dd1f')]:
    p=out/f'RacerRevenge-Checkpoint47M-CompactLeavesNativeStartup-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
doc='''# Checkpoint 47N — compact registry integrated with the current runtime

2026-09-30. D:/Programming/GitHub/RacerRevengePC. Goal active/incomplete.

The previously verified compact exact-PC registry is now integrated with the
47J2 IOP and 47L GS candidate, both with and without the 47M compact leaves.
Both native VBlank-500 startups match their accepted references: full EE/IOP
RAM, PCM, final picture, reported terminal state, exact stop and unchanged cards.
The combined leaf variant again executes 498,152 calls with zero fallbacks.

The registry removes 10,321,920 bytes (9.84375 MiB) from each linked virtual image
relative to its matching dense-table executable. Link maps prove that the dense
table is absent. This includes initializer code removal; no original guest
instruction implementation was removed. The registry-only image is 101,502,976
bytes; the combined image is 101,605,376 bytes. Both already exceed the stock
Xbox's entire 67,108,864-byte budget before guest RAM or renderer/device heaps.
Windows image extents are not Xbox image sizes or resident working memory.

Startup process-memory samples are preserved. They include the Windows PCSX2
renderer and its dependencies and are not an Xbox forecast. The immutable
32-bit registry encoding is 890,386 bytes, excluding mutable replacements and
the rest of the runtime. The 283 leaf spans total only 21,792 bytes in the earlier
link map; their diagnostic executor costs more. Those original leaves remain
linked. Broad compact execution/code generation is needed for the requested fit.

The complete 47L GS sweep is terminal and all modes match saved state at VBlank
7000. Fixed-window capture throughput is 5.387408, 5.910268 and 5.967689 emulated
VBlanks/second for modes 0, 1 and 2. Mode 1 retains history; mode 2 suppresses it.
These are single-run observations with overlapping capture-bracket uncertainty,
not repeat-confirmed speedups, distinct-image FPS or Xbox performance. History
suppression contributes little and is not the route to the requested large gain.

The compact-leaf full-race test is still active at checkpoint preparation, and
the combined-registry race is queued behind it with its executable frozen by
hash. Their mutable directories are excluded. Inspect actual live handles and
terminal reports before starting any duplicate run. No full-race registry or
compact-leaf acceptance is claimed here.

## Recovery and evidence

Restore 47M and its documented dependencies, then overlay this Source archive.
The shared source snapshot includes build/run scripts, both borrowed PCSX2
instruction packages, exact registry encoding/data, unchanged 47E compact
runtime adapter, current isolated GS source, leaf adapter and 47N probes.
The original compact-runtime inputs and outputs were hash-checked before reuse.

The Evidence archive includes both new startup runs, binaries, build logs,
link maps, image/process-memory reports, the completed GS sweep reports and
its mode-2 run. Modes 0 and 1 remain in the required 47M Evidence archive.
Every new ZIP member is reopened and SHA-256 verified. Source recovery does
not rely on the live process or a future successful test.

Build-Memory47N.ps1 prepares and builds both variants with isolated evidence.
Run-Native47N.ps1 selects registry or memory and validates the exact accepted
build hash. Run-MemoryStartup47N.ps1 deliberately requires a new evidence
directory to avoid overwriting the recorded startup. Run-MemoryRace47N.ps1
queues the combined full-race check after the existing 47M race.

Next large-performance investigation: measure synchronous renderer bridge
handoffs. work/pcsx2-bridge41n.cpp flushes staged GIF packets using a future and
waits again for VSync; this source observation alone does not establish its cost.
Preserve guest-visible ordering and true readback/FINISH fences when experimenting.

Open: major CPU/rendering gains, broad compact EE execution, the 64 MiB budget,
single-core game-specific VU/NV2A rendering, remaining sky/surface errors, an
actual completed lap, and Xbox boot/performance. The goal remains active.
'''
(out/(tag+'.md')).write_text(doc)
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
for d in ['third_party/pcsx2-mmi47k','third_party/pcsx2-scalar47m','build/registry47D','build/compact47E','build/leaf47M','build/gs47L']:
    source+=files(d)
source += [p for p in files('build/memory47N') if p.suffix in {'.cpp','.json'}]
source += [root/'evidence/47M/leaf-eligibility.json',out/(tag+'.md')]
evidence=files('evidence/47N')+files(build)
for run in runs:evidence += [p for p in Path(run['run']).rglob('*') if p.is_file()]
mode2=next(r for r in gsruns if r['mode']=='2')
evidence += [p for p in Path(mode2['run']).rglob('*') if p.is_file()]
evidence += [p for p in (root/sweep).iterdir() if p.is_file()]
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',memory_report=memory,
    closed_startup_runs=runs,gs_sweep_runs=gsruns,gs_modes_0_1_in_dependency=True,full_leaf_race_verified=False,full_registry_race_verified=False,xbox_fit_verified=False,xbox_execution_verified=False)
for label,paths in [('Source',source),('Evidence',evidence)]:
    archive=out/(tag+'-'+label+'.zip');records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            name=p.relative_to(root).as_posix();digest=sha(p)
            z.write(p,name);assert sha(p)==digest,p
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
