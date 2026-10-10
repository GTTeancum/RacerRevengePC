# 48P bounded native type-3 query tests

## Scope

`tests/native/type3_query48p.cpp` exercises `queryStaticType3Bounded48P` against the strict initialized original-instruction fixture in `evidence/type3Query48P/native-fixtures.json`. Full limited-list calls compose preparation, ordered filtering, type-3 dispatcher/tree/leaf processing, and the authentic static type-3 callback. Five separately counted direct dispatcher cases bypass list filtering and callbacks. Two begin from the original post-preparation snapshot; three explicitly initialize stale count/fraction fields at the direct dispatcher boundary after authentic preparation.

The tested native scope is positive pair counts with zero **consumed** endpoint-scene metadata, float-mesh candidate shape 0, and flat/mode-1 normal selection. Unused metadata tails retain explicit nonzero words. Reached compound shape 1 and actual executions of the unimplemented higher-mode normal helper are rejected transactionally. Other shape tags have the original no-hit/reset behavior. Higher normal modes that never execute that helper, including absent normal arrays or missed/rejected geometry, remain supported. The fixture's `expected.requires_mode2` records original execution rather than guessing support from the mode number.

Local working start/direction and center arrays compare XYZ only. Their original unused W register carry is deliberately not represented by the native API. All game-facing world vectors, selected vectors, normals, surface points, copied candidate/query vectors, and all 96-byte record slots compare every lane or byte. No approximate tolerances are used: comparisons retain negative-zero bit patterns, normal W=2, correction W=3, selected strict ties, all result resets, and stale active/unused array slots.

This is a caller-owned bounded numerical component test. It makes no PC/RAM retrieval, installation, production ownership, live binding, rendered-race, or console performance claim, and does not alter the selected 48O scalar MIN/MAX implementation.

The final frozen fixture has 245 cases and eleven meshes, SHA-256 `a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f`. Its scope split is 190 full limited-list completions, five direct completions, and 50 explicit scope rejections (44 endpoint-scene, five compound, one mode-2 helper). These classifications were confirmed by both native release and sanitizer executions.

## Differential observations

Each fixture case is executed and classified as a completed full query, a completed direct dispatcher projection, or an explicit original-only scope rejection. Original-only cases remain part of the evidence; they are not silently removed from the test corpus or reported as supported.

Completed calls compare:

- Complete represented query state: shape/world/query centers and radii, five selected Vec4s, selected fraction, face hit count, and opaque result words.
- All four source-backed ray slots, including stale local/world arrays, hit flags, fractions, normals, points, and lengths.
- Query flags/current identity and every ordered candidate occurrence's flags and copied vectors.
- All eleven spring-state words, all source-backed record bytes, and optional explicit after-reset state/records.
- Geometry-hit, callback, appended-record, and saturated-hit counters derived from original call and append events.
- Source/filter/mesh/name immutability and guard/tail preservation for every output and scratch array.

The test initializes direct dispatch from explicit `dispatch_initial` when present, otherwise `after_prepare`, never from unprepared endpoint state. The new boundary-seeded snapshots do not pretend authentic preparation produced the stale count/fraction values. For full calls it initializes solely from `expected.initial`; expected final state is used only for comparisons. Input records and endpoint/metadata arrays retain explicit fixture backing beyond active pair counts.

## Safety and bounded storage

A separate synthetic two-candidate fixture tests:

- Insufficient common ray, separate length, record, effect, endpoint, metadata, and tree-stack capacities.
- Null, misaligned, overflowed, and wrapping declared spans, including nested mesh and scene-name extents.
- All 276 pairs among 24 writable spans: scratch/destination ray arrays, records, effects, tree stack, and result.
- Every writable kind against each readable input descriptor and nested filter-list, mesh-vector, scene-name, endpoint, metadata, record, and source-ray span.
- Tail-only overlap, including otherwise-unused destination capacity.
- Invalid counts, nonfinite consumed arithmetic, malformed tree/leaf/vertex references, and late failures after an earlier candidate has written scratch.
- Transactional unsupported compound, callback, group, dynamic-category, material, normal-mode, and interpolation-boundary cases, plus invalid absent body scenes.
- Successful saturation, overfull logical counts, skipped/disabled callbacks, unhandled shape resets, unused scene metadata, and empty-list preparation/current-identity behavior.

Every rejected call checks that all original input, nested input backing, destination arrays/effects/records, and result bytes are unchanged. Scratch may change on failure, as the API allows. Declared unused tails and outer guards remain protected on completed differential calls.

## Allocation and serial host cost

Test-only global ordinary/array/aligned allocation hooks count allocations only while the native API or explicit reset is executing. JSON parsing, vector/string fixture construction, immutable snapshots, and diagnostics are outside those scopes.

The serial benchmark reuses one bounded fixture for seven batches of 4,000 complete calls. It reports the median batch mean, highest batch mean, completed/appended counts, and exact caller-owned ray/record/effect/stack byte budgets plus descriptor/result sizes. This is a host component observation rather than a gameplay or Xbox FPS claim. All guarded calls, including rejection paths, must perform zero instrumented allocations.

