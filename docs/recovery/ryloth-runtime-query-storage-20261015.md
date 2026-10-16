# Fresh RA runtime Storage / borrowed-arena Follow backing

Base source: bdb9d80 (current e664497 adds only the verified Windows recovery document). Isolated source authoring and manual inspection only. No build, syntax check, test, conversion or runtime invocation.

## Callable API

The actual RA aggregate now offers two inactive stages:

1. `bool prepareRuntimeQueryStorage(const ra_source_camera50LA::RuntimeQueryCapacityInputs&, Diagnostic&) noexcept`
2. `bool bindRuntimeQueryOwners(const ra_source_camera50LA::ActualQueryOwners&, Diagnostic&) noexcept`

`reservedRuntimeQueryCounts()` returns only fixed allocation accounting. No Storage pointer, mutable world, Follow query getter, camera, attach operation or SessionAssets capability is public. The bridge constructor and stages are private to the existing RA aggregate. It obtains source and candidates from that aggregate, never accepts an arbitrary World/Storage pair.

The original `ra_static_registry50LA::ColdStage` file is unchanged. The aggregate rejects either route after the other route was attempted. Neither route resizes, replaces or steals a surviving Storage/world lease. Both attempts remain terminal on failure under the existing aggregate policy.

## Capacity stage and real inputs

`RuntimeQueryCapacityInputs` contains:

- 1..8 actual prepared `native_racer_collision50HW::Owner` pointers, distinct objects at stable addresses. No file loading, decoding or alternate pod selection occurs here.
- Optional actual fresh `undertrack_owners50ew_fixture::Owners*`; its presence selects the recovery pair. It must not already have allocated or prepared storage. Its later `prepareStorage` uses the actual returned membership count.
- A bounded sequence of `WreckReservation`, each retaining borrows of a real unprepared `OriginalWreckQueries50LA`, a distinct prepared `OriginalWreckAssets50LA`, and three planned unique native identities. A count alone cannot reserve wreck records. Current stage never calls wreck prepareCold or publishes a piece.

Before acquiring the exclusive live-world lease, the bridge checks the exact 178 authenticated static bindings, source identity, typed input extents and identity conflicts; copies only stable borrows/identity values; computes mesh extents with checked nodes+faces+1 arithmetic; fixes Limits; and seals capacity. Query records total `178 + 6 + (recovery ? 2 : 0) + 3 * wreck.size()`, bounded by the existing 1024 registry ceiling. The source Storage API still performs all original source/static/material/portal checks and payload accounting.

Recovery mesh stack is the maximum actual mainTarget mesh extent across the retained roster. Wreck stack is the maximum actual E4 mesh extent across the retained prepared assets. Follow also derives its actual static/compound mesh stack through the unchanged constructor helper. No caller capacity or guessed count substitutes for source resources.

The exclusive lease is the existing `ra_cold_sources50LA::Owner::LiveWorldLease`; Storage borrows exactly its pointer/count facade and retains the same lease. The bridge strongly retains candidates, which strongly retain the original source. No second navigation world/cache owner is created.

This is deliberately the known static prefix plus actual query reservations. It does not claim final additive trigger/dynamic extent. LostAI historical geometry remains an independent gameplay dependency. Adding genuine additive owners later requires the coherent fresh route to include them before allocation; no current reservation is quietly treated as complete.

## Actual query binding and ordering

`ActualQueryOwners` carries the existing Ready frame arena, actual application collision normal-mode owner, main and secondary query/filter references, the selected prepared collision owner, real prepared terrain authoring and live single-pair owners, and the same optional recovery/movement objects.

The bridge checks selected collision pointer membership in the retained roster; exact main()/secondary() resource pointers; query/filter aliasing; real distinct query-scene identities; source normal-mode fields 0/0/1; terrain source-size receipt and live segment/resource coherence; actual static/query collision type agreement; distinct nonzero query identities disjoint from source and camera namespaces; and absence of attached memberships. The terrain source-size receipt is a cross-check, not a new byte-authentication certificate: genuine terrain authoring must have been produced through the existing selected-collision/PHY API by the caller. Likewise, the normal-mode reference is supplied by the actual Application owner; the API does not invent a written initial word or claim that arbitrary references prove Application identity.

