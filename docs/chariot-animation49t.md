# 49T: original Chariot animation and hand IK

49T replaces the supplied static internal Chariot pose in the retained TA view
with the complete admitted original `21A000` animation call. Actual Pod01 brake,
Forward/Back and engine motions update the original node hierarchy. Both original
hand IK chains then solve against their authored target nodes. The same persistent
orientation state continues into the existing `49R` engine-root update before
all three model palettes deform their semantic local vertices.

This is a connected visual and numeric diagnostic. The Chariot root and main
body scene, current acceleration, aerodynamic inputs and visual histories are
explicit supplied inputs. `21A660`, authentic starting-grid placement, contact
production and complete gameplay remain open. The stationary root is not a
replacement implementation of the original Chariot root update.

## Original ordering and admitted state

`advanceVehicleAnimation49T` follows the original call through its real return:

1. Read or initialize the source animation-enable lazy state.
2. Advance left/right brake filters from aero `E4/E8`, retaining unclamped history
   and separately clamping the mirrors consumed by motions.
3. Apply Chariot left brake, then right brake.
4. Apply left engine motion, then right engine motion.
5. Under the original Chariot gates, transform body `340+C0` current **linear
   acceleration** by body scene `340+288` using the original inverse-direction
   arithmetic. Update the pose and blend filters, then sample Forward/Back.
6. Run the left target-gated IK, then the right target-gated IK.

The actual Pod01 authoring has no Mid motion records `288/294/298`. That is an
explicit admission; the absent Mid path is not silently replaced with identity
motion. Per-root byte `191`, Chariot byte `1B0`, optional body and target presence,
model word `18`, lazy state and error state retain their original effects.

The initializer projects original `2134A0`: left/right brake history is seeded
from `B04/B0C` with time `0.2`; pose history `B18` also starts from `B04`, and blend
history `B10` from `B0C`, with time `0.1`. Existing pose/blend mirrors are not used
as substitute initializer seeds. The input phase is an explicit caller attestation
that this is the animation phase, before subsequent physical integration.

## Authored motions and graph

The six retained MOT files are Chariot `01_brklt`, `01_brkrt`, `01_forwd`,
`01_back`, and both engine motions. Chariot has 18 authored nodes; each engine has
14. The connected forest has 52 nodes including supplied roots and body scenes.

The complete single and paired motion samplers preserve ordered rotation and
position setter calls, model word `18`, both sampler lazy words and scene
invalidations. Pair sampling uses the **first motion duration for both motions**.
Both position and rotation tracks run; there is no endpoint shortcut that would
skip source arithmetic or side effects. Cached worlds stay lazy until consumed.

Left hand nodes are upper-arm 10, forearm 12 and hand 13; the right chain is
11, 14 and 15. Targets 16/17 are the Chariot's authored `l_hand_dummy` and
`r_hand_dummy` under throttle nodes 7/8. They are not guessed engine grips.

## Persistent IK and source math

`initializeChariotIk49T` projects original `28EB90` for two fixed-axis chains.
It computes coefficients once from an explicitly supplied complete pre-motion
scene. The connected fixture supplies the authored scene with identity model
root at this initialization point, then applies the diagnostic root placement.
This choice is fixture state, not a claim about an authentic race constructor.
The retained axis inputs are left `(0,1,0,1)` and right `(-0,-1,-0,1)`.

Each runtime call follows `290A00`, its fixed-pole `28F580` solve, original target
orientation read, and `28A090` wrist orientation path. The saved elbow local,
length/rest coefficients, node identities and axis persist. Runtime solves do
not recompute rest values from already animated poses. The solver supports the
proven mode-1, debug-zero contract. Other solver modes and debug/profiling effects
remain outside the admission.

The original library acos behavior matters here: actual authored sample poses
produce arguments above +1. Under the verified mode-1/default-matherr contract,
the original wrapper returns **+0 and sets errno 33**. It does not clamp to +1 or
propagate its inner NaN. `sourceAcos49T` preserves widening, software arithmetic,
polynomial, integer square root, wrapper state and final narrowing using bounded
integer code. It introduces no host acos/libm, guest memory or interpreter in
production. Exponent-zero source inputs flush to signed zero; source NaN packing
and other errno preservation are explicit. A negative signed exponent shift was
replaced with equivalent portable floor division and reverified under C++17.

The orientation sentinel `4433D8` is the same caller-owned object used later by
49R. IK's two other lazy records and the motion sampler records are also explicit
caller state. Chariot animation/IK consumes no RNG. The subsequent 49R root
response retains its original noise order and shared RNG consumption.

## Visible result and controls

The original palette producer now visits all 46 selected model node entries.
All 5,755 Pod01 semantic vertices use their current node palettes: 2,884 Chariot,
1,411 left engine and 1,460 right engine. Vertex order, UVs, colors, indices,
texture associations, TA geometry and fixed camera are unchanged. Palette indices
come from the retained source/cache association. There is no invented weighted
skinning, inverse-bind multiplication or second transform of baked bind vertices.

