# Native input-to-model binding: 48W-ControlBinding

2026-10-06. Independent cloud/Linux continuation of preserved 48V-InputShaping.
This milestone adds actual missing numerical connections to the existing engine
and aerodynamic input APIs. It does not resolve or change the separate collision
buffer-capacity contract, grouped-query guards or detector composition.

## Delivered behavior

`src/game/control_binding48w.h/.cpp` implements the remaining numerical portion
of original `FUN_00217DE0`, `[0x2181C4,0x218898)`, immediately following 48V.
Its typed interface produces engine `+68/+6C` and aero `+E4/+E8/+EC/+D8` inputs,
and separately retains original writes/preservation of aero `+50/+F0/+F4`.

`stepAndBindControlInputs48W` composes raw numerical inputs -> original nonlinear
shaping -> +D28 smoothing -> asymmetric quantization -> rate limiter -> original
mode/state mapping. It derives both shaping and mapping selection from the same
supplied `mode58Nonzero`. The original source's order is retained without inventing
how often this function is called by a vehicle scheduler.

`applyControlModelBindings48W` writes only the proven fields into `EngineInput48D`
and `AeroInput48E`, preserving all other producer inputs. This adapter closes the
previous missing numeric field connection. It does not call force producers or
pretend to supply a complete vehicle tick.

## Exact branch contract

Inputs include the filtered `+B6C/B70/B74/B78`, cached body speed `+34`, vehicle
state `+40`, selector `+58`, a caller-resolved configuration word's nonzero test,
`+B7C/+B80` float action values, byte `+B9C`, source `+7D0/+7D4`, runtime steering
coefficients and prior model fields. All field names retain offsets where a
higher-level identity is unknown.

1. States `+40 == 3` or `4` clear engine `+68/+6C`, aero `+E4/+E8` and `+D8`.
   Aero `+EC` and `+50` remain unchanged, rather than being silently reset.
2. Otherwise, when `+58 == 0` and the selected configuration word is nonzero:
   - Engine `+68` receives MAX(-channel1, +0), and `+6C` receives
     MAX(-channel3, +0). States 1/2 multiply both by the supplied global scale.
   - Both channels at least binary32 0.9, outside states 1/2, set both aero
     fields and `+EC` to 1.
   - Otherwise aero `+E4/+E8` receive MAX(channel1/3, +0). States 1/2 scale both;
     state 1 adds the global bias to `+E8`, state 2 adds it to `+E4`. `+EC` is zero.
   - Aero `+50` is preserved.
3. All other non-3/4 cases use channel0 multiplied by the original speed-based
   steering interpolation. Equal speed endpoints produce scale +0. Distinct
   endpoints retain original subtraction/multiplication/division/addition order
   and endpoint-direction-sensitive clipping. State1 applies scale and positive
   bias; state 2 applies scale and negative bias. Steering is limited to [-1,+1]
   using original signed-word MIN/MAX.
   - Nonzero `+B7C`, outside states 1/2, starts both engine drives at 1, reduces
     the corresponding engine on a steering turn, and sets aero fields from
     the steering sign. Nonzero `+B80` then sets `+EC=1` and floors aero fields
     at 0.5.
   - Otherwise nonzero `+B80`, outside states 1/2, sets engine drives to 0 and
     `+EC=1`, with the original asymmetric steering-dependent aero values.
   - Otherwise engine drives and `+EC` are 0; aero values retain signed steering
     split. This includes states 1/2 regardless of the supplied action values.
   - Aero `+50` is written from channel1 on this route.
4. Every non-3/4 route sets aero `+D8` to exactly `(byteB9C >> 2) & 1`.
   Original DSLL32 then DSRL32 is a logical extraction, yielding 0/1, not 0/-1.
5. Every route finally copies `+7D4 -> aero+F0`, then `+7D0 -> aero+F4`.

Channel2 (`+B74`) is not consumed by this tail. No omitted input is silently
mapped to throttle, brake, a controller axis or a physical button.

## Proven identity and unresolved inputs

The hash-verified ELF establishes a useful part of mode identity: constructor
`0x1A02C0` installs vtable `3CED50`, whose descriptor points to the literal
`PodPlayerVehicle`, and writes `+58=0` at `0x1A0304`. Thus nonlinear shaping and
eligibility for the configuration-selected route are the player-class constructor
default. This does not prove all later writes or that every nonzero value means AI.

Base construction initializes `+40=0` and `+58/+5C=-1`. The selected word is read
from the object reached through global `382B80`, then pointer `+2DC`, then
`+11040 + 4*vehicle(+5C)`. Its index originates from a different creation argument
than input-device index `+FFC`. They must not be equated. No complete state enum,
menu option name, permissible index range or live selection ownership is proven.
The native API therefore receives the resolved condition directly and does not
recreate a global pointer tree.

