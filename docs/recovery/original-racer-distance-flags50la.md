# Original racer radius, distance and map-2 preparation

Source-only bounded production increment, 2026-10-13. No build, test, syntax
check, scanner, asset decoding, emulation, guest execution or runtime work was
performed. No performance or visual result is claimed.

## Connected behavior

The existing actual right-engine model now owns its aggregate bounds/radius,
current-camera distance1BC and the two recovered distance-dependent241280
flag bits. The existing specular consumer uses that model-produced map-2 flag
instead of computing an independent distance and launch-subtype threshold.
This refresh occurs for every presentation, including retained stopped-racer
geometry. No graph or animation is advanced to refresh a stopped racer.

1.50CM's existing source-backed49O box computation retains all four lanes of
  center160 and halfExtent170 plus its exact radius198 in the successful draw
  submission. It uses the same current graphical locals, source-index map,
  authored boxes and actual root as before. It adds no bounds recomputation
  and does not use the renderer's display AABB or a physical-body radius.
2.50DU creates one `OriginalRacerModelRenderGlobals50LA` from its existing
  native launch subtype. The constructor expresses the actual1A6AA0/2410E0
  write: subtype3 selects1000; other subtypes select130. All model consumers
  read that same owned382FA0 value. These constants come from the retained
  setter call, not a shader/material-name inference. This remains the explicit
  native ordinary-launch owner; it is not a claim of a complete retail root.
3. The actual player/opponent descriptors borrow the SAME indexed right-model
  packet owners already used to populate their current/retained matrices.
  They also borrow the session's real source fog owner and shared threshold.
  The caller owns that pairing; no new per-frame table/registry/identity scan
  or descriptor identity-bridge dependency is introduced.
4. The renderer's existing pre-GL specular preparation first checks the actual
  source camera against the raster, then calls the actual model packet owner.
  The current source fog preparation, where used, has already completed.
  The new stage reads sourceTrackFog.start34, the input to original13EA74
  state36/24516C B0. It does not substitute fog.end38, a UI default, host fog
  approximation, or camera far plane. No guest render-state bank is modeled.
5. The owner runs the finite multi-node26F070 distance arithmetic from the
  retained aggregate center, retained LOCAL F0/F4/F8 translation, and this
  current source camera. It publishes center160/halfExtent170/radius198 and
  distance1BC into its canonical native model facet. Optional radius/distance
  storage distinguishes a real production from unwritten cold state.
6. It updates only bit02 in1B2 and bit02 in1B3. The source fog bit is
  `!(distance + radius < fogStart)`; the source map-2 disable bit is
  `!(distance <= sharedThreshold)`. Every other existing owner bit is preserved.
  A clearly limited `distanceFlags20` field captures only mask0x0202.
7. The native specular UV leaf consumes the resulting distance/map-2 flag.
  The former renderer-local distance calculation and launch ternary are
  removed. UV math, triangle/group ordering, draw calls, feedback barriers,
  shading equations and the actual GPU map-2 consumption are unchanged.

This is a real producer/consumer join, not an index or diagnostic-only result.
The fog bit is now produced in canonical preparation state, but the existing
paired shader's F255 fog policy is unchanged. This increment does not claim
that source per-vertex fog generation is implemented.

## Source arithmetic and admission

Source26F070 chooses radius194/center140 only when actual nodeCount1A0==1;
otherwise it chooses radius198/center160. This increment admits only the
already-owned, multi-node, single-palette right-engine route. It never
substitutes a source row's radius194 for an unproduced model radius194.

The existing50CM bounds join already rejects sphere/own-box model-root flags
0x14 and requires a seeded49O ordinary box result. The new radius is copied
from that same computation. Existing49O retains fourth bounds lanes as1;
all four lanes are copied rather than inventing a new homogeneous lane.

For each axis, displacement is separately rounded
`(center160 - cameraPosition) + modelLocalTranslation`. Distance is the same
ordered binary32 squares/adds and square-root conversion used by the prior
specular leaf. The new shared helper preserves FE_TONEAREST, finite arithmetic
and output-on-success semantics. No inverse root transform, extra model
translation, normalization, epsilon, nearest-point or projected depth is used.

