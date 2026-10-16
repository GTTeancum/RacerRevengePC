# Tournament medstar canonical ATM owner plan

Source-only recovery, 2026-10-14. No production edits, builds, tests, syntax
checks, imports, sanitizers, project execution, SLUS execution, guest execution,
or emulation. Original bytes were read and decoded as static fields only.
All PCs below are hexadecimal.

## Concrete result

`24DC30` is the ATM candidate's call to the application file/cache resolver
`132820`, not the ATM decoder entry point. The fresh animated texture resource
is the distinct **AnimTextureMap** object: allocate0x28, construct102070,
load102230, then publish1326A0. It owns count20 and an ordered frame-pointer
array24. Each frame is an actual separately constructed PST/TGA texture object,
not one of nine synthetic rows in383400.

The current canonical texture resource only admits PST/TGA. Its immutable UI
frame decoder and the existing medstar frame bank do not create this aggregate,
its actual cache association, canonical per-frame lifetime registrations, or
pass-owned TextureAnimationData. The current canonical material animation
class admits only unique engine vertex leaves and rejects texture animation.
It cannot be reused as if medstar had a vertex animation.

Important UI distinction: the source ScreenTexture draw supplies a null
owning-object key. Source29B1D0 therefore chooses the pass's real prototype10,
even when TextureAnimation is `unique`. Do not invent a racer/model identity
or a29B050 clone registration for this UI route.

## Original resolution and cache/publication

`24DA90` preserves the caller inputs and uses the last dot found by1169D0;
source candidates are PST, IPU, TGA, then ATM. Their132820 callsites are24DAF8,
24DB60,24DBC8,24DC30. The same actual application3E1A70 is used at every step.
An input-vector miss is not proof that the earlier candidates are absent.
Use the same sealed mounted provider and its explicit absence results.

The ATM spelling is rewritten/appended at24DBE0..24DC10. A present result
sets the ATM branch marker at24DC44. Warm object output is checked first at
24DCC4..24DCF0; conditional slot0C acquisition at24DCE4 must remain separate
from native backing/shared ownership. Fresh path:

-24DD00/04 allocates0x28.
-24DD10 invokes102070 on the real allocation.
-24DD18..28 invokes102230 with the resolved filename and SAME open file handle.
-24DD38 publishes that SAME object via1326A0.
-24DD48 closes the file through131040.
-24DD50/54 returns the aggregate object, not its first frame.

The loader does not branch on102230's returned value before the cache store.
Do not claim original malformed-input transactional parity from this path.
A bounded native loader can reject unsupported/bad structure before starting
resource construction, and must expose actual partial construction effects if
failure occurs after registered frame owners exist. No fake cache success or
post-failure retry that conceals those effects.

## Exact aggregate owner and frames

102070 calls1049C0, installs table3CC750 at8, clears20 and24.
The type pointer at3CC750 is387BD8; its name reads `AnimTextureMap`.
Table slots are8=102110 (destructor), C=104960,10=104970,
14=100B80,18=102050 (frame selection),1C=102570 (frame aggregate query).
This table is distinct from the ordinary texture constructor/table.

102230 retains A0=aggregate and A2=file handle. It reads:

1.Revision at10226C;1 is checked at10227C. The diagnostic/error route closes
  the file at1022F4 and dispatches object slot10 at102538.
2.Count directly into aggregate20 at102314.
3.Storage selector into stackB8 at102330.
4.Allocate count*4 and publish aggregate24 at10233C/44.
5.For each frame in authored order,102350/54 allocates0x210 and102364 invokes
  actual24E1B0(A1=1). The frame pointer is stored at10237C before loading it.
6.Selector0 calls24E880(frame,SAME file handle,A2=0) at10239C/3A0. These are
  consecutive complete PST streams, including each frame's own palette and
  mips. Frame objects use the real24E1B0 ->29C6E0 constructor/lifetime path.
7.Nonzero selector is a separate length-prefixed TGA reader path at1023AC..E0,
  followed by the real frame slot24 conversion at1024B0,29FEF0 temporary cleanup
  at1024B8, and source seek at1024DC. It is not padding; leave this branch
  explicitly unsupported in a first medstar PST-only native admission.
8.1024F0..F8 advances by actual count. For positive count,102508..24 copies
  dimensions14/18 from **frame0**, not a decoded host-image guess or largest
  arbitrary frame. Nonpositive count takes object slot10 disposal.

102050 reads aggregate24[index] directly, with source ordinal as input. A
bounded native caller should check selected frame before indexing; it must not
clamp, choose a different frame, or return aggregate as the leaf.

