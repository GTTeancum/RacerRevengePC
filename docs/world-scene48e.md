# 48E: original TA static scene placement

The original `tracks\ta.scb` provides a complete static placement list for TA. The offline 48E decoder resolves all 318 authored entries across nine zones: 284 visual instances referencing all 228 original `geometry\*.psg` members, plus 34 collision-only references. Nothing is placed by hand. The resulting neutral all-static LOD0 diagnostic mesh contains 110,196 vertices and 69,838 triangles.

This is evidence and a format recovery for the next native world importer. It is not a running race, faithful material/sky renderer, portal visibility implementation, collision simulation, or gameplay-performance result. Frozen 48D/native sources were not changed by this recovery.

## Reproduce

From the repository root, run these offline evidence tools with the original local disc data present:

```text
python -B tools/assets/decode_world48e.py
python -B tools/assets/test_world48e.py
python -B tools/assets/source_world48e.py
```

The first tool writes only the selected evidence directory (default `evidence/worldScene48E`). The latter tools use that default directory. These tools depend on the retained Python RES/PSG decoders for investigation; a shipping native world importer must port the bounded format and semantic operations, without requiring Python or guest execution. Source-proof collection rejects optimized Python because its evidence assertions must run.

## Original inputs and outputs

| Input | Size / location | SHA-256 |
|---|---|---|
| `work/native/project/disc/TRACKS/TA.RES` | 3,334,562 bytes; decoded stream 13,099,008 bytes | `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8` |
| `tracks\ta.scb` | 130,511 bytes; decoded offset 73,728 | `c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d` |
| `tracks\ta.tob` | 13,887 bytes; decoded offset 0 | `11e4c4909bf866fee626977c970d9907d4f191c29eeff32bddd162f173916f6b` |
| `work/native/project/disc/SLUS_202.68` | Original ELF used for source-word verification | `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30` |
| `ta-static-placed-neutral.rrmesh` | Neutral original placements; 110,196 vertices / 69,838 triangles | `4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7` |

Evidence directory contents:

- `original-ta.scb` and `original-ta.tob`: exact original member bytes.
- `ta-parameter-blocks.json` and `ta-toy-parameter-blocks.json`: every labelled block, original string value, byte offset and length. SCB has 378 blocks; TOB has 44.
- `world.json`: original zones, portal connections and four vertices each, original instance records, file/field provenance, scalar placement matrices, referenced member hashes and offsets, original root-node matrices/bounds, quantization metadata, material names and per-surface spans. It separately records the two unapplied original animations.
- `ta-static-placed-neutral.rrmesh`: combined LOD0 diagnostic geometry with original UV/color values and topology, positioned from original scene entries.
- `original-loader-proof.json`: 510 original instruction words in 11 relevant address ranges, 499 generated instruction comments checked word-for-word against the original ELF, eight verified original strings, complete source hashes and later Cars reference excerpts. Eleven instructions lacked generated comments and are retained directly from original bytes; nine are zero/NOPs. Nonzero missing-comment words retain no invented disassembly.
- `tests.json`: accepted 2,193-check result.
- `hashes.json`: final evidence/source file sizes and hashes, excluding itself.

Computed bounds of the placed original vertices before RRMESH float32 serialization are:

```text
minimum = (-205.32837188616395, 852.2866500969976, -621.323709666729)
maximum = (10623.428818881512, 2476.968355614692, 13043.414020895958)
```

These are derived bounds of the exported static vertices, not a claim that these six values were stored as an original world bounding box. Their coordinates are retained without recentering, arbitrary scaling or an extra source-axis swap.

## Recovered BINARY1 parameter format

All integer fields below are little-endian uint32. TA SCB and TOB use the labelled-block variant:

1. Seven bytes `BINARY\0`, version `1`, block count.
2. For each lowercase, sorted, unique block name: byte length including final NUL, then name bytes.
3. A block table: `(absolute file offset, block size)` per block, eight bytes per entry.
4. Each block begins with its parameter count, then a sorted label table. A label entry is 43 bytes: a 35-byte NUL-padded lowercase name, relative value offset, and value length including final NUL.
5. Value offsets are relative to the start of the label table, four bytes after the block start. Values are ASCII strings. Their physical order follows the original authoring order and need not match the alphabetically sorted label table.
6. The recorded labelled-block size excludes the four-byte parameter count. Its physical end is `start + 4 + size`.

The parser preserves original value order and proves byte-identical reserialization for both originals. It rejects overlaps, gaps, trailing bytes, invalid names and values, out-of-range tables/offsets, unsupported version and unproven data-line blocks. Investigation limits are 2 MiB/file, 8,192 blocks, 1,024 parameters/block, and 512 bytes/name or value. These are supported-subset limits, not claims about every possible engine file. Scene limits are 256 zones, 4,096 portals, 10,000 model records/zone and 100,000 total model records.

Later source corroboration is `third_party/cars-tools-reference/CarsTools/CARSRESOURCEBUILDER/ParameterBlocks.cpp`: format constants at lines 9–15 and `SaveBinary` from line 1846, especially label offsets at 2116 and size subtraction at 2178–2186. The original ELF reader at `0x1578f8..0x157a94` independently reads the seven-byte magic, version 1, variable name table and eight-byte offset table. The later source alone is not treated as proof of old-game compatibility.

## Placement and node semantics

