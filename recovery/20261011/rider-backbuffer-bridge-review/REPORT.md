# Dormant private-publication BACK-copy bridge

2026-10-11. Staged source-only against checkout commit `6dc2c8b1b3064f8fdeb5f12648fe4583119c1b9e` at `/workspace/shared/rr-recovery-git`, implementing section 2 of `../native-blend-scheduler/PLAN.md`.

## Result and scope

Two renderer candidates are staged outside the checkout, with exact preimages and a unified patch:

- `candidate/platform/linux/native/vehicle_window_gl50ds.h`
- `candidate/platform/linux/native/vehicle_window_gl50ds.cpp`
- `preimages/platform/linux/native/vehicle_window_gl50ds.{h,cpp}`
- `backbuffer-bridge.patch`
- `PREIMAGE-SHA256SUMS` and `CANDIDATE-SHA256SUMS`

No checkout edits, host/frontend/audio/recorder/build-list changes, callers, enable flags or movement-admission changes were made. No dependency was touched. Git publication and centralized execution remain held. This report is source inspection, not compilation or behavioral evidence.

## Public contract

The candidate adds `copyOriginalUiPrivatePublicationToWindow(const OriginalUiPrivatePublication50KP&, OriginalUiWindowBackBufferCopy&, VehicleWindowDiagnostic50DS&) noexcept` and its renderer-issued `OriginalUiWindowBackBufferCopy` result. The provisional names from the plan are retained.

The result records:

- An independent nonwrapping successful-copy sequence within the issuing renderer lifetime.
- A weak renderer lifetime and a copied, immutable private-publication receipt containing the exact publication identity, kind, prior private completion and complete existing lineage.
- The exact publication viewport, exposed by `width()` and `height()`.
- An explicit `windowBack` output kind, with `none` for an empty result.
- Historical checked command acceptance, exposed by `commandsAccepted()`.

It exposes no currentness/validity/consumption operation and no window-copy completion, swap, display, vblank or transition-completion claim. The nested publication's `completed()` describes only the already completed private image. Later BACK-copy GPU completion is unknown. An old result remains a historical observation; it does not pin pixels or authorize a later swap.

## Source-order acceptance and failure behavior

1. Clear the diagnostic. Reject input aliasing `output.publication()` before clearing output, preserving the input in that legitimate nested-alias case and matching the existing completion API convention. Every nonalias failure leaves an empty output. Typed references retain the renderer's existing requirement for distinct live storage; no speculative pointer probing is added.
2. Require `originalUiSourceRgbaAvailable()`, which includes initialized state and the cold source capability/objects. Reuse `Storage::current` for the creating thread and the exact current context/drawable callback contract. Require idle renderer state and `defaultBackBuffer` output mode.
3. Require the supplied private receipt to be completed and to pass `matchesOriginalUiPrivatePublication`, which checks the weak lifetime, exact generation, committed selected UI target and matching committed dimensions. Require positive viewport dimensions within cold bounds and an unsaturated independent copy serial. All of these precede any GL call in the bridge.
4. Reuse the existing GL-error preflight and `checkDefaultFramebuffer()`. Reuse the existing checked `copyToWindow` body for the selected private target, full viewport NEAREST blit, BACK selection and checked framebuffer-zero restoration. GL errors from the copy retain the existing `presentation` diagnostic classification.
5. Only after every helper succeeds, copy the private lineage into the output and increment the independent copy serial. No other generation, publication, source/sample state or raw ownership changes.
6. Failed preparation issues no new receipt. Once the helper attempts the blit, `defaultFramebufferMayBeModified` remains true even on later copy/restoration failure. The caller must not swap. The private selection, publication generation, supplied completion and raw incoming ownership remain unchanged, so an explicit later full copy may repair BACK if the context remains usable.

The input/output alias rejection preserves any preexisting historical output rather than destroying its embedded input; it does not issue or refresh a result. An aliased call must not be treated as a successful copy.

## Exact viewport detail

The existing zero-argument `copyToWindow()` reads working frame `width`/`height`. Those can differ from the retained committed publication after a failed later submission. The candidate preserves every existing call through a zero-argument forwarding overload and adds a dimension-taking overload used by the new bridge. The checked GL body is otherwise unchanged; the new bridge supplies the authenticated publication dimensions without modifying the working frame or committed state.

This does not verify a live drawable resize. The renderer request has no live-size getter. The future window owner must independently verify that the actual drawable still equals the publication viewport before copying and immediately joining a matching host operation. Resize must not silently turn the historical receipt into a cropped/rescaled image claim.

## Static review limits

The staged diff contains only the new receipt/API, one zero-initialized independent serial, the dimension-taking existing-helper overload and the dormant method definition. No new texture/FBO, shader, context, `glFinish`, swap, sleep, event loop, interval policy or allocation is introduced. Existing ordinary callers still reach the identical helper GL sequence with their existing working dimensions.

Independent source review completed with no blocking finding; see `INDEPENDENT-SOURCE-REVIEW.md`, pinned to the unchanged candidate hashes. Exact candidate/preimage hashes are recorded in the manifests; root should match preimages before integrating because other workers may advance the checkout. No project parser/importer/decoder, build, test, syntax check, sanitizer, gameplay launch, GL probe, native project execution, guest instructions or emulation was performed. No denied action was retried or rerouted.

## Remaining host gap

This slice is deliberately dormant and does not admit any Rider direction. The application still lacks the exclusive owner that joins an accepted BACK copy immediately to a matching real host submission, bars intervening window writes and duplicate use, and cancels pending authority on resize, context change, interruption or ambiguous host failure. The renderer cannot see host swaps; no live BACK authority is fabricated.

The existing SDL window swap reports only issuance/return of a void swap call. The active runner requests interval zero. A real supported native presentation-event or separately reviewed one-boundary swap-paced provider remains absent. Private `glFinish`, this accepted blit and SDL swap return cannot supply that missing event or prove display. A later checked BACK-completion operation would still be distinct from a display event. Host scheduling, source-order adaptive alpha, audio/capture joins and movement admission remain separately gated as described in the scheduler plan.
