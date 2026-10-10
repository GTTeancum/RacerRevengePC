# 48O native detector preparation checks

## Scope

`tests/native/spring_detector48o.cpp` tests
`prepareSpringDetector48O` against the separately executed original detector
instructions. The test supplies caller-owned points, indexed scene caches,
endpoint vectors, staged vectors, metadata, scratch, and transactional output.
It does not execute registration, candidate enumeration, collision callbacks,
world traversal, a live detector tick, or any console/FPGA workload.

The original-instruction provenance, range hashes, scalar MIN/MAX correction,
and exceptional-mode limits are documented separately in
`docs/spring-detector-proof48o.md` and `docs/spring-detector-map48o.md`.
Historical 48N implementation and fixtures remain independent and unchanged.
A staged source query tag of **3** is checked explicitly; this seam is not an
adapter that silently treats those pairs as the 48N static-candidate type 2.

## Frozen differential corpus

The executable requires exactly **288** cases with input/output widths
**250 / 160** from `evidence/springDetector48O/fixtures.json`. It validates the
exact original boundary distribution **19 / 44 / 26 / 128 / 71**:

1. No spring: natural original return, no detector/spring work.
2. Query absent or disabled: completed preceding numeric work and optional reset.
3. No pairs and initial query type 3: immediately before registration.
4. Pairs with an explicit candidate list: immediately before limited query.
5. Pairs without an explicit list: immediately before world query dispatch.

The expected JSON SHA-256 is
`672ddbe30d865630eef4926b93ca696abd026d2a5e6c1d2cf8d245307b5cfd24`.
The corresponding 473,492-byte binary SHA-256 is
`642a4fc189eb199a383d4dd73681baf9ad7afaa77a5daff99985d2bed9ce6082`.
The serial runner fingerprints the actual JSON consumed by the test and the
binary evidence. The executable does not manufacture expected values from its
native implementation.

Each case compares every word of the complete 160-word original projection:

- Center and half extents, including both raw W lanes.
- All eleven spring state words, including reset fields and preserved count,
  capacity, and material values.
- All eight backing endpoint Vec4s, including stale first endpoints and unused
  slots. Query-disabled first endpoints are not eagerly overwritten.
- Query type, all eight staged Vec4s, and every metadata word, including tails.
- All sixteen raw world-matrix words and flags for each scene cache.

The native projection additionally checks every scene-local matrix and parent
flag for unchanged copying. Source point/scene/vector/metadata arrays and all
input descriptors are snapshotted byte-for-byte. Destination slots beyond the
source projection and guards beyond declared scratch/destination capacities
remain unchanged. The compact typed API does not expose every byte of the
original 96-byte point record; the original fixture generator independently
checks all original record words for preservation.

Corpus coverage includes prepared and dirty parentless scenes, eight scene
flag combinations, principal entry/reset/query gates, independent point and
shape counts, null shapes, zero shape count, and initialized stale slots copied
when the shape count exceeds the prepared point count. Ordinary finite random
cases and explicit signed-zero/adjacent-word cases compare raw bits. The eight
explicit opposite-zero cases use the corrected signed-word scalar
MIN/MAX semantics. No exceptional physical EE/VU arithmetic guarantee follows
from this finite host-binary32 comparison.

## Native safety and transactional checks

These checks test the native contract rather than claiming additional
original-instruction fixtures. Scratch is permitted to change on rejection;
all public destination buffers and output state must remain unchanged. Every
rejection snapshots immutable inputs and complete public destinations.

All eight variable writable spans are exercised: scene caches, pairs, staged
vectors, and metadata in both scratch and destination.

- Null pointers with nonzero declared extents.
- Misalignment, multiplication overflow, and address-range wrap.
- Insufficient scene/vector/metadata projection capacity in either output set.
- Independent point, pair-source, staging-source, and metadata-source capacity.
- Doubled shape-count overflow and out-of-range active scene indices.
- All 28 writable-span pairs, at their starts and at declared tails.
- Every writable span against each of the five complete source spans, both at
  source starts/tails and with a source beginning only in the writable span's
  unused tail.
