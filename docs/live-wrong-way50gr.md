# Live wrong-way join 50GR

The player now executes source 1A2E30 after navigation and before undertrack
processing. Navigation consumes the previous update's actual flags10CC bit1;
this update then writes the current direction flag and any source FF8 update.
The input is the actual main body-scene LOCAL forward basis and actual map
C8C/C88 positions. Existing applicationByte58 and player state40 supply their
source-owned gates. Application34C is explicitly zero in this development host.
The original optional HUD owner is absent; the separate HOST overlay observes
the resulting flag without pretending to execute original HUD callbacks.

The source's own 13C770 polynomial, zero-vector behavior and branch order are
ported in 50GO. The HOST panel shows red WRONG WAY in place of HOST LAPS while
the flag is set, retaining the completed count. It uses bounded cold576-quad
storage (535 worst case), with no additional HUD draw or hot allocation.
Full race-finish/ranking/records and audio playback remain outside this build.

Fifteen focused checks on actual TA metadata passed, covering forward/reverse,
state/config gates, marker-fill bypass, unrelated-bit preservation and zero hot
C++ allocations. A real8.861-second Linux drive using only throttle/brake input
triggered the warning. Automatic first-flag capture recorded wrong_way=1 at
committed tick439 and the actual JPEG visibly shows WRONG WAY. Exit0;445 physics
steps and442 renders. Its sole completed5-second FPS sample was60.5598; this is
not a long-run performance claim.

Earlier hand-timed capture attempts in solo-wrongway50go missed the visible
warning and included normal damage-recovery and restart events. They remain in
evidence. That longer run also reproduced slow rendering, including a3.72886FPS
interval: render prep/submission2.5568s plus completion wait2.0500s of5.0954s,
versus physics0.4461s. The cause is not fixed. New render diagnostics are separate
work; no physics FPU mode or geometry has been altered to hide this issue.

RR_CAPTURE_WRONG_WAY=1 is an optional headless capture aid, disabled by default.
It requests one committed frame at the first current warning flag and records
both that flag and the committed tick. It does not change simulation inputs,
poses, navigation, damage or race state.
