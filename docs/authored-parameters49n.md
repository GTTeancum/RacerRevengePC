# Bounded authored parameter binding, 49N

## Scope

`src/game/authored_parameters49n.{h,cpp}` projects selected numeric PHY values
into the existing engine48D, aerodynamic48E, joystick48W, main-body49K and
coupled-body49M APIs. It does not initialize pose, integration history, scene
bounds, secondary-body physical parameters, joints, controls history, or world
state. It does not modify any inherited49M production implementation.

The source is the original `SLUS_202.68`, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The actual source PHY is `Pods\Pod01\Pod01.phy`, 6,584 bytes, SHA256
`47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b`,
from the 349,110-byte `POD01.RES`, SHA256
`08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`.
The caller212DE8 passes its already-opened PHY reader to2146C0.
Asset reading and decimal conversion are a separate49N component; the numeric
adapter does not embed a Pod01 table or read live process memory.

## Public contract and admission

- `AuthoredParameters49N`: 61 typed scalar-key slots with explicit presence,
  two engine vectors with explicit presence, and optional Tuning fields.
- `AuthoredSelectionContext49N`: explicit inferred class, vehicle58 nonzero
  test, resolved configuration-entry test, later profile-overwrite gate and
  the selected profile/pod's three abilities. No profile, player, track,
  manager, or pod identity is guessed by the adapter.
- `AuthoredBinding49N`: existing main/control/rate types, vehicle gravity,
  complete inertia and inverse-inertia matrices, final engine5C difference,
  and a diagnostic report of the effective initial class/mode and whether the
  positive-only mass setter actually wrote. The latter preserves the49M
  recipient's own mass fields on a nonpositive-mass projection.
- `bindAuthoredParameters49N`: preserves previous-owned unbound fields and
  atomically writes all represented source-owned numeric outputs.
- `projectAuthoredCoupledParameters49N`: copies only recovered parameters to
  the genuine49M parameter type, including all seven supplemental inverse
  matrix lanes. It preserves secondary parameters, main limits, axis enables,
  centers, probe inputs, force X/Z, and unresolved aerodynamic damping.
- `projectAuthoredGravity49N`: copies vehicle378/37C into already-resolved49K
  substep-selection and49M secondary contexts, retaining their other fields.
- Control outputs already have the exact existing48W/48B types and are passed
  directly to `bindCoupledBodyFrame49M` or `bindMainBodyFrame49K`.

The native domain requires FE_TONEAREST, finite consumed values/intermediates,
valid class A/B/C, and presence of mapped keys consumed on the selected path.
Missing required values reject atomically. This is intentionally narrower than
the original reader:158A70 skips the store when a key is absent, and the loader
does not establish a default for every destination. Physical scalar stack
slots in particular are not initialized before lookup. The native API never
invents values for them. Its complete-input rule also applies to mapped bulk
aero fields whose original temporary is zero-initialized by100430, and direct
aero fields whose original missing reads preserve prior component values.

Unused class/ability keys, inactive profile abilities, and ignored Tuning
fields are not demanded. Raw engine vectors are not demanded when the proven
configuration branch replaces them. The separately bounded text reader may
admit a narrower, complete mapped-key file domain. These gates are admission
choices, not claims about original loader validation.

All transitions use fixed storage, perform no allocation, support normal
input/output aliasing, and leave outputs unchanged on failure. Arithmetic
helpers round each scalar operation to binary32, check intermediate finiteness,
and must be compiled without fast math or FMA contraction. This is equivalence
to original-word host arithmetic on the admitted domain, not a claim of full
PS2 floating-point exceptional-mode emulation.

## Main body and gravity

At2147F4..21488C, with `g = bits4200CCCD`:

1. Gravity and AirborneGravity each become authored value times g.
2. `mass = Weight / g`.
3. `t = mass / 12; t = 2 * t; t = t * 10; t = 10 * t`.
4. A zero matrix from14D7F0 gets XYZ diagonal
   `(PitchInertiaScale*t, YawInertiaScale*t, RollInertiaScale*t)` and lane15=1.
5. Setter165300 writes mass and `1/mass` only if mass>0; otherwise both
   previous fields remain unchanged.
6. Setter165340 copies the complete inertia matrix and calls14E910. The
   adapter retains its scalar-ACC cofactor/determinant order, including signed
   cofactor zeros. It does not substitute reciprocal diagonals. Singular
   determinant returns an all-positive-zero inverse. All inverse lanes
   outside the XYZ3x3 are +0, including lane15.

Only the inverse XYZ3x3 is projected into `BodyParameters48C`; complete
inverse lanes3,7,11,12,13,14,15 are separately projected into
`MainSphereParameters49M.inverseInertiaExtra`. The full `inverseInertia140`
record is canonical for both portions of the49M projection. The binding's
mirrored `main.body.inverseInertiaColumns` exists for the direct49K API and is
not read by the49M projection, so inconsistent copies cannot be combined.

`ChariotWeight` is not a main-body parameter and is deliberately absent from
this projected schema. It is not the mass argument for the secondary body's
earlier construction. No secondary initializer is claimed here.

## Initial selection and Tuning

At214954..214AA0 the manager-returned track string selects A if it contains a
lowercase `a`, otherwise B if it contains lowercase `b`, otherwise C. Exact
`sb` overrides to C; exact `ra` or `sc` overrides to B. The text helper exposes
this operation separately, with a native bound of255 bytes, no embedded NUL,
and explicit length excluding the terminator. It does not find the live string.

