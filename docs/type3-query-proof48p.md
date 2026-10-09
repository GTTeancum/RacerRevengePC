# 48P original type-3 limited-list query proof

## Result and provenance

`tools/gameplay/type3_query_fixtures48p.py` executes 245 initialized cases:

- 240 complete original `122660` limited-list queries, including original `11DD00` preparation, filtering, `126F40` dispatch, `111300` bridge, `10E080` ray-array tree/leaf code, and applicable original `294BB0` type-3 callbacks/`2963E0` appends.
- Five direct `126F40` calls following complete original `11DD00` preparation, covering inactive, sphere-rejection, and exact-tangency paths that outer limited-list filtering can hide. Three of these explicitly initialize stale result fields at the direct-dispatch entry boundary, after preserving the authentic preparation snapshot. These are separately marked `entry: "direct"` and are not counted as complete limited-list queries.
- 849,765 original instructions, 321 dispatcher calls, 276 candidate geometry hits, 274 static callback invocations, and 168 cases reporting any hit.
- Every execution has `data == b''`, an empty allowed backing-source set, and zero `source_reads`. Each data byte is explicit synthetic initialization or a prior original-instruction write. A missing byte raises; no read falls back to RAM, an inferred snapshot, or zero-filled uninitialized memory.
- All loaded instructions are checked against the authorized immutable ELF on the normal run. Its SHA-256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Frozen reruns pin and check the frozen input hashes and per-block words. No instruction patches, helper replacements, forced returns, native expected values, or type-2 shape adapter are used.

The current fixture SHA-256 is `a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f` (8,729,321 bytes). The normal and `--frozen` runs produced byte-identical fixtures and executed-word manifests; `nonfrozen-hashes.txt`, `run.log`, and `frozen-run.log` record the checks.

Geometry consists of all 43 complete typed 48I geometry cases and their three typed meshes, plus explicit synthetic trees for ordinary hits, misses, coincident faces, near/far face order, and normal interpolation. Typed inputs are read only from the pinned `query_fixture48i.h`; no historical oracle output is reused as an expected value. Endpoint world matrices from those cases are applied by original type-3 preparation using explicit endpoint scene objects.

The selected 48O-corrected `SpringOriginal` scalar MIN/MAX implementation is inherited. The active 48P `query_oracle48h.py` also uses finite signed-word VU MIN/MAX lane selection and suppresses VF0 writes. The archived legacy MIN/MAX oracle is not imported or executed. Arithmetic scope remains individually rounded host IEEE binary32 for the fixture domain; this is not a claim of all physical EE/VU exceptional arithmetic modes.

## Fixture schema and boundaries

`evidence/type3Query48P/native-fixtures.json` has `schema: 1`, four explicitly backed array slots, the shared mesh data, and self-contained case inputs/expected observations. Inputs retain the filter metadata, candidate order and shape tags, scene matrices, endpoint raw words, normal-interpolation mode, body transform, initial spring state/records, and stale length/flag/fraction sentinels.

The explicit type-3 object uses:

| Shape offset | Meaning |
|---|---|
| `+00` | pair count |
| `+04` | endpoint-scene pointer array, two words per input pair |
| `+08` | flat input endpoint Vec4 array, two vectors per pair |
| `+0C`, `+10` | target-local start/direction scratch arrays |
| `+14` | per-pair scalar lengths |
| `+18`, `+1C` | prepared world starts/directions |
| `+20`, `+30`, `+40` | target-local center, world center, radius |
| `+44` | normal-interpolation mode |

Expected snapshots:

- `initial`: the explicitly initialized pair/prepared/spring/record state before any original preparation.
- `after_prepare`: pair fields and all prepared arrays immediately after original `11DD00` returns.
- Optional `dispatch_initial`: explicit direct-dispatch input state after authentic preparation. It is present only for the three stale early-gate cases; `after_prepare` remains untouched. Native direct-entry tests should use this state when present, otherwise `after_prepare`.
- `calls[]`: each `126F40` candidate identity, return value, and complete before/after pair/prepared snapshots.
- `pair`: final result count (`+60`), selected five Vec4s (`+00..+4F`), selected fraction (`+74`), all four hit flags (`+68`), fractions (`+6C`), normal Vec4s (`+54`), and surface Vec4s (`+58`). `other_fields` preserve `+78/+7C`.
- `prepared`: all four slots of local/world arrays, lengths, eight scene metadata words, input endpoints, both centers, radius, and query center Vec4/radius.
- Final query flags/current candidate/copied Vec4s and each candidate's flags/copied Vec4s.
- `requires_mode2`: true only if the actual original `10F580` non-mode-1 normal helper executed. Only `normal-array-mode-2` requires it in this corpus. Explicit mode-2 normal-array geometry-miss and filter-reject cases are false, as is mode 2 with no normal array. A restricted native implementation must test the true case as explicit unsupported, not silently discard it, and must preserve lazy handling of the false cases.
- `callback_calls[]`: full result arrays, spring state, and record storage before and after each authentic callback.
- `append_events[]`: source pair index, local start/end, normal/surface, raw scene identity, material, and record count/capacity before/after the append helper.
- `trace`, `leaf_updates`, and `selections`: candidate/tree/leaf order, each geometric leaf hit with tested and retained fractions, and strict selected-result updates with pair/face identity.

The synthetic address map is deterministic. Candidate identities are normalized to 10, 11, and 12; query identity is 100. Callback record scene identities deliberately remain raw explicit scene-pointer words, because the original copies those exact words. Native consumers may initially restrict to all-zero endpoint scenes. No live pointer ownership is inferred.

Normal/point arrays and record storage are completely initialized beyond their active slots so untouched bytes can be checked. The hit flag array is 4-byte aligned but not 16-byte aligned, naturally selecting the original scalar-byte `memset` path even for multiple pairs. No `memset` call is intercepted. `3E02C0 = (0,0,0,1)` is an explicitly chosen data-global fixture input, not a claimed file-backed ELF constant.

The byte-audit read count includes observation snapshot reads, in addition to guest instruction reads. It should not be interpreted as a minimal guest-only data footprint. Both types of read still go through the strict initialized-byte check.

## Established behavior

### Per-candidate reset and ordering

`126F40` clears exactly the active hit flags before its inner inactive/sphere gates. An early rejection leaves count, fractions, selected fraction, normals, and points as prepared beforehand. After a passing sphere gate it sets selected fraction to 1, every active fraction to -1, and count to zero. It does not clear the normal/point arrays or selected geometric Vec4s at that boundary.

Consequently:

- Near candidate then far candidate ends at fraction .75; reversing them ends at .25. Each candidate gets a callback and its own result.
- Repeating the same candidate three times produces three callbacks and three records if capacity permits.
- Hit candidate followed by a geometric miss still returns true for the full query, but ends with count 0, hit flag 0, and fraction -1. Normal/point bytes from the earlier hit remain stale.
- Wrong object tag and empty-list cases preserve the previously initialized query current-candidate field, as original iteration does not write it.
- Unsupported candidate shapes 2, 3, 4, 5, 6, and `FFFFFFFF` return no geometry hit after the applicable reset side effects.

### Per-face and per-pair selection

Every intersecting face increments result count, including farther/equal hits that do not improve a pair's nearest fraction. Two coincident triangles for one pair therefore yield count 2 but only one callback append for that pair.

Both per-pair nearest replacement and selected-result replacement use strict comparisons. Coincident-face ties retain the first selected face. A near face followed by a farther face keeps the near face; reversing order updates from far to near. A hit exactly at fraction 1 sets the hit flag and emits a callback but never selects new geometric Vec4s because the selected fraction started at 1. Existing selected values are then transformed by the original dispatcher. The fraction-0 boundary preserves the original negative-zero word (`80000000`).

