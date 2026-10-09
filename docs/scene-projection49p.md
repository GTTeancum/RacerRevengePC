# 49P: Fixed-node scene state and phased Chariot projection

49P adds actual scene-state behavior that the body diagnostics previously left outside their admitted domain: local-matrix writes, ancestor dirty flags, conditional descendant invalidation, lazy world caches, and the original physical-mode secondary-scene-to-Chariot copy.

The implementation is a separate, bounded projection over caller-owned records. Existing 49K/49M body APIs and admission guards remain unchanged. It does not establish race spawn or turn a constructor state into `FrameReady`.

## Original topology and phase

The original creates a main physical scene at vehicle190 and attaches graphics container194 beneath it at 211A34. The secondary physical scene is vehicle19C. Chariot model1B4, right engine1B8 and left engine1BC are separate graphics roots, not three physics bodies or automatic children of194.

In the supported physical graphics mode, original 21B9A4 checks live838 != 0 and live83C == 0. At 21B9BC–21B9C8 it copies the secondary scene's complete **local C0** matrix to Chariot1B4 through28B490. It does not copy cached world100, add the physical target2F0, or immediately refresh every model node.

Base update228620 runs these stages before later physics:

1. First-base nullcg preparation217440 when needed.
2. Controls217DE0/2176E0.
3. Animation21A000.
4. Engine/container visual response21C120.
5. Chariot graphics21A660, including the selected copy branch.
6. Later body frame caches and physics substeps.

The projection API requires an explicit pre21B9A4 phase and complete supplied scene state through the preceding Chariot prefix. Its tags are caller attestations, not proof that an unimplemented scheduler ran. Current locals alone are insufficient: external animation/visual producers may also have changed cached worlds and ready flags. Unsupported graphics modes reject; they are not silently treated as no-op graphics.

## Fixed-node scene API

`scene_graph49p.h` represents up to64 nodes using native indices, complete local/world matrices and original flag bytes. The caller supplies a coherent acyclic forest. Roots have no sibling link; each child appears exactly once in its parent's child/sibling list. Node storage order need not be parent-first. This is a bounded native admission, not an original heap/ownership implementation.

The implemented operations are:

-289B20, admitted with relative=null and parentless target: replace all16 local matrix words, including translation and fourth lanes.
-28AF90 under the same context: add displacement XYZ to local translation; preserve local W and all basis lanes.
-28B490: direct complete local-matrix copy, including opaque copy-only float bits.
-28C4F0: set bit80 on the changed node and its ancestors, retaining other flags.
-289A20: process next siblings before the current node. Only a ready node has bit01 cleared and its children visited. An already-dirty node skips its descendants.
-2897B0/289940: ready getters retain cached world values. Dirty roots copy local to world; dirty children first refresh required parents, then compute world = local × parentWorld in the exact original separate multiply/add order. Only requested ancestors change.

A setter invalidates descendants only when its target was ready. It does not repair a supplied dirty-parent/ready-child combination. Child invalidation does not set child bit80, change local matrices, or refresh world caches. World refresh does not recompute unrelated descendants or siblings.

All16 lanes and signed-zero behavior are retained. Copy-only paths do not canonicalize float bits. Arithmetic operands/intermediates must be finite under FE_TONEAREST with no fast-math or FMA. A rejected call leaves represented graph state unchanged. No dynamic allocation or runtime callback is introduced.

## What remains supplied or excluded

Scene object's observer field+19C is distinct from vehicle19C, the secondary scene pointer. The separate 28EA20 observer path is not implemented or replaced by a callback stub. Bounds recomputation, scene ownership/lifecycle, complete animation and engine visual response are also outside these functions.

Normal engine mode834=0 has persistent per-engine visual state, authored coefficients, main/component inputs, smoothing and shared noise/RNG. Two physical poses cannot determine all three model transforms. The nondefault834=1 shortcut is not substituted. Current194 and animated model-node states therefore remain explicit inputs at the source phase boundary.

The actual race-grid producer2A2880 needs initialized TunnelTrack nodes and StartNode. That remains dependent on unreleased49C;2169C0 only caches placement inputs, and full physical placement/height/support effects are not closed. Constructors have identity local scenes but dirty zero world caches and zero derivative histories; they are not an authentic ready spawn.

