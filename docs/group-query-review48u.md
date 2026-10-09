# Independent review of bounded outer-group type-3 query (48U)

Status: PASS within the documented bounded native domain. Final review frozen
on 2026-10-06 after original-byte, implementation, original-fixture, native,
mutation, full-regression, resource-scope and preservation reconciliation.
No unresolved blocking finding remains.

## Review boundary and original-byte audit

This review concerns the outer collision-list grouped branch of `122660` for a
query whose shape tag is 3. It does not imply support for candidate shape tag 1
(compound mesh records), full-world enumeration, registration, live scene
ownership, arbitrary callbacks, or a complete vehicle tick.

Read-only verification on 2026-10-06 checked the authorized ELF's SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Its first PT_LOAD is file offset `80`, virtual address `100000`, file bytes
`2D5A00`, memory bytes `34F600`. Every recorded instruction word and each block
hash in the following retained maps matched the file-backed ELF bytes:

- `type3Query48P/map`: 12 blocks, 4,931 words
- `candidateCollection48S/map`: 28 blocks, 2,955 words
- `groupShape48T/map`: 2 blocks, 638 words

These counts include overlapping evidence and must not be summed as unique
executed instructions. This audit reads bytes only; it is not original execution.
Both seven-word query switches also matched ELF bytes. Query tag 3 maps to
`122780` in the ordinary switch at `388B90`, and `122A10` in the grouped switch
at `388B70`. All 8,352 files in the saved 48T baseline manifest matched at the
start of review, with no missing files or changed hashes.

## Independently audited source behavior

1. Ordinary collision-tag matching at `1226C4` takes precedence. Only its mismatch
   reaches the group gate. `query.F8 & 1` then suppresses groups (`122898`), and
   `candidate+2` must equal the separate global group collision tag (`1228A8`).
2. The group coarse center is `(uint16(positionX) * worldScale, +0,
   uint16(positionZ) * worldScale, 1)`. Y is not part of this coarse test.
   `135F30` obtains a radius using signed scale11 and the selected group bounds:
   `base18C + (scale11 * (other194 - base18C)) / 128`, with source operation order.
3. `126E40` compares XZ distance by `sqrt(dx*dx + dz*dz) -
   (queryRadius + groupRadius) <= 0`. Equality passes. This is not the strict
   squared XYZ sphere test inside `126F40`.
4. The signed member count comes from `135CD0`; it is read at each loop test.
   Empty/negative original counts skip the member body. On each reached member,
   `135CF0` runs first, before the returned member becomes `query.F4` and before
   that member's F8 bit 4 is set (`122994..1229B0`). A null member shape makes
   `135CF0` return null, followed by an original dereference; a bounded API must
   reject that reached boundary, not manufacture a skipped member.
5. `135CF0` computes all three unsigned positions and both signed-scale ratios
   before its member shape-tag branch. The tag-0 route invokes the proven 48T
   setter; tags 4/5 mutate their compact fields, and other nonzero tags return
   the member unchanged after common arithmetic. Tag-0 shared shape matrices
   and each member's cached sphere have distinct ownership.
6. Grouped members do not take the ordinary `126EB0` outer sphere or `120E70`
   bilateral filters. The grouped secondary switch dispatches tag 3 directly to
   `126F40`, which still clears pair hit flags before member activation, tests
   the member cached sphere with strict XYZ inequality, and performs its own
   phase-specific result resets. Member identity/category/F8 eligibility from
   the ordinary path must not be added here.
7. A dispatcher hit sets the query hit flag, invokes the type-3 no-op finalizer,
   copies the query's prior +40/+50 vectors to the current member, then invokes
   query callback E4 with `(query, member)` before member callback E8 with
   `(member, query)`. Arbitrary member callback effects are not established by
   the numeric proof. The bounded static callback must consume the member's
   identity/category/name, rather than the outer group's identity or state.
8. The loops continue in source order after hits. Repeated member references,
   repeated group occurrences, and ordinary occurrences after grouped mutation
   must observe current shared physical state. A group's shape-tag-0 update may
   alter a shared matrix without refreshing a different member's cached sphere.
   The original is serial. No deduplication or pre-flattening may erase effects.

## API and runtime review to date

