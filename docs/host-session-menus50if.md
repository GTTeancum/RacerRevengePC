# Host front and session menus 50IF

## Status

Staged implementation only. No compilation, syntax check, test, decoder,
game execution, or visual validation was performed for this change. Root owns
the single central hourly build/runtime slot. The existing mode1 race validation
does not validate these additions.

These are explicitly host-designed menus. They do not reconstruct original
menu objects, transitions, settings, input focus fields, source UI sounds, or
post-finish menu/event waits. Only the existing TA session is offered.

## Player flow

- Windowed first launch opens a front menu after the existing cold asset load:
  Start Race (Start Drive for RR_RACE=0), Controls, Exit.
- Escape opens Pause during countdown, racing, or retained results. Escape or
  Resume returns to the same native session. Up/Down or W/S moves one row per
  fresh edge; Enter, keypad Enter, or Space selects. Holding a key never repeats.
- Pause offers Resume, Restart, Main Menu, Controls, Exit. Restart, Main Menu,
  and Exit selected here use a Cancel-first confirmation page. Escape cancels.
- Main Menu abandons the current race logically and freezes its native/audio
  owners in memory. It offers no Resume. Starting again closes all old owners
  in the existing order and performs the complete authored-grid cold setup.
- Restart still restarts every racer and countdown together. F5/restart retains
  the old immediate restart shortcut. Window close, stdin quit, and EOF exit.
- Controls return to the menu that opened them. Backspace/checkpoint is ignored
  while a menu is open. A stopped native boundary has Resume Unavailable; it
  still supports Restart, Main Menu, Controls, Exit.
- Physical throttle/brake/boost/repair/powerslide keys held across menu dismissal
  must be released before they can control the session. Navigation cannot
  accidentally steer, boost, or disable the host course driver after Resume.
- Only implemented actions appear; there are no cosmetic settings or unavailable
  tracks offered as playable choices.

## Alternate-racer integration seam

The front menu has a capability-gated Anakin/Clegg selector, hidden in the
standalone menu patch. alternateHumanReady50IF remains false until the complete
POD03 native/audio owner integration is applied and reviewed. Never enable the
row based solely on availability of a Clegg display mesh.

main owns selectedRacer60 (initially 1) across sessions. runSession captures its
immutable playerRacer60 at entry; the menu changes only the selectedRacer60
reference for the next session. Start with a changed choice returns through
the existing full cold-restart path. Resume cannot change the loaded racer.
Only race mode can expose the selector. ID1 is Anakin, ID3 is Clegg; no other
identity is accepted or shown as playable by this seam.

## Input and headless capture

Headless retains automatic start unless RR_START_MENU=1 is explicitly set.
RR_START_MENU=0 also suppresses the initial windowed front menu. Subsequent
explicit restarts enter the newly prepared race directly.

New newline-delimited commands are pause, resume, and menu. Pause is idempotent;
resume only dismisses the Pause page; menu returns directly to the front and
abandons the session for any later Start. The same menu state machine receives
raw Escape/Enter/Up/Down key edges from the window or stdin. Enter/Return is now
an accepted headless key name. Existing capture, restart, checkpoint, release,
and quit commands remain.

A key pulse is down Enter followed by up Enter. Direction and activation must
be issued in different polls; if both arrive in one poll, only navigation is
applied. This ensures a destructive choice is displayed before activation.
Keep stdin open: EOF is still an explicit terminal input condition. Wait for
acknowledged state/capture changes before sending another session-level action.

The UI can be captured before the first race frame, including on initial front
display. Capture overwrites the caller's existing PPM path. capture_state adds
host_menu=1 for a menu image, preserving its prior tick/wrong_way fields.

host_ui_state JSON lines accompany ready, menu transitions, and every completed
capture. Fields are reason, session (one-based cold-session number),
audio_bank_id (actual immutable bank address), menu, page, tick, mixed_frames,
loaded_racer, selected_racer.
Pages: Closed=0, Main=1, Pause=2, Controls=3, Restart=4, Leave=5, Exit=6.
The address is process-local diagnostic identity, never a saved pointer or input.

For the central runtime, request two paused captures at least two wall seconds
apart. Both must retain the same session, audio_bank_id, tick, and mixed_frames.
Resume must retain the owner identity; a later real gameplay capture must show
advancing native and mixed clocks. Front/paused stills are separate from any
gameplay video. Do not count menu swaps as completed gameplay frames.

## Ownership and rendering

Open menus release native held input once at the accepted boundary and stop
fixed-step planning, all racer updates, the shared mix, Dax updates, and music
updates. The existing live-audio pause API stops/flushes the output queue while
preserving voices, source group levels, immutable PCM, and source timers. Menu
entry never stops/restarts a music track or invents an original sound event.
Resume rebases host pacing; menu time cannot become catch-up physics.

submitHostMenu uses existing bounded HUD glyph storage and two additional
color-only private targets, allocated only when enableHostHud is selected.
At 1280x720 these add 7,372,800 color bytes. Both native geometry/target banks
and their publication index remain untouched. Pause can copy the last completed
race image; front uses a plain background. No interrupted native scene is
sampled, no cable/flare preparation runs, and no world frame is fabricated.

UI publication is transactional and has its own hostMenuAccepted receipt;
batchAccepted is reserved for actual native draws. Normal readback returns the
most recently accepted race or UI image. Failed submission must never be
swapped. Normal native submission reestablishes its rendering state on Resume.
The existing 1280x720 cold viewport limit still applies; minimize/oversize cannot
advance simulation or render a new menu until a supported drawable returns.

## Required central verification

1. Capture initial front; visit Controls, return, Start, and observe countdown.
2. Pause a real race, capture twice with a two-second hold, verify frozen native
   and mixed clocks and retained bank identity, then resume and verify advancement.
3. Exercise Escape edges/held keys, both Enter paths, Cancel/Back, repeated
   navigation, simultaneous direction+activation, focus loss, minimize/resize,
   and no boost/throttle takeover from menu keys.
4. Confirm Main Menu and check frozen state; Start must rebuild one fresh grid
   with all racers, lap owners, and source audio reset through existing cleanup.
5. Confirm Restart and Exit, and retain direct F5/restart/quit/EOF behavior.
6. Inspect menu and resumed race images; verify no GL publication-bank mismatch,
   native allocations during world work, stale source audio, or capture confusion.
