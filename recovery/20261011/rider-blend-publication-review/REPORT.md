# Dormant private byte-blend publication and incoming promotion

2026-10-11. Exact preimage commit: `f513a1f207a6eaf0b155fc853551a9f26529513a`.

## Result

Two renderer files staged outside the active checkout, with exact preimages and `PATCH.diff`. Adds private opaque effect publication, separate accepted/completed publication receipts, whole immutable incoming promotion, and separately retained raw incoming RGBA ownership. No caller, movement admission, frontend/session change, host presentation, scheduler, clock, PCM, capture ledger, build list, or dependency change is added. Root alone integrates.

This is a dormant renderer ownership slice. It does not complete the presentation pump or establish displayed/fresh-Main/closed-history/no-observer/popup/appearance admission. Promotion does not claim that alpha128, a timing budget, or any transition schedule was executed. Those owners remain separate prerequisites to activation.

Only source reading, textual editing/diffing and hashing were performed. No builds, tests, syntax/sanitizer/gameplay checks, GL probes, parsers/importers/decoders, project execution, guest instructions or emulation ran. The existing legacy-driver-related execution and Git-publication hold remains unchanged. No active checkout file was edited.

## Separate authorities

- `OriginalUiSourceSubmission50KN` remains the actual ordinary source drain observation, with its existing target freshness and material-history semantics. No new source receipt or source generation is issued by this slice.
- `OriginalUiByteBlendSample50KO` remains an actual private sample receipt. Its completed state is required to publish its image, but does not certify the subsequent opaque copy.
- `OriginalUiPrivatePublication50KP` identifies a successful private opaque copy, its kind, independent monotonically increasing publication generation, exact blend/sample lineage, original incoming frame/target lineage, old snapshot and viewport. It begins uncompleted. Only a separate checked `glFinish` creates a completed publication value.
- `OriginalUiPromotedIncoming50KP` identifies immutable raw incoming ownership transferred out of the blend. Its generation uses the renderer's already-monotonic blend generation and its original-source receipt remains historical lineage, not a new source submission.
- No type certifies a default-back-buffer copy, host swap, display event, timing sample, page update or completed movement. New publication methods do not call `copyToWindow`, even on a renderer configured for window output.

The new diagnostic publication fields are distinct. New publication does not set `originalUiAccepted`, `originalUiSourceAccepted`, or `originalUiByteBlendAccepted`, and does not increment the ordinary source publication counter. The original incoming clear is captured at blend begin, so later hidden source work cannot change the letterbox RGB.

## Pixels and ownership

The opaque copy helper is extracted from the existing ordinary source path without changing its GL command sequence, viewport fit, nearest full-image blit, alpha255 clear, or baseline restoration. New APIs reuse it against either the completed private effect target or the immutable incoming target. Only the independent pending opaque UI target is written. There are no new shaders, textures, framebuffer allocations, source depth writes or source clears in this slice.

Every effect sample retains its previous exact639x447 byte operation and1087 old-border pixels; publication adds only its separate RGB/opaque copy. Incoming promotion instead copies the entire640x448 immutable incoming image, including incoming borders, independently of whatever alpha was last sampled. It then moves the already-completed incoming FBO/texture names to the promoted owner without any fallible work after the copy succeeds. Its original RGBA bytes are never made opaque or rewritten into the reusable source target.

After transfer all blend/sample use and promotion retries through that lease are rejected. The old borrow and remaining effect objects remain owned until explicit `releaseOriginalUiByteBlend`; that deletion no longer includes the moved incoming names. `readPromotedOriginalUiIncomingRgba` reads original full RGBA, including original alpha and borders, independently of opaque publication completion or freshness. The promoted image survives later ordinary source work, composer retirement, blend retirement and later opaque publication until explicit promoted-image release or renderer close. One promoted raw owner is allowed at a time.

This does not reclassify promoted raw storage as an actual ordinary source submission, automatically create a retained outgoing snapshot from it, or create a future-transition admission path. A future owner must join that raw provenance explicitly if needed.

## Failure and freshness

- Parameter/currentness/receipt/overflow rejection issues no output and writes no image. Source, sample, private publication and promoted generations have separate scopes; weak renderer lifetime prevents reuse after recreation.
- A failed opaque copy or binding restoration does not select its pending target, transfer incoming names, or issue a publication receipt. The prior selected image stays intact. Immutable pair/sample ownership stays valid, so a deliberate retry can overwrite the entire pending surface. No success or completion is inferred from a partial pending write.
- A successful private copy issues an accepted receipt only. A separate checked completion is mandatory for typed opaque readback. A completion failure invalidates publication authority and preserves independently owned raw incoming storage; it does not permit sampling after a completed ownership transfer.
- Every successful private-image replacement invalidates older private publication receipts: the new API, ordinary source publication, ordinary RGB UI, race and host-menu commits. This includes an ordinary private commit whose later window copy fails. Failed pending copies, hidden source-only work and later blend samples leave the independently copied prior publication intact.
- Completed typed readback checks the exact current publication and viewport; original incoming raw readback uses its separate immutable owner. Existing generic renderer readback still observes the selected private target, without being relabeled a page/source acceptance.
- Promoted release checks real completion before unbinding/deletion, invalidates raw authority before deletion, and preserves unresolved names for retry/close after failure. The existing no-GL-name-reuse rule applies after ambiguous deletion. Cleanup counts promoted names, waits before deletion, invalidates lifetime, and retires moved storage separately from the blend.

## Ordinary behavior and remaining verification

Ordinary paths gain only dormant receipt invalidation fields and zero-name cleanup checks. There is no extra ordinary allocation, shader lookup/compile, GL operation, completion wait or window-copy change. The active renderer's standard acceptance semantics remain unchanged by source inspection, with no execution claim.

Independent source review is recorded in `INDEPENDENT-SOURCE-REVIEW.md`. All execution/driver validation remains outstanding. Still required before activation: actual outgoing/resource ordering and route provenance, finalization ownership, a genuine host presentation boundary, adaptive native timing, independent PCM/capture accounting, interruption policy, and their integration review.

## Artifacts

- `preimages/platform/linux/native/vehicle_window_gl50ds.{h,cpp}`
- `files/platform/linux/native/vehicle_window_gl50ds.{h,cpp}`
- `PATCH.diff`, `PREIMAGE_SHA256SUMS`, `CANDIDATE_SHA256SUMS`, `DELIVERABLE_SHA256SUMS`
- `INDEPENDENT-SOURCE-REVIEW.md`

Hash manifests pin source preimages, candidates, diff and reports. They are source-integrity evidence only.
