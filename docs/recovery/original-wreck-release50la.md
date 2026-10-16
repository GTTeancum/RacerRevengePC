# Original collection release22CA60: bounded one-piece native join

## Delivered route

`releaseOriginalOnePieceWreck22CA60` in `src/game/original_wreck_release50la.h`
composes the complete source22CA60 route for the existing, fully constructed,
authenticated one-piece collection. It consumes the same canonical scene,
physical body, effect owners and collision/visibility memberships as activation.
It creates no additional body or model and performs no asset loading at runtime.

The API requires:

- The retained `OriginalOnePieceWreckOwner50LA`, constructed from its actual
  archive/PHY/model/COL resources. Its selected collection must have one piece;
  group34 and group30 must both be zero. This does not supply the other authored
  racer374 entries.
- The real handler-selected body and body288 graph, scene projection, actual
  zone refs, and shared visual lazy fields, via `OriginalWreckActivationSource50LA`.
  Source body must be distinct from the destination piece body.
- Borrowed application-owned lazy guard bytes gp-780C/gp-7808, corresponding
  scale words434870/434878, and the existing shared308030 raw RNG state. Retain
  them across all collections and racers. A new set per call is not admissible.
- The owner retains its constructor-global binding. Its new read-only reset
  accessor lends that SAME application's reset globals for167A50.

## Exact source order

1. 22CA74..22CABC initializes each zero lazy guard independently, storing
   scale41A00000 or4392AA7F before setting its guard byte to1. Already initialized
   live values are preserved. These writes precede activation.
2. 22CAD0 calls actual22C6A0, retaining its complete placement/registration and
   aggregate/body-enable effects.
3. 22CAE4 invokes group slot18=2CC9C0. Existing concrete effect controls reset
   and stop sparks, reset and stop explosion, and stop the primary splash in
   order, then group141520 runs. Actual source-null effects skip their calls;
   a missing binding is not treated as null.
4. 22CB00 sets collection38[0].flight34=1.
5. 22CB14 calls group2CA2F0. For the genuine one-piece group, capacity34=0:
   2CA320 branches to2CA440,2CA448 loads zero,2CA450 falls through to2CA458.
   This is a true no-write branch. There is no primary detach or recompute to
   invent, and it does not call123FA0 for this route.
6. 22CB34 invokes piece slot20=2CBFB0, which clears12C and activates its base.
   22CB54 ORs piece.mask20 with2 directly (no ancestor-mask rewrite).
7. 22CB70 invokes body slot90=1653C0(true);22CB94 invokes slotAC=167A50 using
   the actual retained application reset globals. The existing implementation
   resets the canonical load, velocity and derivative lanes in source order.
8. The existing22CB9C..22CDE4 numerical leaf consumes six shared308030 draws
   and writes linear70 and angularB0, with both W lanes1. The loop ordinal
   increments at22CD14, and count1 terminates at22CDE8..22CDF4. Return22CE08.

The result reports reached prefixes for diagnosis; only status `complete`
means this bounded collection call returned. It is not a death-handler,
whole-frame, camera-debris-array, or KO-credit completion receipt. All successful
source prefixes survive a later rejected dependency. Entry must not be replayed
following failure. The existing launch leaf's bounded numeric admission still
applies; a rejected tail leaves its own RNG/velocity publication untouched while
all earlier release writes remain committed.

## Integration and remaining ownership

No ordinary-race caller is enabled in this patch. The active race currently does
not instantiate the one-piece owner; existing code is a genuine cold owner API,
not proof of a live registration. Its resource bindings must come from the
actual retained racer archive and process services before a handler can call it.

The existing asset layer already parses the complete ordered PHY Wreckage.PartN
and each collection's PartN authoring. It loads geometry/queries only for the
unique one-piece chariot selection. Simply allocating N one-piece wrappers is
incorrect for multi-piece groups: they need one shared aggregate over distinct
piece bodies, real attached-slot arrays, per-piece effects, and mutual query
exclusion membership.

