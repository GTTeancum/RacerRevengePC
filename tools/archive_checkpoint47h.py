"""Archive fixed-stop evidence, an idle-cache correction and closed game runs."""
from pathlib import Path
import datetime
import hashlib
import json
import re
import zipfile

root=Path(__file__).resolve().parents[1]
out=root/'outputs'
tag='RacerRevenge-Checkpoint47H-FixedStateAndIopCache'
assert not list(out.glob(tag+'*')), 'Never overwrite a checkpoint'
def sha(p):
    with p.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def files(d): return [p for p in (root/d).rglob('*') if p.is_file()]
def readjson(p): return json.loads((root/p).read_text(encoding='utf-8-sig'))

checks={p:readjson(p) for p in [
    'evidence/47F/vblank1-comparison.json','evidence/47F/vblank500-comparison.json',
    'evidence/47G/cache-vblank500-comparison.json',
    'evidence/47G/idle-off-reference-vblank500-comparison.json',
    'evidence/47G/idle-on-reference-vblank500-comparison.json',
    'evidence/47H/corrected-vblank500-comparison.json']}
assert all(c['saved_state_equivalent'] for c in checks.values())
mismatch=readjson('evidence/47G/reference-vblank500-comparison.json')
assert not mismatch['saved_state_equivalent']
for phase in ('47F','47G','47H'):
    assert (root/f'evidence/{phase}/build.exit').read_text(encoding='utf-8-sig').strip()=='0'
for phase in ('47G','47H'):
    assert all(x['matches_frozen45Y_via47B2'] and x['exit_matches']
               for x in readjson(f'evidence/{phase}/regressions/comparison.json'))
for name in ('differential','scheduler'):
    assert (root/f'evidence/47H/{name}.exit').read_text(encoding='utf-8-sig').strip()=='0'