Fog uses one separately rounded distance+radius add. Equality enables the fog
bit; equality at the shared map-2 threshold keeps map2 enabled. The producer
does all fallible numeric work before publishing any model field or output.

## Lifetime and failure boundaries

- The actual240740 owner and model/geometry resources already outlive submit.
  Borrowed model/fog/globals pointers are used synchronously before frame GL
  work. The renderer retains only copied camera/UV/map2 results, not those
  mutable owner pointers.50DU explicitly clears the model/fog/globals borrows
  immediately after submit returns, on both success and failure, before any
  later display completion or Session close. No new retain/release or teardown
  order is added.
- Matrix preparation must have completed on the actual packet owner before
  view-dependent preparation is admitted. Retained stopped batches retain the
  bounds and local-position values from that successful matrix/geometry cut.
- The actual50DU indexed model arrays and successful batches establish the
  pairing. The public frame API requires callers to preserve that invariant;
  the new pointer alone is not a cryptographic identity or lifetime proof.
- Model bounds/distance/flags are preparation state, not GPU acceptance or
  texture-binding history. A later UV/renderer failure can leave prepared
  fields changed, as the existing matrix preparation can. It publishes no
  accepted frame and introduces no history rollback claim.
- New frame fields require paired-body capability; unsupported or missing
  owners reject instead of reverting to the old distance calculation. The
  existing no-paired null policy also covers opponent model-owner pointers.
- Original AI, physics, material callback/RNG/frame-stamp behavior and the
  existing pause/menu paths are unchanged. Refreshing view-dependent fields
  does not advance an animation/material clock or re-enter the racer bank.

## Why full flags and LOD remain closed

The source chain is26EFC0 ->240C00 -> selected191 ->26F070 ->241280.
240C00 needs actual gameplay observer19C membership rows, their union
projection matrix, current core/guard camera matrices, and mutable3833D8 and
4354F0 settings. Native selected-light membership is not proof of those
projection owners. The existing UI box-clip numeric leaf can be reused later;
the UI owner's membership/defaults cannot be relabeled as gameplay state.

This increment does not fabricate240C00 clip bits, rewrite191 selection,
write a complete packet20 word, set1B3.08, or claim the original full packet
snapshot/matrix-copy ordering. The source snapshots full flags BEFORE each
matrix copy and sets08 only AFTER all copies; the native distance subset is
deliberately prepared at the later current-camera presentation stage.

Actual LOD inputs are recoverable: the real geometry scale14 and LOD cutoffs,
current source focalPixels1D8, and source383360/383368 settings. But the actual
native mesh, spans, normals, source colors, texture metadata and UV resources
currently consume LOD0 throughout.26F070 also invokes each chosen-LOD
surface's projection method. Writing1B0 without connecting those consumers
would not implement LOD rendering, so no unconsumed LOD index is added here.
Single-node/sphere routes, full per-surface projection, global FIFO/drain and
original light-cache/spatial selection likewise remain explicit obligations.

## Exact integration and retained evidence

Base c53e1bfee4c93b049dd714326889fc0009145ecb, tree
697316f88423127c4d1ffd865d3d915b604a9311. The collector, representative-binding,
consumed packet producer and root renderer invariants are already in that
base. There are no unmerged prerequisites. The excluded descriptor identity
bridge and earlier primary/lower/chariot/window feature layers remain absent.
Original PSG topology, both performance changes and all root invariants are
preserved. No translation unit or build-source inventory change is needed.

Retained ORIGINAL text SHA256:
04b18bf045cd90361606491a5f54e0e7b549003e306001c870f8cfc9fd4fc1ca.
Original ELF identity:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
The packet retains exact source excerpts for26EFC0/26F070,240C00/2410E0,
26F4B0,241280,1A6AA0, fog and projection producers, plus detailed source
contracts and independent manual caller/lifetime review. These are source
evidence, not executable guest data or runtime validation.
