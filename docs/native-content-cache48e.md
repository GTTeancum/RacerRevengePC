# Native content cache and material binding 48E

The built-in converter now processes the supported PST1 texture and authored PSM material subsets, alongside rigid scene geometry. Original source bytes are read only during conversion; scene inspection with `--cache` consumes only verified native objects.

The generation's converter is `rr-native-content-48E-v1`. Object converters are independent: scene objects keep `rr-native-scene-48D-v1` so accepted48D geometry can be reused without rewriting it; texture objects use `rr-native-texture-48E-v1`; material objects use `rr-native-material-48E-v1`. Keys remain SHA256 of converter, newline and original member SHA256. Every object record also has a canonical metadata hash.

Texture metadata holds every ascending original mip's index, dimensions and native-file hash. Files are `objects/<key>-mip<N>.rrtex`, each bounded RRTEX001 RGBA data. Original rows and raw alpha are retained. Material metadata preserves general fields and each original pass's number, program name, numbered map names, whether the map count used the proven original default, and authored/unspecified fields with source offsets. No original shader is inferred from its program name.

Cache reuse verifies metadata and all mip files, dimensions and structures. Final generation verification checks all scene, texture and material objects before replacing `current.json`. Object files are atomically staged and verified. A cancellation or failure leaves the active generation unchanged; finished objects remain reusable. Existing48D objects are preserved. Unsupported original format variants are reported explicitly. Structural truncation, invalid extents and malformed input still stop promotion.

The runtime entry is:

```
racer_native_pc --cache <native-cache> --container PODS/POD01/POD01.RES --member pods\pod01\chariot.psg --frames 3 --capture <new-output.ppm> --report <new-report.json>
```

The loader verifies the active manifest hash and selected object hashes. Material and texture names resolve case-insensitively to `textures/<name>.psm` and `.pst`, preferring the selected scene container. A cross-container reference is used only when all matching accepted candidates have an identical original source hash. Missing and ambiguous bindings stay unresolved and are recorded in the capture report. This bounded resolution policy is not yet a reconstruction of every original runtime resource-search rule. Unsupported TGA references remain explicit missing bindings.

Native materials retain every parsed pass and texture slot. The diagnostic renderer currently draws the first pass's first texture on each original scene surface, with opaque alpha and the existing vertex-color multiplication. It uploads all retained original mip levels, but diagnostic sampling clamps to the base level until original sampler-state behavior is recovered. All other shader programs, lighting, alpha/blending and additional passes/maps remain pending; their presence is reported. GPU resources are created once before the draw loop, not redecoded or uploaded per frame.

Cache inspections require fresh output files outside the cache, preventing reports or captures from overwriting native inputs, including existing hardlinks. The scene loader has a 256MiB texture-payload bound; this is a desktop validation bound, not an Xbox memory target. The resource figures remain payload measurements rather than total process/driver residency.

`tools/test_import48e.py` exercises migration from a verified48D cache, cancellation with active-generation preservation, reuse, exact independent mip comparisons, damaged texture/material repair, multi-surface cached rendering, corrupt-manifest rejection and output-alias rejection. All selected original fixture files are hash-checked after execution. Full-disc decoder checks and promoted-cache verification are separate evidence; no static inspection result is a gameplay or frame-rate acceptance test.
