# Racer Revenge native PC implementation plan

2026-10-03. Planning checkpoint 48A. Immediate target: a fast native x86-family
PC game; eventual target: the same game code on original Xbox. This plan follows
three read-only assessments and inspection of the saved source and original
assets. No new game executable or performance result was produced in this turn.

Implementation follow-up: [checkpoint 48B](native-progress48b.md) records the
first native viewer, bounded original-data decoder and recovered control helper.
The built-in converter requirement below was added during that implementation.

## Decision

Create a separate native game target, and recover the game in bounded vertical
slices. Commit to the architecture now; do not bulk-rewrite unknown systems.
The major performance opportunity is to execute game operations directly over
native game data, removing PS2 instruction dispatch, hardware scheduling and
graphics-command execution from the production path. This is a direction to
measure, not an order-of-magnitude speedup already demonstrated.

The first rendering milestone is a real track section and pod drawn from
decoded original assets. The first gameplay milestone adds the recovered
vehicle simulation, controls, collision and camera for that section. A static
viewer, moving a model with invented physics, or replaying recorded transforms
does not satisfy native gameplay acceptance.

## What the assessment established

| Finding | Consequence |
|---|---|
| Root CMake targets build compatibility components and checks. `src/core` contains EE registers, an EE/IOP cycle ledger, guest-PC lookup and scalar/MMI instruction execution. | Preserve these as reference helpers; their name does not make them the new game engine. |
| The current Windows reference uses the generated game corpus plus PS2Recomp/PCSX2 machinery. The Xbox executable is diagnostic only. | A native platform and game interface still need implementation. |
| Existing bounded RES decoding succeeds on `TRACKS/TA.RES` (383 members), `PODS/POD01/POD01.RES` (96), and `UI/PODPHYS.RES` (23). | Begin from original resource data, using the existing decoder as an offline recovery aid. |
| All 230 TA geometry members and all 13 POD01 geometry members have `psg\0`, version 3. CarsTools writes PSG version 5. | CarsTools supplies semantic clues, not a drop-in binary schema. |
| `geometry\zone01groundstrips16.psg` is a 1,552-byte TA member and names `ta_sand8`. Its material explicitly requests `PreLitCompressed` and `TextureMap_1=ta_sand8`. | This is a small, concrete first mesh/material case. |
| TA's `ta_sand8.pst` uses an earlier texture layout than the supplied Cars source. | Validate the game's actual palette, mip layout and swizzle; do not apply the later texture reader blindly. |
| The original ELF contains `PodVehicle::RunSimulation`, referenced from `FUN_00219810` at guest address 0x219810. | A strong simulation trace anchor exists; field meanings and live call behavior still need proof. |
| Its caller subdivides a time-like float using a 60.0 constant and invokes vehicle updates repeatedly. | Preserve recovered subdivision and update ordering; do not substitute one physics step per VBlank or assume presentation rate equals simulation rate. |

Reproducible input hashes, resource indexes, small extracted samples and source
excerpts are in `evidence/native-plan48A/assessment.json`. The inspection is
implemented in `tools/inspect_native_plan48a.py`. It reads original files and
writes evidence only. It does not decode a complete renderable PSG mesh yet.

Important anchors:

- `work/native/project/scripts/validate_ui_resources07.py:13`: bounded RES decoder.
- `work/native/project/generated/FUN_00219810_0x219810.cpp`: simulation candidate.
- `work/native/project/generated/entry_001a8060_0x1a8060.cpp:1331`: subdivision setup; vehicle calls around lines 1515–1655; repeat logic around 1788–1814.
- `third_party/cars-tools-reference/CarsTools/SLTConvert/DLL/SLTConvertDLL/SLTConvert.cpp:323`: original-Xbox DXG versus PS2 PSG exporter selection.
- `third_party/cars-tools-reference/CarsTools/SLTConvert/DLL/SLTConvertDLL/BaseConvert.cpp:654`: quantization conventions.

## Architecture and reuse

Use a Windows x64 development executable with a small Win32/D3D11 backend.
Require the same shared game and asset code to build and pass semantic checks
as Win32/x86. This is native x86-family execution, with explicit 32-bit
portability for Xbox; it does not require the Windows graphics backend or CRT
to obey Pentium III instruction limits. D3D11 headers and import libraries are
available locally. Actual device creation remains an implementation check.
Raylib already exists in the reference dependency tree and is a fallback only
if it reduces a demonstrated blocker.

Suggested new areas, leaving historical files in place:

| Area | Responsibility |
|---|---|
| `src/game` | Race and vehicle state, controls, original simulation policy, collision, camera, AI and race progression. |
| `src/assets` | Semantic meshes, textures, materials, collision data, animation and stable resource IDs. |
| `src/render` | Scene instances, transforms, visibility, material parameters and draw descriptions. |
| `platform/windows/native` | Window, native GPU resources, input, audio, file access and timing. |
| `tools/assets` | Offline extraction and conversion with versioned schemas and provenance. |
| `tests/native` | Recovered-function fixtures, semantic replay comparisons and asset validation. |

