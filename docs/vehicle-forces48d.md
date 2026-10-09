# Original engine and gravity force producers — 48D

48D implements a connected portion of original vehicle simulation: the complete engine producer, single-target gravity accumulation, conditional engine-load application through an already resolved body basis, followed by the native rigid-body core recovered in 48C. All production data are ordinary typed structures. Production code does not interpret original instructions, access guest memory, call generated functions or link the compatibility runtime.

This is a **partial vehicle step**, not a complete racing vehicle. It still lacks aerodynamic and repulsor loads, contact detection/response, coupling to the second body, scene reconstruction/writeback, actual input-action mapping and the complete original clock/substep policy. Those missing systems are not replaced with generic racing physics. No gameplay FPS claim follows from these component tests.

## Files and interface

- `src/game/vehicle_forces48d.h/.cpp`: three native operations: `updateEngine48D`, `accumulateEngineGravity48D` and `stepEngineGravityBody48D`.
- `tests/native/physics48d.cpp`: independent original-instruction execution and native differential comparisons, including the connected body core.
- `tools/gameplay/recover48d.py`: offline source/ELF checking, runtime-field extraction, tuning-key chain checking and fixture generation.
- `tools/gameplay/forces_words48d.h`: exact original force-function words and actual stopped-runtime data, used only by tests.
- `tools/gameplay/force_oracle48d.h`: test-only extension of the hash-guarded, frozen 48C original-word interpreter. It adds the four force-function blocks and their required small instruction subset; expected results are not derived from the native force implementation.
- `evidence/gameplay48D`: exact original binary blocks, annotated instruction excerpts, runtime snapshot contract, key provenance, verification results and manifest.

Production source needs C++17 and strict floating-point ordering. The test needs C++20 because it includes the existing reference scalar square-root helper through the 48C oracle. `/fp:strict` is required; fast math, reassociation and fused multiply-add contraction are not permitted by this extraction's comparison contract.

## Proven connected route

The stopped player object already identified in 48C is at `0xC5B0B0`; its body is `0xD110A0`, and its `PodEngineModel` is `0xD9F9D0`. Engine `+0x30` points to that same body. Gravity component `0x987CD0` has exactly one target and that target is the same body. The associated scene object is `0xCFD8F0`, whose flag byte `+0x190` is `0x81`; the cached basis is therefore available. The race-state flag byte at `0x880190+0x58` is `0x66`, selecting the conditional engine-application route in `RunSimulation`.

These are runtime identity and dependency facts for this preserved snapshot, not fixed addresses or controllers in native code. `recover48d.py` asserts them before producing fixtures. It deliberately does not assume this player's model equals the raw Pod01 tuning file; saved values reflect the game's loading/selection and runtime updates.

| Original function / interval | Recovered behavior | Source anchor |
|---|---|---|
| `0x2E7720..0x2E7C04` | Complete engine smoothing, mode-dependent speed falloff, two local thrust vectors and two torque terms | `FUN_002e7720_0x2e7720.cpp:23` |
| `0x2827D0..0x282858` | Add gravity force to each selected target; native extraction handles one established target | `FUN_002827d0_0x2827d0.cpp:28` |
| `0x2E7C10..0x2E7D6C` | Apply both engine force vectors, then both torque vectors | `FUN_002e7c10_0x2e7c10.cpp:29` |
| `0x28B9B0..0x28BAA4`, ready-cache branch | Transform local force with the associated scene's first three cached columns | `FUN_0028b9b0_0x28b9b0.cpp:29`, `entry_0028ba1c_0x28ba1c.cpp:93` |
| `0x2E7680..0x2E76D8` | Setter connecting loader arguments to engine parameter fields | `FUN_002e7680_0x2e7680.cpp:23` |
| `0x2154A0..0x2154D4` | Vehicle PHY loader supplies the setter's arguments | `entry_00214f6c_0x214f6c.cpp:2251` |

All abbreviated filenames are under `work/native/project/generated/`. Exact disassembly comments, original ELF words, source line numbers and source hashes are preserved in the evidence. The original code-to-file mapping is `ELF file offset = guest address - 0x100000 + 0x80`.

`RunSimulation` calls the engine producer at `0x219C84`, then gravity at `0x219C90`, and conditionally engine application at `0x219CC0`. Aerodynamics, spring collision and other vehicle operations follow before the body update. The native partial step preserves the ordering of the recovered stages; it does not claim that omitting intervening unimplemented systems reproduces a real vehicle tick.

## Exact engine model recovered

