# Coherent framebuffer-fetch shader and ordering review

Review date: 2026-10-11. Source-only review; no builds, tests, syntax checks, project parsers, decoders, importers, helper probes, or native/guest launches were performed. The active checkout was read only.

## Verdict

No shader, capability, primitive-order, or preserved-state blocker was found in the candidate identified below. The coherent branch is suitable for the already planned normal-context validation, subject to the validation limits here. This is not a claim that the candidate has compiled, that the live context advertises the extension, that performance improved, or that a full rendered frame is empirically byte-identical.

The exact extension gate is appropriate for the renderer's existing desktop GL3.2+/GLSL150, linear single-sample RGBA8 scope. No normative destination-conversion gap requiring an additional vendor/version gate was found. The justification combines the extension with the base GL normalized-conversion and precision rules; it must not rely on the extension's endpoint sentence in isolation. Stronger same-helper evidence is specific to inspected Debian Mesa 25.0.7-2+deb13u1 llvmpipe. None of these findings constitutes a universal bitwise proof of complete rendering or generated machine code.

## Exact scope and identity

Active baseline HEAD: `1b413008a52de584aff48d19f06b7e8de82dd6bc`.

Baseline renderer SHA-256: `c30fa4f3553bb66c187a3ae923936e9b2069d5ae44743e404ad9ec8b86ab6a9e`.

Reviewed candidate files, all below `files/` in this directory:

- `platform/linux/native/vehicle_window_gl50ds.cpp`: `d4173271ac67d9fd14b8e8feacf6640d40bbeadb98dbdf51a35d8a96ec79663d`
- `platform/linux/native/vehicle_window_gl50ds.h`: `a7207d67995f6f2f4c473fcd5fc751266b54608890ce3b43b04ca12a8ac196e0`
- `tests/native/original_ui_main_entry50kg.h`: `e1577badad3b327b32692e1d0817db93cc314d20a4dd3d8abbb1e3f4ef3d62c2`
- `tests/native/terrain_interactive_drive50du.cpp`: `66429d6ed9953658d224d83d92bd73d304184f510683011118a50ea89336a532`

The renderer preimage matches the active file. Review covers the complete renderer diff and the accompanying diagnostic/receipt diffs. No active files were changed by this review.

## Capability, GLSL, and linking

- `checkOriginalUiSourceSupport` runs in ordinary renderer initialization before `createOriginalUi`. It checks the current context's bounded extension enumeration and uses exact `strcmp` for `GL_EXT_shader_framebuffer_fetch`. The similarly named noncoherent extension cannot select batching.
- The coherent shader has `#version 150 core` first, followed by `#extension GL_EXT_shader_framebuffer_fetch : require`, then its feature macro. The ordinary shader receives the original version prefix without that requirement. No noncoherent qualifier is introduced.
- Desktop OpenGL 3.0+ adopts the extension's modern `inout` fragment-output model. Exact Mesa `ast_to_hir.cpp` lines 4227–4258 independently shows desktop GLSL >=130 accepting framebuffer-fetch outputs and requiring the coherent extension for this declaration. GLSL 150 is therefore an appropriate source version, although no compiler was run.
- `viewerOutput` remains bound to `vehicleGlColorOutput50CZ`, whose value is zero, before link. The source framebuffer still draws color attachment zero. No unsupported GLSL explicit-location qualifier is required.
- The coherent preprocessing branch removes the destination sampler declaration. Its uniform query, required-location check, and uniform update are also bypassed. The existing remaining uniforms are retained. No optimized-out destination sampler is mistakenly treated as a link failure.
- An advertised extension followed by a compile/link failure fails renderer initialization through the existing diagnostic path; it does not silently claim a working coherent path. Absent coherent support, the existing texture-barrier support check, function load, sampler, and per-triangle barrier path remain.

## Destination read and arithmetic

The only source-composition expression replaced is the input to the existing destination byte reconstruction: incoming `viewerOutput` replaces the same-pixel `texelFetch`. Both keep `floor(value * 255.0 + 0.5)`.

The destination is read before any assignment to `viewerOutput`. Vertex-byte conversion, source texture sampling, source-alpha rejection, fog, all five coefficient/difference selections, signed-product floor, destination addition, clamping, and source-alpha replacement are unchanged. In particular:

- Source-alpha rejection precedes feedback and rejects without a color or depth write.
- Fog changes source RGB before blending and leaves the source alpha used by applicable modes intact.
- Destination-alpha and fixed-255 coefficients retain their original meanings.
- Shadow subtraction retains the signed floor before adding destination RGB.
- Every surviving fragment writes raw source alpha; no source-over alpha accumulation is introduced.

### Backend-specific RGBA8 conversion evidence

The exact distribution sources were read directly from Debian's raw source service:

1. `lp_state_fs.c`, `fs_fb_fetch`, lines 541–669: select current color-buffer pointer and format and call `lp_build_fetch_rgba_soa`.
2. `lp_bld_sample_soa.c`, `lp_build_fetch_texel`, lines 2907–3084: the normal integer-coordinate texel-fetch path calls that same helper. The `LP_SAMPLER_OP_FETCH` dispatch at lines 3537–3539 selects this path, rather than the alternative filtered AoS sampler.
3. `lp_bld_format_soa.c`, `lp_build_fetch_rgba_soa`: ordinary 32-bit RGBA8 uses packed gathering followed by `lp_build_unpack_rgba_soa`; unsigned normalized components reach `lp_build_unsigned_norm_to_float`.
4. `lp_bld_conv.c`, lines 395–428: 8-bit input uses integer-to-float conversion and multiplication by the floating representation of `1/255`.

