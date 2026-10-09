"""Freeze the completed compact-leaf race and validated renderer observer."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs'
tag='RacerRevenge-Checkpoint47O-LeafRaceAndRendererProfile'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
build='evidence/gsProfile47O-20260930-160733-694';leaf='evidence/leaf-race47M-20260930-154837-614'
for p in ['evidence/47O/startup.exit',build+'/build.exit',leaf+'/race.exit']:
    assert (root/p).read_text(encoding='utf-8-sig').strip()=='0'
assert js(leaf+'/state.json')['saved_state_equivalent']
leafrun=js(leaf+'/run.json')['run'];profile=js('evidence/47O/startup-profile.json')
assert profile['terminal_vsync_reconciled'] and profile['final_vsync_calls']==500
startups=js('evidence/47O/startup-runs.json');assert len(startups)==2
for run in startups:
    assert js(f"evidence/47O/{run['profile']}-startup-state.json")['saved_state_equivalent']
    assert (Path(run['run'])/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
gate=js('evidence/47O/build-gate.json')
for run in startups:assert sha(Path(run['run'])/'racer_native_probe.exe')==gate['binary_sha256']
provenance=js('build/gsProfile47O/provenance.json')
assert sha(root/provenance['original'])==provenance['original_sha256']
assert sha(root/'build/gsProfile47O/gs_pcsx2_backend41n.cpp')==provenance['candidate_sha256']
dependencies=[]
for label,digest in [('Source','f2f8fffc3c7898ebcc614975ec71ca6cb5079ce1d868c9417185364c2ad894d4'),('Evidence','08d6fd3f1ea608b736407653e37abf4951a59b40b45c2104047d406e322b39bf')]:
    p=out/f'RacerRevenge-Checkpoint47N-IntegratedCompactRegistry-{label}.zip';assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
doc='''# Checkpoint 47O — full leaf race verified; renderer profiler validated

2026-09-30. D:/Programming/GitHub/RacerRevengePC. Xbox-first goal active/incomplete.

The 47M compact executor completes the native VBlank-7000 test with 1,954,337
calls and zero fallbacks. It matches the accepted 47L mode-1 reference in full
EE/IOP RAM, PCM, final image, reported terminal state, exact stop and unchanged
memory cards. This proves the saved boundary; it is not a completed lap or a
compact-executor FPS improvement. The 283 original native functions remain linked.

The new 47O diagnostic measures nine existing renderer DLL calls in an isolated
native backend. It keeps the original lock, packets, transfers, FIFO/readback
handling, fences and PCSX2 DLL. Measurement off/on both match the 47N combined
runtime at VBlank 500. The CSV has complete monotonic counters, consistent packet
histograms and exactly 500 backend VSync calls matching the scheduler stop.

The measured startup lasts 31.578 host seconds. Renderer DLL calls account for
6.761% of that interval, mostly one initialization call at 6.148%. Remaining
renderer calls total about 0.613%. These startup measurements do not establish
the race bottleneck or justify removing synchronization. The full-race profile
is queued after the active 47N registry race; neither unfinished run is accepted
or included in these archives.

The profile is host elapsed time inside each DLL call: producer work, GS-thread
work, blocking waits and preemption. It is not exclusive CPU cost, GPU duration,
or queue-wait time alone. The profile adds observer overhead and is not a matched
throughput benchmark. The requested race-window analysis requires a reconciled
7000-VBlank terminal count and complete cumulative snapshots at 6000 and 6900.

## Recovery

Restore 47N and its documented dependencies, then overlay this Source archive.
It contains current shared code/tools, the isolated backend observer and its
original source hash, borrowed instruction code, registry encoding/adapters,
current native probes and the original backend source used to prepare the clone.
Build-GsProfile47O.ps1 can wait for an active run to pass VBlank 6950 before
compiler work. Run-GsProfileStartup47O.ps1 validates measurement off/on;
Run-GsProfileRace47O.ps1 freezes the binary and queues race validation. Scripts
preserve previous evidence instead of overwriting accepted outputs.

The Evidence archive contains the completed 47M race, both profiler startups,
profile CSV and analysis, state comparisons, accepted executables, logs and
captures. Exact SHA-256 hashes and dependency archives are in the manifest.
Every new archive member is reopened and hash-verified. Read existing live
process/session state before launching duplicate queued tests.

The current compact-registry integration still has a roughly 97 MiB Windows
image before game memory and renderer/device allocations. The 9.84375 MiB image
reduction is accepted at startup, not an Xbox memory fit. Large performance gains,
broad compact EE/code generation, the full 64 MiB budget, game-specific VU/NV2A,
sky/surface correctness, a completed lap and Xbox boot/performance remain open.
'''
(out/(tag+'.md')).write_text(doc)
source=[root/p for p in ['CMakeLists.txt','.gitignore','.gitattributes','README.md']]
for d in ['src','tests','platform','tools','docs']:
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
for d in ['third_party/pcsx2-mmi47k','third_party/pcsx2-scalar47m','build/registry47D','build/compact47E','build/leaf47M','build/gs47L','build/gsProfile47O']:
    source+=files(d)
source += [p for p in files('build/memory47N') if p.suffix in {'.cpp','.json'}]
source += [root/provenance['original'],root/'evidence/47M/leaf-eligibility.json',out/(tag+'.md')]
evidence=files('evidence/47O')+files(build)
for run in startups:evidence += [p for p in Path(run['run']).rglob('*') if p.is_file()]
evidence += [p for p in Path(leafrun).rglob('*') if p.is_file()]
evidence += [p for p in (root/leaf).iterdir() if p.is_file()]
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,goal_status='active/incomplete',
    closed_profile_startups=startups,closed_leaf_race=leafrun,leaf_full_race_verified=True,leaf_calls=1954337,leaf_fallbacks=0,
    accepted_profile_build=gate,profile_startup=profile,full_registry_race_verified=False,full_race_renderer_profile_verified=False,xbox_fit_verified=False,xbox_execution_verified=False)
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
