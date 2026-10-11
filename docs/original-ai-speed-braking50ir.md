Historical source/design notes recovered during main consolidation on 2026-10-11. Later native AI integration supersedes the staging status below. These notes establish no current runtime coverage; all new driving validation must use original in-game AI for every racer, including the followed player. See README.md and current-validation.md for dated observations.

# 50IR: original AI target-speed and ordinary braking components

## Status

Source-only, separate stage. No active integration, build, syntax check, native
unit test, game launch, or original instruction execution was performed. This
component is not the original AI controller as a whole and does not repair or
certify the GC race by itself.

`src/game/ai_speed_braking50ir.h` adds two ordinary native C++ projections:

- `selectAiTargetSpeed50IR`: the arithmetic and branch selection at
  `172C50..172DA8`, with exact source-owned inputs supplied by the caller.
- `applyAiNonBoostSpeed50IR`: `173770..1737D8`, only after the original boost
  decision `173180` has returned zero in the appropriate source schedule.

The code does not emulate a CPU or VU, interpret instructions, load guest state,
run original code, touch physics, invent a speed cap, or alter the HOST driver.

## Source behavior

Original executable SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

### Desired speed

1. `172C6C..172C84` calls `2A2520(vehicle.FF4, vehicle.C80)`. That accessor
   returns the current node's float at `+5C`, the authored `SpeedLimit`.
2. `172C88..172CA4` multiplies it by the live `vehicle.FFC` speed scale.
   `171C60` begins every update by setting this scale to one, but can replace
   it with the original race-packing interpolation at `171E28..171EB8`.
   Its constructor value does not authorize hard-coding one for a live racer.
3. Race-pace processing runs only if the track-control byte at
   `app.28C -> B4 -> 58` has bit6 set, `vehicle.CD8` bit0 is clear, and the AI
   state at `vehicle.FE8` is not one. This byte is not `vehicle.58`.
4. Read `app.28C -> C0 -> 7C` (`AIDriving.RacePace`). Subtract 10 when
   `188F60(app.2DC, 0)` returns nonzero. Convert signed integer `vehicle.CCC`
   to float, then compute `interval = pace + CCC` and
   `remaining = interval - vehicle.CB0`, even when pace is nonpositive.
5. For positive pace, separately round these native float operations:
   `remainingRate = (units * vehicle.CC4) / remaining`;
   `wholeRate = (units * vehicle.FF4->3C) / interval`;
   `cap = scalar MAX.S(remainingRate, wholeRate)`.
   `units` is the original unit-conversion factor, retained exactly in the native implementation.
   Reduce desired speed to cap only when desired speed is greater than cap.
6. Apply the signed integer `vehicle.108C` minimum-speed floor. This is the
   actual `MyMinSpeed` loader destination (`16E7F4..16E804`), not the unused
   `MyMaxSpeed` text key.
7. `2A25E0` extracts current node byte79 bit4 (`DriveFullSpeed`). When set,
   `172D98..172DA4` replaces the result with the unscaled authored node limit,
   after both the pace cap and minimum floor.

The two timing divisions, maximum, minimum floor and final override are
intentional. Replacing them with a minimum of future node limits, a clamp on
heading error, or a host cruise-speed limit changes the original algorithm.

### Ordinary throttle/brake

`172DB0` calls the boost decision with elapsed time and desired speed. Only if
it returns zero does `172DC4` call the ordinary leaf:

- Clear boost flag `B9C` bit0, preserving its other seven bits.
- `convertedSpeed = units * (vehicle.340 -> cachedSpeed34)`.
- `difference = desiredSpeed - convertedSpeed`, rounded separately.
- If difference is strictly negative, write throttle `B7C=0`, brake `B80=1`.
- Otherwise write throttle `B7C=1`, brake `B80=0`. Exact equality and signed
  zero take the throttle branch.

This is a full-action decision, not HOST's proportional speed-error blend.
The cached speed scalar retains its sign. Substituting a planar world-velocity
magnitude or dividing the target by a reciprocal constant is not this source
operation sequence.