Thus both destination mechanisms use the same normalized unpack operation for the same RGBA8 bytes in the inspected implementation. The preserved nearest-byte reconstruction also has a large margin relative to float32 rounding error for the 0–255 input domain. This is a source-level argument, not an executed exhaustive conversion test or an audit of generated machine code.

### Portable normalized-value contract and gate decision

The extension's endpoint wording must be read with desktop GL's general conversion rules. GL3.2 core section 2.1.5 defines unsigned-normalized conversion as `c/(2^b-1)` and explicitly includes framebuffer components in its definition of `b`. GL4.5 section 2.3.5.1 repeats that rule; its blending section also pairs the equation with endpoint-preservation wording. The endpoint sentence does not permit arbitrary nonlinear mappings of intermediate RGBA8 bytes.

For an 8-bit stored value `n`, both paths therefore target `n/255`. Reconstructing `n` requires the scaled error, including subsequent arithmetic, to remain below half a byte. At the base GL precision scale of about one part in 100000, normalization, multiplication by 255, and addition of 0.5 together have an error scale below 0.008 byte, comfortably inside that margin. GLSL150 section 4.1.4 requires the established GL precision guidelines; section 4.5 says desktop precision qualifiers do not reduce precision. This is a rounding-margin argument, not an assertion that two returned floating bit patterns must be identical.

Decision: retain exact-extension selection for the documented desktop/single-sample/linear-RGBA8 scope. No concrete conforming alternate normalization that changes the reconstructed byte was established, so a mandatory llvmpipe-only gate is not justified by this review. A backend allowlist would be an additional deployment-evidence policy rather than a remedy for a demonstrated specification gap. Do not extend this conclusion to other attachment formats, sRGB, multisampling, lower-precision ES shaders, or whole-frame cross-program invariance.

## Order, raster state, and helper invocations

The coherent extension guarantees earlier overlapping samples in API primitive order, including overlap within a draw. The candidate submits each existing contiguous packed index range as `GL_TRIANGLES`, with the same first index, count, base vertex, and span order. It changes neither the source index builder nor admitted triangles. Material, depth, fog, source texture, and alpha-test boundaries still remain separate spans.

There is no ordinary destination-sampler binding on the coherent source path. Consequently its removed texture barriers are not relied on for texture feedback. Opaque source spans continue to avoid the destination-dependent shader branch. Their index data and source arithmetic are unchanged.

The source attachment remains a checked 640x448 RGBA8 `GL_TEXTURE_2D`, with ordinary single-sample D24 storage. Other UI targets also retain their checked zero-sample attachments. Source submission still disables fixed-function blending, framebuffer sRGB, dithering, multisampling, and inherited color logic operations as before. It preserves depth function/mask, viewport, scissor, winding, and all vertex inputs. No sRGB conversion or multisample resolve has been introduced.

Framebuffer-fetch values for helper invocations can be undefined. Here, source UVs, implicit texture derivatives, source-alpha decisions, fog, and geometry do not depend on the destination read. The destination feeds only local RGB arithmetic and the fragment's output. There are no destination-dependent derivatives or side effects that transport a helper value into another fragment.

No early-fragment-test qualifier or explicit depth write was added. In the inspected Mesa `generate_fs_loop`, lines 734–765, `uses_fbfetch_output` prevents inferred early depth and selects late depth/write. This preserves ordinary discard-before-depth-write behavior. It can shade more depth-rejected fragments, a performance caveat rather than a change to the intended color/depth result.

## Existing RGB/UI path and receipts

Without the source-RGBA opt-in, capability selection remains false and the ordinary shader and submission paths remain in use. If ordinary RGB submission occurs on an owner initialized with the coherent shader, its `uiSourceComposition == 0` branches still assign their full output before the later RGB-only fog update; they do not consume incoming destination color. Untextured RGB, conventional blending, alpha masks, and independent presentation-alpha handling are unchanged.

The diagnostic additions report the selected path and actual existing draw/barrier counters. They do not turn source submission or capture failure into success. Source-generation, source-work-start, per-span error boundary, and publication behavior are unchanged by the reviewed diff.

## Remaining validation limits

- The real extension query and shader compile/link must occur through the planned normal launch. No probe was performed by this reviewer.
- The source-equivalence argument does not establish whole-frame equality across driver compiler changes, shader specialization, interpolation, or implicit texture-LOD behavior. Authorized runtime evidence should compare full source RGBA and relevant overlap/depth/alpha cases, not only opaque presentation RGB.
- The draw/barrier reduction follows from the code, but actual speedup, first-use compilation cost, and warm submission time are unmeasured.

## Primary sources

- https://registry.khronos.org/OpenGL/extensions/EXT/EXT_shader_framebuffer_fetch.txt
- https://registry.khronos.org/OpenGL/specs/gl/glspec32.core.pdf
- https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf
- https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.1.50.pdf
- https://sources.debian.org/data/main/m/mesa/25.0.7-2%2Bdeb13u1/src/compiler/glsl/ast_to_hir.cpp
- https://sources.debian.org/data/main/m/mesa/25.0.7-2%2Bdeb13u1/src/gallium/drivers/llvmpipe/lp_state_fs.c
- https://sources.debian.org/data/main/m/mesa/25.0.7-2%2Bdeb13u1/src/gallium/auxiliary/gallivm/lp_bld_sample_soa.c
- https://sources.debian.org/data/main/m/mesa/25.0.7-2%2Bdeb13u1/src/gallium/auxiliary/gallivm/lp_bld_format_soa.c
- https://sources.debian.org/data/main/m/mesa/25.0.7-2%2Bdeb13u1/src/gallium/auxiliary/gallivm/lp_bld_conv.c
