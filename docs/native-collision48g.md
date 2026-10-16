# Native collision recovery 48G

The portable C++17 converter recovers all 38 original TA COL assets as semantic triangle trees. It preserves 10,199 vertices, 14,548 internal AABB nodes, 14,586 triangle leaves, optional vertex normals, every authored plane, and the exact original bytes. It emits bounded `RRCOL001` files suitable for a native collision query implementation. This checkpoint does not implement or claim the original query traversal, contact selection, surface response, or playable racing.

The historical 48G parent build `evidence/nativePc48G-build-20261003-155827-581` passed all 13 CTests on both x64 and x86. Its collision test reports **88,316 checks and zero failures on each architecture**, covering every TA source record against independent Python references and the full-disc degenerate-plane regressions below. All 38 emitted native fixtures are byte-identical between architectures. The earlier build `evidence/nativePc48G-build-20261003-154328-472` passed 88,052 collision checks before the first full import exposed that authored variant. Parent integration checks in `evidence/import48G-x64-yo7tip_x` and `evidence/import48G-x86-qmst15to` exercised import/reuse, corruption rejection and repair, metadata repair, and combined native world loading. There are 38 source members but 37 unique cached objects because two animation members have identical source bytes. The static world retains 37 instances of 36 unique collision assets. The parent maintains full import/runtime evidence separately.

## Original evidence and format

`evidence/importCollision48G/original-loader-proof.json` freezes 1,511 original instruction words, including 1,464 matching generated instruction comments, 86 source hashes, nine string anchors, and three dispatch/vtable tables. The ELF SHA-256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Generated disassembly text is retained alongside the verified original words; it is not independently re-disassembled by the proof script.

The original reader at `0x11BA74–0x11BAF8` requires magic 1234, `col\0`, and version 1. The type reader at `0x124FA4–0x125010` dispatches type 0 to `0x125890`. Its field reads establish this exact little-endian body:

| Offset / order | Original field |
|---|---|
| 0, 4, 8, 12 | u32 magic, four-byte tag, u32 version, u32 type |
| 16 | Twelve binary32 values: three basis rows and translation, xyz only |
| 64 | u32 reverse-mode field |
| 68, 72, 76 | u32 vertex, internal-node, face-leaf counts |
| 80 | u32 quantized flag |
| 84 | Vertex triples, binary32 when the flag is zero |
| After vertices | u32 optional vertex-normal flag; one normal triple per vertex if present |
| After normals | Internal nodes: volume, center.xyz, half.xyz (seven binary32 values), two u16 children: 32 bytes |
| After nodes | Faces: binary32 D, normal.xyz, three u16 vertex indices: 22 bytes |

Original read anchors include affine construction `0x1258B4–0x125A0C`, flags/counts `0x125A18–0x125AAC`, float vertices `0x125BA0–0x125C88`, optional normals `0x125C90–0x126028`, internal nodes `0x12602C–0x12624C`, and faces `0x126250–0x1263F4`. The first node is the root; bit 15 in a stored child distinguishes a leaf index. The supplied Cars `NewCollision.h` and `NewCollision.cpp:5` corroborate the compact records and depth-first indexed tree. `ObjectExport.cpp:2488` writes the header; its `ColVersion > 1` branch at line 2517 adds a material word only for the later format. **The original COL1 assets contain no material ID or per-face flags.**

All 38 TA headers are COL1/type0, all stored affine matrices are identity, and all position quantization flags and reverse fields are zero. Twelve assets include float vertex normals, and 26 omit them. Quantized positions, other shape types, version 2, and single-face roots remain explicitly unsupported. Boolean flags outside 0/1, invalid records, bad indices, shared/cyclic/unreachable trees, truncation, and suffix bytes are malformed. Limits can be tightened by callers and never expanded beyond the hard ceilings.

