# Collection validation cost and bounded alternatives (48T)

## Decision and scope

Keep the immediate collector's full span, alias, capacity and transaction guards.
There is no safe optimization that simply remembers that borrowed descriptors
were valid once and then trusts them after the caller can mutate them.

Two small changes are separable from the shape0 arithmetic closure:

1. After bounded acyclic chain validation, compare nonnull active/spare tails
   instead of comparing every active chunk with every spare chunk. This needs
   no additional storage and accepts/rejects exactly the same chain pairs.
2. Optionally accept caller-built sorted reference orders, but revalidate their
   exact completeness and the **current** identity/range values on every query.
   This reduces the eager quadratic identity and block-range checks to linear
   scans with `4*(objectCount + poolBlockCount)` bytes of borrowed index storage.
   The unhinted path retains its existing strong checks.

Both changes landed in48T through the runtime implementation owner. This audit
changed no runtime source; it reviewed the implementation and measured frozen48S
and current48T separately. Malformed-input and differential validation belong to
the focused48T suite and independent review. The baseline results below are
measurements of unchanged frozen48S, not of the shape0 or hint implementation. No wider asset
loader, ownership system, parallelism, allocation growth or live-memory access
is required or proposed for48T.

## Exact existing costs

The line references in this section refer to frozen48S
`src/game/candidate_collection48s.cpp`, SHA-256
`500c746ca26de2a876e0e36a8fa4db9675cb39c2976f618a205cd2f822c260b7`.
The corresponding header hash is
`495d8d17c54bd40f9460db075117586318f408db8c9f675e57a7dccf6ea7c6e1`.

Let `N` be objects, `G` groups, `B` physical block descriptors, `E` pool entries,
`T` trees, `P` pools and `D` traversal-stack capacity. For one reached pool let
`A`, `U` and `F` be the active-block, spare-block and free-entry chain lengths.
Let `S` be selected-node occurrences in one selection and `d_i` the path depth
before its ith push. `S` is not necessarily the number of distinct nodes: a
shared acyclic subgraph can be revisited in source order. Physical entry and
stack capacities still bound successful work.

### Eager checks and state copies

* Lines394–406: fourteen full writable spans cause91 pairwise overlap checks.
  There are21 fixed read spans, three nested filter spans per object and one
  member span per group. Each read span compares against all14 writes, including
  zero-length spans. On a valid input this is `14*(21 + 3*N + G)` read/write
  overlap tests. Numeric range/alignment checks remain constant per span; they
  do not scan every byte or consume inactive filter values. Thus span validation
  is linear in descriptor count, not quadratic in total input byte capacity.
* Lines407–413: nonzero identities and all nested filter spans are eager. Unique
  IDs cost exactly `N*(N-1)/2` comparisons on a valid world. This happens before
  even the disabled-query flag branch.
* Lines414–425: member spans are eager. Every nonempty physical block range is
  checked against the global entry count and every earlier nonempty range.
  With `K` nonempty blocks, actual interval comparisons are `K*(K-1)/2`, but the
  inner loops also visit earlier empty descriptors. All-nonempty worst case is
  `B*(B-1)/2`; all-empty input is linear. Full ranges, including slots beyond
  a pool's logical per-block capacity, participate in disjointness.
* Lines427–439: capacities are checked, compact state is copied into scratch,
  and a successful call copies it to destination. If
  `M = 44*N + 44*T + 24*P + 4*B + 8*E`, two state-copy passes move `2*M` bytes
  plus `4*result.count` output bytes. Each copy pass also reads its source;
  `2*M` describes memcpy lengths, not total memory-bus traffic. Unused declared
  tails are guarded but not copied. A failure after setup has only the initial
  `M`-byte scratch copy; destination and result remain unchanged.

The 48T shape tables add their own state copies and spans. In the implementation
under review the collector has16 write spans rather than14: `120` write pairs
and `16` comparisons per read span, plus geometry reads. No matrix is added to
every object. Frozen48S counts above should not be quoted as48T stack/span counts.

### Reached pool and traversal checks

