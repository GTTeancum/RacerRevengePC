# Independent source review: renderer source-retention slice1A

## Verdict

No blocking correctness issue found in the final five-file staged version identified below. This is a **source-inspection-only admission recommendation for dormant renderer APIs**, not build, execution, visual, host-presentation, or full slice1 approval.

The earlier authored-pass-only terminal-state gap was corrected during review: the final version copies the retained binding's effective GS state after `recordDrainedBinding`, including the IPUM TEXA override. The conservative failed-deletion/name-reuse restriction is now explicitly documented for the new retained-release API.

The focused post-publication baseline refinement was subsequently reviewed against the exact preimage. It restores the prior post-copy baseline on both success and copy failure before opaque publication can commit. No new blocker was found; the source-inspection-only verdict is unchanged.

## Scope and identity

Reviewed staged `files/` against corresponding `preimages/`, plus relevant unchanged renderer, material-equivalence, binding-history, and frontend source. All five preimages were checked by SHA-256 against the checkout and against the stated HEAD, `0ee94e6707c5088bd99c01f46cb3e984c50dac0c`. They match.

Final reviewed staged SHA-256 values:

| Relative path | SHA-256 |
| --- | --- |
| platform/linux/native/vehicle_window_gl50ds.h | 96adb83d47fba27a8af2d280eece552b8f09d01edd314e883b9e2da57f06c2e2 |
| platform/linux/native/vehicle_window_gl50ds.cpp | 51dbe90ca8a65d0efe6d408db7bb278186586efed9bef6d0f4479ec2e9548e44 |
| platform/linux/native/original_ui_native_draw50kg.h | aaaa82a4eca1c69c8071fee9ab6ac1842508ef27294b5e389a40375e0770de2e |
| platform/linux/native/original_ui_rider_draw_contribution50kg.cpp | 98ec5a291905f3a8896e2fcdd5e372ab83d200f604bc4536b22f3b42c00228d6 |
| src/assets/original_ui_material_binding50kg.h | 0a81ddd6ac43854fd851cf279f57b85c52c27bb9a7cf5cc626c81b10150d1cb9 |

## Reviewed properties

### Actual-drain source terminal

- The composer iterates its validated nonempty packet order, sorted by final redirected leaf and FIFO ordinal. The terminal is overwritten by each actual nonempty leaf encountered, so the final value comes from the last drained representative rather than the highest registered row or the submitting alias's prototype.
- Representative lookup and registration bounds are checked. Terminal identity includes canonical registration token, pass, leaf, registry generation, and bank generation.
- `copyEffectiveGs` copies all original GS fields from the retained semantic binding state. Inactive FIX and the full clamp-region fields are not reconstructed from lossy native enum choices.
- The copy occurs after `recordDrainedBinding` for textured representatives. This carries the actual-image IPUM TEXA mutation. Zero-map representatives retain their initialized construction GS state.
- The source-PC and byte-offset arrays are expressly construction provenance. They are not mislabeled as provenance of the later effective texture-alpha mutation.
- The new accessor uses the already-declared `OriginalUiMaterialBindingState50KG` friendship in the equivalence-key class; its field types and fixed array extent match by source inspection.

### Source acceptance and publication separation

- The existing combined `submitOriginalUiSourceRgba` entry remains the default frontend call. New split/retention entry points have no frontend, suspend, effect, or movement callers in the baseline.
- Successful source acceptance is recorded only after the complete source draw and renderer-baseline restoration check. A subsequent opaque copy/window-copy failure does not erase accepted material binding history.
- Hidden submission passes through the same sealed-frame, texture, geometry, span, sampler, owner, and generation checks. It skips default-framebuffer validation and opaque publication only; it does not bypass the source draw.
- The composer history acknowledgement now uses `originalUiSourceAccepted`, with the existing exactly-once generation and pending-history guards. Potentially partial source work still poisons history; pre-draw rejection remains retryable.
- Hidden source work leaves the private displayed target, committed dimensions, and ordinary opaque readback selection unchanged. The generalized `copyRgba` retains the previous displayed-target fallback and packing logic for ordinary readback.
- Successful combined publication still copies source RGB into the alternate opaque presentation target, forces presentation alpha to one, commits that target, and optionally copies to window BACK. The source RGBA texture remains separate. Explicit publication also disables scissor, blend, sRGB, and dither.

### Focused post-publication GL baseline refinement

