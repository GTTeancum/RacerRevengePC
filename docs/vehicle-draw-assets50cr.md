# Vehicle draw asset ownership 50CR

Status: new source only, **uncompiled and untested**. No compiler, build, test,
executable probe, validation script, renderer or guest CPU was run for this
addition. The team-wide test allowance is controlled by the parent task.

`src/assets/vehicle_draw_assets50cr.{h,cpp}` provides the missing cold ownership
path into `prepareVehicleDrawMesh50CM` and `buildVehicleDrawBatch50CO`. It does
not change an existing asset format, parser, material policy, target or test.

## Cold inputs and ownership

The caller constructs one nonmoving `rr::assets::VehicleDrawAssets50CR` at its
final lifetime location, fills `VehicleDrawAssetsInput50CR::component` in fixed
chariot/left-engine/right-engine order, and calls `load` once. Every component
supplies:

- A pointer to the existing RRSCENE1 byte vector. `parseSceneAsset48D` retains
  the semantic scene and validates its embedded compatibility export. The
  adapter passes that scene's own `neutral_lod0` to `parseMesh48B`, retaining a
  separate `Mesh48B`. No potentially mismatched external mesh is substituted.
- The actual model+1B8 source-index table resolved into canonical native graph
  indices. The table order is PSG source order, not motion/VUT order.
- The actual selected PSG palette bytes and count. The adapter copies those
  bytes and admits only the identity palette supported by 50CM. It does not
  fill a missing palette or invent source-node ranges.

Input vectors and binding arrays need remain alive only through `load`. Their
values are copied into the owner. It owns all three scenes, compatibility
meshes, exact-sized local rigid arrays, source tables, selected palette bytes
and two exact-sized CPU vertex banks per component. Only vertex banks are
mutable after a successful load. Asset/material accessors return const views.

Cold admission checks the binding extents and node-index bounds, then reuses
50CM's geometry/identity-palette/material-index checks. Binding provenance is
still a caller responsibility. Matching a source table to the canonical model
root is checked by 50CM when a concrete current frame is built; a successful
cold load alone is not an attestation of a current graph or graphics phase.

All three components commit together by swapping an owning pointer after all
preparation and allocation succeed. Descriptors point into allocations at
their final addresses. A rejected or allocation-failed load leaves an empty,
retryable owner. A second load on a successful owner rejects and retains its
existing data/views. There is no reload, reset, copy or move operation.

The established retained Pod01 fixture has 18/14/14 palette entries and source
nodes 31..48, 1..14 and 16..29, respectively. Those are fixture facts only; this
API does not hardcode them. Obtain the real tables from the caller's prepared
model bindings. The original extraction in
`tools/visual/export_chariot_lifecycle49v.cpp` reads and verifies the actual PSG
suffix palette before projection. It is evidence for the fixture, not a new
runtime parser invoked by this adapter.

## Runtime consumption

At the explicitly selected render-preparation cut:

1. Choose a CPU bank not used by the currently published draw or a pending
   upload. On the first frame either bank is available.
2. Call `owner.batchInput(bank, selectedRenderPreparation,
   completeCurrentScene, input)`. This only copies three descriptors, sets the
   explicit caller attestations, and binds already allocated staging arrays.
3. Call `buildVehicleDrawBatch50CO(frame, bindings, input, candidate,
   workspace, diagnostic)` with the canonical current frame and its bindings.
   Keep the reusable workspace and diagnostic in separate caller storage.
4. Only if the call succeeds, publish `candidate` and its bank and submit all
   three components. On failure, keep the old published batch and bank. The
   rejected bank may contain a partially rebuilt vehicle and may be retried.
5. After every reader/upload has released the previous bank, it becomes the
   next available staging bank. If both banks are still in use, the caller
   must defer a build or finish a reader; the adapter does not schedule or wait.

`batchInput` and the existing CM/CO build path do not allocate. `batchInput`
preserves its output when unloaded, given an invalid bank, or given unsupported
phase/state tags. All data referenced by a successful batch stays valid until
the corresponding bank is reused or the owner is destroyed. Do not reuse a
published bank merely because the batch descriptor itself is elsewhere: that
descriptor still borrows the bank's vertex memory.

The caller must retain the owner until all scene/mesh views, batch descriptors
and renderer consumers are finished. Serial access is required. In particular,
loading/preparing does not create synchronization or certify lifetimes of
copied raw views. No hot resizing, mutation of semantic geometry, root-pose
replacement, or bind/root transform reapplication is performed.

For material resolution, consume the retained `scene(component)->materials`
and existing span material indices through the existing cache/material owner.
For renderer buffer creation, `preparedMesh(component)` supplies the exact
active counts and immutable indices. `compatibilityMesh(component)` exposes
the original parsed mesh. Texture loading, authored material interpretation,
camera selection and device buffer creation/upload remain existing integration
responsibilities. Neither staging bank has valid live-frame positions until a
successful CM/CO build writes it.

## Registration and limits

Add `src/assets/vehicle_draw_assets50cr.cpp` to the chosen target together with
the existing `scene_asset48d.cpp`, `native_asset48b.cpp`, `vehicle_draw50cm.cpp`
and `vehicle_draw_batch50co.cpp` dependencies. Retain the strict floating-point
flags and existing `model_palette49s.cpp`, `rigid_vertices49s.cpp` and
`scene_graph49p.cpp` dependency chain. No platform or filesystem API is needed.
Cold calls also require FE_TONEAREST through the original preparation contract.

Cold semantic/compatibility owners retain the existing parser limits. New
rigid/staging allocations are bounded to 4,096 vertices per model; the CM
surface bound remains 64. Double staging costs exactly two `Vertex48B` arrays
per component, in addition to the retained parsed compatibility vertices and
local rigid positions. No memory-size, performance, render or platform result
has been measured for this addition.

Existing files/build registries/tests were not edited. Sealed49W, held48U,
excluded49C, unapproved `ee.bin` and the user's PC remain outside this work.
