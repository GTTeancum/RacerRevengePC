# Race prefix disposal: exact remaining owner proof

2026-10-14. Focused static-source/data analysis only. No production edits,
tests, builds, syntax/sanitizer checks, project execution or guest execution.
This supersedes the earlier description of132BD0 as a cache release.

## Outcome

A newly constructed/private Race1A65C0 prefix is not yet proven safely
disposable through full1A6840. Original constructor/launch bytes supply no
constructor-failure unwind path. Full destructor touches real display,
texture-manager, application-owned list and resolver state regardless of
world74. Several corresponding native lifecycle facets are absent. Merely
setting world74=null and clearing a native component reference is insufficient.

Do not add another manager or a callback shell. Existing application Video,
textureLifetimeOwner_, standard material resolver and RiderProfile retain the
right identities for several extensions, but not the actual missing fields.

##1A6840 unconditional operations and actual owners

The function first rejects only a null self, records the deleting argument,
and restores PodRaceEvent table3CEE80. For every nonnull race:

1.1A6868..70 resolves actual globalVideo443A60 and writes its word44=0.
2.1A6874..7C resolves actual renderer442F70 and writes its word2C=0.
3.1A6880..84 writes global3E1C10=0.
4.After conditional raceB8/BC releases,1A68C4 calls1AB4F0.
5.After conditional race-owned resources/world74,1A69C8..D4 resolves SAME
 Application382B80->328 and calls282190 on that real owner.
6.The profile branch may touch SAME Application2DC, then1A6A50 always calls
132BD0 on SAME global resolver3E1A70.
7.Only then1A6A5C calls1413E0, and a positive deleting argument later invokes
2E8B00 on race storage.

These globals are not fields of the race or a CameraSession. Native teardown
must still retain their canonical owners through this reached operation.

### Display/video native equivalents

tests/native/original_ui_main_entry50kg.h::Application retains the genuine
Video via videoOwner_/videoForSession. Its Video struct presently owns GameHost
and authenticated dimensions/material context; it has no sourceVideo44 facet.
OriginalVideoFadeState represents only fade fields3DC..404 and does not supply
44. A fade object cannot be treated as the original Video owner.

Renderer queue state lives in OriginalRendererDispatchFields and the actual
VehicleWindowGl50DS borrowed through CameraSession DisplayInputs. It represents
D8/44/54/60/DC, but not source2C or source40's backend object. In particular
currentQueue44 is renderer44, not Video44. CameraScene rendererCurrentScene0
is another distinct source pointer; it cannot substitute for renderer2C.

No actual native owner/API for global3E1C10 was found in the targeted original
source owners. Data-only immediate-reference scan also finds the same zeroing
sequence in2C7150..68, but this does not prove that global's type, lifetime or
semantic role. Leave it an explicit absent process facet; do not name it from
an inference or equate it to any camera/lease stamp.

###1AB4F0 is a real texture-manager reset

Complete1AB4F0 consists of LUI44, jump2543F0, and delay-slot A0=global443870.
It ignores the incoming race pointer.2543F0 preserves that actual manager,
then unconditionally calls2450A0(globalrenderer442F70).2450A0:
-loads actual renderer40;
-dispatches that object's table24;
-clears rendererD8 bit0 while preserving upper bits.

2543F0 then reads texture-manager13C. If nonnull it calls252EA0(A0=13C,A1=1).
Finally it always writes manager13C=null. No demand-list traversal occurs here.

The SAME Application's textureLifetimeOwner_ is the existing CPU facet of
source443870 constructed253D80 at140030/14004C. Its current class
OriginalStandardTextureLifetime50LA models only the30 registration-list facet
and explicitly excludes the original GPU allocator/residency/upload owner.
It has no manager13C allocation owner or2543F0 method. Even a proven13C=null
would not eliminate2450A0's backend dispatch. The existing244D60 empty-queue
service is a different reached operation, and cannot certify this reset.

Minimal extension target: actual application texture-manager reset, borrowing
the actual renderer/backend and owning/retiring the authentic13C allocation.
It is blocked on that backend/13C lifecycle, not on a missing generic callback.

### Application328 is an authentic owning list

17C794 initially writes Application328=null.17CDBC/C0 allocates8 bytes,
17CDCC invokes281FD0.281FD0 sets real head00=0/count04=0 and returns SAME self.
17CDD4 publishes that owner into SAME Application328.17CDD8 calls282270,
which returns self;17CDE8's delay slot republishes that SAME owner before the
next allocation. The subsequent0x60 Entity is Application330 at17CE0C, not
an overwrite of328. Preserve this delay-slot ordering.

Complete282190 drains the owning list from head00:
-save the current node; publish node.next04 as owner.head00 BEFORE item cleanup;
-if node.item00 exists, dispatch its table0+8 deleting destructor withA1=1;
-free the actual list node via2E8B00;
-decrement SAME owner.count04;
-repeat until head00 is null.
It does not retire the list owner itself. It can be a genuine no-op only if
that actual owner has a proven null head. A substitute empty vector proves
nothing about the original owned items/count.

No canonical native Application328/281FD0 owner exists in the targeted game,
platform and host owners. Needed native API is the concrete application-owned
list with its actual typed item lifetimes, source head/count and drain282190.
Do not label ordinary asset vectors or the Entity children list as this owner.

###132BD0 restores resolver scratch slots; it does not release cache

