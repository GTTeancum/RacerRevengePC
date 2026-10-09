# 49M secondary-body wrapper

## Closed numerical boundary

`secondary_body49m.{h,cpp}` joins the original optional-body force prefix,
49L's ordered constraint methods, and the existing48H root-body update. Production
uses typed fixed-size records and no original instruction execution, allocation,
world query, detector, contact implementation, rendering or platform transport.

The original wrapper is2188B0..219120. Its global-initialization prefix is kept
as source provenance; callers provide the effective live values. The numerical
force prefix218A3C..2190A0 runs before the constraint's three calls. The native
phase transitions are Ready -> ConstraintPending -> Response368Pending -> Ready.
These native guards and atomic failure behavior are API admission contracts.
They are not claimed original game branches.

The wrapper remains a staged subsystem. Its mandatory +368 contact response at
2190E8 is an explicit supplied-state boundary. The enclosing coupled composition
must retain the full represented main body, secondary body, joint and other
external state across that boundary. A copied unchanged snapshot is a caller
assumption, never proof that the original contact response was empty.

## Exact source order and aliases

The hash-pinned ELF has SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Its relevant bindings and call sites establish:

- Main rigid body is vehicle340; main scene is vehicle190/body288.
- Secondary rigid body is vehicle35C; secondary scene is vehicle19C/body288.
- The scene setter1652E0 stores its non-null input to body288.
- The joint at vehicle360 binds A44=secondary and B48=main.
- Secondary construction21256C..2125AC registers it with mask2 and sets320=1.
- `167B80` (vtable38) therefore skips integration, refreshes quaternion-derived
  cached anglesE0 and cached speed34, then routes children. This occurs once at
  the scheduler's frame child phase before substeps, just as for the main body.
- `167C40` (vtable3C) is the actual root integration wrapper. It updates local
  velocity80 and world-angular-velocityA0, but does not refresh cached34/E0.

Within the main substep, producers219C84/219C90/219CC0/219CD4 precede supplied
main354 response219CE8, then probe response219CF4. The optional wrapper call
219D14 occurs only when both vehicle35C and live vehicle838 are nonzero. Thus its
probe104 read is the value AFTER219CF4. In the disabled-probe49K route this has
just become zero, selecting vehicle37C; it does not reuse the main body's gravity
selection, which read the earlier incoming104.

After the prefix, call2190AC invokes49L preparation1B66D0, call2190C0 invokes
correction1B6BA0, and call2190D4 invokes application1B6FF0. Only A receives joint
force/torque. B's scene, velocity and current force-derived acceleration are read
before B's own integration. There is no equal-and-opposite main reaction.

Next come supplied368 response2190E8, secondary48H2190FC, return, main impact
check219D1C..219D9C, main48H219E80, and main-only20C clear219E94. No secondary20C
callback occurs in2188B0. Its238 callback is part of48H and does clear238.

The explicit contact construction binding2127F4..212810 points368 at secondary35C
and scene19C. This narrow binding observation does not prove every alias reachable
by the unrecovered contact stage; the post-stage snapshot must not silently assume
that main state or the joint cannot change externally.

## Prefix arithmetic

All products, sums, divisions and negations below retain source binary32 ordering.
The selected live globals are independent inputs. Their lazy-initialized source
values at2188C8..2189EC are:

| Address | Initial bits | Value |
|---|---|---|
|4346C8|42820000|65|
|4346D0|43AF0000|350|
|4346D8|44BB8000|1500|
|4346E0|43960000|300|
|4346E8|457A0000|4000|
|4346F0|42480000|50|
|4346F8|42F00000|120|
|434700|00000000|0|
|434708|40A00000|5|

`initializedSecondaryGlobals49M` writes only those separately identified fields.
It is not used to overwrite supplied live values or to infer a random seed.

1. At218A3C..218B24 add `(0, (-secondaryMass)*g, 0)` to world force, choosing
   g=vehicle378 if current104!=0 and vehicle37C otherwise. The source performs
   X/Z additions with zero, not merely a Y store.
2. At218B28..218C54, set `a = low + ((cachedSpeed34 - bits422FFFCC) *
   (high-low))/bits41EAAA64`, using low4346D0/high4346D8. The original conditional
   clamp handles ascending, descending and equal endpoints. Add `(-a)*localOmegaB0`
   to local torque, XYZ separately. This reads the frame cache, not current speed.
3. At218C58..218D8C, helper28B1A0 obtains the secondary scene's world translation
   transformed into main-scene coordinates. Subtract it from vehicle2F0, THEN set
   Y to +0. The discarded Y subtraction still consumes its inputs. Compute scalar
   SQRT of squared local error to obtain L. Rotate the error using main28B9B0,
   normalize the resulting world direction with VU reciprocal-SQRT semantics,
   and add `direction * (4346E0 * L)` to world force. This is not equivalent to
   simply multiplying the rotated error by4346E0 for nonorthogonal bases.
