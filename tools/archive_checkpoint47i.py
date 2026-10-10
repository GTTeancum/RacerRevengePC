"""Preserve the import-cache candidate and portable memory-version evidence."""
from pathlib import Path
import datetime
import hashlib
import json
import zipfile
root=Path(__file__).resolve().parents[1]
out=root/'outputs'
tag='RacerRevenge-Checkpoint47I-BoundedImportCache'
assert not list(out.glob(tag+'*'))
def sha(p):
    with p.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def files(d): return [p for p in (root/d).rglob('*') if p.is_file()]
for name in ('build','import-cache','differential','scheduler'):
    assert (root/f'evidence/47I/{name}.exit').read_text(encoding='utf-8-sig').strip()=='0'
regressions=json.loads((root/'evidence/47I/regressions/comparison.json').read_text())
assert all(r['matches_frozen45Y_via47B2'] and r['exit_matches'] for r in regressions)
binary=root/'work/native/ninja-build-release-opt/racer_iop47i_fixed_probe.exe'
doc=f'''# Checkpoint47I — bounded IOP import cache

2026-09-30. Root `D:/Programming/GitHub/RacerRevengePC`. Goal active/incomplete.
The 47I native executable is built but has NOT run the game. The isolated 47H
fixed-VBlank race comparison remains live in orchestrator exec session 93889.
Do not restart that experiment because a polling wait expires.

## Implementation

The original IOP import decoder searches backwards through up to 64 KiB of RAM
on every import. Candidate 47I caches successful results in 256 fixed slots.
It still checks the two stub instruction words, and reuses a result only while
every page read by the successful search is unchanged. The tracked interval
runs from the selected header through the current stub's second word. It also
covers closer headers and earlier terminators that could change the result.
Misses, malformed stubs, uncacheable ranges, and version saturation use the
original search. No guest instruction, cycle charge or library behavior changes.

The shared core adds `IopRamVersions` and `IopImportRangeVersion`. All actual
IOP RAM writes reach the existing ownership marker, now also updating touched
page versions. Reset versions the full RAM range. Audited direct device paths
include SPU DMA and SIO2 DMA; the public RAM span is const. Counter saturation
permanently disables reuse instead of permitting a stale-version match.

The x64 native cache array is 34,816 bytes, plus approximately 2 KiB of page
versions. This is a bounded allocation in the candidate, not total Xbox memory.
The full legacy IOP has not yet been compiled for Xbox. The portable page-version
component has been compiled with the Pentium III-compatible core.

## Evidence

- Windows x86, Windows x64 and nxdk-built CPU objects executed in the Windows
  x86 runner each pass **161,727 checks**, zero failures, digest `d74bf74c`.
  The diagnostic Xbox XBE builds; it has not been executed on Xbox.
- Decoder test: **9,360 comparisons**, zero mismatches, full 2 MiB RAM equality.
  It covers write8/write16/write32, writeRam, zeroRam, aliases, page crossings,
  nearer headers, earlier terminators, collisions, memory-only reset, SPU DMA,
  unrelated-page writes and repeated mutations. Core tests cover counter overflow.
  There are 6,488 cache hits; search-word probes are 154,525 uncached versus
  102,954 cached in this mutation-heavy test. These are not game FPS numbers.
- Four physical IRX cases pass **372** full state/RAM/PCM checkpoints with both
  earlier caches enabled, toggling only import caching. Signatures still match
  the frozen original cases. The scheduler stress test also passes unchanged.
- Both import-cache modes produce exactly the frozen 45Y broad regression logs
  via 47B2. Known sound RPC/PollEventFlag/DBCMAN failures remain, exits 1/1/0/1,
  compatibility 17/18. This is not an all-green legacy suite.

Candidate SHA-256: `{sha(binary)}`.
Build logs: `evidence/47I`; portable logs:
`evidence/windows-20260930-141043-347`, `evidence/xbox-20260930-141029-588`.
Portable builds ended at `evidence/47I/portable-end.txt`, before the 47H baseline
measurement window. Check new archive start/end timestamps against capture
brackets before accepting race timing; do not use whole-run wall averages.

## Continuation and recovery

The running 47H pair is `evidence/native-pair47H-20260930-135828-744`.
Its frozen executable hash is
`c77498e7d1824fc4c82ac3020c59f586b3bf32cc87470a5f7bc2a4d7d5ea34e7`.
Use `tools/compare_fixed47f.py` for its terminal RAM/state comparison.
`tools/analyze_fixed_pair47i.py` now takes explicit baseline/candidate directories
and the expected executable SHA-256. Its default window is 6,000–6,900, since a
run stopping exactly at 7,000 may have no final periodic capture at that boundary.
The analyzer separates live status, capture timing and fixed-stop validation;
timing alone never proves state or gameplay equivalence.

After the isolated pair finishes, `tools/Run-Native47I.ps1` can run the import
candidate with `-ImportCache 0` or `1`, both older caches on, and `-Vblank 500`.
Compare it against the original startup state and its uncached counterpart before
longer measurements. Do not run another game/GPU test during the current pair.

Apply this Source after 47H Source. It contains current shared source/tools/tests
and the complete prepared 47I IOP tree. Native generation/game recovery still
uses 46G and its transitive chain; 47A supplies the pinned nxdk/MSYS2 recovery.
Run the preserved `evidence/47I/build.cmd` with fresh logs to rebuild the native
candidate/tests. Preparers refuse existing destinations to preserve evidence.
All archive members are reopened and SHA-256 verified; exact dependencies are
pinned in the manifest. Original user-supplied PCSX2 and legacy sources stay intact.

Remaining blockers: corrected 47H race comparison, 47I native validation, much
smaller generated code, integrated portable EE/VU core, NV2A rendering, total
64 MiB memory fit, stable full-course input/audio/rendering and real Xbox timing.
No full lap or hardware FPS forecast is claimed.
'''
(out/(tag+'.md')).write_text(doc,encoding='utf-8')
source=[root/p for p in ('CMakeLists.txt','.gitignore','.gitattributes','README.md')]
for d in ('src','tests','platform','tools','docs'):
    source += [p for p in files(d) if p.suffix in {'.h','.cpp','.py','.ps1','.cmake','.md','.json'} or p.name=='Makefile']
