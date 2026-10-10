# Original-roster collision and probe ownership, 50HW

## Cold resource owner

`tests/native/native_racer_collision50hw_fixture.h` provides
`native_racer_collision50HW::Owner`. A caller binds an original RES path and its
exact member prefix, for example `Pods\Pod03\`. Loading extracts all four
collision resources from that archive:

- `podboxm.col`: main-body type2 query
- `chariotm.col`: secondary-body type2 query
- `podboxf.col`: main-body type0 target
- `chariotf.col`: secondary-body type0 target

The source type2 decoder preserves node/ray order and applies its existing
quantization/radius arithmetic. The source type0 import/cache/runtime path
preserves tree/face order and existing normal packing. Target radii and both
tree depths are validated/prepared once, during loading. Display geometry is
never a substitute. Original type0 limits apply, rather than the old Pod01-only
16-vertex/27-node/28-face shape-helper limits.

The owner publishes only immutable borrowed views and is neither copyable nor
movable. All storage is staged privately; failed preparation publishes nothing.
Keep every owner alive until all queries and the contact owner have finished.
`requirements(maxTargetDepth, result)` provides pair-slot counts and traversal
stack capacities. A shared execution arena must allocate the maximum across
every participating pod before registering/sealing bank spans. Per-racer query
descriptors then point at that racer's immutable main/secondary resource.

## Explicit endpoint-W boundary

COL contains XYZ only. `EndpointPolicy` requires a callback supplying one
`Type2OpaqueEndpointWords49X` for each original ray, in order. It receives the
main/secondary selector and ray index. Missing coverage rejects preparation.
No endpoint word is initialized by an inferred default, read from COL, or
claimed to reproduce old heap contents. The existing49X decoder copies the
caller words unchanged, including their raw bit patterns. A host choosing a
uniform W policy must say so explicitly; preserving the Pod01 path requires
supplying its existing caller words.

## Probe setup and stable execution-arena installation

The existing `terrain_probe_authoring50fq_fixture::prepare` keeps its original
Pod01 defaults and accepts an explicit archive member name for other pods.
The preferred overload accepts the prepared collision owner. It requires the
query's resource pointer to equal that owner's main view, then reuses its
validated bounds without reading or decoding the archive again. The actual
pod PHY supplies mass, lift and orientation spring as before.

`installProbeCold(arena, liveOwner, authored, error)` is only for initializing
a fresh bank slot. It admits identical immutable category/identity/filter
semantics, copies registered numeric probe fields, and preserves all existing
vector backing. The live single-pair owner is initialized at its final arena
address, so its resource pointer never borrows a temporary pair. Do not use it
to reset a moving racer. The caller still installs the frame's probe response,
lower endpoint and enabled state, and refreshes memberships through the normal
cold placement path.

## Heterogeneous main-body contact extension

`native_racer_contact50hw_fixture.h` and
`native_racer_contact_bank50hw_fixture.h` add the50HW adapter without changing
the homogeneous50HT diagnostic helper. `Owner::prepare` takes one binding per
racer: its collision owner, copied main spring settings, and copied main
response parameters. `run` takes separate input/output body arrays; each input
contains its own body parameters, local center, current state, identity and
explicit material. `applyAtWorldBoundary` reads that racer's restored bindings
and parameters inside each bank visit, including its own main190 node.

For every caller-ordered directed pair, the primary's original main rays query
the other's original main target mesh. The primary's spring settings process
the generated records against both bodies' physical parameters and the other's
material. Source tree/ray/record order, the ten-slot processed-body gate,
geometry-hit-only reverse suppression, and main logical record capacity40
remain unchanged. Scratch is cold-sized for the largest query/target depths
and ray count. Hot calls do not resize or load assets. All output bodies and
the receipt commit only after every pair succeeds; bank commits still change
only the contact-effect fields after checking every participant.

The empty physical-root material remains an explicit caller-selected host
policy, not an assertion that COL or unknown prefix-padding bytes authored it.
The adapter does not reconstruct original combined static/dynamic candidate
ordering, all-body substep scheduling, collision damage, or secondary dynamic
contact scheduling. Secondary resources are owned for the existing static
secondary query and for later source-backed joins, not silently substituted
into main-body response.

## Validation status

These50HW additions were written and source-reviewed on 2026-10-09. No test,
syntax-check, compiler, sanitizer, benchmark or gameplay command was run for
this work. Runtime integration and validation belong to the parent task's
single authorized hourly verification window. Existing50HT results do not
certify the new50HW bytes.
