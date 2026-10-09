# 49J: original counter quantization and connected timing

## Production closure

`src/game/clock_quantization49j.{h,cpp}` now closes the numeric dependency left
explicit in 49I. The caller supplies a 16-bit timer count and the captured pending-
overflow word. New functions reproduce the original accumulator, integer/float
conversion and saturating timestamp, then compose directly with 49I begin timing.
There is no host OS clock, guessed timer epoch, new catch-up policy or hardware I/O.

- ClockCounterState49J: accumulated uint64 ticks, pending uint32 overflows and
  previous uint16 count; 16 bytes on the validated host. Padding is not an interface.
- ClockQuantization49J: three uint32 words recording rounded tick-float bits,
  scaled millisecond-float bits and the truncated/saturated timestamp; 12 bytes.
- ClockFrameOutput49J: wrapped increment bits, quantization and FrameElapsed49I;
  24 bytes. Together with the retained 28-byte frame state, the caller-owned
  counter/frame/output storage is 68 bytes.

All production operations are serial, fixed-storage and allocation-free. Original
program words and their interpreter exist only in tests. The pending word must be
provided under a data-race-free caller contract. Actual COUNT/interrupt acquisition
and its ordering/races are not implemented or claimed atomic.

## Supplied sample recurrence

The original main sampler140B30..140BC0 and its initialization duplicate use the
same numeric behavior. The source read order is current count, previous count,
pending overflow word; the pending word is then cleared. For supplied count c,
previous p and loaded full 32-bit pending n:

- If c >= p: d = uint32((n << 16) + (c-p)).
- If c < p: substitute 1 only when the entire loaded n is zero; then
  d = uint32((nEffective << 16) - (p-c)).
- Explicitly sign-extend d from 32 to 64 bits and add modulo 2^64 to accumulated ticks.
- Commit the new previous count before timestamp conversion, at the original
  conversion-call delay slot.

Neither a widened unsigned increment nor a masked 16-bit zero test is equivalent.
For example c=0,p=1,n=0 contributes +65535, but n=0x80000000 is nonzero while its
shifted low word is zero, giving d=FFFFFFFF and a signed contribution of −1.
Equal samples with pending 0 stall; they do not imply a wrap. These unusual cases
are retained as typed scalar semantics, not presented as measured hardware races.

`consumeTimerSample49J` implements this recurrence for every typed input pattern,
using unsigned arithmetic to avoid signed C++ overflow. Native source order is
expressed, but final-state tests do not certify machine-store or interrupt visibility.

## Exact counter-to-timestamp conversion

The closure contains the original unsigned64 wrapper 2EAE10, signed helper 30E4E8,
integer-to-double, double multiply/add/pack, double-to-single narrowing, the caller
multiply, and 310F38/3105A8 truncation/unpack. No numeric callee is replaced by a
stand-in in the original-word oracle.

For positive signed-range n, 30E4E8 sets bit 11 when n>=2^53 and any lower 11 bits are
nonzero, without clearing those lower bits. Its exact counter-domain behavior is:
RN32(RN64(adjusted integer)). For zero/high-bit unsigned inputs, 2EAE10 first uses
(n >> 1)|(n&1), calls that helper, then doubles the binary32 result exactly.
All double operations in this domain are finite/zero; only the final high+low
integer sum needs double rounding. The native code preserves these two rounding
stages with bounded integer operations, rather than a host double cast.

This is **not** direct nearest-even uint64-to-float conversion. Original-word
execution confirms counter 9007200865353727 returns 5A000002; a direct cast under
nearest-even returns 5A000001. The source adjustment does not remove every double-
rounding midpoint difference. This rare discrepancy is already beyond final
uint32 timestamp saturation, so no ordinary gameplay timestamp difference is
claimed from this particular counterexample.

The rounded tick float is multiplied, in binary32, by bits 3AE38E39 (approximately
1/576). The positive finite result is truncated and saturated to uint32. Below 1
it becomes 0; at/above 2^32 it becomes FFFFFFFF. The source subnormal/exceptional
converter branches are retained in evidence but are not reached by this counter
pipeline. The output is not a wrapping ideal millisecond count.

`quantizeCounter49J` requires host FE_TONEAREST for that one rounded binary32
multiply; unsupported rounding leaves output unchanged. The integer conversion
stages do not depend on the host rounding mode. `beginFrameFromTimerSample49J`
composes consume → quantize → beginFrameTiming49I and rejects unsupported rounding
before any caller state/output changes, including clearing pending overflows.
This native admission behavior is not a new original game branch.

## Precision and saturation implications

The nominal 576000-tick/second interpretation was source-mapped and separately
corroborated in 49I. Tick-float precision begins coarsening beyond 2^24 ticks, about
29.1 nominal seconds. Millisecond truncation initially hides most small errors;
absolute-time float granularity becomes increasingly visible over longer runs.
These are supplied-counter scenarios, not measured game sessions or proof that
an original process never resets its timer state.

The first saturating source counter is exactly 2,473,901,031,424 under the stated
float model. Immediately before it:

| Counter | Tick float bits | Scaled float bits | Timestamp |
|---:|---|---|---|
| 2,473,901,031,423 | 540FFFFF | 4F7FFFFE | FFFFFE00 |
| 2,473,901,031,424 | 54100000 | 4F800000 | FFFFFFFF |

The scaled float jumps 512ms to 2^32; the stored uint32 timestamp jumps 511ms due to
saturation. 49I caps that elapsed value, then receives zero elapsed while the
absolute timestamp stays saturated. The boundary is near 49.7 nominal days. No
clock “repair,” fractional bank or ideal OS clock is substituted.

