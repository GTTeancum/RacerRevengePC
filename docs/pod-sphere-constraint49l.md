# 49L: original PodSphereConstraint

## Delivered boundary

`src/game/pod_sphere_constraint49l.{h,cpp}` implements a real missing vehicle
component: original constraint setup, anchor preparation, acceleration correction
and prepared-force application. It uses fixed-size typed body and joint records,
strict binary32 operations and no allocation, callbacks, guest instructions,
queries, renderer or platform transport in production.

This is not a generic symmetric joint. **Only the linked secondary/chariot body
receives force and torque.** The main body contributes endpoint position, linear
velocity and force-derived acceleration. Applying an invented equal-and-opposite
reaction to the main body would change the game.

The component stops before the secondary contact response and body integration.
The existing 49K main-body interface is unchanged and still rejects its unsupported
second-body context. No complete two-body vehicle tick, new trajectory or new
rendered coupling is claimed by this checkpoint.

49L begins from verified 49K ZIP SHA256
`ed009690cd0af3c669e9d45927847d7f3fc9406b3c4aaff7e865199886595009`.
All original code and historical evidence remain present. The separately held
49C work remains excluded and unreleased; flagged capacity/ownership work and
reset behavior were not inspected, retried or repaired.

## Original identity, bindings and order

The hash-verified ELF establishes these relationships:

- Main body: vehicle+340, PhysicsRigidBody constructor167920, scene vehicle+190.
- Optional body: vehicle+35C, the same PhysicsRigidBody type, scene vehicle+19C.
- Constraint: vehicle+360, constructor1B6200, vtable3CF290, RTTI literal
  `PodSphereConstraint` at3AC420.
- Constraint body A at+44 is the optional body. Setup1B64D0 binds its local anchor.
- Constraint body B at+48 is the main body. Setup1B6670 binds its local anchor and
  clamped rest length. The generic component also supports absent B and a fixed
  world endpoint.

The optional-body wrapper2188B0 invokes the three constraint slots in order:

1. +74 →1B66D0, preparation, call2190AC
2. +78 →1B6BA0, acceleration correction, call2190C0
3. +7C →1B6FF0, prepared-force application, call2190D4

It then invokes the separate +368 contact stage and secondary-body integration.
Neither is silently skipped or implemented by this new component. RunSimulation
reaches2188B0 only when vehicle+35C and live mode+838 are both nonzero.

The three rendered pieces do not establish three rigid bodies. Source render
selection21B9A4..21B9C8 maps the active physical chariot scene+19C to the rendered
chariot when838!=0 and83C==0; alternate/blended scene modes also exist. This static
binding proof does not close all rendering/mode selection or scene-parent behavior.

## Setup and persistent state

The API preserves four-lane vectors and all16 matrix lanes, plus source flags and
caches. It stores native presence facts rather than guest object addresses. The
caller supplies the corresponding body records at each stage and owns their
initialization, lifetime and call order.

`initializePodSphere49L` implements the represented **numeric constructor suffix**
1B6214..1B6468 after the excluded base initializer1276E0. It does not construct an
original GameObject or implement base-object lifecycle. The suffix itself writes
enabled38=1, unilateral3C=1, suppressed40=0, absent A/B references, zero lengths,
zero matrices and source-initialized vector values. Its coefficients are:

- damping = binary32(2 / binary32(0.05)), bits42200000,40
- stiffness = binary32(1 / binary32(binary32(0.05) × binary32(0.05))),
  bits43C7FFFF,399.999969482421875

Replacing stiffness with the decimal literal400 is incorrect. Initialized global
vector434390 is independently established by original initializer words and used
under that declared initialized-global contract.

Setup A caches the then-current inverse mass and inverse-inertia contribution:

- cached140 = diagonal(uA,uA,uA,1)
- cached100 = S(r) × (IA × S(r)), where S(r)x = r × x

The local anchor r is used as written; setup does not subtract the local center of
mass. Point-force torque application later does subtract the center. Preserve
that distinction rather than substituting a textbook joint formulation.

