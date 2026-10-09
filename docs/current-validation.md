# Current native race validation

The current source adds an **unbuilt and untested** original track-class correction after the run below: loaded TA selects A and GC selects C for all cold racer owners. The earlier result does not validate the resulting class-dependent handling/propulsion. See [track-class scope](gc-authored-track-class50ip.md).

## GC WIP: player advances; two opponents stop before a lap

The 2026-10-09 21:19 central slot built source `e9ece06f227d4834c1591d235e92b1856d944756`: two units compiled, 191 reused, link exit 0. One runtime ran from 21:21:01 to 21:21:45 UTC and exited normally. The GC steering preview and shared world-yaw correction were exercised.

- TA driving, Pause/Resume, Main cancellation/confirmation, GC disclosure and one cold GC transition completed. Menu/session and capture-clock checks passed.
- Capture retained 342 unique actual frames over 20.0112 seconds, 17.040 captured fps, maximum gap 137.6 ms. This was a 20-second run, not 60-second coverage.
- The player advanced to AGC node 28. Opponent slots 2 and 3 stopped with status 3 at nodes 7 and 8. **No GC lap completed.** First-bend improvement does not establish complete field or course success.
- Artifact export verified all 342 frames in a 20.012-second video with actual native PCM. Maximum audio anchor skew was 60.4 ms and final skew -43.2 ms, within the existing admission bounds. No synthesized, normalized, resampled or stretched sound was used; listening quality remains unclaimed.

Runtime binary SHA-256: `7817c40e6baf9da3d5d5c7aea5b574f1a5b4c4f37c7a20652726c4d16f0dea09`.
Only status documentation changed afterward. No additional build, runtime, test or encoding occurred during this update. Gate/LostAI behavior, manual GC usability and full-course completion remain unresolved. Earlier results apply to their own source versions.

## Current Linux menu and Clegg check

The 2026-10-09 17:18 UTC central build compiled four units, reused 189, and linked successfully. One runtime ran from 17:19:27 to 17:20:01 UTC and exited normally.

- Actual Main-menu input selected Clegg. One expected cold rebuild installed the selected eight-racer field and audio bank.
- Pause held for 2.0117 seconds with native ticks, mixed-frame counts, and actual PCM samples frozen. Resume retained the session/bank and advanced the clocks.
- Dense capture recorded 401 unique actual frames across 20.002 seconds, about 20 captured fps, with a maximum 84.1 ms gap. Final Main and quit completed normally. The resulting video passed the capture/encoding checks; this rate describes recording, not uncaptured rendering performance.
- The headless pause blit correction passed this scenario. Audio came from the actual native mixer; no device-playback or listening-quality claim follows.

Tested runtime source: `959d94e1044f74651e7a2713093eec0ac65da932`.
Executable SHA-256: `0ccae9e676b8d9e0ce7e3b9a798745020f67d88f4ced1a5cea61f6430fd67f98`.

Only Clegg selection and the basic Main/start/pause/resume/return flow were exercised. Audio sliders, other human choices, confirmation paths, and Results presentation remain untested. The Results overlay is separately staged. Driving used the HOST authored-pace driver; this was neither a manually driven race nor original AI, and it did not reach a full-race finish.

## Earlier full-race coverage

The 15:17 UTC mode1 baseline `0c3810cadd4750b4f5d4785320f0f3d259abe50a` completed three laps, first at 160.928 host race seconds, with finish-continuation gates passing. Actual Dax types 3/4/6 played; music selected song 29 on the last lap and 27 at finish. All three completed native lap times and best lap were visually inspected. That earlier full-race result does not establish a complete Clegg race on the current build.

## Remaining boundaries

Positive damage-announcer type12 and engine-damage dips, broader input/controller usability, listening quality, full progression, other tracks, and non-Linux execution need coverage. Recovered mode1 logic still uses explicit host scheduling, roster roles, rank/finish observation, a native clock backend, and known-null original HUD fields. The overlay is host presentation. Full original scheduler/console timing, IOP streaming, and fade parity are not claimed. See [mode1 integration](native-mode1-integration50id.md).
