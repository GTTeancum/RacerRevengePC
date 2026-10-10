# All-racer original AI and racer lighting candidate

Status: the current source includes reviewed [batched observations](batched-racer-observation.md) and [fresh scalar reads](typed-racer-observation.md), uncompiled and untested. Its main SHA-256 is `e53c563303f311aded314f6342254d8c5e444f52e0a8887405d068ccb896ed02`. No speedup has been measured. The preceding Linux source `d3d854b67d3cbcae8bd640c85e2e9b6756bbea5f` passed one bounded all-eight original-AI GC run in the central 2026-10-10 01:21:13 UTC slot. All eight real AI owners, native ticks and planner dispatches advanced with zero HOST/controller/keyboard input. The actual capture showed source-normal racer lighting. Full-course behavior remains incomplete.

Before the observation optimizations, the combined AI, lighting and route-choice receipts covered 32 unique source paths. That tested main SHA-256 was `f18e369d8c436cf3c94ee66539a698966a648314ca1572150d2be9e64fda202f`. The build compiled 28 units and reused 165, then linked successfully. One process exited normally after the 12-second ready window. Every racer remained in startup state 4, with CB8 at most 3.28 seconds; no lap or ordinary-state continuation was observed. Later physics timing averaged about 32.75 ms of main-thread CPU per 16 ms native step.

The 88 fresh paired images form an incomplete prefix because one final request remained unacknowledged. The separately verified prefix artifact is silent: corresponding native PCM differs from wall duration by 4.888 seconds. Bounded runtime acceptance passed; complete capture and real-time performance did not. See [current validation](current-validation.md) for the exact scope.

## Intended source behavior

`RR_ORIGINAL_AI_ALL_RACERS=1` admits a native eight-AI, zero-human roster including the camera-followed slot. It implies the original-AI and dynamic-contact joins; direct GC, incomplete-world opt-in, mode2 host scheduling, menus off, contacts and actual audio remain required. `RR_AUTODRIVE=1` is rejected. Every racer uses its own original pod resource, AI constructor/profile and canonical physics/control owners. Retail zero-human bootstrap and original application-root scheduling are not claimed.

Peer observations refresh before every racer frame, outside bank visits, while manager timing/rank advance only after the full required world completes. The recovered state2 combat scan retains target writes and candidate arithmetic; actual AI candidates take original rejection branches. An eligible human still stops at its unjoined geometric helper. The source multi-successor route selector now shares mutable remembered choices and the actual application RNG across real navigation and both lookaheads. Original flag, weighting, rounding and selection order are retained. Attack-target routing, stall/reset, impact, event, lap/finish and incomplete-GC boundaries remain.

Authored model normals reach current per-node transforms and separate CPU/GPU streams. Only opaque non-unlit racers opt into host lighting. Effects retain their separate path. Source-normal lighting was visible in the actual bounded capture. Exhaustive visual correctness and original shader parity remain unverified; runtime performance is limited.

## Remaining observation

Longer genuine simulation progress is still needed to reach ordinary state 2, route-selection branches, contact/event boundaries, reset and lap/finish behavior. Preserve exact source stops and actual capture/audio timing. A short clean exit or advancing ownership counters cannot establish full-course GC or complete original-AI behavior.

See [all-racer ownership](original-ai-all-racers50ix.md), [fresh peers and combat scan](original-ai-live-peers-combat-scan50ix.md), [dynamic contacts and remaining effects](original-ai-dynamic-contact50iw.md), [music order](original-ai-dynamic-contact-music-order50iw.md), [source route choices](original-ai-route-choice50iy.md), and [lighting scope](racer-source-lighting.md). Those frozen component documents retain their authoring-time staging status; this page records central integration. The current bounded result and remaining limits are recorded above.