The canonical shape binding is directly source-backed. `11F7A0..11F7AC` loads
member+8C and then that shape's +114; center and half fields are read from the
first 36-byte node. `127234..127258` loads that same shape's +118/+114 into the
traversal pair and starts node tag 0. The fixture writes one node-array address
at +114 for both paths. Using the canonical mesh root here does not equate two
unrelated geometry objects.

`GroupQueryObjectState48U` owns cached sphere, flags and copied vectors for one
physical object. Shared shape-0 and shape-4/5 tables are separate. A shape-0
setter updates only the reached member's cache even when the shape matrix is
shared. Optional query-object binding synchronizes the prepared query sphere,
flags and copied vectors; a later self-member visit still enters the dispatcher
and may clear pair state after an earlier hit. It does not erase the cumulative
query hit flag. No ordinary member eligibility filter was found in this path.

Ordinary self-query visits do not need duplicate category/list equality: original
`120E70..120E7C` rejects identical query/candidate pointers before all filter
fields. The native helper likewise rejects equal identities first, and the
query-object binding requires that equality. Such a visit updates current state
and takes the outer sphere before rejecting; it never enters the dispatcher or
resets pair fields. Group-self visits intentionally bypass that ordinary gate.

The new runtime's guard covers 28 complete writable spans: both sides of nine
ray/length arrays, record storage, object state, shape-0 state, shape-4/5 state,
plus stack and result. Source descriptor objects, all initial capacities,
nested candidate/filter/name/mesh/member spans, shape tags and optional identity
ordering are checked before staging. Unvisited descriptors and inactive tails
are included. Public failure returns precede all destination copies.

The existing 48P source extraction was compared against its exact 48T baseline
snapshot (SHA-256 `a78d72482853fa3f6dd07295574ef9a6322814c35448fdb75cb5cc5afb473483`).
It only exposes borrowed sphere/mesh/matrix/shape-presence arguments and current
F8 eligibility; the old wrapper supplies its previous values and a present
shape. A review-found direct null-shape ambiguity was corrected before tests:
48U now rejects absent shape-0 backing only after activation, sphere and shape
reset gates. Group-helper null shape still rejects before common member math.
The new arithmetic helper also checks reached operand finiteness, rather than
only the result of an operation such as finite divided by infinity.

With an optional exact-count identity ordering, reference bounds and strict
current nonzero identity order prove a complete permutation on each call.
Without the ordering, duplicate identity validation is pairwise. The hint does
not skip present-value checks or grant lasting trust. Immutable per-shape 4/5
tags reject incompatible physical layouts without a cross-product scan.

Validation and staging are sized by the caller's supplied tables, not just the
members reached in this query. Guarding is linear in descriptor/table count
apart from the documented pairwise identity fallback; no mesh vertex array is
cloned. Each call copies all declared source ray/length/record slots and the
active object/shape table counts to scratch, then copies them to destination on
completion. Full capacities are guarded even though inactive destination tails
are not copied. The caller should supply compact relevant tables and prepare
optional ordering outside the runtime. No new pool-validation optimization or
console performance claim follows from these bounds.

## Final original execution review

The current source corpus has 128 cases: 120 complete native-domain projections
and 8 explicit unsupported cases. Of the latter, 3 complete in the original
(compound shape1, endpoint scene metadata and mode2 interpolation) and 5 stop at
precisely identified original pointer/callback boundaries. There are 123 natural
original returns, 917,559 original steps, 402 dispatcher visits and 303 query
callback invocations. These are source counts, not native replay totals.

The fixture SHA-256 is
`65d184ae5cb91b053fe6dd899d52d2b4a00740ef04c5777d4d52fd24b7794cf1`.
Read-only review verified that hash, the recorded generator hash, all 3,381
unique executed instruction words and all 63 loaded dispatch-data words against
the ELF, and zero source reads in every case. There are 296 passing source
semantic checks. Data memory begins as `b''`; the inherited reader rejects a
missing overlay byte, and per-case read audits require every read byte to have
been initialized or written. Callback/dispatch traces only observe execution.

The local REGIMM64/scalar-load-high extension was reviewed as an ISA operation
implementation, not a function replacement. Its saved audit replays all 245
retained 48P cases with every stored expected field unchanged. No earlier
interpreter file or expected fixture was rewritten.