When recovery is selected, its fixed links/entries must match this Storage's actual membership capacity, namespace and terrain/movement identities. It must have completed prepareStorage. Its selected source may still be unbound, preserving the existing prepareStorage -> Follow allocation -> per-racer selected binding chronology. If already sourcePrepared, the selectedSource must match the actual supplied collision. No body phase is changed and no all-eight Bank Ready condition is incorrectly required before Bank sealing.

The actual borrowed references are retained as an `ActualQueryOwners` record, without copying mutable queries/filters/probes. The bridge then directly constructs the private Ground-free QueryBridge. That real constructor allocates Follow/death backing, binds actual world/workspaces and normal-mode owner, validates SceneInfo, and reserves the actual death identity with no shape. Existing main/secondary/terrain arena authority is preserved. No synthetic ColdScene, private normal-mode word, source reset, AI driver or query identity is created.

All later source preparation/registration remains pending. In particular, no call to Follow::prepare, undertrack attachFreshQueries, wreck bindWreckQueryWorldCold/prepareCold, Bank::seal or camera publication is made. Planned wreck objects are rechecked for freshness, sealed mesh extent and conflict with the now-known actual main/secondary/terrain/recovery identities before Follow construction.

## Lifetime and partial failure

Caller-owned frame/query/filter/probe/normal-mode/recovery/roster/wreck objects must remain stable and alive through aggregate retirement. The input vectors themselves may be destroyed after calls because the bridge copies their typed borrows. The actual immutable source/candidate/world lease is strongly owned internally. No shared_ptr<void> caller token is used to disguise lifetime.

Failure is terminal. Any already constructed Storage and its lease remain retained; constructor effects are not rolled back. Rejected early capacity input has no world lease or Storage effects. A rejected selected binding retains the constructed Storage. Follow allocation/constructor exceptions retain that world backing, while ordinary C++ partial construction cleanup releases its incomplete buffers. No attempted stage is replayed or widened.

Aggregate close preflights runtime ownership before releasing its other owners. An actual Follow prepare attempt must have successfully closed under its original bank0 obligation; the aggregate never calls query close outside bank0. Main, secondary, terrain and recovery memberships must be detached. A prepared wreck object holding this exact world also blocks close even if its memberships were removed: its retained world pointers need a genuine typed retirement join, not a drained boolean. That future wreck retirement API is intentionally not invented here. The current stage exposes no operation capable of establishing such a wreck binding.

The cold-only path destroys Follow backing, then Storage and the exclusive lease, then source/candidate retention. Existing checked destructor/terminate discipline is preserved. Incomplete live cleanup leaves owners available for the real caller's retry rather than orphaning registrations. Caller borrowers may not be destroyed first to bypass checks.

## Files / compilation

- New header `tests/native/ra_source_camera_ownership50la.h`.
- Narrow friend declarations in `ra_cold_sources50la.h` (including nested lease), `ra_static_candidates50la.h`, and `track_runtime_storage50ig_fixture.h`.
- Aggregate stages, accounting and close preflight in `ra_cold_aggregate50la.h`.

The existing main translation unit includes the aggregate, which now includes the new header; no out-of-line source or build-list change is required. Existing Ground QueryBridge constructor is untouched, including the centrally fixed sourceWorld parameter name. No TA/SA/GC admission/constructor is changed.

## Next concrete join

A subsequent typed runtime owner must register the real arena/storage fields with the same Bank, construct genuine per-racer source/query state in existing order, install same-source memberships and complete all required additive collider owners. It then provides bank0 Follow preparation and checked retirement access plus the same aggregate's private lighting owner to SourceCamera. This patch cannot publish a camera or static-light snapshot. The absence of final LostAI history semantics does not prevent this honest cold backing construction, but still prevents declaring RA gameplay complete.
