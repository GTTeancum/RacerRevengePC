# Current native race validation

Current source adds the [batched observation optimization](batched-racer-observation.md), which is uncompiled and untested. The bounded result below applies to unchanged prior source `d3d854b6`; it does not measure the new optimization.

## Bounded all-eight original-AI run passed

The central slot beginning 2026-10-10 01:21:13 UTC built source `d3d854b67d3cbcae8bd640c85e2e9b6756bbea5f`: 28 units compiled, 165 reused, link exit 0. One direct-GC process ran from 01:22:26 to 01:22:42 UTC, reached 12.0007 seconds after ready including countdown, and exited normally. Bounded runtime acceptance passed, with no source boundary, observer error or forced cleanup.

All eight distinct original-AI owners, including camera slot 0, were observed. Every sampled native counter advanced from 1 to 206, and each AI dispatcher and genuine planner advanced. HOST driver calls, controller samples and keyboard packets stayed zero. All eight also recorded genuine brake commands and simultaneous native brake/steer bindings; these counters do not prove completion of the exact sampled physics frame.

All remained in startup state 4, with maximum observed native CB8 3.28 seconds. State 2, ordinary combat continuation, route-selection RNG draws and a completed lap were not observed. The later timing interval measured about 32.75 ms of main-thread physics CPU per 16 ms native step, explaining a major performance constraint. Rendering and other work add cost; the existing trace does not measure all audio work.

## Capture and audio limits

The run retained 88 fresh paired images, 81 distinct pixel images, at about 7.74 captured frames per second. Inspection of the actual capture found the source-normal racer lighting visible. This does not establish original shader parity or exhaustive visual correctness.

One final capture request remained unacknowledged, so `capture_complete` is false despite the bounded runtime pass. A separately verified, explicitly incomplete silent artifact retained the actual 88-frame, 11.2404-second prefix. Finalized native stereo PCM covers the image anchors, but its exact corresponding trim is 4.8884 seconds shorter than wall video; it was omitted without stretching or synthesis. No raw media or receipts are stored in Git.

Runtime executable SHA-256: `35d0589fa155f9229980189c78133f6d47aeef89d20730e2bb3435b8f1563210`.

No full GC lap, reset, finish, pause/restart, retail zero-human bootstrap, complete original-AI behavior, or original scheduler parity is established. See [combined scope](native-ai-lighting-integration.md). The earlier Clegg-only run and tested-main menu/race results apply to their own source versions. The subsequent batching change has received source review only. No additional software check or encoding was performed for its publication.
