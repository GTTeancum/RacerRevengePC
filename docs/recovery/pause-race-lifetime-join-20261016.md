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

Targeted current-source review still finds no concrete resolver132BD0 scratch
slots/reset, texture-manager13C/backend2543F0 reset, Video44/renderer2C/global
3E1C10 facets, or same-profile7E4/7E8 race-constructor mutation. Existing queue44,
fade fields, texture registration list and ordinary cache maps are not those
fields. No guessed absent state or no-op callback is justified.

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
