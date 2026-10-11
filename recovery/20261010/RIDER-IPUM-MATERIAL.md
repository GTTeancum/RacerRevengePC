# Typed Rider19 IPUM material and native base-image policy

Frozen source-only overlay, 2026-10-10. Three replacement candidates and exact
preimages are under files/preimages; based on the corrected Rider renderer
extension integrated by parent as ff80b58. No active edits, builds, tests,
syntax checks, project parsers, image conversions, shaders or guest execution.

## Explicit native policy

Parent authorized using the actual authenticated published highest-resolution
original image, retaining every original mip/provenance on CPU, and documenting
that the current GPU bank publishes base only. This overlay implements that
policy; it never manufactures a historical source-residency snapshot.

prepareOriginalUiRider19IpumMaterial50KM consumes the REAL immutable
OriginalUiScreenAssets50KG and bank index. It requires:

- Canonical Rider19c_1.pst source identity, exact whole-member range, no CEL;
  the source SHA matches the typed cache's copied provenance.
- Successful cache-derived colorPolicyKind original50FcRgb5Texa255Aem1.
  Only the cache owner can publish this result from compile-pinned decoded
  images and the authenticated original asset. An unavailable catalog cannot
  load that cache; no fallback image or general kind2 parser is enabled.
- Actual largest decoded mip RGBA/dimensions exactly equal the CPU bank image
  used by the existing authenticated MaterialTextureGl50EB publication.
- Canonical19c_1.psm, original pass1, DiffuseCompressed, TextureMap19c_1,
  opaque blending and alpha test OFF, and all existing strict lit GS gates.
- MXL0 and matching original min/mag texel filters. Missing GPU mip levels
  cannot be sampled; any MXL requiring them remains rejected.

The typed result explicitly records highestResolutionBaseOnly, the exact
bank owner/index/image identity and stored mip4 of5. All5 original mips remain
on CPU. The dedicated native converter checks that same owner/index/identity
and the bounded policy before producing the DiffuseCompressed span. Prepare
again after real combined-bank remapping; never relabel a policy from another
bank. Existing renderer CPU/GPU bank validation and immutable sampler checks
still apply. Existing ordinary lit(pass,Texture48E) continues to reject kind2
and multi-mip images; no copied provenance string alone grants admission.

## Source and approximation boundaries

The source stores PST mips in ascending size.24EAD4 starts descriptor index
count-1,24EB88 decrements it; last stored largest image is descriptor0. Actual
19c_1 has16/32/64/128/256 square mips. Material247F50 defaults MXL0,247F88 reads
its authored override and248008/248034 writes TEX1; 19c_1 leaves the field blank.
Default min5/mag1 have the same linear texel filter when native base level is
explicitly selected. Original texture24E790 binds descriptor selected byF4;
24E670 adjusts it through a priority/residency path. This native policy does
not claim historicalF4=0 or recreate PS2 memory pressure.

A separate source nuance is retained: material247960..2479EC writes TEXA from
DefaultMT/pass fields, whereas original kind2 binding2421A4..2421FC prepares
TA0=TA1=255/AEM1. Final ordering for all source paths is not claimed here.
The bounded real19c_1 pass has blendingOFF/ATEOFF, and native presentation still
requires no destination-alpha or downstream stored-alpha consumer. Therefore
that unresolved alpha ordering cannot affect RGB on this admitted path.
Transparent kind2, cutout kind2, destination-alpha specular, other assets and
mip-dependent sampler cases remain unsupported, not silently approximated.

Codec RGB5/TEXA policy is original-data-derived native conversion, not measured
hardware inverse-DCT pixel parity. This overlay makes no stronger claim than
the cache provenance and does not run a converter or change any image bytes.

## Dependencies

- Corrected rider-render-extension e58b01bd manifest, integrated ff80b58.
- ipum-texture-reconstruction cache and ScreenAssets APIs, including copied
  provenance.colorPolicyKind. Cache header3653025aa0c6e08b8dbb1b5bb5e1106bb745583f4c23488fa0217593b5e2cef8;
  implementation01de35141ab9b6826537465d352ba7c28a4a397611d0b7fcf84fe201284942e3.
- Actual compiled output catalog and image cache must later be produced and
  authenticated by parent's permitted native original-data conversion.

This closes material policy for that real decoded preview image once available.
It does not assert the UI widget has registered, the cache was generated, a GL
bank was uploaded, a frame was submitted or any execution check passed.
