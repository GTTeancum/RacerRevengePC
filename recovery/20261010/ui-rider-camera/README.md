# Rider effective camera, source-only

New original_ui_rider_camera50kg pair. No existing replacements/preimages.
No builds/tests/syntax checks/project parsers were run.

Rider vtable3D1110+C4 is1D7A90. Unlike Main/Options effective manager camera,
it creates actual page+4BC and reads rider.dat PageInfo CameraPosition1,
CameraLook1,CameraUp1,CameraFOV1,CameraRoll1. Shared2361C0 uses640x448 and
aspect0x3FAAAAAB. Actual values are position(-4.444833,-3.110917,25.587730),
look(0,0,-1),up(0,1,0),FOV30,roll0. Both Rider players use the same page file.
Entry1E9E30 attaches this actual camera. No substitute manager-default camera.

Reuses camera_view49f for source core matrices: viewC0,projection80,combined100.
Reuses the new shared prepareOriginalUiParticleCameraFromView50KG numeric entry
staged by Options worker for source-screen140/combined250/focal1D8. The old
output type retains its Main name but no Main identity is manufactured.
Dependency: ui-options-scene-reconstruction/src/game/
original_ui_main_particle_camera50kg.{h,cpp}, exported shared numeric entry.

Binder source1D4054..4098 reads C0 column2, so binderCameraColumn is precisely
core.view[2],core.view[6],core.view[10]. Standard native projection/clip-W/depth
admission remains with the shared actual Diffuse renderer worker.

Normal camera/debug gates described by camera_view49f remain caller-owned.
This preparation does not itself register a camera or admit a destination.

## Corrected lighting identity

Earlier research called Rider page530 a camera tint. That was incorrect.
Original UI manager's29C points an array of light records, each0xB0; each record's
38 is the actual light pointer. Source page uses array+E8, therefore the SECOND
light (directional). 146A30 packs its color; page opening copies those bytes to
530 including the packed alpha byte. Page520 captures light-manager426F60+50 ambient.
Source149900 writes ambient50 and all active ambient slots60. Source146C40
writes directional3C/4C, with source metadata/observers retained separately.

Rider UI authoring ambient80 becomes native40,40,40 through146FDC divide2;
directional140,140,130 becomes70,70,65. Locked page colors are ambient0,0,0,64
and directional0,0,0,255. Unlocked restores actual captured values, not raw
unhalved authoring constants. Closing threshold restores the same capture.
The shared lighting/render worker has this correction and owns source light
matrices/normal projection. No replacement light/shader or visual is supplied.
