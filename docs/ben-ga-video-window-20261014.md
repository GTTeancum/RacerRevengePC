# Ben Quadrinaros / Watchtower requested video

The original menu evidence route `main-ben02-course-ga` selects real rider 2
(Ben Quadrinaros), preserving source roster order, actual readiness, and the
same three original Course Next events to Watchtower. The selected racer uses
its authenticated five-model rig: four engines and chariot. Every racer remains
on original AI; no driving commands or substitute course driver are permitted.

For the requested 60-second video, `RR_WINDOW_VIDEO_HOLD_SECONDS=75` keeps the
ordinary race loop advancing for 75 wall seconds after the completed baseline
receipt. It does not record video itself, alter clocks, force focus, or change
AI. The exact opt-in is restricted to the Windows unattended path. Normal
smokes retain their existing exit condition. Early exit before the hold finishes
is reported incomplete. The application-wide 300-second deadline remains.

The central Windows recorder must start a real-window recording promptly after
`track-fps-baseline.json` is published, capture 60 seconds at actual speed, and
retain timing/source/process receipts. The 15-second margin accommodates recorder
startup; if startup exceeds that margin or the game ends early, do not claim a
complete clip. Capture only the game window, excluding unrelated desktop content.
The baseline precedes recording so recorder overhead does not contaminate it.
Inspect the resulting video and actual source frames before delivering it. Do
not pad a shortened clip, interpolate gameplay, or replace it with diagnostics.

The requested recording replaces the next routine smoke; it does not authorize
an extra routine take.

## Verified Windows recovery result

Source `1791f95910c64e11af8707029756f25bdee1f3ba`, tree
`8cfb47e88f8690cd2e58b21881f83b489dc0522d`, built successfully on
2026-10-14 at 13:04–13:06 UTC. The first route failed because the selected
POD02 supplemental runtime preparation was absent. Genuine five-model POD02
preparation resolved this without a source or executable change.

The recovery ran visibly from the original disc folder at
13:17:39.668507–13:19:41.246790 UTC, exited 0 and left zero owned processes.
Main, ready Ben selection, Watchtower Course and track screenshots were
inspected. All eight racers used original AI, with zero human, keyboard,
controller or host-driver samples. Baseline before recording: 13.3803335857 FPS,
67 GPU-completed swaps over 5.007349 seconds at 640×448.

The silent window-only recording contains 60.00 wall seconds at actual speed;
recorder interval 13:18:26.080207–13:19:26.409626 UTC. Unused black recorder
canvas from DPI scaling was removed, retaining the entire 640×448 game image
and timeline. No interpolation, padding or speed adjustment was used. The
original recording is retained with the crop and process receipts. First,
middle and last frames were visually inspected before delivery. This is not
a full-race test, 60 simulation seconds, or hardware pixel-parity claim.

Evidence ZIP: `libfile_9275f65259fc81919895418d3545cfc2`.
Delivered MP4: `libfile_e3d0cca0c3d88191a1a6feddac1ce938`, SHA-256
`8112b72a0ae4d1dc0e9ac40e7dd6acf94155b078d318d6f129f394ac193a8cca`.
Next routine test eligibility: 2026-10-14 14:19:42 UTC.
