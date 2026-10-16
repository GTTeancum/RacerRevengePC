# Native material coverage audit, 2026-10-14

Source-only review after the existing GA DefaultMT-alpha recovery. No build,
test, syntax checker, sanitizer, game process, helper executable, guest
instruction execution or PS2/VU emulation was run. The assigned SA moving-world
adapter and its explanatory source comment were also edited after root review.

## Result

No further MA/GA-specific missing-alpha-flag defect is established by the
reviewed native source. The existing Gamorr fix is connected to both static and
moving GA geometry. MA static material policy already inherits authenticated
DefaultMT alpha fields. Ryloth RA/RC currently has a cold-intake candidate, not
an admitted native world/material route, so Ryloth render coverage cannot yet be
certified or corrected from the existing adapters.

One concrete adjacent remaining mismatch is the SA moving-world adapter:
authenticated source PSMs omit alpha state, but its display policy disables
alpha testing instead of inheriting the same DefaultMT owner as static SA.
This is a source-policy discrepancy, not evidence of the cause of a particular
MA/GA screenshot border and not a measured gameplay defect.

## Coverage chain already present

- `src/assets/original_standard_material_object50la.cpp`, `singleState`, copies
  inherited GS fields before applying nonempty pass fields. Missing fields are
  inherited, not zero. `inputAlphaD` precedes `inputColorD` for compatibility.
- `tests/native/material_alpha50gx_fixture.h`, `selectGa`, authenticates the
  DefaultMT identity SHA256
  `1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7`, inherits
  alphaTestOnOff/method/reference/failure policy, then applies source fields.
  GREATER with reference in 0..255 and KEEP is explicitly admitted.
- Static GA binding in `tests/native/terrain_interactive_drive50du.cpp`
  chooses `selectGa` and copies `sourceAlphaTest` and `sourceAlphaReference`.
- `tests/native/ga_dynamic_window_fixture.h` uses the same `selectGa` for moving
  defense-gate/opening model selections and copies those fields.
- MA/SA static binding chooses `selectMaSa`, which inherits the complete
  authenticated GS state and admits GREATER/reference/KEEP. Source blend, depth
  and fog flags are separately carried. The MA shield's distinct program has
  an explicit narrow admission; it does not force all MA material alpha off.
- `platform/linux/native/vehicle_window_gl50ds.cpp` copies alpha fields into
  both retained static and moving span storage. Both draw functions bind mode2
  and the source reference when `sourceAlphaTest` is set.
- The track fragment shader multiplies authored vertex alpha by sampled alpha,
  forms `floor(clamp(alpha * 255 * 255 / 128, 0, 255))`, and discards bytes less
  than or equal to the source reference. That is the declared native GS-unit
  coverage approximation; interpolation/filtering/fragment quantization parity
  with the original hardware is not established by this review.

## Pixels, coverage and blending are different dependencies

`tools/assets/decode_source_textures50fc.py`, `ipu_rgba`, already expresses the
retained original kind2 TEXA TA0=TA1=255/AEM1 policy: nonzero RGB5-expanded pixels
carry alpha255; zero RGB carries alpha0. The source anchors are 2421EC..2421FC.
Its existing independent scalar conversion and FFmpeg cold decoding are source
asset conversion, not guest execution or a reason to add a color key.
`assemble_ma_sa_visual.py` uses this existing conversion for kind2 resources
and `decode_indexed` for indexed resources. Indexed conversion copies original
palette alpha without normalization; kind0 and kind1 remain distinct.

Thus black/alpha0 pixels can still appear black when the chosen native policy
has both alpha test and blending disabled: shader opaque output deliberately
uses presentation alpha1. Uploading alpha alone does not establish coverage.
Conversely opaque black with nonzero authored alpha may be legitimate texture
content. This audit does not label every black region transparent.

`configureBlend` separately chooses disabled blending, source-alpha/source-over
or additive equations. Turning blending on is not an interchangeable repair for
missing GREATER/KEEP: alpha testing also prevents rejected depth writes, and the
source flags decide which path is appropriate. No blanket blending, alpha
clamping, texture RGB rewrite or name-based color key is justified here.

## Proven adjacent SA moving-world omission

`tests/native/sa_dynamic_materials_fixture.h` authenticates eight original
PSM/PST pairs and explicitly requires each decoded material to contain exactly
`microProgramName` and `TextureMap_1`; alpha/depth/blend fields are absent. The
source identities and fields remain retained in its Selection records.

`tests/native/sa_dynamic_window_fixture.h` instead hardcodes opaque/depth-writing
LEQUAL policy, `alphaScale=1`, and `discardZeroAlpha=false`. It neither obtains
DefaultMT/templates/runtime nor assigns `sourceAlphaTest` or reference, leaving
both at their zero-initialized defaults. This differs from the existing static
SA `selectMaSa` and the original inheritance rule. The original constructor's
field table in `src/assets/original_ui_material_fields50kg.inc` also explicitly
records enabled GREATER, reference120, KEEP; the authenticated template must
remain the operative owner rather than substituting these literals.

Implemented source-only fix after integrator file assignment: pass the actual
authenticated templates and runtime into SA moving-world `createGpu`; select
each retained source Selection through the existing `selectMaSa`; propagate its
coverage and other validated material fields, matching the static SA binding.
Keep the existing texture bank, source geometry and owner lifecycle. Do not
edit source pixels. Root should review call sites and run its own authorized
validation. The root owns the terrain caller update to provide those actual
owners. No global renderer or MA/GA adapter change was made. The current MA
smoke cannot establish SA moving-world behavior; this change remains untested.

## Ordering boundary

Static and moving native track draws defer single-pass nonopaque spans into an
explicit transparent pass, preserving stable source-span order within that
pass. Layered passes remain adjacent. The renderer explicitly disclaims global
source sorting. This is a remaining fidelity boundary for overlapping blended
surfaces, not proof of missing flag alpha and not a reason for speculative
camera-distance sorting. The original collector also has later-pass barriers;
a subset-only sort cannot replace that complete source dependency.

## Ryloth boundary

`tests/native/track_source_fixture.h` currently admits only ta/gc/ga/ma/sa.
`tools/assets/prepare_ry_intake.py` names RA/RC and explicitly disclaims gameplay
selection. Until exact RA/RC materials, texture states and native bindings are
retained and admitted, no concrete Ryloth alpha/blend/order repair is proven.

## Required evidence for a specific remaining black-border report

Identify the actual source material and draw span, its texture kind and retained
alpha bytes, inherited-plus-authored alpha test/blend/depth state, vertex alpha,
native bound policy and capture location. A source-backed disagreement in that
chain supports a narrow repair. A screenshot alone does not authorize color
keying or establish whether black RGB is opaque content, rejected coverage,
blended edge filtering or ordering. Root retains all build/test integration.
