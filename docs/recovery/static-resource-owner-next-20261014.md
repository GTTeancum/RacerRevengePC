# Static PSX model resource and placement owner: next bounded join

Source-only recovery, 2026-10-14. No production edits, project execution,
build, test, syntax/sanitizer check, SLUS execution, or emulation. All source
PCs/offsets below are hexadecimal. This is an implementation proposal, not a
claim that original static visibility is implemented.

## Result

The original identity gap is closed: StandardStaticModel's wrapper8 is the
actual PSXRenderableObject constructed by2403A0. Its geometry, material,
row-table and packet owners are acquired by240740 ->26E820, using the SAME
application file/cache/material owners used by the existing native ordinary
model resources. There is no separate static geometry identity to invent.

The missing native branch is concrete: Standard passes A2=0 to240740. Its PSG
row0 aliases the model itself; the existing OriginalRacerModelSceneOwner50LA
implements only the distinct-root A2=1 hierarchy. Initial observer bounds also
require a real root-own-box/aliased-row0 bounds branch which existing reset
bounds deliberately rejects. These two branches and the real world parent and
membership publication must be integrated together before adding a static
visibility facade.

## Exact original cold-load route

1.27ABC0 allocates0x210, calls2403A0(A1=1), and stores the returned model into
  wrapper8 at27AC38.27AC40..58 invokes model1A8 table+A8=240740 with filename
  at metadata180, A2=0, and A3=metadata144. The later successful return is an
  embedded Entity address=model+8, but wrapper8 remains the full model address.
2.240740 preserves A3, calls26E820, rejects its zero result, then copies A3 bit0
  into model1B3 bit0. Actual geometry1B4+20 determines2407E0 versus240A10 packet
  construction; the result is published into model1F0 at2407A4 or2407B4.
3.26E820 starts with the actual model Entity operation141C20. It resolves the
  source PSG filename against actual36B860 path, lowercases, replaces/appends
  `.psg` (literal3B9790), stores resolved name38, then132820 on actual3E1A70
  returns the file handle and actual cache association. A missing lookup takes
  the original release/error path; an unresolved native archive input is not
  proof of that path.
4.A2=0 skips26E94C..98C entirely. No extra1B0-sized Soultree root is allocated;
  model191 bit1 is not set by that separate-root block. On a fresh lookup,
  26ED3C..48 calls28CAC0(A0=actual model). On A2=1,26ED1C..28 instead loads the
  previously allocated separate scene root and copies the count into the model.
5.28CAC0 reads actual nodeCount1A0, writes output row0=its A0 at28CB0C, and for
  each row reads name38, localC0, center140, halfExtent150 and radius194 from the
  PSG hierarchy. Row0 is never reconstructed. Rows1..N-1 invoke the actual
  virtual88 child allocation. Authored box04 is set, bit20 is set, disabled
  bit03.04 cleared; parent -1 skips hierarchy attachment. Source parent links
  become180/184/188/18C, not Entity child links.
6.Fresh resource order is actual26E640 material acquisition, virtualAC=240570
  source quantization load, allocate0x28 and construct240110 geometry, publish
  model1B4 at26EE50, load geometry via its source slot18, store geometry1C=
  actual model at26EE74, and1326A0 cache publication at26EE80. The file then
  closes131040.26EE9C allocates the1B8 table and copies the real row addresses.
7.Cache-hit26E9F0 publishes the cached geometry into this model1B4 and retains
  its actual source reference via geometry slot0C. The prototype is geometry1C.
  A2=0 clones the prototype's actual hierarchy through virtual98, with a
  special childless prototype branch copying140/150/194 and source flags into
  this model directly.26EB44..BA8 copies1C0/1C4 and retains each material handle;
  virtualB0=2406E0 copies source parameters. It does not create a separate root.

