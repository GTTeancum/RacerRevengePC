# Current native race validation

## GC WIP: TA menu sequence passed; GC cold load failed

The 2026-10-09 19:19:23 UTC central attempt built source `c310c4ae12d82a55bc6b68efdbfaed7ed5d73eff`: one unit compiled, 192 reused, link exit 0. The sole runtime ran from 19:20:11 to 19:20:28 UTC and exited 1 during GC material cold loading.

TA mode2 driving, Pause/Resume, Main Cancel then Confirm, track navigation, GC disclosure and pending selection completed. Pause lasted 2.0128 seconds with native/mixed clocks and actual PCM samples frozen; Resume retained the session/bank and advanced clocks. One expected GC cold transition began, but no GC ready state, driving or dense video completed.

Source inspection found slash-separated member names in the dynamic-material and ATM loaders, while the original GC entries use backslashes. The new three-file correction fixes those names and sequences cold loads before reading diagnostic messages. It is **UNBUILT / UNTESTED**. Failed evidence remains outside Git; no extra runtime was launched during preservation.

Tested binary SHA-256: `26924849463703c6ecc823062d0cb701eceb5c29b49afd464e8f802276031601`.
The earlier full Clegg/menu capture and three-lap coverage below remain separate from this partial GC attempt.

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