## Connected input and camera diagnostics

`tests/native/clock_timing_diagnostics49j.cpp` executes 13 scenarios and 915 paired
frames, then repeats them. Each pair has identical supplied count progression,
input channels, pose and stage observations. The comparison side uses TEST-ONLY
floor(accumulatedTicks/576), saturated to uint32, to reveal quantization effects;
it is not an alternative production clock or an OS timer implementation.

The source side runs the new producer, 49I timing, recovered 48V/48B input behavior
and 49F TV view/projection/combined matrices. Original-word checks cover clock,
timing, input, TV state and matrices. Both normal and ASan/UBSan runs produce
identical 915-row, 181-column traces containing exact binary32 words.

Camera 16 comes from the retained authentic TA file: FOV 75/45, distance 1750/5,
authored TV probability 70. The diagnostic explicitly requests the already-selected
TV subsystem; it does not implement camera-type scheduling. Selection starts at
the authored camera position. Supplied resolved pose offset is [100,-20,600],
velocity [100,0,0], identity parent, one complete local scene entry, RNG seed 1 and
aspect fixture 1. These inputs are named fixtures, not inferred physics or a live
scene. The same pose is used on both sides, isolating elapsed-time effects.

Flags are captured immediately after the supplied manager stage and retained
across supplied pre-dispatch state changes. The real opaque actions are not
implemented; input/camera calls are diagnostic consumers, not pretended original
callbacks. Existing 49I leave/finish rules remain explicit.

Selected observed comparisons:

| Scenario | Frames | Source zero-elapsed frames | Frames with elapsed difference | Max FOV difference |
|---|---:|---:|---:|---:|
| Near 2^24 ticks | 128 | 2 | 2 | 0.023514 degrees |
| Near 2^33 ticks | 128 | 1 | 6 | 0.013489 degrees |
| Near 2^40 ticks, ordinary cap | 128 | 117 | 127 | 3.841583 degrees |
| Near 2^40 ticks, optional cap | 128 | 117 | 127 | 5.365417 degrees |
| Crossing saturation | 64 | 63 | 17 | 3.424614 degrees |

The 2^33 and 2^40 cases are about 4.14 hours and 22.09 days at the nominal frequency.
Short wraps, stalls and multi-wrap stalls also match the source. Artificial 64-bit
accumulator wrap, large pending signed-wrap and the helper double-rounding example
are separately labeled synthetic edges. The rare helper case has no timestamp,
input or FOV difference after saturation, as expected.

## Verification and limits

The frozen numeric closure has 809 words: 772 conversion/caller/saturation words
plus 37 sampler words. Native tests execute 682: all 646 reachable conversion words
and 36 reachable sampler words. The 127 excluded words are classified by source
counter-domain/control-flow proof, including unreachable 140B84 and exceptional
paths that cannot arise from finite counter-derived values. No unprovided RAM or
zero-fill exceptional constant is materialized.

The dedicated full conversion corpus passes 940,635 cases, with all three outputs
matching the independent model and 1,778,057,976 original instructions executed.
A 28,835-case ASan/UBSan corpus also passes. The separate supplied-sampler corpus
passes 13,739 cases normally and with sanitizers, executing 26,479,135 instructions
and checking one supplied COUNT read plus all source writes in every case.

The native producer fixture passes 856,514 checks normally, or 856,517 with explicit
coverage export; connected diagnostics pass 275,578 normally, or 275,582 with trace
output. There are zero hot-call allocations. Independent review detects 18 safe
wrong scalar/state variants and independently reproduces the complete traces.
The admitted numerical model is strict nearest-even host binary32 plus exact
software helper behavior, not universal R5900 FPU flags/denormals/cycles or silicon
conformance. LeakSanitizer is unavailable under ptrace and explicitly disabled.

Host object text is 1234 bytes with zero data/BSS. Compiler static frame estimates
are 8 bytes for consume, 48 for quantize and 128 for the composed prefix; nested
calls and caller storage are additional. These are component estimates, not
measured peak stack, whole-game memory, Xbox budget or FPS results.

## Reproduce and preserve

All thirteen final suites pass normally and with ASan/UBSan: 9,193,802 checks
per run, all 97 tracked inputs unchanged and identical test stdout. Check counts
are assertions rather than independent test cases.

Use `tools/test_native_clock_quantization49j.py` for all 13 suites, adding
`--sanitizers --no-leak-check` for ASan/UBSan. The clock test accepts
`--coverage-json NEW_PATH`. The connected test accepts
`TA_CAMERA_PATH [NEW_OUTPUT_PREFIX]` and writes `.trace.tsv` plus `.summary.json`.
Original source checks are:

```
python3 -B evidence/clockQuantization49J/conversion-source/extract_conversion49j.py --check
python3 -B evidence/clockQuantization49J/sampler-source/extract_sampler_words49j.py --check
```

The focused CMake option is RR_NATIVE_CLOCK_QUANTIZATION_ONLY. CMake is absent
here; direct GCC builds are the executed route. Exact commands, trace hashes and
source pins are retained under evidence/clockQuantization49J/.

This tree starts from verified 49I ZIP SHA256
1f4f28ad598bd8569eeba4b9e572c05e521b2bf981a7e38d87cf9c169317da0c.
All inherited files are preserved except intentional CMake/README integration.
49C remains excluded and unreleased; collision/action-reset limits remain open.
No PC/RAM access, installs, rendered-frame scheduling, full gameplay or target
performance claim is part of 49J.
