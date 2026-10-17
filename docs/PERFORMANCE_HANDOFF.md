# Racer Revenge performance handoff

Updated: 2026-10-16 8:03 p.m. EDT (2026-10-17 00:03 UTC).

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
final commit hash. Last independently verified publication before this update:
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

Previous documentation-only handoff publication: `847798661571b1852e3f6c2c5b01ec7b8e09f4a5`,
verified tree `818ee732f9580900201d0e75d02742a9484d3710`, parent `e53d3220`.

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

## Known functional limits

Original Pause → Continue works in the historical muted ALL8 spectator smoke.
Other Pause actions remain unavailable; this is not a complete menu or game.
Audible Pause remains gated on missing original audio receiver semantics.
Aggregate cleanup now releases internal camera observer owners before checking
for external runtime ownership, retaining external-borrow guards.
Wreck collection work is held and not active gameplay evidence.

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