The production interfaces take game concepts and ordinary native values. They
must not expose EE registers, guest PCs, MMIO addresses, guest RAM, VIF/GIF
packets or EE/IOP cycle ratios. Use explicit-width serialized fields and offsets,
not raw host pointers or compiler-dependent struct dumps. Native game correctness
must work on one simulation thread; desktop parallelism remains optional.

Reuse verified asset decoding, original parameters, mathematical behavior,
captured evidence, toolchain pins and recovery tooling. Recovered functions may
originate in recompiled/decompiled code, but accepted native implementations
must operate directly on game data without a CPU/device environment. Keep
PCSX2, VU/GS, IOP and original guest execution available in a separate reference
tool. Preserve their sources and licenses. CarsTools remains a knowledge source
outside production build targets.

## Built-in converter requirement — 2026-10-03 clarification

The PC application must provide a built-in original-data import/conversion
workflow. The user selects the original disc image or an extracted game-data
directory; the application validates supported input and converts it into a
versioned native asset cache. Ship the converter with the PC application so
users do not have to install or operate the old Cars/3DS Max tools or development
scripts. The present bounded offline decoder is the development foundation,
not the completed user-facing importer.

Include progress, cancellation, resumable work and clear unsupported-format
reports. Track source hashes, converter/schema versions and output hashes.
Write new results to staging and promote a verified conversion atomically, so
failed or interrupted imports preserve the last usable cache. Reuse unchanged
validated assets, and rebuild only those invalidated by source/schema changes.
Keep original inputs unchanged. Normal gameplay loads native cached data;
conversion is a setup/update operation. Later Xbox package export uses the same
semantic conversion pipeline with platform-specific outputs.

Implement the import service around the validated decoders as their supported
scope expands. An early mesh-only importer must describe that limited scope;
do not present it as a complete game importer. Integrate the built-in workflow
before PC playable distribution acceptance, while the immediate mesh and
simulation recovery milestones continue.

## Ordered implementation packets

### 0. Freeze the reference and establish native boundaries

Preserve the completed 47W runs and the original source/archive chain. Record
reference bugs, including unverified sky/surface appearance, so incorrect
reference output is not mistaken for intended art. Add the independent native
target and a clean launch directory; source lists and link dependencies must be
explicit. Establish native frame, simulation-time and allocation counters.

Gate: native host builds on Windows x64, shared code builds on Win32, and the
native executable starts without any PS2 runtime, original executable or guest
RAM image. A blank window is infrastructure only, not a gameplay milestone.

### 1A. Recover one original mesh and material

Start with TA `geometry\zone01groundstrips16.psg`, then
`geometry\ta_bigrocka.psg`. Use material `textures\ta_sand8.psm` and texture
`textures\ta_sand8.pst`. Compare the version-3 game loader, raw payload and
CarsTools' later hierarchy/quantization conventions. Explicitly recover vertex
positions, strip/index topology, restart/winding rules, UVs, colors, bounds,
material flags and transforms. Separate static mesh coordinates from placement
and camera state. Unsupported cases must be reported, never guessed silently.

Output a minimal versioned native mesh/material/texture package with original
container hash, member name, logical offset, source hash, converter version and
output hashes. Parsing PS2-packed asset data offline is allowed; dependence on
runtime GS packet playback is not.

Gate: bounded parse consumes the expected records; indices and bounds validate;
decoded attributes agree with independently observed game loader/VU inputs for
the same mesh; texture palette and mip output are checked. This gate must not
depend solely on the converter agreeing with its own serializer.

### 1B. Capture the simulation contract in parallel with 1A

Trace actual calls at 0x219810 and the caller continuations 0x1A8404/0x1A8468.
Record incoming object identity, candidate time value, flags, control state,
substep index, observable object changes and invoked components. Determine
which object is player-controlled. Identify position, orientation, velocity,
collision state, tuning parameters and camera ownership through reader/writer
and call evidence, not plausible-looking floats alone.

Pair guest entry with actual guest completion using thread, saved return
address, original stack pointer and call/substep ID. A host C++ function return
may be a dispatcher yield and is not necessarily completion of the game call.
Preserve exceptions to the common update path and time-conversion semantics.

Gate: one reproducible simulation interval has identified inputs, outputs,
substep order and dependencies. Unknown fields remain named unknowns. If the
full vehicle update is too broad, recover the smallest evidenced component
first, preserving the surrounding trace; do not invent a substitute model.

### 2. Native scene milestone

Render the validated track geometry and an original pod using native buffers,
materials and textures. A free camera is appropriate for inspecting axes,
winding, UVs, depth, alpha, sky and surface appearance. Expand to one contiguous
track section after the single-mesh case passes. Recover placements and
hierarchy; they must not be approximated from a final screenshot.

Gate: interactive native rendering from original semantic assets, validated
against suitable original-game evidence. Captures known to have rendering
defects are diagnostic references, not unconditional visual goldens. Record
which effects and materials are implemented or still missing. Report scene
performance as scene performance, not racing FPS.

### 3. Native gameplay milestone

Implement the recovered simulation over typed native state, integrate original
controls and tuning, collision for the section, and the recovered camera.
Preserve the original subdivision/update policy and relevant numeric behavior.
Rendering and presentation run independently of simulation scheduling.

