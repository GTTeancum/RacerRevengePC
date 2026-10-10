# 50FA: source-authenticated TA track draw materials

`tests/native/track_materials50fa_fixture.h` is a cold development-client adapter.
It does not alter the flattened TA geometry or produce substitute textures.
The owner publishes ordered draw spans and a `MaterialTextureAssets50EA` bank.

## Exact retained inputs

- `evidence/visual49G/asset-selection.json`, SHA-256
  `12fad4579276c9fce30abd3009420a8529a033e1d7271ae6b0688927f8adf674`
- `evidence/visual49G/export-placed-normal/export-report.json`, SHA-256
  `ef2bdffbb36f5669c0d84eb9b31e5a3f8f4853b5ab966898bb768efd42eb47c4`
- The client's `ta-world.rrmesh` contains 110,196 vertices and 209,514 indices.
  The retained file hash is
  `4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7`.
- The source selection includes 228 TA scene assets, 284 world placements,
  279 scene/material slots, 29 material names, and 353 nonempty surfaces.

Every load verifies current selected cache hashes, material requesters,
source-ordered scene spans, original material indexes, UVs, color bytes, all
triangle indices, and positions transformed with the exact recorded world
placement. Source scene roots are replaced by the placement exactly as in the
49G exporter. An incorrect mesh of the same size fails parity. No ordering,
UV inversion, root reapplication, geometry deduplication, or material guessing
is allowed. Index values remain global; draw with base vertex zero.

## Retained cache coverage

At inspection on 2026-10-08, all 21 referenced largest TA RRTEX payloads existed
and matched their retained SHA-256 hashes. They total 3,317,760 RGBA bytes;
the largest dimension is 256. They cover 279 of 353 spans, or 64,396 of 69,838
triangles. The other 74 spans cover 5,442 triangles and carry explicit unresolved
texture indexes and reasons. No texture is generated for such slots.

Remaining source associations:

| Resource | Spans | Actual source format |
| --- | ---: | --- |
| Track, Trackdark, Trackside, Trackwheel | 2 each | PST kind 2 |
| cord | 7 | PST kind 2 |
| crawlerA | 2 | PST kind 2 |
| arrowred | 7 | ATM; no same-name PST |
| crowd | 50 | ATM; no same-name PST |

These are not inferred historical failures. The retained current
`work/native/project/disc/TRACKS/TA.RES` was inspected directly: 3,334,562 bytes,
383 directory entries, SHA-256
`b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.
The six PST kind-2 member hashes exactly match the selected unresolved records.
The unchanged `decodeTexture48E` admits only kind 1. Arrowred/crowd retain actual
ATM resources, not missing art. `cable_shadow` and `specmap5` are absent from
this archive and are not texture slots used by the selected TA materials.

## Cold client interface

Construct `Request` with:

- `root = "evidence/visual49G"`
- `exportReport = "evidence/visual49G/export-placed-normal/export-report.json"`
- `choice = {1, 1, optionalUntextured, largestRetained, 0}`
- `coldSourceByteCapacity = 32 MiB`
- `textureBudgets = {32, 1024, 8 MiB, 8 MiB, 1024}`

Call `Owner::load(request, liveTrackMesh, diagnostic)` before GPU creation.
`bank()` exposes immutable 50EA textures and slots. `spans()` exposes
`indexStart`, `indexCount`, `vertexStart`, `vertexCount`, `textureIndex`,
`materialSelection`, original world-instance index, and surface index.
`selections()` retains exact source scene/material/pass/slot/texture names,
program name, and unresolved reasons. `textures()` retains loaded image hashes.

Translate a resolved texture index to the renderer's `requiredTexture` binding;
translate the unresolved sentinel to explicit `untextured`. The latter must
retain source vertex colors instead of a made-up image. Keep the CPU owner
through GPU creation and the GPU bank through rendering. No runtime allocation
or lookup work is introduced by the cold adapter.

## Optional newly recovered original formats

The request optionally accepts `sourceArchive` and `decodeSourceTexture`
together. The owner verifies the exact original TA archive hash, decodes its
bounded directory/storage using the established reader, extracts only each
explicitly matched original member, and verifies its hash and size before
calling the reviewed decoder callback:

```cpp
bool decode(const char* sourceMember,
            const std::vector<std::uint8_t>& authenticatedOriginal,
            std::vector<std::uint8_t>& rrtex,
            std::uint32_t& mipIndex,
            std::string& error);
```

The callback may consume a verified offline decode; it must not manufacture
pixels or select a different resource. It returns actual RRTEX bytes and the
selected original mip index. The resulting image framing, dimensions and byte
budgets are validated before the bank parser; its image hash and original source
hash remain available in metadata. Each original member is decoded once.
Unsupported decodes remain explicit unresolved slots with their reason.

`allowStaticAnimationFrame` must be explicitly true to attempt either ATM
resource. This admits a real decoded animation frame as a temporary display
policy; it does not claim original animation timing or implement animation.
The pinned retained `.psm` records each contain one pass and one texture slot.
The renderer's simple texture/vertex-color draw is not original microprogram,
alpha/blending, lighting, sampler or PS2 rasterization conformance.

## Verification ownership

No game, build or executable was launched by the adapter author. The root
integration owns focused compilation, live rendering, and measured GPU-complete
frame timing. Current cache presence/hash and actual archive member format/hash
checks were performed read-only. Full live callback coverage must be verified
with the final compiled source and recovered texture artifacts; the initial
partial-cache build does not establish callback execution.

### Focused root integration result, 2026-10-08 17:17 UTC

The integration owner reported that the initial actual interactive client passed
cold source parity and completed 3,159 simulation ticks with a clean quit.
Renderer counters established 353 track draws: 279 actual textured spans and
74 explicit untextured spans. GPU-complete cloud frame timing was approximately
60–63 FPS during driving and collisions. Its captured frame was reviewed and
showed real sand and building texture maps. This validates the initial retained
21-texture path; it does not yet validate any additional kind-2/ATM decode.
The subsequent callback-capable header also compiled and linked successfully.

### Source image adapter available

`tests/native/source_texture50fc_fixture.h` provides
`source_texture50fc_fixture::decodeSourceTexture` for the optional callback.
The generated `source_texture50fc_index.inc` pins all eight source member hashes,
source extents, decoded image hashes, dimensions, and original largest mip
indexes. It reads the already decoded real images from `evidence/textures50FC`;
there is no runtime FFmpeg dependency. The conversion/proof report is
`evidence/textures50FC/manifest.json` and the reproducible converter is
`tools/assets/decode_source_textures50fc.py`.

These eight base images add 720,896 RGBA bytes, making the combined bank
29 textures and 4,038,656 RGBA bytes. Full coverage should be 353 textured spans
and no unresolved spans; this requires a final source-enabled live run, rather
than extrapolating from the partial-cache run. The source conversion reports
that exact PS2 hardware inverse-DCT rounding has not been framebuffer-validated.
ATM playback remains a genuine static first frame, with animation unimplemented.
