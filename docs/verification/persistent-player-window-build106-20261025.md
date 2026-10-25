# Persistent player window and native Windows entry: v106 bounded checkpoint

Status: Windows and Linux compile/link PASS; bounded Linux GUI138 persistent-window/loading/restart/return/exit smoke PASS. This is not Windows runtime validation, full-beta acceptance or a CI pass.

## Exact builds and bounded observation

- Windows game-build-106: compile/link PASS, 501 units, 83 fresh and 418 reused, zero failed units, link exit 0, 912.5148577690125 seconds. EXE SHA-256: `b43c4954cde48066ded70068df42dda86669f79a2691df33ee04a3b485e24a53`. Parent PE inspection identifies AMD64 GUI subsystem 2 with system imports KERNEL32.dll, msvcrt.dll and USER32.dll. This is not Windows runtime verification.
- Linux build106: PASS, 496 units, 86 fresh and 410 reused, 735.6543393135071 seconds. Binary SHA-256: `6d58cbf546d111283b4aa5d96193a5a71e5cc7d0bed196b8a966df50366f5f71`.
- Runtime dependency/archive verification belongs to the separate tester-package receipt, not this source-only publication checkpoint.
- GUI138 parent-observed sequence: normal Main -> Aldar Beedo (rider 10) -> Mos Espa (TA), Single Event -> visible LOADING -> race HUD -> Pause -> Restart confirmation -> same-window loading -> restarted race -> Pause -> return-to-Main confirmation -> Main -> normal Alt+F4 exit. No driving input was applied; both race sessions recorded one tick and zero completed laps.
- Direct terminal result: exit 0, no timeout, 598.7790293693542 seconds, generated runtime root absent. Logs show SDL window ID 1 created once, retained across display generations 1–4, and destroyed once at terminal cleanup. Parent GUI observations retained X11 window ID 39845894 through Main/loading/race/returned Main. Cold-load ready records report 12.674 and 12.719 seconds for the two race starts; these are observed local timings, not performance guarantees.
- Same-run selected-racer, in-race, loading, restart-loading and returned-main screenshots exist locally. Visual contents and input actions are parent observations; the packet preparer verified terminal/log receipts and screenshot presence, not an independent image review. No screenshot bytes or private identifiers are included in this source publication.
- The first GUI admission attempt failed before game startup because the execution environments could not share the private temporary path. The identical verified binary was staged into shared storage for the actual run. This is an admission issue, not a failed game runtime.
- All 4,696 candidate source files match both build106 source manifests at final packet preparation. Manifest membership does not prove compilation or execution of each path.

## Implemented source delta

The candidate introduces a retained normal-player window/context across frontend and race-session handoffs, explicit session GL reset and ownership handling, cooperative loading feedback, stage diagnostics and cancellation handling. The loading presenter is host feedback, not a claim of source resource readiness, gameplay progression, or scene correctness. The bounded Linux sequence above supplies single-window/loading/restart/return/exit observations, not exhaustive lifetime or rendering correctness. Close-during-load cancellation remains unverified.

Normal Windows no-argument entry is implemented for double-clicking RacerRevengePC.exe. It selects the executable's directory as the original asset root, checks required original files, reports startup errors visibly, and uses ordinary original Main entry rather than fixtures or unlock overrides. The requested player presentation remains 1280x896 with a source 640x448 canvas. The GUI subsystem is confirmed by parent PE inspection; absence of a console and double-click behavior still require Windows runtime verification.

Normal double-click and explicit --assets entry append startup/stdout/stderr/stage/exit records to one RacerRevenge.log beside the executable, separated by UTC session headers. Earlier log sessions are retained. The native log writer rejects concurrent writers; unwritable-folder startup reports failure rather than silently choosing another location. Optional launchers are reduced to invoking the native entry, not generating separate timestamped log directories. Other explicit diagnostic modes retain caller-owned output behavior. No automatic uploads, screenshots, memory dumps or telemetry are enabled by this entry.

Best-effort native fault logging records an exception code/address and image-relative offset when available, then continues exception search rather than resuming faulting execution. It is not guaranteed to capture every abort, forced kill, loader failure or fault. Errors before startup may precede log creation. A missing exit marker is not evidence of a clean shutdown.

Log files can contain local paths, device/driver details, game selections and earlier sessions. Users are instructed to review them before voluntarily sharing. This publication does not include user logs, private computer paths, Library identifiers, original assets or save files. Wide Windows startup APIs do not establish end-to-end Unicode correctness of downstream loaders.

## Evidence boundaries

Prior v105 Windows compile/link evidence and prior Linux GUI136/137 observations belong to earlier binaries, not this v106 candidate. Previously observed menu behavior does not establish successful race loading or integrated single-window behavior here. Native Windows runtime, loading feedback, logging/error paths and gameplay must be qualified against the exact final executable and observations; only the explicitly bounded Linux GUI138 observations above are established here. The new native Windows runtime, GUI logger, best-effort fault filter, close-during-load behavior, physical-controller operation and repeated Course/Options visits have not been runtime-verified.

This work does not establish earned-progress persistence, completed laps, race finish/Results, physical-pad behavior, audible output, broad racer/track coverage or full-beta completion. Existing temporal-blending ghost trails and unverified motion-blur/camera-transition visual parity are not resolved by the host-lifetime/loading/logging work. No CI pass is claimed.

The publication payload contains source/docs only. Original assets, executable/DLL binaries, user saves, screenshots, private runtime logs and crash data are excluded. Earlier remote-only documents are preserved. Packet preparation itself performs no build, gameplay test, upload, or GitHub write.
