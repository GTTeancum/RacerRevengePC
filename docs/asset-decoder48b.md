# Original static asset decoder 48B

Date: 2026-10-03. This is the first bounded offline semantic asset conversion
experiment for the native PC engine. It does not run the PS2 runtime or build or
execute the supplied CarsTools. Neither the whole track nor gameplay is accepted.

## Deliverables and repeatability

`tools/assets/decode_static48b.py` exposes importable `decode_psg3`, `decode_pst1`,
`extract_members`, `mesh_bytes` and `texture_bytes` functions. `FormatError`
identifies unsupported/malformed input. Paths resolve from the project location;
there are no hard-coded drive paths. `extract_members` reuses the preserved
bounded RES decoder at
`work/native/project/scripts/validate_ui_resources07.py:13` without changing it.

Run with a configured Python 3 interpreter:

```
python -B tools/assets/decode_static48b.py --output evidence/assets48B/ground
python -B tools/assets/decode_static48b.py --mesh "geometry\ta_bigrocka.psg" --output evidence/assets48B/rock
python -B tools/assets/test_static48b.py
```

The default container is `work/native/project/disc/TRACKS/TA.RES`; `--container`
accepts an explicit original container. These commands write only their output
directories. Each contains original member bytes, a mesh candidate, two clearly
named texture-interpretation candidates, and `conversion.json` with original
container/member/output SHA256, logical member offsets, parsed fields, every
packed attribute command, bounds, normals, and unresolved semantics. Output bytes
are reopened and compared after writing. The final viewer contract is documented
separately in `docs/native-asset-contract48b.md`.

The planned product includes a built-in PC importer: select original data,
convert once into a verified cache, show progress, resume validation, and later
offer Xbox export. These bounded importable functions are its first format
foundation. This command-line experiment is not a complete built-in importer,
and does not establish whole-format or all-disc support.

## Original data and observed format

