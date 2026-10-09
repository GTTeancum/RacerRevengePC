# 48N ordered static spring query: native regression checks

`tests/native/spring_query48n.cpp` tests the caller-owned, bounded
`queryStaticSpringBounded48N` seam. It consumes schema 1 of
`evidence/springQuery48N/native-fixtures.json`. The fixtures contain 132
source-derived/synthetic original-query executions over four frozen mesh
fixtures. Their production and provenance are documented separately by the
original differential proof. This test does not retrieve RAM, contact a user
computer, install software, register a world, or establish live vehicle ownership.

## Reproduce

Run serially from the repository root, using fresh output directories outside
this checkout:

```sh
python3 tools/test_native_spring_producer48n.py \
  --tests spring_query48n --stack-usage --output-dir ../spring-query-release
python3 tools/test_native_spring_producer48n.py \
  --tests spring_query48n --stack-usage --sanitizers --no-leak-check \
  --output-dir ../spring-query-sanitized
```

`--no-leak-check` explicitly excludes LeakSanitizer on ptrace-restricted hosts;
AddressSanitizer and UndefinedBehaviorSanitizer remain required. Omitting that
option is appropriate where LeakSanitizer works. A blocked leak check is not a
leak pass. The runner fingerprints inputs before and after execution, so a
concurrent source/build-file edit blocks the report even if the test exits 0.

## Original-output comparisons

Every fixture checks:

- All 11 state words, including preserved fields and signed count/capacity bits.
- Every byte of every 96-byte record slot, including unused slots, margin,
  flags, preserved bytes and opaque vector fourth lanes.
- Query flags, hit/miss, preserved prior query-current identity when no collision-type
  entry is visited, last enumerated collision-type candidate, ordered
  candidate-occurrence count, and each occurrence's identity/flags/copy40/copy50.
- Global selected point, normal, correction, fraction, candidate and face.
- Strict-nearest pair point, normal, fraction, candidate and face, plus all 16
  pair lanes: first endpoint, second endpoint, normal and surface point.
- Per-candidate geometry hits, callback invocations (including skipped
  callbacks), appended records and original successful logical saturation.
- Requested post-query reset and unchanged records after that reset.

The original type-2 pair backing has no per-pair correction array. The additional
native `nearestPair.correction` field is checked against the existing bounded
collision composition, explicitly as a native wiring consistency check. It is
not represented as independent original-output evidence. The original global
selected correction is independently compared.

Original seeded collision backing on a miss is outside this API's input
contract. The test instead requires zero-initialized native pair/contacts on a
completed miss while checking its state, records, effects and flags against the
original. Internal tree-visit counters and traces are not exposed by this API;
existing bounded-tree tests retain responsibility for those lower-level details.

Fixture totals are 91 queries with geometry hits, 155 candidate geometry hits,
153 callback invocations, 131 appended records and 19 saturated appends. These
are different quantities; a callback can observe a cumulative earlier pair and
a query can hit multiple candidates.

## Native boundary and transactional checks

The test adds 243 negative safety cases, including 171 explicit alias cases:

- All 15 pairings of the six writable regions: record scratch, record output,
  effect scratch, effect output, tree stack and result metadata.
- Every writable kind against all 26 readable regions: top-level query and
  workspace descriptors; segment, sphere, filter and copied vectors; initial
  state and records; candidate descriptors; both candidates' identity/category
  vector backing; query identity/category vector backing; mesh descriptor,
  vertices, normals, nodes and faces; and both scene-name spans.
- Every insufficient record capacity from 0 through 3 for a four-record copy,
  every insufficient effect capacity from 0 through 1 for two candidates, and
  both insufficient tree capacities (0 and 1). An exact two-entry tree stack is
  separately required to succeed.
- Negative signed metadata, logical capacity exceeding physical input backing,
  null nonempty arrays, misaligned typed pointer spans, multiplication overflow,
  address-wrap ranges and malformed nested meshes.