At214AA4..214BA4, UseValues starts enabled. Absent Tuning or absent UseValues
keeps it enabled. Only a found UseValues equal to0 disables it. When enabled,
found UseAValues/UseBValues/UseCValues equal to1 override sequentially, so C
wins. Other integer values do not select that override. UpgradePercent starts
at0 and is read only when UseValues is disabled.

Class mode reads MaxThrustA/B/C, TheoreticalTopSpeedA/B/C and
SpoilerYawMomentA/B/C. Interpolated mode uses separately rounded operations:

`ability = base + upgrade * (maximumAbility - base)`

`selected = minimum + ability * (maximum - minimum)`

There is no clamp in these initial interpolation paths. Steering's two
endpoints receive SpoilerYawMomentOffset first when vehicle58==0; when
vehicle58 is nonzero, the original adds the explicitly initialized zero.

## Engine mapping and late writes

The setter2E7680 is called at2154D0. MaxThrust, BoostThrust, RepairThrust,
RampUpFilter, and RampDownFilter map directly. RampDownSpeed, selected
TheoreticalTopSpeed, BoostSpeed and RepairSpeed each multiply by exact
bits3FBBBB84. LeftEnginePosition and RightEnginePosition are direct torque
bases in48D, not invented cross-product lever arms.

ActualTopSpeed is also converted, but is not a48D producer parameter.
At216000..21600C, after any later profile overwrite, engine5C becomes final
engine58 minus converted ActualTopSpeed. The adapter exposes this separately
as `engineDifference5C`; it does not insert it into an unrelated48D field.

The source-order trap at215408..21549C is preserved. If vehicle58==0 and the
caller-resolved manager11040+4*vehicle5C entry is nonzero:

- Aero steering100 is multiplied by0.5.
- Left torque XYZ becomes `(0,-halvedSteering,0)`.
- Right torque XYZ becomes `(0,+halvedSteering,0)`.

Later, global382B80->350==0 calls216040. The caller supplies the abilities
selected by manager100C0 and vehicle60 at1920/1924/1928:

- Thrust: min/max interpolation with original subtract0/divide1 operations,
  then source comparison clamps supporting either endpoint order.
- Top speed: min/max interpolation without a clamp, followed by units
  multiplication with the original operand order.
- Steering: offset is added to both endpoints regardless of vehicle58,
  then the same clamped interpolation operation.

This later overwrite never rebuilds the already-copied engine torque bases.
For the actual Pod01 class values and an explicit synthetic profile
`(.2,1.5,.25)` with the configuration branch selected, final thrust is4800,
top speed is1100 times the unit constant, steering is-5750, and engine torque
Y values remain +4000 and-4000. Those profile values are a test scenario, not
a recovered player save or the user's selected profile.

## Aerodynamic key-to-field mapping

The bulk temporary→setter path214BC0..214DE4 and1004B0 establishes:

| Existing48E field | Authored key |
|---|---|
| offset68 | DefaultAngleOfAttack |
| lift6c | Lift |
| drag78 | BasicDrag |
| pitch84 | PitchMoment |
| pitchDamping88 | PitchRatePitchMoment |
| lateral90 | SideSlipSideForce |
| rollDampingB0 | RollRateRollMoment |
| timerYawB8 | SideSlipYawMoment |
| yawDampingC8 | YawRateYawMoment |

The direct writes214DE8..214ED4 establish:

| Existing48E field | Authored key |
|---|---|
| driveDragF8 | SpoilerDrag |
| brakeDragFc | SpoilerBrakeDrag |
| steeringRamp104 | SpoilerYawMomentScale |
| timerSteering108 | SpoilerSideForce |
| timerLateral10c | PowerslideSideForce |
| timerYaw110 | PowerslideSpoilerYawMoment |
| timerDrag114 | PowerslideThrust |
| tiltedLateral118 | InvertedSideSlipSideForce |
| tiltedSteering11c | InvertedSpoilerSideForce |
| tiltedYaw120 | InvertedSideSlipYawMoment |

Steering100 follows the selection/order rules above. The inspected PHY chain
does not establish velocityDamping130; it remains an explicit previous-owned
effective input. Extra authored coefficients not represented in48E are not
silently assigned to different producer fields.

## Joystick mapping

At215A80..215AF4, SteerSpeed1, SteerScale1, SteerSpeed2, SteerScale2 map directly
to B84/B88/B8C/B90. XAxisRateLimit/YAxisRateLimit map to the existing48B rates.
These are not multiplied by the engine speed-unit constant. Lazy globals
4346B8/4346C0 remain explicit previous-owned control inputs.

## API checks

`tests/native/authored_parameters_api49n.cpp` tests allocation-free admission,
selection exceptions and sequential overrides, missing/NaN field-demand
matrices across tuning/profile/mode branches, profile clamps and unclamped
speed, the later-overwrite torque trap, singular/nonpositive body cases,
projection preservation, all inverse supplemental lanes, aliasing, overflow,
non-nearest rounding and atomic failures. Its embedded numeric fixture is
labeled separately from actual PHY-byte ingestion. Independent original-word
differential checks and connected actual-asset49M diagnostics are separate
test owners; this API suite does not claim those checks by itself.