4. For vehicle40==0 (218D9C..218EF8), add `(-4346C8)*worldVelocity70` to force.
   Helpers28A700 obtain each scene's world up column. Compute
   `worldTorque = (secondaryUp cross mainUp)*4346E8`; transform it with the
   secondary scene's original inverse-direction helper28BBB0, then add local
   torque. The admitted attached/ready-scene route takes the non-null288 branch.
5. For vehicle40!=0 (218EFC..21909C), evaluate original gradient noise14EEC0 at
   `434708 * vehicle864`. With `h=0.5 + 0.5*noise`, add local Z torque
   `mainCachedSpeed34*(h*4346F8)`. Form local X force
   `mainCachedSpeed34*(h*434700)`, increment vehicle864 by the substep factor,
   rotate that force through the secondary scene and add it to world force.
   Finally add `(-4346F0)*worldVelocity70`. The real source noise module owns
   lazy tables and the explicit shared raw RNG stream; no sinusoid, independent
   seed or fabricated periodic signal is substituted.

Scalar SQRT and VU SQRT/reciprocal behavior remain distinct. The original VU
zero-denominator result saturates Q; a zero restoration vector therefore remains
zero rather than becoming a division-by-zero error or an invented direction.
There is no physical-equilibrium epsilon.49L separately retains its positive
squared-separation admission for the two joint endpoints.

## Canonical state and persistent fields

There is one canonical `RootBodyState48H` per body.49L views are derived at the
actual source stage. The current inverse-mass/inertia values are supplied from
body parameters; extra matrix lanes3,7,11,12,13,14,15 are explicit, with the3x3
entries drawn only from the canonical body's inverse-inertia columns. There are
no independently mutable duplicate3x3 values. All16 world-scene lanes are retained.

The typed bridge retains source velocity70W, angularVelocityB0W, force180W and
torque190W. The prefix and49L change force/torque XYZ only.48H's original14EE40
calls integrate three velocity components; caps and axis-lock writes are also
XYZ, leaving velocity70W/angularVelocityB0W unchanged. Source initializer
3C9290..3C9310 constructs433B80=(0,0,0,1), and full-vector stores1682FC..168340
clear the six accumulators. Therefore retained force180W/torque190W become1 after
48H. Their pre-call values must not be preserved or normalized earlier.

Attached/enabled48H scene setters also invoke28C4F0, which ORs scene190 bit7
(28C4F8..28C50C); final direction transforms refresh bit0. The canonical
sceneFlags190 therefore retains bit7 after integration. Missing/disabled paths
leave scene flags unchanged. The initial source-word comparison exposed this
persistent flag difference before the native bridge was corrected.

Current body velocity, angular velocity, force and torque must flow directly into
49L. Setup-cached49L matrices100/140 persist independently from current mass and
inertia. The previousPositionDerivative, previousLinearAcceleration,
previousAngularAcceleration and previousOrientationDerivative in48C are actual
persistent integration histories. Neither wrapper nor frame cache refresh resets
them. Secondary time864 changes only on the nonzero-state noise route.

## Admission and exclusions

Every bool transition requires FE_TONEAREST and fixed, initialized caller-owned
records. It validates its native phase and elapsed-factor boundary. Prefix scenes
are present and ready; all16 scene lanes are finite. Other checks follow consumed
arithmetic: unused noise fields, time864, main cached speed, alternative gravity
and vector padding are not demanded on state0. Demanded finite arithmetic overflow
fails atomically, including failures after tentative lazy noise initialization.

Hierarchy refresh, absent-scene prefix alternatives, original base/child routing,
scene observers, external contacts and full render propagation remain outside the
prefix's route.48H's established missing/disabled-body clear branch is still
available at the post368 completion leaf. The coupled composition may choose a
narrower attached/enabled domain and must state that admission honestly.

49K's public second-body rejection is unchanged. Actual coupled advancement needs
an explicit49M flow that splits the old combined main-response continuation at
219CFC, inserts this secondary pipeline, and resumes main integration afterward.
It cannot run49K through a transient false secondBodyPresent fact. Existing reset,
main impact223030, query-hit repositioning, vehicle3C override and unresolved
world/contact boundaries are not closed merely by adding this wrapper.

## API verification

`tests/native/secondary_body_api49m.cpp` checks full-lane conversion, source global
bits, phase rejection/atomicity, state0 and nonzero-state routes, selected versus
unused inputs, source noise/RNG persistence, deferred contact effects, cache
lifetimes, zero restoration, actual integration and120 persistent invocations.
The test measures hot-call allocation counts. Source-word differential evidence
is independent of these API/property checks and belongs under
`evidence/secondaryWrapper49M/`.

Final focused API/property runs pass836 checks in both optimized normal and
ASan/UBSan builds, with zero measured hot-call allocations and identical stdout.
The separate original-word differential passes 108 cases and 43,273 full-lane/state
checks (43,274 when writing coverage), after correcting scene190 bit7 retention
found on its first run. It executes 360,020 wrapper/joint instructions, 168,132
original48H instructions, and 1,346,209 independently interpreted noise instructions.
Cold and persistent ready-noise routes are covered. These source tests are distinct
from complete coupled-frame verification.
