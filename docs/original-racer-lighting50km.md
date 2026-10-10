# Original racer lighting and material arithmetic

Source-only reconstruction, 2026-10-10. The two new headers and acceptance-case
source have not been compiled or executed. No shader, runtime, parser, emulator,
test, sanitizer, or gameplay process was run. No visible improvement is claimed.

## Native increment

`original_racer_light_selection50km.h` supplies two separate allocation-free
leaves. `projectSmallDirectionalRegistry` preserves the original directional /
ambient branch of 149BB0. `foldSelectedNegativeLights` preserves 149A50's final
large-registry color operation. Neither creates a registry, guesses a zone,
declares a cache miss, nor changes the running renderer.

`original_racer_material50km.h` supplies the shared original compressed diffuse
RGB equation and SpecularMapDualCompressed's original second-map coordinates.
Inputs are the effective source matrices, compressed signed-byte normal, and
actual selected color rows. There is no generic replacement light, normal
renormalization, per-fragment Lambert calculation, tint, or invented specular
exponent. Output remains unchanged on failure. The explicit arithmetic domain
is ordinary native binary32, nearest rounding, finite intermediates; this is
not certification of every EE/VU floating-point exceptional mode.

The central acceptance source covers selection order, ambient exclusion from
directional slots, zero fourth lanes, unsupported point/spot or oversized
registries, negative-light folding, sun-slot aliasing, unchanged output on
failure, diffuse normal orientation, upper saturation, early-bias rounding,
nonfinite rejection, and generated UVs outside 0..1. It is authored evidence of
intended validation, not a passed test.

## Light selection proof

Original executable SHA256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

14A0A0 first consults the optional manager+0A4 cache at 14A100. Only a miss or
absent owner reaches the branch at 14A128. A registered manager+94 count below
five calls 149BB0 at 14A14C; five or more uses the separate spatial branch.

149BB0 clears all three matrices, scans the actual manager+9C registry in
original order and skips type 4 ambient lights (149C64..149C70). For type 2
directional lights, 149C80..149CB4 writes the negative source look vector down
one direction-matrix column, then copies source diffuse+3C and specular+4C RGB
to the matching color row. 149EA8..149EBC zeroes both color fourth lanes.
149ED8..149EE8 copies the actual selected manager+60 ambient row into diffuse
row 3. It does not compute a hemisphere term or use the camera direction.

The admitted subset excludes type 1/3 point and spot lights. Their 149CFC branch
needs actual object position, distance attenuation and 149F20's cone equation.
At most three nonambient lights fit the original output. Four nonambient
entries are rejected by the native boundary rather than overwriting storage.

The authenticated GC SCB has two authored lights, ambient RGB 80,80,90 and
directional RGB 170,170,160. Light constructor 146FDC..147014 divides source
color by two; those base stored values are therefore 40,40,45 and 85,85,80.
This authoring does not itself publish a live registered owner, establish an
actual cache miss, or prove no subsequent light/ambient update happened.

TA has 26 authored lights and does not qualify for this small-registry subset.
Its exact spatial query ordering, weakest-slot replacement, real sun insertion,
zone membership, portal interpolation and overrides remain necessary.

## Negative lights have a specific original effect

14A704 calls 149A50 after the large path selected its three light rows and
ambient. 149AB8..149ADC sums each negative RGB component in row-major order
and zeroes that individual light-color component. For each channel it then:

1. Updates the actual selected sun row to max(0, sun + 0.75 * negative sum),
   if the source optional sun pointer is non-null (149B18..149B54).
2. Updates ambient to max(0, ambient + 0.5 * negative sum)
   (149B5C..149B90).

The default constants are literal 0.75 at 149A80 and 0.5 at 149A60. The sun
pointer aliases the selected diffuse matrix row installed at 14A43C..14A460;
it is not invariably row zero. Specular rows, directions and fourth lanes are
unchanged. This operation must not be appended to the original small path,
which returns directly after 149BB0.

TA's fourteen authored negative RGB spotlights consequently cannot be treated
as ordinary positive point lights or replaced with a uniform ambient boost.
This leaf recovers their final selected-color treatment, not which particular
spotlights apply to a vehicle at an actual point on the course.

## Original compressed diffuse RGB

Registration 150ABC selects packet 337690 for DiffuseCompressed. Its precise
packet extent is 3376B0..339600, SHA256
`bd4f7c06b524a9f0ba99df4d60cebab81c62d763128829983d2253114b0ff0b1`.

StripPreamble loads source direction rows 20..23 and composes their first three
rows with the actual model/bone basis (3400A0..340160). The compressed branch
applies the authored normal quantization at 3400D0..3400E0. It loads selected
color rows 24..27 at 340308..340328. The caller must supply these effective
inputs; the current renderer's inverse-transpose normalized world-normal
stream is not an interchangeable exact input.

337868..337878 computes the three light amounts from the signed normal lanes;
3378B8 takes max(amount,0). 337900 starts the color accumulator with ambient
plus 8388608, followed by each selected-light RGB times its amount, in slot
order at 337908,337910,337928. 337958 upper-clamps at 8388608+255, and 337978
writes the raw float color words into the packed output. This is an early
bias and per-step rounding sequence, not sum-all-terms then integer truncate.

The material generates RGB from these lights. It does not multiply the scene's
stored vertex RGB into the diffuse result. The candidate does not implement
negative/wrapping packed RGB; such a result is an explicit arithmetic boundary.
Alpha, perspective packet construction, source clipping, fog, texture function
and framebuffer blending remain distinct downstream operations.

## Original dual-map coordinates and pass behavior

Registration 150B3C selects SpecularMapDualCompressed packet 32C3F0. Its actual
packet extent is 32C410..32E2A0, SHA256
`d1f474a2a2bcdf35683f5af1c556df8a5ed26a7f3e11e1de5c11425b359e2cfa`.
The following packet begins at 32E2A0 and must not be included in this program's
instruction-address map.

32C450..32C460 scales the source model/view axes by authored normal
quantization. The unclipped single-palette path computes normal-to-view XY at
32CA58..32CA78, multiplies by 0.5 at 32CAB8, then adds 0.5 at 32CAD8. The
multi-palette setup and equivalent equation occur at 32CD68..32CE40 and
32CFF8..32D080. There is no reflection-vector or shininess equation here.
Neither coordinate is clamped or flipped by the source equation.

The two passes reuse source Q, position and diffuse color. At 32C770..32C7A0,
the second packet copies the first packet's RGBA and XYZ and substitutes the
generated second-map S/T/Q stream. 32C7C0..32C7E0 selects and kicks the second
map then restores the first state. Actual body PSM map-2 blend values are
A=0, B=2, C=1, D=1, with alpha blending enabled and depth writes disabled.
This requires the real destination-alpha contribution, not an arbitrary
additive highlight. An opaque-alpha native presentation target cannot promise
that behavior merely by enabling ordinary source-alpha blending.

The existing Pod01 preservation names specmap5 as an exact TGA (SHA256
`574712ee163003de4bdbdfbeff4d495c52570b9dd2be3b62ec399d7b3dd27b6b`),
not an admitted PST map. This increment neither substitutes a texture nor
claims the dual-map body pass is currently rendered.

## Integration boundary

These are numerical source leaves, not a complete lighting owner. Genuine
scene registration, current matrix/cache publication, object/zone membership,
normal palette/quantization, and material identity must be joined before a
renderer may consume them. Do not enable authentic lighting by filling these
inputs with constants chosen from a screenshot. Current shader/material files
are unchanged by this increment. No GC/TA visual acceptance or full shader
parity is implied.
