# Independent source review: dormant immutable Rider byte blend

2026-10-11. Reviewer: `review_rider_blend_contract`.

## Verdict

**No blocking source-review findings.** The staged renderer-only primitive implements the previously approved documented native byte/raster operation over an immutable old snapshot and an independently completed incoming copy. Its result and completion receipts remain distinct from source-frame acceptance, opaque publication, host presentation, and movement admission.

This approves the scoped source design for root-owned integration and later regression validation. It does **not** establish compilation, driver behavior, measured pixels, physical GS DDA parity, timing, or working Rider movement.

## Scope and baseline

The frozen active checkout was read at `41736c3c1ff4cee02b2d02d3383be9b09d5c14fa`, with empty `git status --porcelain`. No file in that checkout was edited. The staged preimages match the active source by SHA-256:

- `platform/linux/native/vehicle_window_gl50ds.h`: `96adb83d47fba27a8af2d280eece552b8f09d01edd314e883b9e2da57f06c2e2`
- `platform/linux/native/vehicle_window_gl50ds.cpp`: `51dbe90ca8a65d0efe6d408db7bb278186586efed9bef6d0f4479ec2e9548e44`

Only those two renderer files change implementation. The review inspected their complete textual differences, the sealed source-frame/composer terminal projection and binding-history implementation, the existing source receipt/retained-image paths, GL dispatch and cleanup, and these source reports:

- `video-owner-resolution/PRESENTATION-IMPLEMENTATION-PLAN.md`
- `video-owner-resolution/BOUNDARY-ADDENDUM.md`
- `video-owner-resolution/raster-contract/REPORT.md` and `INDEPENDENT-REVIEW.md`
- `video-owner-resolution/terminal-rider-material/REPORT.md`
- `presentation-slice1a/REPORT.md` and `INDEPENDENT-SOURCE-REVIEW.md`
- `presentation-terminal-copy-once/REPORT.md` and `INDEPENDENT-SOURCE-REVIEW.md`

The raster-contract report's TEXFLUSH correction supersedes the older TEXA label. The staged primitive introduces no TEXA write or interpretation.

## 1. Admission establishes available facts without inventing a route certificate

`beginOriginalUiByteBlend` checks the live renderer's creating thread/current context and idle state. It rejects any unresolved previous blend borrow or names. The retained source must have the same private weak renderer identity, completed source state, nonzero exact snapshot generation, matching retained publication/source-target generations, and live owned retained FBO/texture names. The incoming receipt must be completed and match the renderer's exact current accepted source target, frame and texture-publication identity. Its renderer target generation must be strictly newer than the retained source's target generation.

Ordering the renderer target generations is correct. It does not incorrectly compare per-composer frame generations or require registry/bank generations to be equal across real destination replacement. The old source composer may already have retired; the old pixels and terminal values are owned, not dangling pass pointers.

The incoming terminal gate consumes the owned state sealed from the final actual canonical representative and committed by successful source acceptance. It requires canonical Mat_Stbl token 3217/pass 1, nonzero registry/bank generations, enabled one-map UV0/context0, and the exact fields:

- ALPHA A0/B1/C0/D1/FIX128, including inactive FIX
- CLAMP U1/V1 and all four region bounds zero
- COLCLAMP1, PABE0, FBA0

The authenticated scheduler supplies token 3217 for `ui/textures/icon_x`; no arbitrary caller terminal flag is added. The gate does not confuse a schedule-local leaf number with stable material identity or overwrite ordinary icon_x TEX1 state with the effect's separate TEX1=0 descriptor.

These checks do not and cannot establish fresh Main route history, unlocked19/20 selection, popup absence, observer/fader absence, native host-swap completion, or the completed movement/resource protocol. The public contract explicitly excludes those claims. No frontend, session, page, movement callback, environment switch, or new request-time enable flag is wired to the primitive. The complete constructor/history-bound transition owner remains future work.

## 2. Immutable-pair ownership

The old input is the existing privately retained RGBA8 image. The begin path installs its internal borrow before the first fallible blend allocation. `releaseRetainedOriginalUiSource` rejects while that borrow exists, including after a failed begin or partial release.

