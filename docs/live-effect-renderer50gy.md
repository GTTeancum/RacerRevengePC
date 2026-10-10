# Explicit live effect renderer (50GY)

## Scope

`vehicle_window_gl50ds.{h,cpp}` admits explicit material blend/depth policy,
animated UV scale/offset for the existing pod mesh, and a separately supplied
actual authored sky mesh. `vehicle_shader_gl50cz.{h,cpp}` provides the associated
shader uniforms. No physics, controllers, timestep, source mesh positions,
source UV/color bytes, CV submission contracts, or original material identities
are rewritten by this layer.

The default remains opaque, depth-writing, existing RGB shading. Newly linked
50CZ programs also have effect/UV/suppression selectors defaulted to zero so
existing direct/offscreen clients retain their old behavior.

## Explicit materials

Each `VehicleWindowMaterialBinding50DS` adds:

- `blend`: `opaque`, `sourceAlpha`, or `additive`.
- `depthWrite`: independent of blend, default true. Depth test remains LESS.
- `alphaScale`: finite `[0,8]`; opaque requires 1.
- `fixedBlendAlpha`: only admitted for additive materials.
- `unlit`: for track, bypasses host geometric illumination while retaining
  the caller's existing `vertexColorScale`; pod RGB was already unlit.

Source-alpha modes preserve sampled texture alpha times normalized source
vertex alpha, then multiply by explicit `alphaScale` and clamp to `[0,1]`.
Untextured effects use texture RGBA `(1,1,1,1)` and source vertex RGBA.
SourceAlpha uses SRC_ALPHA/ONE_MINUS_SRC_ALPHA. Additive uses SRC_ALPHA/ONE.
Fixed-alpha additive multiplies source RGB by `alphaScale` and uses ONE/ONE,
which admits a factor above one without the GL alpha blend-factor clamp.
Both blended modes explicitly select FUNC_ADD. Opaque alpha remains 1.
The independently requested zero-alpha texture discard stays available.

The caller can select `(255/128)^2` to interpret modulated source texture and
vertex alpha in a 128-unity convention, or fixed additive `255/128` for a
source-authenticated FIX=255 material. These are explicit caller decisions,
not an inferred original VU/GS shading implementation. Texture samplers remain
exactly the selected immutable 50EB publication; the renderer creates no
sampler or texture override. Flame U-repeat/V-clamp and sky edge-clamp belong
to their separate texture-bank upload requests.

## Order and transaction

Within the same pending private framebuffer:

1. Optional actual sky, source span order, depth testing/writes disabled.
2. Opaque track, existing retained culling candidate order.
3. Opaque pod, original CV component/span order.
4. Existing source cable geometry.
5. Selected blended track, retained source order.
6. Selected blended pod, original component/span order.
7. Existing host HUD.

The original CV adapter still validates and submits every original pod span.
A selected blended pod material is fragment-discarded during that first pass:
no color or depth is written. Before commit, its original immutable span and
EBO, uploaded pending-bank VBO/VAO, and base vertex zero are reused for the real
effect draw. No substitute geometry or second vertex upload is introduced.
All deferred drawing must succeed before CPU publication. Abort preserves the
previous private image; framebuffer-zero presentation remains outside commit.

This is a stable two-pass host policy, not recovered original global sorting.
No camera-distance sorting is claimed. Intersecting transparent surfaces and
source materials which explicitly write depth can still be order dependent;
the renderer does not silently disable their requested depth writes.

## Per-frame UVs

`VehicleWindowFrame50DS::materialUvs/materialUvCount` supplies a bounded array
of `VehicleWindowMaterialUv50DS` entries, strictly ascending by original
component then material index. Each entry contains U/V scale and offset.
Every value and identity is checked before frame GL work. Entries are copied
into existing cold material storage; omitted entries reset to identity each
frame. Caller storage is not retained. Current original material textures,
source vertex colors, and source positions remain unchanged.

## Actual sky interface

`VehicleWindowRequest50DS::sky` supplies a real Mesh48B, explicit vertex/index
byte ceilings, a current texture bank, and an exact contiguous source span
partition. As for the track, all positions/UVs and indices are revalidated,
uploaded bytes and buffer sizes are verified, and a matching Vertex48B VAO is
checked. At most 65,536 spans are accepted. Sky materials must explicitly be
opaque, unlit, and non-depth-writing, with zero-alpha discard disabled.

`VehicleWindowFrame50DS::skyWorldViewProjection` is required exactly when a
sky was uploaded. It independently follows the existing row-vector,
column-major GL convention, is finite-checked and copied, and is consumed
without inferred camera translation, scaling, or far-plane changes. Actual
sky rendering may therefore use the caller's translation-free sky view and
appropriate projection without changing the world/track camera. The same
bank publication, creating thread, live context, and callback guards apply.
Sky resources are owned and cleaned up with the renderer; CPU mesh/span
pointers are borrowed only during creation.

## Storage, diagnostics, and verification

All added allocation is bounded and cold: richer original material records
(at most 3*4096), sky span records (at most 65,536), and explicitly budgeted
sky VBO/EBO/VAO. Submission has no application heap allocation or sort buffer.
Existing texture/context/publication/cleanup guards are retained.

Diagnostics include checked `transparentTrackDraws`, `transparentPodDraws`,
`transparentPodIndices`, `skyDraws`, and `skyIndices`. These count accepted GL
commands, not visible pixels or presentation; require `batchAccepted` for a
committed private image. Transparent track commands also count in the existing
track totals. CV's ordinary submission receipt includes its discard-only pod
commands; the explicit transparent pod receipt distinguishes actual replay.

Validation performed for this renderer change: C++17 syntax-only compilation
with `-Wall -Wextra -Wpedantic` on both modified translation units, without
warnings. Runtime rendering and the single focused integrated drive are owned
by the integrating task; no broad suite or extra smoke run was started here.
Preimages are under `solo-render50gy/preimage/`, retaining repository-relative
paths.