The original producer never reads `f12`. Its smoothing is **per invocation**, not multiplied by elapsed time. For each of the two drive channels it chooses the ramp-down filter when the requested drive is less than or equal to zero, otherwise the ramp-up filter, then computes with separate binary32 products and addition:

```
newSmoothedDrive = oldSmoothedDrive * (1 - filter) + requestedDrive * filter
```

The model uses the linked body's cached scalar at `+0x34` as its speed input. The native interface receives this established value; it does not silently substitute a fresh velocity norm. The saved scalar is approximately `13.7742853`. A post-freeze source inspection located a writer in body wrapper `0x167B80`: it squares current velocity XYZ, sums, computes reciprocal square root and then its reciprocal, and stores `+0x34` at `0x167C18` (zero magnitude stores zero at `0x167BF8`). That reciprocal sequence must not be replaced casually with a direct square root. Another conditional vehicle path copies vehicle `+0xC70.xyz` into body velocity and writes its direct square-root magnitude at `0x219F74`. The actual ordering/frequency of those writers relative to the engine call remains a semantic-trace dependency.

The speed-falloff interval begins at `theoreticalTopSpeed - rampDownSpeed`. A nonzero engine flag `+0x34` selects the branch that adds `boostSpeed` to both interval endpoints and adds `boostThrust` to the base thrust. Otherwise, a nonzero `+0x38` selects the branch whose upper endpoint is `theoreticalTopSpeed - repairSpeed` and whose thrust is `maxThrust - repairThrust`. Otherwise the upper endpoint is the theoretical top speed and thrust is the base value. The boost branch has priority if both flags are set. The API's boost/repair names describe these established tuning branches; the physical user actions setting those flags are not established here.

For non-equal endpoints, the factor is the separately rounded expression:

```
factor = 1 + ((bodySpeed - lowerEndpoint) * -1) / (upperEndpoint - lowerEndpoint)
factor is bounded to [0,1] with the original comparisons
```

Exactly equal endpoints produce zero. The extraction retains the original expression and rounding order rather than algebraically rewriting it. In each channel, smoothed drive times this factor scales a local thrust vector along the model's positive Z. It also scales the corresponding three-component parameter vector. If engine flag `+0x3C` is zero, the resulting torque vectors are halved; thrust is not halved.

The immediate flag writers are also now anchored: `RunSimulation` sets engine `+0x34` when vehicle `+0x64` equals 1 or 2 (`0x219A90..0x219AB8`), and writes vehicle byte `+0xB9C` bit 1 to engine `+0x38` (`0x219ABC..0x219AD0`). This closes the local data dependency without asserting the complete upstream player-state or physical-button mapping. The provenance of the full-torque flag is still open.

Those three-component vectors are supplied by the loader from `LeftEnginePosition` and `RightEnginePosition` data, subject to its mode-dependent overrides. However, this specific producer/application route uses their scaled values as torque directly. The native code preserves that behavior. It does not invent a lever-arm cross product or rotate torque to make the code resemble a generic rigid-body engine.

| Engine field | Native meaning | Saved runtime value |
|---|---|---|
| `+0x40` | Base/max thrust selected by loader | 4800 |
| `+0x44` | Boost thrust addition | 1800 |
| `+0x48` | Repair thrust subtraction | 1600 |
| `+0x4C`, `+0x50` | Ramp-up / ramp-down filter | 1 / 1 |
| `+0x54` | Ramp-down speed span | 161.332596 |
| `+0x58` | Theoretical top speed, in converted model units | 879.996033 |
| `+0x60`, `+0x64` | Boost / repair speed adjustments, converted model units | 293.332001 / 36.6665001 |
| `+0x68`, `+0x6C` | Two requested drive channels | 1 / 1 |
| `+0x70`, `+0x74` | Persistent smoothed drive channels | 1 / 1 |
| `+0x80.xyz`, `+0x90.xyz` | Torque-basis parameters from the setter | `(-0.3,0,0)` / `(0.3,0,0)` |
| `+0xA0.xyz`, `+0xB0.xyz` | Produced local thrust vectors | `(0,0,4800)` / `(0,0,4800)` |
| `+0xC0.xyz`, `+0xD0.xyz` | Produced torque terms | `(-0.3,0,0)` / `(0.3,0,0)` |