SCB `SceneInfo` declares nine zones and ten portals. Zone model counts are `[148, 21, 6, 6, 29, 3, 30, 14, 61]`. Each `zoneN-modelM` supplies its original `.slt` basename, `position`, `lookvector`, `upvector`, and `uselighting`. All 318 original entries have `uselighting=F`; this is preserved, not interpreted as permission to alter material/color semantics.

Basenames resolve case-insensitively to present `geometry\<basename>.psg` members or, if absent, present `.col` members. No reference remains unresolved. Zone and model numbering must exactly reconcile with the declared counts. Every visual instance currently uses a one-node PSG hierarchy with parent -1 and bone indices zero. The decoder rejects unproven multi-node static props rather than guessing a hierarchy placement rule.

The crucial root-transform behavior is replacement. Applying scene placement to the already bind-flattened 48D mesh would double-transform 42 original visual instances whose saved PSG root matrix is nonidentity. These include crowds, finish droids and bantha meshes. The original geometry hierarchy reader uses the geometry object itself as node zero, and the static scene loader subsequently writes that object's root translation and orientation:

| Original address | Evidence |
|---|---|
| `0x275f54..0x275f6c` | Zone `NumberOfStaticModels` lookup. |
| `0x27b7f0..0x27b844` | Formats `%s-Model%d`, reads that parameter block and its SLT reference. |
| `0x27a304/0x27a320/0x27a33c` | Reads position/look/up into instance parameters +0x110/+0x120/+0x130; concrete reader also appears at `0x27a574/0x27a590/0x27a5ac`. |
| `0x26ed44` -> `0x28cac0` | Geometry hierarchy read. |
| `0x28cb0c` | First node pointer is the geometry object itself. |
| `0x28cb88` | Reads original root matrix to that object's +0xC0 matrix. |
| `0x27adcc` -> `0x28ab90` | Scene position overwrites root translation at object +0xF0/+0xF4/+0xF8. |
| `0x27adec` -> `0x289ec0` | Scene look/up overwrite the root orientation, called with `a3=1,t0=0,t1=0`. |
| `0x289f9c..0x28a038` | Cross products, normalization, and stores of right/up/look/translation matrix rows. |

The relevant generated source names and exact lines are recorded alongside each original instruction in `original-loader-proof.json`, including `FUN_0027abc0_0x27abc0.cpp`, `FUN_00289ec0_0x289ec0.cpp`, and `FUN_0028cac0_0x28cac0.cpp`.

The recovered matrix operation is:

```text
right0 = cross(up, look)
look1  = cross(right0, up)
U      = normalize(up)
L      = normalize(look1)
R      = cross(U, L)

M rows = (R,0), (U,0), (L,0), (position,1)
world  = local_position * M  [row-vector convention]
```

Original decimal vectors are converted to binary32, corroborated by the original vector reader `0x158c00..0x158cf8`. The export computes derived orientation with explicit scalar binary64 operations and `sqrt`. It does not reproduce PS2 VU RSQRT rounding or claim bit-identical runtime vertices. This arithmetic contract is suitable for ordinary native data and has analytical orientation/handedness tests.

Of 284 visual instances, 253 use local quantization and 31 use world quantization. Every world-quantized instance has original zero position, +Z look and +Y up. Quantization origin is already included by the recovered PSG local-position decode; no ad hoc world flag translation is added. The existing world-quantized zero-pose invariant is enforced rather than generalized beyond observed data.

The combined mesh transforms `vertices_local` from each LOD0 surface by the new scene matrix. It preserves source index order, degenerate triangles, UVs, color presence and raw decoded color values. Per-surface original material references, source normal-alignment diagnostics, and node/LOD indices are kept in the world report. Normals are not a field in RRMESH01; a future native semantic world renderer should use the original scene sidecars and this placement, transforming normals consistently without translating them.

## Acceptance and remaining work

The 2,193 checks cover exact SCB/TOB round trips, original record counts, bounded malformed/truncated inputs, count/header mismatches, invalid asset paths, vectors/lighting/portal references, orthonormal placement matrices, analytical identity/90-degree transforms, output extent/hash and per-instance/surface index isolation. The original-loader proof also rejects opcode mismatches. None of these tests substitutes for a runtime matrix comparison or a rendered material/visibility comparison.

All static entries are exported together for inspection. Original portal vertices, connections and per-zone exclusion lists are preserved but not executed. The 34 collision-only entries are reported without synthesized collision triangles. The final SCB model block's additional authoring keys, including default-start values, are preserved without guessing a race spawn from their location in the file.

TOB has two unapplied keyframed animation records: `xsailbarge.mcf` and `x_sab.mcf`, both authored in starting zone 6 with zero offset. Their full source records are separately retained. TOB `staticmodelparams` also refers to three sunbeam models and shadow-receiving attributes; these are supplemental properties, not new fabricated placements. Sky/cub data, moving actors, effects and gameplay remain separate work.

The next native world step is bounded BINARY1 parsing plus source-reference resolution, then an instanced scene assembled from existing semantic PSG cache objects and these original root matrices. Preserve raw source fields and unknown supplemental keys. Retain a neutral all-static capture as a placement check, then validate materials, sky and zone visibility separately. Later animation and collision work must recover their original formats/calls rather than turn this diagnostic static mesh into a substitute race simulation.
