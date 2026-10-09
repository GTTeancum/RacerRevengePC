# 49H: authored first-pass texture diagnostic

## Evidence boundary

This is the same TA/Pod01 diagnostic view as 49G, now sampling the actual cached
first-pass/slot-one texture associations with the original exported UVs. It is a
source-grounded inspection profile, not an implementation of original material
microprograms. The unchanged scene surfaces identify material names; preserved
material records identify first pass and slot 1; the verified cache selection
resolves container-local resources. No arbitrary texture matching or cross-track
substitution is permitted.

The existing Windows diagnostic proves the chosen profile:
`platform/windows/native/main48b.cpp:405–419` passes UV/color unchanged, samples
texture RGB times normalized vertex RGB, and emits alpha 1. Lines 650–661 bind
first pass/slot 1; lines 633–647 select the largest cached mip at level 0;
lines 675–679 specify linear repeat sampling with maximum LOD 0. The native viewer
uploads all cached mips, but its sampler cannot select smaller ones; 49H uploads
only the largest mip. This is a diagnostic equivalence, not a claim about original
PS2 texture filtering or material behavior.

The inherited 49G selection rules remain unchanged: local candidates shadow
all global candidates, even when the local format is unsupported. Only an absent
local can use the existing unambiguous global rule. All used associations here
are local. The full authored material fields and additional passes/maps remain
in the cache and assessment report, explicitly unexecuted.

## Coverage and ordering

| Component | Textured spans | Fallback spans | Textured triangles | Fallback triangles |
|---|---:|---:|---:|---:|
| TA | 279 | 74 | 64,396 | 5,442 |
| Three Pod01 parts | 7 | 0 | 3,571 | 0 |
| Total | 286 | 74 | 67,967 | 5,442 |

All 360 spans preserve the existing geometry order, partition all 220,227 indices
exactly once, and use the same two mesh sets (four vertex/index buffers). There is no regrouping or mesh
rebuild. All 115,951 vertices and 73,409 triangles are retained. The 34 original
TA placements without exact PSG geometry remain non-drawn as in 49G.

Eight unresolved TA texture names cause the explicit fallback: `arrowred`, `cord`,
`crawlera`, `crowd`, `track`, `trackdark`, `trackside`, `trackwheel`. The additional
Pod map `specmap5` is unresolved but lies outside the selected first-pass/slot-one
profile. The three Pod parts therefore have complete coverage within this profile.

Fallback deliberately preserves the 49G palette: TA `[0.57,0.63,0.68,1]`, Pod
`[0.97,0.58,0.15,1]`. These are declared diagnostic colors, not authored material
colors; they differ from the older Windows viewer's neutral fallback. A texture-
disabled 360-span control must reproduce the 49G two-draw neutral image pixels.

## UV, color and sampling conventions

Native UV values are passed unchanged. No vertical flip, UV normalization,
coordinate generation or texture matrix is invented. RGBA bytes are uploaded in
their stored row order, with the first row at T=0. Repeat sampling matters:
36,273 TA vertices and four Pod vertices have UVs outside the unit interval.
TA values range approximately from -10.39 to 11.66.

Texture RGB is multiplied by the exported vertex RGB as UNORM8. TA's channels
are at most 127, so this profile can look darker than a PS2 renderer with different
color semantics. No brightness-times-two correction is introduced. Pod vertex
colors are 255. Input vertex/texture alpha is ignored and output alpha is 1.
No sRGB decode, authored alpha blend/test, specular map, animated texture or
material shader program runs. In particular flame/window appearances may differ
because this inspection profile is opaque.

The 49G camera JSON, world mesh and placed Pod mesh remain byte-identical.
Camera1 is source-authored; the supplied stationary Pod position remains
`[2903.56103515625,1292.14404296875,7649.8837890625]`, identity rotation. It is not
an original race grid or a physics-derived state. The same recovered view,
projection, combined matrices, FOV 40 and 640x480 viewport are retained.

## Bounded host cost

The 26 actively referenced largest mip images occupy 3,940,352 RGBA bytes:
13 at 256x256, seven at 128x128, four at 64x64 and two at 32x32. Their file headers
add 416 bytes. Two shared mesh sets (four vertex/index buffers) occupy 3,663,732 bytes, so texture plus
geometry upload payload totals 7,604,084 bytes. This excludes alignment, driver
allocations, framebuffer/depth, readback, metadata and runtime/staging overhead.

The frame uses 360 ordered draw calls. Optional adjacent texture merging could
reduce this to 153, but is not implemented in this milestone. There is no wire
pass. All 26 textures are uploaded once; no copies per surface are created.
The separate 49H manifest is bounded at 2 MiB, 512 spans, two meshes and 26 textures.
The inherited 49G renderer and its limits are not edited. The diagnostic remains
one-CPU Mesa llvmpipe at 640x480 with 2 GiB address-space, 60 CPU-second and 90 wall-
second ceilings. These are host tooling limits, not original-Xbox budgets.

