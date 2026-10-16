# Canonical right-engine paired specular rendering

Source-only candidate against `b6ff338bb9ebdaf54e8ad01b38b4df453a90e77c`,
tree `28291bba707270f3b9fd5dca3a4a4560f161526d`. It preserves that base's
MSVC unsigned source-address fix. No build, test, compiler, syntax/lint/scanner,
sanitizer, decoder, asset loader, gameplay, emulator or guest execution was
performed by this candidate's authors.

## Bounded production chain

The actual canonical right-engine material still selects the same original
model, source node mapping, material token, parameter-free dual program and
base-map bank. Its second texture is resolved through the same mounted source
provider and canonical texture table. The completed TGA resource's original
packed1555 level is copied into a separate immutable native bank using the
actual second context's TEXA and wrap fields. The original row orientation is
already present; RGB lanes expand by left shift three, and alpha consumes
TA0/TA1/AEM. There is no substitute texture, repeated row flip, channel bit
replication, screenshot-derived tint or guessed specular exponent.

A distinct host GPU owner uploads that bank for the player and every actual
opponent. The renderer borrows the exact immutable GPU publication through
its context/thread/token lifetime, alongside the unchanged base bank. Partial
cold failures use the existing cleanup owner; renderer resources close before
either GPU bank, and both close before their Visual/model/domain/Video owners.

Draw projection retains the current model/root matrix, original aggregate
center160 and model positionF0 from the same actual source graph and node map.
The aggregate calculation uses source stored boxes, current post-motion locals
and source scene flags, rather than the native draw AABB. The bounded adapter
admits multi-node models only. Retained static metadata puts every current
original-eight and supported supplemental right-engine scene in that subset;
this is not a runtime admission or visual acceptance result.

The actual source camera publication is supplied directly by the live camera
Session, alongside the same launch11258 field it already borrows. The renderer
checks that camera's converted native projection against the submitted world
projection. Each presentation recomputes second-map UVs from rootWorld × view,
authored qNormal and raw compressed normals. It also recomputes original
camera/model distance and the second-map enable gate: inclusive distance 130,
or 1000 for source subtype 3. Stopped racers keep their geometry and lights but
still consume the current camera for these presentation-only values.

## Source group order and local alpha

Each admitted original PSG group has 3–16 vertices and its own resume marker.
The retained strips preserve that group order. Manual recovery of both
single-palette and multi-palette dual-program paths establishes one complete
base packet followed by its matching second packet before the next group.
Multiple strips sharing a group remain together; duplicated continuation
endpoints do not collapse neighboring groups. A whole material followed by a
whole second material, or alternating base/second triangles, is not substituted.

The native draw owner therefore replaces only the authenticated body's span
submission with paired source groups. The ordinary player backend and the
opponent opaque path use the same paired routine. The second map is not
queued with flames or other deferred effects after unrelated scene writers.
The original distance gate may omit a group's second draw, never its base.

Both actual contexts must use source GEQUAL, lowered to native LEQUAL on the
same positions, indices, linked position program, projection, viewport and
raster/sample coverage. Base has no alpha test, destination-alpha test or
blend, and writes all RGBA channels and depth. Map2 writes no depth and uses
its actual destination-alpha additive equation. These properties guarantee
that every surviving second fragment reads alpha written by its own group's
base, or by an earlier surviving second fragment from that group. No claim
about unrelated track or other racer framebuffer alpha is needed.

Both passes compute quantized source bytes:

    sourceRGBA = floor(clamp(textureRGBA * 255 * vertexRGBABytes / 128, 0, 255))

The generated vertex RGBA uses source screen-linear interpolation. Base stores
all four source bytes. Enabled dual-program fog consumes its recovered F255,
reducing to floor(sourceRGB ×255 /256), leaving alpha unchanged. Map2 optionally
applies the actual GREATER0/KEEP test after source-alpha quantization, then:

    RGB = clamp(floor(sourceRGB * currentDestinationAlpha / 128) + destinationRGB, 0, 255)
    A = sourceAlpha

Alpha is stored as a byte divided by 255, independently of the RGB coefficient
that divides by 128. Ordinary source-over accumulation, a fixed opaque alpha,
or a deferred alpha-only replay would change this dependency.

The paired capability uses the existing transactional gameplay targets with
texture-backed RGBA8 color storage. Ordered feedback requires native texture
barrier support; each actual second-map triangle is submitted in source order
with the barrier needed before its same-pixel read/write. Unrelated draws do
not retain the destination texture binding. This is a native GPU algorithm,
not a PS2 rasterizer, register model or guest instruction interpreter.

The local source-alpha lifetime ends after all paired body consumers. Existing
current particles have no destination-alpha dependency, and the original race
HUD owns its separately admitted private alpha domain. Opaque presentation
alpha is restored before HUD/blur publication; no global gameplay-source-alpha
certificate or UI source receipt is minted.

## Admission, ownership and behavior limits

The canonical pair requires both actual parameter-free contexts, original
TCC1/MODULATE/Gouraud/STQ/clamp state, ordinary FBA/PABE/AA and fragment state,
matching inclusive depth, base opaque depth writes, and second A0/B2/C1/D1 with
no depth writes. Runtime material overrides, destination-alpha tests, unknown
parameter behavior, unsupported test equations and other programs do not enter
this path. Actual map2 blank test fields inherit DefaultMT's GREATER0/KEEP;
its source alphaReferenceValue is 0, not the constructor's 120.

The same actual selected-light owner/publication/graphics sequence/racer slot
and complete material mask still authenticate generated RGBA. The new body
metadata is produced before destination publication. Camera-driven projection
uses bounded caller-owned scratch and retains no camera or source-arena pointer.
The actual live caller preserves all original AI, fixed-step, RNG, material
animation and pause paths. Menu, inactive, focus/minimize and recovery branches
remain ahead of draw preparation. Explicit diagnostic opt-out retains the old
base-only route and acquires no second GPU publication.

Native clipping/raster precision, depth representation, largest-level linear
sampling, drawable stretching and bounded native selected-light scheduling
remain explicit policies. The full original visibility/cache/LOD scheduler,
retail framebuffer masks/target transitions, other components' materials and
single-node sphere-distance path are outside this increment. It does not
claim full pixel parity or a complete original hardware model.

The retained specmap5 raw header has still not been independently inspected.
The existing source resource's exact 64×64 BGR24 bottom-origin admission remains
strict and untested. This candidate consumes that resource only after its
actual successful construction; it never replaces a rejected source image.

## Evidence anchors

Original executable SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

- Dual group pairing: base 32CC28 / second 32CD18 / restore 32CD28 / end 32CD34;
  independently base 32D670 / second 32D750 / restore 32D760 / end 32D76C.
- Second-map gate: row41.x bit 0x200 at 32CC30..32CC60 and 32D660..32D690;
  actual flags producer 241424..24146C and packet copy 241478/241484.
- Distance producer 26F0E0..26F190; actual launch threshold 1A6AFC..1A6B28.
- Root camera-normal UV and shared generated RGBA:
  `docs/original-racer-lighting50km.md` and `original_racer_material50km.h`.
- TGA/context2 TEXA: 24989C..2498BC, 24E310..24E320, 250F7C..250F98.
- Native byte compositor precedent: existing UI fragment math, reused only
  where the equations match. Its sealed UI owner is not reused as gameplay.

The frozen packet's SOURCE-EVIDENCE.md includes narrow retained raw-word
excerpts and explains their provenance. The old listing's displayed EE
mnemonics are not reliable annotations for the embedded VU words. New manual
interpretation was cross-checked by reading primary opcode/control-bit source;
no decoder or guest code was executed.