Two late source additions make shared ownership discriminating: one ends after
a partial final group, leaving equal shared shape words but unequal per-object
spheres; another moves only member10 and its shared matrix, then queries member12
ordinarily with its stale sphere. The latter enters the dispatcher and resets
the result count from 1 to 0 rather than being filtered out by an incorrectly
refreshed cache. Query-self membership similarly preserves a prior cumulative
hit while resetting the final pair state and making the query its own current
identity. Unsupported member callbacks are stopped after the query callback,
including a null-query-callback case that still reaches the member boundary.

## Appended original cached-sphere control

The final addition is
`group-updated-far-sphere-direct-mask-retained`; all 127 earlier case objects and
all non-case fixture sections compare exactly to the retained
`before-direct-sphere-extension/native-fixtures.json` (SHA-256
`75df1b2e81926fc8422bee30735c03a27757ada8b2b67b2cb7994f7ac9744442`).
No earlier case was changed to make the native implementation pass.

The appended case executes 3,392 original instructions. A group moves member10
far in Y while passing the XZ coarse gate; ordinary member11 then hits; ordinary
member10's updated outer sphere rejects its final visit. Original dispatcher
identities are exactly `[10,11]`, with returns `[0,1]`. Final current identity is
10, query return is 1, and the full pair state equals member11's last dispatcher
state, including hit flag 1. This independently backs the cached-sphere
mutation's ordering property, without altering any old expected output.

## Native verification reviewed

The fresh final run is `evidence/groupQuery48U/native-final-r6`. Independently
checked all 31 input fingerprints against current files, its runner fingerprint,
and every output in `file-hashes.json`. Release and ASan/UBSan each passed
673,053 checks, with zero failures or instrumented allocations. LeakSanitizer
was explicitly disabled; no leak-check claim follows from these runs.

The final r6 loader changes only the query-object collision-list tag to the
explicitly zero-initialized original query+2 value, plus an explanatory comment.
Reversing those exact text changes reproduces the r5 test-source hash; all other
native input hashes are unchanged. Current grouped-self cases do not inspect
that collision-list tag, but retaining exact metadata prevents a later ordinary
self case from silently acquiring the global collision tag. No production change
was made for this correction. The final summary SHA-256 is
`3f08c58413f868114375a1493d3294ae1c35cbff73fccef353cf540eaebffc16`.

Both identity validation modes replay all 128 original cases: 240 complete
comparisons and 16 transactional unsupported cases. The reported 3,928 safety
calls include those 16 unsupported replays and 3,633 alias probes. The harness
compares every represented query/ray/spring/record/object/shape output, stale
slots and destination guards. Shared shape initialization uses actual original
pre-execution words. Query-object sphere, flags and copied vectors have separate
comparisons. Owning templates/filters, descriptors and mesh/name backings are
snapshotted outside allocation-instrumented calls.

Reviewed the corrected far-end descriptor tests, nonempty filter/vertex-normal
backings, inactive source capacities, and 13 additional backed writable-tail-only
probes whose active prefixes are disjoint. Serial reuse makes 7,500 calls, then
checks exact original final state plus immutable inputs and guards. Caller
scratch for that fixture is 3,864 bytes, including unused declared capacities;
this is not a general memory bound or target-console measurement.

All 29 semantic mutants compiled and linked with strict warnings and failed
normal assertions with exit 1. Successful production/test builds have empty
compiler/linker diagnostic logs. The final jobs comprise 83 successful commands
and 29 expected assertion-failure commands. Controls cover group/ordinary
precedence, query suppression, inclusive XZ bounds, helper-before-activation,
filter bypass, group-owned translation, object cache/shared shape distinctions,
shape4/5 fields and type domain, stale ordinary cached sphere, member dispatch,
query/self/callback ownership, exact hint validation and transaction/commit.

Earlier attempts remain archived, not presented as passes: an output object-name
collision, a mutant-only misleading-indentation compile failure, and a surviving
ordinary-cache-source mutant. That survivor exposed a test gap rather than a
runtime bug. A separate synthetic native guard-order assertion now obtains an
ordinary hit between grouped movement and a later outer-sphere miss; it checks
that the miss preserves prior ray flags. The corresponding mutant now fails.
The separate safety assertion remains labeled native safety coverage. The final
appended original case independently establishes the same effect; the mutant now
fails three checks, including two original-backed comparisons. The 127-case r4
run is retained as an earlier successful corpus, not the final fixture-matched
validation result.