Fresh-only native admission can explicitly reject an encountered warm cache
association, but cannot hide it by constructing a second unregistered geometry.
Repeated static filenames will require the actual warm branch before claiming
all authored static entries are supported. Existing geometry ownership requires
warm users to retire before its borrowed creator1C; never retarget that pointer.

## Recovered A3 identity: authored UseDynamicLighting

Static metadata reader27A5CC..27A5E0 calls158B30 with literal3BB190=
`UseDynamicLighting`, default0, and output metadata144. The preceding field at
27A5B4..27A5C8 is `UseLighting` (literal3BB178), default1, output metadata140.
Thus Standard240740 A3 is the original UseDynamicLighting choice. It is not
the racer's always-true animation-load choice. Existing packet API already has
the bool sourceA3 overload; the fixed racer diagnostic overload must not be used.
WorldInstance48F retains UseLighting but does not retain UseDynamicLighting.
The actual retained ParameterDocument48F is the input for this field and its
source default; absence must be distinguished from an unknown parameter owner.
Other metadata flags, including the later wrapper read at148, must preserve
their own source meaning and must not be conflated with this identified144.

## Placement and initial observer bounds

Standard27ADCC calls28AB90(model, metadata110 position). It writes model local
translationF0/F4/F8, calls28C4F0 to mark bounds dirty80, and invalidates ready
world/descendants only when previously ready.27ADEC then289EC0(model,
metadata120 look, metadata130 up, A3=1, T0=T1=0) replaces the source local basis
while preserving that local translation, normalizes the recovered basis, marks
dirty80, and performs the same ordered ready invalidation. The imported
WorldInstance48F.matrix explicitly replaces the PSG root transform, but its
existence alone does not prove these canonical graph operations occurred.

28E8A0 obtains bounds via28E230 BEFORE2809C0 observer initialization.28E230's
dirty80 path invokes model virtual90=26F4B0, then ensures actual world100 is
ready.26F4B0 has distinct sphere10 and ordinary-box branches. For ordinary box,
the root own04 box seeds the union; actual1A0/1B8 source rows are subsequently
visited in their original order through28DAF0, including the model alias atrow0.
28DAF0 transforms each authored box with that row's actual world100 and converts
point/directions using the model root-basis transpose. It is not a general
matrix inverse or renderer display AABB.

26F780 publishes160 center.26F7E8 publishes170 half extents;26F7EC..828 forms
their ordered squared length and square root and publishes198.28E230 then
projects the aggregate box through the placed model world100, combining the
absolute transformed axis extents, and returns world minimum/maximum for the
observer. Actual radius198 is therefore distinct from2809C0's initial overlap
radius0.28E8A0 allocates/constructs19C and invokes2809C0 with First mode1, same
world4C0[currentZone], the full actual model identity, radius0 and those bounds.
Only afterward27AE38 parents Entity=model+8 under world3C4[currentZone], mask0.

### Existing native arithmetic is useful but not a drop-in

-scene_graph49p.h provides the actual lazy world refresh, ancestor marking and
  ordered invalidation. No existing setter directly expresses the complete
  static28AB90+289EC0 producer. An authenticated placement matrix may be used
  only after documenting the exact native numerical producer and source order.
-model_bounds49o.{h,cpp} already implements26F4B0/28DAF0 box union math, root
  own04 seed and radius198. Its node loop always computes local*parentWorld.
  Passing row0's placed local as an ordinary descendant would apply placement
  twice. Dropping row0 also changes original union/projection order. Add an
  explicit direct-root row0 alias branch which uses the SAME rootWorld100 for
  that row's projection, after own-root seeding, with descendants retaining
  original source-parent indexes. Do not substitute an identity matrix multiply
  as proof of a cache copy or silently omit the repeated root projection.
-reset_model_bounds50ba.{h,cpp} rejects root04/10 and rejects root in the flat
  input. Keep its existing racer behavior unchanged. A separate direct-root
  static wrapper can use the new numeric branch and canonical graph readiness.
