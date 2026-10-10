# Authored player input: 48X-PlayerInput

2026-10-06. Cloud/Linux continuation of preserved 48W-ControlBinding.

## What is now implemented

`src/game/player_input48x.h/.cpp` adds the original player component conditioning
and action-reader behavior ahead of the recovered shaping/filter/binding stages:

1. Ready DualShock packet-axis bytes -> original normalized range/deadband
2. Component values -> original ordered action-code overrides and flag updates
3. Resulting numerical controls -> shaping, quantization and rate filtering
4. Original state/configuration mapping -> existing engine/aero input fields

The connected API takes an explicit ready sample and an already-bound,
observation-only action-query provider. It is a working native dataflow over
supplied device inputs. Device transport, connection negotiation, OS/controller
integration, actual event semantics and the complete gameplay scheduler remain
separate work. Nothing here claims a playable game or measured gameplay FPS.

## Proven component and packet mapping

Original player creation constructs `+FE0/FE4/FE8/FEC` with axis IDs `0/1/2/5`.
All four have minimum -1, maximum +1, initial value/center +0, and deadband radius
bits `3E4CCCCD` (0.20000000298023224). The upper/lower edges are center plus/minus
that radius. The values are original instruction immediates, not inferred from
an absent configuration resource.

The original branch named by its diagnostic as DualShock-compatible assigns
packet offsets `6/7/4/5` to those axis IDs. The update converts each byte to float;
axis2 alone uses `255 - byte` before conditioning. The denominator is exactly 255.
Other packet layouts and the device handshake are not accepted implicitly by the
ready-DualShock adapter. The original read-failure path supplies neutral values
127,127,128,127 after that inversion, which all fall inside this deadband; this
milestone does not implement the connection/read-failure state machine.

The component producer performs these separately rounded operations:

- `q = numerator / denominator`
- `value = minimum + q * (maximum - minimum)`
- At or above the upper edge, rescale `(value - upper)` by
  `(maximum - center) / (maximum - upper)`, then add center
- At or below the lower edge, rescale `(value - lower)` by
  `(center - minimum) / (lower - minimum)`, then add center
- Strictly between those edges, return center

There is no invented output clamp. The general native numeric function requires
finite ordered range/deadband parameters, positive denominator and finite
intermediates; these safety gates are not claims about original validation.

Registration additionally stores approximately 0.01 into component `+1C/+20`.
Those values do not enter this producer and are not its 0.2 deadband. The original
configuration key `JoyDirectionFlipped` exists and its low bit is stored, but its
consumption by this path is unproven. Only the observed unconditional axis2
inversion is implemented. No left/right-stick or X/Y names are inferred merely
from packet positions.

## Exact player reader and action queries

The reader copies component `+24` into vehicle `+B5C/B60/B64/B68`, then:

- Query code 15 with mask 0x3F; a nonzero match overrides channel 0 to -1
- Only if code 15 does not match, query code 13; a match overrides channel 0 to +1
- Query selected configuration slot 694 into `+B7C`, and slot 690 into `+B80`,
  writing exact +1 or +0; skip a query only when its configured code is -1
- Evaluate the original state gate: bit 1 must be set and bit 0 clear
- If enabled, query slots 698,69C,6B4 in order to update flag bits 0,1,2 respectively
- Bit1 additionally requires at least one of `+7D4/+7D0/+7DC/+7D8` to be positive,
  in that original comparison order
- If the gate is disabled, clear only those low three bits; all other bits survive

The gate byte is `+58` on the object reached through global 382B80 -> +28C -> +B4.
It is distinct from vehicle `+58`, which selects nonlinear shaping and mapping.
Slot 698/69C/6B4 negative codes, including -1, are still queried. Applying the
694/690 sentinel rule to them would change the original sequence.

The callback receives the exact signed numeric code and mask 0x3F and returns
whether the original virtual query would return nonzero. It does not assert
whether that match means held, pressed, released or another event condition.
There are at most seven queries. Their order and short circuits are observable;
repeated codes are not cached or merged. An unavailable or invalid callback
result fails atomically for the output, but observations already made cannot
be rolled back. The provider must not mutate supplied gameplay state and is
responsible for its own bounded/allocation-free implementation.

## Device selection and remaining physical identities

Player creation receives a device index into `+FFC` and obtains the device pointer
from the original input manager's `+10 + 4*index` array, storing it at `+FF0`.
A missing pointer takes the original joystick-creation failure route. The reader
uses that device and the same `+FFC` to select a settings row with stride 40 bytes
and fields beginning at offset 690. Native callers supply that already-resolved
provider and row. Device-array bounds/ownership and selection policy remain a
caller responsibility, not a reconstructed global-memory subsystem.