- Every writable span against each of four descriptors and against result state.
- Result state against all five source spans and all four descriptors.
- Additional padded-descriptor probes isolate each descriptor inside an unused
  writable tail, so a neighboring descriptor cannot accidentally explain the
  rejection.
- Declared writable-span validation even on a no-spring semantic bypass.

The source-capacity-overflow probes preserve the implementation's error
precedence: if an enormous source projection also exceeds ordinary destination
capacity, `InsufficientCapacity` is a valid early transactional rejection. A
point-span overflow, which has no projected point destination, reports
`InvalidInput`. Separately isolated span/address tests cover invalid ranges.

Semantic guards reject nonpositive signed active pair counts, active positive-
count provider work, dirty parented scene refresh, and enabled no-pair type-2
preparation. The latter belongs to the separate query-preparation API. Finite
arithmetic checks reject late nonfinite inputs, transform overflow, bounds
addition overflow, and bounds subtraction overflow.

The late-transaction probe refreshes an earlier dirty root cache and prepares
an endpoint in scratch, then encounters a nonfinite later scene. It verifies
that none of those earlier valid changes leak into public outputs. Staging
capacity failures likewise occur after numeric preparation without a partial
commit.

## Separately labeled positive native wiring

The final test includes twelve positive native-wiring cases, separate from the
288 original differential cases:

- Multiple point records referencing one dirty root by the same scene index;
  all use the refreshed cache, and upper flag bytes are preserved.
- Prepared scenes with opaque parents and nonfinite unused local/W payloads;
  consumed XYZ remains finite and copied cache words remain raw.
- Unused point/scene slots with invalid payloads, which are projected unchanged.
- Disabled query preserving old first endpoints while reset still runs.
- No spring bypassing unused semantics and projecting existing values unchanged.
- Null shape still changing source query type to 3 without copying staged data.
- Zero and negative signed counts with no pairs: no point iteration or provider
  call, even when both provider gates are set; preserved center/half and count.
- Exact declared output capacities and null/zero extents.
- A 1,024-point, 1,024-shape-pair call using caller-owned storage and one shared
  prepared scene, proving there is no fixed four-point fixture cap.

The shared-scene probes are deliberately labeled native wiring. The original
compact corpus uses independent per-record scenes and does not independently
establish duplicate-scene-pointer execution.

## Allocation and host-cost observations

All calls into the native seam, including errors, the large variable-capacity
case, and the serial benchmark, run with test-only ordinary and aligned C++
`new`/`new[]` allocation instrumentation enabled. Setup, fixture parsing,
snapshots, and diagnostics are outside those measured scopes. Source inspection
is also necessary: this hook does not independently intercept arbitrary direct
`malloc` calls. The native seam contains no allocation or thread creation.

The benchmark reuses caller storage for seven consecutive batches of 20,000
calls. It reports median and maximum batch-mean host nanoseconds, completion
count, a result-dependent checksum, and allocation count. One of three active
points refreshes a dirty parentless cache on each call because immutable input
is reused. There are four scene slots, eight backing pair vectors, eight staged
vectors, and a shape count of three. This is a serial component measurement,
not an end-to-end gameplay benchmark or a console/FPGA FPS estimate.

Observed Linux x86-64 ABI sizes:

- Point: 20 bytes; scene: 136 bytes.
- Source arrays: 912 bytes for the benchmark backing.
- Scratch arrays: 1,176 bytes; destination arrays: 1,176 bytes.
- Input: 96 bytes; source descriptor: 80 bytes.
- Each buffer descriptor: 64 bytes; output state: 80 bytes.

Those buffer figures include intentionally larger declared destination/scratch
tails. They do not measure compiler stack frames or total process memory. The
runtime has fixed stack temporaries; variable storage is caller-owned.

## Reproduction and validation records

The focused runner includes the detector and its independently tested query
preparation helper:

```sh
CPU=$(python3 -c 'import os; print(min(os.sched_getaffinity(0)))')
taskset -c "$CPU" python3 tools/test_native_detector48o.py --stack-usage
taskset -c "$CPU" python3 tools/test_native_detector48o.py --sanitizers --no-leak-check
```

