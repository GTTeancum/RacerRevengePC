# Native recovery map: starting evidence and unknowns

2026-10-03. Consolidated from three read-only agents and a parent inspection.
All paths are relative to `D:/Programming/GitHub/RacerRevengePC`. These findings
support [the implementation plan](native-pc-plan48a.md); they do not establish a
working native engine. Detailed member hashes and extracted samples are in
`evidence/native-plan48A/assessment.json`.

## Original assets

The existing `work/native/project/analysis/resource-indexes10.json` records
103 indexed RES containers, 34,128 members and zero rejected indexes. Index
acceptance is not proof that every asset schema has been decoded. The bounded
decoder in `work/native/project/scripts/validate_ui_resources07.py:13` reads
RES version 3 / codec 0x340, decompresses 24,576-byte zlib blocks, and checks
index bounds, block boundaries and complete compressed consumption.

Parent inspection independently decoded TA, POD01 and PODPHYS into memory and
hashed each member. TA SHA256 is
`b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`;
POD01 is `08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`;
PODPHYS is `8361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8`.

| Original member | Logical offset / bytes | Use |
|---|---|---|
| TA `geometry\zone01groundstrips16.psg` | 8,527,872 / 1,552 | First ground mesh, one hierarchy object, material ta_sand8. |
| TA `textures\ta_sand8.psm` | 9,560,064 / 261 | Plain material INI: one PreLitCompressed pass and TextureMap_1=ta_sand8. |
| TA `textures\ta_sand8.pst` | 11,599,872 / 88,412 | First paletted texture; earlier revision than Cars reference. |
| TA `geometry\ta_bigrocka.psg` | 393,216 / 2,016 | Alternate local prop; material ta_rock2. |
| POD01 `pods\pod01\englt1b.psg` | 1,966,080 / 6,660 | Two-object engine/damage component; not an assembled pod. |
| POD01 `pods\pod01\englt1.psg` | 1,351,680 / 59,504 | Full left engine, 14 objects. Chariot/rider is a later, more complex case. |
| TA `tracks\ta.tob` | 0 / 13,887 | Scene/placement lead, BINARY identifier and version 1; semantics unvalidated. |
| TA `tracks\ta.scb` | 73,728 / 130,511 | Scene configuration lead, BINARY/version 1, environment/fog/camera strings. |
| TA `geometry\Zone9_COL_floor.col` | 8,577,024 / 28,226 | Collision lead, header 1234, col identifier, version 1; topology/tree semantics unvalidated. |

Ground mesh SHA256:
`34135fefe81be7a98797d64cb3068feebdd7ee063cd5ccd34c42fb635dc23e4e`.
Material SHA256:
`d31a59956f5792c58740a72905c229cabb9f6e4c06e36b7f174a55c77cb3953a`.
Texture SHA256:
`d43ee9e14e4bf4033248b298b4ff55f428394c77cf562a7b90f2ed22e849f63a`.

### What the later exporter explains, and what it does not

In `third_party/cars-tools-reference/CarsTools/SLTConvert/DLL/SLTConvertDLL`:

- `BaseConvert.cpp:955–992` describes a hierarchy record with 128-byte name, 64-byte matrix, center, half bounds, radius and parent. The first ground sample's candidate offsets agree: name +12, matrix +140, bounds +204 and material count +236.
- `BaseConvert.cpp:654–746` explains inverse quantization scales and center. On this sample, candidate position scale at +304 is 0.2085168958, normal scale is about 1/127, and UV scales are about 1/2047. Quantization center +320 differs from the earlier bounds center. Their coordinate relationship needs actual loader validation; do not combine them by intuition.
- The sample's word at +364 is 0x4a0, exactly its remaining 1,184 bytes from +368. Packed upload commands occur there. This is a stream-decoding lead, not a validated vertex/index buffer.
- `PSGConvert.cpp:89–117` writes the observed material INI schema. At line 22 it declares PSG version 5; the 243 inspected original geometry files use version 3. Header versions differ even where interior conventions correspond.

The sand PST starts with revision 1, type 1, mip count 5. A candidate palette
of 1,024 bytes starts at +12. Five 16-byte headers at 1,036 / 1,308 / 2,348 /
6,460 / 22,860 have square dimensions 16 / 32 / 64 / 128 / 256; width*height
payloads account for all 88,412 bytes. Their last two words are zero. Palette
order, swizzling, channel/alpha convention and mip meaning still need verification.
Cars `Streaming/StreamingTexture/StreamingTextureDLL/defines.h:6` uses revision
2; `StreamingTextureUtility.cpp:676–703` orders fields differently and reads a
SizeBytes value. Applying it literally to this revision-1 asset is incorrect.

Other tools to reuse as evidence: `work/inventory-track-ipu26.py:15` for ipum
texture headers, `work/decode-track27.py:19` for offline IPU reference decoding,
and the preserved GS CPU backend for palette/swizzle algorithms. These are
analysis aids, not dependencies of the future native runtime. Sky, placements,
visibility, collision and pod assembly remain separate recovery tasks.

