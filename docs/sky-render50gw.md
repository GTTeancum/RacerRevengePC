# 50GW: original TA sky in the native live client

`tests/native/sky50gw_fixture.h` recovers the actual `skies\ta.cub` asset.
This is a separate original cube resource, not a PSG scene or a track material.
No replacement sky image, missing-face image, or substitute model is created.

## Original identities

- `work/native/project/disc/TRACKS/TA.RES`: 3,334,562 bytes, SHA-256
  `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`
- `skies\ta.cub`: archive logical offset 9,043,968, length 164,187, SHA-256
  `27e43db810442e9fd90fda8551e6804ab68d48e8398992a1b2997945e88d68f4`
- Original `SLUS_202.68`: SHA-256
  `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`
- `tracks\ta.scb`: 130,511 bytes, SHA-256
  `c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d`.
  Its `Fog.ColorRGB` field at member byte offset 9,928 is `175,182,207`.

The CUB contains one palette at offset 16 and one cube at offset 99,100.
The palette is an original 768-byte RGB table and two lookup tables; it is not
used to synthesize these IPUM sky images. Cube format is 32, both flags are
zero. The six source tile masks are `00ff,00ff,00ff,00ff,ffff,0000`.
Thus the four side faces have their upper eight tiles, the top has sixteen,
and the bottom has no authored imagery. All selected size codes are 2 (128).
The 48 tile records contain four IPUM mip levels each: 16, 32, 64 and 128.
Every mip extent is validated; the final record ends exactly at byte 164,187.

## Source behavior retained

- `1295B0`: palette/cube count and offset-table loader.
- `1289C0`: cube format, flags, six masks, six packed size-code words, and
  origin/look/up vectors. TA has origin zero, look +Z, up +Y.
- `1290E0`: visits faces in ascending order and tiles 0..15, loading only
  the corresponding authored mask bits.
- `128D50`: extracts each two-bit size code, reads the compressed descriptor,
  and passes the original data to the texture loader.
- `12A8A0`: constructs the cube in face order +Z, -X, -Z, +X, +Y, -Y.
  Each face is a 4×4 grid. Source coordinates are spaced 128 and scaled 100,
  giving a half extent of 25,600. The native mesh is the planar tile-corner
  projection of that constructor. It does not recreate the original
  microprogram's internal strip subdivision.
- `12B34C`, `12B634`: each tile's UV interval is exactly 0.03125..0.96875.
  Original row order is preserved and UVs are not flipped.
- `274BB0` supplies a 1,024 offset to `288A90`; `288B00` follows camera X/Z
  and camera Y+1,024. The immutable native mesh already includes this Y offset.

Exact selected original ELF byte slices and readable listings are retained
under `solo-sky50gw/`. No PC capture, excluded 49C/48U evidence or `ee.bin`
was used. The decoded contact sheet shows the original cloudy blue sky and
distant pale horizon across the authored faces.

## Cold conversion and load contract

Run `python tests/native/recover_sky50gw.py` from the repository root to
reproduce `evidence/sky50GW/` and `tests/native/sky50gw_index.inc`.
The converter uses the already retained 50FC scalar FFmpeg IPUM decoder and
its PS2 RGB16 color conversion. Each actual source tile and decoded image is
individually SHA-pinned. Original hardware inverse-DCT rounding has not been
framebuffer-validated; this is the same explicit limitation as the 50FC path.

Package `evidence/sky50GW/*.rrtex` with the live client. Runtime needs no
FFmpeg or codec process. It authenticates the original archive and extracted
CUB, source header/masks/pose, every source tile, and every decoded image
before publishing its load-once owner.

```cpp
sky50gw_fixture::Owner sky;
sky50gw_fixture::Diagnostic diagnostic;
sky50gw_fixture::Request request{
    "work/native/project/disc/TRACKS/TA.RES", "evidence/sky50GW"};
if (!sky.load(request, diagnostic)) { /* report diagnostic.message */ }
const rr::assets::Mesh48B* mesh = sky.mesh();
const rr::assets::MaterialTextureAssets50EA* bank = sky.bank();
const std::vector<sky50gw_fixture::Span>* spans = sky.spans();
const float* clearRgba = sky.clearRgba(); // immutable four floats; null unloaded
```

Each span contains `indexStart,indexCount,vertexStart,vertexCount,textureIndex,
face,tile`, all `uint32_t`. Indices are global, so use base vertex zero.
Exact capacities: 192 vertices, 288 indices, 48 spans, 48 textures, 48 slots,
3,145,728 RGBA bytes, 3,146,496 framed texture bytes and dimension 128.

Render the sky with a separate orientation-only view matrix: eye zero,
target equal to the ordinary camera target minus eye, same camera up/FOV.
The mesh already includes Y+1,024; do not add that offset again. Its bounds
are X/Z ±25,600 and Y 1,024..26,624. A sky projection far plane must include
the furthest cube corner. Draw before world geometry with depth writes and
depth testing disabled, opaque texturing and clamp-to-edge sampling.
These are native background render choices, not a claim of full original
VU/GS rasterizer equivalence. The source color multiplier is represented as
native neutral white, with no modification to source decoded sky pixels.

## Authored background clear and the lower horizon

The original cube renderer checks both the visible-tile mask and authored
texture mask at `12C26C..12C284`; absent lower-side and bottom tiles really
are skipped. The camera-relative Y+1,024 offset and existing masks must remain
unchanged. Exposing dark diagnostic clear behind that open lower hemisphere
produces the broad dark horizon band seen with the shorter world far plane.

The original background clear uses the authored fog color even without
introducing geometry fog. `27580C..275850` packs the selected source RGB and
calls `13E830`. At `13E8A0..13E8A8`, that setter stores the packed color in
the renderer object reached through `4432F70`, member `+28`. At `2B4878`,
the viewport clear reads that exact member into argument `a3` and calls
`25C5C0` with flags 3. `25C81C..25C830` extracts the original RGB bytes for
the clear primitive. Thus the native clear is `(175,182,207)/255`, alpha 1;
it is not a guessed sky continuation, stretched texture or added fog.

`Owner::load` extracts and authenticates the complete original TA.scb from
the already authenticated archive, verifies the exact terminated ColorRGB
field, and converts its three fixed decimal triplets to normalized floats.
`clearRgba()` exposes those four immutable values after successful publication.
The fixed field offset is valid only for the pinned TA fixture and no more
general BINARY parser is claimed.

TA.scb also references `Environment.TerrainFile=nat13.trn` at byte 9,838,
width/breadth 1 and width/breadth scale 51.0. None of the 383 retained TA.RES
directory entries is `nat13`, a `.trn` file, or a terrain member. Its resolved
archive location and primitive format are therefore unavailable in the
retained inputs; no distant terrain stand-in is made here.

## Focused verification

`tests/native/sky_assets50gw.cpp` checks actual source load, all exact counts,
source-relative bounds, authored face/tile masks, per-tile index extents,
selected image dimensions, transactional missing-image rejection and
reload rejection without invalidating published views. This is a focused
asset-owner smoke, not a broad test suite or live render acceptance.

Live render integration and screenshot acceptance belong to the parent live
client/renderer work. This adapter does not claim those checks have passed.