## Root regression and measurement audit

Reviewed all 22 reports under
`evidence/nativeGroupQuery48U-linux-20261006`: 11 suites in release and again
with ASan/UBSan, totaling 66 test executions. Every report/test/build passed,
with no changed inputs. Independently checked 650 recorded source/dependency/
fixture fingerprints and 1,574 total input/log/object/executable/stack artifact
fingerprints against their bytes; no mismatch or missing artifact was found.
Actual stdout check totals are 6,711,303 per mode. The retained 48P query and 48Q
context/pipeline paths pass against the extracted borrowed dispatcher.

Exactly three old terrain compiler warnings occur in each mode: two in
`import_collision48g.cpp` and one in `collision_tree48i.cpp`, all misleading-
indentation warnings in unchanged retained files. No new source/test warnings
were observed. LeakSanitizer remains explicitly disabled. CMake execution,
Windows and Xbox builds were not run; build wiring was inspected only.

The separately measured release test process peaked at 71,348 KiB RSS. Its
binary, fixture, stdout and empty stderr hashes match the host-process report.
That figure includes JSON loading, relocation, safety cases, diagnostics and
benchmark infrastructure; it is not runtime-only memory. The sanitizer compiler
peak is another build-process measurement, not runtime storage.

The root regression benchmark's median batch mean is 5,456.902 ns, with a maximum
batch mean of 6,117.992 ns, for 7,500 calls. The separate RSS measurement run has
a median of 5,235.912 ns; the focused r6 run has a median of 5,378.391333333333 ns.
These are distinct host runs, not conflicting measurements or target frame-time
claims. All use the same 3,864-byte declared scratch fixture. Per-function frame
estimates must be reported separately; they are not summed into a live-stack
bound.

## Final freeze and preservation

The final aggregate validation SHA-256 is
`e8c739571f5718726d74951904ddb36b272f88a92401cc02cf10290934845dc1`;
the host-process report SHA-256 is
`b4bd79f8beeed6a28864e72cee8c79f2442093333d32454ba6e5e05911db3119`.
Both were checked against the actual final files. Their frame metadata matches
the release `.su` artifacts: entry 1,696 bytes, borrowed dispatcher 592, callback
304 and dispatch wrapper 112 (dynamic bounded). They are individual host compiler
estimates, not a maximum nested stack or target-console ABI budget.

A complete rehash of the 8,352-file saved 48T manifest found no missing file.
Exactly three existing files changed: root `CMakeLists.txt`, `README.md` and
`src/game/type3_query48p.cpp`. All retained tests, original fixtures, interpreter
sources and historical evidence therefore match their 48T bytes. New 48U files
are additive. The old dispatcher source change was separately reviewed against
its exact baseline snapshot; old entry semantics also passed the retained suites.

The final README, `docs/group-query48u-linux.md` and
`outputs/RacerRevenge-Checkpoint48U-GroupedQuery.md` consistently distinguish this
standalone grouped query from the still-unwired detector/vehicle composition.
They keep live ownership, compound shape1, endpoint metadata, alternate reached
interpolation, dynamic/member callbacks and remaining vehicle stages outside
the claim, and distinguish tool-process RSS from caller-owned runtime scratch.
Historical incomplete attempts remain visible and excluded from pass claims.

The review is frozen for archival. No source or test edits were made by this
reviewer; findings were resolved by the source/proof/test owners and revalidated.
The implementation is accepted for its explicitly supplied, finite, serial,
transactional grouped-query projection only.

## Efficiency and scope caveats

This review requests no new efficiency work. Existing collection validation still
has repeated free-list/spare-block and selected-entry duplicate scaling costs;
48T's tail comparison and caller-provided exact-revalidated ordering hints do not
remove those costs. Bounded, serial, allocation-free is not a console frame-rate
or end-to-end gameplay claim. The reference tables are explicit synthetic/caller
inputs; no live-world ownership or persistent scene binding has been verified.
