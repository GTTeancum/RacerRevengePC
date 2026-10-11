# Independent source-only Rider19 shadow review

## Result

No blocking source defect found in the six-file staged candidate for the narrowly scoped original `19_shdw` admission. The candidate closes the observed unsupported `inputColorA=2` branch without widening ordinary lit/material conversion. This is a source review, not compilation, renderer validation, or evidence that the complete initialRider19 route succeeds.

Reviewed active base: `a4497ffc6ec46c955026e7261949951502d8c11d`.
Candidate root: `shadow-policy/files` alongside this report.
Patch SHA256: `4668503199fdf99d46563f0815b63b3e970c8458b2ad941e2ae91e65fa10563f`.
Review completed 2026-10-11 around 04:44 UTC.

All six active files were byte-identical to the corresponding staged preimages. Every preimage and candidate SHA256 matched `manifest.json`. Inspection used source/text reads, raw-file hashing, and independent Python standard-library byte inspection only. No active source was modified. No build, syntax check, project parser, project decoder/importer, test, gameplay, sanitizer, or guest execution was performed. The central execution freeze was respected.

## Exact original evidence

Files read under `next-rider-admission/members`:

- `rider/ui/textures/19_shdw.psm`: 2624 bytes, SHA256 `b7612d86899932fc84faba786d705c29decb6e4201a7d332d0e7cbed8d7015fa`.
- `rider/ui/textures/19_shdw.pst`: 17436 bytes, SHA256 `74182f87bba2b29cd8e27f8df2542e0e8548fbfecf0cb6b14bf7543abab1b39c`.
- `rider/ui/textures/20_shdw.psm`: 2622 bytes, SHA256 `b648d543ff05022a310db17cd7700d8a1e2452f9180fe10e741b9fb7276bd7be`.
- `rider/ui/textures/20_shdw.pst`: 17436 bytes, SHA256 `7bbcaf638b960cb335936a84f5880e5b6da1cfadb3c98e2acd22941f6952d57a`.

These match the immutable catalog entries in `src/assets/original_ui_archive_catalog50kg.inc`.

Both shadow PSMs explicitly contain:

- TotalShaderPasses=1; lowercase `diffusecompressed`.
- One map, the respective `19_shdw` or `20_shdw`, UV index0.
- ALPHA A=2, B=1, C=0, D=1. The authored spelling is `inputAlphaD`, whose existing compatibility alias resolves to `inputColorD`.
- Alpha blending enabled.
- Other relevant GS settings blank, inheriting the authentic DefaultMT/constructor values. `appStart/appstart/defaultmt.psm` explicitly supplies alphaReferenceValue=0.
- Resolved alpha test ON, GREATER, reference0, failure KEEP; fog ON; depth GEQUAL; zWriteDisabled=0, therefore depth writes ON.

The program comparison is already case-insensitive; the candidate preserves the authored program spelling and admits it through that established comparison. No source file normalization is needed.

Both PST headers independently read as revision1, flags0, kind1, mipCount1. The 256-entry raw RGBA palette starts at byte12; the only mip header at1036 is `(height128,width128,0,0)`, followed by16384 source index bytes at1052. There is no suffix, second mip, kind2/IPUM dependency, or additional texture-shape obstacle.

Both palettes have black RGB in every entry. The32 palette entries referenced by source indices, using the established CSM1 bit3/bit4 swap, have alpha values:

`0,1,4,7,9,11,13,14,16,20,21,24,26,27,31,33,34,37,38,41,44,45,47,48,51,54,57,58,60,61,65,67`.

The224 unused palette entries have alpha255. They must not be mistaken for the maximum alpha of the actual published texels. The real referenced alpha range is0..67 for both images. Shadow19 has10252 zero-alpha texels and609 texels at67; shadow20 has10531 and907 respectively. The images are distinct and must not be substituted for each other.

Raw PSG string inspection confirms the respective shadow names are present in the original19 and20 geometry. No PSG importer was run. Actual shadow geometry/projection success remains unexecuted.

## Candidate review

### Typed asset admission

`src/assets/original_ui_material50kg.cpp:822` adds a19-only wrapper. It requires the canonical19 PSM identity, pass1, DiffuseCompressed, exact texture basename and exact relevant ALPHA/ATE/FGE/ZTST/Z-write state. It also requires:

- Loaded immutable image-bank owner and an actual image at the supplied index.
- Canonical19 shadow PST identity, complete PST extent at offset0, no CEL or IPUM provenance.
- Indexed kind1, one mip,128x128 dimensions.
- Published native image dimensions and all RGBA bytes equal to that decoded original mip.

The existing common gate still enforces authored PSM origin, DefaultMT identity, enabled one-map UV0 profile, register widths, TCC/TFX, clamp/PABE/FBA/DAT restrictions, revision/flags, exact mip extent, and bounded same-texel min/mag sampler policy. It is reached with the new private `authenticatedRiderShadow` flag only from this typed wrapper.

The ordinary public lit/material overloads omit the new flag and still reject A=2. The enum is appended, preserving existing enumerator values. No source PSM, palette, image, or vertex color is changed. The result is committed only after all checks.

### Native conversion and dispatch

`original_ui_lit_material_adapter50km.h:90` verifies the same concrete image-bank pointer, image index, image identity and texture count, plus DiffuseCompressed, the new attenuation blend, GEQUAL, depthWrite, fog and alpha-test flags. Reusing the common converter with a temporary sourceAlpha raster policy only obtains the existing sampler/depth/scale validation. The actual emitted span is restored to the distinct attenuation blend and alphaScale1. Output remains unchanged on rejection.

