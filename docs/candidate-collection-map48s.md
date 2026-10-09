# Candidate collection and spatial dependencies, 48S

## Scope and result

This is an **immutable-ELF source map**, not a game execution, RAM snapshot,
native correctness proof, live object binding, complete vehicle scheduler or
world-registration implementation. It establishes a bounded native route through
original `122350`, real `294590/294600/294740` spatial iteration and an explicitly
identified `QuadTree` implementation. Initialized native structures must preserve
shared identities, serial state changes and original order. It does not authorize
replacing the iterator with a flattened list of presumed candidate results.

The source reveals several behaviorally important differences from a conventional
bounded candidate gather:

- Output count is cleared immediately. Query-disabled collection clears query
  flag bit 5 and returns before bounds or spatial reads.
- Nominal capacity is checked **after** fetching the next outer iterator item.
  One additional item can have its spatial cursor/stamp effects even when full.
- Group expansion has **no inner capacity check**. Every returned member changes
  the query's current candidate and its own flag, but the stored output identity
  is the **outer group**, repeated once per member. No member predicate runs.
- Real quadtree iteration marks a shared 16-bit object stamp. It is neither a
  stable identity set nor independent deduplication for each returned list.
- Cached rewind does not clear the current item-block cursor. An interrupted
  search and a fully exhausted search need not restart identically.
- The spatial wrapper returns the first result of a newly selected method
  directly. If that result is null, this collector stops, even if later zones
  remain in the linked structure.
- Query shape 3 recomputes cached bounds from its endpoint metadata and cached
  scene matrices here, before the separate detector endpoint-staging phase.

These facts are source findings. A later native proof must state precisely which
of the mapped branches it exercises; freezing a branch does not prove it native.

## Reproducible source packet

Run from the repository:

`python3 -B evidence/candidateCollection48S/map/freeze_map.py`

