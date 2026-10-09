# Bounded vehicle contact handler, checkpoint48R

`runVehicleContacts48R` is a typed, allocation-free recovery of original
`219420`. It covers the supplied candidate list, `1202E0` bounds tags1/6,
prepared or dirty-parentless scene caches, the known rigid-body contact-list
callback `165430`, and the existing `appendSpringRecord48N` recovery of
`2963E0`. It does not enumerate a spatial index, infer live ownership, substitute
virtual callback results, or provide a complete frame/simulation scheduler.

The original handler does not set a semantic boolean return. Native
`VehicleContactStatus48R::Complete` means that this bounded call reached the
original natural completion. It is not a hit flag or a claim about residual
guest register `v0`.

## Explicit caller-owned state

The source exposes scene matrices/cache flags, bodies with fixed ten-slot
contact lists, candidate descriptions, an independent candidate-index order,
and complete 96-byte spring records. All capacities are supplied by the caller.
Scene/body indices identify mutable objects: equal indices share one state;
distinct body entries must carry distinct identity labels. Repeated candidate
indices and contacts with the primary body itself are supported without
reordering, filtering, or deduplicating the supplied candidate sequence.

Semantic identity labels replace original addresses in contact slots and
`record.other44`. They are never dereferenced as host or guest pointers. Reached
body labels must be nonzero because `SpringBodyId48N` reserves0 for no body;
both original body objects are dereferenced on a hit. This check is lazy and
does not restrict zero payloads accepted by the standalone165430 helper. A
candidate's `cachedRadius` is exposed to preserve the distinction between its
cached and shape state, but original219420 never consumes it. The radius read
at original21957C is `vehicle364->80`, represented by `input.vehicleRadius`.

The two original static flags and words are explicit input/result state. A
zero flag initializes its word to `0.5` or `1.0` and sets its flag to1. Nonzero
flags preserve their word bits. Only the first word is used as a scale, and
only when an eligible candidate reaches the radius calculation. This is not
a hardcoded0.5 approximation of previously initialized mutable state.

## Preserved sequence and arithmetic

1. Run both static initializers, then scene-position `28B1A0` and scene-axes
   `28A700`, before testing the signed candidate count. A count of zero or a
   negative count still refreshes a dirty parentless position scene.
2. Visit the supplied indices in order. Test candidate category against the
   supplied live mask before consuming its bounds, cached center, body link,
   or the vehicle radius. Skipped candidates may contain unused unsupported
   bounds or callbacks.
3. Execute tag1/tag6 bounds arithmetic from its separate shape matrix, local
   center, and local half extents. The transformed center is computed, though
   the handler does not use its value. Each of the three sparse half-axis
   vectors is fully transformed with its explicit zero multiplications and
   additions. Half extent is `abs(rotZ) + (abs(rotX) + abs(rotY))` in that order.
   All three extent axes are computed before the handler overwritesY with+0.
4. Compute candidate radius as scale times the scalar square root of
   `(Hx*Hx + 0*0) + Hz*Hz`. Separately subtract the candidate's cached center
   from the position scene's world translation. Remove its projection on the
   position scene's cached up axis, without normalizing that axis. Accept only
   a strict lateral distance less than vehicle radius plus computed radius.
5. Normalize the accepted lateral vector using the original VU-style reciprocal
   square root, then multiply its lanes by that reciprocal. This differs from
   dividing each lane by a scalar distance. For zero squared length, the
   inherited finite oracle policy supplies signed maximum-finite reciprocal;
   multiplying the zero lanes preserves the resulting zero signs.
6. Form the primary surface point as position minus vehicle radius times
   normal. Run inverse-point `28BD50` twice on the explicitly selected local
   scene: first for position, then for surface point. Each helper transposes
   the basis with the original add-zero steps, transforms translation,
   negates it by multiplication, transforms the point separately, and adds
   those results. It does not reassociate this to `transpose(point-T)` or
   replace the transpose with a general inverse.
