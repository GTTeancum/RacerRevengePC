# Authenticated RA TGA cold importer bridge
Source-only patch. No builds, tests, syntax/import checks, converter runs, generated assets, project executable runs or guest instruction execution.
Base importer: Library libfile_cfead395aafc8191ba34dd8d801b3ffd,10366 bytes. It matched the local importer byte content; no baseline replacement commit was needed.
Dependency: shared OriginalTgaConversion50LA, reviewed/integrated by root as bf2de81 (helper commit302c09a).

## Exact owned production files
- tools/assets/original_roster/import_original_pods.cpp
- tools/assets/original_roster/ra_tga_import.h (new)
No assembler, renderer, runtime admission, public canonical header, shared conversion header or canonical resource CPP edits.

## Sampling proof established before bridge implementation
RGB5 expands by left shift3 into high bits (0..248), without bit replication or255/31 scaling. Existing source-backed original_ui_controls_specmap50kg.cpp documents this sampling rule; original-racer-source-color50la.md explicitly distinguishes1555 mip coverage from sampled alpha.
Actual original RA PSMs:
- textures\\metalgrate.psm:263 bytes,SHA256 f3d25acf8b10db9b805ff177dc65db73d851f01859ee7ff5a3678f6fafeebfcc.
- textures\\twidesigns02.psm:265 bytes,SHA256 675453947b6b308444b9fe98d0bafeb848b38d4c7eee52ab1cb677afab12493c.
Both are one-pass PreLitCompressed with TextureMap_1 and no authored TEXA overrides.
Original APPSTART.RES:58977 bytes,SHA25604eb376a507d0745925e4351903a8ef7a1024d37d8250b78892cba0c74220b89.
AppStart\\DefaultMT.psm:82 bytes,SHA2561907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7. Its only fields are microProgramName=DiffuseCompressed and alphaReferenceValue=0; no TEXA overrides.
Original24A8F4 loads255;24A8FC/24A900 store TA0/TA1;24A904 stores AEM0. Standard pass getters24798C/2479A4/2479BC inherit these when absent. Original TGA constructor clearsF8bit0;249818..24983C conditionally replaces TEXA only when that bit is set. Canonical original_standard_material_table50la.cpp preserves this branch.
Therefore MetalGrate/TwiDesigns02 sampled alpha is255 for BOTH coverage values, including source-key magenta and a zero packed word from coverage rejection. Writing coverage?255:0 would be wrong for these proven consumers. Packed words retain bit15 throughout original mip generation; sampled RGBA is derived separately at each level. This is a fixed interpretation of the authenticated original materials, not a general1555 sampler.
LargeFlag is8888: preserve every alpha byte and the original168470 bottom-right contribution quirk. No TEXA mapping, normalization or premultiplication applied to its RGBA.
Full earlier conversion proof: Library libfile_5fc2e13dbe688191a4c668dd85de63bd. No hardware pixel-parity or gameplay claim.

## Admission and preservation
Only --authenticated-ra-compressed triggers the new preflight and conversion branch. Existing RA geometry authentication remains first. New preflight, before output directory creation, rechecks RA byte count/hash,1010 entries, and exactly one member under each original spelling with directory size, extracted size/hash and shared header shape. Missing/duplicate/mismatched required members fail, rather than downgrade to retained source.
Exact pins:
textures\\LargeFlag.tga,65580,352286ac266f59e3d06531cfab7a67ce8ad9e7355fe544ba932550278abd613d,128x128 BGRA32.
textures\\MetalGrate.tga,196652,20cd5335e1d870295983807467defecbf2c45dd1ee1ceeaa10c8bfd2e8e85fa9,256x256 BGR24.
textures\\TwiDesigns02.tga,49196,693031b2ed08b5437c250ea875eb9e8f6f0444b858530d7a0e0c1737630aba15,128x128 BGR24.
The existing RA all-member retention loop and raw source-file writes remain intact, including all other TGA sources. Default/MA selection and PST branches are unchanged.
Conversion uses shared read24/read32,1555 conversion and sequential mip1555/mip8888. Mips are generated downward from each immediate predecessor in original storage, then the list reverses for publication. No RRTEX data was produced during this task.

## Exact metadata contract
Existing source_member,source_index,logical_offset,source_bytes,source_sha256,source_file and kind remain; kind is "tga".
For exactly the three converted members:
- status: "converted_texture"
- source_format: "tga_bgr24" for MetalGrate/TwiDesigns02; "tga_bgra32" for LargeFlag
- converter: "rr-original-tga-50LA-v1"
- width,height: original base dimensions128 or256
- mips:4 for128,5 for256
- native_mips: ascending dimensions16,32,64,128[,256]; zero-based consecutive index
- each native_mips entry: index,width,height,rrtex,rrtex_bytes,rrtex_sha256
- rrtex filenames: "<source_index>-mip<index>.rrtex"
- top-level rrtex equals native_mips.back().rrtex, the largest/base mip
- top-level rrtex_bytes and rrtex_sha256 equal that largest entry
- NO texture_kind is fabricated: original TGA source format is separate from PST kinds.
Every level is serialized by encodeTextureLevel48E. rrtex_bytes and rrtex_sha256 derive from the actual encoded bytes immediately before write at future execution. No predicted native identity is supplied in this handoff.
RRTEX001 itself contains magic,width,height and unpremultiplied RGBA only; existing encodeTextureLevel48E performs no shader/alpha normalization.
Generated mips are not represented as authored archive levels: TextureMip48E source offsets are absent sentinels for generated levels, with0/18 only on the base. These offsets are not emitted as invented PST metadata.

Root owns the assembler join, output-byte authentication and all execution/validation. Apply only the enclosed patch; earlier helper history is not part of this change.

