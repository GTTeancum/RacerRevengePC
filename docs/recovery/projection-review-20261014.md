# Independent source review: private draw position scratch

Reviewed commit `06d70239fa06cd7fdb5cf55d13537a53ab3406cc` on 2026-10-14.

## Result and limits

Source-only support: no concrete defect found in the single-pass position
change. The private helper retains exactly the public projector's admitted
first-pass result, and the surrounding draw transaction permits scratch
mutation on rejection. This is not a build, runtime, performance, signed-zero
byte comparison, sanitizer, or gameplay result. No test, build, syntax check,
sanitizer, gameplay, executable, helper script, or commit was run for this
review. Only this review document was written.

The reviewed source files remained identical to the versions at `06d7023`
when compared from the current integration working tree. Unrelated working
tree changes were left untouched.

## Operation and rejection equivalence

Compared `projectDrawPositions` in `src/game/vehicle_draw50cm.cpp` with
`projectRigidVertices49S` and its `project` kernel in
`src/game/rigid_vertices49s.cpp`.

- Both initialize the nested diagnostic to `none, 0, 0, 0`.
- Both check null views, bounded nonzero input/palette counts, bounded output
  capacity, sufficient capacity, and `FE_TONEAREST` in the same order.
- Both validate every palette entry, including unused entries. For each entry,
  lanes 0 through 15 must be finite before affine lanes 3, 7, 11, 15 are
  checked against 0, 0, 0, 1.
- Each vertex checks its palette index before finite local XYZ lanes.
- Each axis starts binary64 `sum = 0`. For k = 0, 1, 2 it forms the same
  binary64 product using the exact promotion of the binary32 coefficient,
  checks the product, adds it separately, and checks the sum. Translation is
  added separately, checked for finiteness and inclusive +/-FLT_MAX bounds,
  and converted once to binary32.
- Every rejection selects the same error, vertex, matrix and lane. The outer
  rejection still sets `VehicleDrawError50CM::vertices` without overwriting
  that nested diagnostic.
- The helper publishes a vertex into scratch only after all three axes pass.
  It never publishes an incomplete vertex. Its only semantic change is that
  accepted prefix scratch can be modified before a subsequent rejection.

Under the existing stable, disjoint-storage and strict-FP preconditions, the
old successful second pass sees the same input, matrices and rounding mode.
Its result therefore matches the retained first-pass result, including signed
zeros. Both files retain fast-math rejection guards; the public contract also
requires contraction disabled. This review does not independently establish
the compiler or FP environment of the central Windows executable.

## Transaction, aliases, and retries

The 50CM header requires input records/backing arrays, destination vertices,
output, workspace and diagnostic to be distinct live storage with stable
lifetimes. The public 49S projector has the corresponding stable/disjoint
precondition. Aliasing position scratch with a later input vertex or palette
matrix is outside these contracts; the change does not introduce a supported
alias case that would invalidate the equivalence.

In `buildVehicleDraw50CM`, palette and optional packet matrix preparation,
positions, source colors, optional normals, and optional specular geometry all
finish before the destination vertex loop. There is no fallible return after
the first destination vertex write. Normal publication follows the vertex
loop, and `output = next` is last. Thus any admitted failure preserves
destination vertices, destination normals and the previous submission.
Workspace and optional model packet preparation slots may change, as already
documented. A successful retry recomputes every active position before use;
there is no readiness flag or retained pointer into position scratch.

The sole direct source caller found by repository-wide symbol search is
`buildVehicleDrawBatch50CO` in `src/game/vehicle_draw_batch50co.cpp`. It reuses
one workspace serially and sends each component's distinct staging destination
and its private `next.component[i]` submission to 50CM. A later component
failure can leave earlier staging arrays changed, but preserves the batch
descriptor. This is the batch header's existing explicit transaction boundary.

Caller ownership reviewed:

- `VehicleDrawAssets50CR::Storage::Component` owns separate immutable semantic
  scene, compatibility mesh, rigid local-vertex vector, two vertex staging
  banks and two normal staging banks. `batchInput` borrows these destinations;
  it does not point them into the supplied workspace.
- `presentVehicle50DA`, headless drive, and interactive player preparation use
  the inactive CPU bank and a private next batch before publication.
- The draw/controller/submit diagnostic drives use asset-owned alternating
  staging banks. The two static culling checks use a separately allocated
  workspace and asset-owned staging.
- `native_racer50HN::DrawOwner` copies source descriptors but substitutes its
  own per-component vertex and normal vectors. Its prepare method invalidates
  access to its batch on rejection. The interactive opponent path reuses the
  same draw workspace serially after player preparation; completed submission
  positions and normals reside in destinations, not in that workspace.

These inspected callers provide no concrete scratch/input alias defect. This
is source inspection of repository callers, not a universal guarantee about
external callers that violate the documented preconditions.

## Canonical state

The outer `state.nodes` copy into `workspace.nodes` is retained. The palette
projector also retains its separate private graph snapshot and refreshes world
caches there. Its successful cache publication goes to the draw's private
outer graph, never the canonical frame. Positions only read mesh locals and
the selected palette or optional packet slots. No new frame-cache, physics,
RNG, hierarchy, getter-order, normals, colors, packet-selection or material
mutation is introduced by this commit.

## Remaining central evidence

Central verification should establish strict-FP successful byte equality and
diagnostic/output-preservation behavior, particularly rejection after a valid
prefix, nonfinite/overflow/range cases, signed zeros and retries. Real roster
geometry, source-color/normal/specular routes and immutable canonical-cache
coverage remain relevant. A timing improvement requires a comparable Windows
profile; the source removal alone is not a measured performance claim.
