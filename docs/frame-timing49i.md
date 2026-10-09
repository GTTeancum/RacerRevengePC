# 49I: source-bounded frame timing and state

## Delivered production behavior

`src/game/frame_timing49i.{h,cpp}` adds a real elapsed-time producer for existing
native control/camera/simulation consumers. It accepts the original **already
quantized and saturating uint32 timestamp**, then implements the post-clock
numeric and state operations of `140AC0`. The API does not substitute a host OS
clock, choose a target frame rate, accumulate catch-up time, perform callbacks or
name an unproven bit a pause/focus setting.

The typed caller-owned state is 28 bytes: current230, previous234, raw250,
lastDispatched23C, phase22C, active11C and pending120. These are semantic fields,
not a claim that this compact struct is the original object layout. Every lane
must be initialized explicitly. The output contains an unsigned millisecond
difference and binary32 elapsedSeconds (8 bytes). All five operations are serial
and allocation-free; no guest interpreter, JSON or hardware access is production.

## Exact timestamp and clamp contract

Original source `140B10..140B24` tests previous234 for zero. In that branch the
clock-reset helper eventually writes the supplied timestamp to previous234 then
current230 (`140E38/140E3C`), yielding a zero difference. The zero sentinel is not
replaced by a separate initialized boolean: a timestamp of zero can cause the
bootstrap branch again on the next call. The normal sample writes current230.

`140BE4..140C8C` then computes:

1. Low32-bit wrapping difference current230 minus previous234.
2. Unsigned32-to-binary32 conversion. High-bit values take the original sticky-
   half signed-conversion path, then a separately rounded doubling operation.
3. Binary32 multiplication by bits `3A83126F` (approximately 0.001).
4. Store this **uncapped** result to raw250, including the original branch-delay
   effect at `140C50`.
5. Upper cap at bits `3D75C28F` (0.05999999865889549).
6. If the supplied global423ED8 word is nonzero, apply a second upper cap at bits
   `3D08850A` (0.033330000936985016).

The optional cap does not force every frame to 0.03333. For example, a 17ms delta
keeps its smaller result. A 60ms delta produces raw bits `3D75C290`, one ULP above
the ordinary cap, before being capped to `3D75C28F`. Integer precision boundaries,
unsigned high-bit cases, wrapping differences and saturated timestamps are retained.
The provider itself saturates; wrapping the subtraction does not imply that the
original absolute timestamp is an ideal wrapping millisecond counter.

`beginFrameTiming49I` performs these operations and returns the prepared factor.
Unsupported host rounding mode is a native admission error: it leaves state and
output untouched before any source-like writes. This rejection is not an original
in-game validation branch. The admitted arithmetic model is strict nearest-even
binary32, with each required rounding step explicit.

## Correct placement around external calls

The original calls manager `128080` with the prepared capped factor at `140C90`
**before** reading root byte228 bit3. A cap-word change during the manager stage
therefore affects a later begin, not the factor already held in f20. The bit3
decision, in contrast, is captured after the manager.

There is an important intervening action: after that decision, the admitted
branch calls `245030` and conditionally global 442F70 virtual slot20. Those opaque
actions occur before the phase/last-factor stores. A correct future caller must:

1. Begin timing and pass the returned factor to the real manager stage.
2. Capture flags/bit3 immediately after the manager.
3. If the captured bit is clear, perform the original gated opaque pre-dispatch
   actions. They may change live state; do not reread flags afterward.
4. Call `enterFrameSimulation49I` with the **captured** flags and prepared factor.
   Ready writes phase22C=2 then lastDispatched23C=factor. Skipped preserves both
   and does not inspect the factor. The native ready path rejects unsupported
   factors outside finite [0, original 0.06], while preserving either zero sign.
5. On Ready, invoke the actual root callback, then `leaveFrameSimulation49I`
   clears phase22C. The app virtual28 and global26F910 calls remain external.
6. Invoke the original always-reached opaque 267BE0 stage, then
   `finishFrameTiming49I`. It reads the **live** current230 into previous234 and,
   for any nonzero live pending120, writes active11C=1 and pending120=0.

These APIs are staged numeric/state primitives, not a fabricated game-loop
orchestrator. Source instruction order is retained as a contract. Adjacent writes
without an intervening observation cannot be distinguished by final-state tests;
no machine-store ordering, interrupt observation or concurrency proof is claimed.

Within this source interval, skipped or capped frames still advance previous234
to the full current timestamp. No leftover-time bank is introduced here. This
local fact does not establish a global pause or catch-up policy for unknown
callbacks or other consumers of the original timing fields.

## Clock provenance and boundaries

The original Timer0 sample is unsigned 16-bit at `10000000`; previous sample is at
`3D61C0`, overflow counter at `435368`, and accumulated 64-bit ticks at `435370`.
Setup `239C40` selects clock mode 2 and enables counting/overflow handling;
handler `23A180` updates the overflow counter. Original CRT and constructor
writes establish initialization without observing or reconstructing live memory.

`2EAE10` converts accumulated unsigned 64-bit ticks through the signed 64-bit helper and
sticky-half path to binary32. Multiplication by bits `3AE38E39` approximates 1/576;
`310F38` then truncates and saturates to uint32. Thus absolute timestamp precision
is already reduced before subtraction, and large values saturate instead of
wrapping. This provider is documented but is not implemented or executed by 49I.
An arbitrary ideal `steady_clock` millisecond count is not a proven equivalent.

