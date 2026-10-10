# Original UI route-decision leaves (50KG)

## Scope and verification

`src/game/original_ui_route_decisions50kg.h/.cpp` add bounded, allocation-free,
native C++ confirm/back decision leaves for original OptionsMenu, GalleryMenu,
RiderSelect and the closed CourseSelect back routes. Course activation is
prospective metadata only and always returns `unavailable/courseLaunchOwner`.

This is new source-backed reconstruction, not restored historical source. The
changes received source/text review only. No compiler, syntax checker, test,
asset importer/decoder, gameplay executable, guest instruction, interpreter or
emulator was run for this work. Registration and all execution/verification
belong to the root's centralized permitted slot.

The evidence is retained under
`recovery/20261010/ui-route-reconstruction/evidence/`. Its README records the
original `SLUS_202.68` SHA-256 as
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The evidence's static VA-to-file mapping is `VA - 0x100000 + 0x80`.

## Ownership and rejection contract

- Event 4 is activate and event 6 is back. Directional events remain unsupported
  here, so a page owner cannot accidentally move selection twice. Rider
  directional events 0/1 require the separate rider page/data owner.
- Menu selection comes from controller 0's attached group and its original
  one-based slot, preserving holes, real control IDs, enabled state and blocked
  state. `0x2A6F60` follows the controller record to its group and reads group
  `+0x3C`; the relevant cut is also retained in
  `ui-static-membership-reconstruction/evidence/generic-render-2A6780-2A6FD0.txt`.
  The source-page owner must authenticate the actual page/screen pairing.
- These functions never mutate navigation, profile, manager, renderer or save
  state. They never invoke callbacks, complete an animation or launch gameplay.
  Source PCs and numeric profile field values are provenance, not pointers.
- Invalid page/group/slot/controller/rider/course values and unsupported inputs
  leave the output unchanged. Suppressed controller/blocking inputs publish an
  empty `noChange` decision. An unavailable result may contain a prospective
  route and profile changes solely to describe the source branch; none may be
  committed.
- A caller must require returned `ok`, `admitted` and
  `profileProposalComplete`, verify source screen/page and snapshot freshness,
  and atomically accept the whole profile/request transaction. A rejected
  manager request must not leave any remembered or packed profile changes.
  Generic `requestScreen`/`requestPage` do not perform these profile changes.
  No integrated profile transaction owner is claimed by this leaf.
- Source controller, selection owner, profile owner and destination page
  admission are independently closed by default. A destination needs both its
  recovered bounded factory identity and an explicit readiness bit backed by
  actual native controller/assets/callback owners. A registry match alone is
  insufficient; setting a bit cannot manufacture a missing identity.
- The currently unrecovered bounded page identities for gallery screens 1/2/3
  and HOF screen 11 keep those destinations unavailable even if a mask bit is
  mistakenly set. Their exact prospective route IDs remain inspectable.
- `word` changes replace 32 bits. `maskedByte` and `maskedHalfword` changes
  preserve bits outside `replaceMask`. Changes are listed in source profile
  write order; this is not a complete interleaving of page flags, sound,
  transition-helper calls and profile writes. Those effects still need owners.
- Fixed native bounds are at most 12 proposal entries, existing page/control
  limits, riders 0..31 and one-based course/subtype slots within the supplied
  group count (at most 256). These are fail-closed admission bounds, not claims
  about the original complete roster or course count. No rider32-to-rider0 or
  invalid-slot fallback is permitted.

## OptionsMenu: screen 12/page 1

Source: `options-input.txt`, `source-tables-and-rtti.txt` and
`original-mode-store.txt`.

- Handler `0x1F9640` accepts controller 0 and checks popup state first.
- The selection table at `0x3B00F0` maps slots 1/2/3/5/6 to same-screen pages
  2/3/4/5/6 respectively. Slot 2 requires the second controller. A missing
  controller returns an explicit request for the source controller-required
  behavior, without rendering a substitute popup or committing the proposal.
- Slot 4 requests screen 11/page 1 and sets original profile mode `+0x34` to 5
  through `0x17DE50`. If the old mode differs, the old value is proposed at
  `+0x38` before writing 5 to `+0x34` (`0x17DE64/6C`). If it is already 5,
  neither mode word is changed.
- Accepted activate branches remember the one-based slot at `+0x6E4`
  (`0x1F9940`). Back remembers it at `0x1F9994` and requests screen 4/page 1.
- The original controller-required branch also reaches the remembered-slot
  store. The native rejected decision is intentionally all-or-nothing: that
  prospective write cannot be applied separately from the rejected request.
- Controller configuration, settings application, memory-card persistence,
  credits and HOF page behavior are not supplied by these route leaves.

## GalleryMenu: screen 4/page 5

Source: `gallery-input.txt`.

- Controller 0 activate maps slots 1/2/3 to screens 1/2/3, each page 1, through
  `0x20CF6C`, `0x20CF88` and `0x20CFA4`.
- Back requests same-screen page 1 at `0x20D014`.
- The source remembers selection at profile `+0x71C` on activate (`0x20CFDC`)
  and back (`0x20D048`). These writes are retained as typed proposals.
- Activation needs the actual availability owner and the selected enabled bit.
  That owner must derive availability from profile `+0x11018` bits 5..6 and
  original enable callbacks; this leaf does not invent their missing setup.
  Bound/present controls or existing art do not establish an unlocked gallery.
- Directional behavior is outside this leaf. In the source, events 2/3 check
  the profile availability count before calling group previous/next. A generic
  unconditional directional handler would lose that original gate.

## RiderSelect: screen 5/pages 1 and 2

