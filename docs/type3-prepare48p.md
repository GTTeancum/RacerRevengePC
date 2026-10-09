# 48P bounded type-3 ray preparation

`prepareType3Rays48P` recovers the type-3 branch of original `11DD00`, specifically
`11DEB8..11E2D4`, for the already-world-space endpoint path staged by 48O. It is
a native numerical kernel, with no guest addresses, source-ELF dependency,
runtime allocation, threads, gameplay cap, or persistent global state.

## Boundary and input contract

`Type3PrepareInput48P` supplies a positive signed-compatible pair count, a flat
array of two complete endpoint Vec4s per pair, a flat metadata array of one
32-bit word per endpoint, and an explicit sentinel Vec4. Every consumed metadata
word must be zero. Nonzero endpoint scenes return `UnsupportedScope`; there is
no fabricated scene refresh, matrix lookup, or resolver.

The sentinel represents initialized data at original `3E02C0`. That location is
beyond the file-backed ELF segment and is **not an ELF-proven constant**.
The original-word fixture driver explicitly supplies `(0,0,0,1)`. Production
callers must supply their own initialized value. All sentinel lanes are copied
as bits, even when they contain nonfinite payloads or unusual fourth lanes.

The three written array spans are `worldStarts`, `worldDirections`, and
`lengths`. Each must cover the pair count. The first two use buffer `capacity`;
lengths use `lengthCapacity`. Supplied untouched array spans use `capacity` and
are guarded too; those arrays may instead be null. Every declared span must be
aligned, address-representable, and backed by caller-owned storage. All buffer
spans, including untouched tails, must be mutually disjoint and disjoint from
the input spans, both descriptors, and query state. State must also be disjoint
from sources and descriptors. Read-only source spans may overlap one another.
These guards cannot prove that a syntactically valid pointer is actually backed
by an allocation; that remains the normal native caller contract.

No empty-bounds result is invented: zero, negative-as-unsigned, or greater than
`INT32_MAX` pair counts return `InvalidInput`. Insufficient declared capacity is
`CapacityExceeded`. Invalid ranges/alignment are `InvalidInput`; overlaps are
`AliasedBuffers`; nonfinite consumed XYZ or intermediates are `NonFinite`.
Arithmetic is ordinary individually rounded binary32 with contraction disabled
by the build. This is not a claim about every physical EE/VU exceptional mode.

## Exact retained arithmetic and effects

For each pair, in original index order:

1. Copy the complete source start, including its raw W, to world starts.
2. Form XYZ `end - start`; compute length as
   `sqrt((dx*dx + dy*dy) + dz*dz)` with separate binary32 rounding.
3. When length is positive, multiply XYZ delta by scalar `1/length` and write
   direction W as 1. Otherwise copy the supplied sentinel Vec4.
4. Accumulate minimum and maximum bounds, each using prior bound, then end,
   then start, in the original VU order.

For each XYZ axis, center is `0.5*(maximum + minimum)`; half extent is
`0.5*(maximum - minimum)`. Radius is the same ordered XYZ norm of half extents.
Center W is 1. World and query center/radius receive the same values. `result00`
and `result10` become the supplied sentinel; selected fraction becomes 1 and
hit count becomes zero.

Local start/direction XYZ, local center, per-pair normals/points/hits/fractions,
selected normal/point/correction, opaque result fields, and every unused tail
are unchanged. Native candidate-local scratch intentionally represents XYZ
only; no claim about omitted guest vector-register W history is made.

The preparation observes only endpoint XYZ in retained arithmetic. Endpoint W
is copied for starts and does not affect the final XYZ bounds, center, or radius.
It is therefore not incorrectly rejected or normalized.

## VU minimum/maximum ordering

