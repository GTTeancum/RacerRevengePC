# Submission allocation observation 50EH

Status: source-only; not compiled, tested or executed. No existing renderer,
fixture, build registry or frozen `terrain_offscreen_drive50dh.cpp` was changed.

`tests/native/render_allocation_stats50eh.h` provides
`rr::test::RenderAllocationStats50EH`, a caller-owned collector with exactly
160 in-object records. `observe`, `summary` and `frame` use no heap storage,
logging, clocks, GL calls or allocation-hook replacement. Recording and summary
access take constant work per frame. It is single-threaded and non-atomic.

## Next separately authorized fixture integration

Construct the collector before the frame loop. Copy the existing counter just
before submission, copy it again immediately afterward, then call `observe`:

```cpp
rr::test::RenderAllocationStats50EH submissionAllocations;
// Inside each frame, with no logging/assertions between these snapshots:
const std::uint64_t before = ::allocations;
const auto result = presentVehicleOffscreen50DE(/* existing arguments */);
const std::uint64_t after = ::allocations;
const auto observation = submissionAllocations.observe(before, after);
// Inspect result/observation and do diagnostic output only after observation.
```

The call above is placement pseudocode, not a replacement fixture. Keep the
original checked-native allocation guard separate. Do not insert warm-up frames
or discard the first measured submission. After the loop, serialize
`summary().firstFrame`, `all` and `afterFirst`, with the validity/overflow fields;
`frame(i)` exposes each recorded delta for `i < summary().all.frames`.

- `all.frames` counts chronological recorded submissions, including invalid
  observations. `validFrames` counts known deltas; their `sum`, `maximum` and
  `nonzeroFrames` exclude invalid observations. No samples is distinct from a
  valid zero. The invalid count is `frames - validFrames`.
- `firstFrame.valid` must be checked before using its allocation count.
  `afterFirst` excludes chronological frame zero only; it means the observed
  post-first-frame range, not proven steady state or completed driver warm-up.
- A counter decrease records an invalid sample and returns `counterDecreased`.
  Supply snapshots from the same uninterrupted monotonic uint64 counter; the
  helper cannot detect a full counter cycle or a reset that overtakes its old
  value. Never treat an invalid sample's stored zero as an allocation result.
- Sums saturate at `UINT64_MAX` with sticky `sumOverflow`. More than 160 calls
  return `capacityExhausted`, preserve the first 160 samples/totals, and set
  `capacityOverflow`; `rejectedFrames` saturates with its own overflow flag.
  Report partial coverage explicitly. No ring-buffer overwrite is performed.

This records counter events, not bytes or allocation ownership. The current
thread-local C++ counter does not measure other threads or all C/driver memory.
First-frame versus later counts can locate when activity happens, but cannot
classify those allocations as application-owned or driver-owned.

## Bounded source inspection

The reviewed 50DE presenter, 50CR `batchInput`, 50CO/50CM preparation and
50DD/50CS/50CV submission paths use retained arrays, caller workspaces and
descriptor copies. 50CR vector growth and 50DD wrapper `new` expressions occur
in cold load/create paths. No obvious per-frame application allocation was found
in these inspected paths; that is source inspection, not an allocation proof.
EGL/GL calls remain inside the observed submission interval. The reported
23,945 events across the preceding 160-frame run remain unattributed until new
measurement provides evidence. This addition consumes no runtime test slot.