Setup B stores its anchor, clamps length using the original MAX with bits3DCCCCCD
(approximately0.1), and squares the resulting binary32 length. The cached matrices
remain unchanged by ordinary preparation. Correction separately reads current A
inverse mass/inertia; a later body parameter change must not retroactively rebuild
setup caches.

## Recovered equations and demand

For explanation, R denotes A's ready world basis, r its local anchor, u inverse
mass, I inverse inertia, and F/T accumulated world force/local torque. The actual
implementation preserves original full-lane matrix layout and operation order.
It does not substitute a general mathematical inverse for the ready-scene helper's
transpose-based procedure or reassociate its matrix products.

Preparation1B66D0:

- pA = A's transformed local anchor, stored80
- pB = B's transformed local anchor or fixed endpointA0, stored90
- d = pA−pB, storedD0; D = d·d, with **no square root**
- centripetalC0 = omegaA × (omegaA × r)
- v = vA + R(omegaA × r) − vB; absent B contributes zero
- speedSquared54 = v·v, not speed
- E = d × binary32(1/D), storedE0; not separately divided XYZ components
- K = cached140 − R × (cached100 × transposeBasis(R))
- inverse180 = original inverse3x3(K)
- q = stiffness30 × (lengthSquared50−D) − damping34 × (d·v)
- preparedForceF0 = inverse180 × (qE)

B's angular velocity, inertia and torque are not read. In particular, the original
omits B angular motion even if B's local anchor is nonzero.

Correction1B6BA0:

- aA = uA FA + R[(IA TA) × r + centripetalC0]
- aB = uB FB, or zero without B
- C = inverse180 × [(-speedSquared54 − d·(aA−aB)) E]

If unilateral3C is nonzero and dot(F0+C,d)>0, correction sets suppressed40=1,
applies neither force and returns+0. Equality to zero does not suppress. Otherwise
it clears suppressed40, applies C, and returns its original scalar-root magnitude.
The final stage applies the already prepared F0 unless disabled or suppressed.
It does not recompute F0 after correction changes A's loads.

For each world force X applied at pA:

- localForce = original inverse-direction transform of X
- localLever = original inverse-point transform of pA − A.localCenter
- A.worldForce.xyz += X
- A.localTorque.xyz += localLever × localForce

The correction and prepared force are added in separate, ordered calls. Body force
and torque fourth lanes remain untouched. All B fields remain unchanged.

## Full-lane fidelity, singularity and native admission

Original VU mask-XYZ helpers retain and then increment a register's W lane. The
persisted preparedForceF0.w is generally `add(add(P[3],1),1)`, where P is the final
preparation matrix product; it is normally2 for the admitted ordinary affine
case, not1. The full cache is preserved rather than normalized to a conventional
homogeneous vector. Tests also cover non-affine matrices as numerical inputs;
they do not assert such matrices are ordinary physical vehicle poses.

The original inverse compares its computed determinant **exactly** with zero.
Zero produces an all-zero16-lane result. Nonzero determinants are not replaced by
an epsilon cutoff. All non-3×3 output lanes are zero even on the nonsingular path.

There is no original guard before reciprocal squared separation. The native API
admits finite intermediates and strictly positive computed D, rejecting coincidence
or underflow-to-zero D without inventing an epsilon, direction, or silent skip.
That rejection is a native boundary, not recovered original error handling.

All entry points require FE_TONEAREST, consistent with49I/49J/49K. Enabled stages
require the corresponding present A and ready scene (flag190 bit0). Preparation
also requires a ready B scene when B exists. Dirty-scene refresh/traversal and
missing-scene alternatives are outside this route. Disabled stages return without
changing joint/body state; correction returns+0 and preserves prior suppressed40.
The final suppressed stage returns before demanding body data.

Finite checks are demand-driven. Review found two easy-to-miss actually consumed
W lanes: present B sceneWorld100[11] during preparation and current A
inverseInertia[11] during correction. Original masked operations later subtract
those lanes from themselves, so NaN/Inf is outside the finite-intermediate domain.
The final code rejects those inputs after the disabled gates; unused lanes3/7/15
remain accepted where the source does not consume them. Initial reproductions,
the correction and final318-field source-arithmetic audit are retained.

