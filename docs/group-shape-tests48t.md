# Native group shape tests (48T)

## Scope and independence

The additive tests are `tests/native/group_shape48t.cpp` and
`tests/native/candidate_collection48t.cpp`. They consume the independent
instruction-executed packets described in `group-shape-proof48t.md`; they do
not calculate expected matrices or spheres using the implementation under
test. Test-only relocation lives in the two `*fixture48t.h` headers and reuses
the unchanged48S spatial/filter loader.

The helper packet has187 original executions. Exactly91 completed group135CF0
tag0 cases are compared to the native helper; one reached null-geometry case
checks `MissingGeometry` and unchanged outputs. The other95 cases are explicitly
original-only: direct11F680 entries or group comparison/null-shape routes. They
are not counted as native helper coverage. In particular the native helper is
not advertised as a general matrix setter.

The collector packet has110 original executions, including108 completions and
two pointer boundaries. Each is run with neither validation order, only the
identity order, only the block order, and both orders. Thus the report contains
432 completed differential invocations and eight boundary invocations, not432
independent original worlds. The239-case48S corpus and old tests are unchanged
and are exercised separately by the full regression runner.

## Bitwise state checks

The typed helper state is projected into copies of all72 original shape words.
The member sphere is projected into copies of all64 original member words.
Every projected word is compared, including untouched words, opaque pointer
identities, metadata, all matrix W lanes, member center W, and signed zeros.
All nine geometry words are verified unchanged in the original snapshots;
the native borrowed geometry and source descriptors are snapshotted bytewise.
There is no dereference of a fixture's guest address.

Collector comparisons include every represented object, tree, pool, pool-block
and entry field, source-order output identities, scratch state, complete source
spans, and all physical destination/scratch capacity tails. Shared shape and
geometry alias IDs become one actual native table entry or geometry object.
This preserves repeated members, distinct members sharing one shape, two groups
changing a shared shape, and per-member spheres independently of shared matrix
state. Comparison tags4/5 retain their existing five-field projection.

Fixtures distinguish the source's important details:

- Translation XYZ is prewritten before the previous-cache copy, so previousC0
  contains the new translation and all original basis/W bits.
- Composition consumes all four terms and lanes with the original arithmetic
  association; sparse terms are not elided.
- Sphere center uses the incoming world matrix, rather than the composed local
  matrix. Center W is positive1, while radius uses the unscaled geometry half.
- Common group scale-ratio arithmetic remains executed, but those ratios do
  not scale the tag0 matrix or radius.
- Query bounds remain authoritative. An active tag0 query bounds input remains
  unsupported; duplicate member shape fields cannot override prepared nonzero
  query bounds, and an inactive query shape0 reference cannot alias them.

## Safety and bounded execution

All native calls are watched for ordinary, array, aligned and aligned-array
`new`. The final release and sanitizer reports require all four counts to be
zero. Fixture parsing, relocation, snapshots, sorting hint arrays, reporting
and timing setup are outside the watched call.

The safety suite covers:

- Null, unaligned, overflowing and address-wrapping new declared table spans;
  null, unaligned and wrapping geometry; all new writable/read-only overlaps,
  descriptor overlaps, overlaps confined to inactive tails, and result aliases.
- Source, scratch and destination shape capacity shortage; missing and invalid
  shape references; late geometry/numeric/output shortages after prior shape
  work; and bytewise atomic preservation of every destination and result on
  rejection. Scratch is intentionally allowed to change on rejection.
- Every consumed local/world matrix lane, translation lane, center/half lane
  and finite-to-infinite intermediate. Old composed/previous data and old
  translation XYZ are allowed to contain nonfinite bits because overwritten.
- Disabled query, ignored group, nominal-zero termination, empty members and
  other unreached shape arithmetic. A successful read-only alias uses a real
  geometry Vector3 subobject, not a reinterpreted unrelated C++ object.
- Exact-count hints, zero/duplicate/out-of-range/missing/unsorted references,
  stale current identity and block ranges, overlapping nonempty ranges, empty
  range permutations, full hint-span aliases and changed nested spans. A
  still-valid ordering after a descriptor change is accepted, establishing that
  hints are revalidated current values rather than trusted generation tokens.
- Present nonnull/count0 hints for an empty table, an all-empty block table,
  and valid touching nonempty ranges.
- Pool-tail disjointness: both chains empty, either chain empty, two disjoint
  chains, shared head, different heads with a shared suffix, wrong active tail,
  active/spare cycles and out-of-range links, both with and without hints.

These are finite backed synthetic C++ inputs, not a claim that arbitrary host
pointers are safe. Reference parameters must denote actual aligned objects.

