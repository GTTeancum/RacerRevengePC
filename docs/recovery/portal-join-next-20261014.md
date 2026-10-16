# Next actual portal scene/render join

Source-only owner review on 2026-10-14. No build, test, syntax check, sanitizer,
gameplay, original executable, guest instruction stream or emulation ran. This
note changes no visibility behavior and adds no runtime owner or receipt.

## Finding

The current native owners do not yet admit a complete original render phase.
Changing `beginDraw` to permit `activeCamera`, calling the camera unconditionally
per swap, or releasing the camera lease mid-walk would each bypass an actual
ownership or scheduling requirement. A lease type by itself would be unused
scaffolding; it is not the smallest complete production visibility join.

The following concrete source/native boundaries explain the missing join.

### 1. Application root and ancestor ownership

`OriginalApplicationEntities50LA` constructs the real base `26C -> 10` edge
(`13FDC0..13FE0C`) and retains it in the process Application, outside races.
Its `initialRootDomain()` contains exactly those two actual addresses. Its
`releaseApplication1411AC()` requires root10 to have no children and outer26C
to have exactly root10. New attachments require corresponding genuine teardown,
not relaxing this check or copying the roots into a compact scene registry.

Retained `140000..140130.fields.txt` establishes these reached source edges:

- `14006C`: application10 -> actual global443870, mask FFFFFFFF, following
  constructor253D80 and the new object's real slot30.
- `140094`: application10 -> actual147ED0 object, mask00000402.
- `140108`: application10 -> application0, maskFFFFFFFF, after the preceding
  application callbacks and1412E0 succeed.

Retained `13FDC0..13FF00.fields.txt` identifies application0's construction,
previously distinct from its attachment: `13FE24` calls `102BE0` with source
A1=0; `13FE2C` publishes its result into application+0; `13FE38` calls
`102DE0(application0,0,8)`. The additional bounded recovery below establishes
its actual table and base render slots and distinguishes this initialization
from race insertion. Neither nearby names nor the constructor alone assign
application0 a race-parent relationship.

The known `1AAD40` race attachment body appends its own scene34 at1AAF6C with
maskFFFFFFFF. Its controller48 attachment1AB1E8 and other admitted controllers
use mask2. The race ancestor and complete preceding/later siblings remain absent from
the native Application forest. The new source recovery below establishes a
direct application10 -> race edge on two actual source routes; application0
is a separate sibling on those routes. Mask2 update
eligibility does not establish mask1000/8 render eligibility. The current
Session's camera binding is not an Entity child record in that root domain.

Required source proof after the additional recovery below: actual race removal,
concrete methods for all reached siblings and race pre-base callbacks, plus
the real scene34 Entity relation. Application0 constructor/table and the
direct application10-to-race insertion/mask are now established below. Required native
join: embed these identities in their real Application/Session owners, extend
actual ordered topology and teardown, then traverse it. Do not attach the
camera directly to application10 merely to make it reachable.

### 2. Renderer method20 already has a bounded concrete provider

The retained141C90/141C94 and27EC60/27EC64 load the same global renderer442F70,
its table pointer at+40, then method+20, with argument zero. The existing
`serviceOriginalRenderer244D60` in `original_camera_start_native.h` is already
used by the genuine native141ED0 child prelude in `original_wreck_frame50la.h`
and by camera Session's `serviceBeforeChild()`.

Its required actual state is `OriginalRendererDispatchFields`: flagsD8,
currentQueue44,count54,count60,pendingCallbackDC. The camera Session borrows
these from its retained `DisplayInputs`; `OriginalRaceCameraDisplayFields`
points to the same rendererDispatch owner. The actual GL renderer and its
native diagnostic are supplied alongside those fields.

Exact incremental reached-child join, once the real walker exists:

1. Apply parent descendants24 and child mask20/flags29 gates using the actual
   linked children, preserving141C30 mask1000 or141D70 mask8 independently.
2. Immediately before that reached child, call
   `serviceOriginalRenderer244D60(actualFields,false,actualGL,diagnostic,v0)`.
3. Only a complete result reaches the child's concrete slot40 or slot48.
4. Read actual child.next14 AFTER the completed callback.

This reuses the actual native service; no caller-supplied Boolean or zeroed
renderer aggregate proves its gate. Nonempty queues and nonnull callbackDC
remain explicit `rendererQueue`/`rendererCallback` boundaries requiring their
actual245030/244F20/callback/DMA owners. A native glFinish does not satisfy them.
The source returned v0=0 means no submission, not failure of the prelude.

The27EAF0 post-block renderer20(0) call must also be preserved after the block
scan. A prelude at the host swap or once per zone does not replace both sites.

### 3. Canonical4C0 registry and real selected queues

`original_gc_gameplay_lighting_candidate::Owner::Storage` owns the live4C0
zones, SearchStorage registries, two methods per zone, racer memberships and
camera identity. Its registrations array has `maximumRacers+1` rows; the
admitted lifecycle objectCount is racerCount+1. This is not the complete set
of world static models, searchable entities or the separate global environment
zone. The immutable GA AuthoredRenderZone rows provide exclusions/flag only.

The source List constructor27E260 installs actual table3D2D30. Its27E880/27E920
methods own selected collection18 and actual shared-registry type2C/2D, then
queue embedded model Entity+8 or searchable containing Entity-30. The current
native lifecycle methods are membership owners, not implementations of these
selected queues. `OriginalRacerModelSceneOwner50LA` already retains real
model type02/flags03/entity8, so a consumer must use those actual records.
Unknown world model owners cannot be represented by copied draw spans or new
wrappers. Each method owns its own queue; preserve order and duplicates, and
preserve the documented multi-block semantics rather than globally clearing or
deduplicating once per frame.

The separate fallback zone443108 and method443A98 need stable independent
identity and their proven construction/teardown; authored ordinal0 is not a
substitute. No all-zone-visible publication or fake stamp is acceptable while
any of these owners are absent.

## Ordered lease design for the eventual real callsite

The current `CameraVisibilityLease` intentionally excludes DrawLease, reset,
frame/membership mutations and traversals. `beginDraw` rejects activeCamera;
`cameraLeaseCurrent` rejects activeLease. Preserve these standalone rules.

When the real application/render dispatch and concrete methods above exist,
add one exclusive `SceneRenderLease` owned by the SAME lighting Owner Storage,
not a second registry. A dedicated canonical camera/render borrow begins only
with no draw/camera/reset lease and no pending reset. It retains session/scene
identity, current renderer selection0, actual application entry index0C and
selected camera254 identity, camera spatial registration revision, publication
serial and current range/matrix publication. `prepareGameplayFogRender()`
changes near/far after a pose publication, so camera pose revision alone cannot
prove the retained projection is current.

