# Canonical right-engine source colors and base-map MODULATE

Source-only candidate against `c1a02900ad4a701a151c7e49e5d7ce7d0ef1201e`,
tree `6fab0aef460af581691900cf1ab1b73748f1e920`. No build, tests, compiler,
syntax/lint/scanner/sanitizer, decoder, asset loading, gameplay or guest execution
was performed. Native host arithmetic and the existing host GPU remain the only
intended execution path.

## Actual connected increment

The previous canonical right-engine material join still supplies the exact
PSG, PSM, material slots, source texture and native bank. This packet adds an
immutable per-material selection for the actual `SpecularMapDualCompressed`
base context. It requires two maps/two contexts, no active parameter override,
TCC1/MODULATE0, Gouraud shading, texture mapping, STQ coordinates and clamped
colors. The admitted base context has blending, alpha testing and destination
alpha testing disabled and depth writing enabled. Unsupported context or
parameter behavior is never silently interpreted as this subset.

The live caller publishes the same genuine selected-light lease before this
body's numeric draw projection. The other caller routes retain their previous
order. The same per-node palette used by current position projection supplies
the source normal basis. Each semantic normal must exactly round-trip through
its authored normal quantization to a signed byte. There is no inverse
transpose, normal normalization, derivative fallback or imported vertex-color
multiplier in this selected color calculation.

The recovered arithmetic scales selected light-direction rows by qNormal
before composing each node basis, takes three raw-normal light dots and their
maxima with zero, then starts each RGBA accumulator at ambient + 2^23. Three
ordered multiply/add terms follow. The result is upper-clamped at 2^23+255
and its raw low byte is the packet color. The actual selected row alpha is
consumed as well; no alpha64 literal substitutes for that owner.

All fallible color work uses existing bounded caller scratch before publishing
the destination component. Position, topology, source UV, flame colors and
unselected material bytes retain their established paths. No per-frame
allocation, source geometry replacement, frame mutation or RNG use is added.

The real renderer verifies the complete material mask against its copied cold
canonical policy. Generated-color metadata must match the exact selected-light
owner, publication, graphics sequence and racer slot submitted with that batch.
Stopped racers retain their completed geometry and matching light publication.
The original right-engine base-map draw then uses non-perspective generated
RGB interpolation and the recovered byte MODULATE equation:

    rgb = floor(clamp(textureRGB * 255 * interpolatedVertexRGBBytes / 128, 0, 255))

The native shader bypasses its imported-RGB/per-fragment host lighting only for
this explicit source-color mode. Other component/material paths and the explicit
native-lighting diagnostic opt-out remain unchanged. The new mode is not
admitted for track, dynamic-world, sky, effect or other racer components.

## Pause and ownership

The new preparation occurs inside the existing gameplay draw branch. Main
inactive, host menu, focus/minimize and recovery pause branches all return
before it. No color update, selected-light publication or flame/RNG advancement
is introduced in paused presentation. Immutable material masks belong to the
same Visual that outlives GPU/renderer borrows. Draw submissions retain only
that stable mask and copied scalar light-publication metadata, never a bank
arena or draw-lease pointer.

## Why this does not enable the second map

The second-map UV and blend equations are source-backed, but the real gameplay
target does not yet own complete original framebuffer-alpha composition. Its
body path previously wrote opaque alpha and selected normalized per-fragment
Lambert lighting. A second draw using that alpha would be the wrong effect.

This packet prepares genuine RGBA in the CPU stream and consumes its RGB for
the actual base draw. Output framebuffer alpha deliberately remains the
existing opaque presentation alpha. It neither advertises that alpha as a
source value nor enables a destination-alpha consumer. The necessary next
dependency is a gameplay-specific source-RGBA target with ordered alpha writers
and consumers, followed by the actual second context/map draw. The sealed UI
source-RGBA owner is not relabeled or reused as proof for a gameplay frame.

Native clipping, depth comparison, largest-mip/linear sampling and existing fog
policy remain explicit rendering boundaries. Recovered selected-light numeric
rows still come from the existing bounded native authored-zone selection;
retail cache/scheduling parity is not implied. No complete dual shader, original
rasterizer or visual-fidelity claim is made.

## Retained source evidence for the next second-map step

- Original ELF SHA256:
  `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
- `docs/original-racer-lighting50km.md` and the existing numerical leaf describe
  `SpecularMapDualCompressed` packet 32C410..32E2A0. Second packet
  32C770..32C7A0 copies first RGBA/XYZ/Q and replaces generated STQ.
- UV is raw signed-byte normal dotted into authored-qNormal-scaled model/root
  world × source camera-view XY, then ×0.5+0.5. The root UV basis is distinct
  from the selected bone's position/diffuse basis. No reflection vector,
  shininess, normalization, Y flip or clamp belongs to this equation.
- Source map2 ALPHA A0/B2/C1/D1 requires
  `clamp(floor(Cs * currentDestinationA / 128) + Cd, 0, 255)` and independent
  replacement of destination A by source A. A normal GL source-alpha blend
  does not establish this behavior or ordered primitive feedback.
- Dual program locations 32C548/32C550, 32C9B8/32C9C8,
  32CFC0/32CFC8 and 32D4F8/32D500 establish constant F255. Enabled source fog
  still uses /256 byte arithmetic; this is not host radial world-distance fog.
- TGA source 24E310..24E320 clears resource F8 bit0; 250F7C..250F98 sets bit2.
  Second-map binding 24989C..2498BC only replaces TEXA when bit0 is set.
  Therefore specmap5 must consume actual context2 TEXA. Its existing packed1555
  RGB lanes expand by <<3, not bit replication; bit15 is mip coverage and does
  not by itself select sampled alpha. If actual TA0=TA1=255/AEM0, both coverage
  values sample A255, including coverage0 magenta. The Controls specmap3
  decoder is identity-specific and is not called for this gameplay texture.
- Full dual clipping/raster behavior and gameplay framebuffer-alpha ownership
  remain unfinished. The exact retained specmap5 header remains the previous
  documented runtime admission uncertainty.
