# CarsTools source reference intake

Checkpoint date: 2026-10-03. Purpose: knowledge for the shared native x86 PC/original-Xbox port. No supplied tools were built or executed, and no runtime code was replaced.

## Provenance and preservation

User supplied https://www.mediafire.com/file/pa379eq09066rbb/CarsTools.rar/file and described it as later, same-engine Pixar Cars tooling received from an Angel Studios developer. Preserve that account as user-provided provenance, not independently verified engine lineage. Inspected converter files carry 2002 Rainbow Multimedia Group copyright headers. Compatibility with Racer Revenge remains unproven.

Original: `evidence/cars-tools-intake/CarsTools.rar`.
SHA256: `3c5d77c0be4d4081d10b9d44bffa693fc040a10eea1d6564f537fa567fd941b4`.
Reference tree: `third_party/cars-tools-reference/CarsTools`.

Windows tar listed the archive successfully. The initial whole-archive extraction reported CRC/decompression/filter errors on bundled object files, debug symbols, precompiled headers and IDE databases. This is not proof that the download is corrupt. A subsequent extraction restricted to source/configuration/documentation members exited 0 with an empty error log. Do not describe the entire extracted tree as validated. The selected member list, extraction log/exit and per-file SHA256 manifest are retained in the intake evidence directory. The original RAR is the recoverable authority for all other members.

## Concrete findings

Paths below are relative to the reference tree; line numbers refer to the preserved source.

| Source | Observation | Relevance |
|---|---|---|
| `SLTConvert/DLL/SLTConvertDLL/SLTConvert.cpp:323` | Platform factory selects DXGConvert for original Xbox, XNGConvert for Xbox 360, PSGConvert for PlayStation, GCGConvert for GameCube. | Separate original-Xbox and PS2 asset output paths already exist in this reference. XNG is not the original-Xbox path. |
| `SLTConvert/DLL/SLTConvertDLL/BaseConvert.h:48` | Shared conversion interface reads materials, components, vertices, indices, skinning and hierarchy from SLT data. | A guide to semantic geometry independent of PS2 packets; not a PS2-to-Xbox converter for already cooked game files. |
| `SLTConvert/DLL/SLTConvertDLL/PSGConvert.cpp:22` | PSG version 5; writes four-byte psg/pgi/pss identifiers, version, surface metadata and DMA payloads. | Candidate schema to compare against original disc bytes; never assume version 5 applies to Racer Revenge. |
| `SLTConvert/DLL/SLTConvertDLL/DXGConvert.cpp:13` | DXG version 5 and .dxg output; writes vertex components and index data. | Reference for how equivalent source geometry becomes native Xbox-oriented assets. |
| `SLTConvert/DLL/SLTConvertDLL/BaseConvert.cpp:654` | Position quantization uses bounding-cube center and half extent; writes inverse scales and center. UV scaling reserves tiling bits and has separate 8-bit and 24-bit cases. | Useful hypotheses for position/UV reconstruction and surface errors. A generic normalized-integer decoder would miss these conventions. |
| `SLTConvert/DLL/SLTConvertDLL/DXGConvert.cpp:850` | Quantized positions subtract BoundingCubeCenter before multiplying XYZQuantizationScale. | Expected reconstruction is quantized value / scale + center, subject to the actual format variant. |
| `SLTConvert/DLL/SLTConvertDLL/Defs.h:24` | Optional coordinate-system conversion negates Z through a matrix scale. | Coordinate handedness and triangle winding require explicit validation; the macro does not establish that conversion is always enabled. |
| `Streaming/StreamingMotionPack/StreamingHeaders.h:15` | Packager version 0x0101; Xbox alignment 128, PS2 64; geometry .dxg/.dgs versus .psg/.pgs, textures .dxt/.dts versus .pst/.pts. | Header/payload separation and alignment provide concrete checks for streamed asset recovery. |

Additional areas inventoried include track/level/object Max exporters, material definitions, collision streaming, motion packs and particle tooling. Their presence is not evidence that their formats match the older game.

## Application to the native port

Next asset investigation should compare original Racer Revenge resource names, magic values, versions, counts and offsets against these definitions, then trace the game's loader for differences. Validate one real static mesh's positions, indices, UVs, materials and bounds against captured rendering before expanding to tracks. Use captures as correctness evidence, not as the native game's content source or packet playback engine.

The desired runtime consumes semantic meshes, materials, transforms and gameplay state through native PC/Xbox interfaces. This reference can explain how to recover those meanings; it does not supply a demonstrated compatible game runtime, solve physics recovery, prove engine lineage, or establish Xbox performance. No graphics bug fix or FPS improvement is claimed by this intake.

No third-party source has been copied into the project's native implementation. The new reference remains separate from build targets.

## Subsequent comparison

The [48A recovery map](native-recovery-map48a.md) now compares actual TA/POD01
assets: their PSG version 3 differs from the supplied version-5 exporter, and
the sand texture's PST revision 1 differs from the later revision-2 layout.
Material fields and some hierarchy/quantization conventions correspond. Those
observations narrow the decoding work but do not establish complete compatibility.