7. Form the other surface point as cached candidate center plus computed
   radius times normal. Call the primary body's known+98 callback with the
   other identity, then the other body's callback with the primary identity.
   Only then invoke the spring append helper. Both callbacks still execute
   when spring storage is logically saturated. A shared body is mutated once
   for each invocation, so the second call observes the first one's state.

Append fields00/10/20/30 are the two local points, world normal, and other
surface point. Their W lanes are1. `scene40` and `material4C` are0;
`other44` is the other body's identity. Existing margin48, flags50 and
preserved bytes51..5F remain untouched. No spring reset is inserted.

## Scope and unsupported boundaries

Prepared scene caches ignore parent presence. Dirty parentless caches copy all
16 local matrix words and set flag bit0 while preserving the other flag bits.
Dirty parented scenes report `UnsupportedSceneParent`. Bounds tags other than1
and6 report `UnsupportedBounds` only after passing the category gate. Unknown
body callbacks report `UnsupportedBodyCallback` only when reached on a hit.
The known callback is documented independently in `body-contact-list48r.md`.

Body links, the local scene, callback metadata and spring append metadata are
not consumed on strict misses. Unused forward-axis and matrix/vector W lanes
are opaque bit patterns; merely copying scene words does not require finite
XYZ data that never reaches arithmetic. An eligible bounds operation still
computes its discarded center and pre-overwriteY extent, so arithmetic overflow
there is rejected rather than silently elided.

All consumed arithmetic inputs and intermediate results use the existing
ordinary finite, separately rounded binary32 evidence model. Scalar square
root uses rounded `sqrt(abs(x))`; VU reciprocal square root separately rounds
the root and reciprocal. Compilation must disable contraction/reassociation
and use round-to-nearest. No claim is made about every physical EE exceptional
mode, subnormal policy, or hardware timing. No Xbox or Windows execution, frame
rate, or performance target is established by this native source alone.

## Transaction and memory contract

Every source scene, body and record, including inactive tails, is copied to
caller-owned scratch. Complete commits all those source-length arrays and the
whole spring/static result. Additional destination tails are untouched.

Every other status, including unsupported boundaries, leaves every destination
byte and the whole result unchanged. Scratch may have changed. This deliberate
native safety contract differs from an interrupted original prefix: the
original may already have initialized statics, refreshed a scene, or completed
the first body callback when an unsupported boundary is reached. Original
prefix evidence is retained separately; it is not mislabeled as a natural
completion or compared to a committed native partial result.

Before writing scratch, the handler checks aligned, representable full declared
spans. All scratch/destination arrays and the result must be mutually disjoint
and disjoint from every input array and descriptor. Inline contact lists are
covered by the body-array spans; element types contain no nested pointers.
Read-only spans may alias one another. Zero-length spans ignore their pointers.
Insufficient scratch/destination capacity cannot produce partial success.

At a reached append, the full declared logical spring capacity must have
initialized source record backing, even if saturated. Existing48N rejects
negative count/capacity metadata; the fixed-list callback rejects negative
contact counts while preserving the original positive overfull no-op. These
native safety checks do not imply that malformed original backing was safe.

Runtime uses no allocation, resizing, recursive traversal, retained pointers,
global state, instruction interpreter, or threads. Stack use is independent of
input counts. Full-array projections are linear; validation of distinct body
identity labels uses a constant-storage pairwise check. Per-candidate work is
constant except the known callback's fixed ten-slot scan. This keeps the
single-core, low-memory baseline explicit without making timing promises.

## Evidence and verification

The source map is `evidence/vehicleContact48R/map/`, frozen from the immutable
ELF with SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
It includes complete original instruction blocks and the tag-dispatch/vtable
data. `tools/gameplay/vehicle_contact_fixtures48r.py` executes those original
words through the existing offline48H/48J arithmetic oracle, with no guest RAM,
patched helper returns, or suppressed callbacks. Fixture records normalize only
semantic body identities; complete raw spring slots are preserved.

Native differential, guard and serial-build results are recorded separately in
the48R test documentation. Source construction alone is not a passing test.
