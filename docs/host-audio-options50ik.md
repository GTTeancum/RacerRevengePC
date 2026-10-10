# Host Audio Options 50IK

## Status and boundary

UNTESTED staged implementation. No build, syntax check, test, helper test,
decoder, game execution, or visual validation has run for this addition. Root
owns the single central hourly test slot. Earlier race/audio/menu evidence does
not validate these new settings or navigation paths.

This is a bounded host menu. It reuses the recovered original configuration
values and existing race group initializer. It does not reconstruct the
original options UI, save-game configuration, or live volume setter behavior.

## Player behavior

- Front and Pause menus offer Audio Options when audio is enabled. If audio
  startup is disabled, the row is absent rather than presenting inert controls.
- Effects, Music, Character, and Dax use integer levels from 0 through 100.
  Left/Right changes the selected value by five, clamped at either endpoint.
  Five-point editing is a host convenience, not a recovered source menu step.
- Up/Down or W/S selects a row. All editing uses fresh physical key edges.
  Held keys and SDL repeat events do not repeat the edit. Simultaneous Left and
  Right does nothing. Navigation takes priority over editing/activation.
- Enter, keypad Enter, or Space activates Restore Original Defaults or Back.
  Enter on a value is inert; Left/Right edits it. Escape and Back both retain the
  next-session choices and return to the menu that opened Audio Options.
- The page states: "NEXT START/RESTART ONLY. NOT SAVED TO DISK." It offers the
  four numbers, Restore Original Defaults, and Back. Default values are the
  existing original reset values: Effects 50, Music 100, Character 80, Dax 80.
- Changes do not affect Resume. While settings differ from the current session,
  Resume says Current Audio and Start/Restart says Apply Audio. Restart still
  uses the existing Cancel-first progress-loss confirmation. Cancel preserves
  the pending settings without changing the current audio.
- Start from the front after a settings change goes through the complete cold
  session rebuild. Start with unchanged settings uses the already prepared
  session when the existing menu conditions allow it.
- Explicit Restart, F5/restart, and race-mode Backspace/checkpoint restart apply
  pending settings through that same whole-race restart path. Returning to Main
  Menu alone leaves the old loaded audio intact until Start is selected.
- Free-drive automatic or manual checkpoint reconstruction keeps the effective
  configuration and carries the pending choice forward. It does not silently
  apply a menu change merely because recovery recreated its native owners.
- Choices survive in-process races, racer changes, and checkpoints only. Exit
  discards them. The next process launch uses the original fresh defaults.

## Source configuration and group ownership

RaceAudioSettings50IC remains the sole shared value shape: effects11020,
music11024, character11028, announcer1102C. The existing original admitted
domain check is reused. Owner::prepare rejects out-of-range input before bank
loading; its default argument preserves the original defaults for existing
callers.

The existing initializer maps Effects into Effects110, Engines114, World4D8;
Character into Character11C; Music into Music118; and Dax into Announcer120.
It retains the original binary32 scales/truncation and the unchanged auxiliary
group. Every currently bound provider, including selected-player Clegg,
opponents, materials, race music, and shared Dax, therefore uses the same group
ownership mechanism.

The application keeps requested settings separate from effective settings.
Each runSession captures the immutable effective settings. An explicit
SessionExit::Restart copies requested into effective only after the old session
has completed its existing cleanup. The next fresh Owner::prepare initializes
the original groups from that snapshot.

No live call to setConfiguredLevels is made. That existing API changes only the
eventual Dax-end restoration target and cannot provide a coherent immediate
four-control UI: it does not synchronously rewrite all live gains or the Dax
group. Pretending it were a complete live setter would leave mixed old/new
levels until unrelated callback boundaries.

Pending menu edits cannot change the current session's ducked or restored
levels. After explicit Start/Restart, both the group's initial levels and its
stored Dax-end restore configuration use the newly effective values. The
unchanged recovered begin/end functions still lower current integer groups and
restore from that effective config, never by adding saved deltas. Dax remains
outside its own ducked group traversal. No extra mixer, voice, callback, event,
decoder, timer, sample clock, or preview sound is added.

## Diagnostics and limits

Existing Page IDs remain unchanged; Audio is appended as 7. host_ui_state gains:

- audio_enabled: whether this session requested audio
- audio_pending: whether the enabled menu has unapplied differences
- audio_effective: four effective configuration values, in Effects/Music/
  Character/Dax order
- audio_requested: the four next Start/Restart values in the same order

These arrays describe configuration; they are not measured output, a device
playback claim, or the already-scaled live group values. Existing group
diagnostics remain the source for actual stored group levels. Menu edits keep
the existing session ID, bank identity, native tick, and mixed-frame count.

The editor stores four fixed-size strings and six rows. It borrows the existing
renderer, its permitted ASCII glyphs, and its 48-character text limit. The row
buffers live inside the menu owner and remain valid through synchronous
submitHostMenu. No renderer or native frame publication changes are needed.

Headless stdin gains the key names Left and Right, using the same SDL scancode
values as the windowed path. Use separate down/up pulses and wait for observed
state changes between navigation and activation, as with the existing menu.

## Required central verification, not run here

1. Build the integrated snapshot once through the root-owned slot. Check the
   front menu with Clegg selection enabled and disabled, with audio enabled and
   disabled, plus Pause, Controls, and every existing confirmation row.
2. Capture Audio Options at 640x480 and 1280x720. Confirm numbers, text, active
   row, Back, and original defaults are visible with the existing glyph set.
3. Exercise all four level rows, both bounds, simultaneous keys, held/repeated
   edges, Enter/keypad Enter/Space on value/action rows, Escape, menu commands,
   focus loss, and resize. Verify the original input-release guard on Resume.
4. Start a race with changed settings. Observe a new session and bank owner,
   exact effective config, unchanged requested config, and no pending flag.
   Verify original group initialization matches the configured values.
5. Pause during a real Dax episode, edit settings, take two separated captures,
   and Resume. Confirm native/mixed clocks froze while editing and the current
   episode restores the original effective values rather than pending values.
6. Cancel Restart, then confirm it. Check that the completed cold restart
   applies all four values, including Dax, and that a later real episode restores
   the newly effective group values. Test mute (0), maximum (100), and defaults.
7. Exercise front Start with dirty settings, dirty values returned to loaded
   values, Main Menu without Start, F5/restart, and race checkpoint restart.
8. In free drive, leave pending changes, Resume, trigger manual and automatic
   checkpoint reconstruction, then open Audio Options again. Effective audio
   must remain unchanged and the requested values must still be pending. A
   subsequent explicit Restart must apply them.
9. Exit/relaunch and confirm fresh defaults. Recheck selected-player Anakin and
   Clegg audio, full race completion, and shared Dax/music behavior in the same
   central validation process; none is established by this staged patch.
