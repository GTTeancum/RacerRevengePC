# Original aerodynamic vehicle forces — 48E

48E adds the complete numerical aerodynamic producer and force application for a linked rigid body with a ready scene basis. It includes the original angle approximation, optional flow subtraction, lift and drag, angular damping, differential steering, low-speed handling, contact-triggered response, powerslide response, inverted-body response and two persistent timers. Production uses ordinary typed native data, without guest addresses, instruction interpretation, generated functions or the compatibility runtime.

The connected native fixture now executes engine generation, gravity, conditional engine application, aerodynamic force application and the recovered 48C rigid-body numerical core in their original relative order. It is still a **partial vehicle step**, not a complete vehicle or playable race: repulsor/contact response, the second-body interaction, scene reconstruction/writeback, actual scheduler input, physical action mapping and a verified whole-substep semantic trace remain outside it. There is no gameplay FPS claim.

## Deliverables and contract

- `src/game/vehicle_aero48e.h/.cpp`: `applyAerodynamics48E`, `stepEngineAeroBody48E` and separately callable `refreshCachedSpeed48E`.
- `tests/native/physics48e.cpp`: differential execution of original instruction words, including the original angle and transform helpers, connected with the frozen 48D/48C word fixtures.
- `tools/gameplay/recover48e.py`: checks immutable ELF/RAM hashes, extracts five original code blocks, verifies 27 original generated source excerpts and records the saved aerodynamic component and local-body fields. Generates the test-only `aero_words48e.h` and `aero_oracle48e.h`.
- `tools/gameplay/dependencies48e.py`: verifies 37 literal/parser/destination tuning-key chains, supplemental source excerpts, the body vtable, and bounded candidates for speed-refresh dispatch.
- `evidence/gameplay48E`: raw original blocks/snapshot slices, word-verified source excerpts, dependency and parameter evidence, preserved failing/passing build logs, validation record and final owned-file manifest.

Production needs C++17 with strict binary32 arithmetic ordering; the test needs C++20 and `/fp:strict`. Fast math, reassociation and fused multiply-add contraction are outside the comparison contract. Finite input values and intermediates, nonnegative elapsed factor and a ready associated scene basis define the accepted native domain. Invalid input or a later body-stage failure leaves the complete output unchanged. This validation policy is the native API boundary, not a claim that the original game rejected the same inputs.

The API deliberately receives the body's **cached local velocity at +0x80** and angle at **+0xE0**. It does not replace these with the current world velocity or a newly invented orientation conversion. The linked body is required; the original no-linked-body early return and dirty scene-cache traversal are not exposed as native gameplay behavior. The ready inverse direction route uses the transpose of the cached basis, as the original helper does. Quaternion-to-angle convention and scene cache maintenance are still outside this extraction.

## Proven original route and identities

The preserved 47W RAM identifies player `0xC5B0B0`, body `0xD110A0`, aerodynamic component `0xD11650` and scene `0xCFD8F0`. The component's `+0x30` points to that body. Its vtable is `0x3CE0B0`, whose `+0x78` entry is `0x16CD90`. The body vtable is `0x3CE000`: slot `+0x3C` is integration wrapper `0x167C40`, while slot `+0x38` is separate update wrapper `0x167B80`. The saved body's `+0x320` is 1, so that latter wrapper skips its conditional inner integration call on this state. These are runtime identity facts used to establish dependencies; native code contains none of these addresses.

All abbreviated source paths below are under `work/native/project/generated/`. Each preserved excerpt includes original source line numbers, exact words and verified source hashes. ELF file offset is `address - 0x100000 + 0x80`.

| Original address | Behavior | Source anchor |
|---|---|---|
| `0x219CD4` | `RunSimulation` calls aero slot `+0x78`, after engine/gravity application | `FUN_00219810_0x219810.cpp:2595` |
| `0x16CD90..0x16D870` | Complete aerodynamic producer for the supported linked-body/ready-scene route | `FUN_0016cd90_0x16cd90.cpp:31` |
| `0x16CE68` | Load cached body-local velocity | same file, line 221 |
| `0x16CECC` | Call original angle approximation | same file, line 307 |
| `0x13C140..0x13C1E8` | Rational angle approximation and sign restore | `FUN_0013c140_0x13c140.cpp`, `entry_0013c1ac_0x13c1ac.cpp` |
| `0x28A700..0x28A884` | Get ready world forward/up axes | `FUN_0028a700_0x28a700.cpp` and continuations |
| `0x28BBB0..0x28BD44` | Ready inverse-direction transform for optional flow | `FUN_0028bbb0_0x28bbb0.cpp` and continuations |
| `0x16D468` | Begin local force calculation after mode selection | `FUN_0016cd90_0x16cd90.cpp:1959` |
| `0x16D7A0`, `0x16D800` | Rotate local force, then add torque directly | same file, lines 2721 and 2812 |
| `0x167BC8..0x167C18` | Separate cached-speed scalar writer | `FUN_00167b80_0x167b80.cpp`, `entry_00167c00_0x167c00.cpp` |

