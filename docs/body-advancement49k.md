# 49K: staged main-body advancement diagnostic

## What advances

The new native `body_advancement49k` interface joins recovered input, model loads,
body histories and scene writeback into persistent position, velocity and
orientation evolution. It is a deliberately staged main-body subsystem. It does
not claim an original free-flight vehicle, world collision, hover support, race,
full game loop or playable controls.

Production remains typed, serial and fixed-storage. No guest instructions,
filesystem, transport, graphics, heap allocation or world queries execute in it.
Original instructions occur only in independent test interpreters. The diagnostic
supplies a timer sample and pending-overflow count to 49J, then obtains the admitted
factor from 49I. It does not replace that clock with an ideal OS timer.

The source tree was freshly extracted from verified 49J, SHA256
`6376d5cd58cec22eaa1661ae003d3a73a463f90502bad4a515a0a2975bdaaf8e`.
The separate unreleased49C work is absent. Existing collision-capacity and reset
limitations remain unresolved and were not investigated or repaired here.

## Explicit external boundaries

Each continuation consumes a typed observation or post-stage state at its actual
source position. It is not a zero-return callback or an assertion that an omitted
stage has no effect:

1. Ready device-axis/action samples feed the original 48X reader.
2. A post-player snapshot supplies values after unrecovered intervening player
   calls, before the original 217DE0 shaping/filter/binding call.
3. A post-base snapshot supplies the component state after other vehicle frame
   operations, before its registered body-child cache update.
4. The original first/later-substep detection decision selects the required
   preparation boundary. A supplied post-world state carries its effects.
5. A supplied post354 response snapshot carries body, loads, corrections and
   response flags after the mandatory original contact-response call.
6. A supplied completed219120 query result must select its original no-hit
   branch. A query hit is unsupported, rather than silently ignored.
7. An explicit scene-consumer observation acknowledges delivery of the resulting
   scene matrix. The original consumer/observer graph is not implemented.

Test fixtures visibly inject nonzero external loads and corrections, as well as
selected post-player changes, so the composition is tested against supplied
changes rather than demonstrated only with empty callbacks. These are synthetic
fixtures, not recovered contact response or observed gameplay. A caller that
copies a native state unchanged across a boundary is making an external
assumption; that copy is never represented as proof of zero original effects.

The unrepresented auxiliary BA0-to-BB0 inverse-direction stage at219F78..219F8C
is outside the retained body/cache state. No whole post-body vehicle state is
claimed. The integration is useful precisely because its real body output and
its remaining external dependencies are both explicit.

## Source order that must survive composition

Player vtable3CED50 slot38 selects1A11E0. Its reader call is1A15C0; after intervening
player calls it invokes base228620 at1A1644 with the frame factor unchanged.
Base228620 calls217DE0 at22877C, then other operations, and finally its child router
at228F04. Both that binding call and scheduler RunSimulation dispatch are skipped
for vehicle states3/4. The admitted diagnostic rejects those contexts.

Main-body construction calls167920 at211F80, stores vehicle+340 at211F88,
registers it under the vehicle with mask2 at211FA8, and writes body+320=1 at211FB8.
With that explicit topology,167B80 skips integration, calculates all three cached
angles from the body's stored quaternion, then refreshes cached speed. Its child
router follows. The no-child/root-scene domain is explicit.

Consequently binding uses the **old frame cache**. The following body-child phase
refreshes the angle/speed cache once. Those refreshed values remain unchanged
through the subsequent physics substeps; local velocity80, unlike speed34, is
refreshed by each completed48H body update. Refiltering input or recomputing speed
for every substep would change original behavior.

Scheduler1A7B40 takes a child-only route when application+11C is nonzero. The
advancing route requires that supplied observation to be zero. Its child phase
at1A8304 precedes the exact substep policy:

- separately rounded factor times live scale;
- original CVT.W.S truncation, then integer +1;
- original float conversion and factor/count division.

The effective initialized scale is60, but it remains explicit. A zero factor
still selects one substep. Engine smoothing remains per invocation, even for a
zero factor. No leftover-time bank, fixed-frame substitute or catch-up policy is
introduced. The native diagnostic admits only resulting counts1..4 with a
prepared factor in[0,bits3D75C28F]; this bounded admission is not an original
count-limit check.

