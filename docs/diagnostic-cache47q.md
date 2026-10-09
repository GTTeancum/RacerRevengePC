# 47Q: IOP diagnostic-setting cache, startup accepted

2026-09-30. Xbox-first goal active and incomplete.

47P's disassembly attributes 34 of 197 main-thread samples to getenv or its
string-length path. This justifies a targeted experiment, not a predicted FPS
gain. 47Q clones the accepted 47J2 IOP source and changes only literal RR_TRACE_
and RR_CAPTURE_ environment queries to an optional per-call-site owned cache.
The launch flag RR_CACHE_DIAGNOSTICS47Q=1 enables caching; 0 preserves getenv
on each call. Cached strings retain absent-versus-empty semantics and lifetime.
Values are captured on first use. The native process does not change these
settings during execution; changing them dynamically in a future host would
require explicit invalidation or the original-query mode.

Guest instructions, scheduler rules, hardware accesses, diagnostic payloads,
capture limits, audio mixing, input, renderer DLL, registry and leaf executor
are unchanged. The Windows reference source and accepted 47J2 clone remain
untouched. Separate object files for all 24 IOP implementation units prevent
mixing changed inline diagnostic helpers with old IOP definitions.

Both modes pass 372 existing IOP differential checkpoints with zero failures.
Both native VBlank-500 startups exactly match accepted 47N in EE/IOP RAM, PCM,
final image, reported state, stop and unchanged memory cards. Candidate SHA-256:
83021ddfbb4ea45c2a19e0b23c20e7851d0b60ca079f07fb668635b77ea668ce.

A full VBlank-7000 off/on race pair is running with one frozen executable and
GS profiling disabled. Neither full-race equivalence nor speedup is accepted
at this checkpoint. The pair will compare both results with accepted 47N and
report the same captured 6000–6900 VBlank window. It is a single pair, not a
repeat-confirmed gain, unique-picture FPS or an Xbox forecast.

Read-only CPU/IO observers attach to both runs. Separate 20-second PSS pilots
are scheduled after VBlank 5200 and before 5700, outside the timing window.
They retain last-syscall metadata to investigate the still-unattributed write
samples. They neither establish the write target nor change OS policies.
Any spill into the timing window must be reported rather than ignored.

Recovery: restore 47P and its dependency chain, overlay 47Q Source, run
Build-LaunchEnv47Q.ps1, validate Run-LaunchEnvStartup47Q.ps1, then run
Run-LaunchEnvRace47Q.ps1. Scripts preserve immutable outputs and will not silently
reuse existing acceptance directories. Inspect active process/session state
before launching duplicates. This archive includes only completed build/tests
and startups; active race evidence is explicitly excluded.

The broader goal remains open: large single-core gains, compact whole-game
execution, 64 MiB residency, full x86-32/SSE1 support, direct VU/NV2A rendering,
sky/surface correctness, a completed lap and actual Xbox boot/performance.
