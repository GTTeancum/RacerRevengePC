# Round4 direct-root static hierarchy review

2026-10-14. Source review only. Reviewed the delivered ROUND4.patch and
ROUND4-REPORT.md under local-codex-round4/packet/handoff/local-codex-lifetime,
the existing scene owner, retained original field evidence, and the direct-root
numeric API committed by root as f416e75. No production edits, patch application,
tests, builds, syntax/sanitizer checks, project execution, or guest execution.

## Assessment

The bounded fresh A2=0 hierarchy patch matches the retained original evidence.
No blocking defect was found within its stated single-root, parent-first fresh
hierarchy admission. This conclusion is a source-review result, not compiled or
executed validation. It does not establish the static cold caller, warm cache,
world membership, bounds publication, renderer, or full world-list retirement.

## Identity and allocation

-Existing construct2403A0 explicitly selects private RootMode::separate.
  New constructDirectRoot2403A0 selects direct. Direct admission requires
  sourceRowIndices[0]==modelIndex and rejects every later model/duplicate alias.
-prepareDirectRoot26E934 stores the genuinely resolved filename without the
  separate-root constructor, extra reference, self-attachment, or model191
  bit1. This matches the skipped26E94C..26E98C block for sourceA2=0.
-The shared fresh loader uses sourceRowObject(0). In direct mode that is the
  actual model_, while rows_[0] remains empty. It does not reconstruct row0.
  The backing loop allocates and constructScene2893A0 constructs only rows1..N-1.
  N=1 therefore has one model and zero extra SceneObjects; N>1 has model plus
  N-1. This matches28CB0C's row0=A0 store and28CB30..48's later-row construction.
-The actual28CB68/88 name/local and140/150/194 source stores apply to model
  row0 directly. Overwriting the earlier resolved name38 with PSG row0's name
  is the original reached store, not an identity error. Direct nodeCount1A0 is
  written on that same model; the separate-mode26ED38 copy remains separate.

## Graph and row table

-Direct graph admission requires the actual model to be a fresh SceneGraph root.
  Source row0 parent=-1 is skipped; later source-parent indexes resolve through
  sourceRowObject, so parent0 links a real child under model_, never under a
  second synthetic root.180/184/188 and previous18C are kept in the canonical
  graph/object facets; no Entity links are created for PSG rows.
-allocateSourceRowTable26EE9C copies actual sourceRowObject addresses. Thus
  table[0], sourceRow(0), and model() resolve to the same object in direct mode.
  Later pointers address the owned SceneObjects. Lookup and name handling use
  the same accessor. No unique_ptr owns model_ as an alias.
-The bounded decoder continues requiring parent-first single-root hierarchy:
  later source parent=-1 and forward-parent cases reject before mutation.
  The original loader's broader forest allowance is not silently claimed.

## Existing A2=1 behavior

Its public construction/loading APIs select and require separate mode. The
existing distinct row admission, separate root allocation/name/bit1/attachment,
source rows, source parent links, model-count copy and return boundary26ED38
remain. For separate mode sourceRowObject(row) is exactly rows_[row].get(),
so table publication, uppercase names and object lookup retain their original
values. The new one-way started guard applies only to direct loads. Existing
separate cleanup object count remains model plusN source SceneObjects.

The internal factoring changes common implementation text, but no separate
path source stores or hierarchy links were identified as changed. Regression
behavior has not been executed and remains root's validation responsibility.

## Alias retirement and failure

Normal scene preflight counts/checks model once, skips the exact source-row alias,
then counts each separately allocated source SceneObject. Recursive graph
retirement still starts at model and follows actual children/siblings; lookup
resolves their actual owner addresses. A direct hierarchy hasN total constructed
objects, notN+1. Row0 receives no second reference decrement/destructor.