## Per-substep stages

The new `advancement_selectors49k` leaves preserve these source operations:

- Dynamic gravity21990C..219A88 reads the **incoming**348+104. Zero selects
  vehicle37C. The other branches use brakeEC, cached speed, scene up-axis Y,
  vehicle378 and effective globals434730/434738. Their source initializer bits
  are43C13334/3F34FDF4. Fixed PHY gravity is not substituted.
- Engine flag selection219A90..219AD0 uses vehicle64 equal to1/2 for engine34,
  and vehicleB9C bit1 for engine38.
- First-substep preparation is required. Later preparation depends on incoming
  vehicle380; its zero branch skips the original358/219420 calls. The typed
  boundary distinguishes later-required from later-skipped preparation.
- The explicitly disabled-probe348 preparation runs at219C78. It still updates
  its original component state and transforms its local point.
- Engine generation219C84, gravity219C90, conditional engine application219CC0,
  then aero219CD4 execute in that order. Engine application uses the live supplied
  application/race gate byte's bit1; generation still runs when application does not.
- After the required supplied354 result, the disabled-probe response219CF4 runs.
  It clears104, clears stale11C, sets10C and adds its original airborne torque
  from cached angleE8, component204, effective4342C8 and effective vector434290.
  This branch is not a no-op. Its cleared104 does not retroactively change this
  substep's already-selected gravity.
- Existing48H at219E80 reconstructs the body center/quaternion from the scene,
  runs the persistent Adams–Bashforth integrator, commits scene pose, refreshes
  local/world velocity caches, clears accumulators and performs its238 callback.
  The separate219E94 callback165420 then clears20C.
- After the explicit219120 no-hit query, its current point becomes the prior
  point. Vehicle380 becomes exactly0/1 from response354+44. Only then are the
  original late feedback fields installed for the next producer invocation:
  348+104 to engine3C/aeroE0 and354+3C/+44 to aeroD0/D4.

No convenience function first calls48E's old composed bare-body step and then48H;
that would integrate twice. The new chain calls the existing individual force
producers and invokes48H exactly once at the admitted body boundary.

## Admitted context and arithmetic

The caller supplies initialized body/engine/aero histories, selected runtime
parameters, an attached enabled root scene, zero body/scene children and observers,
body320=1, and the disabled-probe configuration (linked body present,120/124 absent,
100 zero). Second-body work, vehicle3C override, reset, the nonzero-impact223030
branch and query-hit repositioning are unsupported and rejected.

Every new floating transition requires FE_TONEAREST, matching49I/49J. Public
continuations validate their phase and admitted schedule/context. Ordinary finite
binary32 operation ordering is retained; exception flags, arbitrary physical
R5900 denormals/extended range, cycles and hardware concurrency are not certified.
Integer-only selector/clear/feedback leaves do not require a rounding mode.

The persistent phases and rejection policy are native API contracts, not a claim
that the original game used those guards. Native failure does not commit output.
External observations and completed action-query reads cannot be rolled back.

## Restored historical reference and preserved limitation

The exact existing `ee_sqrt14.h` reference was supplied read-only through Library:
1756 bytes, SHA256
`1c479c20842c7334e1112adebfb1903d182b943f9c1bc3ff6aa9e3ed973c3071`.
It is restored at its original test include path. No approximate replacement was
created. Its own bounded host-reference caveat remains in force; scalar SQRT and
RSQRT must not be conflated with the separate VU sqrt-then-divide sequence.

Four unchanged historical numerical suites pass normally and with ASan/UBSan.
The old complete48H test exposes a pre-existing GCC14 -O2 FE_DOWNWARD constant-
folding difference: scene zero+0 versus the original-word reference-0. Its source
and failing log are preserved. The unchanged test passes with inlining disabled.
This milestone does not repair historical48H. A clearly labelled mechanical
nearest-only test copy verifies the admitted49K domain, with source/diff hashes
and all eight original wrapper branch gates retained. It is not reported as a
four-rounding-mode pass.

