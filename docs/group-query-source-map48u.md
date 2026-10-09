# 48U: original 122660 outer-group source map

## Boundary and authenticated inputs

This is the outer **collision-tag group** branch of the original limited-list
query `122660`, for query shape tag 3. It is distinct from candidate shape tag
1, the mesh-record array inside `126F40`. Group members are independent collision
objects whose shared shape backing may change during iteration. Shape-1 records
instead share a single dispatch reset and cumulative transforms. Neither can be
replaced with a flattened independent-candidate list.

`tools/gameplay/group_query_fixtures48u.py` authenticates all supplied instruction
words and dispatch data against the existing ELF
`work/native/project/disc/SLUS_202.68`, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
File offset is `VA - 100000 + 80`. New copies of the complete entry, group count,
member, radius, XZ sphere and shape setter are in `evidence/groupQuery48U/map`.
Full executed dependency words, including preparation, mesh traversal and static
callback, are recorded separately. No RAM input, registration or live world is
used. World pointers, dimensions, tags and the preparation sentinel are explicit
initialized fixture inputs, not inferred file-backed constants.

## Entry, namespace priority and coarse gate

`122660(query, signedListCount, pointerList)` marks query F8 bit 4 and calls
`11DD00` once. It then visits supplied outer entries in order.

- At `1226C4`, byte object+2 equals global byte `gp-7FF0`: use the ordinary
  candidate route. This wins even when both collision tags are equal.
- Only a mismatch reaches `122898`. Query F8 bit 0 suppresses the group route.
- At `1228A8`, byte object+2 must match separate global byte `gp-7FEC`.
  Neither comparison is an object shape-tag or category-mask comparison.
- `1228B4..122954` creates a center `(u16(x8)*scale, +0,
  u16(zC)*scale, 1)`. It gets scale from `(*3E1CE8)+590`; position Y is not
  consumed by this coarse gate.
- `135F30` loads archetype pointer from `world+6C+4*u8(group+E)`. With signed
  group byte +11, radius is `base18C + (signedByte11 *
  (other194-base18C))/128`, retaining each original binary32 operation.
- `126E40` computes XZ distance, subtracts `query.radius+group.radius`, and
  clamps nonpositive separation to zero. The caller accepts exactly zero.
  Equality passes. Do not replace this with the strict squared XYZ test in
  `126F40`, nor add a coarse Y test.

A rejected group has not called its member helper, changed query current-candidate,
or marked any member. Outer ordinary candidates have different early effects:
they publish current-candidate and member bit 4 before their sphere/filter gates.

## Member resolution, mutation and signed loop count

The first loop condition is `122B28 -> 135CD0`. This helper obtains signed count
at archetype+1FC. It is called again after every member, including after the last
one; nonpositive counts skip the body. The source does not cache, deduplicate,
or stop after the first hit.

At `12298C`, `135CF0(group,index)` loads member pointer from
`*(archetype+204)+4*index`. It returns zero for null member shape+8C. Otherwise it
computes all three unsigned positions times world scale and both signed-byte
ratios **before** selecting the member shape-tag:

- `ratio18C = (base18C + signedByte11*(other194-base18C)/128)/base18C`
- `ratio180 = (base180 + signedByte10*(base180-other184)/128)/base180`

Ordinary-finite arithmetic is the proof domain. Numerically unused ratios are
still executed; they are not permission to silently skip reached denominator
validation in a bounded native contract.

Shape 0 copies base+80, replaces translation XYZ with the group position, writes
those translation lanes into the physical shape at +B0/+B4/+B8, then calls
`11F680(member, temporaryMatrix)`. The now-closed 48T setter copies current+80 to
previous+C0, copies the supplied matrix to+80, composes its **full four-lane**
columns with immutable local+40 into world+00, and updates this member's sphere
from root node at shape+114. Because group translation was written first,
previous+C0 already contains that translation. Radius uses the root half-vector
length without scaling; center uses the supplied base transform, rather than
composed world+00. Physical shape state and individual member sphere state have
separate ownership. Updating an aliased shape does not refresh other members'
cached spheres.

Shape 4 writes ratios to shape+68/+6C and translation to+30/+34/+38. Shape 5 writes
ratios to+58/+5C and translation to+40/+44/+48. Their member cached spheres stay
unchanged. Other nonzero tags, including shape 1 and query shape 3, leave shape
state unchanged after common arithmetic and return the member.

Only after helper return does `122994` publish member to query+F4 and
`122998..1229B0` set its F8 bit 4. Null shape is therefore a real unsafe original
read at `1229A4`, not a skipped member. Null tag-0 geometry is a separate first
read at `11F7B0`, after matrix writes but before sphere writes.

## Secondary query switch and dispatch

Grouped members do **not** call ordinary `126EB0` or bilateral `120E70`.
Member list-byte tags and normal query/candidate filter eligibility do not apply
here. `388B70` is a seven-word query-shape switch; its tag-3 entry is `122A10`,
which calls `126F40(query.shape8C, member, query.result90)`.

The ordinary switch is separately located at `388B90` and tag 3 selects `122780`.
Exact table words and hashes are frozen. Other query branches are mapped, not
claimed as part of the type-3 native seam.

`126F40` first publishes mode and clears pair hit flags. It then tests member
flags31 bit 0 and its strict XYZ sphere. On success it resets fractions/count/
selected fraction, then dispatches candidate shape tag. Tag 0 enters one mesh;
tag 1 enters compound records; other tags return a reset-phase miss. Thus a
shape helper may mutate a member whose later activation or sphere rejects it.
Inactive shape 1 does not reach compound traversal. See the retained 48P map
for traversal, reset timing, stale vectors and full-W numerical details.

## Hit finalization and callback ownership

`122AB0` rejects a returned miss and increments the member index. A hit:

1. Sets query F8 bit 5, then calls `11E330`, a type-3 no-op.
2. Copies prior query+40/+50 vectors to the current member+40/+50.
3. Calls query+E4 with `(query, member)` if nonnull.
4. Reloads current member, then calls member+E8 with `(member, query)` if nonnull.

The outer group's identity, category, name and callback are not substituted.
The proof executes genuine static callback `294BB0` and append `2963E0`, including
material-name parsing, record saturation and retained spring state. Unknown
member callback entry is recorded at exact `122B18` (or ordinary `122888`) and
is a bounded unsupported boundary; no function is stubbed to return success.
A reached dynamic-category294BB0 case stops at `294C34`, immediately before
reading the body vtable, after any previous static member callback effects.
This is distinct from material/skip/category checks that complete naturally.
After all outer entries, a set query hit bit causes `11E470`, another type-3
no-op, and determines return. A later miss may reset pair results without erasing
an earlier query hit, prior callbacks or appended records.

## Native dependency boundary

The supported composed projection uses positive-count initialized ray pairs,
zero scene metadata, finite source arithmetic and explicit object/group/shared
shape tables. It includes direct and grouped order, same-query membership,
shared full-W shape-0 matrices, tag-4/5 compact updates, no-op tag misses,
static callback ownership and reached late guards. Source-complete compound,
nonzero-metadata and mode-2 interpolation cases remain explicit native
unsupported partitions. Pointer and callback boundaries retain source partial
state solely for fidelity; a native transactional error must publish none of it.
No full-world lookup, live binding, dynamic-body callback, arbitrary callback,
heap allocation, guest interpreter or instruction data is implied in production.
