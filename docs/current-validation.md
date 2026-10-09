# Current native race validation

## Finish continuation

The 2026-10-09 11:16 UTC Linux validation built successfully (21 units compiled,
172 reused, link exit 0) and completed one focused finish-continuation run.
The process exited normally without stopped racers or reported errors.

- Player completed three laps, finishing first at 160.928 race seconds.
- All eight native update counters continued after the human finish. The
  player's CD8 finish bit and ended manager gate 3 remained set.
- Official standings, sealed place/time and lap count stayed locked. Unfinished
  AI were not falsely recorded as finishers after official results closed.
- A six-second S + Down brake hold could not replace the native finish driver.
- The four-second countdown reached RESULTS; all seven opponent models and
  eight standings rows remained present.
- The original mix appended 10.08 seconds of actual PCM after the human finish.
- 34 completed-render samples: 54.3274–62.5233 FPS in this cloud VM.

## Direct keyboard probe

The 2026-10-09 12:16 UTC probe reused the same binary with autodrive absent.
One scripted keyboard session observed actual native dual throttle, brake,
differential steering, boost and release fields, plus exactly one Space press
and release. Player motion and all seven advancing opponents were observed.
Six committed captures and 24.816 seconds of finalized PCM were retained.
Exit was normal, with no observed native-boundary failure, checkpoint recovery
or restart. Completed-render FPS ranged from 50.4934 to 62.13 in this cloud VM.
This validates the scripted input path, not human/controller usability.

Tested source: `b97e661b0b86e81a08e434045f945ecfa272cae9`.
Executable SHA-256:
`5f84dd50403ca6d5fa9f5c3bf9a057db1ec464466bf2947a498f725a8040f61f`.

## Scope and remaining gaps

The full-race continuation check used autodrive; the short keyboard probe did
not complete a manual race. Neither current-source check restarted. Restart passed on
`ff0763ca42cd0f67127816df30e5c0a8aea3d333` before the finish-policy change and
needs rechecking with continuation.

The live post-results scene is a bounded host observation window. Original
results-menu transitions and audio/event waits remain unjoined. Listening
quality, earlier clipping reports, broader original-game integration and
non-Linux execution remain open. See [finish continuation](finish-continuation50ia.md)
for the implementation boundaries.