The 96-frame diagnostic feeds recovered 48W controls and a declared acceleration
sequence into the complete internal animation, then 49R, all three palettes and
rigid display transforms. Source comparison includes the complete original
21A000 return, all 52 graph records, filters, models, lazy state and errno.
Engine mesh bytes at frames 0, 31, 63 and 95 exactly match the saved 49S output.

Two frame-95 counterfactual display controls isolate the new behavior. Freezing
Chariot local matrices at frame 0 changes 832 Chariot vertices. Restoring the six
actual pre-IK joint locals changes 149 Chariot vertices. Both controls change
zero engine vertices. The IK control executes full animation first and then
restores the captured pre-IK locals with explicit setters; it is labeled as a
counterfactual display, not an alternative recovered game mode.

Counts against the old placed 49H mesh include placement/bind rounding and are
not all attributed to animation. Images retain the same neutral diagnostic
material profile. They do not establish original VU vertex execution, lighting,
visibility, target GPU performance or gameplay frame rate. Four-sample animation
presentation uses a declared display cadence unrelated to game performance.

## Verification and reproduction

All new production code uses fixed caller storage, serial access and no hot heap
allocation. Public admissions validate finite data, graph/binding contracts,
mode and phase. Failed admitted operations preserve the projected outputs and
persistent state; distinct initialized live records are a caller precondition.
Ordinary semantic late-failure tests are separate from the held historical
ownership diagnostic.

Original source authentication pins the supplied ELF and every selected code/data
word. The original instruction oracles admit only initialized fixture reads,
file-backed original data and explicitly proved cold globals. No runtime RAM
was retrieved or reconstructed. Branch accounting distinguishes observed directions, proved exclusions and
remaining coverage limitations, including REGIMM, likely and COP1 branches.
The acos kernel is fully reached in the corpus, while 74 unexecuted helper words
and 37 missing conditional outcomes remain explicitly unproved coverage gaps.
They are not labeled unreachable; this is sampled source equivalence supported
by independent numeric checks, not an exhaustive proof over every input word.

The source-acos proof includes 1,250 actual IK arguments, boundary/random full
chains and independently generated elementary arithmetic vectors. The IK proof
includes 805 runtime cases and 13 constructors. The complete orchestration proof
includes 524 runtime cases, 16 initializations and 819 original IK calls. Focused
proofs compare ordered setters and use semantic original-word/native mutations;
the normal aggregate runs ordinary production without those trace hooks.

Read-only reproduction commands, from this directory:

```
python3 -B tools/test_native_source_acos49t.py
python3 -B tools/gameplay/model_motion_recover49t.py
python3 -B tools/gameplay/chariot_ik_recover49t.py
python3 -B tools/gameplay/vehicle_animation_recover49t.py
python3 -B tools/visual/run_chariot49t.py --output-dir /tmp/rr49t-reproduction
python3 -B tools/test_native_chariot_animation49t.py
python3 -B tools/test_native_chariot_animation49t.py --sanitizers --no-leak-check
python3 -B tools/visual/render_chariot49t.py --self-test
```

The direct compiler runner is verified on this cloud host. The focused CMake
entry is `RR_NATIVE_CHARIOT_ANIMATION_ONLY`; CMake is unavailable here, so no
CMake configure/build is claimed. ASan/UBSan are used; LeakSanitizer is explicitly
disabled because of the tracing environment. Host single-core cost and resident
payload measurements are recorded under `evidence/chariotAnimation49T/budget`.
They are not Xbox timing or full-game memory estimates.

The measured new 49T caller payload is 28,636 bytes, including its 28,152-byte
workspace. Four new production objects add 47,355 executable-text bytes and
10,796 read-only bytes, with zero data/BSS. The full connected measurement
harness has 349,423 bytes of caller payload plus 15,624 retained motion bytes;
that includes existing graph, authoring, visual, palette and all-vertex staging,
so it is not the incremental 49T cost. Compiler stack reports separately record
per-function estimates and a selected deepest 6,552-byte IK/graph chain; they
are not a measured whole-program stack maximum.

Fifteen retained CPU0 samples per variant measured medians of 39.16 microseconds
for full 49T animation, 65.10 microseconds including 49R and three palettes, and
235.73 microseconds including all 5,755 diagnostic rigid vertices. These are host
single-core samples with initialized authored fixtures, zero counted ordinary-new
allocations and no source interpreter in the timed executable. All samples,
including an animation outlier, are retained. They exclude rendering and are
not a game FPS estimate.

The displayed scene remains 360 draws, 73,409 triangles and 7,604,084 bytes of GL
geometry/texture payload. The Python/Mesa renderer's measured process peak was
134,307,840–135,790,592 bytes across the twelve render runs. That diagnostic host process
footprint is separate from native caller payload and from a target-console budget.

## Preservation and remaining dependencies

The source starts from the exact saved 49S archive. Only the inherited README
and top-level CMake selection gain milestone references; old production and
historical evidence remain unchanged. 49C is not included and remains unreleased.
The separate historical collision-capacity and ownership diagnostic operations
are not resumed here. Real root updates, physical/contact sequencing, race spawn,
complete rendering and a playable game loop remain the next integration work.
