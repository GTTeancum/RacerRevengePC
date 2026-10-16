# Native original audio-options bridge, 2026-10-14

Source-only delta from 79fc223eda2da3d98929c9f3b1253745d17e7c2a. No builds, tests, syntax checks, preparation, extraction, or execution were performed.

The recovered audio controls are groups 2 through 6 of the authored screen 12/page 4 Game Options page, not a separate menu. The existing options.dat controller, shared widget bank, localized text, aperture, input and transition owner remain authoritative. No host menu, invented page, input injection, or driving-controller change is introduced.

## Evidence

- recovery/20261010/OPTIONS-GAME-SETTINGS.md:45 identifies factory 1D0E30, constructor 1F46A0 and the borrowed camera; :51 identifies the eight authored groups; :53 documents profile fields; :61 documents navigation, clamp, defaults, Accept and Cancel.
- src/game/original_ui_game_options_controller50kg.cpp:65 authenticates options.dat and Widgets.Dat; :100 loads the local edit buffer; :139 restores local defaults; :153 adjusts volume by ten with clamping.
- src/game/original_ui_game_options_apply50kg.h:19 maps sound mode and four volume stores; the remembered group follows 188740, not a generic batch of profile writes.
- src/game/announcer_callbacks50ic.h:19 names the same four race audio fields; :73 applies category-specific restore gains. Copying the profile values does not justify replacing these gains or resetting live ducking.
- recovery/20261010/ui-options-audio/README.md:5 authenticates BACK event 5. That directory contains only the README in this clone; no claimed inspection of absent original bytes or instruction payloads.

## Implemented

The game bridge projects validated committed profile fields into OriginalUiAudioOptions50KG/RaceAudioSettings50IC. Invalid or unknown profiles leave output unchanged. It neither creates a profile nor reads the page's edit buffer.

The platform bridge additionally requires the exact sound-mode owner installed in the frontend transport and agreement with committed profile mode. The session exposes this through appliedAudioOptions. The result is a value snapshot, not a persistence receipt, race-group application receipt, or durable readiness token.

Accept now checks capacity for two transport revisions (188740, SELECT1) and two mixer revisions (possible SELECT stop/restart). Standalone 188740 also rejects exhausted transport revision capacity before mutating the mode owner. Existing profile-write ordering, manager admission and deferred SELECT remain unchanged. Calls require the existing serialized main-thread contract.

## Parent integration

Register both new .cpp files in all builds that compile the Options session. Supply the existing session with the same real application profile and sound-mode owner. Continue routing Main -> Options menu1 -> Game Options4 using the existing coordinator; do not add an Audio page or host overlay.

Use game::readOriginalUiAudioOptions50KG on the sole committed profile when handing settings to the actual race audio owner, or session.appliedAudioOptions while the Options owner is operational when frontend mode agreement is also required. Feed result.volumes into the existing race settings contract; preserve original group initialization/ducking sequencing. Do not apply arbitrary frontend music/effect gains: this inspected evidence does not establish those semantics.

Persistence, actual race-owner wiring, shared Main integration, and central build manifests are outside exclusive ownership. No end-to-end audibility or save/load claim is made. Parent must perform all integration review and testing.