`original_ui_rider_draw_contribution50kg.cpp:49` routes the19 shadow basename through that typed wrapper and converter; basename alone cannot grant admission. The wrapper still authenticates canonical PSM and PST identities. Existing generic, IPUM, body, window and specular admissions remain unchanged. The20 shadow is intentionally not admitted by this patch.

The unchanged common material and untextured converters keep their default rejection for unknown/special blend enums. Direct ordinary lit conversion of the new policy therefore remains closed.

### Renderer and arithmetic

The candidate renderer separates `specularBlend` from `shadowBlend` and permits shadow only on the sealed source-RGBA route with sourceDiffuseCompressed, GEQUAL, depthWrite, fog and alpha-test flags. Existing span validation requires alphaScale1 for the special blend, retains actual sampler/image publication validation, and still requires authentic perspective-W/fog streams. The RGB-only route rejects shadow before submission.

Shader mode5 uses:

`result = floor((-destination.rgb) * source.a / 128) + destination.rgb`

followed by existing channel clamp. This is the signed-floor form of ALPHA `(0-Cd)*As/128+Cd`. It is not interchangeable with `Cd-floor(Cd*As/128)` for fractional products: for Cd=100 and As=1, the candidate result is99 rather than100. The dispatch maps FIX255 to4 and the already-validated shadow enum to5; modes0..4 preserve their previous meanings.

The texture/vertex MODULATE and source alpha byte quantization remain before alpha testing. GREATER0 discards zero-alpha fragments, preserving destination RGB, stored alpha and depth. Every surviving fragment keeps independent source-alpha replacement; the patch does not substitute source-over alpha, preserve destination alpha, or multiply alpha by the attenuation factor.

Fog remains a source-program operation on RGB and does not alter alpha. The shadow ALPHA equation ignores source RGB, so the black texture is not a reason to replace the pass with ordinary sourceAlpha blending: ordinary blending could incorrectly incorporate fogged source RGB. Native sampling/interpolation and geometry/rasterization fidelity remain the existing route's separate limitations, not newly established by this review.

## Ordering and lifetime requirements retained

No scheduling, geometry, frame sealing, or lifetime logic changes in the candidate.

- Preserve original material registry registration/pass dependency ordering and redirected-leaf FIFO. Do not force shadows to the end or reorder them by transparency.
- The unchanged composer sorts by final registry leaf, then original packet ordinal.
- The unchanged sealed renderer issues one actual triangle per draw with a texture barrier before each subsequent source read/write draw. This preserves sequential destination color reads for overlapping triangles.
- Passing shadow fragments write reverse-Z depth because zWriteDisabled is0. Treating shadows as depth-read-only would violate this original material.
- Retain source RGBA through all later source consumers, then make opaque display/readback only on the independent presentation surface.
- Preserve source owner generation, complete-frame sealing and binding-history acknowledgment; an admitted material is not by itself a submitted or published frame.

## Scope and remaining verification

No additional shadow-specific source blocker was found for initialRider19 in the reviewed material/admission/renderer path. The reviewed six-file candidate is suitable for the parent's next authorized frozen-stage integration and central verification, subject to the existing execution schedule.

This review does not authorize or report a test run. It does not establish end-to-end geometry, lighting, clipping, binding schedule, GPU shader compilation, actual renderer behavior, or initialRider19 admission success. Those remain for the already-planned central run no earlier than05:19:42 UTC.

`20_shdw` has matching state/layout evidence but remains unsupported deliberately. This patch must not be described as Rider20 support; other Rider20 admissions were outside this narrow review.

## Final explicit fail-closed review

Re-read the complete candidate validator, shader branch and C++ dispatch after the parent's specific request:

1. **Unknown blend enum values reject before any draw.** `specularBlend` is true only for destinationAlphaAdditive or fixed255Additive, and `shadowBlend` only for destinationSourceAlphaAttenuation. The following validation condition rejects every enum value other than opaque, sourceAlpha, additive or those three explicitly recognized special values. It is evaluated for every span before submission setup, framebuffer clear, uniform dispatch or draw calls. Consequently the dispatch's terminal `:5` can receive only the validated attenuation enum; it does not create a permissive unknown-enum route.
2. **Ordinary RGB-only submission rejects attenuation before drawing.** `submitOriginalUi(native,...)` invokes the private implementation with sourceRgba=false. Its `shadowBlend && (!sourceRgba || ...)` condition immediately returns a material error. This is independent of the generic converter's own default rejection and remains true even for a manually assembled otherwise-valid native span.
3. **Clamp and alpha replacement are unchanged.** Source MODULATE produces `floor(clamp(...,0,255))` for all four channels. After the alpha test, only RGB enters ALPHA arithmetic; its final result is clamped0..255. Shader output alpha is exactly the already-clamped/quantized source.a divided by255. Source-RGBA draws disable fixed-function blending and enable source alpha writes, so surviving fragments replace stored alpha; they do not accumulate source-over alpha or preserve destination alpha. Discarded fragments replace nothing.
4. **Signed negative flooring matches the existing source-RGBA arithmetic contract.** Mode5 changes the difference term to `-destination.rgb` and uses the same existing `floor(difference*coefficient/128.0)+destination.rgb` operation. The source.a coefficient remains unmodified. This retains the existing signed floor behavior already used when ordinary sourceAlpha has source.rgb below destination.rgb. It does not change to truncation toward zero or to subtraction of a floored positive product.

No blocking finding arose from this final explicit review. This conclusion is based solely on source control-flow inspection, without execution.
