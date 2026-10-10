# Original UI material inheritance and native CPU policy (50KG)

Isolated additive source only. No active files were changed; no compiler,
syntax check, tests, importer, game, guest code or Git was run. The original
APPSTART and UI archives have not arrived. No missing PSM text or actual
resolved material value is synthesized from the historical hashes.

## Important corrected ownership

The original executable stores pointer 443650 at 3833E8 and pointer 443720 at
3833F0. Static initialization 3CAFAC/3CAFB8 calls constructor 24A8E0 for these
two distinct objects. Startup 17CAE8 loads literal `AppStart\DefaultMT` into
3833E8; 17CAFC loads literal `AppStart\ScreenMT` into 3833F0. Earlier particle
notes identifying 3833F0 as DefaultMT were incorrect. Do not edit those frozen
packets to obscure the correction.

Both objects use the same constructor, but each is subsequently overridden by
its own `[DefaultShaderParams]` file through 24A9A0. Required original members:

- `AppStart\DefaultMT.psm`, 82 B,
  SHA-256 `1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7`.
- `AppStart\ScreenMT.psm`, 748 B,
  SHA-256 `34a6e34f523a4babe9e593037340ddf6c8ae15b6a7a174f349e688c2ead39982`.

The prior 48E importer rejected DefaultShaderParams and repeated keys. This
resolver uses the separately recovered authenticated DAT reader with
`preserveInOrder`; `find()` selects the first matching PRESENT value as
158648 does. A bare `key=` is absent; a quoted empty value is present and is
rejected by strict integer admission rather than invented into a number.

ScreenTexture at 2DF10C and ScreenText at 2DFAA0 explicitly pass the ScreenMT
owner to wrapper/material registration. Pass creation retrieves the real
associated template from registry+30[materialKey] through 14D5E0; a source-known
null falls back to 3833F0. An unknown owner is not equivalent to null.

## Recovered precedence and scope

`OriginalUiMaterialTemplates50KG::load` accepts only authenticated APPSTART and
loads both real files over the constructor. The 35 integer fields and every
getter PC are explicit in `original_ui_material_fields50kg.inc`. Constructor
defaults are executable facts, not graphics API defaults. Every resolved field
retains its source member/byte offset/getter PC, or a null member and original
constructor/runtime PC.

`readOriginalUiMaterial50KG` requires an explicit associated owner selection
and known original runtime flag 435670. It admits 1..8 numbered passes with
one map each. Original one-map state builder is 2476A0. The previously cited
2480D0 is the TWO-map, context-suffixed builder and is not silently reused for
UI single-map materials.

Pass values override the selected template. Important source exceptions:

- Program name inherits the selected template string (247194..2471AC).
- EnableThisShader and totalTextureMaps each default to 1, not template data.
- A missing/empty TextureMap_1 uses the registered material basename.
  RenderTarget and path-bearing texture names fail the bounded reader.
- TextureCoordinateIndex_1 defaults to 0; ShaderPassGroup defaults to `none`.
- `inputAlphaD` is a compatibility alias read at 247A7C, followed by
  `inputColorD` at 247A94. A present inputColorD overrides the alias. This is
  distinct from duplicate-spelling first-present precedence.
- The one-map builder starts environmentContext0or1 at 0 independently of the
  template (247C88), then reads an authored override.
- TEX1 fields are pass-local defaults: LCM=0, MXL=0, MMAG=1, MMIN=5, L=0,
  K=-75 (247F38..247FE8). They are not inherited template fields.
- When actual 435670 is nonzero, the builder overwrites inputAlphaC=2,
  fixedAlphaValue=16, fog=0, alphaBlend=1, depthMethod=1. The native caller must
  bind a real owner for this source flag; absence is not assumed zero.

The full authored pass section is retained for animation/group owners and for
strict native-program admission. This stage does not implement material
callbacks, ShaderPassGroup enablement/order, texture animation advancement,
multi-map programs, fill-only materials, or generalized render-target lookup.

## Typed native policy and renderer boundary

`prepareOriginalUiNativeMaterial50KG` consumes a resolved pass and its exact
decoded original Texture48E atlas. It admits enabled, single-map ScreenObject,
UV set 0, one stored indexed-RGBA mip, and original RGBA MODULATE. Palette data
and geometry bytes remain raw. Source policy is conservatively restricted:

- COLCLAMP=1, Gouraud=1, texture=1, FST=0, FIX=0.
- Context must be 0. The one-map builder writes TEX0_1/ALPHA_1/CLAMP_1/
  TEST_1/TEX1_1/ZBUF_1 addresses regardless of authored PRMODE context. A
  context-1 selection would consume the other context's separately owned state;
  retaining its integer alone would not establish the effective native policy.
