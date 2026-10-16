# Actual PodRaceEvent Entity prefix and connected static-world join

2026-10-14, source-only bounded proposal. This extends the typed retirement
handoff in rr-static-scene-round4-review.md. No production edits, tests, builds,
checks, project execution or guest execution were performed.

## Result and correction

The source race parent is the real PodRaceEvent constructed at1A65C0, not a
camera structure. Its canonical native Entity owner is currently missing.
However, a relevant SAME application-lifetime17DF80 owner subset DOES exist:
OriginalUiRiderProfileOwner50KG, plus its shared OriginalUiMainProfile50KG.
The earlier report that no17DF80 owner existed was too broad. Its real profile
fields must be extended by identity, rather than copied into a new manager.

The complete static publication path still needs counted lifecycle facets for
that existing owner and the actual race, authentic launch ordering, and a
source-count-sized world population domain. The following is an implementation
plan, not a claim that race ownership or static visibility is implemented.

## Source identity, construction and publication

Base141300 calls1048F0, installs3CD240, sets category28=FF, nulls the real
previous10/next14/firstChild18/parent1C fields, clears source flags29 bits0/2/3
and sets bit1 from sourceA1, zeros mask20/descendants24, and increments SAME
application/process count423F60. Use the canonical component initializer and
counter, not a private counter or synthetic reference.

1A65C0 invokes141300 exactly once, preserving A1=1 from its authentic caller,
and installs table3CEE80 at8. The type record at3AB7A8 points to literal
PodRaceEvent at3AB738. The table identifies slot8=1A6840, slot10=142590,
slot30=1A6AA0, slot40=1A84B0, slot48=1A8500. This requires a distinct concrete
kind, not entity141300 or the searchable racer27E130 kind.

The constructor nulls real owning world74 and relevant100/104 fields, initializes
its source scalar/array state, and at1A6808 writes descendants24=FFFFFFFF.
The inherited firstChild18 remains empty until actual world preparation appends
children. The all-descendants mask is source state, not a fallback filter.

17C158..170 allocates0x1B8 and constructs1A65C0(A1=1).17C174..17C17C publishes
this SAME object in Application28C.17C180..198 calls its actual slot30=1A6AA0.
Only afterward17C19C..1B0 appends this race to Application10 with maskFFFFFFFF.
17D678..6A4 repeats the relationship. Application0 is a sibling, not this race's
ancestor. A native publisher must retain this ordering and alias lifetime.

1AAD D8..E8 calls1B48A0(world=race74,parent=actual race).1B49C8..1B4A08 appends
the already constructed real world3C4 StaticModels parents in ascending zone
order to that SAME race with mask2. Their children are model+8 Entities,
previously registered at27AE38 with mask0. PSG graph180 is a separate graph.

## Reuse the actual existing17DF80 profile subset

Files:
-platform/linux/native/original_ui_rider_profile50kg.h/.cpp
-platform/linux/native/original_ui_main_session50kg.h
-tests/native/original_ui_main_entry50kg.h

RiderProfile's Storage owns actual constructor17DF80 colors and planet79C/7A0,
100C0,1125x, career fields and authenticated authored abilities. It retains the
same Main profile for source34/38. Application already owns one riderProfile
and one Main profile; all pages borrow them. Neither currently owns the
17DF80 counted Entity prefix, actual Application2DC alias, or7E4/7E8 fields.

Original startup17CE38 allocates0x11318;17CE4C constructs17DF80(A1=1), which
invokes141300 at17DFB4 and installs table3CE5A0 at17DFC8.17CE54 publishes the
returned object into Application2DC,17CE58..68 invokes its slot30, and17CE6C..78
appends this actual Entity under Application10 mask2. Embedded1464 construction
23AE90 and1830 construction23AD30 occur at17DFD0/17DFDC; these are missing
lifecycle state, not proof that the current profile subset is a full constructor.

Exact missing scalar facet:
-17E288 sets t0=0;17E290 sets a3=-2.
-17E330..33C performs byte7E4=(byte7E4 & FE), preserving the other seven bits.
-17E348 sets uint32 field7E8=0.
-Race1A66CC..F0 resolves SAME globalApplication382B80->2DC, preserves7E4 upper
 bits, sets bit0=1, and zeros7E8 on that SAME profile owner.
Do not invent names/meanings for the other bits or duplicate the Main mode34.

Proposed concrete facet operations, private to authentic application/race owners:
constructEntityPrefix17DF80(applicationEntities), initializeRaceFields17E330,
and beginPodRace1A66CC. They must belong to the existing RiderProfile/common
profile lifetime, with the application retaining the SAME counted Entity and
Application2DC alias. Implementing only the two scalar writes does not establish
17DF80 construction, slot30, attach or destructor support.

Current host creates RiderProfile only if RR_ORIGINAL_RIDER_IPUM_CACHE is set;
Application constructor returns early when Main is disabled. Direct gameplay
cannot pretend those conditional owners exist. Either authentic startup must
be expanded explicitly for the real gameplay launch, or admission must reject
the missing owner. It must not instantiate a shadow manager on race entry.

## Legitimate host connection and allocation order