Incoming is not merely observed through a current-source receipt. Begin allocates a private 640x448 RGBA8 texture/FBO, copies the entire current source with a nearest framebuffer blit, and requires checked real `glFinish` plus checked binding restoration before issuing a lease. It also allocates an independent effect target. Actual dimensions, internal format, framebuffer completeness and relevant same-namespace name distinctness are checked.

Thus four source-color roles are distinct: reusable ordinary source, retained old, immutable incoming copy, and effect output. No effect output is sampled as an input. Later ordinary source clears, source-owner retirement, or real bank replacement cannot mutate the completed private inputs. Rejecting those operations for the lifetime of this copied-input lease is unnecessary for pixel or pointer safety. Lease metadata contains owned value receipts, not source pass, bank, page or composer pointers.

## 3. Exact documented native pixel operation

The shader uses a full-target triangle and fixed 640x448 viewport, integer `gl_FragCoord` addressing, and `texelFetch` at matching coordinates in both inputs. There are no normalized UV interpolants, logical1024x512 rescaling, half-texel corrections, or additional Y flips.

The owned source targets retain the ordinary renderer's lower-left GL orientation. The explicit old-pixel branch at GL column639 or row0 therefore preserves exactly the source's final column and final row: 1087 complete old RGBA pixels. The remaining 639x447=285633 pixels use the corresponding incoming RGB.

Byte samples are reconstructed with `floor(texel*255+0.5)`. Each interior channel computes `old + floor((incoming-old)*alpha/128)` using signed integer arithmetic. For negative products, `-((-value+127)/128)` explicitly compensates for division toward zero. Products are bounded by 32640, so the intermediate negation/addition is well within signed32 range. With alpha in0..128, the result stays in byte range. Output normalization occurs only at the RGBA8 write boundary.

Interior alpha is the raw source alpha byte0..128 divided by255, not a normalized blend fraction. Alpha0 and alpha128 both execute real draws, preserving their required alpha and border behavior. No endpoint aliases a whole-old or whole-incoming publication.

The public descriptor retains the logical source XYZ, fixed16 UV, TEX0 extent/pitch, TEX1, RGB-only texture mode, modulation and clamp facts. The shader implements their already approved documented analytic consequence. This is not newly claimed evidence about undocumented silicon DDA precision.

## 4. GL state and normal-path isolation

All new allocation, shader compilation/linking, empty-VAO creation and `glDrawArrays` lookup occur only when the new begin API is explicitly called. Normal renderer creation gains no shader, allocation or required loader call. Existing ordinary source submission, publication, material-history acknowledgment, host menu/race publication and source-owner retirement are unchanged.

Samples explicitly select the independent effect FBO, fixed viewport, full color writes, polygon fill, and lower-left clip convention when the existing clip-control entry is available. They disable ordinary blending, sRGB conversion, dithering, logic operations, depth/stencil/culling/scissor, sample-related state used by the renderer, rasterizer discard and active clip distances. Each source texture unit and sampler binding is explicit. There is no framebuffer feedback path.

The restoration helper returns the renderer-owned baseline, unbinds both texture/sampler units, program, VAO, array buffer and framebuffer, and restores depth/color controls. It does not pretend to restore arbitrary foreign GL state. It does not touch an EBO while VAO0 is current and does not clear ordinary source depth. The next ordinary source path retains its real fresh color/depth clear responsibility.

No sample calls opaque publication, changes `displayedTarget` or ordinary accepted-source generation, copies the window back buffer, swaps, updates a page, samples a clock, consumes RNG, or advances audio. Ordinary committed-image readback is not relabeled as an effect result.

## 5. Receipt freshness, completion and readback

Blend and sample values contain private renderer-issued generations and weak lifetime identity. A later renderer lifetime cannot revive old values by numeric generation reuse. Begin checks serial exhaustion. Effect sample serials advance independently from ordinary source and publication generations and reject overflow.

