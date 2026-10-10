# Explicit keyboard-to-native packet 50DR

Status: source-only, uncompiled, untested and unexecuted. This addition contains
only `keyboard_packet50dr.h`, `keyboard_packet50dr.cpp` and this usage note. It
does not register a target, poll SDL, probe a device or modify existing gameplay.
Root-owned integration and validation remain pending; no test slot was consumed.

## Boundary

50DR maps a caller-owned raw SDL scancode held array to the same native48Y/48X
packet fields as50DL. It does not depend on SDL headers,50DQ, a window class or
Linux device transport. Pass `KeyboardHeldSpan50DR{bytes, 512}` and explicit
`KeyboardAvailability50DR{connected, focused}`. `connected` means the owner has
a usable keyboard/window source, not that50DR detected physical hardware.

The owner must pump events/update its snapshot before composing and keep the
array unchanged throughout the call. Bytes are indexed by SDL scancode, not
locale-dependent keycodes, text characters or original game button names.
Mapped entries must be exactly0 or1. Scancode0 is the unknown sentinel and is
rejected as a binding;1..511 fit the input domain. SDL-reserved indices in that
domain have no extra interpretation. Only the owner can provide a meaningful
scancode. Unmapped entries are not inspected. Source byte counts other than512
are rejected on live input, including a larger buffer passed with a larger count.

The mapping is entirely caller-authored:

- `mapping.axes[0..3]` writes packet bytes4,5,6,7 respectively. Choose `KeyPair`
  with distinct negative/positive scancodes or explicit `Centered` for every slot.
  Negative alone writes0, positive alone255 and neither128. Every pair chooses
  `Center`, `NegativeWins` or `PositiveWins` for the both-held case. `Centered`
  always writes128 and ignores its other members.
- `mapping.buttons[0..15]` chooses `Key` with one scancode or explicit `Released`
  for each original numeric native code. `Released` ignores its scancode.
  A key can drive several buttons and axes, but the two ends of one pair must
  differ. Default `Unspecified` bindings are errors, not disabled controls.
- This API has no inversion flag: swap negative and positive keys when the
  desired packet orientation is reversed. Neither packet positions nor native
  numeric codes imply a physical key or an original game binding.

These are host keyboard policies. There is no acceleration ramp, deadband,
smoothing, timer, repeat, text handling, modifier interpretation, pressed-edge
cache or last-key-wins rule. A held array cannot establish which key was pressed
last.48X subsequently reads packet6/7/4/5, performs its original packet4 inversion
and applies its recovered conditioning.50DR neither repeats nor bypasses it.

## Focus and unavailable input

Every call must specify `KeyboardUnavailablePolicy50DR::Reject` or `ReleaseAll`.
An unspecified/invalid policy is rejected even while the input is available.
Mappings are always validated before deciding how to handle unavailable input.

- Connected and focused: use the current held span. A missing span, non512 count
  or nonbinary mapped key is an error. Neither unavailable policy turns corrupt
  live input into a successful neutral frame.
- Disconnected or unfocused with `Reject`: return false, leave output untouched
  and report `NotConnected` or `NotFocused`. The caller must stop or explicitly
  handle that input boundary; a previous output is not a new accepted poll.
- Disconnected or unfocused with `ReleaseAll`: do not read or validate the held
  span; `{nullptr,0}` is permitted. Publish axes128 and every button released,
  preserving all opaque template data. `diagnostic.disposition` records
  `ReleasedDisconnected` or `ReleasedUnfocused`, rather than `Live`. If both
  availability flags are false, disconnected takes precedence.

`ReleaseAll` is an explicit synthetic host sample. If passed to50CX as an accepted
sample,48Y can generate its normal release transitions against the previous bank.
50DR does not directly clear records, dispatch transitions, silently reuse stale
keys or claim an original device supplied a neutral packet. The caller must
consume a pending transition batch before another sample replaces it.

On restored focus/connection,50DR immediately uses the supplied current held
state. The window/input owner must refresh or clear stale keys and choose any
resume suppression, pause or reactivation policy. No ongoing connection or
keyboard-state query is performed here. Callers that want to freeze gameplay on
focus loss should use their own pause boundary; neutral input does not itself
stop the simulation.

## Packet format and failure atomicity

