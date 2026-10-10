# Native rigid-body numerical core — 48C

48C recovers and implements the original vehicle's rigid-body numerical integration core over typed native data. This is a meaningful simulation component, not a whole vehicle, race, or playable game. It is not connected to the native viewer. No full reference-runtime build or run was needed for this recovery.

The implementation is `src/game/body_integration48c.h/.cpp`. It has no guest addresses, virtual memory, instruction dispatch, generated game functions, PS2 registers, or runtime dependencies. Original instructions exist only in the independent test fixture/interpreter in `tests/native/physics48c.cpp` and `tools/gameplay/physics_words48c.h`.

## What is proven

The preserved player object at `0xC5B0B0` contains its main body pointer at `+0x340`, pointing to `0xD110A0`. The body vtable at `0x3CE000` identifies `PhysicsRigidBody` through original type information; slot `+0x3C` is `0x167C40`. Its integrator pointer at body `+0x290` is `0x14EE40`. The vtable and type-name chains are checked against the original ELF. These addresses identify this stopped snapshot, not stable native object identities.

`PodVehicle::RunSimulation` at `0x219810` calls this body through the `+0x340` component at `0x219E80`, passing its substep factor unchanged in `f12`. The body obtains a position and quaternion from its associated scene transform before the extracted interval. Its actual numerical core is `0x167CD4..0x1680F0`, with three original helper blocks:

| Original interval | Role | Generated source anchor |
|---|---|---|
| `0x167CD4..0x1680F0` | Forces to acceleration; locks; four magnitude limits; velocity, position, angular velocity and quaternion integration | `work/native/project/generated/FUN_00167c40_0x167c40.cpp:705` |
| `0x14EE40..0x14EEB4` | Persistent two-step integrator | `entry_0014ee40_0x14ee40.cpp:23`, `entry_0014ee6c_0x14ee6c.cpp:24`, `entry_0014eeb0_0x14eeb0.cpp:23` |
| `0x14F7B0..0x14F86C` | Quaternion derivative from angular velocity | `FUN_0014f7b0_0x14f7b0.cpp:23` |
| `0x14F410..0x14F4EC` | Quaternion normalization and zero fallback | `FUN_0014f410_0x14f410.cpp:23` |

All abbreviated generated filenames above are under `work/native/project/generated/`. The original ELF maps these addresses to file offsets using `offset = address - 0x100000 + 0x80`. `tools/gameplay/recover48c.py` extracts exact words, checks all annotated instructions in fifteen bounded original source files against the ELF, and retains source-line-numbered excerpts in `evidence/gameplay48C/`.

The integrator is the two-step Adams–Bashforth formula, executed with separate binary32 operations:

```
halfFactor = float32(0.5 * factor)
state[i] += float32(halfFactor * float32(float32(3 * derivative[i]) - previousDerivative[i]))
previousDerivative[i] = derivative[i]
```

It is used for linear velocity, then position using the updated and limited velocity; angular velocity, then quaternion using the updated and limited angular velocity. Substituting Euler integration or discarding previous derivatives changes behavior.

## Typed state and parameters

The boundary deliberately begins after scene position/quaternion reconstruction. The native caller must supply the same values at that boundary; blindly feeding back the previous core output may differ from the complete original body update's transform reconstruction and corrections.

| Body field | Native role and evidence |
|---|---|
| `+0x60.xyz` | Position being integrated; reconstructed from associated scene object and local point before the core |
| `+0x70.xyz` | Linear velocity; integrated from force-derived acceleration and used for the position derivative |
| `+0xB0.xyz` | Angular velocity; integrated from inverse-inertia-transformed torque and consumed by quaternion derivative |
| `+0xC0.xyz`, `+0xD0.xyz` | Current linear/angular acceleration |
| `+0x140/+0x150/+0x160.xyz` | Three inverse-inertia columns applied to torque |
| `+0x180.xyz`, `+0x190.xyz` | Force/torque accumulations consumed by this core |
| `+0x1F0..+0x1FC` | Scalar-first quaternion `[w,x,y,z]` |
| `+0x26C`, `+0x270` | Mass positivity gate and force multiplier/inverse mass; their numerical use is explicit |
| `+0x274/+0x278/+0x27C/+0x280` | Squared limits for linear acceleration, angular acceleration, linear velocity, angular velocity |
| `+0x2A0/+0x2A4/+0x2A8` | Translation-axis enables |
| `+0x2AC/+0x2B0/+0x2B4` | Rotation-axis enables |
| `+0x2B8.xyz`, `+0x2C4.xyz` | Current and previous position derivative |
| `+0x2D0.xyz`, `+0x2DC.xyz` | Current and previous linear acceleration |
| `+0x2E8..+0x2F4`, `+0x2F8..+0x304` | Current and previous quaternion derivative |
| `+0x308.xyz`, `+0x314.xyz` | Current and previous angular acceleration |

The API omits SIMD padding/W lanes and scratch copies whose values are not consumed by the extracted three-dimensional calculations. Current acceleration/quaternion-derivative inputs are ignored and overwritten; persistent history inputs are required. The caller supplies already accumulated forces, torque and the inertia columns in the same coordinate convention. This change does not guess how collision or engine models produce them.

