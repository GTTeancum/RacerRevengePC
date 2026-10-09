# 48P: original type3 query dispatcher source map

## Scope and immutable evidence

This map freezes original `126F40`, its reachable numeric mesh traversal and
interpolation helpers, and the limited-list preparation/finalization seams.
It is source analysis, not an executed fixture or production implementation.
No guest RAM, saved-runtime source data, registration, game execution, desktop
access, installation, build, or test is used by the freeze script.

Run `python3 evidence/type3Query48P/map/freeze_map.py`. It reads the existing
`work/native/project/disc/SLUS_202.68`, requiring SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
It verifies the ELF32 little-endian PT_LOAD at file offset `80`, VA `100000`,
file size `2D5A00`, memory size `34F600`. File offset = VA - `100000` + `80`.
There are 4,931 frozen words / 19,724 bytes in 12 named ranges. Each has a raw
binary, annotated text, exact words and hash in `original-words.json`.
Separate `dispatch-tables.json` and two 28-byte binaries freeze the exact seven-entry
limited-list switches at `388B90` and `388B70`; query tag3 resolves to `122780`
and `122A10` respectively. Existing 48O evidence and sources are unchanged.

Archived annotations are reused only for ELF-verified words. Location-sensitive
branches are decoded from the current PC. Identical VU instruction words can
reuse register-operation annotations at other addresses. Two packed memset
instructions remain explicitly raw, not guessed. `4BE5233C` at `127028` is
`VMOVE.xyzw vf5,vf4`: SPECIAL2 index 48, with destination Ft and source Fs.
This annotation was checked against [PCSX2's opcode table](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/R5900OpcodeTables.cpp)
and [raw-lane MOVE implementation](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/VUops.cpp)
on 2026-10-06. This confirms a decoder/model operation, not physical EE timing.

## Three distinct tag/category namespaces

- `query+88 == 3` means the query owns the ray-array shape consumed by `126F40`.
  Detector staging `11CA80` writes this tag, including for a null shape.
- `candidate+88` selects the candidate shape. Only **0 (one mesh record)** and
  **1 (an ordered array of mesh records)** enter geometry in this dispatcher.
  Candidate shape tags 2, 3 and every other value return zero.
- `candidate+2` is a collision-list enumeration tag compared with `gp-7FF0` or
  the outer-group tag `gp-7FEC`. It is not the shape tag. Callback static/dynamic
  categorization separately uses the candidate category at `+A4` and masks.

Nothing inside `126F40` changes query/candidate shape tags. Shape-tag 0 does not
by itself establish a static-body callback. Runtime ownership and the concrete
values of collision-list tags and category masks remain caller inputs.

## Dispatcher inputs and exact gates

`126F40(a0=shape, a1=candidate, a2=result)` uses nonnull, valid backing without
allocation or capacity validation. All counts below are signed loop counts
unless stated otherwise.

1. `126F94..126FB0`: copy `shape+44` to global `3DF5B0`; call actual
   `306478(result.hitFlags, 0, shape.count << 2)`. This clears all pair hit flags
   **before** checking candidate activation or any geometric bound.
2. `126FB4..126FC8`: if byte `candidate+31` bit 0 is clear, return 0. No fraction
   reset or result count reset has happened.
3. `126FCC..127050`: compute center difference `shape+30 - candidate+60`, sum
   its XYZ squares, compare **strictly less** than
   `(candidate.radius80 + shape.radius40)^2`. Equality is a miss. On failure,
   return 0 with only the global mode and hit-mask effects above.
4. After passing that sphere check: set `result+74=1.0`, fill all pair fractions
   at `result+6C` with raw `BF800000` (-1), and set `result+60=0`.
   Selected vectors and per-pair vectors are not cleared.
5. Read candidate shape tag at `+88`. Tags 0 and 1 execute below; all others
   return 0, retaining these reset effects.
6. The return hit is derived by scanning pair hit flags after traversal; it is
   not the mesh-walk helper's integer return. The helper always returns 1 even
   when pruned or with no geometric hit (`10F56C`).

Thus misses at different gates have different observable state. A transactional
native wrapper may stage these writes, but Complete/miss must commit the exact
phase's effects. Negative/wrapped counts must be rejected before host writes:
source memset length is a 32-bit shift, while later loops compare signed counts.

## Shape and result layouts at this seam

| Object | Offset | Actual use |
| --- | --- | --- |
| shape | `00` | Number of rays / endpoint pairs |
| shape | `04` | Source scene-pointer metadata, two 32-bit entries per pair |
| shape | `08` | Source endpoint Vec4 array, two per pair |
| shape | `0C` | Writable candidate-local starts, Vec4 per pair |
| shape | `10` | Writable candidate-local directions, Vec4 per pair |
| shape | `14` | World-derived ray lengths, one float per pair |
| shape | `18` | Prepared world starts, Vec4 per pair |
| shape | `1C` | Prepared world unit directions, Vec4 per pair |
| shape | `20` | Writable candidate-local overall center Vec4 |
| shape | `30` | Prepared world overall center Vec4 |
| shape | `40` | Overall bounding sphere radius |
| shape | `44` | Normal interpolation mode |
| result | `00`, `10` | Reset to supplied global sentinel by `11DD00`; not per-hit selected data |
| result | `20` | Selected normal Vec4 |
| result | `30` | Selected surface point Vec4 |
| result | `40` | Selected correction Vec4 |
| result | `54` | Per-pair normal Vec4 array |
| result | `58` | Per-pair surface point Vec4 array |
| result | `60` | Count of geometric triangle/ray hits, including non-improving hits |
| result | `68` | Per-pair 32-bit hit flags |
| result | `6C` | Per-pair fractions |
| result | `74` | Selected global fraction, reset to 1 |

`126F40` does not use `result+50` or write pair IDs, face IDs, scene metadata, or
candidate identity. Those cannot be invented from the numerical result. The
outer limited-list query updates current candidate at `query+F4` separately.

### Preparation from staged endpoints

`11DD00`'s type3 branch is `11DEB8..11E2D4`. It reads the independent shape count
and both metadata slots for each pair. A nonnull endpoint metadata pointer uses
the matrix at that scene `+100`; a null pointer copies the endpoint Vec4 raw.
There is no scene refresh call here. The detector's `11CA80(...,0)` staging
therefore leaves endpoints already in world space and both metadata slots zero.

For each pair, it writes world start at `shape+18`, computes XYZ delta=end-start,
length as `sqrt((dx*dx + dy*dy) + dz*dz)` with original scalar rounding order, and
writes one float length at `shape+14`. Positive length writes XYZ delta multiplied
by scalar reciprocal length, with W=1, to `shape+1C`. Nonpositive length copies
the supplied global Vec4 at `3E02C0` instead. Null metadata preserves source
start.W; transformed starts have W=1.

The branch accumulates endpoint bounds, writes their midpoint (W=1) to shape+30
and query+60, and writes the half-extents' norm to shape+40 and query+80. It then
sets result+00/+10 from `3E02C0`, fraction74=1, count60=0. `124F70` only publishes
the result pointer to global `3DF5B8`; it does not clear result storage.

`3E02C0` lies beyond the file-backed PT_LOAD end `3D5A00`. Its value is **not an
ELF constant proven here**. Original-word fixtures must explicitly initialize
it and label it as input. No saved RAM should be retrieved to fill this gap.
The finite production preparation contract should require positive pair count
and finite used intermediates; empty bounds otherwise propagate extreme values.

## Accepted candidate 0: one mesh record

`candidate+8C` points to a mesh record. Its first 64 bytes are its complete
world matrix. `mesh+100` is the vertex descriptor; `mesh+114` points to 36-byte
nodes and `mesh+118` points to 20-byte triangle records. The dispatcher always
starts with node tag 0, not a caller-supplied root tag.

`1270C4..12715C` copies the matrix to stack and constructs a transpose-based
inverse: transpose its 3x3 basis and negate the transformed translation. It is
not a general inverse and does not validate orthogonality or repair scale/shear.
The original zero additions must remain for signed-zero behavior.

For every ray, in index order:
- `12716C..1271A8`: rotate world direction by the transposed basis, then add vf0
  to the full Vec4 and store in `shape+10`.
- `1271AC..1271E4`: transform world start by that inverse with W explicitly 1,
  store in `shape+0C`.
- Length is unchanged. Do not renormalize local directions after transform.

`1271FC..127230` transforms world overall center into shape+20 (W=1).
`127258` then calls:

`111300(shape, 0, &{mesh.nodes114, mesh.triangles118}, mesh+100, result, 1)`.

`111300` publishes the descriptor to global `3DFE60`, shifts the result/mode
arguments, and tail-calls `10E080(shape, tag, treePair, result, mode=1)`.
These globals establish original serial behavior; native code should carry
explicit context instead of making this path concurrently reentrant by accident.

After traversal, `127278..1272F8` scans pair flags in ascending order. Every hit
pair's point and normal are transformed in place to world space. If any pair
hit, `127304..127398` also transforms selected point, normal and correction,
regardless of whether the selected fraction was ever improved.

### Fourth lanes

Point transforms explicitly set vf9.W=1. Rotation-only transforms update XYZ,
then `VADD.xyzw vf9,vf9,vf0` increments its existing W. Consequently:
- Per-pair output point.W=1 and normal.W=2.
- Selected point.W=1, normal.W=2, correction.W=3.
- First candidate-local direction.W depends on incoming vf9.W + 1. Each later
  direction.W=2 because the prior start transform set W=1.

These are source sequences; an executed full-call proof must establish incoming
vf9.W before claiming the first scratch W is constant. Do not normalize or zero
untouched vectors and raw W lanes.

## Candidate 1: ordered mesh-record array, separate unsupported boundary

`1273A8` loads candidate shape+8C, then count from container+E0 and the first
record from container+F0. Records advance by `130` bytes and signed loop order
is preserved. Each record has the same matrix/mesh fields as candidate 0.

There are important source differences, so tag1 cannot be flattened into an
ordinary independent-candidate list:

- World starts/directions are transformed anew per record, but this branch has
  **no shape+30 to shape+20 center transform** before its mesh walk. It consumes
  the existing shape+20 for every record's broadphase.
- Hit flags, fractions, count and selected fraction are reset once per outer
  candidate, not once per record. The `s5` hit flag is cumulative too.
- After each record, all previously flagged pair vectors are transformed with
  that record's matrix, even if the record produced no new or closer hit.
- Once any record hit, selected vectors are similarly transformed after every
  later record, including a later record that missed.

These cumulative/local-versus-world effects are not corrected here. Support
requires explicit initialized center/state and fixtures including nonidentity
matrices, a hit followed by a miss, and equal/farther hits. Candidate0-only is
the recommended first production boundary. Empty/negative tag1 counts skip its
record loop but retain dispatcher reset effects.

## Original ray-array mesh traversal

### Tree/node contract and order

`10E080` masks the node tag to 16 bits; bit `8000` means triangle leaf and the
low 15 bits index triangles at stride20. Otherwise it indexes nodes at stride36.
Nodes supply child tags as halfwords at +8 and +A, center XYZ at +C/+10/+14,
and half-extents XYZ at +18/+1C/+20.

Mode1 first checks the node box against the **overall local center and unchanged
radius** using three inclusive `abs(delta) <= half + radius` tests. It then tries
rays in pair order. A start already inside the box passes. Otherwise it tries
six box-face intersections in order +X, -X, +Z, -Z, +Y, -Y; each requires time in
[0,length] and the other coordinates within inclusive extents. Zero direction
components are divided by, not skipped. IEEE-model infinity/NaN behavior in
these transient calculations is distinct from physical EE exceptional behavior.

As soon as any ray overlaps, recurse into child+8 then child+A, **both**, with
all rays and mode1. It does not recursively visit once per overlapping ray or
stop after a hit. A leaf tests every ray in index order. Thus result tie order
is leaf traversal order first, ray order inside each leaf.

Mode0's separate spherical-node branch is frozen but never selected by this
dispatcher's constant mode argument. Do not silently replace mode1 pruning with
that branch, the type2 segment/AABB routine, or a conventional slab test.

### Vertices, geometric hit, and interpolation

Descriptor `mesh+100` has vertex pointer+0, optional vertex-normal pointer+4,
and storage-format flag+8. The first three unsigned halfwords of each triangle
record are vertex indices. Format0 uses float3 vertices; nonzero uses packed
signed shorts and an external scale at `383640`. Packed input is a separately
bounded feature and is not required for the first native scope.

The leaf obtains vertices through `106ED0`, computes ordered cross-product face
normal, normalizes with `106EA0`, and requires faceNormal dot localDirection <0
and length>0. It computes fraction in inclusive [0,1], then point in exactly
`start + length * (fraction * direction)` order. Dominant-axis projection and
ordered edge scanline comparisons establish inclusion; no replacement
barycentric algorithm is implied.

On an improving per-pair hit, the normal is the computed face normal when
normals are absent or shape.mode44=0. If normals exist, mode1 calls `1106B0`;
other nonzero modes call `10F580`, after vertex-normal loading via `111100`.
The latter reads float3 or packed-normal triples and sets each supplied W=1.
Mode1 interpolation is independently reached code here; similarity to the
existing 48H triangle function alone does not prove equivalence. Preserve or
explicitly reject original edge/interpolation boundaries, rather than inventing
fallback normals.

### Result selection and correction

- `10EAE4` accepts a per-pair result only for strict smaller fraction, or when
  its existing fraction is negative (`10EAFC`). Equal/farther hits retain the
  earlier per-pair vectors and fraction.
- Every geometric hit nevertheless sets hitFlags[i]=1 and increments result+60
  (`10EC28..10EC3C`). Count is not the number of distinct hit pairs.
- Global selection at `10EC48` is **strict `<`**, using the pair's retained
  fraction, against result+74. This differs from the type2 global `<=` rule.
- On selection, copy that pair's full normal and point to +20 and +30. Compute
  `remainingDirection=(1-pairFraction)*localDirection`; **do not multiply by
  ray length**. Dot it with the selected per-pair normal, then multiply that
  normal by the dot to form correction+40, W=1 before world conversion.
  The runtime packed face normal used by type2 is not used for this correction.
- A geometric hit at fraction exactly1 can set flags/count and cause dispatcher
  return1 while global selected vectors remain prior bytes because 1 is not <1.
  The dispatcher still transforms those prior vectors. This is an initialized
  stale-state contract, not permission to silently use a new endpoint contact.

Fractions/masks reset for each `126F40` candidate. They do not accumulate across
outer list candidates like the existing type2 48N pairs. Candidate-order effects
therefore mainly arise from callbacks and retained selected/raw state, with the
separate candidate1 record aggregation described above.

## Limited list, callbacks, and remaining boundaries

`122660` sets query.F8 bit4, then calls `11DD00` once. Candidate input is examined
in supplied order. Ordinary collision-tag matches update query+F4 and set that
candidate's F8 bit4 before the outer `126EB0` sphere and `120E70` bilateral
filters. Query tag3's switch case at `122780` invokes `126F40` as above.

On a dispatcher hit, set query.F8 bit5, call `11E330` (type3 no-op), copy query
+40/+50 to candidate+40/+50, then invoke query callback+E4 and candidate callback
+E8 if present. These copied query fields are not replaced from type3 selected
results by `11E330`. They must be explicit prior state. The list continues after
hits; its final `11E470` is also a type3 no-op. Return is query.F8 bit5.

The outer grouped collision-tag branch uses world/group helpers `135F30`,
`135CF0`, `135CD0`, a global world scale and a secondary type switch. This is a
different grouping mechanism from candidate shape1. Full-world `121920`, outer
group enumeration, world registration, dynamic-body callbacks, and candidate
callback arbitrary function pointers are not included in a static numeric API.
Static type3 `294BB0` callback mapping and proof belong to the separate callback
work; its support must not be inferred from this dispatcher alone.

## Smallest useful fixture and native API scope

Start with explicitly initialized positive-count staged world endpoint pairs,
zero metadata, candidate shape0, valid caller-owned result and scratch arrays,
float vertices, mode0/no normals, a valid node-root0 tree, finite affine matrix,
and explicit prior selected vectors. Cover at least two pairs and two ordered
leaves. Then add mode1 normals and prove the interpolation domain separately.

Expose prepared world rays (start, direction, length), center/radius, mode,
candidate active flag and sphere, typed mesh/matrix, previous result state,
caller-owned local-ray scratch, pair outputs, selected result and geometric-hit
count. Keep original reset/derive and list/callback composition as visible
separate phases unless their source ordering is fully incorporated. Use native
IDs for identities; no guest pointers, interpreter, global register arrays, or
invented helper stubs in production.

Required discriminating fixture cases include gate-specific resets; sphere
strict equality; tag2/3/default no-geometry effects; zero rays for direct
already-prepared dispatcher; partial-pair miss; equal/farther/closer triangles;
selected tie across pair indices; fraction1 with initialized prior selected
vectors; changed candidate matrix; zero direction components; raw W behavior;
and a later outer candidate miss after an earlier hit. Unknown capacity, invalid
indices/cycles, aliasing or unsupported shapes must be explicit transactional
errors, not truncated hits or false misses.

Memory should scale with pair count and validated tree depth, using supplied
buffers and reusable traversal stack. The original recursive walker frame is
0x390 bytes; a native serial iterative stack avoids copying that frame without
altering child order. No low-memory console target budget or live binding is
established by source mapping. Native arithmetic must retain original binary32
operation order; NaN payloads, denormals and full physical EE/VU exceptional
semantics are outside this map's numerical proof claim.

## Leaf-worker translation contract

For isolated original-word execution, enter `10E080` with:
`a0=shape`, `a1=8000|triangleIndex`, `a2=&{nodes,triangles}`, `a3=result`,
`t0=1`, global `3DFE60=&vertexDescriptor`, global `3DF5B0=normalMode`.
Set shape.count=1 for a one-ray fixture. The leaf path starts at `10E0EC`, loops
through `10E140..10ED0C`, and exits through `10ED14/10F540..10F574`. It does not
read node bounds. All source arrays, result arrays and selected prior Vec4s must
have explicit initialized backing. This is a valid helper call, not a patched
helper stub or a manually simulated return.

A pure native kernel can take:
- float3 vertices A/B/C and optional float3 vertex normals, plus normalMode;
- local start/direction Vec4 and scalar length (direction need not remain unit
  after the dispatcher's transpose of a nonorthogonal basis);
- mutable per-pair fraction, point/normal Vec4 and hit flag;
- mutable selected fraction, point/normal/correction Vec4 and hit count.

No triangle packed face normal or plane-D input is needed: those record fields
are not loaded by this path. Bounds belong to traversal, not this leaf kernel.
A finite affine target matrix is sufficient to exercise the original transform;
do not demand an orthogonal basis merely to make the unchanged length easier.

Let `add/sub/mul/div` denote individually rounded binary32 operations and
`dot(a,b)=add(add(mul(ax,bx),mul(ay,by)),mul(1,mul(az,bz)))`.
The operation anchors are:

| Range | Kernel operation |
| --- | --- |
| `10E19C` / `106ED0` | Load indexed A,B,C as Vec4, W=1 |
| `10E1A4..10E22C` | U=B-A,V=C-A; cross(U,V) in ordered VU products/subtractions; cross.W=0 |
| `10E230` / `106EA0` | Normalize cross via VU dot, reciprocal sqrt and XYZ multiply; preserve normal.W=0 |
| `10E274..10E2A4` | facing=dot(normal,direction); require facing<0 and length>0 |
| `10E2B4..10E338` | fraction=div(dot(normal,A-start),mul(facing,length)); require 0<=fraction<=1 |
| `10E340..10E39C` | point.xyz=start.xyz + length*(fraction*direction.xyz), point.W=1 |
| `10E3A0..10E4F4` | Choose projection axes; Z dominates only when abs(Nz)>abs(Nx) and abs(Nz)>abs(Ny); otherwise Y dominates iff abs(Ny)>abs(Nx); remaining case drops X |
| `10E4F8..10EAD4` | Six ordered scanline edge-pair branches; final interval is inclusive |
| `10EAD8..10EB20` | On geometric hit, update pair iff newFraction<oldFraction or oldFraction<0; point copied only on update |
| `10EB28..10EBF4` | Normal update only on that pair update, mode/normal-pointer dispatch described above |
| `10EC20..10EC3C` | Set pair hit flag=1; hitCount++ for every geometric hit |
| `10EC40..10EC74` | Compare retained pair fraction strictly<selectedFraction; copy pair normal/point if so |
| `10EC78..10ECD8` | V=(1-pairFraction)*direction; correction=dot(V,pairNormal)*pairNormal; W=1 |

When projected coordinates are `(x,y)`, denote `a=A.y`, `b=B.y`, `c=C.y`,
`q=point.y`. The edge pairs in order are identical to the branch structure below;
all inequalities and edge orientations matter:

1. `q<=a && !(q<b)`: first A→B; second B→C if q<=c, else A→C.
2. `q<=b && !(q<a)`: first A→B; second A→C if q<=c, else B→C.
3. `q<=c && !(q<b)`: A→B,B→C if q<=a, else B→C,A→C.
4. `q<=b && !(q<c)`: A→C,B→C if q<=a, else A→B,B→C.
5. `q<=a && !(q<c)`: A→C,B→C if q<=b, else A→B,A→C.
6. `q<=c && !(q<a)`: A→B,A→C if q<=b, else B→C,A→C.
7. Otherwise miss.

Each directed edge I→J intersects the scanline at
`add(I.x, div(mul(sub(J.x,I.x),sub(q,I.y)),sub(J.y,I.y)))`.
For intersections r0/r1, accept when
`(point.x<=r0 && !(point.x<r1)) || (point.x<=r1 && !(point.x<r0))`.

### Reusing 48H carefully

The face-normal/intersection/projection operations match the existing 48H
geometric algorithm at source level. It is reasonable to validate reuse of
`intersectTriangle48H` with `hasVertexNormals=false` for this geometric pass,
then independently test any mode1 normal reuse against `1106B0`. This map alone
is not a claim that the frozen 48H fixture corpus validates type3.

Do **not** unconditionally call the 48H routine with normals enabled. It
interpolates on every geometric hit, whereas type3 interpolates only when the
per-pair fraction improves. An equal/farther hit at an interpolation boundary
must still mark/count the hit without reading the unused interpolation slots.
Nor can 48H's hit-only output replace type3's prior-state/strict-global-selection
contract or its separate unit-direction correction calculation.

For flat normals the pre-world local normal.W is0; for valid mode1 interpolation
it is1. Local point and freshly selected correction W are1. Dispatcher world
conversion subsequently imposes the W behavior documented above.
