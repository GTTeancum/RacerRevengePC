# Capture-only visibility evidence 50FO

`tests/native/capture_visibility50fo_fixture.h` is a read-only development-host
diagnostic. It neither changes presentation nor supplies collision response.
It makes no recovered-game parity claim.

## Calling contract

Call `capture_visibility50fo_fixture::inspect(trackMesh, batch, camera, report,
diagnostic)` only when a screenshot is requested, using the actual world-space
track mesh, the exact successfully submitted `VehicleDrawBatch50CO`, and the
camera used for that submission. Retain those inputs for the committed capture;
do not combine a last-frame image with a next-frame batch or camera. The caller
owns capture gating. This helper must never become an ordinary per-frame scan.

The caller may retain the CPU track mesh solely in its headless capture/debug
route. No scene, physics, rendering, or camera state is written. Output and
diagnostic must have distinct storage and must not alias inputs.

## Results

- `cockpitCenter` is the center of component 0's actual submitted world AABB.
  It is a diagnostic sample, not a center of mass or guaranteed mesh interior.
- `cameraEye` is the submitted camera's eye.
- `cameraToCockpit` is the nearest two-sided triangle intersection on the
  closed eye-to-cockpit-center segment. Its `t` is normalized: 0 is the eye and
  1 is the center. Endpoint hits are reported and are not automatically evidence
  of an obstruction strictly between the two points.
- `cockpitVertical.above/below` and `cameraVertical.above/below` independently
  report the nearest surface crossing in each vertical direction. Here `t` is
  nonnegative world distance; `point` gives its world coordinates. A crossing
  exactly at the origin is reported in both directions.
- Every `Hit` has `hit`, `t`, zero-based original triangle `triangle`, and
  world-space `point`. Missing hits have `hit=false`; their numeric fields are
  placeholders, not measurements. Exact-distance ties keep original triangle
  order. `trianglesScanned` records the completed scan size.

An above hit may be a ceiling, bridge, wall overhang, or floor above an embedded
sample. A below hit is merely the nearest surface below the sample. Neither is
classified as drivable ground. Compare triangle identity, world position, both
vertical directions, and the screenshot before inferring terrain penetration.

The test ignores material alpha, textures, and GPU rasterization. A center-line
hit is geometric evidence, not proof that every cockpit pixel is occluded. A
miss likewise does not prove complete visibility. Coplanar/parallel lines and
degenerate triangles have no unique crossing and are skipped. Near-parallel
triangles use a double-precision relative determinant tolerance; shared edges
have a small barycentric tolerance. The zero-length segment gives no crossing.

## Bounds and failures

There is one finite scan of the existing triangle index array, capped by the
existing `max_mesh_indices` (3,000,000 indices / 1,000,000 triangles), with three
line/triangle evaluations per triangle and constant extra storage. No sorting,
heap allocation, mutable cache, callback, or external query is added.

`inspect` rejects empty/non-triangular/oversized index arrays, invalid indices,
nonfinite referenced vertices, invalid cockpit bounds, invalid camera, and
nonfinite arithmetic. It returns false with `Diagnostic::error` and, for
triangle-specific errors, the offending triangle. The previous `Report` is
preserved on failure. A failed or missing report is not an unobstructed result.

## Validation status

Implementation was source-reviewed only. No build or test was run for this
change. Suggested focused cases are unobstructed center, a blocking triangle,
surfaces independently above/below each origin, a ceiling-only hit, shared-edge
ties, parallel/degenerate triangles, invalid input, and unchanged output on
failure. Runtime integration and capture synchronization belong to the caller.
