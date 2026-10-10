# Rider original dynamic preview: cold owner and runtime recovery

Source-only staged, UNTESTED. No compiler, syntax check, native parser, importer,
validation binary, graphics, gameplay, guest execution, or emulation was run.
Active repository unchanged by this worker. New candidate files only; no existing
file replacements/preimages. Root owns integration and all execution.

## Candidate cold asset owner

files/src/assets/original_ui_rider_character_assets50kg.{h,cpp} loads one actual
Soultree_Character widget by original widget ID from authenticated Rider assets.
It retains the archive, source identities, authored transforms, original MCF,
PSG hierarchy, VUT binding, six source-named MOTs and semantic PSG render geometry.
It reuses original_ui_pages, readPsgHierarchy49O, readMotionNodeBindings49O,
readMot49O and decodeScene48D. It does not introduce a separate model decoder or
reuse an unrelated race cockpit rig. Source asset/model association is fixed and
reviewable; missing assets reject. The class has no ready-destination flag.

This requires the ShortAxis motion owner extension integrated centrally as
b7b6bcc. Without it actual Rider MOTs reject at rotationType1. All material,
texture, root-transform, model-playback/crossfade, observer and render owners are
still needed. The cold class intentionally exposes those raw authentic owners;
it does not fake animation completion or display a static portrait as a model.

MCF authoring data is NOT a playback clock. MOT header duration is authoritative
(source1525C4 writes resourceD0). For19c, entry duration3, breathe2, idle1/2/3 and
selection2.6666667461395264. Some MCF FPS/FrameCount values differ from these
compiled motions. Character resources use source name suffixes _ent,_sel,
_breathe,_idl1..3, not caller-selected arbitrary animation IDs.

## Source-authoritative model lifecycle

Class constructor1D1F90 installs vtable3D0340. Fields:
- 1D8: dynamic model load result; initially0
- 1DC: selection-animation request latch; initially0
- 1E0: character transition-ready flag; initially0
- 1E8: actual model instance; initially null
- 1EC: idle scheduling counter; initially0
- 1F0: actual currently selected motion resource; initially null
- 194: MCF stem;199: stem_sel;1AD: stem_ent;1C1: last idle name (starts NONE)

Create1D2120 copies exact MCF name, strips extension, builds suffix names, reads
actual Position,LookVector,UpVector. Generic widget setup remains source2A7830.
The authentic roster definitions all supply those transforms. Source annotation
SelectionMotion=1 is not used to select _sel by numeric MCF slot here; the class
explicitly constructs its names. Do not turn comment values into playback state.

Select1D2550: if not loaded, call real load1D22A0 and store result1D8; then shared
widget select2A78C0; clear1DC. Enable1D2650 similarly loads if needed then calls
shared2A79D0. Deselect1D2590 drains pending render state, disables the actual model
root, removes its observer28EA80, detaches/destroys it, clears1E8/1F0/1D8, then
shared widget deselect2A7940. Source readiness1D2710 returns1 when1D8==0, otherwise
1E0. This is an observation of an actually unloaded source object, never a reason
to omit a required native preview owner and advertise ready.

Load1D22A0 creates model1536F0(1,1), then1538B0(MCF,0,1,1), sets model root scene
flag190 bit8, attaches actual root to widget, applies authored Position28AB90 and
orientation289EC0, resolves stem_ent via1523F0, starts155800, performs actual model
update154480 with float bits3A83126F (0.001), attaches observer28E8A0 to source1E4,
calls parent virtualA4, captures original model matrices155730, masks optional
l_ik/r_ik nodes156620, and sets model204=1. No gameplay/chariot IK is needed.
The observer is UI scene visibility/update ownership, not physics.

Confirm1D2690, only if loaded: set1DC=1, clear1E0, resolve stem_sel, call155830
with blend duration1.0 and preservePhase=0. Never skip this in favor of a timer.

Update1D2730, only if loaded:
1. BEFORE advancing model, poll1D27C0: current model34/current motionD0 >0.8f.
   Exact source constant bits3F4CCCCD; equality is false.
2. If true, call motion-choice1D2810 described below.
3. Advance the real model154480(dt,mirror0,mask0), notify scene observer28EA20.
4. Child update141ED0 always follows.
Render1D27B0 simply traverses children141D70; actual model renders in that tree.