Source: `rider-input.txt`, `rider-page-factory.txt` and `rider-ctor.txt`.

The original constructor player index `page+0x4C0` selects controller 0/page 1
or controller 1/page 2. Original `page+0x4C4` is a rider value, not a generic
one-based UI slot. Popup/blocking inputs are suppressed. Activation requires
the real nonzero profile unlock word at `+0x10FAC + rider*4`; defaults or asset
existence never unlock a rider. Back does not require an unlock/comparison row.

Page 1 activate preserves these source profile writes:

1. Remember rider at `+0x6F0` for mode34=0, `+0x6F4` for mode34=1, otherwise
   `+0x6FC` (`0x1EA538/548/54C`).
2. Replace only byte `+0x1125D` bits 3..7 (`0x1EA58C`).
3. Write original rider value to `+0x100C0` (`0x1EA5A8`).

Human-count bits `packed1125C[6:5] == 1` request screen 9/page 1 for mode34=2,
otherwise screen 6/page 1. Value 2 requests same-screen page 2. The source has
no route for values 0/3, so native admission rejects them atomically. Page 1
back remembers the mode-specific rider then requests screen 4/page 1.

Page 2 activate replaces only byte `+0x1125E` bits 0..4, conditionally changes
`+0x100C0`, remembers selection at `+0x6F8`, then requests screen 6/page 1.
Back remembers `+0x6F8` and requests same-screen page 1.

### Rider2 comparison boundary

`0x1EAB80..0x1EABEC` resolves two actual profile rows with stride `0x894`.
The current comparison rider is the value already at `+0x100C0`; the candidate
is the packed second rider. Each row supplies words at row offsets `+0x2190`
and `+0x2194`. The source subtracts with 32-bit wrap (`SUBU` at `0x1EABC4` and
`0x1EABD4`), then uses strict signed `SLT` at `0x1EABD8`. It replaces
`+0x100C0` only if the candidate difference is smaller. Equal differences
preserve the current rider.

The small evidence printer marks word `0x70851018` at `0x1EABAC` unknown. Its
MMI function-24 encoding is `MULT1 v0,a0,a1`; the repository's retained
`src/core/mmi47k.cpp` switch and `MULT1` implementation independently document
that low-product register destination. They were read only, never called or
linked by this leaf. The adjacent ordinary multiply resolves the same current
row for the other word. No guest CPU state exists in the new implementation.

The native leaf requires both row-owner readiness flags and matching original
rider IDs. Missing/mismatched rows leave `riderComparisonOwner` unresolved and
the profile proposal incomplete. The arithmetic uses explicit unsigned32
subtractions and signed64 interpretation of their resulting bit patterns,
avoiding signed C++ overflow and implementation-defined unsigned-to-signed32
conversion. It does not pick an arbitrary default rider or synthesize rows.

## CourseSelect: screen 6/page 1

Source: `course-input.txt`, `track-selection-store.txt` and `course-entry.txt`.

The native leaf preserves controller 0, source blocking and confirm-enabled
gates. Mode34=1 requires both controllers for both activate and back
(`0x1DEC64..78` and `0x1DEF98..AC`).

Back remembers `14E4/14EC/14F0/14F4` at profile `700/704/708/70C` for mode34=0
and requests rider screen 5/page 1. Mode34=1 remembers `14E4/14F0/14EC` at
`710/714/718` and requests rider screen 5/page 2. Other original modes request
standings screen 9/page 1 without those remembered writes.

Activation always has an unresolved `courseLaunchOwner` and cannot be admitted.
When supplied a proven selected-control word `+0x44`, the leaf can describe:

1. Its low nibble into profile byte `+0x1125F`, through `0x187300`/`0x187328`.
   This is never inferred as selected course slot minus one.
2. Opponent count in halfword `+0x1125C` bits 7..10: selected total minus human
   count when mode34=1 or page14F4=2, otherwise 8 minus human count, masked to
   four bits exactly as in source.
3. Race subtype word `+0x11258`: page14F4 for mode34=0, otherwise 1 for modes
   1/2. It is a separate field from main/options mode word `+0x34`.
4. Low five bits of byte `+0x1125C`: page14EC for modes 0/1, 3 for mode 2.
5. The mode-specific remembered course fields, in source order.

Mode34 values outside 0/1/2 leave activation's proposal incomplete rather than
inventing a subtype. Missing control data also leaves it incomplete. Even a
complete profile proposal is not permission to start: original manager state8,
global UI `+0x474/+0x478`, delayed exit completion, deferred application flags9,
PodLauncher, loading readiness and complete race ownership still need their
native transaction owners. There is no host race-mode cast or TA fallback.

## Future centralized verification cases (not run here)

- Invalid/unbound pages/groups and slots 0 or past end preserve output.
- Default admissions, disabled/hole controls, blocked input and wrong
  controllers cannot request a route or mutate any source snapshot.
- All Options slots, HOF unchanged/changed mode34 and second-controller absence.
- Gallery remembered selection, locked/unresolved availability, Back without
  gallery unlock, and missing bounded destination identities despite set masks.
- Both Rider controller variants, mode0/1/2 remembered fields, human bits0..3,
  locked rider, bound32 rejection, comparison row identity mismatch, equal
  differences, signed negative differences and wrapping subtraction.
- Course mode0/1/2 back destinations, mode1 controller loss, source control
  values different from slot minus one, packed-bit preservation and unconditional
  launch rejection regardless of destination masks or complete profile data.
- Caller profile/request transaction rejection, stale source snapshots,
  double input, back during a pending transition and unavailable callbacks.
