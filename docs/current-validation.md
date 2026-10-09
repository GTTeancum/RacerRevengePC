# Current native race validation

## Engine-audio integration and finish continuation

The Linux validation began on 2026-10-09 at 13:17:03 UTC: one unit compiled,
192 reused, link exit 0. One autodriven race launched at 13:18:01 UTC and
exited normally with no stopped racers or reported errors.

- Player completed three laps, finishing first at 160.928 race seconds.
- All eight native update counters continued after finish. Player CD8 and ended
  manager gate 3 persisted; official standings and sealed results stayed locked.
- A six-second brake hold did not replace the native finish driver. The
  four-second countdown reached RESULTS with seven opponent models retained.
- Original PCM finalized at 174 seconds, including 10.064 seconds after finish.
  Completed-render FPS ranged from 52.0452 to 62.5956 in this cloud VM.
- The last modulation sample recorded 10,410 updates and 20,820 shared RNG draws,
  with zero dips, returns or completions and pitch scale 1. This validates
  integration and healthy-state execution, not positive damage-dip behavior.

Tested runtime source: `c10a655a2764314f485998318dd03ad644cdaa5d`.
Executable SHA-256:
`342850cf7d1cbb510eac72f3e1ffc222e18baf078bb4a6a5acfb9cba4bbf7dae`.

## Earlier direct keyboard probe

The 12:16 UTC probe on `b97e661b0b86e81a08e434045f945ecfa272cae9`, before
modulation integration, observed native dual throttle, brake, differential,
boost and release with autodrive absent. It retained six captures and 24.816
seconds of PCM, with normal exit and no observed recovery or restart. This
establishes the scripted input path on that source, not controller usability
or a complete manually driven race.

## Remaining scope

Positive damage dips, thresholds, cooldown/return curves, and controls after
the shared-RNG change need focused coverage. Restart needs rechecking with
continuation and modulation; its earlier pass used the prior finish policy.
Original results-menu/audio-event waits, listening quality/clipping, broader
original-game integration and non-Linux execution remain open. The retained
post-results scene is a host observation window. See
[engine modulation](engine-damage-audio50ia.md) and
[finish continuation](finish-continuation50ia.md) for exact boundaries.