## Verified results

On 2026-10-06, GCC 14.2.0, strict binary32 (`-fno-fast-math -ffp-contract=off`), one CPU core:

- Release (`-O2`) and AddressSanitizer/UndefinedBehaviorSanitizer (`-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer`) both completed 341,118 checks with zero failures and zero guarded allocations.
- 190 complete limited-list cases, five direct cases, and all 50 original-only cases executed with the required status and transactional result.
- 154 supported queries with geometry hits, 260 candidate geometry hits, 258 callbacks, 268 appended records, and 39 saturated appends.
- 1,307 negative safety cases, including 1,135 explicit alias cases. Fixture maximums are 18 tree-stack entries, 12 record slots, and three ordered candidate occurrences.
- Sanitizer execution explicitly used `ASAN_OPTIONS=detect_leaks=0` and `UBSAN_OPTIONS=halt_on_error=1`; LeakSanitizer was not run, so this is not a leak-check pass.

The final warning-free release benchmark observed 2,531.94 ns median and 2,652 ns maximum batch mean per complete query; these timings vary by run. The benchmark's caller-owned scratch totals 1,400 bytes: 600 ray bytes, 576 record bytes, 160 effect bytes, and 64 tree-stack bytes. Destination buffers, immutable inputs/meshes, descriptors, and result are additional. On this 64-bit host, result metadata is 240 bytes, workspace and source descriptors are 136 bytes each, and query input is 464 bytes. These sizes are printed rather than assumed for a different target.

GCC release `-fstack-usage` reported separate runtime frames of 1,728 bytes for the bounded query, 432 for preparation, 576 for the triangle leaf, 40/128 for sphere/ray node predicates, and 640 for the type-3 callback. These are separate compiler estimates, not a measured or summed maximum live stack.

The 36 added original cases retain the first209 input/expected observations exactly and add rotated, reflected, nonuniformly scaled, and sheared candidate matrices; near/far orders; fraction-1 stale selected transforms; transformed body callbacks; distinct-normal equal ties; explicitly seeded direct inactive/sphere/tangency gates; and a deeper tree with multiple pairs.

Integrated logs, compiler stack reports, exact historical209 source/fixture bytes, both sensitivity result sets, and a reproducible mutation runner are retained under `evidence/type3Query48P/native-query-sensitivity/`. Its README gives standalone commands. The production runtime is unchanged by mutation checking.

## Standalone reproduction

From the repository root, create a fresh directory outside the checkout and compile serially. Replace the output-directory example as needed:

```sh
mkdir -p ../type3-query-release
taskset -c 0 g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
  -fno-fast-math -ffp-contract=off -fstack-usage \
  -Isrc/game -Isrc -Ithird_party/native-json \
  tests/native/type3_query48p.cpp src/game/type3_query48p.cpp \
  src/game/type3_prepare48p.cpp src/game/collision_type3_node48p.cpp \
  src/game/collision_type3_leaf48p.cpp src/game/spring_type3_callback48p.cpp \
  src/game/spring_callback48n.cpp src/game/spring_records48n.cpp \
  -o ../type3-query-release/type3_query48p
taskset -c 0 ../type3-query-release/type3_query48p .
```

For sanitizer verification, use a separate output directory, replace `-O2` with `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer`, and prefix execution with the sanitizer environment options above. `taskset -c 0` reflects this host's permitted affinity; use one permitted CPU on another host. All test construction and API execution remain serial.


## Mutation sensitivity (209-case checkpoint)

Temporary compiled copies outside the checkout were linked against the unchanged test and other runtime objects. Before/after SHA-256 fingerprints confirmed all production sources, headers, test, and original fixtures stayed unchanged. The passing baseline was rerun first. These six mutations were rejected:

- Remove the per-candidate `rays.fractions[i] = -1.f` reset: 1,192 failed checks.
- Remove the per-candidate `rays.hits[i] = 0` reset: 4,604 failed checks.
- Change selected replacement from `<` to `<=`: nine failed checks.
- Transform selected output only when `hit && selectedFraction < 1.f`, suppressing the original stale selected transform at endpoint fraction 1: four failed checks.
- Reconstruct callback end components using `lengths[index]` instead of `lengths[axis]`: 147 failed checks.
- Write transformed pair normal W=1 instead of W=2: 585 failed checks.

An additional per-pair replacement mutation from `<` to `<=` survived the 209-case corpus: its coincident equal-distance payloads were indistinguishable. This is a detected test-coverage gap, not evidence that non-strict replacement is correct. The final245 corpus adds authentic original executions with coincident equal-fraction faces carrying distinct interpolated normals in both orders. A fresh passing baseline followed by the same per-pair mutation now produces 12 failed checks. The historical209 result is retained as-is; this final245 result closes that specific coverage gap.

The full-query scope's required zero endpoint metadata makes `metadata[i]` and `metadata[2*i]` indistinguishable. Nonzero raw metadata indexing belongs to the separately verified static type-3 callback seam; this full-query test claims no mutation coverage for that distinction.