TA.RES SHA256 is
`b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.
The original container decompresses to 13,099,008 bytes. Logical offsets below
refer to that decompressed storage, not offsets in the compressed RES file.

| Member | Offset | Bytes | SHA256 |
| --- | ---: | ---: | --- |
| `geometry\zone01groundstrips16.psg` | 8527872 | 1552 | `34135fefe81be7a98797d64cb3068feebdd7ee063cd5ccd34c42fb635dc23e4e` |
| `textures\ta_sand8.psm` | 9560064 | 261 | `d31a59956f5792c58740a72905c229cabb9f6e4c06e36b7f174a55c77cb3953a` |
| `textures\ta_sand8.pst` | 11599872 | 88412 | `d43ee9e14e4bf4033248b298b4ff55f428394c77cf562a7b90f2ed22e849f63a` |
| `geometry\ta_bigrocka.psg` | 393216 | 2016 | `f66b3bb55b7fadd4c855c42376d0862f2de1824ec932d49428ace548e4eeaaaa` |

Every one of the 230 TA PSG files and 13 POD01 PSG files inspected during planning
begins with `psg\0` and version 3. The implementation supports only the demonstrated
static subset: one object with an identity matrix, no parent, one material, one
LOD, one surface, the observed flags, and packed signed-16 positions,
signed-8 normals, signed-16 UVs and unsigned-8 colors. Unknown layouts reject;
they are not silently coerced into static geometry.

The material is an original INI file with one `PreLitCompressed` shader pass and
one texture name. The decoder requires that exact subset. The final native mesh
contains positions, UVs, colors and triangle indices; it contains no microprogram,
register state, VIF command, GS packet or host pointer.

## Source lineage and differences

Paths in this paragraph are relative to the preserved
`third_party/cars-tools-reference/CarsTools` tree. Source was read for knowledge;
none was copied into the native implementation.

* `SLTConvert/DLL/SLTConvertDLL/BaseConvert.cpp:955` writes the hierarchy count,
  128-byte names, 64-byte matrix, bounds center/half/radius and parent. These
  offsets match the selected original version-3 headers.
* `BaseConvert.cpp:276` and `:318` write material count/name. `:654` through `:746`
  describe the inverse position/normal/UV scales, quantization center, and
  world-space flag. The candidate inverse transformation is independently checked
  against stored unquantized hierarchy bounds.
* `PSGConvert.cpp:1393` quantizes positions after subtracting the center;
  `:1499` describes signed-16 UV packing; `:1562` describes unsigned color packing;
  `:1821` documents suppressing the first two vertices of each strip.
* The later exporter writes PSG version 5 (`PSGConvert.cpp:22`). Its 16-bit ADC
  encoding uses bit 15 (`0x8010` for the relevant bone value), whereas these original
  version-3 files use signed negative values `-16` (`0xfff0`). The decoder explicitly
  rejects the later encoding and accepts only the observed `-16,-16,+16...` runs.

The original PST loader supplies additional independent support. Generated source
retains original instruction comments under `work/native/project/generated`:
`FUN_0024e880_0x24e880.cpp` checks revision 1, reads type and mip count; its
continuations read a raw palette through `FUN_0011a630_0x11a630.cpp`, which uses
palette count times four bytes. `entry_0024edf8_0x24edf8.cpp` at original addresses
0x24EE14–0x24EE3C computes width times height times bits-per-pixel divided by eight
for the type-1 mip read. Conditional post-processing follows at 0x24EE48. These
reads support the structural parser; they do not settle all later upload and
sampling transformations. The parent's loader-source evidence preserves the
discontiguous function coverage and original ELF correspondence.

The later CarsTools PST revision-2 layout is incompatible with directly reading
this sample. It uses other format enums, puts palette data before mip count and
has a `SizeBytes` field. Original revision-1 samples place mip count at +8,
palette at +12, and have two zero words after each height/width pair. An initial
read-only attempt to use the later `SizeBytes` interpretation failed; the bounded
decoder uses the original loader's dimension-derived byte count instead.

## Geometry results and independent checks

| Candidate | Vertices | Source strips | Triangles | Normal alignment | Largest bounds error |
| --- | ---: | ---: | ---: | --- | ---: |
| Ground | 53 | 14 | 25 | 24 positive, 0 negative, 1 degenerate | 0.185245812 |
| Rock | 73 | 16 | 41 | 40 positive, 0 negative, 1 degenerate | 0.003805459 |

The decoded bounds differ from the original stored pre-quantization bounds by
less than one position quantum. Ground has quantum 0.208516896. No positions are
recentered in the saved mesh, no geometry is invented, and duplicate/degenerate
triangles are retained. Inspecting the mesh near the origin is a viewer camera or
model transform, not an asset alteration.

The packed group counts agree with the original header vertex and strip counts.
ADC pairs divide the packed data into source strips. Local alternating triangle
winding yields the normal alignment in the table. Continuing parity across each
whole packet instead would disagree with half the nondegenerate ground normals
(12 positive / 12 negative); this separate attribute provides a useful independent
check. This establishes a consistent native source-strip orientation, not final
game viewport/culling or VU shader equivalence.

The minimal mesh contract omits normals; the exact decoded normal list remains in
provenance. Original vertex-color bytes are retained unchanged. Ground alpha is
63. The viewer must report any chosen color/material interpretation; these bytes
must not be silently doubled, made opaque, or otherwise described as verified
original presentation. Material shader arithmetic and UV-size metadata remain
open for differential validation.

## Texture candidates and remaining limitations

Both texture samples are revision 1, flags 0, type 1, with a 256-entry RGBA-byte
palette and five ascending square mip dimensions (16, 32, 64, 128, 256). Headers
at 1036, 1308, 2348, 6460 and 22860 and their dimension-derived payloads consume
exactly 88412 bytes. Every mip payload and the palette have hashes in provenance.

`texture-linear-palette.rrtex` indexes the original palette directly with the
original base-level bytes in stored row order. `texture-csm1-palette.rrtex`
additionally swaps palette index bits 3 and 4, the GS CSM1 permutation documented
in preserved `gs_cpu_backend.cpp:296`. These are **interpretation candidates**,
not two accepted alternatives. Neither texture was resampled or generated.
Original palette ordering, index swizzle, row origin and RGB/alpha treatment
must be established from the remaining original loader/upload path and captured
material evidence before selecting the accepted conversion. The first viewer
can inspect geometry with an explicitly neutral material while this is resolved.

No sky, full-track placement/visibility, collision, animation, pod assembly or
gameplay is implemented here. Runtime packet playback is not used for conversion
or native viewing. Interpreting packed attributes once during offline import
does not require the native game to retain the PS2 graphics pipeline.

## Verification

`tools/assets/test_static48b.py` records its result in
`evidence/assets48B/parser-tests.json` and the run log. The initial final suite
passes 5781 checks: every strict byte prefix of both PSG inputs; malformed
versions, counts, extents, transforms, nonfinite values, quantization origins,
commands, ADC encoding and trailing data; targeted PST truncations and malformed
dimensions/mips; deterministic output contracts; a manually derived disconnected
strip case; and original bounds/header-count/normal invariants.

Parser success proves the supported layout is bounded and internally corroborated.
It does not prove original shader/texture visual fidelity or gameplay performance.
