# Explicit host-to-native input packet 50DL

Status: **source-only, uncompiled, untested and unexecuted**. No build, compiler,
test, executable, device probe or validation script was run. Only the new50DL
header, implementation and this note were added. Existing source/build/test
files, protected milestones, game assets and the user's computer were untouched.
No hardware input, original transport parity or playable-game claim follows.

## Supported boundary

`joystick_packet50dl.{h,cpp}` supplies one fixed-storage, allocation-free mapping
from50DK joydev observations to the fields consumed by native48Y and48X. It uses
`initialDualShockDescriptors48Y()` directly. The downstream caller must use that
exact descriptor table and `dualShockActiveButtons48Y` (16). Custom descriptors,
other pressure offsets, unspecified controls, physical button labels, control
presets, analog pressure and controller discovery are outside this first adapter.
No external descriptor pointer is accepted, so a supplied
pressure layout cannot overwrite packet axes or the ready marker.

Every mapping is caller-authored:

- `mapping.axes[0..3]` chooses the raw axis indices for packet bytes4,5,6,7.
- Each axis supplies signed joydev minimum, center and maximum, and inversion.
  The calibration must lie in[-32768,32767] with minimum < center < maximum.
- `mapping.buttons[0..15]` explicitly chooses a source kind for each original
  numeric code. `Button` uses `input.rawIndex` and `input.inverted`, ignoring the
  calibration members. `AxisThreshold` uses the complete `input` axis mapping
  and an explicit threshold1..255. `Released` deliberately disables that slot,
  ignoring its other members. Default `Unspecified` always rejects.
- All four axis mappings and all sixteen button source choices are required.
  Repeated raw indices are allowed: one observed source may drive more than one
  target with separately supplied calibrations/inversions. Axis index0 and button
  index0 are distinct source banks. No physical layout or source is inferred.
- Target offsets come only from the fixed array positions and original numeric
  descriptors. Callers cannot assign multiple controls to one target byte. The
  implementation also rejects descriptor pressure offsets outside8..19 or reused
  by another button, and digital masks that overlap, are not single bits, or fall
  outside0F00. It never accepts an axis/ready byte as a pressure destination.

The function takes connection state and a snapshot from the same50DK owner/open
session. Access must be serialized, with no intervening transport mutation.
It rejects anything except `Open`, unknown metadata, counts outside the unsigned-
byte UAPI domain, bad mappings, invalid calibration, any unobserved mapped field
or a mapped raw button value other than0/1. Unmapped fields need not be observed.
An explicit `Released` binding requires no source observation; it represents a
caller decision to disable that control, never a fallback for a missing sample.
Count and per-field `observed` checks are used; the aggregate observation counters
are not treated as proof that an individual source has been sampled.

`Open` means50DK still owns a successfully initialized descriptor. Unplugging may
not yet have been detected by a read, and values in the snapshot are last accepted
events rather than an atomic whole-device measurement. The bridge performs no
I/O and cannot strengthen that guarantee. It does not consume timestamps, demand
an initialization event rather than an ordinary event, drain an event queue or
choose how old an observation may be. Those policies remain with the caller.

## Explicit host conversion policy

These rules are deliberately host-adapter policy, not claimed recovered PS2
hardware behavior or source arithmetic parity.

For a mapped raw axis, clamp the value to the caller's minimum/maximum. Normalize
below center using `(value - center)/(center - minimum)` and at/above center using
`(value - center)/(maximum - center)`. Apply the requested sign inversion to this
normalized value, then map[-1,+1] linearly to[0,255]. Quantize to the nearest byte,
with exact half-byte ties toward the larger byte. The implementation uses bounded
signed64-bit integer rational arithmetic, independent of a floating-point rounding
mode. Endpoints become0/255 (exchanged by inversion), and exact center becomes128
with either inversion. It never computes inversion as `255 - quantizedByte`, which
would change a centered128 to127. Asymmetric calibrated ranges retain their own
side denominators before inversion. No host deadband or smoothing is added.

