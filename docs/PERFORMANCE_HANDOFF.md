# Racer Revenge performance handoff

Updated: 2026-10-16 9:17 p.m. EDT (2026-10-17 01:17 UTC).

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

## Checkpoint and evidence

The commit containing this document is the current handoff checkpoint; use its
Git history to identify the exact revision. A document cannot contain its own
final commit hash. Last independently verified **tested** publication:
[e53d3220](https://github.com/GTTeancum/RacerRevengePC/commit/e53d3220ddcd721f67aeda7a84a80b2504c0e5e7).
Its exact source tree is `7cdb5a7a64d53a3897c978177dd382545aa6bcfc` (local tested
source `75fbe2e0c61ca5b0041b877c2450edce731008c6`).

Last Linux build passed at 2026-10-16 22:40:30 UTC. Last smoke ended at
22:44:22 UTC: exit 0, cleanup true, five real gameplay swaps, native ticks
28–48, eight original AI racers, zero human racers/host-driver calls/keyboard
packets. Main menu, rider selection, course selection, track and original Pause
screenshots were reviewed. Escape → Pause → Continue resumed gameplay.
Historical baseline: **0.851877902726142 FPS** on the Linux llvmpipe VM.
This result does not validate changes after that tree. No GitHub CI pass was
established. No fresh Windows test was performed.

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

## Current untested changes

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

## Avoid unsafe shortcuts

- Every racer, including followed player, must retain original in-game AI.
- Native host CPU/GPU code only; no PS2/VU emulation or guest instructions.
  SLUS is static recovery input and must never be executed.
- Preserve original geometry, assets, byte composition, coverage, depth and
  alpha semantics. Do not reduce quality or omit racers to improve FPS.
- Multisample same-texture feedback with texture barriers remains unproven;
  per-texel exception does not establish safe four-sample read/write feedback.
- Shared-edge triangle batching is held: CPU/GPU transform and subpixel error
  bounds have not been proven. Arbitrary epsilon/padding is not a proof.
- Existing disjoint conservative-rectangle grouping, VAO query hoist, private
  FBO selectors and sampler initialization already exist; do not duplicate them.

Previous verified untested-source publication: `71aa1b2149b5a9e91ec197e0524fed5d57681cbf`
(tree `6d2090763a992dd5148f59fdf49703a34b247e2a`, parent `7787eb22`).
It includes the texture-selector/binding reuse above. The current checkpoint adds
the narrowly bounded checked depth/framebuffer reuse and documentation. All changes after the
tested baseline are untested. Source review does not establish compilation or
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
