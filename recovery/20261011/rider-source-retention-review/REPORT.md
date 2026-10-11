# Dormant native source presentation: slice 1A

Date: 2026-10-11. Exact preimage commit: `0ee94e6707c5088bd99c01f46cb3e984c50dac0c`.

## Result and deliberate scope

This is a coherent first part of presentation slice 1, staged outside the checkout for root integration. It implements real renderer-owned retained source RGBA pixels, accepted/completed source receipts, and a separated opaque publication operation. All new APIs are dormant: no frontend, session, entry, movement, suspension, effect, timer, audio, capture, or swap caller has been added.

It does **not** claim full slice 1 or movement readiness. The renderer never swaps the host window. The image is consequently named **renderer-published source**, not host-displayed source. Actual host-swap provenance, a constructor/history-bounded fresh Main-to-Rider certificate, source-target pinning for a future effect, suspend/resume ownership, an independent effect-result surface, the blend primitive, pacing, and PCM/capture accounting remain future joins.

The source target and retained copy are exactly 640×448 RGBA8. This patch does not crop the original last row/column, enlarge storage to TEX0's logical 1024×512 extent, blend pixels, interpret sprite endpoints/UVs, issue a synthetic TEXA write, or invent an elapsed-time event.

## Exact touched paths

- `platform/linux/native/original_ui_native_draw50kg.h`
- `platform/linux/native/original_ui_rider_draw_contribution50kg.cpp`
- `platform/linux/native/vehicle_window_gl50ds.h`
- `platform/linux/native/vehicle_window_gl50ds.cpp`
- `src/assets/original_ui_material_binding50kg.h`

Preimages and candidate files retain these paths under `preimages/` and `files/`. There are no build-list changes or new translation units.

## Source-state provenance

The composer follows its actual nonempty ordered packet leaves through the final canonical representative table and retained registry history. It records canonical Mat_Stbl token, source pass, resolved leaf, registry generation, and bank generation. It retains every source GS field as an owned integer array, including inactive FIX and all clamp-region limits, plus the original TEX1 inputs and other pass scalars. Construction source-PC/byte-offset arrays are explicitly construction provenance, not a claim that the values were unchanged by a later bind.

The new read-only `OriginalUiMaterialBindingState50KG::copyEffectiveGs` accessor copies the already-owned effective semantic state. It is called **after** `recordDrainedBinding` for the actual representative and actual immutable selected image, preserving real IPUM TEXA mutation at 249818..24983C. The composer cannot derive final state merely from its native sampler/blend enums or authored pass. Zero-map passes use their existing initialized effective state without inventing a texture bind.

This value is initially staged inside the private sealed `OriginalUiSourceFrame50KM`. The renderer copies it into current-source state only after the complete actual source draw sequence and renderer-baseline restoration pass their GL checks. A partially failed frame issues no accepted source receipt. An accepted receipt is explicitly **not completed**; only checked `glFinish` produces a completed receipt. This is native semantic material provenance, not a reconstruction of all inherited guest GS register memory and not a bounded-route absence certificate.

## Pixel ownership and successful flows

The existing source RGBA texture/depth attachment remains the incoming source target. The existing two opaque UI targets remain separate. The retained texture/FBO is a third, distinct source-color owner, allocated only when the dormant retain API is used.

1. The existing `submitOriginalUiSourceRgba` remains the normal combined submission entry. It performs the same source draw and same nearest-fit RGB copy/alpha1 presentation operation. It does not gain an automatic completion wait or new snapshot allocation.
2. `submitOriginalUiSourceUnpublished` executes the full sealed source draw, increments the real target generation, and acknowledges material binding history exactly once. It does not change `uiPublished`, the committed readback image, the opaque targets, or the window back buffer. It requires no host swap/default-framebuffer check because no default-framebuffer work occurs.
3. `originalUiSourceSubmission` issues a receipt for the current accepted source. `completeOriginalUiSource` revalidates context/thread, renderer lifetime, frame generation, target generation, and publication identity, then checks a real `glFinish` before issuing a completed copy.
4. `publishOriginalUiSource` accepts only that still-current completed receipt. It copies the source RGB to a private opaque target, commits that target, and performs the existing optional back-buffer blit. It does not submit geometry, acknowledge material history twice, swap, or advance a page. Publication generation is source-publication-only, not a host-swap counter.
5. `retainPublishedOriginalUiSource` accepts only the latest successful renderer publication while the exact source target still contains it. It allocates/verifies a distinct RGBA8 texture/FBO, copies all four channels with a full640×448 blit, and checks completion. It returns one immutable snapshot receipt, including its originating source and publication generations. Source receipt lifetime identity is a privately owned shared object; no borrowed/no-op-deleter context owner is created.
6. A later source clear invalidates ordinary current-source receipts but cannot alter the retained texture. The retained receipt owns all its material values, so it remains readable after the old source composer retires and is destroyed. `readRetainedOriginalUiSourceRgba` reads that exact retained target and size; ordinary `readbackRgba` still reads the ordinary committed opaque image.

