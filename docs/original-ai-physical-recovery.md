# Original AI state-5 physical recovery

Source-only implementation, 2026-10-12. This joins the already-completed
16E830 scalar reset to original2255B0 -> AI virtual78/216A30 and then the
16EAA8 ->16EAE8 return. It is not a player, crash/damage, career or checkpoint
reset. No emulator, guest-instruction executor or VU interpreter is introduced.
No build, test, syntax/import check, asset-decoder execution or gameplay was
performed on this candidate.

## Entry and continuation

`driveOriginalAi50IV` already commits navigation/state, packing, repair and
16E830 before publishing `physicalReset2255B0Pending`. The new concrete
`native_original_ai_recovery::resume` consumes that boundary in the same
`PostPlayer` frame. A pending invocation skips the next frame-reader prefix.
A successful reset clears pending and credits one dispatched frame, then the
existing native children/graphics suffix continues. It never enters ordinary
state0 driving just because16E830 changed FE8 to0.

The exact source edge was checked independently from immutable original ELF
raw words:16EAA0 calls2255B0;16EAA8 branches directly to16EAE8. The virtual78
call passes a1=0,a2=1, and vtable3CE258 selects216A30 for AI. Player1A24F0,
1A0240 and camera reset are not traversed.

A dedicated `OriginalAiPostPlayer` domain was added to50EW. Existing player
calls keep their PostPackedBytes50S/vehicle58=0 defaults; no temporary phase
or vehicle-type impersonation is used. All slices retain vehicle3C=0 and
canonical graph/two-body admission.

The banked continuation snapshots the actual C80 before committing Next0 at
225640, then selects geometry using that original anchor. It binds actual
post1ACF70 nodeIndex78 and the real application4C0/4C4 rows. It does not use
Reverb, a nearest zone, a spawn grid or a new waypoint authority. Completed
slices and registry cursors survive a rejection without replaying the scalar
prefix, C80 store, earlier removals or attachments.

## Actual owners

- Selected authenticated collision owners supply the real podboxm, podboxf,
  canonical parameters.shape, body1E0 and authored MaxLiftHeight. There is no
  POD01-only archive fallback in the new recovery assembly.
- Missing694 gets its actual type0 filter and its own registration. Existing
  7A4 endpoint/metadata/filter/query owners get a distinct registration; neither
  is represented by the main/secondary spring memberships.
- Existing348 terrain preparation/resource/membership is borrowed, not
  duplicated. Source self-peer identity exclusions are restored in exact order.
- Real7F0 remains inside `original_gc_gameplay_lighting_candidate::Owner` with
  its checked serial reset lease, existing visibility world and application4C0.
  Draw/camera leases cannot overlap a pending reset.
- Canonical PodCables254+50 belongs to each existing `Cables` instance. Its
  source constructor initializes null; ordinary successful graphics publishes
  the same7F0 identity; reset clears that same field.
- D40 is the existing original-AI engine emitter. D44 is the same selected
  stable PodRaceAudioManager row introduced by the auxiliary-owner packet.
  Only source22C/24C reset effects are applied. Material playback completion
  is not claimed.
- POD02 retains five graphics/model-bound owners and two physical bodies.
  The same optional-three/five logic covers every selected original rig.

Order is 7F0,690,694,7A4,348 removal; initial physical reset;694 preparation;
7F0,690,7A4,348,694 attachment. Ground124CB0 then uses actual C34's temporary
one-zone input, actual SceneInfo-absent/F environment fallback, actual FLOOR
mask/type tag, current object/method projections, and the application-wide
`collisionNormalMode.boundWord()`. No forced flat normal mode is supplied.
Accepted F8/cursor effects publish to their actual owners.

Twenty genuine ground misses produce the original exhaustion warning before
publishing BC4, retain source lift semantics, and do not fabricate a hit or
floor. The existing player's Hit-only admission remains unchanged. Final
physical/model/effect reset retains damage and returns with PostPlayer intact.

## Collision and camera lifetime

Cold fixed storage reserves the two recovery refs before the actual camera's
trailing pair. For N authored collision owners:690=N+1, secondary=N+2,
348=N+3,694=N+4,7A4=N+5; the sole camera has N+6/N+7 in bank0. Opponent active
worlds end at N+5. No fake camera rows or namespace holes are introduced.
Legacy callers without recovery retain their original reference layout.

The same camera query owner now sees the real694 mesh/composed00 and7A4
filter/copies. Its cold stack extent includes the largest actually selected
podboxf mesh. Accepted copied40/copied50/F8 effects write back to those owners;
no second camera owner or camera-reset route is created.

## Admission and remaining boundaries

Runtime activation is the existing fresh ALL8 original-AI/native-visibility
route. The physical continuation also requires the existing original-AI
D40/native-audio owner, real selected Cables and all concrete query bindings.
Missing owners stop at their source boundary rather than supplying stubs.
MA/SA LostAI actions with reset=false are unchanged.

Height-provider guards remain exact:

| Admitted track | terrain330 | water334 | Height guard |
| --- | --- | --- | --- |
| TA | Absent | Absent | Passes |
| GA | Absent | Absent | Passes |
| MA | Absent | Absent | Passes |
| SA | Absent | Absent | Passes |
| GC | Absent | Present | Stops at216B78 |

Fresh1A9EE0 clears terrain and never calls terrain+12C, so TA/MA/SA's authored
nat13.trn does not create a present330 owner on this lifecycle. Environment
waterfile is absent in TA/GA/MA/SA. GC authors gc.wtt and has a genuine334
provider; its unfinished live water-height/wake/time owner is not replaced by
Absent or height0. GC recovery remains incomplete until that real join exists.
GB/MB/RA/RC/SC/TB/TC are metadata-only registry rows without runtime SessionAssets
and receive no support claim.

Authored evidence: TA Environment starts9533, seven fields, terrainfile value
9838 and no waterfile; GA Environment starts21378, nine fields through21818,
neither terrainfile nor waterfile; MA Environment29218, seven fields,
terrainfile value29523 and no waterfile; SA Environment56428,33 fields,
terrainfile value57851 and no waterfile. GC Environment27133, ten fields,
waterfile value27593=`gc.wtt`; authenticated tracks/gc.wtt has2622 bytes and
SHA256 1ff8b0c60869ccde9bfc89b470bbe5591ce4c0eb7d747e55a9df3a3fe751524a.
See existing `gc-height-admission50jb.md`, `native-water-height50jc.md`,
`track_height_cold50jb_fixture.h`, and `track_height_descriptor50jb.h`.

Other existing bounded domains still reject honestly: non-SearchList ground
methods, reached enabled non-type0 FLOOR geometry, unsupported numeric or
portal cases and actual environment fallback T. Existing movement7A4's
limited-list callbackEnabled=false policy is unchanged; this work does not
claim its separate E4=21EE10 callback implementation. Passing height guards
alone is not a claim that every possible reset input is implemented.

## Build integration

The existing `src/game/reset_ground_query50iz.cpp` was added to the interactive
source catalog. All remaining additions are headers or changes to source
already in that catalog. Validation is still source review only; compilation
and gameplay are deliberately left to the centrally controlled root slot.
