# Shared collision normal-mode owner50JA

Source-only continuation of the registered pod/reset-ground work. Uncompiled and untested. It connects the represented native collision calls to one application-wide3DF5B0 owner, while leaving cold reset activation and the AI state5 guard unchanged. The current all-AI route test candidate remains separate.

## Authentic values and dispatch order

The normal-mode value is a runtime source owner, not a reset option.121438 copies query8C->54 before target-shape dispatch.126FA0 copies queryShape+44 before target flags31 eligibility.124CB0/124580 preserve the last value and consume it only when a reset normal leaf needs it.

Actual constructor arguments establish the represented query fields:

- Main690:21219C calls11BBC0 with argument0;11BBEC stores shape+54.
- Secondary364:212788/21278C passes0 to that same constructor.
- Terrain348 is type2:1B1468 and fallback1B16CC pass1 to11C3F0;11C524 stores shape+54.
- Movement7A4 is type3:212444 calls11C710;11C898 initializes shape+44 to0.

The main/secondary type2 producers retain their existing mode0 numeric domain. Terrain's single-pair producer retains its existing mode1 domain. These fields are now explicit and unsupported values reject rather than silently using the old specialized leaf. Each carries a separate authenticated-binding flag. Legacy numeric-only callers retain their old specialized domain but produce an unbound receipt; they cannot establish a source global by relying on a C++ default. The interactive caller explicitly binds all genuine constructor values and rejects an unbound receipt. Movement's existing actual normalMode remains its input. No new steering, speed, collision geometry, filtering or interpolation policy is added.

## Joined effects

CollisionNormalModeWrite50JA is emitted only when a genuine per-candidate dispatch is reached. Empty lists, excluded categories, outer-sphere misses and source-filtered candidates cannot produce a write. Reached dispatches still write when later geometry misses. Type3 direct dispatch writes before its flags31/inner-sphere early returns; the ordinary list wrapper retains its earlier source filter gate.

Both type2 producers, terrain compatibility/bounded wrappers, and type3 direct/group wrappers carry the final write in their successful output. Genuine heterogeneous racer-contact queries also retain their last reached write, even when no geometry hit occurs. A later filtered pair does not erase an earlier write. The world contact receipt publishes after the accepted atomic pair pass.

The existing queries are bounded atomic calls. Their callbacks are fully represented numeric/copy operations and cannot reenter another query or read/write3DF5B0. Every dispatch within one call uses the same immutable query-mode field. Collapsing those same-value stores to their final successful-call receipt therefore preserves the value visible at the next source call. No query error becomes Complete or Miss: rejected calls retain their existing terminal boundary, and no stale receipt is published. This does not add resumable original failure prefixes to the existing atomic query APIs.

The interactive runtime owns one CollisionNormalModeOwner50JA in main, outside runSession and outside all racer-bank snapshots. Each successful main, secondary, terrain, movement and world racer-contact call publishes its actual receipt before the next source operation. The owner is retained across cold-session replacements. It starts unbound, without claiming an original initializer. boundWord returns null until the first actual accepted writer. A successful empty/filtered query preserves the previous binding and value.

## Reset connection still required

The staged reset collector must borrow this owner's boundWord as normalMode3DF5B0. If it is null, reset admission must reject. A source default, private per-racer mode or forced zero must not be substituted. The current runtime does not yet call that reset collector, so this stage deliberately supplies no new reset call or admission bypass.

Full reset still requires real visibility7F0, distinct690/694/7A4/348 construction and source-ordered detach/prepare/reattach, actual post-placementC34/fallback/type/floor bindings, canonical collection-state publication, real model/effect resets, and the correct AI PostPlayer continuation through packing/repair,16E830 and2255B0. The native world contact schedule remains its explicitly documented composition; publishing its actual query effects does not claim recovered retail all-body scheduling.

Publication contains native source and this description only. Static original evidence is retained separately; no original executable or asset data is embedded. No active edits, build, syntax check, test, gameplay, importer, guest execution or emulation was performed.
