# Build 100: bounded basic GUI135 checkpoint

Status: local build PASS and bounded basic GUI smoke PASS. This is not a full-beta completion claim, broad regression pass, or CI result.

## Exact build

Build 100 compiled 496 units, with 273 fresh compiles. Binary SHA-256: `0f16fc52ce99ba3eec5a4b2ba4dfc383c2e42ca3c146ee9db72eb68edc885c24`. The publication source delta is pinned to the build100 SOURCE-MANIFEST; no later source edits are substituted. Manifest presence is not evidence that every file was compiled or every path executed.

## Observed flow

Normal Main -> Anakin Skywalker (POD19) -> Mos Espa (TA), Single Event -> startup -> W held 5 seconds -> Pause/Resume -> Pause -> Restart Cancel -> Restart Accept -> new grid -> Pause -> original Main confirmation -> Main -> normal Alt+F4 exit. The restarted session remained at tick 1, with no additional driving.

Exit code 0, no timeout, generated root absent. Both recorded sessions completed zero laps.

## Evidence and limits

The local GUI135 basic-smoke-observation.json records the parent-observed sequence and independently read result, launch, stdout and stderr receipts. Its writer checked screenshot presence, not image pixels. Screenshots were not uploaded because Library returned HTTP 401; no uploaded screenshot evidence is claimed here.

Keyboard input and software rendering were used; physical audio output was disabled while logical audio remained enabled. No broad racer/track sweep, physical-pad verification, hardware-audio verification, new Garage behavior, Tournament behavior, subtype-3 behavior, persistence, completed-lap/finish, or Results verification is claimed. Source presence and compilation do not prove inactive or unexercised routes work. The prior private Options/cut paths remain unverified by this basic flow.

This packet adds source/docs only: no game assets, original asset bytes, executables, object files, screenshots, or runtime caches. Previously published remote-only files are preserved through a base-tree delta. No implementation, additional review, build, gameplay test, or CI run was performed to prepare this publication packet.
