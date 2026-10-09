# Independent bounded-collection review, 48S

## Review status

This is a read-only source/contract review of the additive 48S collector, bounds
helper and capped merge. It is not an independently executed native test pass.
No builds, fixture generation, game execution, PC/RAM access or installation were
performed by this review. Only this review document was written. Existing 48R
files and evidence were left untouched.

The first reviewed collector implementation had SHA-256
`97a63616ff78999fc4bf52e2159173e438794a7652961c268f2d37e55d95288e`.
The immutable source packet reports the authorized ELF SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`,
28 mapped blocks and 2,955 words. Its `original-words.json` hash is
`05ce6db129c12888b32c7892526a957c745e6108e5bf1d602d913c711f735863`.

Review complete: R1–R4 below are resolved in the final inspected code/evidence.
No remaining correctness blocker was found within the documented closed domain.
The final collector implementation hash is
`500c746ca26de2a876e0e36a8fa4db9675cb39c2976f618a205cd2f822c260b7`;
the header hash is
`495d8d17c54bd40f9460db075117586318f408db8c9f675e57a7dccf6ea7c6e1`.

At first inspection the source-execution report contained 215 cases, 202 natural
returns, 13 explicit boundaries, 47 ISA checks and 18 semantic checks. The final
reconciliation below supersedes those initial counts. All native execution
results cited here were produced by the native-test owner and independently
inspected and hash-checked by this reviewer, not rerun by this review.

## Initial findings, now resolved

### R1: reached pool cycles can be committed successfully

The initial implementation's `acquire`/`resetPool` methods checked reference
ranges but did not validate free, spare or active chain topology or detect reuse
of a selected-node entry in one selection pass. The later `treeNext` counters
were insufficient: they restarted on every call and a successful object return
could precede encountering the cycle.

A concrete source-inspection reproducer is a fresh tree with a root and one
selected child, an initialized pool with `used20 == perBlockCapacity`, no spare
block, and `freeEntry2C == 1` with `poolEntries[0].next == 1`. The child
acquisition pops entry 1 and selection overwrites its next with null. The free
head still equals 1. The root acquisition then reuses entry 1 and writes its
next as the previous selected head, also 1. The resulting selected chain is a
self-cycle. Give the root one unstamped object and nominal capacity zero: the
first next can stamp and return the object, after which the outer capacity test
ends the transaction with `Complete`, committing the malformed chain.

This is a bounded-safety issue, not a claim that original code promises to handle
malformed pools gracefully. Rejecting such input transactionally is part of the
native contract. Recommended validation must not reject merely unused canary
entries or invent ownership rules for otherwise supported shared references.
The implementation and native-test owners were notified with the reproducer.

### R2: category allow-list access order is simplified

The initial `list` helper broke at the first match for every list. Original
`120E70` stops early for identity lists and category deny mode 1, but category
modes other than 1 scan all positive-count entries, even after finding an
intersection (`12116C..1211B0` and `121224..121268`). The native return value
is equivalent for its fully backed, immutable, nonvolatile arrays. It is not an
exact source read-order reproduction. Preserve the full allow-list scan if the
contract promises exact lazy consumption; otherwise explicitly bound the claim
to values and mutable state. This is not evidence of a wrong output identity.

### R3: initial proof did not exercise all claimed pool routes

The initial fixture setup fixed free and spare heads to zero and allocated one
active chunk. Its coverage omitted free-pop instructions `10576C/105770/105774`,
the existing-active/spare append instructions `10579C..1057A8`, and reset's
active-to-existing-spare append `1059A8..1059B4`. A cache miss did exercise reset
and subsequent reuse, but that is not coverage of all preallocated routes.

The proof owner was asked for real free-list acquisition, multiple active/spare
blocks and reset-append accounting, plus a positive fallback transition. The
initial cases included only the already-current fallback identity case. These
are coverage gaps, not inferred output failures.

### R4: zero object identity is indistinguishable from null

The parent raised the zero-identity contract for independent assessment. A real
source candidate/group is nonnull, while source current identity and identity
list entries can be null. The initial native schema supplied only raw identity
words, with no separate null/presence tag, and documented zero as a valid object
identity. It therefore could not preserve both meanings distinctly. In
particular a source identity list containing a null entry must not match a real
object, whereas native `list([0], realObject.identity == 0)` matches.

The recommended bounded contract requires every declared object's identity to
be nonzero and unique, reserving zero for null current/list identity. An eager
`InvalidInput` guard with full transaction canaries should enforce this. This
restriction does not apply to the separate merge, which correctly copies raw
zero words without interpreting them as candidates or pointers.

## Source-order checks that match the closed implementation

### Collector and shared object state

- Count starts at zero. The disabled query clears only F8 bit 5 and preserves
  initial current identity, active tree and other state. Numeric bounds and
  spatial values are not interpreted on that path; structural span checks still
  precede it under the native API's stated contract.
- The enabled query sets bit 4, prepares collection-time bounds, then opens the
  selected spatial method. Iterator next precedes the signed nominal-capacity
  comparison. Zero/negative capacity therefore still allows the original first
  iterator/stamp effects.
- The same object-state table is used for the query, spatial occurrences, direct
  candidates and group members. Stamps and F8 changes alias coherently. Direct
  self rejection occurs after current/bit-4 effects, as in `120E70`.
- Direct current identity and candidate F8 bit 4 are assigned before sphere or
  filter rejection. Direct sphere arithmetic uses the shared cached sphere,
  ordered XYZ length and inclusive gap comparison. Sphere-skip bypasses those
  numeric fields.
- Bilateral eligibility order is self, candidate F8 bit 2, candidate byte 31
  bit 0, candidate category versus query accept mask, both identity-list
  families in both directions, then both category lists. Nonpositive identity
  counts bypass their modes. Mode 1 excludes matches; mode 0 requires a match;
  other identity modes do not reject. Non-1 category modes require an
  intersection, including rejection for empty/negative counts. See R2 for the
  separate read-order distinction.
- Group expansion has no inner nominal-capacity limit or member predicate.
  It emits the outer group identity once per member, preserving repeated group
  outputs and repeated members, while updating current identity and the member's
  F8/shape state. Common XZ arithmetic occurs even when the coarse group sphere
  is skipped. Member Y and scale ratios remain lazy until a positive member
  count and a nonnull member shape are reached.
- Signed scale bytes and the distinct `base180-other184` versus
  `other194-base18C` formulas match the mapped member routine. Supported member
  shape tags 4/5 update only their projected translation/scales. Other nonzero
  tags preserve shape state after common arithmetic. Tag 0 and null shape
  reject explicitly; the frozen setter is not silently treated as implemented.
- Actual writes are independently bounded by physical identity capacity. The
  scratch append bound and the post-run destination bound preserve full-batch
  semantics without exposing partial final state on shortage.

### Spatial wrapper and tree state

- Method choice prefers the first selected method, falls back to the second
  under mask bit 2, and compares method identities before reopening a second
  method. Reached unknown methods reject. Null-chain/global ownership is an
  explicit caller binding; it is not reconstructed from a guessed world list.
- Exhausting a method selects at most one replacement, opens it, and returns
  its first next directly. An empty newly selected method ends collection even
  when later zones exist. Fallback compares zone identity, not link identity.
- Disjoint open preserves old cursors and its false source return is ignored
  by the wrapper. Initialized stale state can therefore return a candidate.
- Finite coordinates are truncated to signed32 only after range checks; the
  original `value & -quantum` and max `+quantum-1` arithmetic is retained.
  Positive quantum is an explicit native boundary, not a source normalization.
- Matching cached open restores A8 from A4, clears AC/B4 and wraps the ushort
  epoch. B0 and blocked898 are preserved. A fresh search clears B0 and blocked
  state; a changed active search executes pool reset before fresh construction.
- Object stamps are shared ushort state, not an identity set. Equal epochs in
  two trees suppress the shared object; different epochs can return it again.
  `65535 -> 0` wrap remains visible. Group member state is separate from the
  outer group stamp except where object references genuinely alias.
- Iterative selection preserves child order 0..3, inclusive overlap, full
  subtree selection, postorder allocation and prepending. Thus traversal is
  parent first with reversed selected-child order. The caller-owned DFS stack
  has an ancestor-cycle guard and no recursive/native-stack growth.
- Ordinary initialized pool accounting matches the eight-byte source stride:
  active unused slots, then free entries, then spare blocks; reset chains active
  blocks before spare blocks, zeros active heads/free head/bytes, and sets used
  to capacity. Exhaustion remains the explicit `105870/305700` boundary. See
  R1/R3 for malformed topology and unexercised branches in the initial snapshot.

### Collection bounds and capped merge

- Tags 1/6 preserve cached center/radius bits while returning transformed
  center/half. Tag 3 consumes positive endpoint pairs and optional cached scene
  matrices, computes its own cached sphere, then still executes the identity
  transform tail. Returned center may differ from cached center for signed zero.
  The helper is additive 48S collection preparation, not substituted 48Q detector
  endpoint staging.
- The bounds helper preserves separate binary32 operation boundaries, sparse
  zero multiplies, final `+0` and `abs(z)+(abs(x)+abs(y))` order. Ignored W lanes
  remain outside numeric validation. Reached unsupported tags/nonfinite values
  and inadequate endpoint/binding backing reject before the result commit.
- `mergeCandidates48S` computes the first source prefix capped at 64 and then
  the second prefix capped by remaining room. Positive logical counts above the
  cap do not require or load later identity elements. Original count-only
  iterations are explicitly collapsed only for immutable nonvolatile inputs.
  Zero and repeated raw identity words survive, first-list order is preserved,
  and signed nonpositive counts consume no identities.

## Transaction and implementation constraints

Inspection found no heap allocation, growing container, retained pointer,
recursion, filesystem operation or original-code execution in the three runtime
implementations. Immutable world definitions remain borrowed. Only compact
mutable tables are copied into caller scratch. The explicit traversal stack is
also caller-owned.

Full declared writable spans, not merely active prefixes, are checked for
alignment, representable address ranges, mutual overlap and overlap with input
descriptors/tables/nested arrays. Read-only spans may alias. Actual backing is a
caller obligation of this C++ span API. On collection error, only scratch may
have changed; all destination tables and result remain uncommitted. On success,
declared state counts and the output prefix are committed, preserving destination
tails. The merge additionally validates all failures before any scratch write.

Native guard evidence should include late group physical-capacity failure,
late destination failure, output/scratch/descriptor/nested inactive-tail aliases,
invalid references, traversal-stack exhaustion, list cycles and malformed pool
topology, with bytewise final-buffer/result canaries. These properties cannot be
concluded merely from the original-instruction fixtures.

## Local ISA-extension audit

The inspected proof adds authentic instruction models, not helper replacements:
SD64, MULT with destination/HI/LO results, MOVN64, SLTU64, REGIMM BLTZ/BGEZ,
in-range CVT.W.S, and scalar-load upper64 extension. It continues fetching the
frozen words and takes ordinary branches/delay slots.

The LHU correction zeros bits 32..63 and preserves bits 64..127. That agrees
with the vendored PCSX2 `LHU` assignment to `UD[0]`. Signed LB/LH/LW extend the
low32 sign into bits 32..63; LBU/LHU zero it. The new BLTZ model tests bit 63,
not bit 31, matching the cited signed64 branch semantics. The 47-check audit
separately supplies poisoned upper32 values for load tests and mismatched
low/high sign cases for BLTZ, while executing the genuine SRL delay word.

This audit supports those local changes and the bounded fixture model. It makes
no claim that all inherited integer instructions or physical EE exceptional
floating modes are general-purpose emulation, and no unsupported claim about
the correctness or incorrectness of prior published outputs.

## Final reconciliation

The revised implementation was inspected after the initial findings:

- R1's reproducer is closed in code. Reached pool validation bounds and checks
  free/active/spare chains, verifies the active tail, rejects active/spare
  overlap and free entries that overlap spare/current-unused storage, and runs
  before acquisition/reset mutation. Selection also rejects re-acquiring an
  entry already in its newly built chain. The self-cycle regression is present
  in the final native guard suite. Distinct chunk descriptors also require
  disjoint full entry ranges; shared identity uses the same reference.
- R2 is closed in code: category modes other than 1 now scan the whole positive
  count after a match. Identity lists and category deny mode retain their source
  early exits.
- R3 is closed in the inspected 233-case source report. Free-pop, first-active
  spare, active/spare append and reset-append PCs are covered. Positive fallback
  and repeated links to the same zone are explicit fixtures. Added deep-full
  and mixed-subtree fixtures execute `265534` and check multi-level ordering.
- A nonnegative stale slot above four now skips the old item block as in the
  source, rather than being rejected. Dedicated original cases cover 5 and
  INT32_MAX. Negative reached slots remain an explicit safe topology boundary.
- The query's prepared bounds shape is now authoritative when that same object
  occurs as a group member, eliminating contradictory duplicate shape metadata.
- Later null method selections are now modeled under an explicit effective
  diagnostics flag. When enabled they reject with `MissingMethod`; when both
  source paths are initialized and disabled, they may fall through to fallback
  or return null without skipping additional later zones. Initial null method
  remains an unsafe dereference boundary. The appended original cases distinguish
  these paths; debug-global ownership or arbitrary lazy initialization is not
  silently synthesized.
- R4 is closed in code and the header contract: every declared object identity
  must be nonzero and unique. Zero remains reserved for null current/list values.
  The independent raw-word merge's zero-preserving behavior is unchanged.

The source proof is now stable at 239 cases, 224 natural returns and 15 reached
boundaries, with 282,725 instructions, 47 isolated ISA checks and 32 semantic
checks. Its fixture hash is
`a737d045269c03ec44bd71cefa90b895b3d63aacb169e6fb02fd913ab7c807e9`.

Final native artifacts are under
`evidence/candidateCollection48S/native-final2/`. This review read
`summary.json`, `release-report.json`, `sanitized-report-no-lsan.json`,
`input-hashes.json`, `file-hashes.json`, `jobs.json`, the sanitizer stderr logs
and compiler stack-usage output. All seven recorded input hashes still matched
the current runtime, header, fixture adapter, native test and source fixture.
All 196 recorded artifact hashes matched their files.

Both release and the successful ASan/UBSan run report:

- 239 fixtures, including 224 differential natural-return cases and 15 source
  boundary cases
- 391,437 checks and zero failures
- 1,197 native safety cases (the counter includes source-boundary rejections),
  including the early-capacity pool self-cycle and null-identity regressions
- 868 exhaustive declared-span overlap cases and 17 lazy/alias cases
- Zero observed ordinary, array, aligned or aligned-array allocations inside
  the guarded calls and serial measurements

The successful release and sanitizer jobs both returned zero. The first
sanitizer job returned one because LeakSanitizer cannot operate under this
executor's ptrace environment; its diagnostic is retained. The successful retry
explicitly disabled leak detection and enabled ASan/UBSan halt-on-error, and its
stderr is empty. LeakSanitizer did not pass and is not claimed.

All ten test-only semantic mutants failed as intended: capacity-before-next,
group-emits-member, group-inner-limit, stamp-dedup-disabled, cached-block-cleared,
children-reversed, direct-flags-not-set, group-flags-not-set,
tree-epoch-not-incremented and strict-sphere-touch. Their respective failed-check
counts are 50, 74, 26, 31, 17, 51, 1,490, 132, 1,927 and 2. These sensitivity
checks leave production code and original source words unchanged.

The inspected compiler output reports static frames, including a 1,056-byte
collector entry frame, 208-byte open frame and 40-byte pool-validation frame.
These are individual frame measurements, not a claim that their maximum alone
is the complete call-stack high-water mark. Traversal storage remains separately
caller-owned. Reported microbenchmarks are serial synthetic Linux-host helper
measurements, not gameplay or Xbox frame-rate evidence.

Residual boundaries remain explicit: live world/registration ownership,
arbitrary virtual methods, allocator growth, unsupported bounds and member tag 0,
physical EE exceptional floating behavior, and repair/certification of unconsumed
cached tails after early nominal termination. None is converted to a success
claim by this review.

## Integrated sweep and source-snapshot reconciliation

After the focused review, the parent ran the standard serial native runner
against the retained milestones and all three additive 48S targets. This
reviewer did not rerun any binaries. The completed reports were inspected on
2026-10-06 after the final sanitizer run ended at 09:04:22 UTC.

The sweep comprises 18 reports: release and ASan/UBSan reports for each of
`spring48s-regression`, `terrain48s-regression`, `producer48s-regression`,
`detector48s-regression`, `minmax48s-regression`, `type3query48s-regression`,
`detectorpipeline48s-regression`, `vehiclecontact48s-regression` and
`candidatecollection48s-final`. Sanitized directory names append `-asan-ubsan`.
These are build/report directories beside the source checkout, not runtime
source or frozen fixture inputs.

All 60 test-target invocations passed. Every report has empty `errors` and
`changed_inputs`. This reviewer independently reconciled all 153 distinct input
paths across these reports to the current checkout, and verified all 772
referenced artifact byte-count/hash records. There were no mismatches. The
reported retained-milestone outcomes are regression-test results within their
existing scopes; they do not expand those implementations' original claims.

Portable copies of the 18 report folders and their summary are retained at
`evidence/nativeCandidateCollection48S-linux-20261006/validation.json`.
This reviewer checked all 18 copied report hashes and independently summed the
checks from the actual per-test stdout records: **4,729,942 checks per mode**,
matching the portable summary for both release and ASan/UBSan. The summary
SHA-256 is
`1e91460d0eec3a3210d570bf39f30a6c99f5f9729b028dd41187694ed0175652`.
Its `host-process.json` additionally records a successful collector test process
with 25,756 KiB peak resident set; the recorded binary hash matches the root
release executable. That RSS includes JSON loading and test/diagnostic machinery
and is not runtime-only memory, a console memory budget or target performance.

Both final 48S modes passed the current source and fixtures:

- Merge: 274 fixtures, 323 focused operations, 40,903 checks, zero failures and
  zero observed runtime allocations; protected-page laziness passed
- Collection bounds: 254 differential cases, 19 boundary cases, 4,884 checks,
  zero failures and zero observed allocations
- Collector: 239 fixtures, 224 differential cases, 15 source boundaries,
  391,437 checks, zero failures and zero observed allocations

The standard runner retains separate strict floating flags, serial execution
and the sanitizer configuration with leak detection disabled. The final report
SHA-256 values are:

- `candidatecollection48s-final/report.json`:
  `62a18c0c7b6548c789641361aeacf3e5f3693bad69fb667d29952b982a3c6eed`
- `candidatecollection48s-final-asan-ubsan/report.json`:
  `8e1c5979f9c50eecb21ad0c3feaac63afc0a3df697d2aefbbc13520188b39b8c`

Before portable source packaging, the parent moved newly generated compiled
products out of the source evidence tree. This reviewer checked every entry of
`evidence/candidateCollection48S/generated-build-artifacts.json` (SHA-256
`c2a153e3129eea5c1d4808800f916c5b69ad5dbc79c3cf8be5ce853f66d630c4`).
All 49 recorded products are ELF executables or objects; each is absent from its
old source location and present with identical bytes and hash at the manifest's
preserved build-artifact location. None of those entries names a frozen original
binary fixture. Historical run manifests were not rewritten to hide the moves.

Consequently the earlier `native-final2/file-hashes.json` is a verification-time
manifest, not a claim that every compiled product is shipped in the source ZIP.
It still reconciles completely: 168 entries remain at their source paths and
28 compiled entries resolve through the relocation manifest, totaling all 196
entries with matching hashes. The seven focused-run inputs also still match
the current checkout. `native-final2/BUILD-ARTIFACTS.md` explains the packaging
boundary. Logs, reports, source mutants, stack records and frozen fixtures remain
available; generated binaries are rebuildable and are not required source inputs.

This reconciliation leaves the review conclusion unchanged: no remaining
correctness blocker was found within the documented bounded domain, and no
runtime source edit, build or executable invocation was made by this reviewer.