`validPool` (lines81–108) is called by every `acquire`, and by reset:

* bounded active and spare chain walks: `A+U` steps;
* a second active-tail walk: `A` steps;
* active/spare physical chunk cross-product: `A*U` comparisons;
* bounded free-entry walk with spare-range membership checks: `F*U` comparisons,
  plus up to `F` active-unused-suffix checks.

Its valid-input cost is therefore `O(A + U + F + A*U + F*U)`. This is **not**
one setup check per query; it repeats before every allocation. With `S` spare
chunks of capacity1, initially no active chunks or free entries, allocation
number `a` sees `a` active and `S-a` spare chunks. The cross-product alone totals

    sum(a*(S-a), a=0..S-1) = (S*S*S-S)/6.

Thus a complete selection has a cubic worst-case family despite each individual
validation scan being at most quadratic. A nominal output capacity of zero does
not avoid selection: open/next precede the outer capacity test.

The tail optimization removes this particular cubic term. It does **not** make
the whole collector quadratic or linear: the remaining `F*U` scan also repeats.
For example, while drawing `S` sequential slots from one active block, a stable
free list and spare chain can still cost `O(S*F*U)`.

`push` (lines154–158) checks the current ancestor path in `O(d_i)`. A single deep
chain costs `S*(S-1)/2` ancestor comparisons. `selectNodes` (lines183–189) checks
each acquired entry against the newly built list, adding another
`S*(S-1)/2` comparisons. These checks respectively prevent reached recursion
cycles and duplicate allocation creating a selected-link cycle. Replacing them
with global visited-node deduplication would change valid shared-node behavior.

`treeNext` retains consumed-reference and bounded-hop checks rather than eagerly
certifying every cached tail. A later cached tail is deliberately unconsumed
after early nominal termination. A proposed setup phase must not accidentally
turn these lazy behavior boundaries into unconditional rejection.

## Serial frozen48S scaling measurement

New evidence is in `evidence/collectionValidation48T/`; no48S evidence changed.
The retained `baseline-profile.cpp` harness was compiled against a separate
byte-identical48S source/header snapshot recovered from the published48S ZIP.
The evidence manifest records the ZIP, source, harness and result hashes.

GCC14.2, `-O2`, C++17, strict floating flags, one process/thread, five samples
per row. Each sample repeats the call3–256 times. Construction and printing
occur outside allocation observation. All26 rows completed with zero reported
failures and zero ordinary/array/aligned C++ allocation calls inside calls.
The benchmark checks completion/count and allocation behavior; it is not a new
original-instruction differential proof or an exhaustive state/guard test.

Selected median times, rounded for readability:

| Family | Size | Median per call |
| --- | ---: | ---: |
| Disabled query; unique object IDs | 1,024 objects | 374 us |
| Disabled query; unique object IDs | 4,096 objects | 4.96 ms |
| Disabled query; unique object IDs | 8,192 objects | 22.8 ms |
| Disabled query; disjoint one-entry blocks | 1,024 blocks | 335 us |
| Disabled query; disjoint one-entry blocks | 4,096 blocks | 5.59 ms |
| Fresh deep selection; one large active block | 128 nodes | 19.6 us |
| Fresh deep selection; one large active block | 512 nodes | 316 us |
| Fresh deep selection; one-entry spare blocks | 64 nodes/blocks | 55.0 us |
| Fresh deep selection; one-entry spare blocks | 128 nodes/blocks | 479 us |
| Fresh deep selection; one-entry spare blocks | 256 nodes/blocks | 4.35 ms |
| Fresh deep selection; one-entry spare blocks | 512 nodes/blocks | 41.4 ms |

The512-spare-block case has exactly22,369,536 active/spare comparisons per call.
The two disabled families isolate eager checks while still performing guarded
state copies. The fresh one-chunk case retains ancestor and selected-entry
quadratic scans. Raw min/median/max values and formulas are in `results.csv`.
Noise and differing iterations make these measurements illustrative, not a
hardware throughput guarantee. They are Linux synthetic host measurements,
not console timings, real-world object distributions, gameplay FPS or frame
budget evidence. `jobs.json` cumulative child RSS includes compilation and must
not be interpreted as runtime-only collector memory.

