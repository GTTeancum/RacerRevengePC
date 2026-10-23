# Frontend return and reentry milestone: October 23, 2026

This source-only delta advances published commit 2a0d5fba61613987e7a5f4c4299c26ef19b2da85 to frozen frontend-reentry v46. Eight source leaves include the dispatcher/shared-dialog/aural and entity-scope reentry work, diagnostic additions, and the ScreenText correction. Earlier v44 GUI111 failed Main return with shared UI initial ownership rejection; it is not a pass and remains preserved. GUI113 is a later separately observed accepted result.

## Accepted GUI113 scope

The ordinary menu flow was Main Single Play to Tzidik (racer 24) to The Citadel. Physical W+Up acceleration reached HUD 500, and W+Left steering was observed at HUD 301. In Pause, choosing Restart and then Cancel returned to Pause. Confirming Restart produced a fresh grid. From Pause, Original Main was confirmed and visibly returned.

The returned Main was also interactive: it opened Tzidik selection again, and Escape/Back returned to Main. Closing via the titlebar X ended the process normally with exit code 0, no timeout, a runtime of 454.353 seconds and the generated runtime root absent. This establishes the observed return/reentry path rather than merely a visible but unresponsive Main screen.

No renewed driving after Restart was verified in this run. No lap completion, finish/Results, all-menu coverage, audio audibility, purchases, persistence, every racer/course pair or full original fidelity is claimed. HUD observations are not performance benchmarks. Earlier milestones remain scoped to their own snapshots, not full regression tests of this candidate.

## Build and distribution

The v46 build passed with 479 translation units, one fresh compilation and a recorded duration of 46.205 seconds. Its binary SHA-256 is 0e017c30814e1d7fef79a9be369286901cbbae97180bae34f73f61f49ffaeedd. Each changed source leaf matches the build source manifest. Build and GUI observations are separate evidence; the build receipt itself does not claim runtime testing.

This packet contains only the eight source changes and this status note. Original assets, generated payloads, binaries, objects, logs, backups, screenshots and private input/evidence records are excluded. Existing remote-only files are preserved without deletions. This is not a distributable game release or a claim that every default build configuration has been validated.
