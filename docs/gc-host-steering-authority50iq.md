# Optional GC host steering-saturation allocation (50IQ)

Historical scope: this page records the 2026-10-09 GC HOST-controlled experiments, including focused source `53d44876270a095ac956720784437a16126ae584`. Its candidate, runtime and remaining-boundary statements describe that snapshot, not consolidated main. The historical HOST driver is not an approved current driving-test entry point; current driving validation requires original in-game AI for every racer. See the [dated GC receipt](history/gc-host-mixer-20261009-2320.md) and [current project status](../README.md). No new build or runtime acceptance is claimed by this merge.

## Focused runtime coverage; full-course driving incomplete

The 23:20 central slot built source 53d44876 and completed one 60-second GC capture. All eight racers passed the opening and node 40, then stopped OffCourse at nodes 43/44. No GC lap or complete-field pass is claimed. This exercises the integrated mixer and shows progress over the class-only run; it does not validate every control boundary. The separate original recovery branch remains intact. See [dated GC receipt](history/gc-host-mixer-20261009-2320.md).

This is an explicit HOST packet-mixing policy, not original AI. It does not
change PHY coefficients, class selection, speed targets, preview selection,
manual inputs, native control owners, navigation or safety bounds. It does
reduce positive common acceleration demand when necessary to preserve the
requested differential inside the fixed [-1,1] axis range. That tradeoff must
not be described as unchanged acceleration demand or unchanged achieved speed.

## Separate throttle/brake and steering provenance

The existing speed controller supplies a signed demand in [-1,1] from target
minus observed planar speed. Positive is forward throttle, zero neutral and
negative ordinary brake. The selected source-derived speed target, conversion,
speed gain and authoredSpeedOnly behavior are unchanged.

The existing heading/yaw controller separately supplies turn in [-1,1]. The
old mixer independently clips demand+turn and demand-turn. At demand=1 and
turn=1 this yields1/0, not1/-1, discarding half the requested differential.

Native preset4 parameter binding (original215408..21549C) halves the selected
spoiler-yaw coefficient and supplies that half as both aero steering100 and
oppositely signed engine torque bases. Both native mechanisms contribute to
full steering. In the original input binding:

- A negative filtered Y axis produces engine drive.
- A positive filtered Y axis produces its side's aero input.
- Global brakeEC is a separate both-brakes branch; it is not the missing
  inside steering input.

The normal full-throttle/full-turn endpoints are:

| Control request | Engine left/right | Aero left/right | EC |
| --- | --- | --- | --- |
| Old host preset4 1/0 | 1/0 | 0/0 | 0 |
| Native throttle plus full steering | 1/0 | 0/1 | 0 |
| Reachable preset4 1/-1 | 1/0 | 0/1 | 0 |

These are ideal settled binding endpoints. Original conditioning, nonlinear
shaping, asymmetric quantization (~0.99 throttle endpoint), rate limiting,
engine smoothing, support flags, drag, tilt and contact response still apply.
No equality of transient trajectories or complete AI behavior is claimed.

In the recorded 21:19 run Dud and Gasgano had turn=1 while target-speed error
was large enough to saturate positive demand=1. Thus they were in the concrete
no-inside-aero case. Their observed failure is not proof that this mixer is
required after the independent original class bug is corrected.

## Bounded proposed mapping

Only GC enables fullThrottleSteeringRange. For positive speed demand:

    commonDemand = min(demand, 1 - abs(turn))
    left = clamp(commonDemand + turn, -1, 1)
    right = clamp(commonDemand - turn, -1, 1)

Unsaturated demand remains unchanged. When acceleration and steering will not
both fit, the common component is lowered enough to retain left-right=2*turn.
For full positive demand and positive full turn the outside engine stays at
full throttle and the inside channel crosses neutral to full airbrake. No
reverse thrust is requested. Negative-turn behavior is symmetric.

Zero and negative speed demands retain the prior mapping, avoiding unrelated
changes to the original both-brakes threshold behavior. TA leaves the flag
false. Manual control bypasses the host output as before.

The same existing packet helper writes only bytes7 and5. The original device
reader, player conditioner/shaper, rate limiter, binder and engine/aero
producers consume them in their existing order. No direct force/model/state
writes or synthetic buttons are introduced.

Per-node telemetry additionally reports original signed demand, allocated
common demand, left/right output and actual packet bytes7/5. All added fields
are already covered by whole-POD racer-bank owner registration. This stage
has no build-list changes.

## Expected verification

In the next authorized central run, compare all eight slots against the
source-correct class-C candidate. Confirm unchanged target policy, actual axis
and native input behavior, positive-demand steering saturation, manual
handoff, unchanged negative-demand behavior and retained genuine stop bounds.
Original inside SpoilerDrag will affect physical speed when used; no arbitrary
corner speed cap is introduced. Actual course progress must be observed.
