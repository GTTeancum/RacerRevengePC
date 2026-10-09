# 48M: bounded terrain preparation and real-mesh query

2026-10-06. This continues the separately recoverable 48L cloud checkpoint.
The original-Xbox-oriented baseline remains serial, low-memory native execution.

## Implemented boundary

`prepareTerrainSinglePair48M` implements the numerical preparation seam from
original 1B1800 through the type-2 single-pair matrix/sphere setter. Independent
original execution stops exactly before 11FFA4 invokes spatial registration.
The opaque saved registration pointer remains nonnull and unread; the call is
not patched or suppressed. The proof does not claim a whole saved/live query.
Read `terrain-prepare48m.md` for its exact fields and original instruction proof.

Important recovered distinctions are retained:

- The component lower endpoint+140 is transformed through the full scene even
  when the subsequent query chooses identity rotation plus scene translation.
- Prior-contact active 104 controls query matrix selection. Shape endpoints and
  component+140 remain distinct inputs.
- Sphere center is the transformed local midpoint; radius remains local. It is
  not scaled by an affine/nonorthogonal scene basis.
- Resets clear documented counts/flags, not the collected-array storage, old
  normal or current-candidate identity. NoBody and QueryDisabled preserve the
  original distinct subsets of state.

`queryOrderedCollisionBounded48M` moves the existing ordered traversal into a
separate numerical translation unit with caller-owned stack/optional trace.
`queryTerrainViewBounded48M` preserves 48J limited-list filtering, sphere gating,
exact geometry, callback ordering and final surface assembly with reusable effect
buffers. The view borrows persistent filter metadata rather than copying vectors.
The old allocating APIs remain explicitly labeled compatibility/regression paths.

`prepareAndQueryTerrain48M` composes those components with an EXPLICIT
caller-provided ordered candidate list. It never performs spatial registration,
world discovery, arbitrary group/dynamic callbacks or spring-contact production.
Composition tests are native wiring tests against the independently verified
components; they are not independent evidence of the entire original live phase.

## Storage and failure contract

The new runtime paths allocate nothing and require no worker threads, filesystem,
JSON or guest-instruction execution. Mesh/filter ownership and setup allocation
are separate from the query hot path. Callers reserve buffers and reuse them.

Stack/trace/effect capacity failure is explicit, never a truncated Hit or a silent
Miss. Output metadata/effects remain unchanged on failure; scratch may change.
Pointer alignment, extent arithmetic, descriptor overlaps and deep mesh/filter
storage aliases are checked before writes. Two gaps identified during independent
review were corrected: outer pipeline output aliasing persistent backing storage,
and terrain pointer alignment before preflight/write. Sanitizer regressions cover
those cases, including skipped query/candidate branches.

A full, unpruned left-first traversal of the verified mesh topology establishes
18 stack words for the cave and 12 for the floor. Reusing 18 uint 32 words costs 72
bytes and covers both assets, without inventing a maximum tree-depth constant.
The existing original query fixtures actually peak at 12 words. Optional diagnostic
tracing is separate and can be omitted; the maximum recorded trace needs 720 bytes.

For two candidates, scratch-plus-output effects cost 160 bytes (40 bytes each).
Terrain result metadata is 224 bytes; the composed preparation/query result is 504
bytes. Preparation input/state are 360/268 bytes. Meshes, persistent filters and
nested call frames are additional; none of these numbers is whole-game residency.

GCC 14.2 release per-function frame estimates are 464 bytes for preparation,880 for
bounded traversal,624 for the terrain view and 704 for composition. These are not
a measured peak call-chain stack bound. Object-symbol inspection finds only
ordinary sqrt/memory helpers and native component calls, with no allocator,
threading, platform or compatibility-interpreter dependency in the bounded units.

## Verification

The complete terrain suite passes 47,906 checks in both normal and ASan/UBSan builds:

- Triangle primitives: 4,025 checks /710 original-word cases
- Compatibility ordered queries: 5,351 checks /43 cases
- Bounded ordered queries: 6,944 checks /the same 43 cases and 1,195 exact visits
- Existing limited-list terrain: 5,143 checks /124 cases
- Bounded terrain and composition/defensive checks: 10,769 checks /124 original
  limited-list cases plus native wiring, allocation, capacity and alias tests
- Numeric preparation: 15,674 checks /223 original-word cases

All 180,428 spring checks also pass in both configurations. Total: 228,334 checks
per configuration. New bounded runtime and spring builds are warning-free; the
unchanged collision importer and legacy tree test retain three misleading-
indentation warnings. Final input hashes are unchanged and bounded-path allocation
counters remain zero. ASan/UBSan run with explicit
`--no-leak-check`; LeakSanitizer cannot operate under the executor's ptrace limit.
Windows and optional CMake configurations have not been executed.

The two complete native meshes match the retained original fixture records and
source instance placements. Their canonical paths were restored from verified
Library inputs. Identical copies were subsequently confirmed in the nested 48K
Windows build evidence; no full RAM dump was needed or obtained.

Three intentionally wrong temporary copies confirm sensitivity: using the stale
query matrix for the lower seed, computing world-scaled radius, and reversing
child traversal order all fail their original-word checks immediately. They are
preserved as labeled test artifacts and were never applied to production.

A serial host microbenchmark of numeric preparation plus the supplied two-candidate
list reaches the real floor mesh, with 1,332.81 ns median and 1,378.01 ns maximum
batch mean. It excludes registration, world discovery, full vehicle scheduling,
rendering and gameplay. This is not game FPS or an original-Xbox prediction.

## Reproduce

```sh
python3 tools/test_native_terrain48m.py --output-dir ../terrain48m-check --stack-usage
python3 tools/test_native_terrain48m.py --output-dir ../terrain48m-asan --sanitizers --no-leak-check
python3 tools/test_native_spring48k.py --output-dir ../spring48m-regression
```

Use new/empty directories outside the source tree. The runners use the installed
C++ compiler/Python, download nothing and build/run serially. Missing evidence is
reported as blocked. Optional CMake uses `-DRR_NATIVE_TERRAIN_ONLY=ON`; its bounded
numerical target is separated from allocating fixture/compatibility support.
The preserved Windows entry is now `tools/Build-NativePc48M.ps1`.

Full logs, input hashes, frame reports, symbol audit and sensitivity evidence are
under `evidence/nativeTerrain48M-linux-20261006/`; spring regression evidence is
included separately there. `evidence/terrainPrepare48M/` owns its deterministic
original-word extraction and fixtures.

## Next source-derived seam

Static mapping identifies detector 295630 → limited query 122660 → spring callback
294BB0 → generated append 2963E0 → response 2964C0. That callback consumes cumulative
nearest-per-pair state across ordered candidates. It must not be replaced by
isolated per-candidate hits or just the final global contact. Persistent point
initialization 295E00/296190 is separate from per-tick discovery. The optional
point-height provider 2E83D0 is an original assertion stub; no height geometry is
invented for that enabled path.

The next bounded packet can recover the one-pair/static callback and generated
record producer from existing source/evidence, retaining explicit unsupported
world/dynamic/auxiliary paths. Actual saved detector ownership, live spatial
registration and full vehicle-substep scheduling remain unproved. This is still
an unfinished game port, with full races, camera/AI/audio/progression, rendering
fidelity and target performance acceptance ahead.
