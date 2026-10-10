# 48N: bounded static spring-contact production

This milestone composes the independently recovered generated-record append and
reset routines, static type2 spring callback, and the existing bounded mode1
single-pair triangle query. Candidate order is supplied explicitly by the caller.
The production functions use native values and IDs, never guest-memory reads or
an interpreter. Original instruction execution is isolated offline test tooling.

## Exact behavior at this boundary

- Optional original296490 reset occurs once before the explicit candidate list.
- Original query reset flags and collision-tag/group rules precede sphere and
  bilateral eligibility filtering. Candidate flag/copy side effects are retained.
- Each geometric candidate hit runs the spring callback, even if farther than
  the current nearest pair. The callback appends cumulative per-pair data.
- Strict `<` nearest-pair ownership and global `<=` selection are distinct. An
  equal-distance later candidate can become globally selected while its callback
  still appends the earlier pair. No final-contact-only deduplication is applied.
- Local endpoint fourth lanes retain their raw bits. Whole-query original traces
  establish incoming vf9.W=1 at121590 for this mode: fresh normal.W becomes2,
  surface.W becomes1; an already transformed cumulative pair remains unchanged.
- Static spring+130 ownership retains the first static candidate. Query current
  candidate identity is a separate field and advances before sphere/filter tests.
  When no collision-tag entry is enumerated, its supplied prior value survives.
- Callback skip mask runs first. Material comes from the original bounded name
  suffix parser, including signed-byte subtraction without digit validation.
- Record append preserves margin48, flags50 and unknown tail bytes. Original
  logical saturation is successful no-append behavior, never workspace failure.

Detailed component contracts: [append/reset](spring-records48n.md) and
[static callback](spring-callback48n.md). Full original-query evidence and replay
instructions are in `evidence/springQuery48N/`.

## Bounded ownership and memory

`queryStaticSpringBounded48N` takes a non-owning view, persistent read-only asset
and filter data, input record slots, caller-owned scratch, and separate outputs.
All declared writable capacities, including unused tails, must be disjoint from
one another and all inputs, including nested mesh/filter/name data. Actual C++
object lifetime and backing allocation validity remain caller responsibilities.

Scratch can change on a failed query, but destination records, candidate effects
and metadata commit only on Complete. Completed misses commit reset/flag effects.
An undersized workspace never truncates a candidate list or silently drops a hit.
Only supplied record slots and ordered effect slots are copied on success;
unused destination tails are preserved. Negative record metadata, unsupported
callbacks and malformed native spans are explicit transactional errors.

The hot path is serial and allocation-free. Filters and meshes use existing
persistent vectors loaded outside the frame loop; no vector copies or growth
occur inside the query. Record buffers are96 bytes per slot each for input,
scratch and output; when prior frame output serves as next input, two writable
buffers plus retained prior state are enough for the transactional boundary.
Candidate effects are40 bytes per ordered slot each for scratch and output.
Tree scratch is4 bytes per stack entry, reusable between candidates. The real
48I mesh topology bound remains18 entries/72 bytes. No locks, background work,
desktop renderer, JSON parsing or interpreter is shipped in this runtime path.

Host allocation instrumentation, compiler-frame estimates and timings are test
observations. They do not establish a target Xbox memory ceiling or frame rate.
A full game budget must also cover graphics, audio, assets and every scheduled
vehicle. Behavior is preserved before reducing copy volume or retained buffers.

## Build and scope

Run `python3 tools/test_native_spring_producer48n.py --output-dir ../producer48n-check`.
Run sanitizer coverage in a fresh output directory with `--sanitizers
--no-leak-check`; LeakSanitizer is unavailable under this traced container.
The optional focused CMake path is `-DRR_NATIVE_SPRING_PRODUCER_ONLY=ON`.
`NativePc48N.cmake` and `Build-NativePc48N.ps1` retain Windows integration, but
CMake/MSVC are not available in this cloud environment and were not executed.
No toolchain installation is required for the Python/GCC path.

The independent test corpus executes original122660/121410, filters,294BB0 and
2963E0 with explicit synthetic objects, plus retained native mesh geometry.
It does not establish the saved detector+30/+38/+60/+68/+70/+74 contents, live
world registration, runtime candidate ownership, optional terrain provider,
point generation, a multi-pair query tree, dynamic-body callbacks, query tags3/5,
a scene-ID registry, producer-to-response scheduling or a complete vehicle tick.
These remain explicit dependency boundaries, not approximated gameplay.
No full RAM dump was read, reconstructed or transferred for this milestone.
