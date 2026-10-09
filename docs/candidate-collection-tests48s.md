# Native candidate collection verification, 48S

## Result and scope

The final source-executed packet contains **239 cases: 224 genuine returns and
15 explicit boundaries**. The native release and AddressSanitizer +
UndefinedBehaviorSanitizer runs each pass **391,437 checks with zero failures**.
All four call-only allocation counters are zero. Ten independent native semantic
mutants are detected by the source differential tests.

This is a bounded, initialized, synthetic-world proof. It is not execution of the
game, a RAM capture, discovery of a live world, a recovered registration system,
an allocator implementation, a detector/scheduler integration, or an Xbox frame
rate claim. Source code/dispatch authentication and genuine instruction execution
are documented in `candidate-collection-map48s.md` and the collector proof packet.
The native scope is specified in `candidate-collection48s.md`.

Final evidence is in `evidence/candidateCollection48S/native-final2/`:

- `summary.json`, `release-report.json`, `sanitized-report-no-lsan.json`
- `input-hashes.json`, `jobs.json`, `file-hashes.json`
- Exact compiler commands, stdout/stderr, child wall/CPU/RSS measurements
- Release and sanitizer `.su` compiler stack-usage reports
- Ten separately compiled mutant sources, hashes, results, and diagnostics

The fixture SHA-256 is
`a737d045269c03ec44bd71cefa90b895b3d63aacb169e6fb02fd913ab7c807e9`.
The runtime implementation SHA-256 is
`500c746ca26de2a876e0e36a8fa4db9675cb39c2976f618a205cd2f822c260b7`.
All tested source/header/fixture fingerprints were checked before and after the
serial run. The final manifest also records the runner's own hash.

## Reproduction

Run from the repository with its existing GCC and Python installation:

```sh
python3 -B tools/gameplay/candidate_collection_native48s.py \
  --output evidence/candidateCollection48S/native-rerun \
  --fixture-sha a737d045269c03ec44bd71cefa90b895b3d63aacb169e6fb02fd913ab7c807e9
```

The output directory must be new. `--resume` only reuses already successful exact
commands when all pinned source inputs are unchanged. The runner pins itself and
its children to one existing allowed Linux CPU, compiles and executes serially,
and uses standard-library `wait4` for resource measurements. It installs nothing,
uses no network or PC, and writes only the chosen new 48S evidence directory.

The native harness is `tests/native/candidate_collection48s.cpp`; the test-only
fixture relocation helper is `candidate_collection_fixture48s.h`. Runtime code
never reads JSON. The release uses GCC 14.2, C++17, `-O2`, no fast-math or floating
contraction, and strict warnings as errors. Sanitizer compilation uses `-O1`,
ASan+UBSan, and frame pointers. The harness selects round-to-nearest before calls.

The first sanitizer exit encountered a verified executor limitation:
LeakSanitizer cannot operate under ptrace. Its stderr and failed job remain
preserved. The **same binary** was rerun with leak detection disabled; address
and undefined-behavior checking both passed with empty stderr. This is not a
LeakSanitizer pass. Call-only allocation instrumentation remains enabled.

## Independent fixture relocation

The adapter constructs borrowed object/group/tree/zone/node/item-block/pool
structures from the initialized source **input topology**, never from expected
candidate output. It relocates source addresses to checked index-plus-one refs.
It preserves:

- Four-slot item blocks and their actual linked order
- Hierarchical children and selection inputs, including deep complete/mixed
  subtree traversal; the native runtime performs selection itself
- Cached selected-node links, interrupted block cursor, and every initial pool
  entry, including stale unused records
- Active, spare, and free preallocated pool chains and chunk entry extents
- Shared query/direct/member object identity and shared epoch storage
- Distinct zone links pointing to one shared zone, including fallback identity
- Explicit first/second method selection and optional fallback

A source test whose global chain is resolved by the original is adapted as that
same explicit resolved chain. The native API does not perform a global lookup.
Optional later-method diagnostics are mapped explicitly; initial missing methods
remain a boundary. Unsupported dependencies are not replaced with fake methods,
forced source returns, or allocator fallback.

## Exact state and source-quirk checks

Every successful call compares count, current identity, active tree, output order,
the entire original output backing/tail, and these complete native projections:

- Objects: 16-bit epoch, flags, raw cached center XYZW/radius, and compact member
  translation/scales
- Trees: epoch, flags, blocked value, all four cached rectangle words, saved and
  current list refs, item-block ref, signed slot, and auxiliary value
- Pools: active first/last, spare first, used count, free entry, and bytes used
- Every pool-block next ref and every pool-entry next/node ref

The compact state is compared bitwise, including unchanged projection padding.
For represented member shapes 4/5, the five compact float fields are projected
back into the source's 64-word shape snapshot, and all 64 words are compared.
Unrepresented raw shape words remain source inputs; this reconstruction does not
claim the runtime stores or mutates a full raw source shape.

All input descriptors, borrowed arrays, nested lists, and initial mutable-state
backing are byte-snapshotted and must remain unchanged. All unused destination
and scratch table tails are checked; output scratch beyond its reached prefix is
also checked. Scratch traversal frames are explicitly disposable implementation
state.

The genuine differential corpus establishes disabled-count reset/flag clear,
next-before-signed-capacity, group overrun of nominal capacity, repeated outer
group output identities, member/current-identity effects without member filters,
shared stamps across equal/unequal epochs, cached B0 carryover, disjoint opens
retaining cursors, first-new-empty-method termination, explicit fallback,
repeated-zone aliases, later null methods with diagnostics enabled/disabled,
source child/prepend traversal order, and preallocated sequential/free/spare/reset
pool paths. Query bounds 1/6/3 are exercised, including transformed shape-3
endpoint/scene input. Unsupported query and member-shape routes are explicit
transactional boundaries.

