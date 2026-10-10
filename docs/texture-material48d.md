# Original texture and material recovery 48D

48D resolves the source texels for the bounded original PST revision 1,
flags 0, type 1 subset. Seven textures from TA and POD01 are exported as native
RGBA8 with original material-to-texture bindings. This closes the 48B linear
versus CSM1 candidate ambiguity for those files. It does not establish final
shader, transparency, lighting, animation, filtering, or mip-selection fidelity.
No Cars PST2 layout is substituted, and no PS2 runtime or packet playback is
needed by the converter or resulting native images.

## Integration outputs

`evidence/textures48D/TA/base-texture-bindings.json` and
`evidence/textures48D/POD01/base-texture-bindings.json` provide portable relative
paths by original material, pass, and map slot. Null paths identify the preserved
but undecoded `specmap5.tga`. The fuller `material-mapping.json` in each directory
records original RES/member offsets, hashes, every selected PSG's LOD/surface
material index, authored PSM fields, unspecified blank fields, and all mip outputs.

| Group/name | Base size | Native base SHA256 |
| --- | --- | --- |
| TA/ta_sand8 | 256x256 | f9cf879fc2cb3b201ab0907830bfe9deba3465ad4ca2f0639cacbc960e301879 |
| TA/ta_rock2 | 256x256 | b98a11d47668713e0796e1123c52c51d1420ac13d0cd643bf70cc05f880a4669 |
| POD01/01_1 | 256x256 | 6aaeebc148eb70169f93e59ff810afc1ff215910e8d817d112930f032fffb28f |
| POD01/01_2 | 256x256 | 93f209719544b5ff943754ae60dccc3a8cd72f534db3bf459074f53fd17a43ff |
| POD01/01_3 | 128x128 | 13c5c3e8e9c24dc8353782cbfeb8fb26a26ef5ad52e287ba3543a139077e31be |
| POD01/pod_wndw | 64x64 | c87cdf958f58a1be4a771a5f4a1e80299d3d8958d428a7403c499b29c0fc95d6 |
| POD01/flame | 64x64 | fec76a45b680057c65d4e18b3a398025a764332f66d99362c11d0df8d2f9e231 |

Base files are `<group>/textures/<name>/texture.rrtex`. Each follows
`docs/native-asset-contract48b.md`: RRTEX001, little-endian width and height, then
exactly width*height*4 RGBA bytes. Original source row zero becomes output row
zero (T=0), without a vertical flip. Individual native mip files preserve the
original smallest-to-largest chain; the minimal RRTEX format itself contains
one level. Original PST, PSM, and the unsupported TGA bytes are retained.

## Original source proof

