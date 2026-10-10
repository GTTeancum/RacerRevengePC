# Native zero-map original UI rendering candidate

## Result and status

Staged native renderer and adapter changes support the authored zero-map
ScreenObject aperture material without inventing a texture. The five replacement
files are under `files/platform/linux/native/`; their exact active source copies,
taken before any staged edits, are under `preimages/platform/linux/native/`.
The parent supplied baseline HEAD `e6b5d7f`. No active repository file was edited.

This is source-reviewed, uncompiled, untested implementation. No project helper,
importer, parser, decoder, test, syntax check, shader check, build, gameplay,
guest execution or emulator was run. Static file reads and source edits only.
The root owns integration and its centrally limited validation slot.

## Integration dependency

Integrate this candidate together with the material worker's staged
`ui-material-fallback-reconstruction/files/src/assets/original_ui_material50kg.h`
and `.cpp`. That work provides `OriginalUiNativeUntexturedMaterial50KG` and
`prepareOriginalUiNativeUntexturedMaterial50KG(pass, policy, diagnostic)`.
The result type is separate from the existing atlas-taking textured policy.
The staged adapter includes and consumes that type; applying this header alone
to the old material header is intentionally insufficient.

## Typed material and span path

- `OriginalUiMaterialMode50KG` is `unspecified`, `texturedModulate`, or
  `untexturedVertexColor`. Default/unknown values reject. A missing texture does
  not select untextured rendering.
- The existing `convertOriginalUiMaterialSpan50KG` only gains the explicit
  `texturedModulate` assignment. Its original admission, bounds, source sampler,
  blend/depth mapping, alpha rules and output-on-failure contract are unchanged.
- `OriginalUiUntexturedMaterialSpanBinding50KG` contains only `firstQuad`,
  `quadCount`, and `drawQuadCount`. It cannot carry an atlas/index/sampler.
- `convertOriginalUiUntexturedMaterialSpan50KG` consumes the separately admitted
  zero-map policy. It retains the unresolved texture sentinel and entirely
  unspecified sampler; it maps the real blend and depth settings explicitly.
  Invalid bounds, scales, enum values or owner flags reject transactionally.
- The renderer rejects an untextured span carrying any texture index or sampler
  and rejects a textured span without its genuine published texture. Its exact
  partition and depth admission remain common to both modes.

The shader has one uniform-controlled source-mode branch. Textured MODULATE
retains its existing expression and pre-blend RGB clamp. The untextured branch
reads interpolated raw vertex color directly and never calls `texture()`. The
submission loop sets the mode and alpha scale for every span, unbinds texture2D
for untextured spans, and leaves all spans in their supplied order. No source
image, white texture, fallback atlas, recoloring, reordering or generated art is
introduced. Vertex/index layout and native triangles are unchanged.

## Color, alpha and depth

For normalized raw vertex inputs, zero-map RGB and stored alpha use scale1.
When source-alpha/additive RGB blending is admitted, alpha uses `255/128` once.
Textured MODULATE remains RGB scale `255/128` and blended alpha scale
`(255/128)^2`. Opaque spans retain the existing API's alphaScale1 convention.
Source packet bytes are never rewritten. Raw vertex alpha<=128 remains a
bounded admission for both modes, not a statement that all original opaque
materials required that bound.

The authenticated Main NoTexture material disables blending, so aperture
vertex alpha63 does not make its RGB translucent. The policy supplies ALWAYS
depth and inherited depthWrite=true; neither the converter nor shader changes
those settings. The original aperture owner still supplies both real rectangles
at its actual render point. Their source TL,BL,TR,BR order must be permuted
0,2,3,1 for this existing TL,TR,BR,BL interface. Source packet Z and RGB remain
the geometry owner’s responsibility; this candidate invents neither.

All existing D24 reverse-depth, integer packet-Z bounds, 640x448 canvas,
letterbox/scissor, retained-race image, private-target publication and failure
behavior remain. The same `opaquePresentationAlphaOnly` proof is required.
Targets retain alpha1 through RGB-only writes. Supporting zero-map color does
not implement source framebuffer alpha storage or certify a complete pass/
target graph without destination-alpha consumers.

## Texture ownership

Cold renderer creation may now receive both original UI banks null for
explicitly untextured-only use. One missing bank still rejects. A supplied pair
still receives the original loaded/publication/identity/raw-alpha admission;
empty supplied banks still reject. Submission must retain the same cold bank
pointer and publication, including null/0 for the no-bank case. Therefore a
renderer created without UI banks cannot later sample a texture.

Mixed draws retain their original exact CPU/GPU publication. All textured spans
still receive the existing per-span immutable sampler verification before target
writes. Untextured spans skip image lookup and sampler queries. There is no
special bank entry, synthesized empty texture owner or texture-state mutation.

## Source basis and remaining work

`evidence/SOURCE-NOTES.md` identifies the static source and primary hardware
semantics. The separate mixed widget owner exposes original traversal order;
neither this renderer nor that owner fabricates a final ordered pass graph.
The caller must preserve authenticated registry/pass ordering, texture
animation, real geometry/depth and alpha-consumer ownership. No frontend
switch, complete Main controller, page tree, input, scene, particle, or renderer
call site is changed by this candidate.

Root's future validation should cover mixed alternating modes, opaque aperture
alpha63, source-alpha/additive untextured coefficients, unchanged textured
MODULATE, wrong/default mode, missing real textures, inconsistent samplers,
unknown publication, source depth behavior, ordered partition errors, and
failure preserving the previously committed image. These are proposed targets,
not performed checks or reported passes.
