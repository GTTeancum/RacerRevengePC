# Native PST1 and authored PSM conversion 48E

The standalone C++ decoder ports the frozen 48D source-texture interpretation
and material parser into the native importer. It preserves original ascending
mip dimensions, source offsets, the raw palette, complete RGBA pixels, and every
authored material field. It does not implement or guess a native shader from a
PS2 microprogram name. Final shader and blend fidelity remain separate work.

## Public contract

`src/assets/import_texture48e.h` exposes three transactional operations using
the existing `DecodeError48C` and optional `DecodeHooks48C` callbacks:

- `decodeTexture48E(bytes, texture, error, hooks, limits)` returns `Texture48E`.
  `mips` retains the original smallest-to-largest order; each `TextureMip48E`
  contains width, height, source header/index offsets, and tightly packed RGBA.
  `original_palette` retains all 256 original four-byte entries. `base_rrtex`
  serializes the largest source mip under the RRTEX001 contract.
- `encodeTextureLevel48E(mip, bytes, error)` serializes any retained level as
  RRTEX001. Dimensions and exact RGBA extent are checked before committing.
- `decodeMaterial48E(bytes, material, error, hooks, maxBytes)` returns ordered
  one-based passes, original program names, and one-based texture name/slot
  bindings. General/pass `authored` fields retain nonempty strings, while
  `unspecified` retains explicit blank fields and their source offsets.

Program names and texture names retain original capitalization. Neither the
parser nor decoder opens files, uses JSON, performs texture lookup, executes
guest code, invokes GS upload logic, or depends on a platform API. The sole
shared project header provides data/error/callback types; the implementation
requires only the C++ standard library. Test code uses JSON and Windows SHA256
for independent saved-reference comparisons.

Successful calls clear earlier errors. Failed or cancelled calls leave the
entire caller output unchanged. Synchronous callback rules are inherited from
`DecodeHooks48C`: callbacks must not throw, mutate input/output, or reenter the
decode. Cancellation is checked before parsing, during texture rows, at every
mip/pass boundary, and before output commit.

## Proven interpretation and strict boundaries

48D original loader/upload/TEX0 instructions and the supplied PCSX2 layout
oracle establish the conversion: source rows stay linear and palette selection
swaps index bits 3 and 4. The original byte order, RGB values, and alpha values
are preserved exactly. No premultiplication, alpha rescaling, RGB doubling,
vertical flip, resampling, or generated content is applied. See
`docs/texture-material48d.md` and `evidence/textures48D/source-evidence/anchors.json`
for original address anchors and hashes; those files remain frozen.

The native decoder accepts the same bounded PST subset as frozen Python48D:
revision 1, flags 0, kind 1, 256 palette entries, one through seven mip levels,
power-of-two width and height in 16..1024, observed zero mip words, and no
unconsumed suffix. Both dimensions must double at each following mip. All mip
headers and source extents are validated before allocating pixels. Input is
capped at 8 MiB, and retained RGBA plus base RRTEX at 16 MiB; caller limits may
reduce these hard caps but cannot increase them.

PSM input is capped at 64 KiB and must be ASCII without NUL. The supported
grammar requires General plus one through eight numbered ShaderPass sections,
unique case-insensitive keys within each section, a program identifier, and one
or two texture maps per pass. Texture basenames accept ASCII letters, digits,
underscore, and hyphen, without path separators. Unknown authored fields are
retained as strings; unsupported layouts return named errors. The one default
applied is the original loader's missing `totalTextureMaps=1`, marked by
`texture_count_defaulted`; a present blank count fails. No default render states
are supplied for other missing or blank fields.

`DecodeError48C` distinguishes malformed input, unsupported subsets, byte limits,
and cancellation, with code, source offset, and detail. Rejected original files
are outside this recovered subset; rejection does not assert that the original
game considers the file corrupt.

## Full-disc reference coverage

`evidence/importTexture48E/build_reference.py` builds a test-only reference from
all 103 RES containers under the supplied extracted disc. It uses the unchanged
original RES reference decoder plus frozen Python48D PST/PSM functions. Source,
container, member, native mip, and RGBA hashes are preserved in
`reference-index.json`; shared source blobs are deduplicated by SHA256 while
every container occurrence remains listed.

| Type | Accepted occurrences | Rejected occurrences | Accepted unique blobs | Rejected unique blobs |
| --- | ---: | ---: | ---: | ---: |
| PST | 3,850 | 1,431 | 881 | 536 |
| PSM | 6,107 | 341 | 2,237 | 85 |

