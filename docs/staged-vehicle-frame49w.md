# 49W: staged advancing vehicle frame

49W connects the recovered input, graphics and body stages on one persistent
vehicle. The original main body keeps its real graphics child. Body poses,
orientations, derivative histories, lazy scene caches, shared noise and mode
state survive from one frame to the next. The late authored type-0 collision
object also follows the original integrated main transform.

This is a staged advancing diagnostic with explicit caller-supplied boundaries.
It is not a complete game loop. Authentic spawn, active world/contact production,
query-hit/reset handling and the omitted component/observer effects remain open.
The unreleased49C work is not included. All49V and earlier production guards and
historical evidence are preserved.

## Production contract

`src/game/vehicle_frame49w.h` defines fixed caller-owned state, workspace,
parameters, bindings and twelve ordered phase calls. One `CoupledBodyState49M`
owns both numerical bodies, their histories/caches, vehicle838 and secondary864.
The persistent53-node forest owns local and cached world transforms. The body
scene records are checked projections of that forest. One serial RNG/noise
context serves engine roots, Chariot response and admitted physics.

Two new complete numerical leaves support the bridge:

- `graph_body49w` executes the admitted original167C40 integration wrapper with
  actual descendants and lazy cache invalidation. It retains full vector lanes
  and the exact52-byte activation supplement, plus six otherwise absent W words.
  The root's initial point getter, integration, orientation setter, optional
  correction, second point getter, translation and final cache consumers keep
  their source order. Descendants are invalidated, not eagerly refreshed.
- `pod_shape49w` decodes actual `Pods\Pod01\podboxf.col` and executes original
 11FF90/11F680/120230 type-0 transform semantics: previous/current matrices,
  original four-term local composition, root center and scalar-root radius.
  The source EE scalar-root behavior includes flushing subnormal squared radii.
  Current registrationE0 must be null; spatial registration is not implemented.

The bridge admits the ordinary initialized route: vehicle58=40=3C=0,
application11C=0, body320=1, present/enabled parentless physical roots, actual
empty body component routes, null physical scene observers, original integration
and callback selectors, coherent fixed topology and current shape registrationE0=0.
Main190 retains child194. Canonical playerB9C bit7 must already be set, excluding
first-base217440 initialization. Finite source domains and separate binary32
rounding under FE_TONEAREST apply.

Each native call stages and atomically publishes represented state in fixed,
disjoint caller storage. It allocates no memory. The whole frame cannot roll
back an external action query or a caller's completed effects. Parameters,
bindings and effective source constants must remain stable within a frame.

## Original order and remaining boundaries

The native order is reader1A15C0, shaped/bound217DE0 controls, internal animation
21A000, engine response21C120, Chariot lifecycle21A660, postgraphics BA0 clear,
main/secondary frame-cache updates, original substep selection, forces, secondary
and main graph-body wrappers, query219120 result handling, inverse impact XYZ,
feedback, and the actual late authored shape transform.

Player/base work and other component children are named supplied preserving
cuts. Their represented scenes, bodies, history, controls and noise must remain
unchanged except for explicit supplied post-player results. World/354/368
contracts permit typed velocity, load, correction, accumulator, W-lane, body20C,
joint and impact effects. They cannot replace a pose, derivative history,
frame cache, phase864, topology or RNG. Only354 can supply current838.

The original no-hit query point is derived from the integrated main transform.
Its XYZ setter preserves existing7CC.W. The caller supplies actual query-result
flags; hit repositioning is rejected. The inverse impact record contains BB0.XYZ
only. BB0.W depends on surviving source scratch after opaque work and remains
unknown. The admitted native consumers do not observe it, as checked against
original words and poison controls. All supplied stages must likewise exclude
consumption of that omitted word.

Frame caches34/E0 update once after graphics, before all physical substeps.
Disabled-probe, engine, aero and dirty-secondary getters retain source ordering;
an eager cache refresh would change behavior. A reverse lifecycle transition
retains its dirty secondary local/world distinction until the original getter.
The final displayed roots remain the prephysics graphics result. The renderer
obtains palettes on a disposable graph copy, so its observation cannot alter the
next native frame's cache state.

The exact phase/admission contract and source addresses are retained in
`evidence/frameBridge49W/frame/FRAME-CONTRACT.md`. The independent review includes
stale-cache, shared-state, source-order and unknown-W sensitivity checks.

## Authored inputs and advancing controls

The connected fixture uses actual Pod01 PHY parameters, fresh PSG/VUT geometry,
MOT animation, IK, joint setup, original preset4 and the original podboxf.col.
Track classA, configuration-present, mode58=false and absent profile overrides
are explicit selection inputs. Previously unproven velocityDamping130,
limits/axes/probe context and the initial numerical state remain supplied.
This is not a recovered live profile or race grid.

Initial speed is120 at the retained TA inspection anchor. Initial secondary
placement uses the actual authored joint target/rest relation. Placement and
body initialization occur once. Every later pose, orientation, history and mode
comes from the preceding native frame. Delivered packets and action-query
results are explicit inputs; no hardware/device-transport claim is made.