Original ELF: `work/native/project/disc/SLUS_202.68`, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
TA.RES SHA256 `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.
POD01.RES SHA256 `08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`.
`source-evidence/anchors.json` records the exact generated source fragments,
source hashes, original address ranges, and 2,550 instruction words checked
against the ELF. Forty discontiguous source fragments, four raw ELF data tables,
and eight supplied PCSX2 excerpts are preserved alongside it.

| Original addresses | Meaning established |
| --- | --- |
| 24E880, 24EA24, 11A630 | Revision 1/type 1 reads 256 palette entries as raw four-byte words. |
| 24EE14, 24EE38 | Mip dimensions and width*height original 8-bit index bytes are read. |
| 24EE44, 24F014, 24F044 | Optional fast-upload conversion at 24D6F0 is controlled by 382FC8, then sets a mip flag. |
| 24D3E0, 24D470, 24D6F0 | Byte, block, and page translation prepares PSMCT32 uploads from PSMT8 source pixels. |
| 250680, 250698, 2506B0, 2506D8 | PSMT8 buffer width is at least two GS units and rounded even. |
| 250CA0, 250CB4, 250CC8, 250CE0 | Fast upload halves dimensions and buffer width and uses 32-bit upload pixels. |
| 250CE4, 250CE8, 250CEC, 250CF0 | Ordinary upload retains dimensions and 8-bit pixels. |
| 250D58, 250D68, 250D6C, 257CB8, 257D34, 257D74 | Raw palette pointer is uploaded unchanged as a 16x16 PSMCT32 image. |
| 242150, 242154, 242274 | Original 8-bit texture binding selects PSM=19 (PSMT8). |
| 242284, 242288, 24228C, 242294 | Descriptor fields explicitly set CPSM=0, CSM=0, CSA=0, CLD=1. |
| 242814, 242820, 242828, 242834, 242840 | Those fields pack into TEX0 bits 51, 55, 56, and 61 and register 06. |
| 24719C, 247214, 247248, 247260, 2472CC, 2472F0, 2472F8 | Original material loader reads program name, map count (default one), TextureMap_N names, resolves textures, and stores handles. |
| 26E670, 26E6A4 | PSG material table reads count and fixed 64-byte material names. |

The original conversion tables are uint32 data at 382FD0 (128 values), 3831D0
(32 values), and 383250 (32 values). Flag 382FC8 is also preserved. The oracle
literally translates the three original upload functions, then writes and reads
memory using independently sourced PCSX2 `GSTables.cpp` block/column tables.
PCSX2 `GSClut.cpp` and `clutTableT32I8` provide independent palette addressing.
`GSLocalMemory.h` confirms 8-bit texel sampling returns the selected palette
word. These are supplied local reference sources, with exact hashes recorded;
they were neither built nor copied into the native runtime.

## Resolved conversion

The original PST source indices are already linear. Applying the optional
upload rearrangement to a native image would rearrange it incorrectly: that
operation exists to transfer 8-bit texture data through a 32-bit GS upload.
The actual palette lookup, however, is non-linear. For index i, select palette
entry `(i & ~24) | ((i & 8) << 1) | ((i & 16) >> 1)`; equivalently swap bits 3
and 4. This follows the raw 16x16 palette upload and the confirmed CSM1/CPSM32/
CSA0 binding, rather than choosing the better-looking 48B candidate.

The selected palette word's R, G, B, A bytes are retained exactly. There is no
RGB doubling, alpha doubling, gamma correction, or premultiplication. This is
source texture recovery; original fixed-point shader modulation and GS alpha
arithmetic are separate from normalized native rendering and remain unresolved.

## Material coverage and remaining shader work

Selected TA meshes are `geometry\zone01groundstrips16.psg` and
`geometry\ta_bigrocka.psg`; their materials bind `ta_sand8` and `ta_rock2` under
`PreLitCompressed`. Selected POD01 meshes are `chariot.psg`, `englt1.psg`, and
`engrt1.psg` beneath `pods\pod01\`. All their LOD surface assignments are saved.

| Pod material | Authored program and bindings | Remaining requirement |
| --- | --- | --- |
| 01_1, 01_2 | SpecularMapDualCompressed; slot 1 corresponding base map, slot 2 specmap5 | Recover native equivalent of dual specular shader and second-map state. |
| 01_3 | DiffuseCompressed; 01_3 | Establish lighting/modulation and sampling behavior. |
| pod_wndw | DiffuseCompressed/pod_wndw, then SpecularMapCompressed/specmap5 | Preserve two passes and verify alpha, depth, and specular behavior. |
| flame01l, flame01r | DiffuseTexTransCompressed; Flame | Recover animated UVs and transparent blending. |

`specmap5` is present as `textures\specmap5.tga`, not a PST file. Its original
12,332 bytes are preserved with SHA256
`574712ee163003de4bdbdfbeff4d495c52570b9dd2be3b62ec399d7b3dd27b6b`.
No decoding or replacement specular map is claimed. Blank PSM fields remain
explicitly unspecified; authored flags are preserved without invented defaults.

## Validation and reproduction

From the repository root, using the available Python interpreter:

```text
python -B tools/assets/verify_texture48d.py
python -B tools/assets/decode_texture48d.py
python -B tools/assets/preserve_texture48d.py
python -B tools/assets/test_texture48d.py
```

The independent layout oracle passes every pair of power-of-two dimensions
16 through 1024: 49 shapes, each using unique integer labels for every source
coordinate. This avoids the false positives possible with repeated byte test
patterns. It also checks the entire 256-entry palette permutation independently.
Real-file differential checks cover seven original textures, 25 mip surfaces,
and 375,040 complete RGBA texels. Parser tests reject truncations, bad dimensions,
unsupported revisions/flags/types, extra bytes, ambiguous material declarations,
and unsafe texture names. Portable binding paths and hashes are checked against
the complete material reports and saved exports. Exact final check count is in
`evidence/textures48D/tests.json` and the validation record.

Supported PST scope is deliberately bounded: revision 1, flags 0, type 1,
one through seven ascending mip levels, power-of-two dimensions 16..1024,
and the observed zero mip header words. Unsupported inputs fail explicitly.
`decode_texture(blob)`, `parse_material(blob)`, and `rrtex_bytes(surface)` are
importable, with clear format errors. The offline batch runner accepts `--disc`
and `--output`; it has no hardcoded drive path. This is a foundation for the
planned built-in PC importer and validated cache, not acceptance of the full
import UI, every asset format, or Xbox packaging.

A read-only search of 533 saved index dumps found no exact selected-mip hash
match. A broader VRAM search was stopped without a completed result. Therefore
48D makes no claim of matched in-game texture captures. Its evidence is original
instruction/data provenance plus an independent layout differential, not an
executed original loader fixture or a final visual comparison. Frozen 48B/48C
sources and manifests are checked unchanged during the 48D freeze.
