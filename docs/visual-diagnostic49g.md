# 49G: authored TA and Pod01 diagnostic frame

## Result and scope

This milestone connects authentic authored geometry to the existing recovered
camera and actual pixels in Linux. The renderer is a small external diagnostic
consumer using installed Mesa/EGL/OpenGL through Python ctypes. It is not the
Windows viewer or an integrated game loop. The inspected images are retained in
`evidence/visual49G/frames/ta-pod01-neutral.png` and
`ta-pod01-wireframe-wire.png`. Their exact approved recipes reproduce byte-for-byte
in this environment. The wire overlay is useful for seeing TA topology; the solid
frame deliberately uses only flat gray world geometry and orange Pod geometry.

The source tree starts from verified 49F archive SHA256
`8c9a74ba2c9e5acd53f1e1e0b313a9e3a36c93981ed964e6b1ccf05e10dbf47e`.
No old source is imported with the assets. All inherited production and CMake
files remain byte-identical; only the inherited README changes. The separate 49C
work remains excluded and unreleased. Existing collision/action-reset limitations
remain unresolved and were not investigated here.

## Authored assets and transform rules

The authenticated reference is
`RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Evidence.zip`,
256,356,241 bytes, SHA256
`60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c`.
The active cache generation is
`3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39`.
Only the TA RRWORLD and its exact selected scene/material/texture dependencies,
plus Pod01 Chariot/LeftEngine/RightEngine bind components, are copied unchanged.
The full archive and full generation manifest stay outside this source tree.

TA has 318 authored instances: 284 have an exact supported scene and are drawn;
34 missing exact PSG references remain explicitly non-drawn. There are 228 unique
TA scenes plus three Pod parts. No substitute mesh is generated. The exporter uses
the existing native scene parser and supported LOD0 single-root/bone-zero or
worldspace-identity behavior. An authored RRWORLD transform replaces its scene root
exactly once. There are 42 nonidentity saved roots; applying both would be wrong.
Three binary64 products plus translation are evaluated in source order before a
single binary32 position conversion. TA output matches the retained48F mesh hash:
`4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7`.

The three Pod components use their original bind assembly, then a supplied
identity-rotation translation of
`[2903.56103515625, 1292.14404296875, 7649.8837890625]`.
Each translated position is rounded binary32; UV/color/index bytes stay unchanged.
This diagnostic placement is not a race grid or a physics-derived vehicle pose.

Full paths, member hashes, ordered placement records and texture-resolution
reasons are in `evidence/visual49G/asset-selection.json`, `provenance.json` and
`ASSET-SELECTION.md`. All 859 retained native files match the reference archive.

## Camera and graphics adapter

The real TA `taTVCam.pos` fixture is 5,527 bytes, SHA256
`e384bcf4309808e7b752b20546ae8beceb17f0fe6c3b59dd320bea1a7beb5ac8`.
Authored camera 1 is at `[2888.56103515625, 1302.14404296875, 7589.8837890625]`.
The supplied stationary Pod position is offset `+15, -10, +60`; target Y offset is 0.
The existing 49E selector selects camera 1. Its local input and the resolved/global
view input are explicitly equal only for this identity-parent fixture. General
local/resolved coordinate contracts remain distinct. A zero-speed scene sample
cannot trigger nearby-vehicle shake. Camera-type scheduling and intervening game
RNG draws are not simulated; this fixture expressly requests the TV subsystem.

The unchanged 49F numeric path produces FOV 40, aspect field 0.75, near 1, far 100000,
roll 0, and the exact float32 core view/projection/combined matrices. Its ordinary
caller flags are both 0. The fixture records all matrix bit patterns and RNG values.
No renderer-side autofit, pose transformation or perspective substitution occurs.

Native row-major row-vector matrix bytes uploaded with `GL_FALSE` are interpreted
as the transposed column-vector matrix by GLSL. This gives the same transform;
the uniform bits round-trip exactly. `GL_LOWER_LEFT / GL_ZERO_TO_ONE` preserves
the native clip-depth domain. PNG readback only reverses row storage. Nonsymmetric
matrix tests detect accidental transpose and independently check the separately
rounded native view-times-projection product.

The graphics profile has depth LESS, RGBA8/depth24, one sample, no face culling,
no blending, no textures, no lights and no visibility/portal policy. All supplied
triangles are submitted. The wire companion overlays edges as a separate debug
pass. This does not prove original shading, PS2 viewport behavior or native
renderer parity.

## Counts and measured host resources

| Resource | TA | Three Pod parts | Total |
|---|---:|---:|---:|
| Vertices | 110,196 | 5,755 | 115,951 |
| Indices | 209,514 | 10,713 | 220,227 |
| Triangles | 69,838 | 3,571 | 73,409 |
| Active LOD0 surfaces | 353 | 7 | 360 |

The surfaces are flattened into two neutral draw calls. Solid mode submits 73,409
triangles; solid plus wire uses four draws and 146,818 triangle submissions.
These are submission counts, not visibility or on-screen-triangle measurements.

