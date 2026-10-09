# Native implementation checkpoint 48C

2026-10-03. Goal remains a complete, stable native PC game with correct gameplay
and a measured sustained-60-FPS/720p target on the available PC. This checkpoint
adds a built-in importer, expanded original scene recovery and a substantial
native rigid-body numerical core. It does not satisfy full gameplay, full
material fidelity, racing FPS or original-Xbox execution acceptance.

## Built-in converter

Launching `racer_native_pc.exe` without arguments opens the native Windows
importer. Choose a cooked 2048-byte original ISO or extracted game directory
and a separate cache directory. The current supported game revision is checked
against the original SLUS_202.68 SHA256. Raw BIN images and other revisions are
explicitly rejected. Original data is opened for reading, with read locks held
through commit to prevent mutation or replacement during conversion.

Conversion runs on a worker thread with progress and cancellation. Versioned,
content-addressed mesh objects and metadata are written to staging, flushed,
reopened and checked, then atomically promoted. A verified generation manifest
is made active by replacing `current.json` only after final validation.
Cancellation or malformed input leaves the preceding active generation intact.
Completed objects are reusable after cancellation. Reuse checks hashes,
metadata integrity, structure, counts and original material/world-space
fields. Converter versions invalidate old entries. Cache imports hold an
exclusive lock; unrelated folders and overlap with original data are rejected.

The production importer is C++ with portable vendored zlib and JSON support,
plus Windows file/hash/UI services. It does not launch Python, CarsTools or
the historical PS2 runtime. Dependency licenses and exact copy provenance are
preserved under `third_party/native-zlib`, `third_party/native-json` and
`evidence/native48C-parent/dependency-provenance.json`.

Its accepted geometry grammar remains the frozen 48B static PSG3 subset.
Textures, animation and other resource types are listed as unsupported. This
is visibly described as a **partial geometry cache**, not a ready-to-play game.
The richer offline 48C decoder below has not yet been ported into this importer.
Adding that coverage is the next conversion task, not a second user workflow.

The full extracted game scan read 103 RES containers and recorded hashes for
34,128 members. It converted 2,083 distinct original meshes, with 101 reused
duplicates: 2,184 supported member entries total. It reports 31,944 unsupported
entries. A separate Python execution of the frozen decoder rederived every
imported mesh from original resource bytes and confirmed exact native bytes,
material names and world-space flags. A subsequent final-build scan reused all
2,184 accepted entries and retained the same generation.

The frontend automated check populated its real controls, activated Convert /
Resume, pumped actual progress/completion messages and captured the resulting
window. The parent inspected the capture and fixed a window-title encoding
defect. Native picker dialogs compile and are connected; interactive operation
of the operating system's picker dialogs was not separately automated.

## Expanded original scene recovery

`tools/assets/decode_scene48c.py` decodes 228 of 230 TA meshes and all 13 POD01
meshes, preserving materials, LODs, hierarchy matrices and rigid bone indices.
The two remaining TA cases have unsupported animated V4_32 position data.

All 47 former strip-count discrepancies are accounted for by exporter splits:
166 continuations repeat the preceding strip's last two vertices across every
stored attribute. The decoder still verifies the source strip count and emits
the original topology. The 140 previously supported TA exports remain exactly
byte-identical. Original-loader evidence independently confirms hierarchy
records and the local-times-parent matrix order; 751 instruction comments were
verified against the original ELF.

The neutral ground aggregate grows to 26 world-space meshes, 9,981 vertices
and 6,035 triangles. It excludes unplaced local props and members with opposing
normal diagnostics. Pod01 uses the original PHY-selected chariot/left-engine/
right-engine meshes and their hierarchy bind transforms: 5,755 vertices and
3,571 triangles. Runtime animation, racing pose and whole-track placement are
not accepted. Opposing-normal faces are preserved and reported, not flipped.
See `docs/scene-recovery48c.md` for the full bounds/material/LOD evidence.

The parent inspected the native pod readback: two engine bodies and a separate
cockpit are visible in the neutral material. Both ground and pod captures are
pixel-identical across x64 and x86. Two-sided opaque rendering still omits
original shader/color/lighting/fog behavior. Texture palette/swizzle and sky
remain unresolved; the existing compatibility renderer has known visual faults.