Its phase transitions must be driven by completed authentic dispatches:
reached phase3 root40 admission -> reached phase4 root44 preparation -> reached
renderer slot0C/application24 gates -> reached phase5 root48 drawing. This is
per nonnull application254 entry, not a host frame or swap counter. No public
`markComplete(bool)` or unauthenticated phase token should authorize drawing.

At the concrete selected model/entity callback only, grant one ordered nested
DrawLease through a separate `beginSceneDraw(SceneRenderLease&,...)` entry.
Retain standalone `beginDraw`'s overlap rejection. The nested entry checks the
parent's actual address/token/phase and SAME4C0 owner, the exact queued entity
and bound roster slot, current expected graphics sequence, canonical frame and
bindings, roots, membership revision and resetNeedsGraphics. Never authorize a
racer merely because its ordinal is in a visible-zone array. Nested drawing
borrows the canonical bank visit already used by terrain's beginNativeLightDraw;
it does not retain the transient shared arena pointer across another bank visit.

Add `activeRender` exclusion to attachFreshVehicle/updateAfterGraphics/reset/
CameraVisibility/standalone beginDraw/writeCablesVisibilityAfterGraphics and
all corresponding current-lease checks. For nested writes/select/publication,
validate its retained parent instead of deleting current exclusions. Ending
the nested DrawLease clears only its own selected-light publication; its parent
continues guarding4C0, camera state and render fields. End the parent only after
its nested borrow has ended, and fail closed on premature parent destruction.
Owner teardown must clear/end the child before invalidating the parent token.

Source admission/dispatch counters and recursive depth belong to application
lifetime, outside this race-local lighting Storage and camera/racer snapshots.
The startup-zero proof does not authorize a per-race reset, host counter, or
zero native lease generation. Stable A4/A6 fields must live beside the SAME
actual4C0 rows, including the distinct fallback row, with a separately admitted
dynamic allocation/stamp and wrap policy. Current Wreck bindings require stable
field addresses; a lease-local copied tag is not a substitute.

## Smallest useful production sequence

1. Consume the newly proven application10 -> race edge and constructor/render
   tables below. Recover remaining reached concrete methods/removal. Join the corresponding real forest
   and teardown together. This is the first blocking source/ownership work.
2. Implement reached base40/44/48 dispatch on that actual topology using the
   already concrete renderer20 provider. Reject unknown reached child methods.
   Selected camera254 must be selected through26F940 before each actual entry.
3. Join the authentic static/dynamic List method queues and fallback owner to
   canonical4C0, with all actual model/searchable identities represented.
4. Integrate parent render lease and nested DrawLease in those real callbacks,
   replacing terrain's current independent all-racer drawing only where that
   original dispatch is actually complete. Root owns terrain's integration.
5. Only then publish original admission stamps and use them to suppress native
   world/racer/wreck submissions, preserving existing source draw order and
   every feedback barrier. Central validation remains root-owned.

No h/.cpp change is made now: none of the current production callsites can
invoke the proposed render borrow as an authentic completed traversal. An
unused lease type or permissive overlap exception would not close this boundary.

Evidence: docs/recovery/portal-render-entry-20261014.md; retained authenticated
field maps under portal-traversal-owner-map/render-entry114d00; native headers
and implementation named above; tests/native/original_camera_session50kx.h;
tests/native/terrain_interactive_drive50du.cpp beginNativeLightDraw. Existing source slice hashes in the entry note/maps continue to apply. The
additional bounded static source slices below were generated in this pass; no
runtime evidence was generated.

## Additional static source recovery: corrected ancestor relationship

The following findings supersede the earlier assumption that the missing race
edge must descend through application0. No current native forest was mutated.
All newly decoded ranges use the authenticated SLUS file's retained merged
load mapping, file offset = VA -100000 +80. The small decoding scripts read
bytes, unpack instruction fields/words and compute hashes only; they do not
execute any guest instruction or project software.

### Application0's phase methods now proven

102BE0 calls actual base141300, then102BF4..102C00 publishes table3CC900 at
its Entity+8. Table3CC900 slots40/44/48 contain141C30/141CE0/141D70. Thus this
actual application0 object has base phase3/4/5 child walkers; it is not an
unidentified render override. Slot38 is103720 and slot3C is102040, so this
finding does not claim all its methods are base.

102DE0 is a separate initialization operation: it calls141C20, allocates
9940 bytes, constructs2329A0 into app0+A0, calls232AA0 with its incoming A2,
stores incoming A1 at app0+38, gets232D40 into app0+3C, and initializes three
collections at app0+5C/+70/+84 with104410. Its invocation from the Application
constructor does not insert the race.

### Race is directly attached to application10 on two source routes

17C138/17C13C loads the actual Application pointer from global382B80.
17C158..17C170 allocates1B8 bytes and constructs1A65C0 with A1=1.
17C174..17C17C publishes that result into the SAME Application+28C.
17C180..17C198 invokes race28C's actual table+30. Then17C19C..17C1B0 loads
A1=Application28C and A0=Application10 and calls141960 with maskFFFFFFFF.
This exact edge is Application10 -> race28C. Application0 is not an ancestor
on this route; its independently proven140108 attachment makes it a sibling
under application10. This closes the immediate race parent/mask relationship.

17D678..17D6A4 repeats the same race constructor,28C publication,slot30 call
and10->28C append with maskFFFFFFFF using its retained application owner S2.
The global-backed17C route independently establishes the Application identity,
without needing to infer S2 provenance from the narrower17D slice.

The complete race detach/release and its preceding application10 siblings are
still required before extending native Application teardown. This evidence is
not permission to omit those siblings or to seed a compact root forest.

### Actual race phase methods now known

1A65C0 calls141300, then1A65D4..1A65DC publishes table3CEE80 at Entity+8.
That table's slots40/44/48 are1A84B0/141CE0/1A8500, respectively.
Its slot30 is1A6AA0, identifying the preceding initialization callback at the
newly proven attachment sites. These methods are not all base render walkers.

1A84B0's complete body:

1. If race+74 exists, invoke that object's actual table+40.
2. Call141C30 on the race itself.
3. Return its source result.

The race74 callback occurs outside141C30's child eligibility checks. A native
walk cannot start directly at scene34 and silently skip it.

1A8500's complete body through1A86DC:

- Read Application2DC's config byte125C bits5..6. When this extracted value is
  at least2, copy the actual renderer-current-camera0 viewport fields1E0..1F4
  into stack records, create the second rectangle0,0,640,448, then call the
  actual245F90/245EA0 operations on race19C and race1A0 in order. Native one-view
  admission must prove that actual branch input; it cannot erase the branch
  merely because the host currently presents one window.
