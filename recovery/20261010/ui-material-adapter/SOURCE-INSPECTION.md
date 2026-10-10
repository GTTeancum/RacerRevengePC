# Source-only UI material adapter inspection

This is NEW reconstruction source, not recovered historical bytes. It has not
been integrated into any checkout or frontend. No tests, builds, compiler/syntax
checks, lint, imports, gameplay, PC access or Git operations were performed.

## Deliverable/API

`files/platform/linux/native/original_ui_material_adapter50kg.h` provides inline
`rr::platform::convertOriginalUiMaterialSpan50KG(policy, binding, output)`.
The binding supplies first quad, span quad count, total draw quad count, texture
index and total texture count. Successful output contains `span` of type
`OriginalUiSpan50KG` and `native` of type `OriginalUiNativeSpan50KG`. Copy those
members into matching ordered draw arrays; this API does not construct a draw.
It returns `false` without modifying output on any rejected input.

## Inspected dependencies

- Baseline `platform/linux/native/material_texture_gl50eb.h` in
  `/workspace/scratch/74cd1d3d7a45/remote-recovery-51c6e707-20261010T1230/source`:
  native filter/wrap enums start with unspecified; sampler fields are
  minification, magnification, s and t.
- Baseline `src/assets/material_texture_assets50ea.h`: maximum bank count 4096,
  unresolved texture index 0xffffffff.
- `ui-exact/assets-author/files/src/assets/original_ui_material50kg.h/.cpp`:
  admission requires matching min/mag, context zero, exact modulation/alpha
  normalization, maximum raw vertex alpha 128, and derives depth/alpha flags.
- `ui-exact/coordinator/transcript-complete/files/platform/linux/native/original_ui_draw50kg.h`:
  8192 maximum quads; opaque span alpha scale is unity in native presentation.
- `ui-renderer-reconstruction/files/platform/linux/native/original_ui_native_draw50kg.h`
  and `vehicle_window_gl50ds.cpp`: depth/sampler typed state and explicit opaque
  presentation-alpha prerequisite.

## Source inspection findings

- Every source blend, filter, wrap and depth value is explicitly switch-mapped.
  Invalid enums reject. Source depth 0 maps to native never, never unspecified.
- Nonempty span and draw counts are bounded. Subtraction after checking the
  first-quad bound avoids overflow; texture index must be within bounded count.
- Source color, stored alpha, blend alpha and alpha-store conversion scales
  must match admission exactly; these comparisons also reject NaN/infinities.
  Color scale and depth-write are copied unchanged. Nonopaque blend-alpha scale
  is copied unchanged. Opaque alpha scale is 1 as required by the recovered span
  API, without claiming any source framebuffer-alpha storage.
- Matching min/mag, context zero, alpha ceiling, clamp requirement, derived
  depth-owner flag and blend-specific separate-alpha-store flag are enforced.
- All mutations before success affect a local temporary. Output is assigned
  only once, after every check succeeds. There is no allocation or external IO.

## Caller obligations and limits

Policy must already have been admitted by
`assets::prepareOriginalUiNativeMaterial50KG` for the exact source material and
atlas. A well-formed struct does not prove admission or provenance. This adapter
cannot authenticate texture/material/depth ownership, verify live publication,
resolve pass groups or animation state, validate actual vertex/texture bytes,
certify cross-span partition/order, prove absence of source framebuffer-alpha
consumers, or enable frontend execution. All remain caller/renderer duties.
In particular it does not set `opaquePresentationAlphaOnly` or replace its
independent proof. No claim of runnable or gameplay-correct integration is made.
