# 47P: completed race profiles and remaining attribution

2026-09-30. Goal active and incomplete.

Both the combined compact-registry 47N race and renderer-profile 47O race finish
at VBlank 7000 and exactly match their accepted references in full EE/IOP RAM,
PCM, final image, reported state, stop and unchanged memory cards. Both retain
1,954,337 compact-leaf calls with no fallback. These are deterministic boundary
checks, not a completed lap or an Xbox performance result.

The 47O renderer window from backend VSync 6000 through 6900 lasts 154.105868
host seconds. All nine measured DLL operations total 6.006640% of this interval:
packet submission 3.992948%, FIFO 1.475128%, flush 0.335153%, VSync 0.091929%,
VRAM 0.087197%, snapshot 0.024285%. Backend count reconciles with scheduler 7000.
These are elapsed call durations including waiting and preemption, not exclusive
CPU/GPU costs. Removing these measured calls entirely would only give about
1.064x on this observation; this is a scoped arithmetic ceiling, not a forecast
for removing the entire renderer or its unmeasured surrounding code.

Read-only CPU counters show one thread accounted for 94.404% of process CPU in
the capture-bounded 6042–6859 subset (145.920 wall seconds, 128.156 process CPU
seconds). The prior unprofiled 47N late-race observation similarly attributes
94.251% to one thread. Latest capture markers lag sampling; neither interval
is an exact 6000–6900 boundary.

A 20-second PSS thread-context pilot collected 197 snapshots without errors.
Capture calls took 150.903 ms total, maximum 1.176 ms. Its main thread has 62
IPs at ntdll ZwWriteFile+0x14 and 34 in getenv/string-length code. Disassembly
proves the exported getenv jumps to a wrapper that calls the observed environment
scan at RVA 0x31580. It must not be called strchr merely because that is the
nearest exported symbol. Frequent diagnostic getenv calls are present in the
IOP and VU paths. A launch-setting cache is a concrete next experiment.

The WriteFile samples are real syscall return addresses, but the write target
and cost are not yet attributed. Logs total only about 1.93 MB. Do not claim
logging is the dominant bottleneck from these samples. Sampling frequency is
wall-based, not CPU-weighted. The PSS observer overlaps the 47O race window;
47O is an instrumented diagnostic, not an accepted throughput comparison.

An isolated relink produced an exact symbol map. The entire .text section and
all virtual section layouts match the frozen sampled executable byte-for-byte;
the report preserves both executable hashes, map hash and section hash. Native
samples are distributed among guest routines, scheduling, IOP, registry and VU.
No single guest function is yet proven to dominate.

WPR CPU.light could not start: 0xc5585011, unable to enable the system profiling
policy. Status confirms WPR is not recording. No policy, privileges or elevation
were changed. PSS succeeded through process-local queries. Evidence includes
the failure, raw samples, module inventory, counters, disassemblies and analyses.

Recovery: overlay Checkpoint47P Source on Checkpoint47O and its dependency chain.
The original 47O executable remains frozen in each completed run. Its build-tree
copy was relinked only to obtain a map and has a different file hash; existing
47O runners require the old hash and should receive the frozen binary explicitly.
No behavior acceptance is claimed for the relinked copy independently of the
identical instruction-byte check. Never relax a runner hash gate silently.

Open: much larger CPU gains, compact whole-game code, total 64 MiB residency,
full x86-32/SSE1 runtime, VU/NV2A rendering, sky/surface correctness, completed lap,
and actual Xbox boot/performance. Windows image size still exceeds Xbox memory.