- Unconditionally read race74, then its+540. When540 exists, read the SAME
  owner's4C0[0].A4 and compare to the unsigned current gp8BE0. Invoke540's
  actual slot48 only on equality. This is a concrete additional real stamp
  consumer; it must not be fed a temporary tag or fabricated visible Boolean.
- If global434160 is nonzero, invoke raceBC's actual slot48.
- If race100 exists and its flags29 bit0 is set, invoke its actual slot48.
- If race104 exists and its flags29 bit0 is set, invoke its actual slot48.
- Finally call141D70 on the race itself.

The pre-base direct100/104 calls test enabled bit0 here; do not silently add
141D70's hidden/mask tests to these distinct sites. Source order and direct
callbacks remain significant even after the complete camera traversal is
joined. The race74 concrete owner/table and each reached pre-base direct draw
owner are now exact next dependencies, rather than a vague app0 ancestor gap.

### New retained hashes

-102BE0..103100: e42fbdb6857b3e4c57a3467eb2bfb346458ce66ee5be0ab2f7e554860a6ca66d
-3CC900..3CCA00: 7f4fea13ba43944966e9358c176d4e7614c1467cf2d67670985e7fea0bc97d39
-1030B0..1043E0: 643b12c9d0bc4d69c525c8fc6e17ccb713ee0cea73d3156dcdec00613548a9c9
-101F00..102BE0: 84e246683ecdc65c8f91a41a7c73a2b083bdbfcfa6a23eaf059af336b62562e4
-17C110..17C1E0: 28dd00b594ba6515a6d3466a6a322a9894bf2173b2fb1031274fd6766430fed2
-17D5C0..17D710: d6b151f88e2f8569b2121eecb270d2158716b52404678277e1e627267d568756
-1A7660..1A7840: 154e2f493bed5bbcb3457e69e34345d62f776f6bc591b100760d8e0589a5966c
-1A65C0..1A66C0: f7f8021401689e2c559986ff0c667f974f4fc0a3f11ce4ae5fad49b3e9726ca1
-3CEE80..3CEF00: d32b722694bc712a88c50ca1c1cc8fb556acd841f42471faed28305d36155169
-1A84B0..1A86E0: 5dec58c33cb25c1775db0d5074fc777c7777ee3f39acc0db46825f908e59be3c

Bounded maps and raw slices are retained beside the existing source maps in
portal-traversal-owner-map/render-entry114d00. Literal reference queries are
syntactic and do not establish computed callers or global reachability.

## Continued owner recovery, saved at stop boundary19:59 UTC

Further work stopped here at the parent's direction. No implementation or test
was added. The following is exact static evidence for continuation in the
requested execution environment.

### race74 concrete world callback

1A6BC0..1A6BDC allocates550 bytes, constructs1B2870 with A1=1, and publishes
the result into race74. Constructor1B2870 delegates26FC90, then installs actual
table3CF110 at+8. Its40/44/48 slots are1B32E0/141CE0/141D70; its destructor
slot8 is1B2920 and generic destruction slot10 is142590.

Complete1B32E0 through its ending return near1B3888 has these dependencies:

- If world540 exists and world4C0[0].A4 equals gp8BE0, call298E60 with shared
  manager382B38 and world544; pass its returned resource to29BFF0.
- If signed world4AC is positive, use actual raceB0/B4 and its own world510
  records in a substantial ordered preparation loop. Reached calls include
  231840,231900,1AF330,28E110, repeated2316D0, and2315E0. No complete native
  adapter for these world records was found in this review.
- Finally call275630(world), including when4AC was nonpositive. The recovered
 275630 prefix immediately returns through its own tail when world4F0 bit0 is
  clear; otherwise it reaches278430/275990 and actual application camera/range
  fields. A zero default must not stand in for a proven current world4F0.

Race destructor1A6840 calls world74's actual slot10 when nonnull and clears74
at1A69C4. World destructor1B2920 cleans its510 and other owned resources then
calls26FD80. The complete recursive142590 destruction contract still matters;
merely invoking the native lighting Owner destructor is not that world cleanup.

### world540 actual identity and callback

1B3B80's setup allocates80 bytes at1B3BAC, constructs23BCA0 with A1=1, and
stores the result in world540 at1B3BC8. Constructor23BCA0 delegates150320 then
installs table3D1EA0. Its source48 is23CA50, not a model2403A0 callback.
Setup invokes its real74 initialization with a retained literal/resource;
world544 is separately supplied by298C80 at1B3C0C. Setup's failure path calls
540.slot10 then clears540 at1B3DD8. It must not be replaced with a permanent
fallback draw object after failure.

An additional actual attachment site is1B4D0C: when containing-owner S1+540 is
nonnull, append that exact child to S1 using141960 with mask8. The caller at
1AB150 invokes1B4CA0 with A0=race74 and A1=race. The subsequently retained1B4CA0 prologue establishes S1=world
and S0=race, closing normal parentage and recursive142590 teardown (details
below). The failure release is not its only lifetime operation.

Complete23CA50 through23CB14:

- If object48 bit1 is set, skip its own rendering and return via the tail.
- Lazily create object50 through23CB20 when absent; write its20=7.
- With object40 null, copy global426F60+50 matrix to object50+C0. Otherwise,
  only object48 bit0 triggers its actual70 matrix publication;23CADC..23CAE4
  writes(flags48 & ~1) | 1, retaining/set bit0 rather than clearing it.
- Invoke246760(object30,object50,0,0,-1), then141D70 on the same object.

Thus this direct draw requires actual lazy packet50 and246760 submission,
not an invented static-model draw span. Source48-bit1 and original admission
stamp gates are distinct. Existing map2 GPU feedback requirements remain.

### raceBC concrete owner

1A7068 allocates78 bytes, constructs2A1160 with A1=1 at1A707C, and stores the
result in raceBC at1A7084. It initializes through2A1260 using the actual retained
source filename buffer. Constructor2A1160 calls141300 then installs3D39B0.
Source48 is2A2FE0; destructor8 is2A11E0 and destruction10 is142590. Race
teardown1A68A4..1A68BC calls BC's actual slot10 when nonnull.

The complete2A2FE0 body through2A3138 has no calls and no externally addressed
stores: it reads actual record30/count34 and indexed subrecords, computes only
stack-local values, and returns1. This is a concrete observation of that bounded
body, not permission to presume other2A1160 methods or its whole lifetime empty.
A faithful typed native implementation would retain its actual record/count
ownership and reject unsupported input instead of manufacturing success for an
unconstructed owner. No such owner was integrated by this pass.

### race100 and race104 distinct construction/lifetime

The race constructor stores null100 and104 at1A67A4/1A67A8. Later1A8A40:

