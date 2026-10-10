# Source-derived enabled probe authoring50FQ

`tests/native/terrain_probe_authoring50fq_fixture.h` is a cold integration owner
for the actual Pod01 vehicle+348 single-pair terrain probe. It does not execute
guest code, use captured RAM, provide a ground-height substitute, run a frame,
or manufacture a contact normal. Root owns compilation and live integration
verification; this authoring recovery performed no tests or game execution.

## API and lifetime

Namespace `terrain_probe_authoring50fq_fixture` provides `Bindings`
(`SourceBindings` is an alias), `Receipt`, `Owner`, and `prepare`.

`prepare(owner, main690, parameters, phy, bindings, error, archivePath)` accepts
the existing main690 query/resource, already-authored canonical frame parameters,
actual PHY bytes, and an explicit application binding. The optional archive path
defaults to `work/native/project/disc/PODS/POD01/POD01.RES`.

It re-extracts actual `Pods\Pod01\podboxm.col`, uses the existing production
native decoder, and compares every root/matrix/node/ray field against main690.
File-absent opaque endpoint W words come from that existing resource solely to
perform the comparison; they do not become new probe geometry. Actual probe
endpoints are constructor-defined four-lane points with W=1. The probe is
rootless type2 with one pair, unlike main690's multi-pair podboxm.

The cold call checks positive authored mass against the canonical main body's
mass and checks authored lift against canonical visual7EC. It returns:

- `initial`: constructor terrain history plus local pair geometry
- `follow`: all consumed `TerrainFollowParameters48G` fields
- `lowerEndpoint140`: the separate component seed, before pair-local conversion
- `metadata`: persistent query filter, callback masks, type tags and copied words
- `receipt`: source PHY values, converted mass, podboxm half-height324, endpoints,
  local offset and actual source byte counts

Output is committed only on success. Calling this on an already-prepared owner
is rejected. It does not modify the frame, publish an owner, choose membership,
or reset a moving vehicle. The integration must retain this metadata and use
the same unique identity for actual live collection/membership and queries.
The filter vector allocates only during this cold call.

## Exact numerical source chain

The immutable executable inspected was `work/native/project/disc/SLUS_202.68`,
whose established SHA-256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Address-to-file mapping is `address - 0x100000 + 0x80`. Source words were read
directly from the executable and disassembled; no original instructions were
executed. Retained independent source documentation is
`vehicle-contact48g.md`, `undertrack-reset50ew.md` and the native49N/49X/50BY
components. The retained actual podboxm resource SHA-256 is
`ae5cc68e9548b66e03898b5429615a3f1ca16e47d9eae7d494275c5a4f13fa1d`.

Actual Pod01 PHY has Physical Properties.Weight=1000,
Repulsor.MaxLiftHeight=5 and Repulsor.OrientationSpring=100. Values are parsed
from the passed actual source bytes with the existing strict native numeric
parser; these three decimal values are not substituted in the implementation.

- `214838..21487C` converts Weight by binary32 division through
  `bits(0x4200CCCD)` and applies the mass setter. The cold call checks that this
  mass equals the existing canonical body's mass; it does not overwrite mass.
- `2121C8` establishes vehicle310/320 bounds from freshly constructed main690;
  `21491C` reads half-height324. Existing `prepareType2Bounds50BY` applies the
  recovered quantized podboxm-root bounds with the fresh identity body placement.
- `21492C..214950` passes setup offset `(0,324,0,1)` and authored lift to1B1240.
- `3C9B90..3C9C0C` establishes shared up434280=`(0,1,0,1)` and zero434260.
- `1B1340..1B1390` writes probe130=`body1E0+setupOffset`.
- `1B1394..1B145C` writes
  probe140=`(probe130-2*setupOffset)-(5*lift)*up434280`.
- `1B1468` calls11C3F0 with count1, pair at130, mode1 and linked body scene.
  `11C4B8..11C4D8` initializes pair-resource identity;113070/107140 converts
  endpoints; `11C508..11C52C` sets root0x8000, no tree, scene and pair count1.
- `1B1480..1B1540` writes
  localOffset1C0=`(probe130-2*setupOffset)-lift*up434280`.
  This is deliberately different from the longer lower140 endpoint.
- OrientationSpring is parsed at214904, loaded into f15 at214928, and passed
  at21494C. `1B12AC..1B12CC` produces
  `abs(f32(bits4200CCCD * f32(OrientationSpring * bodyMass)))` into204.
