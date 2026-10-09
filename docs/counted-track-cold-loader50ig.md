# Counted original COL cold ownership, 50IG follow-on

Status: source-reviewed only, uncompiled and untested. No build, syntax check,
fixture execution, asset decode, game, emulator or render was run. No active
checkout, menu, Clegg, prior-stage file or original asset was edited.

## Deliverable and integration order

This is a narrow two-header follow-on in `patch.diff`:

1. `tests/native/authored_runtime_ground50cp_fixture.h`, based on the staged
   track-path `Request` loader, not the active checkout's original TA-only
   loader. Its preimage is the first stage's `files/` version.
2. `tests/native/track_runtime_storage50ig_fixture.h`, based on the new helper
   in `storage-stage/files/`. Its four small changes make the helper consume
   the loader's shared ceilings instead of admitting 1024 terrain owners.

Apply only after those prerequisite staged versions, or port the narrow hunks
with their prerequisites. The storage-header correction depends on the shared
constants introduced in the loader. Do not overwrite the active main, menu or
Clegg work. Full postimages, original staged preimages and their SHA-256 hashes
are included. Earlier stage copies remain unchanged for review.

## Bounds rationale

`SearchStorage50BS::words` has 1024 slots; cold List initialization has the same
1024 per-block limit and its grow/new-block path remains unsupported.
The runtime storage helper retains three collection references: main query,
secondary query and terrain query. The conservative whole-session ceiling is
therefore 1024 - 3 = **1021 original COL placements**. It works even if all
static and dynamic/query owners occupy a single zone method; no zone-density
assumption or repartitioning is used to admit more original owners.

- `runtimeSearchSlotLimit50IG = 1024`.
- `runtimeDynamicReferenceReserve50IG = 3`.
- `hardCollisionOwners50IG = 1021`.
- `Request::collisionOwnerLimit` defaults to 1021 and may only reduce that
  ceiling. Zero and values above 1021 fail before the original archive is read.
- The storage helper's `Limits::terrainOwners` uses the same 1021 ceiling.
- At the maximum, collection object references are 1..1024: static owners
  1..1021, then main 1022, secondary 1023 and terrain 1024. Existing aligned
  SearchList identities are still object-reference << 2; no identity semantics
  changed. The separately owned movement List uses only original terrain refs.
- Storage limits of 256 zones, 4096 portals, a 32 MiB element-payload budget and
  the original 64-entry selection/merge capacity remain unchanged. In
  particular, owner capacity is not a license to enlarge the source 64-entry
  query/merge result semantics. Existing overflow/failure boundaries remain.
- This change does not add an archive, mesh or total-process-memory allowance.
  The original importers and their input restrictions remain authoritative;
  the storage payload budget still excludes original COL meshes, allocator
  bookkeeping and racer-bank snapshots as documented in the prior stage.

## Original ordering and pointer lifetime

The loader first scans `world.instances` in its existing authored order and
counts exactly those collision-member names found by the existing
case-insensitive original-archive membership predicate. Repeated uses of a COL
member count separately because they are distinct original placements. This
is an owner/placement count, not a distinct filename or geometry count.

The complete count is checked against the caller's reduced bound before any
`Owner` is constructed or the owners backing vector is reserved. There is one
`owners.reserve(count)`, followed by the original ordered construction loop.
The second scan uses the same predicate over the unchanged decoded world and
archive. Every append is bounded by that count and a final count equality check
is explicit. There is no sort, deduplication, truncation, second reserve,
shrink-to-fit, alternate geometry or placeholder owner.

The original COL extraction, decode, mesh-cache load, placement/category/sphere
preparation and audit remain unchanged for each selected instance. No existing
`Owner` can be relocated by an append within the pre-reserved complete capacity.
Returning the vector by move transfers its backing storage. Consumers still
must retain the final `ColdScene` and not copy, resize, erase, reorder or mutate
owners while candidates borrow their mesh/name/subobject addresses. This is
cold setup only; neither the loader nor the runtime helper gains a hot resize
or asset-discovery path. Temporary archive/decoder/audit allocations are still
cold allocations, not a new zero-allocation claim about the import process.

TA keeps the same paths, grid rank, masks, original placement order and loading
operations. Its retained 37 owners require 37 requested backing elements rather
than the former reserve of 64. Runtime reference counts remain 38/39/40 and
source selection remains 64. This is source reasoning, not a tested equivalence.

## Deliberate admission boundaries

Missing archive collision members are still skipped exactly as before. The
count is only the set actually present in that original archive; it does not
prove that an absent member is optional or that an external original archive
is unnecessary. A zero-owner scene retains the prior loader behavior, while
the staged runtime helper continues to require a nonempty exact binding set.
These rules must not be used as whole-track resource admission.

This stage does not claim GC supported or playable. GC.RES, all required real
COL/PSG/material/texture/sky resources, M/A navigation, spawn, AI, authoring
admission and source bindings still require their own original-byte closure.
The existing List-only zone admission, importer requirements and unsupported
live-zone link-removal boundary are untouched. Current main's fixed candidate
construction and arrays still require integration with the earlier storage
stage. No track-selection readiness flag changed.

## Root-owned validation still required

Before integrating or presenting gameplay, root must compile and exercise the
actual original assets in its permitted test window. Focused future checks
should include retained TA equality and candidate pointer stability, repeated
COL placements in original order, 64/65 and 1021/1022 count boundaries, reduced
caller limits, an absent original member, and all three dynamic references at
the one-method/zone limit. These are proposed checks, not executed results.
