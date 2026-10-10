# Optional development-client phase timing 50GK

`tests/native/phase_timing50gk_fixture.h` is a bounded, caller-owned host
diagnostic. It does not alter native simulation time, physics, lap progression,
render completion policy, or sleep policy. The helper performs no heap
allocation, logging, locking, or background work. Linux observations pair
`CLOCK_MONOTONIC` elapsed nanoseconds with `CLOCK_THREAD_CPUTIME_ID` CPU
nanoseconds on the calling thread. Other platforms retain monotonic wall
observations and explicitly flag thread-CPU observations as unavailable.

## Reason for measuring

The newly built 50GJ live drive counted its first represented lap. Before the
lap callback, two ordinary telemetry intervals reported 1.24557 and 0.583777
GPU-completed FPS near the reported 3413/3409 position component, zone 8 and map
target 81. The latter interval is genuinely below one completed frame per
second. These coarse observations do not identify a slow phase or establish a
root cause. In particular, a small calling-thread CPU duration relative to wall
time does not prove a VM pause: GPU completion, software-renderer workers,
kernel waits, other threads, and scheduling are also possible contributors.

## API and interval semantics

```cpp
#include "phase_timing50gk_fixture.h"
namespace timing50GK = phase_timing50GK;
// Cold setup: choose enabled once, before the loop and capture lambda.
timing50GK::Interval phaseTiming{enabled}; // Default constructor is disabled.

{
    timing50GK::Scope phase(phaseTiming, timing50GK::Phase::Physics);
    // Existing work, including its existing allocation guard.
} // Reads the ending clocks and adds one completed observation.
```

The phases are `Input`, `Physics`, `RenderPrepareSubmit`, `CompletionWait`,
`Capture`, and optional `HostSleep`. Storage is a fixed `totals[phaseCount]`
array indexed by the enum. `phaseName(phase)` returns a static printable name.
`Scope::finish()` explicitly closes a scope, is idempotent, and permits
boundaries without changing existing variable lifetimes. Destruction closes
early-exit paths too. Scopes cannot be copied or moved.

Each `Totals` contains:

- `calls`: ended scopes, including observations with a failed clock
- `wallCalls`, `cpuCalls`: valid sample-pair counts for each clock independently
- `wallNs`, `cpuNs`: sums from the corresponding valid sample pairs
- `maxWallNs`: maximum valid wall delta
- `cpuAtMaxWallNs`, `cpuAtMaxWallValid`: the CPU delta of that exact maximum-wall
  observation, rather than an unrelated maximum CPU observation
- `clockFailure`: sticky failure/unavailability or a backwards clock reading
- `overflow`: sticky integer-counter/sum saturation

All times are integer nanoseconds. Maximum-wall ties retain the first
observation. With no valid wall observations, `maxWallNs` is unavailable even
though its storage is zero. A false `cpuAtMaxWallValid` means unavailable CPU,
not zero CPU. Totals include only valid clock pairs; never divide by `calls`
as if failed observations were zero-time successes. Clock reads are sequential,
not atomic; tiny CPU/wall discrepancies include clock-read overhead and skew.

`phaseTiming.resetInterval()` performs fixed-size assignments and preserves
`enabled`. Only reset or change configuration with no active scope. Keep all
sampling, reading and resetting on the one owner thread; there is deliberately
no synchronization. Disabled scope construction/destruction performs no clock
reads or aggregate updates, just bounded local initialization and the disabled
branch. This is minimal overhead, not an unconditional zero-instruction claim.

## Suggested 50DU hook boundaries

The client integration is owned separately. The following anchors refer to
`tests/native/terrain_interactive_drive50du.cpp`; use statement anchors rather
than fixed line numbers as the client changes. Create the interval during cold
setup, before `captureCommitted` and `for(;;)`, with opt-in parsed once. An
environment option can require an exact `1`; never query the environment in
the loop. Keep the existing allocation checks around the actual native work.

1. **Input:** start at the beginning of `for(;;)`, before `host.poll`:

   ```cpp
   timing50GK::Scope inputTiming(phaseTiming, timing50GK::Phase::Input);
   ```

   End immediately after normal-path `sampleController()` and before
   `platform::FixedStepPlan50DT plan{}`:

   ```cpp
   inputTiming.finish();
   ```

   Also end it before `captureCommitted()` in the first `if(recoveryPaused)`
   branch and before the 8 ms sleep in `if(paused)`. Quit/restart/checkpoint
   early exits close by RAII. The input phase then includes polling, packet
   composition, sampling, and host input decisions, without counting the
   deliberate sleep or retained-frame capture as input.

