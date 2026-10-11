# Independent renderer source review

Reviewed 2026-10-11 UTC. Result: no blocking semantic defect found in the staged candidate. This is a source-only review, not build, execution, image-equivalence, or performance validation.

## Exact reviewed inputs

Relative source: `platform/linux/native/vehicle_window_gl50ds.cpp`

- Preimage SHA-256: `7d3ec414d71325891c04e6302b06697d3b4b65fe0f9d70b21407a97e2de64f0d`
- Candidate SHA-256: `fdcb8fe450362d6f39d64ff48aed50cebf237f852abc87c4e8b331f8976df0b1`
- Preimage location: `preimages/platform/linux/native/vehicle_window_gl50ds.cpp`
- Candidate location: `files/platform/linux/native/vehicle_window_gl50ds.cpp`

The read-only source checkout was `/workspace/shared/rr-recovery-git`. Its renderer matched the preimage hash above when inspected. No active source was edited by this reviewer.

## Evidence for the bounded improvement

Run 03's existing `rider-capture-sequence/rider-validation-03/game.log` records an accepted actual Rider opening frame at native frame/update 34 with 16,140 source quad slots, 15,690 source triangles, 15,690 native draw calls, source frame/target generation 2, attached rider 19, and Song7. The next active update aborts with the existing one-second stall gate. This supports reducing native submission overhead as a concrete candidate; it does not isolate the total frame cost or establish that the patch clears the gate.

The old source path performed textureBarrier, drawElementsBaseVertex, and glGetError for each surviving triangle, including opaque spans whose fragment-shader branch does not read the destination. Actual model packets cover a whole surface/pass. Model projection in `src/game/original_ui_lit_geometry50km.cpp:306-309` expands each source triangle as [a,b,c,c], copying the last projected vertex, raster W, and fog verbatim. Batching eligible opaque spans therefore has a concrete path to removing many driver calls and barriers without reducing source geometry.

## Geometry and state preservation

1. The same ordered material spans remain distinct. Their depth function/write mask, sampler/image, texture policy, fog, alpha test, source alpha, shader, and raster W policy are unchanged. There is no global material sorting or feedback batching.
2. The cold index buffer is the existing repeated sequence 0,1,2,0,2,3. A contiguous opaque span draw preserves its indexed primitive order and index values. The shader does not depend on primitive ID, draw ID, or vertex-side effects.
3. The candidate keeps the original equal-XY skip predicate for source triangle accounting. For every would-be skipped triangle, batching additionally requires a corner pair equal in all three position components and effective W. Effective W is the validated parallel W stream only when sourceRaster is enabled, otherwise 1. If any skipped triangle lacks that pair, the whole span uses the original triangle selection and index offsets. This excludes same-XY/different-Z-or-W cases from the optimization.
4. The guard admits the actual [a,b,c,c] model slots. Any extra degenerate half submitted by the opaque batch has exactly repeated transformed geometry and cannot introduce a covered fragment. Color/UV/fog equality is not required for that geometric proof.
5. On guard failure, the partial pre-scan sourceTriangles value is reset before the fallback recount. sourceDraws remains zero until a draw is actually issued. An all-skipped opaque span issues no draw, as before, while still checking its GL state at the span boundary.
6. The 17,408-slot source cap limits a contiguous draw to 104,448 indices; the existing signed GL count conversion remains bounded. The UInt32 index buffer remains unchanged.

## Feedback and error behavior

- Texture unit 1 is explicitly unbound before opaque submissions and rebound to the source color attachment for feedback spans. Unit 0 is restored before drawing. The existing end-of-frame unbind and state restoration remain.
- Every surviving feedback triangle still receives one barrier immediately before its draw, in the original order. This includes the first feedback triangle after preceding opaque writes. No non-overlap or coverage assumption is introduced.
- Primary specification: https://registry.khronos.org/OpenGL/extensions/ARB/ARB_texture_barrier.txt, Rendering Feedback Loops section, lines 63-79. The specification requires separating writes and later texel reads with TextureBarrier unless another stated exception applies. This candidate retains that dependency for every feedback draw.
- No source-path operation consumes GL errors between a span's state setup/draws and its final `s.check`. Errors remain detectable at that boundary. A rejected span commits none of its local draw, triangle, or quad counters. The texture-barrier diagnostic continues counting attempted calls, consistent with the preimage.
- Span-level polling can issue the rest of the bounded span after an earlier GL error and changes failure attribution from triangle-level to span-level. It does not make such a frame acceptable.
- `sourceWorkStarted` is still set before the first material state change. The existing `finishSourceSubmission` poisons pending history when source work started but `originalUiAccepted` is false. The separate opaque-presentation-surface copy and restoration checks remain required before private publication; the existing later window-copy failure semantics are unchanged. Failed or partially executed source work cannot commit source history through this change.

## Validation boundary

Only ordinary source/log reads, text diff, hashing, and this report write were performed. No test, build, compiler, syntax checker, project script, parser/importer/decoder, native launch, or guest launch was run. The runtime effect, image equivalence, and whether the unmodified one-second gate is satisfied remain unverified. Any subsequent source change invalidates the candidate hash covered by this review.