Choice1D2810 saves old1E0, computes blendTime=0.25*oldMotionD0, sets1E0=1.
If1DC!=0 it returns, so requested-selection exit readiness is established at
80-percent observation and remains tied to the real model clock.
Otherwise, if1EC>=6 draw process RNG308030, convert via single float operations
int(1+5*(float(random)*2^-31)). Only values1/2 draw a second RNG and choose
int(1+3*(float(random)*2^-31)). If the result is<4 and the name stem_idlN differs
from1C1, store that name, clear1E0 and1EC, and mark a motion change. If new1E0 is1
and old1E0 was0, choose stem_breathe and mark a change. A change resolves the
actual resource then calls155830 with the saved quarter-duration blend. Finally
if1E0!=0 increment1EC. This counter is update-observation based, not an idle-loop
counter. Shared RNG is the application's existing process owner; no private seed.

## Mandatory playback join not supplied by a simple sampler

Shared sampleModelMotionTime50IG handles direct154480 sampling. It does NOT
implement155830 transition capture plus154A40/154FB0 crossfade. New integration
must preserve these real branches:

155830(duration>0) stores old current34 into38 and40;3C=current34+duration;
44=duration;1C=old18, unless old18 is null or already blending, in which case it
captures current node quaternion/position into1FC/200 and clears1C. Then resets
10/34, assigns new18, sets30=1. A zero duration uses155800-style direct reset.

154480 increments actual34 by dt. Once duration is exceeded, looping subtracts
exactly one duration; nonlooping still samples the overshoot frame before setting
stopped10=1 and clearing34/38 at the end. During blend it increments38, calculates
u=((38-40)-dt)/44, clamps0..1 and applies smoothstep (3-2u)*u*u. Hence the first
blend call has weight0. If1C exists it uses154A40 with old time38 and new time34;
otherwise154FB0 blends captured pose into new motion. Model204=1 invokes special
scale contribution156960 and restores designated scales with28F580 after blending.
When38>=3C, clear30/38 only AFTER that frame's blend. Do not use the existing
pair-sampler's shared normalized time as a substitute for these two independent
motion clocks, or silently ignore scale restoration.

Source ranges model-motion-start.txt, model-update.txt retain exact word evidence.
The complete154A40/154FB0/156960 arithmetic is the next narrow missing owner.

## Exact model/render reuse

The19c.psg header contains32 parent-first nodes,3 material slots (19c_1,19c_2,
19c_2), local geometry with zero origin, one LOD, three surfaces with2044,1626,861
vertices and an identity32-node palette. Repeated material names are intentional;
retain their distinct source surface/index ordering. Total4531 vertices exceeds
maxRigidVertices49S=4096 for a single call, but each surface fits. Project per
surface using its original bone associations; do not drop vertices or increase
limits silently. No native scene decode was executed to assert full admission.

Reuse scene_graph49P for original dirty/ready and hierarchy state, model_palette49S
for node-world palettes, and rigid_vertices49S per surface for the existing native
compatibility vertex path. Preserve its explicit diagnostic arithmetic policy;
this is not a new VU implementation. Original normals/material slots must be
carried through the real native diffuse material pipeline. Bind actual19c_1/2
PSMs (DiffuseCompressed) and PST textures; material190 bit8, scene camera, lights,
alpha/depth and registry ordering come from real UI owners. Do not call the
three-component gameplay VehicleDraw adapter with fabricated empty engines.
A reusable single-model UI collector should use the existing semantic mesh,
material registry and GL infrastructure without claiming a whole race rig.

First User_Rotate pod widget is separate: actual model is UI/Geometry/19.psg,
not UI/Anim/19c.psg. Direction changes preserve its scene transform via28A530 then
289EC0. This model must get the original rotating-widget callback owner; the
character motion owner cannot stand in for it. The selected control's fourth
widget is the character whose readiness gates Rider exit.

## Evidence/materialization

Independent read_preview_members.py only uses Python stdlib byte reading,zlib,
struct,hashlib to authenticate original archive/member bytes. Parent explicitly
allowed this data-only source research. It never calls native/project code.
Archive identity/size and complete table checked; every member digest checked
against catalog. Only23 tiny MCFs and Rider19 dependencies were materialized.
archive-member-identities.json records hashes and paths; original assets remain
unchanged. These are source-data observations, not importer/renderer validation.

The stat-text directory is a separate staged bounded custom drawing projection,
with exact source profile stats, six labels, medals, portraits, panel offsets,
and source missing-PSM ScreenMT requirements. It must join real sprite/text/material
owners before Rider is available. No standalone label/portrait placeholder is
an acceptable substitute for this screen.
