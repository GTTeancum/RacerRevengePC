# Rider dynamic owners, source-only candidate

Four NEW files under files/src/game. No existing-file replacements/preimages.
No build, syntax check, project parser/importer, test or gameplay was run.
Root owns integration. Cold assets/stat files in ../preview remain frozen.

## Concrete character join

original_ui_rider_character50kg directly consumes the frozen authentic
OriginalUiRiderCharacterAssets50KG and shared model_playback50kg API from the
ShortAxis worker. Six stable descriptors use actual MOT headers/MCF looping.
Shared motion/IK lazy records and original_shared_rng50iu belong to the whole
application, never to a per-character seed or snapshot.

Manager sequence for source1D22A0:
1. Load the authentic cold owner and bind it once. Build actual model graph and
   root position/orientation. Supply graph-rebased VUT rows, not local indices
   if the model has a graph prefix.
2. startEntry:155800 entry then154480 with exact source float .001.
3. Attach actual observer28E8A0 and perform parent+A4 callback.
4. captureAfterAttachment:155730. Only now initialize actual optional l_IK and
   r_IK mode0 chains, in order, at this sampled graph pose. Register shared
   playback endpoints/restore rows, solver/context, set enabled204=1.
5. modelAttached publishes the lifecycle loaded flag. This call is a manager
   completion acknowledgement, not proof that steps1..4 ran or render readiness.
6. selected clears the request latch after generic2A78C0. Re-selection does not
   reset ready, idle counter or last idle string in the original.
7. confirm transitions exact _sel with1-second blend and preservePhase=false.
8. advance observes time/duration>0.8 BEFORE shared update. Source1D2810 uses
   .25*old duration, source31-bit RNG float arithmetic, optional second draw,
   excludes repeat idle, and increments counter each eligible observation.
   Once selection is requested, observation sets ready and skips RNG/counter.
9. After advance success perform observer28EA20 and child141ED0. On unloaded
   frames still call children. Render is original child141D70.
10. Actual deselect drains render, disables root, removes observer, detaches and
    destroys model, then modelDestroyed clears only model-backed owner state.

False after a lifecycle decision is fatal to that unpublished frame, not a
retry/rollback recipe: source RNG chronology and UI decisions already happened.
The shared numerical operation retains its own transactional guarantee.
sourceReadiness1D2710 preserves source unloaded=>true but must never be used to
claim a missing destination/model/render implementation is ready.

## Concrete pod rotation join

original_ui_user_rotate50kg reuses detail::rotateAxisGraph49W, the existing
28B840/112B20 native owner, for X,Y,Z in source order. Keeps enabled zero-speed
calls. Uses exact2pi constant, dt*speed then2pi multiplication. Reads local
look/up (28A530), reconstructs1D3B60 t0=1,t1=0 orthonormal pose, copies it through
shared28B3E0, restores saved widget position. Graph scratch makes failure atomic.
Normalization uses native float sqrt; no PS2/VU execution or precision emulator.
It is a numerical leaf, not a fabricated complete User_Rotate widget.

Source dynamic User_Rotate is ctor1D30C0/create1D31D0, not static1D2A00.
- Defaults: speeds0, axis flagsfalse, pausedtrue; actual Rider authors all axis
  flags true, YVelocity.05, StartPausedfalse, dynamic load and CleanFromMemory.
- Config: SLT string, IsLit, Position, LookVector, UpVector, axis flags/speeds.
- Select1D33C0 calls generic2A78C0, loads if absent (or schedules delay250), then
  calls generic2A78C0 again, clears paused, recursively shows via virtual24.
- Load1D3590 instantiates actual model2403A0, loads SLT through+A8, attaches to
  widget at priority2, applies authored position/orientation or retrieves them,
  attaches observer. If both binderleft1/binderright1 exist, creates helper1D3C60
  and initializes1D3D80; repeats binderleft2/binderright2. Helper attach priority
  is0x100A. Parent+A4 follows. Missing pair skips only that actual helper.
- Deselect1D3510 generic deselect, optionally unload if CleanFromMemory and
  loaded, generic deselect again, pausedtrue, virtual1C. Unload1D3800 drains,
  disables, retains local matrix at210, removes observer/detaches/destroys.
- Update1D38B0 supports deferred load via actual elapsed wall clock>delay250;
  constructor delay is0. Then numerical branch, then141ED0 regardless of loaded.
- Render1D3C50 is child141D70.

## Remaining required owners

Actual graph construction + rebased bindings/source palette table, character
root orientation, mode0 solver wiring, shared renderer/materials/lighting and
observer/widget traversal. User_Rotate cold pod asset/material owner and actual
binder quad/effect helper1D3D80/1D3F90 remain required. The helper reads the two
actual named-node world positions and camera basis, not a static painted line.
Do not reuse vehicle-only three-part GL rig by creating empty fake parts.
Do not mutate shared original_ui_material50kg without coordinating parent and
Options worker. No route registration/readiness is supplied here.

## Evidence

../preview/evidence/character.txt, character-update.txt,
character-motion-choice.txt, user-rotate-base.txt, user-rotate-axes.txt,
user-rotate-binder.txt; original executable identity in parent README.
../preview/archive-member-identities.json authenticates extracted Rider members.
Existing code reused: graph_body49w, chariot_scene49u, scene_graph49p,
original_shared_rng50iu. New shared model_playback50kg and character_mode0_ik50kg
are staged separately by their assigned owners.