The early private close path still visits only rows_ allocations plus model_;
detached constructed prefixes can be retired without double-visiting row0.
directRowsStarted_ prevents reconstructing counted objects after a mutation-
stage failure. Pre-mutation decode/name/backing failures may be retried while
the owner is otherwise private. Geometry/material/packet/observer publication
still blocks the private early close, so a loaded static caller must provide
real staged resource cleanup and cannot relabel this method as a full destructor.

### Concrete broader retirement blocker

Existing preflightModelReference142590 requires c.next14==null. Existing
releaseModelReference142590 repairs the previous sibling's next14 and parent's
firstChild18, but has no next sibling previous10 repair. This is the existing
constrained racer tail/single-child ownership seam. A populated world3C4 static
Entity list cannot be treated as fully admitted merely because row0 graph
retirement is correct. The next real static owner must establish the original
parent's supported disposal order or add the source-proven full domain release
operation. Do not remove this admission check without implementing the required
actual sibling/link/reference ownership. Graph parent180 is separate from Entity
parent1C; a world Entity parent must not be inserted into the static PSG graph.

## Connection to computeDirectRootModelBounds50KG

The patch provides the exact alias/source rows required by the independent
numeric branch. The real cold owner must, after actual source placement:

1.Refresh canonical graph model world100 using the actual lazy source operation.
2.Build context from that world plus model140/150 and graph model190. Build
  source rows in actual1B8 order, including row0 and source parent indexes.
  Row0's context and row box/flag values must match exactly; later parents must
  reference earlier rows. Never invent a ready rootWorld or copy a renderer AABB.
3.Invoke computeDirectRootModelBounds50KG. It copies the SAME root world for
  row0, seeds root-own04 box, performs the original repeated row0 projection,
  then processes remaining rows. It never multiplies placed root local twice.
4.Publish160/170/198 and represented graph cache results through the concrete
  owning method. The current scene header still has no canonical initial-bounds
  producer/publication operation, so a const_cast or arbitrary public ready flag
  is not a valid join. Later28E230 world-min/max projection and19C publication
  are still separate actual ownership operations.

The unchanged reset_model_bounds50ba helper rejects this root-own04/row0
domain. The current packet distance producer also requires a different paired,
multi-node view-dependent cut. Neither is a substitute for initial static bounds.

## Next connected cold-owner packet

Recommend assigning a real source-loaded StandardStaticModel cold owner in
src/game/original_standard_static_model50kg.{h,cpp}, plus the necessary explicit
scene-owner bounds/placement/world-parent methods and root-owned world caller.
It must perform the actual reached sequence rather than create an unused type:

-Retain the actual World48F parameter entry, authenticated PSG/member identity,
  SAME application resolver/provider3E1A70, real generation/count/type/lock
  ownership, and canonical direct graph. Use the new direct-root sequence.
-Use actual lookupModel132820; fresh-only must explicitly reject a warm
  geometry association. A repeated name cannot create a parallel unregistered
  geometry. Full repeated-entry support needs the real warm hierarchy clone,
  geometry retain and material clone branch, with creator1C lifetime preserved.
-Acquire OriginalStandardMaterialReferences50LA via26E640 and actual36B970
  path owner, load quantization240570, construct genuine240110 geometry with
  this SAME model creator, publish1B4 and real1326A0 association, then allocate
  1B8 and actual OriginalRacerModelPackets50LA. SourceA3 is authored metadata144
  UseDynamicLighting(default0), not the racer's fixed-true overload.
-Perform actual placement, produce/publish initial canonical bounds, project
 28E230 world box, and construct19C under SAME expanded world4C0 storage using
  First mode and overlap0. Parent Entity=model+8 under actual world3C4 mask0.
  Concrete model resources must reach the actual selected draw consumer.
-Provide ordered packet/material/geometry/cache/table/scene/observer disposal
  and the broader world Entity-list retirement described above. Wreck Second,
  flat UI zone0, and racer-only registries retain their separate domains.