These Linux commands choose one permitted CPU and use fresh output directories
outside the source tree. The sanitizer command explicitly disables only
LeakSanitizer; AddressSanitizer and UndefinedBehaviorSanitizer remain required.
Run the two commands serially. `cmake/NativeDetector48O.cmake` supplies the
equivalent focused CMake
targets and enforces strict floating-point options and serial test execution.
No renderer, package download, gameplay assets, guest RAM, or user computer is
required for the focused native checks.

## Verified final results

On 2026-10-06, the release and ASan/UBSan focused suites both passed against the
same final test source, SHA-256
`379ec7d4ffbe2d1b77da022e3b605cd715ffe28475e4f73cc01b0a0fd0d30a7a`.
Both reports have no changed inputs or errors. Every compilation and test
stderr log is empty.

Both detector runs reported:

- 288 original differential cases; 62,885 checks; zero failures.
- 345 safety cases, including 257 alias probes.
- Twelve separately labeled native-wiring cases.
- 140,000 benchmark calls; checksum 1,399,930,000.
- Zero observed native-call allocations.

The same focused suites also passed the helper's 84 original staging and 248
original sphere cases, 20,446 checks, and zero allocations.

The release build used GCC 14.2.0, C++17, `-O2`, `-Wall -Wextra -Wpedantic`,
`-fno-fast-math -frounding-math -ffp-contract=off`, and `-fstack-usage`. Its
observed median cost was **271.47 ns/call** and maximum batch mean was
**354.322 ns/call**. The sanitizer build used `-O1 -g`, address/undefined
sanitizers, frame pointers, and no sanitizer recovery; its diagnostic benchmark
was 1,844.44 ns median and 1,872.76 ns maximum batch mean. Sanitizer overhead
makes these separate observations, not interchangeable performance estimates.
LeakSanitizer was explicitly disabled in that run.

GCC's release stack-usage file reports a 656-byte static frame for
`prepareSpringDetector48O` and a separate 32-byte static commit-lambda frame.
These are per-function compiler estimates, not a measured peak stack bound;
nested helper frames and caller storage are additional.

The verified runner report hashes are:

- `evidence/nativeDetector48O-corrected-linux-20261006/detector-release/report.json`:
  `494fe5a67de4a1b0264e8fc3d7c3145daeea1fd2dc2734c238ccf2b3dfce69a5`.
- `evidence/nativeDetector48O-corrected-linux-20261006/detector-asan-ubsan/report.json`:
  `540fe0bce4770b44277bc0cfa80ee4fed79fc43162c97bda6526ba999a49033b`.

## Mutation sensitivity

A separate run compiled eight deliberately changed copies of the detector
implementation against the unchanged final test and original oracle. All eight
executables returned failure, with these detected-check counts:

- Historical float-comparison MIN/MAX: 36.
- Eager first-endpoint update before the query gate: 359.
- Omitted spring reset: 1,585.
- Incorrect half-extents W: 233.
- Omitted dirty-scene prepared flag: 236.
- Point count substituted for independent shape count: 1,812.
- Local point W reused as transformed W: 1,107.
- Missing first-endpoint fill: 4,060.

The sensitivity report records the same test-source hash, the unmodified base
runtime hash, and a separate hash for each changed source. These probes confirm
that the suite notices these specific semantic regressions; they are not an
exhaustive mutation proof.

## Process-local initializer boundary

The original detector also contains process-local initializer side effects.
With a spring present, a zero flag byte at `gp-0x7360` initializes the word at
`0x443590` to `0xFFFFFFFF` and then sets the flag byte to 1. After numeric
preparation, a zero byte at `gp-0x735C` similarly initializes `0x443598` and sets
its flag byte. A missing spring returns before either initialization. These
original effects are described in the map and exercised by the separate
`natural-static-initializers` original boundary probe.

The native API projects detector state, spring state, scene caches, pairs,
staged vectors, and metadata only. It does not expose, reproduce, or maintain
those guest GP-relative flags and global sentinel words. Therefore the
160-word differential comparison is explicitly a bounded object-state
projection, not a proof of all process-wide detector side effects. It also
does not establish spatial registration or the persistent ownership/lifetime
of a live detector.