Gate: a controllable original pod drives a real track section using native game
state and collision. A fixture compares one complete recovered step, then a
deterministic input sequence. Discrete events must agree; numeric tolerances
must be specified in known game units before evaluating results. Compare
trajectories, orientation, collisions, speed, boost/damage where applicable and
camera behavior. Do not repair mismatches by overwriting state from captures.

This is the first milestone that can support a claim of native gameplay.
Saved EE/IOP RAM equality is useful for reference-runtime changes, but is not
the acceptance condition for different native memory layouts.

### 4. Complete a native race and meet PC performance targets

Expand to the complete track, checkpoints/laps, opponents and AI, sound,
effects, loading and the minimum selection/restart flow. Add representative
cases for straight driving, cornering, collisions, boost/damage, crowded scenes
and sector boundaries. Include restart and repeated laps to expose state and
resource leaks. A complete lap and repeatable racing are required before
claiming a playable race; broader game content remains a separate completion
scope.

Profile actual native code and assets at this stage and earlier milestones.
Optimize measured native bottlenecks in batches, preserving behavior. More
diagnostic caching in the compatibility runtime is not the main workstream.

### 5. Original Xbox packaging and platform work

After the PC path is validated, specialize texture/buffer formats, streaming
packages and the Xbox renderer/audio/input implementations. Keep semantic
asset definitions shared; the Cars DXG format is an example, not a requirement.
Measure total Xbox residency and transient load peaks, including GPU resources
in shared RAM, and execute on the target before accepting memory fit or FPS.
This stage must not block the immediate native PC milestones.

## Performance and fidelity acceptance

Provisional PC target: full-speed gameplay with 60 presented frames per second
at 1280x720 on the available machine. This is a planning target, not a promise
or a claim that the user specified 60 FPS. The host is a Ryzen 5 5500U with
integrated Radeon graphics and approximately 11.9 GiB visible memory; record
power state, resolution, driver, executable and asset hashes in each result.

Use at least three matched repetitions after warm-up. Record simulation-time
advance per real second separately from presented frames, CPU update/render
submission time, GPU time where available, frame-time median/p95/p99/maximum,
loading stalls, working set and owned asset/GPU/scratch memory. Aim for a
16.67 ms frame budget and inspect the tail. Normal input, sound and declared
scene complexity must be included for a gameplay result. Never obtain a gain
by changing game speed, skipping collisions or hiding missing features.

The historical 5.77 to 10.05 and 10.05 to 11.29 measurements were emulated
VBlank throughput under particular reference conditions, not native presented
FPS. Do not multiply those separate ratios, compare a static viewer against a
full reference race, or scale desktop clocks to predict Xbox performance.

## Production independence gate

Before calling a milestone native, audit the source allowlist and linker map
for compatibility objects as well as PE imports and loaded modules. Imports
alone miss runtime `LoadLibrary` paths. Launch from an isolated package
containing the new executable, allowed platform dependencies and converted
assets. Verify that gameplay does not read a PS2 executable, memory snapshots,
VU programs or captured frame streams, and cannot fall back into the reference
runtime. Offline analysis tools are separate and can use those inputs.

Keep Xbox portability visible through Win32 shared-code checks, bounded
allocation reporting, explicit serialized widths and occasional nxdk shared
object compilation/ISA audits. These checks are not Xbox boot or whole-program
memory acceptance. Do not force PC-only dependencies into the Xbox target.

## Risks, decisions and stop conditions

- Geometry and texture revisions differ from CarsTools. Resolve the real game loader and bytes before extending a schema.
- Simulation fields, units, component calls and collision contracts are not fully mapped. Finish the trace contract before committing a bulk physics rewrite.
- Original subdivision means a naive fixed 60 Hz replacement could alter handling. Recover policy first; presentation can still target 60 FPS.
- A visual reference can contain known renderer errors. Validate art from appropriate original execution and asset evidence rather than reproducing defects.
- Gameplay, collision, materials and audio may require substantial reverse engineering. No defensible completion date or Xbox FPS forecast exists yet.
- If a proposed native subsystem still needs guest instruction/device execution, mark it as a reference adapter and revise the boundary before accepting the milestone.

## Coordination and recovery

Three bounded work areas are appropriate: assets/rendering; gameplay tracing
and recovery; platform/portability plus independent review. Agree data contracts
before parallel edits, assign non-overlapping files and keep one integration
owner. Run performance measurements with other builds, large scans and archives
idle. Agents can inspect independently; simultaneous rewrites of shared
interfaces are not the plan.

At each gate create a Markdown checkpoint, source and evidence ZIPs, and a
manifest containing exact hashes, toolchain/settings, observations and blockers.
Reopen every archived member and verify its SHA256. Preserve failed candidates
and clearly identify the last accepted state. No user-run Windows tests or new
uploads are required to begin these packets.

Immediate next implementation work is packets 0, 1A and 1B, followed by the
native scene and gameplay gates. The old goal record currently reports
`usageLimited`; this plan records the user's newer PC-first direction without
claiming that the goal service has been resumed or its old wording updated.
