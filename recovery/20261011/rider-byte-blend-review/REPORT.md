# Dormant native immutable-pair byte blend

Date: 2026-10-11. Exact preimage: `41736c3c1ff4cee02b2d02d3383be9b09d5c14fa`.

## Result and scope

Two-file renderer-only staging, outside the active checkout. Adds private immutable incoming RGBA ownership, a retained-old borrow, independent RGBA effect storage, an integer-byte sample shader, typed accepted/completed sample receipts, readback, and checked completion/deletion retirement. There are no new ordinary UI callers or enable flags, and no changes to source submission, page/session logic, movement rejection, timer, audio, window swap, capture ledger or build lists.

This is a low-level dormant renderer primitive, **not complete presentation slice2 or transition admission**. It authenticates exact source/snapshot lifetimes and terminal material state, but does not manufacture a fresh-Main/closed-history, no-observer, popup-free, unlocked-appearance, finalization or host-displayed certificate. Those genuine frontend/host owners remain required before activation. No effect publication API is supplied here: an effect sample is a private renderer result, not an accepted page update, ordinary source frame, presentation or host swap. Final whole-incoming promotion is outside this stage.

Only source reading, textual editing/diffing and hashing were performed. No build, test, syntax check, project parser/importer/decoder, shader compilation, GL probe, native or guest launch, sanitizer or instruction emulation was run. The shader compilation code is staged source, not executed validation. No image-generation tool or generated transition artwork was used.

## Exact files

- `platform/linux/native/vehicle_window_gl50ds.h`
- `platform/linux/native/vehicle_window_gl50ds.cpp`

`preimages/` and `files/` preserve the exact paths. `PATCH.diff` is the candidate patch. Hash manifests separately pin preimages, candidates, patch and reports. Active checkout was read only and remained at the pinned commit during staging; root owns integration and all future execution checks.

## Admission and ownership

`beginOriginalUiByteBlend` requires an actual retained snapshot from this exact live renderer/context/thread and a strictly newer, still-current, completed ordinary incoming source receipt. It does not compare composer frame generation or bank/registry generation across old/new: a real retired/replaced composer may reset those local sequences. Renderer target generation remains monotonic and establishes order.

The incoming completed receipt must carry the actual terminal canonical token3217 (`ui/textures/icon_x`), pass1, nonzero registry/bank generations, enabled one-map UV0/context0 state, and these complete source semantic fields:

- ALPHA A0/B1/C0/D1/FIX128 = `0x0000008000000044`
- CLAMP U1/V1/MINU0/MAXU0/MINV0/MAXV0 = `0x5`
- COLCLAMP1, PABE0, FBA0

The already-integrated composer copies these values once from the final actual drained canonical representative after binding-history updates; the renderer seals them only after successful source work and completes the source receipt through checked `glFinish`. This stage reads those owned values, not a constructor default or a lossy native sampler/blend enum. It uses the authenticated canonical token already present rather than adding a caller-writable readiness/name flag. Schedule-local leaf numbers and the ordinary prompt's TEX1 values are deliberately not mistaken for fixed effect packet fields.

Current-source receipts are still observations, not pins. Begin instead allocates a distinct640x448 RGBA8 incoming texture/FBO and a separate640x448 RGBA8 effect texture/FBO, verifies storage/completeness, copies the entire actual incoming source with nearest RGBA blit, and checks real `glFinish` before issuing the pair lease. Thus later normal source clears, bank changes or successful source-composer retirement cannot mutate the sampled incoming image or leave a dangling composer/material pointer in the lease. All source metadata is an owned value.

Old remains the existing immutable retained640x448 RGBA8 snapshot. Begin records its borrow before any fallible GPU allocation/copy. `releaseRetainedOriginalUiSource` rejects while that borrow exists, including failed begin/sample/deletion states. No new public handle exposes the private texture/FBO names. Existing single-thread, live-current-context, no-external-mutation ownership rules continue to apply.

## Exact native sample

The immutable descriptor retains source XYZ endpoints0,0→639,447; UV endpoints1/16,1/16→640,448; TEX0 logical extent1024x512 and640-pixel buffer width; PSM0/RGB-only modulation; nearest TEX1=0; U/V clamp1; modulationRGB128; alpha0..128; original PRIM/TEST and inherited ALPHA/CLAMP values. The existing source textures remain640x448; no invented1024x512 image or padding is allocated.

Each sample executes a full private target pass with integer pixel addresses and an explicit border branch. It is not a generic fullscreen normalized texture mix. Source top-down(x,y) corresponds to GL texel(x,447-y), so preserved GL borders are column639 and row0. The shader fetches old and incoming with identical integer `texelFetch` coordinates.

