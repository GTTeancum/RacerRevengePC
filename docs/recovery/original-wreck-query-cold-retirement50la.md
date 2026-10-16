# Typed unpublished wreck-query borrow cancellation

## Delivered bounded join

`OriginalWreckQueries50LA::canReleaseUnpublishedWorld` and
`releaseUnpublishedWorld` close the native cold-allocation cancellation route.
They require the actual same world identity and prove that all three source
queries remain unpublished, with no component binding, contact/derived/mutual
binding, collection filter publication, primary/portal registration, membership,
or membership revision. A detached published query is explicitly rejected.

`OwnershipBridge::close` invokes the typed release only after its existing
terminal bank, camera, scene, roster, query, registry and FIFO lifetime gates.
`canClose` merely inspects; it performs no cancellation. The release destroys
query-owned backing and its nested world/target/workspace borrows without
writing any canonical world definition, filter, geometry or selected-hit state.
Nothing was published by this owner, so there is nothing to restore or remove.
The terminal `retired_` latch forbids future preparation/rebinding.

The enclosing one-piece `constructCold` now also requires queries.prepared()
before touching its retained constructor/world bindings. This prevents a later
attempt from dereferencing the old world after a legitimate cold cancellation.
An already attempted source prefix remains terminal under its existing guard.

This is actual native pre-publication lifetime management. It is not a source
collision destructor receipt, a complete piece retirement, or permission to
continue a destruction handler. Published pieces retain the existing close
rejection even if all memberships were removed. No death/KO path is activated.

## Why published retirement remains a separate dependency

Static original source establishes these distinct operations:

1. Collection deactivation22CE10 removes model visibility28EA80, then calls
   query120290 for E0/E4/E8 at22CEBC/22CED0/22CEE4, ascending pieces. Existing
   native remove(i) performs the real membership removal, but retains published
   definitions and their aliases into owned filters/geometry/backing.
2. Entity slot10=142590 recursively retires children through1424E0 from the
   sibling tail, repairs links, then decrements reference00 through104970.
   Only zero references dispatch destructor slot08. Query destruction order
   is E8,E4, then D8/DC, then E0, following the reverse original piece children.
3. Collision component vtable3CCD08 slot08=1276D0 adjusts self by-8 and jumps
   to11B4A0.11B4A0 does NOT call120290. It destroys a residual registration
   ownerE0 via280920, frees filtersC0/CC/D8, calls shape/result cleanup11B600,
   retires embedded Entity through1413E0, and optionally frees allocation.
   The enclosing genuine unregister operation must already have happened;
   destructor-only cleanup is not a substitute for detachment281490.

Consequently a future published-query retirement API must be reached by the
actual typed piece child-release cursor, not a caller boolean saying "detached".
It must preserve source reverse child order and genuine reference ownership,
perform exact result/shape cleanup for each reached query type, remove aliases
from canonical tables before native backing disappears, and close actual
callback/detector/model/world borrows. The enclosing piece/model/effect/visibility
retirement is a separate owner sequence. No fake callback completion is valid.

## Integration prerequisites and limits

- The reservation's query and asset owners must outlive OwnershipBridge.close,
  as its existing input contract requires. Terminal close is serial and exclusive.
- The bridge still requires its real scene owner to be gone; canClose alone
  cannot authorize close while sceneOwner_ remains bound.
- After cancellation, no operation requiring a prepared query may use its
  unchecked query/world accessors. Existing owning runtime APIs reject via
  prepared/constructed/coherent, and constructCold now rejects explicitly.
- A failure after an earlier reservation was cancelled retains Storage. On
  retry, already cancelled queries remain terminal and are skipped; no source
  membership or publication operation is replayed.
- Real active/published retirement stays blocked. The new join covers a cold
  allocation whose first source query publication never occurred, including
  a setup failure after prepareCold but before publishColdQuery.

Source-only work against the isolated eed335c worktree, layered after the
one-piece22CA60 patch. No builds, tests, syntax/sanitizer checks, gameplay,
project decoder execution or guest instructions were run. Runtime behavior
remains unverified; root owns integration and validation.

## Exact later destructor branches for implementation planning

11B4A0 calls280920 at11B4E0, freesC0/CC/D8 at11B4F4/11B508/11B51C,
calls11B600 at11B528, and Entity1413E0 at11B534. Optional allocation release
is11B54C. By contrast120290 calls281490 at1202AC,280920 at1202B8, and clears
E0 last at1202C0.281490 removes the primary registry before secondary links.

Shape/result jump table3888F0 selects these actual wreck branches:

- E4 shape0:11B648 removes the matching manager3E1A70 resource-cache entry
  identified by shape124 through1327D0;11AEE0 releases shared shape references
  and backing. This cache/reference owner is not represented by simply deleting
  the native PodShapeState49W projection.
- E0 shape2:11B7E4 frees shape4C/50, then result90 arrays at offsets
  6C,50,70,54,68,64,58.
- E8 shape3:11B688 frees shape arrays4,8,C,10,14,18,1C, then the same result
  array sequence.
- Common11B8E0..11B904 frees shape8C, then result90, then clears both pointers.

Entity1413E0 decrements the same application423F60 and runs104910; parent-link
repair and reference decrement must already have occurred. Nonzero surviving
references cannot authorize query destruction. These branches remain unclaimed
by the unpublished cancellation API, which runs before any such source query
constructor publication or resource-cache insertion is represented.
