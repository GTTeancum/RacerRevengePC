# Native input shaping and quantization: 48V-InputShaping

2026-10-06. Independent cloud/Linux numerical milestone after the preserved 48U
checkpoint. It does not resolve the separate collision buffer-capacity contract
or compose the grouped query with the detector. Those areas are unchanged.

## New production behavior

`src/game/control_shaping48v.h/.cpp` supplies a typed, serial, allocation-free
shaper and composes it with the corrected `stepControlRateFilter48B`. This closes
the previously missing nonlinear/quantization prefix; it is not merely another
rate-limiter test. No guest registers, address-space memory, instruction dispatch,
original executable, resource archive, or emulation dependency enters production.

The caller supplies four numerical channels corresponding to object
`+B5C/B60/B64/B68`, previous `+D28`, the selection corresponding to `+58`, previous
filtered `+B6C/B70/B74/B78`, X/Y rates `+B94/+B98`, and entry `f12` elapsed factor.
The result contains all four quantized targets, updated `+D28`, and filtered
channels. This makes the newly recovered persistent state explicit.

The contiguous original order is:

1. If object `+58` is zero, independently shape every channel with the original
   degree-10 cosine polynomial at `float32(PI_2 * channel)`. For channel < +0,
   use `cosine - 1`; otherwise use `1 - cosine`. Both signed zeros take the
   nonnegative comparison branch. Nonzero `+58` bypasses nonlinear shaping.
2. Update `+D28` as separately rounded `0.1f * shapedChannel0`, `0.9f * oldD28`,
   then their sum. This happens before quantization and does not use elapsed time.
3. For each channel, multiply by 100, add +0.5, truncate toward zero to int32,
   convert to float, then multiply by binary32 0.01. This rule is asymmetric for
   negative values. It is not round-to-nearest or an invented symmetric deadzone.
4. Apply the existing X/Y/X/Y rate limiter. Only this stage uses elapsed factor.

For example under round-to-nearest, nonlinear raw -0.5 produces approximately
-0.28 after quantization, while +0.5 produces approximately +0.29. Raw -1 maps to
binary32 `BF7D70A3` (approximately -0.99); +1 maps to +1. Exact endpoint shaping
slightly exceeds magnitude 1 before quantization because the original polynomial
at PI/2 returns `B5000000`, not mathematically exact zero. Replacing the helper
with the host cosine would change the original numerical model.

## Exact recovery and reference boundary

Only current cloud ELF `work/native/project/disc/SLUS_202.68` was read. Its SHA-256
is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
`tools/gameplay/input_shaping_recover48v.py` extracts and hashes:

- 249 caller words in `[0x217DE0,0x2181C4)`, including the 70 existing limiter words
- 64 words in cosine leaf `[0x13BBA0,0x13BCA0)`

The frozen header is `tools/gameplay/input_shaping_words48v.h`; complete
disassembly and block hashes are under `evidence/inputShaping48V/`. Running the extractor with
`--check` verifies them against the ELF without changing files. It reads no RAM,
PODPHYS resource, historical generated source, or other live-machine data.

The test-only oracle independently decodes those words, original register use,
synthetic object fields, delay slots, calls, comparisons, accumulator and stores.
It does not call production formulas or production MIN/MAX helpers. Its memory
is a bounded synthetic fixture, never reconstructed game RAM. The numerical
reference is the project's strict, separately rounded IEEE binary32 operation
convention across four host rounding modes. It does not certify all physical EE
underflow, denormal, extended-range, exception flags, or hardware rounding quirks.
Subnormal probes test the declared host reference convention only.

CVT.W.S uses truncation toward zero independently of the host rounding mode;
MULA/MADD use separate binary32 products and addition. These instruction semantics
are consistent with the project's existing oracle convention and the
[PCSX2 FPU implementation](https://github.com/PCSX2/pcsx2/blob/master/pcsx2/FPU.cpp).
The original words establish the selected instructions and constants; this is
not a claim that an emulator source proves every hardware edge case.

The original prologue lazily initializes globals `4346B8=0.5` and `4346C0=0.25`,
using flag bytes at `gp-78C4` and `gp-78C0`. Neither global is consumed by the
selected numerical interval. The oracle exercises initialized/uninitialized
branches and verifies those writes. The production API intentionally has no
process-global initialization state; later consumers of those values remain
outside its output contract. Prologue register saves likewise are not gameplay
outputs. In the cosine helper, branches reflecting arguments beyond +/-PI/2 are
frozen in the oracle but unreachable for the admitted raw input range.

## Domain, storage and errors

The native API requires raw channels in [-1,1], finite previous +D28 and a known
typed mode. This is a conservative API domain gate, not evidence that upstream
input code clamps that range. The limiter retains its finite/nonnegative
rates/elapsed and finite-intermediate gate. Invalid input or any late limiter
failure leaves the entire output untouched. Inputs may alias result members;
local staging precedes the only output assignment.

No allocation, threads or growing containers are used in production. Result
structures are 20 bytes for shaping and 36 bytes for the combined step on the
verified host. Caller state is reusable. Compiler frame/object measurements are
host observations, not an Xbox peak-memory or frame-time guarantee.

## Proven and missing connections

`FUN_001A1A20` copies input-component `+24` floats to the four vehicle channels,
then performs queries/overrides, as recorded by the prior simulation contract.
48V consumes the resulting numerical fields. It does not assert which physical
button, axis, action name, input configuration, deadzone or inversion supplies
any channel. `+58 == 0` versus nonzero is the proven numerical branch condition;
its higher-level mode identity and selection policy remain unresolved. No input
binding or deadzone is invented from the observed quantization behavior.

The 48D engine API expects two drives at model `+68/+6C`, plus mode flags. The 48E
aero API expects `+E4/+E8/+EC`, triggers and steering mode. Later portions of
`FUN_00217DE0` perform additional mode-dependent mapping. None of the four 48V
outputs is wired directly to those engine/aero fields in this milestone.

The adjacent shaping -> quantization -> limiter order is proven. Prior 48D/48E
work establishes engine generation -> gravity -> conditional engine application
-> aero -> body core in its bounded route, with some aero/torque flags written
later for a subsequent update. The input call's full frequency and relationship
to that scheduler, pause/substep policy, action state and later mapping are not
established here. Therefore this API stops before a complete vehicle tick.

## Reproduce

Run from the repository with the installed GCC-compatible C++17 compiler:

```
python3 tools/gameplay/input_shaping_recover48v.py --check
python3 tools/test_native_input_shaping48v.py --output-dir ../input48v-release --stack-usage
python3 tools/test_native_input_shaping48v.py --output-dir ../input48v-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_input_shaping_sensitivity48v.py --output-dir ../input48v-sensitivity
```

Output directories must be new/empty and outside the source tree. The focused
CMake alternative is `-DRR_NATIVE_INPUT_SHAPING_ONLY=ON`; its sanitizer option is
`RR_NATIVE_INPUT_SHAPING_SANITIZERS`. CMake is not installed on this cloud host,
so its new configuration is source-reviewed but unexecuted. No install was made.
Windows and Xbox builds/execution, renderer wiring, full races and gameplay FPS
remain untested. LeakSanitizer is explicitly excluded due to the existing host
ptrace limitation; ASan and UBSan remain enabled with no silent fallback.

Final counts, source hashes, review findings and archive verification are recorded
in the checkpoint and `evidence/inputShaping48V/validation.json`. Earlier evidence,
original files and the failed development golden-vector assertion are preserved.
