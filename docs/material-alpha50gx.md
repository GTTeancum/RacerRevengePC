# 50GX: retained TA material alpha and blend policy

## Solid-beam cause

The 50GU track fragment shader retained only `rrColor.rgb` and emitted
`viewerOutput = vec4(color, 1.0)`. The draw owner never selected a blend
function. The earlier `discardZeroAlpha` path was limited to crowd/arrow
cutouts and left every surviving fragment opaque with writable depth.
The texture uploader already submitted exact RGBA8 bytes, and the draw adapter
already bound each original material through its owner. Neither 50EB nor 50CV
needs a change; their preimages and hashes are retained under
`solo-alpha50gx/preimage/`.

## Exact original source evidence

`work/native/project/disc/TRACKS/TA.RES` has SHA-256
`b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.
Read-only directory and zlib-block extraction verified all 29 selected TA
material member hashes against the pinned 49G records. The four original
blended `.psm` members are retained unmodified in `solo-alpha50gx/materials/`.
The inspection JSON retains source/record hashes, authored values with source
offsets, scene requesters, world spans, and relevant original alpha histograms.

The reported tunnel rays are real `zsunbeam.psg`, `zsunbeam3.psg`, and
`zsunbeam2.psg` surfaces using `rclightstrip`. Their source material key is
`0e8d108a38f22235038543b86fb9d6d8c1c648d7ba6959cdadef6c026d04b47e`;
original PSM SHA-256 is
`404d0f90492c924bc7bca32ce73f1863c4d0320ce4b045f9bbd18c8aed78100b`.
Its first pass explicitly authors:

- `alphaBlendingOnOff=1`
- `inputColorA=0`, `inputColorB=2`, `inputAlphaC=0`, `inputAlphaD=1`
- `zWriteDisabled=0`
- `alphaTestOnOff=0`
- `foggingOnOff=0`
- `microProgramName=DiffuseCompressed`

The GS equation `(Cs - 0) * As + Cd` maps to additive OpenGL factors
`SRC_ALPHA, ONE`. Conventional source-over would darken the backdrop and is
not the equation this material requests. Depth writes remain enabled because
`zWriteDisabled=0` is explicit. A future no-depth-write policy would be a host
ordering choice and must not be presented as recovered source state.

The actual largest retained 128x128 texture is
`evidence/visual49G/cacheobjects/5b5d221ce9533b1c7d425545ed93854df202e0d90983462fbc59e3822d61f004-mip3.rrtex`,
SHA-256 `fd58d018df9a1de2f5d91dc8b25cab31da743ad15f0789cc206bf7c9b76fec59`.
Its alpha has 20 distinct values, from 0 to 128. RGB ranges are
R/G 240..248 and B 192..224. All 68 source vertices across the three meshes
have alpha 63. Discarding alpha therefore turns the intentionally soft beam
texture into a nearly white solid plane.

### Selected blended material table

| Material | Authored A/B/C/D | Host equation | Spans | Source depth write |
| --- | --- | --- | ---: | --- |
| rclightstrip | 0/2/0/1 | additive `Cs*As+Cd` | 3 | enabled explicitly |
| arrowred | 0/1/0/1 | source-over | 7 | enabled explicitly |
| bantha | 0/1/0/1 | source-over | 4 | unspecified, retain enabled host policy |
| crowd | 0/1/0/1 | source-over | 50 | unspecified, retain enabled host policy |

The remaining 25 selected materials retain the prior opaque policy. A source
blank is still blank, not proof of an original default. Crowd/arrow retain
the existing explicit zero-alpha cutout policy. Crowd's `alphaReferenceValue=120`
is retained as evidence but not guessed into an alpha-test operator: its
`alphaTestOnOff`, method, and failure handling are unspecified.

## Narrow cold adapter

`tests/native/material_alpha50gx_fixture.h` admits only the 29 authenticated
50FA material keys and verifies corresponding material name, program,
pass 1, and slot 1. It does not infer blending from texture names or alpha
histograms. The lookup returns a transactional policy and refuses unknown or
mismatched records, including an unresolved texture for a blended material.
The 50FA owner already authenticates source material and texture associations.

The four blended materials preserve source vertex and texture alpha and use
an explicit host GS-style modulation normalization:

`clamp(vertexAlphaGL * textureAlphaGL * (255/128)^2, 0, 1)`.

This yields ray maximum 63/128 = 0.4921875 and, for a texture alpha of 255,
63*255/16384 = 0.98052978515625. Both input byte streams remain unchanged.
The retained PS2-oriented converter reference explains its 128-based color
range in `third_party/cars-tools-reference/CarsTools/SLTConvert/DLL/SLTConvertDLL/PSGConvert.cpp`
near line 544. That later tool reference is supporting context, not proof of
this game's microprogram. Original TFX/TCC fields are blank, and original VU
shader execution has not been recovered here. Thus this is an explicit
host modulation policy, not pixel-exact PS2 shader, test, sorting, AA, or
rasterizer conformance. Existing opaque lighting/exposure remains unchanged. The three beam placements
explicitly have `use_lighting=false`; their policy additionally selects unlit
host shading while retaining the caller's source-color scale. Other materials
keep their existing geometric lighting. This does not equate the source
`DiffuseCompressed` program with a fully recovered unlit shader.

## Useful ray locations

| World instance | Source member | Index start/count | World placement |
| ---: | --- | --- | --- |
| 169 | zsunbeam.psg | 46122 / 30 | 5387.4585, 2007.0010, 12887.2041 |
| 174 | zsunbeam3.psg | 52644 / 36 | 4044.9524, 1409.6539, 11888.4844 |
| 180 | zsunbeam2.psg | 58869 / 36 | 5554.9956, 2221.6877, 12787.2275 |

## Verification boundary

This material investigation only inspected authenticated assets and source.
No test or game executable was launched by its author. The integration owner
owns focused compilation, live draw counters, screenshots, and alpha/depth
ordering checks. Exact original hardware blending and microprogram parity are
not established by this change.