The process Application::entities() is the canonical application10/26C,
count423F60, lock423F58 and registry383408 owner. It survives frontend and
serial races. Add actual Application2DC and Application28C publication facets
here/by the authentic application owner; do not use CameraSession identity.

terrain_interactive_drive50du.cpp already borrows this process Application.
At3145 it prepares APPSTART and the canonical material domain; at3683 onward
it constructs actual counted racer model owners through prepareEarly. A real
race cold owner must be admitted/constructed BEFORE these counted allocations,
with stable backing retained across the frame session. The later5275
CameraSession ActualOwners setup can borrow this SAME owner; it is too late to
be its constructor. This corrects the earlier tentative5275 constructor point.

The earlier Rig.install path can be prepared while the frontend is still active.
Therefore do not unconditionally construct a source race at3145 just because
resources are prewarmed. Connect to the existing authentic committed Course/
direct-gameplay launch gate, and split resource prewarming from counted race
construction if that gate has not yet occurred. Required source order is:
application prefix -> actual17DF80 manager prefix/publication -> race1A65C0 ->
Application28C publication -> reached source1A6AA0 world/racer work -> race
append toApplication10. Source1A6BC0 allocates world before source1A6C80/88
racer construction. The entire reached1A6AA0 dependency sequence is not yet
implemented; do not infer that every static model precedes every racer merely
from this world-object allocation ordering.

## Capacity and canonical population

OriginalWreckComponentDomain50LA validates count<=64. Model scene joinDomain
uses fixed64 pointer scratch and at most four peers. This supports the existing
racer cut; it cannot represent a real multi-zone static Entity forest.

Add a separate source-count-bounded borrowing API OVER THE SAME canonical
component addresses and actual parents/ancestors, owned by the authentic world/
race population. It is not a compact alternate identity registry. Existing
64-entry caller behavior remains. Authenticate source zone/entry counts,
checked size arithmetic and capacity before mutation; account for the real
race, StaticModels parents, models and any reached other concrete children.
Reserve native unconstructed backing and borrow-array capacity first, preserving
stable addresses, then invoke counted constructors in source allocation order.
Preallocation does not increment423F60; no grouping all model constructors ahead
of parent/race construction, wholesale counter reseeding, or constant-only
increase is justified. Typed tail-first retirement must see the entire actual
child list and refuse unsupported physics/flare or other real child dispatch.

## Exact staged boundary and remaining lifecycle blockers

A bounded prefix can represent1A65C0..1A66CC, stopping BEFORE the foreign
Application2DC writes. It genuinely owns the reached base Entity fields, but
must remain private and explicitly incomplete. It cannot publish static parents,
claim slot30 completion, or call the entire1A6840 destructor on this basis.
After the SAME existing profile owner has its lifecycle/scalar facet, the full
constructor through1A6830 is finite and contains no additional external calls.

Full zero-reference race destructor1A6840 is NOT reducible to releasing world74
and1413E0: it unconditionally writes actual global443A60->44,442F70->2C and
3E1C10 at1A6868..84, calls1AB4F0, accesses Application328 via282190 at1A69D0,
and resets actual resolver3E1A70 scratch slots through132BD0 at1A6A50. Later
source profile subtype11258 may trigger real profile writes/17FD40. These owners
and reached phase must be established for a complete race close. Childless/
world-null alone is insufficient preflight. A partial constructor close needs
its own exact reached-prefix unwinding proof; no fabricated successful no-op.

ApplicationEntities' releaseApplication1411AC currently accepts exactly its
two roots and no later children. Authentic manager/race attachment must add
typed ordered detach/retirement and clear owned aliases before process roots
close, preserving source previous/next/head repairs. Follow the source-proven
tail-first plan in rr-static-scene-round4-review.md. world3C4 and4C0 backing
remain alive through model19C cleanup and parent/wrapper retirement.

Smallest connected next implementation packet: extend the SAME RiderProfile
owner with its authentic counted startup/scalar facet; add actual application
2DC/28C publisher/lifecycle; create actual PodRaceEvent cold lifetime at the
committed launch before counted models; provide true world source-count borrow
domain and typed StaticModels parent close; bind static cold resources/bounds
and19C publication through that actual race/world. Each incomplete reached
dependency must stay an explicit boundary. A standalone parent wrapper would
not close this integration path.

## Retained data evidence

ELF SHA256:c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Static field/provenance files are under portal-traversal-owner-map/render-entry114d00:
-1A65C0..1A6840:2e1dea96423795c64b98c3c3161cc5019b4f486f76870f6eda0ec7c9bfeda410
-141300..1413E0:58113e6b1a5ee317ca3c7c19a821a97a31684f26a5e518949605d16d35e3454e
-17CE10..17CE80:560cbaf7d658563ed130415b05956c1d767930295a33c21f0cc0f5a0ef21aa8e
-17DF80..17F970:a4f3ab46820d53d1e876c3c9db8e710350af7fb3661f01e2d94031aa81f575bf
  This slice includes complete ctor through17F8F8 and destructor17F900..17F96C.
-1A6840..1A6AA0:00a54449daef070217ecd133b6730ccd6ce0e66fb8b37e47a59fc1df5c86bb5a

