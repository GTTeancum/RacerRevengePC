# 48U grouped limited-list type3 query

This packet implements the outer grouped-collision branch of original `122660`
for the bounded type3 query. It reuses the closed 48T group setter and borrowed
48P dispatcher without copying owning filters or modifying old entry semantics.
The 48T archive and all prior evidence remain intact.

## Source behavior retained

- Ordinary collision-list tag takes precedence. Otherwise query F8 bit0 suppresses
  groups before the group tag is considered.
- Group coarse overlap is inclusive XZ sphere gap. Member helper `135CF0` updates
  shape state before current/member bit4 and the secondary type3 dispatch.
- Members bypass ordinary outer-sphere and bilateral eligibility filters. Their
  dispatcher still performs its own activation and strict cached-sphere gates.
- Shared shape-0 matrices are canonical while each member sphere remains separate.
  Updating one member does not refresh an unvisited member's cached sphere.
- Shape4/5 use compact shared five-float state; no-op shape tags preserve the
  source's later miss/reset behavior. Mixed physical4/5 layouts through one
  compact binding are an explicit native projection boundary.
- Query callback E4 receives `(query, member)`. The subsequent member E8 response
  remains unsupported when reached. The outer group is not substituted for the
  member identity in current state, copied vectors, material or spring records.
- Mixed ordinary/group order, repeated groups/members, saturation, stale selected
  values and per-candidate resets are retained. Geometry misses still commit
  source-reached shape and flag updates.

A queryObject binding represents the query itself in the object table. Its
prepared sphere/flags/copies remain synchronized. Ordinary self eligibility
short-circuits on identical identities in original `120E70` and native code,
before duplicate filter fields; no speculative filter-equality patch was added.

The new public entry is `queryStaticType3GroupsBounded48U`. It uses explicit
caller object/member orders, canonical shape/mesh tables, mutable state and
reusable ray/record/stack buffers. It does not discover a world. Every non-Complete
return preserves all destination/result bytes; scratch may change. Full declared
capacities, nested filters, mesh arrays, names and inactive tails are guarded.
Optional sorted identity references are checked against current values each call;
there is no unchecked token or retained borrowed pointer.

Read `group-query48u.md` for the complete API and ownership contract, and
`group-query-source-map48u.md` for exact source ordering and offset evidence.

## Independent original proof

The final corpus has 128 genuine full-entry executions: 123 natural returns and
five exact pointer/callback boundaries. Of these, 120 are native-comparable;
eight are explicitly unsupported, including three natural original completions
outside the native domain. It executes 917,559 original instructions, 402
dispatches and 303 completed callbacks, with 296 semantic checks and zero
source-memory reads. Frozen words/data are authenticated against the authorized
ELF. No words, callbacks or expected results are replaced by native behavior.

The local adapter composes previously proven 48S signed64/load-high semantics
with the 48P query executor. All 245 earlier 48P results replay unchanged; shared
interpreters and all previous evidence remain untouched. The 239-case 48S and
297-case 48T records are preserved. These are initialized source-derived worlds,
not saved live ownership or registration.

A cached-sphere mutation initially survived the corpus. One additional original
case now proves the distinction: a far-Y group update, an intervening ordinary
hit, then an ordinary cached-sphere rejection must preserve the prior hit mask.
All previous 127 case objects remain unchanged. The final mutant fails both
original-backed comparisons plus the independent focused assertion.

## Final native validation

All retained and new suites pass **6,711,303 checks per configuration**, release
and ASan/UBSan. The new query suite contributes 673,053 checks, including 240
complete comparisons (120 worlds times two identity-validation modes), 16 scope
rejections, 3,928 safety/boundary calls and 3,633 alias probes. These repeated
invocations are not counted as additional original worlds.

All 29 semantic mutants compile/link normally and fail ordinary assertions with
exit 1. No compiler warning, crash, timeout, sanitizer or harness error is counted
as a detected mutant. Runtime has zero observed ordinary/array/aligned allocations.
No new compiler warnings appear; three existing terrain warnings remain per mode.

Final focused evidence is `evidence/groupQuery48U/native-final-r6/`. Earlier
runs preserve their tooling errors, survivor and input fingerprints. The final
loader uses original query+2 zero, rejects unknown scope labels, and models
original default group filters. The full portable aggregate is
`evidence/nativeGroupQuery48U-linux-20261006/validation.json`.

Reproduce with fresh output directories outside the source tree:

    python3 tools/test_native_group_query48u.py --stack-usage --output-dir /tmp/rr48u-release
    python3 tools/test_native_group_query48u.py --stack-usage --sanitizers --no-leak-check --output-dir /tmp/rr48u-sanitized

`group-query-tests48u.md` describes the dedicated mutation runner and probes;
`group-query-review48u.md` records independent reconciliation. LeakSanitizer is
explicitly disabled because this host's ptrace route is unsupported. CMake,
Windows, Xbox, rendering and a complete game are not tested. The Windows target
is extended additively. No new installations, PC/RAM access, push or publication.

## Console-oriented resource accounting

Calls are serial and allocation-free. A bounded one-pair, one-group, four-object,
three-shape sample has 3,864 bytes of declared scratch (including test capacity
tails), a 184-byte workspace descriptor and 240-byte result. Initial state,
destination backing and immutable geometry are additional. The final root sample
measures a 5,456.902 ns median batch mean over 7,500 reused calls on this host.

GCC reports individual host frames of 1,696 bytes for the public entry, 592 for
borrowed dispatch, 304 for callback and an additional bounded 112-byte dispatch
wrapper; these are not a measured maximum nested stack or Xbox ABI budget.
The complete test process peaks at 71,348 KiB RSS, including JSON, fixture copies,
guard machinery and diagnostics. Sanitizer compilation peaks around 1 GiB. Neither
figure is the shipped runtime's memory use.

Caller identity hints cost 4*N bytes and make current uniqueness validation linear;
without hints, the explicitly supplied object table uses the guarded pairwise
path. Span validation and state copies scale with supplied tables. This is not a
whole-world scaling benchmark. The previous collector's repeated free-entry/spare
checks can still have aggregate cubic cost; this packet makes no new pool
optimization claim. No host timing here establishes console FPS or a frame budget.

## Recovery and next bounded scope

All new compiled objects/executables remain outside the source tree. The recovery
snapshot contains code, original fixtures, logs, stack records and mutant sources,
with every member hash verified and no prior evidence changed.

Next compose this grouped query into the existing `295630` supplied-list detector
using explicit shared state and original callback/cached-scene bindings. Prove the
cross-stage alias/rollback contract before vehicle scheduling. Existing ELF and
fixtures provide source references; actual live registration/ownership remains
unverified. Compound shape 1, consumed nonzero endpoint metadata, reached alternate
normal interpolation, dynamic/member/unknown callbacks and unclosed vehicle stages
remain separate boundaries. The grouped query is not yet that detector wiring.
Vehicle +344 remains gravity, +354 remains spring; the withdrawn 48J impact replay
remains withdrawn.