Reproduction: use a fresh48S source checkout from the hash-pinned ZIP, compile
the new harness with its `candidate_collection48s.cpp` and
`collection_bounds48s.cpp`, include that checkout's `src/game`, and use the
flags above. Run the resulting executable once to produce CSV. Exact original
commands are in `jobs.json`; build paths are local evidence locations. For a48T
comparison compile against48T and include its new group-shape source dependency.
Do not overwrite the frozen48S results with a later run.

### Current48T comparison

`evidence/collectionValidation48T/current48t/` retains two further26-row runs,
one unhinted and one with both optional orders. They use the same synthetic
world families and strict compiler settings as the baseline. Every one of the
52 rows completed with zero failures and zero observed C++ allocation calls.
The input hash check found no source change during compile/run. The measured
collector SHA-256 is
`14f1fd5b0624d6121e5efba1020222d2c847fb80af03bd691e1d1fa680ef8e48`;
all eleven source/header/harness inputs are listed in `input-hashes.json`.

| Family | Size |48T unhinted median |48T hinted median |
| --- | ---: | ---: | ---: |
| Disabled query; unique object IDs | 1,024 objects | 406 us | 48.5 us |
| Disabled query; unique object IDs | 4,096 objects | 6.41 ms | 178 us |
| Disabled query; unique object IDs | 8,192 objects | 25.8 ms | 397 us |
| Disabled query; disjoint blocks | 1,024 blocks | 323 us | 3.10 us |
| Disabled query; disjoint blocks | 4,096 blocks | 5.59 ms | 12.1 us |
| Fresh single-chunk chain | 512 selected nodes | 295 us | 303 us |
| Fresh one-entry spare-chunk chain | 512 nodes/blocks | 1.37 ms | 1.29 ms |

The512-spare case was41.4ms on frozen48S. The unhinted48T comparison already
includes the constant-storage tail optimization; hinting then removes only
the eager block-range pairwise work. The largely unchanged single-chunk case
demonstrates that ancestor/new-selection scans remain. Do not attribute all
before/after differences to hints or claim that every world has these gains.

Hint setup is outside timed calls and allocation observation. These particular
synthetic worlds already have ascending identities and block starts, so setup
stores exactly `N+B` sequential references into `4*(N+B)` retained bytes, with
no sorting required. CSV columns report this setup-store count and active hint
storage separately; N8192 uses32KiB, and B4096 with one object uses16,388 bytes.
The C++ harness constructs its vectors before measurement in both current
modes; a fixed-capacity production caller supplies equivalent arrays directly.
No general-world sort timing or asset-loader performance was measured.

The current CSV's `baseline_pairwise_comparisons_per_call` column is the old
48S family formula used to select repeat counts, **not** an instrumentation
count of comparisons still executed by48T. As before, timing checks establish
completion/count and observed allocation behavior only. Malformed/stale hint
rejection, every-span alias guards and transaction equivalence are verified
separately by the focused tests; they are not inferred from fast timings.

## Constant-storage active/spare tail proof

Preserve both existing bounded `validBlockChain` checks and the activeLast
reference check. Determine the active terminal reference and preserve the
existing check that it equals `activeLast18`. Determine the spare terminal
reference. Reject precisely when the active terminal is nonzero and equal to
the spare terminal.

Two finite acyclic singly linked chains over the same next-reference table
intersect if and only if they share their terminal node: after a shared node,
the remaining successor sequence is identical. Conversely a shared terminal
is already an intersection. Pre-validating references and cycles is essential;
the theorem does not replace those checks.

* Both empty: tails are both zero, but the nonzero condition accepts them.
* Only one nonempty: distinct terminals, no intersection.
* Empty active with nonzero activeLast: the existing mismatch/range check rejects.
* Same head, shared suffix, or one head inside the other: equal nonzero terminal,
  so rejection remains `InvalidTopology` before any resetting/allocation commit.