## Reproduction and retained evidence

Run the serial evidence tool after obtaining the shared build/test window:

```sh
python tools/gameplay/group_shape_native48t.py \
  --output evidence/groupShape48T/native-final2 \
  --helper-sha "$(sha256sum evidence/groupShape48T/helper-fixtures.json | cut -d' ' -f1)" \
  --collector-sha "$(sha256sum evidence/groupShape48T/collector-fixtures.json | cut -d' ' -f1)"
```

The output directory must be new. Use the hashes in the independent proof
report when checking an externally supplied packet. The runner pins one CPU,
executes one compiler/test at a time, enforces180seconds per command, and records
all input hashes before and after. Strict flags include C++17, `-O2`,
`-ffp-contract=off`, `-fno-fast-math`, `-frounding-math`, warnings-as-errors and
GCC stack-usage output.

ASan+UBSan are run separately with halt-on-error. LeakSanitizer is explicitly
disabled because the executor's ptrace route is unsupported; it is not claimed
to have passed. Build objects and executables are under temporary directories
outside the source tree. Retained evidence consists of commands, compiler/test
logs, timings, reports, stack-usage files and mutant source. No retained object
or executable is needed to rebuild.

The13 semantic negative controls must compile and run normally, return1, and
record positive ordinary assertion failures. A crash, timeout, sanitizer error,
compile error or harness exception is never accepted as a killed mutant:

1. Copy previous cache before translation prewrite
2. Force incoming translation W to1
3. Drop the fourth matrix-composition term
4. Zero composed W lanes
5. Use the composed matrix for the first sphere-center term
6. Preserve incoming W for sphere center
7. Scale the sphere radius
8. Reassociate sphere-center addition
9. Omit per-member sphere stores
10. Omit shared shape-state updates
11. Omit destination shape-state commit
12. Override query bounds with duplicate member shape metadata
13. Remove the validated shared-pool-tail guard

`native-final2/summary.json` is the final source-matched run. Earlier
`native-dev/` and `native-final/` retain their original input hashes and logs;
they are not silently overwritten. In the first final run the benchmark key
`shape_state_bytes_each` meant used bytes **per buffer**, not per state. The
final report corrects its name to `shape_state_used_bytes_per_buffer`.

### Supplemental hint sensitivity

`tools/gameplay/group_shape_hint_sensitivity48t.py` creates the separate
`evidence/groupShape48T/hint-sensitivity/` packet. It freshly rebuilds all four
baseline objects from sources matching final2, records source and object
SHA-256 provenance, and first reproduces the1,305,422-check collector baseline.
It reuses no temporary objects. Before and after the run it verifies all final2
input hashes and every retained final2 evidence file remain unchanged.

Three additional controls are counted separately from the original13:

- Removing identity strict ordering causes36 assertion failures.
- Removing block strict ordering causes18 assertion failures.
- Removing hinted interval disjointness causes9 assertion failures.

All three compile cleanly and fail with normal exit1 and zero watched
allocations. There are no crash, timeout or harness-error kills. This closes
sensitivity coverage for the optional validation-hint optimization in addition
to the original pool-tail control. The supplemental summary SHA-256 is
`f108408e89e788317f17ff0f5877d0bff46b0c47570c25b664c6211b8ac8e9f9`.
This packet adds no new sanitizer claim; final2's source-matched ASan+UBSan
evidence remains the sanitizer result.

## Resource interpretation

The final run passes1,308,308 assertions:2,886 helper and1,305,422 collector.
It records56 helper and369 collector safety invocations,278 new overlap cases
in total, zero failures and zero watched allocations. Both sanitizer suites
pass under the stated no-LSan configuration, and all13 negative controls fail
through normal assertions. The summary SHA-256 is
`19edc5ad597a5f09672f832cd73de9b8987cfd807cc83b807c382f8f2a7920ee`.

One shape state is192 bytes, one source descriptor72 bytes, geometry24 bytes
and returned sphere20 bytes on this native host. For three shapes the used
state is576 bytes per initial/scratch/destination buffer; a declared seven-slot
scratch allocation is1344 bytes because it includes four canary tail slots.
The final JSON reports both used and declared backing. Runtime retains no heap
workspace or pointers. The release helper's GCC stack-usage record is a464-byte
static function frame, not a cross-platform peak-stack measurement.

Benchmarks are serial synthetic native Linux call timings, with input parsing
and caller-memory preparation excluded. Compiler/sanitizer maximum RSS is a
tool resource measurement, not game or runtime memory usage. None of this is a
game FPS, Xbox, live-PC, RAM-image, installation, renderer or end-to-end driving
claim.