For285633 covered pixels (`639*447`), RGB is `old + floor((incoming-old)*alpha/128)` in integer byte units. Signed negative division is explicitly floored using `-((-value+127)/128)`; every product is bounded by32640. Output alpha is the current source byte0..128, normalized only at the RGBA8 store. All1087 excluded border pixels retain old RGBA. The previous effect result is never an input.

Alpha0 still draws: interior RGB is old, interior alpha is0, and old RGBA survives on borders. Alpha128 still draws: interior RGB is incoming and alpha128, while all old border channels remain. Neither endpoint is optimized into old/incoming publication. Subsequent normal incoming promotion must still select the entire incoming image and its actual alpha under a separate future owner.

No conventional GL blending, depth consumer, sample coverage, MSAA, gamma correction, premultiplication, fog, dithering or color logic participates. The pass explicitly sets lower-left clip convention where supported, fixed viewport, fill mode, all-channel writes and isolated input samplers, and disables depth/cull/stencil/scissor/clip-distance/sample/rasterizer-discard interference. It draws with a private empty VAO and never modifies the ordinary UI element buffer. Input texture units and draw bindings are unbound afterward. Restoring the renderer depth baseline is **not** a source depth clear; the next ordinary source submission retains its existing actual source color/depth clear.

The documented native raster contract in `../video-owner-resolution/raster-contract/REPORT.md` and its independent review is the basis. Same-index sampling follows the manual's analytic interpolation/integer-part contract. Undocumented physical GS DDA finite precision remains unverified; this patch makes no measured silicon, retail timing, PS2/VU emulation or hardware parity claim. The earlier TEXA label is not repeated: the source packet's zero selector0x3F write was TEXFLUSH, and RGB-only texture sampling does not require an invented TEXA operation.

## Receipts, completion and failures

Begin success issues a generation-bearing immutable-pair lease only after the incoming copy and checked completion/restoration succeed. Partial allocation, failed shader compile/link, incomplete framebuffer, failed copy or completion retains every acquired name and the old borrow for explicit release/close; it issues no valid lease.

A sample attempt invalidates earlier sample authority before any target write. Only a successfully checked actual draw and binding restoration publish a new sample generation. An attempted write failure poisons the lease; later sampling is rejected until retirement. The effect serial is monotonic across leases and a CPU serial increment never stands for GPU completion. Parameter/receipt rejection before beginning the attempt leaves existing valid storage intact.

`completeOriginalUiByteBlendSample` validates the exact live pair/sample generation and issues a completed value only after checked `glFinish`. A completion failure poisons the pair and invalidates current sample authority. `readOriginalUiByteBlendSampleRgba` requires that exact current completed sample and reads the actual640x448 effect RGBA. Later sample attempts, lease retirement, close or renderer replacement invalidate prior sample authority. Separate completion output must not alias its input; begin rejects output aliasing its own nested input values.

`releaseOriginalUiByteBlend` also handles partial begin/sample states. It checks real completion, invalidates authority, unbinds both input textures/samplers/program/VAO/FBO, and deletes all owned names before releasing the old borrow. Failed deletion keeps unresolved ownership and the borrow for retry. As with the existing conservative renderer cleanup contract, no owner may create/reuse GL names following ambiguous deletion until retirement succeeds. Context/thread failure does not erase ownership.

Close accounts for all new names, checks completion while any blend borrow exists, invalidates weak receipt lifetime before deletion, and deletes the blend objects before retained old. Failed cleanup remains retryable through close; existing destructor termination on unresolved ownership is preserved. No no-op-deleter context owner, synthetic fence or fabricated successful abort state is introduced.

## Ordinary behavior and remaining work

All new textures, shader compilation, VAO allocation and the `glDrawArrays` entry-point lookup are lazy inside explicit begin. The normal create, source-composition, publication, readback and host swap paths gain no new GL operation or completion wait. Ordinary close's new name-deletion helper is a no-op when these handles are zero. The retained-release extra condition is false unless the new explicit blend path has acquired a borrow. Existing normal UI pixels and movement rejection behavior are unchanged by source design; no execution-based claim is made.

Independent source review is in `INDEPENDENT-SOURCE-REVIEW.md`. Future integration still needs a genuine bounded history/host-display owner, outgoing resource retirement, source suspend/resume/finalization, selected incoming promotion, adaptive native presentation timing, independent PCM/capture accounting and execution/driver validation. This stage must not be wired to movement before those separate joins are admitted.
