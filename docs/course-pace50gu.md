# Authored-limit course pace (50GU)

`CourseDriverTuning50GK::authoredSpeedOnly` is an explicit HOST course-driver
option. Its default is `false`, preserving the existing target-speed calculation
and turn-throttle reduction. The caller must explicitly enable it.

With `authoredSpeedOnly = true`:

- Target speed is exactly the minimum authored ATA `SpeedLimit` encountered in
  the existing bounded lookahead traversal, multiplied by the explicit
  `speedLimitToBodyScale` value.
- The HOST `cruiseSpeed`, `minimumCornerSpeed`, heading/bend severity and
  narrow-width speed factors do not cap or otherwise alter that target.
- The HOST `turnThrottleReduction` multiplier is skipped.
- The existing speed-error gain and demand clamp remain. The ordinary saturated
  differential demands remain `left = clamp(demand + turn, -1, 1)` and
  `right = clamp(demand - turn, -1, 1)`. Turning can therefore still lower one
  engine's demand or apply its ordinary braking input; this option does not
  guarantee a particular achieved speed.

Lookahead selection, including its existing width-based distance bound, is
unchanged. Route choice still follows authored `Next0` from the source-owned
current segment without changing progression. All existing tuning validation,
finite-input checks, invalid-route diagnostics, and stuck/off-course stop
latches remain active. Unused HOST cap settings must still pass their existing
tuning validation. A non-finite authored-limit/scale product is rejected by the
existing target-speed finite check, with neutral output.

No source physics, vehicle state, collision handling, route/lap state, or packet
dispatch path is changed. The packet helper still writes only the existing two
preset4 analog throttle bytes. This remains a HOST-authored test aid, not the
original game's AI. The scale is explicit; no physical speed unit or equivalence
to the original AI's behavior is established by this option.

## Live-client selection and source-grounded conversion

The live client defaults to authored mode and explicitly enables this option.
`RR_AUTODRIVE_PACE=authored` selects that mode; setting
`RR_AUTODRIVE_PACE=conservative` restores the previous capped demonstration.
The reusable driver's default remains `false` for existing callers.

Original ELF inspection establishes the narrow speed-domain relationship:

- Loader `0x2A15E4..0x2A160C` reads the authored `SpeedLimit` and copies it to
  node field `0x5C` without scaling; getter `0x2A2520` returns that raw value.
- Consumer `0x173798..0x1737A4` compares desired speed against the body's cached
  speed multiplied by the binary32 constant with bits `0x3F2E8BC1`.
- The live client's authored mode therefore explicitly applies the reciprocal,
  approximately `1.4666628`, as `speedLimitToBodyScale`. An authored limit of
  `600` produces a target of approximately `880` canonical body-speed units.
  This is a numeric domain conversion, not a verified physical-unit label.

The loader instructions are retained in
`evidence/gridCenter50AC/start-assessment/track-loader-nodes.txt`; the original
AI consumer and wider integration boundary are described in
[the 50GK driver assessment](host-course-driver50gk.md).

The original target multipliers, race-state decisions and boost logic remain
unjoined. This HOST driver measures planar speed, whereas the original consumer
uses cached three-dimensional speed. Reusing the proven scalar conversion does
not establish original-AI equivalence or promise an achieved speed or successful
course traversal. Build and drive evidence belong to the separate live-client
integration; no build, test, or live run was performed by this driver/docs edit.