A useful bounded next implementation is the cold static-owner transaction plus
its verified initial placement/bounds producer, with explicit unresolved/warm
admission failures. Visibility publication must wait for a real instantiated
caller and same-world storage; no arbitrary callback/readiness shell is needed.

Evidence: /workspace/shared/rr-static-resource-owner-next.md and retained static
byte slices in /workspace/shared/portal-traversal-owner-map/render-entry114d00.
Original ELF SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

## Follow-on retirement proof closed, 21:18 UTC

The original parent/disposal-order relationship requested after this review is
now proven from retained bytes. This resolves the earlier missing disposal-order
evidence, while native world/race ownership integration is still absent.

### Actual race owns the StaticModels parents

Race1AAD D8..E8 loads race74 and calls1B48A0(A0=world,A1=actual race).
1B48C0 saves A1 inS0.1B49C8..1B4A08 iterates world3C4 in ascending zone order
and141960 appends each real StaticModels Entity to that SAME race, mask2.
The static models themselves already attach to their respective StaticModels
parent at27AE38, in the actual successful entry-registration order.
Thus the hierarchy is race Entity -> per-zone StaticModels Entity -> model+8
Entities; it is not a new world-root shortcut. Model PSG graph180 remains
parentless and owns its own direct-root descendants separately.

###142590/1424E0 is exact sibling-tail-first disposal

142590 loads firstChild18 intoS1. If it has next14,1425CC..DC calls1424E0 on
that next sibling.1424E0 recursively processes later siblings first. Before
dispatching a child's actual slot10,142530..40 clears its previous sibling's
next14 (if previous exists) and its own next14.142554..64 does the same for
the originally requested sibling after the recursive suffix has retired.
After its child's suffix,1425E0..F0 similarly clears the first child's previous
next14 if present and its own next14, then dispatches that first child's slot10.
This is reverse actual append order, not an invented reverse resource vector.
The first-child head remains real until the concrete child's142590 operation
repairs it. Model source reference104970 is released only after these links.

Full142590 also supports arbitrary self unlink after its children:
-142604..14: if previous10 exists, previous.next14=self.next14.
-142618..28: if next14 exists, next.previous10=self.previous10.
-14262C..4C: if parent1C exists and parent.firstChild18==self,
  parent.firstChild18=self.next14.
-142654 invokes104970; only its actual zero result dispatches destruction.
No arbitrary clearing of previous10/parent1C, mask reset, or extra reference
is part of these stores.

### Concrete resource/world lifetime order

The actual race uses this child-first generic destruction. Its StaticModels
parents retire in reverse zone append order; within each parent, models retire
reverse model append order. Model240450/26E460/2896D0 cleanup, including19C,
therefore runs while SAME world's4C0/search methods and application resources
are alive. Standard's geometry creator is the earlier cache-miss instance;
later warm instances normally retire before that earlier creator in this order.

Only AFTER those model children does StaticModels zero-reference destruction
27B650 run.27B698..6E8 traverses wrapper34 in ascending source-index order,
calling each actual wrapper destructor(table4+8,A1=1) and nulling that pointer;
27B6F0 frees wrapper-array backing, then1413E0 retires the parent's Entity.
Wrapper destructors do not release aliased model8 a second time.

Only later race destructor1A6840 releases world74. Derived world1B2920 calls
26FD80, whose26FE7C..90 frees/clears world3C4 POINTER ARRAY backing; it does
not destroy another set of live StaticModels objects.26FF08..1C subsequently
frees/clears4C0 pointer-array backing. Native world/lifecycle teardown must
preserve these owner lifetimes; clearing world arrays before race child cleanup
would make observer/model retirement invalid.

### Exact canonical native API extension

The ordinary source tail-first parent operation can preserve current model
preflight's next14==null: introduce a concrete actual race/StaticModels parent
retirement owner which preflights the FULL real sibling chain, references,
generation, attachment lock, and concrete model cleanup. It must reproduce the
1424E0 suffix ordering and source next14 stores before invoking each actual
model owner release. Each currently retiring model is then genuinely a tail.
Do not forge a detached parent, rebuild a compact model registry, or skip
unsupported real children such as physics/flare owners.