pair='evidence/native-pair47H-20260930-135828-744'
live='work/native-run47h-0-0-20260930-135829-565'
assert not (root/pair/'pair.exit').exists(), 'Update checkpoint status if the pair has finished'
closed=[]
for pattern in ('native-run47f-*','native-run47g-*','native-run47h-*'):
    for d in sorted((root/'work').glob(pattern)):
        if not (d/'native.exit').exists(): continue
        assert (d/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
        assert json.loads((d/'fixed-stop-validation.json').read_text(encoding='utf-8-sig'))['verified']
        closed.append(d)
assert len(closed)==9, [p.name for p in closed]
corrected=root/'work/native-run47h-1-1-20260930-135653-267'
statlines=[s for s in (corrected/'native.log').read_text(errors='replace').splitlines()
           if 'idle-batch advances=' in s or 'ready-cache enabled=' in s]
binary=root/'work/native/ninja-build-release-opt/racer_iop47h_fixed_probe.exe'
doc=f'''# Checkpoint47H — exact startup comparison and IOP cache correction

2026-09-30. Root `D:/Programming/GitHub/RacerRevengePC`.
Goal active/incomplete. The long corrected race comparison is running.
No Xbox game execution, Xbox FPS, total memory fit, or completed lap is claimed.

## Verified results

- Fixed-VBlank diagnostic runners stop cooperatively after the original selected
  VBlank event applies its side effects. They require exactly one matching stop
  marker, successful native exit, and unchanged memory-card manifests.
- The compact registry runtime matches the reference at VBlanks 1 and 500:
  all 32 MiB EE RAM, 2 MiB IOP RAM, PCM bytes, final PPM image, reported CPU/thread
  state, and exact terminal EE cycle. It retains original instruction objects.
- Ready-selection caching passes 200,001 scheduling decisions with full thread
  state and RAM comparisons. The extended stress test includes 381 blocking
  event waits and 219 semaphore waits. Full selection scans fall from 200,001
  to 9,564, dead-thread scans from 160,891 to 7,435; these are synthetic counters,
  not a game FPS claim. Cache reuse preserves priority/id ordering and delay wakes.
- Four physical IRX scenarios pass 372 full state/RAM/PCM checkpoints. Both cache
  modes produce byte-identical broad regression logs relative to frozen 45Y via
  47B2. Existing sound-RPC payload, PollEventFlag and DBCMAN failures remain.
  Suite exits remain 1/1/0/1; the compatibility result remains 17/18.
- With both corrected idle batching and ready caching enabled, candidate 47H
  matches the original unbatched execution exactly at VBlank 500. Final EE cycle
  is 2,457,649,512; IOP cycles 297,635,639; IOP instructions 28,557,644.

## Fault found and corrected

The first 47B2 idle-batching race timing showed 3.188851 versus 4.436830 capture
VBlanks per host second over VBlanks 6,000–7,000, ratio 1.391357. Both 1,800-second
runs exited successfully. This is measured guest throughput, not distinct-image
FPS. The later exact-state comparison found that the batched version diverged:
7 EE RAM pages and 3 IOP RAM pages differed, PCM length differed by 1,028 bytes,
and the EE stop cycle differed by 8. Images alone did not expose the fault.
That earlier 39% difference is NOT accepted as a behavior-preserving speedup.

The discrepancy reproduces in both the original 47B2 binary and the 47G clone
with ready caching disabled. Disabling idle batching restores exact equality.
This isolates the problem from compact lookup and ready selection caching.

Nested RPC execution can establish an idle-window proof inside runCycles, then
make a thread runnable through a returned mutable pointer. 47H invalidates the
proof when reacquiring that pointer, around nested CPU/RPC execution, and both
before and after public mutation scopes. The corrected full native VBlank-500
comparison reproduces the original state, including all audio bytes. This is
strong startup evidence; later race state still needs comparison.

Corrected probe SHA-256: `{sha(binary)}`.
Corrected startup counters:
```
{chr(10).join(statlines)}
```

## Running experiment

`{pair}` freezes the executable and DLLs before either run. The same binary
runs with both caches off, then both on, with the same inputs/bridge and empty
memory cards. Each must reach exactly VBlank 7,000; 3,000 host seconds is only
a failure timeout. Orchestrator exec session **93889** is the existing handle.
Baseline is `{live}`. Do not restart it because an observation wait expires.
Only frozen pair files and stable launch metadata are archived while it runs.
Live capture/log files are intentionally excluded from this checkpoint.

When both finish, compare saved state with `tools/compare_fixed47f.py`, then
measure matching capture windows. The 47C analyzer is currently specific to the
old pair and needs a new invocation/adapter for this pair. Preserve mismatches.
The ordinary reference executable and user-supplied PCSX2 source are unchanged.

47C timing windows finished before the 47F/G builds, the 47E archive finished
before the batched 6,000 boundary, and later 47H work began after both 47C runs
closed. Whole-run timings are contaminated by early/post-window build/archive
overlap; only the documented selected 47C window was used. New archive start/end
times are recorded separately and must be checked against the new pair window.

## Recovery and remaining work

Apply this Source after 47E Source (and its 47D/47B dependencies). This includes
the shared core, all current tools/tests, complete 47G and 47H IOP source clones,
fixed scheduler and compact adapter source. Full legacy generation/game recovery
still needs 46G and its transitive chain; toolchains remain pinned through 47A.
The build command at `evidence/47H/build.cmd` selects the prepared 47H clone and
builds the fixed probe plus tests. Save new build logs instead of overwriting
archived evidence. Preparers intentionally reject existing candidate paths.
The manifest pins dependencies and every archive member; all members are reopened
and SHA-256 verified after ZIP creation. Failed comparisons are retained too.

Portable core checks remain the latest 149,413 per build, digest `947da74c` on
Windows x86/x64 and nxdk-built CPU objects executed in Windows x86. No portable
core source changed here. The diagnostic XBE has not been booted on Xbox.
Machine code still exceeds stock Xbox RAM; compact translation, device scheduling
and an NV2A renderer remain substantial work. No 30/60 FPS Xbox forecast is valid.
Latest 47C picture shows textured racing at lap 1/3, position 8/8, timer 1:29.26,
speed 11 near a canyon wall; sky is white. No whole-course/render/audio acceptance.
'''
(out/(tag+'.md')).write_text(doc,encoding='utf-8')
source=[root/p for p in ('CMakeLists.txt','.gitignore','.gitattributes','README.md')]
for d in ('src','tests','platform','tools','docs'):
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
for d in ('build/compact47E','build/fixed47F','build/iop47G','build/iop47H'):
    source += files(d)
source.append(out/(tag+'.md'))
evidence=[]
for d in ('evidence/47F','evidence/47G','evidence/47H',
          'evidence/native-pair47C-20260930-124035-308'):
    evidence += files(d)
batched='work/native-run47c-batched-20260930-131039-775'
assert (root/batched/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
evidence += files(batched)
for d in closed: evidence += [p for p in d.rglob('*') if p.is_file()]
evidence += [root/p for p in ('evidence/47C-final.json','evidence/47C-batched-final.png',
    'evidence/47C-progress-1326.json', pair+'/pair.json')]
evidence += files(pair+'/bin')
for name in ('process-launch.json','launch.json','binary-hash.json','bridge-hash.json','buttons.txt','cards-before.json'):
    evidence.append(root/live/name)
for pattern in ('racer_fixed47f_probe.exe','racer_compact47f_probe.exe','racer_iop47g_*.exe','racer_iop47h_*.exe'):
    evidence += list((root/'work/native/ninja-build-release-opt').glob(pattern))
dependencies=[]
for name in ('RacerRevenge-Checkpoint47E-CompactNativeAdapter-Source.zip',
             'RacerRevenge-Checkpoint47E-CompactNativeAdapter-Evidence.zip'):
    p=out/name
    dependencies.append(dict(path=name,sha256=sha(p),bytes=p.stat().st_size))
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
    goal_status='active/incomplete',dependencies=dependencies,
    legacy_dependency='46G and transitive chain; 47D/47B through 47E; nxdk/MSYS2 via 47A',
    corrected_startup_saved_state_equivalent=True, original_idle_candidate_accepted=False,
    corrected_race_performance_verified=False, xbox_execution_verified=False,
    live_pair=pair, live_session=93889, closed_startup_runs=[p.name for p in closed])
for label,paths in (('Source',source),('Evidence',evidence)):
    archive=out/(tag+'-'+label+'.zip')
    records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            assert p.is_file(),p
            name=p.relative_to(root).as_posix()
            digest=sha(p)
            z.write(p,name)
            assert sha(p)==digest,p
            records.append(dict(path=name,bytes=p.stat().st_size,sha256=digest))
    with zipfile.ZipFile(archive) as z:
        assert len(z.namelist())==len(records)
        for r in records:
            with z.open(r['path']) as f: assert hashlib.file_digest(f,'sha256').hexdigest()==r['sha256'],r['path']
    manifest[label]=dict(path=archive.name,bytes=archive.stat().st_size,sha256=sha(archive),files=records)
    print(label,manifest[label]['sha256'],len(records),flush=True)
manifest['verified_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
(out/(tag+'-Manifest.json')).write_text(json.dumps(manifest,indent=2),encoding='utf-8')
print('All archive members reopened and verified.',flush=True)