The native leaf uses the existing `PlayerReadResult48X` only as a typed view
of shared raw/action fields. It preserves all raw channels, including steering.
It does not invoke the player input reader or claim that the racer is a player.

## Caller prerequisites and remaining gap

Do not enable the header in the current HOST path by supplying convenient
zeros, default scales, guessed race clocks, or a fabricated boost-declined flag.

The genuine join must provide:

- The same source-owned AI track, current node, node speed, and node flag.
  Native `TrackNavigation50EU.nodes[node].flags79` already retains
  `DriveFullSpeed`. The actual speed metadata is available from the spline,
  but is currently stored in the explicitly HOST `CourseSpeedLimits50GK` view.
- A native AI owner with live state `FE8`, packing scale `FFC`, signed
  `MyMinSpeed108C`, flags `CD8`, and exact `CCC/CB0/CC4` timing/progress fields.
  Original initialization and updates are not replaced by this component.
- Actual track-control flag58 and loaded `RacePace7C`; the resolved settings
  query result; and the actual same-track length3C. Unused pace fields need not
  be manufactured when the original branch does not read them.
- Correct dispatcher order: navigation `16FFE0`, state update `16EB00`,
  packing `171C60`, repair `1734C0`, and then the state's driving routine.
  The original jump table at `3A7C20` selects ordinary direction+speed for
  states0,2,3. State6 uses the separate `172B50` speed path. Other states can
  select combat or reset routines; this header does not cover them.
- Real direction/steering output before speed selection and the real
  `173180` boost decision before the ordinary leaf. Boost has live heat,
  steering, race/target state, cooldown and shared-random dependencies.
- The existing source-compatible non-player control binding and shaping
  mode. Current HOST preset4 consumes twin analog channels, so simply writing
  `B7C/B80` in that path will not turn this into original AI. Do not flip a
  vehicle mode or bypass the real initializer to force it through.
- The source-time cached speed owner. Native `m.cachedSpeed34` exists, but
  must be sampled at the correct source scheduling point rather than replaced
  by `hypot(worldVelocity.x, worldVelocity.z)`.

This stage deliberately does not duplicate the separate optional inside-
airbrake steering mixer, change original racer coefficients, broaden stop
bounds, synthesize contacts, advance navigation, reset a racer, or alter laps.

## GC facts from original authoring

Original `tracks/agc.spl`, SHA-256
`52f9a4aa91d71a11b1cabdce7e4b7faac3311001d439579d985410d6bd87f306`,
has195 nodes:155 limits700,15 limits500,12 limits300,10 limits600,3 limits400.
Opening nodes0–4 are700; nodes5/6/7 are600/500/600. No `DriveFullSpeed` key
occurs in this spline. `tracks/gc.tun`, SHA-256
`75c5c331497e786698fb2244f94036c958dd0a3e013ff96f8f072785e29e7d6a`,
authors `RacePace=100`, `Lookahead=0.7` and packing percentages0.5..1.0.
These values alone do not establish a live target: the gating, timing and
packing owners still matter.

The inspected ordinary target-speed routine has no additional curvature cap.
Authored lower limits, source steering and per-side aero inputs can all affect
bends. This is not proof that every source AI state lacks another speed policy.

## Arithmetic and future verification

Admission is intentionally limited to finite, normal-or-zero native float
inputs/intermediates and `FE_TONEAREST`; zero denominators and unsupported
arithmetic reject while preserving outputs. Strict floating-point compilation
is required, with FTZ/DAZ disabled so rejected subnormal results cannot be
silently flushed before admission checks. FE_TONEAREST alone does not prove
those host settings. The existing Linux gameplay target uses -fno-fast-math,
-frounding-math and -ffp-contract=off. The component makes
no guest-FPU edge-case or bit-equivalence certification claim.

The integration owner's next authorized validation should cover strict
threshold/equality/reverse cached speed, preserved raw channels/flag bits,
all three pace gates, the optional minus10, both division branches, signed
integer conversions, minimum floor, DriveFullSpeed's final override, rejected
inputs preserving outputs, and the actual control-binding route. Only after
real owner/scheduling integration can gameplay establish braking behavior.
No tests have been run or scheduled by this stage.