## Preservation and verification

This tree starts from verified 49G ZIP SHA256
`23e772638cb870677a46991d9a8085ca648dd34089980e79df7c69206f235f2c`.
All 859 selected asset files, 231 scene objects and their companion meshes are
unchanged. The independent assessment checks every active surface's UV/color
bytes against its semantic scene. The original unlit screenshots remain intact.
Only README is intentionally changed among inherited files.

New implementation, exact command reports, coverage and final review are retained
under `tools/visual/` and `evidence/visual49H/`. The final checkpoint records actual
rendering, normal/sanitizer results, pixel controls and measured host memory.
The separate 49C branch is excluded and remains unreleased. Collision/action-reset
contracts remain unresolved. No PC access, RAM reconstruction, installs, old-source
overlay, pushes, full gameplay or target-FPS validation are part of this work.


## Final observed frame and tests

The inspected final image is
`evidence/visual49H/frames/ta-pod01-textured-final.png`: recognizable blue/white
Pod01 and original engine textures over textured TA ground/buildings. The final
texture-disabled image matches all 307,200 RGBA pixels of 49G's original neutral
frame. Final and initial textured pixels also match exactly despite the final
manifest adding explicit input-limit provenance. Initial artifacts are preserved.

Measured final host peak RSS is 131,870,720 bytes textured and 125,906,944 bytes
for the neutral control. Validation plus rendering took 0.422s and 0.206s respectively.
These single diagnostic runs include staging/setup/readback/PNG work, use software
Mesa, and are not gameplay-FPS or target-memory results.

Seven positive CPU controls and 12 actual 64x48 GL pixel probes pass. The allocated
probe uses two 2x2 images (32 RGBA bytes),12 draws and 24 triangles. It distinguishes UV
orientation, RGB order, UNORM modulation, alpha ignoring, texture rebinding, repeat,
linear sampling and the declared fallbacks. UNORM readback allows an explicit
one-byte rounding tolerance; the independent oracle uses exact rational arithmetic.
Eleven numeric sensitivity predictions distinguish alternative wrong semantics;
these are oracle checks, not alternate GPU mutant executions. Independent tests
also reject 14 ordinary schema/scalar errors and catch five valid-but-wrong binding
or span mutations. No pointer, ownership or allocation-failure experiment is used.

The C++ exporter passes 33 focused cases normally and 33 with ASan/UBSan, including
independent comparison of every span and asset identity. It bounds the selected
input payload aggregate at 32 MiB and retains explicit per-input limits. Fresh builds
from `tools/build_visual49h.py` reproduce the final manifest exactly in both modes.
All new translation units compile with `-Werror`; the unchanged inherited scene
parser's existing warning is retained. LeakSanitizer is disabled because of the
executor's ptrace incompatibility; leak checking is unavailable, not passed.

Ten unchanged camera/input suites pass normally and with ASan/UBSan:
7,334,097 checks each, 74 tracked inputs unchanged, identical stdout and empty runtime
stderr. Full reports are in `evidence/visual49H/regressions/`. Independent review
also verifies the same source/asset boundary and reproducible final images.

## Reproduction

The checkpoint includes all selected inputs; no extra resource transfer is needed.
Run from the repository root. Use new output paths; original evidence is retained.

```
python3 -B tools/build_visual49h.py --output-dir /tmp/rr49h-normal
python3 -B tools/build_visual49h.py --sanitize --output-dir /tmp/rr49h-san
/tmp/rr49h-normal/export_surface_bindings49h --selection evidence/visual49G/asset-selection.json --export-report evidence/visual49G/export-placed-normal/export-report.json --output /tmp/rr49h-bindings.json
python3 -B tools/visual/render_textured49h.py --recipe evidence/visual49H/frames/recipe-textured-final.json --plan --plan-output /tmp/rr49h-plan.json
python3 -B tools/visual/render_textured49h.py --recipe evidence/visual49H/frames/recipe-textured-final.json --render --draw-ack CURRENT_PLAN_SHA --output-prefix /tmp/rr49h-textured
```

Use the printed current plan hash. Paths and CPU affinity contribute to the hash,
so moving the checkout can change it. Add `--disable-textures` to both planning
and rendering for the neutral control. The report records runtime and exact hash
pins. The included recipe points at the retained canonical final manifest; newly
exported manifests use paths relative to their output directory. Normal/sanitized
copies in the same directory reproduce exact bytes. For sanitizer execution set
`ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1`.

Reproduce the tiny probe with `--write-probe-fixture NEWDIR`; the script's
`--self-test` is CPU-only. Final probe and positive-control summary are under
`evidence/visual49H/renderer-positive-tests/`. Exact image-byte reproduction is
claimed only for the recorded source, recipe and installed Mesa environment;
different drivers may vary raster/UNORM rounding.