- The exact preimage's successful headless source path leaves both framebuffer bindings at the private `uiTargets[pending]`. Only the successful window path reaches framebuffer zero through `copyToWindow`; there is no preexisting headless framebuffer-zero restoration to preserve.
- The refined helper records the opaque-copy check result, then unconditionally reapplies disabled scissor/blend, writable depth, LESS depth comparison, all-channel color writes, clear depth one, VAO zero, and array-buffer zero. It checks that restoration before committing any new opaque target, dimensions, acceptance flag, or publication generation.
- A failed opaque copy therefore still receives the baseline-restoration attempt. A failed copy or restoration prevents the private presentation commit and prevents a window copy. Earlier successful source acceptance/material-history acknowledgement remains separate, as intended.
- The source draw already restores the UI VAO's immutable RGB element buffer before it unbinds the VAO and acknowledges source acceptance. The publication helper does not change an element-buffer binding, so it neither needs nor attempts an invalid element-buffer restoration while VAO zero is current.
- This refinement adds no framebuffer binding change to the historical headless/window behavior. GL-error behavior remains source-inspected only; no fault injection or driver execution was performed.

### Receipt freshness and authority

- Default receipts contain no usable lifetime or nonzero generation. Issuance records a renderer-owned weak identity and exact source target/frame/texture-publication values.
- Completion and separate publication revalidate current context/thread, idle state, weak renderer identity, current target generation, accepted target generation, frame generation, and recorded texture publication.
- The next attempted source clear advances the target generation and clears its accepted/publication eligibility. Old current-source receipts therefore cannot be reused after the reusable source has changed, even when the later frame fails.
- Successful owner retirement clears current source authority while preserving an already retained immutable snapshot. Closing/recreating the renderer cannot revive old weak-lifetime receipts through numeric generation reuse.
- Completion flags are minted only after a checked `glFinish`. The fields exposed by getters do not themselves grant authority, and the public contract explicitly states current-source receipts are not pins.

### Retained storage and publication eligibility

- Retention requires the current accepted source to match a successful renderer publication and the currently committed original-UI presentation target. Successful world, host-menu, or ordinary RGB publication clears source-publication eligibility.
- A failed post-commit window copy clears eligibility rather than claiming a new successful retained publication. No swap or host-display acknowledgement is fabricated.
- The snapshot allocates a distinct framebuffer and 640×448 RGBA8 texture, checks allocation names, texture dimensions/internal format, and framebuffer completeness, and copies all four channels with a full-size nearest blit.
- Scissor is explicitly disabled for the snapshot copy. Successful receipt issuance follows the checked copy, checked `glFinish`, and checked final framebuffer unbind.
- Readback validates the snapshot's weak renderer identity, snapshot generation, retained source target generation, retained publication generation, and actual owned GL names. It is intentionally independent of later current-source generation and owner retirement.
- Readback requires the full 640×448×4 destination extent and reuses tight, PBO-safe pack-state configuration. Snapshot and publication serial overflow are rejected before issuing new authority.

### Cleanup and failure ownership

- Failed allocation/copy attempts do not issue a snapshot receipt. Any allocated GL names stay owned by the renderer; subsequent retention rejects until release or close resolves them.
- Explicit release checks completion before deletion and invalidates snapshot generation before any possible partial deletion. Failed deletion preserves unresolved names for retry.
- The public failed-release contract now prohibits GL name creation/reuse, including through other texture owners, until release or close completes. This matches the existing conservative `deleteName` behavior.
- `hasGlResources` includes both retained names. Close adds a checked completion obligation when explicit source leases or retained names exist, then resets receipt lifetime and retires the retained names along with the source resources.
- The preexisting close/destructor failure ownership behavior is preserved; unresolved resources are not silently discarded.

## Nonblocking documentation follow-up

Two older comments still use “source publication” where the new invariant is successful complete **source acceptance/drain**: the composer's pending-history comment near line 680 and the existing `recordDrainedBinding` contract in `original_ui_material_binding50kg.h`. They do not change execution, but should eventually use the same distinction as the new split API so future callers do not mistake opaque publication for the history-commit boundary.

## Limits and exclusions

Only file reads, textual searches/diffs, and hashes were used for implementation inspection. No implementation files were edited by this reviewer. No build, test, syntax check, project parser/importer/decoder, GL probe, native/guest launch, sanitizer, PS2 emulation, or VU emulation was run.

Source inspection did not reveal a declaration/include mismatch, but it does not establish compilation or runtime correctness. Actual driver behavior, GL-error injection, allocation-failure behavior, byte-for-byte output parity, and timing remain unexecuted. This review certifies neither host swap/display provenance nor complete slice1 route, movement, effect, suspend, or parity behavior.
