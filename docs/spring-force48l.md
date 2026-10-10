# Native spring force and secondary response (48L)

`src/game/spring_force48l.*` recovers original `297190` and `2983C0` as
allocation-free native C++17 helpers. They use the existing 48K body model and
48J point-force helper. No guest addresses, PS2 memory, emulation loop, global
state, host services, parallel work, or allocation enter either production API.
The separate generated-contact coordinator owns ordering and cache preparation.

## Original spring-force path

`applySpringForce48L` consumes the caller-prepared relative velocity cache
`contactVelocity120`, normal, world point, penetration, selected settings and
recipient loads. Its instruction order is:

1. Select `impactStiffnessA4` / `impactDampingA8` exactly when `impact54` is true
   and either no other body exists or `otherSpring58` is false. Otherwise select
   `stiffness9C` / `dampingA0`.
2. With no other body, use mass factor 1 without reading mass or inverse mass.
   With another body, multiply the smaller mass by the larger body's **stored**
   inverse mass. Equality takes primary mass times the other inverse mass.
   This does not divide masses or recompute reciprocals.
3. Compute spring term as stiffness times `min(penetration,maxPenetration90)`.
   There is no lower penetration clamp.
4. Dot the cached relative velocity with the normal in the original binary32
   order, then compute `min(max(speed,-maxSpeed94),maxSpeed94)`.
5. Separately round damping times clamped speed, subtract from spring term,
   clamp to at least zero, multiply by the mass factor, then scale the normal.
6. Call native `addSpringPointForce48J` on the primary, and on the other with
   the exact negation when present. The world-zero-point sentinel omits torque;
   both recipients use the primary body's configured local center, or the
   custom center, according to the unchanged 48J helper.

As corrected in48O, scalar MIN.S/MAX.S use signed-word ordering, including
-0 below +0. The earlier comparison-based evidence model collapsed opposite
signed-zero ties; its saved corpus outputs remain unchanged, but new secondary
edge fixtures expose an output difference. See [the audit](minmax-audit48o.md)
for the correction, preserved historical source and current regression. Normals,
limits, masses and coefficients are not silently normalized or made positive.
Only finite consumed values/intermediates are accepted. Ignored coefficient
pairs, unselected inverse mass, derivative caches and unrelated body fields
may be nonfinite. A self-other pointer is rejected. All output fields commit
only after both recipient paths succeed; no-other secondary loads are zero.
The caller owns the rounding mode and strict binary32 compiler configuration.

## Secondary inward-velocity path

`removeSpringInwardVelocity48L` implements original `2983C0`:

- Dot the cached contact velocity with normal
- Select `min(speed,0)` and negate it
- Multiply by normal and add to the primary velocity

The primary's current velocity is an accumulation destination, not the source
of that projection. The original ignores point, penetration and other-body
arguments. No angular velocity, loads, derivative history or correction
accumulator is updated. Output may exactly alias an input vector; failures
leave it unchanged.

## Independent instruction evidence

`tools/gameplay/spring_force_fixtures48l.py` produces:

- 2,048 force cases, 76 input and 15 expected output binary32 words per case
- 512 secondary cases, 9 input and 3 output words per case
- Explicit source hashes, per-instruction coverage, branch outcomes and force
  case labels in `evidence/springResponse48L/`

Force words come from hash-pinned `springOrchestration48K` and `spring48J`
proofs. Secondary words cover exactly `2983C0..298474` exclusive (45 words),
checked against every generated annotation and the authenticated original ELF
SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The frozen secondary proof can regenerate fixtures without the original ELF.
Every data byte read by these runs was explicitly initialized or written by
original instructions. There are no patched instructions, native expected
values, helper stubs, guessed object bindings or implicit zero-filled RAM.

Force fixtures span all 32 combinations of other-body, impact, other-spring,
custom-center and yaw-only modes. They include adjacent binary32 values at
penetration, both speed clamps, zero-force and equal-mass thresholds, stored
inverse masses intentionally unequal to recomputed reciprocals, signed zeros,
nonzero loads, transformed scenes and finite negative settings. Secondary
fixtures cover inward, outward and zero speed, independent cached/current
velocities, signed zeros and the complete 45-word function.

The strict GCC C++17 focused test passed 39,001 checks: bitwise fixture outputs,
unchanged inputs, selected/ignored finite validation, overflow that a later
clamp would conceal, opposite-recipient transactional failures, same-body
rejection, secondary in-place output, and preserving the caller's rounding
mode. Build/test logs are outside the source tree. The parent milestone's
runner and aggregate checks remain the integration authority.

This is ordinary individually rounded IEEE binary32 instruction-model evidence,
not proof of all physical EE/VU exceptional arithmetic modes, whole-game
integration, native gameplay performance or original-Xbox execution.
