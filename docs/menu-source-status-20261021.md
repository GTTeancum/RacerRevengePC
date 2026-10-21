# Menu source verification checkpoint: October 21, 2026

This source-only work-in-progress delta advances published v22 commit c484d2722be38c1dff52ec2db410515749a70121 to frozen shared-selection/load v42. It retains the prior v41 source changes and adds the accepted 29-path fresh Tournament Course navigation change, tested in GUI50. It is not complete Tournament functionality, a complete release or all-menus certification.

## Current v42 build and GUI50 verification

The v42 Application linked successfully: 382 translation units, 70 fresh objects and 312 dependency-verified retained objects, exit 0 and completed final postcheck. Binary SHA-256: 5014ac93e34352c69b7538b2ebd5417c62889e95c5c5ae98ad76e951fa7af7c9. Build duration was 315.618 seconds. This is the explicitly tested build configuration, not complete default-build or cross-platform validation.

During this packet preparation, all 4,219 frozen v42 source-manifest entries, 382 object hashes, recorded dependency hashes and the binary hash were checked against local files. The accepted Course proposal pins, launch binary hash, recorded parent observation and terminal GUI result were reconciled. All files in the prior v41 packet were hash-verified and retained unchanged. No new build or GUI run was performed for packet preparation. Observations below are recorded observations, not newly repeated tests; historical passes apply to their own frozen versions.

GUI50 on v42 passed normal Main to Tournament, selection of actual Anakin Skywalker (racer 19), Career Info, and Continue to the first Course page. The parent observed Mos Espa Open / Galactic Trials, a 2,200 purse, the KO percentage table, PAR 2, bribe text and hologram. Four directions and R left the fresh Course page unchanged. Confirm stayed on Course, with the log explicitly reporting unavailable input 4 at source 0x1DED58 and no gameplay. Escape returned to Career Info, Continue reentered Course, then Back returned through Career Info and Rider to Main.

Single Play recovery was separate: actual Tzidik (racer 24) and The Citadel (GC) were selected normally, followed by physical throttle to HUD speed 367, steering, Pause/Restart with a reconstructed grid, renewed throttle to 403, Pause/Main and normal close. These HUD values are observations, not performance benchmarks. The recovery was not Anakin and was not Tournament racing. Terminal result: exit 0, no timeout, generated runtime root absent.

This milestone is fresh, clean Anakin19 Tournament Course NAVIGATION ONLY. No earned progression, unlocked-course browsing, Tournament racing, Garage functionality or storage is admitted or verified. Detailed course-state snapshots were not emitted: visual values and the unchanged page are parent observations corroborated by activation/error logs, not complete runtime state-diff proofs. No full campaign, finish, all-combinations, audible device output or pixel-parity claim is made. The established Tzidik24/TC startup fixture, 1,800-second outer timeout, original asset inputs, muted device/race-audio policy and existing bounds were unchanged.

## Historical v41 build and GUI49 verification


The v41 Application linked successfully: 381 translation units, 1 fresh object and 380 dependency-verified retained objects, exit 0 and completed final postcheck. Binary SHA-256: 8a6c9a1fbdb3108d346f7d972761ea0738039321c143abe6abd639e5c5799197. This is the explicitly tested build configuration, not complete default-build or cross-platform validation.

The only source changes from v40 are original_ui_tournament_draw_contribution50kg.cpp and the added original_ui_tournament_prompt_placement50kg.h. During preparation of the historical v41 packet, all frozen v40 and v41 source-manifest entries, object hashes, recorded dependency hashes and binary hashes were checked against local files. Build receipts, launch binary hashes, parent observations and terminal GUI results were reconciled. The prior v40 packet was hash-verified and retained unchanged. No new build or GUI run was performed for packet preparation. Observations below are recorded observations, not newly repeated tests; historical passes apply to their own frozen versions.

GUI49 on v41 passed normal Tournament Anakin19 Career Info entry. Garage and Change Career icons appeared beside their labels with no observed overlap. This resolves the specific observed v40 placement defect in this bounded test, not full pixel parity. Back to Rider and Main passed. Single Play Tzidik24 and The Citadel were selected normally, with physical throttle to HUD 352, steering, Pause/Restart, reconstructed race and renewed throttle to 345, then Pause/Main and normal close. Terminal result: exit 0, no timeout, generated runtime root absent. HUD values are observations, not benchmarks.

GUI49 did NOT repeat the repeated-entry or unavailable-destination tests. Those remain prior GUI48 observations on v40. Tournament Course, racing, Garage functionality, progression, storage, full campaign, finish, all combinations, audible playback and pixel parity are not verified.

## Tournament Career Info: bounded navigation only

GUI48 on v40 passed Main to Tournament, normal selection of Anakin19 and rendering of Career Info. Four directions left the page unchanged; Confirm and R were unavailable without a destination. Back to Rider/Main, repeat entry/Back, and onward Options/Back were observed.