* Disjoint chains: unequal terminals, even if descriptors have adjacent ranges.

This changes `A*U` to a spare-tail walk, requires no extra array, does not read
entry values earlier, and leaves the free/spare and unused-suffix guards intact.
Validate through the existing transaction tests plus explicit empty, shared-tail,
shared-interior, invalid-tail, cyclic and disjoint multi-chunk cases.

## Minimal optional sorted-reference contract

The implemented placement is four trailing default-zero workspace fields:

    const CollectionRef48S* identityOrder = nullptr;
    size_t identityOrderCount = 0;
    const CollectionRef48S* blockOrder = nullptr;
    size_t blockOrderCount = 0;

Each hint is independently absent exactly when pointer and count are both zero.
If present, its declared count must equal the current world count (`N` or `B`).
The full declared hint span receives the same null/alignment/size-overflow/
address-overflow/read-versus-write checks as other borrowed input spans before
any element is consumed. It may alias other read-only data, but not any scratch,
destination, traversal, result or optional shape-state write span. It need not
be copied into scratch. Read-only descriptors/hints must remain stable during
the call, exactly as other borrowed inputs do.

### Identity order

For exactly `N` entries, validate each reference is in `1..N`. Load its current
object identity and require it to be strictly greater than the previous one,
starting with previous identity zero. This simultaneously rejects zero IDs,
duplicate IDs, duplicate references and unsorted stale orders.

Proof of completeness: any repeated reference reads the same current identity
and violates strict order. Therefore the list has `N` distinct references from
a set of size `N`, so it contains every object exactly once. Consequently no
unlisted duplicate identity or zero identity can evade the check. No bitmap or
trusted precomputed unique flag is necessary. All natural-order descriptor and
nested-filter span checks still run independently.

### Block order, including empty blocks

For exactly `B` entries, validate each reference is in `1..B`. Define its key
from the **current** descriptor as:

    (entryCount != 0 ? firstEntry : 0, reference)

Require strict lexicographic key order. Each nonempty descriptor retains the
old full-range check: `firstEntry != 0`, `firstEntry <= E`, and
`entryCount <= E-(firstEntry-1)`. For each nonempty descriptor in sorted order,
require its zero-based start to be at least the previous nonempty end, then
set the end to `start+entryCount`. Perform range validation before addition.

Empty descriptors all sort before nonempty ones, ordered by reference. Their
opaque firstEntry values are not constrained, just as on the current path.
They must still appear exactly once. Repeated references have identical keys
and cannot satisfy strict order; `B` distinct in-range references therefore
cover all physical blocks. Sorted nonempty starts plus adjacent end comparisons
are equivalent to pairwise full-range disjointness. Same-start nonempty ranges
are rejected even though their reference tie-breakers are distinct. Touching
half-open endpoints remain valid. Zero-count worlds and the independently
absent/present combinations must be tested explicitly.

### Mutation, lifetime and transaction

These spans are ordering hints, not a certificate. Current identities, starts,
lengths and all nested pointer/capacity descriptors are checked again each call.
A caller edit that preserves the checked conditions remains valid. An edit
that makes a hint unsorted, incomplete, duplicated, out of range or exposes a
malformed current range rejects rather than silently skipping validation.
Replacing/reordering a table or changing its count may require rebuilding the
hint. Changing a nested filter/member pointer is covered by the unchanged
current-span guards regardless of whether the hint stays sorted.

Hints do not validate pool next pointers, counters, free entries, tree caches,
object state, shape transforms or per-query buffers. Their reached runtime
checks remain. Nothing is retained beyond return; caller storage may be reused
or freed afterward. Concurrent or reentrant mutation during a call is outside
the serial borrowed-storage contract, with or without hints.

Malformed hints are rejected before initial-state copying where practical and
always before destination/result mutation. The existing `Complete`-only commit
rule and unchanged destination tails remain. No validation hint may be marked
valid by writing into a caller buffer that aliases initial data or output.

