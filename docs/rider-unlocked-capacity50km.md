# Default-unlocked Rider source-frame capacity

Source/header review only, 2026-10-12. No decoder, compiler, tests, gameplay, or runtime verification was performed for this derivation.

## Bound and admission

The sealed source-RGBA slot capacity is 20,480. The largest reviewed full-frame upper bound is Rider12: 19,120 model slots plus the existing 1,033-slot static UI/helper allowance = 20,153. Main/Options remain at 8,192. Existing per-surface CPU limits of 8,192 vertices and 8,192 triangles need no expansion: the largest new surface has 7,047 vertices and a 5,437-triangle upper bound.

Only the final selected pod/character pair contributes model geometry. The actual deselect callback always unloads the character, and always sets the pod hidden even if CleanFromMemory preserves its graph. The collector rejects unattached/hidden models before binder traversal. Intermediate movement callbacks retain their original order; they do not render intermediate pairs together.

| Rider | Pod weighted slots, upper bound | Character weighted slots, upper bound | Full frame including 1,033 | Maximum surface vertices | Maximum surface triangle bound |
|---|---:|---:|---:|---:|---:|
| 02 | 14,936 | 3,070 | 19,039 | 5,253 | 3,551 |
| 04 | 6,770 | 2,155 | 9,958 | 2,338 | 1,682 |
| 10 | 6,500 | 2,252 | 9,785 | 2,827 | 2,049 |
| 11 | 8,004 | 2,072 | 11,109 | 2,916 | 2,032 |
| 12 | 13,722 | 5,398 | 20,153 | 7,047 | 5,437 |
| 15 | 11,316 | 2,159 | 14,508 | 4,159 | 2,893 |
| 19 | Existing model-slot total: 15,956 | Included in preceding total | Existing bound: 16,989 | Existing admission retained | Existing admission retained |
| 20 | 12,647 | 3,136 | 16,816 | 5,383 | 4,173 |

These are conservative header-derived slot bounds, not claims about exact rendered or decoded triangle counts. No truncation, pass removal, mesh replacement, or geometry simplification is introduced.

## Raw evidence and method

Original inputs were extracted by the root task and read with `od` at explicit header offsets and plain-text PSM reads. Whole-member identities are recorded in:

- `/workspace/shared/unlocked-rider-original-models-20261012/manifest.json` (14 PSGs and seven MCFs)
- `/workspace/shared/unlocked-rider-original-inputs-20261012/manifest.json` (new-rider PSM text)
- `/workspace/shared/unlocked-rider-original-materials-extra-20261012/manifest.json` (cable and Rider20 PSMs)
- Repository identity source: `src/assets/original_ui_archive_catalog50kg.inc`

PSG v3 layout is documented by the existing reader in `src/assets/import_scene48d.cpp`: node count at byte8; 224 bytes/node starting at12; material count followed by64 bytes/name;32 bytes quantization/world; LOD count; each LOD has an8-byte header; each surface has a20-byte header. Surface fields are material index at+0, vertex count V (u16) at+12, source-strip count S (u16) at+14, and payload size at+16. Skipping the payload locates the next header without decoding geometry.

The reader emits `vertices - 2 * decoded_strips` triangles and proves `decoded_strips - continuations == source_strips`. Thus V-2S is a safe upper bound. Multiply each surface by its actual material's uncommented TotalShaderPasses and sum. Duplicate material slots remain separate geometry contributions. All reviewed models have one LOD.

Pod material slots are `[XX_1, XX_2, cable, XX_shdw]`, with pass counts `[2,2,1,1]`. Character slots normally have one pass each; Rider12 slot3 (`12c_4`) has three passes. Rider20 material order is `[20c_1,20c_2,20c_4,20c_3,20c_5]`, all one pass. Commented-out PSM declarations do not count.

## Surface header receipts

Rows below are in original surface order, with `(header byte offset, material index, V, S, payload bytes)`. Original material names above determine the pass weight.

- 02: (1436,0,4734,607,83712), (85168,1,5253,851,93664), (178852,2,928,68,17152), (196024,3,4,1,96)
- 02c: (5852,0,2167,376,38544), (44416,1,1778,294,31664), (76100,2,601,68,10576)
- 04: (988,0,2338,328,41680), (42688,1,2298,397,41168), (83876,2,464,34,8560), (92456,3,4,1,96)
- 04c: (6524,0,1632,311,28928), (35472,1,1102,200,19424), (54916,2,691,124,12272)
- 10: (988,0,2827,389,50336), (51344,1,1506,252,26960), (78324,2,464,34,8560), (86904,3,4,1,96)
- 10c: (5180,0,1127,165,20208), (25408,1,1361,219,24096), (49524,2,662,65,11744)
- 11: (988,0,2916,442,51936), (52944,1,2491,360,44160), (97124,2,464,34,8560), (105704,3,4,1,96)
- 11c: (6300,0,1007,155,18064), (24384,1,1359,232,24160), (48564,2,714,117,12688)
- 12: (988,0,7047,805,126352), (127360,1,1672,223,29776), (157156,2,454,30,8128), (165304,3,4,1,96)
- 12c: (5820,0,1475,197,26400), (32240,1,1534,320,27088), (59348,2,812,110,14560), (73928,3,1088,223,19232), (93180,4,921,167,16400), (109600,5,460,71,8144)
- 15: (988,0,3672,553,65536), (66544,1,4159,633,74528), (141092,2,464,34,8560), (149672,3,4,1,96)
- 15c: (6076,0,1559,227,27792), (33888,1,592,90,10752), (44660,2,976,167,17584)
- 20: (988,0,5383,605,95200), (96208,1,2583,357,45840), (142068,2,669,54,12336), (154424,3,4,1,96)
- 20c: (3516,0,1732,249,30992), (34528,1,1994,313,35600), (70148,2,177,47,3152), (73320,3,443,92,7952), (81292,4,252,30,4576)

Static/helper allowance and Rider19 totals are preserved from the preceding bounded source-RGBA implementation. This capacity change does not establish material-policy readiness, cache availability, or runtime visual acceptance.