The unchanged48X reader subsequently uses packet6/7/4/5 for its four channels,
applies its recovered packet4 inversion, and performs its recovered deadband
conditioning. Caller mapping inversion describes the desired packet orientation;
it does not disable, replace or implicitly compensate for48X's inversion.

A `Button` source must have an observed0 or1. Optional inversion swaps those states.
An `AxisThreshold` source goes through the same explicit calibrated/inverted axis
conversion and becomes pressed exactly when the resulting byte is at least the
caller-supplied threshold. Threshold0 is rejected;1..255 are admitted. This allows
explicit trigger or directional-axis bindings without guessing which raw index
represents one. Inversion occurs before quantization, and thresholding after it;
there is no hysteresis or extra deadband. `Released` always selects released.
Pressure descriptors receive byte0 for released and255 for pressed. Consequently
48Y's existing threshold26 yields released or held value1.0, with no intermediate
pressure values. Digital descriptors receive their active-low mask bit instead.
Unusual nonzero joydev button values are rejected rather than guessed as pressed.

The original numeric descriptor layout is:

| Numeric code | Flags | Pressure byte | Digital mask |
| --- | --- | --- | --- |
| 0,1,2,3 | 3 | 12,13,14,15 | 0010,0020,0040,0080 |
| 4,5,6,7 | 3 | 18,19,16,17 | 0001,0002,0004,0008 |
| 8,9,10,11 | 1 | unused | 0100,0800,0400,0200 |
| 12,13,14,15 | 3 | 10,8,11,9 | 1000,2000,4000,8000 |

48Y ignores the digital mask for pressure descriptors.50DL therefore preserves
those twelve unused digital bits from the caller's template; it does not create
a digital mirror of a pressure button. Only masks0100/0800/0400/0200 change, using
the original high-byte-first packet2/3 convention. The packet contains exactly
the information these readers consume, not a complete physical PS2 packet.

## Template, publication and ingress

`composeJoystickPacket50DL(connection, snapshot, mapping, packetTemplate, output,
diagnostic)` publishes only after every required observation and mapping passes.
Success writes byte0=0 as the host packet's admitted-read marker, axes4..7,
pressure8..19, and digital mask bits0F00. All other bytes and bits are copied
verbatim: byte1, byte3, byte2's high nibble and bytes20..31. No packet type byte,
opaque response, transport negotiation or unused button state is invented.

On any failure the complete output remains unchanged. The diagnostic identifies
the failure category and, where relevant, the mapping array index and whether it
belongs to `mapping.buttons`. The index is-1 for non-field errors and success.
A retained prior output must not be presented as a new accepted poll after
conversion failure. The template may be the same
complete object as output; other input/diagnostic/output storage must be distinct.
No borrowed pointer survives the call. Loops are bounded by4 and16; no per-frame
heap storage is needed.

A later authorized integration can use this sequence:

1. Poll the explicitly selected50DK owner and inspect its result/connection.
   Partial progress and queue-empty/budget/interruption distinctions remain real.
   Do not relabel a failed poll as a successful complete device read.
2. Supply that owner's connection/snapshot, explicit mappings and opaque packet
   template to50DL. A false result does not justify a successful50CX sample.
3. Only on true, pass `true, true, output, initialDualShockDescriptors48Y(),
   dualShockActiveButtons48Y, deviceId` to50CX's `samplePacket` with the caller's
   real device ID. The two Boolean values describe this accepted host conversion;
   they make no claim about original PS2 SDK readiness/return values.
4. Continue50CX's existing transition ownership and actual frame-entry contract.
   Initial history and initialization-event edges remain explicit caller choices;
   this bridge neither resets held records nor calls shaping/filtering/binding.

If conversion fails, readiness/read-failure scheduling, previous-output retention
and whether to run a frame remain caller decisions. No disconnected neutral frame,
automatic retry, stale accepted frame or synthetic release is issued here.

A consuming target must eventually compile the new50DL implementation and the
existing48Y implementation along with its transport/ingress dependencies. No
build registry was edited. Root-owned compilation, tests and hardware verification
remain pending under the team-wide test cap; this addition consumed no test slot.
