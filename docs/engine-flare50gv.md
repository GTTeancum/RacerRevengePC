# 50GV source-derived engine flames

The retained Pod01 engines already contain the flame geometry and texture. This
change supplies the missing original per-engine render callbacks; it adds no
billboards, substitute mesh, gradient texture, bloom, or arbitrary glow.

## Actual retained inputs

- Left `englt1.psg` / right `engrt1.psg` each have one LOD0 flame surface at
  material index 1 (`flame01l` / `flame01r`). Their full four-LOD material lists
  alternate static and flame entries; indices 3, 5, and 7 are other LODs.
- The two LOD0 flame surfaces together have 336 vertices and 144 triangles.
  Original UV ranges are U 0..1 and V 0.00048852..1. Original rigid geometry,
  current scene transforms, index order, and colors remain in the ordinary
  vehicle draw path.
- Both materials reference the actual `flame` 64x64 texture, retained key
  `abec7a3d99994cdedfaf7cab18576f51dc903f89afb20e977e791d97743ac378`.
  Dark texels belong to additive blending; they are not a black opaque panel.
- PSM explicitly sets ALPHA A=0, B=2, C=2, D=1, FIX=255, alpha blending on,
  depth writes disabled, V clamp. Its blend equation is Cs*(255/128)+Cd.
  U repeats. Generic renderer policy remains separate from this numeric adapter.

## Native callback and state

`src/game/engine_flare50gv.*` reads source20F110 for the left engine and20EE90
for the right engine. Existing50W owner private rows and50X AC4/AC8 fields are
reused; there is no second smoothing owner or isolated visual RNG.

Each side takes the norm of its actual engine localThrust vector. The original
low-speed steering branch instead takes6000*max(rawB5C,0) on the left and
6000*max(-rawB5C,0) on the right, only when aero.lowSpeedDc and application58
bit1 are both set. An enabled AD0/ACC smooths magnitude with source per-call
coefficients0.15 rising and0.05 falling, writes AC4/AC8, and computes
max((4000+smoothed)/10000,0.6). When disabled it keeps AC4/AC8 unchanged and
uses the unsmoothed magnitude/10000.

The callback consumes one actual shared308030 LCG draw and writes TexGen row2.y
as normalized/1.1 +0.1*random01. It advances row3.x by the exact source float
bits0x3de353f8, subtracting1 only when the new value exceeds1. The constructor
246B70 establishes row1/2/3 pointers at animation14/18/1C; the callbacks write
row2.y and row3.x, respectively. No other row element changes.

The output affine representation is:

- U' = U*row1.x + V*row2.x + row3.x
- V' = U*row1.y + V*row2.y + row3.y

This matches `uv[side][6]` order `(m0,m1,m2,m3,m4,m5)` for the generic renderer.
Left is side0 and right side1. Scrolling is horizontal and clamp is vertical.

Original29AC90 skips a callback when animation+8 already equals app frame230,
then writes that frame stamp at29AD58 after execution. The adapter uses the
existing50W private record words[2], initiallyFFFFFFFF, as those source stamps.
One accepted rendered frame invokes each material once even though it may have
multiple scene references. Do not invoke separately per triangle, source strip,
physics substep, or repeated same-frame submission. The current native host has
its own monotonic rendered-frame stamp; this does not claim restoration of the
original application's complete rendering scheduler.

## Integration contract

Call `advanceEngineFlare50GV(frame, baseTail, coldFlame, renderFrameStamp, output,
diagnostic)` before submitting a prepared vehicle render. All canonical inputs
are const. A successful result owns candidate private rows, AC4/AC8 bits, shared
RNG, and frame stamps; output.uv is directly renderer-facing. Bind side0 to left
engine material1 and side1 to right engine material1. Source blend/depth/sampler
policy must be applied to those same spans.

Only after accepting that exact serial frame call
`commitEngineFlare50GV(output, frame, baseTail, coldFlame)`. Do not commit an old
output after newer physics work, and do not replace source random input with an
independent visual seed. Two invoked callbacks add exactly two draws to the
existing source-shared stream, which is an intentional recovered source side
effect. Repeating a committed stamp has zero callback/RNG/scroll effects.

The ordinary Ready/PostGraphics path is represented. Paused application11C
handling is explicitly outside admission. Finite binary32/FE_TONEAREST inputs,
finite intermediates, source binding tags and affine matrices are checked.
Unsupported states reject transactionally; no canonical state or output changes.
The production path is allocation-free, bounded, and performs no I/O.

## Verification

`tests/native/engine_flare50gv.cpp` decodes actual Pod01.RES, prepares50W owners,
binds50V callbacks, checks actual flame geometry/material settings, and tests:

- Distinct left/right thrust, rising0.15 and falling0.05 smoothing
- Positive and negative low-speed steering branches
- Disabled smoothing, a3-4-5 thrust norm, and unchanged source templates
- Per-frame stamp deduplication and100 repeated scroll updates
- Exact source LCG draw count and no hidden visual seed
- Zero hot allocations and transactional nonfinite, pause, and binding rejection

The focused native fixture passed. This test is not original guest execution or
an original/native raster parity claim. Full VU lighting, pixel rounding, exact
original render visibility/order, and alternate vehicle flame assets are not
covered. The existing semantic vertex/color and texture data are preserved.

Compact static source excerpts, decoded source texture inspection, build logs,
and native-test JSON are under the50GV evidence directory. Original ELF SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
