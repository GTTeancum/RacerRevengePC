# Typed original ordered wreck storage: cold ownership boundary

`OriginalWreckRacerStorage50LA` exclusively adopts a complete
`OriginalWreckAssetInventory50LA` and owns stable nested collection/piece arrays.
It is the smallest ownership join between retained original assets and future
racer370/374 construction. No source runtime array is published by this change.

## Concrete ownership

`adoptCompleteInventory` takes the caller's unique inventory owner by reference
and the actual retained session generation/racer key. It validates all authored
ordinals and selected asset identities, then allocates final-size backing:

- One collection owner per original Wreckage.PartN, with its ordered pointer slot.
- One piece owner per original collection PartN, with distinct ordered pointer
  slot even when several occurrences share a stem.
- Each piece borrows its OWN existing asset object and graph from the adopted
  inventory; no asset is decoded again, copied into another mutable graph,
  replaced with an intact body or wrapped in a fake one-piece group.
- Each piece embeds its canonical future physical, piece and gravity backing,
  and allocates sceneCount+8 source-unconstructed component slots: piece,
  model/scene rows, body, gravity, E0, contact, detector, E4, E8.
- Each collection reserves a collection component and one group component.
  No group aggregate fields, bodies, slots or completion are fabricated.

The selected asset must match both original ordinals and the authored stem.
All allocations and validation finish BEFORE the inventory is transferred.
Failure preserves the caller's inventory and all asset addresses. Success
moves only ownership of the existing inventory object. The inventory is
stored before borrower arrays, so it is destroyed last. The owner and all
collection/piece types prohibit copying/moving; there is no reset or rebuild.

C++ default initialization of native backing does not execute22BF00,2CB4B0,
165E90,2CBFC0 or2CA020. Component kinds remain unbound, source reference/key
fields stay zero, body constructor flags stay false, and body/contact/gravity
aliases stay null. In particular gravity.target50 must remain null until its
real initializer runs; prebinding it would make that initializer reject.

## API and integration

The result status is explicitly `coldBacking`, not `complete`/`constructed`.
`coldBackingPrepared()` describes only native storage. Bounds-checked const
`collection(i)` and `piece(j)` expose authored order, exact stable assets,
planned counts, and read-only unconstructed backing. `plannedPrimary()` names
piece0 as authoring information; it does not populate group40/3C. No mutable
container, source readiness flag, runtime component domain, camera conversion,
query callback receipt, active racer374 view, or destruction entry is exposed.

The driver includes both the prior retained-Rig inventory adapter and this new
header for root's next full build. There is no runtime callsite or changed AI,
death, KO, renderer, or lifetime admission. No local compile was performed.

A future cold aggregate can create/load an inventory through the existing
retained-Rig adapter, then transfer that unique owner into this storage. Actual
source construction must be implemented as methods on this owner so its private
canonical backing is used; there must be no separate duplicate mutable pieces.
Before enabling construction, the owner needs the genuine external component
forest, application counters/lock, piece component identities, query namespace,
visibility/material/effect providers, and typed retirement continuation.

## Source order and explicit remaining blockers

2129C0..212AAC reads the actual racer collection count, allocates374 and
constructs each collection in ascending PartN order.22BFD0 constructs/appends
all its pieces in ascending PartN before the shared group. The group primary is
piece0, and its nonprimary capacity is count-1. The new arrays preserve exactly
that nested order. They do not execute those source calls early or parallelize
their observable mutations.

1. Existing `OriginalWreckQueries50LA` is typed to
   `OriginalOnePieceWreckOwner50LA` (owner9C and prepareCold). New pieces cannot
   masquerade as that type. A real shared typed piece interface must bind the
   same canonical fields and callbacks before query construction is enabled.
2. Nonzero-capacity group2CA020/2CA480/2CA620, per-piece query exclusion and
   contact-body rebinding remain missing. A group per fragment is incorrect.
3. Existing component-domain helpers have a64-component runtime admission.
   Cold assets admit up to80 scene nodes, and complete collections may exceed
   that domain cap. This storage preserves the actual requested capacity;
   it does not discard valid assets or silently relax runtime checks. A bounded
   larger-domain implementation needs separate source review.
4. Collection/effect/query/visibility construction, original tuning and source
   child/parent publication remain unimplemented for these arrays.
5. Reverse Entity/refcount retirement and original query/shape/resource-cache
   destruction remain prerequisites. Ordinary C++ teardown is valid here only
   because no source object or external membership was ever constructed.

The completed inventory and cold backing therefore cannot authorize2261B0,
226750, a death camera debris scan, or KO attribution. Existing guards remain.

Source-only against3766a3c. Independent static design review found no cold
ownership blocker. No builds, tests, syntax/sanitizer checks, gameplay, project
decoder execution, or guest execution were performed. Runtime behavior remains
unverified; root owns integration and centralized validation.
