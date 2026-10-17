# Optional named image copy for RA world snapshots

UNTESTED source-only proposal. RA source base 2744d11908c39d63a74242519eacf66193e01b94 becomes c9769d3408fbc7119c348ebad7e62c17b5b8753d. This depends on the previously accepted optional pointer and capability admission in vehicle e18555a25946f22005060070022cb0f388a3796b and coverage storage 0a2c1215c01956409634ead3dbe00eb913a68ae4. No builds, syntax checks, tests, benchmarks, or gameplay were performed.

## Helper-local change and admission

Only snapshotRaWorld changes. The nonnull optional pointer selects CopyImageSubData from coverageLive.color[bank] to coverageLive.snapshotColor. Both targets are TEXTURE_2D_MULTISAMPLE, level zero, z zero, with identical region origins, region width/height and depth one. The original read-FBO bind/read selector, draw-FBO bind/draw selector and blit remain in the else branch, with only indentation changed. The original common error check and diagnostic text remain unchanged.

The shared admission already requires an actual checked desktop GL version of at least 4.3 and a nonnull optional proc. Lower versions and missing procs retain blit. No additional loader, capability policy, ABI change or runtime-error retry is introduced here.

## Image and rectangle proof

Shared createCoverageExtent independently allocates the scene color and snapshot color as RGBA8, four-sample textures with fixed sample locations and identical dimensions. coverageTextureStorage queries and verifies actual format, sample count, fixed locations and extent. Both are distinct allocations without texture views or aliased storage.

snapshotRaWorld has one caller in drawRaWorld, only for ordered-current-destination blended primitives. raTriangleRasterRegion returns either a contained conservative rectangle or the full target; the caller intersects it with the selected draw scissor, skips empty results and establishes that exact rectangle as the enabled scissor before copying. CopyImageSubData does not apply scissor, but writes the same rectangle as the original scissored blit. Samples outside it remain outside the unchanged primitive/composition scissor.

## RA-specific consumers and restoration

Snapshot capture still occurs before the next original blended primitive. No depth, payload or fog texture is copied. No visibility, topology, material, alpha-test, source-byte fog arithmetic, depth function/mask or draw ordering changes.

For blended AA0, the next target bind explicitly selects the raster framebuffer, retains its two payload/depth outputs, clears the alpha sentinel, and executes the unchanged source material. The existing subsequent scene bind precedes per-sample composition using the separate payload, center depth and destination snapshot.

For blended AA1, the next target bind explicitly selects the scene framebuffer before the original source material samples the distinct snapshot through destinationImage at gl_SampleID. The snapshot is not attached to the scene framebuffer.

Neither success path relies on the old copy helper's framebuffer binding. Private scene/snapshot selectors are initialized to COLOR_ATTACHMENT0 and retained; named copying does not read or modify them. The caller preserves all subsequent explicit target and selector setup.

A failed copy returns through the unchanged check; the caller sets accepted=false and breaks. Its unconditional tail unbinds the sampled textures, binds the scene framebuffer, restores read/draw COLOR_ATTACHMENT0, disables scissor, resets pod state and checks restoration. The failed primitive does not acquire success accounting. No blit retry follows a failed named copy.

## Specification basis and limits

[ARB_copy_image](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_copy_image.txt) describes conversion-free image copying and requires compatible formats and equal sample counts. [OpenGL core specification](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf), sections 18.3-18.3.2, distinguishes raw image copies from filtering/resolving blits and forbids overlapping aliased copies. Per-sample identity here follows raw image-copy semantics; no extra explicit sample-index sentence is claimed. Section 2.1 supports ordered same-context command behavior. These are ordinary framebuffer writes, copying and later texture sampling, not incoherent imageStore or concurrent contexts; no new memory barrier is required.

The admitted path removes two framebuffer binds and two selector calls per executed RA snapshot. Copy count, rectangle and bytes remain unchanged. Actual RA snapshot incidence and performance have not been measured. This is a source-semantic preservation argument, not a tested driver correctness or speed claim.
