# Actual HUD and dynamic image transfer coverage

Base2b2f983. Source-only incremental integration, no full2543F0 callback.

RaceDisplayLifetime::bindCreationRequest now seals its SAME Application texture
lifetime into VehicleWindowRequest50DS. The real renderer stores that request,
so later HUD-local upload requests inherit the same binding even after initial
renderer creation. TimeStats, PositionStats, Damage, SpeedBoost, legacy race HUD
and inactive original Pause all propagate it to their actual MaterialTextureGl
calls. The immutable binding weakly references a const display and the stack
transfer retains a const display; this preserves the existing const factory
signature without casting away constness or creating an owning request cycle.

## Existing dynamic paths already covered

The actual driver passes its previously bound textureRequest to each of these
real createGpu methods, which copy caller before changing sampler/budget fields
and invoke their existing MaterialTextureGl owner:

- gc_dynamic_window50ig_fixture.h and ga_dynamic_window_fixture.h
- sa_dynamic_window_fixture.h
- gc_atm_window_fixture.h and ga_atm_window_fixture.h
- ma_sa_atm_window_fixture.h
- gameplay_binder_window_candidate_fixture.h
- gc_particle_materials50kg_fixture.h

Thus they inherit the sealed binding already; no additional fake dynamic upload
owner is required. Their later animation changes frame/geometry selection, not
a separate retained client-memory transfer pool. Primary/opponent/track/cable/
sky/specular MaterialTextureGl request copies are likewise already bound.

All direct texImage2D calls in vehicle_window_gl50ds.cpp and the HUD/blur/coverage
storage includes pass nullptr: these allocate native render targets, not borrowed
client image transfers. Their actual GL names retain the existing checked
renderer cleanup. Neither these names nor persistent material image banks are
source252EA0 CPU transfer allocations to delete at manager reset.

## Remaining boundary

This closes the HUD-local omission for the actual race display. Independent
frontend bootstrap owns its separate request/renderer and is not silently
certified by this race binding. Generic MaterialTextureGl callers can still be
unbound; no all-covered flag or aggregate reset success is manufactured. A full
native2543F0 join must establish the admitted root route and all participating
actual transfer/cache owners before treating the direct-upload strategy as its
transient-storage equivalent. Existing reset already rejects registered active
transfers and performs the one actual renderer drain. This patch adds no new
reset caller, releases no image, and changes no source image bytes or policies.

No build, test, syntax check, gameplay or decoder execution was performed.