- `1B0FA8..1B0FBC`, `1B10EC/1B10F4` establish208=`bits3F1C61A9`
  and20C=`bits40A00000` (5). These are constructor constants, not PHY keys.

All arithmetic stages use separately rounded binary32 operations. No algebraic
cancellation of the mass conversion or endpoint operations is performed.
The actual root's decoded half-height324 is3; with center1E0=(0,0,0), the source
results are130=(0,3,0),140=(0,-28,0),1C0=(0,-8,0). The implementation consumes
the canonical center rather than assuming its value or using current position.

## Fresh histories, filter and callback

Constructor1B0EE0 supplies query-enabled100=1, zero active104/pending10C/
detached11C/hit flags/current exclusion and zero prior point180/normal190.
`1B106C..1B1078` specifically initializes current normal170 from up434280.
This up-vector is an authenticated unqueried constructor value, not a fabricated
surface result. Fresh preparation resets flags and seeds150; live query hits
must supply their actual geometric normal through the existing native query.

Generic constructor11B190 establishes categoryA4=0, acceptMaskB8=FFFFFFFF,
both identity lists empty with mode1, and no callbacks. Setup1B11B0 applies
11B570 mode7, then124160 mode0 and124170 with global4342A0. The probe therefore
has lowF8=6 and an inclusive category list containing exactly FLOOR. Original
bits6/7 ofF8 remain explicit preserved input. At1B1478/147C the terrain callback
is bound and query owner9C becomes the probe itself.

Actual Pod construction212060 passes a1=1 through1B0EE0/11B190 into141300.
`141340..1413A8` therefore establishes owner31 low nibble3 (bits0/1 set,
bits2/3 clear), preserving the high nibble. `Bindings.flags31` supplies only
that preserved nibble; the cold fixture applies the source-established low bits.

Source3C81A0..3C8220 initializes sentinel3E02C0=(0,0,0,1), copied to query40/50
at11B444..11B45C. These source-defined sentinel words are installed directly.

The callback's category identities are source-proved by their registry keys:

- `3E0718`: PHYSICSBODY, literal388CD8, assignment3C83C0;
  callback1B0EB0..1B0EC8 copies matching target owner9C into exclusion12C
- `3E0720`: STATICOBJECT, literal388CE8, assignment3C8448;
  callback1B0E50..1B0E90 appends matching targets to a list capped at16
- `4342A0`: FLOOR, literal3ABA30, lookup3C9C18/store3C9C24;
  used by the query filter, not the callback
- `4342B0`: TUBE, literal3ABA40, lookup3C9C50/store3C9C5C;
  callback1B0E94..1B0EAC sets forceAngleTest108

Numeric registry bit positions are application-runtime outputs. `Bindings`
requires distinct nonzero one-bit mappings for these four names, plus a fresh
native probe identity, CollisionObject/group type tags, explicit owner flags31,
and preservedF8 bits. Names and behavior are recovered; arbitrary historical
numeric values and guest addresses are not claimed. The integration must use
the same mapping as its actual track/candidate owners. It must not copy an
unrelated query's entire filter or infer PHYSICSBODY from another category name.

## Membership and query-dispatch boundary

Live1B1800 calls11FF90, including120230 spatial refresh, before querying. The
new owner therefore needs its own real collection object and membership; reusing
690's membership or querying a nearest-zone guess does not close this boundary.

Actual Pod initialization212084..212094 installs component1E0=`&vehicle488`
(count pointer) and component1E4=`&vehicle48C` (candidate array). At1B1984 the
source reads component1E4. A nonzero array pointer takes1B1990..1B199C:
122660 receives count=`*component1E0` and array=`component1E4`. A null array
pointer instead takes1B19A8..1B19B0:121920 receives probe and argument0 for full
registered traversal. Actual initialized Pods use the limited branch even with
count0. The integration must consume the real vehicle488/48C list rather than
run a second arbitrary spatial candidate selection for this terrain query.

Source219AD4..219C20 constructs vehicle488/48C on the first substep as the
second122350 enumeration: capacity64 and arguments(t0,t1,t2)=(1,0,0).
The first enumeration is vehicle58C/590, capacity64 with(2,1,1), and the combined
vehicle384/388 list serves a distinct detector. Terrain follows the second
list's source order directly, not the combined list or callback's16 slots.
The callback's16-slot collected-target list is not an authorization to truncate
query candidates or spatial enumeration to16.

After traversal1B19B4..1B1A10 accepts hit bit5 only with zero exclusion12C and
applies the original optional-surface precedence before copying the actual
result point/normal. No optional surface120/124 is supplied by this fixture.
