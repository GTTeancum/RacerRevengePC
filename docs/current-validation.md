# Native validation history and current consolidation limits

Consolidated main remains unbuilt and untested. Its latest completed focused check is the October 11, 2026 10:27 UTC Rider19 menu observation on earlier source `03df3c8`, as summarized in the [README](../README.md). The gameplay receipts below apply only to their identified historical sources. They do not validate later UI/resource/lifetime changes, the branch consolidation, or full-course GC behavior.

At the time of the all-eight receipt, subsequent [batched observations](batched-racer-observation.md) and [fresh registered scalar reads](typed-racer-observation.md) had source review only. The bounded result below applies to prior source `d3d854b6`; it does not measure those optimizations.

## Historical all-eight original-AI run: 2026-10-10 01:21 UTC

The central slot beginning 2026-10-10 01:21:13 UTC built source `d3d854b67d3cbcae8bd640c85e2e9b6756bbea5f`: 28 units compiled, 165 reused, link exit 0. One direct-GC process ran from 01:22:26 to 01:22:42 UTC, reached 12.0007 seconds after ready including countdown, and exited normally. Bounded runtime acceptance passed, with no source boundary, observer error or forced cleanup.

All eight distinct original-AI owners, including camera slot 0, were observed. Every sampled native counter advanced from 1 to 206, and each AI dispatcher and genuine planner advanced. HOST driver calls, controller samples and keyboard packets stayed zero. All eight also recorded genuine brake commands and simultaneous native brake/steer bindings; these counters do not prove completion of the exact sampled physics frame.

All remained in startup state 4, with maximum observed native CB8 3.28 seconds. State 2, ordinary combat continuation, route-selection RNG draws and a completed lap were not observed. The later timing interval measured about 32.75 ms of main-thread physics CPU per 16 ms native step, explaining a major performance constraint. Rendering and other work add cost; the existing trace does not measure all audio work.

## Capture and audio limits

The run retained 88 fresh paired images, 81 distinct pixel images, at about 7.74 captured frames per second. Inspection of the actual capture found the source-normal racer lighting visible. This does not establish original shader parity or exhaustive visual correctness.

One final capture request remained unacknowledged, so `capture_complete` is false despite the bounded runtime pass. A separately verified, explicitly incomplete silent artifact retained the actual 88-frame, 11.2404-second prefix. Finalized native stereo PCM covers the image anchors, but its exact corresponding trim is 4.8884 seconds shorter than wall video; it was omitted without stretching or synthesis. No raw media or receipts are stored in Git.

Runtime executable SHA-256: `35d0589fa155f9229980189c78133f6d47aeef89d20730e2bb3435b8f1563210`.

No full GC lap, reset, finish, pause/restart, retail zero-human bootstrap, complete original-AI behavior, or original scheduler parity is established. See [combined scope](native-ai-lighting-integration.md). The earlier Clegg-only run and tested-main menu/race results apply to their own source versions. The subsequent batching and scalar-read changes have received source review only. No additional software check or encoding was performed for its publication.

## Earlier source-specific receipts

- [GC HOST mixer, 2026-10-09 23:20 UTC](history/gc-host-mixer-20261009-2320.md): source `53d44876270a095ac956720784437a16126ae584`; all eight cleared node 40, then stopped OffCourse at nodes 43/44. No completed GC lap; silent video with separately retained drifting PCM. This used the historical HOST control policy.
- [Clegg-only original AI, 2026-10-10 00:20 UTC](history/original-ai-first-slice-20261010-0020.md): source `93201627ee0b2afd834cb2ab51885fc55f749c6d`; only Clegg in bank slot 1 used original AI. Sampled state 4 then state 2, ten brake commands and simultaneous brake/steer bindings were observed. This does not establish state 2 for the later all-eight run, which observed only startup state 4.

These receipts preserve prior evidence; they are not approvals to run their historical mixed/HOST driving configurations. Current driving validation requires original in-game AI for every racer, including the camera-followed player. No new build, test, syntax check, runtime, capture or encoding was performed for this documentation reconciliation.