The original ELF proves the timer selector and arithmetic. Separately, the
primary open-source [PS2SDK timer definitions](https://ps2dev.github.io/ps2sdk/timer_8h_source.html)
and [timer implementation](https://github.com/ps2dev/ps2sdk/blob/master/ee/kernel/src/timer.c)
corroborate BUSCLK 147456000Hz divided by 256 for selector 2: 576000 ticks/second.
Consequently 1/576 yields milliseconds and the downstream 0.001 factor yields
seconds. This is hardware-reference corroboration, not a physical oscillator
measurement or a recovered Linux/Windows/Xbox clock adapter.

Global423ED8 starts zero by original initialization. The gated event path at
`140F78..140F8C` toggles it as 1−old when root bit3 is clear, bit2 is set, and the
original matcher accepts raw code 3C/type 0/mask 80. No physical key label is invented.
The bit3 constructor clear is proven; its enabling writer and semantic name remain
unrecovered. Its established effect is dispatch suppression, not a named pause mode.

Constructor zeros are explicit for current230, previous234, raw250,
lastDispatched23C, active11C and pending120. There is no general constructor
assignment to phase22C; singleton BSS initialization and later setup phase 1 are
separate facts, not a default for arbitrary constructed objects.

## Static route to the original scheduler

Original construction and registration close the previous missing route:

- PodGame slot30 uses the 17DE00 trampoline to 140AC0.
- Application+26C root GameObject has vtable 3CD240, slot38→141ED0.
- Application+10 is a second GameObject registered under that root with mask FFFFFFFF.
- PodRaceEvent constructor 1A65C0 installs vtable 3CEE80, slot38→1A7B40.
  Both creation paths register app+28C under app+10 with mask FFFFFFFF.

The two routers forward f12 unchanged. Their mask 2/child-flag gates and ordered
global 442F70 callback before each eligible child remain real dependencies. The
scheduler captures the unchanged factor; active app+11C can select child-only
traversal instead of physics. Its own child updates precede substep arithmetic.
This proves the original static route, not an observed live scene topology.

Existing substep policy remains trunc(float32(factor*liveScale))+1. Source initializes
liveScale434188 to 60; no other direct writer was found, but indirect alias exclusion
is incomplete. 49I does not hardcode that global as immutable or replace the scheduler
with one physics step per rendered frame. No substep planner or world callback is
silently fabricated in this milestone.

## Proof and native admission

The independent test-only interpreter freezes 69 original instructions directly
from the hash-verified ELF. It uses fixed registers, synthetic caller-owned fields,
branch-delay semantics and integer-significand nearest-even COP1 arithmetic. It
neither reuses production float formulas nor constructs a guest RAM image.
All 68 reachable words execute, with both arms of all seven conditional branches.
The retained 140C04 duplicate shift is unreachable from the selected entry and is
honestly unexecuted. Additional source-context blocks are retained for inspection,
not falsely counted as executed hardware/callback coverage.

Final standalone tests pass 727,613 checks (727,616 with explicit coverage export),
including 1,472,881 original timing instruction executions, 8,192 staged frames and
3,855 timing-to-48V/48B input frames matching 1,042,670 original input instructions.
Tests cover zero epoch, integer conversion ties, 60ms raw-before-cap, cap toggles,
captured flags across intervening state mutations, skipped factor non-consumption,
signed zero, ignored signaling-NaN lanes, atomic unsupported-rounding rejection
and live tail updates. All five production APIs have zero hot-call allocations.

Independent review catches all 28 meaningful wrong scalar/state variants. Three
independent-write swaps and the equivalent multiply-by-two variant correctly pass;
no artificial observation callback was added merely to kill equivalent mutants.
The proof is source-instruction behavior under the stated finite host model, not
universal R5900 FPU exception/denormal/cycle or silicon equivalence.

## Footprint, reproduction and preservation

Typed state 28 bytes, elapsed output 8 bytes. Optimized host GCC object text 569
bytes, zero data/BSS; static compiler frame estimate 64 bytes for begin and 8 bytes
for each other API. These exclude caller storage, nested library calls and platform
overhead, and are not measured whole-game/Xbox memory or FPS results.

All eleven final suites pass normally and with ASan/UBSan: 8,061,710 checks per
run, all 85 tracked inputs unchanged, identical test stdout and timing coverage.
Counts are assertions/instruction executions, not distinct independent cases.

Run `python3 -B tools/gameplay/frame_timing_recover49i.py --check` to verify the
frozen source against the original ELF. Run `tools/test_native_frame_timing49i.py`
for eleven focused timing/camera/input suites, or add `--sanitizers --no-leak-check`
for ASan/UBSan. The final timing binary accepts `--coverage-json NEW_PATH` to save
per-PC/per-branch evidence. LeakSanitizer is unavailable under ptrace and is not
claimed passed. CMake option `RR_NATIVE_FRAME_TIMING_ONLY` is provided; CMake is
absent here, so the direct installed-compiler runner is the executed build route.

This branch starts from verified 49H ZIP SHA256
`9f4bb6720cd4a73abd80a81b6ce7d1c424f06aa15eea6244d8537db030eec04c`.
All inherited files are preserved except the intentional CMake/README integration.
The separate 49C work remains excluded and unreleased. Existing collision and
action-reset limitations remain unresolved. No PC or RAM access, new installs,
opaque callback execution, rendered-frame scheduling, complete gameplay or target
performance claim is part of 49I.
