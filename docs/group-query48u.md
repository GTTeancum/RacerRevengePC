# Bounded type3 outer-group query (48U)

## Runtime seam

`queryStaticType3GroupsBounded48U` is an additive, caller-fed projection of
original `122660` for query shape3. It consumes an explicit `LimitedList`
context, prepares the type3 rays once, and processes the supplied outer order.
It adds the collision-list group branch without treating collision-list tags as
shape tags or flattening group members into ordinary candidates. The existing
48P entry and 48Q context entry retain their prior behavior and APIs, including
the separate one-candidate `Dispatcher` entry.

`group_query48u.cpp` reuses the original-order 48P numeric dispatcher through a
private borrowed view and uses the proven 48T group shape setter. It does not
clone the candidate's owning filter vectors, copy an owning mesh, discover a
world, allocate, retain caller pointers, or launch threads. The new public entry
uses the same status type as 48P; only `Complete` commits caller destinations.

## Source order

For each outer object, the ordinary collision-list tag comparison wins first,
even if direct and group tag values are equal. Ordinary candidates update
current identity and their flag bit4 before the existing inclusive XYZ sphere
and bilateral filter. Mutable cached spheres, flags and shared shape matrices
come from the staged object/shape tables.

If the ordinary tag does not match, query flag bit0 suppresses groups. Otherwise
the group tag must match. The group position uses unsigned 16-bit X/Z times the
explicit world position scale. `135F30` supplies the signed-byte radius
interpolation, followed by the original inclusive XZ distance test. The Y
position is not part of this coarse test. A signed nonpositive member count
skips the loop. Positive counts require the full declared member backing.

For each reached member, in original order:

1. `135CF0` checks shape presence, computes XYZ and both ratios, and performs
   the tag-specific shape update. Both ratios are consumed even for tag0 and
   the nonzero no-op tags.
2. Update current identity and the member's flag bit4.
3. Enter the type3 dispatcher directly, without an ordinary per-member outer
   sphere or bilateral filter. The dispatcher still clears pair hit flags,
   checks activation and its strict XYZ sphere, then performs its gate-specific
   resets and shape dispatch.
4. On a geometry hit, set query flag bit5, preserve the type3 no-op `11E330`,
   copy query40/50 into this member, invoke the explicit query callback, then
   reach the member-response-callback boundary.

The group owner does not receive the member's current identity, flags, copied
fields or callbacks. Repeated outer objects and repeated members are retained;
there is no deduplication or early exit after a hit. Fractions, hit count and
selected state follow the per-candidate 48P reset rules, not type2 accumulation.

## Physical identity and borrowed tables

All references are one-based table indices; zero means an absent binding.
Object identities are separate opaque, nonzero values, unique in the object
table. The same reference can occur repeatedly in outer and member orders.

Each object descriptor borrows a `Type3Candidate48P`. Immutable filters,
activation, category, shape tag, scene name and response-callback presence come
from that descriptor. The mutable object-state table supplies cached center
and radius, flagsF8 and copied40/50. The descriptor's initial sphere, flagsF8
and copied fields are not alternate mutable state. No filter is copied.

A nonzero tag0 `shape0` reference selects a canonical shared shape source and
state. The source contains local40 and the mesh; its root node0 supplies exactly
the center/half used by the group setter, and that same mesh supplies traversal.
The state contains composed00, world80 and previousC0. Candidate geometry mesh
and world matrix are ignored when this binding is active; geometry.id still
must agree with the object identity when reached by shape0 dispatch. Unbound
ordinary tag0 objects retain their descriptor's immutable mesh/world matrix.
Reached group tag0 objects require a binding.

Distinct objects sharing a tag0 shape retain separate cached spheres. Updating
one member changes the common matrices and only that member's cache. A later
ordinary occurrence therefore combines the current shared matrix with that
object's possibly older sphere, as the source does. The 48T previous matrix
contains the new translation because the group helper writes it before the
setter copies world80 to previousC0. Group ratios do not scale tag0 matrices or
radii. All full-W matrix composition and ordered binary32 operations are kept.