The output surface W is 1, normal W is 2, and selected correction W is 3 on the exercised successful tag-0 transform paths. Local-direction W follows the actual incoming VU state; its exact words are captured rather than synthesized from an assumed type-2 convention. The native `Type3RayBuffers48P` candidate-local start/direction buffers deliberately represent XYZ only, so native comparisons for these two scratch arrays must compare only XYZ. This does not relax any world-start/direction, normal, point, selected correction, or callback Vec4 W checks. No reproduction of unused guest scratch-register W carry is claimed by that native API.

### Authentic callback composition

Callbacks walk the signed spring `+7C` bound and inspect result hit flags by pair index; they do not iterate result hit count. Tests vary that bound, capacity 0–4, initial count at/above capacity, prior current-candidate state, material masks/name parsing, disabled callbacks, skipped categories, and repeated candidates. Record capacity suppresses writes without inventing a different return path.

The original callback's unusual accesses are preserved:

1. End XYZ is world start XYZ plus world direction XYZ multiplied componentwise by the **first three length words**, not by that pair's scalar length. One-pair cases explicitly back all three words; changing the otherwise stale second length changes the callback end Y.
2. Preparation consumes endpoint-scene metadata as two words per pair, but callback record `scene` copies metadata word `i`, not word `2*i`. The distinct-endpoint-scene case appends raw identities `D0000,D0400,D0000,D0400` for hit pairs 0 and 2 across two candidates.
3. Start and computed end pass through the original body-scene inverse transform. Their output W words are forced to 1.

## Verified VU correction and mixed-zero impact

The pre-correction 183-case corpus is preserved under `evidence/vuMinMax48P/type3Query48P-before/`; the legacy interpreter source is preserved by the independent VU audit. All 183 original expected objects remain exactly equal after the correction, excluding only the newly added `requires_mode2` marker. The generator does not overwrite archived or 48N/48O evidence.

Twenty-four additional complete queries vary opposite signed-zero endpoint coordinates and endpoint order, including zero-length preparations. All 24 exercise changed VU selected-lane words. Across them there are 64 lane-selection differences; 20 cases have 296 observed output-word differences, including repeated snapshots. Every observable change is `80000000` to `00000000` in world-center/query-sphere coordinates. Return values, control flow, hit flags/counts, fractions, normals, points, and callback records are unchanged. Four cases have internal lane changes but no observable output changes.

`vu-minmax-impact.json` records every changed instruction/lane and output path, with original operands and legacy/corrected words. Its legacy comparison explicitly enables only `Original.legacy_vu_minmax`; the scalar 48O correction stays enabled. The 24 diagnostic legacy reruns execute another 45,000 instructions, separate from the 849,765-instruction corrected corpus count. `--frozen` reproduces this impact report byte-for-byte as well as the fixture and executed-word manifest.

Two additional mode-2-with-normal-array cases establish lazy miss/filter-rejection behavior without running the unsupported interpolation helper. Together with the mixed-zero additions, that checkpoint contained 209 cases. No existing 183-case inputs or expected fields were rewritten to hide a delta.

## Zero-scene transform and boundary extension

A coverage review found that all candidate linear matrices in the initial supported zero-endpoint-scene subset were identity matrices. The source-derived typed 48I cases had nontrivial matrices, but their endpoint scenes lay outside the initial native API scope. The final extension therefore adds 36 bounded original-execution cases, with no original code, interpreter, or prior expected-output change:

- Thirty zero-scene full queries exercise six finite candidate-transform families: 90-degree Z rotation, 90-degree Y rotation, a binary32 `.8/.6` Z rotation, X reflection, nonuniform scale `(2,.5,1.5)`, and a finite shear. Each family covers a single hit, near/far ordering in both directions, a fraction-1 stale-selected transform, and multiple candidates combined with a nontrivial callback body transform.
- Every fraction-1 transform case hits without selecting a new geometric result, forcing the original dispatcher to transform stale selected normal/point/correction values. The `.8/.6` rotation uses zero candidate translation in this one boundary case to keep its endpoint exactly representable at the selected-fraction boundary; other scenarios retain translation `(3,-2,5)` and their exact original rounding. Near/far bit patterns are captured exactly, including ordinary binary32 rounding in non-exact rotations.
- Two equal-fraction cases use coincident triangles with disjoint vertex indices and different uniform vertex normals under mode 1. One face order must retain `(0,1,0,2)`; the reversed normal assignment must retain `(1,0,0,2)`. Both faces hit, but strict per-pair nearest selection preserves the first normal. This makes changing the per-pair `<` comparison to `<=` observably wrong.
- Three direct cases explicitly initialize hit count 7, selected fraction `.375`, and the first pair fraction `.625` at the `126F40` boundary. Inactive, disjoint-sphere, and exactly tangent spheres all clear active hit flags while preserving those stale values and normal/point/selected arrays. These are declared synthetic entry inputs, not attributed to preparation or to an original-instruction write.
- One full query uses a validated two-level tree with three interior nodes and four triangles, two hit pairs, and multiple candidates. The original visit trace reaches node IDs 0, 1, and 2 and validates deeper caller-stack traversal.

The exact 209-case checkpoint and its report/manifest are preserved in `evidence/type3Query48P/before-transform-extension/`, pinned by fixture SHA-256 `4cee7d2a492a75f34ac65c5823432fe957783979747f8066b459b0ce946c1a14`. The generator asserts equality of all 209 complete case objects, including inputs and expected outputs, before writing the final corpus. `transform-extension.json` records the extension counts and preservation assertion.

The original dispatcher uses transpose-style candidate transforms even for the scale/shear fixtures. These cases prove exact source behavior for finite inputs; they do not substitute a mathematical inverse or assert physical correctness of arbitrary nonrigid matrices. The original VU legacy/corrected comparison remains scoped to the preserved 183 baseline cases and 24 mixed-zero cases described above; no additional legacy-mode comparison is claimed for these 36 extension cases.

The final native-supported projection is 190 complete queries plus five direct dispatcher entries; 50 remaining cases require explicit scope-rejection tests. There are eleven meshes. The only `requires_mode2` case is still `normal-array-mode-2`.

## Deliberate native scope separation

Five complete original-only compound candidate-tag-1 cases cover zero/one/two identity records and both near/far record orders. They are retained as evidence with `shape_tag: 1`; they do not assert that a native compound implementation exists or is validated. A native tag-0 implementation should select its supported subset and explicitly reject tag 1 until separately implemented.

The same separation applies to nonzero endpoint-scene metadata and any unsupported normal-interpolation mode: the oracle evidence is broader than a restricted initial native API. Five direct-dispatch cases must not be reported as complete limited-list queries. This proof makes no production/live-registration, PC, RAM-retrieval, rendered-race, or live spring-ownership claim.

## Reproduce

From the project root:

```
python tools/gameplay/type3_query_fixtures48p.py
sha256sum evidence/type3Query48P/native-fixtures.json evidence/type3Query48P/executed-original-words.json
python tools/gameplay/type3_query_fixtures48p.py --frozen
sha256sum -c evidence/type3Query48P/nonfrozen-hashes.txt
```

Only the owned query script and `evidence/type3Query48P` query outputs are written. `map/` is independently maintained by the source mapper. No native binaries or test builds are run by this generator.

Two bounded opcode additions are local to the observational oracle subclass: authentic VMOVE lane copying (`127028`, independently decoded in the source map) and authentic MIPS SD for the original normal-interpolation helper call's stack argument. They execute the original fetched words; they do not replace game helper functions. All other instructions, including the complete original scalar `strlen`/`memset` paths, use the existing corrected interpreter.
