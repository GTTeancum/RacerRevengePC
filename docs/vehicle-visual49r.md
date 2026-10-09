# Original container and engine-root response, checkpoint 49R

This stage produces the original normal-mode graphics-container orientation and both primary engine-root local matrices. It replaces the supplied container/root matrices in the earlier scene diagnostic. Actual Pod01 PHY parameters and PSG offsets drive persistent response position, velocity, angular velocity, quaternion and phase clocks. The connected diagnostic runs the existing authored brake/fin MOT producer first, then this complete admitted `21C120` call through its real return at `220894` and delay slot at `220898`.

The original ELF is `work/native/project/disc/SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Retained instruction blocks, asset hashes, source execution, semantic controls and independent review are under `evidence/vehicleVisual49R/`.

## Complete admitted call and remaining inputs

`updateVehicleVisual49R` admits vehicle mode `834==0`, both primary engine roots, absent optional roots `1C0/1C4`, and an empty `B54` rotating-node list. The coherent caller graph contains at most 64 records. Main `190`, right `1B8` and left `1BC` are distinct parentless roots; graphics container `194` is a direct child of main. These checked bindings are supplied resolved identities, not a resource-discovery algorithm.

The normal base order is controls, `21A000` engine-node animation, `21C120` container/root response, `21A660` Chariot response, then later physical substeps. The new phase enum is a caller assertion. This stage does not implement the entire dispatcher. Current main pose, application bits/frame factor, cached body speed, engine and aero state, damage, probe and impact fields remain explicit stable phase inputs. Engine `70/74` are distinct prior component values, not aliases of current `68/6C`.

The selected call runs to completion. Other vehicle modes, optional roots and nonempty rotation lists reject. Chariot motion/IK, original model creation, renderer palette consumption, authentic spawn and contact generation remain open. The existing 49M supplied PostBase contract and childless physical scene admission are byte-identical to 49Q. No body wrapper is invoked with the diagnostic's expanded model forest.

## State and source order

1. Initialize the six entry lazy graphics globals only when their guard byte is zero. Preserve effective warm words. Read main world position and axes through the original lazy scene behavior.
2. Smooth persistent C20 with the source per-call coefficient, form the corrected orthogonal axes with source VU arithmetic, orient the child through the original two-transpose parent conversion, refresh its world and publish AF0. Preserve child local XYZ; the orientation path sets its translation fourth lane to one.
3. Shape engine/aero differential, slide, damage, speed and response amplitudes with the original branches, comparisons and operation order. Update all four clocks, including the two associated with absent optional engines.
4. Read both old response quaternions and positions before either side advances. Build the common separation/binder and transform impact BA0 with the main scene inverse direction. Lazy impact globals remain shared.
5. Integrate the right response, generate its temporary visual noise, apply source Y feedback, normalize its visual quaternion, compose its local pose with container world, and write the right model-root LOCAL matrix.
6. Integrate and publish the left using the same old-state common preparation. Initialize the final lazy global even with the admitted empty rotation list.

Response integration is the original semi-implicit position/velocity and quaternion update, not the 48C body history integrator. Roots remain lazy after local writes. Scene flag/order semantics reuse 49P; the proof compares complete records and ten logical source-PC stages, while the focused container proof separately compares ordered flag stores. Native memory stores are not claimed to have an identical instruction-by-instruction traffic pattern.

`844==0` selects twelve original noise calls, right six then left six, even when the selected amplitude is zero. The calls consume the same caller-owned 49M noise/RNG context across vehicles. The four clocks and lazy graphics words also persist. Noise is normally temporary visual displacement/orientation; the source post-noise vertical clamp writes persistent Y and clears Y velocity. The noisy quaternion is not fed back into the persistent quaternion. The second quaternion normalization also occurs when the noise flag is nonzero.

## Actual authoring and fresh projections

`readVehicleVisualAuthoring49R` reads the 26 consumed PodAnimation scalars and `RepulsorMaxLiftHeight` from the retained actual Pod01 PHY. It reuses the 49N scalar parser and rejects missing required keys, duplicates, malformed numbers and unsupported source bounds. It supplies no invented fallback tuning.

`bindFreshVehicleVisualParameters49R` applies the original assignments and floating-point multiplication order. With conversion word `3FBBBB84`, authored speed 400 produces A54 word `48A80DD5`; authored input speed 200 produces A68 `4392AA7F`; lift height 5 becomes `40A00000`. Right/left graphics offsets come from actual PSG nullcg world positions under fresh identity wrapper roots, then source Z clearing. They are not PHY engine-position fields or a raw-local shortcut. Their X words are `409D9F95` and `C09D9F95`.

Two explicit initializer helpers project only the proven fresh visual records and cold shared graphics globals. Fresh response XYZ is zero with vector W one; quaternions are scalar-first identity; all four clocks are zero; C20/AF0 is `(0,1,0,1)`. Neither helper takes, seeds or resets a noise/RNG object. They are not whole-object constructors, live-state repair, `FrameReady` or race-grid initialization.

## Numeric and storage contract

The path uses nearest/even, separate binary32 operations with contraction disabled. It preserves the retained CPU reciprocal-square-root and VU square-root/divide distinctions. VU normalization admits genuine zero with saturated Q behavior; positive subnormal squared norms are outside the admitted domain. It adds no fallback axis. Consumed values/intermediates must satisfy the documented finite domain. Copy-only W words, ready unused locals, unselected parameters and warm store-only globals can retain opaque words; there is no blanket all-fields-finite scan.

Graph/state/shared/noise/workspace/diagnostic and immutable views must be distinct live caller records. Defensive overlap checks cover specified mutable spans and scratch versus input descriptors; they do not establish lifetime or complete alias safety. Diagnostic is assigned first. Within the admitted record contract, failure preserves graph, persistent state, shared graphics and noise/RNG; caller scratch can change. Internal staged functions are not separately atomic public APIs.

The hot path is serial and performs no heap allocation, I/O, callbacks or ownership changes. The workspace is 11,548 bytes: 9,216 bytes for 64 scene records, 176 response-state bytes, 96 shared graphics bytes and 2,060 noise-context bytes. The benchmark's 32-node caller payload totals 18,804 bytes. Four new strict C++17 production units contribute 34,751 text bytes, 818 read-only-data bytes and 648 relocation-backed constant-descriptor bytes, with zero BSS in the measured host objects. The runtime three units contribute 30,645 text bytes and 268 read-only-data bytes. Compiler stack reports are estimates, not measured high-water usage.

Installed GCC 14.2, one host CPU and a 64 MiB normal-process limit measured six modes over fifteen samples each. Warm zero-delta medians are 3.488 microseconds with noise and 3.294 without; bounded 240-step 1/60 histories are 3.340/3.329; cold first calls are 11.271/8.567. Flag 844 also changes a downward-force term, so that pair is not an isolated noise overhead. These are subsystem host timings, not game FPS, original-Xbox or full-frame results.

## Evidence and reproduction

Focused normal and ASan/UBSan builds pass; LeakSanitizer is explicitly disabled in this traced environment. The complete-call proof executes 810 calls, 2,000,747 checks and 9,020,719 original instructions, including seven full cold noise initializations and 12,537 RNG calls. It authenticates 7,017 original words and records reached/excluded/unexercised coverage honestly. Fourteen source/native semantic mutants fail as intended. No normal source instruction is substituted.

The connected fixture runs six interleaved persistent vehicles for 576 frames, including 24 original base skips and 552 complete visual calls. It uses actual 49Q engine MOTs, 52 scene records, shared graphics/noise state and explicit later saved physical poses. Every active frame moves engine roots; 3,815 actual brake/fin nodes change. Three normal and three sanitized repeats produce identical traces. Its six mutants and five deliberately wrong connected alternatives demonstrate sensitivity to source ordering, old component inputs, clock choice, shared RNG and later physical pose placement.

The connected fixture's prepared 49I factor comes from explicit quantized timestamps; control/animation clocks remain separate supplied fields. Later physical poses come from preserved 49P fixtures and are applied only after the visual outputs and explicit world getters. Initial physical poses are saved post-substep samples, not original spawn. Chariot state stays preserved. This is a concrete numeric production connection with supplied external boundaries, not a complete game frame.

Run the 49 related suites without CMake or downloads:

```sh
python3 -B tools/test_native_vehicle_visual49r.py --output-dir /tmp/rr49r-normal
python3 -B tools/test_native_vehicle_visual49r.py --sanitizers --no-leak-check --output-dir /tmp/rr49r-sanitized
```

Focused original-word and connected runners are retained beside their evidence. Instrumented source proofs require their documented stage hooks; ordinary aggregate production runs omit hooks while retaining all numeric comparisons. All 49 related suites pass in each mode, with 150,183,835 checks, 42 production units and 283 unchanged tracked input files. Final pins, release checks and preserved baseline differences are in `evidence/vehicleVisual49R/validation.json` and the checkpoint manifest. The aggregate inherits C++20 for retained interpreter headers; separate focused and footprint builds require C++17 for the four production units. CMake registration is provided but could not be executed because CMake is not installed here.

49Q remains a recoverable baseline. The historical collision contract remains unresolved and its files unchanged. The separate 49C work is excluded and unreleased; neither held historical diagnostic was retried or repaired for this stage.
