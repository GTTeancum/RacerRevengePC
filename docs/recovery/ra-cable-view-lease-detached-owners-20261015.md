# Cable callback target lookup and detached-source recovery

Additive follow-up to libfile_f416e7a00fe88191962913b10211dedc. Inactive and untested. No tests, builds, syntax checks, converters, sanitizers, emulation or gameplay execution. No central checkout/disc edits. Parent-added uncalled include is outside this patch and must be retained.

## Viewing identity versus callback identity

114D00 calls27F650 for the current owner38, then141C30 for its children. This camera/world traversal is not a request to bind a separate camera for each callback racer. The previous equality between CameraVisibilityLease.racer_ and callback slot was wrong for view0 scanning the other racers.

Binding::run now authenticates the unchanged active viewing lease against the actual Session visibility owner, then enters the actual Bank::visit(callback slot). Inside that visit, it proves the actual publication/generation/frame, arena/bindings/Bank identities, actual Rig roots and the cold racer/Application identities. The private Owner::cableCallbackVisibility7F0 lookup independently checks the active exclusive camera lease, target bound membership/registry/storage, target main/model roots, reset/graphics constraints and actual cable cache identity/disjointness. It is unavailable to ordinary callers.

RenderBorrow contains the viewing lease reference and a separately named callbackRacer. Lookup is performed before source writes and again at174360. The returned pointer is borrowed only within the Bank visit and active lease. It addresses storage.vehicles[callbackRacer].resetOwner.membership7F0. No new lease, lease rebinding, generation increment, copied membership, or DrawLease is introduced. The original no-argument racerVisibility7F0 still means the viewing racer and is unchanged. Existing camera/world exclusive lifetime checks remain active.

## Proven detached inputs

-434660: startup constructor table3CC548 points to3C9F70. Its stack40 is written (0,0,0x3F800000,0x3F800000), loaded at3C9FD4 and stored at3C9FFC. The actual shared Application now retains the typed native vector(0,0,1,1). It is not yet consumed by the blocked detached branch.444660 from a separate constructor is unrelated.
-854: source2119CC..211A28 constructs target2F0 and stores its length. AuthoredGeometryBinding49O.length854 is projected by chariot_root_authoring49u.cpp into VehicleFrameParameters49W.lifecycle.restLength854. This is the existing authored parameter owner; a second mutable854 field is unnecessary.
-864: source20FB9C initializes zero;218FBC and21A93C advance the actual clock. The native canonical alias is VehicleFrameState49W.bodies.secondary.time864, already used by secondary-body and chariot-lifecycle code. It must not be replaced by elapsed wall time or a new cable-local timer.
-14EEC0 is gradient noise, not sine. Its existing typed counterpart is evaluateGradientNoise49M. It mutates scratch4332A8 and can initialize shared tables/RNG. A detached join must use the actual serial shared noise/RNG owner; the lookup change does not authorize a copied per-callback RNG or caller seed.
-28B9B0 refreshes main190 world100 and transforms using its first three columns, without translation. The detached source then adds that transformed displacement to the engine endpoint. Treating this as a point transform would double-apply translation.

## Field48 and remaining blocked branches

Source20F6B4 initializes racer48 to zero.2274D0 and227BA0 clear it during left/right destruction handling. The four224720 matching-side branches read it before854/864/noise. The retained source currently has no canonical racer48 field or integrated complete producer/destruction lifecycle. The scan records direct offset48 stores and racer-region reads; this is evidence, not a proof that all indirect/alias writers are absent. Constructor zero alone cannot justify synthesizing a live value.

Accordingly, detachedField48Unsupported remains at the original reached read. No detached numeric branch has been enabled, and ordinary behavior has not been broadened by a guessed zero. Implementing noise/854/864 calculations before that proven field48 gate would consume unreached inputs and mutate shared noise incorrectly. Known producers and typed aliases are documented to support the next complete join.

Callback registration, material/texture/child joins and runtime/RA activation remain closed. All racers remain original AI; no MSAA/coverage changes.

## Root source-review checklist

1. Apply only included diffs against exact preimages; preserve newer central edits and its uncalled include.
2. Review view0/other-racer identity separation and target lookup inside the canonical Bank visit; no lease state may change.
3. Review the unchanged exclusive camera checks and concrete target7F0/cache alias checks; expired/foreign views, wrong roots/cache and reset overlap still reject.
4. Review434660 constructor/table evidence and existing854/864 aliases. Retain field48 and shared-noise-lifetime blockers.
5. Central root owns all compilation/tests/runtime review. This delivery contains no validation or gameplay claim.

Read-only source identity: SLUS_202.68 SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Static annotations and range hashes only; no guest executable or original assets are packaged.