The snapshot evidence stores every value's exact bits. Rounded numbers above are explanatory only. Six scalar tuning-key chains are automatically checked from original literal address through parser destination, setter argument and final engine-field store. The loader multiplies its speed-related fields by the original float constant `0x3FBBBB84` before the setter; the native API expects those already converted runtime values. Base thrust/top speed can be selected or interpolated by the loader; this work does not bypass that process with arbitrary defaults.

## Gravity and application contract

The gravity routine writes its Y force as `(-bodyMass) * gravityMagnitude`, retains the component's X and Z force values, and adds X/Y/Z to the body's force accumulators in order. The native operation handles one target whose membership the caller has established and returns the body-accumulator effects; it does not expose the gravity component's overwritten scratch Y field. The saved component's list has one target; a future multi-body vehicle must recover and supply the original target membership, not assume every body receives gravity identically.

Engine application obtains each local thrust vector in turn. If the body has an associated scene transform, it rotates that vector by the ready cached columns and adds it to force. The numerical order is `column0*x`, add `column1*y`, add `column2*z`, then add the VU zero XYZ vector. Each arithmetic result is rounded to binary32 as in the reference path. It applies the first thrust, then the second, rather than summing local thrusts before transforming.

It then adds the first and second torque vectors directly, without rotation. If there is no associated scene transform, the original skips thrust and still adds torque. The standalone accumulation API reproduces that branch. The composed partial step requires the caller's resolved basis and body-core entry state, so it uses the ready-transform route.

The native `applyEngine` input corresponds to the caller's decision to call `0x2E7C10`. It is separate from engine generation: when application is disabled, generation/smoothing still occurs. Gravity still occurs. The API accepts existing accumulated loads and preserves addition ordering. It does not clear accumulators; clearing belongs to the as-yet incomplete outer body-update contract.

## Validation scope

Both Windows x64 and x86 passed **423,693 checks each** in parent build `evidence/nativePc48D-build-20261003-134330-366`. The local validation record and copied logs are in `evidence/gameplay48D/validation.json` and `validation-{x64,x86}.log`. The test compares all **56 output floats** in the connected result: fourteen engine-state values, six accumulated-load values and thirty-six body-state values, including the persistent integrator histories.

The oracle executes the original engine/gravity/application words and ready-cache transform branch, then the original 48C body words. It uses a bounded 8 KiB fixture, true delayed branches/calls/returns and an instruction budget. Test-only addresses are relocated fixture addresses; no full guest runtime or game boot is involved. Dirty transform-cache traversal is deliberately outside the supported oracle/production contract.

Coverage includes 6,400 randomized connected steps, 1,024 successive partial steps and the actual saved-runtime fixture in each of four host rounding modes. Inputs exercise boost/repair priority, speed-falloff and equal-endpoint branches, positive/zero/negative drive channels and varied filters; conditional application; nontrivial bases and existing force/torque. Specific assertions distinguish time-independent engine smoothing, boost priority, half-torque behavior and absent-scene force suppression. Domain failures and a late body-stage failure must leave the complete output unchanged. An initial test compilation failed on a missing aggregate closing brace; the test-only syntax repair and failure log are preserved, and the corrected source passed both architectures.

The saved snapshot case and trajectories are controlled partial simulations, **not observed before/after racing ticks**. Trajectories explicitly supply their speed and ready basis; they do not claim ownership of the missing scene/clock/state updates. Ordinary scalar/VU oracle arithmetic uses the host reference semantics; the 48C SQRT/RSQRT helper remains in the body oracle. This does not certify all physical PS2 exceptional modes, FTZ/DAZ settings, extended float or exception flags. Finite consumed values and intermediates form the accepted native domain; API rejection of invalid inputs is not a new claim about the original game's validation behavior.

## Next integration gate

The recovered engine now produces actual original-model loads for the proven body core. The next gate is a real semantic trace across one original substep, recording the cached speed, mode flags, drive channels, ready basis, preexisting loads and every additional force producer's contribution. Match the engine output and gravity/application accumulations first, then recover aerodynamics/repulsor/contact loads and the body's scene wrapper in original order. Preserve true guest-return identity across yields.

The timestep issue remains unchanged: the original scheduler splits its incoming factor using the recovered `trunc(float32(factor*60))+1` policy. Engine smoothing is evaluated for each actual producer invocation. Presentation frequency must not redefine that policy. Upstream clock scaling, pause behavior, dynamic cached-speed updates and physical action mapping still require proof.

Original hashes and exact extraction provenance are in `recovery-manifest.json`. The final owned-file manifest is `checkpoint-manifest.json`; parent checkpoint archives preserve complete recoverability. All 48B and 48C source/evidence are preserved.