Tag4/5 use an additional compact shared table containing only the five floats
written by `135CF0`: XYZ translation and the two scale ratios. Immutable
`shape45Tags` select each table entry's tag4 or tag5 layout. References must
agree with that layout. Mixed tag interpretations of one physical backing are
an explicit native projection boundary, not a claim that the original rejects
such aliases. Other nonzero tags leave their shape payload unchanged in the
group helper. Compound shape1 retains its gate-specific dispatcher boundary.

An optional `queryObject` binds the same query identity in these orders. It must
have shape3, matching activation and shape presence. Preparation supplies its
cached sphere; its flags and copied40/50 follow the query's authoritative
state. The corresponding initial object fields are therefore replaced from
the query state. Group visits compute the common helper arithmetic, perform the
tag3 no-op, and preserve the dispatcher's shape3 miss/reset behavior. A query
identity present without this explicit binding is invalid.

## Callback and metadata domains

The entry accepts positive pair count and zero endpoint scene metadata, as in
the existing composed48P path. The six execution fields come exclusively from
`Type3QueryContext48P`; the owning template remains borrowed. Unknown entry or
callback enum values are invalid. This new entry accepts `LimitedList` only.

`GroupQueryInput48U::queryCallback` explicitly identifies query+E4 and is
authoritative instead of the template's callbackEnabled flag:

- `None` models a null query callback.
- `Static294BB0` selects the proven static type3 spring callback, including its
  skip-mask precedence, material-name parsing, shared first-three-length-word
  endpoint reconstruction, metadata indexing and bounded record appends.
- `Unknown` rejects only if a geometry hit reaches that callback.

Dynamic294BB0 bodies remain unsupported when reached after the skip-mask gate.
Any nonnull member+E8 response callback is unsupported when reached, after the
query callback and copied fields in scratch. The reversed-argument member
callback is not treated as another spring callback. Such late failures roll
back all destination state, including any earlier group shape mutations.

## Validation and transaction

The caller supplies immutable initial ray, record, object and shape state plus
separate workspace and destination arrays. The complete declared writable
capacities, including inactive tails, must be representable, aligned, mutually
disjoint, and disjoint from every input descriptor and nested backing span.
All candidate filters/names/meshes, group member backing, canonical shape meshes,
shape45Tags and optional identity order are guarded even if never visited.
Read-only spans may alias; all backing must remain stable throughout the call.

Ray slots and length slots copy through at their separate full source
capacities, and record backing copies through at its full source count. Object
and shape states commit at the supplied table counts. Unused destination tails
are preserved. Any non-Complete return leaves every destination and output
result byte unchanged; workspace bytes may change. A reached invalid reference,
insufficient physical capacity, missing shape/binding, unsupported callback or
nonfinite reached arithmetic is never converted to a partial hit or miss.

All reached group arithmetic operands and intermediates must be finite. Signed
zero and individually rounded binary32 operations are retained. The inherited
48P mesh-topology importer precondition, numeric domain, normal-mode limits and
unsupported interpolation boundary remain in force. The explicit supplied
meshes are native data, not guest addresses or an interpreter in production.

## Bounded work and storage

Object/group/shape tables describe the supplied query domain; callers should
supply a compact domain when possible. The runtime always scans their declared
nested spans for safety and copies mutable table counts. Without a validation
order, identity uniqueness uses a small-table pairwise scan. An optional
workspace identityOrder replaces that scan with an exact-count, current-key
strict ordering check. Every reference and current nonzero identity is checked
on every call. Exact length plus strict keys proves a complete permutation;
no sorting, bitmap allocation, stale token or unchecked cache is introduced.
Shared tag4/5 layout agreement is checked linearly against shape45Tags.

Group iteration is bounded by the signed member counts and outer order length;
repeated members intentionally repeat their work. Ray/record backing and the
reusable traversal stack retain their48P costs. Geometry/filter complexity is
unchanged. This does not alter48T collection pool free-list/spare-entry checks
or their remaining scaling caveat, and is not a broad allocator optimization.

The runtime is serial, allocation-free and independent of desktop, renderer or
live-world services. Host compiler frame estimates and synthetic timing do not
establish console stack, memory, frame-rate or gameplay performance bounds.
Independent source proofs, native differential results and mutation controls
are recorded separately under `evidence/groupQuery48U`.
