# Optional named image copy for racer coverage snapshots

UNTESTED source-only proposal. Vehicle source base eb9aaa7d8e651acd209c3432383c0e399e89533c becomes e18555a25946f22005060070022cb0f388a3796b. Coverage source base 8363323bb319350de773485e36c414b2da9c4254 becomes 0a2c1215c01956409634ead3dbe00eb913a68ae4. No builds, syntax checks, tests, benchmarks, or gameplay were performed.

## Admission and ABI

The already checked actual desktop context version reaches checkCoverageSupport. That helper first clears the optional pointer. Only coverage-demand contexts reporting desktop GL 4.3 or newer attempt to resolve glCopyImageSubData. A nonnull proc and the verified version are both required; no pointer-only admission occurs. A null optional proc leaves the established blit path enabled without invoking the mandatory load failure helper. A resolution error still rejects through a new error check. Existing required coverage entry points and storage limits remain required and checked.

The new function pointer has all fifteen GL parameters with the existing fixed-width unsigned/signed 32-bit aliases. The Windows alias explicitly uses __stdcall, matching APIENTRY; the non-Windows alias uses the ordinary calling convention. The repository's native_dynamic_library50win.h explicitly admits only Windows AMD64; window_host50dq.cpp verifies eight-byte pointers and resolves GL functions through SDL_GL_GetProcAddress. No wider Windows support is claimed.

Primary ABI reference: [Khronos glcorearb.h, PFNGLCOPYIMAGESUBDATAPROC](https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/api/GL/glcorearb.h).

## Exact transfer scope

Only drawCoveragePairTriangle changes its snapshot-copy command sequence. On the admitted path it copies coverageLive.color[bank] to the distinct coverageLive.snapshotColor using TEXTURE_2D_MULTISAMPLE, level zero, identical region origins, z zero, the region width/height, and depth one. The other path retains the original read-FBO bind, draw-FBO bind, and blit verbatim.

Creation independently allocates both textures and queries their actual RGBA8 format, four samples, fixed sample locations, and identical extent. They are not texture views and share no storage. Region construction stays bounded to that extent; empty regions still return before the copy. No format conversion, resolve, scaling, filtering, or source/destination overlap is introduced.

The source-pair shader, conservative region admission, disjoint-run grouping, source triangle order, map-2 alpha replacement, and composition are unchanged. The RA-world snapshot helper and final color resolve continue using their original blits.

## Scissor, ordering, and cleanup

The racer bridge's RA scissor equals the copied rectangle; its non-RA path disables scissor. The existing framebuffer blit therefore has the same texel region as the named copy. Snapshot contents outside that region remain unconsumed by this bridge's unchanged raster/composition logic.

The payload framebuffer remains bound during named copying. The existing next bind establishes the scene framebuffer before composition. The original copy error gate remains after either route. Any copy error rejects the bridge; there is no runtime retry through blit and no error swallowing. The original failure tail restores scene bindings, and every feedback unbind, program/VAO/sample/depth/scissor restoration and error gate remains.

## Specification support and limits

[ARB_copy_image](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_copy_image.txt) describes conversion-free image copies and requires compatible formats and equal sample counts.

[OpenGL core specification](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf), sections 18.3-18.3.2, distinguishes raw image copies from filtered/resolved blits, forbids overlapping aliased copies, and describes equal-sample blits as preserving samples. Per-sample identity here follows the raw image-copy semantics; no additional explicit sample-index sentence is claimed. Section 2.1 provides ordered same-context command behavior. This sequence uses ordinary framebuffer writes and later texture sampling, not incoherent imageStore or concurrent contexts; it introduces no new memory-barrier requirement.

The named route removes two framebuffer bind calls per executed racer bridge. It retains one destination copy, the same transfer region/volume, both raster/composition draws, and all checks. The cited 5,181-bridge frame would correspond to 10,362 fewer bind calls if this route were admitted, not a measured reduction in elapsed time. Driver behavior and performance remain unvalidated.