## Recovered native rigid-body code

`src/game/body_integration48c.cpp` implements the original numerical core over
typed native state: forces/inverse mass, inertia/torque transformation, axis
locks, magnitude limits, persistent Adams-Bashforth two-step histories,
velocity/position/angular integration and quaternion derivative/normalization.
It preserves unusual original behavior, including the zero-quaternion fallback,
history behavior for nonpositive mass, operation ordering and observed
SQRT/RSQRT semantics. It has no guest-memory, CPU interpreter or device API.

The independent test-only interpreter executes four exact original instruction
blocks. 299,019 checks pass on each architecture, including four host rounding
modes, trajectories, original stopped-body values, subnormal/zero/axis-lock/
history cases. This is finite-domain agreement with the recorded reference
arithmetic contract, not proof of every PS2 floating-point mode. The existing
four-channel control filter also continues to pass 130,349 checks per build.

The numerical core is not yet connected to live driving. Full engine,
repulsor/aerodynamics/steering force production, collision, scene writeback,
upstream timestep domain and physical controller action mapping remain to be
recovered and integrated. Exact component and input IDs are preserved in
`docs/simulation-recovery48c.md` and `evidence/gameplay48C/`.

## Accepted verification

Final build: `evidence/nativePc48C-build-20261003-131135-609`.
MSVC 14.44.35207, SDK 10.0.26100.0, Release/static CRT; strict floating-point
semantics on recovered game and import decoder code. All four native test
targets pass on x64 and x86: asset loader (414), control filter (130,349), import
decoder (3,250), and rigid-body integration (299,019). The expanded offline
scene decoder passes 6,130 checks. Counts describe those specific checks,
not complete game correctness.

Fourteen end-to-end native importer invocations pass per architecture. They
cover ISO/directory equivalence, interruption/resume, reuse, corrupt-object and
metadata repair, version invalidation, malformed source preservation, unrelated
cache rejection, original/cache overlap, source writer locks and concurrent
cache import exclusion. The frozen 140 TA mesh outputs match independently.

Final executable SHA256:

- x64: `e6ce03706db073f2c8f11a73a0addbd2b65f5b56fe9cdda07ef4448dfcc283fb`.
- x86: `94b33dd454783230f99802f8f6d8d36c55d4d2906055109743de74a10bea81e8`.

Final captures under `evidence/`:

- Ground: `nativePc48C-run-x64-20261003-131215-547`, `nativePc48C-run-x86-20261003-131216-105`.
- Pod: `nativePc48C-run-x64-20261003-131216-700`, `nativePc48C-run-x86-20261003-131217-186`.
- Importer window: `nativeUi48C-20261003/final-cache/import-ui-check.bmp` (lossless PNG alongside).

Ground PPM SHA256 is `2c222a635457e4f6581a676bcc69db22acf0a2865dc300bc0af42afcd0df65cc`;
pod PPM is `2aa6106547944182c7acfed031db2946e194f4d04e2dfc41962a842ea8371d58`.
Hardware Radeon readbacks, source/binary/input/output hashes, source allowlist,
link/import and loaded-module audits are recorded in
`evidence/native48C-parent/`. These are bounded offscreen inspections, not
presented racing FPS. Win32 does not establish Pentium III ISA compatibility,
Xbox residency or an Xbox frame rate.

Earlier failures remain saved: the native normal-mutation test initially
assumed one 53-vertex packet and was corrected to mutate only actual normal
payloads; a real supported-subset strip discrepancy was separated from malformed
vertex counts; review found and fixed consumed-source mutation and cached
semantic-metadata acceptance gaps. None was hidden by relaxing all validation.

## Next work

Integrate the expanded scene grammar into the native converter; carry original
materials/normals/LOD/instance data into the runtime and resolve texture/sky
semantics. Recover and connect force-producing components and collision to the
native body step, with original live timestep/input evidence. Then assemble
continuous native driving and the complete race/progression systems, profiling
meaningful CPU/GPU/simulation workloads. Intermediate checkpoints remain
progress toward the full goal, never substitutes for stable performant gameplay.
