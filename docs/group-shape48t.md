# Bounded tag-0 group shape update (48T)

## Scope and evidence

`setGroupShape0Bounded48T` implements the tag-0 path of original
`135CF0 -> 11F680`, after the caller has performed the common group position
and ratio arithmetic. The frozen 48S map is in
`evidence/candidateCollection48S/map/group_member.txt` and
`evidence/candidateCollection48S/map/shape_matrix_setter.txt`; the independently
executed 48T fixtures and proof record are in `evidence/groupShape48T`.
This is a group-member update, not a general matrix setter or shape-0 query
bounds implementation. Other matrix-setter branches and their helper calls
remain outside its claim.

No live world, guest address, scene refresh, registration, allocator, PC files,
RAM or game process is needed. Runtime uses only borrowed caller-provided C++
storage. The intended baseline is serial and allocation-free, with fixed native
stack usage and no worker threads. It makes no console performance claim.

## Source order and arithmetic

The group caller computes X, Y and Z from unsigned 16-bit positions times the
world position scale. It also computes the same two scale ratios used for
member tags 4 and 5. Both ratios remain reached arithmetic for tag 0, but neither
ratio scales the matrix, the center or the sphere radius on this branch.

For the shape-0 branch:

1. Copy all 16 words of shape `+80` to a temporary matrix.
2. Replace only temporary translation XYZ (words 12, 13, 14).
3. Write those XYZ values into shape `+B0/+B4/+B8`.
4. Enter the tag-0 setter, which copies shape `+80` to shape `+C0`, then
   installs the temporary matrix into shape `+80`.
5. Compose shape `+00` from the temporary matrix and shape `+40`.
6. Copy geometry `+0C/+10/+14` into the member center and force its W to 1.
   Compute radius from geometry half `+18/+1C/+20`, then transform the center.

The ordering in steps 3 and 4 matters: the previous matrix at `+C0` contains
NEW translation XYZ. Its other 13 words retain the original `+80` values. At
the end of this group entry, `+80` and `+C0` therefore contain identical words;
this equality is not generally true for direct calls to `11F680`.

Matrices are four column vectors. For each output column `r` and lane `c`,
composition is exactly:

```
(((world[c] * local[4*r])
  + world[4+c] * local[4*r+1])
  + world[8+c] * local[4*r+2])
  + world[12+c] * local[4*r+3]
```

Every multiply and add rounds separately to binary32. All XYZW lanes participate,
including the fourth multiplication; sparse zero terms must not be dropped.
The sphere center instead uses three ordered multiplication terms followed by
an addition of translation XYZ, and its W is positive 1. The radius is
`sqrt((halfX*halfX + halfY*halfY) + halfZ*halfZ)`, with the same separate rounding.
It is the length of the original geometry half: matrix scale, group ratios and
composed matrix are not used to rescale it.

The finite native model requires every reached arithmetic input and intermediate
to remain finite. A prior `+80` translation XYZ is overwritten and need not be
finite. Prior `+00` and `+C0` values are replaced without arithmetic consumption.
All 16 local-matrix lanes and the 13 retained world-matrix lanes are consumed.
No affine-W, normalized, orthogonal, positive-half or rigid-transform restriction
is imposed. Strict rounding and signed zeros are preserved; physical EE/VU
exceptional modes are outside this bounded model.

## Borrowed storage and API

- `GroupShapeSource48T`: fixed local matrix `+40` and a nullable pointer to a
  single `GroupShapeGeometry48T` containing center XYZ and half XYZ.
- `GroupShapeState48T`: the mutable composed `+00`, world `+80` and previous
  `+C0` matrices. No object-wide state expansion is needed.
- `GroupShapeSphere48T`: the updated member center vec4 and radius.

Source descriptors may share the same geometry pointer. Read-only spans may
alias. The complete destination state and complete sphere result must be
aligned, representable, mutually disjoint and disjoint from every input object
and the geometry object. Pointer capacities describe real backed C++ objects;
this interface cannot authenticate arbitrary addresses. Null geometry returns
`MissingGeometry`; invalid aliases/alignment/address ranges return `InvalidInput`;
non-finite reached math returns `NonFinite`. Any failure preserves every byte
of both outputs. Successful results are committed only after complete numeric
validation. Scratch consists of fixed-size local values, with no owning heap
copy and no retained pointer.