## Safety and laziness

The final report records **1,197 rejection cases**, including the 15 source
boundaries, and **868 exhaustive declared-span overlap cases**. The matrix covers
all 14 mutable spans: six scratch arrays, six destination arrays, the explicit
traversal stack, and result. It tests every ordered mutable/mutable pair and every
mutable span against every present root descriptor, immutable table, initial
state table, bounds array, nested filter list, and group member span in the rich
fixture. Tail-only read-span overlaps are included.

Further checks cover null/misaligned/unrepresentable/address-wrapping spans,
short initial/scratch/destination backing, null/duplicate identities, full and
partial pool-chunk entry-range overlap, invalid references, node/item/list/zone
cycles, pool active/spare/free cycles and ownership collisions, pool exhaustion,
stack shortage, unsupported selected methods, late member/filter/numeric errors,
and physical output shortages after real scratch mutations. Every rejected call
must preserve **every byte of every destination backing array and result**;
scratch may change as the API specifies.

Seventeen lazy-read/alias checks include protected-page filter backing behind
source-proven self/flag/category/sphere/identity gates, negative/zero list counts,
query-disabled bounds/method bypass, skipped cached sphere NaNs, valid shared
read-only spans, zero physical output capacity for nominal zero, and query/member
shape identity despite contradictory duplicated object descriptor fields.

Null or misaligned C++ result references are not fabricated: creating such a
reference would itself be undefined before the API could reject it. Validly
aligned result aliases are tested against all other declared spans.

Ordinary, array, aligned, and aligned-array global allocation entry points are
instrumented **only while inside the runtime call**. JSON parsing, setup,
snapshots, comparisons, reporting, and buffer allocation occur outside that
window. All four counters remain zero on successful, rejected, and benchmark
calls. No allocation interceptor supplies pool fallback.

## Mutation sensitivity

The final runner changes one native semantic rule per separately built mutant.
All are caught using the genuine source-return comparisons and boundaries:

| Mutant | Failed assertions |
|---|---:|
| Check nominal capacity before calling next | 50 |
| Emit member IDs instead of repeated group ID | 74 |
| Add an inner nominal-capacity group limit | 26 |
| Disable shared object-stamp deduplication | 31 |
| Clear B0 during cached rewind | 17 |
| Reverse child traversal | 51 |
| Omit direct-object flag side effect | 1,490 |
| Omit group-member flag side effect | 132 |
| Omit cached tree epoch increment | 1,927 |
| Make sphere touching exclusive | 2 |

This measures sensitivity to these ten specific semantic errors, not a proof
that arbitrary implementation errors are impossible.

## Exact storage and bounded resource estimates

On this GCC/x86-64 build the compact element sizes are:

| Element | Bytes |
|---|---:|
| Object state | 44 |
| Tree state | 44 |
| Pool state | 24 |
| Pool-block state | 4 |
| Pool entry | 8 |
| Explicit traversal frame | 20 |
| Result | 12 |

For declared world counts O/T/P/C/E, one compact state copy is exactly
`B = 44O + 44T + 24P + 4C + 8E` bytes. Minimum logical caller scratch is
`B + 4I + 20D` bytes for I output identities and D traversal frames; initial and
final state remain separate caller storage. A successful call copies B bytes to
scratch and B bytes to destination, plus four bytes per emitted output. Extra
caller capacities are validated and preserved; they are not silently narrowed.
The harness intentionally adds four tail elements to each state array, adding
496 bytes per state storage set, and uses 132 output slots (528 bytes).

The release `.su` reports give static native frames: collector 1,056 bytes,
open 208 bytes, pool validation 40 bytes, bounds 448 bytes, bounds transform
8 bytes. The longest known project call-chain sum is **1,512 bytes**
(1,056+448+8). This is a compiler-specific estimate excluding external library,
ABI, and sanitizer internals. Runtime tree traversal is iterative; its depth
storage is the caller's 20-byte-per-frame array, not native recursion.

The four serial release microbenchmarks each run 2,000 calls from identical
initialized inputs:

| Synthetic case | Mean ns/call | B bytes | Declared traversal frames/bytes |
|---|---:|---:|---:|
| Cached basic three candidates | 1,073.622 | 724 | 5 / 100 |
| Group followed by direct | 955.570 | 768 | 5 / 100 |
| Fresh root and four children | 1,162.151 | 724 | 9 / 180 |
| 72-object source, nominal limit 64 | 5,225.066 | 3,540 | 5 / 100 |

These include validation and transaction-copy costs. They are host-only synthetic
measurements, subject to scheduling/noise, and imply no game/Xbox FPS. Release
whole-harness peak RSS was 25,756 KiB; sanitizer whole-harness peak was
136,172 KiB. Those include JSON/fixtures and test overhead, not runtime scratch.
Verification compilation peaked at 453,328 KiB for release and 1,036,664 KiB for
the sanitizer test translation unit; compilation was one job/one CPU. The latter
is a disclosed verification-toolchain peak, not a low runtime memory claim.

## Preserved intermediate evidence

The earlier 215-case proof packet is archived intact by the source prover.
`native/first-differential.*` records the initial successful 215-case comparison;
`native/second-full.*` records the successful 231-case full run. The first full
native run intentionally remains present with eight failed assertions from two
incorrect test expectations: source allows nonnegative overfull slots, and a
malformed declared chunk extent is rejected eagerly as InvalidInput. Those tests
were corrected using the source instructions/contract, then verified by added
source cases and the clean final run. The failed `/usr/bin/time` setup attempt
and unsupported LeakSanitizer exit are likewise retained. No 48R artifact was
rewritten.
