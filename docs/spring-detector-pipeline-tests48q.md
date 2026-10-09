# 48Q supplied-list spring detector pipeline tests

## Scope and evidence boundary

`tests/native/spring_detector_pipeline48q.cpp` tests the bounded composition of detector preparation, type-3 staging, supplied-list query and static spring callback. Expected numerical results come only from the separately generated original-instruction fixture in `evidence/springDetectorPipeline48Q/native-fixtures.json`. The original run starts at 295630, rather than starting at the 48O preparation boundary or substituting native results into the interpreter.

The native test loads initialized inputs and complete expected observations separately. Test-only JSON parsing, vectors, strings and snapshots are not production ownership. Existing 48O and 48P test scaffolding informs the guards and parsers but none of those files or archived proof is edited.

Supported completion must keep the detector's always-return-1 observation distinct from the supplied query's hit/miss observation. No-spring and disabled-query cases are explicit control-flow outcomes. Point count and shape count are independent. Source-backed slots, stale tails, scene cache flags, matrix W payloads, endpoint W lanes, zero staging metadata, spring state and all record bytes are checked.

Candidate-local working vectors compare XYZ, the public native representation. All game-facing Vec4 values compare every raw binary32 word, including signed zero. Local matrices, world caches and cache flags are compared without normalization.

## Atomicity and alias contract

Synthetic negative cases complement original differential evidence. Every failing call snapshots all immutable inputs and nested backing, every public destination buffer and the complete result. Detector work that succeeds before a later query failure must not leak into those public destinations. Caller-owned scratch is permitted to change on error.

Tests enumerate all declared mutable spans, including unused capacity, against each other and against input descriptors, detector arrays, ray arrays, records, candidate lists, nested filters, mesh vectors and scene names. Additional cases cover nulls, misalignment, size overflow, address wrap, exact and insufficient capacities, reached invalid tree/leaf/vertex references and cycles. Unsupported provider, compound, normal/interpolation, dynamic callback and dirty-parent paths are rejected transactionally. Registration and world-query boundaries are separately named successful prefix commits; they preserve the completed detector prefix and explicitly leave detectorReturned=false.

Explicit callback binding to a detector scene must read its newly prepared world cache. Tests compare that binding to an independently cached fresh matrix and distinguish it from the old stale matrix.

## Allocation and cost observations

Ordinary, array and aligned global allocation hooks are active only around the runtime API. Fixture construction, JSON parsing, snapshots and reporting remain outside the measured scope. The benchmark reuses bounded caller buffers serially, reports host batch means and exact caller-owned byte budgets, and makes no Xbox FPS, rendered race, live binding, PC access, RAM, installation or production detector-ownership claim.

Compiler `-fstack-usage` outputs report separate function-frame estimates, not a measured or summed maximum live stack. Sanitizer runs, when recorded below, disable LeakSanitizer explicitly; they do not establish a leak-check pass.

## Verification

Release and sanitizer evidence, exact scope classifications and reproduction commands are recorded below.

## Verified results (2026-10-06)

The frozen corpus contains 170 cases and eleven meshes, SHA-256 `f349aeef2a7260e485e1cd30120b897e676a505b562d37d003846f8535fa1edb`. Native tests classify all of them:

- 138 complete ordinary-finite supplied-list queries
- Eight no-spring and eleven query-disabled completions
- Two committed prefix boundaries: registration and world query, neither claiming detector return
- Seven original-only scope stops: three provider, one dirty parent, one compound, one higher normal helper and one absent query shape
- Four explicit `InvalidInput` rejections for original shape-count-zero IEEE probes, whose original preparation creates an infinite radius

The last four are retained diagnostic evidence and are not represented as ordinary-finite native compatibility. The independent point/shape-count cases include callback iteration beyond the active shape count, where initialized stale hit flags, lengths and metadata cause authentic additional records.

GCC 14.2.0 release and AddressSanitizer/UndefinedBehaviorSanitizer builds each passed 690,699 checks, zero failures and zero guarded runtime allocations. Runs used one CPU core, strict binary32 options `-fno-fast-math -ffp-contract=off`, and `ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1` for sanitizer execution. This is not a LeakSanitizer result.

The successful original comparisons observe 114 hit queries, 198 candidate geometry hits, 197 callbacks, 274 record appends and 64 saturated append attempts. Native-only safety adds 3,863 rejecting cases, including 3,510 alias cases, and fourteen successful wiring probes. All 1,081 unordered pairs among the 47 writable spans are tested. Every writable kind is also tested against input descriptors and nested backing, including unused allocated filter/mesh capacity. Those full nested capacities are snapshotted for immutability. Fixture maximums are eighteen tree-stack entries, twelve record slots and three ordered candidate occurrences.

