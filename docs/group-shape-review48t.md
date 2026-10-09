# Group shape-0 setter and collection integration review, 48T

## Scope and status

This is an independent source, contract and evidence review of the bounded
`135CF0` tag-0 group-member route through `11F680`, plus its additive integration
with the 48S collector. It does not claim live world ownership, arbitrary shape
dispatch, shape-0 query bounds, spatial registration, game execution or physical
EE/VU exceptional-mode equivalence. The reviewer changes documentation and
read-only verification evidence only, not runtime code or native tests.

The runtime and completed evidence review found no correctness blocker within
the declared finite, caller-owned projection. The independent original execution
packet, focused release/ASan/UBSan results, semantic negative controls and full
integrated retained-milestone sweep have been reconciled below. No native pass
is inferred merely from source inspection.

## Independently checked original provenance

On 2026-10-06 the reviewer read the actual authorized ELF and compared its bytes,
the preserved 48S raw blocks, and the frozen per-instruction JSON:

- ELF: 2,972,720 bytes, SHA-256
  `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
- Frozen instruction map: SHA-256
  `05ce6db129c12888b32c7892526a957c745e6108e5bf1d602d913c711f735863`.
- Frozen dispatch data: SHA-256
  `8ee2d9c392e9e54994ff8d6f0ca0060c361b85135bcfeb1ffbfc81fa27556d30`.
- `135CF0..135F24`: 141 words, raw block SHA-256
  `f394d1c539b4f2ace96adc2ff68ac2ce5f49408a0a03fd0870ce1d400991cab4`.
- `11F680..11FE44`: 497 words, raw block SHA-256
  `862cb3238b2fce7e8368ecce5b9ddd8798fe56a60d6df883515f2307a92e7a42`.

The complete raw blocks equal both the JSON words and file-backed ELF bytes at
`VA - 0x100000 + 0x80`. The actual switch at `388A00` selects `11F6C8` for tag 0.
The setter block includes unsupported sibling tags, but their presence in the
source map is not a native implementation claim.

The initial baseline reconciliation checked all 7,062 files recorded in the 48S
baseline manifest. None was missing. Only the current collector runtime/header,
build wiring and native test/adapter wiring differed; no preserved evidence,
original fixture or source-proof generator had changed at that checkpoint.

## Exact source order and numerical contract

`135CF0` first resolves the member and its nonnull shape. Common XYZ and both
ratio calculations precede the tag branch. Even tag 0, which does not store the
ratios, must retain reached arithmetic and its finite-domain rejection. The two
ratios have distinct subtraction orders; tag 0 is not permission to skip them.

The tag-0 branch copies all 16 words from shape `+80` into a stack matrix. It
replaces only translation XYZ, preserving translation W. Crucially,
`135EF8/135F00/135F0C` also write new XYZ to the shape's `+B0/+B4/+B8` before the
setter starts. Thus `11F6D8..11F6F4` copies an already-translated current matrix
to `+C0`. Previous `+C0` must contain the new translation, not the old one.
The following loop copies the incoming matrix into `+80`.

To avoid matrix-layout naming ambiguity, the composed matrix formula is stated
by storage indices. Let `M` be incoming/current `+80` after translation and `L`
be local `+40`. For each vector `r` and output lane `c`, both ranging 0..3:

`C[4*r+c] = (((M[c]*L[4*r]) + M[4+c]*L[4*r+1]) + M[8+c]*L[4*r+2]) + M[12+c]*L[4*r+3]`.

Every multiply and addition has a separate binary32 rounding boundary. All four
terms and all XYZW lanes are consumed, including zero terms and non-affine W.
No identity, sparse or rigid-matrix shortcut is justified. `11F790..11F79C`
writes the four composed vectors only after all four have been computed.

The geometry pointer at shape `+114` provides center XYZ at geometry `+0C` and
half XYZ at `+18`. The member cached center is first copied from geometry and
given W=1. Radius becomes `sqrt((halfX*halfX + halfY*halfY) + halfZ*halfZ)` with
the source's separately rounded arithmetic. This radius is unscaled. The final
center is transformed using `M`, not the composed matrix `C`:

`center[c] = ((M[c]*x + M[4+c]*y) + M[8+c]*z) + M[12+c]`, for XYZ only.

The final center W is positive 1. Matrix W inputs do affect the full composed
matrix, even though this point-center operation consumes only XYZ lanes.
Signed zeros are not normalized. Prior composed/previous matrices and the three
overwritten translation lanes may contain arbitrary bits because they are not
numeric inputs to the completed route.

The native helper computes into fixed local results and commits after all
numeric validation. Its final externally visible values follow this source
order; it does not expose intermediate writes, instruction tracing or partial
state on failure. This is appropriate for the declared immutable-read,
transactional compact projection.

## Collector integration and shared identity

The optional shape-source and shape-state tables are absent by default. A zero
shape reference retains the prior explicit tag-0 unsupported boundary. A
reached nonzero out-of-range reference is invalid topology; a reached missing
geometry is an unsupported member shape. Reached finite arithmetic failures
remain `NonFinite`. Inactive references and numbers are not eagerly interpreted.

One shape reference denotes one mutable shape state. Repeated group members or
distinct objects sharing that reference read the preceding update and mutate it
serially. Member cached spheres remain object-indexed: updating a shared shape
for one object does not retroactively update another object's cached sphere.
Shared immutable geometry pointers are permitted.

The query's prepared bounds shape remains authoritative when that same object
appears as a group member. A stale duplicate object shape tag or shape-0 reference
does not redirect a supported query bounds shape into this new setter. Tag-0
query bounds themselves remain unsupported, because this setter does not close
the separate `1202E0` tag-0 route.

Group outputs remain repeated outer-group identities. The setter runs before
current identity, member flag and append effects. Group expansion still lacks an
inner nominal-capacity check, and the outer iterator advances before checking
nominal capacity. A later direct occurrence therefore sees the member sphere
actually written by its earlier group occurrence.

Host read-only span aliasing means aliased immutable snapshot storage. It does
not establish support for original guest geometry or local-matrix reads that
alias mutable shape/object fields written during execution. Such original
read/write aliases require explicit modeling rather than a second independently
copied projection. Shared supported mutable shape identity uses the same table
reference.

## Transaction, backing and resource review

The standalone helper checks full destination/sphere objects against one another
and every input object, including the optional geometry. The collector's guard
was expanded from 14 to exactly 16 writable spans: seven scratch spans, seven
destination spans, traversal stack and result. All declared capacities, including
inactive state tails, are checked for alignment, representable ranges and
overlap. Shape-source and initial-state tables and every nonnull geometry object
participate in the read/write disjointness check before scratch initialization.

The new shape count participates in reference-width and capacity checks. Only
the declared count is copied into scratch and committed, so destination tails
are preserved. On any failure, including late identity capacity or late setter
failure, destination shape matrices, existing destination state and result bytes
remain unchanged. Actual backed C++ storage is a caller obligation; address
arithmetic checks do not prove allocation provenance.

Inspection found no allocation, recursion, native thread, retained pointer or
external access added by the setter/integration. Geometry/local data are borrowed;
only the compact mutable three-matrix shape projection is mirrored. Stack usage,
allocation instrumentation and performance results must be taken from the final
native compiler/run evidence rather than estimated from this source inspection.

## Independent source-execution packet review

The initial 48T packet contains 187 helper and 110 collector cases, 293 natural
returns, four reached boundaries and 264,764 recorded instruction steps. Of the
helper cases, 92 exercise the native group-entry contract (91 returns and one
missing-geometry boundary); direct arbitrary-matrix setter cases deliberately
remain original-only evidence. The collector has 108 returns and two boundaries.
These counts distinguish source coverage from the subset actually comparable
through the public native API.

The reviewer inspected the generator, inherited interpreter and passive trace
hooks. Expected outputs come from actual fetched original instructions over
explicit synthetic initialized bytes. No native implementation, native expected
result generator, forced helper return or patched instruction is imported. The
new generator adds passive read/write tracing but no ISA implementation. Its
read audit requires all execution-read bytes to have been initialized or already
written and records no guest RAM/source-image reads.

All 1,050 recorded executed words match the authenticated frozen map. The new
fixture hashes independently reconciled to the proof report:

- Helper: `5e6eba1dac3b96a764cce2e19bdac3c6a8c4d90b3169bd0b799aaa326e7d4511`.
- Collector: `0da0156a0a6675dd8b3b0311b312e0267182239dc5edab93610de6b9abae96d0`.

The corpus includes noncommuting shear/full-W matrices, randomized all-lane
matrices, signed zeros, unscaled radii, dirty metadata preservation, repeated
members, shared shape and geometry backing, two-group order and direct-object
occurrences before/after expansion. The 33 semantic checks explicitly separate
group previous-cache behavior from direct-setter previous-cache behavior and
check the pretranslation/cache/composition write ordering.

The proof report also records exact re-execution of 239 preserved 48S cases and
338 preserved 48R numerical cases using the unchanged interpreter. This reviewer
verified the recorded interpreter hashes and prior fixture fingerprints against
current files, without independently rerunning the oracle or native binaries.

## Pool-tail optimization review

The active/spare cross-product is replaced only after both chains pass their
existing bounded-reference and cycle guards. Over one stable singly linked
`next` table, any common node forces all subsequent nodes to be common, so two
finite chains intersect exactly when their nonzero terminal references match.
The implementation still checks that the active terminal equals `activeLast18`,
then rejects equal nonzero active/spare terminals. It accepts two empty chains
and does not mistake one empty chain for an intersection.

This retains the old rejection domain without a marking table or allocation.
The free-entry/spare-range checks, active-unused-suffix checks and newly selected
entry-duplicate checks remain unchanged. The theorem relies on the existing
serial, stable-during-call storage contract. No broad linear-time query claim
follows: remaining pool, selection and immutable-table checks have their own
costs.

## Optional validation-order review

The added workspace identity/block orders are optional borrowed reference arrays,
not cached certification tokens. Each independently defaults to the original
pairwise path when pointer and count are both zero. Otherwise its count must
equal the entire corresponding current world table count. Its full declared
span is checked against every writable span before it can be consumed.

For identity order, each reference must be in 1..objectCount and the current
referenced identities must be strictly increasing from zero. A repeated reference
would repeat its current identity, violating strict order. Therefore exact count
plus in-range distinct references proves a complete permutation; duplicate or
zero identities cannot be hidden by omitting an object. No extra marker storage
is needed.

For block order, every reference is range-checked and its current key is
`(entryCount ? firstEntry : 0, reference)`. Strict lexicographic increase likewise
proves a full permutation, including empty descriptors. The old full range
validation still executes for every nonempty descriptor, and the sorted pass
checks each nonempty interval against the preceding nonempty interval's end.
This establishes full range disjointness. Empty descriptors ignore their unused
firstEntry just as before, while their reference tie-breaker prevents duplication.

Changed identities, ranges or order arrays are revalidated on the next call.
Compatible updates remain accepted; stale invalid orders reject before scratch
initialization. No immutable-across-calls promise or ownership subsystem is
assumed. All borrowed descriptors, references and geometry must remain stable
during the serial call, under the same lifetime contract as existing inputs.
Reached mutable pool/topology guards are retained. These hints reduce immutable
uniqueness/disjointness checks to linear passes, not the entire query to linear
time; neither a hint nor its absence changes traversal/output order.

## Findings and disposition

1. The legacy focused runner initially omitted the new group-shape object from
   mutant link commands. This would leave the modified collector's helper call
   unresolved. The root corrected the link list; the source was rechecked.
2. A proposed successful helper alias test cast an actual matrix object to a
   different geometry-object type and dereferenced it. This did not satisfy the
   backed C++ object contract and could violate strict aliasing. The test author
   replaced it with the actual geometry.localCenter Vector3 subobject also
   serving as translation input. The corrected case is valid read-only sharing;
   the source was rechecked. This was a test-validity issue, not a runtime defect.

## Final focused native evidence reconciliation

The reviewer inspected `evidence/groupShape48T/native-final2/` after the final
test changes on 2026-10-06. All 14 recorded current input hashes, all 230 retained
artifact hashes and the runner's own hash matched. The summary SHA-256 is
`19edc5ad597a5f09672f832cd73de9b8987cfd807cc83b807c382f8f2a7920ee`.

Both release and the successful ASan/UBSan mode report:

- Helper: 2,886 checks, 91 original-derived completed differential cases, one
  original boundary case, 56 native safety cases and 10 declared-span overlap
  cases; zero failures and zero observed allocation calls.
- Collector: 1,305,422 checks, 432 completed differential invocations of 108
  original worlds across four independent hint modes, eight boundary invocations
  of two original boundary worlds, 369 native safety cases and 268 overlap cases;
  zero failures and zero observed allocation calls.
- Combined new focused checks: 1,308,308 per mode. Repeating one original world
  under different validation modes does not create additional source fixtures.

The inspected final tests cover every new full writable span against inherited
and new spans, source/scratch/destination shape capacities and inactive tails,
late member/geometry/numeric/output failure, query-shape authority, overwritten
nonfinite state and unreached numeric laziness. Hint tests include all four
presence combinations, duplicate/missing/out-of-range references, stale current
identities/ranges, valid current edits, present-nonnull/count-zero empty tables,
all-empty descriptors, touching nonempty ranges and changed nested spans that
alias a writable tail despite otherwise sorted hints. Pool tests cover empty,
disjoint, shared-head/shared-suffix, invalid-tail, cyclic and invalid-link paths
both with and without hints.

All four successful native run jobs returned zero. Both sanitizer stderr files
are empty. The runner uses strict floating flags, one CPU and serial commands.
LeakSanitizer is explicitly disabled; it did not pass and is not claimed.
The 95 helper executions outside the group-entry API remain original-only.

All 13 source mutants compiled and linked successfully, ran with normal exit 1,
and produced positive ordinary assertion failures. The reviewer cross-checked
their source hashes, report failure counts and job exits; no sanitizer error,
harness exception, timeout or crash was accepted as a semantic failure. The
failure counts are 368, 69, 107, 94, 68, 66, 91, 40, 2,254, 1,127, 2,407, 2 and
32 in the order listed in `docs/group-shape-tests48t.md`. The center/composed
matrix mutant changes the first center multiplication term; it is not a claim
to replace every term in the center calculation.

Final report hashes:

- Release helper:
  `c06e984e0c93a90dc14a033939ef6da4faf17561fd5341b3e3a0924368ec5d2d`.
- Release collector:
  `a5e6212448de5658b46eae8c21e21f283475b5c5b49680fff82d7adad9721a08`.
- ASan/UBSan helper:
  `1ecf8382ecdaea5f7cf864cea2909ccf5d78c75910dcde5b80d13bbfc2450e6c`.
- ASan/UBSan collector:
  `a132a6364ef4647d884fd4cd6554da51139d32f9de83733943dfb703f12a6833`.

Compiler stack output reports a 464-byte static helper frame and a 1,408-byte
static collector-entry frame for this build. These are per-function estimates,
not their sum asserted as the complete peak call stack. Caller-owned traversal
and shape scratch remain separate. One mutable shape projection is 192 bytes
on this host; the three-shape case uses 576 bytes per state buffer. The 1,344-byte
declared scratch in that test includes four additional canary slots. Reported
maximum child RSS includes compilation/harness costs and is not runtime memory.

The efficiency evidence was separately reconciled: all 17 baseline/current
artifact fingerprints and all 11 current input fingerprints match; all 52
current hinted/unhinted rows report zero failures and allocation calls. Its
current manifest SHA-256 is
`5e67f1a3f52c9e3b56704e075144536bc1ea3225b8ba2bb9b825d1615cadacfd`.
Those are synthetic Linux timings excluding caller setup. They neither imply
console FPS nor remove the residual repeated free-entry/spare-range cost.

The reviewer performed no build, native executable invocation, oracle replay,
game run, PC/RAM access or runtime edit. CMake and Windows execution remain
unverified unless a later explicitly reported platform run establishes them.

## Supplemental hint sensitivity reconciliation

The separate `evidence/groupShape48T/hint-sensitivity/` packet freshly builds
the final collector and three additional source mutants without editing the
focused final2 inputs or evidence. Its ordinary baseline passes the same
1,305,422 collector checks. Removing identity strict order, block strict order
and hinted interval disjointness produces respectively 36, 18 and nine ordinary
assertion failures, with normal exit 1 and zero observed allocations. No crash,
timeout, harness exception or sanitizer diagnostic is counted as detection.

The reviewer checked all 14 current input hashes, 67 supplemental artifact
hashes, seven freshly built object/source provenance records and all 231
preserved final2 files. The supplemental runner hash also matches. Its summary
SHA-256 is
`f108408e89e788317f17ff0f5877d0bff46b0c47570c25b664c6211b8ac8e9f9`.
This brings semantic negative controls to 16: 13 original focused controls plus
three separately retained hint controls. The supplement is not an additional
sanitizer run or new original-execution corpus.

## Final integrated sweep and source-snapshot reconciliation

The root completed a serial 20-report sweep, with release and ASan/UBSan modes
for spring, terrain, producer, detector, min/max, type-3, detector pipeline,
vehicle contact, 48S collection and the two additive 48T targets. All 64 test
invocations passed. Every report has empty errors and changed-input lists.

The reviewer independently verified:

- All 20 retained report hashes.
- All 163 distinct input byte-count/hash records against the current checkout.
- All 832 referenced artifact byte-count/hash records, including built objects
  and executables outside the source tree.
- All 666 retained portable log/stack copies corresponding to those records.
- Exactly 6,038,250 checks per mode, summed independently from actual test stdout.
- Three existing terrain compiler warnings per mode and no warnings in the
  other suites, matching the aggregate's qualification.

The preserved 48S collection suite still passes 437,224 checks per mode:
40,903 ordered-merge checks, 4,884 bounds checks and 391,437 collector checks.
These regression results retain their earlier documented scopes; they do not
expand prior milestones into full-game or live-world claims.

The final portable aggregate is
`evidence/nativeGroupShape48T-linux-20261006/validation.json`, SHA-256
`bf1e8773678a1cbbed9799eaf0452b981f6c654b397308636abbf30623e2f6f4`.
Its focused and supplemental summary references match the exact hashes above.

The separate whole-process RSS run returned zero, repeated 1,305,422 collector
checks and observed zero hot-scope allocations. Its executable matches the
standard final release report, SHA-256
`8d3cb5be7e39fc590eb258675933bd61de7d73df0e7f2ef687019cca52f5a39f`.
All four recorded executable/fixture/stdout/stderr fingerprints reconcile.
`host-process.json` SHA-256 is
`3bc0a815e6172a571bcaa7e08f9ac5fdaa7cf95c9f5d4f47325f3d6a2bd5c96b`.
The 15,020 KiB peak RSS includes JSON loading, relocation, diagnostics, guard
tests and timings; it is not a runtime-only budget, console memory measurement
or peak native-stack measurement.

The final baseline audit again checked all 7,062 recorded 48S files. None is
missing and no prior evidence or fixture changed. Exactly eight existing files
differ: README, top-level CMake wiring, the two 48S CMake files, collector header
and implementation, and the two collector runner files. New 48T evidence is
additive. Compiled products remain outside the new source evidence tree.

## Final conclusion

Both identified review issues were corrected. Source order, full-lane numerical
behavior, shared-shape identity, query-shape authority, optional legacy defaults,
atomic full-span backing guards and per-call hint validation are supported by
the inspected implementation and retained evidence. No remaining correctness
blocker was found inside this bounded domain.

Residual limits remain explicit: general direct setters, shape-0 query bounds,
other grouped/compound dispatch, arbitrary mutable guest aliasing, external pool
growth, live registration/ownership, physical exceptional EE/VU modes and the
complete vehicle scheduler. Remaining repeated pool scans can still be cubic.
CMake, Windows, Xbox, rendering and gameplay execution were not validated, and
LeakSanitizer was not run. None of these limits is hidden by the native pass.
