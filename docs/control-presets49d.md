# 49D: original controller preset state

## New production behavior

`src/game/control_presets 49d.h/.cpp` removes caller-invented preset defaults
from the existing supplied-ready-packet gameplay input path. The two supported
slots each retain all ten signed action codes, the preset index, the independent
configuration word, and five additional numeric initialization fields.
`ControlPresets49D` is exactly 136 bytes; the immutable eight-row table is 320
bytes. All new APIs use fixed scalar storage, execute serially and allocate no
heap memory. No file access or original-instruction interpreter runs in production.

- `selectControlPreset49D` selects a preset for native slot 0 or slot 1.
- `initializeControlPresets49D` executes the complete local two-slot default loop.
- `previewControlPreset49D` reproduces the original stateful preview operation.
- `resolveControlPreset49D` supplies existing 48X action codes, 49A action6AC and
  the independent 48W configuration predicate.
- `playerEventPresetRows49D` returns a caller-owned two-row 49A snapshot. Its
  lifetime, environment binding and refresh after selection remain explicit.
- `stepReadyPresetPlayerControls49D` composes real native preset state with
  existing 48Y ready-packet held sampling, 48X input reading/shaping/filtering and
 48W engine/aero numeric output binding. It replaces only the supplied binding
  context's configuration predicate, preserving the separate vehicle mode58.

The wrapper is an actual numerical gameplay dataflow, not an OS device loop,
force-update scheduler, event queue or playable race. Existing downstream
numerical and action-effect boundaries remain unchanged.

## Exact source intervals and domain

The only original source is the existing ELF `work/native/project/disc/SLUS_202.68`,
SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The standalone recovery reads no RAM, generated translations or game resources.
It freezes:

| Operation | Half-open original interval | Words |
|---|---|---:|
| Selection |188880–188C04|225|
| Complete local defaults loop |1864F8–18658C|37|
| Stateful preview |188C10–188D10|64|
| Jump table |3A8DA0–3A8DC0|8 data words|

Boundary-only evidence is recorded separately. The defaults interval is a local
loop in1864B0, not that enclosing constructor. The previously identified interior
186538–186588 omits necessary setup and is not described as complete initialization.
Original setters have no slot check. Native APIs deliberately admit only the two
slots established by this loop; unsupported slots fail without mutation. This
restriction does not claim the original rejects other indices.

All 32-bit preset words are supported. Selection writes the saved index first.
Unsigned preset>=8 then returns with mode and all ten codes unchanged. Valid
presets write configuration first, then codes in original order:
694,690,6A8,698,69C,6AC,6A0,6A4,6B0,6B4. Serial typed state has no callback or
concurrent observer between those stores.

## Rows and defaults

Columns are original offsets, with no invented physical-button meaning:

|Preset|690|694|698|69C|6A0|6A4|6A8|6AC|6B0|6B4|11040|
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
|0|3|2|5|4|9|0|1|8|6|7|0|
|1|0|1|7|6|9|3|2|8|4|5|0|
|2|1|2|5|4|9|0|3|8|6|7|0|
|3|3|2|7|5|9|0|1|8|6|4|0|
|4|-1|-1|5|4|9|0|1|8|6|7|1|
|5|-1|-1|7|6|9|3|2|8|4|5|1|
|6|-1|-1|5|4|9|0|3|8|6|7|1|
|7|-1|-1|7|5|9|0|1|8|6|4|1|

Defaults initialize slots 0 then 1. Per slot they write10F94=7,10F9C=7,10FA4=1,
11040=0,11048=0, select preset 0, then 11030=5 and11038=0. The meanings of those
five extra fields and disk persistence are not inferred.

Preview saves slot 0's index, applies the requested index to slot 0, snapshots its
ten codes, and reapplies the saved index. It does not save and restore all state.
If the saved index is invalid, restoring it leaves the requested row/mode in
place. If a valid saved index accompanied a custom row, restoring it reinstalls
that preset's canonical row. An invalid requested index snapshots the current
row. Slot1 and the five side fields are preserved throughout.

## Consumer binding and remaining limits

Player+FFC selects the action row. Player/vehicle+5C selects configuration11040.
Those two indices, the packet's deviceId and the vehicle mode58 are distinct;
no equality is assumed. Both native indices must be0 or1, with rejection before
sampling. Preset0 produces48X `{action694=2,action690=3,flag698=5,flag69C=4,
flag6B4=7}`,49A action6AC=8, and false configuration mode. Presets4–7 retain the
original -1 sentinels in690/694; existing 48X skips queries only for those two.

This milestone does not change packet masks/pressure offsets, held-query semantics,
physical labels, JoyDirectionFlipped or axis inversion. Parent-provided original
UI research is useful future provenance, but the 49D proof does not depend on it.
No configuration-file parser, device assignment, UI widget routing or disk save
is invented. The required 49A action2255B0 and other unresolved helper effects
remain mandatory external bindings. No new call to them is silently stubbed.

## Branch provenance and verification

This checkout was extracted only from the verified 49B archive
`RacerRevenge-49B-WorldIndex-20261006.zip`, SHA256
`4d68b1e4134d9920cd22998aa73ef3526ad830820deea3af63e665239b2c65f0`.
No unverified 49C source or data was copied. The separate 49C work remains frozen
and unreleased. The prior collision contract remains unresolved and unchanged.

Run `python3 tools/gameplay/control_presets_recover49d.py --check` and
`python3 tools/test_native_control_presets 49d.py --output-dir /new/external/path`.
Add `--sanitizers --no-leak-check --stack-usage` for ASan/UBSan on this host.
The focused CMake option is `RR_NATIVE_CONTROL_PRESETS_ONLY`; CMake is absent
here, so the direct existing-GCC runner is the executable validation route.
Windows/Xbox targets and hardware were not built or run. LeakSanitizer is omitted
because this host uses restricted tracing; no leak-check or FPS claim is made.

The eight-suite normal and ASan/UBSan runs pass 6,286,844 checks each. The new
suite contributes 414,597 checks and 3,088 integrated frames, exercises all 326
selected original instruction words, and allocates zero across 20,000 hot
iterations. Independent review rejects 35 observable scalar mutations; one
equivalent redundant prewrite removal is separately excluded. Final test, review,
sensitivity and footprint results are recorded in `evidence/controlPresets49D/validation.json` and the checkpoint note.