`geometry\Zone9_COL_floor.col` is the original named lead: logical RES offset 8,577,024, 28,226 bytes, 267 vertices, 402 nodes, and 403 faces. The exact member hash and every other source hash are in `reference-index.json`. The source TA.RES SHA-256 is `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.

## Whole-disc authored degenerate planes

The first full import stopped after 54 containers on `TRACKS/MC.RES` member `geometry\colmountd.col`, with `col.nonfinite`. The failed attempt and prior-cache verification remain in `evidence/importFull48G-gu9agzaq`. The source member is 6,836 bytes at logical offset 7,593,984, SHA-256 `fa3d929308e910001b240052d8d9ca6f865d56325f602de8cc94d8cc62391e77`. Its faces 28 and 29 begin at source offsets 0x1400 and 0x1416; both repeat vertex 62 and have no area.

`tools/assets/audit_collision48g.py` examined all 2,505 COL occurrences in all 103 original RES containers. Of these, 2,281 have the supported COL1/type0 float layout, 11 use quantization, and 213 have other headers. Exactly nine float assets contain nonfinite words: 54 face planes, comprising 216 binary32 words. Every affected face has two exactly coincident original vertex positions, including cases whose indices differ. Every affected D word is `0x7FC00000` and every normal component is `0xFFC00000`. All original matrices, positions, optional vertex normals and AABB fields in the float subset are finite. The nine exact original members, complete container/member hashes, offsets, bit patterns and independently computed triangle cross products are retained in `nonfinite-originals/audit.json` and its hash-named `.col` fixtures.

| Container | Member | Degenerate planes |
|---|---|---:|
| MC.RES | geometry\colmountd.col | 2 |
| RC.RES | geometry\Zone09_Wall_COL.col | 1 |
| RC.RES | geometry\Zone13_Wall_COL.col | 2 |
| SA.RES | geometry\shipyard_wall08.col | 16 |
| SA.RES | geometry\shipyard_wall12.col | 24 |
| TB.RES | geometry\arena_c14.col | 6 |
| TB.RES | geometry\TB_FarmCliff02_C06.col | 1 |
| TB.RES | geometry\TB_FarmPaths_C14.col | 1 |
| TC.RES | geometry\tc_col.col | 1 |

The supplied later Cars reference `BoundingBoxTreeBuild.cpp:714–734` computes a cross product, then normalizes it by reciprocal square-root of its squared length without a zero-length guard. Lines 750–764 call this for every indexed face. This source behavior is consistent with the exact authored zero-area/NaN pattern; it is not proof that this particular exporter binary produced the original assets. The original Racer loader reads these planes at `0x126250–0x126398`. The bounded native projection and query constraints below supersede the earlier unsupported classification.

The converter now admits only the complete authored four-word pattern in an unquantized face with exactly coincident finite vertex positions. It validates every index and the entire ordered tree. The original encoder and native cache preserve D, all normal words, every face and all links bit-for-bit; cache parsing reconstructs and compares the complete original bytes before acceptance. Unknown NaN payloads/signs, partial NaN planes, infinities, NaNs on nondegenerate triangles, nonface nonfinite values, invalid topology and truncation remain malformed. Quantized nonfinite planes remain rejected. No face is removed or replaced with an invented source plane.

### Compact-normal arithmetic evidence

The retained loader instructions in `evidence/collisionRuntime48H/normal-pack.txt` show multiply, bias, `CVT.W.S`, and low-halfword stores at `0x126330/34/38/44`, repeated for Y/Z through `0x126374`. The [Sony EE Core User Manual, sections 8.1.1, 8.5, 8.7 and 8.8](https://docs.alexrp.com/mips/ee.pdf) documents signed floating-point representation, overflow saturation, truncation, and the absence of IEEE NaN semantics. For this exact authored negative `0xFFC00000` word, the positive scale and bias chain remains negative with out-of-range magnitude. The [Instruction Set Manual, CVT.W.S, page 356](https://studylib.net/doc/28989088/ee-insns) specifies negative conversion overflow as `0x80000000`; its stored low 16 bits are zero. This is a source/manual arithmetic derivation, not a new emulator or hardware measurement.

Accordingly, only that validated authored pattern derives compact normal `{0,0,0}`. Raw source normals and D remain intact in provenance/cache data, and ordinary finite normals retain the existing packing path. Type2 resource validation admits nonfinite D only when its exact word is `0x7FC00000`, the packed normal is zero, indices are valid, and finite positions coincide. Other bounds, topology and workspace checks remain unchanged.

### Query scope and validation status

No camera or collision-query arithmetic changes accompany this admission. The camera sphere query recomputes plane distance from the packed normal and a vertex; a zero normal can still produce a zero-distance contact. These faces must not be skipped or described as universally inert. Type2 SAT and leaf queries continue through their existing traversal and arithmetic.

The importer/cache regression now expects exact original and cache roundtrips for all nine retained members, while keeping malformed-pattern and topology rejection cases. The historical test totals above predate this admission change. This local integration has not been compiled, syntax-checked, decoded, or tested; compact projection, type2 admission, cache tamper rejection, original query evidence and the Sullust smoke remain subject to the parent's centralized validation.

## Placement and activation

The original collision creator has its own transform path. At `0x27B178–0x27B268`, it normalizes the source Up and Look independently, then calculates Right = Up × Look. Its `t1=0` skips Look orthogonalization. This differs from the geometry placement rule in 48F. Both helpers use an explicit scalar binary64 policy; neither claims PS2 VU reciprocal-square-root rounding equivalence.

The original type-0 setter, dispatch table `0x388A00` and instructions `0x11F6C8–0x11F79C`, computes **COL local matrix × collision instance matrix**, using row vectors. This is composition, whereas the separate PSG scene-root setters replace the saved root. Applying the original COL transform is mandatory even though the TA subset happens to store identity matrices. The C++ analytical fixture includes a rotated, translated nonidentity COL matrix to catch composition-order mistakes.

Presence alone is insufficient to select collision instances in general. The original loader reads `NumberOfCollisionObjects` with default -1 at `0x2764C4`; the missing-field path at `0x2764EC–0x276508` uses the static count and clears the separate-list bit. `0x27BD04–0x27BDA0` then selects `ZoneN-ModelK`. `StaticType` defaults to `Standard` (`0x27BDD4`, `0x27BAE0`); the collision parameter reader uses an authored `COL` override or falls back to the `SLT` basename (`0x27A9D4–0x27AA5C`). The original creation routine attempts to load the COL and drops that collision object if no file exists (`0x27B11C–0x27B144`).

The exact TA SCB contains none of those three overrides: all nine zones use the fallback count and all 318 model records omit `StaticType` and `COL`. Of those records, 37 resolve to 36 unique geometry COL members. `rockjb04_col.col` is instanced twice. All 37 TA collision basis matrices happen to equal the geometry matrix numerically; the independent nonorthogonal fixture exercises the distinction. Every binding preserves the original section, zone, ordinal, block offset, and source field offsets. `anim\xsailbarge.col` and `anim\x_sab.col` decode but are excluded from this static binding set. Animated transforms remain unrecovered here.

Runtime category/material masks and user data require their own binding. The collision creator copies a global mask into object +0xA4 and fields into +0xA8/+0xA9 at `0x27B0D8–0x27B114`. These are not COL face fields. The gameplay agent found that terrain callbacks inspect the object mask and user data before accepting a ground point. This decoder does not assign missing material categories, enable hits by default, or replace ground queries with a visual mesh.

## Native API and cache

`src/assets/import_collision48g.h` exposes `Collision48G`, `decodeCollision48G`, `encodeOriginalCollision48G`, `encodeCollisionAsset48G`, `parseCollisionAsset48G`, `collisionPlacement48G`, and `collisionWorldMatrix48G`. The production translation unit depends only on the shared decoder error/hook definitions and the C++17 standard library. It has no JSON, Windows, renderer, or guest-runtime dependency. All failure paths leave output objects unchanged. Progress/cancellation hooks cover conversion phases.

`Collision48G` stores source-precision float vertices, planes and AABBs, a sixteen-float affine matrix, the original flag words, exact source offsets, u32 triangle indices, and typed tree branches with native bit 31 denoting a leaf. The original bytes are retained in full. Native limits are 65,535 vertices, 32,767 nodes, 32,768 faces, 8 MiB original input, and 16 MiB encoded cache.

`RRCOL001` is explicit little endian:

| Native offset / order | Contents |
|---|---|
| 0 | Eight-byte magic `RRCOL001` |
| 8, 12 | u32 schema 1 and total encoded bytes |
| 16 | Five u32 source fields: version, type, reverse, quantized, normals-present |
| 36 | Four u32 counts: vertices, nodes, faces, original-byte count |
| 52 | Four u32 source offsets: vertices, normals flag, nodes, faces |
| 68 | Sixteen binary32 affine values |
| 132 | Binary32 vertex triples, then optional vertex-normal triples |
| After vectors | Nodes, 40 bytes: source offset, volume, center.xyz, half.xyz, two native u32 children |
| After nodes | Faces, 32 bytes: source offset, D, normal.xyz, three native u32 indices |
| End | Exact complete original COL bytes |

The native cache parser checks finite values, extents, counts, graph structure, indices, source offsets and affine layout. It validates provenance by serializing the typed records back to their exact source representation and comparing the retained raw bytes. It never invokes the original COL decoder. It therefore cannot accept changed typed fields paired with unchanged source provenance. The original source serializer also rejects inconsistency instead of modifying the evidence.

## Validation and outstanding work

The independent Python decoder uses the existing original RES recovery function. It saves all 38 original files and full semantic JSON references. Its geometric checks independently recompute face-plane residuals, source winding alignment, unit-normal error, descendant AABBs, and full-tree reachability. All 14,586 faces have positive winding alignment, and every original internal node and leaf is visited exactly once.

Authored imperfections are preserved. The largest source unit-normal error is about 9.70e-8. The maximum stored AABB undercoverage is 0.000244140625 units, consistent with stored float bounds. `geometry\zfloor_colramp1.col` has a maximum plane residual of 0.02900488109020216; the other source planes have much smaller residuals. These are measured diagnostics, not decoder repair tolerances. Future query work must choose policy from the original traversal, particularly near edges and almost-coplanar hits.

The original loader converts face-normal components from float to signed short using `32767*x + 0.5` and a MIPS float-to-integer operation at `0x126300–0x126398`. Native asset normals deliberately retain the original authored float values. Original query precision, signed-short reconstruction, traversal order, reverse-mode behavior, and hit tie breaking are not settled by asset conversion. The eventual query implementation must distinguish authored planes from the original runtime normal representation.

The native test compares every typed source field on both architectures, checks source and canonical native roundtrips, rejects meaningful truncation boundaries and malformed tree/index/flag/provenance mutations, validates transactional output and cancellation, and tests independently authored two-triangle and nonidentity-placement fixtures. It emits `.rrcol` fixtures into the build output directory; reports contain source hashes, member identity, output hash and path for independent built-in-converter comparisons.

This converter is a foundation of the planned built-in import workflow: select original data, convert once into a verified resumable cache, then run the native game. It is not a full importer acceptance claim or an Xbox asset packaging implementation. Xbox streaming and memory budgets remain separate work. No FPS gain or original gameplay equivalence is measured by these asset tests.

## Reproduction

From the repository root, run `python -B tools/assets/decode_collision48g.py`, `python -B tools/assets/source_collision48g.py`, and `python -B tools/assets/audit_collision48g.py`. Their output directory is `evidence/importCollision48G`; the decoder accepts explicit `--root` and `--out` paths. The native checker takes `ROOT [REPORT_JSON] [OUTPUT_FIXTURE_DIRECTORY]`. Parent CMake wires `tests/native/import_collision48g.cpp` with the native decoder library, test-only JSON, and the Windows BCrypt hash provider. Do not build the supplied Cars tools or run a reference race to reproduce this conversion.