- Allocates8F0 bytes, constructs1BA130 with A1=1, stores100 at1A8AB8, then
  invokes100.slot30 and100.slot1C.
- Allocates18C0 bytes, constructs1BE940 with A1=1, stores104 at1A8B00, calls
  1BEEB0(104,race), then invokes104.slot18.

Both constructors first install shared base table3CF4A0. Their derived stores
must be retained:1BA204 installs3CF420, whose48=1BAF60 jumps directly1B9220;
1BEA2C installs3CF3A0, whose48=1C17A0. Their derived40 methods are1BAE00 and
1C15B0. They are not the separately existing native text HUD overlays constructed
at1A8CC4, and they cannot share a generic HUD-render success Boolean.

Race destruction explicitly handles them separately: when100 exists,141A70
at1A695C detaches it, its slot10 is invoked1A6970, then100 is cleared1A6978.
The104 route repeats this at1A6988/1A699C/1A69A4. This is stronger lifetime
provenance than treating them as borrowed draw descriptors.

The retained1C17A0 prefix gates rendering on actual208, then actual200 selects
substantial derived rendering branches. The complete bodies were subsequently retained (see resumed recovery below).
1BAF60's exact jump identifies1B9220 as the concrete callback, not the nearby
1BAF70 function. Neither callback has been joined.

### Implementation-ready next scope, without a visibility claim

The source graph now identifies actual parent and concrete pre-base owners.
A complete production visibility change still cannot be safely confined to
original_gc_gameplay_lighting_candidate.h/.cpp: the native process forest lacks
actual race74/540/BC/100/104 component construction and the corresponding ordered
callbacks, while current canonical4C0 lacks complete selected method queues.
No permissive lease change was made.

A concrete next implementation packet should own the actual Application10 ->
race28C component construction and142590 teardown together, wire the reached
1A84B0/1A8500 dispatch to real retained world74 and these derived owners, and
use the existing renderer244D60 provider immediately before each reached base
child call. Recover/port1B32E0's current4AC/4F0 gates and actual reached methods;
recover normal540 attachment provenance plus23CB20/246760; recover complete
1B9220/1C17A0. If the admitted native source domain proves a gate skips an
operation, preserve that proven gate instead of inventing an empty callback.
Only after these actual consumers exist can the proposed ordered parent lease
and nested beginSceneDraw be a used production join rather than scaffolding.

### Additional hashes retained for continuation
- 1a6aa0-1a7660: ce022620d453cb4476ce56661fc0966984e4e3144fb7e479fbf185ae5d718840
- 1b2870-1b2a00: a0fbb6fb5c1e4986248924ae2b44053a4af38ed4bd2d59eccc2c81cb1302777f
- 3cf110-3cf190: 2d53f2fcab3a6c3386cbe55443dfb76a8adf13626358b26c0433274978b338b5
- 1b32e0-1b3860: fbd9c1b2c5f9172fffa1b2e632768f723f87987f7d0e40199295a8fcf27fc8f3
- 1b3830-1b38b0: acecf4175d40401d52bab9ece47baff6a8028cd27d55ea7971f97c4844574133
- 275630-275730: 41998341e2519d3e2c5d5f54f14827493c522e6aeadd73a4bb28df303d715a4f
- 1a66c0-1a6840: 4d71e8d5bbcd4caae54b95035299a6e75d1e08ae0a80459cd04e6609553ac227
- 1a6840-1a6aa0: 00a54449daef070217ecd133b6730ccd6ce0e66fb8b37e47a59fc1df5c86bb5a
- 1b2920-1b2ab0: 29545eaee040750cdf604bf4fd50841f7397d8805296e0619bced726a0d404d6
- 1b3b50-1b3e00: f450cdc51b489d445b50d913837b25d5480a5024a7bf16c5fbf24654b4270fe7
- 1b3e00-1b4090: dbb7f31d45e99a3a9c6a82e7afdc39b47250a31604388097685619c4d631b07b
- 1b4090-1b4230: aee37977720ff8d3832f8ccbb4440ce9f311fd8ab11d5336abe41ecbaecc4a4d
- 1b4cc0-1b4d60: 5c49394047bea938f409c6dc56219dcbd3de6f1d5f2597e945316b185bf8bff3
- 1b4810-1b4970: 259529e19c24b388851c3bcd61239bf6baecbc667dbd2a687685ed6269b6f6da
- 23bca0-23bd40: e6a0e18762cfc0c07792114398832027cfa4db78f76a0ae904b6c36847718770
- 3d1ea0-3d1f20: 6316c3dc43bbd6da7721be71cfadb797b86a4410286dda87e04eb77a2e45abe7
- 23ca50-23cb40: 16b6f622df2dd9d6b67839d9328bc425e6ab4f43f1d934792dfd8a6f0538caae
- 2a1160-2a1270: eb2849b705c851f320110c5d0e73cac0befd734cbc41e9a22c5d5fc6e18c86ce
- 3d39b0-3d3a30: ed83e8760ea06450f9a19e26ec29e6f74991a3e813a0089aa6b31819587e853a
- 2a2fe0-2a3150: 04eb000b03bbbff354144e6cfe99cc8114074d072c0289ef4e87b0cbb1aa5a6e
- 1a8a40-1a8bc0: 85e274d853f6e73ae169154aae3cda96d258eeb120c0bc662b9c7ad420fa92cc
- 1ba130-1ba430: ce11e0f63a680fd2af1ce0a45cd40937b6dcf8716bacac9b6a780e7eb300b5ea
- 1be940-1bec50: 5addef852f1c64d7c4c258703e22ab69d8816f9257635ee431e63cd01727f4f1
- 3cf3a0-3cf4a0: 3ebbefa97b4376dcbac4733e0ee55022f5b457356c157216453ed9e71b556021
- 1baf60-1bb170: 9d29a6b824b15f27289774c72b5b41105dce53f46c692f99cdac0639021d8c48
- 1c17a0-1c1900: 82f85c8f9ef6df5671b4c6e8b63fd290a6112a6fb6290f14c7b1dc393b6a53dd
- 26fd80-26ff20: 507fb9731df743b25f4f6545ace994ddce4201d9e8ef6edc6fd0f2c4e91333e7
- 26ff20-270030: 2ba8994495756392b2b8d0b07b1e70d7b61fa159e0ea15329106d19ad8cb968a

All ranges are static data-only retained slices with .bin/.fields.txt/.range.json files in the same external source-map directory. No source callbacks were executed.

## Resumed static recovery: complete concrete preparation and menu bodies

### Finishline normal attachment is closed