- Fog, antialiasing, PABE, FBA, alpha testing and destination-alpha testing are
  rejected when enabled; they do not disappear as implied UI defaults.
- Wrap 0/1 maps to repeat/clamp-to-edge. Region clamp/repeat is rejected.
- MXL must be 0. For the admitted one-level atlas, MMAG 0/1 is nearest/linear;
  MMIN 0/2/3 uses nearest texels, 1/4/5 uses linear texels after level clamping.
  Effective minification and magnification texel filters must match: MXL=0
  does not independently prove that GS LCM/L/K chooses min versus mag in the
  same way as desktop GL. Equal filters remove that unresolved dependency;
  the original default MMAG=1/MMIN=5 pair qualifies as linear.
- Opaque RGB is admitted when original ABE=0. For ABE=1 the bounded tuples are
  (A,B,C,D)=(0,1,0,1), source-over, or (0,2,0,1), source-alpha additive.
  Fixed/destination alpha factors and other equations are explicit failures.
- Source texture alpha must be <=128. The caller must likewise preflight every
  actual raw vertex alpha <=128. This keeps the original source-alpha factor
  within the representable 0..1 native blend-factor range.

The result returns exact GS depth method (0 never, 1 always, 2 GEQUAL,
3 GREATER), depth-write enable, environment context and sampler requirements.
Depth cannot be replaced by painter order without a separate source proof.
The current renderer's z=0/color-only target/disabled depth is not certified by
this CPU policy. `requiresOriginalDepthOwner` remains true unless depth is
always and writes are disabled.

For normalized raw RGBA8 vertex/texture inputs, the GS MODULATE relationship
requires RGB scale 255/128. Raw stored fragment alpha requires the same scale,
but the GS RGB blend coefficient divides that raw alpha by 128 rather than
255. Therefore:

- colorScale = 255/128
- storedAlphaScale = 255/128
- blendAlphaScale = (255/128)^2
- alphaStoreFromBlendFactor = 128/255

ScreenTexture's original RGB>>1 and alpha>>2 packet conversion is already
performed by its source geometry owner. Do not undo those shifts separately or
apply these scales twice. CEL/font owners likewise supply their original raw
packet bytes.

When a fragment shader emits alpha in blend-factor units, the source stores
raw alpha independently of RGB blending. An appropriate native RGBA8 mapping
needs separate-alpha CONSTANT_ALPHA/ZERO with constant 128/255. Opaque output
uses storedAlphaScale directly. The existing UI span alphaScale affects both
fragment alpha and blend factor, while current draw uses one blendFunc for all
channels. That code does not preserve the original framebuffer alpha merely
because its RGB looks right. The typed policy deliberately reports
`requiresSeparateAlphaStore`; platform integration must supply the missing
owner or explicitly reject. MODULATE color must also clamp before blending.

The existing MaterialTextureGl50EB owner already accepts one explicit sampler
per immutable bank texture; it has no inferred sampler defaults. Map this CPU
policy's sampler enums at cold GPU creation. If the same image is required with
conflicting policies, it needs separate compatible bindings/owners; never
mutate the published texture's parameters during draw.

These are bounded native floating-point shader/blend semantics, not a claim of
bit-exact PS2 integer rasterization. Preparing CPU policy is not GPU readiness
or confirmation that a pending original material/atlas has passed admission.

## Immediate missing inputs and integration

The smallest missing asset owner is actual APPSTART ScreenMT bytes for UI
inheritance, plus each actual UI material and texture. Constructor-only state
must not substitute for the real file override. Authenticated UIMAIN retains
mmbkg and several font material identities that the historical importer
rejected for duplicate keys. This reader supports genuine first-present keys,
but those unseen contents may still request unsupported shader/state features.

Recommended cold sequence:

1. Authenticate real archives with OriginalUiArchive50KG.
2. Load both APPSTART templates.
3. Resolve each actual PSM with explicit source template/runtime ownership.
4. Decode the actual named PST/ATM frame and prepare typed CPU material policy.
5. Join actual ShaderPassGroup, animation, depth and alpha-storage owners before
   converting policy into platform spans/samplers. Do not declare a complete
   original frontend from the CPU resolver alone.

Frozen prior catalog, atlas, ATM and pages packets remain untouched. This
stage's patch contains only additive material files; provenance authenticates
source intervals, static strings/pointers, catalog receipts and dependencies.
