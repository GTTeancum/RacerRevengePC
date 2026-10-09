# Live race Results presentation 50IL

Status: staged implementation only, rebased onto the GC native-scene/projected
renderer source after the earlier TA pause/Clegg/video check passed. No
compilation, syntax check, test, game, decoder, or visual validation was
performed for this rebase. The integrated GC changes also remain untested.

## Presentation boundary

The existing humanResultsReady flag enables the screen after the existing
approximately four-second finish countdown. No new finish detection, result
sealing, standings order, audio admission, or original scene/menu event is
implemented. This is native client presentation of already committed data;
the original game's complete menu/audio-event wait sequence remains unported.

The screen uses:

- Player name from the existing exact named standings row.
- Place and finish seconds from the sealed race observer's finishPlace and
  finishSeconds. Finish time is displayed to the nearest hundredth; it is the
  existing HOST crossing-time observation, not source CB8 or a sum of lap times.
- Completed lap durations and best lap from the existing native CC0/CB4 HUD
  publication. No running CB0 value is substituted after finish. Without those
  native fields, the screen explicitly says LAP TIMES UNAVAILABLE.
- The existing complete named standings snapshot. The heading is STANDINGS AT
  YOUR FINISH because the one-human race owner closes official results at that
  boundary while native racers continue moving. Remaining rows say UNFINISHED,
  never DNF or a fabricated later finish result.

## Live behavior and controls

Results is drawn in the normal accepted world-frame HUD path. Its display,
selection, and Cancel-first confirmations do not open the blocking Pause page,
stop simulation, modify the native finish driver, reset voices/groups, or add a
second audio mix. The existing finish driving, all opponent draws, and shared
source audio continue until the user actually leaves or explicitly pauses.

Up/Down or W/S selects Restart Race, Main Menu, or Exit. Enter, keypad Enter,
or Space opens a confirmation with Cancel selected. Up/Down then Enter accepts
the chosen action; Enter on Cancel or Escape returns to Results without leaving.
Selecting and confirming cannot happen in one drained input snapshot.

Escape outside a Results confirmation still opens the existing Pause menu.
Explicit pause/menu commands keep their existing meaning. Focus loss or entry
into another menu cancels a pending Results confirmation; returning requires
freshly released navigation/activation keys. Held input cannot accidentally
confirm an old choice. The original normal-menu key-release suppression remains.

Confirmed Restart uses the existing complete session cleanup and cold grid
rebuild. Confirmed Main Menu routes the explicit request through the existing
front-menu path, preserving its abandoned-session/start semantics. Confirmed
Exit uses existing cleanup. F5/restart, Backspace/checkpoint, window close, quit,
and EOF preserve their previously documented direct-action behavior.

## Renderer and diagnostics

The panel uses bounded existing HUD glyph storage, stack formatting buffers,
and the ordinary native frame transaction. It adds no heap strings, GPU targets,
framebuffer blits, texture ownership, or mixer/world updates. It checks selected
action/time/state bounds before GL work; the existing glyph overflow rejection
remains authoritative. New HUD fields default off and are appended to preserve
existing aggregate field order.

Results captures are ordinary live race images: capture_state host_menu stays0.
host_ui_state adds results_visible, results_action (0 Restart, 1 Main Menu,
2 Exit), and results_confirming. Confirmed departures also emit
host_results_action with Action values1 Restart,2 Main Menu,3 Exit plus actual
tick/mixed_frames. Existing session/bank identity and timing observations remain.

## Deferred central verification

1. Complete a real race. Confirm the screen appears only after the existing
   finish countdown and uses exact sealed place/time and native completed laps.
2. Compare captures several seconds apart while Results and its confirmation
   are open. Native finish/opponent ticks and shared mixed-frame counts must
   advance with unchanged session/audio bank, and official result fields remain
   sealed. Do not mistake the continued world for changing official standings.
3. Inspect all eight names, unfinished labels, lap times, selected buttons, and
   readable layout at supported viewport sizes. Check mode2's unavailable timing.
4. Cancel each action with both Cancel and Escape, then verify continued driving
   and sound. Test held activation, simultaneous navigation/activation, focus
   changes, Pause/Resume, and existing key re-arming.
5. Confirm Main Menu, Restart, and Exit through the existing ownership paths;
   verify complete old-session cleanup and fresh selected-player/audio setup.

None of these verification steps has been executed for this staged change.