Required new tests include every write-versus-hint alias, exact counts, alignment,
overflow, null-with-nonzero count, invalid/duplicate/missing refs, nonzero-ID
changes, duplicate-ID mutations, same-start/overlap/range-tail mutations, empty
blocks with arbitrary firstEntry, all-empty/zero-count tables, valid descriptor
edits that keep the ordering correct, independent hint absence and bitwise
hinted/unhinted differential parity. Retain lazy-read and no-allocation tests.

## Setup work and storage choices

The caller that already loads/builds a bounded world can build the two orders
once into caller-owned fixed-capacity arrays. `4*N + 4*B` bytes suffice to retain
the finished orders: e.g. N4096/B1024 uses20KiB. An iterative in-place heapsort
needs no heap or recursion and takes `O(N log N + B log B)` setup work. A small
radix helper could use an additional reusable reference array and a fixed
histogram; it is unnecessary for the per-query contract. The helper must not
reorder the actual objects/blocks or reinterpret opaque IDs as dense indices.
Its own writes, failure behavior, overflow and alias checks need a separate
contract if it becomes a public API.

Other options, for a later independently tested change:

* A selected-entry bitmap costs `ceil(E/8)` bytes and replaces the new-list
  duplicate scan with constant-time test/set. Clear it at each selection or
  maintain a fully specified generation protocol including wrap. It cannot
  deduplicate node identities because repeated nodes can be observable.
* An active-path node bitmap costs `ceil(nodeCount/8)` bytes. Set on push and
  clear on pop, so only ancestors are forbidden; global ever-visited state is
  incorrect. Every ref must be validated before bitmap access.
* A spare-entry bitmap costs `ceil(E/8)` bytes. Clear it for each `validPool`,
  mark the **full** entry ranges of current spare blocks, and test each free
  entry. Existing global disjointness bounds the marked range sum by `E`.
  This changes `F*U` to `O(E+U+F)` per validation, without a stale cross-call
  cache. Repeated calls still cost `O(S*E)`, but avoid the cubic product with
  only1KiB of bits at E8192. Empty blocks and full declared range semantics
  must remain exact.
* A dense `Ref[E]` entry-to-block owner table costs `4*E` bytes (32KiB at8192),
  plus block state marking, and supports fast membership. Its validity must be
  tied to current block descriptors by rebuilding or exact revalidation; a
  mutable borrowed map cannot be accepted merely because setup populated it.
* Sorted physical ranges plus binary search and current spare membership bits
  trade owner-table memory for `O(log B)` membership work. Keeping marks correct
  across reset/acquire needs explicit proof of all mutations and shared pools.

The existing object/tree/pool/block/entry state sizes are44/44/24/4/8 bytes;
these are GCC host measurements, not a console ABI assertion. Source, scratch
and destination together retain `3*M` state bytes when all three coexist;
scratch and destination identity capacities and `20*D` traversal bytes are
additional. Optional order/bitmap storage should be compared with this budget,
not with Python/JSON harness residency. There is no global hash set, thread,
dynamic allocation or matrix-per-object requirement.

## Why a trusted setup token is not the48T solution

A pointer address, caller-set generation, old checksum or boolean does not prove
that current borrowed descriptors still meet prior conditions. A finite hash
also is not an exact collision-free certificate. `const T*` only restricts one
access path; other writable aliases can change the data between calls. Caching
old nested span addresses would miss newly overlapping descriptor values.

A genuine immutable prepared-world API could instead own a caller-backed copied
snapshot, expose no writable alias, require all mutations through its builder,
invalidate every dependent index on mutation, and keep storage alive until all
queries return. It would need to distinguish immutable topology/identity from
mutable pool, tree, object and shape state. Generation checks would then be
valid only because that ownership/mutation discipline was enforced. Query
buffers and mutable state still need per-call alias and reached topology checks.

That is a larger lifecycle design. It also changes which snapshot a query reads
and must not be smuggled into the current borrowed-input API as a performance
flag. Exactly revalidated sorted-reference hints offer most of the eager-check
benefit without that ownership expansion, while retaining malformed-input
rejection after arbitrary caller edits between calls.
