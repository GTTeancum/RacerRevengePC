# 48U bounded outer-group type-3 query tests

## Contract

The additive native harness consumes `evidence/groupQuery48U/native-fixtures.json`.
Expected observations come only from initialized execution of the original
`122660` limited-list query, its secondary group dispatch, the genuine `135F30`,
`135CF0` and `135CD0` helpers, type-3 dispatch and static callback. Native output
is never used to create an expected fixture. The retained48P/48Q/48T tests and
fixtures remain unchanged.

The harness compares every represented game-facing vector lane, raw spring-state
word, record byte, object flag/copied-vector/sphere field, and shape matrix word.
Candidate-local working vectors retain the established XYZ projection. Source
word snapshots retain opaque synthetic addresses without dereferencing them on
the native host. Explicit original-only partitions must reject transactionally;
they are not removed or counted as supported native execution.

All native calls use caller-owned serial buffers. Allocation instrumentation
separately counts ordinary new, array new, aligned new and aligned array new.
JSON parsing, setup, snapshots and diagnostics are outside hot scopes. Every
non-Complete status must leave all destination buffers and the result bytewise
unchanged, including failures reached after earlier members changed scratch.

## Reproduction

`tools/gameplay/group_query_native48u.py --fixture-sha SHA256` builds release and
ASan/UBSan configurations, executes the unchanged normal assertions and then
compiles semantic mutants. All objects and executables are created under a fresh
system temporary directory outside the source tree. Evidence includes exact
input fingerprints, commands, resource measurements, reports, compiler stack
estimates, mutant source and normal assertion failure logs. A compiler error,
crash, harness exception or sanitizer failure does not count as a killed mutant.

LeakSanitizer is explicitly disabled (`ASAN_OPTIONS=detect_leaks=0`) because this
executor's ptrace route does not support it. This is not a leak-check claim.
Host timings and compiler stack estimates are bounded component observations,
not gameplay FPS, console budgets, a renderer test or live-world integration.

## Differential and boundary coverage

The frozen packet has128 cases, SHA-256
`65d184ae5cb91b053fe6dd899d52d2b4a00740ef04c5777d4d52fd24b7794cf1`:
120 complete and8 explicit unsupported scope cases. Every case executes twice:
once through null/zero identity hints and once through an exact, revalidated,
strictly identity-sorted reference permutation. These are240 complete native
calls and16 boundary calls, rather than256 distinct original scenarios.

The corpus covers direct/group ordering, repeated groups and members, shared
shape backing, stale per-object spheres after another member changes their
shared shape, ordinary visits after group mutation, signed group counts,
query-self members, group-bit0/tag gates, XZ-only coarse bounds and inclusive
coarse equality, strict dispatcher-sphere equality, member filter bypass,
activation after setter mutation, shape4/5 compact state and shared layouts,
no-op shape tags, full-W matrices, signed-zero group position scale, material
mapping, callback ownership and static/dynamic prior owners, saturation,
multiple rays and seeded finite variations. The original-only partition keeps
reached compound shape1, nonzero endpoint metadata, higher normal interpolation,
member response callbacks, null reached shape/geometry, and dynamic-body
callback behavior explicit.

The loader reads actual pre-execution shared shape words, rather than choosing
one member's independent matrix description. It binds each physical shape once,
projects every alias view back into all72 original words, and preserves opaque
metadata. Query-self object center/radius, flags and copied vectors are checked
independently against the original query snapshot. All game-facing W lanes are
bitwise comparisons, with no tolerance or native expected generation.

## Declared spans and transactional safety

The harness enumerates28 mutable spans: two sets of9 ray arrays, records,
object states, shape0 states and shape4/5 states, plus tree stack and result.
It tests every writable pair and every writable kind against declared input
arrays, nested candidate/filter/mesh/name/member storage, and all descriptors.
The far-end read aliases use the last suitably aligned byte of the full span,
including fixed one-object descriptors. It includes inactive capacity tails;
initial shape/object capacities exceed active table counts deliberately.

Null, misaligned, multiplication-overflow and address-wrap pointer spans are
rejected. Reference-taking parameters use real aligned object aliases rather
than creating invalid C++ references in the test harness. Capacity checks cover
source rays, lengths, records, endpoints, metadata, object/shape state, compact
state and tree stack. Overflow probes enlarge dependent capacities when needed
to distinguish an invalid declared extent from an earlier legitimate capacity
shortfall.

