# RA source-world storage candidate

Base: central 2f4de36. Isolated, source-inspected only. No execution checks or central edits.

## Delivered boundary

`ra-storage.patch` factors the existing cold storage kernel through `SourceWorldView<CollisionOwner>` (the actual immutable SCB plus a stable contiguous collision-owner span). The legacy `prepare(Ground,...)` API remains and selects the existing owned-portal construction route. No dummy ColdScene, spawn, pod matrix, grid, query identity or authored replacement is constructed.

The private borrowed route is callable only by `ra_static_registry50LA::ColdStage`. It obtains the actual authenticated RA source through the prepared candidate owner's retained source, rather than accepting an arbitrary World/span pair publicly.

`ColdStage::construct(shared_ptr<const ra_static_candidates50LA::Owner>, Diagnostic&)` prepares only the 178 authentic static registry records. It exposes cold accounting (`reservedCounts`) and whether this construction actually completed. It does not expose Storage, Bank registration, live queries, membership mutation, runtime readiness or admission. This is deliberately a bounded construction stage, not a final RA registry.

## Shared topology and serial lease

- RA source `Owner` alone mints the private `LiveWorldLease`; it requires successful decode, both actual navigation bindings and exact pointer/count equality with its own heads, links and geometry arrays.
- One serial lease per source. Concurrent calls are not supported. The lease retains the actual source and releases exclusivity on destruction, including control-block allocation failure.
- ColdStage retains the candidate owner; its storage dies before candidates/radii/source. Storage retains the lease until its own registry and scratch members die.
- `Storage.world` remains the existing facade expected by callers. In the borrowed route its pointers refer to the SAME RA topology/cache that supplied both navigation bindings. No second topology is parsed, copied or allocated. This facade is not another mutable cache owner.
- Local portal arrays have zero capacity in the borrowed route; private transaction scratch still has the actual link count. Budget accounting follows these allocations. The added retained pointer increases sizeof(Storage), so exact custom byte ceilings see that genuine small ownership cost.
- `registerMutableOwners` now registers the selected `world` backing. Previously it always registered local portal arrays. This is essential: membership writes portal-link distance caches, see `live_zone_membership50en.cpp` layout guard at 81–113 and publish at 303–305. Existing owned-world callers bank the same addresses, counts and order as before.
- Existing public const source views contain raw pointers. The lease enforces the new supported writer route; it cannot prevent an unrelated caller violating the established read-only view contract. Do not run navigation rebinding, external cache writes or another Bank registration underneath an active lease. Do not bank the entire DecodedSources object.

## Preserved construction semantics

The source checks and operations remain in the original kernel: zone List selection, exact-once SCB owner binding, duplicate candidate/identity rejection, mesh/pose/category/sphere equality, additive animated/trigger validation, same SCB zone/ordinal MaterialID lookup, filter capacities, ordered movement registry, zone lifecycle, source static registration queue and drain.

The existing three required query positions remain capacity reservations. For this static-only stage their records remain zero/unbound; `collectionInput.objectCount` retains the old `terrain + 3 + recovery` expression. No genuine query owner is inferred from those numbers. RA ColdStage requests no camera, recovery or wreck slots. It does not expose this collection input for execution.

Construction mutates only private storage and the serial lease flag, not the source world/cache or candidates. Failure is terminal for ColdStage; allocated local storage is discarded, lease is released and retained source/candidate prefixes may survive. Source parser/libm effects from the already-completed RA decode are not rewound. A second attempt does not revoke a previously completed stage.

## Remaining integration boundary

This stage cannot be promoted by merely exposing its Storage pointer: its fixed extent excludes the actual RA additive trigger/dynamic owners. Once those owners exist, root must decide the complete binding order and actual query reserve set before the single final allocation. The existing trigger/animated checks should be reused intact; RA-specific trigger provenance still needs its genuine owner. A final owner should retain all of these owners and the lease, verify query/member publication, and bank the selected shared portal backing exactly once at the genuine serial boundary.

Rebuilding a final storage after retiring this isolated cold stage is safe only before any live registrations, Bank leases, queries or memberships have been exposed. The current API deliberately exposes none. There is no unbind/reset shortcut and no claim of live membership completion.

No profile, SessionAssets, host/Course capability, generated asset, pixel path or gameplay admission is changed.