Several easily lost behaviors are intentionally preserved:

- A disabled translation axis clears velocity, acceleration and previous acceleration, but leaves the previous position derivative intact. It can therefore produce one residual position movement through history.
- Nonpositive mass sets linear acceleration to zero and skips the translation lock block. It retains previous acceleration for the integration step; it does not mean a frozen body.
- The angular path still runs when mass is nonpositive. Its locks clear angular velocity, current angular acceleration and previous angular acceleration.
- Limits compare squared magnitudes and apply `sqrt(limitSquared/magnitudeSquared)` when exceeded. Magnitude accumulation and matrix-column operation order follow the original instruction sequence.
- Normalization of a zero quaternion returns the four-zero constant at `0x382B60`. Both original ELF and saved running RAM contain zeros there. Replacing this with identity would change the extracted behavior.
- Scalar square root and reciprocal square root retain the existing reference's special handling. Positive reciprocal square root performs a double-precision square root and division followed by one float conversion; using a rounded float square root first is different. The existing reference header also flushes subnormal operands/results at specified SQRT/RSQRT boundaries.

## Numerical acceptance and limits

Parent integration compiled and ran this core and the original-word oracle for both Windows x64 and x86 with `/fp:strict`. Each passed **299,019 checks** in `evidence/nativePc48C-build-20261003-130411-296/{x64,x86}-tests.log`. The earlier x64-only build also passed. Later parent checkpoint builds can supersede these binaries; their logs/hashes remain separate evidence.

The test executes the exact original body interval, including its branches, delay slots and calls to all three original helper blocks. It uses bounded fixture memory and a strict instruction budget. Its output is compared bit-for-bit across all 36 native state floats, including persistent histories. Coverage includes:

- 6,000 randomized core inputs across four host rounding modes, including nonpositive mass, locked axes, full inverse-inertia matrices and low limits that take the clamp branches;
- 2,048 successive integration steps, with zero-factor steps interspersed;
- the actual stopped body's values under four rounding modes, including saved subnormal angular values, as synthetic continuation inputs;
- zero/signed-zero, subnormal and tiny quaternions; direct assertions distinguishing Adams–Bashforth from Euler, translation-history drift and nonpositive-mass behavior;
- invalid factor/limit and intermediate-overflow rejection with unchanged output, and input/output aliasing.

This establishes extracted control-flow and operation-order equivalence in the tested reference arithmetic domain. It is not a physical PS2 floating-point certification: scalar and VU ordinary arithmetic in the test use host binary32 operations; SQRT/RSQRT use the preserved reference `ee_sqrt14.h`. Flags, physical EE extended-float behavior, every denormal/overflow mode, exception traps and different FTZ/DAZ configurations are not certified. The API rejects nonfinite consumed inputs, negative squared limits/factor and nonfinite intermediates without mutating output; these are native API domain guards, not newly claimed original-game validation rules. The caller owns the floating-point environment.

No real before/after vehicle tick was captured. Feeding the stopped body into the instruction oracle is a controlled synthetic experiment, not evidence that its next real racing tick was reproduced. These tests measure correctness of this component and do not establish gameplay FPS.

## Remaining original call graph

The following are identified dependencies of `RunSimulation`, not completed native implementations. The evidence stores the full bounded original `0x219810` instruction listing; branches mean not every call happens on every substep.

| Component / call | Proven route | Status |
|---|---|---|
| Spring detection | Vehicle `+0x358`, vtable `+0x3C` -> `0x295630`, called `0x219BDC` or `0x219C50`; optional `+0x36C` called `0x219C08` | Collision candidates/contact geometry remain unrecovered |
| Contact handling | `0x219420` called `0x219C14/0x219C6C` | Not implemented |
| Component `+0x348` | `0x1B1800` before force models, `0x1B1A30` after collision model | Meaning/contract not yet closed |
| PodEngineModel `+0x34C` | Direct `0x2E7720` at `0x219C84`; conditional `0x2E7C10` at `0x219CC0` | Engine/load generation not implemented |
| SelectiveGravityModel `+0x344` | Direct `0x2827D0` at `0x219C90` | Source multiplies each target body's mass by negative gravity and adds to force Y; not yet native |
| PodAerodynamicsModel `+0x350` | Actual slot `+0x78` -> `0x16CD90`, call `0x219CD4` | Aerodynamic/steering load generation not implemented; slot `+0x3C` is not this route |
| SpringCollisionModel `+0x354` | Slot `+0x3C` -> `0x2964C0`, call `0x219CE8` | Contact response/load generation not implemented |
| Optional second PhysicsRigidBody `+0x35C` | Same body type, plus vehicle helper `0x2188B0` | Multi-body coupling not implemented |
| Vehicle helpers | `0x223030`, `0x219120`, plus matrix/vector helpers after body update | Contracts not yet closed |
| Main PhysicsRigidBody `+0x340` | Slot `+0x3C` -> `0x167C40` at `0x219E80`, then slot `+0x94` at `0x219E94` | Only numerical core implemented |

