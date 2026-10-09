# Native powerslide input 50FU

## Scope

The development client can expose the existing source-backed powerslide action
with **Left Shift**, through the same packet, persistent held-state, player
reader and aerodynamic path as its other native controls. This adds a host input
binding. It does not add slip forces, change steering or aerodynamic tuning,
force a slide mode, or replace the original timer priority.

The original numeric action is `flag6B4`, selected as button code **7** by both
preset 0 and the client's advanced preset 4. Preset 4 retains the preset 0
action-button row while replacing the two digital throttle/brake queries with
the existing `-1` sentinels and enabling the independent-engine configuration.
Left Shift is a declared development keyboard choice, not a recovered original
keyboard binding. Historical original UI research corroborated code 7 as R1;
that physical label is not newly recovered or independently verified here.

## Existing source path

The following links were checked in the current source without executing the
game or an original-instruction oracle:

1. `src/game/control_presets49d.cpp`: row 4 and row 0 both contain
   `flag6B4=7`. `resolveControlPreset49D` carries that field into
   `SelectedControlPreset49D::actions`. The preset extraction's original setter
   interval is `188880..188C04`, recorded in
   `evidence/controlPresets49D/recovery-manifest.json`.
2. `src/game/device_held48y.cpp`: initial descriptor 7 is
   `{flags=3, packetOffset=17, digitalMask=0x0008}`. Because it is a pressure
   descriptor, `sampleDeviceButtons48Y` reads packet byte 17. A value at least
   26 sets held state to 1. The keyboard packet adapter writes 255 when pressed
   and zero when released; it does not inject player flags directly.
3. `src/game/player_input48x.cpp`: `readPlayerInput48X` queries `flag6B4` with
   mask `0x3F` when `(gateByte58 & 3) == 2`, and writes its result to
   `flagsB9C` bit 2. The other gate clears the low three bits.
4. `src/game/control_binding48w.cpp`: `bindVehicleControls48W` copies bit 2 to
   `aeroTriggerD8` as exact 1 or 0 in the advanced configuration route. The
   source-disabled states 3 and 4 clear this field. `applyControlModelBindings48W`
   copies it into `AeroInput48E::triggerD8`.
5. `src/game/vehicle_frame49w.cpp`: the live frame calls the same player reader
   and binding code. `produceVehicleForces49W` applies those bindings and then
   calls `applyAerodynamics48E` with the persistent aero state. The visual stage
   also receives the actual binding as `VehicleVisualInput49R::aeroD8`.
6. `src/game/vehicle_aero48e.cpp`: `triggerD8` refreshes `timer144` to 4 only
   when the first timer is inactive. The existing second-timer branch consumes
   `timerLateral10c`, `timerYaw110` and `timerDrag114`. Their authored keys are
   `PowerslideSideForce`, `PowerslideSpoilerYawMoment` and `PowerslideThrust`,
   bound by `src/game/authored_parameters49n.cpp`.

This held action needs no new player-event helper. The existing press-dispatch
path still handles ordinary button transitions, but its configured category-2
action is `action6AC=8`, not powerslide code 7. The host continues to keep that
unimplemented action released; no successful stub is introduced.

## Client integration

In `tests/native/terrain_interactive_drive50du.cpp`, derive the binding from the
resolved preset and reject an unexpected identity before indexing the mapping:

```cpp
const auto powerslideButtonCode =
    static_cast<std::uint32_t>(selectedControls.actions.flag6B4);
require50DU(powerslideButtonCode == 7,
    "retained powerslide button identity changed");
keyMapping.buttons[powerslideButtonCode] = {
    platform::KeyboardButtonSource50DR::Key, 225}; // Left Shift
```

The surrounding mapping still initializes every other button to Released, then
maps repair to R and boost to Space. Packet composition, focus/disconnection
release handling, ordered transition dispatch and frame scheduling remain the
existing path. No additional fixture helper is needed for this single binding.
The adapter supports one physical scancode per button; Right Shift is not
implicitly an alias.

`platform/linux/native/game_host50er.cpp` now accepts the case-insensitive
headless key names `shift`, `lshift`, and `leftshift`, all resolving to the same
SDL physical Left Shift scancode 225. Use separate held commands such as
`down shift` and later `up shift`. As with every headless key, a press and
release consumed in one poll are not converted into an artificial gameplay
pulse. `release`, `restart`, EOF and `quit` retain their normal release-all
behavior.

## What observation must distinguish

- Held code 7, `flagsB9C & 4`, and `aeroTriggerD8` establish the input path.
- `timer140`, `timer144` and `lowSpeedDc` establish the selected source mode.
  `triggerD0` resets the first timer to 0.25 and clears the second timer. That
  contact-priority mode suppresses a powerslide refresh while active.
- Speed below 50 has higher force-mode priority even if `timer144` is live.
  Above that speed and without the first timer, the second-timer mode uses
  the authored powerslide coefficients and the existing differential aero
  inputs. A held key alone is not evidence of changed cornering forces.
- Releasing the key clears the held input and D8 binding through the normal
  reader, but it does not erase `timer144`. The source's timer continues to
  decay. Preserve this behavior rather than making release snap the car back.

## Verification status

This change was source-inspected only. No builds, tests or runtime driving were
performed for the parser/documentation edit. Existing physics-test results are
not new validation of this host binding. The integration owner should verify
press, hold, release, focus loss, restart and the higher-speed differential-turn
case before claiming usable cornering or a runtime pass.

The original `game_host50er.cpp` was copied before editing to
`solo-powerslide50fu/preimage/platform/linux/native/game_host50er.cpp` under the
workspace root. This documentation is new. No physics source, tuning,
held 49C/48U work, original memory image or user computer was changed.
