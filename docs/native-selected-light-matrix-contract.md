# Native selected-light matrix admission

Source-only repair, 2026-10-13, based on `2c2d2d9f9b8cc2a5d83ade2203c9e7c065a7d881`.
No build, test, syntax/lint/sanitizer check, runtime decoder, renderer, gameplay,
emulator or guest execution was performed for this repair.

## Observed boundary and established cause

The parent-reported Windows Main/Rider19/Course-to-TA smoke at 03:48:03–03:48:17
UTC reached session 2, tick 1, then rejected component 0 with:

    native GC selected matrices exceed finite admitted color/direction bounds

The selected TA owner reported 17 authored spotlights. All eight original-AI
owners were initialized/bound/active and had one native tick; there were no
controller or host-driver calls. The rejected frame was not swapped and no track
or FPS result was established. The old diagnostic did not record a matrix, lane,
value or selected-light ordinal. Consequently the precise runtime predicate
that fired remains unknown.

Source inspection independently establishes that the renderer's shared
`0 <= diffuse/specular <= 127.5` rule is incompatible with admitted producers:

1. TA local negative lights retain signed specular coefficients after the
   required diffuse-only fold. Their negative specular must not be rejected,
   made positive, clamped away, or folded a second time.
2. Selected colors are floating-point coefficients in source color units, not
   packed bytes. MA's authentic direct sun overrides include `(220,210,190)`
   and `(150,155,160)`, all above the old 127.5 cap. Spatial selection also
   divides local colors by attenuation, rather than quantizing them to bytes.

This corrects a demonstrated producer/consumer contract mismatch. It does not
prove that the next smoke passes, nor identify an unlogged historical lane as
fact. No change to world/root identity, geometry, camera, matrix convention,
light eligibility, normalization or producer values is justified by this error.

## Original-source evidence

Original executable SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

- `146FDC..147014` divides authored Light::Init RGB by two. This explains the
  base source units, not a universal range after later light operations.
- `14A498..14A4A8` forms reciprocal selection attenuation. `14A560..14A574`
  scales source diffuse `Light+3C`; `14A580..14A590` separately scales source
  specular `Light+4C`. Both remain signed floating-point values.
- `14A704` calls `149A50` with the selected diffuse matrix (`s3`) and optional
  selected sun row (`s1`). The specular matrix (`s2`) is not passed to the fold.
  `149AB8..149ADC` accumulates/clears negative diffuse RGB. `149B18..149B54`
  adjusts sun with 0.75 of that sum; `149B5C..149B90` adjusts ambient with 0.5.
  Specular and direction matrices remain unchanged.
- `1498A0` directly copies RGB into the per-zone sun table at manager+44.
  There is no additional division by two or upper 127.5 saturation there.

The current native `selectAuthored` preserves the relevant signed-color
behavior: selected local RGB is copied to both diffuse and specular rows, then
`foldSelectedNegativeLights` updates diffuse only. See
[the original numerical reconstruction](original-racer-lighting50km.md) and
[the deliberately native TA selection policy](native-ta-selected-lighting.md).

TA SCB SHA-256 is
`c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d`.
Its Light13 negative RGB text begins at byte 12281; attenuation text at 12302 is
`1.0,0.000001,0.000002`. Light26's negative RGB begins at byte 18071, with the
same attenuation at 18092. These are genuine negative source values, not a
matrix transpose or transform-scale artifact.

MA SCB SHA-256 is
`5766af3e7a0bba0644058ae6f31eab802cadd9be7cc71911c349f603c7ca873b`.
The zone-1 direct override is `220,210,190` at byte 49542; zone 5 has
`150,155,160` at byte 49590. These literal offsets were read from retained
metadata and compared directly with the authentic SCB bytes, without executing
an asset decoder.

## Corrected consumer contract

`copyNativeGcLights` retains all packet provenance, real racer/component,
material/texture, fleet-owner and geometry pairing gates. Before any GL call,
it still requires:

- All direction, diffuse and specular lanes are finite.
- Every direction lane has absolute value at most one. No epsilon, rescaling
  or renormalization is introduced.
- Diffuse is nonnegative after the existing selected-negative-light fold.
- Direction fourth row/column are zero, diffuse fourth-column layout is
  `(0,0,0,64)`, and specular fourth row/column are zero.

The false upper color cap and specular nonnegative assumption are removed.
All accepted matrix values are copied unchanged. There is no new exposure
constant, color clamp, negative-light drop or permissive fallback rendering.
Future consumers of specular must establish their own material contract; this
repair does not activate specular shading.

On any remaining matrix rejection, one failure-only stderr record reports
racer, component, row, lane and all three original values with `%a` hexadecimal
floating-point formatting. The existing first-error diagnostic and rejected-
frame transaction remain intact. This uses no public diagnostic-layout change
and introduces no successful-frame logging.

## Existing shader and output behavior

The native shader upload remains `diffuse RGB / 128`. Signed specular is retained
but is not uploaded. The fragment shader sums ambient and three nonnegative
Lambert terms using the existing normalized world-normal path and unchanged
selected direction columns. Values above 127.5 therefore legitimately yield
coefficients above `127.5/128`; no second normalization is appropriate.

The finite color domain does not require a replacement arbitrary upper cap to
keep this sum finite. With admitted normalized normals and each direction lane
bounded by one, a conservative bound on each Lambert term is three. Even four
maximal finite binary32 input colors give at most approximately
`(1 + 3*3) / 128 * FLT_MAX` after the existing scaling and sum. This leaves a
large overflow margin. Actual authored values here are far smaller.

The existing base texture and normalized unsigned-byte vertex colors are in
0..1, and this native-lit material path is opaque with blending disabled. The
unpublished gameplay target is RGBA8, so conversion saturates out-of-range
final color. This is the existing native output behavior, consistent with
[OpenGL 3.2 normalized fixed-point conversion](https://registry.khronos.org/OpenGL/specs/gl/glspec32.core.pdf),
sections 2.1.5 and 3.9.2. It is not a claim of original generated-color rounding,
material-program or PS2 pixel parity. Neither shader nor framebuffer setup is
changed by this repair.

## Integration and deferred verification

Only one production translation unit changes:
`platform/linux/native/vehicle_window_gl50ds.cpp`. Recompile that unit and
relink the approved Windows executable through the parent-owned build process.
No header ABI, source manifest, shader source or runtime-main change is needed.

A later parent-authorized smoke must establish whether TA passes this boundary,
reaches a published track frame, and can report measured FPS. If another matrix
predicate rejects, its exact lane/value record is now available. Source review
alone cannot establish those runtime outcomes.