The separate vehicle configuration index `+5C` used by 48W is not interchangeable
with `+FFC`. Device-type IDs, axis IDs, allocated component runtime IDs and numeric
action codes are also distinct identities.

Physical button names and complete action-layout names remain unresolved. The
current extracted disc directory contains the ELF, but no controller-layout/UI
resource archive or original SDK/header enum definitions. Those original layout
resources or independently verified enum/binding evidence would be needed before
assigning physical names. No such source was fetched from the PC or RAM.

## Endpoint compatibility with preserved 48V

The authored deadband arithmetic can produce `3F800001` (1+1 ULP) at an endpoint
under upward rounding. Its negative envelope is likewise bounded by `BF800001`.
All 256 byte values are checked in all four host rounding modes. Clamping to +/-1
would silently change the original arithmetic, while the conservative 48V API
correctly rejects inputs outside its declared [-1,1] domain.

The new player shaping entry admits only this proven one-ULP extension. Ordinary
values still delegate to unchanged 48V. At the extended endpoints it retains the
original cosine helper's +/-PI/2 reflection, exact coefficients and operation
order before smoothing/quantization. Both reflection directions are compared to
the already-frozen original helper. No broader controller range is assumed.

The connected frame consistently replaces binding actions, flags and source
`+7D0/+7D4` with those of this player read. It preserves the previous filter,
+D28 and model state through explicit caller inputs. It does not insert engine
or aerodynamic force calls into an unproven scheduler.

## Evidence and numerical boundary

The ELF SHA-256 remains
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

New exact-word oracle blocks:
- 37 words: numeric component body `[0x127DA4,0x127E38)`
- 235 words: player reader `[0x1A1A20,0x1A1DCC)`, before restoration epilogue
- 12 words: complete state-gate helper `[0x1B0B80,0x1B0BB0)`

The axis diagnostic/assertion prologue is outside the numeric API; the proven
component types are defined. The reader oracle uses bounded synthetic memory,
original loads/stores, register operations, delay slots and branch-likely
annulment. Its direct gate call executes original words. Virtual device-query
outcomes are explicitly supplied test inputs, not claimed original device/event
execution. Production code never interprets instructions or guest memory.

A separate audit recovered constructor, registration, packet update and device
configuration provenance from original ELF bytes. Seven bounded blocks and named
strings are frozen by `player_input_provenance48x.py`. The report and source hashes
are under `evidence/playerInput48X/`. No RAM, resource retrieval, generated
translation, PC access, installation, push or collision diagnostic was used.

Floating-point comparisons use the project's strict separately rounded host
binary32 convention across four rounding modes. This is not a full physical EE
exception/denormal/extended-range certification. Volatile operands and strict
compiler flags preserve the known downward-rounding cancellation requirements.

## Runtime and verification

Core production is serial and allocation-free, with fixed caller-owned values.
The complete result is 116 bytes on the verified Linux x64 host. The new object
has 6,701 text bytes and zero data/BSS. GCC frame estimates are 120 bytes for generic
axis conditioning, 96 for packet conditioning, 112 for the reader, 232 for extended
shaping and 304 for composition. Nested frames, the provider and caller state are
additional. These observations are not Xbox memory limits or FPS measurements.

Final counts and hashes are in `evidence/playerInput48X/validation.json` and the
checkpoint. Coverage includes all 284 new words, all byte endpoints, explicit
threshold signed zeros, every gate byte, query traces and sentinels, state/field
preservation, invalid providers, aliases, late failures, reflected endpoints and
zero allocations over 20,000 connected native steps. An independent strict-FP +
UBSan harness passed 923,702 assertions over 131,072 reader cases and 2,048 mixed
endpoint shaping cases. All 19 candidate-only semantic controls are detected.

```
python3 tools/gameplay/player_input_recover48x.py --check
python3 tools/gameplay/player_input_provenance48x.py --check
python3 tools/test_native_player_input48x.py --output-dir ../player48x-release --stack-usage
python3 tools/test_native_player_input48x.py --output-dir ../player48x-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_player_input_sensitivity48x.py --output-dir ../player48x-sensitivity
```

Use new/empty output directories outside the source tree. The focused CMake
option is `RR_NATIVE_PLAYER_INPUT_ONLY`, with `RR_NATIVE_PLAYER_INPUT_SANITIZERS`.
CMake is absent and was not executed. Normal and ASan/UBSan runs retain all 48W,
48V and 48B input regressions. LeakSanitizer is explicitly excluded due to the
existing host ptrace restriction. Windows/Xbox execution, OS/game-loop input,
complete force scheduling, rendering, races and gameplay FPS remain untested.
Collision capacity remains unresolved; existing grouped-query/detector files
and all 48V/48W source/evidence are unchanged.