## Simulation and camera anchors

ELF `work/native/project/disc/SLUS_202.68` SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The literal `PodVehicle::RunSimulation` is at file offset 0x2B2D20, virtual
address 0x3B2CA0, using the ELF load mapping. It is referenced inside guest
function 0x219810–0x21A000.

Generated files below are in `work/native/project/generated`:

| File / lines | Evidence |
|---|---|
| `FUN_00219810_0x219810.cpp:568–582` | Arguments a0→s3, f12→f20, a1→s2. Object, time-like float and flag are candidates pending observation. |
| Same file:656–666 and 869–877 | References the RunSimulation string, apparently for a profiling region. |
| `entry_001a8060_0x1a8060.cpp:1331–1455` | Initializes float 60.0 at 0x434188; n=FPU_CVT_W_S(f20*60)+1, then divides f20 by float(n). Preserve exact conversion semantics; domain/units need live tracing. |
| Same file:1515–1655, 1788–1814 | Iterates vehicle updates and repeats substeps. First step flag 1, later 0. Direct calls at 0x1A83FC and 0x1A8460. |
| `FUN_001af330_0x1af330.cpp:429` onward | Vehicle getter, backed by PodRacers::GetVehicle error text; reads an indexed pointer table through object+0x44. Caller skips field+0x40 states 3/4. |
| `FUN_001a0570_0x1a0570.cpp:82–160` | Player creation associates an input object with +0xFF0 and selection with +0xFFC; joystick-related diagnostic supports identity. Actions/filter semantics unresolved. |
| `FUN_00210770_0x210770.cpp:846` | Vehicle creation references failure opening the pod's PHY file. Bridge from configuration to object fields. |
| `FUN_00216a30_0x216a30.cpp` | Reset-above-ground diagnostic around guest 0x216F44/4C; useful reset/collision-query anchor. |
| `FUN_0017a050_0x17a050.cpp` | Physics-body association for PodFollowCamera around guest 0x17A2A4/AC; not a proven camera update function. |
| `FUN_001a9240_0x1a9240.cpp` | LoadCameras in PodRaceEvent diagnostic near 0x1A9358/60. |

The caller belongs to discontiguous `FUN_001a7b40`; see
`work/native/project/analysis/function-ranges.csv:2150`. Looking only in the
first generated function file misses its later simulation calls.

Candidate fields inside RunSimulation, explicitly not accepted semantic names:

- Object +0x340 is a component updated through virtual slot +0x3C beside a PodBody label. Component pointers +0x344 through +0x35C have additional ordered updates.
- Object +0x190 leads to an associated object receiving a 64-byte block at +0x100 and a subsequent copy to +0xC0 (`FUN_00219810:3604–3746`); possible transforms, conventions unresolved.
- Object +0xC70 is copied to the body candidate +0x70; the norm of three floats is written to body +0x34 (`:3746–3806`). This is consistent with velocity/speed, not proof of field identity.
- Lists at +0x384/+0x388, +0x488/+0x48C and +0x58C/+0x590 are merged around calls to 0x122350. Collision ownership/contact semantics remain unknown.

Trace genuine guest completion at 0x219FF8 or caller continuations 0x1A8404 /
0x1A8468, paired with thread, saved SP/RA and call/substep ID. Existing host
return observers may see dispatcher yields. A dependency-complete fixture
must include configuration, native input, collision dependencies, random state
and external events. Unknown reads are blockers, not zeros. Temporary recorded
query answers may isolate a diagnostic, but cannot qualify as playable native
collision or gameplay.

## Platform assessment

Use a new Win32/D3D11 PC backend. Installed SDK headers/import libraries include
x86 support; x64 can be the main desktop development build while the shared
game and asset code is checked on Win32. Device creation and frame output are
not yet tested. Raylib 5.5 is an available fallback in the reference dependency
tree (`work/native/sources/offline-sources.cmake:8`).

The existing GS interface takes VRAM, registers, GIF data and PS2 primitives
(`work/native/sources/src/PS2Recomp/ps2xRuntime/include/runtime/gs/gs_backend.h:14`).
It must not become the native renderer interface. New scene input uses semantic
meshes, materials, textures, transforms and camera state. Existing GS DLL loading
also means imports alone cannot prove independence: inspect linker maps, loaded
modules, file inputs and clean-package execution.

Observed host: Ryzen 5 5500U (6 cores / 12 threads), Windows 11 Pro x64,
10.0.26200, Radeon integrated graphics driver 31.0.12027.9001, roughly 11.9 GiB
visible RAM. This is inventory, not a benchmark. Do not interpret nominal CPU
frequency as sustained speed or WMI AdapterRAM as a reliable VRAM budget.

Keep shared logic single-thread-correct, pointer-width-neutral and bounded in
allocation. Xbox-specific GPU formats, renderer and shared-memory accounting
come later. Existing nxdk diagnostic builds and ISA checks are useful tooling;
they do not establish whole-game Xbox memory fit, execution or FPS.
