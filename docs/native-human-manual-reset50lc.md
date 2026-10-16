# Genuine human controller reset (50LC)

Source-only candidate, 2026-10-13, based on main5f37eda. No build, test,
syntax/import/lint check, sanitizer, gameplay, asset decoder, guest execution or
user-computer operation was run. This packet preserves the existing undertrack
continuation and adds its actual separate input caller.

## Recovered action and effects

Player press1A16F0 reads the live selected action row's6AC, category2/mask3F,
using the real Player5C device. All eight exposed original presets use descriptor8
(SELECT), which the existing SDL positional mapping assigns to BACK. Native T
is an explicitly native keyboard binding to this same descriptor, enabled only
after the concrete physical reset owners and actual camera Session are ready.
Native C remains the separately bound camera cycle. Focus/pause release and
manual takeover detection now include T.

The existing49A handler still runs its three held queries for every press;
it does not turn reset into an invented edge-only action. Holding SELECT/T while
another descriptor produces a press can perform another reset, including twice
in one batch. No new debounce, consumption or cooldown is added.

The actual reset gates are flags29 bit2 clear, manager58 bit1 set/bit0 clear,
and Player1098 strictly negative.1A17A4 synchronously calls2255B0. Its source
225AD4/225AFC explicitly supplies virtual78(a1=0,a2=1), then225B0C runs the human
camera tail. This is the same physical2255B0 body reached by228BBC, not an
assumption that controller reset means full damage reset. Only after the complete
physics and camera call returns does49A store1094=2.5 followed by1090=-1.
There is no217940 call, lives/cost deduction, damage repair, DNF clearing, fresh
grid placement, AI-state reset or host checkpoint in this source branch.

## Eligibility and blur owners

The previous live49A fields were zero scratch, so binding a callback alone would
never pass1098<0. The existing PlayerEventState is now the single represented
1090/1094/1098 owner. At accepted human cold grid preparation, the source
constructor1090=-1/1094=0 and full-reset1A2588 value1098=5 are initialized.
Neither undertrack nor manual2255B0(a1=0) resets1098.

The genuine1A1294..1A12D0 prefix decrements1098 while it is nonnegative and
manager58 bit1 is set. It runs before the same human's1A1A20 input reader, using
that update's actual prepared factor. Once negative,1098 stops decrementing.
Zero still cannot pass the press handler's strict negative gate.

For the normal1090<0 branch,1A14D8..1A1590 updates1094 and submits the actual
2AED10 blur request to the very same GameplayBlurPending used by the camera
and renderer. Source order is ratio1094/2.5, multiplication100 then80, source
CVT.W.S truncation and integer divide100. The timer decrement occurs before the
request; reaching or crossing zero requests strength0 and stores1094=-1.
First-request-wins behavior remains in the existing real manager leaf. No extra
blur manager, texture, rendering receipt or fabricated effect is introduced.
The unrelated nonnegative1090 damage branches remain explicit source boundaries.

## Distinct call and publication domains

Controller1A17A4 receives a distinct PlayerControllerReady50LC physics domain
and Ready-only visibility entry. It does not change frame phase/class, fabricate
a BaseUndertrackContinuation50X, or replay frame input/navigation/packing.
The undertrack wrapper still requires its true228BBC saved pose/cursor and
PostPackedBytes50S. Both share the same persistent human physical reset state,
including its represented850, and the same D40/D44, models, queries and effects.
The physical state records caller domain with serial, preventing equal per-caller
serials from impersonating a completed call.

Ready reset leaves the genuine7F0 publication requiring the next normal graphics
update. A zero-step input poll keeps resize/capture/forced scene redraw pending
until that actual next sequence exists; the last displayed image survives. It
does not synthesize graphics serials or draw a stale publication.
Undertrack keeps its existing post-graphics publication behavior. Camera reset
begins only after the physical visibility lease has ended, and uses the source
pre-lift map anchor. Its retained cursor also records input/undertrack caller
identity, so manual serial1 and undertrack serial1 are distinct operations.
No camera mode/preset or death state is silently changed.

The input provider is synchronous. Any reached unavailable owner, numeric,
registry or camera operation returns failure with actual completed prefixes
retained, and the existing48Z/50EC event queue stops without replay or its
following1094/1090 stores. A pending undertrack continuation cannot be replaced
by a manual request.

## Boundaries and evidence

This live route is limited to the existing TA human Course roster: one actual
Player and seven original AI. Manual reset in nonzero vehicle40 remains an
explicit unsupported boundary until the separate wreck lifecycle is joined.
All AI-only routes and unbound legacy keyboard clients retain their prior input
admission. Wider Player update/damage branches and physical-controller runtime
acceptance are not established by this source packet.

Immutable original ELF SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

Bounded raw instruction formatting covers1A02C0..1A16EC (constructor/update),
1A16F0..1A185C (press reset),225AD0..225B9C (virtual/camera tail). The existing
50LB evidence supplies1A24F0..1A25AC, physical216A30 and concrete camera owners.
Formatting reads immutable words only; no reference emulator or asset decoder
was run. Runtime/build acceptance is reserved for the root's authorized cadence.
