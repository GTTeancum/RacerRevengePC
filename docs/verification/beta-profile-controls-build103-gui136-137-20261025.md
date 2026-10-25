# Build 103: bounded profile/control checkpoint

Status: build 103 PASS and focused GUI137 correction/process-restore check completed. This is not a full-beta completion claim, broad regression pass, or GitHub CI result. GUI136 used v102 and had known menu defects; it is not relabeled as a v103 pass.

## Exact build and source

Build 103 compiled 496 units, 56 fresh, in 597.651453 seconds. Binary SHA-256: `14701e7dbf470c5d2c49fbbdebe05fd8eafcdb037a955b06d692cfe62ac2f9fc`. The 64 source/docs changes versus published v100 match build103 SOURCE-MANIFEST.json. This separate publication checkpoint is not candidate source or a compiled input. Manifest presence does not prove every file was compiled or every route executed.

## GUI137: corrected controls and process restore

The parent observed Tab/Circle Save producing SAVED; R/Square Load opening confirmation; Escape canceling while retaining the page; and R then R confirming LOADED. Automatic Back returned the ordinary Original UI title instead of retaining stale hints. These are keyboard-adapter observations, not physical-controller verification.

The process reused genuine save files written by GUI136, not seeded files. Startup showed restored volumes [40,100,80,80], Main saved slot 3, and profile generation 3. This establishes the bounded settings/profile restore observed across these processes, not persistence of earned race progress.

Same-run selected-racer and in-race screenshots were observed by the parent: Anakin Skywalker (racer 19), Mos Espa (TA), Single Event startup grid. The run ended after startup proof with no driving input and no repeated Career intro test. Direct terminal records show exit code 0, no timeout, 326.6603248119354 seconds, generated runtime root absent, one development-drive tick and zero completed laps.

## GUI136: preceding v102 observations and defects

The earlier v102-resume1 binary was `723155c6840e5930925661845f866da4f40eae6df3ea064cf22849f1e6a620ad`. The parent observed Game Options Cancel preserving the old volume, Accept saving the new volume, manual Save/Load, Anakin 19 -> Tournament/Career -> Mos Espa, actual moving authored intro, grid/countdown, five seconds of W after GO, Pause -> Main confirmation -> Main return, and normal exit. Two shorter W holds occurred during the intro; independent intro-skip behavior was not verified. Direct records show exit 0, no timeout, 488 development-drive ticks and zero completed laps.

The GUI136 intro screenshot has visible temporal-blending ghost trails. Motion-blur strength/fidelity and camera-transition visual parity remain unverified; the intro evidence is a startup/lifecycle observation, not visual acceptance.

GUI136 exposed reversed Save/Load adapter shortcuts and stale title hints after automatic Back. Those defects were corrected for v103 and checked in GUI137. GUI136 is bounded partial runtime success with known defects, not a full smoke pass; its intro and drive observations must not be attributed to the v103 binary.

## Evidence boundaries

The packet preparer read build/launch/result/log receipts and checked source hashes and screenshot presence. Visual actions and screenshot content are parent observations, not an independent image review by the preparer. Five GUI136/137 screenshots were privately delivered to the owner through Library. Screenshots are not included in this GitHub publication, and no private Library identifiers are published here.

Both runs used keyboard input and software rendering with physical audio output disabled. No audible-output, physical-pad, earned-progress, completed-lap, finish, Results, broad racer/track sweep, or full-beta validation is claimed. Source presence and compilation do not establish inactive or unexercised paths. No GitHub CI pass is claimed.

Publication contains source/docs only, with no original asset bytes, executables, object files, user saves, screenshots, runtime logs or caches. Remote-only files and the older checkpoint are preserved through a base-tree delta. Packet preparation performed no candidate edits, additional code review, build, gameplay test, upload, or GitHub write.
