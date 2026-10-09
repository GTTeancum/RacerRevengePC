# Current native race validation

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

Tested source: `b97e661b0b86e81a08e434045f945ecfa272cae9`.
Executable SHA-256:
`5f84dd50403ca6d5fa9f5c3bf9a057db1ec464466bf2947a498f725a8040f61f`.

## Scope and remaining gaps

Autodrive was enabled. The post-finish input hold does not validate manual
racing with autodrive disabled. This run did not restart; restart passed on
`ff0763ca42cd0f67127816df30e5c0a8aea3d333` before the finish-policy change and
needs rechecking with continuation.

The live post-results scene is a bounded host observation window. Original
results-menu transitions and audio/event waits remain unjoined. Listening
quality, earlier clipping reports, broader original-game integration and
non-Linux execution remain open. See [finish continuation](finish-continuation50ia.md)
for the implementation boundaries.
