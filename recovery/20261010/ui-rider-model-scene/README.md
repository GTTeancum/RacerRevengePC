# Rider concrete model scene ownership

New original_ui_rider_model_scene50kg pair, based on active ad9a02a/e956d4e.
No existing replacements/preimages. No builds, syntax checks, tests, project
parsers/importers or gameplay were run. Parent owns integration/validation.

## Actual graph and motion joins

A separate source model root owns PSG nodes in original source order. All PSG
locals/names/parents remain intact, including geometry-empty IK/binder nodes.
Root position/look/up is authored, using the existing native1D3B60 equivalent
of source289EC0(a3=1,t0=0,t1=0). No fabricated chassis/engine rig is introduced.
Node source-index table and identity palette remain separate from VUT row order.
Character VUT scene-node references are rebased+1 for the actual model root.

Source2893A0 constructs local identity, zero world and dirty80. PSG source-load
flagsA4 match the established native model_geometry49O/GC graph owner. Character
adds source1D23DC root08. UI widget entity attachment does not add another scene
matrix or multiply source placement twice.

Character beginLoad builds source graph and calls exact entry start/.001 sample.
It does NOT complete observer registration. Real scene manager attaches the
observer and executes the source parent callback, then calls finishAttachment.
This captures155730 saved matrices, finds optional l_IK/r_IK by the original
case-insensitive sibling-before-children traversal28C590, initializes mode0 at
that sampled pose using the genuine shared libm owner, installs real callback,
and enables IK204. No source endpoint is skipped because a solver is missing.
Source19c's cross-named legs are left untouched.

Each dynamic widget keeps a stable scene owner. modelDestroyedAfterDetach is
called only after actual render drain/disable/observer removal/detach/destruction.
It clears the model graph/playback but preserves the character widget's idle
count, ready bit and last idle choice. reload reconstructs the actual graph and
entry playback without replacing that persistent widget chronology. The
attachment boundary must run again. Cold authenticated assets may stay pinned.

Updates directly call existing native character playback/mode0 or pod axis
rotation. Pod selected/deselected changes paused state. Actual CleanFromMemory
routing, deferred delay250, children and observer callback ordering belong to
the widget manager, not an implicit scene no-op. The manager can access actual
graphs for source capture/restore of the selected pod transform.

## Palette and binder joins

projectPalette directly uses projectModelPalette49S with actual model root,
source node table and original identity palette. Caller supplies the real
modelFlags191 selected-for-render state. Source48D semantic surfaces remain
available with local positions/normals, bone IDs, UVs and material identity.
No diagnostic neutral mesh or projectRigidVertices49S output is presented as
original VU geometry or shading. World palette already includes root pose.

Pod scene locates both optional binderleftN/binderrightN pairs, drives original
rotation, and projects actual world quads using source camera column. A missing
pair is an absent original helper, not missing invented geometry. Unique binder
animation/assets now live in their separate integrated shared owners.

## Render boundary still open

This is concrete CPU scene ownership, not destination admission. The existing
native UI draw adapter admits ScreenObject/ScreenTexture/MainParticle only.
Real Rider mesh DiffuseCompressed and binder Diffuse require their actual
lighting, normal/quantization, camera projection, clipping/packet/depth and GPU
material admissions. Never rename them to ScreenObject, invent white textures,
or reuse the three-part race adapter with empty fake models. Authentic render
registration/observer effects, UI widget callback completion and page camera/
lighting orchestration must be joined before Rider can be published.

Evidence: evidence/model-constructor.txt, character-model-constructor.txt,
scene-node-constructor.txt and ../preview/evidence/character.txt. Existing
original source-aware native reuse: scene_graph49p, model_palette49s,
model_playback50kg, character_mode0_ik50kg, original_ui_rider_character50kg,
original_ui_user_rotate50kg and original_ui_rider_binder50kg.
