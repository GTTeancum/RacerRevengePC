# Native Linux Rider observation, 2026-10-11 10:27 UTC

One centralized focused build and one bounded UI capture, against local
`03df3c864e572e3ef174ec16a920f583f4d9521d`, published as
`d65aa4086dd937a24574268caa0db945ed7d7b62`.

- Build passed: 3 compiled, 274 verified reused, link exit 0.
- Executable SHA256: `de524c2ed31fdd02fa870c371016436282cac6ed5d3dde27899394406b524a78`.
- Capture passed: `bounded_original_rider19_rejections_back_observed`.
- 217 matched captures, 217 preserved images, all recorder checks true.
- Main entered original Rider19; 106 captures show the fully open Rider panel.
- Three seconds of accepted source idle completed. Left, Right and Enter each
  returned the intended unavailable result without changing checked owners/profile.
- Back returned to Main slot1 with original Song7/Back5/Song9 transition receipts.
- Explicit quit and cleanup succeeded; native and recorder exits were zero.
- Genuine WAV finalized: 462084 stereo 48kHz frames, 9.62675 seconds.
- No driving: world ticks and window swaps were both zero.

## Renderer evidence

All 205 Rider native submissions selected coherent framebuffer fetch, succeeded,
and used zero texture barriers. Draw counts ranged from 139 to 149. Fully open
captures used 149 draws, 16196 source quads and 15784–15804 retained triangles.
Rider native submission measured median 10.468 ms, nearest-rank p95 24.244 ms,
maximum 178.087 ms. All 2089 stage receipts reported success. This is an observed
distribution from this run, not a like-for-like cross-run benchmark or pure GPU timing.

## Delivered evidence and limits

The delivered MP4 contains all 217 actual capture frames at their native PCM sample
boundaries, with the matching trimmed original UI audio. No interpolation or added
duplicate frames. Video duration 9.436458 seconds; original capture ACK cadence
22.469 Hz, maximum gap 315.530 ms. It is authentic native UI capture, not gameplay.

This run used headless Linux llvmpipe. It does not prove Windows behavior, physical
audio playback, audible quality, stable 30 fps, retail pixel parity, other riders,
or a menu-to-race path. Racer movement and race confirmation remain unavailable.

Retained evidence: `rider-validation-06` build and
`rider-capture-sequence/rider-validation-06` capture under the recovery workspace.
Next centralized check is no earlier than 2026-10-11 11:28:00.214413 UTC.
