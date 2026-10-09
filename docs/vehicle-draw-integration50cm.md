# Vehicle draw bridge 50CM

Status: new source only, **uncompiled and untested**. No executable, test,
compiler, sanitizer, validation script, guest CPU or renderer has been run.

## Concrete connection

`src/game/vehicle_draw50cm.{h,cpp}` connects the canonical `VehicleFrameState49W`
graphics nodes to the existing Windows viewer's exact draw inputs:
`Vertex48B` (24-byte XYZ/UV/RGBA), `uint32_t` indices and `SceneSpan48D` ranges.
It calls `projectModelPalette49S` and `projectRigidVertices49S` on a disposable
53-node graph snapshot. It does not refresh or overwrite canonical frame caches,
replace graphics roots with postphysics bodies, or append a second placement.

The viewer currently owns its D3D11 calls inside
`platform/windows/native/main48b.cpp`; it has no reusable render-submission API.
This bridge stops at fully populated caller-owned inputs for those calls. It
does not claim rendered gameplay, frame rate, original shader parity or a
completed Windows integration.

## Integration

1. Add `src/game/vehicle_draw50cm.cpp` to the chosen native target. Link the
   existing `model_palette49s.cpp`, `rigid_vertices49s.cpp` and
   `scene_graph49p.cpp` implementations, with their existing strict floating-point
   options. No new asset parser, guest executor or device dependency is needed.
2. Load each selected semantic RRSCENE1 and parse its existing `neutral_lod0`
   bytes into a separate `Mesh48B` once. Retain both objects. Allocate each
   model's `RigidVertex49S` and destination `Vertex48B` arrays once, at that
   model's LOD0 vertex count. Call `prepareVehicleDrawMesh50CM` once. It verifies
   semantic-to-compatibility UV/color/index/span agreement, preserves existing
   material indices and copies only local rigid position/bone data. It admits
   object-space scenes with identity palettes, at most 4,096 vertices and 64
   surfaces per model. Missing original colors are accepted only when the
   existing compatibility mesh already contains its documented white fallback.
3. Keep authentic source-index tables and actual PSG palette IDs per model.
   For the current retained Pod01 graph, existing exporters use chariot source
   nodes 31..48, left engine 1..14 and right engine 16..29, with identity palette
   IDs and counts 18/14/14. Those are integration facts for this graph, not
   inferred defaults in the new API. The corresponding component vertex counts
   are 2,884/1,411/1,460; index counts are 4,998/2,835/2,880.
4. At the explicitly selected render-preparation observation cut, call
   `buildVehicleDraw50CM` for chariot, left engine and right engine. Supply
   `selectedRenderPreparation` and `completeCurrentScene` only when true. The
   frame must be `PostGraphics` or completed `Ready`; the latter does not itself
   establish that graphics ran. Derivation uses canonical component flags and
   model roots. A flag191 bit01-clear model rejects rather than pretending a
   visibility decision succeeded. Use one reusable workspace serially and
   separate persistent output arrays/descriptors per model. If any call fails,
   do not submit a partial new vehicle; inspect the nested49S diagnostic.
5. The viewer creates its initial vertex buffers as `D3D11_USAGE_IMMUTABLE`.
   A live client must create the vehicle vertex buffers once as updatable
   `D3D11_USAGE_DEFAULT` (or explicitly manage a dynamic-buffer path), retain
   immutable index buffers, and upload each successful output's active
   `vertexCount * sizeof(Vertex48B)` bytes each displayed frame. Use its existing
   POSITION/TEXCOORD/COLOR input layout, R32_UINT index format and triangle-list
   topology. For each retained span, bind the model's existing resolved material
   then call `DrawIndexed(span.index_count, span.index_start, 0)`. Bind each
   model's own vertex/index buffers at offset zero. Positions are world-space;
   the camera matrix is view/projection only. Material resolution and camera
   choice remain with their existing owners.

Prepared views borrow their backing arrays and scene/mesh vectors. Do not move,
resize or mutate those owners while the views are in use. Destination arrays,
scratch, diagnostics, input arrays and descriptors must be disjoint. Cold and
per-frame failures leave published output and destination arrays unchanged;
always check the returned bool before using them as a new frame. Successful
output contains no pointer into the reusable workspace.

Existing source/export references inspected: `vehicle_frame49w.cpp`,
`model_palette49s.{h,cpp}`, `rigid_vertices49s.{h,cpp}`,
`tools/visual/export_chariot_lifecycle49v.cpp`,
`tools/visual/export_vehicle_frame49w.cpp`, and
`platform/windows/native/main48b.cpp`. No existing source, CMake file or test was
edited. Sealed49W, held48U, excluded49C and unapproved `ee.bin` remain outside
this adapter's work.