- Full declared-capacity alias validation, including overlaps solely in unused
  tails, and invalid nonzero declarations when the used count is zero.
- Failures after prior successful work in scratch: unknown response callbacks,
  dynamic callback scope, group objects, material parsing, identity mismatch,
  null mesh, nonaffine matrices, invalid spheres and interpolation boundaries.
- Malformed tree cycles, leaf/vertex bounds and nonfinite geometry.

Every rejection compares the complete result and destination buffers plus all
owned readable backing against pre-call byte snapshots. Scratch may change on
ordinary failure; alias-rejection checks additionally snapshot scratch. This
separates the transactional destination guarantee from permitted scratch work.

Positive native boundary cases cover larger declared destination capacities with
unchanged tails, zero logical record capacity, original count-above-capacity
saturation, zero physical record storage, an empty null-array query, previous query-current identity preserved independently
  of reset spring-current identity, skip-before-
dynamic ordering, disabled callbacks, group bypass, and an unreachable response
callback on a broad-phase miss. Fixture runs also surround used storage with
canaries and verify input descriptors, nested lists, names and all mesh storage
remain byte-for-byte unchanged.

## Allocation and serial memory evidence

Test-only replacements instrument ordinary, array and over-aligned C++ allocation
operators. Guards cover differential calls, negative/positive boundary calls,
lower-level native composition and 84,000 serial benchmark queries. JSON parsing,
fixture construction, snapshots, reporting and all other setup remain outside
these guards. The runtime API uses caller-owned buffers and performs no threading.

On the verified 64-bit Linux host, the two-candidate/four-record benchmark uses:

| Caller-owned item | Bytes |
| --- | ---: |
| Record scratch, four slots | 384 |
| Effect scratch, two entries | 80 |
| Tree stack, four entries | 16 |
| Record/effect destination backing | 464 |
| Result metadata and contacts | 256 |
| Workspace descriptor | 48 |
| Query view | 80 |

The largest fixture's reusable scratch is 768 bytes: six record slots (576),
three effect entries (120), and 18 tree-stack entries (72). Immutable input record
storage, filters, meshes, query metadata and destinations are additional and are
not hidden inside that scratch total. Sizes are printed by the test rather than
assumed to be identical on a 32-bit target.

The benchmark runs seven serial batches of 12,000 two-candidate queries and
reports median and maximum batch-mean nanoseconds per complete query. It is a
host component observation, not a full-game frame-time estimate, concurrency
benchmark or original-Xbox performance result. Timing varies between runs.

The 132-case release and ASan/UBSan runs produced 96,728 checks with zero failures
and zero observed guarded allocations, including prior query-current identity
preservation. LeakSanitizer was explicitly disabled on the ptrace-restricted host. GCC's release `-fstack-usage` report for the current
runtime gives 848 bytes for the ordered-query frame, 880 for the bounded collision
frame, 384 for the triangle routine and 288 for the static callback frame. These
are separate compiler estimates, not a measured or summed maximum live stack.
Use the final runner evidence for the exact compiler, input hashes, sanitizer
configuration and run results.

## Explicit limits

This proves the explicit-list, one-pair, mode-1, float-mesh static callback seam
and its native safety contract. It does not recover world registration,
detector ownership, candidate scheduling, live race integration, dynamic bodies,
unobserved original interpolation behavior, Xbox execution or gameplay FPS.
Existing tests and fixtures are left unchanged.

## Sensitivity checks

After a successful release producer build, run:

```sh
python3 tools/test_spring_query_sensitivity48n.py \
  --build-dir ../producer-check --output-dir ../producer-mutations
```

The tool first checks the release input hashes, compiles temporary mutated source
copies outside the repository, and requires each binary to fail its unchanged
original-derived test. It never edits the production source or original oracle.
The seven rejected mutations cover prior query-current preservation, cumulative
pair reuse, strict pair ties, global equal ties, per-hit callback ordering,
normal.W, and successful original saturation. Results and logs are retained in
`evidence/nativeSpringProducer48N-linux-20261006/sensitivity/`.