Prior holds concern specific capacity-alias and ownership/allocation-failure diagnostic/repair operations, not a blanket ban on every reset or query function.49P neither retries those operations nor imports49C changes.

## Verification

All41 aggregate suites pass in normal and ASan/UBSan builds:124,535,876 checks per mode, with all243 tracked input hashes unchanged. The aggregate uses the ordinary uninstrumented production object and explicitly reports ordered-store tracing off. Separate focused normal/sanitized builds require a compile-time evidence hook and compare18,757 exact source-ordered flag stores. A requested trace build without that hook demonstrably fails.

- API:1,842 operations and70,728 checks; zero hot allocations.
- Focused original-word scene proof:586 fixtures,14,141 stages,43,437,251 checks and862,000 original instructions; all10 semantic controls detected. Uninstrumented aggregate scene proof:43,385,595 checks.
- Chariot proof:1,168 fixtures,295,424 original instructions, all12 dispatcher words covered, both gate outcomes,16 detected controls.
- Independent review:8,812 operations,10,316,668 comparisons,779,831 original instructions and nine concrete-state/order controls; no production defect found.
- All542 selected original words and five static topology/caller/observer boundary ranges are pinned to the original ELF.

LeakSanitizer is disabled because of the recorded ptrace environment limitation. ASan and UBSan are active. Production is strict C++17; the inherited aggregate uses C++20 for historical test helpers. CMake is unavailable here, so installed GCC directly runs the supported scripts.

## Persistent projection replay

The new diagnostic consumes a compact hash-pinned sequence from the verified49O body trace as **supplied physical states**. It does not rerun old body tests and call them new scene behavior. It uses a22-node forest: main190, child194, separate secondary19C, separate Chariot root and18 authentic Pod01 hierarchy nodes.

The replay has14 scenarios,1,680 admitted pre-physics copies and3,780 later physical samples. It compares46,074 complete scene states against original instructions, including every local/world lane, flag and link. Native calls allocate nothing. Normal, repeated normal and sanitizer output traces are identical.

The first sample per scenario is an explicit diagnostic seed, not a recovered pre-frame0 or race spawn. Current animation/container records are supplied fixtures; nullcg clearing is a labeled post-first-frame condition, not execution of the whole217440 producer. The earlier graphics gate is explicitly838=1/83C=0 and is not inferred from later physics838 fields.

Every later physics sample invalidates the known child194 lazily, while the already copied Chariot pose persists until the next base phase. Wrong post-physics copying changes1,515 frames/3,475 substeps; eager worlds and an invented2F0 offset each change1,680 frames; cached-world copying changes1,066 frames. These controls directly test the recovered temporal/data boundaries.

## Cost and recovery

A SceneNode49P is144 bytes:22 active records occupy3,168 bytes;64 occupy9,216. The view and phase input add36 bytes. Existing49O body/state/parameter sizes are unchanged. The two new host objects contain2,542 executable bytes and20 read-only bytes, with no mutable data/BSS.

CPU0/64MiB-cap host benchmark,15 ×20,000 iterations: the explicit copy → later root writes →20 getters workload has median4.873 microseconds; ready-cache getters2.969 microseconds. Timed code has neither interpreter nor allocation counter; separate ordinary-new probes show zero calls. This is a native stage measurement, not gameplay FPS or an Xbox measurement.

Compiler frame estimates include4,496 bytes for refresh and32 bytes per recursive invalidation frame, at most64 calls. Validation uses the host red zone despite a small `.su` number. These are not measured stack high-water bounds; caller records and runtime costs are additional. Full raw measurements and caveats are retained under `evidence/sceneProjection49P/{footprint,benchmark}`.

Reproduce all suites with `python3 -B tools/test_native_scene_projection49p.py --output-dir <new-external-directory>`; add `--sanitizers --no-leak-check` for sanitizer mode. Focused required-store proof uses `tools/gameplay/scene_graph_recover49p.py --build-dir <new-external-directory> --output-dir <evidence-directory>`.

The source derives from exact49O SHA256e4819740af694cf436dfa03cb4a026481ea57e1d480c912e0de55fdff76a0bbe. All18,203 baseline members are preserved, except the intentionally updated inherited README/CMakeLists.49C remains excluded and unreleased; collision-capacity and ownership repair work remains untouched.
