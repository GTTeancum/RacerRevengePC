# Racer Revenge performance handoff

Updated: 2026-10-17 12:28 a.m. EDT (2026-10-17 04:28 UTC).

## Current direction

Continue performance-focused work until the user says to stop. Publish reviewed
source checkpoints to `GTTeancum/RacerRevengePC` main every 15 minutes and update
this handoff with each publication. The outgoing session is focused entirely on code and is publishing changes
without execution-based validation. This restriction applies only to the outgoing
session, not to the next owner. New source changes are **UNTESTED**. Do not infer
an FPS improvement from fewer API calls. The previous Friday-evening cutoff has
been superseded.

Keep integration centrally owned. The outgoing session uses source-only helpers,
Medium maximum; the next owner should establish its validation plan with the user.
Use the VM only, not the user's PC. Preserve remote history, never force-push,
and independently verify the resulting remote commit, tree and parent. Git
publication is explicitly authorized, including untested source checkpoints.
Continue independent coding if publication is blocked, and report exact blockers.

## Current checkpoint summary

Latest independently verified main before this checkpoint is
[81d27c3](https://github.com/GTTeancum/RacerRevengePC/commit/81d27c302282f600c3ba95e5f8b659c4cca2b84c).
The user explicitly requested another one-off FPS test. Exact source
`b4c303ec54d64de206b188aa06790518c2161baa`, tree
`9afdc22f9906e86d4283607fd92000107dd7ce06`, built successfully at
2026-10-17 04:23:32 UTC. The Linux smoke ended at 04:27:02 UTC with exit0,
no timeout and confirmed cleanup. Root reviewed menu, Mawhonic character select,
Ryloth course, track and original Pause screenshots; Pause → Continue resumed.
Eight original AI racers, zero human racers, host-driver calls or driving input.

Measured **0.795234613517296 FPS** on llvmpipe: four GPU-completed gameplay swaps
in 5.029962142 seconds, native ticks28–44. Previous 4f82775 measured
0.8035408549754633 FPS over five swaps/ticks28–48. Current sample is about1.03%
lower; unequal short windows and ordinary variability prevent attributing that
difference. No performance improvement is demonstrated.

This checkpoint retains that exact game code and adds only result/handoff
information afterward. Earlier UNTESTED labels record the source-review stage;
this limited smoke now covers changes through the pass-constant HUD setup.
Any subsequent source changes require their own validation. Routine testing
restrictions remain specific to the outgoing session; the successor may validate.
See [full result](recovery/performance-one-off-b4c303e-20261017.json).

### Historical smoother Windows results

[MA intake history](recovery/ma-intake-20261014.md) records Ody/Mon Calamari source
c06a1b0364f81137315b20a07af2f73244eb524b at **36.6928 FPS** on October14:
184 completed swaps/5.0146019 seconds, 640x448, all8 original AI, warmup excluded.
Other documented Aldar/Mon Calamari Windows samples were about30.75–31.24 FPS.
These are historical different-course/racer results; hardware identity was not
recorded there. They do not measure current Windows Ryloth performance. The
recent Linux software-rendered Ryloth samples cannot establish that comparison.

## Checkpoint and evidence

The commit containing this document is the current handoff checkpoint; use its
Git history to identify the exact revision. A document cannot contain its own
final commit hash. Earlier independently verified **tested** publication:
[e53d3220](https://github.com/GTTeancum/RacerRevengePC/commit/e53d3220ddcd721f67aeda7a84a80b2504c0e5e7).
Its exact source tree is `7cdb5a7a64d53a3897c978177dd382545aa6bcfc` (local tested
source `75fbe2e0c61ca5b0041b877c2450edce731008c6`).

The earlier Linux build passed at 2026-10-16 22:40:30 UTC. Its smoke ended at
22:44:22 UTC: exit 0, cleanup true, five real gameplay swaps, native ticks
28–48, eight original AI racers, zero human racers/host-driver calls/keyboard
packets. Main menu, rider selection, course selection, track and original Pause
screenshots were reviewed. Escape → Pause → Continue resumed gameplay.
Historical baseline: **0.851877902726142 FPS** on the Linux llvmpipe VM.
That earlier result does not validate changes after its tree. See the newer
one-off validation below. No GitHub CI pass was
established. No fresh Windows test was performed.

## One-off Linux validation, 2026-10-17 01:40 UTC

The user explicitly requested one FPS test. Exact local source
`61b289e3bc34bbab7a505553be5a937a2e97854c`, tree
`9683a493b8cbedd70258385b4c0678592a578735`, built successfully at 01:37:20 UTC.
The Mawhonic/Ryloth Linux llvmpipe smoke ended at 01:40:48 UTC: exit0, no timeout,
cleanup confirmed. Root inspected main/rider/course/track/Pause screenshots;
muted spectator Pause → Continue resumed. Eight original AI racers, zero human
racers, host-driver calls or driving keyboard packets.

Measured **0.8289711243839988 FPS**, five GPU-completed gameplay swaps over
6.031573179 seconds, native ticks28–48. Prior75fbe2e measured0.851877902726142 FPS
on the same tick window: about2.7% lower in this single sample. No performance
improvement is demonstrated; this is not a controlled benchmark or Windows result.
The source-only changes described below through scratch/operand initialization
are covered by this limited smoke. Any later source changes remain UNTESTED.
This one-off authorization does not establish a recurring test schedule.
See [one-off result](recovery/performance-one-off-61b289e-20261017.json).

## Latest one-off validation, 2026-10-17 03:17 UTC

The user requested another test. The initial build was interrupted by full VM
replacement. Original source tree `ed51765d87b357345095efd8f5c7494c3555e62e`
(local source `4f82775f79ce205511919e5ad7bee60f67fbf4a3`) was recovered exactly
from verified GitHub objects and a source backup, including raw line endings.
Original disc/runtime inputs were recovered; all144 rider textures and48 sky
images matched their committed expected hashes. SLUS was read only as data.

Rebuild completed successfully at03:10:36 UTC. The Linux smoke ran03:13:40–03:17:04
UTC, exit0, no timeout, cleanup confirmed. Root reviewed main/rider/course/track
and Pause images; Continue resumed. All8 original AI, zero human/host-driver/
driving-keyboard counts. **0.8035408549754633 FPS**, five completed gameplay swaps,
native ticks28–48. Previous61b289e measured0.8289711243839988 FPS; this sample is about3.1% lower. VM replacement and single-sample scope prevent a controlled
comparison; no speedup is demonstrated. Renderer: llvmpipe LLVM19.1.7, GL4.5 Core,
Mesa25.0.7. The optional copy procedure branch was not independently traced.

This limited smoke covers the code through both optional image-copy paths below,
including earlier post61b289e changes. Later source must remain UNTESTED until
validated. No fresh Windows test. The one-off does not authorize routine tests.
See [exact result](recovery/performance-one-off-4f82775-20261017.json).

## Performance findings

- Rendering submission dominates: roughly 1.1 s/frame; opaque opponents roughly
  0.68–0.79 s/frame, world roughly 0.084 s, HUD blur roughly 0.03 s. Stage samples
  are not all aligned with the FPS window, and named stages omit some player work.
- Roughly 5,000 surviving original map-2 bridge runs per frame each perform AA0
  payload raster, four-sample destination snapshot, then sample-specific
  composition. Region scissoring already bounds work.
- Physics is roughly 4.84 ms/native tick; four ticks per swap. It is not the
  leading observed cost. Do not replace original AI or alter simulation cadence.
- Fixed composition sampler uniforms were moved to program-link initialization
  in native_coverage_storage.inl and ra_static_world_gl_storage.inl. Retained.
- A broader per-group restoration experiment passed a historical basic smoke
  but was reverted: 0.824 FPS versus prior 0.871 FPS, then sampler-only 0.852 FPS.
  Single samples do not prove a causal regression or a speedup.
- Historical 7.327 FPS used an incorrect transposed CPU clipping matrix. The
  corrected GLSL-compatible matrix yielded ~0.805 FPS. Windows, workload and
  tick-window differences prohibit treating those results as a clean benchmark.
  Never restore incorrect geometry rejection to obtain higher FPS.

## Changes since the earlier baseline

Integrated source-reviewed group-local invariants in
`platform/linux/native/native_coverage_storage.inl`: redundant sampler-object
bindings/composition mode and fixed coverage/viewport/color-mask/blend state.
The change retains existing per-run error gates, draw order, destination snapshots,
texture feedback unbinds and failure-path restoration. A local firstRun flag is
reset per original MSCNT group; it advances only after a successful nonempty
bridge. Generic callback/span state binding remains unchanged. See
[the source rationale](recovery/coverage-group-invariant-state-20261017.md).
These changes have not been built or tested, and no speedup is claimed.

Also integrated a fixed 32-entry actual-index cache for vertex clip operands,
scoped to one synchronous racer MSCNT group. It is reset at entry and disabled
on success or failure; static-world clipping remains uncached. Dot-product and
rectangle arithmetic are unchanged. Cache collisions recompute. Lookup/copy
cost versus saved arithmetic is unmeasured. See
[operand cache rationale](recovery/racer-raster-operand-cache-20261017.md).

Earlier documentation-only handoff publication: `847798661571b1852e3f6c2c5b01ec7b8e09f4a5`,
verified tree `818ee732f9580900201d0e75d02742a9484d3710`, parent `e53d3220`.

The RA world draw path also reuses six unchanged material/camera uniforms
after the first checked nonempty primitive of a selected draw. Program binding
still occurs on every primitive, and AA0 composition mode remains explicit.
Source-reviewed only; see [RA uniform rationale](recovery/ra-material-uniforms-20261017.md).

RA clip-plane rejection also reuses each identical matrix-position product
within a vertex. Original accumulation order, thresholds and nonfinite fallback
are preserved. Source-level products fall from 48 to 16 per fully reached vertex;
compiler optimization and lookup costs make runtime impact uncertain. See
[clip-product rationale](recovery/ra-clip-product-reuse-20261017.md).

RA selected-instance placement lookup now uses binary search in the cold-built,
strictly ordered retained placement vector. Exact identity/LOD checks remain;
no allocation or additional retained pointer is introduced. See
[placement lookup proof](recovery/ra-placement-lookup-20261017.md).

Selected-light submission now borrows its already-validated local value snapshots
through synchronous drawing, instead of making a second Storage copy. A scope
guard clears all pointers before the snapshots die, including failure exits.
See [snapshot lifetime proof](recovery/racer-light-snapshot-borrow-20261017.md).

RA world position/UV lanes are now copied once at renderer creation from the
immutable retained geometry. Per-frame dynamic color/fog and all selected draw
checks remain. The later split-stream change below also reduces GPU uploads. See
[immutable lane rationale](recovery/ra-immutable-vertex-lanes-20261017.md).

Repeated racer normal-length scans now reuse a completed validation receipt
only inside callback-free CPU preflight. Batch/component pointers and counts
must match; all other mesh/material checks remain. The later ready() scan still
runs after external currentness callbacks. Receipts expire before drawing. See
[normal validation scope](recovery/racer-normal-validation-receipt-20261017.md).

The static diffuse preparation path now validates the complete material/publication
set once per selected lit surface, then privately borrows its current color stream
inside one synchronous callback. Previously that complete scan and surface search
ran for every vertex. Before/after currentness and exact camera identity remain;
raw-position preparation is scoped separately as described below. No cross-frame cache or public unchecked API
was added. This is a potentially larger CPU-work reduction, still unmeasured.
See [diffuse surface lifetime proof](recovery/ra-diffuse-surface-borrow-20261017.md).

Fogged surfaces similarly resolve raw source geometry and validate camera/world
currentness once around synchronous vertex preparation, instead of repeating deep
owner lookup per vertex. Exact quantization recovery, roundtrip/ambiguity checks,
output commit and radial fog arithmetic remain per vertex. The public observation
API is unchanged; no-fog surfaces do not acquire this scope. See
[raw-position scope proof](recovery/ra-raw-position-surface-scope-20261017.md).

Racer component diffuse/specular-presence flags are now derived alongside cold
material policy copies, avoiding repeated whole-bank OR scans during preflight.
Live source masks, spans and provenance checks remain. See
[cold material summary proof](recovery/racer-cold-material-summary-20261017.md).

Inside the private diffuse validation scan, adjacent identical raster IDs now
reuse only the preceding successful material check. Every changed ID is checked
in original order, with no persistent cache. See
[adjacent raster validation proof](recovery/ra-diffuse-raster-runs-20261017.md).

After complete diffuse publication validation, the selected row lookup uses
binary search over the strictly source-ordered generated rows, with an exact
equality check. See [diffuse row ordering proof](recovery/ra-diffuse-row-lookup-20261017.md).

## Recovered code checkpoint

The coding VM became unavailable around 2026-10-17 00:40 UTC. This checkpoint
recovers reviewed source changes from retained exact diffs applied to the verified
27e14fc68f4fc9741c658a3b073728fc3fae49d7 GitHub files. Reconstructed source blob IDs
are checked against the recorded diff identities and contents are read back.
At recovery publication time the complete unavailable local tree was not claimed
byte-for-byte identical. VM access returned at 00:49 UTC: all three source files,
five proof notes and README matched their recovered Git blob hashes exactly.
After accepting the updated recovery handoff, local checkpoint 794f104 has the
exact published b90a8f9 tree d28d1afe877a2786333480d07b795fdfd22a4000.
No builds, tests or runtime measurements were performed during recovery.

Additional untested source changes:
- Separate immutable position/UV and dynamic color/fog GPU streams. All float
  values, shader inputs and indices are retained. Upload size changes from 40
  bytes to 20 bytes per retained vertex before selection bounding. The extra
  buffer has checked allocation/cleanup; fog attestation uses its actual layout.
  See [stream split](recovery/ra-split-vertex-streams-20261017.md).
- Upload only one contiguous dynamic interval covering all selected surface
  vertex ranges, or no interval when selection is empty. The full GPU buffer is
  initialized at creation. Unselected values can differ from abandoned CPU
  staging, and diagnostics continue reporting that honestly.
  See [bounded upload](recovery/ra-bounded-operand-upload-20261017.md).
- Retain the first compressed-normal recovery within the same lighting
  publication and reuse it after exact scene/surface/count checks. All dynamic
  lighting and RGBA projection remain current; there is no cross-publication
  normal cache. See [normal reuse](recovery/ra-normal-publication-reuse-20261017.md).
- Reserve lighting metadata from the actual lit-surface count and recovered
  normals from a new bounded, exact lit-vertex count in the cold draw plan.
  Actual recovered count is checked. Earlier allocation failure remains an
  unpublished failure. See [metadata reserve](recovery/ra-light-metadata-reserve-20261017.md)
  and [exact normal reserve](recovery/ra-exact-normal-reserve-20261017.md).

These are source-level changes and byte/work reductions, not measured FPS gains.
The speculative opponent-light uniform receipt was not independently reviewed
or integrated and is not part of this recovery.

## Post-recovery shader change

Two destination-input clamp expressions are removed from the coverage and RA
fragment shaders. Their inputs are exclusively the private, queried RGBA8
unsigned-normalized snapshot texture, already finite in [0,1]. Original
fetch *255.0 +0.5 and floor order is retained. Floating payload and final result
clamps remain. Ownership, initialization, extent/sample bounds and official GL
range guarantees were independently source-reviewed. This may already be
optimized by the driver; no measured gain is claimed. See
[destination clamp proof](recovery/destination-rgba8-clamp-20261017.md).

The racer coverage composition reuses texture unit0 established by its checked
payload setup. Later nonempty runs within the same synchronous group also reuse
unit0/image established by the previous checked successful tail. First runs remain
explicit. All composition binds, error gates and unconditional tail transitions
remain; no persistent cache is added. These changes remove one composition-entry
call and two later-run payload-entry calls, without a measured FPS claim.
See [selector scope proof](recovery/coverage-payload-texture-selector-20261017.md).

The coverage bridge additionally reuses checked depth/framebuffer state within
one invocation: composition does not repeat payload depthMask(0), and successful
tails omit the already-established scene framebuffer/depth tuple. Rejected paths
retain the original restoration calls at their original positions. Every error
gate and required texture/program/VAO/sample/scissor transition remains. This is
not the reverted broad group restoration change. Later nonempty runs also omit
MULTISAMPLE-disable and depthMask(0) repeats, inheriting those values from the
previous checked successful tail; first runs remain explicit. Later runs also
reuse the source program restored by that tail, keeping the mode3 uniform write
and every tail program bind unconditional. See
[checked depth-state proof](recovery/coverage-checked-depth-state-20261017.md).

The group lookahead rectangle scratch array now initializes only entries it uses.
The 14-row local array is not zero-filled; row0 and each appended row are fully
assigned before the only reads of prefix [0,run). Unused rows never escape.
All geometry arithmetic, bounds and thresholds remain unchanged. The compiler
may already remove these dead stores; no speedup is established. See
[scratch-prefix proof](recovery/coverage-region-scratch-prefix-20261017.md).

Native raster operand local arrays also avoid duplicate member-zero initialization.
The producer still explicitly resets every member, cache storage stays value-
initialized, and consumers require three fully produced or copied finite records.
Failure paths bypass partial arrays. No clipping arithmetic changes. See
[operand initialization proof](recovery/raster-operand-initialization-20261017.md).

## Source changes after the first one-off test

The racer operand cache now lazily retains eight ordered projection quotients per
eligible indexed vertex within one synchronous group. Common-plane rejection and
W guards still precede lookup; hits preserve each finite check and ordered min/max
operation. Slots are rechecked by exact index after possible collisions, readiness
is reset on operand replacement and set only after all eight finite quotients.
Generic RA calls remain uncached. Cost includes roughly2KiB fixed storage, reset
stores and lookup/miss overhead. No measured gain is claimed. This change was
NOT included in the one-off61b289e smoke. See
[quotient-cache proof](recovery/racer-projection-quotient-cache-20261017.md).

Racer normal-buffer uploads are now skipped only when a complete immutable cold
material table proves no material can enable host lighting. The summary uses the
same predicate as applyPodMaterial. Null tables remain conservative. Cold zero
initialization, VAOs, CPU normal validation/lighting/specular use, buffer binds and
error polls remain unchanged. The new code is UNTESTED and was not part of the
one-off smoke. See [normal-upload proof](recovery/racer-unused-normal-upload-20261017.md).

Coverage paired spans now omit redundant unit1-zero/active0 transitions between
checked groups; single-sample destination binding and span cleanup remain.
Immutable sample/sRGB/blend/viewport/color-mask/sampler/composition-mode setup is
also reused across groups within one synchronous span. A separate group-first
flag still establishes source program, image and mask after base→map2 transitions.
Empty groups never authorize reuse; every restoration and error gate remains.
The operand/quotient cache still resets per group, not per span. Both changes are
UNTESTED after the one-off baseline. See
[span texture proof](recovery/racer-coverage-span-texture-unit-reuse-20261017.md) and
[span setup proof](recovery/racer-coverage-span-invariant-setup-20261017.md).

Composition also reuses the LEQUAL comparison established by checked span entry;
the rejection-tail write remains. Deferred racer reuploads now omit vertex data
only for complete cold all-opaque components, while every potentially nonopaque
component receives its full current interleaved stream. Deferred normals are
output-independent and their transfers are omitted with a conservative null-table
fallback. All binds, checks, ordinary uploads and validation remain. Both changes
are UNTESTED. See [depth-function proof](recovery/coverage-depth-function-reuse-20261017.md)
and [deferred-upload proof](recovery/racer-deferred-upload-gating-20261017.md).

Generic fragment normal preparation is now guarded by draw-uniform useLighting.
Derivatives remain before the existing generic discard; lit arithmetic, varyings
and outputs remain unchanged. Original source-diffuse/pair/payload paths already
return earlier, so this does not address the dominant bridge workload. The driver
may already eliminate the unused work; no generated-instruction or FPS improvement
is established. Independent source/spec review is not a cross-compilation
bit-identical derivative guarantee. UNTESTED. See
[uniform normal guard proof](recovery/generic-normal-uniform-guard-20261017.md).

Optional named image copies now retain the same distinct four-sample RGBA8
snapshot for racer and RA composition. Admission requires checked desktop GL4.3+
and a nonnull optional procedure. Unsupported/missing-procedure contexts retain
the exact original framebuffer blit. Runtime copy errors reject through existing
checks/restoration without retry. Rectangle, sample storage, order and original
composition remain; no same-image feedback is introduced. This removes two racer
or four RA copy-setup calls per eligible snapshot, with no measured speedup claim.
See [racer copy proof](recovery/racer-coverage-copy-image-20261017.md) and
[RA copy proof](recovery/ra-world-copy-image-20261017.md).

## Avoid unsafe shortcuts

- Every racer, including followed player, must retain original in-game AI.
- Native host CPU/GPU code only; no PS2/VU emulation or guest instructions.
  SLUS is static recovery input and must never be executed.
- Preserve original geometry, assets, byte composition, coverage, depth and
  alpha semantics. Do not reduce quality or omit racers to improve FPS.
- Multisample same-texture feedback with texture barriers remains unproven;
  per-texel exception does not establish safe four-sample read/write feedback.
- Shared-edge triangle batching remains held after focused specification review.
  AA0 pixel ownership, not just destination-sample disjointness, is required.
  Current transform margins and SUBPIXEL_BITS do not establish complete portable
  post-quantization orientation bounds. See [held proof](recovery/shared-edge-batching-held-20261017.md).
- Existing disjoint conservative-rectangle grouping, VAO query hoist, private
  FBO selectors and sampler initialization already exist; do not duplicate them.

Previous verified publication: `86743c2f1102891590f461efb84c3ace690492c8`
(tree `fac7376491acc7bead51b50784c61f8442152caa`, parent `97139ec1`).
It contains both image-copy paths, exact tested source from the latest one-off,
and result documentation. The subsequent 9aa6a5b6 checkpoint added documentation
only. The pending selector change below modifies game code. Earlier UNTESTED labels describe their
source-review state; the limited latest smoke coverage is stated above. Changes after the latest tested4f82775 source remain untested. Source review does not establish compilation or
runtime correctness.

## Known functional limits

Original Pause → Continue works in the historical muted ALL8 spectator smoke.
Other Pause actions remain unavailable; this is not a complete menu or game.
Audible Pause remains gated on missing original audio receiver semantics.
Aggregate cleanup now releases internal camera observer owners before checking
for external runtime ownership, retaining external-borrow guards.
Wreck collection work is held and not active gameplay evidence.

## Pending investigation

A broader cross-publication cold normal cache remains held pending stronger
lifecycle proof. The opponent-component lighting uniform proposal is also held:
actual repeated eligible hostLit/nativeGc span incidence remains unproven.
Source-lit body/specular spans bypass those seven uniforms, so total opponent
draw count does not establish a useful saving. Keep this draft excluded.
VM access returned at 2026-10-17 01:14 UTC. Recovered source exactly matched the
local source blob; after documentation reconciliation the complete local tree
matched verified remote 71aa1b21. Unreviewed local drafts remain excluded.

## Next steps for the next owner

1. Read the latest commits and this handoff; separate tested baseline from all
   subsequent untested changes. Inspect current publication state before pushing.
2. Continue small source-reviewed reductions in coverage bridge submission cost;
   document ownership boundaries and failure paths for each change.
3. Update this document with the actual changes and remaining work on each push.
   Publish untested checkpoints without implying a build/smoke/FPS result.
4. The next owner may validate these untested changes; the outgoing session’s
   code-only restriction does not carry over. Prioritize a build and focused
   smoke before claiming correctness or performance improvement.
   Any future Windows validation must be visible from the original asset folder,
   native exe beside SLUS, with menu/rider/course/track screenshots and baseline FPS.

## Pending source-only checkpoint after 9aa6a5b6

Latest independently verified publication: [9aa6a5b6](https://github.com/GTTeancum/RacerRevengePC/commit/9aa6a5b6929e4435aacffce2c324be08a0d6d1ee), a documentation-only checkpoint.

New source is **UNTESTED**: successful racer coverage cleanup reuses its already
selected texture unit2, retaining explicit selection on every rejection. The
snapshot unbind, remaining restoration and error checks are unchanged. Two
source reviews accepted the guard. See [proof](recovery/coverage-tail-texture-selector-20261017.md).
One API call is removed per successful bridge; no measured gain is claimed.
The latest 0.8035408549754633 FPS result predates this source change.

Map-2 alpha-invariance research also remains held: opaque materials do not prove
constant alpha, equal-depth base provenance matters, and the current payload
cannot accumulate overlapping fragments. See [findings](recovery/map2-invariant-alpha-held-20261017.md).

A second **UNTESTED** source change reuses the RA private raster/scene FBO draw
selectors initialized and checked at extent creation. It removes only three
per-primitive reassertion sites; snapshot fallback and final cleanup selectors
remain. See [proof](recovery/ra-coverage-fbo-selector-reuse-20261017.md).
No new build, smoke or FPS measurement was run.

The RA blended-AA0 path also reuses composition mode1 after the first checked
primitive of each selected draw. New draws and clipped-leading cases retain
initialization, and failures stop before reuse. This is **UNTESTED**; see
[mode proof](recovery/ra-composition-mode-reuse-20261017.md).

Direct RA blended-AA1 draws now retain their program after the first checked
primitive within a selected draw. Program0/AA0 bindings remain unconditional,
and new selected draws still bind before uniform writes. This is **UNTESTED**;
see [program proof](recovery/ra-aa1-program-reuse-20261017.md).

## Checkpoint after d6651457

Latest independently verified publication: [d6651457](https://github.com/GTTeancum/RacerRevengePC/commit/d6651457efa7b8814151b0ca5fc936f9879d99e8),
tree37fb609d8bbb99764d7df34c8cab4d9d44436aca, parent9aa6a5b6. It contains the
four untested state-reuse changes above, with exact source-only review.

The new **UNTESTED** change removes two redundant private-FBO selector writes
from the RA snapshot blit fallback. Bindings, blit, error check and final
restoration remain; CopyImage is unchanged. See [proof](recovery/ra-snapshot-selector-reuse-20261017.md).
No new test ran and no FPS improvement has been measured.

RA lighting preparation now constructs each private Snapshot in its reserved
vector rather than copying a completed stack row. Publication remains gated on
full preparation success; all arithmetic and validation retain their order.
This source-level copy removal is **UNTESTED**, with no measured allocation or
FPS benefit. See [proof](recovery/ra-lighting-snapshot-in-place-20261017.md).

Original HUD packets now reuse depth-mask, blend-uniform and texture setup only
when adjacent material IDs match. Packet/triangle order, texture barriers and
all error gates remain. Clear and bright gleam IDs remain distinct. This is
**UNTESTED**; see [proof](recovery/hud-adjacent-material-state-20261017.md).
