# 47T: scheduler and diagnostic candidates pass startup

2026-09-30. Goal active/incomplete. Full combined race comparison is running;
this checkpoint accepts startup and the completed tests listed below only.

## Completed cache experiment

47Q both modes completed VBlank 7000 with exact EE memory, IOP memory, audio,
final image, cycle counts and captured IOP state matching accepted 47N.
The initial control timing was contaminated by a generated-source search.
A fresh control, native-run47q-0-0-1-1-1-1-20260930-171244-067, also matched
the accepted candidate native-run47q-1-0-1-1-1-1-20260930-170018-199.
The clean comparison recorded 5.785174 versus 5.895108 emulated VBlanks/s
over the interpolated 6000–6900 interval: about 1.9%, with overlapping capture
bounds and only one clean pair. This is not distinct rendered FPS or an Xbox
forecast, and falls far short of the requested large performance improvement.
PSS pilots still found frequent environment-scan addresses after IOP caching.

## Diagnostic output and scheduler

47R line-buffer tests preserved bytes across fragmented records, explicit
flush, long lines, short downstream writes and output failure. Both native
VBlank-500 startup modes matched accepted 47Q exactly. The optional 4 KiB
buffer can lose a partial line on a mid-line crash; the reference is retained.

47S skipped empty scheduler processing while retaining cycle accounting,
pending preemption, asynchronous posts and GS/VIF level checks. Its real
scheduler fixture passed. The original gate was withdrawn while investigating
a possible zero-deadline ambiguity. That suspected public-API bug was NOT
reproduced: setAlarm normalizes zero ticks to one. The first expanded test
driver failed because it expected a divergence that did not occur. Its logs,
binary, original source, and the corrected interpretation are preserved.

47S2 conservatively always uses original processing for a zero deadline marker.
Expanded fixtures cover two same-deadline alarms (requested ticks 0 and 1),
GS and VIF interrupts, asynchronous stop, 2000 callbacks, exact ordering and
528032 EE cycles. Original and candidate traces match. Candidate counters are
4008 fast / 8 full calls versus 0 fast / 4016 full. Both native VBlank-500
startup modes also match accepted 47Q. No standalone S2 race speed is claimed.

47T combines unchanged 47R probe buffering and unchanged 47S2 scheduler,
retaining IOP cache47Q, compact registry47N, leaves47M and renderer47L/47O.
Both startup modes match accepted 47Q. Exact executable SHA-256:
53762c35cb847cb30d7c5b2e6abc0f1636959db859652a4e9f39702b650231cb.

The full race pair uses this same frozen executable with buffering/event gate
off/off and on/on; IOP caching stays on in both. Evidence directory:
evidence/combined-race47T-20260930-173405-793. That active pair and its observers
are excluded from this checkpoint. Do not rerun it without inspecting status.

## Recovery and blockers

Restore 47R and its dependency chain, then overlay this Source archive.
Completed build directories and startup runs are included in Evidence.
Build-Combined47T.ps1 composes the generated candidate; Run-CombinedRace47T.ps1
checks startup acceptance, freezes the executable and DLLs, runs both modes,
compares each saved state and writes the fixed-window timing report.

The 47K dispatch profile implies approximately 122862 dispatches per VBlank.
At 733 MHz and 30 VBlanks/s, about 199 CPU cycles per dispatch would have to
cover ALL work; this is a budget constraint, not measured Xbox performance.
Whole-game generated code remains too large for 64 MiB, the current Windows
renderer uses PCSX2, and full SSE1/x86-32, NV2A rendering, sky/surface repairs,
a completed lap and a stock-Xbox boot remain unverified. Startup equivalence
does not establish full-race correctness or stable gameplay.
