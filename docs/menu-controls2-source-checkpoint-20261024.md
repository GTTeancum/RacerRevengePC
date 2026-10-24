# v61 menu-source checkpoint: bounded GUI126 regression PASS

This source-only checkpoint describes the delta from published v58 commit 182ab80d5c1372e688fc39a9d1642b35c67c9294 to compiled v61. GUI126 completed a bounded regression pass. This is not complete menu, physical Controller 2, storage, race-finish or release certification.

## Included source changes

The delta contains 50 source/build-registration files: 44 modified and six added, with no removals. The changes span shared menu raster inputs and warning packets; shared raster, warning and callback implementation; menu and Garage material/binding and draw plumbing; frontend/Main/Options/Garage session integration; profile-name/private Save/Load ownership; Controller 2 menu admission; and the corresponding build registrations and entry integration. These are source changes, not claims that every path now works.

Controller 2 admission preserves the reviewed shared menu integration. Physical Controller 2 connected/positive behavior and disconnect behavior remain untested. Save/Load source changes establish private ownership only: storage operations remain gated, and no storage/persistence functionality is claimed.

## Build PASS and bounded runtime evidence

The compiled v61 Application build passed with 495 translation units, 70 fresh objects and 425 reused objects. Binary SHA-256: 650da3485c5aabb29c292359bbc09278af5798eda8221417b348779c568443ba. Packet preparation checked object and recorded dependency hashes against local files and verified the binary hash. The build receipt itself records runtime_tested=false because it is a compile receipt; subsequent GUI126 runtime evidence is recorded separately below. No new build or GUI run was performed for packet preparation. This is not cross-platform or comprehensive default-build certification.

## GUI126 bounded regression PASS

The final parent observation and terminal result agree on the following bounded checks:
- Tzidik/The Citadel startup, short driving input and Pause to Main.
- Controller 1 Normal 2 alternate-preset preview, Cancel, reentry showing original Normal 1, and unchanged Accept.
- Genuine absent Controller 2 popup and dismissal.
- Save/Load entry and Back without storage IO.
- Hall fresh defaults Mos Espa to Soro Suub and reverse, then Back.
- Garage after Options fully rendered; Cancel returned to Career.
- Garage reentry fully rendered; unchanged Accept returned to Career.
- Career Back to Rider, Back to Main, then normal Alt+F4 close.

The run exited with code 0, timeout false, elapsed 1750.9489707946777 seconds and generated runtime root absent. The final frontend closure record reports cleanup_complete=true. Host lap telemetry records completed=0, marker_cursor=1, callbacks=0 and unavailable_sound_calls=0. The run did not complete a lap or test Results. Runtime duration is not a performance benchmark. Launch binary hash matches the compiled v61 binary; supplied proof hashes and terminal evidence were verified during packet preparation, not by repeating the run.

No purchases, changed Garage upgrades, storage IO or physical second-controller operation were performed in this bounded run. Unchanged Accept is not proof of upgrade purchase/application or persistence. Genuine Controller 2 absence is not a positive connected-controller or disconnect test.

## Explicit limits and distribution

Physical Controller 2 positive/disconnect operation is untested. Save/Load storage remains gated. No lap completion, finish/Results, all combinations, complete menu functionality, full campaign, audible playback or pixel-parity claim is made. Short driving is not race completion.

Only source/build-registration files and this verification note belong to the publication payload. Original assets, binaries, generated runtime data, private inputs/proofs, screenshots, .orig backups and cache/bytecode files are excluded. Existing remote-only files and history must be preserved with no deletions, forced update or new branch. Users must supply their own original game assets.