The downstream descriptor table must be exactly
`initialDualShockDescriptors48Y()` with `dualShockActiveButtons48Y` (16).50DR
obtains that profile directly; the caller cannot redirect a button into an axis
or opaque packet byte. The implementation also refuses out-of-range/colliding
pressure destinations and reused, nonsingle-bit or unsupported digital masks.

A successful conversion writes only:

- Byte0=0, marking an admitted host packet
- Axis bytes4..7
- The twelve pressure bytes8..19, as binary0/255
- Digital mask bits0F00 in byte2, using the original high-byte-first2/3 layout

Everything else is copied verbatim from the opaque caller template: byte1,
byte2's upper nibble, byte3 and bytes20..31.48Y ignores digital mirrors for the
twelve pressure buttons, so those mirror bits are also preserved. Consumed
digital buttons are active-low. Pressure255 becomes held with value1.0 under
48Y's threshold26; no analog pressure is synthesized. This is the subset consumed
by48Y/48X, not a complete physical PS2 packet or recovered keyboard transport.

Every failure preserves the complete output. The diagnostic is replaced on every
call and contains the error, mapping index, axis/button selector and offending
scancode where applicable. Successful calls have error `None`, mappingIndex=-1,
scancode=-1 and an explicit disposition. Output/template may alias as complete
objects; other input/output/diagnostic storage must be separate and valid. No
pointer is retained. Loops and stack storage are fixed-size, with at most24 raw
key reads and no allocation.

## Usage and50CX integration

The following establishes intentionally disabled bindings, then fills one
caller-selected axis and button. All variables named `caller...` must come from
the application configuration; this is not a built-in keyboard preset.

```cpp
rr::platform::KeyboardPacketMapping50DR mapping{};
for (auto& axis : mapping.axes)
    axis.source = rr::platform::KeyboardAxisSource50DR::Centered;
for (auto& button : mapping.buttons)
    button.source = rr::platform::KeyboardButtonSource50DR::Released;

// Caller validates callerAxisSlot < 4 and callerNativeCode < 16.
mapping.axes[callerAxisSlot] = {
    rr::platform::KeyboardAxisSource50DR::KeyPair,
    callerNegativeScancode, callerPositiveScancode,
    rr::platform::KeyboardOpposingKeys50DR::Center
};
mapping.buttons[callerNativeCode] = {
    rr::platform::KeyboardButtonSource50DR::Key, callerButtonScancode
};

rr::platform::KeyboardPacketDiagnostic50DR diagnostic;
const bool composed = rr::platform::composeKeyboardPacket50DR(
    {callerSourceConnected, callerWindowFocused},
    {callerHeldBytes, rr::platform::keyboardScancodeCount50DR}, mapping,
    rr::platform::KeyboardUnavailablePolicy50DR::ReleaseAll,
    callerPacketTemplate, callerPacketOutput, diagnostic);
if (!composed) {
    // Report/resolve diagnostic and do not treat callerPacketOutput as fresh.
    return;
}
// Consume/copy any preceding transition batch before this successful sample.
const auto descriptors = rr::game::initialDualShockDescriptors48Y();
if (!callerIngress.samplePacket(true, true, callerPacketOutput, descriptors,
        rr::game::dualShockActiveButtons48Y, callerDeviceId)) {
    // Respect50CX failure; do not prepare a new frame from an older sample.
    return;
}
```

After successful sampling, keep the50CX owner at a stable address. Consume/copy
its ordered transitions through the existing48Z owner, resolve any queue failure
before replacing that batch, and dispatch using the actual live bank. At the
real frame entry, prepare exactly one borrowed49K frame reader with the real
action codes/context and ready49I factor, then consume it before another sample
or preparation. Follow50CX/49W's existing conditioning/filter/binding path. Do
not also run the composed48Y player-control helper; that would filter twice.

The sample's `true,true` flags admit the host conversion, including an explicitly
selected synthetic release sample; they do not prove original PS2 SDK readiness.
No keyboard polling, event ownership, original preset selection, device ID,
frame clock, transition queue, binding schedule or game launcher is supplied by
this adapter. A consuming target must compile50DR and the existing48Y/native
dependencies. No build registry, tests, protected milestone or game binary was
changed by this source-only addition.
