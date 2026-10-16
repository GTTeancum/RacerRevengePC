# Gasgano / Sullust Windows comparison

Tested source `da7ca23fc1b53117345c190a9809366ab1a6eb3e`, tree `4452b7f51baea69405e955fbc2a8688ba9ce71b4`.

- Build: 2026-10-15 01:41:26.837781–01:43:56.003836 UTC, 358/358, exit 0.
- Visible smoke: 01:44:18.121879–01:45:11.549650 UTC, natural exit 0.
- Both supervisors confirmed cleanup and zero remaining owned processes.
- EXE SHA256: `635210bdc839328a8202bd2cd6ea717e0c745a49f26a39e721cdc00e448fb51b`.
- Evidence Library: `libfile_e848b3bfe9f4819180d6f7ea5854e4f8`; ZIP SHA256 `d47fd33c51116cad5ff3f77035514df7f951b3051fdeeda134127784fd94693c`.

Root inspected Main, Gasgano selection, Soro Suub Facility selection and actual native track images. Native EXE ran in the original asset directory beside SLUS; SLUS was data only. All eight racers used original AI, including followed Gasgano in slot zero. Human, controller, keyboard and host-driver counts were zero. The same disclosed process-local Gasgano test profile was used. This is not an earned unlock, full-race or full-roster navigation validation. Clegg preview remained disabled. No Linux test ran.

## Performance result

At 640x448, swap interval zero, muted output with logical audio, original fog/blur/native shadows and unchanged trace settings:

- Current: 43 GPU-completed actual window swaps / 5.0754524 seconds = **8.472151172179252 FPS**.
- Previous 602643e: 45 / 5.0164073 seconds = **8.970563454845463 FPS**.
- Observed FPS difference: approximately **−5.56%**. No speedup is demonstrated. One comparison does not establish causal regression attribution.

Both logs identify ATI Technologies Inc., AMD Radeon(TM) Graphics, GL 3.2.0 Core Profile Context 22.20.27.09.230330. This is reported renderer identity, not a hardware-performance diagnosis.

## Timing interpretation

The final current periodic phase window is approximately 5.00302 seconds, 37 renders and 148 physics steps, ending at tick 176. Its disjoint phase total is 4.9700294 seconds, leaving about 32.9906 ms unassigned (using the rounded reported interval).

- Physics: 10.349900 ms per step.
- Render preparation/submission: 14.860622 ms per frame.
- Audio commit: 1.000388 ms per step.
- Post-world service: 0.014387 ms per step.
- Completion: 2.7376519 seconds / 37 calls = 73.990592 ms per frame, maximum 85.6053 ms.

The completion scope includes history staging, existing glFinish, actual host swap and bookkeeping. It is not a GPU-only measurement. Its coarse thread-CPU samples do not resolve that attribution. Prior zero completion calls meant missing instrumentation, not free completion. Render subphases are nested and must not be added to top-level totals.

The FPS baseline spans ticks 156–328; this periodic phase window ends at 176. The prior final phase window ended at 304 while its baseline spanned 156–336. Neither periodic window is baseline-aligned, so their phase averages cannot explain the FPS difference. Next source work separates existing completion operations without adding synchronization; exact baseline-aligned accumulation remains a further instrumentation requirement.

## Follow-up boundaries

The bounded smoke passes, while performance remains poor. No failure-recovery exception is active. Next routine centralized test is no earlier than 2026-10-15 02:45:11.549650 UTC. Subsequent source changes require their own validation; this pass does not cover the later default-off Clegg transport or any pending instrumentation candidate.