The aggregate is constructed through1049C0, not29C6E0: it does **not** itself
enter the ordinary texture-manager lifetime list. Each child24E1B0 does.
Do not attach a fabricated aggregate A0/A4 lifetime link to that list.
Aggregate20/24 are count/backing;20 is not an ordinary texture name20 string.
Its common references00/demand10/dimensions14/18 can be represented, but a
PST/TGA sourceKind/formatAC/palette/level layout must not be fabricated on it.

## Canonical texture animation ownership

29B890 independently constructs texture, vertex and pixel leaves via29B950,
29BAF0,29BBD0. With any leaf, it constructs a0x20 controller through29AAA0 and
stores it as the material/pass prototype10 at29B920.

29B950 reads `TextureAnimation` (literal3BDDC0) with default `none`.
For a non-none mode, it allocates0x1C TextureAnimationData at29B9A8/AC:

-Leaf0:FPS from animatedMapFramesPerSecond, default1, read29B9F8.
-Leaf4:reverse-direction and looping/reverse/stopped flag state; constructor
  initializes it and sets authored bits, rather than borrowing UI bank state.
-Leaf8:loopOnEndToFrame, default-1, read29BA7C.
-LeafC:initial/current floating frame phase, default0, read29B9DC.
-Leaf10:previous source timestamp initialized-1 at29B9C8.
-Leaf14:actual owning shader pass, stored29B9E0; it is used to obtain that
  pass's real texture token during update.
-Leaf18:unique mode marker.29BA90 compares with `unique` (literal3BDDE0).
  Unique sets18=1 at29BAA8 and does not register a named3833F8 owner.
  Other names invoke the actual named manager at29BAC0; retain that boundary.

29AAA0 stores texture leafC, vertex10, pixel14; sets own unique1C from those
leaves; initializes controller stamp8=-1 and null callback0/context4; reads
animatesWhilePaused(default0) at29AB30.29AB50 clones only genuinely unique
leaves into separate0x20 instance data when a real object registration exists.
Texture unique clone is a0x1C copy at29AB8C..29ABB0. That clone domain remains
necessary for non-UI users, but is not a prerequisite fabricated for medstar.

29AC90 checks same Application230 timestamp first at29AC9C..29ACB0, then actual
Application11C pause state against controller18. Callback, texture update,
vertex update, pixel update occur in source order at29ACDC..29AD44; controller
stamp is stored only at29AD58.29A020 gets the owning pass's actual token via
slot24, resolves it against SAME383400 at29A050, and reads aggregate20 for
frame count at29A10C. It advances using Application230 and its own previous
stamp/phase/FPS/flags. A count inferred from a nine-image bank is not this lookup.

The existing `src/game/original_ui_texture_animation50kg.h` implements useful
bounded numerical phase/loop arithmetic and duplicate-stamp behavior. It can
be an internal numeric leaf, provided its state is owned by the genuine pass
controller and source count comes from the canonical aggregate. Its current
standalone state is not canonical animation publication.

## Actual ScreenTexture update and frame binding

2DF0B0 constructs ScreenTexture54's material-reference object via2461E0;
2DF10C selects actual ScreenMT association;2DF144 calls14D370, then2DF150 calls
29E340 on SAME383400.2DF158..17C follows material row -> first pass -> signed
texture token -> aggregate, and dimensions are read from that resource.

At draw2DF864..2DF878,246760 receives the actual material reference with
A1=A2=A3=0,T0=-1.246760 resolves it and calls24A620 with that null owning-object
key.24A740 calls29B1D0;29B1E0..29B204 returns genuine prototype10 when the key
is null.24A758 then invokes29AC90 on this prototype. A non-null unique object
with no real registration has a different diagnostic/null path; do not extend
UI prototype semantics to hide a missing racer registration.

Frame selection uses29AF90: its29B00C..29B02C prototype route obtains texture
leafC and converts its float phase by source CVT.W.S.24B254 resolves the pass's
actual383400 object,24B26C gets animation frame ordinal,24B278..24B284 invokes
resource slot18, and102050 returns actual frame24[ordinal]. Only then do the
texture-manager2559D0/256340 calls and249610 operate on that leaf resource.
Current prepareColdRepresentativeMap249610 assumes slot18 returns the same
ordinary resource29C6D0, so it explicitly cannot certify this ATM path.

Collector equality also needs the source unique texture exclusion:
249C9C..249CC4 and249CC8..249CF0 reject merging if either pass has a texture
leaf whose18 is nonzero. Existing unique vertex merging must remain unchanged.

## Concrete native implementation packet

