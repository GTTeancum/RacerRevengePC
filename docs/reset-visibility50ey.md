# Vehicle reset visibility 50EY

`src/game/reset_visibility50ey.h/.cpp` supplies concrete native vehicle7F0
registration removal and reattachment for216A30/2167A0. It uses bounded
caller-owned SearchList/membership storage. There is no hot allocation, guest
execution, success callback, collision-reference substitution, or frame-phase
advance. Source was manually inspected; no build, test, or game launch was run
for this addition. Compilation and integration verification belong to the root.

## Actual separate visibility ownership

`ResetVisibilityWorld50EY` contains an independent50CL lifecycle and50EN portal
world. Its backing must not alias the collision4C4 owner. Stable native refs map
authored zone ordinal+1 in this namespace; equal numeric refs in two independent
tables do not make their owners interchangeable. The vehicle's `object30` is its
visibility base identity, not690,694,7A4,348 or either spring-query identity.

The retained original source establishes why50EN's portal geometry/registration
mechanics can be reused with independent storage:

-272A58..272AE4 constructs distinct4C0 and4C4 zone objects, each carrying its
  own zero-based authored ordinal at+48.
-272B3C..272C70 visits authored portals in ascending order.272BC8 and272C10
  call27F3B0 for opposite directions in4C0;272BEC and272C34 do the same in4C4.
  All four calls use the same original230450 portal object.
-27F3D8..27F3F8 prepends each distinct directional record, with its own mutable
  distance cache. Therefore each namespace has the same incident order and
  geometry, but separate directional records and SearchList registrations.
-2809C0 calls27F590 with the supplied mode. Mode2 selects zone+38, the second
  method. This is `ZoneMethod50CL::Second`, not a source mode1 registration.

The cold caller must read each retained zone with
`assets::readZoneSearchSelection50CN` and admit the actual
`visibilityDynamic114 == List`. Collision methods alone do not prove that
choice. Current50CL represents bounded SearchLists only. The static visibility
method, original draw manager, other visibility objects, and QuadTree admission
are separate integration work; this seam does not manufacture them.

## Caller-owned storage and sequence

Initialize the embedded `lifecycle` with `initializeZoneLifecycle50CL` and
`portals` with `prepareLiveZoneWorld50EN`, using dedicated4C0 backing and the
authored portal inputs. The latter helper's retained collision-oriented name
does not change the proven identical4C0 portal construction above. Reserve one
stable visibility object ref per actual owner. Each registered owner needs its
own50EN links/entries. Storage for one primary plus the authored portal count
is a conservative fixed bound. The reusable50EN scratch can be shared serially
between namespaces when sized for both; live registries/portal caches cannot.

`ResetVisibilityOwner50EY` retains the real vehicle membership7F0 and borrows
the sole projected `*(vehicle254)+50` field. Bind that pointer even if its next
operation is an unconditional clear. Attach never invents a nonzero write to it.
Do not create a second scratch field merely to satisfy this binding.

The undertrack order is:

1. After216A50..216A84, call `removeResetVisibility50EY` before any of the four
   collision-owner removals. This executes the actual281490 primary removal and
   linked secondary removals through50EN, clears7F0, then clears254+50. A real
   null7F0 follows the original skip and leaves254+50 unchanged. Missing owner
   identity, incoherent lifecycle registration, or aliased backing rejects.
2. Remove690,694,7A4,348; perform the initial scene/body placements through50EW.
3. Execute `prepareUndertrackPodQuery50EW` for source21684C/21685C. It must
   prepare the canonical actual694 from main190 and the authored type0 root.
4. Call `attachResetVisibility50EY` with the canonical frame, scene binding,
   actual `parameters.shape`, and placementC30. It verifies694 is detached
   type0, main190 is ready, and its world cache matches694.world80. Bounds are
   derived from694.composed00 and the authored root through50EW's type0 leaf.
   The actual cached694 radius is supplied to2809C0's portal overlap scan.
5. Continue50EW collision attachment690,7A4,348,694. The second694 preparation
   at21698C remains required; visibility preparation does not replace it.

Source216820/280890 allocation and216AB8/280920 destruction are represented by
stable native storage reuse. They are not heap emulation, pool accounting, or
proof of original allocator side effects. The membership state becomes live
only after actual registries and views publish. On failure, live state and254+50
are preserved; only supplied scratch may change. Successful calls cover their
named slices and do not authorize bypassing228BBC or completing2255B0.

## Camera remains a real required boundary

The source225B00/225B04 skips the camera tail only for actual `vehicle58 != 0`.
The supported player context uses58=0. There is no null-camera skip in178860:
it unconditionally uses controller3C and64. The host50DG chase view therefore
cannot justify omitting the source camera reset.

Minimum source-owned inputs for that separate integration are:

- The actual vehicle5C player index and the corresponding
  application28C.player48[index] controller.
- Its camera scene3C, distinct visibility object30 identity, current
  registration34, saved38 destination, and position180 packet.1148D0 removes
  that scene's registration.178894 copies the original pre-lift spline anchor
  to180.114770 attaches it with visibilityC30/mode2 and zero overlap radius,
  using actual constant3E0060 half bounds.
- Its actual camera64 and virtual18 dispatch identity.17BCE0 clears that
  camera's180/1A0 packets from the shared433DB0 packet before178860.
- The selected collisionC34 and the original pre-lift spline anchor. Final
  ground-adjusted BC0 is not a replacement for that anchor.
- For the known follow-camera table3CE440, virtual18 is179A80. That routine
  requires the actual sphere-query24, current radius178, segment-query190,
  endpoint/metadata backing, filters/callback ownership, and two real collision
  registrations. It removes/reprepares/reattaches both owners in source order
  and clears motion packets. Missing query owners enter real initialization
  paths; a null pointer is not a source skip.

The visibility lifecycle can reserve a distinct camera ref and support its
actual114770/1148D0 owner. The vehicle7F0 API deliberately does not impersonate
that camera registration or implement the camera scheduler.

## Source identity and boundaries

Static instruction reads used the retained `work/native/project/disc/SLUS_202.68`,
SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Relevant ranges were2167A0..2169B4,216A90..216AC8,272A58..272F20,
27F3B0..27F410,27F590..27F5C0,280890..280BEC,281490..281584,
114770..11491C,178860..1788DC,179A80..179C98,17BCE0..17BCF8, and
the retained2255B0 listing. This was static source inspection only.

The sealed49W files, excluded49C/48U work, and unapproved `ee.bin` were not
changed. No user-computer access was used. Useful focused root verification is
real attached removal/reattach across distinct visibility/collision namespaces,
cached-radius portal overlap, preservation on capacity failure, detached skip
preserving254+50, and rejection of stale694, aliasing, and wrong object refs.
