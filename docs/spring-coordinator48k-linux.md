# 48K generated-contact coordinator: focused Linux milestone

Date: 2026-10-06. This continues the current 48K source and corrected saved
spring binding. It does not use Checkpoint14E as a current baseline and does not
restore the withdrawn 48J saved-impact interpretation.

## Scope

The native generated-contact phase of original `2964C0` is integrated with
`296F10` contact preparation, `297610` impact and `297390` friction. Every positive
record prepares its contact velocity once, performs enabled impact, then performs
enabled friction using that cached pre-impact velocity. Updated body state is
retained for the next record, including repeated contacts with the same other
body. Record ordering and individually rounded binary32 operations are retained.

The caller supplies current world matrices and already-generated contacts. Point
contacts, spring-force response, the alternative other-spring branch, secondary
response, live query production and complete vehicle scheduling remain explicit
unsupported scope. This is neither a whole vehicle update nor a playable race.
Full races, AI/camera/audio/progression, final rendering and gameplay performance
acceptance remain future work.

## Console-oriented runtime contract

The coordinator is serial and allocation-free. It has no production dependency
on JSON, Python, filesystem, threads, Windows, Direct3D or the original-word
interpreter. Those dependencies belong only to tests or other separate targets.

`SpringGeneratedBuffers48K` supplies body and flag storage with explicit
capacities. The caller reserves output and scratch buffers once and reuses them.
There is no invented original contact-count limit. Pass the local participating
body set, not an entire world registry: duplicate-identity validation is O(B²),
lookup is O(B) per contact and buffer copying is O(B). Numeric work is O(C).

All extents/capacities/overlaps are checked before scratch writes. Output body
storage may exactly alias the input body array. Scratch, output flags, result
metadata and buffer descriptors must satisfy the documented disjointness rules.
On failure only scratch may have changed; committed bodies, flags and metadata
stay untouched. On success the result reports used counts. Only used ranges are
copied/written; unused capacity is not cleared every frame. World owners control
buffer lifetimes and release backing storage at their normal unload boundary.

No per-step `new`, `delete`, `vector` growth, garbage collection or worker thread
is required. The test overrides ordinary global allocation operators to count
allocations only during the coordinator calls, checks output canaries, and also
inspects production object dependencies. This is a component boundary, not a
claim that the unfinished whole game fits the original Xbox memory ceiling.

## Independent evidence

The source snapshot ZIP was SHA-256 verified as
`807803f1fbad17eeb5cf4d0d641700313ff72a24b18d60c6acb31333b674d86d`.
All 2,164 original manifest files matched their recorded bytes and hashes before
editing. Four additional frozen reference files were copied from the PC without
modifying it: spring48J original words, friction binary/report and query48H
original words. The broader legacy tree and historical outputs remain on the PC.

`tools/gameplay/spring_model_fixtures48k.py` calculates expectations from
unmodified recovered instruction words. It never calls the native implementation.
It pins the original-word proofs and corrected saved report/data hashes.

- Four corrected saved variants exactly reproduce recorded steps, helper calls,
  input/output body fields, state and caches. Reads are restricted to the 764
  captured bytes; the full RAM snapshot and ELF are not needed for this replay.
- The preserved chain is vehicle `+0x354` → spring `+0x30` → primary body. Vehicle
  `+0x344` is gravity. The sparse replay validates the captured spring/body link;
  it asserts the preserved report's vehicle/detection chain rather than pretending
  the uncaptured vehicle/detection bytes were re-read.
- Uncaptured, unused native input words are explicitly listed zero-defaulted
  projection scaffolding. They are not claimed as recovered saved values, and
  defaults are never inserted into the guarded original oracle memory.
- 192 synthetic cases explicitly initialize every original data read. They cover
  zero through three ordered contacts, static and repeated other-body contacts,
  positive/nonpositive depth, explicit/fallback scenes, rotated/non-orthogonal
  affine matrices, custom centers, all ten materials, all impact modes, and all
  impact/friction toggle combinations.
- Coverage assertions require velocity-changing impacts, state propagation to
  later preparation, and impact changing body velocity before friction. These
  close a real gap in the four saved cases, whose impacts do not change velocity.
- Native tests compare body words, spring flags, caches and every record flag
  bit-for-bit; they also check untouched native histories and caller state.