Further cases check missing/out-of-range references, duplicate/zero identities,
invalid exact hints and stale hint ordering, canonical compact-tag mismatches,
late setter/callback/material failures, nonfinite ratios and source arithmetic,
callback suppression, negative signed member counts, saturation and empty-list
preparation. Rejections snapshot all genuine source storage and every output
byte. A deliberately late missing-shape failure additionally proves that earlier
members changed scratch before the all-destination rollback was checked.

Serial measurement reuses one caller-owned fixture for7500 calls, then checks
its exact original final state, immutable inputs and all guards. The reported
scratch budget includes full declared capacities, including inactive tails.

## Final native evidence

`evidence/groupQuery48U/native-final-r6/summary.json` records the final fresh,
serial run on2026-10-06 with GCC14.2, strict floating-point flags and one allowed
CPU. Its input manifest contains31 native/test/header/fixture fingerprints;
`file-hashes.json` covers every retained final output.

- Release and ASan/UBSan each passed673,053 checks with zero failures.
- There are3,928 safety/boundary calls, including3,633 alias probes;16 of those
  calls are the8 original-only cases repeated across both identity-hint modes.
- Ordinary new, array new, aligned new and aligned array new each counted0
  inside every guarded native call and serial benchmark.
- All29 semantic mutants compile, link and fail the unchanged normal assertions
  with exit1. No crash, harness exception, sanitizer diagnostic or compiler
  failure counts as a killed mutation.
- The original differential totals, doubled for the two validation modes, are
  586 candidate geometry hits,580 callbacks,536 appended records and74 saturated
  append attempts across170 queries that hit geometry.
- The release serial benchmark completed7500 exact-state-verified calls with
  median batch mean5378.391333333333ns and maximum batch mean6305.048ns. Declared caller
  scratch is3864bytes; output storage, immutable inputs and result are additional.
  This host's result is240bytes and workspace descriptor184bytes.
- GCC's separate frame estimates are1696bytes for the group query wrapper,
  304bytes for its callback adapter and112bytes (dynamic bounded) for its dispatch
  adapter. These are not a measured maximum live stack or a sum of callees.

The29 mutations discriminate group suppression/tag precedence, inclusive XZ
bounds, single-member enumeration, ordinary-filter bypass, activation after
setter writes, enclosing-group position ownership, per-object spheres versus
shared matrices, unvisited alias spheres, compact translation/scales,
canonical cached dispatch inputs, reached ratio errors, canonical compact tags,
member dispatch/current identity/copied vectors, query-self flags, explicit
callback identity and response boundaries, callback member ownership,
identity-hint strictness, transactional failure output and both shape commits.
Individual failure counts and mutant source fingerprints are in the summary.

The first cached-outer-sphere mutation survived the original corpus. This was
recorded as a coverage gap. A new synthetic native guard-order case places a
member far away through an XZ-passing group, obtains an intervening ordinary
hit, then revisits the far member ordinarily. Its outer-sphere miss must retain
the prior ray hit flags. The same mutation now fails this normal assertion.
The complete127-case prefix is preserved under `before-direct-sphere-extension`.
A128th original-executed case, `group-updated-far-sphere-direct-mask-retained`,
now independently closes the differential gap: original3392 steps execute only
member10 (miss) and ordinary11 (hit), leave current identity10, and retain
hitFlags[0]=1 after the final ordinary10 rejection. This case uses the identical
neutral filter/normal backings as the native control. The formerly surviving
mutation now fails both original-derived validation modes plus the independent
native assertion, for3 normal assertion failures.

Earlier attempts remain visible: `native-final` failed on runner object-name
collision, `native-final-r2` stopped on a mutant's misleading-indentation compile
warning, and `native-final-r3` retained the surviving cached-sphere mutation.
They are historical incomplete attempts, not passes. `native-final-r4` is a
passing127-case checkpoint before the extra original execution; `native-final-r5`
is a passing128-case checkpoint before correcting the loader's unused grouped-self
collision-tag metadata. The final-r6 run uses the original zero-initialized
query+2 value and is fresh against all128 cases, without reusing earlier builds.
All build products stayed under `/tmp`.
The optional resume switch is for unchanged native inputs, commands and mutation
definitions; use a fresh output directory after editing the runner or a mutant.

Ordinary query-self visits short-circuit on equal identity before any bilateral
filter fields in both original120E70 and the native predicate. Grouped self
visits require matching identity, shape3 and activation, then use the canonical
prepared query sphere and flags. The current self fixtures exercise that grouped
path; they do not claim a separately original-executed ordinary-self scenario.

The query-self loader retains original query+2=0; this byte is distinct from
the global ordinary collision-list tag. Existing grouped-self fixtures do not
consume that byte, but their complete native input binding now reflects it.