The prologue's effective global scale/bias default to 0.5/0.25, established in
48V. This API accepts their effective values: use those defaults after the
original first-use initialization, or already-resolved values for initialized
state. It does not own original process-global initialization flags. Connected
oracle tests select the initialized route, allowing those effective values to
vary independently; 48V's retained tests separately cover first-use initialization.

The complete existing 48E aero producer does not consume model `+50/+F0/+F4`.
The apparent +50/+F0 accesses inside its source are stack fields, while its flow
helper reads only aero+40..4F. These three outputs are preserved separately without
inventing force effects. This is limited to that producer, not an assertion that
the entire original game never uses them.

Physical button/action bindings, deadzone, inversion, normalization ownership,
full selection/state meanings, engine/aero scheduling and body-cache refresh
remain outside this proof. Existing engine/aero inputs not written by this
function still require their independently established state and timing.

## Recovery and numerical fidelity

Source ELF SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

`tools/gameplay/control_binding_recover48w.py` freezes 437 original words and
separate bounded provenance bytes. `--check` verifies the header, disassembly,
manifest and constructor/selector/consumer evidence directly against the ELF.
No RAM, resource archive, generated translation or live machine was used.
The independent provenance audit and frozen blocks are under
`evidence/controlBinding48W/`.

The test oracle executes original instructions with bounded synthetic object,
model and settings data. It retains branch delay slots, original comparisons,
64-bit logical extraction and exact stores. Initial `v1=3` at this tail's entry
comes from the preceding original `0x2181B8`; isolated tests set it explicitly,
while connected tests obtain it by executing 48V's original prefix. Expected
values do not call production mapping, shaping or MIN/MAX helpers.

The reference remains the project's separately rounded, strict IEEE binary32
convention in four host rounding modes. It is not a certification of every
physical EE denormal/overflow/exception mode. Finite input/intermediate gates
are native API requirements, not claims about original gameplay validation.

A real optimized-build issue was exposed under FE_DOWNWARD: after saturation,
GCC folded `1 + (-1)` / `1 - 1` to +0 even with the existing strict flags.
The native arithmetic now uses volatile operands as well as volatile results.
Explicit +/-1 cancellation goldens preserve the required -0 for downward
rounding. Removing those barriers is a detected semantic mutation.

## Storage, ownership and failure

Production is serial and allocation-free. Mapping results occupy 36 bytes and
connected input/binding results 72 bytes on the verified Linux x64 host. Prior
fields are explicit so partial original writes preserve state correctly.
All outputs commit only after validation and computation succeed, including
late mapping errors after the 48V stage. Natural same-object aliases are tested;
failed aliases preserve both input and output bytes.

The typed engine/aero adapter stages both complete structures before either
output assignment, updates only six proven fields, and rejects nonfinite mapped
floats without changing either output. Other producer state remains caller-owned.

The GCC object has 4,968 text bytes, zero data/BSS and no allocation imports.
Compiler frame estimates are 248 bytes for mapping, 24 for the adapter and 176 for
composition. Nested calls and caller-owned state are additional. These are host
observations, not target performance or a whole-frame memory ceiling.

## Validation and reproduction

Final release and ASan/UBSan runs each pass 1,523,953 checks:

- New 48W: 579,627 checks; 52,747 exact-word cases, including 12,000 connected prefixes
- All 437 binding words visited; 7,646,062 original instructions per run
- Retained 48V: 812,697 checks; retained 48B limiter: 131,629
- Four rounding modes, interpolation directions/equality, saturation neighbors,
  state/action combinations, persistent transitions, typed adapter preservation
- Aliases, rejected inputs, late-stage atomic failure, zero hot allocations
- All 16 candidate-only semantic mutations rejected by ordinary test assertions
- Independent source/provenance review and an independent normal test rerun

```
python3 tools/gameplay/control_binding_recover48w.py --check
python3 tools/test_native_control_binding48w.py --output-dir ../binding48w-release --stack-usage
python3 tools/test_native_control_binding48w.py --output-dir ../binding48w-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_control_binding_sensitivity48w.py --output-dir ../binding48w-sensitivity
```

Directories must be new/empty and outside the source tree. Focused CMake option:
`RR_NATIVE_CONTROL_BINDING_ONLY=ON`; sanitizer option:
`RR_NATIVE_CONTROL_BINDING_SANITIZERS=ON`. CMake is absent and its configuration
was not executed. No installation was performed. ASan/UBSan passed; LeakSanitizer
is explicitly excluded because of the existing host ptrace limitation.
Windows/Xbox builds, renderer/game-loop integration, full gameplay and FPS remain
untested. No PC access, push or publication occurred. 48V source/evidence and the
original files are preserved. Collision work remains unchanged and unresolved.