The script writes only `evidence/candidateCollection48S/map/`. It verifies the
entire authorized ELF SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`,
checks ELF32 little-endian headers and the exact file-backed load segment
`(type=1, offset=128, VA=PA=1048576, filesz=2972160, memsz=3470848,
flags=7, align=128)`, and rejects any extraction not uniquely file-backed.
The address translation for these extents is `VA - 0x100000 + 0x80`.
BSS variables are described as runtime inputs, never extracted as ELF data.

The packet contains 28 bounded code blocks, 2,955 words / 11,820 bytes and
168 bytes of dispatch/identity data. Each block has raw bytes, address/word text,
SHA-256, an explicit extent rationale and per-word annotation provenance.
`original-words.json`, `dispatch-data.json`, `call-dependencies.json`,
`source-checks.json` and `freeze-summary.json` are machine-readable.
The decoder and prior 48H/48J/48K/48O/48P annotation manifests are hash-pinned;
all reused annotations are matched to exact ELF words. The local scalar extensions cover COP1 conversions/MULA and SPECIAL
conditional moves/MULT. The final map has no unannotated words. New encodings
were checked against [PCSX2's opcode table](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/R5900OpcodeTables.cpp);
MULA multiplies into ACC, and MULT also writes its low product to a nonzero
destination register, as checked in the project's [FPU implementation](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp)
and [integer implementation](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/R5900OpcodeImpl.cpp).
References were checked on 2026-10-06 and recorded in the manifest. Reproduction
is offline. This is not a claim of full EE exceptional-mode emulation. No 48R source, proof, archive or prior manifest is rewritten.

### Authenticated extents

| Block | Half-open extent | Entry/return evidence |
|---|---|---|
| Collector | `122350..122660` | Caller JALs `219AF0/219B14`; JR `12264C` plus delay; three zero padding words |
| Bounds | `1202E0..120E68` | JAL `1223E4`; seven-way switch `388AB0`; common JR `120E60` |
| Bilateral filter | `120E70..121288` | JAL `12247C`; JR `121280` |
| Group XZ sphere gap | `126E40..126EB0` | JAL `122570`; JR `126EA8` |
| Direct XYZ sphere gap | `126EB0..126F38` | JAL `122458`; JR `126F30` |
| Spatial constructor | `294590..2945FC` | JAL `1223EC`; JR `2945F4` |
| Spatial open | `294600..29473C` | JAL `122410`; JR `294734` |
| Spatial next | `294740..294B40` | JAL `122604`; JR `294B38` |
| Quadtree open | `265040..265330` | Vtable `3D2340+30`; JR `265328` |
| Select overlapping nodes | `265330..2654C0` | JAL `2652F8`, recursive `265458`; JR `2654B8` |
| Select complete subtree | `2654C0..2655C0` | JAL `265428`, recursive `265534`; JR `2655B8` |
| Quadtree next | `2655C0..2656A0` | Vtable `3D2340+38`; JR `265698` |
| Quadtree rewind | `265C40..265C60` | Vtable `3D2340+3C`; JR `265C58` |
| Quadtree close | `265C60..265CD0` | Vtable `3D2340+40`; JR `265CC8` |
| Vtable install fragment | `263C0C..263C18` | Three exact words inside destructor `263BF0`; not a standalone function |
| Pool acquire | `105720..10582C` | JALs from node selection; JR `105824` |
| Pool growth | `105870..1058E8` | JAL `1057D8`; JR `1058E0`; external allocator boundary |
| Pool reset | `105990..1059DC` | JAL `265C7C`; JR `1059D4` |
| Group count | `135CD0..135CF0` | JAL `1225E8`; JR `135CE8` |
| Group member | `135CF0..135F24` | JAL `12259C`; JR `135F1C` |
| Group radius | `135F30..135F80` | JAL `122560`; JR `135F78` |
| Shape matrix setter | `11F680..11FE44` | JAL `135F08` for member tag 0; common JR `11FE3C` |
| Vec3/W constructor | `106790..1067AC` | Bounds helper; JR `1067A4` |
| Vec4 minimum | `107910..10793C` | JAL `120720`; JR `107934` |
| Vec4 maximum | `11E300..11E32C` | JAL `12075C`; JR `11E324` |
| Identity matrix | `14D840..14D8BC` | JAL `1208B0`; JR `14D8B4` |
| Assertion check | `133660..1336B4` | Iterator helper; JR `1336AC` |
| Assertion print | `1336C0..133724` | Iterator failure path; JR `13371C` |

All ordinary JR extents include the delay slot. Each listed full routine's last
return/delay is preserved, and internal early returns/branches remain in its map.

## Collector calling convention and state

The verified ABI is:

- `a0`: query object
- `a1`: signed nominal capacity, saved as `s6`
- `a2`: mutable signed count pointer, saved as `s4`
- `a3`: output identity array, saved as `s3`
- `t0`: spatial method bit mask, saved initially as `s0`
- `t1`: nonzero skips coarse sphere tests, saved as `s2`
- `t2`: nonzero skips groups, saved as `s7`

At `12237C`, `*count = 0` occurs before the query flag read. Query byte `+F8`
bit 1 is tested with the original shift sequence. If clear, only bit 5 is
cleared, retaining all other bits; no output elements, bounds, sphere, current
candidate or spatial state are consumed. This is distinct from clearing every
query result or returning a Boolean success. The collector does not establish a
uniform Boolean result register; its meaningful outputs are count, array and
state effects.

The enabled branch sets query `+F8` bit 4 without clearing bit 5. It initializes
temporary center and half vec4s from VF0, runs `1202E0`, constructs a stack
iterator and calls `294600(iterator, query+E0, mode, center, half, 0, 0, 1)`.
Thus the extra global-zone option is enabled; optional min/max output pointers
are null. A null query registration chain does not mean an empty list: the
iterator substitutes the runtime global chain pointer at `443148`.

At each loop test, it calls `294740` first. A null result ends collection.
Otherwise the signed count is reloaded and compared to the signed capacity.
Only if count is less does it inspect returned object's byte `+2`. Zero and
negative capacity still reach enabled bounds/open/one next call. If capacity is
reached after an append, the subsequent next may stamp and advance an object
which never becomes query current-candidate and never gets bit 4.

### Direct-object branch

Object `+2` equals the current runtime byte at `gp-7FF0`:

1. Store the outer identity to query `+F4`.
2. Reload query `+F4`; set that object's `+F8` bit 4.
3. Unless sphere-skip is nonzero, call `126EB0(query, current)`. Continue only
   when the resulting float equals positive zero under the actual comparison.
4. Call `120E70(query, current)`; append only if nonzero.
5. Store outer identity to output at the current count; reload/increment count.

Sphere gap uses ordered binary32 XYZ subtraction, `(dx²+dy²)+dz²`, square root,
then subtraction of `(query.radius+candidate.radius)`. Nonpositive gap becomes
positive zero. It is an inclusive-touching test, not a strict squared-distance
comparison. Skipping it must also skip its input reads. Bit 4/current-candidate
side effects occur even on sphere or eligibility rejection.

The filter checks, in order: self identity; candidate `F8 & 4`; candidate
`+31 & 1`; candidate category `+A4` intersecting query mask `+B8`; then both
identity-list families in both directions and bilateral category lists.
Identity family 1 is `(mode +BC, array +C0, signed count +C4)`; family 2 is
`(+C8,+CC,+D0)`. Nonpositive identity counts bypass that family. For positive
counts, mode 1 rejects membership; mode 0 rejects nonmembership; other modes
impose neither rejection. Category lists use `(+D4,+D8,+DC)`: mode 1 rejects
any intersecting category; every other mode requires an intersection. Empty or
negative category allow-lists therefore reject. No read-only list is sorted,
normalized or replaced by a guessed default category.

### Group branch and expansion

If the outer type did not match direct-object, groups are skipped when `t2`
is nonzero. Otherwise the byte must match runtime `gp-7FEC`. The initial ELF
bytes at `3D5980` and `3D5984` are both `FF`; these are initialization data,
not valid evidence that either type ID remains `FF` after registration. Runtime
type IDs must be supplied and distinguished explicitly. If equal, the direct
branch wins because it is tested first.

Even with sphere-skip set, the group branch constructs center
`(u16(group+8)*worldScale, +0, u16(group+C)*worldScale, 1)`, reading
`world = *(3E1CE8)` and scalar `world+590`. Unsigned conversion expansion is
preserved; the negative conversion paths are unreachable for a true `LHU`.
When sphere testing is enabled, `135F30` looks up archetype
`*(world+6C+4*u8(group+E))`. With `b = s8(group+11)`, it returns
`archetype+18C + (float(b)*(archetype+194-archetype+18C))/128`, preserving
operation order. `126E40` then computes XZ sphere gap and clamps nonpositive
to zero. Query Y and group Y are not sphere inputs on this branch.

The signed member index starts at zero. Every loop test calls `135CD0` anew,
which reads archetype signed count `+1FC`. For each index below that count:

- `135CF0(group,index)` resolves `member = archetype.+204[index]`.
- It returns null if member shape `+8C` is null. The collector then still
  dereferences current candidate `+F8`; null is an unsafe reached boundary,
  not an original graceful empty-member result.
- The helper computes group XYZ from unsigned `+8/+A/+C` and world scale,
  plus two scale ratios from signed group bytes `+10/+11` and archetype
  `+180/+184/+18C/+194`. These calculations happen before its shape branch.
- Member tag 5 writes shape `+58/+5C` ratios and `+40/+44/+48` XYZ.
  Tag 4 writes `+68/+6C` ratios and `+30/+34/+38` XYZ.
- Tag 0 copies the shape's matrix `+80`, changes its translation, writes
  shape `+B0/+B4/+B8`, and calls the real `11F680` tag-0 matrix setter.
  That setter updates shape matrix caches and member sphere; it is not a
  no-op or registration call. Its tag-0 branch has no further helper calls.
- Other member tags return the same member without those shape writes, after
  computing the common arithmetic.

The collector stores the returned member in query `+F4`, increments member
index, sets member `+F8` bit 4, and appends **outer `s0`**, the group identity,
at `1225D4`. No per-member sphere, filter, category, nominal-capacity check,
deduplication or null recovery is present. A group can increase count beyond
nominal capacity. Native backing must be sufficient for the actual expansion or
report an explicit safe unsupported/backing boundary; silently capping changes
the source. Later groups/objects may share member identities and overwrite the
same member's payload/flags sequentially.

## Bounds preparation, including shape 3

The table at `388AB0` selects tags 0..6 as
`120378,1203FC,12044C,1205EC,1209F8,1208E4,1209A8`.
Tags 1 and 6 match the frozen 48R contract: tag 1 takes payload center `+0`,
half `+10`, matrix `+80`; tag 6 takes center `+40`, half `+50`, matrix `+0`.
Their full matrix/vec4 copies precede the common scalar center and sparse-axis
half-extent transform. The common half XYZ is
`abs(rotZ) + (abs(rotX) + abs(rotY))`. Sparse zero multiplies, additions and
final `+0` terms must not be collapsed. The center output W becomes 1; the
half output W is retained from the caller's initialized VF0. See the preserved
48R map for the complete ordered arithmetic; this packet freezes the same
bounds words independently and does not edit that record.

For shape 3, `shape = query+8C` has signed pair count `+0`, a per-endpoint
pointer array at `+4` and vec4 endpoint array at `+8`. For pair `i`, pointer
entries `2*i` and `2*i+1` control the corresponding endpoint independently:

- Null pointer: copy all four endpoint words to the temporary vector.
- Nonnull pointer: directly read its matrix at `pointer+100` and execute the
  original cached-world point transform; output W becomes 1. There is no
  scene dirty-flag check, cache refresh, parent traversal or setter call here.
  An arbitrary metadata token cannot be substituted for an optional scene.

Initialize min/max XYZ to `+FLT_MAX/-FLT_MAX`; initial W is 1. Full-lane
min/max can change W during traversal, independently of the XYZ results.
For each pair, first take min(previous min, second endpoint), then min with
first endpoint; similarly max(previous max, second), then max with first.
Use corrected VU lane selection including signed-zero ordering. The helper
computes `half = 0.5*(max-min)` and `center = 0.5*(max+min)`, sets temporary
W to 1, copies center to query `+60` and computes radius as the ordered length
of half, writing query `+80`. It builds a real identity matrix via `14D840`
and then still executes common transformed center/half output code. Replacing
that tail with simple assignments can alter signed zero and consumption.

This is separate from `11CA80` detector pair staging. It reads the endpoint
arrays/metadata that exist **at collection time**; it does not use a detector's
later staged endpoint state or infer that scene-pointer metadata is absent.
The positive finite-pair branch is the clear bounded recommendation. Zero or
negative signed pair count performs no endpoint reads but leaves opposing
finite extrema; their subtraction overflows in half/radius arithmetic under
the existing finite host model. Do not present it as a harmless empty AABB.
Other bounds tags are frozen for boundaries, not implicitly implemented.

## Spatial iterator: actual structures and order

Iterator `294590` initializes center to `(-FLT_MAX,-FLT_MAX,-FLT_MAX,1)`, half
to `(0,0,0,1)`, and zeros `+20/+24/+28/+34/+38`; `+2C/+30/+3C` become defined
by open. The 64-byte iterator layout is:

| Field | Meaning |
|---|---|
| `+00/+10` | Center/half vec4 copies |
| `+20` | Method mask, bit values 1 and 2 used |
| `+24` | Current zone |
| `+28` | Current search-method identity |
| `+2C/+30` | Initial/current zone-link identity |
| `+34/+38` | Optional caller min/max result pointers, null in collector |
| `+3C` | Allow global fallback zone, set to 1 by collector |

A zone link has `+0 = next link`, `+4 = zone`. Each zone has search methods
at `+34` and `+38`. Open uses the supplied chain or `*(443148)` when null,
loads its first zone, prefers method `+34` when mask bit 1 is set, and otherwise
(or if null) chooses `+38` when bit 2 is set. It calls method `+8` vtable slot
`+30` with center, half and optional min/max. There is no null-chain guard.
A missing selected method reaches assertion/dereference boundaries.

Next first invokes selected method slot `+38`. A nonnull identity is returned
immediately. On null:

1. If the current method equals zone `+34`, mode bit 2 is set and zone `+38`
   differs from `+34`, select the latter. Identical pointers are not reopened.
2. Otherwise clear method, advance one zone link if a current link remains,
   load the new zone and select first/second method under the same mask rules.
3. If still no method and fallback is allowed, read global zone `443108`.
   If nonnull and different from the current zone, use it and select a method.
   This is at most the observed fallback transition, not an invented world list.
4. If no method remains, return null. Otherwise open the newly selected method
   and call its next exactly once. Return that value, including null.

The collector uses null optional min/max pointers, so the extended union-output
branch `294A04..294AF8` is unreachable for this call contract. It is frozen but
need not be synthesized. The wrapper never closes a method at collector return.
Next/open return values other than next's candidate identity are not used to
turn a failed open into a skipped method.

## Authenticated QuadTree route

The vtable words at `3D2340` identify open `265040`, next `2655C0`, rewind
`265C40`, close `265C60` in slots `+30/+38/+3C/+40`. `263C0C..263C14`
constructs `3D2340` and stores it at object `+8` within the destructor's type
reset. The table's first word points to RTTI `3B8FA8`, whose name pointer
resolves to the frozen `QuadTree` string at `3B8F40`. This authenticates one
specific possible type. It does not prove arbitrary runtime methods have this
vtable. Every reached native method needs an explicit supported identity.

### Open and selection

Read query center/half X and Z, origin tree `+30/+38`, sizes `+48/+4C`.
Compute `(center-half)-origin` and `(center+half)-origin`; swap each interval's
ends if reversed. Reject when maxX<0, minX>sizeX, maxZ<0 or minZ>sizeZ.
**Rejection returns zero without clearing previous tree traversal state.** The
spatial wrapper ignores that return and still calls next, so stale initialized
state can remain observable.

Otherwise multiply all four bounds by scalar tree `+5C` and use the four
original `CVT.W.S` conversions. The [PCSX2 interpreter CVT_W](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp)
and [recompiler recCVT_W](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/x86/iFPU.cpp)
independently use truncation toward zero for representable finite inputs; the
recompiler uses CVTTSS2SI and separately saturates positive overflow. A native
cast is appropriate only after establishing its representable domain. Saturation,
exceptional inputs and other EE arithmetic remain separate proof obligations. For signed quantum `q=tree+58`,
source computes mask `-q`; mins become `value & mask`, maxes become
`(q-1) + (value & mask)`. It does not simply clamp values to a native grid.

If search flag tree `+16` bit 0 is set and all four cached coordinates match
`+8A8/+8AC/+8B0/+8B4` (minX/minZ/maxX/maxZ), call rewind. Rewind restores
`+A8` from `+A4`, zeros `+AC/+B4`, and increments the 16-bit epoch `+14`.
It does **not** clear item-block cursor `+B0`, finished field `+898`, or object
stamps. The original truncated ushort wrap must remain observable.

For a changed active rectangle, call close before rebuilding. Close calls
pool reset `105990`, clears `+A4/+A8/+AC/+B4`, writes `80000000` to all
four rectangle caches, and clears search bit 0. It also does not clear `+B0`.
Fresh rebuilding subsequently zeros `+B0`, `+A8/+AC/+B4` and `+898`, sets
search bit 0, increments epoch, saves rectangle and writes global active-tree
pointer `442E18`. Root is tree `+94`; pool is `+9C`.

The recursive selection starts with cell half-size `4000`, origin `(0,0)` and
minimum subdivision size tree `+58`. Child array is node `+0`, four nullable
child pointers in index order 0..3. Child bit 1 offsets X; bit 0 offsets Z.
Overlap tests are inclusive integer comparisons. Fully covered child subtrees
use `2654C0`; partial overlap recurses `265330`. Each selected node is prepended
as `{next, node}` through the actual pool helper. The parent is prepended after its children have been processed, so the resulting chain visits parent then
reverse child traversal order. Do not sort nodes or objects afterward. Root
null leaves the selected-node head empty. At completion `+A4 = +A8` caches
this head for a matching future query.

### Pool and allocation boundary

`105720` obtains an 8-byte node link from initialized pool state. Its first
route uses signed used count `+20 < capacity +10`, stride `+24`, active block
`+18`; result is `block + stride + oldUsed*stride`, then used count increments.
Other original routes pop a free item at `+2C`, or reuse spare block `+1C`
while updating active-head/current `+14/+18`. A nonnull acquisition adds stride
to accounting `+34`.

If these routes cannot supply backing, `1057D8` calls `105870`, whose
`105880` calls external allocator `305700` with size `pool+28`. That is a
precise allocation boundary. No allocator behavior, ownership, failure recovery
or zero-allocation claim is inferred. A bounded native implementation should
accept caller-preallocated serial workspace sufficient for the selected node
links, preserve their original construction order and report exhaustion instead
of guessing an allocation result. Actual pool acquisition/reset semantics can
be covered independently; merely stopping before growth does not prove them.

### Next, duplicate suppression and carryover

`2655C0` returns null if tree `+16` bit 0 is clear or `+898` is nonzero.
Otherwise it writes global active-tree `442E18`, then traverses:

- `tree+A8`: current selected-node link `{+0 next,+4 node}`
- `node+4`: linked item-block head
- each item block: `{+0 next, +4/+8/+C/+10 four object identities}`
- `tree+B0`: current item-block identity
- `tree+AC`: next slot index in that block

For each slot, increment slot index first. A null identity is skipped. A
nonnull identity is returned only if its `u16(object+0)` stamp differs from
`u16(tree+14)`; set its stamp in the return-path delay slot. Once four slots
are consumed, reset slot index and advance item block; once blocks are empty,
advance selected-node link. Search does not compare the object's own AABB.
The quadtree selection is the coarse geometric test.

Shared identities must remain shared. Two trees with equal epochs can suppress
an object already stamped by the first; different epochs can return it again.
The same underlying object may be query identity, direct identity or group
identity. Group members have their own flags and payloads, while the spatial
stamp belongs to the outer returned group. There is no general permission to
flatten groups, deduplicate outputs or reset all stamps between collector calls.

## Boundaries and implementation recommendation

The suitable next branch is serial collection over caller-initialized, explicitly
bound quadtree/zone/node/item/pool state with original identity aliasing. Keep
count/output, query flags/current candidate/cached sphere, candidate/group flags
and payloads, tree epoch/cursors, pool accounting and any reached global
active-tree state separate and test them. Nominal output capacity and physical
backing capacity are distinct. Keep the vehicle caller's two passes in order:
`(mode,skipSphere,skipGroups)=(2,1,1)` then `(1,0,0)`, each nominal capacity64;
its ordered capped merge belongs to the separately mapped caller work.

Recommended proof staging is: disabled query; cached initialized QuadTree route;
actual fresh bounded node selection with preallocated pool; direct filtering;
group expansion; type-3 bounds/cached-sphere writes; two-pass/caller composition.
Do not infer a feature's completion merely from this recommendation.

Exact unresolved external calls in the frozen dependency graph are:

- `105880 → 305700`: pool block allocation, reached only on growth.
- `11F910 → 28B550`, `11F9AC → 11E630`: other shape-matrix-setter branches;
  not reached by group member's tag-0 route. No arbitrary scene/shape completion.
- `13368C → 3075A8`, `133694 → 2EB0C8`, `133710 → 30D920`: assertion failure
  diagnostics. Safe initialized nonnull method checks avoid those branches;
  unsupported null/unknown state must not be converted to an artificial return.
- Every indirect method call without an explicit verified QuadTree identity,
  including unrelated derived vtables, remains an unknown virtual boundary.

Iterator assertion lazy bytes at `gp-7370/-736C/-7368/-7364` and state words
`443528/443530/443538/443540` have source-visible initialization branches.
Their initialized state must be explicit in a bounded proof. The check helper
reads `36C100` (initial ELF word1), returns zero for a satisfied condition,
and only invokes external diagnostics for an enabled failed condition. The
map neither patches these calls nor provides fake successes.

World registration maintenance (`120230/280D40`), live engine ownership,
constructors/destructors, type-ID assignment, scene refresh of endpoint owners,
arbitrary object models, malformed/cyclic graphs and allocator behavior remain
outside this map's implemented claim. No PC files, guest RAM, emulator process,
fixture source snapshot or running game were accessed to derive this packet.