1.In `original_standard_texture_resource50la.{h,cpp}`, add an explicit owning
  aggregate representation matching102070/102230. Prefer a private tagged
  resource interface/variant covering common source references/demand/dimensions
  and checked virtual18 semantics; retain the ordinary concrete frame owners.
  Do not expose arbitrary frame/cache/demand setters or force the aggregate
  into ordinary PST format fields. Child construction must accept authenticated
  ATM substream extents internally without fabricating named provider files.
2.In `original_standard_texture_table50la.{h,cpp}` and resolver storage, let
  object04/cache identities own the legitimate ordinary-or-aggregate resource.
  Add ATM candidate only after proven PST/IPU/TGA absence; pin SAME provider,
  member bytes, context and stable file row. Cache one aggregate for the ATM,
  not nine renamed PST rows. Preserve warm104960 versus shared backing pins,
  terminal partial construction and append/revision validation.
3.Reuse `original_ui_atm50kg.{h,cpp}` solely for bounded structural extent
  admission. Validate each stored PST against the canonical frame subset
  (currently1..5 mips, not the splitter's1..7). Medstar currently has9 frames,
  64x64, but all exact frame formats/extents must come from authenticated bytes.
4.In `original_standard_material_animation50la.h` add a distinct genuine
  texture-leaf/controller domain, or factor shared controller ownership while
  preserving the current engine vertex APIs and their stricter model lookup.
  Parse actual29B950 defaults/overrides, retain owning pass, derive count via
  SAME token/aggregate lookup, and support the source-proven null-key UI
  prototype route. Do not require program43/TexGen rows for medstar.
5.In `original_standard_material_object50la.cpp`, admit medstar's authenticated
  texture unique pass through real29B890 construction, with state/parameters/
  map-retain/animation order and existing cleanup. In material table collector
  and map binding, implement unique texture nonmerge and selected real frame
  before leaf249610. Keep residency/upload/accepted renderer drains separate.
6.Join the canonical medstar row/reference to the SAME Application material
  source/context/program/texture provider owners used for Tournament. The
  existing `original_ui_tournament_career_resources50kg.cpp` image frames15..23
  remain immutable CPU image assets; when rendering, authenticate their frame
  ordinal/extents against the selected canonical aggregate child. The existing
  marker sampler is useful preparation, not an owner receipt or lifecycle seal.

## Retirement and failure

Aggregate102110 calls1021A0 first: ascending frame24 entries invoke each actual
slot10 at1021E8, array24 frees at102208, pointer clears102210, count clears102214.
Then102144 clears aggregate's SAME1327D0 cache association,104910 retires base,
and backing frees when required. Each ordinary frame unregisters its genuine
29C7A0 lifetime membership during its own destruction. Aggregate owns frames
until final reference; views/renderer borrowers must retire first.

Pass247030 releases state, parameters and map references before base animation
retirement, as current native material destructor already documents.29ADC0
releases registered instance clones first, then prototype10; texture-leaf data
frees at29AE38 /29AEEC, with vertex/pixel virtual cleanup kept separate.
A controller's owning-pass pointer is borrowed and must not outlive the pass.
The texture token is owned by map references, not by a second hidden animation
retain. Cache clear must compare exact aggregate identity, never retarget a
borrowed association to its first frame.

Partial construction tracks only actually constructed frame owners and their
registered backing. Native safe-prefix cleanup may guard unconstructed pointer
slots; do not claim original1021A0's uninitialized failure-array parity. Keep
terminal source effects visible and perform ordered session cleanup. Complete
ScreenTexture ownership, inherited widget traversal, application phase/update
ordering, GPU residency and renderer retirement remain open broader joins.

## Static evidence identities

Original ELF SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Static words were decoded with an external data-only awk formatter, not run as
guest instructions. Direct original slice SHA256:

-102050..102570: a917f5a15b16820f5b92acbbcc59de3e2429ce7a9e6553844395fdf35153e3ed
-24DA90..24DD58: 83035fa9e302fad098429b959de808b398a1ad08ef4764c45c59787dff3f1ae4
-29B890..29BAE4: 3f38003b92c34a3bd11167f0612e809d02dfb801afc940e9418ef33f78237473
-29AAA0..29AF90: 8200a6ede0d9e2906a805e88f6983d3d9b86223b6353353f058232c896647017
-24A620..24A810: 4de26ae0ce0f2258b30a356339dde05f78d1e5b0087796f31ecb3827d8a8e143

External static field files `/workspace/shared/rr-atm-*-fields.txt` retain the
read ranges used above. These are recovery evidence, not runtime receipts.
