# Host course-check driver and measured live drive 50GL

This is a host-authored testing aid, not the original game's AI profile/controller.
The actual Pod01 profile exists, but its missing native driving owner/planner and
controller are documented in host-course-driver50gk.md. The test driver reads the
real ATA route, ordered Next0 links, widths and SpeedLimit entries. It supplies
normal twin-throttle packet bytes through the existing controller ingress and
native physics. It cannot write poses, contacts, progression or lap counters.

Enable with RR_AUTODRIVE=1 (or the packaged launcher --autodrive). Default is off.
It updates once per actual 16 ms physics step. The default host steering polarity
+1 was verified in the live drive; RR_AUTODRIVE_POLARITY accepts only 1 or -1.
W/S/Up/Down input disables auto immediately. Stuck/off-course stops issue neutral
input and report status, never a reset. The development checkpoint mechanisms
remain separate. Manual brake takeover was exercised after the autonomous lap.

RR_TRACE_PHASES=1 (packaged launcher --trace-phases) enables fixed per-phase wall
and thread-CPU counters. These clocks do not drive simulation. Disabled scopes
skip clock reads. See frame-phase-timing50gk.md for interpretation limits.

## Actual Linux VM result

Binary SHA-256: 7d6301bcfc12b82d2524fa031a05a9b672e1cb3e9fc77634e23329bdd57e290e
One continuous actual drive reached HOST LAPS 1 with zero manual/automatic
checkpoint resets and no course-driver stop. It traversed nine main and nine
secondary portals, with 111 map / 115 AI sequential advances by clean quit.
Manual brake input then disabled the driver and stopped the vehicle.
19,083 successful physics steps and 18,574 committed renders completed; the
successful-physics-step C++ allocation guard reported no failure.

All completed-FPS samples span 43.3219–62.7452. Observed per-operation maxima:
physics 32.144 ms wall / 32.138 ms calling-thread CPU; render preparation/submission
166.925 / 166.897 ms; completion wait 371.482 / 12.807 ms. These are observations
from this drive, not an attribution of the earlier severe stall. GPU/driver worker
CPU is not included in the calling-thread counter.

The earlier manual-drive sub-1-FPS stall did not reproduce on this different,
automatically steered route. It remains unresolved. No controlled speedup claim,
Windows run, full original AI, wrong-way/race-finish/ranking or audio playback
claim is made. HOST LAPS retains the scoped mode-2 semantics documented in
live-laps50gj.md. JPEGs are actual committed frames from this run.