1AB150 calls1B4CA0(world74,race). In1B4CA0,1B4CB0 retains incoming
A1 as S0=race and1B4CBC retains incoming A0 as S1=world.1B4CFC
loads actual S1+540 into A1;1B4D0C calls141960 with A0=S1 and mask8.
Thus the actual PSXMesh finishline is a child of its PodScene world, and
generic142590 child destruction supplies its normal lifetime. This supplements
the separately proven setup-failure release. It is not a new parallel draw root.

### PodScene preparation controls real descendant activity

Complete275630..275984 reads world4F0 bit0 at27563C..275644. If clear,
it branches to275974 and returns without invoking base141C30. If set:

-278430 constructs the bounded stack20 color/fog record: clears16 bytes,
 installs table3D2AA0 at18, initializes10=1.0 and14=0.5.
-275990(world,stack20), complete through275B74, uses actual renderer442F70
 current camera0; currentCamera34+4 and linked region nodes30; world350
 indexed records of size352 and their12C..140 values;231150 selects
 overlapping contributions. It writes the normalized blend into the actual
 passed stack record. It is not a host camera-color approximation.
-Application264==1 updates retained world4DC/4E0/4E4/4E8/4EC interpolation
 owners against Application250; the other route uses the current blended
 values directly. Both pack color into actual world3EC through13E830.
-If world3A4[0] is nonzero, world3F8 exists, and currentCamera38+48
 is less than world34C, it obtains selected child index from
 world3A4+8[currentCamera38+48]. For every actual world3F8 child it sets
 flags33 bit1 clear and flags29 bit0 set. A nonnegative selected index then
 sets that child's flags33 bit1 and retains flags29 bit0 set. Both states
 must remain distinct; this is not equivalent to removing a child.
-Finally27596C calls141C30(world). This is phase3 descendant dispatch
 inside the actual world.slot40 path, not phase5 model emission.

The source initializer near2705C0 zeros world3F8,4C0 and4DC..4EC.
270550 is the base table3D2BB0 slot78 initializer invoked by26FCCC.
It sets A1=-2 and A2=1 at270580/270584, so270630..27063C
sets actual4F0 bit0. Fresh-world defaults do not authorize skipping preparation.
276314..276328
sets bit0 after comparing newly parsed record12C against retained world3AC
through278480. Therefore constructor-zero or unconditional skip is not a
faithful loaded-world admission rule. Full loaded-world data publication must
carry this gate and its actual indexed and interpolation owners.

### Pause and result rendering cannot be silent no-ops

Retained derived table names resolve through their actual pointer tables:
world74=PodScene,world540=PSXMesh,raceBC=TunnelTrack,
race100=PodPauseMenu,race104=PodEndRaceMenu.

Pause100.slot1C is1415A0. Its complete retained body clears flags29 bit0
and bit1 before recursive child slot1C calls (excluding hidden-bit3 children).
The exact operand at1415B4 is30070001, ANDI a3,zero,1, producing zero;
1415BC/1415C0 clear bit0 and1415CC/1415D0/1415D4 clear bit1. In contrast,
141630 uses64070001, DADDIU a3,zero,1 at141644, and sets both bits.
Evidence: recovery/20261010/ui-static-membership-reconstruction/evidence/
entity-traversal-141300-141EF0.txt, addresses1415A0..1416BC.
Thus the constructor caller's slot1C leaves Pause disabled for1A8500's bit0
render gate. Genuine open1BE8A0 invokes slot24=141630 to enable it;
close1BE830 invokes slot1C=1415A0 to disable it. The native host pause overlay
cannot replace these callbacks or their Entity lifetime.

Complete1B9220..1B9F5C is substantial shared menu rendering: retained
resource-controller382B20 initialization, geometry/cache preparation,
localized strings, real derived slots78/74,2E0170 and final141D70.
The separate adjacent1B9F60 return1 is not evidence this callback is empty.

Complete1C17A0..1C21A8 initially gates on actual208 then switches actual200.
It uses actual18AC60 localization,1C9B10 text owners, formatting and
retained result fields; its tail invokes1B9220 on the reached launch3 route
and otherwise updates actual30,20C and390. Constructor explicitly zeros
200,204 and20C, and the initial slot18 disables flags29 bit0. Actual208
publication and later result enablement must be recovered before claiming
this owner available.

### Newly retained exact ranges

All are data-only .bin/.fields.txt/.range.json in the external source-map directory.
-1B4CA0..1B4CC0:0e61721c8873af1272f4ec6fab9b115077aef9398130d73008a6b42e09486b4e
-1B9220..1B9F68:92a8765c5072db1ad589c9beb1e6cc4afeec4755607dbd64b918d29f7c7d02e0
-1C17A0..1C21B0:063e4fb25feedab6308da0f0af47b6ab937d0276d93e74125de6acac699d78d0
-1415A0..141620:b95497de2941707a251ec3361fdbe100be26765c59afd5c09d3afef2d268356b
-275630..275990:d08cd2cb319d8efbcfb9aea7af7a8e83dcfd4d1f390612fc2bcf44fed880fad4
-275990..275B80:d2f9b775ec9d7932677c8d37a02e33e1c47adc6c1e70ffed04d96848ce23772b
-278430..278560:fd715354f6aa9f31151d4af3fed3eea3c126bfc353c165cec8fb5f7cb3cb92df
-26FC90..26FD80:fdd7133323686a0f3e49cfcb0234db9570521661872292441e94a72d887af4d1
-270430..270660:d2b8592381681eb572c2acf43e72c2ffcf82912c75877fd466e5a88a4cd29961
-276200..276370:4a26897b64468cb3fc865c644d2aae9be76de7c72396ac4cf64d0b60c9a6a16e
-231360..231A00:8aa983cc078ac328a831e276da37f3c5888c17bb2c829333dc52c043e757165d

### Actual SkyCube namespace and fallback SearchNode parent

BasicScene base table3D2BB0 has40=275630,48=141D70,78=270550.
Its type pointer3BBE58 ->3BBE30 reads BasicScene. Derived PodScene overrides
40 with1B32E0 and calls the retained base preparation.

274AE0 is the actual sky-owner construction callback. If world3A4 exists and
its count0 is nonzero,274B24 allocates count*4 pointers into world3F8.
For each authored name unequal to literal3BA330 (`none`),274B60 allocates
AF0 bytes and274B74 constructs288A50(A1=1). Its table3D2FF0 points through
3BCEB0 ->3BCD88 to SkyCube. Actual load288A90 builds the source filename
and calls1295B0; failure274C78 destroys the object's slot10 then clears
its actual array entry. This array is not canonical4C0 zones or7F0 members.

288B90 is the actual SkyCube slot48. If its7D4 field is nonzero it first
invokes114C80(actual current camera0), then calls12BE50(SkyCube).
Its slot40=12C710; source slots and source model/resource backing must be
retained before reproducing selected sky rendering. No sky substitute was added.

