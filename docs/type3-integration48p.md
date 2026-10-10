# 48P bounded type3 query integration

## Delivered seam

`queryStaticType3Bounded48P` composes independently recovered ray preparation,
ordered static candidate filtering, original type3 single-mesh dispatch, mode1
node traversal, triangle contacts, and the type3 static spring callback. This is
an explicit caller-fed headless seam. It does not discover collision worlds,
resolve live scene ownership, schedule a vehicle, or execute a frame of the game.

The normal entry projects `122660`. The one-candidate Dispatcher entry projects
`126F40` directly and bypasses list preparation, filtering and callbacks. Inputs
include the original mode/sentinel/masks/tags and initial retained state. They
are not inferred from the ELF's uninitialized globals.

The three namespaces remain separate: list byte tag, shape tag, and collision
category. Shape0 is one mesh. Reached shape1 compound meshes remain unsupported;
other shape tags produce the original gate-specific miss/reset behavior.
Normal-array mode0 is flat and mode1 uses the recovered interpolation helper.
A higher mode rejects only when an improving hit reaches its alternate helper.
Nonzero endpoint scene metadata remains unsupported in the composed path.
The standalone callback accepts explicitly supplied metadata and cached body
matrices, with broader independent fixtures.

## Integration contract

Callers supply immutable source state and two distinct writable storage sets:
reusable scratch and destination. All declared writable spans, including unused
capacity tails, must be mutually disjoint and avoid input descriptors and nested
mesh/filter/name backing. Aliasing, malformed extents and insufficient capacity
return a status. Errors may change scratch but leave destination and result
unchanged. Complete misses commit the exact reset phase, not an all-zero result.

The source ray capacity and separate length capacity are copied in full. Unused
destination capacity stays unchanged. Record backing is likewise copied in full,
including stale bytes. A separate caller-supplied stack drives child0-before-
child1 traversal. Mesh topology must satisfy the existing importer's rooted-tree validation,
including no shared or unreachable records. The query bounds reached traversal
and catches malformed reached indices; it does not redo complete topology
validation on every call. No runtime recursion, heap allocation, thread or growing cache
is used. Imported meshes and collision-filter vectors are borrowed read-only;
loading them is outside this runtime call.

Use the same immutable source for independent queries, or explicitly promote the
returned arrays and state for the next call. A completed type3 candidate resets
fractions and hit count after its strict sphere gate, and transforms selected
vectors even when an exact endpoint hit retained an earlier selected vector.
It does not have the cumulative nearest-per-pair behavior of the separate48N
type2 query. The original callback reconstructs its endpoint with the shared
first three length words and reads metadata[i]. Replacing either with a more
conventional per-ray calculation changes original behavior.

`spring_detector48o` and `query_prepare48o` remain separate numeric preparation
APIs. Connecting their storage/lifetimes into this interface and then into a
live vehicle scheduler is subsequent work. No type3-to-type2 substitution exists.

## Console-oriented memory baseline

The runtime is serial and allocation-free. With ray backing capacity C, length
capacity L, record backing R, candidates K and tree-stack capacity T, each ray
storage set occupies 96*C + 4*L bytes; each record set occupies 96*R bytes.
There are source, scratch and destination sets. Scratch and destination each
have K candidate-effect records; the stack is 4*T bytes. Input meshes, filter
lists, scene names, descriptors and output state are additional. Exact C++
descriptor sizes depend on the target ABI. Storage is reusable and caller-owned.
No collection or deallocation is needed inside the hot call; owners can release
whole level/session allocations at explicit lifetime boundaries.

Caller-supplied sizes bound work and storage; this API does not yet enforce a
whole-game RAM or per-frame time budget. High-density collision workloads and
filter scans still require target profiling. Compiler stack reports are
per-function estimates, with nested frames additional. Linux benchmark timings
and process working set include host/tooling effects and cannot establish OG
Xbox FPS, total game memory or physical EE exceptional floating-point behavior.
Strict individually rounded binary32 arithmetic is preserved before optimization.

Python, JSON, the instruction interpreter, sanitizers and CMake are validation
or build tools. They are not dependencies of the shipped query runtime. The
Windows native target includes the new sources without replacing the renderer
or changing the platform baseline.

## Reproduce

From the repository root, use a fresh output directory outside the source tree:

    python3 tools/test_native_type3_query48p.py --stack-usage --output-dir /tmp/rr48p-release
    python3 tools/test_native_type3_query48p.py --stack-usage --sanitizers --no-leak-check --output-dir /tmp/rr48p-sanitized

The explicit no-leak-check option is required on this ptrace-restricted cloud
host. AddressSanitizer and UndefinedBehaviorSanitizer are checked; LeakSanitizer
is not claimed. The runner serializes build/test processes and records source,
fixture and executable hashes, complete logs and compiler stack estimates.

An optional CMake path is `RR_NATIVE_TYPE3_ONLY=ON`; its sanitizer option is
`RR_NATIVE_TYPE3_SANITIZERS=ON`. CMake is not installed in this cloud environment,
so that configuration and Windows/MSVC execution remain untested here. The
existing Windows path includes `cmake/NativePc48P.cmake` and has the preserved
`tools/Build-NativePc48P.ps1` entry. No installations were performed.

## Validation status

The final 245-case corpus includes 190 supported complete queries, five direct
dispatch projections, and 50 explicit unsupported-case checks. The full-query
test performs 341,118 bitwise, state, guard and safety assertions. Original
evidence includes nonidentity transforms, exact selected/pair ties, stale early
gates and tangency. Retained earlier spring/terrain/producer/detector/scalar
regressions remain part of the final suite. See the final validation JSON for
aggregate counts and hashes rather than treating any earlier subset as current.

## Evidence and correction

See the separate dispatch map, original-query proof, node/leaf/preparation/
callback notes and query test report. Expected fixtures execute frozen original
instructions on explicit synthetic overlays with zero source RAM reads. They
are isolated source proofs, not saved live registration evidence.

The VU MIN/MAX oracle correction is documented in `vu-minmax-audit48p.md`.
Mixed signed-zero endpoint cases expose it; new48P preparation uses the verified
selector ordering. Historical48H–48O fixture data and prior recovery archives
remain intact, with old oracle source preserved for their provenance. The
separate scalar48O correction stays active.

## Next bounded scope

Recover endpoint scene-transform preparation with explicit caller-owned scene
identity/matrix bindings, and then reached shape1 compound dispatch state and
alternate normal helper10F580 before broader live scheduling. The available ELF
and frozen instructions cover this source-derived work. Live registration,
saved cached transforms and the actual sentinel/global values remain unverified;
no RAM dump is required or approved for the present synthetic milestone.

## Final aggregate and host observation

All retained and new suites passed 2,715,549 checks per configuration in release
and ASan/UBSan, with unchanged final input hashes. New components emitted no
warnings; the three previously documented terrain warnings remain. Twenty-one
compiled negative controls are rejected across kernel and pipeline tests.

The final optimized complete-query benchmark observed 2,609 ns median per
query in its bounded synthetic workload. A separate run of that already-built
Linux test executable peaked at 28704 KiB resident memory. This includes
JSON fixtures, assertions and diagnostics; the hot query itself performs zero
observed allocations and uses explicit caller buffers. Neither figure is a
whole-game or console performance/memory measurement. The reproduction tool
is tools/measure_type3_host48p.py.
