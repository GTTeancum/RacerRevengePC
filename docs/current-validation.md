# Current native race validation

## Shared announcer and finish continuation

Linux validation began on 2026-10-09 at 14:17:23 UTC: three units compiled,
190 reused, link exit 0. One autodriven race launched at 14:18:20 UTC and
exited normally without reported errors.

- Player completed three laps, finishing first at 160.928 race seconds.
  All finish-continuation criteria passed: native owners kept advancing,
  official results stayed sealed, post-finish input could not take over,
  and opponent models/audio remained present.
- Five actual Dax starts each produced one begin, end and completion.
  Preemptions, rejections and group failures were all zero.
- Observed group levels ducked to29/27/41/45/29/80/100 and restored to
  45/42/64/70/45/80/100. Dax remained at80 throughout.
- The original mix finalized to174 seconds of PCM. Completed-render FPS
  ranged from51.3866 to62.6776 in this cloud VM.

Tested runtime source: `79ece3b97e9e184a8e34c1c79b73f7137749c4c2`.
Executable SHA-256:
`6411a8673f85428c1ec7366a5fe7bbf020e91f16bebd2afd4ce74db62846ca81`.

## Scope and remaining gaps

This validates the shared host-Dax completion/duck/restore path. Pause,
preemption and restart were not exercised. Mode2 remains active and original
type12 is still unjoined; this is not complete original announcer admission.

Player engine modulation previously passed healthy-state integration with
zero positive dips; damage thresholds and outward/return curves remain
unverified. The earlier no-autodrive keyboard probe used source `b97e661b`;
it established scripted input, not current-build usability or a full manual race.

Original results-menu/audio-event waits, listening quality/clipping, broader
original-game integration and non-Linux execution remain open. The retained
post-results scene is a host observation window. See
[shared announcer](shared-announcer-integration50ic.md),
[engine modulation](engine-damage-audio50ia.md) and
[finish continuation](finish-continuation50ia.md) for exact boundaries.
