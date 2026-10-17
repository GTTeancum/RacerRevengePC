# RA raw-position surface scope

Status: independently source-reviewed; UNTESTED.
Base cc6ff68. No builds, tests, syntax checks, gameplay,
profiling or benchmarks; no measured performance claim.

The public per-vertex rawPosition path repeatedly reads the current camera/fog
publication, validates the world/image/collector lifetime, locks the actual Bank
lease, copies camera observations, then resolves the same source placement, scene,
LOD and surface. These are stable within the renderer's synchronous selected
surface preparation. The original coordinate recovery remains per vertex.

## Bounded change

A private fog Binding visitor performs the same current(camera) and source-geometry
lookup before lending a local callable for one selected surface. The callable
invokes the shared recoverRawPosition numeric leaf with the actual retained scene
and surface. Successful visitation rechecks current(camera). The private aggregate
forwarder is accessible through the existing renderer friendship only.

The renderer acquires this scope only when the actual source fog-byte demand and
placement fog predicate are both true. It nests inside the diffuse borrow when
lighting is required. A no-fog surface uses the same loop without a fog acquisition
and cannot invoke its unused reader. No pointer, callable, camera or geometry
result is cached beyond the synchronous callback.

## Exact arithmetic and observation proof

recoverRawPosition contains the previous numeric body unchanged, apart from scene
reference access: vertex extent/bone checks, finite positive q, per-axis ordered
subtraction/division, signed24 range, truncation, decoded roundtrip and adjacent
ambiguity checks, integer conversion, and final out=next only after all axes.
decodedPosition retains its volatile double product. radialByte is untouched.
The public rawPosition retains its original current check and lookup before
calling this same helper; its public aggregate forwarding API is unchanged.

The fog Binding strongly pins the world and camera; the world retains the actual
payload/scene, which has no mutation in this scope. Renderer callback operations
are native frame-vector writes, unchanged RGBA copies and pure numeric recovery/
fog operations. They do not allocate, service materials, run GL or call simulation,
owner retirement or publication replacement. Thus the pre-scope source identity
remains identical at the post-scope current check. This relies on the existing
synchronous owner-thread contract, not an invented lock or concurrent guarantee.

## Rejection and publication

Source/current admission rejection and post-scope current failure retain the
original raw-position/fog diagnostic. Reached callback failures preserve their
specific diagnostic through both nested scopes. Pre-scope rejection can precede
the first frame color write; the failed frame remains unprepared either way.
Whole diffuse-material invalidation, exact camera generation/publication/frame,
all per-surface fog observations, final frame checks and draw-time checks remain.
