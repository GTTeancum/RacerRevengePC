# Animated engine view, checkpoint 49S

This milestone makes the recovered Pod01 engine animation visible. The real 49Q brake/fin node locals and 49R engine-root response feed an original-source-backed model palette, then the retained semantic mesh vertices. Four native frame samples and two explicit frozen-motion controls are rendered in the existing TA inspection view. The original 49H camera, world, material associations, draw order and static supplied Chariot remain intact.

The authoritative image runs are `evidence/modelPalette49S/frames/pass-4` and `pass-5`. Earlier passes are retained preliminary artifacts whose exporter provenance predates the final read-only verification CLI. The final pair uses the same frozen native outputs and must match in PNG bytes and decoded pixels. `frames/presentation` contains an unscaled four-frame sheet, enlarged control crops and a lossless four-sample loop. That loop's presentation timing is not a measured game frame rate.

## Original association and palette producer

The original ELF is `work/native/project/disc/SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. All source ranges and actual PSG/cache comparisons are retained under `evidence/modelPalette49S/assessment`, `source` and `cache-assessment`.

The original hierarchy reader's source-node array is copied to model+1B8 at 26EEA4–26EEB8. The PSG palette trailer is read by 240210. Actual engine surfaces have component word 00000107, selecting palette zero; their sole palette is the identity list 0–13. Packed position-W magnitude divided by 16, minus one, identifies one rigid palette entry; its sign controls strip emission. There are no recovered weights or inverse-bind matrices in this admitted subset.

`projectModelPalette49S` implements a selected numeric projection of three source spans, not a complete 241280 render call:

1. 2412E4–2412FC conditionally refreshes a dirty model root through 2897B0.
2. 2414F8–241518 copies all 16 current root-world words separately.
3. 241518–2415A4 reads the selected palette and visits every stored byte ID in order. 241540–241550 resolves the ID through model+1B8; 24155C calls the scene getter; 241564–241580 copies the complete current node-world matrix.

The source advances its packet slot by 0x50. This API represents the 16 matrix words, not unimplemented lighting/material packet fields. Its nonnull admission requires every referenced source node to exist. Every entry is visited, even if its node has no geometry or its ID repeats. Original null-node behavior is outside this admission. The root may have a parent; 49P supplies exact dirty/lazy world propagation. Ready opaque caches remain source-visible rather than being silently repaired.

Current node worlds already contain the model root and parent chain. The vertex consumer must not apply the separately copied root world again. Visibility, LOD selection, original material/VU execution, observers and original model ownership remain outside this projection. The complete-scene and selected-phase tags are caller attestations, not a renderer scheduler.

## Actual mesh binding and vertex arithmetic

All three selected resource members match the preserved PSG fixtures, semantic RRSCENE1 cache and bind exports. Engines have 14 nodes each; Chariot has 18. Each engine has four LODs; this view explicitly retains the previous LOD0 choice.

The two engine LOD0 meshes contain 2,871 vertices and 1,905 triangles. The body surfaces use nullcg, brakes 2/3 and fins 11/12/13; separate flame surfaces use nodes 8/9. Brakes and fins directly control 640 vertices and 410 triangles. All engine vertices receive root response. There are no mixed-bone triangles in these selected meshes.

The existing RRMESH01 Pod is already bind-baked and placed. It is not an input to the deformation formula. Immutable compact records instead retain RRSCENE1 node-local double XYZ and the rigid palette index. Identity palette admission makes the actual cached bone index equal to its palette slot. General unsupported source palettes are not inferred from this fact.

`projectRigidVertices49S` multiplies local XYZ by the selected current node world exactly once. It preserves the earlier diagnostic export order: begin with double zero, multiply/add XYZ in order, add translation, then convert once to float32. This is the documented 48D/49G diagnostic cache/display arithmetic, not a recovered original VU vertex microprogram. No inverse bind, weighted skinning, normal transformation or root/placement reapplication is added.

Only the 2,871 engine position fields are replaced in the 5,755-vertex/10,713-index Pod buffer. Every UV, color and index byte, and every byte of the first 2,884 supplied static Chariot vertices, remains identical to 49H. Surface/material partitions stay unchanged. The importer and inherited renderer files are preserved.

## Connected numeric sequence and explicit state

The native exporter advances 96 persistent frames in this order: 48W supplied numeric controls, actual 49Q engine MOT, complete admitted 49R root response, selected original palette projection, then diagnostic rigid positions. Original-instruction references compare the represented state and every palette. The original 49Q reference also executes its authentic Chariot brake prefix to obtain engine clamp gates; no Chariot display output or IK is projected natively.

The main pose is explicitly held stationary. Its position uses the existing TA inspection anchor plus the actual right nullcg Z for the engine-frame origin. Response positions are supplied at the authored lateral offsets, with explicitly supplied remaining histories/component fields. These are current diagnostic state, not an authentic constructor/spawn result. Engine nullcg XYZ clearing is an explicit incoming condition. No physical integration, contact generation or post-physics body copy occurs.

The retained scenario-zero raw control samples/control elapsed values remain supplied inputs. Quantized 49I timestamps prepare the same application frame factor for both 49Q and 49R. The exporter records all chosen pose, speed, old engine 70/74, damage, probe, impact, flags, phase clocks, shared noise and seed values. The connected README is the complete input contract. It must not be read as an OS/device binding or whole-game timing loop.

The 96 native updates produce 850 moving brake/fin node observations, 1,152 original noise calls and 1,791 RNG calls. Four captured frames are 0,31,63,95. The two frame95 controls replace either engine node locals or engine roots with their frame-zero values using explicit scene setters. They are labelled counterfactual display controls. Their 640/2,871 differing vertices distinguish node animation from whole-engine response.

## Storage and host cost

Both new production APIs are serial, fixed/bounded caller-owned operations with no allocation, I/O or callbacks. Palette admission is 1–64 source nodes/entries and at most 64 coherent scene records; unused output-tail entries are preserved. Graph/output commit is transactional through a 13,380-byte caller workspace. A palette output is 4,164 bytes. All descriptor/backing/output/scratch/diagnostic records have explicit distinct-live-record preconditions; this is not general lifetime or alias inference.

Rigid batches admit 1–4,096 vertices and 1–64 finite affine matrices. Two passes validate then commit without output scratch. Stable caller inputs, nearest/even and strict separate arithmetic are required. On failure output positions remain untouched. Compact inputs are 32 bytes per vertex; outputs are 12 bytes. The actual two-engine inputs/outputs use 91,872/34,452 bytes; measured component caller payload including the existing 52-node graph, two palette outputs, workspace and descriptors is 155,821 bytes.

Installed GCC14.2 C++17 objects add 1,710 .text bytes and 64 .rodata bytes, with zero .data/.bss. Reported stack values are static compiler estimates, not measured high-water or Xbox memory budgets. The benchmark pins one CPU, preserves all 14 getter calls per engine, uses actual local vertices and checks complete graph/palette/position checksums. Fifteen samples per mode give medians of 10.217 microseconds for two ready palettes, 13.773 including both root setters, and 81.744 for all rigid vertices. Combined medians are 92.259/95.249 microseconds. These are host subsystem timings, not full-frame/gameplay FPS.

The view still submits 360 ordered draws and 73,409 triangles, using 26 textures and 7,604,084 bytes of geometry/texture GPU payload per frame. Framebuffer/readback, driver, Python, validation and image work are additional. Mesa llvmpipe is constrained to one CPU and one software worker with the preserved 2 GiB/60 CPU-second/90 wall-second limits. Actual host RSS/timings are recorded in each render report; they are not target-console requirements.

## Verification and recovery

- Palette proof: 542 fixtures,1,077 projections,2,575,914 checks normally and with ASan/UBSan; 21,060 getters and10,860 ordered flag stores. Coverage accounts for all178 selected words (174 reached,4 excluded). Semantic controls distinguish source-state mismatches from the separate source-index-shift bounds rejection.
- Rigid proof:371,680 checks, all2,871 engine vertices under three supplied palettes, exact rational arithmetic oracle and seven executable semantic controls, including premature float rounding, FMA and double root application.
- Connected proof:1,110,835 checks in normal, plain no-hook read-only, and ASan/UBSan modes. All six mesh, trace and golden outputs agree. Independent reconstruction verifies every engine vertex and detects wrong consumer alternatives.
- Independent review checks analytic/metamorphic behavior, source/asset identity,196 full palettes and17,226 exported engine positions, malformed report/mesh cases, final image metadata and repeated actual framebuffer pixels.

LeakSanitizer is disabled under the recorded tracing limitation and is not claimed tested. The inherited aggregate uses C++20 for retained interpreters; focused production and footprint builds require C++17. CMake registration is supplied but cannot be executed here because CMake is absent. Final 52-suite regression totals, all input hashes and the release seal are recorded separately in `checkpoint-validation.json`; component `validation.json` remains intact.

```sh
python3 -B tools/test_native_animated_view49s.py --output-dir /tmp/rr49s-normal
python3 -B tools/test_native_animated_view49s.py --sanitizers --no-leak-check --output-dir /tmp/rr49s-sanitized
python3 -B tools/visual/render_animated49s.py --self-test
```

The renderer requires the exact current native export-report hash and a separate non-drawing plan before drawing. All selected assets, native fixtures, source proofs and actual pixels are included in the recovery archive. 49R remains preserved. The collision contract remains unresolved and unchanged; separate49C work is excluded and unreleased. Neither held historical diagnostic was retried or repaired.