Every full original case deliberately supplies contradictory template `query.spring`, `query.pairCount`, `query.resetSpring` and `query.entry` values. The detector's state, independent shape count, already-performed reset and supplied-list dispatch must win. A separate successful probe starts with an existing record, disables detector reset, enables template query reset, and confirms appending preserves the existing record and count.

The final warning-free release benchmark observed 6,165.54 ns median batch mean and 7,172.68 ns highest batch mean over seven batches of 4,000 calls. This is a serial host observation and varies by run. With two points, two shape pairs and two candidates, the caller-owned workspace backing is 5,088 bytes: two 1,176-byte detector buffer sets, two 1,336-byte query buffer sets and a 64-byte tree stack. Final destinations require another 2,512 bytes. On this 64-bit host the result is 328 bytes, workspace descriptor 384 bytes, source descriptor 184 bytes and input descriptor 568 bytes. Immutable inputs/meshes and test-only outer guards are additional.

Release `-fstack-usage` reports separate runtime frames: 1,216 bytes (`dynamic,bounded`) for the composed pipeline, 656 for detector preparation, 1,760 for the borrowed-context query, 432 for ray preparation, 576 for the triangle leaf and 640 for the callback. These separate compiler estimates must not be added and presented as measured maximum live stack.

The native representation retains point-local Vec4 values and scene indices rather than reconstructing guest pointers or opaque point-record words. Original full point records and untouched guest bytes remain in the independent original execution evidence. Native spring output records compare every byte of every initialized 96-byte backing slot.

## Standalone reproduction

From the repository root, use an external output directory and one permitted CPU. No installation or downloaded dependency is needed.

```sh
mkdir -p ../pipeline48q-release
taskset -c 0 g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
  -fno-fast-math -ffp-contract=off -fstack-usage \
  -Isrc/game -Isrc -Ithird_party/native-json \
  tests/native/spring_detector_pipeline48q.cpp \
  src/game/spring_detector_pipeline48q.cpp src/game/spring_detector48o.cpp \
  src/game/query_prepare48o.cpp src/game/type3_query48p.cpp \
  src/game/type3_prepare48p.cpp src/game/collision_type3_node48p.cpp \
  src/game/collision_type3_leaf48p.cpp src/game/spring_type3_callback48p.cpp \
  src/game/spring_callback48n.cpp src/game/spring_records48n.cpp \
  -o ../pipeline48q-release/spring_detector_pipeline48q
taskset -c 0 ../pipeline48q-release/spring_detector_pipeline48q .
```

For sanitizer verification use a separate output directory, replace `-O2` with `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer`, and prefix execution with the sanitizer options given above. The root `tools/test_native_detector_pipeline48q.py` runner integrates this composed test and the borrowed-context seam test; retained component suites have their own runners.

Build logs, runtime logs, source/fixture fingerprints, stack reports and compiled-control reproduction live under `evidence/springDetectorPipeline48Q/native-pipeline-tests48q/`. The sensitivity runner accepts the absolute repository root and an external output path, compiles unchanged objects once and compiles every mutation serially. Only normal assertion-failure exits after successful compilation count as detection; crashes or setup failures do not.


## Compiled mutation sensitivity

The exact passing baseline was rebuilt from unchanged objects before these seven external pipeline-source mutations. Every mutation compiled successfully and exited normally with failed test assertions; none was credited for a crash, missing file or build error:

- Use the old source body cache for bound callbacks: 926 failed checks
- Apply reset from the unrelated query template: 2,400 failed checks
- Substitute spring point count for independent shape count: 615 failed checks
- Query the old pre-stage endpoint backing: 50,041 failed checks
- Query old pre-stage metadata: 176 failed checks
- Derive detector return value from query hit: 47 failed checks
- Commit detector destinations before a late query error: 140 failed checks

Before/after SHA-256 files establish unchanged production sources, public headers, test source and frozen original fixture. Reproduction from any checkout path:

```sh
python3 evidence/springDetectorPipeline48Q/native-pipeline-tests48q/run_sensitivity.py \
  "$PWD" /absolute/external/pipeline48q-sensitivity
```

The runner selects one permitted CPU, compiles serially, reuses unchanged objects and records its full commands. Archived results and mutation sources are under the neighboring `sensitivity/` directory.
