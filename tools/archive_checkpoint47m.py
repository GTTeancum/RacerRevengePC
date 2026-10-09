"""Freeze verified compact-leaf startup plus the two closed GS race runs."""
from pathlib import Path
import datetime,hashlib,json,zipfile
root=Path(__file__).resolve().parents[1];out=root/'outputs'
tag='RacerRevenge-Checkpoint47M-CompactLeavesNativeStartup'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def js(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
scalar='evidence/scalar47M-20260930-153944-204'
native='evidence/leaf47M-20260930-154534-694'
startup='work/native-run47m-1-1-1-1-20260930-154627-034'
sweep='evidence/native-sweep47L-20260930-152035-712'
for name in ['x86','x64','xbox']:assert (root/scalar/(name+'.exit')).read_text(encoding='utf-8-sig').strip()=='0'
for name in ['build','check']:assert (root/native/(name+'.exit')).read_text(encoding='utf-8-sig').strip()=='0'
assert js('evidence/47M/startup-state.json')['saved_state_equivalent']
assert not js('evidence/47M/isa/report.json')['forbidden_instructions']
gate=js('evidence/47M/accepted-native-build.json')
assert sha(root/startup/'racer_native_probe.exe')==gate['binary_sha256']
selection=js('build/leaf47M/selection.json');assert selection['eligible']==283 and len(selection['deferred'])==12
inventory=js('evidence/47M/leaf-eligibility.json')
for e in inventory['functions']:assert sha(root/e['path'])==e['source_sha256']
closed=[dict(kind='leaf startup',run=startup)]
for mode in ['0','1']:
    report=js(sweep+'/state-'+mode+'.json');assert report['saved_state_equivalent']
    runs=js(sweep+'/runs.json');runs=runs if isinstance(runs,list) else [runs]
    run=next(x['run'] for x in runs if x['mode']==mode)
    closed.append(dict(kind='GS mode '+mode,run=str(Path(run).relative_to(root))))
for run in closed:assert (root/run['run']/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
dependencies=[]
for label,digest in [('Source','e8ab0a1daa5af06f47d986a3bc47aa5f8da803eb03a21501dd8aaccb7d37a7f7'),('Evidence','d388f21a0c47cdb5440dc82904bfddbe35101e1f19746e676c0c258b69532a39')]:
    p=out/f'RacerRevenge-Checkpoint47L-RawGifCandidateStartup-{label}.zip'
    assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))
doc='''# Checkpoint 47M — compact leaves connected to the native game

2026-09-30. D:/Programming/GitHub/RacerRevengePC. Xbox-first goal active/incomplete.

283 original-ELF register-only functions now execute through the compact
PCSX2-derived scalar/MMI core in the native Windows game. Native VBlank 500
startup installed all 283, executed 498,152 calls and used zero fallbacks.
EE/IOP RAM, PCM, final image, exact stop, reported terminal state and unchanged
memory cards match the accepted reference. This is startup acceptance only.

The three scalar/leaf builds each passed 593,931 checks, digest c03675d4:
Windows x86, Windows x64, and the exact nxdk-compiled objects run on Windows x86.
The direct original-function comparison passed 72,448 complete CPU-context
cases, digest a769d217. The Xbox diagnostic XBE builds; Xbox execution is not
verified. No target-incompatible instructions were detected in seven inspected
project objects; external helpers and hardware behavior are outside that check.

The initial 295-function comparison exposed conditional-move semantics that
differ between generated native code and the supplied PCSX2 implementation.
Twelve MOVN/MOVZ functions remain on the original path. The failed comparison
and compiler rejection are preserved, with details in docs/compact-leaves47m.md.
The first rejected native executables were overwritten before they could be
copied; their hashes, sources and logs remain. Accepted executables are intact.

The first two GS full-race runs are also now closed and archived. Both match
saved state at VBlank 7000. The batch-elision candidate with full history records
5.910268 vs 5.387408 emulated VBlanks per host second in the fixed 6000–6900
window (1.097052x). Capture-bracket bounds overlap; this is one measured pair,
not a repeat-confirmed speedup or distinct-image FPS. Reduced-history mode 2
remains outside this checkpoint. Its diagnostic effect must be reported separately.

The compact-leaf full-race comparison is queued after the GS sweep, with the
accepted binary frozen by hash. Neither a queued nor a running test counts as
acceptance. Inspect the existing live sessions and their terminal reports before
starting duplicate runs. No compact-leaf speedup is claimed: the full original
native code remains linked, so executable memory savings are not yet realized.

## Recovery

Restore 47L and its documented source/dependency chain, then overlay this Source
archive. It includes the full current shared source, both borrowed PCSX2 scalar
and MMI packages, isolated GS source, compact-leaf generated adapter and its
selection manifest, and the 295 original generated functions used by the audit.
The Evidence archive contains both closed GS races, compact-leaf startup,
standalone builds/objects/XBE, accepted native binaries, failed sources/logs,
ISA disassembly and SHA-256 manifests. Every ZIP member is reopened and verified.

Build and run instructions, exact limitations and the conditional-move blocker
are in docs/compact-leaves47m.md. Runtime launch checks the accepted binary hash.
Changes to the native candidate require rerunning the direct differential and
updating that gate from its actual successful result.

Still open: large performance gains, native code removal/compact EE expansion,
64 MiB total fit, game-specific VU/NV2A rendering, sky/surface fixes, a complete
lap, and actual Xbox boot/performance. The preserved PCSX2-backed Windows
version remains the behavioral reference.
'''
(out/(tag+'.md')).write_text(doc,encoding='utf-8')
source=[root/p for p in ('CMakeLists.txt','.gitignore','.gitattributes','README.md')]
for d in ('src','tests','platform','tools','docs'):
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
for d in ('third_party/pcsx2-mmi47k','third_party/pcsx2-scalar47m','build/gs47L','build/leaf47M'):source+=files(d)
source += [root/e['path'] for e in inventory['functions']]+[root/'evidence/47M/leaf-eligibility.json',out/(tag+'.md')]
evidence=files('evidence/47M')
for d in ['evidence/scalar47M-20260930-153906-940',scalar,'evidence/leaf47M-20260930-154223-090',native]:evidence+=files(d)
for run in closed:evidence+=files(run['run'])
# Snapshot only immutable completed mode reports; never include the live sweep's
# changing runs.json, active mode-2 files, or the queued leaf race directory.
for name in ['sweep.json','0.log','1.log','state-0.json','state-0.log','state-1.json','state-1.log','timing-0-1.json','timing-0-1.log']:
    evidence.append(root/sweep/name)
for d in ['build/scalar47M-x86','build/scalar47M-x64','build/xbox-scalar47M']:
    evidence += [p for p in files(d) if p.suffix in {'.obj','.exe','.xbe','.lib'}]
evidence += [root/p for p in ['src/core/mmi47k.obj','src/core/scalar47m.obj','src/core/compact_leaf47m.obj','tests/scalar_reference47m.obj','tests/scalar_suite47m.obj','platform/xbox-scalar/main.obj','platform/xbox-scalar/main.exe'] if (root/p).exists()]
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),goal_status='active/incomplete',dependencies=dependencies,
    closed_native_runs=closed,accepted_native_build=gate,scalar_checks_per_build=593931,scalar_digest='c03675d4',xbox_execution_verified=False,
    full_leaf_race_verified=False,native_code_removed=False,gs_modes_archived=[0,1],live_runs_excluded=True)
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
