# Native Windows track FPS baseline

Status: source-reviewed candidate only. Uncompiled and unexecuted. No FPS result,
Windows presentation, screenshot, or gameplay success is established by this patch.

`RR_TRACK_FPS_BASELINE=1` requests one short baseline per native race session.
The default is disabled. It requires Windows, the actual SDL window, eight real
original-AI owners including camera-followed slot 0, no human or host-course
driver, and the original camera/display path. Headless and private-render-only
paths cannot produce this receipt. It neither starts a race nor changes driving,
simulation, rendering, swap interval, source clocks, GPU completion or pacing.

## Measured quantity

The `track_fps_baseline` JSON line is emitted only after
`completeGameplayDisplay` has completed the genuine gameplay image and performed
its actual window swap call. Both `submittedWorkCompleted` and
`gameplayWindowSwapSubmitted` must be true. SDL's void swap API does not prove
physical monitor scanout, so the receipt calls these completed swap submissions.
No direct GPU-duration claim is made.

The first eligible actual swap anchors warmup. At the first later actual swap
at least five steady-clock seconds afterward, that entire warmup is excluded and
a measurement boundary is recorded. At the first actual swap at least another
five seconds afterward, FPS is the difference in actual swap counts divided by
the measured elapsed seconds. The boundary image itself is not counted again.
The final native tick must have advanced. This is frame cadence with existing
host pacing, audio, telemetry and native workload, not CPU loop throughput.

Menus, pause, focus loss outside explicit unattended mode, minimization, resize,
invalid dimensions, event-budget interruption, recovery, source display fades,
non-Racing phases, Results and capture requests discard unfinished intervals.
Changes to actual focus, dimensions or swap interval also require fresh warmup.
The Windows screenshot integration calls `trackBaseline.discard()` immediately
after every successful readback/write. Thus the next real swap anchors fresh
warmup after capture overhead, rather than subtracting guessed I/O durations.
One successful JSON receipt ends baseline collection for that session. Normal
session exit before completion emits `track_fps_baseline_unavailable` without a
numerical result. Exceptions retain their original failure path.

## Evidence and settings

The receipt includes elapsed nanoseconds, measured swap count, exact counter and
tick endpoints, excluded warmup duration/count, discarded interval count, track
code/archive identity, drawable resolution, queried swap interval, actual focus,
`focused_windows` or `unattended_windows`, racer/control provenance, native
shadow/blur/fog/culling settings, audio mode, MSAA request and fixed-step policy.
All eight owners are rechecked against actual native AI fields before emission.

Source revision is deliberately null because the existing build does not embed
it. Pair the line with the approved Windows build receipt, verified source
revision/input hashes, executable SHA256 and runtime log. An environment label
alone is not executable provenance. Existing `window_submit_fps` telemetry
remains unchanged and is not this baseline: it counts renderer commits, including
private underlays, and has different pause/warmup semantics.

## Integration contract

This candidate is paired with the Windows framebuffer capture patch. The native
session receives `windows_capture50LD::Run& captureRun`; the baseline observes
`captureRun.unattended()` without altering focus. That patch must call
`trackBaseline.discard()` after each successful window capture and before its
next gameplay iteration. Keep those hooks together when integrating. The
global finite capture deadline remains owned by the capture patch.

No build, test, helper, syntax check, lint, sanitizer, executable probe or runtime
was run to prepare or review this candidate. Only source files were read/edited.
