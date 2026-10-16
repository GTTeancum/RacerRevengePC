# HUD archive-provider lifetime candidate

Base: ed87dbe7eacf4b387327e1a36ca39d33647a627c.

Patch: hud-provider-lifetimes.patch. Only original_time_stats50la.h and original_speed_boost50la.h change. Candidate files and unchanged baseline copies are retained in candidate/src/game and base/src/game. Central checkout was not modified.

## Findings and exact effects

- Both existing nativeCpuResourcesPrepared queries checked owner preparation, live context, entity lifetime and current material collector, but omitted archive-provider phase/revision. The provider commit path increments revision and unseals on a new mount (original_standard_archive_provider50la.h:170); sealNativeMounts merely seals again (:97). An identical mount is a no-op (:160).
- Each owner now samples the actual domain provider revision after requiring a non-null sealed provider during finishNativeColdPreparation50LA. Existing resource/provenance/map/demand checks remain in their original order. Immediately before publishing preparation success, each verifies that the provider remains sealed at that sampled revision, then stores the revision in a private scalar. It is never caller-supplied and is not written by failed completion.
- Each readiness query preserves all prior conditions and then requires a non-null, sealed provider at the captured revision. A subsequent new mount therefore rejects readiness both while unsealed and after resealing. Identical remounts do not invalidate readiness by themselves. These observations allocate nothing and mutate nothing.
- The domain creates and assigns its provider only in successful prepareFresh, which rejects an already-prepared domain; no supported provider replacement path exists (original_standard_material_domain50la.h:41-79). The retained domain therefore supplies the same provider identity, so an additional owner pointer is unnecessary for this change.
- Existing nativeMaterial, CPU update/draw admission and renderer resource/frame/submission gates already call nativeCpuResourcesPrepared. Their existing control flow now rejects a stale provider without renderer edits. Direct nativePixels/nativeDraw accessors remain unchanged; readiness does not erase an earlier CPU draw receipt or certify GPU retirement.
- New cold-completion failures use the existing stop path with source error and 0x132820, matching the archive-provider lookup boundary. Partial material references, font borrows, geometry, decoded pixels and collector effects remain owned for existing teardown; nothing is rolled back or reacquired. No destructor, friend/access boundary, layout calculation, simulation arithmetic, shader policy, draw ordering or renderer resource management changes.

## Scope and limits

This is a lifetime guard for the provider revision observed during successful cold preparation, not a new full archive namespace revalidation routine or a recovery/repreparation mechanism. It does not claim to fix provider changes before completion begins. Existing completion validation remains responsible for resource correctness. The provider contract is cold serial; the added comparisons do not provide concurrency synchronization.

Source reviewed only. No tests, builds, syntax checks, sanitizers, gameplay checks, converters, or project code were executed. No compilation, runtime or visual result is claimed.
