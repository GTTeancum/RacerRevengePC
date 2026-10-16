# Complete ordered original wreck asset inventory and cold loading

## Delivered

`original_wreck_asset_inventory50la.h` owns the full authored collection/piece
inventory and a stable asset owner for each successfully loaded occurrence.
It uses the existing authentic PHY/RES/PSG/COL readers. No decoder format or
vertex-attribute admission has changed. No original bytes were decoded during
this source-only implementation.

`inventoryRetained` consumes actual retained RES bytes and retained racer PHY,
extracts that PHY from the same archive, and requires byte equality. The actual
PHY Pod Setup.Prefix must match the archive member prefix and PHY identity.
All Wreckage.PartN collections and every collection PartN are kept in their
original nested order, including repeated stems. No sorting, grouping by name,
mesh substitution, intact-body fallback or occurrence deduplication occurs.
The full decoded original archive remains retained for later cold loads.

Each inventory occurrence records its one-based collection and piece ordinal,
original SLT request, resolved PSG and m.COL/f.COL names, exact archive entries,
and uniqueMemberFound flags. A false flag means missing or ambiguous identity;
it is never evidence of an empty piece. Inventory can succeed while describing
such a missing source dependency. Actual loading rejects it precisely through
the same existing member lookup.

`loadNextCold` visits collection0/piece0, then each subsequent piece, then the
next collection. It calls the same shared `loadSelectedPiece` implementation
as legacy one-piece preparation. Success publishes only that occurrence's
complete geometry/query/target owner. Failure leaves that slot unloaded, keeps
the cursor at that occurrence, and retains earlier completed pieces plus the
entire authoring inventory. It never skips an unsupported source piece.
`complete()` becomes true only after every occurrence has loaded successfully.
Failures return the source collection/piece ordinals and error text; the last
successful call also identifies its loaded occurrence.

Assets retain the full original collection authoring while exposing the exact
selected piece through `selectedPiece()`. No synthetic single-piece collection
is created around a fragment. The old `prepare` entry still requires the unique
one-piece chariot and defaults to piece0 exactly as before. Full inventory
parsing does not require that legacy selection; if it is not unique, its
inventory-wide selectedCollection is the collection count sentinel. Per-piece
loaded assets always carry a valid actual collection index and piece index.

The endpoint policy receives both original collection and piece authoring plus
the actual COL ray ordinal. Missing opaque endpoint W ownership rejects loading;
no W value is inferred from COL data, defaulted, or borrowed from an intact body.
Constructor vectors remain a borrow of the actual application-owned globals.

Native storage admission retains the existing512 total authored piece bound,
existing individual readers' bounds, and a128MiB aggregate source-byte ceiling
using DecodeLimits48C.resource_decoded_bytes. That aggregate counts every loaded
occurrence's PHY and PSG/m.COL/f.COL bytes, including repeated occurrences.
Capacity rejection is explicit, not source omission or simplification.

## Concrete retained-racer integration

`tests/native/original_wreck_rig_inventory50la.h::inventoryRetainedRig` accepts:

1. The existing retained `native_racer50HW::Rig`.
2. The complete retained bytes of that same RES.
3. The actual application constructor-vector owner, retained for later loads.
4. The fresh full inventory owner.

It resolves the original racer selection from Rig.podId, checks the RES size and
SHA256 and retained PHY SHA256 against existing source identity pins, then calls
the inventory with that Rig's actual resource path and original PodNN members.
It opens no file and selects no generated cache. This is the concrete cold join
for the parent aggregate; no active race callsite is enabled in this patch.

After inventory, bind actual endpoint metadata and call loadNextCold in source
order. Then the aggregate can reserve actual per-piece collision/visibility
capacity using loaded pieceAssets(c,p). The source endpoint policy itself must
already be backed by authentic constructor/opaque-word ownership; reservation
does not establish those values. Loaded assets are owned by the inventory and
remain address-stable.
The current one-piece runtime owner still owns its own assets member; a future
typed adoption/borrowing API is required before it or full multi-piece owners
consume these retained assets. Do not reload a second mutable graph and claim
it is the same owner, or copy these projections into fabricated source pieces.

## Original source anchors and remaining work

Racer2129C0 reads Wreckage.NumberOfParts,2129D4 stores374, and212A18..212AAC
constructs/appends each collection in ascending PartN order.22BFD0 constructs
pieces in ascending PartN order: allocation22C128, constructor2CB4B0 at22C13C,
store38[index]22C16C, setup2CB5A0 at22C1A0, append22C1B8. Only afterward is the
shared group constructed at22C278..22C2C0, with primary38[0] and capacityN-1.
Separate tuning calls occur215B70/215B98.

165C30's model slotA8=240740 reaches26E820;26E8F4..26E934 substitutes original
'.psg' for the SLT extension.165F80..166020 derive the same stem's m/f collision
members. This patch uses that existing source-backed naming rule, not a new SLT
binary interpretation. No MCF/material/texture or particle owner is claimed by
loading model geometry alone.

Remaining dependencies:

- Typed runtime adoption of these actual per-occurrence graph/resource owners.
- Full piece Entity/body/gravity/detector/query/effect factories and identities.
- Shared nonzero-capacity group2CA020/2CA480/2CA620, contact rebinding and mutual
  exclusion topology; no array of one-piece groups can replace it.
- Original tuning per collection and source order, then runtime registrations.
- Material/texture rendering and any reached damage-emitter arrays.
- Genuine component/query/visibility/effect retirement, including resource-cache
  references and actual world-borrow lifetimes.
- Existing unsupported original geometry must be recovered separately. The
  reader remains strict; a report of unsupported attributes is not permission
  to invent a format or substitute an intact model.

This is asset readiness only. It does not publish racer370/374 runtime objects,
produce a camera debris view, alter AI control, enable death handlers, grant KO
credit, or weaken lifetime gates.

Source-only against26cf869 in an isolated worktree. No build, test, syntax or
sanitizer check, gameplay, project decoder execution, or guest instructions.
Root owns integration and runtime validation; no runtime success is claimed.
