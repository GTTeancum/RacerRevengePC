# 49M: secondary-body forces and ordered two-body advancement

## Delivered boundary

This milestone begins from the exact verified 49L archive (SHA256
`080b1eceaf1072a258140f046fdb510275ab20e391b66668932862296752d479`).
Its production code implements the numerical secondary wrapper around the
one-way PodSphereConstraint, and a staged, persistent two-body route through
existing input, timing, engine, aero and root-body integration components.

The new source is fixed-size, serial C++ with no production instruction interpreter,
world query, heap allocation, platform input transport or random seed policy.
Existing 49K and 49L APIs and historical evidence are retained unchanged.
The separately held 49C work remains excluded and unreleased. Collision-capacity,
ownership and reset work remains outside this milestone.

This is a supplied-state diagnostic. It requires initialized bodies, ready root
scenes, inertia/mass, original joint setup and effective globals. It also requires
explicit completed external stages. It does not establish a playable race,
authentic spawn, full contact processing or target-console performance.

## Original order and persistent state

The original main-body routine reaches these stages in this order:

1. Once-per-frame input shaping/binding and child-phase body cache refresh.
2. Original substep selection; first/required world preparation supplied explicitly.
3. Main engine update, gravity, gated engine forces and aerodynamic forces.
4. Required main contact response at219CE8, supplied explicitly.
5. Disabled support/probe response at219CF4.
6. If the secondary body exists and live vehicle838 is nonzero,2188B0:
   secondary forces; constraint preparation2190AC, correction2190C0 and prepared
   force application2190D4; required secondary contact stage2190E8; secondary
   root-body integration2190FC.
7. Main impact gate; admitted main root-body integration219E80 and separate
   main20C clear219E94.
8. Required post-body query219120, admitted only with explicitly supplied no-hit
   result; subsequent vehicle380 and next-producer feedback.

The secondary integrates before the main body. The main endpoint's current pose,
velocity and accumulated force contribute to the joint before main integration.
The joint writes loads only to the secondary. External contact stages may change
represented main and secondary state; those supplied effects are distinct from
an invented joint reaction.

Both bodies' cached speed34 and anglesE0 refresh once in the frame child phase,
not after every substep. Root-body integration refreshes its separate local80 and
world-angularA0 fields. The secondary has no separate20C clear following its
integration; the root-body callback still clears238. Both root-body updates copy
the initialized433B80 vector(0,0,0,1) into force/torque accumulators, including W.

## Secondary wrapper forces

The gravity selector reads the current support field104 after the main response,
independently of the earlier main gravity selection. Its gravity force is added
using the secondary body's current mass. Angular damping uses secondary cached
speed34 and the source interpolation/clamp order, including reversed endpoints.
Scene-relative horizontal restoration is transformed by the main scene basis,
with the original scalar root and VU normalization semantics.

For vehicle state40 zero, the wrapper adds secondary world-velocity damping and
up-axis alignment torque. For nonzero state40, it evaluates the original 1-D
gradient noise, adds speed-conditioned torque and rotated force, advances864 in
source order, and uses the separate damping coefficient. The coupled route keeps
49K's exclusions for state3/4 and other unsupported scheduler/impact contexts.

Global coefficients are explicit effective inputs. A separate helper exposes the
original lazy-initializer values65,350,1500,300,4000,50,120,0,5. Using this helper
is a declared initialized-global route; it does not implement all game-global
writers or assume later runtime changes cannot occur.

## Projected original noise and random state

The noise implementation retains the original 1-D permutation and gradient tables,
lazy state and last signed cell, plus a caller-supplied shared raw32 random state.
It does not select an initial seed. A cold evaluation consumes the original1791
random transitions: six per each of256 entries, followed by255 shuffle draws.
A ready evaluation consumes none.

The two- and three-dimensional gradients are outside this projected component.
The proof separately verifies their discarded normalization cannot affect the
retained permutation,1-D gradients, random sequence or subsequent control flow.
This is not a claim to initialize every shared game-noise table.

Coordinates follow rounded binary32(4096+x), signed truncation toward zero,
low-byte cell selection and original cubic interpolation. Negative shifted values
retain negative fractions rather than being changed to floor/fract semantics.
The finite signed-conversion domain is explicit. Invalid native inputs fail
transactionally instead of invoking unchecked source behavior.

## Data and external-effect contract

Canonical root-body state is projected into full-lane49L body views at the exact
constraint stage. Supplemental source W lanes and the inverse-inertia matrix's
extra lanes remain explicit; they are not silently zeroed or taken from stale
independently mutable body copies. Setup-cached joint mass/inertia stay distinct
from current body parameters.

The dedicated49M API keeps secondBodyPresent true and does not evade49K's public
rejection by spoofing absence. Common unchanged phase bodies are separately
checked against their retained49K source. A live838 zero route skips the whole
secondary wrapper without removing the body or inventing a force-free wrapper.

The external records expose represented body/component/joint effects at precise
boundaries. They do not establish unrepresented global, hierarchy, parameter,
registration, callback or scheduler effects. Callers must satisfy the documented
admitted context. Synthetic tests label their nonzero supplied contact effects and
no-hit query assumptions explicitly.

## Verification and resource measurements

Final verification: all26 focused suites pass normally and with ASan/UBSan,
27,451,557 checks per build,159 tracked input files unchanged. Per-test stdout,
new original-word coverage and the persistent trace are byte-identical between
builds. The trace has120 supplied frames,270 substeps,250 secondary updates and
20 live838 skips. The original49K rejection of second bodies remains unchanged.

The wrapper proof exercises all540 instructions of2188B0..219120;2,144 of2,377
included wrapper/helper words are visited, with all233 remaining words explicitly
accounted for by the admitted source domains. Cold global-initializer instructions
are included in that source exercise, while production exposes effective global
values and an explicit initialized-value helper. Noise adds24,514 cases and
13,771,587 checks across four host rounding modes. Its projected proof includes
187 executed words,78 excluded words and12 unreachable negative-RNG words; a
639-word helper audit establishes the normalization exclusion. Production wrapper
and composition require FE_TONEAREST. Safe mutation controls catch13 noise,
14 wrapper and13 composition changes.

The new per-vehicle state is1,580 bytes; shared noise/RNG is2,060 bytes, reused in
original serial vehicle order. Parameters occupy448 bytes. The three new strict
C++17 host objects total19,870 text bytes with zero data/BSS. The largest
individual compiler-reported bounded frame is4,624 bytes; nested calls add to
live stack usage. These figures are not whole-game resource measurements.

A native-only synthetic benchmark runs prefix→49L→identity supplied368→secondary48H
on one CPU under a64MiB address-space cap, with zero timed allocations. Its
ready-noise table is initialized before timing and fixed caller-owned records are
copied inside each iteration. Host timings and process-RSS observations are in
`benchmark/verification.json`; they are not gameplay or console benchmarks.

Detailed results and measurements are recorded with the checkpoint and under
`evidence/secondaryWrapper49M/`. The installed compiler is used directly;
CMake wiring is provided but CMake is unavailable in this environment. ASan/UBSan
runs disable LeakSanitizer because the executor's ptrace environment does not
support it. These checks do not establish whole-game memory usage or gameplay FPS.

## Remaining integration

Original model/PHY-to-runtime initialization, live mode selection, contact/query
processing, unavailable reset effects and complete scene consumers remain open.
The three visual Pod01 parts do not imply three rigid bodies. No new mesh placement
or camera/render binding is inferred from this numeric two-body route.
