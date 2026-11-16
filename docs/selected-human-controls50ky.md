# Selected native human controls (50KY)

Source-only candidate, October 12, 2026. Not compiled, syntax-checked, run, or
hardware-tested. Driving validation remains exclusively original in-game AI
for all eight racers. This work does not authorize simulated human-input tests.

## Selection and ownership

The persistent `ControlPresets49D` owner remains authoritative. The original
Controls pages retain their own defaults, preview/Accept behavior and selected
row; the host never silently replaces that selection with preset4.

For the separate direct-human startup, `RR_HUMAN_CONTROL_PRESET=0..7` explicitly
selects a canonical original49D row. The default remains4. This setting is
rejected when original Main is enabled, so it cannot override original UI
preferences. `RR_HUMAN_INPUT_DEVICE=keyboard` is the default native transport;
`controller` opts into a real SDL2 GameController in a native window.

The existing original Course route still admits ALL8 only. It does not become
a human Course lifecycle. ALL8 bypasses human binding and both packet adapters,
keeps every racer as original AI, and does not open a gameplay controller.
No new course driver, AI/human slot conversion, or device-presence override is
introduced. Original frontend second-controller routing and its actual missing-
controller popup are separate unfinished work; this patch does not admit page3.

Normal0–3 and advanced4–7 differ in actual input and cold physics configuration.
The selected human configuration enters the existing49N authored handling bind,
including source215408..21549C's steering/engine-torque branch. Configuration is
banked per racer so a human selection cannot alter another existing control
owner. ALL8 preserves its existing authored mode58 handling and input bypass.
A human selection changed after cold construction rejects before activation;
a fresh world is required instead of silently using stale coefficients.

## Explicit native keyboard policy

These are native host keys, not a claim to recover a retail PC keyboard layout.

- All rows: R feeds selected `flag69C` (repair); Space feeds selected `flag698`
  (boost); Left Shift feeds selected `flag6B4` (powerslide).
- Normal0–3: W/S feed the selected `action694`/`action690` throttle/brake
  descriptors. Left/Right feed packet6, the original left-stick X channel.
  Up/Down feed packet7, the original left-stick Y channel. This preserves the
  downstream source value; it does not claim a newly completed pitch mechanic.
- Advanced4–7: W/S feed packet7 (left engine), Up/Down packet5 (right engine).
  Independent negative/positive input reaches original throttle/airbrake
  consumers. The source conditioning, shaping, rate filters and twin-engine
  binder remain intact. Opposing keys on an axis center it.
- Keyboard Escape/F5/Backspace retain existing native host navigation actions.
- Zero-g lean (PC enhancement, not original; default preset 0 only): A leans
  left and D leans right on packet4, the byte the controller's right stick X
  writes (right stick X leans on a controller). The original reader conditions
  packet4 into channel 2, which nothing in a preset-0 race reads; A and D are
  bound nowhere else in a race. The lean acts only while the pod is in zero-g
  flight (`src/game/native_zero_gravity50zg.h`): N64 tilt, target -1/0/+1,
  0 below speed 200, ramp 3.2/s, halving per frame when released under 0.1.

The host Controls help now reflects normal/twin mode and the selected transport.
Keyboard re-arming includes steering keys as well as the old twin/action keys.
The old HOST course-driver option has no normal-preset owner and rejects that
combination; this patch neither adapts nor activates that driver.

## Real SDL controller transport

`WindowHost50DQ` optionally initializes SDL's GameController subsystem, loads
its public entry points and opens up to two devices already recognized by SDL's
mapping database. Unknown raw joystick layouts are not assigned guessed keys.
An opened device's actual joystick instance ID is checked against enumeration.
Stable slots are not renumbered when another device disconnects. `connected`
requires a real opened, still-attached SDL handle; failed opens remain absent.
Handles close before SDL/window teardown. Keyboard-only and ALL8 windows keep
this subsystem disabled.

The raw window snapshot exposes six SDL axes, fifteen logical buttons, and
genuine drained button edges. Start presses are not manufactured from held
state. A press/release pair in one drained poll retains its actual press edge.
A disconnect during sampling produces an absent snapshot. Absent/unfocused/
paused input emits an explicit neutral release packet. The native host world
pauses while the selected pad is absent or its reconnect latch awaits neutral.
A changed instance must return its controls to neutral before driving can
re-arm. A focus-loss/minimize and regain in one poll still disarms input and
suppresses that batch's Start edges; final focus cannot erase the interruption.
Opponent visits
do not touch the physical human latch or device identity.

The native positional policy is A/B/X/Y to Cross/Circle/Square/Triangle; SDL
shoulders/triggers map to L1/R1/L2/R2. Source49D still decides their actions for
each preset. SDL left/right stick axes become original packet6/7 and4/5 without
swapping engines, changing axis signs or adding host steering math. Signed
16-bit sticks map to original8-bit endpoints0/255 with center128. Actual trigger
magnitudes map to0–255; digital face/shoulder buttons supply0/255. This is not a
claim of pressure-sensitive face-button hardware support.

A genuine Start edge opens/resumes the existing native host Pause menu. Start
is kept out of original gameplay action dispatch: this is an explicit host
presentation policy, not a successful-looking original pause callback. Menu
navigation otherwise remains keyboard-owned. Camera, temporary-look, reset,
stick-click and unused pad directions have not acquired original human owners.
A reached unsupported descriptor reports its exact code and stops before
publishing that packet or dispatching a fake source success. GUIDE has no
original descriptor and stays with the operating system.

## Source and API provenance

- `src/game/control_presets49d.cpp`: exact eight original rows and configuration
  predicate; independent action/configuration indices are retained.
- `src/game/player_input48x.cpp`: packet6/7/4/5 channel ordering, original
  left/right digital steering descriptors15/13, selected action694/690 and flag
  queries. `src/game/control_binding48w.cpp` retains both numeric input branches.
- `src/game/authored_parameters49n.cpp`: configuration-sensitive cold handling.
- Original `UI/UIOption/config.dat` Control comments and `1F40F0` label routes
  corroborate descriptors0 Triangle,1 Circle,2 Cross,3 Square,4 L2,5 R2,6 L1,
  7 R1,8 Select,9 Start. The Controls recovery owns that authenticated evidence.
- [PS2SDK libpad packet declaration](https://github.com/ps2dev/ps2sdk/blob/master/ee/rpc/pad/include/libpad.h)
  corroborates the public packet axis/pressure offsets consumed by48Y. No
  libpad implementation or guest hardware execution is included here.
- [SDL2 axes](https://wiki.libsdl.org/SDL2/SDL_GameControllerAxis),
  [axis ranges](https://wiki.libsdl.org/SDL2/SDL_GameControllerGetAxis),
  [logical buttons](https://wiki.libsdl.org/SDL2/SDL_GameControllerButton),
  [open](https://wiki.libsdl.org/SDL2/SDL_GameControllerOpen),
  [attachment](https://wiki.libsdl.org/SDL2/SDL_GameControllerGetAttached), and
  [instance identity](https://wiki.libsdl.org/SDL2/SDL_JoystickGetDeviceInstanceID)
  define the native device interface. The selected controller path requires
  SDL2.0.6+ exports; Windows compares public enum/event offsets with official
  build headers. No external driver or controller-mapping package is installed.

No build/test result is claimed. Manual source review and file hashes are the
only review performed for this packet. Root retains the sole next bounded
smoke opportunity, with all eight original AI owners.