2729E0 constructs the genuine world4BC base container:272A00 allocates
30 bytes,272A14 invokes141300(A1=1),272A1C stores world4BC. The
source then separately allocates actual4C0 and4C4 arrays from world34C.
This parent is not an arbitrary renderer root.

The actual fallback443108 construction is2730D8..273114: allocateB0;
construct27F120(A1=1); store443108; clear fallback48; append fallback
under world4BC using141960(mask0).27F120 installs3D2DD0. Its exact
type pointer3BC3A8 ->3BC340 reads SearchNode;40=27F5D0,
44=141CE0,48=27FE60,8=27F180,10=142590.

273128/273138 register actual method443A98 with fallback using27F420
and27F490. Those complete functions replace prior methods34/38 using
actual slot10 release; set incoming method10=1 or2 respectively; store the
method and invoke its slot0C reference acquisition. No success token can
replace this reference-counted shared method lifetime.

Complete SearchNode.slot48=27FE60..27FEDC contains the original
diagnostic assertion path and returns0. It does not emit models. Actual
selected rendering must invoke the previously recovered queue callback27EC60,
not silently route SearchNode through raw slot48.

### Minimal production join narrowed by these relationships

A complete call path now requires actual BasicScene/PodScene preparation
with retained SkyCube selection/interpolation and descendant activity, genuine
world4BC child lifetime, SearchNode queues/method references and the ordered
camera-selected27EC60 invocation. A proposed lease-only edit in lighting.h/.cpp
still has no complete truthful caller. The production code packet should
therefore add an owning source-scene adapter beside actual application/race
forest ownership, then join camera Session.selected queue dispatch to those
owners and real model callbacks. It cannot replace world4BC with canonical
registrations or treat SearchNode.slot48 as a render implementation.

Further exact data-only ranges:
-3D2BB0..3D2C40:22937756e5e4092607aff35b802dca8703b907b4236d6aa910ce1ef73bb96ad1
-270A10..270E90:e075ba0a54ada88d16ad8d47582bc422d7606b0f103d1534814ebff78982b9e0
-274A40..274D20:7250e70a6694d27e6c4b083f35909a3ef423ed9888986580e697b1651b13a2ea
-288A50..288AB0:739243e06b39ea0a60a14d2cd790d608285b6ab2dd59ca49811dc06cb303ee16
-288A90..288D10:3f13bdca537914ae672e07b32b72da4b7ace9e879bfba75511a3162235fe570e
-2729D0..272A50:c90e0c36ca6d261f7af0f45d88bfbadcc56515d05182d2a3a3c326f52ce62c8a
-273080..273150:035b0e0a54c914e76ed43b87014329b110dde5b94a2474953f7633681a437e4d
-27F120..27F510:ad4477f836cb345964f58b2fd51b87f49ef6b0be16a7c773d1aa71ef8e8774ef
-27FE60..280060:730ab8a2779a0fd05e804ead80e991fadd567529a0e48ff2b4b5dbae8c298dfc

### Result104 state and one activation route closed

The complete shared setter1CA8E0..1CA938 compares incoming mode A1
with actual200. When changed, it saves prior200 into204, publishes A1
into200, and sets208=1. If incoming A2 requests fade, it sets390=0.5
for mode4 or0.25 otherwise. Its tail at1CA930 transfers to1AF770
with the genuine Application382B80->race28C->B0 racer manager.

The actual race callback at1AB940..1AB94C calls this setter with
A0=race104,mode1,A2=0. Another source route1BDB18..1BDB50 loads
Application28C->104, invokes actual slot20, then invokes this setter
with the same owner,mode3,A2=0.104 table3CF3A0 slot20=141560,
whose complete141560..1415A0 sets flags29 bits0 and1 then tail-calls
actual slot2C. Hence the mode3 route has concrete activation as well as
state publication; initial construction's slot18 disabling is not permanent.
Table3CF3A0 slot2C=1414C0. Its complete retained body walks actual
child18/next14 in order and calls each non-hidden child.slot2C. It does not
replace those reached derived callbacks with a generic success return.

Additional exact data-only ranges:
-1AB900..1AB980:c25d316ca7cacd7c7cd12ac0d41c6f3a306cf97546e6a8483dda219f7ed9c571
-1BDB10..1BDB80:d8ae9b11eab224a477019d6d6c533537ca11bafad50cf6680b7928365331ad35
-1CA8E0..1CA950:29799e94e4226196e72f594549024d653aa73ce1b979e0f604000740ed527a87
-141520..141620:12fd622b0dbf16cd7e84fd5c3f2e92f265746b4011f0d21d3a04cc0860e341b8

-1414C0..141520:2950d9840100a3226280bb6af74a739377e8e79612a5ecf54331c39ad6a63c18

## Stable restart: loaded metadata and actual selected-queue publication

### EnvironmentInfo and ZoneInfo are genuine loaded owners

270A58 allocates0x550 bytes;270A68 constructs276950;270A70 publishes
world3A4. Constructor276950 installs table3D2B50 at54C and initializes
count0, names4, zone-index8 to zero. Its exact type is EnvironmentInfo
(3BBDC0 ->3BBDB0), reader slot8=276C00.270A84 calls that reader with
the SAME world34 parameter reader and world34C zone count.

276C00..277218 selects Environment; reads NumberOfSkyCubes(default1);
allocates count*4 name pointers, each filename buffer **0x104=260 bytes**;
reads CubeFileN with1-based suffix and default `none`. Only a failed first
numbered-name read retries unnumbered CubeFile. It allocates zoneCount*4
indices, reads CubeFileForZoneN(default1), performs the original upper-bound
diagnostic, then subtracts1. Input0 thus remains selected index-1. A single
`none` filename is freed and count canonicalized to0. Numeric recovery
immediates elsewhere in this note are hexadecimal; filename buffers described
as `104` mean0x104, not104 decimal bytes.

world350 is a real array of0x160-byte ZoneInfo records, not4C0 entities.
270B98 constructs the array with275C50 and publishes it at270BA0;
each record reader is invoked through its15C table slot8 with(world,
parameterReader,oneBasedZoneOrdinal).275C50 installs3D2B90 at15C;
its exact type is ZoneInfo(3BBE10 ->3BBE00), reader275E30.
275E30..27652C selects ZoneN (or StaticModels fallback), retains static
model count10C, collision count108, search-policy fields110/114/118/11C,
authored exclusions and FogColorRGB/FogVisibility/FogHaziness at12C..140.
It compares those fog values with actual world3AC and sets world4F0 bit0
when changed. Existing World48F.parameters retains these original blocks and
fields, but its typed World48F.zones does not already instantiate ZoneInfo.