## Preserved behavior

When model `+0x34` is nonzero, the original transforms flow vector `+0x40.xyz` into local directions and subtracts it from cached local velocity. It computes the magnitude of that relative velocity using the original scalar square-root reference behavior. If local forward velocity is **greater than 20**, it calls the game's approximation with `-localY/localZ`; otherwise it uses cached body angle `+0xE0`. The approximation is implemented directly from its original rational expression with constants `0x3E8F5C29` and `0x3FC90FDB`. Replacing it with library `atan` changes the game model and was not done.

The velocity response factor is `min(float32(0.02 * speed), 1)`. Its signed version changes sign only when forward velocity is strictly negative. The producer uses separate float operations, including the original multiply-then-add order; it rotates the completed local force through the ready scene basis, then adds the generated torque directly without rotating it.

The persistent timer logic is consequential:

1. Nonzero `+0xD0` sets timer `+0x140` to 0.25 and timer `+0x144` to zero.
2. If timer 140 was positive at that point, the first mode is active for this call and elapsed factor is subtracted. Crossing below zero does not deactivate it until a later call.
3. Nonzero `+0xD8` sets timer 144 to 4 only when the first mode is inactive.
4. If timer 144 was positive, the second mode is active and elapsed factor is subtracted. Coefficient interpolation uses the **decremented** timer, then bounds its result using the original endpoint comparisons.

Mode priority is: speed below 50; first timer; second timer; world up-axis Y less than or equal to 0.5; ordinary upright response. The low-speed branch uses a bounded speed-dependent yaw response instead of the high-speed steering branches. The first timer steers through local lateral force, while the powerslide branch combines timer-interpolated lateral/yaw/thrust terms with differential drive. The inverted branch switches lateral/yaw response and steers through lateral force. This is recovered original model behavior, not an approximation based on generic racing physics.

The original global constants at `0x433C10`, `0x433C18`, `0x433C20` initialize to 0.5, 0.25 and 4. Their initializers and exact saved RAM bits agree. Test fixtures select the initialized-statistics route; native code uses those established constants directly. The producer's return code, unused vector W lanes, generic scene traversal and floating-point exception flags are not modeled outputs.

## Tuning and action provenance

The loader's base coefficient block passes through `0x1004B0`, copying the reader's stack fields to the component beginning at `+0x5C`. Additional values are parsed directly into their component fields. `dependencies48e.py` checks the exact original instructions for 37 key chains. A key's existence does **not** prove that this producer consumes it: the original base coefficient structure also includes generic aircraft fields not read by `0x16CD90`.

| Native API field | Original offset | Proven PHY key |
|---|---|---|
| `offset68`, `lift6c`, `drag78` | 68, 6C, 78 | `DefaultAngleOfAttack`, `Lift`, `BasicDrag` |
| `pitch84`, `pitchDamping88` | 84, 88 | `PitchMoment`, `PitchRatePitchMoment` |
| `lateral90`, `rollDampingB0` | 90, B0 | `SideSlipSideForce`, `RollRateRollMoment` |
| `timerYawB8`, `yawDampingC8` | B8, C8 | `SideSlipYawMoment`, `YawRateYawMoment` |
| `driveDragF8`, `brakeDragFc` | F8, FC | `SpoilerDrag`, `SpoilerBrakeDrag` |
| `steeringRamp104`, `timerSteering108` | 104, 108 | `SpoilerYawMomentScale`, `SpoilerSideForce` |
| `timerLateral10c`, `timerYaw110`, `timerDrag114` | 10C, 110, 114 | `PowerslideSideForce`, `PowerslideSpoilerYawMoment`, `PowerslideThrust` |
| `tiltedLateral118`, `tiltedSteering11c`, `tiltedYaw120` | 118, 11C, 120 | `InvertedSideSlipSideForce`, `InvertedSpoilerSideForce`, `InvertedSideSlipYawMoment` |

`steering100` is selected or interpolated in `0x214Fxx..0x215078`; the API consumes its resulting runtime value. The three velocity-damping coefficients at `+0x130` are also consumed as runtime values. Test fixtures use the saved object's exact bits; they do not assume the object equals unmodified Pod01 PHY defaults.

Local action dependencies are now more precise:

- `FUN_00217de0` clears aero `+0xD8` at `0x218200` (source line 1039), and on other paths writes vehicle byte `+0xB9C` bit 2 to it at `0x2184BC` / `0x21887C` (lines 1693 / 2731). Its powerslide-named coefficients support that branch's description; the physical controller button remains unproven.
- `RunSimulation` copies spring-collision component `vehicle+0x354` field `+0x3C` to aero `+0xD0` at `0x219FBC` (line 3896). This occurs **after** the current aero call and body update; it must not be silently moved ahead of them.
- It copies coupling component `vehicle+0x348` field `+0x104` to engine full-torque `+0x3C` at `0x219F9C`, and aero full-steering `+0xE0` at `0x219FAC` (lines 3864 / 3880). These also affect a subsequent force update on this path.
- Aero `+0xD4` is written alongside the collision state but is not consumed by this producer. This extraction does not invent an effect for it.

The drive fields `+0xE4/+0xE8` and brake-drag field `+0xEC` remain established component inputs. Their full input-mode scaling and physical controls are not replaced with arbitrary throttle/brake mappings.

## Body cache and ordering evidence

The complete body wrapper calls inverse-direction helper `0x28BBB0` on current world velocity at `0x1682D4`, then stores its result into cached body `+0x80` at `0x1682E4` (`FUN_00167c40_0x167c40.cpp:2703`, `:2737`). It clears force and torque accumulators at `0x168304` and `0x168310` (lines 2787 / 2799), after integration and scene synchronization. The recovered 48C core ends earlier, so 48E does not claim that its output has completed these wrapper effects.

The separate wrapper `0x167B80` updates body angle `+0xE0` through helper `0x14FC00`, then cached speed `+0x34`. The speed writer squares world velocity XYZ, adds X/Y first then Z, returns zero on an exactly zero sum, otherwise executes **RSQRT(1,sum) followed by 1/result**. `refreshCachedSpeed48E` preserves that sequence, including the existing reference's positive reciprocal-root treatment. It is not replaced with a direct `sqrt` and is not automatically called between engine and body stages.

Generic child update `0x141ED0` dispatches slot `+0x38` at `0x141F5C/0x141F60`, subject to the original node flags and child-list membership. A bounded scan of the original 2,972,160-byte load segment found 16 exact slot-load/direct-call candidates; these are recorded as candidates, not asserted to be body calls. The particular saved node's participation and its ordering relative to the scheduler's vehicle-substep loop still need semantic trace confirmation. Its `+0x320=1` and distinct slots establish why blindly appending a speed refresh to every native substep would be premature.

The existing conditional vehicle override still writes a direct-square-root cached speed at `0x219F74`. Therefore even a numerically correct general refresh helper cannot establish the complete dynamic cache policy by itself.

## Validation and limits

Both integrated **x64 and x86 passed 354,461 checks each**, with all nine project CTests passing in `evidence/nativePc48E-build-20261003-141953-482`. Exact built source hashes, frozen executable hashes and copied test logs are recorded in `evidence/gameplay48E/validation.json`. The earlier standalone x64 pass is retained in `iteration02.log`. Tests compare bitwise float outputs and exact discrete state, not whole guest RAM or cycle counts.

Coverage includes 10,000 randomized aerodynamic calls, 2,500 connected engine/gravity/aero/body steps, four saved-snapshot aerodynamic cases, 1,280 consecutive timer updates and 4,000 separate cached-speed comparisons, spread over all four host rounding modes. An execution trace of the original word oracle confirms that each of the five mode branches was exercised. Boundary cases distinguish forward speed 20, magnitude 50, world-up Y 0.5, timer priority and overshoot. Alias, invalid input, overflow and late body-stage failure cases verify output ownership. The connected tests compare all 14 engine floats, aero timers/flag/local loads/accumulators, and all 36 body floats including derivative histories.

The oracle executes the exact preserved original instructions and helpers within 8 KiB of fixture memory, with delayed branches/calls, an instruction budget, and true return checks. It does not call production force formulas. An initial run failed at `0x16D7A8`: `LQ v0,0(v0)` exposed a decoder error that reread its now-overwritten base register for later lanes. Capturing the effective address once fixed that test-only bug; the native aerodynamic arithmetic then passed unchanged. The initial integrated failure log and both local iteration logs are preserved. Generated oracle line endings were normalized at the same time.

Supplied PCSX2 `pcsx2/FPU.cpp:92–100` provides signed-word MIN/MAX ordering, including `MAX(-0,+0)=+0`. Native code explicitly handles the signed-zero tie; the independent oracle uses the supplied reference's signed-word comparison. Its source hash and excerpt are preserved. Ordinary scalar/VU operations otherwise use the established host binary32 reference domain, with the existing 48C square-root/reciprocal-root reference. These tests do not certify every physical PS2 denormal, extended-range, FTZ/DAZ or exception-flag behavior.

The stopped snapshot is not an observed before/after tick. Test elapsed factors, trajectories, local velocity and ready bases are explicit fixture inputs. The original scheduler's `trunc(float32(elapsed*60))+1` subdivision still applies; the clock source and pause policy remain unproven. A playable native vehicle requires the missing contact/repulsor/second-body stages and scene wrapper, followed by a semantic comparison across a real original substep.
