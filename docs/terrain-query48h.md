# 48H: original terrain segment query and native triangle leaf

The native mode-1 segment/triangle calculation now passes **4,025 checks from 710 original-word fixtures on both x86 and x64**. Independently, offline execution of the original static-mesh tree and limited-candidate query reproduces the saved 47W contact point, interpolated normal and segment fraction exactly. The native game does not yet call a complete native terrain query: candidate selection, tree traversal, transform handling and contact assembly remain integration work.

## Implemented native boundary

`src/game/collision_query48h.h/.cpp` implements the unquantized-vertex, mode-1 path of `0x10B840`, over ordinary typed vertices, optional float vertex normals, start, direction and length. There is no guest-memory lookup, generated game function, interpreter, register array or instruction dispatch in this production calculation. The terrain caller supplies a normalized direction and positive length. The exact caller normalization belongs to `0x1082B0`; it is not silently replaced by this API.

The implementation preserves:

- Computed triangle normal from the ordered cross product, followed by the original VU-style binary32 normalization sequence.
- One-sided facing test `normal dot direction < 0`, positive length, and segment fraction in inclusive `[0,1]`.
- Original order `start + length * (fraction * direction)` for the point.
- Original dominant-axis projection and six ordered scanline branch pairs for inside-triangle testing. This is not a replacement barycentric intersection algorithm.
- Original mode-1 normal interpolation: select ordered triangle edges using an X or Z cut, interpolate both edge positions and normals, interpolate across those positions using the original second-axis choice, then normalize.
- Transactional output: only `Hit` writes the result.

The API deliberately reports `UnsupportedInterpolationBoundary` when the original smooth-normal path lacks two initialized edge slots, or when interpolation cannot produce a finite normal. This is an explicit incomplete domain, not a substitute normal or a false miss. A future query caller must preserve this distinction.

## Actual shape, transforms and original traversal

The terrain component is **type 2, a tree of endpoint pairs**. `0x11C3F0` takes count 1, mode 1, and the two local endpoints. `0x113070` builds the pair tree; the count-one leaf at `0x112D7C..0x112DA0` copies both vectors. `0x113880` flattens them to 32-byte leaves. There is no capsule radius or swept-sphere substitution in this setup.

The saved player terrain component is `0xD113F0`, linked body `0xD110A0`, scene `0xCFD8F0`, query structure `0xE6F3D0`, endpoint leaf `0xC8CD10`, result `0x9512E0`. Its local endpoints are `(0,3,0)` and `(0,-28,0)`, root ID `0x8000`, count 1 and mode 1. Initialization `0x1B1240` derives the endpoints from body-local center, the supplied vertical offset and MaxLiftHeight; these constants are evidence from the saved object, not universal values hardcoded into production.

`0x1B1800` is the prequery phase. It resets contact flags and the largest velocity correction, transforms the lower endpoint, and optionally handles the two auxiliary surface sources. The saved object's auxiliary pointers `+120/+124` are null. If prior contact `+104` is active, `0x11FF90` receives the body basis; otherwise it receives identity rotation with body translation. `0x11FF90` calls `0x11F680` and spatial-index refresh `0x120230`. The type-2 setter starts at `0x11FA30`; for the single-leaf query, `0x11FB1C` copies the matrix and derives the bounding center/radius from the pair. The matrix is not inferred from the visible rendering mesh.

The saved limited-candidate arguments are count `*(0xC5B538) = 2` and **inline pointer array at `0xC5B53C`**. This is not a pointer/count pair in the reverse order. Its candidate objects are `0x9E7D30` and `0x9F7350`, both type 0; category words are respectively `0x8` and `0x28`.

Original limited-list flow:

`122660 → 11DD00 (result reset) → 126EB0 (bounding spheres) → 120E70 (candidate filters) → 121410 → 1089F0 → 107170 → 1082B0 → 10B840`

`0x1089F0` establishes both relative matrices. The leaf uses inverse(target matrix) times query matrix. Tree IDs use bit `0x8000` for leaves; target leaf stride is 20 bytes, pair leaf stride is 32 bytes, node stride is 36 bytes. In the pair-leaf / mesh-node path, original bounding tests prune before recursively visiting child `+8` and then child `+A`; both children are considered even after a hit. This order must be retained for ties.

The full-world alternative is `0x121920`, which obtains spatial candidates before the same type dispatch. Its full scene enumerator is source-anchored but has not been replayed or converted to a native production query here.

## Hit selection and normals

At `0x108794`, an already-present result for the same endpoint pair is replaced only by a **strictly smaller** fraction. At `0x1088A4`, the global selected result accepts **less than or equal** fraction. Consequently equal-distance traversal and candidate order can change the selected contact normal. Sorting native triangles or returning the first hit would not preserve this contract.

`0x10B840` recomputes the face normal from positions for intersection and interpolation. Optional vertex normals remain source float3 values. Separately, `0x1082B0` reads the runtime signed-short face normal for its penetration/correction vector. These two normal representations must remain distinct. Parent-owned `collision_runtime48h` recovers the original biased float-to-short packing; the leaf API does not quantize vertex normals.

