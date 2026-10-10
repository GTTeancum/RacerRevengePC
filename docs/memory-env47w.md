# 47W: measured diagnostic cache built and accepted at startup

2026-09-30. Goal active/incomplete. Full cache race comparison is active;
no cache throughput gain or Xbox performance is accepted by this checkpoint.

## Completed capture-off result

47V disabled VU capture explicitly, using the unchanged 47T executable. Both
startup and VBlank7000 saved EE/IOP memory, reported state, audio, final image
and unchanged cards match the accepted reference. The matched 6000–6900 race
window records 10.050989 versus 11.292220 emulated VBlanks/s, 1.123493x. This
is one comparison with capture-bracket uncertainty, not distinct-picture FPS.
The control is the prior 47T-on run; there was no competing build or archive in
the candidate's fixed timing window. The earlier combined scheduler/buffering
pair measured 1.740681x; do not present separate measurements as one combined
controlled experiment.

Capture-off run: work/native-run47v-1-1-1-1-0-1-1-1-1-20260930-180133-410.
Evidence: evidence/captureOff47V-20260930-180114-129. Its executable remains
53762c35cb847cb30d7c5b2e6abc0f1636959db859652a4e9f39702b650231cb.

## Targeted cache

Full 47U attribution recorded 68,347,425 getenv calls, zero table overflow,
including 29,202,212 VIF-unpack trace checks and 13,368,503 IPU-stall trace checks.
47W caches 13 literal diagnostic call sites across memory/VIF implementation,
IPU helpers and the VU capture header. Each enabled call site owns an optional
string initialized on first use; absent values remain absent. Only launch-fixed
RR_TRACE_/RR_CAPTURE_ settings are affected. RR_CACHE_MEMORY_DIAGNOSTICS47W=0
retains original live getenv calls. Other settings and dynamic arm-file checks
are untouched. The host must establish these diagnostics before launching the
process; changing them inside an already-running process is outside this mode.

The reference source remains unchanged. Isolated copies live in build/memory47W.
Three replacement objects (memory, VIF interpreter, VU interpreter) are compiled.
The VU implementation itself is byte-identical to the original; its included
capture header changes only the setting lookups. Link-map validation confirms
all three replacements are present and all three original archive members absent.
Original/candidate hashes of all 114 copied files and changed site names are saved.

Build: evidence/memoryEnv47W-20260930-180826-424. Executable SHA-256:
f6da2e716339a2e3a7bfb2563f389c6efa9b9c97c12d803da966ef84c4eb6724.
Map SHA-256: 91eba9521f5c3c0254d4a0001845f94775e17fd5aa736feb5876b07287711a87.
Standalone off/on tests cover owned values and absent settings across 100 CRT
storage mutations. Both pass. Native off/on VBlank500 startup also matches the
accepted capture-off reference in saved state and media.

Startup runs under work/:
- native-run47w-0-1-1-1-1-0-1-1-1-1-20260930-180901-953
- native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-180922-153

Full off/on runs use the frozen executable in evidence/47W/bin, with VU capture
disabled and existing scheduler/buffering/IOP caches enabled. Each requires
the original saved-state comparison plus explicit audio/image equality before
timing analysis. CPU/IO observers run without injection or suspension. Active
full runs/observers are excluded from this checkpoint.

## Recovery and remaining work

Restore 47V and its dependency chain, then overlay this Source archive. The
completed 47V race, 47W build/tests/map, startup reports and startup runs are in
Evidence. Revalidate evidence/47W/native.exit and live processes before rerunning
Run-MemoryEnv47W.ps1 or its observer. Build-MemoryEnv47W.ps1 deliberately waits
for accepted 47V; the recorded build has already completed.

This remains an experiment in removing host diagnostic overhead. Whole-game
code still exceeds the Xbox memory budget; full 32-bit/P3 runtime, NV2A renderer,
rendering validation, stable playable laps and stock-Xbox boot/FPS remain open.
The timer-polling analysis in docs/polling-loop47u.md is an additional execution
lead, not an implemented acceleration or permission to skip guest side effects.