2. **Physics:** one scope for each `advanceFrame()`, within the existing
   allocation-counter boundary. For example:

   ```cpp
   const auto nativeFrameAllocations=::allocations;
   timing50GK::Scope physicsTiming(phaseTiming, timing50GK::Phase::Physics);
   const bool frameAdvanced=advanceFrame();
   physicsTiming.finish();
   // Existing require50DU(::allocations==nativeFrameAllocations, ...) follows.
   ```

   This includes the native frame's joined navigation/lap callback when one
   actually executes. It does not add calls or change native ordering. Four
   catch-up steps produce four physics observations, not one rendered frame.

3. **RenderPrepareSubmit:** start inside the existing conditional render block,
   before `const auto bank=...`:

   ```cpp
   timing50GK::Scope renderTiming(phaseTiming, timing50GK::Phase::RenderPrepareSubmit);
   ```

   Call `renderTiming.finish()` immediately after the successful
   `renderer.submit(...)` block, before `if(headlessLive&&capturePending)`.
   Includes batch preparation, camera, cables, HUD and submission. It excludes
   explicit completion and capture. Submission itself can still block or do
   driver/software-renderer work; the label does not imply queue-only cost.

4. **CompletionWait:** bracket only the existing
   `renderer.finishSubmittedWork(completionDiagnostic)` call with this scope:

   ```cpp
   timing50GK::Scope completionTiming(phaseTiming, timing50GK::Phase::CompletionWait);
   ```

   Use a separate block inside `if(headlessLive)` so destruction precedes
   publishing CPU-bank ownership. On the window path, bracket the existing
   `host.swapWindow(hostDiagnostic)` call separately using the same phase.
   This preserves the current policy: no new finish, fence, swap, or wait is
   introduced. Calls count actual timed operations, not uniquely identified
   frames; a host doing both operations would contribute two observations.

5. **Capture:** put a scope at the top of
   `if(headlessLive&&capturePending)` around visibility inspection and the
   JSON receipt, then another in `captureCommitted` after its early-return
   guard. Both use:

   ```cpp
   timing50GK::Scope captureTiming(phaseTiming, timing50GK::Phase::Capture);
   ```

   The lambda scope should outlive its existing `std::ofstream` so final file
   close/flush is included. This observes existing diagnostic allocation and
   I/O without adding either to the helper or blaming physics. A captured
   frame can contribute two capture calls; no-request lambda returns should
   contribute none. Existing capture-only logging is part of that work.

6. **HostSleep (optional):** use a local block and `Phase::HostSleep` around
   each existing `std::this_thread::sleep_for`, including both pause sleeps
   and the active-loop capped sleep. End input before pause sleep and leave
   all existing durations and predicates unchanged. The measured wall time
   includes sleep overshoot and scheduling delay.

Avoid nesting these scopes. Explicit `.finish()` at transitions keeps their
totals mutually exclusive. Work outside their boundaries, especially ordinary
telemetry output and host-loop bookkeeping, remains unclassified.

## Five-second reporting

Emit one line per phase in the existing
`if(afterWork-lastStatusNs>=5000000000ull)` status block, after the usual drive
state and outside the physics allocation guard and all timing scopes. Include
the enable state, each phase name, call/valid counts, total wall/CPU, maximum
wall and its associated CPU/valid flag, clock failure and overflow. Then call
`resetInterval()` at that same boundary. Formatting is client-owned; the helper
contains no streams, strings, JSON, or callbacks. Avoid per-frame output and
additional telemetry threads.

Report nanoseconds or clearly labeled milliseconds; retain valid flags with
any derived means or ratios. Disabled timing should not print misleading zero
measurements. Ordinary five-second telemetry is a cadence threshold, not an
exact five-second window: a stall or paused-loop branch can delay reporting.
The status position/node describes the end of the interval, not necessarily
the location of its slowest phase observation.

Compare total phase wall times with the actual `interval_s` and inspect
`maxWallNs`/`cpuAtMaxWallNs` for the same observation. High CPU suggests work on
the measured main thread; low CPU leaves waits, worker work, preemption and
host scheduling among the possibilities. A large unclassified wall residual
needs its own boundary investigation. Do not infer whole-process CPU, GPU
execution time, worker utilization, VM suspension, or a lap-callback cause
from these measurements alone. Simultaneous whole-process or host observations
would be needed to distinguish some of those possibilities.

## Focused verification contract

`Totals::record(before, after)` accepts supplied `Sample` values for narrow,
deterministic checks of sums, maxima/CPU pairing, tied maxima, one-clock failure,
backwards readings, saturation, and reset behavior. `Sample` aggregate field
order is `wallNs, cpuNs, wallValid, cpuValid`. A disabled scope should leave all
totals unchanged. Repeated `finish()` must record only once. An enabled Linux
scope should produce valid wall/thread-CPU pairs without C++ heap allocation.

This helper/document change has only been inspected; no build, test suite,
guest execution, or gameplay run is claimed here. It neither modifies nor
executes the excluded 49C/48U paths or `ee.bin`. The integrating owner can run a
single focused follow-up with timing enabled and record the actual outcome;
these hooks themselves do not establish the cause of the observed stalls.
