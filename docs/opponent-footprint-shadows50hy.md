# Native opponent footprint shadows

Status: built and exercised during the full eight-racer race at 09:16 UTC on
2026-10-09. No renderer rejection occurred; 46 frame-rate samples ranged from
52.7322 to 62.5762 FPS. Separate solo equivalence, invalid-input and all-surface
shadow comparisons were not run. See current-validation.md.

## Result

Extend the existing native player footprint approximation to the renderer's same submitted opponent set, using each opponent's independent world-space draw batch and selected original archetype/material bank. This is a native gameplay track-shader change, not a diagnostic rendering path.

Changed source files:
- `platform/linux/native/native_pod_shadow50hm.h`: conservative world-XZ receiver bounds for a three-component footprint group; the original geometry fitter is unchanged.
- `platform/linux/native/native_pod_shadow_glsl50hm.h`: 24 constant-index caster slots and seven explicit, bounded opponent blocks. Each block first rejects receiver fragments outside its conservative XZ bounds. The player's three calls and expression remain the same; zero-opponent frames return before extra work.
- `platform/linux/native/vehicle_window_gl50ds.cpp`: fixed renderer-owned caster/bounds storage; own-archetype opaque/depth-writing masks; preflight fitting for submitted opponents; bounded opaque-pass uniform uploads.
- `platform/linux/native/vehicle_window_gl50ds.h`: correct the obsolete player-only shadow comment.

Integrate all four source files together. No build-system or gameplay-client edits are needed.

## Contract and cost

- Source data already exists: `frameOpponentBatches`, `frameOpponentArchetypes`, and `frameOpponentCount` are copied before synchronous submission. The renderer already preflights those independent world-space batches and retained meshes before its backend-ready callback.
- Retired opponents currently remain visible at their last completed display batch. Shadows intentionally use that same submitted batch. No separate simulation-active check, fabricated body pose, player-position reuse, or independent opponent visibility list is introduced.
- Player slots are 0..2; opponent i uses slots 3 + 3*i through 5 + 3*i. Capacity is exactly seven opponents, tied to `maxVehicleWindowOpponents50DS` with a static assertion.
- Only opaque, depth-writing caster spans contribute. Each opponent selects its own material offsets/policies. Existing track receiver blend/depth/unlit gating is unchanged.
- Bound support is computed from the same float ellipse matrix sent to GL. It encloses the projection over the complete permitted receiver-height range and maximum soft-edge spread, rounds outward, and uses inverted bounds for an empty group. Invalid bounds reject the unpublished frame before drawing.
- Inactive GPU slots cannot leak after a reduced opponent count: an explicit per-pass count gates them, and only present slots are uploaded. The transparent track pass resets both enable and count, without repeating caster uploads.
- No new draw call, pass, render target, GL entry point, heap allocation, sorting, index change, or frame-order change. Opaque racers still precede cables/transparent track/pod effects/HUD exactly as before.
- CPU work is the existing three-pass fitter for up to 21 extra components, plus fixed-size bounds construction. Worst-case source vertex visits therefore increase. Fragment work has no loop or dynamic uniform indexing; seven inexpensive receiver bounds precede optional extra ellipse work. Performance remains unmeasured.
- Shadow combination is the maximum existing component occlusion, not additive darkening. Original approximation limits remain: no recovered source shadow program, exact silhouette, self-shadow, world occlusion, or nearest-receiver test. Nearby stacked track surfaces can both receive a footprint within its height reach.

## Verification status

The integrated full-race check completed normally with all eight racers,
standings and shadows enabled. This establishes the actual combined runtime
path and observed pacing, not exact original shadow parity. No independent
solo pixel comparison, resized-window test or exhaustive shadow case sweep
was run. Remaining practical focus is finish presentation and restart.
