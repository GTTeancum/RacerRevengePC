# Bounded static-track frustum culling 50GD

The Linux 50DS renderer has an optional host visual optimization using the
existing conservative [50EI helper](draw-visibility50ei.md). It is disabled by
default. Set `request.track.enableFrustumCulling = true` before `create()` to
enable it for that renderer lifetime. Enabling it without an actual track mesh
is rejected. This is not recovered original visibility, occlusion or LOD policy.

## Cold geometry and storage

The renderer first performs its existing complete mesh/span/material validation.
For a textured track, each original ordered source span is one candidate draw,
including explicitly untextured spans in that table. Its world AABB is built
only from the actual vertices referenced by that span's unchanged index range.
No serialized display bounds, collision proxies or unreferenced vertices are
used. Vertices are already world-space; no transform is reapplied.

Neutral and source-vertex-color tracks still have one whole-mesh indexed draw,
so they get one AABB over all indexed vertices. This integration does not invent
extra subdivisions for those modes.

With culling enabled, cold allocation owns one `StaticDrawBounds50EI` and one
`uint32_t` retained ID per candidate. Both arrays have the complete candidate
capacity, at most 65,536 entries. The combined element storage is 32 bytes per
draw on the current binary32/uint32 layout, at most 2 MiB, plus allocation and
small owner overhead. Culling disabled allocates neither array. The original
track upload and material table are unchanged; there is no second geometry copy
or extra GPU resource. CPU input pointers remain borrowed only during create.

## Per-frame selection and drawing

After validating frame inputs, `submit()` copies the camera matrix into the
existing renderer-owned 16-float array. Selection passes those exact bytes to
`collectVisibleStaticDraws50EI`; the same array is supplied to both applicable
`glUniformMatrix4fv(..., GL_FALSE, ...)` uploads. The shader's row-vector,
column-major world-to-clip convention is unchanged. Selection explicitly uses
`negativeOneToOne` clip depth and `retain` for uncertain arithmetic.

50EI rejects a box only when all eight corners are provably outside one common
standard homogeneous clip plane, with conservative binary32 error allowances.
Plane-touching, intersecting and numerically uncertain draws remain. The existing
frame setup disables depth clamp and user clip distances. Track vertex shaders
apply no displacement beyond the supplied world-to-clip matrix.

Each frame starts with the complete source list selected. Only a successful
helper result publishes the retained count and switches iteration to its output
IDs. A helper error, including unsupported rounding/fast-math arithmetic, retains
the entire original list and records the failure. No partially written prefix
or previous frame's selection is consumed. Invalid frame matrices still follow
the existing frame rejection path before selection.

Retained spans are drawn in their original order with the original material,
alpha-discard flag, index count and EBO offset. Full material-publication checks
still cover the original table, including spans selected out. When every track
draw is culled, track submission is skipped while the private target clear,
vehicle, cables, HUD and transactional publication proceed unchanged. No pod,
cable, HUD, collision, registration or simulation visibility is filtered.

The cold indexed-vertex pass is linear in the total index count. Frame selection
is bounded by the candidate count, with the existing helper's eight-corner
classification plus a retained-index counter pass. There are no application
allocations, sorting, callbacks, geometry uploads or GPU readbacks in selection.

## Diagnostic receipts

New `VehicleWindowDiagnostic50DS` fields describe CPU selection for a frame that
reaches selection, even when culling is disabled:

- `trackCandidateDraws`: original draw count.
- `trackRetainedDraws`: selected count; candidate minus culled.
- `trackCulledDraws`: successfully excluded candidate count.
- `trackUncertainRetainedDraws`: helper-classified arithmetic uncertainty retained
  on a successful call, a subset of retained. Ordinary touching is retained but
  does not increment this field.
- `trackCulledIndices`: total indices in excluded original draws.
- `trackCullingApplied`: the helper succeeded and its complete result was used.
- `trackCullingFallback`: the helper failed and the complete source list was used.
- `trackVisibility`: exact helper error and failing input ordinal, if any.

Disabled culling reports all candidates retained and no applied/fallback flag.
Fallback reports all candidates retained, zero culled/uncertain counters, and the
helper diagnostic. Counts reset with every public-call diagnostic. A frame that
fails validation before selection leaves these fields zero.

The existing `trackDraws`, `trackTexturedDraws`, `trackUntexturedDraws` and
`trackIndices` still count only GL commands that passed the renderer's error
checks. Retained counts are not a substitute for those submission receipts.
Require `batchAccepted` for private image publication; neither family of counters
measures visible pixels, GPU duration or presented frame rate.

## Integration and verification boundary

Any target linking this renderer must also link `draw_visibility50ei.cpp`, with
its existing strict floating-point compilation contract. The native gameplay
library already lists that source. Client option handling and reporting are
separate integration work.

This renderer change was reviewed in source only by its implementation worker;
no build, test, capture or performance run was performed by that worker. Focused
integration checks should compare identical frame/camera sequences with culling
off/on, inspect equal output pixels and unchanged pod/cable/HUD receipts, and
compare submitted draw/index counts and completed-frame timing. Cover fully
retained, partly culled and fully culled lists; camera movement; boundary/eye
crossings; whole-mesh modes; and a helper failure retaining the entire list.
Performance and playable benefit require those actual measurements.

## Interactive client integration

The current 50DU client explicitly enables this host optimization. Set environment variable `RR_DISABLE_TRACK_CULLING=1` before launching to retain the same scene and physics with all track spans submitted. Live telemetry prints candidate, retained, culled and fallback counts. The API default remains disabled for other consumers.