Only one retained snapshot is admitted at a time. Current-source receipts are generation-validated observations, **not pins preventing reuse**. A future immutable incoming effect lease still needs to prevent another source clear while it borrows the incoming surface.

## Failures, cleanup, and retirement

- Source-generation advance occurs at the actual source clear attempt. It invalidates the previous accepted/current-source publication, even when later work fails. A CPU generation increment is never a completion fence.
- Material history is committed on successful source acceptance, independently of later opaque-copy/window-copy failure. This deliberately corrects the previous conflation: once source binding actually succeeded, a presentation failure must not replay or discard that binding history. Existing `originalUiAccepted` continues to mean opaque private image acceptance for ordinary callers/capture.
- Failed or partial source draws retain the existing concrete composer retirement obligation and history poisoning behavior. Explicit source-owner retirement still checks `glFinish`; it also invalidates current-source receipts. It does not delete a retained pixel copy.
- Snapshot allocation, framebuffer validation, copy, or completion failure publishes no snapshot receipt. Any GL names already acquired remain owned by the renderer and are visible to its resource/cleanup accounting. The caller can explicitly release them or close, with retry on failure. A second retain cannot overwrite those names.
- `releaseRetainedOriginalUiSource` checks completion before deletion. It invalidates snapshot authority before deletion begins. Failed deletion retains conservative name ownership for retry. Its public contract forbids GL name creation/reuse by this or another owner until release/close finishes after an ambiguous deletion failure, matching existing failed-close behavior.
- Close performs checked real completion before deleting resources whenever the new explicit receipt/snapshot path has been used. Context/thread mismatch or completion failure leaves unresolved ownership and existing explicit-close/destructor policy intact. Successful retirement expires the weak receipt identity, preventing lifetime-address reuse from accepting old receipts.
- A failed back-buffer blit retains the established private committed-image/readback semantics, but it does not establish a new retainable source publication. A successful foreign race/host-menu/RGB-UI publication invalidates the current source-publication provenance. An already-retained snapshot remains a valid old image, without becoming evidence that it is currently displayed.

## Verification and remaining admission gate

Only source/file inspection, textual editing, diffing, and hashing were used. No build, test, syntax check, project parser, asset importer/decoder, GL probe, native/guest launch, sanitizer, PS2 emulation, or VU emulation was run. No runtime, driver, pixel, timing, or performance result is claimed.

Independent source review is recorded in `INDEPENDENT-SOURCE-REVIEW.md`. Root must compare exact preimages before integrating the staged patch. This source-only result is not a validation pass for the remaining active movement/presentation path.


## Root-review refinement: post-publication baseline

Root requested a focused comparison of the old combined path's post-call GL state. The exact preimage restores framebuffer0 only in `copyToWindow`; successful headless source submission already leaves the selected private UI framebuffer bound. The staged helper preserves that distinction rather than adding an unsupported default-framebuffer operation to headless mode.

The helper now reapplies and checks the original depth/color/VAO/array-buffer baseline **after** the opaque copy, including its error path, and before private publication commits. This addresses a real narrower failure-path difference introduced by moving publication after source restoration: previously, an opaque-copy error still passed through the baseline restoration. Source acceptance/material history remains established before this separate publication operation; a failed post-copy restoration creates no new opaque acceptance or publication generation. Window success still finishes through the unchanged `copyToWindow` framebuffer0 restoration.

The prior successful-state equivalence is preserved: scissor/blend disabled, depth mask enabled, depth function LESS, full color mask, clear depth1, VAO0, array buffer0, actual output viewport and alpha1 clear color. The UI VAO's immutable RGB element binding is restored by source composition before publication; publication does not touch element bindings. The source composition already disables sRGB/dither, and explicit publication also disables them. Current program, texture, and sampler bindings are not changed by the copy helper. Headless read/draw framebuffer remains the pending UI target with color attachment read/draw selection, as in the preimage; window success binds framebuffer0.
