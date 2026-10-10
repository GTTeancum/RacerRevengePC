# 48S bounded candidate collection in Linux

This packet implements source-proven122350 collection over explicit spatial
inputs, its authenticated QuadTree/iterator/preallocated-pool behavior, separate
query bounds, and the64-entry ordered merge in219810. It does not infer a live
world or replace traversal with a pre-enumerated list.

## Behavior retained

- Bounds tags1/6 retain the cached query sphere; tag3 updates it from explicit
  endpoints/cached scenes before a separate returned-bounds transform.
- Iterator next happens before the nominal capacity check, including zero and
  negative capacity. One additional object can be stamped after output is full.
- Group expansion can exceed that nominal limit and emits repeated outer-group
  identities while current identity/member flags and supported shape fields
  refer to members. Physical backing is independently guarded.
- Epoch stamps belong to shared objects, not a private deduplication set per
  tree. Equal/unequal tree epochs, ushort wrap, interrupted block cursor and
  disjoint-open state are preserved.
- New-search node selection preserves recursion/prepend order using a bounded
  caller stack. Free/spare/current pool routes and reset bookkeeping are real
  source behavior; allocation growth remains a boundary.
- Later missing-method fallback is supported only under an explicit disabled-
  diagnostics precondition. Initial missing methods and unknown callbacks are
  not treated as an empty world.
- The vehicle merge copies the first list then the second, stops source loads
  after64 outputs, preserves zero/repeated words and unused destination tail.
  Saturated count-only source iterations are safely collapsed for immutable
  input counts; this is an isolated merge proof, not a whole vehicle return.

## Storage and safety

Immutable spatial/filter tables are borrowed. Compact mutable state, pool
entries, traversal stack, output backing and transactional mirrors are caller-
owned and reusable. Object IDs are nonzero/unique, with zero reserved for null;
shared identities use shared references. Reached malformed pool chains are
rejected before mutations can commit a cyclic selected-node state. All full
mutable spans, inputs and nested backing are guarded, including unused capacity.

The hot calls allocate nothing and require no threads, recursion, owning-vector
copy or desktop services. Some validation performs pairwise uniqueness and pool/
selection scans, so cost may be quadratic in caller table size. Validation
amortization or an additional linear marking workspace is future work before
large-world performance acceptance; no arbitrary FPS claim follows from small
synthetic benchmarks.

Object/tree mutable states are44 bytes each, pool state24, pool-entry8 and
pool-block state4. Caller traversal frames are20 bytes. GCC reports per-function
frames of1,056 bytes for collection,208 for open,40 for pool validation,448 for
bounds,8 for transform and120 for merge. These are compiler host estimates, not
measured maximum total stack or Xbox ABI sizes.

## Reproduce

Use a fresh output directory outside the source tree:

    python3 tools/test_native_candidate_collection48s.py --stack-usage --output-dir /tmp/rr48s-release
    python3 tools/test_native_candidate_collection48s.py --stack-usage --sanitizers --no-leak-check --output-dir /tmp/rr48s-sanitized

The shared runner executes all three components serially, validates required
fixtures, and records source hashes before/after. ASan/UBSan are verified;
LeakSanitizer is explicitly excluded on this ptrace-restricted host. The earlier
collector runner's blocked LeakSanitizer exit is preserved rather than relabeled.
Its approximately1GiB sanitizer compiler peak is a verification-tooling cost,
not runtime residency. Python/JSON and original-word execution are tooling.

CMake options are RR_NATIVE_CANDIDATE_COLLECTION_ONLY and
RR_NATIVE_CANDIDATE_COLLECTION_SANITIZERS. NativePc48S.cmake and
Build-NativePc48S.ps1 extend the preserved Windows target. CMake, Windows, Xbox,
rendered gameplay and complete vehicle simulation remain untested here.

## Evidence and model qualification

The full collector has239 genuine-entry fixtures with224 natural completions
and15 explicit boundaries,282,725 original instructions and zero source reads.
Separate merge proof has274 cases/409,136 instructions; bounds proof has273
cases/154,992 instructions. All expected outputs execute original words on
strict initialized overlays, without RAM, patched words, stubs or native-derived
expectations. Forty-seven local ISA checks and32 invariants accompany collection.

The local ISA extension correctly handles scalar upper-register lanes and the
64-bit sign test required by collector helpers. Shared interpreters are unchanged.
An audit replays753 prior48P/Q/R cases: numerical/state outputs and low32 returns
are unchanged, and395 prior files remain untouched. One archived48R diagnostic
comparing the full128-bit V0 changes last-writer PC219770 to219750; that observation
is explicitly qualified and never becomes a Boolean return or changed native
expected output. See candidate-collection-proof48s.md for exact provenance.

Read candidate-collection-map48s.md, candidate-collection48s.md,
candidate-collection-tests48s.md and candidate-collection-review48s.md. Separate
components are documented in collection-bounds48s.md and candidate-merge48s.md.

## Recovery and next boundary

Generated ELF/object/cache products were preserved outside the source tree after
verification; they are excluded from the portable source/evidence snapshot.
Original logs/manifests retain their verification-time hashes, and the relocation
manifest resolves those entries. No prior baseline file or frozen fixture moved.

Next close group member shape0's authentic135CF0 ->11F680 matrix/cache mutation
before broader static-mesh collection integration. External allocator growth,
other methods/bounds, actual registration/ownership and remaining vehicle stages
still need their own proof. Existing ELF supplies the mapped setter; no PC/RAM
transfer is required merely to continue source-derived recovery.
