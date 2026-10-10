# Independent vehicle-contact review, 48R

## Scope and verdict

Read-only review of `vehicle_contact48r.{h,cpp}`,
`body_contact_list48r.{h,cpp}`, their original-word proof and native checks.
The reviewer changes only this report and performs no build, PC access, RAM
read, installation, original-file edit, or scheduler integration.

No runtime semantic defect was identified in the bounded implementation.
Acceptance is limited to the documented finite arithmetic model, bounds tags
1/6, explicit supplied candidate order, ready/dirty-parentless scenes, and the
authenticated `165430` callback. This is not a completed vehicle substep.

Final fixture-expansion and test-manifest reconciliation passed. No unresolved
runtime findings remain in this reviewed bounded scope.

## Authenticated source

An independent read-only check hashed the authorized
`work/native/project/disc/SLUS_202.68` to
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Its ELF32 little-endian program headers were parsed rather than assuming a
file offset. All 1,857 frozen instruction words in 11 blocks and all six frozen
dispatch/data arrays match their file-backed load segment and saved binary
hashes. The nonempty segment is `offset=0x80`, `vaddr=0x100000`,
`filesz=2972160`, `memsz=3470848`, `flags=7`, `align=128`. A second zero-sized
load segment does not back any reviewed extent.

The reviewed source evidence is `evidence/vehicleContact48R/map/`, including
the full handler, helpers, caller, body-vtable slots, bounds switch and initial
data. Authentic words do not establish execution coverage of every mapped
branch. Unsupported branches remain explicit boundaries.

## Source-to-native semantics

- Both one-time static initializers precede mandatory position/axes cache
  preparation and the signed candidate-count gate. Nonzero flag bytes preserve
  their raw values; initialized scalar word two remains opaque. Empty and
  negative counts still refresh a dirty parentless position scene.
- Ready caches ignore parent presence. Dirty parentless caches copy all 16
  words and set bit zero. Shared position/inverse scene indices coalesce one
  mutable cache; the second inverse call observes the first refresh. Dirty
  parent traversal is not invented.
- Mask rejection occurs before shape, radius, inverse-scene, body and spring
  semantic consumption. A strict overlap miss does not consume the inverse
  scene or callback bindings. Structural whole-span/identity validation is
  intentionally earlier and is a native safety condition.
- Bounds tags 1/6 compute even the discarded temporary center. Sparse-axis
  transforms retain all three products and explicit zero terms. Half-extents
  retain `abs(Z) + (abs(X) + abs(Y))`. The candidate's cached center is separate;
  cached radius is not read.
- Scalar length squares retain `(X*X + Y*Y) + Z*Z`; VU dots retain their
  explicit final `1*Z`. The lateral vector is `D - dot(D,U)*U`, without a
  normalized-up assumption. Overlap is strict `<`, with no positive-radius
  or zero-distance gate.
- Normal construction uses the separately recomputed VU dot, rounded square
  root, rounded reciprocal and per-lane multiplication. It does not divide
  each component by the earlier scalar distance. Zero magnitude uses the
  inherited maximum-finite reciprocal model and is not physical EE/VU
  exceptional-mode certification.
- Inverse-point arithmetic retains the transposed basis's zero additions and
  computes transformed point and negative transformed translation separately.
  Four existing `rotate37` fixtures already distinguish this from transforming
  `point - translation` first. Generated W values are one; raw unused W/cache
  words are not eagerly rejected.
- Main callback precedes other callback; both precede record append even when
  append is saturated. A shared/self body index operates on one list, so the
  second callback sees the first insertion. Distinct native body entries may
  not label the same identity. Reached identities are nonzero because the
  spring-record identity reserves zero for absence; standalone `165430`
  still accepts zero as an opaque by-value contact payload.
- `165430` checks signed fullness before deduplication, preserves unused slots,
  supports positive overfull no-op states, and rejects unsafe negative counts
  as a native guard. Separate `165420` clears count only for every signed bit
  pattern; it is not called by this handler. The mapped caller invokes it at
  `219E94`, after body integration at `219E80`.
- `2963E0` preserves record margin/tail words and unrelated spring fields;
  only accepted appends change count/last material. The ten negative-capacity
  source cases naturally saturate, but native 48N rejects them explicitly.
  They are not bitwise native-completion successes.
- Natural `2197FC` return is separate from a stopped boundary. Neither handler
  nor callback fabricates an original Boolean result from residual `v0`.