For separate early removal of a non-tail static model, add explicit named
full-domain model preflight/release methods in original_racer_model_scene50la.h,
retaining the existing racer methods. Require genuine same-domain predecessor,
successor and parent-head coherence, and perform all three source repair cases
above before the actual reference release. This is a new proven operation,
not deleting the old next14 admission check. Source model parent firstChild18
and masks remain the actual parent's fields.

Existing OriginalWreckComponentDomain50LA is capped at64 and the scene owner's
joinDomain uses a fixed64-entry scratch plus at most four peers. A many-model
world cannot be claimed supported by those racer-domain APIs. The actual static
population owner must provide bounded canonical membership at its authenticated
source count (and genuine parents/ancestors), or reject capacity explicitly.
It may reuse the native component records, but must not invent a parallel
compact identity domain or extend the constant without reviewing all consumers.

Retained static slices and SHA256:
-142590..142670:f86d42e9ddcb07f6213583366fdac8b1efc26b08a90c26ebbaaeec805c3ffe4c
-1424E0..142590:2ee0f8c5889695f78f2d443f4e28d5f475801e59eaba713b6e0dec8ad572665b
-27B650..27B740:b4dad0b8119f6b593dd5f59be83b5db7de7b2cb32fc568feae4790f01634d326
-26FD80..26FF20:507fb9731df743b25f4f6545ace994ddce4201d9e8ef6edc6fd0f2c4e91333e7
-26FF20..270430:00dcab759d71bf6e102ad7ef9b836f59908163bc1529f687bc0aa07b37c6eb76
-1B48C0..1B4960:a0f2996eee3ce660afd02e6e932ecf6e385c68333b1c15155f4c1ce20f54d238
-1B4960..1B4A50:04eaf6937b1fccbcad8f8d324b39b38fc68141581773703fb72884fc53ab2dc0
-1AADC0..1AAE10:775bd7fcca9f13f33dff4d2d13bf0643042fe86f63b99956680604f41ede285a

These are data-only extraction/provenance records. No production code changed
and no tests/builds/checks or guest/project execution occurred.

## Concrete typed API handoff for the lifetime session

These names describe proposed real owner APIs, not existing fabricated types
or approved production edits. An arbitrary generic unlink is outside this next
packet. The proven parent tail-first route is sufficient for ordered teardown.

### Owners and missing inputs

-Existing OriginalApplicationEntities50LA owns application10, application26C,
  count423F60, lock423F58 and registry383408. Borrow these SAME genuine fields;
  do not create a second application's counters/type domain.
-Missing actual Race1A65C0 Entity owner must expose the race's genuine Entity
  component, real generation and child-list lifetime. Existing camera/race
  frame value structs do not supply this ownership. No such canonical race
  Entity owner was found in the current original_* source owners. The upstream
  authentic launch/race Entity join must provide it before1B48A0 publication.
-Proposed OriginalStaticModelsParent50KG owns each real27B610 Entity plus its
  source30/34 wrapper count/storage. Proposed OriginalStandardStaticModel50KG
  owns each type1 wrapper record, direct model/graph, actual material reference
  array, geometry/cache binding and packet owner, with a stable wrapper8 alias.
  Geometry creator1C and all model/Entity addresses remain stable until explicit
  retirement. Wrappers stay alive after models retire until27B650's later loop.
-Existing actual model scene/resource/packet/texture/material owners are reused;
  the new original_static_model_visibility owner must own19C and real SAME-world
  links. The concrete static model close calls it while world storage is alive.
-Missing actual race/world population domain must hold authenticated counts and
  canonical real owner addresses. A bounded dynamic borrow over those actual
  records is admissible; a new compact synthetic identity registry is not.
  The existing64-entry racer component domain remains unchanged.

### Required public operations and state transitions

