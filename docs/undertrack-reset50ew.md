# Undertrack reset50EW: physical and collision-owner join

New production files: `undertrack_reset50ew.h/.cpp` and
`type0_bounds50ew.h/.cpp`. The latter also supplies a minimal tag0 dispatch
extension to `live_zone_membership50en.h/.cpp`; existing tags are unchanged.

## Concrete operations

- `removeUndertrackCollisionOwners50EW` removes real690,694,7A4,348 memberships
  in216ACC..216AE8 order. All four owners must exist before it starts. Its cursor
  preserves earlier successful removals on a later rejection;694's canonical
  E0 marker clears only after the actual registry removal.
- `placeInitialUndertrackPhysics50EW` executes216BD0..216D64 on the current49W
  scene forest: initial main/hidden poses, actual main/lifecycle reset, initial
  displayed/secondary poses and actual secondary reset. It explicitly rejects
  unbound/present terrain330 or water334 rather than ignoring height providers.
- `prepareUndertrackPodQuery50EW` performs the detached694 transform/history /
  sphere preparation at21684C and exposes real tag0 bounds for visibility7F0.
  The50EY visibility adapter joins here using its separately owned namespace.
- `attachUndertrackCollisionOwners50EW` prepares and attaches690,7A4,348,694
  in21690C..2169A0 order.690/348 use actual type2 resources, including the
  rootless one-pair348 route.7A4 endpoints come from canonical main190 transformed
  body1E0.694 is prepared a second time in its actual source position. Mode2
  maps to the native second method; collision overlap radius is source zero.
- `finishUndertrackPhysics50EW` consumes the actual Hit-state50AG ground result,
  performs final main/hidden/chariot/secondary/right/left scene placement and
  all three model-bound updates, then main-body reset, prior7B0/850 stores,
  actual effect Stop84 calls, four820 timer clears, and canonical D28 clear.
  It preserves damage, progression, engine timers, body integration poses,
  unrelated caches and every frame/body phase. Sourcea1=0 does not run the
  full-initialization block at217374.

The two scene/body slices stage canonical state and commit atomically. Registry
calls are real serial transactions: earlier owners remain committed if a later
owner rejects. `SourceSliceComplete` means only the named slice succeeded.
No callback, completion boolean, artificial ground hit, grid teleport or Y clamp
stands in for an original effect.228BBC and the existing49W guards are unchanged.

## Required caller data and order

Use the prepared50ET placement from live50EU progression. Remove visibility7F0,
then the four collision owners; apply the initial physics slice; prepare694;
reattach actual visibility7F0; reattach the four collision owners; execute the
actual source ground collector/query; finish physical/effect reset; finish the
camera sequence. Player/D40/D44 prefix leaves remain required before these steps.

Caller storage must include:
- Four distinct actual query refs/membership chains. The existing main and
  secondary spring refs cannot stand in for694/7A4/348.
- Actual690/348 type2 query caches/resources,7A4's real two endpoint / metadata
  words, tag and sphere, and canonical `frame.shape` for694.
- Three complete authored model-bound spans in chariot/right/left order, with
  persistent result owners; all real effect owners and850 storage.
- Actual2F0 offset, canonical2C0/2B0 parameters, shared activation/lifecycle
  records, and the resolved50AG probe. Exhausted probes still need their source
  diagnostic and therefore reject this bounded final-placement adapter.

50BA's dirty-descendant admission is retained. Source scene setters must produce
that state naturally; no cache flags are coerced to pass it. Body position and
orientation are not manually synchronized: original167C40 reloads those from
the actual scenes when physics advances.

The authentic694 attachment now produces nonzero `frame.shape.registrationE0`.
The current49W admission and49W late shape updater still require current-null
694. They must acquire their real registered694 update route before this reset
can return to full advancing frames. Do not erase its E0 to fit the old path.
Visibility7F0 and camera-scene / follow-camera query resets are separate owners,
not optional successful omissions.

## Tag0 bounds closure

The exact120378..1203F8 branch reads geometry-root+0C/+18, forces both W lanes
to1, and copies shape00 before entering120C50's already-native common bound
tail. `prepareType0Bounds50EW` does precisely that, preserving cached sphere
words. Its `composed00` input is deliberately distinct from shape.world80.
50EN retains actual tag0 in its receipt and passes this real resource projection;
it does not relabel the query as a box or spring.

Evidence: retained `evidence/resetProbe50AG/reset216a30.txt`,
`evidence/collectionBounds48S/map/candidate_bounds.txt`, the source ranges in
`undertrack-reposition50et.md`, and existing49W/50BA/50BK/50EN proofs.
No tests, game execution, guest execution or PC/device access were performed by
this worker. Root owns focused compilation and live integration verification.

## Concrete cold owners

`tests/native/undertrack_owners50ew_fixture.h` is an integration fixture, not a
test runner. Its `prepare` loads actualPOD01.RES, fully decodes podboxf into an
owned native mesh/source and verifies the source against canonical694. It also
re-decodes actualpodboxm using only the existing owner's opaque W lanes and
compares all node/ray/matrix values to existing690. No secondary-query identity
or resource is substituted for a missing owner.

Original2121C8 writes690 bounds to vehicle310/320.21491C consumes half-height324;
21494C passes offset `(0,324,0)` and authored Repulsor.MaxLiftHeight to1B1240.
That producer writes first130=`body1E0+offset`, second140=
`(first130-2*offset)-(5*lift)*shared434280`. Startup3C9B90..3C9C0C proves
434280=`(0,1,0,1)`.11C3F0 /113070 /112D30 /113880 create the one-pair,
rootless0x8000 type2 resource. The fixture implements these concrete geometry
operations and initializes7A4's source single-pair storage separately.

Construct `Owners` at its permanent address; it cannot be copied/moved after
its borrowed views are formed. Call `prepare`, then `bindProbeLocal140` during
cold assembly so the disabled probe and the new348 query use the same source140.
`binding` accepts the existing690 owner and three distinct newly allocated refs
for694/7A4/348. Membership backing and filter vectors allocate only in preparation.
The caller must increase the native collection/lifecycle object tables and use
these same owners for later query preparation; a second disposable fixture is
not a runtime binding.

Application mask-registry results3E0718/3E0720/4342A0 and extra3B14B0 remain
explicit `SourceGlobals` inputs, as do native identities, source+31 flags,
preservedF8 high bits and shared sentinel3E02C0. The concrete constructor modes
5/3/7 and category-list modes/entries are applied in the fixture. Saved-RAM
values, nearest-zone guesses and another query's entire filter are not defaults.
These sources must be bound before publishing the new objects to live collection.
