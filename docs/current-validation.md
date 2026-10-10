# Current native race validation

The current combined all-eight AI/dynamic-contact/lighting source is **uncompiled and untested**. The result below belongs to the earlier guarded Clegg-only source. See [combined candidate](native-ai-lighting-integration.md).

## Bounded original-AI first slice passed

The central slot beginning 2026-10-10 00:20:53 UTC built source `93201627ee0b2afd834cb2ab51885fc55f749c6d`: 34 units compiled, 159 reused, link exit 0. One direct-GC process ran from 00:22:05 to 00:22:21 UTC, exited normally, and reached the 12-second observation window after ready, including countdown. No source boundary, observer error or forced cleanup occurred.

**Only Clegg in bank slot 1 used original AI.** The human and six other racers retained existing HOST controls. All eight distinct racers were active, and each native counter advanced from 78 to 342 between status samples. This is not all-eight original-AI validation.

- The actual `vehicle58=1` racer was observed in startup state 4 at native CB8 1.232, then state 2 at CB8 5.45599, with source boundary 0.
- The later sample recorded 313 startup and 28 ordinary planner updates, ten genuine brake commands, and ten successful simultaneous native brake/steer bindings. The sampled instantaneous brake was zero; the counters establish prior binding, not completion of that exact physics frame.
- The take retained 116 fresh paired images, 107 distinct pixel images, and finalized 48 kHz stereo PCM totaling 10.064 seconds. Image observations covered 11.5276 seconds at about 9.98 captured fps. The bounded milestone and capture reports passed.
- A separately verified silent export retained a contiguous 115-frame, 11.2097-second prefix. Actual PCM diverged from wall video and was omitted without stretching, resampling or synthesis. Raw media and receipts remain outside Git.

Runtime executable SHA-256: `5406d7bdd37ba97b61f96b3643772e2b61f286ded283e76dff2adf83ca60c691`.

Five-second telemetry does not expose cold state 0, the exact state-transition instant, or AI-specific terrain support. Planner/binding counts are not complete-frame evidence. No lap, full GC world, combat continuation, actual AI contact ownership, failed-frame guard trigger, restart, pause or complete original-AI behavior was established. See [first-slice scope](original-ai-first-slice50iv.md) and [guard scope](original-ai-first-slice-guards50iv.md).

## Next source scope

The combined candidate now includes all-eight original-AI ownership, dynamic contacts and source-normal racer lighting in source. It has not been compiled or run. The previous Clegg-only result does not validate it. No additional build, runtime, syntax check or encoding was performed for this documentation update.

## Earlier independent coverage

The 17:18 UTC Linux source `959d94e` passed actual Clegg menu selection, one expected cold-bank change, pause/resume with frozen native/audio clocks, a 20-second capture, Main return and normal quit. The earlier mode1 source `0c3810c` completed three laps with native lap timing, Dax, last-lap/finish music and completed-lap display. These results apply to their own source versions and do not establish current all-racer AI, full-course GC driving, general controller usability or original scheduler/console parity.
