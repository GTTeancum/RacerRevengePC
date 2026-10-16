# Actual race ownership required by Pause

Source-only current-state review at ed1b686. No build, syntax check, test,
project decoder or guest execution. No new runtime owner or activation.

## Constructor and retention boundary

Use the real native race lifetime already observed by Camera Session. Do not
create a second independent race state or replace actual race fields with an
Entity-only facade. Source17D664 allocates1B8,17D678 constructs1A65C0(A1=1),
17D680 publishes Application28C,17D690 dispatches slot30, and17D6A0 attaches
the resulting complete sibling chain under actual Application10. The alternate
17C158..1B0 route has the same order. Race load adds Pause100 as a sibling via
141860(mask1402), followed by result104(maskFFFFFFF7), before root attachment.
Source race1A8500 separately dispatches enabled Pause100's render callback;
Pause's mask excludes ordinary root draw bit8.

Current Session receives ActualOwners after native counted racer preparation.
It can RETAIN the eventual real race owner, but this late construction point
cannot be used to claim the original race-before-world/racer constructor order.
Actual committed launch must construct the admitted race backing before reached
counted world/racer work; Session later borrows or strongly retains that same
address. Resource prewarming alone is not a committed race launch.

Keep the race's immutable original session generation. Application's roots have
process lifetime identity. The in-progress private Application canonical Entity
registry is the appropriate synchronous mixed-generation chain authority; do
not rewrite node generations or relax unrelated wreck-domain validation.

## Actual cleanup must be admitted before counted construction

Existing detailed source proof remains in
`race-prefix-unwind-proof-20261014.md`.1A65C0 has no failure-return or prefix
unwind branch. It calls141300, initializes finite local state and writes actual
Application2DC profile7E4/7E8 before returning. Its constructor does not establish
B8/BC/C0 destructor null gates; those appear in later load stages.

Full1A6840 is not just a detached base Entity release. It unconditionally touches
Video44, renderer2C, global3E1C10, texture/backend reset1AB4F0→2543F0→2450A0,
Application328 drain282190, and resolver scratch reset132BD0. It then reaches
1413E0. World74=null cannot bypass these operations. A new Session field plus
base-counter decrement would therefore not prove original race disposal.

One earlier blocker IS now closed: actual Application.fonts328Owner_ retains
OriginalApplicationFonts50LA and its concrete close282190. That close rejects
outstanding ScreenText/renderer font borrows and retires in publication order.
It also leaves the current native font registry terminal. A reusable serial-race
launch still needs the authentic subsequent font population/owner lifecycle;
blindly closing the shared registry on a speculative race-prefix unwind is not
safe. Source-order Pause/result/UI retirement must precede the owning font drain.

Update through efd1f7d: same-owner profile7E4/7E8 writes, resolver132BD0
scratch producer/reset, Video44/renderer2C scalars, opaque process3E1C10, and
actual renderer2450A0 completion/D8 clear are implemented as bounded facets.
They remain inactive until their exact reached race boundaries. Manager13C
allocation/transfer-cache retirement is still distinct from the native direct
upload scope and persistent GPU image banks. Existing queue44, fade fields,
texture registration lists and ordinary cache maps cannot replace that owner.
No guessed absent state or no-op callback is justified.

## Smallest honest connected implementation sequence

1. Extend the SAME retained process/profile/display/texture owners with only the
   reached race constructor and cleanup facets. Pre-admit actual cleanup inputs
   and stable storage before incrementing the original Entity count.
2. Construct the real race lifetime at committed launch, register its canonical
   identity privately, then publish the same Application28C alias. Retain all
   reached load ownership and partial-failure state rather than claiming success.
3. Construct/load the actual Pause100 owner, build race/Pause/result sibling
   order, and attach the whole source chain. Do not invent a result sibling if
   that source branch is not reached; prove the actual branch first.
4. Retain the real race owner through Session and native renderer retirement.
   Bind root input and direct race render through these same actual nodes, with
   actual Application camera-index/count and update250 authorities.
5. Retire native GPU/font borrows, perform original child/sibling detachment and
   typed releases, execute reached race cleanup, clear owning aliases, unregister
   immediately before the terminal reference transition, and only then release
   native backing. Application outlives every admitted child.

Device suffix1AF770 is no longer a leaf-semantics blocker: see
`pause-device-feedback-join-20261016.md`. Group2 audio suffix is implemented,
but its actual world Entity142040 prefix remains a separate required join.
Navigation and layout already have genuine inactive consumers. This review does
not add another disconnected race wrapper or claim complete Pause open/close.


## Next concrete race child ownership joins

RaceB8 andBC are map and AI TunnelTrack entities.1A6F4C/1A707C call2A1160,
then publish B8 at1A6F54 andBC at1A7084 and call2A1260. Source string3AA718
is `.spl`;3AA720 and3AA780 identify the map/AI fallback failures. Constructor
2A1160 calls141300 and installs3D39B0; it explicitly initializes30/44 and
other scalar fields. Destructor2A11E0 releases array30 through reverse node
teardown2A1100 (each node's next array6C), frees44, then calls1413E0.

Actual RA navigation is already held by DecodedSources.map/ai and the sealed
ColdNavigationLease, which strongly retains that same source owner. The driver
and Camera observation borrow those actual objects. A genuine child Entity
facet can retain this lease without copying navigation or introducing a second
map; it must preserve the original reached construction/count order and typed
retirement. Current plain inline TrackNavigation arrays are not already these
counted Entity owners. Merely adding two null pointers to the race is invalid.

RaceC0's actual producer is1AAB30: allocateA8 at1AAB4C/50, publishC0 at1AAB54,
then initialize tuning and read authored overrides. Existing
ai_cold_owners50IU models source defaults; impact_authoring49Y and the same
DecodedSources aiTuning/aiImpact/dynamicImpact/finalImpact model portions of the
actual tuning. A future owned combined projection must map every reached field
and preserve source lifetime; independent POD subsets do not yet certify full
C0 allocation retirement. Its source producer is not a constructor-zero gate.

### Admission before race141300

Required inputs are actual Application/profile/Video/display/resolver/fonts,
the genuine committed launch, pre-reserved canonical Entity enrollment and
stable storage, plus an implemented cleanup path for every reached conditional
race owner. Manager13C retirement and reached B8/BC/C0 ownership remain explicit.
Original1A65C0 does not initialize B8/BC/C0, and no private constructor unwind
was found. Native zero-initialization or base-only1413E0 cannot close this gap.
Consequently this update does not create a counted race that cannot be safely
retired. The useful safe child target is the finite2A1160/2A11E0 ownership layer,
followed by actual tuning ownership and manager transfer retirement; only then
can Application28C publication and complete race/Pause sibling wiring proceed.
