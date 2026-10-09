# Opt-in render preparation diagnostics 50GQ

This change adds observation-only development-host instrumentation to
`tests/native/terrain_interactive_drive50du.cpp`, backed by
`tests/native/render_diagnostics50gq_fixture.h`. Enable it explicitly with
`RR_TRACE_RENDER=1`; every other value, including an absent variable, leaves it
disabled. The setting is read once during cold session setup. It is independent
of `RR_TRACE_PHASES=1`, although enabling both permits comparison against the
outer render and explicit completion measurements.

There are no floating-point mode writes, physics changes, native-clock changes,
scene/cache changes, additional GL completion calls, or scheduling changes.
The helper uses fixed caller-owned storage and introduces no heap allocation,
logging, worker, lock, or global mutable state. Client reporting is at the
existing five-second status threshold, outside native allocation guards. The
existing wrong-way50GO and first-flag capture50GR behavior is retained.

## What the existing evidence does and does not say

The parent investigation supplied a 3.72886 GPU-completed FPS interval lasting
5.09539 seconds. Its existing `render_prepare_submit` total was 2.5568 seconds,
with a maximum of 1.4097 seconds and 1.3886 seconds of calling-thread CPU on that
same maximum. `completion_wait` totaled 2.04999 seconds; its maximum of 0.714
seconds had only 0.000813 seconds of calling-thread CPU. Physics totaled 0.4461
seconds. These figures identify both substantial render-path main-thread work
and a separate completion wait. They do not identify denormals, the GPU, worker
threads, or VM scheduling as the cause. These figures were supplied by the
parent; this change does not claim to have rerun or independently reproduced
that capture.

Code inspection found distinct regions inside the old aggregate:

- `src/game/vehicle_draw50cm.cpp` snapshots canonical scene nodes, projects a
  current palette, validates/projects rigid vertices, and copies output vertices.
- `src/game/model_palette49s.cpp` refreshes lazy worlds in staging. The 50CM
  caller supplies its copied nodes, not canonical vehicle nodes.
- `src/game/rigid_vertices49s.cpp` validates before committing with two ordered
  binary64 projection passes, including binary32 matrix promotions and outputs.
- `src/game/pod_cables50fd.cpp` copies scene nodes and performs strict recovered
  binary32 cable arithmetic.
- `platform/linux/native/vehicle_window_gl50ds.cpp::submit` includes validation,
  host HUD geometry, track visibility, buffer/draw work, and possible window copy.
  Submission is not guaranteed to be queue-only or nonblocking.
- `finishSubmittedWork` calls `gl.finish()` separately on the headless path.

The palette, projection, cable, and culling functions have explicit arithmetic
contracts. A blanket render FTZ/DAZ switch is deliberately absent. Even finding
subnormal source words would not by itself justify changing those contracts.

## Instrumentation boundaries

One `render_subphase` line is emitted for each of these nonoverlapping scopes:

| Label | Included work |
| --- | --- |
| `census` | Read-only scene observation before this render preparation |
| `draw_bank` | CPU bank choice, descriptor initialization, `batchInput` |
| `vehicle_projection` | Batch setup and `buildVehicleDrawBatch50CO` |
| `host_camera` | Chase view, pod framing, view matrix, cable-facing vector |
| `source_cables` | Existing guarded `buildPodCables50FD` call |
| `host_hud` | HUD values, lap/wrong-way/route cue, clear color |
| `driver_submit` | Existing `renderer.submit` call |

The outer 50GK render scope intentionally encloses these scopes. Do not add
its duration to the subphase sum. Capture, explicit completion, publishing,
window swap, and status output remain outside the subphase scopes. Scope
construction/closing and inter-scope bookkeeping account for a small residual.
The census has its own measurement so its cost is not attributed to projection.

Each record retains the 50GK `Totals` semantics: call/valid-pair counts, wall and
calling-thread CPU sums, maximum wall, CPU from that exact maximum-wall call,
CPU validity, clock failure, and saturating-counter overflow. The helper reuses
the existing 50GK clocks and total accumulator. Non-Linux CPU measurements are
explicitly unavailable. Missing CPU is not zero CPU. Maximum ties retain the
first observation. The scopes do not measure process-wide or GPU CPU time.

## Read-only subnormal census

The observation scans both 16-word matrices of all 53 canonical scene nodes:
1,696 binary32 words per rendered frame. Each float object is copied with
`memcpy` into an integer and classified using exponent/fraction masks. There
is no floating-point comparison, conversion, arithmetic, or `fpclassify` in the
census. Both signs of subnormal are counted; signed zero is excluded. NaN and
infinity words have a separate nonfinite count. Unrelated object bytes and
padding are never scanned.

The counters distinguish local, ready-world, and dirty-world matrices. Dirty
worlds can be stale caches. All 16 lanes are scanned, including fourth/opaque
lanes, so a counted word may never be used by the slow arithmetic. No body,
contact, smoothing, or animation state is altered or comprehensively scanned.
Transient arithmetic subnormals and subnormals solely in other fields are not
covered; a zero count cannot rule those out.

For every subphase maximum, `sceneAtMaxWall` preserves the same render's
preparation-start census and simulation tick. This is a correlation aid, not a
sample collected at the slowest instruction. `first_source`, zero-based
`first_node`, `first_lane`, and unsigned decimal `first_bits_u32` locate the
first subnormal in deterministic node/local/world/lane order. The location is
available only when `first_source` is not `none`. `scene_at_max_valid=0` means
there is no associated valid census, not an all-normal scene.

On an SSE-capable x86 build the census reads MXCSR without changing it.
`mxcsr_u32` is unsigned decimal; DAZ is bit 6 and FTZ is bit 15. The value is
sampled at preparation start, not separately during each subphase. When
`mxcsr_available=0`, the value is unavailable. x87 control and other threads'
FP environments are not observed.

`render_census` summarizes sampled render frames, frames with any subnormal or
nonfinite word, maximum subnormal count, and the latest scene's dimensions and
counts. These are render observations, not unique simulation ticks. The same
tick can be rendered again for resize/capture. All interval data resets after
reporting. Disabled mode performs no clocks, census reads, or diagnostic output.

## Public helper API and verification status

The namespace is `render_diagnostics50GQ`. The core API is
`Interval{enabled}`, `Scope(interval, Subphase::...)`, idempotent
`Scope::finish()`, `Interval::observeScene(state, tick)`, and
`Interval::resetInterval()`. `subphaseName` and `matrixSourceName` return static
labels. Keep observation/report/reset on one thread; never reset with an active
scope. Preserve `latestScene` through each scope; the census scope intentionally
updates it before closing. The `classifyBinary32Bits` and `censusSceneNodes`
functions provide deterministic supplied-input seams for future focused checks.

This implementation was source-reviewed only. No compilation, test, gameplay
run, PC validation, guest execution, excluded 49C/48U work, or `ee.bin` access
was performed by this worker. Root owns the proposed single bounded reproduction.
The immediate question is which subphase dominates slow frames, and whether its
same-frame source census differs from fast frames. It is premature to claim a
performance improvement or a denormal root cause.
