# Native player boost controls50FK

`src/game/boost_controls50fk.{h,cpp}` implements the retained original
`2176E0..217934` controller for the player subtype. It does not create a force
multiplier: its resulting vehicle64 is consumed by the existing49K selector,
48D boost thrust/speed branch, and49Z overheat-damage consumer.

The source ELF SHA256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
`evidence/boostControls50FK/source-words.json` and `source-disassembly.txt`
retain the bounded constructor, loader, caller, controller, query and player
virtual-target evidence. The extraction reads instruction words only; no guest
CPU/device execution, program-counter execution, RAM snapshot or held49C/48U
route is involved. Cop1 comparison disassembly's first displayed register is
its unused fd field; the comparisons use fs then ft from the recorded word.

## Recovered behavior

- Every call copies64 to68 first. Holding B9C.bit0 with repair bit1 clear
  computes `heating = vehicle88 * (engine74 + engine70)`. Positive heating
  writes64=1. A nonpositive result retains the prior64; it does not clear it.
- Released boost or active repair clears64 and contributes zero heating.
  Source player vtable3CED50.A8 is1A2B70, a genuine return+nop, for both
  ending state2 and entering state2. This is not an omitted callback stub.
- Heat7C adds `factor * ((heating - speed34 * (cooling84 * speed34)) - cooling80)`
  with each operation rounded separately. Original188F10(profile,1) returns0
  for profile34=2; otherwise it returns profile10FC0. Vehicle58=0 and a
  nonzero result zero heat instead. The same profile34=2 is already explicit
  in the baseline's49Z impact context.
- Heat at or above1 clamps to1 and writes64=2. Negative heat clamps to0.
  State2 still selects native engine boost and must reach49Z periodic damage;
  silently forcing64 back to0 would change source behavior.
- SpeedRatio70 is the original `(bits3F2E8BC1*speed34 - 20) /
  (bits3F2E8BC1*engine5C - 20)`. It is diagnostic state, not an invented
  boost-eligibility threshold. Duration6C increments in states1/2 and clears
  in state0.

The fixed supplement starts at constructor-proven zero64/68/6C/70/7C.
Parameters use original constructor heatRate88=bits3E2E147B (.17), authored
CF8 copied to cooling80, and cooling84=bits37A7C5AC*CF8. The existing authored
binding supplies the final engineDifference5C. Strict finite binary32 and
FE_TONEAREST are the native admitted domain, with atomic rejection on failure.

## Optional owner and ordinary-player boundary

The development client has no D40 audio/control owner. Its source null checks
at217764 and2177C0 therefore genuinely skip those calls. `d40Present=false`
must describe that supplied topology; it does not assert that original-game
memory had a null D40. A present D40 rejects with AudioOwner. In the original,
start slot90=19FF10 calls D40.A8(4) before setting byte22C.bit1, and that
additional owner may submit audio. No success is invented for it.

The supplement is outside49W's represented storage, so performing this source
call before `graphicsVehicleFrame49W` leaves that function's preserving
contract intact. No49W phase/admission guard is removed. The caller must retain
the ordinary player route and connect the supplemental result to both existing
downstream consumers.

## Authored cooling and the adjacent repair selection

`tests/native/boost_controls50fk_fixture.h` is a cold diagnostic loader. It
first validates the complete actual PHY through49N, then reads the cooling
keys using existing49Y decimal conversion. It never executes original code.
It follows `authored.binding.usedAuthoredClassValues` and `selectedClass`,
which the existing native49N binding already resolved from Tuning.

The branch at215CD8 tests that same UseValues mode. Class A loads
BaseRepairAmountA intoCF4 at215D20 and BaseCoolingTimeA intoCF8 at215D34;
B/C have corresponding branches. Its jump to215FA0 keeps the selectedCF8
in the delay-slot load. Only UseValues=0 reaches the interpolation beginning
215DCC. The cold helper supports that six-operation cooling interpolation
and rejects a later profile overwrite, which this baseline already excludes.

Actual Pod01 has UseValues=1 and classA: CF8=.0057 (bits3BBAC711), and adjacent
repairCF4=.013. Absence of the later216040 profile overwrite does not select
the interpolation branch. The previously unconditional repair interpolation
in50DU instead produced .014; the parent integration owns correcting that
separate client assignment.

## Exact client join

Cold setup after the existing authored bind:

```cpp
BoostParameters50FK boostParameters{};
BoostState50FK boostState{};
float boostCoolingCF8=0.f;
require50DU(boost50fk_fixture::bind(
    reinterpret_cast<const char*>(phy.data()),phy.size(),selection,
    authored.binding,boostParameters,boostCoolingCF8),"actual boost authoring");
```

Once per native frame, immediately after `bindVehicleFrame49W`, before
`graphicsVehicleFrame49W` (original caller228788):

```cpp
BoostInput50FK input{m.frameFactor,
    m.components.engine.smoothedDrive[0],m.components.engine.smoothedDrive[1],
    m.cachedSpeed34,m.controls.player.flagsB9C,s.fields.vehicle58,2,0,false};
BoostDiagnostic50FK boostDiagnostic{};
require50DU(advancePlayerBoost50FK(input,boostParameters,boostState,
    boostState,boostDiagnostic),"native boost controller");
x.selectors.vehicle64=boostState.vehicle64;
```

Then assign `consumer.vehicle64=boostState.vehicle64` before
`consumeVehicleImpacts49Z`. Do not replace the current retained engine70/74
with freshly bound engine68/6C, or move this update after current physics.
The existing graphics cut preserves all its represented state. New source
dependency is only `src/game/boost_controls50fk.cpp`; the cold helper uses
already-linked `pod_physics49n.cpp` and `impact_authoring49y.cpp`.

Selected preset4's `flag698` is button code5. The explicit development mapping
is Space (SDL physical scancode44) to action button5, routed through the same
50EC/50CX/48Y/48X held/edge ingress as R. Retain the preset-code check, normal
release-all handling and49A edge dispatch. Do not directly set B9C or64.

## Focused verification

One component build/run passed29 assertions: actual class cooling and source
constants, held without thrust, start, retained state on zero thrust, release,
repair inhibition, overheat/state2 engine selection, cooling, profile query,
present-owner/nonfinite atomic rejection, distinct interpolation and profile
overwrite rejection. Command/output are in `focused-check.txt`. No regression
sweep, original-word interpreter or game launch was performed by this worker.
The parent owns the client integration and short live boost test.
