# v72 to v78: compiled source and bounded GUI131 regression

This publication proposal contains 20 exact source files (18 modified and two added) plus this document, with no deletions. Files under src/assets are C++ source, not bundled original game assets. Only files/ contents are proposed payload; build outputs, logs, screenshots and local metadata are excluded.

## Active changes versus private foundations

Canonical TA startup now obtains its native camera Session from the actual Application factory instead of directly allocating that Session at the previous root. The Application retains weak custody of the same Session, verifies admitted inputs, and checks that it has retired before Application destruction. This allocation and custody change is active on canonical TA startup. It must not be described as wholly dormant code.

The source controller constructor remains private and uncalled. Allocation through the factory does not establish source-controller construction, cut playback, source-loader completion, attachment, audio publication or renderer admission. Related camera/entity ownership foundations remain bounded by their source gates. No complete Tournament or intro playback claim follows from a successful TA startup.

Private MemoryCard foundations extend the component work through four ordinary icons and the first authored-strip packet. Later strips, containing load completion, page438 publication, attachment, input and drawing remain gated. These are not an activated Save/Load or storage feature. Shared Garage quad and material code also changes, so existing Garage presentation and return behavior are qualified only by the bounded observations below.

## Build and bounded runtime qualification

The v78 terminal build receipt records PASS: 496 translation units, 108 fresh, 388 reused, 800.120448 seconds. Binary SHA-256: f319531cbf9c934819f374d53c9d8fd5175339fdc3412236a4f208db2d2d7661. Packet preparation did not rebuild or operate the application.

GUI131 completed with exit code 0, timeout=false, elapsed 1673.5464857 seconds, and generated_root_absent=true. The parent verified these bounded routes:

- Main to Anakin to Mos Espa Open TA through normal selectors.
- TA startup, brief initial W input, Pause/Resume, then a bound W press with durationMs=5000, HUD 180, and Pause to Main.
- Options Save/Load entry and Back without IO.
- Anakin Garage showing six stats, Cancel to Career; Garage reentry showing six stats, unchanged Accept to Career.
- Back to Rider and Main, normal Alt-F4 exit.

Read-only log corroboration is separate from the visual observations and input-tool duration. Both frontend closures report cleanup_complete=true. The development drive records 1124 ticks, 491 render commits, 490 window swaps, one controller press and one controller release. Those input counters do not by themselves prove the five-second duration or HUD value. Completed laps, marker cursor and lap callbacks are all zero. Final frontend counters show 22,324 updates/renders/submissions, 22 input events, zero unavailable destinations and audio_failed=0. Screen activations include 13→9→13→9→5→4, consistent with the observed Garage/Career/Rider/Main route.

Controller 1 and Controller 2 configuration were not tested in this run. The drive input counters are not controller-configuration coverage. Final controls_accepts=0 and controls_saved_presets=[0,0]; Garage button outcomes are visual observations, not persistence proof. Intermediate Garage page-custody diagnostics twice report status=15/source_pc=1927212 before later successful stages. Frontend world_ticks=0 is not the separate development-drive count.

This run exercises canonical TA native Session allocation/custody and existing menu/short-driving paths. It does not call or qualify the private source TA controller or private Save/Load owners. No Restart, purchases, storage IO, positive-cut/Tournament startup, lap completion, race finish or Results tests were performed. Hardware audio was not requested. No full-game pass or private-feature activation is claimed. Six parent-listed proof hashes were independently verified; the packet preparer did not independently review image pixels. Earlier GUI130 coverage is not substituted for this run.


## Publication gate

Parent alone may publish after fresh remote verification; GUI131 is qualified only within the bounds above. Recheck the expected main commit and complete base tree, reconcile v72 source blobs, preserve remote-only files and previous documentation, and calculate the merged tree independently. Do not force-update, delete paths or create a new branch. No external write occurred during preparation. Prior packets and source snapshots remain unchanged.