-Bounds publication in original_racer_model_packets50la.h currently occurs
  only inside view-dependent prepareDistanceFlags241280, with multi-node,
  sourceA3=true paired-matrix admission. Static observer construction happens
  earlier and cannot be routed through that unrelated distance producer.

## Concrete implementation packet for root review

No production edit is requested merely to add an unused type. The smallest
complete cold ownership packet should contain these connected changes:

1.src/game/original_racer_model_scene50la.h: retain existing separate-root APIs
  unchanged; add an explicitly named direct-root static construction/loading
  branch. Its binding requires sourceRows[0]=modelIndex, allocates only later
  source rows, and exposes sourceRow(0)=the canonical model. Row-pointer table,
  object lookup, uppercase/name handling, cleanup preflight and source subtree
  disposal must use this alias-aware accessor; row0 must never be owned by a
  second unique_ptr or counted/destroyed twice. This can be isolated behind a
  private construction-mode tag selected by the new static cold caller, with
  default/current racer branch preserved. No observer bypass flag.
2.src/game/model_bounds49o.{h,cpp} plus a new direct-root static bounds wrapper:
  add explicit numeric row0 alias admission/order and produce canonical model
  160/170/198 from actual source boxes and placed canonical graph. Add an owned
  method to publish this verified production before observer attachment; do not
  expose a fabricated-ready or arbitrary caller bounds setter.
3.New src/game/original_standard_static_model50kg.{h,cpp}: the actual wrapper
  cold owner, instantiated by world StaticModels source entries. Bind its
  authenticated PSG to the SAME application OriginalStandardTextureResolver50LA
  and mounted provider, acquire actual OriginalStandardMaterialReferences50LA,
  construct/publish actual OriginalStandardModelGeometry50LA, source table1B8
  and OriginalRacerModelPackets50LA with authored sourceA3, and perform placement
  and initial bounds. A loaded PSG payload alone is not this owner. The existing
  geometry/packet APIs use the same scene-owner type, so the additive mode
  avoids unnecessary generic resource-interface changes.
4.New original_static_model_visibility50kg owner plus the real world cold
  callsite: expand SAME world's canonical storage/lifecycle/object registration
  to include these actual static models. Use attachLiveZoneMembership50EN with
  First, same4C0 zone, real produced bounds and overlap0; store actual19C in
  model ownership; parent its embedded Entity in the actual world3C4 domain.
  Wreck Second, UI flat zone0 and racer-only registries retain their domains.
  Render callbacks consume these exact1B4/1C4/1F0 owners, not imported instance
  names or an observer-only placeholder. Root owns broader callsite assignment.

The scene-mode and bounds branches are source-proven, bounded changes that can
be implemented without changing A2=1 behavior. A complete visibility-only patch
in two new files is still insufficient: the cold static owner and world caller
must actually instantiate and publish it. Full material population, authentic
provider/path36B970 binding, warm hierarchy clone, gameplay240C00 projection
inputs and actual renderer20 drain remain their genuine integration boundaries.

## Retirement and failure ownership

Wrapper destructor27AAD0 frees still-retained metadata10 and wrapper backing;
it does not release aliased model8. Successful27ABC0 already frees metadata10.
The real world Entity parent retires the model. Its zero reference dispatches
241920 ->240450: free actual packets1F0 first,26E460 release each material
reference14D500, inspect/reset surface indexes only on final geometry reference,
geometry104970/240150/26E0A0 then clear real cache1327D0 on final release, free
1B8 and1C4 backing, then2896D0 scene graph and observer280920 release, then
1413E0 Entity destruction. Source disposal does not invent null stores to stale
fields. Native observer memberships must be removed while SAME world generation
and model/zone storage are alive; detach observer ownership before existing
scene preflight rejects19C, and preserve parent/ref/source disposal ordering.
Direct-root cleanup has one model plusN-1 source SceneObjects. A2=1 has one
model plusN separate source SceneObjects. An early native failure must dispose
only genuinely constructed objects and the actually acquired material prefix;
it cannot call the existing all-private close route after resources/publication.

