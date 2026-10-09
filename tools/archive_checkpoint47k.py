"""Archive the completed 47J2 race/profile and verified scalar PCSX2 MMI port."""
from pathlib import Path
import datetime
import hashlib
import json
import re
import zipfile

root=Path(__file__).resolve().parents[1]
out=root/'outputs'
tag='RacerRevenge-Checkpoint47K-Pcsx2ScalarMmiAndRaceProfile'
assert not list(out.glob(tag+'*')), 'Immutable checkpoint already exists'
run='work/native-run47j2-1-1-1-20260930-144756-248'
buildlog='evidence/mmi47K-20260930-145941-695'
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def readjson(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
state=readjson('evidence/47K/race-state.json')
assert state['saved_state_equivalent']
assert (root/run/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
assert sha(root/run/'racer_native_probe.exe')=='f1427daf3344629f570fc52d927e80813025bc0b0edeca8d38af681a354b4881'
for target in ('xbox','x86','x64'):
    assert (root/buildlog/(target+'.exit')).read_text(encoding='utf-8-sig').strip()=='0'
logs='\n'.join((root/buildlog/(target+'.log')).read_text(encoding='utf-8-sig') for target in ('x86','x64'))
results=re.findall(r'MMI47K checks=(\d+) failures=(\d+) first_line=(\d+) digest=([0-9a-f]+)',logs)
assert results==[('2427678','0','0','f22b8723')]*3, results
isa=readjson('evidence/47K/isa/report.json')
assert not isa['forbidden_instructions'] and len(isa['objects'])==4
assert readjson('evidence/47K/reproduction.json')['reproduced']
profile=readjson('evidence/47K/profile/race-window.json')
provenance=readjson('third_party/pcsx2-mmi47k/provenance.json')
assert provenance['handlers']==99
for record in provenance['files']:
    assert sha(root/record['path'])==record['sha256']

dependencies=[]
expected={'Source':'f1e642711935504c0f7749aff0a49dcbe145cbce83d40669868be6b0b7a2d500',
          'Evidence':'16da713e5da51294216ca8da43278971bc0c7d3cec1d10ff3413984383cef8f7'}
for label,digest in expected.items():
    p=out/f'RacerRevenge-Checkpoint47J2-VerifiedRaceAndCompactIop-{label}.zip'
    assert sha(p)==digest
    dependencies.append(dict(path=p.name,sha256=digest,bytes=p.stat().st_size))

doc='''# Checkpoint 47K — PCSX2 scalar MMI and completed race profile

2026-09-30. Project: `D:/Programming/GitHub/RacerRevengePC`.
Goal remains active and incomplete. All work archived here is terminal; no
game or compiler process is required to restore this checkpoint.

## Accepted native game state

The 47J2 runtime with import/ready caches, corrected idle batching and compact
ownership completes the same VBlank-7,000 sequence as accepted 47H. The exact
stop is EE cycle 34,407,093,000. The saved 32 MiB EE RAM, 2 MiB IOP RAM,
22,631,340 PCM bytes, final image and reported CPU/thread/device state all match.
Memory cards remain empty and unchanged. Run exit and fixed-stop validation pass.

Executable SHA-256: `f1427daf3344629f570fc52d927e80813025bc0b0edeca8d38af681a354b4881`.
The report `evidence/47K/race-state.json` records exact hashes for every compared
artifact, along with the frozen bridge and input hashes. The executable, DLLs,
resources, logs, profile, periodic captures and terminal data are archived.
This does not prove hidden state equivalence at all times or a completed lap.

The existing accepted 47H benchmark remains 3.135670 -> 5.347029 emulated
VBlanks per host second, 1.705227x, for VBlanks 6,000–6,900. That is guest
throughput, not distinct-picture FPS. The new 47J2 run enabled profiling and
is NOT a controlled performance comparison against that benchmark.

## Bottleneck finding

Cumulative host-profile snapshots near captured VBlanks 6,043 and 6,913 show:

| Sampled category | Share of sampled outer-phase elapsed time |
| --- | ---: |
| Guest-function inclusive | 69.92% |
| Scheduler/observers | 10.30% |
| Checkpoint phase | 10.27% |
| Tail events | 9.51% |
| Nested IOP-inclusive | 11.29% |
| Nested EE timers | 4.59% |
| Nested VU run | 7.58% |
| Nested DMA service | 27.23% |

Nested categories overlap each other and the outer phases. Do not sum or
subtract them as exclusive CPU costs. Samples include preemption and observer
overhead. The GS worker/GPU is not independently measured. Profile and capture
boundaries are not simultaneous; the result is a bottleneck guide, not a precise
per-frame budget. Full-run and near-race reports preserve those qualifications.

The IOP is no longer the dominant sampled category in this section. More IOP
caching alone cannot justify an order-of-magnitude performance forecast. One
concrete next target is duplicate native GS frontend work: `GS::kickVertex`
constructs full draw batches and calls `Submit`, while the borrowed PCSX2 raw-GIF
backend implements `Submit` as an empty method. Preferred-display tracking and
debug history still use the native state, and FINISH/CSR/readback ordering must
remain correct. No shortcut to this path was enabled in this checkpoint.

## Borrowed scalar MMI implementation

All 99 packed/extended-integer handlers from the user-supplied PCSX2 `MMI.cpp`
are transplanted with 50 stateful helpers. The handlers receive an explicit CPU
reference; no production global CPU, TLS, allocation or SIMD dependency is added.
The state uses 560 bytes on both x86 and x64. Major/reserved opcode rejection
leaves the state unchanged. PC, timing, events, memory, FPU and VU remain separate.

The original source, opcode tables, bit utility, GPL-3.0-or-later notices and
full license are preserved under `third_party/pcsx2-mmi47k`. Exact upstream file
hashes and changes are documented there. The deterministic preparer reproduces
the compiled production and reference source byte for byte.

Three executions each pass **2,427,678 checks, zero failures, digest f22b8723**:

1. MSVC Windows x86 with `/arch:IA32`.
2. MSVC Windows x64.
3. The exact nxdk/Pentium-III production and test objects linked into a Windows
   x86 runner. The diagnostic Xbox image also links, but has NOT been booted.

Tests compare all registers, HI/LO, code and SA with the unmodified PCSX2 source
across 327,680 combinations of function/subfunction, aliasing and input states.
They separately cover all unsigned-byte saturation pairs and QFSRV byte shifts.
The shared decoder provenance means this is transplant validation, not an
independent proof of every upstream instruction. Signed-wrap and union-alias
assumptions remain documented; this is not a sanitizer clearance.

The nxdk production object's code sections total **18,524 bytes**. The object
audit inspects 22,920 decoded instructions across four production/test objects
and detects no SSE2-or-newer instructions. It excludes CRT/external helpers and
does not replace execution on an Xbox. The MMI component is NOT game-connected;
its size is not the size of a complete compact CPU runtime and carries no FPS
claim. The present full generated CPU code is still too large for stock Xbox.

## Recovery and next work

Restore the 47J2 Source/Evidence dependency pair and its documented earlier
recovery chain, then overlay this Source archive. This adds the complete MMI
port, original-source snapshot, build/test scripts, profile tools and current
documentation. Preserve `work/reference/pcsx2-master` and ordinary Windows game
sources. The existing project-local toolchains are the same pinned toolchains
recorded by 47A and 47J2. No new download or user test was needed.

Reproduce the MMI transplant with `tools/prepare_mmi47k.py`, then build and test
with `tools/Build-Mmi47K.ps1`. The latter produces independent logs, builds the
diagnostic XBE, and executes both MSVC and nxdk-object differential checks. The
frozen native game binary can be rerun through `Run-Native47J2.ps1 -BinarySource`
using the executable from the archived completed run. Retain the same input,
bridge and VBlank target for state comparisons.

Next: measure/remove redundant GS work without disturbing guest-visible state;
continue compact EE backend integration using the borrowed MMI semantics. Larger
event/dispatch changes require exact deadlines and interaction-boundary audits.
Open blockers are full 64 MiB fit, complete CPU/VU coverage, Xbox NV2A rendering,
actual Xbox boot/performance, remaining sky/surface issues, and a complete lap.
Do not turn this foundation or the 1.705x Windows result into an Xbox FPS claim.
'''
assert sum(r['instructions'] for r in isa['objects'])==22920
(out/(tag+'.md')).write_text(doc,encoding='utf-8')
source=[root/p for p in ('CMakeLists.txt','.gitignore','.gitattributes','README.md')]
for directory in ('src','tests','platform','tools','docs'):
    source += [p for p in files(directory) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source+=files('third_party/pcsx2-mmi47k')+[out/(tag+'.md')]
source += [root/'work/native/sources/src/PS2Recomp/ps2xRuntime/src/lib/gs'/name for name in ('gs_frontend.cpp','gs_pcsx2_backend41n.cpp')]
evidence=files('evidence/47K')+files(buildlog)+files(run)
evidence += [root/p for p in ('build/mmi47K-x86/racer_mmi47k_check.exe',
    'build/mmi47K-x64/racer_mmi47k_check.exe','build/mmi47K-x86/racer_mmi47k_xbox_objects_check.exe',
    'build/xbox-mmi47K/default.xbe','platform/xbox-mmi/main.exe','platform/xbox-mmi/main.obj')]
evidence += [root/r['path'] for r in isa['objects']]
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
    goal_status='active/incomplete',dependencies=dependencies,closed_native_run=run,
    verified_native_vblank=7000,saved_state_equivalent=True,mmi_handlers=99,mmi_checks_per_execution=2427678,
    mmi_digest='f22b8723',mmi_nxdk_code_bytes=18524,xbox_execution_verified=False,mmi_game_connected=False,
    completed_lap_verified=False)
for label,paths in (('Source',source),('Evidence',evidence)):
    archive=out/(tag+'-'+label+'.zip');records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            assert p.is_file(),p
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