A geometry pointer always denotes immutable geometry for the call. Aliasing it
onto independently mutating physical shape storage is not part of this model;
shared immutable geometry and shared shape identity are modeled separately.

## Collector integration

`CollectionObject48S::shape0Ref` is an optional one-based index into compact
shape tables. Zero preserves the old 48S `UnsupportedMemberShape` boundary for
a reached tag-0 member. Nonzero out-of-range indices return `InvalidTopology`.
The input provides `shape0Sources` and `shape0Count`; initial state, workspace
and destination provide `shape0States` and their independently declared
`shape0Capacity`. Multiple object references to the same index share the same
mutable shape matrices and are updated in source order. Each object's sphere
remains in its existing `CollectionObjectState48S` record, matching per-member
sphere ownership even when a shape is shared.

The collector still computes common ratios before examining the member tag.
For a reached supported tag-0 member, it updates scratch shape state, then that
member's cached sphere, then current identity and member flag bit 4, and finally
appends the outer group identity. No new filter, per-member sphere test,
deduplication or nominal capacity check is inserted. Repeated members and
shared shape indices retain their sequential effects. Later direct sphere tests
consume any member cached sphere already changed by earlier group expansion.

The query's already-prepared bounds shape remains authoritative when the query
also appears in a group. This extension does not add shape-0 bounds support.
A tag-0 query therefore still fails at the existing bounds boundary before
member expansion. Members with other tags retain the 48S behavior and do not
consume their optional shape-0 reference.

All new initial, scratch and destination shape spans use their full declared
capacities, including inactive tails, in the existing global alias guard. Every
source descriptor and each nonnull nested geometry object is guarded even when
not selected by the query. Shape count is bounded to the reference domain;
initial, scratch and destination capacities must cover it. Shape tables are
copied to scratch before execution and committed only on complete collection
success. Output capacity, allocator, topology or numeric failure after an earlier
shape mutation still preserves ALL destination state, identity and result bytes.
Unused destination shape tails remain untouched.

The existing full validation requirements are retained. No unchecked setup
token or cache makes future mutable input changes bypass validation. One
allocation-free pool optimization follows the unchanged range/acyclic checks:
finite singly linked active and spare block chains intersect if and only if
their nonnull tails are equal. Comparing their tails replaces the active-by-spare
cross-product, preserves the active-last identity check and every malformed
rejection, and accepts two empty chains. The free-entry-by-spare-block and
selected-entry duplicate guards remain unchanged.

The workspace optionally supplies caller-prepared `identityOrder` and
`blockOrder` borrowed reference arrays, each with its own count. Null plus zero
independently retains the old pairwise identity or block validation. A supplied
order must contain exactly the corresponding world table count; all full spans
are guarded before any reads. Every reference must be in current range. Current
nonzero identities must be strictly increasing in identity order. Block keys
`(entryCount ? firstEntry : 0, reference)` must be strictly increasing; every
original block range check remains, and adjacent nonempty ranges must be
disjoint. Empty blocks use key zero regardless of their unused first-entry field.
Exact length and strict current keys prove a complete permutation, so no bitmap
or sorting allocation is needed at query time. A duplicate or missing
reference, invalidated order or malformed range is rejected as `InvalidInput`
without destination writes. An order still valid after input mutation is safe to
reuse because no validation result is cached. Read-only hints may alias other
read-only backed spans, and must remain stable throughout the call. Preparing
and optionally sorting these arrays is caller work outside the runtime API.

## Explicit boundaries

- Missing shape reference or geometry is unsupported; no default geometry is
  invented and no null original dereference is disguised as an empty member.
- The standalone API does not accept arbitrary direct-setter input matrices.
  Such independently frozen direct-setter fixtures remain source evidence,
  not a claim that this group-entry API implements the general setter.
- Other shape tags, query bounds, live registration and scene ownership retain
  their existing bounded contracts.
- All storage remains caller-owned; no allocator growth or background work.