Each bool return reports native admission/computation success. Failure leaves
represented output unchanged. These API guards and atomic commit policy are not
claimed original validation logic. Floating exception flags, every physical EE/VU
exceptional mode, cycles and concurrency remain outside the proof.

## Parameter and integration boundaries

Source construction binds A's local anchor from vehicle300=(0,0,3), and derives the
supplied constraint length from vehicle858. Effective inertia, anchors, length and
body scene values remain explicit native inputs.

Do not substitute a raw `ChariotWeight` label or a stored render bound for live
body parameters. Construction supplies secondary mass bits40C6C1FD and derives its
inertia from a refreshed aggregate scene radius. Named PHY data alone does not
prove the final runtime value. This checkpoint does not implement the asset-to-
live-body initializer, bound refresh, model selection or rendering attachment.

The preceding optional-body force prefix also contains gravity/damping/restoration
and another state branch with gradient noise and RNG-dependent initialization.
That noise is not a sine wave. None of those missing operations is fabricated here.
The new source closes exactly the constraint component and can supply its actual
load effects to a future correctly ordered secondary-body pipeline.

## Evidence and reproduction

`tools/gameplay/pod_sphere_constraint_recover49l.py --check` verifies exact original
instructions/data from the pinned ELF. `sphere_bindings_provenance49l.py --check`
verifies narrow static type/binding/caller evidence. Neither reads RAM or runs the
game. The independent interpreter operates only on bounded initialized synthetic
records and the admitted original instruction graph.

`tools/test_native_sphere_constraint49l.py` builds the new component and retained
body/timing/input/camera tests serially using installed GCC. Output directories
must be new and outside the source tree. Add `--sanitizers --no-leak-check` for
ASan/UBSan; LeakSanitizer is unavailable under executor ptrace. CMake option
`RR_NATIVE_SPHERE_CONSTRAINT_ONLY` is provided but CMake is absent and unexecuted.
The exact historical SQRT reference from49K remains unchanged; no substitute was
introduced. The historical48H downward-rounding GCC issue remains separately
recorded, with only its labelled nearest-only copy in the admitted regression set.

Proof, full coverage/exclusions, API tests, independent review, footprint and final
verification are under `evidence/sphereConstraint49L/`. No full vehicle, actual
world traversal, Xbox memory ceiling or gameplay FPS is inferred from these checks.

## Final verification and footprint

Both final installed-compiler builds pass all 22 focused suites: 13,166,474 checks
per normal and ASan/UBSan run, with all 142 tracked inputs unchanged and identical
per-test stdout. The retained 49K trace and earlier evidence remain unchanged.

The new exact-word suite covers 535 cases and 2,102,648 assertions without output,
or 2,102,649 when writing/validating coverage. It executes 1,419,380 original
instructions and measures 3,202 allocation-free native calls. Coverage reaches
1,514 of 1,639 frozen instruction words. The 125 exclusions are explicitly
classified: 5 inherited-constructor prefix words and 120 dirty-scene words.
Admitted branch outcomes are asserted; ready-scene and null-scene boundaries are
not falsely credited as traversed.

The API suite passes 26,314 checks and 12,134 zero-allocation transitions.
Independent review adds 325 source-word cases, 1,283,431 comparisons, six detected
scalar/state mutations, and a 318-field nonfinite-demand audit with no remaining
accepted/nonfinite-intermediate gaps. Both initial admission defects and their
verified corrections are preserved in evidence. Normal review executables also
passed on one CPU under a 64 MiB virtual-address limit; that is a test-process
observation, not a claim that the full game fits that limit.

Host records are 16-byte vectors, 64-byte matrices, 220-byte bodies and a 388-byte
joint. One joint plus two body records is 828 bytes; other caller state and copies
are additional. New optimized host object text is 18,428 bytes, with zero data/BSS.
Largest individual compiler static frame is 1,248 bytes; nested helpers and library
calls are additional. No target-console or whole-call-chain memory bound is claimed.