The newly integrated original_world_environment50kg owner is the smallest
actual runtime consumer of EnvironmentInfo authoring. Its camera selection
must receive the actual currentCamera38+48 zero-based ordinal, not a racer
membership zone or fabricated visibility admission stamp. It does not itself
construct SkyCube model resources or complete BasicScene phase3 preparation.

### Authentic SearchList publication and QuadTree shared selected queue

Resolver276530 maps literal QuadTree to2, List to1, other strings to0.
273190 reads ZoneInfo118 (static visibility policy). Policy1 allocates
0x50 bytes, constructs27E260(perBlock=0x400=1024) at2731D8, publishes
that actual SearchList owner in world4C8[zone] at2731EC, then installs
the SAME owner in world4C0[zone].method34 via27F420 at273204. This is
a source-grounded method constructor/publication path; a racer-only compact
registry does not reproduce its model contents. Policy2 constructs the actual
VisibilityQuadTree family instead.

The fallback443A98 constructor2AEF70 first delegates27E9D0 and finally
installs3D4580, exact type VisibilityQuadTree(3C02E0 ->3C0230),
slots44=2B00E0 and48=2B00D0. The source2AF150 call with5000 returns through jr ra with a DELAY-SLOT
store of A1 to method8C0 (2AF154=AC8508C0). This publishes the requested
query capacity5000; allocation is deferred to2AF1D0, not completed here.

Complete2B00E0..2B016C invokes its actual slot40, then queries2AF1D0
using actual currentCamera180/188 and requested capacity8C0. The actual
tree root is method94, read at2AF340. The returned physical
candidate buffer is owner898 and count8A0. It calls27EAF0(owner,
buffer898,&count8A0), thereby reusing the original flag/type scan and
selected18 publication without pretending the owner is SearchList.
2B00D0 loads buffer898/count8A0 and tail-calls27ECA0. Thus its phase5
uses the SAME actual selected18 queued entity+48 emitter.2AF1D0 has
separate root traversal, allocation/growth and generation14/flag16 state;
the shared scanner/emitter does not prove that query implemented natively.

### Actual static-model list publication boundary

2710F0 allocates real per-zone world3C4 pointers and constructs27B610
owners (table3D2640, exact type StaticModels). Its slot74=27B740 loads
each owner's actual model entries using world3D4, world34 parameters and
the corresponding ZoneInfo record.2741F8 calls27B930 for the actual
StaticModels owner and SAME world. Complete27B930..27B9CC visits
count30/entries34 in source order and invokes each entry's table4->slot10
with A1=world. The entries therefore are genuine static-model wrappers,
not arbitrary native SceneNode indices relabeled as searchable objects.
The entered wrapper registration callback and its concrete model/searchable
identity remain the next recovery target. No pointer alias or registration
success was manufactured in this pass.

### Exact production integration boundary at pause

No production scene/render lease change was made. Next integration needs an
actual loaded-scene owner retaining ZoneInfo350, SkyCube3F8 and interpolation
4DC..4EC; genuine world4BC/4C0 methods and lifetimes; actual StaticModels
wrapper registrations, physical SearchList/QuadTree backing and selected18
queues; concrete queued model+40/+48 callbacks. Connect this to the existing
real camera/renderer244D60 barrier before changing lease exclusions or using
admission stamps to suppress host world/racer/wreck draws. SearchNode raw
slot48 remains an assertion boundary, never a replacement emit callback.
The EnvironmentInfo owner is useful loaded authoring, but original portal
visibility remains unimplemented. Root owns all later integration and tests.

All continuation ranges below have stable .bin/.fields.txt/.range.json in
the external render-entry114d00 source-map directory, decoded only as data:
-276950..276BC0:055257f5ffbaabafd7da77e95600bd8ad432365df92898871c83818733d6bf5d
-276C00..277240:a36faa9a42917bb197d169f7cb95c573ee95415954a1ce7a84dd7c9e19f2d2c7
-275E30..276540:fdde3315cc6c3c71ef0731882f341efd107d87c2c838658e16a42ff1003f45e0
-276530..2766F0:cc416d9e2b25b4c38820cde6f774384f0c4a50ccd51fbd9a2278227943051044
-273150..273220:ce5fcdf504e6937dbb7a442067318f2d1b99732e99e42ccc190d82df23c4cc9d
-2AEF70..2AF210:1829dcb3d91d94b36a3f3babd9f06ac9d5ff9ddc0084f893341788438cce32f1
-2B00C0..2B0310:22dc4929426dc2f4a2344cd1bdd6bd1fdad10c27b2c0a9946aea7ab7bde99289
-2AF1D0..2AF590:e54a6ccf937ae285fa8a03c034822d1730f71ddd9babf3c725e057b3daf0a63c
-27B610..27B980:ce4e5cbc8d9b2e4779d56bdc957f6f444a2440c2ab4935e3971bf9ee9d528bb8
-27B930..27BB40:0baf38041e345910e9acdaf1b258cbe48796134fbb15f4f176d8e50e64d0431e
-274120..274240:233d64cd1063442cdf4f5666930256603623af253f1722ee59ed03358c1d5ad7

## Bounded resumed proof: StaticModels wrappers to actual queued PSX models

### Entry dispatch and constructor identities closed

27B930 reads actual StaticModels30/34 in order; each entry.table4 slot10
is a concrete wrapper method. Factory27B9D0 constructs table3D26F0 for
type1 (StandardStaticModel;3BB9A8 ->3BB980) and table3D26C0 for type2
(PhysicsObjectStaticModel;3BB950 ->3BB900). Their slot10 targets are
27ABC0 and27B3D0 respectively. A source unknown-type default uses the
Standard wrapper; a native bounded loader may reject unsupported authoring,
but must not silently reinterpret a reached Physics wrapper as Standard.

Complete27ABC0..27AF94 retains A0=wrapper in S2 and A1=actual world
in S1. It rejects wrapper10's `none` filename.27AC1C allocates0x210
bytes;27AC30 constructs2403A0(A1=1);27AC38 publishes wrapper8.
It loads through actual model1A8 table.slotA8 with resolved filename and
actual authored load parameter144, sets source model1B3 bit4 from authored
parameter148 bit0, applies authored position through28AB90 and look/up
through289EC0. These are source placements, not a draw-bound-only matrix.

27AE0C calls28E8A0(A0=actual model,A1=world4C0[world354],A2=1).
27AE20 converts the same model pointer to its embedded Entity at model+8;
27AE38 appends that Entity to real StaticModels parent world3C4[world354]
with mask0. The source mask0 is not permission to attach it to an invented
global draw root. Metadata pointerwrapper10 is freed/cleared at the successful
tail. Wrapper destruction27AAD0 frees retained metadata/wrapper storage,
not the aliased model8; actual model lifetime belongs to the real Entity parent.