## Safety and resource review

All seven writable spans (three scratch arrays, three destination arrays and
the result) are checked for full-declared-span pairwise overlap and overlap
with all nine read spans (the four descriptors and five input arrays). Array
checks include inactive/excess declared tails. Null nonempty, misaligned,
size-multiplication-overflow and address-wrap ranges are rejected before any
scratch copy. Backed, live C++ object storage is still the caller's obligation;
integer range checks cannot validate arbitrary foreign memory mappings.

The implementation stages complete source projections in caller scratch and
commits only after the whole ordered loop completes. Every non-`Complete`
status leaves all public destination arrays and the entire result unchanged.
Scratch on failure is intentionally unspecified. This is a native transaction,
not a claim that the original rolled back a reached prefix. Reached append
requires backing for the complete logical spring capacity even if saturated.

The runtime has no heap allocation, containers, threads, recursion, retained
references or mutable globals. Production object undefined symbols are only
the two bounded helpers, `memcpy` and `sqrt`. Body identity uniqueness is an
allocation-free quadratic scan over the declared body count; callers should
keep that supplied set appropriately bounded. Source projections scale with
declared counts and use caller-owned storage. The observed release handler
frame is 864 bytes; helper estimates are 8 bytes. These are compiler
per-function estimates, not a measured total peak stack bound.

Strict build flags prohibit contraction/fast-math. The native test explicitly
uses round-to-nearest. Arbitrary host rounding modes, denormal/exception modes,
physical EE/VU behavior, Windows/x86 compilation and console timing are outside
the reviewed Linux proof. Finite input/intermediate guards are native safety
policy, not extra original branch conditions.

## Findings and disposition

1. An early handler wiring test still expected a reached zero body identity
   to succeed after the runtime introduced the sentinel guard. It was replaced
   with transactional primary/other zero-identity rejection checks, plus lazy
   skipped/missed zero-identity success checks. Standalone zero-payload proof
   remains intact. The header documents the handler restriction.
2. All 298 initial fixtures used `(0,1,0)` for numerically consumed position
   up vectors. The independent original-word worker added 40 genuine-entry
   cases for tilted, nonunit, negative and cancellation-sensitive up vectors,
   crossed with prepared/dirty, shared/separate inverse scene, and hit/miss.
   All prior 298 case objects are preserved exactly. Independently checked
   diagnostics match original trace values: 20 new hits, 20 new misses; 24
   cases distinguish dot reassociation, 32 distinguish normalizing up, and 32
   distinguish a Y-only projection. Normalizing up would also flip acceptance
   in eight cases. This coverage gap is closed.
3. Caller round-to-nearest and exceptional-mode limits were made explicit in
   the header after review. This is documentation only; source fingerprint
   reconciliation must include that change.

## Earlier evidence and retained history

Standalone callback evidence in `evidence/bodyContactList48R/` matches all six
current manifest input hashes. Release and ASan/UBSan report 3,260 insertion
fixtures, 1,024 count-only reset fixtures, 96,803 checks and no failures or
runtime allocations. Six disposable-copy mutants are detected: capacity nine,
dedup disabled, zero rejected, full misdiagnosed, reset clearing slots and reset
omitted. These are bounded helper results, not vehicle scheduling evidence.

The interim 298-case handler proof has 284 natural original returns and 14
source boundaries. Native classification is 274 bitwise completions, 14
transactional boundary rejections and 10 explicit negative-capacity guard
divergences. Three completion cases are the separately labeled zero-normal
finite-model diagnostics. Release and ASan/UBSan runs both passed with zero
runtime allocations and 168 full-span alias checks. The latest interim
sanitized harness has 19 wiring cases and 372,158 checks; its manifest matched
current inputs when read. The earlier release run had 17 wiring cases and is
not the final test-source fingerprint.

These interim totals are retained as review history. The final evidence below
supersedes them and covers the later fixture/header changes.

## Final evidence reconciliation

The expanded corpus contains 338 cases, 324 natural returns and 14 boundaries.
Its SHA-256 is
`b227f70f85c8820f661dd1f9b045943743513dddbfecc8d53cfe4c8e4b49a838`.
The 40-case position-axis diagnostics are independently checked against the
corresponding original threshold events. There are now 71 contacts that
distinguish reciprocal-multiply normalization from per-lane division.