Two144-frame cases share initial state, packets and the repeated16/17/17ms
original prepared-factor cadence. The first32 frames use packet(128,80,128,48),
next16 use(136,128,120,128), and the remaining96 use(128,255,128,255).

The synthetic-support case adds an explicitly generated upward force at the
allowed354 boundary, balancing the selected authored gravity for the main body
and an active secondary body. This is a diagnostic load, not recovered ground
contact. No body pose, velocity or history is overwritten by that support.
The source lifecycle changes physical→virtual at22, reverses into blend at68,
and expires to physical at129. Equality at blend elapsed1.0 still retains blend;
1.016 expires it. Cached speed is visibly one graphics frame behind the current
postphysics velocity, as in the original source order.

The no-support case supplies no supporting force and retains no-hit world
results. The body falls626.2 authored Y units and remains virtual after its
frame12 transition. This control is retained alongside the supported visual so
the effect of injected support cannot be mistaken for original collision.

Both cases contain240 substeps and144 changing positions. Supported orientation
changes on116 frames; no-support orientation changes on143. There are403 and260
full original graph-body comparisons respectively, with a full original shape
comparison on every substep. Each case independently reconstructs828,720 posed
vertices while preserving exact UV/color/index/material order.

## Verification and use

Focused proofs retain551 complete original body-wrapper returns plus384 point/
axis leaves,364 complete shape cases plus30 independent EE boundary controls,
and81 staged frames with132 substeps. The staged proof includes132 complete
late shape-state comparisons. There are24 body,24 shape and13 frame semantic
mutants; each fails its corresponding proof. Native hot calls allocate zero.
Normal, plain and ASan+UBSan connected runs produce identical traces, goldens,
reports and ten selected meshes. LeakSanitizer is unavailable in this traced
host and is explicitly disabled.

Run the portable aggregate with `python3 -B tools/test_native_vehicle_frame49w.py`
and a fresh output directory; add `--sanitizers --no-leak-check` for this host.
Production translation units compile as C++17; original-word test harnesses use
C++20. `RR_NATIVE_VEHICLE_FRAME_ONLY` provides corresponding CMake wiring for a
host with CMake. Final aggregate, rendering, host budget and release-validation
reports are retained under `evidence/frameBridge49W/`.

The final aggregate passes69 suites and201,470,517 checks in each normal and
ASan+UBSan run, compiling62 production units against1,596 unchanged tracked
inputs. All64 inherited suite outputs are byte-identical to49V. Four additional
aggregate captures reproduce both connected scenarios exactly;400 process logs
per mode are retained.

The views use the retained TA camera, actual geometry and supported first-pass
cached textures. Four paired samples retain both scenarios. The no-support
vehicle falls below the track and disappears from the fixed view after the first
selected sample; supported
late samples are small because the camera stays fixed. A frame31 display control
replaces only the Chariot root with the current postphysics secondary root; a
second freezes the three displayed component roots while retaining internal
motion. These are labeled counterfactual display controls on disposable copies. They do not implement original visibility, material programs,
lighting or the VU vertex program. Presentation cadence and host timing are
separate diagnostics, not gameplay FPS or original-Xbox performance.

## Recorded footprint and host timing

The native replay's disjoint caller payload is122,904 bytes on this host ABI:
12,248 bytes of canonical state,73,388 bytes of reusable transactional workspace,
and37,268 bytes of immutable descriptors/MOT views and bytes, timing and
diagnostic records. This excludes stack, executable code, cold load buffers,
world/render assets and process/driver overhead. The three new runtime49W
objects contribute28,291 bytes of executable text and524 bytes of read-only
constants, with no mutable static payload. The allocation-bearing cold shape
decoder is itemized separately. The maximum individual compiler stack frame is
4,496 bytes; partial nested examples reach8,344 bytes, not a measured total
stack bound.

Both144-frame histories passed exact preflight against all serialized initial,
frame-entry, prepared-graphics/cache and completed-frame trace fields. Full-state ABI
images additionally check native consistency for fields outside that JSON
projection; those fields are not claimed independently serialized source proof.
The ABI images are pinned host fixtures, not a portable storage format.

On the pinned single Xeon8573C CPU, fifteen samples per case measured median
93.108 microseconds per staged frame with synthetic support and86.701 without;
p95 was111.436 and97.665 microseconds. Complete histories restart outside timing.
Native prepared-factor, input/graphics/body work and explicit supplied cuts are
included. Cold parsing, source interpreter, trace checking, palette/vertex
projection and drawing are excluded. These are subsystem host measurements,
not gameplay frame rates, console predictions or a before/after speedup.

The two-pass visual check submits360 draws and73,409 triangles with7,604,084
bytes of requested geometry/texture payload per image. Its maximum observed
process RSS was131,772,416 bytes, separately from the native-state accounting.
The paired loop's900ms sample display duration is presentation timing.