The four original VU sites are `107924` MINI, `11E120` MINI, `11E15C` MAX, and
`11E314` MAX. The independent VU audit checked
[PCSX2 VUops.cpp](https://github.com/PCSX2/pcsx2/blob/master/pcsx2/VUops.cpp),
lines 724–773: signed-word selection with reversed ordering when both operands
are negative. In the finite native domain that is the same monotonic order key
used by `scalarOrderKey48O`, including **negative zero below positive zero**.
The implementation reuses the verified key with an explicit VU-specific
comment. It does not infer VU behavior from scalar instruction names, replace
branch comparisons with min/max, or rely on host floating comparison ties.

## Transactionality and resource use

All range/capacity/alias/metadata checks precede arithmetic. A first constant-
stack pass validates every ray and accumulates bounds. The aggregate center,
half-extents, and radius are then validated. Only after all of this succeeds
does a second pass recompute and commit the rays, followed by the affected
state fields. No failing path writes any output, source, descriptor, or state.
The commit assumes the documented serial execution and stable floating-point
environment and source storage. The caller need not allocate extra temporary
ray arrays for this kernel.

## Differential and safety tests

`tests/native/type3_prepare48p.cpp` reads the complete original-query fixture
file, `evidence/type3Query48P/native-fixtures.json`, and compares the
`expected.after_prepare` projection bit for bit. Every full-query case with
zero consumed scene metadata is eligible, regardless of later candidate shape,
because no candidate traversal occurs in this preparation phase. Full-query
cases with nonzero metadata must reject transactionally. The two direct
`126F40` dispatcher cases are skipped because they never execute `11DD00`.

The fixture includes mixed positive/negative-zero endpoint order cases added
by the independent VU audit. No test computes an independent native expected
length, direction, center, or radius: every numerical differential expected
word comes from executed original instructions. Test-local cases cover invalid
counts, null/short/misaligned/overflowing ranges, aliases, late nonfinite ray
arithmetic, aggregate-only overflow, unusual raw W and sentinel payloads,
omitted unused arrays, unchanged tails, and a 17-pair call proving there is no
invented 16-pair cap. Allocation instrumentation surrounds all runtime calls,
including a 10,000-call serial 17-pair stress loop.

The focused GCC release object reports a 432-byte static preparation frame and
a 96-byte static ray-helper frame with `-fstack-usage`. Neither frame grows with
the caller's pair count.

Initial focused verification used the 209-case original fixture snapshot (SHA-256
`4cee7d2a492a75f34ac65c5823432fe957783979747f8066b459b0ce946c1a14`):

- Optimized and ASan/UBSan builds both pass **29,295 checks**.
- **163** eligible original preparation projections match exactly.
- **44** nonzero endpoint-scene cases reject without mutation; **2** direct-only
  dispatcher cases are skipped.
- 10,000 serial 17-pair calls observed **zero runtime allocations**, taking
  6.69 ms optimized and 48.1 ms sanitized in this cloud Linux run. These are
  local measurements, not a console or physical EE performance claim.
- Sanitizer verification used `ASAN_OPTIONS=detect_leaks=0`.

Focused reproduction from the repository root:

```sh
mkdir -p work/type3-prepare48p
g++ -std=c++17 -O2 -ffp-contract=off -fno-fast-math -Wall -Wextra -Werror \
  -fstack-usage -Isrc/game -Ithird_party/native-json \
  src/game/type3_prepare48p.cpp tests/native/type3_prepare48p.cpp \
  -o work/type3-prepare48p/check-release
work/type3-prepare48p/check-release .
g++ -std=c++17 -O1 -g -ffp-contract=off -fno-fast-math -Wall -Wextra -Werror \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Isrc/game -Ithird_party/native-json \
  src/game/type3_prepare48p.cpp tests/native/type3_prepare48p.cpp \
  -o work/type3-prepare48p/check-sanitize
ASAN_OPTIONS=detect_leaks=0 work/type3-prepare48p/check-sanitize .
```

Frozen original source mapping is in `docs/type3-dispatch-map48p.md` and
`evidence/type3Query48P/map/reset_finalize.txt`. The complete query instruction
proof and the VU delta audit remain separate evidence; this component does not
claim to implement the full dispatcher, callback, or outer query.

## Compiled negative-control sensitivity

The following mutants use temporary copies outside the repository. The
production source, shared state header, native test, and original fixtures are
never modified. The unchanged preparation test is compiled once and linked
against each separately compiled mutated kernel. Compilation must succeed;
only a runtime assertion failure establishes sensitivity.

1. **Host-comparison VU ties:** replace `vuMin`'s finite bit-key selector with
   `return b<a?b:a;` and `vuMax`'s selector with `return a<b?b:a;`. These ordinary
   host comparisons retain the first operand on a zero tie. The mixed-zero
   original projections detect the resulting world/query-center sign
   changes. No branch comparison is mutated.
2. **Clear stale selected results:** insert `state.selectedNormal={};`,
   `state.selectedPoint={};`, and `state.selectedCorrection={};` on the success
   commit path, before returning. The original preparation snapshots require
   these preexisting fields to remain untouched.
3. **Clear opaque result words:** insert `state.opaque78=0;` and
   `state.opaque7c=0;` on the success commit path. The initialized opaque words
   in original snapshots detect this unrequested clearing independently
   of the selected-vector mutant.
4. **Invent a zero-length sentinel:** after the zero-length branch's raw
   sentinel copy, overwrite direction with `{{0.f,0.f,0.f,1.f}}`. The explicit
   arbitrary-sentinel safety case fails even though all original fixtures
   happen to initialize the sentinel to that conventional value. This is
   intentionally a caller-contract test, not an invented original numerical
   oracle.

All four compiled mutants were detected at runtime against the archived
209-case snapshot identified above. The unmodified baseline passed all 29,295
checks. Each mutant compiled successfully and exited with test status 1:

- Host-comparison VU ties: **64 failed checks**, including world/query-center
  zero-sign differences in the original mixed-zero cases.
- Clearing stale selected vectors: **1,956 failed checks**.
- Clearing opaque result words: **163 failed checks**.
- Inventing the zero-length sentinel: **1 failed check**, specifically the raw
  caller-supplied sentinel preservation assertion.

`evidence/type3Prepare48P/sensitivity.json` records exact replacements, compiler
and execution commands, full diagnostics, input and mutated-source hashes, and
exit codes. Source, test, and both preparation/state header hashes remained
unchanged. All mutations, compilation products, and the archived fixture input
were outside the repository; only this report and documentation were added.
The mutation run froze its own 209-case input, so later expansion of the
complete-query corpus does not retroactively change these measurements.


## Final 245-case corpus

The final controller proof adds 36 original-execution transform/gate/tree cases
without changing the prior209 objects. Preparation now has196 eligible projections,
44 explicit scene-endpoint rejections and5 direct-only skips. The test pins these
counts. An initial full-suite run matched every numeric projection but failed
its old direct-count assertion (2 versus5); the assertion was updated, without
changing runtime code. The old209 test source is preserved beside the sensitivity
report as `test-before-transform-extension.cpp`. Final suite validation below
supersedes that count-only failed run; historical sensitivity remains209 scoped.

Final serial runner verification passed 35,169 checks in both release and
ASan/UBSan against fixture a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f.
See evidence/nativeType3Query48P-linux-20261006 for logs and source hashes.