Complete132BD0 visits EIGHT0x18-byte slot records at resolver88,A0,B8,D0,E8,
100,118,130 inclusive. In each, load64-bit status+8. If status!=-2, continue
without altering that record. For status==-2, after the actual lazy assertion
state/check on slot.pointer00, call100120(A0=0x40,A1=0x6000), store its pointer
in slot00, set status+8=-1 on success or-2 on allocation failure, and write
slot.size10=0xFFFFA000 (source signed word-0x6000). This is an allocation/reset
operation, not geometry-cache eviction or blanket resolver destruction.

The application standardMaterialDomain owns the SAME resolver3E1A70 via
OriginalStandardTextureResolver50LA. Its current native file/cache/geometry
association owner does not represent these source88..130 allocation records,
their status sentinels, assertion3E1BC0/gp8288, or100120 backing lifetime.
No-op admission would require the real owner to prove every status!=-2. It
cannot be derived from prepared(), empty geometry associations or source count.

This corrects earlier report's seven-slot count and its 'cache release' wording.
The earlier proposed destructive resource-only close must not implement that
incorrect operation.

## Other conditional dependencies still matter

RaceB8/BC slot10, C0 heap backing,19C/1A0 table0+8, global443848->400C table8,
100/104 detach141A70 plus slot10, and world74 slot10 all have explicit source
branches. They are bypassed only by actual reached null values, not by omitted
native fields. Source1A65C0 initializes19C/1A0/100/104/74, but DOES NOT store
B8,BC,C0. Slot30 first observed writes B8 at1A6F54 and BC at1A7084. Thus a
completed constructor alone does not establish their destructor gates. No
allocator-zero admission was proven; native value initialization must not be
reported as source constructor stores.

The source profile branch at1A69D8..1A6A48 tests race1B0 lowbits==2 and bit7
to decide17FD40(A0=Application2DC,A1=0); otherwise it reads SAME profile subtype
11258 and, when subtype==3, copies race1AC into profile10F9C. Existing
RiderProfile owns subtype11258 but not the full17FD40 lifetime or10F9C facet.
Those dependencies remain private authentic owner work, not public setters.

## Constructor-failure/unwind evidence

1A65C0..1A6830 calls only141300, then performs finite scalar/loop stores and
the real Application2DC writes. It has no allocator, constructor-status check,
destructor call, or failure-return branch. Its conditional branches are finite
initialization loops. It returns SAME storage. No partial-prefix unwinder is
present in this complete constructor.

Data-only aligned direct-JAL scan of retained text100000..330000 found exactly
two direct calls to1A65C0:17C16C and17D678, and no direct JAL to1A6840. The
destructor is reached through its table. This scan is corroboration, not a
claim to resolve every indirect caller or original exception runtime.

The explicit17C route branches around construction when2E8D90 returns null,
then publishes the returned value into Application28C and immediately loads
its table8/slot30 without another null test. It has no graceful race allocation
failure path, constructor unwind or cleanup of a partly initialized race.
Neither the recovered source nor current native owner justifies calling
1413E0 alone and naming it the original derived-prefix destructor.

## Minimal root-owned connected packet and gate

1.Pre-admit exact launch dependencies and stable native backing BEFORE the
counted141300 construction or foreign7E4/7E8 writes. Preserve SAME application
identity and actual launch ordering in rr-race-entity-prefix-next.md.
2.Extend the SAME existing Video/renderer/texture-manager/resolver owners only
through their actual race cleanup consumer; recover absent Application328 and
3E1C10 facets. Implement authenticated bounded absence cuts where the source
truly permits them, retaining unconditional backend work.
3.Admit only a reached race initialization/disposal phase that establishes B8/
BC/C0 and every live conditional owner, or separately recover a source-proven
private-prefix disposal cut. Current evidence establishes neither complete cut.
4.Only then create the counted PodRaceEvent owner, publish Application28C,
complete reached slot30/static-world work and source append, and use the proven
typed tail-first child retirement before full1A6840. Preserve terminal ownership
on a partial native failure rather than manufacture successful cleanup.

The next genuine static-world parent cannot be connected safely by another
unused race wrapper. The exact blocking seam is now the actual unconditional
display/texture/backend reset, Application328 list, resolver slot owner and
reached race initialization phase. Production edits remain gated.

## Retained provenance

ELF SHA256:c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Slices in portal-traversal-owner-map/render-entry114d00:
-1A6840..1A6AA0:00a54449daef070217ecd133b6730ccd6ce0e66fb8b37e47a59fc1df5c86bb5a
-1AB4F0..1AB720:42a1eb0caa3d9b4fd3f4993dfff040d59161b1c372eed0e842313f73eec3c02f
-2543F0..254520:58b7c50e75a9e0d55ce194004e57abfa5ee8d34185f8b9e5374ed3e5ca01325c
-2450A0..245150:77dfb3844e06b412be6c88e7020ce1fde1dcc0090467c6dfa27f29cdc06ea468
-281FD0..282020:b0a6212441568a731bdc8bd6e8f79d3fb8ce0694a634279fef9b5c305a2fd56f
-282190..282320:72111f9f68649a2843382a5278fd2e2b72440367e1ab3026045497dae9888342
-132BD0..132D00:fe188c71baaba612ab94b6fde42cf3f4ae4abe5121cbb8e85428ff1eeab6ceda
-17C6C0..17D2B0:69d84a8f67a77b19cd1c36c649c145c4053df4fe92a6042aa61acf7e5f7403d3