## Reproduction and evidence

`tools/test_native_body_advancement49k.py` runs the focused advancement and related
retained timing/input/camera/body checks with installed GCC, serially. Use a new
output directory outside the repository. Add `--sanitizers --no-leak-check` for
ASan/UBSan; LeakSanitizer is unavailable under this executor's ptrace setup.
`--stack-usage` records compiler frame estimates, not a measured whole-call-chain
peak. CMake option `RR_NATIVE_BODY_ADVANCEMENT_ONLY` is supplied separately;
CMake is absent here and its route is not claimed executed.

Evidence is under `evidence/bodyAdvancement49K/`: exact ELF words and instruction
coverage, persistent source-compared traces, scalar/state sensitivity, unchanged
legacy regressions and their retained limitation, independent review, final
build records, footprint and checkpoint validation. A diagnostic visual, if
included, uses the trace's verified native scene output and retains its explicit
synthetic external-state and display-placement labels.

## Final measured verification

The final installed-compiler runner passes all20 focused suites normally and with
ASan/UBSan:11,037,512 checks per run,132 tracked inputs unchanged, and identical
per-test stdout. The direct full historical48H all-mode test is not included in
that count; its explicitly named nearest-only copy is. The historical failure
and unchanged non-inlined pass remain available alongside it.

New numeric leaves cover253/253 selected original PCs over8,173 cases and54,019
checks, with227,532 interpreted instructions and8,797 allocation-free hot calls.
The independent review also checks3,607 retained original words against the ELF.
The persistent chain runs180 frames/404 substeps,214,588 checks without a trace
file (214,589 with trace validation),1,202,596 original-word instructions and2,740
allocation-free native transitions. Both final builds reproduce the exact trace
SHA256 `262a69dbcb0a651d69cc207f10174c93c60fccedc72d7c1e859168e8a023a098`.
All16 numeric and12 composition scalar/state mutations are detected, plus4
independent review variants. These are safe behavior controls, not excluded
ownership/capacity diagnostics.

MainBodyState49K is764 host bytes; parameters304. State, parameters and the three
clock/timing records total1,136 bytes. The largest external post-stage record is
536 bytes; caller output copies and other simultaneously retained records are
additional. New optimized object text is7,522 bytes total, with zero data/BSS.
Maximum individual compiler static frames are160 bytes for the new selectors and
1,056 for composition. None is a whole-call-chain or Xbox residency measurement.

## Same-camera diagnostic views

`visual/frame-000.png` and `visual/frame-030.png` show the source-compared scene
outputs at supplied-counter timestamps0 and1012ms. Both raw framebuffer and
captioned PNGs reproduce byte-for-byte. I inspected the actual pixels: the Pod
moves farther into the unchanged TA view and becomes smaller. The fixed recovered
camera,26 textures,360 draw calls and73,409 submitted triangles are unchanged.
All5,755 transformed Pod vertices pass independent scalar checks; UV, colors,
indices and textures retain their prior bytes.

The new renderer applies an explicitly declared **display placement**, not an
original race spawn: remove the old49G translation, apply the native scene, then
add the recorded display offset. Its ordered binary64 display arithmetic followed
by binary32 upload is a diagnostic choice, not a recovered original geometry
program. The native trace stores exact binary32 words separately from readable
numbers. All404 scene records are checked against those words.

The full synthetic trajectory ends at approximately[6.25576,-153.87732,200.11491]
in its fixture coordinates. It can fall through the displayed ground because
world/contact effects and a no-hit result are supplied. No ground clamp, camera
re-aim, recentering or invented collision support was added. The two captions state
these limits. Original lighting/material programs and real gameplay FPS remain
outside the view.

Per-frame GL geometry/texture payload is7,604,084 bytes. Existing input plus GL,
framebuffer, readback and flip payload accounting is20,123,816 bytes, with a138,120-
byte Pod vertex copy and bounded transformation work additional. Actual diagnostic
process RSS was about132MB, including Python/Mesa/driver overhead. This installed
single-thread llvmpipe route is development visualization, not an Xbox renderer
memory budget or a game performance measurement.