## Retained bytes

ELF SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Static field slices are retained under
/workspace/shared/portal-traversal-owner-map/render-entry114d00 as .bin,
.fields.txt and .range.json. They are byte data, not executable programs.

-240500..240C00:423446536b11657d6e616169eb3f3e5792b03109f9d63077be5fe42ad3a6a22c
-26E820..26EFC0:21ad43f7e33d53c511513f1b640e582d33f6e0b02e00a9a7de1dd27ba04501ff
-28CAC0..28CD60:9051d4e585e83bd6eb38c07e0c230329bd0b8d97e1faccd3462bea35cfeebb67
-26E460..26E820:64945e45d88416e72de1ef5be5105d6d17fd7d7d5bb7e68f023dea9f621653df
-26F4B0..26F780:ec7fc8137db3c231470d24537b35bf0731b6386541002ad03fc7644645908b33
-26F780..26F860:90074f30fbbcd31c965633bf12bd03a102a6a8878049153b9ac1178cba2ecc95
-28E230..28E510:8ccc951e488eb09965f93a4d90504d491b91fef07e91f04f4eeecc627ced0a56
-28E510..28E5D0:57049d35adb3cb7c015b8ae0f5a13c243a22e11e33a00621ccd11111306ae1d1
-28D940..28DEA0:e8faa1c19bded4ef9d8b7fddb5cde3f62734af2a6dae162519d43053ebd3cae3
-28AB90..28AC40:4fa5006f13f7e5aed0cabb338c5a139f466918e3c5ce7f7aa8891ee987ec1073
-289EC0..28A140:3a42cb64dbabd1da317028fa29484c349ae7ca64c93b579920f8c29e5e16044f
-27A3E0..27AAD0:7c1311c145b8beff582e282ceba13eb3b2f06db7127df39cdb6f5f5b98a0e18d

Range endpoints describe retained evidence, not claims that every enclosing
function in every range is complete.26F4B0 ends26F854,28E230 ends28E5C8,
26E820 ends26EEE8; their complete bodies are contained in the paired slices.

## Assigned numeric branch implemented, 21:09 UTC

After root assigned the independent bounds slice, only these production files
were edited: src/game/model_bounds49o.h and src/game/model_bounds49o.cpp.
New computeDirectRootModelBounds50KG accepts the actual source-row order,
including the model alias atrow0. It admits root own04, rejects sphere10,
requires exact root box/flag agreement, and requires all later source parents
to reference earlier rows. The implementation copies the ready rootWorld100
into workspace.world[0], seeds the own-root box, projects row0 through that
same world and root basis, then processes later rows in source order. Existing
computeModelBounds49O retains its original numeric path. Both use the same
ordered union and radius arithmetic and publish output only after success.

API handoff to the separately assigned scene-owner work:

1.Complete the real A2=0 source hierarchy and real source placement first.
2.Refresh the actual canonical model world100 through its graph owner; supply
  that ready world and actual model140/150/190 as the context. Supply original
  1B8 source order, source boxes/flags, and source-parent row indexes. Row0's
  local remains a finite affine source input, but is deliberately not multiplied.
3.Invoke the new direct-root numeric API. Publish returned160/170/198 and
  represented graph cache results through the concrete scene owner's verified
  operation, preserving unrelated source fields and owning one row0 alias.
4.The later28E230 world-box projection and19C/world4C0 membership publication
  remain the actual static owner/caller's next operations. The numeric leaf
  does not supply identity, readiness, resource acquisition, observer19C or a
  fabricated-ready field. No reset_model_bounds50ba behavior was changed.

No build, test, syntax/sanitizer check, project execution, or commit was run.
Root review and centralized integration remain pending.
