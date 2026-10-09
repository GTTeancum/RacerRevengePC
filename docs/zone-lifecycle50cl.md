# 50CL: persistent explicit zone registrations

Status: code written, uncompiled and untested. No test, build, sanitizer run,
executable probe, or validation script was run for this change. The parent owns
the next permitted team-wide test slot. Build files and existing tests are not
changed here.

## Scope

`zone_lifecycle50cl.h/.cpp` adds a caller-owned persistent adapter over
`zone_registration50cj`, `zone_search_views50cg`, `search_storage50bs`, and
`search_cold50bu`. It initializes cold zone-method registries, tracks one
registration per explicitly numbered object, transfers registrations, removes
them, and refreshes the affected published SearchList method views.

The caller supplies the destination zone and first/second method on each update.
Object and zone references are 1-based. A record at `objectRef-1` has the native
list identity `objectRef<<2`; it is not a mapping of arbitrary game identities.
The object table must therefore retain stable reference order for the adapter's
lifetime. Register objects in the caller's intended source order.

No zone ownership, portal, crossing, scene refresh, or geometry policy is added.
No linked zone chain, global head, fallback zone, or collector cursor/state is
created or modified. In particular, the original global `443148` single-link
projection remains separate; this adapter does not make all authored zones
reachable from that head. The numeric all-zone chain in the separate 50CH
diagnostic remains an explicit fixture assumption.

## Backing and lifetime

For `Z` zones and `O` object references, the caller provides:

- `2*Z` `SearchStorage50BS` registries, ordered first/second for every input zone
- `O` `ZoneRegistration50CJ` records, indexed by object reference minus one
- `2*Z` `CollectionTree50BX` method descriptors and `Z` zone descriptors
- `4*Z` `SearchListBlock50BR` descriptors
- One reusable `ZoneRegistrationScratch50CJ` containing two registry copies
- A value-initialized `ZoneLifecycle50CL` state

Full supplied capacities must be backed, correctly aligned, mutually disjoint,
and disjoint from state, scratch, and the initialization storage description.
Inactive capacity tails participate in alias checks but remain untouched.
Pointers are borrowed for the lifetime of the adapter and any collection using
its views. The adapter owns no allocation and does not free caller storage.

Initialization requires a fresh state and fresh registry ownership. The explicit
uniform `perBlock` capacity is 1 through 1024. Initialization resets only active
registry headers, active records, methods, and zones. It leaves registry backing
words and cold descriptor slots opaque. The initializer uses 50BU and prepares
the empty SearchList method template with 50CG before publishing any output.

Each registry reserves two fixed descriptor slots. They become the live block
descriptor and required null terminal descriptor after first insertion. A newly
populated method cannot move another method's descriptors. Each method is bounded
to its first physical block; importing prepared multi-block registries and
allocating further blocks are outside this adapter.

## Mutation and publication

`updateZoneRegistration50CL` is an upsert for one explicit object reference. An
empty record registers it, the same destination retains its existing slot, and a
different destination transfers it. `removeZoneRegistration50CL` clears a live
record and is idempotent for an already valid empty record.

Mutations reuse 50CJ. Removal preserves physical tails and pushes the released
slot onto the registry free list. Insertion consumes that free head before a
fresh tail slot. Removing the last object does not collapse the existing block.
No registry sorting, compaction, candidate flattening, hidden registration, or
capacity-driven eviction occurs. A full destination with no free slot returns
`GrowthBoundary`, preserving the old registration and every published view.

Before mutation, the adapter validates all declared backing spans and the
affected published views, including stable method references and terminal block
descriptors. 50CJ validates the complete affected list topology and performs its
two-registry transaction. Once 50CJ succeeds, only infallible view assignments
remain. The affected methods then point at their actual persistent registry
words, never at scratch. Other methods, zones, and records are not rewritten.

Every non-`Complete` result preserves state, registries, records, and views;
scratch may change. Invalid arguments/layout return `InvalidInput`. An affected
registration/view inconsistent with adapter ownership returns `InvalidTopology`
or the underlying list-validation status. This is a serial transaction: calls
must occur between collections with no active iterator or concurrent observer.
It does not make intermediate writes atomically observable to other threads.

Do not mutate or relocate active backing, replace views with a separately packed
50CG rebuild, reset a registry behind the adapter, or copy state to create another
owner. The normal collector's state/cursor lifecycle is still the caller's job.

## Example

```cpp
constexpr std::size_t zones=9,objects=37;
// Keep this persistent backing out of a small per-frame stack.
SearchStorage50BS registries[zones*2]{};
ZoneRegistration50CJ registrations[objects]{};
CollectionTree50BX methods[zones*2]{};
CollectionZone50BX zoneViews[zones]{};
SearchListBlock50BR blocks[zones*4]{};
ZoneRegistrationScratch50CJ scratch{};
ZoneLifecycle50CL lifecycle{};
ZoneLifecycleStorage50CL backing{
    registries,zones*2,registrations,objects,
    {methods,zones*2,zoneViews,zones,blocks,zones*4}};

auto status=initializeZoneLifecycle50CL(
    lifecycle,backing,zones,objects,64,scratch);
if(status!=SearchStorageStatus50BS::Complete) return status;

// Object reference 1 is explicitly assigned to zone 3, first method.
status=updateZoneRegistration50CL(
    lifecycle,1,3,ZoneMethod50CL::First,scratch);
if(status!=SearchStorageStatus50BS::Complete) return status;

// A caller decision moves that same object to zone 4, second method.
status=updateZoneRegistration50CL(
    lifecycle,1,4,ZoneMethod50CL::Second,scratch);
if(status!=SearchStorageStatus50BS::Complete) return status;

status=removeZoneRegistration50CL(lifecycle,1,scratch);
// Bind methods/zoneViews to the collector. Supply zone links and query heads
// independently; never keep a collector iterator open over these updates.
```

The illustrative counts match the current diagnostic's nine zones and 37
collider owners. The API does not assume those counts or manufacture their
authored assignments.

## Remaining integration and verification

- Add the new translation unit to the intended build/diagnostic target.
- Bind real persistent object refs and explicit destination decisions in the
  runtime; preserve the caller's initial registration order.
- At the parent-authorized test slot, cover cold insertion, same-owner updates,
  source/destination view refresh, removal/re-registration free-slot order,
  transfer into a full destination, aliased backing/scratch, malformed records,
  view corruption, zero/count limits, and preservation of inactive tails.
- Portal/spatial destination selection, multi-block growth, mutable object-table
  indexing, authentic scene registration events, and global-chain integration
  remain explicit dependencies rather than inferred behavior.

Sealed 49W, held 48U, excluded 49C, and unapproved `ee.bin` are unchanged. No user
computer access is involved.
