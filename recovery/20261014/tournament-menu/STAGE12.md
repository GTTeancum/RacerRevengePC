# Stage12: canonical Tournament outline ScreenTexture reference and demand floor

Incremental afterb7e1c28; root equivalent integratione8d9809 has earlier stages.
No builds/tests/syntax checks/project execution/SLUS execution. AIDamage unchanged.

New build-list source for root:
src/assets/original_ui_screen_material_reference50kg.cpp
Existing canonical table/object/texture-resource translation units also changed.
No build lists edited here.

Exact original evidence in tournament-screen-reference.txt:
1EF884 loads medlgout3AF0A8 into each outline4C0+60*i (13 objects).
2DF0B0 allocates primitive2461E0 atScreenTexture54; vtable3D20C0+0C
points2465B0, which calls299BE0 then stores source primitive flags0C=15.
2DF11C supplies actual ScreenMT3833F0.299BE0 reads actualdirectory36B970,
appends backslash if needed, appends original name, strips suffix if present,
looks up actual382B38 through298C80, stores material00, then retains14D590.
2DF144 and2DF150 separately perform actual material/texture population.
2DF158..17C and2DF8A0 resolve firstpass4C/map1A through actual383400.
1F2508..2530 raises the real outline texture demand10 to10, preserving higher.
Existing stage10 evidence records this latter demand path.

Scope: authored medlgout branch only, outline indices0..12. Each separate
noncopyable owner acquires a real numeric retain even for repeated aliases.
Pins Tournament archive, application template owner, actual material table,
and expected texture table. Actual directory is mandatory; only case-folded
ui\textures with optional final backslash accepted. No guessed directory,
caller-supplied row, fabricated program/resource, or arbitrary source member.

Private table gate requires SAME application template object and APPSTART
owner, positive numeric ref, actual ScreenMT association, no active registry
mutation, and SAME population texture table. Material consumer resolves only
its actual first pass/map and rejects pending/null resources. Resolved PSM
and PST original bytes must match retained authenticated Tournament members.
A private texture method performs the literal finite/nonnegative demand floor;
104A00 is not called (it converts projected size and is not equivalent).
No public mutable texture pointer, demand setter or callback is introduced.
No material/texture-table constructor domain is broadened.

Acquisition proves ONLY material-reference segment, not full2DF0B0. The caller
must perform actual14D370/29E340 on those SAME owners before demand. Existing
missing-PSM and unique-ATM population restrictions remain unchanged.
Each demand call is serial, preflighted before its sole write, reports actual
effects, and neither resets demand nor claims upload/residency completion.
Release calls actual14D500 once, preserving table pins until destruction.
Session must retain references through renderer retirement and enforce its
real profile generation and phase. This leaf deliberately does not invent
a profile stamp or certify session admission.

Remaining integration: authentic source providers/context population joined
to Tournament; missing-PSM owner construction; unique ATM resource/material
population; remaining original ScreenTexture branches;141C30 concrete widget
traversal/video call; full submission/retirement and route/session lifecycle.
The full Tournament update and frontend remain closed.

Files:
src/assets/original_ui_screen_material_reference50kg.h
src/assets/original_ui_screen_material_reference50kg.cpp
src/assets/original_standard_material_table50la.h
src/assets/original_standard_material_table50la.cpp
src/assets/original_standard_material_object50la.h
src/assets/original_standard_material_object50la.cpp
src/assets/original_standard_texture_resource50la.h
src/assets/original_standard_texture_resource50la.cpp
recovery/20261014/tournament-menu/STAGE12.md
recovery/20261014/tournament-menu/tournament-screen-reference.txt
