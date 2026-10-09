# Current native race validation

The Linux check began on 2026-10-09 at 15:17:47 UTC: two units compiled,
191 reused, link exit 0. One mode1 race launched at 15:19:01 UTC and exited
normally without reported errors.

- Player completed three laps, finishing first at 160.928 host race seconds.
  All finish-continuation criteria passed: native owners continued, results
  stayed sealed, post-finish input could not take over, and models/audio remained.
- The source announcer prepared 54 clips and actually started types 3, 4 and 6.
  Last-lap music selected song 29; finish music then selected song 27.
- The completed-lap HUD showed all three native completed times and best lap;
  the captured presentation was visually inspected.
- PCM finalized at 174.032 seconds. Completed-render FPS ranged from
  53.6133 to 62.6681 in this cloud VM.

Tested runtime source: `0c3810cadd4750b4f5d4785320f0f3d259abe50a`.
Executable SHA-256:
`3a3f6b3f175e12e8f9ad7cd22d2db00129c74b809efb005cb1473f4769436ad7`.

## Scope and remaining gaps

This was one autodriven three-lap race and post-finish observation. Positive
damage-announcer type12, positive engine-damage dips, pause and restart were
not exercised. Other Dax/music selections and adverse-input paths need coverage.
The earlier direct keyboard probe used older source and does not establish
current-build controller usability or a fully manually driven race.

Mode1 uses recovered timing, record, queue, Dax and music logic with explicit
host scheduling, roster roles, rank/finish observation, a native clock backend
and known-null original HUD fields. The displayed overlay is host presentation.
Full original scheduler/console timing, IOP streaming and fade parity are not
claimed. Listening quality/clipping, full game progression, original results
menus and non-Linux execution remain open. See
[mode1 integration](native-mode1-integration50id.md) for detailed boundaries.