source += files('build/iop47I')+[out/(tag+'.md')]
evidence=[]
for d in ('evidence/47I','evidence/windows-20260930-141043-347','evidence/xbox-20260930-141029-588'):
    evidence += files(d)
evidence += list((root/'work/native/ninja-build-release-opt').glob('racer_iop47i_*.exe'))
evidence += [root/p for p in ('build/windows-x86/racer_core_check.exe',
    'build/windows-x86/racer_xbox_objects_check.exe','build/windows-x64/racer_core_check.exe',
    'build/xbox-core/default.xbe','src/core/cycle_ledger.obj','src/core/compact_registry.obj',
    'tests/core_suite.obj','platform/xbox/main.obj','platform/xbox/main.exe')]
dependencies=[]
for suffix in ('Source','Evidence'):
    p=out/f'RacerRevenge-Checkpoint47H-FixedStateAndIopCache-{suffix}.zip'
    dependencies.append(dict(path=p.name,sha256=sha(p),bytes=p.stat().st_size))
manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
    goal_status='active/incomplete',dependencies=dependencies,game_candidate_executed=False,
    checks_per_portable_build=161727,portable_digest='d74bf74c',
    xbox_execution_verified=False, live_race_pair_session=93889)
for label,paths in (('Source',source),('Evidence',evidence)):
    archive=out/(tag+'-'+label+'.zip')
    records=[]
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(set(paths)):
            assert p.is_file(),p
            name=p.relative_to(root).as_posix(); digest=sha(p)
            z.write(p,name); assert sha(p)==digest,p
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