The accepted texture occurrences contain 11,317 mip surfaces and 124,658,432
RGBA texels. Native tests compare every RGBA and RRTEX SHA256, source dimensions
and offsets, and every material field/pass/program/slot against frozen Python.
Every original container/member SHA256 is checked before comparison. Accepted
material declarations can still refer to unsupported textures or shader effects;
their parsing success is not proof that a full material can be rendered.

| Native rejection code | Original occurrences |
| --- | ---: |
| pst.kind | 1,064 |
| pst.dimension_subset | 299 |
| pst.non_power_of_two | 68 |
| psm.map_count | 134 |
| psm.sections | 144 |
| psm.texture_name | 48 |
| psm.duplicate_key | 14 |
| psm.default_parameters | 1 |

Distinct program capitalization is retained, including `PreLitCompressed`,
`PrelitCompressed`, and `prelitCompressed`; the complete frequency table is in
`coverage-summary.json`. These declarations must not be treated as known native
shader implementations merely because a similarly named program is supported.

The first integrated scan exposed an error-classification issue: complete
original alternate layouts were being labeled malformed, which correctly caused
the import service to stop. Inspection of every rejected original is preserved
in `original-rejection-structure.json`, with representative original bytes and
hashes in `unsupported-originals/`. The targeted correction preserves the
accepted subset while reporting these complete alternate layouts as unsupported:

- `AppStart\DefaultMT.psm` uses the original `DefaultShaderParams` schema.
  Its SHA256 is `1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7`.
- Fourteen original PSM occurrences repeat authored keys. The decoder completes
  lexical validation before reporting their unproved precedence semantics;
  it neither chooses a duplicate value nor ignores later malformed syntax.
- 134 materials retain extra inactive ShaderPass_2/3 sections beyond the declared
  first pass, and ten use Prologue/Epilogue sections. Required declared passes
  must still exist; arbitrary unexpected sections or missing passes remain fatal.
- All 68 non-power-of-two PST occurrences are complete 640x448 UI images, with
  exact source extents. All 299 small-mip cases begin with a complete 16x8 level.
  The decoder checks every known-kind1 mip extent and suffix before returning
  unsupported for these dimensions. Truncation and zero dimensions remain fatal.

The full-disc native test now requires every rejected original member to carry
the nonfatal `unsupported` classification. Separate synthetic tests require
malformed syntax/counts, missing required passes, all proper PST prefixes, and
truncated unsupported-size images to remain `malformed`. Limits and cancellation
retain their distinct classes. No generic ignore-malformed policy is introduced.

## Validation and integration

`tests/native/import_texture48e.cpp` checks the entire source corpus above plus
all 1,308 proper prefixes of a valid PST fixture, bad headers/dimensions/counts,
unsupported layouts, output allocation limits, trailing bytes, mip ordering,
49 supported dimension combinations, independent frozen GS palette mapping,
all raw alpha values, export size checks, material ambiguity/encoding/path
rejection, unspecified field preservation, and transactional cancellation.

The final x64 and x86 builds each pass **256,233 checks with zero failures**.
Their texture/material reports are identical after removing only the architecture
bit count. Both reports preserve all 1,772 explicit original-member rejection
records. They are copied into `evidence/importTexture48E/accepted-x64.json` and
`accepted-x86.json`; source/binary provenance and the final manifest accompany
them. The parent build record is
`evidence/nativePc48E-build-20261003-141953-482/`.

The preliminary x64 build had passed 254,444 checks before the integration
classification correction. Those earlier results do not test the final skip/
fatal policy. A test-only invalid-ASCII constant warning was also corrected;
the final build source hashes identify the tested versions precisely.

Parent-owned CMake integration links the production source into the existing
native import decoder library. The test target adds `third_party/native-json`
and links `bcrypt` only for testing. Its invocation is:

```text
racer_native_texture_check PROJECT_ROOT [REPORT_JSON]
```

The optional report contains every named rejection with original container,
member SHA256, offset, and reason. Standard output contains compact totals.
The Python reference can be reproduced from the repository root with:

```text
python -B evidence/importTexture48E/build_reference.py
```

The parent integrates per-mip native cache objects and material metadata, source
selection/import progress, cache migration, named resource resolution, and
renderer binding. These are separate from this standalone decoder's acceptance.
Using only the first pass and first texture slot is a labeled base-color preview;
it does not reproduce dual specular passes, windows, animated flames, GS alpha
arithmetic, mip choice, lighting, or final sampling behavior. Xbox packaging and
gameplay performance are also outside these conversion tests.