The minimum source closure is 13,120,368 bytes. The saved 21,989,939-byte closure also
includes 3,487,932 bytes of companion parity meshes,137,009 bytes of materials and
5,244,630 bytes of resolved texture records/mips. All 35 material bindings resolve
to 34 records; 26 of35 texture bindings resolve. Nine intentionally unresolved names
are Pod01 `specmap5` and TA `arrowred`, `cord`, `crawlera`, `crowd`, `track`,
`trackdark`, `trackside`, `trackwheel`. Unsupported local resources shadow global
fallbacks. No texture is bound in 49G, including the 26 available ones.

The two derived files total 3,663,764 bytes; uploaded vertex/index buffers total
3,663,732 bytes. At 640x480, nominal RGBA8+depth storage is 2,457,600 bytes and one
RGBA readback is 1,228,800 bytes. These exclude driver, Python and staging overhead.
Measured peak process RSS was 118,018,048 bytes for solid and 122,855,424 for the
wire recipe. Both are external host diagnostic measurements, not Xbox budgets.

Mesa 25.0.7-2+deb13u1 / llvmpipe LLVM 19.1.7 used one CPU affinity and
`LP_NUM_THREADS=1`; BLAS/OpenMP workers were also constrained. Rendering is bounded
by a 2 GiB address-space ceiling, 60 CPU seconds and 90 wall seconds. The observed
validation-plus-render times were 0.183s solid and 0.230s wire. They include setup and
PNG work and do not establish gameplay FPS, realtime performance or hardware-GPU
memory/throughput. No new software was installed. New diagnostic translation units compile with
`-Werror`; inherited parser warnings remain recorded and unchanged.

## Verification

- Asset selection: 116,673 assertions, all 859 original file comparisons,
  seven resolver cases and seven invalid-input cases; deterministic fresh selection.
- Export: normal and ASan/UBSan output identical; all 231 embedded/companion meshes
  equal, full TA hash equal to historical 48F, every translated Pod vertex checked.
- Camera: fresh normal and ASan/UBSan JSON identical to the retained fixture.
- Renderer: eight non-drawing CPU checks, bounded EGL/shader probe, queried clip
  control, uniform bit equality, framebuffer completeness and clean GL checks.
- Independent read-only review reconstructs all valid authored geometry, verifies
  source equality to 49F, and reproduces both approved recipes byte-for-byte.
- Relevant unchanged camera/input regression results are retained in
  `evidence/visual49G/regression-summary.json`.

ASan/UBSan passed with LeakSanitizer explicitly disabled because this environment
uses ptrace. Leak checking is unavailable, not passed. The new diagnostic exporter
and Python renderer allocate offline/staging memory; they are not zero-allocation
production hot-loop integrations. The unchanged 49F camera remains allocation-free.

## Reproduce with installed tools

Run from the repository root and choose new output directories. No installation
or CMake is required. CMake is absent in this executor; its inherited file is not
changed. The new builder compiles only the exporter and camera diagnostic:

```
python3 -B tools/build_visual49g.py --output-dir /tmp/rr49g-build-normal
python3 -B tools/build_visual49g.py --sanitize --output-dir /tmp/rr49g-build-san
/tmp/rr49g-build-normal/export_authored49g --selection evidence/visual49G/asset-selection.json --output-dir /tmp/rr49g-export-normal --pod-translation 2903.56103515625 1292.14404296875 7649.8837890625
/tmp/rr49g-build-normal/make_camera49g evidence/tvCamera49E/authored/taTVCam.pos 0 2903.56103515625 1292.14404296875 7649.8837890625 0 /tmp/rr49g-camera-normal.json
```

For sanitizer runs use the second build and
`ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1`.
The source fixture and supplied values are pinned by this report; accepting 19
cameras in the small camera CLI is not a general cryptographic authentication API.

The selected bytes and derived meshes are included, so screenshot reproduction
needs no full 48F archive or manifest. To compare against the original archive,
follow `evidence/visual49G/ASSET-SELECTION.md` and `independent-reproduction.txt`.
Plan fingerprints include absolute paths/CPU and therefore may differ after a
checkout move. Always produce and inspect the current plan before using its hash:

```
python3 -B tools/visual/render_neutral49g.py --recipe evidence/visual49G/frames/recipe-solid.json --plan --plan-output /tmp/rr49g-solid-plan.json
python3 -B tools/visual/render_neutral49g.py --recipe evidence/visual49G/frames/recipe-solid.json --render --draw-ack CURRENT_PLAN_SHA --output-prefix /tmp/rr49g-solid
```

Repeat with `recipe-wire.json` for the wire companion. Use a fresh output prefix.
Driver versions may affect raster edge pixels; exact PNG reproduction is claimed
only for the recorded environment and recipes. Original materials, missing scene
references, animation, visibility, camera scheduling, vehicle/world interaction,
platform renderer integration and target FPS remain separate open work.
