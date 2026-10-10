# 48Q Linux detector integration

The bounded `runSpringDetectorPipeline48Q` entry composes original295630 paired
preparation, reset, common endpoint/type3 staging, supplied-list122660, mesh
queries and static spring callbacks. Its inputs explicitly bind cached body
scenes; no world registration, object ownership or live frame is inferred.

Read the API contract in `spring-detector-pipeline48q.md`, independent original
proof in `spring-detector-pipeline-proof48q.md`, and guard/differential coverage
in `spring-detector-pipeline-tests48q.md`. The additive context API and its
poisoned-template tests are described in `type3-context48q.md`.

## Build and regression entry

Use a fresh output folder outside the source tree:

    python3 tools/test_native_detector_pipeline48q.py --stack-usage --output-dir /tmp/rr48q-release
    python3 tools/test_native_detector_pipeline48q.py --stack-usage --sanitizers --no-leak-check --output-dir /tmp/rr48q-sanitized

The shared runner uses strict floating-point options, builds/runs serially,
validates required fixtures and records source hashes before and after. No
fixture is skipped when missing. Python/JSON are test tooling, not runtime
requirements. The sanitizer command explicitly excludes LeakSanitizer on this
ptrace-restricted host; it checks ASan and UBSan without claiming a leak pass.

Optional CMake configuration uses `RR_NATIVE_DETECTOR_PIPELINE_ONLY=ON` and
`RR_NATIVE_DETECTOR_PIPELINE_SANITIZERS=ON`. Windows native integration now layers
`NativePc48Q.cmake` over48P and provides `Build-NativePc48Q.ps1`. CMake, Windows
and Xbox execution remain untested here; no tools were installed.

## Storage and original behavior

Caller-owned detector and query stage buffers are reused. The immutable filter
vectors are borrowed through a small scalar context, avoiding a copy, resize or
allocation between stages. The old48P public API still forwards its original
six scalar fields and is revalidated against the unchanged245-case corpus.

All47 mutable capacities are disjoint across both stages and checked against
all declared inputs and nested backing. The final arrays/result commit only
after successful composition; supported registration/world boundary statuses
commit just the proven detector prefix and explicitly do not report a natural
return. A completed detector returns1 even when the separate query hit is false.

Bounds/staging/shape counts are independent. A shorter active query may leave
stale callback slots and metadata tails which are deliberately retained. An
active dirty parentless scene can refresh before the callback reads its world
matrix; a bound scene outside the active point loop keeps its old cache. Scene
binding selects a matrix source and never overrides logical body-scene absence.

The benchmark workload uses5,088 bytes of workspace backing and2,512 bytes of
final destination backing, with immutable inputs/meshes/filter vectors additional.
There is no per-call garbage collection or container growth. Storage owners may
release whole level/session buffers at an explicit lifetime boundary. Caller
capacities bound work and memory; no whole-game RAM/frame budget is established.
GCC's pipeline frame estimate is1,216 bytes (dynamic,bounded), with nested frames
additional; this is not a measured maximum total stack or target ABI size.

## What remains

Provider calls, dirty-parent refresh, full world registration/enumeration,
compound dispatch, alternate normal responses and dynamic callbacks remain
explicit boundaries. The four original zero-shape-count probes produce infinite
query radii and are labeled diagnostic out-of-domain executions, not native
finite-domain support. Physical EE exceptional modes, real game ownership,
vehicle scheduling, full races and target performance are not validated.

Before connecting detector output directly to spring responses in a vehicle
substep, map the actual caller order in219810 and its intervening contact handler
219420. Earlier recovery identifies contact handling between detection and the
engine/gravity/aerodynamic/spring response stages; skipping it would be an
unproven scheduler. Existing ELF words allow a bounded source-derived follow-on;
no RAM or new PC dependency is needed merely to inspect this order.

## Final aggregate observation

All seven retained/new suite groups passed 3,770,437 checks in each release
and ASan/UBSan configuration. Final input hashes match current code. New tests
and runtime emit no warnings; three existing terrain warnings remain. Thirteen
compiled negative controls are rejected.

Final optimized bounded-pipeline timing was 6,098.63 ns median and 6,541.37 ns
maximum batch mean. The separate complete Linux test process peaked at 42016 KiB
RSS, including JSON fixtures and diagnostics. These are reproducible host-only
observations, not Xbox FPS or whole-game memory. Final reports, logs, frames
and fingerprints are under evidence/nativeDetectorPipeline48Q-linux-20261006.
