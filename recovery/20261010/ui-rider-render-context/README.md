# Rider UI-manager render fog/camera

New original_ui_rider_render_context50kg pair, source-only. No builds, tests,
syntax checks or project parsers. No existing replacements/preimages.

Source1CE7B0 constructs actual UI manager fog: Visibility reader default1,
Haziness default.5, then actual maximum1200/minimum900/base200 passed13E780.
Authenticated Rider ui.dat has misspelled Visibilty=1, so actual Visibility
getter uses default1; Haziness0 and ColorRGB255,255,255 are authored. Result
13E830 end1200/start1000,2451D4 scale255/(1000-1200). Source fog color is white.

13EA30..13EA60 binds fog range to actual active camera,114F20 clamps near/far,
then115350 recomputes source matrices. Candidate keeps original normalized
viewC0 and XY projection lanes, sets exact Z terms, recomputes combined100,
and calls the shared numeric source-screen140/250 owner. This is a distinct
render-boundary camera from cold initial far100000. It does not silently
replace that initial camera or change original input/debug/matrix-skip gates.

No assertion is made that any source model has its fog packet bit enabled.
Renderer owner must preserve actual flag and inherited material FGE1; a no-fog
packet branch must use original constant F=255 rather than silently disabling
material fog. Diffuse/Compressed packet producer owns this branch.

Evidence: ../camera/evidence/manager-fog.txt. UI core camera evidence and exact
Rider identity are retained in ../camera. Existing source-equivalent arithmetic
pattern follows original_ui_main_particle_render_state50kg, with DIFFERENT
actual UI-manager1200/900/200 constants, not Main scene's4000/1333/64.
