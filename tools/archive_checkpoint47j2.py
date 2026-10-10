"""Archive verified race-state equivalence, compact ownership and x86 IOP checks."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile
root=Path(__file__).resolve().parents[1]
out=root/'outputs';tag='RacerRevenge-Checkpoint47J2-VerifiedRaceAndCompactIop'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def files(d):return [p for p in (root/d).rglob('*') if p.is_file()]
def jsonfile(p):return json.loads((root/p).read_text(encoding='utf-8-sig'))
for report in ('race-state-final','import-off-vblank500','import-on-vblank500','ownership-vblank500'):
    assert jsonfile(f'evidence/47J2/{report}.json')['saved_state_equivalent']
for check in ('build','build-lifetime','memory-final','scheduler-final','differential','import-cache'):
    assert (root/f'evidence/47J2/{check}.exit').read_text(encoding='utf-8-sig').strip()=='0'
x86='evidence/iop-x86-47J2-20260930-143607-397'
assert (root/x86/'checks.exit').read_text(encoding='utf-8-sig').strip()=='0'
timing=jsonfile('evidence/47J2/race-timing-final.json')
assert timing['terminal_pair_verified'] and timing['same_binary_bridge_input_cards']
closed=[
 'work/native-run47h-0-0-20260930-135829-565',
 'work/native-run47h-1-1-20260930-142343-505',
 'work/native-run47i-0-1-1-20260930-143709-530',
 'work/native-run47i-1-1-1-20260930-143741-749',
 'work/native-run47j2-1-1-1-20260930-143813-579']
for run in closed:
    assert (root/run/'native.exit').read_text(encoding='utf-8-sig').strip()=='0'
binary=root/'work/native/ninja-build-release-opt/racer_iop47j2_fixed_probe.exe'
validated=[root/p for p in ('src/core/byte_ownership.h','src/core/iop_ram_versions.h',
 'build/iop47J2/src/emulator/core/iop_memory.cpp','build/iop47J2/src/emulator/core/iop_memory.h')]
(root/'evidence/47J2/validated-source.json').write_text(json.dumps(dict(
    native_binary_sha256=sha(binary),sources=[dict(path=str(p.relative_to(root)),sha256=sha(p)) for p in validated],
    acceptance='Original-memory differential and native VBlank 500 pass; longer 47J2 race pending'),indent=2))
doc=f'''# Checkpoint47J2 — verified race gain and compact IOP

2026-09-30. Root `D:/Programming/GitHub/RacerRevengePC`. Goal active/incomplete.
All runs archived here are terminal. No game process is needed to restore this
checkpoint. Xbox execution, total memory fit and whole-race acceptance are open.

## Measured race result

The corrected 47H idle/ready caches produce **3.135670 -> 5.347029 emulated
VBlanks per host second**, ratio **1.705227**, in the same 6,000–6,900 race window.
Ten-second capture-bracket bounds are 2.99031–3.20453 baseline and 5.26273–5.95975
optimized. This measures guest throughput, not every distinct rendered picture.
The gain is real for this tested section and still far below the overall target.

At the exact VBlank-7,000 stop, both runs match all 32 MiB EE RAM, 2 MiB IOP RAM,
22,631,340 PCM bytes, final PPM picture, reported thread/CPU/device state and the
terminal EE cycle 34,407,093,000. IOP cycles are 4,332,683,824; instructions
494,741,600. The same frozen executable, bridge, input and empty cards were used.
The state-comparison and timing reports remain separate: their conjunction
supports this result, not a blanket proof of every hidden CPU/GS state or a lap.

47H probe hash: `c77498e7d1824fc4c82ac3020c59f586b3bf32cc87470a5f7bc2a4d7d5ea34e7`.
Full closed baseline and optimized runs, captures, VRAM, RAM, audio, copied
binaries/resources, inputs and logs are included. Previous session 93889 ended
successfully; do not poll or restart it. The older faulty 47B2 39% result stays
rejected. Its idle-proof mutation defect was corrected in 47H.

Builds were gated after baseline capture 6,951. Optimized-run builds and checks
finished before its 6,000 bracket; x86 validation started after its final bracket.
Exact gates/timestamps are retained in 47J/47J2 evidence. Whole-run wall times
include early/post-window activity and must not replace the selected window.

## Ownership representation and failure retained

`ByteOwnership` stores one bit per guest byte. The IOP backing allocation falls
from **2,097,152 to 262,144 bytes**, saving **1,835,008 bytes (1.75 MiB)**.
Marking, allocation, free, reset, aliases and DMA preserve original semantics.
The native memory owner is noncopyable/nonmovable because devices and its bitmap
view retain references. This guard compiles in both x64 and x86 builds.

The initial 47J candidate missed freeAllocation's byte-indexed clear. Its
original-memory comparison failed (exit 6) and scheduler process crashed with
`0xc0000005`. It is rejected, not a usable checkpoint executable. Both original
source/binaries and failure logs are preserved. 47J2 clears the freed bit range.
A preparer assertion also initially expected four backing-vector references
instead of the audited three; the partial candidate was explicitly verified
before resuming preparation. No rejected candidate was run in the game.

The correction passes **2,226,205** queries against the actual preserved original
memory implementation, including every IOP byte, and full RAM equality. The
shared Windows x86/x64 and nxdk-object suite passes **464,977 checks** per build,
digest `81029ff8`. The earlier 464,971 result lacked free-range tests and is
superseded. The diagnostic Xbox XBE builds but has not been booted.

## Full IOP x86 and native integration

All 24 IOP implementation files now also build under Windows x86 with
`/arch:IA32`. Original-memory, scheduler, import-cache and physical-module
differentials match x64, except expected cache layout size. The x86 import cache
array occupies 29,696 bytes versus 34,816 on x64. Its page versions add about
2 KiB. An object audit covers 197,350 decoded instructions and finds no forbidden
post-Pentium-III instructions. Scope excludes CRT/DLL routines and Xbox linking.

Both cache modes still match frozen 45Y broad regression output byte for byte.
Known sound RPC payload, PollEventFlag and DBCMAN failures remain: exits 1/1/0/1,
compatibility 17/18. This is not an all-green legacy suite or verified sound fidelity.

Native VBlank-500 runs form an exact comparison chain: 47H -> 47I import cache
off -> cache on -> 47J2 compact ownership. RAM, PCM, final picture and reported
terminal state all match. Startup cache statistics: 121,288 probes, 86,463 hits.
Short-run wall timings overlap independent validation and are not FPS results.
47J2 probe SHA-256: `{sha(binary)}`.

## Recovery and next work

Apply this Source after 47I Source and its pinned 47H/47E/47D/47B dependencies.
The full legacy game/generation chain still starts with 46G and its dependencies;
47A contains nxdk/MSYS2 recovery. This source bundle includes current shared
source/tools, rejected 47J and corrected 47J2 IOP trees, and renamed original
memory source used by the differential test. Original PCSX2/reference files remain
untouched. All archive members are reopened and SHA-256 verified.

Use `evidence/47J2/build.cmd` to rebuild corrected native targets with fresh logs;
`tools/Build-IopX86-47J2.ps1` builds the full 32-bit IOP checks. Core builds remain
`Build-Xbox.ps1` then `Build-Windows.ps1 -Architecture both -CheckXboxObjects`.
Preparers refuse completed candidates. Initial failure files are evidence only.

Next: run the fully optimized 47J2 candidate through VBlank 7,000, compare its
saved state to the accepted 47H reference, and collect host profiling to locate
the larger remaining costs. Profiling-on wall rates are not a matched speed test.
The import/ownership changes have only startup game validation so far. The large
generated code, portable EE/VU integration, NV2A rendering, total 64 MiB fit,
normal full-course controls/audio/rendering and actual Xbox timing remain open.
The latest verified picture is lap 1/3 at 00:29.35 near a canyon wall, speed 9.
It does not establish a completed lap or stable rendering throughout the track.
'''
(out/(tag+'.md')).write_text(doc,encoding='utf-8')
source=[root/p for p in ('CMakeLists.txt','.gitignore','.gitattributes','README.md')]
for d in ('src','tests','platform','tools','docs'):
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += files('build/iop47J')+files('build/iop47J2')+[out/(tag+'.md')]
evidence=[]
for d in ('evidence/47J','evidence/47J2',x86,
          'evidence/windows-20260930-142406-203','evidence/xbox-20260930-142402-437',
          'evidence/windows-20260930-142832-728','evidence/xbox-20260930-142829-841',
          'evidence/native-pair47H-20260930-135828-744'):
    evidence+=files(d)
for d in closed:evidence+=files(d)
for pattern in ('racer_iop47j_*.exe','racer_iop47j2_*.exe'):
    evidence+=list((root/'work/native/ninja-build-release-opt').glob(pattern))
evidence+=list((root/'build/iop47J2-x86').glob('racer_iop47j2_*.exe'))
evidence+=list((root/'build/iop47J2-x86/CMakeFiles/rr_iop47j2.dir').rglob('*.obj'))
evidence += [root/p for p in ('build/windows-x86/racer_core_check.exe',
 'build/windows-x86/racer_xbox_objects_check.exe','build/windows-x64/racer_core_check.exe',
 'build/xbox-core/default.xbe','src/core/cycle_ledger.obj','src/core/compact_registry.obj',
 'tests/core_suite.obj','platform/xbox/main.obj','platform/xbox/main.exe')]
dependencies=[]
for suffix in ('Source','Evidence'):
    p=out/f'RacerRevenge-Checkpoint47I-BoundedImportCache-{suffix}.zip'
    dependencies.append(dict(path=p.name,sha256=sha(p),bytes=p.stat().st_size))
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
 goal_status='active/incomplete',dependencies=dependencies,closed_runs=closed,
 verified47H_race_throughput_ratio=timing['throughput_ratio'],verified47H_final_vblank=7000,
 verified47J2_final_vblank=500,ownership_saved_bytes=1835008,
 portable_checks=464977,portable_digest='81029ff8',x86_iop_objects=24,
 xbox_execution_verified=False,completed_lap_verified=False)
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