1.OriginalStaticModelsParent50KG::construct27B610(applicationEntities,
  worldGeneration,actualZoneSource) constructs one genuine parent Entity and
  owns original ordered wrapper storage. It registers its actual canonical
  component in the real population domain. Source type/dispatch must identify
  StaticModels27B610, not the unrelated racer27E130 or an external ready flag.
2.OriginalStandardStaticModel50KG::load27ABC0(parent,worldSource,
  actualApplicationResources) constructs/acquires/places the genuine model
  through the staged existing APIs. Its owner has a concrete initial bounds
  producer and actual19C lifetime, and returns failure for unknown/warm source
  branches until they are supported. It does not expose caller-set modelReady.
3.OriginalStandardStaticModel50KG::appendToStaticParent27AE38(parent,
  actualPopulationDomain) joins its private canonical model Entity to that
  parent's child list with141960 mask0. An explicit friend/typed owner method
  can access the actual embedded Entity without publishing a mutable generic
  field setter. Parent list, ancestry masks, lock and actual generation must
  be checked against the real domain. Multiple siblings must be admitted by
  genuine source-count storage, not the existing four-peer racer shortcut.
4.OriginalStaticModelsParent50KG::appendToActualRace1B49F0(actualRaceOwner,
  actualPopulationDomain) is invoked by the authentic1B48A0 caller in ascending
  zone order. It appends the SAME parent Entity to the SAME race mask2. This
  operation remains blocked until that actual race Entity owner exists; no
  borrowed camera snapshot or newly fabricated race root substitutes for it.
5.The actual race lifetime owner invokes a concrete closeChildren142590 before
  its1A6840/world74 disposal. It resolves children from the actual canonical
  typed population, preflights their coherent previous/next/head links and
  generation/lock/ref state, and follows the real1424E0 sibling suffix order.
  Each reached StaticModels parent calls closeModelChildren142590 using its
  actual firstChild18 chain and concrete StandardStaticModel owners. Before
  each typed dispatch, reproduce source previous.next14=null and self.next14=
  null at142530..40/142554..64 or1425E0..F0 as applicable, with original head
  lifetime intact. Then call that actual model's tail-preflight and staged close.
  Required source reference is still1 until its real142654/104970 operation.
6.OriginalStandardStaticModel50KG::closeTail142590 performs actual19C removal,
  model reference release, packet/material/geometry/cache/table cleanup and
  scene destruction in their source boundaries. The current tail admission is
  retained. Concrete resources must be preflighted for the reached disposal
  cut, including warm creator ordering. A terminal partial failure retains the
  remaining real ownership; it is not hidden by an atomic-success claim.
7.Only after firstChild18 is genuinely empty does
  OriginalStaticModelsParent50KG::closeWrappers27B650 destroy wrappers in source
  ascending order, clear each34 entry, free its backing and retire the parent's
  actual reference/base Entity. It never calls model8 twice. Race child-head
  repair uses the parent's real142590 release cut.
8.After actual race children, its genuine1A6840 phase disposes world74. The
  world owner frees3C4/4C0 backing at the proven26FD80 boundaries. Existing
  lighting/draw lease exclusions and all observer storage generations remain
  effective during the prior model disposal; no early world-array reset.

The full race child list includes owners beyond StaticModels:1B48A0 appends
4BC,3A8,404,3EC,3F4,sky3F8,3F0,3FC,3CC and subsequent source children too.
A static-specific lifetime implementation must not claim generic race close by
silently omitting these real siblings. Either the authentic race owner already
owns their concrete dispatches, or publication rejects unsupported scope.
Within a static parent, optional flare and PhysicsObject entries likewise need
their actual typed close or explicit construction admission failure. A generic
callback supplied by the caller is not recovered dispatch identity.

This packet identifies a real source-ordered teardown design compatible with
the current tail model release; implementation can be delegated once the
actual race/population owners and source scope are assigned. No production
code was added for these proposed operations.
