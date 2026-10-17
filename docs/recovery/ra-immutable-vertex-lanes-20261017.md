# RA immutable native vertex lanes

Status: independently source-reviewed; UNTESTED. No build, syntax check,
test, gameplay, profiler or benchmark was run. No measured performance claim.
Base: 02e69a0.

## Change

Copy the actual retained world's three position and two UV float lanes once in
`createRaWorld`, immediately after both existing frame-vector allocations succeed.
Remove only those same two copies from the selected-vertex preparation loop.
All dynamic colors, fog bytes, current source observations, crowd-map service,
selected surface ordering, ownership validation and GPU uploads remain unchanged.

## Immutable source and lifetime proof

`tests/native/ra_static_world50la.h::lowerPlacement` computes worldPosition and
sourceUv from the authentic cold scene and placement, checks finite values, then
appends each Vertex to private vertices_. This is its sole producer; the public
vertices() getter returns a const vector. Neither crowd image selection nor the
lighting/fog observation paths mutate this geometry bank. The renderer retains
the same Owner and its renderer publication; Owner::canClose rejects a live
renderer or image publication. Existing raWorldCurrent checks still authenticate
the aggregate, owner, draw plan and actual GPU image lease each frame.

Only createRaWorld and the removed frame copies write native position/UV lanes.
The frame loop continues writing color/fog. Initializing all vertices earlier can
change bytes for previously unselected vertices, but indexed draws remain bounded
by the cold-validated selected surface ranges, and their dynamic operands are
prepared before use. No new vertex or surface is admitted.

## Creation and failure behavior

Initialization follows the existing allocation try/catch and precedes GPU
allocation. A failure of either vector allocation still returns through the same
error path without starting the new copy loop. Float-to-float copies do not
allocate or introduce arithmetic. Every creation repeats the copies, including
recreation with equal vector size, so no previous owner contents are reused.
Failed partial GPU creation retains the same existing resource cleanup path.
No extra owner, cache, readiness state or callback is introduced.

This trades one cold five-float copy per vertex for removal of those copies on
every selected-vertex preparation. Full interleaved GPU buffer upload remains;
actual benefit is unknown until future authorized measurement.
