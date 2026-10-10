# Track lighting source correction and racer-lighting boundary

Frozen source-only candidate, 2026-10-10. No active files changed. No builds,
tests, syntax checks, project parser execution, gameplay, screenshots or guest
execution. All source programs were inspected as bytes; no VU/PS2 emulation.

## Concrete correction ready for integration

`source-prelit-correction.patch` changes four files. Matching staged full files
and their exact active preimage/staged SHA256s are in `files/` and
`candidate-manifest.json`. Prefer the small patch or manual equivalent over
copying whole files, because the parent owns concurrent UI/audio integration.

The authenticated PrelitCompressed material dispatch copies precomputed source
RGB without running the original diffuse-light equation. Current native track
rendering nevertheless multiplies those RGBs by an extra gray host light.
The candidate adds an explicit `sourcePrelit` flag and bypasses only that extra
illumination for authenticated prelit program selections. It covers26 of29 TA
track materials and53 GC single-pass materials. Existing636 GC ProjectedPrelit
passes remain prelit; the other4 GC DiffuseCompressed passes keep their current
host fallback pending original lighting ownership.

The new flag is separate from `unlit`: it does not disable current native shadow
receipt, reclassify materials as emissive, change depth/alpha policies, or affect
fog eligibility. The flag defaults false, is appended to the existing policy
structures, and is selected only by an already-authenticated material program.
Existing racer/dynamic callers retain false. Renderer material application
honors the flag consistently if a future authenticated caller supplies it.
No tint, global exposure, texture, light direction, far plane or fog setting
changes. This is original prelit dispatch recovery, not full lighting parity.

## Original program proof

Registration150AD8/150ADC maps `PrelitCompressed` at3A5740 to packet339600.
Original capitalization variants identify the same admitted program through
case-insensitive source-name matching already used by material resolution.
`evidence/PrelitCompressed.words.txt` is the complete static instruction listing.

- No-fog clipped path:339740 loads input RGBA into its color vector;339858
  writes that vector directly as output RGBA. Its other operations transform
  position and texture coordinates, not the loaded RGB.
- Fog clipped path:339E00 loads the input RGBA;339F10 writes it unchanged.
  Fog is computed and packed independently, so fog does not turn prelit RGB
  into a diffuse normal/light calculation.
- No-fog/unclipped and fog/unclipped branches likewise preserve the color
  stream; the complete bounded program is included for independent review.
- Compare actual DiffuseCompressed337690:337868..337878 computes lighting
  normal components;3378B8 clamps them against0;337900..337928 combines light
  colors with ambient;337958 clamps the result before packed color output.
  That is an actual different material algorithm, not just a different label.
- ProjectedPrelit32E690 also preserves source prelit RGBA while calculating its
  extra texture projection. Current GC code already bypasses added host light
  for that program; the candidate makes ordinary prelit dispatch consistent.

Original geometry reader/import_scene48d.cpp preserves each authored byte color
at its source color stream. Vehicle/static preparation retains those published
colors. No replacement colors are created by this candidate.

## Current versus authentic lighting ownership

Current static track shader: vehicle_window_gl50ds.cpp/createTrackShader.
With source normals, it uses:

    direction = normalize(0.35, 0.85, 0.30)
    gray = 0.32 + 0.16*max(normal.y,0) + 0.52*max(dot(normal,direction),0)

Without source normals, it uses derivative-flat0.45+0.55*abs(dot). Both are
explicit host display policies. TA/GC scene lights never supply these constants.
The candidate removes this multiplier only for actual prelit programs.
Existing TA vertexColorScale2 and GC255/128 remain unchanged.

Current racer shader: vehicle_shader_gl50cz.cpp. It uses the same gray
0.32/0.16/0.52 formula. applyPodMaterial enables it for opaque, non-unlit
materials and bypasses it for blended/additive effects. Therefore current opaque
racers are not literally fullbright in the inspected code; their shading is a
fixed host substitute, independent of track lighting. This inspection cannot
explain the appearance of an older capture or prove a current rendered result.

Current normals are real scene normals transformed by the native live geometry
owners, with a derivative-flat fallback for explicit zero/missing normals.
Replacing gray light uniforms alone would still not implement the original
racer material dispatch, color normalization, light selection or specular maps.

## Authenticated scene authoring

`evidence/scene-lights.json` records exact Fog-stage-authenticated SCB offsets
and every light field, with no project parser execution. Scene reader277494..
2774F0 uses26FA60 raw integer RGB; creation277D80/277E84 passes those channels
to146F60;146FDC..147014 divides color by2 before146C40 stores the light.
This constructor normalization must not be mistaken for normalized0..1 RGB.

GC has exactly2 authored lights:
- Ambient RGB80,80,90 (SCB27930).
- Directional RGB170,170,160 (28428), position1920,1028.208,1132.750,
  target1920,90,1920. It emits light, is at infinity, and casts shadows.

TA has26:1 directional,8 zone-associated ambient lights and17 spotlights.
- Directional RGB252,255,237; position7366.977,7057.284,9972.466 and
  target5265.405,0,6939.574.
- Normal authored ambient zones use101,84,69. Zones3/4 use51,42,25, and the
  Lights block authors directional overrides30,32,32 for both those zones.
- Fourteen authored spotlights have negative RGB(-100,-100,-100); three use
  positive RGB252,255,237. They have real positions, cone angles and attenuation.
  They cannot be replaced by a global
  ambient boost or ignored when claiming original tunnel lighting.

These are real authoring facts, not proof that every light is simultaneously
active. The per-object/zone live selection and source light matrices remain to
be restored for diffuse and specular materials.

## Why there is no global green adjustment

GC ambient/directional authoring is blue-ish/neutral, not green. Its retained
world vertex colors are likewise mostly neutral/blue: the most common colors
are49,51,57 and55,58,67. Byte statistics are recorded separately and are not
screen-area-weighted or screenshot analysis. Green surface appearance, where
present, needs the actual texture/geometry/material composition, not an invented
green light or a blanket color grade. This candidate preserves that authored
composition while removing an extra unsupported illumination multiplier.

## Racers: explicit future work

`evidence/racer-programs.json` inventories the original eight selected racer
material records. They use SpecularMapDualCompressed bodies, DiffuseCompressed
parts, DiffuseTexTransCompressed effects, and some SpecularMapCompressed passes;
none are the PrelitCompressed surfaces corrected here.

Program registrations in `evidence/racer-program-registration.txt` resolve:
- DiffuseCompressed ->337690 (150ABC)
- SpecularMapCompressed ->33B8C0 (150B1C)
- SpecularMapDualCompressed ->32C3F0 (150B3C)
- DiffuseTexTransCompressed ->33C740 (150B5C)

Follow-up must join actual light-manager selection/normal and color matrices,
TA zone overrides and negative spotlights, then each racer program's diffuse,
reflection/specular and texture-coordinate behavior. No blanket racer lighting
correction is included or claimed here.

Original executable SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
