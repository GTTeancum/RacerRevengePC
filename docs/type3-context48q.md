# 48Q borrowed type-3 query context

## Additive API and ownership

`queryStaticType3WithContextBounded48P` accepts an immutable
`Type3QueryInput48P` template plus a separate `Type3QueryContext48P` containing
six execution fields:

- `spring`
- `bodyScene`
- `pairCount`
- `resetSpring`
- `bodyScenePresent`
- `entry`

Those fields come exclusively from the context. All other fields come from the
template, including the owning identity/category filter vectors. The operation
borrows those vectors; neither the wrapper nor the runtime clones or grows them.
This permits detector composition to pass prepared state without copying the
owning template. The existing `queryStaticType3Bounded48P` entry captures these
same six fields into a local context and delegates to the common implementation.
The retained 48P fixtures, tests, and historical proof are not changed.

The context is a read-only input span subject to the same full-span overlap
contract as the template and nested borrowed data. No declared mutable ray,
record, effect, stack, or result span may overlap any byte of the context.
Failures may change scratch only; all destination storage and result commit
together on `Complete`. This remains serial, caller-bounded numerical work with
no allocation, renderer, live-world discovery, PC/RAM retrieval, or installation.

## Independent poisoned-template replay

`tests/native/type3_context48q.cpp` reuses the fixture parsing, exact comparisons,
synthetic setup, and allocation hooks copied from the retained 48P test. It calls
only the context overload. It reads the unchanged 245-case original-instruction
fixture at `evidence/type3Query48P/native-fixtures.json` (SHA-256
`a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f`).

For each fixture, the test constructs the correct context first, then poisons all
six corresponding template fields before entering the allocation watch:

- Spring words become `0xDEADBEEF`, with negative generated count/capacity.
- Every body matrix word becomes a quiet NaN.
- Pair count becomes zero.
- Reset and body-presence booleans become the opposite of the correct context.
- Entry becomes the invalid enum value 99.

The context is a separate by-value scalar object, so these writes cannot modify
its fields or alias filter storage. Both objects are snapshotted after setup and
must remain byte-identical. The expected outputs remain the original fixture
outputs; they are never recomputed by the old native entry or by a template-copy
adapter. In particular, the test performs no filter copy inside an instrumented
call. JSON loading, vector copies for initial fixture construction, immutable
snapshots, and diagnostics all occur outside the allocation watch.

All 195 supported cases compare complete represented query/spring state, full
Vec4 outputs, all four source-backed ray slots, all 96-byte record slots, ordered
candidate effects, exact float bits, stale data, geometry/callback/append/
saturation counters, and source/filter/mesh/name immutability. The 50 unsupported
cases retain their original-only classification and check transactional rejection.
The scope split is 190 limited-list completions, 5 direct dispatcher completions,
and 50 explicit unsupported-scope rejections.

## Negative, alias, and serial-reuse checks

The additive harness also replays the synthetic 48P safety cases through the new
overload, with mutations of the six execution fields applied to the context.
That includes early validation errors, insufficient capacity, late callback and
geometry failures, original-only boundaries, nested filter/input alias checks,
all mutable/mutable span pairs, saturation, no-callback paths, and empty-list
preparation/reset behavior.

Context-specific overlap checks cover all 24 declared writable kinds:
18 scratch/destination ray spans, 2 record spans, 2 effect spans, tree stack, and
result. Every kind is tested twice, against the context start and against its
final suitably aligned bytes. The latter ensures validation protects the full
context extent, rather than only the first member. Context, template, and nested
borrowed filters must remain byte-identical across every rejection and every
successful synthetic call. Destination/result bytes are additionally checked on
rejection. Successful differential calls protect unused tails and array guards.

Seven serial batches of 4, 000 calls reuse one prepared fixture. The benchmark
checks stable outputs and zero observed ordinary/array/aligned allocations.
It snapshots borrowed input before the entire loop and verifies it afterward.
Timing is a host numerical-component observation, not gameplay or Xbox FPS.

## Reproduction

From the repository root, use fresh output directories outside the checkout and
one permitted CPU. The example uses CPU 0:

```sh
taskset -c 0 python tools/test_native_type3_context48q.py \
  --output-dir ../type3-context48q-release --stack-usage

taskset -c 0 python tools/test_native_type3_context48q.py \
  --output-dir ../type3-context48q-sanitized --sanitizers --no-leak-check

taskset -c 0 python tools/test_type3_context_sources48q.py \
  --baseline-dir ../type3-context48q-release \
  --output-dir ../type3-context48q-source-sensitivity
```

The release and sanitizer runner records exact source/fixture hashes, commands,
compiler version, stdout/stderr, and exit status in `report.json`. It uses strict
floating-point flags and serial compiler/test processes. The sanitizer command
explicitly disables LeakSanitizer; it checks AddressSanitizer and
UndefinedBehaviorSanitizer only.

The sensitivity tool first verifies the passing release report still matches all
current source/fixture bytes and reruns the unchanged baseline. It then creates
six temporary runtime copies outside the checkout. Each copy replaces reads of
one context field with the corresponding poisoned template field. The unchanged
harness is linked against that one mutated runtime plus the baseline's other
objects. A mutation counts as detected only if compilation/linking succeed and
the test exits 1 with positive failed checks. A crash, build error, or fixture
error does not count as a detection. All runtime inputs, the independent test,
the retained 48P test, and original fixtures are rehashed afterward.

## Verified results

On 2026-10-06, GCC 14.2.0, strict floating point, CPU 0, one build/test process at
a time:

- Release and ASan/UBSan each passed 364, 189 checks with zero failures and zero
  instrumented runtime allocations. Actual runtime/test compilation was warning-free.
- All 245 poisoned-template cases executed: 190 full, 5 direct, 50 transactional
  unsupported-scope rejections.
- 1, 355 negative safety cases, including 1, 183 aliases and all 48 context head/tail
  overlaps, passed. Borrowed context/template/filter bytes remained unchanged.
- Original supported counters matched: 154 hit queries, 260 candidate geometry
  hits, 258 callbacks, 268 appended records, and 39 saturated appends.
- All six one-field-at-a-time wrong-source mutations compiled/linked and failed
  normally: spring 657 failed checks; bodyScene 180; pairCount 298; resetSpring 1, 540;
  bodyScenePresent 181; entry 298. Input fingerprints were unchanged afterward.
- The release reuse benchmark completed 28, 000 calls at a median batch mean of
  2, 624.82 ns and highest batch mean of 2, 667.19 ns. These observations vary by run
  and are not a target-console or gameplay performance estimate.
- Caller-owned scratch remains 1, 400 bytes for the benchmark fixture: 600 ray,
  576 record, 160 effect, and 64 stack bytes. Destination and immutable inputs are
  additional. This host reports 120 context bytes, 464 template bytes, and 240 result
  bytes. GCC's per-function frame estimates are 1, 760 bytes for the context entry
  and 160 bytes for the old-entry adapter; these are not summed live-stack bounds.
- ASan/UBSan used `detect_leaks=0`; there is no LeakSanitizer pass claim.

Reports, source snapshots, logs, and mutation controls are retained under
`evidence/type3Context48Q/`. The deliberately wrong pair-count control reports an
unused-context-parameter warning; no actual runtime/test warnings were present.
The retained 48P test SHA-256 remains
`019503b55f84fa870f90f1c98510cff3c8de564832b28bdd07b96a2ee6429f55`.
No production implementation defect was found by this independent overload test.

