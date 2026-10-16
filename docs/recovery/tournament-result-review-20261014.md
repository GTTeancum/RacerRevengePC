# Tournament result source review, 2026-10-14

Independent source-only review of `original_tournament_result50kg.{h,cpp}` and
the tournament-bank/result changes in `original_ui_rider_profile50kg.{h,cpp}`.
No build, test, syntax check, sanitizer, project executable, guest execution,
gameplay, profile mutation, unlock, or commit was performed. Original source
instructions were inspected only as little-endian data using static word reads.

Authenticated source: SLUS_202.68, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
File offset is source VA minus `0xFFF80`. The semantic reference is
`course-unlock-join-20261014.md`; this review also checked source words directly.

## Findings and resolution

1. The initial profile projection still referenced the removed
   `Storage::medals1938`. Reported immediately to author/root. Author replaced
   it with the sole bank's diagonal
   `tournament.careers[rider].pods[rider].medals1938[0]`; reread confirmed fix.
2. During independent review, the author corrected kill-table admission and
   percentages. The final code has seven entries, ordinary courses
   `20,30,50,60,70,80,100`, with TA/order0 and MB/order6 using
   `30,50,60,70,80,90,100`. Static constructor writes at 17EF24..17EF88 and
   17F31C..17F380 agree. The following course-record word is class, not an
   eighth percentage. This corrects the earlier eight-entry recovery text.
3. Arming initially rejected only ended bit0, thereby also admitting a zero
   timing-manager flag byte. Reported for stronger active-manager admission:
   `(flags58 & 3) == 2`. Author applied that restriction; final source reread
   confirmed it. The end commit requires both active and ended flags,
   `(flags58 & 3) == 3`.
4. An initial reviewer claim that shared RNG state must be masked was withdrawn
   after register-level reinspection: 308054 word `00621024` writes masked
   return **v0**, while 30805C `ACA30058` stores unmasked **v1** to raw owner+58.
   The implementation's unmasked raw32 state and masked low31 return are correct.

## Exact source effects checked

- Full storage is 27 careers by 27 pod rows, each six abilities, thirteen
  medals, and thirteen signed-halfword balances. Career totals and packed
  progress are separate. All original racer medal writes remain in the selected
  career, rather than reducing them to its selected-pod diagonal.
- 17FFB0 copies every pod's six base abilities into every career, clears medals,
  copies per-order prizes, and clears totals/progress low bits. New-profile
  typed zero initialization agrees with this reset subset. Static constructor
  immediate/store pairs give prizes `2200,2400,2700,3700,3200,3400,3700,4700,
  4400,4700,4900,5200,6000`, in track order `0,11,4,8,7,1,5,9,12,6,10,3,2`.
- 18766C clears tournament boundary before retirement and prior-medal early
  returns. The new result preserves that ordering; unused kill operands are
  not admitted/read on those early-return paths.
- 187C80 qualification uses actual selected course-order equality to packed
  career low-five progress and first-human place below four, after retirement
  exclusion. Native place1..8 admission is intentionally stricter than the
  original signed comparison, preventing malformed ranks from earning progress.
- 187900's prior-medal/reward row is pod[C], independently of first-human pod.
  Place8 returns zero. Prize calculation retains signed16 balance conversion,
  binary32 percentage/100, multiply, add .5, and truncation. Balance write is
  low16 SUBU, not saturation. 187A20 uses the awarded prize independently with
  the actual course kill percentages and the same rounding sequence.
- Winnings/totals use modulo32 ADDU; all manager rows' original pod IDs receive
  their place byte, and first-human CD0 increments career kills.
- 1AF4B0 uses the first manager row's place, binary32 words 3ECCCCCD,
  3E4CCCCE, C0000000, and 3F19999A, in source subtract/multiply/divide/add
  order. Its selected career's other 26 pods are visited in ascending pod
  order, six lanes each. Each lane independently calculates
  `(maximum-base)/13`, consumes one shared308030 transition, converts the
  low31 return to float, multiplies by 2^-31, divides by two, adds rank factor,
  multiplies by step, adds the old ability, and applies original scalar MIN.S.
  Exactly 156 draws occur on admitted growth, including zero-step absent rows.
  No growth occurs after reaching progress13 or on a nonqualifying result.
- Volatile per-operation binary32 stores prevent fused multiply/add replacement;
  FE_TONEAREST and ordinary-finite checks explicitly bound native admission.
  These checks are source-domain restrictions, not claims about all EE exceptional
  floating-point behavior.
- 187D80 is run after every successful 1875C0 return, including retired/prior
  medal returns. It compares low-five values, preserves upper availability bits,
  wraps the increment to five bits, and clamps a resulting count above13. It
  neither consults world readiness nor conflates course count with tier bits.

## Transaction and private caller boundary

The result leaf copies the bank, availability and raw RNG locally. Any owner or
numeric admission failure leaves all three actual owners unchanged. Only the
final successful publication writes them back. Receipt fields can describe
attempted work on a rejected transaction and must not be treated as committed
draws or earned progress when status is not complete.

The profile result boundary retains launch generation/mode, exact selection,
application address and timing-manager address, rechecks the current selection,
requires ended-active flags and first-human selected-pod identity, consumes its
arm once, marks dirty, and increments generation on success. The private caller
must still establish that racer rows are fresh observations from that same real
manager: the retained LapTimingManager50ID subset has only clock/flags and cannot
itself prove row membership or standings provenance. No public progress or
availability setter was introduced. Existing mode0 launch is not a mode2 launch.

These source owners do not complete outer1AB520, genuine7E4/7E8 facts, retirement
and tournament result routing, save IO, or the still-missing mode2 launch/session
lifecycle. They must not be used to claim an earned native unlock until that
concrete source-owned caller is integrated and root validates it.