The body still needs its full outer contract: enabled/associated-object gates (`+0x204/+0x288`), scene point/matrix reconstruction, orientation matrix writeback, constraints/corrections at `+0x1A0/+0x1B0`, transformed velocities, accumulator clearing, and final virtual callback. The core intentionally does not clear caller-owned force/torque accumulations early. Camera, race state, AI, event/weapon interactions and asset-driven collision topology also remain outside this work.

## Timestep and action mapping: stronger evidence, remaining gaps

The existing 48B substep contract remains authoritative: scheduler `0x1A7B40` captures entry `f12`, then near `0x1A8390` computes a binary32 product with 60, applies the recovered truncate/saturate `CVT.W.S`, adds one, and divides the original factor by that count. The first substep flag is one and subsequent flags are zero. A binary32 `1/60` factor produces two substeps, not one. Presentation at 60 Hz must not become a naive one-body-call-per-frame policy.

Static ELF search found the scheduler pointer at `0x3CEEB8` and no direct JAL to `0x1A7B40`; the upstream virtual caller/clock remains unclosed. Gravity's 32.2 scaling, velocity/position integration and the 60 multiplier are consistent with a seconds-based factor, but they do not prove the actual run's clock scaling, pause behavior, catch-up policy or supplied factor sequence. The native API continues to name it `elapsedFactor` instead of asserting measured seconds.

Input construction at `FUN_001a0570_0x1a0570.cpp:266` and following calls creates four axis components through `0x127BB0` with source IDs **0, 1, 2, 5**, min/max `-1/+1`, center zero and 0.2 initial thresholds. `0x143700` subsequently attaches them to the selected input device with two 0.01 parameters. `0x127BB0` stores the source ID at component `+8`; this matches the stopped snapshot. Player `0x1A1A20` copies their `+0x24` values into `B5C/B60/B64/B68` before shaping/quantization and the already native 48B rate limiter.

The first filtered channel `B6C` is multiplied by speed-dependent scaling and used to adjust engine `+0x68/+0x6C` and aerodynamic `+0xE4/+0xE8` fields (`0x218574` onward). `B70` is also written to aerodynamic `+0x50` at `0x218868`. These are concrete consumers, not a complete physical-control mapping.

The configured discrete source IDs in the saved player settings are **2 -> B7C** and **3 -> B80**, selected through settings offsets `0x694/0x690 + 40*controllerIndex`. Button queries dispatch to `0x143600`, which examines the input device's `0x324 + 24*sourceId` state record. A nonzero B7C sets both engine channels to 1 before differential adjustments; B80 selects strong aerodynamic adjustments and can zero both engine channels. Acceleration/brake interpretations are plausible, but the complete physical button backend and all mode-dependent action semantics have not been proved. No Xbox control labels were invented.

## Next semantic trace gate

1. Instrument the existing reference at actual guest instruction boundaries, not C++ generated-function exits. Capture vehicle `0x219810` entry (`a0`, `f12` bits, first-substep flag, true return address and stack), raw/configured input IDs and consumed axis values, selected component pointers/types, contact identifiers and accumulated loads.
2. Capture body `0x167CD4` after scene-derived position/quaternion reconstruction. Record the typed fields above, factor bits, integrator pointer and component identity. Accept only the observed `0x14EE40` route for this adapter; other callbacks require their own recovery.
3. Capture `0x1680F4` after the original normalization copy and quaternion-to-matrix helper return, before outer transform writeback. Correlate body pointer, saved stack and call identity. A yield is not a guest return. For a whole-vehicle trace, await the original `0x219FF8` JR/its delay slot and matching saved caller, across any yields.
4. Run the native core from each captured entry and compare the 36 semantic outputs. Discrete IDs, enables and history ownership are exact; for the present reference arithmetic, demand bitwise core equality. If a physical-console or alternative backend requires tolerances, determine units/scales from observed data, set tolerances before comparing, and preserve maxima and first divergences.
5. Recover force producers and scene writeback in dependency order, then compare one complete original vehicle substep, followed by consecutive substeps including contact and changing input. A native mesh display is a separate asset/render milestone; this gate is needed before calling the result native gameplay.

## Provenance

- Original `work/native/project/disc/SLUS_202.68`: SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
- Stopped `work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin`: SHA-256 `38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48`.
- Extracted 0x320-byte stopped player body: SHA-256 `3c30718c93f26512a8d81760a664fefa2f16b77648e338779ec927f498c2d7c5`.
- Test-only original-word/snapshot header: SHA-256 `9f3b9fed356d9e7a84c7610bb9dab2aa931b8a3e35f35f029532d3a3b898a69a`.

`evidence/gameplay48C/recovery-manifest.json` records original block/file offsets, every source hash, fixture hash and square-root-reference hash. `checkpoint-manifest.json` records all 48C-owned delivery files and local verification logs. The parent checkpoint owns complete recoverable archives and executable hashes. 48B source/evidence are preserved.
