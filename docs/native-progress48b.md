# Native implementation checkpoint 48B

2026-10-03. The independent Windows native target now renders a bounded subset
of original geometry. One recovered control function executes over ordinary
native data. This is an asset inspection and recovery milestone, not playable
gameplay, a complete material implementation, or a measured racing FPS gain.

## Native path

`RR_NATIVE_PC_ONLY=ON` selects an explicit source list through
`cmake/NativePc48B.cmake`, bypassing the historical compatibility targets.
The viewer loads versioned meshes/textures, creates native D3D11 buffers, draws
to a 960x720 target and saves a completed GPU readback. The final isolated launch
directories contain only the viewer, converted data and resulting evidence.
Source selection, linker maps, imports and loaded module names passed the
independence audit. That audit is not an operating-system trace of every file
opened. No original executable, guest memory or device execution is part of
this viewer's implementation.

Both Windows architectures built using MSVC 14.44.35207, Windows SDK
10.0.26100.0, Release configuration and the static CRT. D3D11 ran on the hardware
AMD Radeon adapter; no WARP fallback occurred in the seven final captures.
The viewer's Windows/D3D11 backend is not an Xbox backend. Win32 checks do not
prove Pentium III instruction compatibility, Xbox memory fit or Xbox execution.

Final build evidence: `evidence/nativePc48B-build-20261003-123513-121`.
This includes source hashes before/after the build, compile commands, maps,
PE inspection, logs, test executables and executable hashes. Earlier build
failures and superseded captures remain preserved and are not the accepted
source/binary pairing.

| Final executable | SHA256 |
|---|---|
| x64 viewer | `6738c61829c370a237efef79b0aa5acb2bfed813cbbf0fb27fa7fad35c1a4e90` |
| x86 viewer | `d78c547c2e13cd14804eec446f069778c709421e200ed054655cf84b2656da4b` |

## Original assets

The bounded converter decodes the observed PSG version-3 static,
single-object, identity-transform, single-material `PreLitCompressed` subset.
It preserves quantized positions, UV/color bytes, strip boundaries and
triangle topology. Header counts, recovered bounds and stored normals provide
separate consistency checks. See `docs/asset-decoder48b.md` and
`docs/native-asset-contract48b.md` for exact restrictions.

The unchanged decoder inspected all 230 original TA meshes: 140 decoded and
90 were rejected as unsupported. The rejections are 47 strip-count
discrepancies, 23 multiple materials, 10 nonidentity transforms, eight winding
versus normal disagreements, and two multiple-object hierarchies. These are
format-recovery limits, not evidence that original files are corrupt.

Twenty decoded world-space ground meshes were merged at their original
positions: 2,736 vertices and 1,414 triangles. The 120 decoded local-space props
were excluded because their scene placement is not established. The merge
verified every original vertex byte and each member's relocated index span.
Its SHA256 is `5a70f429e2250975a7e5a586079575fad84a652520077898abf7e2fb2dff858c`.
This is a diagnostic ground aggregate, not a complete assembled track.

The original ta_sand8 PST version-1 palette/mips were bounded and extracted.
Linear and CSM1 palette interpretations are both retained as candidates.
Neither is accepted as original texture/material fidelity. The neutral viewer
uses two-sided, opaque rendering and omits original lighting, fog, source
vertex color/alpha and material programs. No sky interpretation is accepted.

## Recovered game function

`src/game/control_filter48b.cpp` implements the original four-channel rate
limiter over typed float arrays. The test-only oracle independently decodes
the exact 70 original ELF instructions at 0x2180AC through 0x2181C4; the native
function itself has no instruction interpreter, guest registers or memory API.
The helper is not connected to the viewer or to a driving simulation yet.

The implementation passed 130,349 checks on each architecture, covering random
finite inputs, trajectories, four host rounding modes, ties, signed zero,
aliasing and rejected invalid inputs. These comparisons use host IEEE
arithmetic and do not establish every PS2 floating-point/denormal behavior.
The offline recovery also preserves 23 original PHY files, the subdivision
formula and player/AI identity evidence. See `docs/simulation-contract48b.md`.

## Verification and captures

| Check | Result |
|---|---|
| Native asset loader | 414 checks passed on x64 and x86 |
| Original geometry decoder | 5,781 checks passed |
| Native control helper | 130,349 checks passed on x64 and x86 |
| Offline gameplay recovery | 12 tests and 4,396 subdivision comparisons passed |
| Viewer output aliases | Eight rejection/preservation cases passed on each architecture |
| Final native captures | Seven successful hardware readbacks; source/executable/input/output hashes verified |
| Cross-architecture pixels | Ground and merged-ground PPMs exactly identical between x64 and x86 |
| Agent evidence manifests | All 378 recorded members reopened and SHA256 verified |

Final capture directories under `evidence/`:

- Ground: `nativePc48B-run-x64-20261003-123548-705` and `nativePc48B-run-x86-20261003-123549-117`.
- Rock: `nativePc48B-run-x64-20261003-123549-534`.
- Ground palette candidates: `nativePc48B-run-x64-20261003-123549-864` and `nativePc48B-run-x64-20261003-123550-203`.
- Merged ground: `nativePc48B-run-x64-20261003-123550-542` and `nativePc48B-run-x86-20261003-123550-864`.

The parent inspected the merged-ground PNG: a nonempty connected-looking
ground silhouette is visible, with small separate portions. Original PPMs
remain authoritative. PNG previews are lossless encodings with identical
decoded RGB verified. Neutral ground PPM SHA256:
`7fe444df2e5cbf33618065fea57106e151cd2db3f0647997eebe673fa84369dd`.
Merged-ground PPM SHA256:
`99191f4767280c0aaad0ec40961c5935be56e5757658af681d1346e208573eb0`.

The audit and cross-architecture comparison records are in
`evidence/native48B-parent/`. The three offscreen submissions per run do not
measure presented FPS or GPU execution time. No native gameplay speedup or
Xbox FPS forecast is inferred from these captures.

## Built-in converter

The user explicitly requires a built-in converter. The implementation plan
now requires the PC application to accept an original ISO or extracted game
data, validate it and produce a versioned native cache. Include progress,
cancel/resume, hashes/provenance, incremental rebuilds, staged validation and
atomic promotion while keeping original inputs unchanged. Normal gameplay
loads converted data; later Xbox export shares the same semantic pipeline.
The current offline decoder is the foundation, not a finished import service
or application UI. It must retain honest unsupported-format reporting.

## Next implementation gates and blockers

1. Recover the rejected strip variants, hierarchy transforms and scene instance
   placement from the original loader and data. Assemble a genuine pod and
   track section without guessed transforms.
2. Resolve PST palette/swizzle, material state, UV/color behavior, sky and
   surface appearance against original execution and asset evidence. The
   existing compatibility renderer has known visual faults and is not an
   unconditional art oracle.
3. Establish live timestep/action mapping, native vehicle state and collision
   queries. Extend independently checked native functions into one genuine
   simulation step, then controls/camera. Do not substitute invented physics.
4. Wrap stable conversion behind the application import service as format
   coverage grows. Complete the built-in workflow before playable distribution.
5. Measure native CPU/GPU/simulation timing only with a declared meaningful
   workload. Continue single-thread correctness and x86 portability checks;
   defer Xbox-specific packing/backend work until the PC slice is validated.

The overall port remains unfinished. Checkpoint 48B preserves this accepted
partial milestone, not a claim that the gameplay or performance goal is met.
