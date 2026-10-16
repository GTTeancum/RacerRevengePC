# Original RA/RC geometry admission — source-only handoff, 2026-10-14

Base: c06a1b0, isolated branch helper/ryloth-geometry. No builds, tests, syntax checks, project executables, guest execution, or gameplay checks were run. Only original archive decompression, field inventory, source inspection, and patch preparation were performed. Native conversion acceptance remains unmeasured.

## Original evidence

Original assets were read only from D:/Programming/GitHub/RacerRevengePC/work/native/project/disc/TRACKS.

| Archive | Bytes | SHA-256 | Entries | PSG | PSM | Decoded bytes |
| --- | ---: | --- | ---: | ---: | ---: | ---: |
| RA.RES | 4604561 | 1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2 | 1010 | 710 | 50 | 29687808 |
| RC.RES | 4655141 | ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b | 1121 | 912 | 46 | 31703040 |

All 710 RA and 912 RC PSGs have one node. Every scene material reference resolves to an actual PSM in its archive. No scene flattening is needed for this data set.

RA has 583 PSGs containing V4_32 position groups: 580 color-bearing PrelitCompressed scenes, plus loft01, loft02 and smokenull01, whose actual black PSM selects DiffuseCompressed and whose surfaces omit original colors (00ff0107). Together these scenes refer to 39 exact PSMs. Their quantized position words range from -8388607 to 8388607, inside the existing signed24 guard. All observed group bone words are -16/+16. There are 7031 V4_32 groups and 1434 V4_16 groups across RA. Wide groups reference PreLitCompressed (6508), PrelitCompressed (453), or DiffuseCompressed (70); spelling is preserved in the inventory.

RC has 14995 V4_16 groups and no V4_32 groups. ubershroom4_col uses the one-node 00ff0107 no-color surface form with PreLitCompressed. Across all original groups, position/normal/UV/color command words match the address/count forms already handled in import_scene48d.cpp; group header command, terminal word, and rigid bone words also match. This is static source-data compatibility evidence, not a claim that the complete importer accepts these scenes: topology, bounds, material parser behavior, serialization and runtime integration still belong to root's verification.

## Change and rationale

The current decoder already has allow_signed32_quantized_positions and the one-node 00ff0107 variant. No change to src/assets/import_scene48d.cpp or .h is warranted by this inspection. Historical unsupported counts cannot be reused as current acceptance results.

The new ra_compressed_scene_admission.h follows the existing MA admission design in a separate namespace. It authenticates exact RA.RES bytes, archive extents, 583 exact PSG identities and their 39 exact PSM dependencies, and requires actual single-pass PrelitCompressed or DiffuseCompressed material selection. Only authenticated, pinned source scenes get the existing signed32 opt-in. Nonmembers and unauthenticated callers retain default limits. Altered pinned bytes fail closed. The decoder retains all existing command, signed24, bone, topology, bounds and palette guards. Missing original colors remain absent; no authored white or mesh substitutes are added. RC needs no new opt-in.

The separate header deliberately repeats the existing small admission primitives instead of refactoring shared MA code during parallel integration. No existing tracked source was edited.

## Root integration contract

Include tools/assets/original_roster/ra_compressed_scene_admission.h in the chosen cold-preparation importer. Construct ra_compressed_scene::Admission, call authenticate(originalArchiveBytes, decodedArchive) before publishing outputs, then pass limitsFor(originalEntry, extractedBytes) to decodeScene48D as its SceneLimits48D argument. Use only this exact RA archive. Keep RC on default limits. Full native conversion and regression checks are root-owned and remain outstanding.

This patch does not modify the importer CLI, native-import/import.json, RRSCENE/RRMESH serialization, materials, textures, or mip outputs. The inspected base tools/assets/original_roster/import_original_pods.cpp lacks the cloud assembler's per-member source_sha256 field; if root uses that older producer, it must retain/add the exact raw member hash in its integration. The supplied source inventory includes each PSG/PSM byte count and SHA-256, per-surface material/program bindings and position format counts. It must not be substituted for native-import/import.json or treated as imported geometry.

## Exact patch file list

- tools/assets/original_roster/ra_compressed_scene_admission.h
- tools/assets/original_roster/ra-rc-geometry-source-inventory.json
- tools/assets/original_roster/README-ra-rc-geometry-20261014.md

## Additional local evidence

All additional files remain under D:/Programming/GitHub/RacerRevengePC/codex-local/helpers-20261014/ryloth-geometry:

- inspect_original_geometry.py: independent static RES decompression and PSG/PSM field inventory; imports no project decoder.
- RA-geometry-inventory.json and RC-geometry-inventory.json: full per-group command words, offsets, position extrema and bone word sets.
- summary.json: archive and format counts.
- summarize_geometry.py: data summaries used during review.
- prepare_admission.py: deterministic source-pin/compact-inventory generation from the inventories and the base MA header.

The source-thread messaging tool could not find the provided parent thread ID, so direct progress delivery was unavailable. This handoff is intended for automatic parent collection. No central-owner files or dirty checkout state were changed.