The same run then selected Single Play Tzidik24 normally, loaded The Citadel (GC), observed physical throttle to HUD speed 306 and steering, Pause/Restart with a fresh grid, renewed throttle to 386, and Pause/Main with normal return. These HUD values are observations, not performance benchmarks. Terminal result: exit 0, no timeout, generated runtime root absent.

The v40 run had overlapping square/triangle icons at Garage and Change Career labels, so visual parity did not pass. That historical defect remains recorded; the specific overlap was subsequently visibly resolved in v41 GUI49 as described above.

Tournament racing, Garage functionality, progression, storage, audible playback, all-menu functionality and pixel parity are NOT verified. This milestone covers Career Info navigation, not those unavailable or untested destinations.

## Preserved failed Tournament attempts

- GUI45 on v37: Tournament confirmation stayed at Select Career with unavailable destination/model status 3; missing Tournament archive registration was diagnosed separately. Recovery reached Single Play Anakin19/GC and observed speed 13 and steering, then speed 0 near terrain. This is not sustained-driving or Restart success. Pause/Main and normal exit were observed; the Tournament flow failed.
- GUI46 on v38: Anakin19 Career Confirm was rejected with tournament status 15/draw 4 after model status became 0; a canonical image-owner mismatch was diagnosed separately. Recovery used Tzidik24/GC, with acceleration to 373, steering and Main return. Restart was not tested, and the requested Anakin Single Play flow was not completed. Tournament/full-flow checks remained failed despite exit 0.
- GUI47 on v39: screen 9 activated, then the original UI update was rejected with status 13/source 2768232/tournament 16. The application exited with code 1 before visible standings. No in-race proof or recovery attempt was recorded. This is an explicit failure, not a pass or normal close.

GUI45, GUI46 and GUI47 had no timeout and their generated runtime roots were absent. Later GUI48 navigation success does not erase these failures.

## Earlier checkpoint observations

## Bounded human observations

- GUI40 on v32: Hall of Fame completed all 13 course selections, forward and reverse wrap, Confirm staying on the page, Back to remembered Options selection and reentry retaining Boonta Eve Classic. Rapid interrupted input, earned records and disk persistence were not verified.
- GUI41 on v33: Options Controller 2 absent-popup showed the original message and OK. Escape was ignored, Confirm dismissed only, repeat opening/closing and underlying Down navigation were observed without destination activation on dismissal. Main/Options reentry remembered slot 2. Connected Controller 2 and hotplug were not verified.
- GUI42 on v34: FAILED Save/Load entry. Selection stayed in Options with destination unavailable status 4; the missing checked glDrawArrays capability prerequisite was diagnosed separately. The run did drive with Tzidik and return to Main, but did not test Restart. Normal process exit does not turn this into a menu pass. This failure remains preserved.
- GUI43 on v35: Save/Load NAVIGATION ONLY passed. Original labels/icons appeared; Confirm and four directions were ignored; load action 5 and save action 7 were explicitly unavailable. Back, repeated reentry, remembered Options slot 5 after a Main roundtrip and subsequent Hall entry were observed. Save/Load functionality, storage operations and persistence did not pass and were not verified.
- GUI44 on v36: Main VS absent-popup showed the original message/OK; Escape was ignored, Up/Down and tested Left/Right did not change selection, Confirm dismissed only, repeat opening/closing and Main/Options popup roundtrips worked, and Hall entry after the roundtrip was observed. No obvious Main baseline visual regression was observed, but pixel parity and connected VS were not verified.

Each successful bounded run above continued through normal Main/Rider/Course selection with Tzidik (racer 24) and The Citadel (GC), physical acceleration and steering, Pause/Restart, renewed acceleration and verified Main return. GUI40 and GUI41 explicitly recorded a fresh restart grid. All five runs closed normally with exit 0 and no timeout; their generated runtime roots were absent.

Earlier Citadel GUI32 on v25 and Credits GUI37 on v29 are retained historical bounded observations, not a retest on v36. Credits observed original cards/logos, ignored Confirm/Right, Back to remembered Options, reentry and unattended timed return; not every card was captured.

## Limits and source-only distribution

No connected-controller/VS play, storage/persistence, finish/Results, all-menus, audible device output, pixel parity, performance benchmark, original-hardware timing equivalence or exhaustive racer/course-pair claim is made. Earlier menu passes are historical, not comprehensive v42 regressions. The specific v40 icon overlap was corrected and observed resolved in GUI49; full visual fidelity is not certified.

Distribution contains source, build-registration metadata and this document only. Original game assets, binaries, generated runtime data, screenshots/proofs, private inputs, Python bytecode, .orig backups and unrelated EndRace work are excluded. Existing remote-only files and history are preserved with no deletions. Users must supply their own original game assets.
