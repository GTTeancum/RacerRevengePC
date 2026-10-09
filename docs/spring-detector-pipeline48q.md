# Bounded supplied-list spring detector pipeline (48Q)

## Runtime seam

`runSpringDetectorPipeline48Q` in `src/game/spring_detector_pipeline48q.{h,cpp}`
composes the existing numeric detector and static type-3 supplied-list query:

1. `prepareSpringDetector48O` performs the supported original `295630` prefix:
   point scene-cache preparation, transformed pair ends, detector AABB, optional
   spring reset, common pair starts and `11CA80` endpoint staging.
2. Only `LimitedQueryBoundary` continues into
   `queryStaticType3WithContextBounded48P`. This executes the supported
   `122660` supplied-list preparation, dispatch and `294BB0` static callback.
3. All final destination arrays and the complete result commit together after
   the entire supported operation succeeds.

This is a bounded native projection over explicit initialized inputs. It is
not a live detector owner, object-registration system, world enumerator,
type-2 adapter, provider implementation or vehicle scheduler. Candidate order,
identities, transforms, filters, mesh backing and callback masks are supplied
by the caller. No detector/candidate/body relationship is guessed.

The original full-call evidence is documented in
[the original-instruction proof](spring-detector-pipeline-proof48q.md), and
[the native test report](spring-detector-pipeline-tests48q.md) records runtime
validation. The runtime reuses 48O/48P arithmetic instead of
introducing a second implementation of their numerical operations. The new
48P context overload borrows the immutable template filter vectors; the old
48P entry point retains its original signature and behavior.

## Authoritative fields and data flow

The pipeline input contains both a detector input and a query template. Six
query execution fields are intentionally overridden at the composition seam:

- `spring` is the state produced by detector preparation.
- `pairCount` is `detector.shapePairCount`.
- `resetSpring` is false. The detector already performed its one optional reset.
- `entry` is `LimitedList`; direct-dispatch mode is a separate 48P API.
- `bodyScene` and `bodyScenePresent` come from the explicit binding below.

The other query template fields remain authoritative, including initial query
state, filter lists, callback enablement, masks, sentinel, normal mode, copied
candidate payloads and initial current-candidate/category state. The spring
state in the detector input is authoritative even if `query.spring` differs.

The source exposes detector backing, query ray backing and records, rather than
an independently supplied query endpoint list. Query endpoints and metadata
are always the detector's prepared staging arrays. The staged array counts are
the source's full declared backing counts; they are not truncated to either
logical point count or shape count.

### Independent logical counts and initialized stale tails

The signed spring `pointCount7C` controls detector point preparation and the
static callback loop. The independent shape count controls staging and type-3
query preparation. The pipeline does not make them equal:

- Shape count larger than spring point count consumes initialized stale pair
  tails, after the active detector points have been updated.
- Shape count smaller than spring point count leaves staged endpoint/metadata
  tails unchanged. Query dispatch resets only its active shape prefix.
- A callback can inspect initialized nonzero hit flags beyond the shape prefix
  when the spring count is larger. It then uses the corresponding supplied
  stale world arrays. Metadata is read as `metadata[i]`, not `metadata[2*i]`.
- Every callback hit retains 48P's original `lengths[0..2]` use, independent of
  the hit's pair index. Fewer than three logical pairs therefore still require
  three initialized length words when a hit callback consumes them.

All source-backed slots are copied through, including these stale tails. Extra
destination capacity beyond the declared source counts stays unchanged. This
does not establish an unbounded or implicit backing allocation in the game.

## Explicit cached-body scene binding

`bodySceneIndex == -1` selects the query template's independent cached
`bodyScene`. A nonnegative index explicitly identifies the corresponding
caller-owned detector scene. After detector preparation, the callback consumes
that prepared scene's `world` matrix. Thus a scene dirtied before the call and
refreshed by an active detector point cannot accidentally use an earlier copied
body matrix at callback time.

The binding does not cause a refresh. If no active point visited the bound
scene, its cached `world` matrix stays unchanged, even if its dirty flag is set
or it has a parent. This matches the static callback, which reads its cached
body matrix without inspecting flags or refreshing it. Candidate mesh world
matrices remain the explicit values in their candidate descriptors; there is
no implied coupling to detector scene indices.

The template's `bodyScenePresent` remains a presence gate. An out-of-range index
or a negative index other than -1 also makes the callback scene absent. 48P
rejects this only when a non-skipped static callback actually needs its matrix.
A no-spring, disabled-query, miss, disabled-callback or skipped-callback path
does not reject unused binding values or unused matrix arithmetic. Declared
array and descriptor storage is still structurally validated on every call.

