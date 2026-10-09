# Static draw asset preparation 50DW

Status: new source only, **uncompiled and untested**. This addition has not run
a compiler, build, test, runtime probe, validation script, renderer or guest CPU.
The parent task owns the shared test allowance. Existing build files and tests
are unchanged.

`src/assets/static_draw_assets50dw.{h,cpp}` supplies a cold, load-once owner for
actual retained static geometry. Its output is an immutable `Vertex48B` array,
global `uint32_t` triangle indices, ordered `SceneSpan48D` ranges and matching
source records. This connects retained RRSCENE1 assets to renderer-ready static
buffers; it does not itself register world objects or execute a renderer.

## Inputs and supported subset

Construct a nonmoving `rr::assets::StaticDrawAssets50DW`. Pass one
`StaticDrawAssetsInput50DW` containing:

- An asset table of `StaticDrawAssetInput50DW` records, each pointing to actual
  RRSCENE1 bytes. The file's own embedded RRMESH01 is the sole compatibility
  mesh. Separate potentially mismatched mesh bytes are not accepted.
- An ordered table of `StaticDrawInstance50DW` records. Each gives an explicit
  asset-table index and finite affine `std::array<double, 16>` placement matrix.
  This is the same layout and precision as `WorldInstance48F::matrix`. The
  matrix's default all-zero value rejects; identity must be supplied explicitly.
- Explicit `StaticDrawBudgets50DW` capacities, described below. These are
  ceilings; allocations use actual admitted counts instead of reserving every
  capacity.

The caller resolves authentic static visual entries and preserves its intended
instance order. The owner neither resolves filenames nor parses SCB/TOB. No
retained evidence paths, asset hashes, instance IDs or fixture-specific geometry
counts are hardcoded. Repeated instances of the same input asset are allowed.
Duplicate entries in the asset table remain distinct and each consumes source
budget; no deduplication changes caller identity.

The supported scene is exactly one root with parent -1, no matrix-palette flag,
all semantic bone IDs zero across all LODs, and the existing one/four-LOD layout.
It emits only LOD0. Multi-node scenes, matrix-palette/skinned inputs, nonzero
bone IDs, unsupported serialization and nonaffine matrices reject explicitly.
Animation provenance external to RRSCENE1 is not inferable; supplying only
static visual entries is a caller precondition. Moving actors are not frozen
into this owner automatically.

For world-quantized scenes, only an explicit identity instance matrix is
accepted, matching the recovered static-world subset. The quantization origin
is already present in semantic positions. No additional origin translation,
axis swap, recentering or inferred transform is performed. Arbitrary local-space
affine placement is an explicit caller input; this owner does not prove that a
given matrix came from source SCB orientation construction.

Standalone flattened RRMESH01 input is intentionally insufficient: it cannot
identify semantic local positions, root replacement or original per-surface
material membership. Collision-only entries are excluded by the caller; no
collision triangles or substitute visual geometry are generated here.

## Root replacement and scalar arithmetic

The saved PSG root is replaced by the supplied instance matrix. Its bind matrix
is not applied first. This follows the existing static assembly in
`platform/windows/native/import_service48f.cpp` and the recovered placement
contract documented in `docs/world-scene48e.md`. The latter records why applying
SCB placement to a bind-flattened compatibility mesh would double-transform
some authored static assets.

Positions come from `SceneVertex48D::position_local`. For each output axis `c`:

1. Start binary64 `sum = 0`.
2. For `k = 0, 1, 2` in that order, compute a separate binary64 product
   `position_local[k] * matrix[k * 4 + c]`, then a separate addition to `sum`.
3. Add `matrix[12 + c]` once.
4. Require every intermediate finite and the final value within +/-FLT_MAX;
   convert once to binary32.

Volatile intermediate stores separate product/add operations. The cold call
requires `FE_TONEAREST`; compile this source and its parser dependencies with
strict IEEE floating-point options and contraction disabled. Fast-math builds
are explicitly rejected by the source. This is the existing native static
display arithmetic contract, not a claim of original VU vertex-program or
RSQRT bit parity. Matrix rows are right/up/look/translation; basis xyz lanes
occupy 0..2, 4..6 and 8..10, with translation at 12..14 and affine W lanes
0, 0, 0, 1.

Each output vertex starts as an exact copy of the validated embedded mesh's
`Vertex48B`. Only position changes. UVs and all four color bytes are retained,
including the existing compatibility export's white bytes when original color
is absent. The span's `has_original_colors` flag remains unchanged, so this
compatibility convention is not misrepresented as original authored color.

## Budgets before allocation

All budget fields must be nonzero and may only reduce these hard ceilings:

| Budget field | Meaning | Hard ceiling |
| --- | --- | ---: |
| `assetCapacity` | Supplied asset-table entries | 4,096 |
| `instanceCapacity` | Supplied static instance records | 100,000 |
| `sourceByteCapacity` | Sum of all RRSCENE1 input byte lengths | 128 MiB |
| `sourceVertexCapacity` | Sum of semantic vertices over every source LOD | 1,000,000 |
| `sourceIndexCapacity` | Sum of semantic indices over every source LOD | 3,000,000 |
| `sourceSurfaceCapacity` | Sum of surfaces over every source LOD | 16,384 |
| `vertexCapacity` | Instantiated LOD0 output vertices | 1,000,000 |
| `indexCapacity` | Instantiated LOD0 output indices | 3,000,000 |
| `spanCapacity` | Instantiated LOD0 output surfaces | 100,000 |

Counts are summed with subtraction-before-addition overflow checks. Input
descriptor counts and aggregate source-byte limits pass first. The only early
allocation is a compact per-asset count array bounded by `assetCapacity`.
A nonallocating little-endian framing scan then walks every source file. It
bounds names/nodes/materials/LODs, all semantic vertex/index/surface counts,
the separate serialized span count, and the embedded mesh's vertex/index
counts and exact byte extent. The embedded mesh must already match the scanned
LOD0 counts. All-LOD totals must agree with the scene header. Every bone ID
is checked before semantic arrays can allocate.

The loader next checks aggregate source geometry budgets, every explicit
instance matrix and aggregate instantiated output budgets. **Only after all
these succeed** does it call `parseSceneAsset48D` or allocate vertex, index or
span arrays. This prevents an oversized embedded mesh or deceptive scene
header from bypassing the caller's capacities through an internal parser.

The framing scan is not a replacement semantic parser. The existing parser
still checks finite values, hierarchy/bind composition, original bounds,
surface/material validity, local indices, contiguous source spans, and exact
semantic-to-compatibility position/UV/color/index agreement. `parseMesh48B`
then retains that same embedded mesh. Parsed sizes are reconciled again with
the scanned counts before draw preparation.

These are geometry/count/input-byte budgets, not a promise that total heap
usage equals serialized bytes. The owner retains semantic scenes (all LODs),
their embedded serialized compatibility meshes, separately parsed compatibility
meshes, copied instances and aggregate placed arrays. The existing scene
validator additionally uses a temporary compatibility mesh during parsing.
Strings/vector/allocator overhead is implementation dependent. All these cold
allocations remain bounded by admitted input/count limits; no memory or timing
measurement has been run for this addition.

## Output and renderer integration

After `load(input, diagnostic)` returns true, `owner.draw()` returns a borrowed
`const StaticDrawView50DW*`. Arrays are in input instance order, then original
LOD0 surface order, then original vertex/index order. Degenerate triangles and
winding are preserved. No index welding, triangle reordering, normal-based
flipping or material sorting occurs.

Each span retains its original material index and color-presence flag, while
vertex/index starts are rebased into the aggregate arrays. Global indices are
`globalSurfaceVertexStart + originalSurfaceLocalIndex`. Use index type uint32,
triangle-list topology and `baseVertex = 0`. Upload the immutable geometry once;
do not transform positions by either the instance or saved PSG root again.
The vertex layout remains XYZ float3 at byte 0, UV float2 at byte 12, RGBA
unsigned bytes at byte 20, with 24-byte stride.

`view->spanSources[s]` identifies the original asset, instance and surface of
`view->spans[s]`. Resolve materials using:

```cpp
const auto& origin = view->spanSources[s];
const auto& span = view->spans[s];
const auto& materialName = owner.scene(origin.asset)->materials[span.material_index];
```

Material index alone is not a global material identity. The owner deliberately
does not merge or renumber separate scene material tables. Texture loading,
material/shader interpretation and resource bindings remain caller work.
`scene(asset)`, `compatibilityMesh(asset)` and `instance(index)` provide
immutable retained source access; invalid indices and unloaded owners return
null. `assetCount()` and `instanceCount()` return zero when unloaded.

All source bytes/descriptors can be released after successful loading. The owner
is noncopyable and nonmovable, has no reload/reset and never resizes its storage
after publication. Accessors make no allocations. Keep the owner alive until
all borrowed views, GPU uploads and renderer readers have finished. Admission
and lifetimes require serial access; this API introduces no synchronization.

If anything fails, the candidate storage is discarded and the owner remains
unloaded and retryable. No partial aggregate is published. An already-loaded
owner rejects another load without altering its existing data or pointer
identities. Diagnostics distinguish input/budget/framing/unsupported/matrix/
rounding/parser/arithmetic/allocation failures and report relevant source
indices. Existing parser messages are retained when available.

To integrate, add only `static_draw_assets50dw.cpp` to the selected target with
existing `scene_asset48d.cpp` and `native_asset48b.cpp` dependencies. The code
requires C++17 and no platform API, filesystem reader, graphics context or game
state. Caller-side authentic world instance resolution, resource creation,
camera selection, materials, texture loading, portal visibility, LOD scheduling
and render submission remain outside this layer. No gameplay, rendered-frame,
performance or platform-validation claim follows from these untested sources.
