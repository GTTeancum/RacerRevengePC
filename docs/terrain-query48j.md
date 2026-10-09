# 48J: original limited-list filtering, callbacks and terrain surface assembly

The native terrain path now connects original candidate filtering and sphere bounds to the recovered mesh query, then performs the identified terrain callback and constructs `TerrainSurface48G`. **Both x86 and x64 pass 5,143 checks across 124 original-word before/after transitions**, including 55 geometric hits and 69 misses. This is the prepared limited-list phase, not the full world enumeration or an entire vehicle tick. Final native validation is recorded in `evidence/query48J/validation.json`.

## Implemented boundary

`src/game/terrain_query48j.h/.cpp` implements `queryTerrainLimited48J`. Inputs are an already prepared segment/world matrix and query sphere, collision filter metadata, ordered candidates, callback masks and the terrain state present at that boundary. Candidates contain actual typed `CollisionMesh48I` assets, original instance matrices, native identities, sphere/category/list metadata and callback support information. The returned state contains:

- `TerrainSurface48G`: force-angle flag `108`, source flags `110/114`, point `150` and normal `170`.
- The collected candidate identities in original order, capped at 16, exclusion owner and current candidate identity.
- Query/candidate `F8` flags and the original `40/50` field copies performed on a hit.
- Whether a geometry hit occurred, and selected/per-pair native contact results from the 48I calculation.

The production path has no guest addresses, original function dispatch, PS2 register state or guest-memory lookups. Identity zero represents a null owner/current candidate; the fixtures explicitly map original addresses to native IDs. The prepared state can represent a prior auxiliary surface result; the original routines that produce those auxiliary results remain outside this API. Unknown response callbacks and active group-object paths return `UnsupportedScope`, rather than silently becoming a miss.

`Complete` writes the resulting state even when there was no geometric hit. Every other status leaves the supplied output unchanged. The caller must distinguish this completion status from `hasGeometryHit` and from `surface.hit114`. In particular, a genuine geometry hit can be excluded from the final terrain surface.

## Original order and filters

The implemented original sequence is:

```text
122660 result reset
  → candidate type tag
  → candidate F8 bit 4 set
  → 126EB0 sphere gap
  → 120E70 identity/category predicates
  → 121410 recovered typed tree calculation
  → query F8 hit bit 5 set; original field copies
  → identified 1B0E50 callback, when enabled
1B19B4 final terrain surface selection
```

The query sets flag bit 4 and clears hit bit 5 before processing. Matching object-type candidates set their own bit 4 before sphere/filter checks, even if they are later rejected. The current candidate identity records the last matching type encountered, independently of the selected closest contact. Other object tags are skipped according to the original branch. A group tag is explicitly unsupported when the query's bit 0 does not suppress it.

Sphere gating preserves `sqrt((dx*dx + dy*dy) + dz*dz) - (queryRadius + candidateRadius)`, with separate rounded binary32 operations and the original inclusive comparison against zero. Squared-distance comparison is not substituted. The supported native sphere domain has finite centers, nonnegative finite radii and finite arithmetic intermediates.

`120E70` rejects self identity, a candidate without `F8 & 4`, a candidate without `flags31 & 1`, or no overlap between candidate category `A4` and query mask `B8`. It then checks both original identity-list groups on both objects. For each nonempty list, mode 1 rejects a listed identity; mode 0 rejects an unlisted identity; other modes impose neither of those rejections. Empty identity lists bypass these mode tests. The final bilateral category lists have a different empty-list rule: mode 1 rejects any intersecting category mask, while other modes require at least one intersection, so an empty category allow-list rejects. No default ground category is invented.

For a hit, `11E330` is a no-op for query type 2. The original then copies the query's existing `40/50` values to the candidate; these are not replaced with newly computed point and normal. The native result preserves those copies explicitly. `11E470` also adds no numerical change for type 2.

The terrain callback applies its three configured masks in order:

1. Append candidate identity if the collect mask intersects and count is below 16. Duplicate candidate occurrences remain duplicates; no sort or de-duplication occurs.
2. Set force-angle flag `108` to one if that mask intersects.
3. Store the candidate's **owner identity** in exclusion field `12C` if that mask intersects. An owner of zero clears the exclusion; matching the category alone does not imply exclusion.

Finally, `1B19B4` uses the query hit flag and exclusion owner. A valid, non-excluded selected contact replaces the prepared terrain point/normal unless `hit110` is set and its prepared Y is greater than or equal to the mesh-contact Y. On replacement it clears `hit110` and sets `hit114`. Equal heights preserve the prepared surface.

## Source anchors

All exact words and line annotations are retained in immutable `evidence/query48H/original-words.json` and `source-anchors.txt`.

| Address | Anchor | Behavior |
|---|---|---|
| `122660` | `work/native/project/generated/FUN_00122660_0x122660.cpp:28` | Limited-list entry and result reset. |
| `1226B8` | `work/native/project/generated/entry_001226b4_0x1226b4.cpp:43` | Original object type tag branch. |
| `1226F0`, `122714` | Same file, lines 107 and 181 | Sphere gap then eligibility filter. |
| `126EB0` | `work/native/project/generated/FUN_00126eb0_0x126eb0.cpp:23` | Original sphere arithmetic. |
| `120E70` | `work/native/project/generated/FUN_00120e70_0x120e70.cpp:23` | Self, flags and query category mask. |
| `120ED0`, `120F5C` | `entry_00120ed0_0x120ed0.cpp:23`; `entry_00120f0c_0x120f0c.cpp:125` | First bilateral identity-list group. |
| `120FEC`, `12107C` | `entry_00120f98_0x120f98.cpp:128`; `entry_00121028_0x121028.cpp:128` | Second bilateral identity-list group. |
| `12110C` | `entry_001210b8_0x1210b8.cpp:128` | Bilateral category-list logic. |
| `122840`, `122870` | `entry_001226b4_0x1226b4.cpp:753,827` | Type-specific result update and query callback. |
| `1B0E50/94/B0` | Corresponding `entry_001b0e50`, `entry_001b0e94`, `entry_001b0eb0` generated files, line 23 | Collect, force-angle and exclusion callback masks. |
| `1B19B4`, `1B19E8`, `1B19F8` | `entry_001b19ac_0x1b19ac.cpp:43,100,117` | Hit/exclusion test, prepared-height precedence and surface copy. |

