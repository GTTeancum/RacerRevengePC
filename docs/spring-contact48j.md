# Native spring friction and point-force accumulation, checkpoint48J

`applySpringFriction48J` implements original297390; `addSpringPointForce48J`
implements original296C10. These are native typed C++ functions, with no guest
registers, addresses, dispatch, PS2 devices or instruction execution in production.
They are compiled into `racer_native_game` and independently exercised on Windows
x64 and x86. Full live collision orchestration is still incomplete.

## Preserved behavior

The friction input velocity is the caller-prepared spring contact velocity,
original SpringCollisionModel+120. Its normal component is removed. The negative
normalized tangential direction is scaled by
`32.2 * (primaryMass * (min(0.1 * tangentLength, 1) * coefficient))`, with the
original multiplication order and separately rounded binary32 operations.
Normalization uses the original VU-style rounded square root then reciprocal;
the scalar length calculation separately uses scalar reciprocal-square-root
then reciprocal. These are deliberately distinct calculations. Zero tangent
preserves the original signed-zero force behavior.

The force is added to the primary body and, when present, its negative is added
to the other body. The point-force helper accumulates world force and local
torque. It transforms both world force and contact point through the recipient's
transposed scene basis, computes the lever arm and cross product, applies the
original yaw-only restriction when enabled, and scales torque before accumulation.
The default lever-arm center comes from the **primary** body even when applying
force to the other body. The custom-center flag substitutes spring+100.

World point `(0,0,0)` is the original no-torque sentinel. The force is still added;
scene, center and torque scale are not consumed in that branch. This sentinel is
preserved from original RAM4435A0 with a hash-verified 16-byte evidence file.
Finite-domain failures leave output untouched. Matrices are finite affine
column-major matrices; this path transposes the basis, without repairing or
generally inverting nonorthogonal inputs. Round-to-nearest is the validated mode.

## Independent evidence and native checks

`spring_recover48j.py` extracts 1,776 instruction words from the immutable ELF,
checks available generated-source annotations and stores exact bytes, source
line anchors and hashes. The broader extraction also prepares the next normal
response implementation. It includes complete world/local direction helper
bodies and their scene-refresh/composition dependencies.

`spring_fixture48j.py` executes the actual original297390 and296C10 words through
the existing offline48H instruction oracle, extended only for scalar min/max.
It runs 1,024 deterministic cases: one/two bodies, original sentinel, zero tangent,
the tangent-speed clamp boundary, coefficient zero, yaw-only and custom centers,
positive/negative torque scale, existing forces/torques, rotations and finite
nonorthogonal scene bases. Every input data byte is explicitly initialized; there
are no reads from unspecified source RAM. The oracle executes 396,096 instructions.
This evidence models ordinary individually rounded IEEE binary32 behavior, not
all physical EE exceptional or denormal modes.

Fixture SHA-256:
`c8ef93ae47db9393a9bac6c82bdbbbc35dbacd5ea25badba0211ab8bf391b3b8`.
The native test compares all 15 output float words exactly, including zero signs,
and verifies failure transactionality and sentinel input consumption. Both
architectures pass **16,392 checks**. The full suite passes **20/20 tests** each at
`evidence/nativePc48J-build-20261003-172835-433`.

The first build failed because this new source lacked its `<cstdint>` include.
That compile failure and the correction's successful build are retained. The
older48G collision test retains its pre-existing integer-conversion warning.

## Next response routine: recovered, not yet native

`spring_impulse_replay48j.py` executes original297610 with its real helper calls
against the saved primary/second-body state. Eight explicit scenarios combine
optional second body, zero/positive penetration and saved/approaching contact
velocity. Contact point and normal come from the verified48I floor query; this
is not a claim that the live caller selected these exact arguments. Four cases
explicitly replace contact velocity with `(0,-25,0)` to exercise the impact branch.

All eight replay exactly from a guarded **711-byte source-data fixture in40 slices**.
Instruction counts are451,723,935,1207,583,1122,1067,1606. Before/after force,
torque, velocities, accumulated velocity changes and correction vectors are
preserved in `evidence/spring48J/impulse-saved-replay.json`.

Recovered ordering for the upcoming native implementation:

1. Choose restitution from normal Y or the other body's mass; prepare local arms.
2. Compute inverse effective contact mass from both translational and rotational terms.
3. For approaching relative velocity, apply the impulse to primary and optional
   second-body linear/angular velocities and their accumulated changes.
4. Compute primary contact acceleration from current loads; the other body uses
   its cached acceleration fields. Apply the opposing normal force to the primary
   body through296C10 with its separate torque scale.
5. When enabled with positive penetration, aggregate position and rotation
   corrections through165580/165740. These avoid double-counting correction in
   an already covered direction; simple vector addition would be incorrect.

Inline inverse-direction operations omit a final add-zero, while helper28BBB0
performs that add at28BD20. Preserve this distinction when implementing the
remaining routine. Scene dirty/parent refresh must be represented explicitly at
the native scheduling boundary, not silently assumed complete.

ELF SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
RAM SHA-256: `38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48`.

## Remaining goal work

Implement native297610 and surrounding spring orchestration, prepare collision
queries from live body state and native world metadata, close auxiliary/group
paths, and connect the recovered functions in the original vehicle schedule.
Native full races, camera/AI/audio/progression, final rendering and 60 presented
FPS remain unverified. No runtime FPS gain or Xbox performance is claimed here.