## Status and return-value contract

- `Complete`: detector prefix and supplied-list query complete. The detector
  returned 1; query hit/miss is independently `result.query.hasAnyHit`.
- `NoSpring`: the exact supported detector no-spring path returns 1 without
  executing the query.
- `QueryDisabled`: the detector's earlier scene/pair/AABB/reset effects are
  retained; the query does not execute, and the detector returns 1.
- `RegistrationBoundary` and `WorldQueryBoundary`: commit the supported detector
  prefix, then stop. They do not execute registration/world enumeration and
  do not claim a natural detector return.
- `InvalidInput`, `UnsupportedScope`, `UnsupportedInterpolationBoundary` and
  `InsufficientCapacity`: no final destination byte or result byte changes.
  Caller-owned workspace may have changed.

The first three statuses set `detectorReturned=true` and
`detectorReturnValue=1`. The two prefix boundaries set false and 0 respectively.
`queryExecuted` is true only on `Complete`. Original `295630` returns 1
regardless of the return value observed from `122660`; substituting a query hit
Boolean for detector success is incorrect.

On any query-skipped prefix, query ray arrays and record arrays are projected
unchanged from their sources. Candidate effects are left untouched. Query state,
flags and current-candidate values retain their template inputs, while the
shared spring state reflects detector preparation. On query completion,
`result.detector.spring` and `result.query.spring` contain the same final state.

An enabled supplied-list query with no linked shape is explicitly unsupported,
rather than fabricated as an empty query. Zero/nonpositive active detector
counts, zero or excessive shape counts, invalid arithmetic and missing backing
retain the relevant 48O/48P error behavior. Reached enabled providers, dirty
parented detector scenes, compound/group expansion, dynamic callbacks,
unsupported interpolation and response callbacks remain explicit boundaries.
No unsupported helper is replaced with a guessed result.

## Transaction and alias contract

The caller supplies two detector workspace buffer sets and two query workspace
buffer sets. They allow each existing component to keep its own transactional
commit without exposing any part of the pipeline before the later component
succeeds. Workspace is not retained across calls by the runtime.

A fixed-size global guard validates all 47 mutable spans before the first
workspace write:

- Three detector buffer sets, four spans each: scratch, prepared, destination.
- Three query buffer sets, eleven spans each: scratch, prepared, destination.
- The query traversal stack and the entire output-result object.

The full declared capacities are mutually disjoint, including unused tails.
They must also avoid every input descriptor, detector point/scene/pair/staging
span, source ray/length/record span, the complete candidate descriptor array,
scene-name backing, nested filter backing, mesh descriptor and all mesh vector
backing. Nested vectors use their allocated capacities, not only their active
sizes, for this alias check. Read-only source spans may overlap one another.

A zero declared count ignores its pointer, matching the component APIs.
Nonzero counts require aligned, non-null, representable, genuinely caller-backed
storage. Byte-count overflow and address wrap are rejected. The caller must
provide valid native C++ descriptors; the API cannot establish actual allocation
ownership or repair corrupted vector objects from arbitrary addresses.

Destination capacity checks are independent of the internal prepared buffers.
Only initialized source lengths are copied. Query effects require capacity for
the supplied candidate count only when the query is reached. Source-sized final
ray and record backing is required even for a query-skipped prefix because the
pipeline projects that unchanged state. Declared but unused workspace spans
still participate in structural alias checks.

## Resource and evidence limits

There are no runtime allocations, owning-vector copies, resizes, cache growth,
recursion, threads, locks, guest-memory reads or desktop dependencies. All
variable-capacity storage and traversal stack are caller-owned. Scalar stack
usage is fixed with respect to candidate, pair and mesh counts.

Meshes must already satisfy importer validation as rooted trees without shared
or unreachable records. The 48P visit limit and caller stack bound reached work;
they are not a replacement for complete topology validation. Guard checks prove
the declared address ranges, not live object ownership.

Arithmetic keeps the existing finite ordinary binary32 scope, original
operation ordering and signed-zero scalar MIN/MAX rules. Copied game-facing
Vec4 lanes and record bytes remain intact; native candidate-local scratch
represents XYZ only. Build with strict floating-point options, without fast-math
or contraction. Native tests, sanitizer results, allocation counts, source
provenance and host-only timing belong in the companion evidence/test reports;
this implementation alone does not establish hardware FPS or a rendered race.