## Saved contact and native assets

The actual saved query uses object tag 3, group tag 4, local endpoints `(0,3,0)` and `(0,-28,0)`, radius 15.5, query category zero, query category allow-list `[0x20]` and callback masks collect `0x8`, force-angle `0x80`, exclude-owner `0x4`. These values are fixture inputs from the preserved state, not constants hardcoded into production.

The saved cave candidate's category is `0x8`; the original full query rejects it at the category allow-list. The floor candidate's category is `0x28`, so only that candidate reaches the mesh tree and terrain callback. This distinguishes the actual limited-list path from merely running the two mesh narrowphase calls.

Native tests load both complete collision meshes from the retained 48I `RRCOL001` caches and regenerate their original source instance placements. Their source/cache/placement binding remains documented and hashed in `evidence/query48I/cache-binding.json`. The current C++ tests compare those native matrices to each fixture's original matrix before querying. No sparse copied triangles are used by native production tests.

The saved prepared-list result is query flags `0x76`, collected IDs `[2]` (floor), no exclusion, current candidate ID 2, and surface flags `(forceAngle108=0,hit110=0,hit114=1)`. Point is `(4979.41552734375,2120.56689453125,12752.63671875)`; normal is `(0.006339237559586763,0.9701666831970215,-0.2423563450574875)`. All ten selected point/normal/correction/fraction float words remain exactly those verified in 48I. The original query plus assembly tail executes 9,577 instructions for this fixture.

## Executable before/after evidence

`tools/gameplay/terrain_query_fixtures48j.py` executes the complete original `122660` entry and then the exact original `1B19B4` assembly tail. It supplies the original stack epilogue's return frame when entering the latter boundary. No numerical instruction, branch or called routine inside either measured phase is patched or replaced. The frozen 48H test-only original-word executor produces expected results independently of the new C++ code.

The fixture seed separately executes original `1B1800` only up to `1B1904`, with both optional surface pointers null, to recover the original reset flags and transformed lower endpoint. It then uses the saved query matrix and bounds as explicit prepared inputs; the fixture's initial current-candidate identity is explicitly set to null. It does **not** claim that stopping there performs the later matrix/spatial-index refresh. The saved scene matrix and query matrix differ at this snapshot; pretending they were interchangeable would produce an invalid whole-phase proof.

The 124 fixtures cover identity-list modes/membership on both objects and both groups, category allow/deny lists including empty lists, candidate flags, query accept masks, skipped object types, reverse candidate order, empty candidates, callback disabling, duplicate-hit collection near the 16-entry cap, force-angle/exclusion with null/non-null owners, three prepared surface height cases, exact/adjacent sphere boundaries and deterministic mixed filter settings. Across them, the oracle executes 668,817 instructions and 2,343 unique original words. All input/output data and oracle event traces are stored in `fixtures.json`; the compiled fixture is the corresponding `fixtures.bin` and generated header.

Every case was then re-executed from an independently reconstructed **1,640-byte source-data fixture, 91 slices**, with every original-data read outside the retained byte addresses rejected. All before/after states and event traces matched the full-source runs. `original-data.bin/json` preserve those bytes, addresses and per-slice hashes; `sparse-replay.json` records the guarded replay result. Whole guest RAM is provenance, not a native runtime dependency.

Snapshot SHA-256 is `38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48`. The immutable ELF SHA-256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Exact original word and imported asset dependencies are hashed by the owned checkpoint manifest.

## Validation scope and remaining work

Final integrated build `evidence/nativePc48J-build-20261003-172835-433` passes **all 20 CTests on x86 and x64**. On each architecture, the terrain target passes **5,143 checks across 124 original-word queries and contact-state transitions, with 55 hits and 69 misses**. Details are recorded in `evidence/query48J/validation.json`; the manifest verifies compiled source/header/test hashes, logs and frozen binaries. The C++ checks compare exact discrete state and binary32 point/normal/correction/fraction words, callback count and order, flags, current candidate, exclusion owner and original copied fields. Explicit native-only rejection checks cover unsupported response callbacks/group objects and invalid list capacity. Floating checks use `/fp:strict` and round-to-nearest, not four host rounding modes.

This API consumes a prepared limited list. It does not refresh the original spatial index, generate the full world list, produce query matrices/bounds from live body state, execute auxiliary surface providers, implement arbitrary candidate callbacks or handle compound/group objects. Its output is ready for the recovered `TerrainSurface48G` consumer, but this checkpoint alone does not establish the surrounding vehicle substep or live driving integration. Runtime material/category binding beyond the saved metadata still needs a source-derived native loader path.

The underlying tree retains its 48I limits: one endpoint pair, float vertices, mode 1, reverse flag zero and node root zero; explicit unsupported original smooth-normal boundaries remain visible. The ordinary host IEEE instruction oracle is not a proof of every PS2 exceptional, denormal or overflow mode. Extreme finite inputs overflowing intermediates are outside the validated domain. This saved-state boundary proof is not a paired full simulation tick or an FPS measurement.
