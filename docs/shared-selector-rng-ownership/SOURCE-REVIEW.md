# Independent shared RNG guard candidate review

2026-10-12 03:05 UTC. Candidate: `/workspace/shared/shared-rng-guard-candidate-20261012/changedfiles`, based on main `c8e53dfd95c5e9cddb84349cd3de9bc1c1bd24c0`.

## Result

No definite source/API/lifetime blocker found in the five-file candidate for the root-approved bounded scope. This is source review only; no build, test, syntax/compiler probe, project script or gameplay execution was performed. No live/shared source files were edited by this reviewer.

The original same-address source evidence and the menu-versus-race impact distinction are in `/workspace/scratch/74cd1d3d7a45/course-shared-selector-review-20261012.md`.

## What the candidate closes

- `SharedRandomOwner50ID sessionRandom` replaces the independent race `sessionLazy`, `dynamicRange` and `gcAnimationLazy` owners.
- Dax/announcer and lap borrow `sessionRandom.seed`; contact music borrows seed and range; GC service borrows the whole aggregate.
- The former lap and contact structs retain their exact fields/defaults/types in a shared header. Their existing function APIs remain unchanged.
- The sole aggregate is a stable local owner outside per-racer snapshot storage. Announcer's retained pointer addresses the stable seed subobject, and the caller serializes all current consumers.
- All consumers still use the same `s.noise.sharedRandomState` and existing application clock. Initializing the raw RNG does not falsely initialize the selector lazy cache.
- A successful GC seed now makes a later Dax/lap/contact selector observe the same nonzero cache and skip the previously duplicated reseed. Conversely, an earlier Dax/contact seed is observed by GC. A cached zero preserves the source conditional reseed behavior.
- Source reads found no remaining old owner variable references in the changed runtime and no additional live `ServiceShared` callers that need adaptation. The new header's include path is available to both former declarations, including dynamic impacts through its existing lap-event include chain.

## Deliberately unchanged limits

The aggregate resets at the existing cold native race boundary. This does not establish the original application's cross-race lifetime. Course and menu→race integration remain excluded, and Course's private voice cache remains a separate pending candidate improvement.

GC service still stages its aggregate and raw RNG, committing only after successful model advance; thus its existing failure rollback differs from source completed-prefix behavior. Lap's existing assertion/numeric semantics are unchanged. The candidate does not broaden acceptance of disabled-assertion states or change any float choice formulas. These exclusions match the root's explicit scope decision.

## Exact reviewed SHA-256 pins

| Relative path | SHA-256 |
|---|---|
| src/game/shared_random_owner50id.h | 1f210028ef529605d72e64a253db118f5a8ee62113587c1e7ed8ecd6a2d77471 |
| src/game/lap_event6_subscriber50id.h | 07995500dbe7930dbdb112f5e871ca87ce6895c0d6edc8a1425d3dc0b1d46a13 |
| src/game/dynamic_impact_events50iw.h | 26c123f9eb6b732d95c8a68e45ed3bdb30cac9cba1390a7a76d7d3ff0e8affdd |
| tests/native/gc_animated_world50ig_fixture.h | 82ae5fc9d2d40f10307ec1f7dbd9c9363448087f533c6ebbb011b44349c940e5 |
| tests/native/terrain_interactive_drive50du.cpp | 585bf57dcf84acab278dbb9e72c825f7602b01d82e33e067605eec2c519d5d4f |

Root identity reconciliation: final GC fixture SHA256 b9947d53938c12a7a82b7336dec10985e7df6a730badd5c48c5b80643b84af31 adds only the two-line failure-staging comment. Removing those exact comment bytes reproduces the independently reviewed82ae5fc9 hash. No executable validation was run.