Final ELF-verified and frozen original replays each exit zero and reproduce all
six current output hashes. The source hashes recorded by the original proof
also match current files. The original execution corpus covers 726 unique
words in 417,491 interpreter steps; it does not execute unsupported work.

Final release and ASan/UBSan runs each pass 425,478 checks with:

- 314 complete bitwise comparisons, including the three explicitly labeled
  zero-normal finite-model cases;
- 14 original-boundary transactional rejections and ten explicit native
  negative-capacity guard divergences;
- 305 rejection cases, including 168 full-span start/tail alias cases;
- 19 wiring/gating cases, 17,500 deterministic serial reuse calls and zero
  observed runtime allocations;
- empty compile/test stderr, no changed inputs during execution, and every
  input-manifest hash independently matched against current files.

Durable reports and logs are retained beneath
`evidence/vehicleContact48R/native-validation/` in `release338/`,
`sanitized338/` and `mutations338/`. Their `report.json` files match the original
run outputs byte for byte. Every retained file's length and SHA-256 was checked
against `native-validation/manifest.json`. No binaries, objects or mutated
production-source copies are required in the deliverable.

The two runs compare 754 source callback events and 377 append calls on native
completion cases. Larger whole-corpus event counts include stopped/guarded
source paths and must not be substituted for native-completion totals.

All 15 disposable-source mutations compile and are detected by normal test
failure, exit one; the unmodified control passes. Mutations cover cached radius,
non-strict equality, per-lane normalization division, forced static scale,
temporary instead of cached center, body/spring count resets, skipped callbacks
on saturation, reversed candidate order, conflated scene bindings, invented
zero normal, reassociated dot, normalized up, fixed Y-only up, and cleared stale
record margin. Mutation checks leave production source/test/fixture hashes
unchanged. Crashes, build errors and timeouts are not counted as detected
mutants.

The reviewed final implementation fingerprints are:

- `src/game/vehicle_contact48r.cpp`:
  `b31e6266fe3fb518a392bdc32c0b0813ee5f666f9af003916fb49b5c436c4245`
- `src/game/vehicle_contact48r.h`:
  `223b93db99d4e9d8b775bb139af495603a88c88e83776b49fe7bddd71fc367ef`
- `src/game/body_contact_list48r.cpp`:
  `c91b08855de012a964c82ef89b2cc1e4b653a1506d213eaf62c8ccbdfdea6fd9`
- `src/game/body_contact_list48r.h`:
  `21ab4aea7f2dfe023fd847866655ef8a2c90116fe6e187ccb554d5a6b93f5f5f`
- `tests/native/vehicle_contact48r.cpp`:
  `7932636011dda32f79d1d45ba422e61789e45df7625b627ff918c54debefc4db`

The final review does not authorize or certify detector/handler/response
integration, real world enumeration, parent traversal, unknown callbacks,
other bounds types, optional bodies, remaining `RunSimulation` dependencies,
Windows/x86 builds, live gameplay, rendering, whole-game RAM acceptance or
console FPS. The mapped caller ordering remains a requirement for future work.

## Final integrated regression reconciliation

The final eight-suite release and ASan/UBSan sweep was independently reconciled
against `evidence/nativeVehicleContact48R-linux-20261006/validation.json` and
its 16 retained reports. All report SHA-256 values, current input source/fixture
hashes, command exit statuses, log hashes and per-suite check totals agree.
Each flavor passes **4,292,718 checks**. The new vehicle suite contributes
522,281 checks: 96,803 callback/reset checks and 425,478 handler checks.

The requested final vehicle reports in `vehiclecontact48r-final/` and
`vehiclecontact48r-final-asan-ubsan/` each match all 19 current manifest inputs,
report no changed inputs, and have no compiler/test errors. Their report hashes
are respectively
`6af2c1dbaa1f3f90734804b6e190470ba0b3f03dbb086e3c294f4c7f9f3fcd71`
and `13213b4f005695e5c69fc97e2dc5395b928f094b42989cd6e341ec820967e36c`.
The retained aggregate copies are `vehicle-release/report.json` and
`vehicle-asan-ubsan/report.json` within the evidence directory above.

Terrain retains three compiler warnings in each flavor; the other suites have
none. LeakSanitizer, CMake execution, Windows and Xbox runs remain explicitly
unperformed. This integrated regression sweep does not expand the bounded
source/synthetic proof into live vehicle/world ownership or a complete game.
No binaries were rerun during this reconciliation. This review document is
finalized for packaging.
