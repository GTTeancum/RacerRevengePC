# 47R: diagnostic line buffering built; scheduler cost identified

2026-09-30. Goal active/incomplete. No new throughput or Xbox acceptance.

The 47Q control mode completes VBlank 7000 with exact saved-state equivalence
to accepted 47N. The cache-enabled race remains in progress. Its control timing
is potentially contaminated: a generated-source search overlapped the end of
the race window and was stopped. timing-contamination.json records the event.
Do not report the initial pair as a clean speedup. A fresh control is queued
after the pair and the short 47R startup checks; it must be inspected before
making any performance claim.

The completed 47Q control CPU/IO observer records 660,805 process writes over
638.828 seconds. Its capture-bounded race subset records 198,158 writes over
145.937 seconds (about 1,358 writes/s). These include all threads/destinations;
they do not by themselves identify stderr or measure blocked write time.

A second 20-second PSS pilot, before the race timing window, captured 197
snapshots without errors. Main-thread samples include 66 in ucrtbase, 58 in
ntdll and 55 in the executable. 51 hit the previously disassembled getenv scan
loop and 26 hit ZwWriteFile's syscall-return address. All last-syscall metadata
fields were zero/unavailable, so this experiment did not identify a destination.
The report is tied to the exact native executable/map and includes raw samples.

47R adds an optional bounded 4 KiB stderr stream buffer to the isolated probe.
It combines individual C++ insertions until newline, explicit flush or capacity,
then writes to the original stream. It retains text, newline and flush behavior;
no guest, scheduler, audio or renderer path is altered. Scope exit restores the
original stream, flags and tie. Normal completion fails with code 13 if flushing
failed. Mid-line process crashes can still lose an incomplete line: this is an
opt-in diagnostic performance experiment, not a change to the reference logger.

The same executable switches the behavior with RR_DIAGNOSTIC_LINES47R=0/1.
SHA-256: 44f5da294a40daf27cd7eff5163a6b1726795f6d550dfaed3cb86bea8033f6e8.
The native candidate built after the control passed VBlank 6950. A standalone
test verifies 1,000 fragmented records become 1,000 writes with exact bytes,
partial-line explicit flush, a 9,000-byte line, short downstream writes and
output failure. Native startup checks are queued behind the 47Q race pair;
native equivalence and buffering performance are not yet accepted.

## Larger execution work

The prior 47K profile records 110,575,616 scheduler dispatch iterations across
its nominal 900-VBlank race interval: roughly 122,862 per VBlank. At 733 MHz,
30 VBlanks/s permits only about 199 CPU cycles per such iteration, or 99 cycles
at 60 VBlanks/s, including all other work. These are approximate arithmetic
constraints using near-boundary captures, not an Xbox FPS forecast. Diagnostic
cleanup alone cannot establish stock-Xbox feasibility.

Current-source inspection corrects an earlier working assumption:
build/fixed47F/EeScheduler.cpp processDueDeadlines() does query steady_clock
under m_eventMutex on every invocation. processPendingEvents() calls it and
also acquires the event mutex twice, even when the queue is empty. A candidate
can skip this work when no deadline/event/timer/GS/VIF interrupt is pending,
while retaining IRQ level checks, asynchronous event visibility and preemption.
Such a candidate still requires dedicated boundary tests and full native replay.
It is not implemented or accepted by this checkpoint.

The generated code also splits some software floating-point helpers across
scheduler entries. For example FUN_0030f8d0 returns at a taken branch into
entry_0030f908. The latter accounts for 7.533% of sampled dispatch count in 47K,
despite only 0.176% of sampled phase time in its guest body. This is evidence
for investigating dispatch density, not permission to replace math semantics
or discard scheduled cycle charges.

## Recovery and remaining work

Restore 47Q and its dependency chain, then overlay this Source archive. Build
with Build-Diagnostic47R.ps1 (which waits out the named active timing window).
Run-DiagnosticStartup47R.ps1 waits for the 47Q full race pair before native
validation. Repeat-LaunchEnvControl47Q.ps1 then runs the clean control. Inspect
existing sessions before starting duplicates. Only completed control evidence,
build and byte-preservation tests are archived; active candidate/startup/repeat
jobs are excluded and are not accepted by this checkpoint.

Open: large single-core speed gains, full 64 MiB fit, compact whole-game code,
full x86-32/SSE1 runtime, VU/NV2A rendering, sky/surface correctness, completed
lap, stock-Xbox boot and measured Xbox performance.