The original callbacks also affect the prequery outcome. `0x1B0E50` appends objects selected by category mask at `0x3E0720` (saved value 8); `0x1B0E94` can set component `+108` using mask `0x4342B0` (saved 128); `0x1B0EB0` can set exclusion pointer `+12C` using mask `0x3E0718` (saved 4). `0x1B19B4` then checks hit bit 5 and exclusion state, compares an optional surface height if present, and copies result point/normal into component `+150/+170`, setting `+114`. No default ground category is invented.

## Executable evidence

`tools/gameplay/query_extract48h.py` extracts 9,021 exact ELF words in bounded ranges. Every available generated annotation is checked against the ELF word. `evidence/query48H/original-words.json` stores range hashes and exact words; `source-anchors.txt` stores address, instruction, generated filename and line. The original ELF SHA-256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`; file offset is VA minus `0x100000` plus `0x80`.

`query_oracle48h.py` is an offline, test-only instruction-subset executor. It separately implements word decoding, branch delay slots and likely-branch annulment, bounded memory, scalar binary32 operations and the encountered VU arithmetic. It uses rounded host IEEE operations for ordinary values; it is not an assertion of all physical PS2 denormal, exception, overflow or extended-precision modes. SQRT.S correctly reads Ft. No game binary is launched by these tools.

`query_fixtures48h.py` generated 710 fixtures: 216 axis/permutation/boundary cases, 6 segment-endpoint cases, parallel and degenerate cases, 480 deterministic 3D cases and all 6 original leaves reached by the saved two-candidate query. The oracle executed 1,288 unique leaf/helper words. C++ tests compare result status exactly, and all seven point/normal/fraction words exactly for defined hits. Both architectures passed with `/fp:strict`, round-to-nearest: 434 hits, 261 misses and 15 explicitly unsupported interpolation boundaries. Four host rounding modes were **not** tested for this leaf.

`query_probe48h.py` additionally executes the saved query:

| Entry and candidate | Result | Original instructions | Evidence |
|---|---|---:|---|
| `121410`, `9E7D30` | Miss | 6,751 | Stored result point is stale and is not claimed as a hit. |
| `121410`, `9F7350` | Hit | 9,113 | All seven selected contact words match 47W. |
| `122660`, original two-object list | Hit | 9,553 | Includes original reset, filters, traversal and callbacks; same exact contact. |

Matched point: `(4979.41552734375, 2120.56689453125, 12752.63671875)`. Matched normal: `(0.006339237559586763, 0.9701666831970215, -0.2423563450574875)`. Fraction: `0.35526010394096375` (`0x3EB5E4A7`). Snapshot SHA-256: `38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48`.

The same three replays passed again from a **1,908-byte sparse snapshot fixture, 116 slices**, with every read outside preserved bytes rejected. `saved-query-data.bin` and its address/length/hash JSON preserve those inputs; `saved-query-replay.json` preserves outputs and every executed PC. Whole PS2 RAM is not needed to represent this recovered calculation. The original RAM remains provenance and is unchanged.

## Exact boundary discovery

Fixture 10 contains vertices `(-2,0,-2),(0,0,2),(2,0,-2)`, their recorded distinct normals, start `(-2,1,-2)`, direction `(0,-1,0)`, length 2. The original intersects the first vertex, reports hit fraction 0.5, then reads unwritten second-edge pointers at PCs `0x10CDC8/0x10CDCC`, stack addresses `0x01FEFB38/0x01FEFB3C`. In the explicitly zero-initialized fixture those reads lead to NaN normals (`0xFFC00000`). Real stale stack contents are not known. All 15 boundary fixtures record the actual unwritten pointer reads. Other exact-vertex cases can have two valid edges and remain defined hits; the classification follows the original reads, not a blanket vertex rule.

The initial fixture classifier was too broad and failed fixture 11 (`Hit` versus expected boundary). It was corrected to detect those actual unwritten original slots. The initial C++ test also required a missing `<string>` include. Failed preliminary build logs remain in the parent evidence directories `nativePc48H-build-20261003-163156-442` and `nativePc48H-build-20261003-163213-468`.

## Validation and remaining work

Final integrated build: `evidence/nativePc48H-build-20261003-163334-944`; **all 16 CTests pass on x64 and x86**. The owned manifest verifies compiled source/header/test/fixture hashes against both build manifests, and preserves test log and executable hashes.

Next production work is original endpoint normalization, relative transforms, AABB pruning and ordered traversal over the already imported native collision tree, followed by original per-pair/global result selection and callback/category behavior. The saved-list replay provides a concrete acceptance target. Preserve the original body/scene synchronization before querying and feed the assembled surface into the recovered 48G response.

Remaining explicit limits: full production tree/candidate/material assembly; quantized-vertex leaf input; modes above 1; original interpolation boundaries; spatial-index update in full `1B1800`; optional surface sources; moving collision objects; full vehicle substep and controller/timestep mapping. Finite but extreme inputs that overflow intermediates are outside the tested native domain; status values do not comprehensively classify every overflow path. The functions reject nonfinite input values, but this does not establish all-finite-input PS2 equivalence. The saved snapshot is not a before/after simulation-tick pair. No whole-race correctness, playable-game or FPS claim follows from these tests.
