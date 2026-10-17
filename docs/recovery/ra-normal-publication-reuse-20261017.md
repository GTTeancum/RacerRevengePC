# Same-publication compressed normal reuse

Status: independently source-reviewed; UNTESTED.
Base b3c6c6b. No builds, syntax checks, tests, gameplay,
profiling or benchmarks. No measured performance benefit is claimed.

Binding::prepare already recovers every lit source normal to validate its signed
8-bit representation, then discarded that result. prepareDiffuseStream repeated
that recovery from the same immutable scene before projecting dynamic RGBA.
This change retains the successful first results inside that same Binding.

## Ownership and identity

A private flat vector holds the actual three-byte CompressedNormal values.
A private parallel surface vector records the actual retained Scene48D and
SceneSurface48D pointers plus bounded first/count into that flat vector. No
public Snapshot field or borrowed API changes. Binding retains its actual world,
which retains the model payload and const source scene; these source normals and
quantization have no mutable publication path.

prepareDiffuseStream keeps the original current/material/source/slot/vertex-count/
index-count checks. It then verifies the same scene and source-surface identities,
exact normal count, and overflow-safe range in the retained vector before use.
It feeds normals by const reference in original vertex order to the unchanged
projectDiffuseCompressedRgba leaf. Current normal axes, light matrices and RGBA
are rebuilt by the existing publication flow. No generated color is cached.

The aggregate resets and replaces the entire Binding on every
prepareNativeStaticLighting call. This backing cannot survive that replacement,
and stale camera/light/material checks remain unchanged. A retry of diffuse
projection within the same publication can use the same already-validated bytes.

## Validation, failure and cost

The first normal recovery still runs at the same point after original source,
light selection and axes checks, in the same surface/vertex order. Every numeric
recovery check remains in the original leaf. Bytes are appended only after that
vertex passes. The removed second recovery cannot newly fail for the admitted
same immutable source; source mismatch now rejects before projection.

Two flat vectors avoid a separate normal allocation per surface. No trustworthy
lit-vertex total is already available without another pass, so normal storage
uses vector geometric growth. This adds allocations, normal-byte copies during
growth, three payload bytes per lit vertex and surface identity/range metadata.
It removes one recovery per lit vertex per publication (q validation and three
division/truncation/range/roundtrip sequences). Actual tradeoff is unmeasured.

Binding::prepare is called inside the aggregate's existing try/catch with a local
unique_ptr. Any new vector allocation/length failure is translated to the existing
native static lighting allocation diagnostic and destroys the unpublished partial
Binding. No successful partial normal bank is published, and previous snapshots
are invalidated at the same aggregate boundary. Allocation failure can occur at
new append sites; this is explicitly native backing allocation, not source
callback completion. Original normal-validation order is otherwise unchanged.