For multi-piece22CA60,2CA2F0 must additionally rebind the released piece's contact
body (295C80), set its mask/wake, remove mutual E0/E4 exclusions through123FA0,
clear its aggregateA0 slot, and recompute the same primary mass/inertia/center
through2CA620. These operations are source2CA33C..2CA428, nested in the actual
capacity34 scan. The zero-capacity proof cannot stand in for those operations.
The original full22C6A0 activation, group2CA480 attach, nonzero2CA620, collision
exclusion topology, and nonprimary tuning must be recovered together.

Keep the selected-destruction rejection until actual ordered collection owners,
intact-racer removals, full2261B0/226750 handlers, their query/callback tails,
and source225EC8..22618C continuation are complete. No credit or AI control
policy changes are part of this patch.

## Evidence and validation boundary

Source-only inspection of authenticated SLUS_202.68, SHA256
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Raw word annotations: `/workspace/shared/wreck-collection-audit-20261016/`
`release22CA60.txt`, `detach2CA2F0.txt`, `group-deactivation2CC9C0.txt`.
Existing native component, activation, effects, reset and launch implementations
were inspected as source. No build, test, syntax/sanitizer check, decoder check,
gameplay, or original instruction execution was performed. Runtime behavior is
unverified; root owns integration and validation.

## Complete racer collection factory and retirement split

Static factory audit corroborates the two-level ownership graph:

- Racer20FA14/20FA1C initially zeros370/374.2129C0 reads Wreckage.NumberOfParts
  into370;2129CC/2129D4 allocates/stores374. Ascending PartN iteration allocates
  collection0x48 at212A18, constructs22BF00 at212A2C, stores374[index] at212A4C,
  calls22BFD0 at212A90, then appends with141960(maskFFFFFFFF) at212AAC.
- Collection22BF00 initializes141300 and zeros30/38/3C/34.22BFD0 reads its own
  NumberOfParts, allocates38, and visits ascending PartN: piece allocation0x180
  at22C128,2CB4B0 constructor22C13C,38[index] store22C16C,2CB5A0 setup22C1A0,
  append mask2 at22C1B8. Only after all pieces: group allocation0xD0 at22C278,
  2CBFC0 constructor22C28C,3C store22C294,2CC060 setup22C2B0 with capacityN-1
  and primary38[0], append mask2 at22C2C0.
- Separate racer tuning pass calls22C310 at215B70 and22C610 at215B98, ascending
  collection order. Factory setup alone is not fully tuned ownership.
- Racer/collection/piece/group slot10 is142590.142590->1424E0 releases children
  in reverse sibling order, then unlinks/releases through104970; destructor
  slot08 runs only when the reference count reaches zero. Thus collection
  release is group first, piecesN-1..0, then collection. Piece children release
  E8,E4,D8,DC,E0,D4,D0,model14C. This differs from ascending deactivation22CE10.
- Collection22BF50 frees38 at22BF84, then1413E0 at22BF90. Racer2104D0 frees374
  at2105BC, then1413E0 at210730. Piece2CB500 frees160/164/170 at2CB534/548/55C,
  then165BD0. Group2CC000 calls2C9F30, freeing44, backing48-0x10,A0,A4,A8,B0,
  then1413E0. Host deletion without these membership/lifetime effects is not
  an original source retirement join.

Actual retained input sites: `native_racer_rig50hw_fixture.h:76–83,106,120`
retains each Rig's RES path/PHY/model identities; `ra_cold_roster_resources50la.h:
47–67` authenticates RES/PHY and existing model/motion/intact COL data;
`ra_cold_roster_operation50la.h:388–397` mounts the complete same RES in the
Application provider. Fragment assets must be acquired from those same retained
original resources, preserving model identities and source ordering.

Lifetime prerequisite: `ra_source_camera_ownership50la.h:1103–1108` explicitly
rejects retirement while any prepared wreck retains world storage, even after
membership detach. The typed piece-retirement join is still missing. This patch
must not bypass that guard or claim lifetime closure.

Recommended independent next owners: (1) full ordered collection assets/pieces,
(2) nonzero group2CA020/2CA480/2CA620 and exclusion topology, (3) piece/group
source retirement and query-world borrow release. Only after these join should
full racer370/374 publication and multi-piece22CA60 be enabled.
