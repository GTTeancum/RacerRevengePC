# Staged race HUD readouts

Prepared 2026-10-09 against the active source snapshot listed in `preimage-sha256.txt`.
This directory is staging only. The active b180 menu/Clegg build remains untouched.

## Change

The existing top-left race panel gets two compact readouts in mode1:

- RACE: the human racer's native `lapContinuity.timing.timeCB8`.
- LAP TIME: native `lapContinuity.timing.timeCB0`, while the human is unfinished.

Values are copied at the existing HUD presentation point after the accepted
native timing, HOST race observer, and event flush. Formatting is `m:ss.cc`,
with live values truncated for display only. Source state is never changed.
The previously validated completed-lap/record panel retains its separate
centisecond rounding and CC0/CB4 data source.

The current-lap row disappears at finish. CB0 resets to the native frame delta
at every lap crossing, including the final crossing, so showing it as a final
lap would be wrong. RACE remains CB8; it is neither the sum of quantized laps
nor the HOST interpolated finish time. It can differ from the HOST finish log.
The existing one-human mode1 manager closure freezes this native value at the
accepted human-finish boundary while the world continues.

Countdown, GO, wrong-way, finish/results status, standings order, completed-lap
times, route bearing, heat, and damage retain their existing inputs and logic.
The new clocks are opt-in and remain absent from mode2/solo presentation.

## Layout and bounds

The top-left panel retains its 168-cell width and existing glyph size. Its
height grows from 32 to 60 cells while both clocks are present, then to 46
cells after finish. The standings panel begins four cells below its bottom;
its existing fit-to-available-height scaling also accounts for the added rows.
Route and heat/damage anchors are unchanged.

Each duration occupies at most 12 glyph positions at the admitted bound, ending
before cell 138 inside the panel. Both clock labels and values add at most 357
quads under the existing font's digit/label run counts; no extra panel or heap
storage is introduced. The existing 8192-quad batch is retained. This is a
source-read layout/capacity argument, not an executed geometry or visual test.

Enabled clocks require a race and finite, nonnegative displayed values no
greater than `maxVehicleWindowLapSeconds50DS`. This is an explicit HOST
presentation limit, not a new clamp or a claim about a source clock cap.
Disabled clock values are ignored. Current-lap seconds are ignored at finish.
Validation remains ahead of OpenGL submission.

## Speed boundary

There is no new player-facing speedometer or MPH/KPH label. The retained
source proves a body-to-authored-speed-domain scalar, but the inspected source
and documentation do not establish an original physical display unit or HUD
conversion. Specifically:

- `docs/course-pace50gu.md` identifies original consumer `0x173798..0x1737A4`
  multiplying cached body speed by binary32 `0x3F2E8BC1`, and explicitly limits
  that evidence to a numeric domain relationship.
- `src/game/authored_parameters49n.cpp` applies the authored engine unit scalar;
  it does not establish a HUD physical-unit label.
- `m.cachedSpeed34` is already consumed as the real native cached body-speed
  field in the main's native finish/Dax/physics wiring. Its three-dimensional
  source identity is distinct from HOST course-driver planar speed.

The existing periodic diagnostic line gains only
`native_cached_speed34_internal <value>`, read directly from `m.cachedSpeed34`.
No conversion, smoothing, speed estimate, new logging cadence, or mutation is
introduced. MPH/KPH remains deferred until original display-unit evidence exists.

## Timing source references

- `src/game/lap_timing50id.h`: fresh initialization zeros CB0/CB8; the
  `beginLapManagerClock50ID` prefix advances source manager time by the accepted
  binary32 physics delta; `beginRacerLapTiming50ID` updates CB0; numeric/event
  completion copies the live manager time into CB8 at `0x1B01EC`.
- `tests/native/race_mode1_host50id_fixture.h`: `beforeHostCommit` runs the
  timing pass; `afterHostCommit` sets manager ended bit0 once the HOST observer
  closes the one-human results. Later manager passes do not advance timing.
- `tests/native/terrain_interactive_drive50du.cpp`: the existing HUD snapshot
  follows that accepted world boundary. The existing native finished flag is
  exposed as `raceFinished`; no new finish inference is needed.
- `docs/native-mode1-integration50id.md`: accepted-world scheduling and source
  numeric behavior are bounded native integration, not original full-scheduler
  or original HUD-object parity.

## Delivery and integration

- `preimage/`: exact three active source files read before staging.
- `files/`: those files with the narrowly scoped proposed edits.
- `race-hud-readouts.patch`: unified diff, active repository-relative paths.
- `preimage-sha256.txt`: source snapshot identities for conflict review.

Apply the diff after the frozen test slot, resolving any later menu/main edits
by hunk. Do not overwrite newer active files with the full staged copies.

No build, syntax check, unit test, decoder, game launch, screen capture, or
visual validation was performed. No helper was spawned and no active source
file was edited. The following checks remain for the root's next authorized
integration/test slot:

1. Compile the changed renderer and main together.
2. Observe initial countdown at zero clocks; GO and ordinary racing advancement.
3. Observe current-lap reset and completed-lap table coexistence at each crossing.
4. Observe wrong-way text still present with both live clocks.
5. Verify pause/resume does not accumulate paused time; restart resets both clocks.
6. At finish, verify current-lap row disappears, CB8 freezes, and final CC0/BEST
   entries stay visible through retained-scene results.
7. Check 1280x720, 960x540, and 640x480 for panel separation/readability, plus the
   existing supported resize minimum and maximum admitted glyph values.
8. Verify explicit mode2 and solo routes retain their earlier layout; reject
   enabled NaN/negative/out-of-range clocks before any GL work.

These are pending checks, not claimed passes.