PhysicsObjectStaticModel27B3D0 instead constructs1659C0, exact type
PhysicsObject(table3CDF80); uses BOTH actual4C4 collision and4C0 visibility
zones in165C30; activates its source methods and appends under the same
StaticModels parent with mask2 at27B5C8. PhysicsObject40/44/48 are the
real base141C30/141CE0/141D70 child walkers. Its actual graphical child
and collision lifetime must remain separate from the direct Standard route.

### Static observer19C and First registration closed

28E8A0 releases an existing model19C observer through280920 before
allocating0x30 bytes and constructing280890. It publishes the genuine
observer at model19C; derives current model bounds through28E230; and
28E984 calls2809C0(observer,actualModel,actual4C0zone,mode1,zeroRadius,
currentMin,currentMax). Mode1 is the existing native ZoneMethod50CL::First.
The0.f initial overlap radius is source-proven; it is not a claim the current
model radius198 is zero. Subsequent updates remain their own28EA20 path.

The reusable exact native membership operation is
attachLiveZoneMembership50EN(actualLifecycle,actualPortals,member19C,
stableStorage,actualModelRef,actualInitialZone,ZoneMethod50CL::First,0.f,
currentBounds,actualWorkspace). It requires the actual model already present
in that SAME expanded4C0 lifecycle namespace and an empty primary record.
Current compact racerCount+1 registrations do not contain these static models.

### Queued entity identity and reached callbacks closed

2403A0 delegates26E3A0/2893A0, publishes model table3D1F40 at1A8,
and embedded Entity table3D1F48 at model10. Its exact type is
PSXRenderableObject (3B5340 ->3B5200). Model words and embedded Entity
must remain distinct identities:

- SearchList27EAF0's type2C branch receives the actual model, checks
  model31 bit0, then invokes its embedded Entity table40 at model10.
- Queued selected18 identity is that same model+8 Entity.
- Its40=2418F0 subtracts8 and transfers to26EFC0(actualModel).
- Its48=2418E0 subtracts8 and transfers to241280(actualModel).

Complete26EFC0..26F064 consumes actual model03 bit2. The disabled route
clears191 bit0 and returns. Otherwise it invokes model1A8.slotBC=240C00,
publishes that result's low bit in191, runs26F070 only for selected models,
and increments the actual Application278 processed-model field. This is not
the optional host AABB visibility optimization.240C00 and selected distance
must consume the SAME current model graph and actual camera publication.

Complete241280..2418A8 first gates on191 bit0. It uses actual graph/world
cache2897B0, geometry1B4, source rows1B8, materials1C4, packet owner1F0,
lighting14A0A0, actual model clip/LOD/flags and material298E60 path. Its
concrete emission includes246760 per-submesh submissions at241828 or
the source alternate241844 path, then141D70(model+8) at24186C.
The nearby2418B0/2418C0 leaf returns are separate functions, not substitutes
for the complete selected callback. Native CPU/GPU code must reproduce this
resource/packet behavior; retained original command bytes are never executed.

### Minimal implementation proposal and precise current blocker

Proposed new files: src/game/original_static_model_visibility50kg.h/.cpp.
They should own one real per-static-model19C membership with stable links and
entries, bind its source-loaded PSX model graph/resource and the SAME expanded
world4C0 lifetime, derive current28E230 bounds from that owner, register First
with radius0, update via the existing live-membership API and remove before
the actual model/scene teardown. An opaque generation must name that genuine
owner; no caller-writable prepared Boolean or generic callback shell is valid.

That patch is **not yet implementable under new-files-only ownership**:

1. No loaded StandardStaticModel wrapper8 -> canonical PSX model owner is
   published by World48F.instances. Imported draws alone are not this owner.
2. Existing OriginalRacerModelSceneOwner50LA retains exact2403A0 fields,
   but explicitly admits null observer19C, has no observer-publication or
   static-world3C4 append API, and teardown rejects nonnull observers.
3. OriginalStandardModelResource50LA/Geometry50LA are real resource types,
   but existing instantiated1B4/1C4/1F0 owners are racer-bound. No actual
   loaded static wrapper currently supplies those fields, source placement,
   current aggregate160/170/198 or matched graph lifetime.
4. The expanded actual world4C0 static-model Ref/registration namespace and
   real world3C4 parent lifetime remain absent. A new observer cannot allocate
   a parallel compact registry or use a racer/wreck Ref for a static model.
5. OriginalWreckVisibility50LA registers Second and pins wreck assets.
   OriginalUiRiderVisibilityOwner50KG is tied to UI assets and flat zone0.
   Its numeric clip helper may be reused where exact, but neither complete
   owner can be borrowed as static-world visibility.

Therefore no new static visibility files or public generic callbacks were
created. The smallest complete next code packet must additionally own the
source-loaded static PSX model/graph resource and real3C4 entity publication,
plus the approved canonical4C0 expansion. Only then can the proposed new
visibility owner have a truthful production caller. Full renderer246760 and
240C00/LOD admission remain explicit boundaries; original portal visibility
is still unimplemented. Root owns broader file assignment and all tests.

Retained data-only ranges for this exact bounded continuation:
-27B740..27B930:e590e56b339e163b7ce8e1f7c8d6fff52b97df4ec6471acaa876cadc862f74ed
-27ABC0..27AFB0:d6caa92cf161241f529cc49f938a6f4c0f7ffe890b5bea64c66b9b986a39d40f
-27B3D0..27B610:a0ce64e1e8d69cf1b2767b1ccbb0855faa0f4aefb441a28c8c44915f3a0a889d
-27AAD0..27ABC0:105e12ce6eaa53c93da7178ba959b60c647ebe70687028b948890e295e1f8ef5
-2403A0..240510:63be4ea212376a828c846cc2eb49392cd26562143fdb854cd671d0b147247c50
-28E8A0..28EB10:86d35294ab19c4b74479f6d5b7ab0e3540f0d21f0a678a326f25ee8e8f6b716f
-2418E0..241970:62e978a63777e9e1273677df1ceeb27163d08a6732331755fd89a0ccec6bcc0a
-26EFC0..26F150:6d04a94351de9de03b0c70ee0e10f293a21e82fdf5eda53591941442f5e80c54
-241280..2418E0:832384e4e3f7f61821324989c6ddda245cc83b37677de5c57b72f98d86e62d9b
-3D26C0..3D2730:df019d688d17218f233e0d8358b30e219681b0bdfad04f0a628c4917fc82448d
-3D1F40..3D2010:f44578ada00fbd8b5be62bd091dfa6d00df837275318d195d4e2692b81e450e0