The fixture generator is deterministic. Its final output hash and checks are
recorded in the milestone manifest. Arithmetic equivalence is ordinary,
individually rounded IEEE binary32 over the exercised finite domain, not every
physical EE/VU exceptional mode.

## Build and test

From the source tree, using an already-installed GCC/Clang-compatible compiler:

```sh
python3 tools/test_native_spring48k.py --output-dir ../spring48k-check --stack-usage
python3 tools/test_native_spring48k.py --output-dir ../spring48k-asan --sanitizers --no-leak-check
```

Each output directory must be new/empty and outside the source tree. The runner
uses only Python's standard library and the installed compiler, builds/runs
serially and downloads nothing. It records source/fixture hashes, exact commands,
compiler/host details, logs and outcomes. Missing inputs are BLOCKED, not skipped.
Explicit `--tests` subsets are marked as partial coverage. `--stack-usage` records
compiler static frame estimates, which are not a measured call-chain peak.

`--no-leak-check` is explicit here because LeakSanitizer cannot operate under this
executor's ptrace restriction. AddressSanitizer and UndefinedBehaviorSanitizer
still run. LeakSanitizer is not reported as passed.

An optional focused CMake route is also supplied:

```sh
cmake -S . -B ../build-spring -DRR_NATIVE_SPRING_ONLY=ON
cmake --build ../build-spring --parallel 1
ctest --test-dir ../build-spring --output-on-failure
```

CMake is not installed in the current Linux executor, so that route has not been
executed. It is a separate opt-in target and does not replace or stub the
Windows/D3D11 build. `NativePc48K.cmake` adds the same coordinator source/test to
the existing Windows target, and its build script hashes the new fixture.
Windows x64/x86 builds have not been rerun under this cloud-only milestone.
The complete asset/game suite still requires omitted original inputs and older
fixtures; passing the three spring tests does not certify that suite.

## Verified results

The serial Linux release build and a separate AddressSanitizer/UndefinedBehaviorSanitizer
build each pass all 90,964 focused checks: 16,392 friction, 53,254 impact and
21,318 coordinator checks over 196 cases. Both builds are warning-free and their
reports confirm no build/test input changed during the run. Measured coordinator
allocations are zero.

Two deliberately wrong temporary source copies verify test sensitivity. Recomputing
friction velocity after impact produces 153 failed checks; preparing later contacts
from stale primary state produces 120. In both experiments the four old corrected
saved cases still pass and the new synthetic cases catch the defect. These mutations
were never applied to production. Source copies, commands and failed logs are
preserved as explicitly labeled test-sensitivity evidence.

On this Linux x64 GCC 14.2 release build, two participating bodies and two records
need 1,620 bytes for output plus scratch and 72 bytes for result metadata. The
single-thread microbenchmark median is 1,344 ns/call, with a maximum batch mean
of 1,405.78 ns. These are local diagnostic observations, not a platform budget.
GCC static frame reports list 1,488 bytes for the coordinator, 1,536 for impact,
272 for friction and 128 for point-force. Nested calls, input storage and libc
frames are additional; no full peak-stack claim is made.

Production undefined-symbol inspection finds ordinary sqrt/sqrtf/memmove and
intra-spring calls, with no allocation/thread/platform symbols. Complete logs,
source/fixture hashes and compiler frame reports are under
`evidence/nativeSpring48K-linux-20261006/`. Linux executables themselves are
reproducible with the recorded commands and are not required as runtime assets.

## Performance interpretation and next work

The test includes a bounded single-thread, two-body/two-contact microbenchmark
with reused caller buffers and zero measured coordinator allocations. It records
host nanoseconds per call and buffer/type sizes. It excludes renderer, asset
streaming, complete physics, gameplay and presentation. Host timing is diagnostic;
it is not game FPS or a prediction of original Xbox performance. Compiler stack
estimates and test-process memory must not be conflated with shipped runtime
residency.

Next, recover the remaining supported-in-original response branches and the live
query producer, then integrate the original vehicle-substep policy with explicit
ownership/lifetime budgets. Keep the original Xbox's single-core/low-memory goal
visible while preserving original behavior. Only target execution and full-race
measurements can establish final memory fit, responsiveness and FPS.
