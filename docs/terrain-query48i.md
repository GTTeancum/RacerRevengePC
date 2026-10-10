# 48I: native ordered terrain tree query over imported collision assets

The recovered native calculation now covers the saved terrain segment's relative transforms, original AABB pruning, ordered mesh traversal, triangle intersection, hit selection and world-space contact assembly. **Both x86 and x64 pass 5,351 checks across 43 original-word queries.** Its two saved target meshes are loaded from retained `RRCOL001` caches, and their world matrices are regenerated from the original track instance properties. This closes the numerical mesh-query path over native assets; world candidate enumeration, eligibility/callback behavior and the complete driving loop remain separate work.

## Native calculation and original contract

`src/game/collision_tree48i.h/.cpp` accepts one pair of local endpoints, the query world matrix and an already filtered, ordered candidate list. Every candidate supplies a typed `CollisionMesh48I`, its composed world matrix and an ordinary native identity. It calls the frozen mode-1 triangle routine from 48H. Production code has no guest-memory access, generated original function calls, register array or instruction dispatcher.

The implementation preserves these original operations:

| Original address | Recovered behavior | Generated source anchor |
|---|---|---|
| `1089F0`, especially `108BA4..108CF4` | Transpose target basis, negate transformed translation, compose target-relative query matrix in original binary32 order. | `work/native/project/generated/FUN_001089f0_0x1089f0.cpp:30` |
| `1067B0..106B48` | Segment/AABB separating-axis tests, exact inclusive comparisons without an added epsilon. | `work/native/project/generated/FUN_001067b0_0x1067b0.cpp:28` |
| `107170` | Depth-first child `+8` then child `+A`; continue after hits. | `work/native/project/generated/FUN_00107170_0x107170.cpp:23` |
| `108628..108708` | Original scalar reciprocal-square-root/reciprocal length, then reciprocal length and normalized direction. | Exact words and line annotations in `evidence/query48H/source-anchors.txt`. |
| `108794` | Per-endpoint-pair result accepts a strictly smaller fraction after its first hit. | `work/native/project/generated/entry_0010875c_0x10875c.cpp:85` |
| `1088A4` | Global selected result accepts less than or equal fraction. | `work/native/project/generated/entry_0010875c_0x10875c.cpp:305` |
| `1088F8..108980` | Correction uses remaining segment projected onto the separate runtime packed face normal. | Exact words and line annotations in `evidence/query48H/source-anchors.txt`. |
| `121410`, `1214E0..121564` | Convert contact point, interpolated normal and correction to world space. Preserve the normal/correction `VADD vf0` operations. | `work/native/project/generated/FUN_00121410_0x121410.cpp:31`; `caseD_0_0x121468.cpp:162,201`. |

The relative transform deliberately uses a transposed basis, including the original additions of zero. It does not repair a nonorthogonal matrix or substitute a general matrix inverse. The fixtures include deliberately nonorthogonal targets to exercise that distinction. Only finite affine matrices with basis W components zero and translation W equal to one are accepted.

Traversal uses native 32-bit child tags while retaining original node and triangle order. The imported runtime face representation is 16 bytes, rather than the original 20-byte RAM record; it retains face plane D, three packed signed-short normal components and three vertex indices. Optional smooth vertex normals remain float3. Intersection and interpolation use the original position/float-normal path; correction uses the packed normal scaled by float word `38000100`. These representations are not interchangeable.

The API exposes selected and per-pair contacts separately because they differ on equal fractions. The global contact chooses the last accepted tie; the per-pair contact retains the first. The tests distinguish them using coincident triangles with different normals, and also coincident candidates. An optional diagnostic trace records each visited node or leaf and its overlap/hit result. Only `Hit` writes the result; misses and rejected inputs leave it unchanged.

## Complete saved mesh binding to source assets

The snapshot mesh was not approximated by a few copied triangles. `query_fixture48i.py` follows every original child tag from root zero and requires every reachable node and leaf index to form dense, unique ranges. `query_binding48i.py` compares all extracted typed records to the preserved source COL references, including the original biased float-to-short face-normal packing. The C++ check independently loads the retained native cache and compares every vertex, optional normal, node and face bitwise before replacing the copied snapshot mesh with the imported one.

| Saved shape | Native cache / original source | Complete mesh | Original instance |
|---|---|---|---|
| `009E7E40` | `17-ta_colcavetop.rrcol` / `ta_colcavetop.col` | 528 vertices, no vertex normals, 924 nodes, 925 faces | `zone4-model4`, source block offset 83959 |
| `009F7460` | `18-zone04_col_floor.rrcol` / `Zone04_COL_floor.col` | 249 vertices and normals, 395 nodes, 396 faces | `zone4-model5`, source block offset 84289 |

The original Position strings are respectively `5350.316406,1774.107178,11737.883789` and `5350.316406,1765.926270,11737.883789`. Both have Look `(0,0,1)` and Up `(0,1,0)`. Source properties come from the preserved track reference `evidence/worldImport48F/ta-tracks_ta.reference.json`; they are converted to binary32 and passed through parent-owned original-order `placementCollision48I` and `composeCollisionWorld48I`. Their resulting complete 16-word world matrices must equal the saved matrices before running the queries. The cache-local matrices are identity for these actual assets.

`evidence/query48I/cache-binding.json` records the exact source sections, original property strings, source block offsets, input/output float words and source/cache hashes. The retained caches were copied byte-for-byte from the verified 48H build's collision fixtures:

- Cave cache SHA-256: `d8c487c6354b447b535b367250e1bd0acfb5e2b602f37d0a2671eca0bf22b348`.
- Floor cache SHA-256: `ba25e97fc450d725e7261b8449d77d19b651cd9f945779656843e422d0e3fd57`.
- Original cave COL SHA-256: `f4465ff6990c7c135637ecf9cbce061707268b65cc897bd483def10db1c7c38c`.
- Original floor COL SHA-256: `8a01ddd984e95bfc115ad0fda7a5ce46bfd321de710829448aa61d6d6cfc8ac5`.

## Independent original-word evidence

`tools/gameplay/query_fixture48i.py` uses the immutable 48H word executor and extracted original words. It constructs explicit typed query/tree fixtures in otherwise zeroed test memory and executes the original `121410` entry for each ordered candidate. The seven original dispatch-table words at `388AF0` come directly from the hash-verified ELF. Expected results do not come from the new C++ implementation. The oracle additionally captures branch outcomes at the original node and leaf return sites, and selected/per-pair identity updates at their original PCs.

There are **43 original-word fixtures: 15 hits and 28 misses**, with **1,195 ordered visits** and **382,863 original instructions executed** across all fixtures. They comprise the actual saved query, 30 deterministic world-translation variations with alternating candidate order, two tie cases, four AABB boundary cases, and six rotated/nonorthogonal target cases. Expected status, every ordered visit and accept/reject outcome, selected ten float words and identities, per-pair seven float words and identities, and native visit counters are checked. The test also rejects a cyclic tree transactionally. No assertion of full-world coverage follows from these 43 queries.

The saved query has local endpoints `(0,3,0)` and `(0,-28,0)` with the recorded query matrix. Candidate 1 is the cave mesh and misses; candidate 2 is the floor mesh and selects face **175**. Its complete selected output exactly matches the saved original ten words:

```text
point       459B9B53 45048912 4647428C
normal      3BCFB961 3F785CD8 BE782C43
correction  BF9F97DB C197C369 40BE365E
fraction    3EB5E4A7
```

That replay visits 48 nodes/leaves in original order and executes 15,868 original instructions for the two narrowphase calls. The selected point is `(4979.41552734375,2120.56689453125,12752.63671875)` and fraction is `0.35526010394096375`. The source ELF hash remains `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`; snapshot hash remains `38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48`. Original addresses map to ELF offsets using VA minus `100000` plus `80`.

The independent 48H full `122660` saved-list proof, including original filters and callbacks, remains valid and preserved. The new 48I production API covers ordered **narrowphase** calls. It does not claim to implement those filters/callbacks merely because the saved two-candidate contact matches.

## Validation and retained failure

Final integrated build `evidence/nativePc48I-build-20261003-170723-706` passes **all 18 CTests on x64 and x86**. The tree test reports **5,351 checks, 43 original-word ordered queries, 15 hits, 28 misses and 1,195 exact visits** on each architecture, using both complete native cache meshes and their original instance placements. Both-architecture validation is recorded in `evidence/query48I/validation.json` and `checkpoint-manifest.json`. Compiled source/header hashes are checked against the parent build's recorded hashes; test logs and frozen executables are hashed. The native target uses `/fp:strict` and round-to-nearest. Four host rounding modes were not tested for this tree or its frozen triangle leaf.

The first preliminary x64 run failed after 1,398 checks at fixture 31: correction X was negative zero where the original returned positive zero. `1214FC` and `121530` explicitly add `vf0` after transforming normal and correction, including positive-zero XYZ lanes. Adding those original operations fixed the discrepancy. The failure log remains at `evidence/nativePc48I-build-20261003-170000-689/x64.log`; expected values were not relaxed and comparisons remain bitwise. The fixture generator initially omitted the original dispatch table in its fresh fixture memory; the corrected generator reads its exact words from the verified ELF.

## Remaining limits and next connected step

The API requires already filtered candidates in original order. It does not yet perform `120230` spatial-index refresh, full-world `121920` enumeration, limited-list `122660` bounding/category filtering, original callbacks, optional surface-source comparisons, exclusion handling or final component flag assembly at `1B19B4`. Moving collision objects are not integrated. The parent world loader exposes static collision assets and placements; this numerical API is not yet invoked by the running viewer. Connecting it to the recovered terrain response must preserve those remaining category and contact-state semantics.

The supported shape is one endpoint pair, mode 1, float vertices, reverse flag zero, with mesh node root zero. Quantized vertices, alternate modes, multiple endpoint-pair trees, reverse-mode meshes and a direct leaf root are outside this implementation. The inherited `UnsupportedInterpolationBoundary` result is preserved for the original smooth-normal path's uninitialized edge-slot cases; no fallback normal or invented miss is returned. Zero-length segments return a miss without executing the original transient invalid floating-point operations, so exact trace/exception equivalence for that degenerate case is not asserted.

The oracle uses ordinary rounded host IEEE arithmetic for its encountered instruction subset. This is not a proof of all PS2 denormal, exception, overflow or extended-precision modes. Finite extreme inputs that overflow intermediate arithmetic are outside the validated domain; status codes do not comprehensively classify every overflow path. The saved RAM is a single state, not a paired before/after vehicle tick. This checkpoint does not establish a complete vehicle substep, controller/timestep mapping, stable gameplay, or an FPS result.