An admitted sample attempt invalidates the prior effect generation before entering GL preflight or writing the reused effect target. Structural rejections such as invalid alpha or stale lease happen before that point and need not destroy an otherwise valid prior image. A failed attempted sample poisons the pair; no accepted sample receipt is issued. Successful sample receipts are initially incomplete and are issued only after checked draw and checked restoration.

Completion revalidates the exact current accepted sample, rejects input/output aliasing, calls checked real `glFinish`, and issues a completed value only on success. A completion failure poisons the pair and invalidates sample authority. Merely increasing a CPU generation is never treated as completion.

Effect readback requires that exact completed current sample, a full640x448x4-byte destination, and the existing PBO-safe tightly packed readback path. A later admitted sample invalidates earlier completed samples even if the later attempt fails. Readback keeps the existing partial-destination-write diagnostic. No effect readback becomes a source-frame, presentation or host-swap receipt.

## 6. Failure and retirement ownership

Every newly acquired shader, program, VAO, FBO and texture name is stored immediately in renderer-owned fields and included in `hasGlResources`. Partial creation/compilation/link/copy/restoration failures do not issue leases and cannot be overwritten by another begin. The retained-old borrow survives until explicit retirement or close.

Explicit release checks real GPU completion before invalidating successful generations, restoring/unbinding state and deleting any blend names. It stops at an unacknowledged deletion, retains unresolved names and the old borrow for retry, and reports incomplete cleanup. The existing conservative no-name-reuse requirement after ambiguous deletion is explicitly carried into the public contract. Only successful complete deletion unlocks retained old. Blend/sample serial counters are not reset by release.

Renderer close includes the blend borrow in its completion obligation, resets receipt lifetime only after successful completion, invalidates blend/sample authority, unbinds both input units through the existing source-capable cleanup path, and deletes blend resources before releasing retained/source storage. Existing wrong-thread/context, failed-close retry and destructor-termination ownership behavior remains in force.

A failed readback does not mutate the immutable inputs or effect target; it may be retried subject to current-context/error recovery and the existing destination-discard rule. A release failing before its destructive phase retains resources and the obligation to retry release/close, rather than pretending retirement succeeded.

## Verification limits and remaining joins

Only source/file reads, textual searches/diffs, Git metadata reads, hashing and writing this review were performed. No builds, tests, syntax checks, shader compilation, project parsers/importers/decoders, native/guest launches, GL probes, sanitizers, PS2 emulation or VU emulation were run. Root retains the regression slot.

The renderer-only result is intentionally narrower than complete presentation slice2: it produces private effect RGBA and readback, with no effect publication API. Future work still must provide actual source/effect publication and final incoming promotion, genuine host presentation, constructor/history/observer/fader ownership, adaptive timing, typed PCM/capture accounting, model/material/pose lifetimes, and an explicit movement-admission decision. The present source gate must not be presented as satisfying those joins.

## Final reviewed artifact pins

Final staged contents were re-read and hashed after the descriptor added PRIM/TEST/PSM and inherited ALPHA/CLAMP, sample-state setup explicitly disabled SAMPLE_ALPHA_TO_ONE/SAMPLE_MASK, and header wording distinguished admitted sample attempts from structural rejection. No additional finding resulted.

- Candidate `.cpp`: `95cabbc3e263354acd458274af27bdf82ffa6a9a31841560ef98762b9586e04e`
- Candidate `.h`: `d9bd702bd3fc942eab5a51b16896a188ec894985eb57f2904aca3f8432397ac9`
- `PATCH.diff`: `3138cd1fca4b40ed2fe98656074ca30c8ad7a0fa8fa863fb2d90c3ac585eac33`
- `REPORT.md`: `c98936d26a401b1caa8857afab339494006163837e2e0f0a5519db3f4c34ca93`

The supplied patch was compared with an independently generated textual unified diff of both final candidate/preimage pairs. Hunk alignment differs around repeated closing braces/blank lines, but the reviewed changes agree; byte-identical diff formatting is not asserted. The implementation report's scope and verification qualifications agree with this review. This comparison did not apply or execute the patch.
