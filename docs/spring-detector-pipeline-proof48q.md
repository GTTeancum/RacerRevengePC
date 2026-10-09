# 48Q original supplied-list detector pipeline proof

## What this proves

`tools/gameplay/spring_detector_pipeline_fixtures48q.py` begins every case at original `295630`, rather than invoking the prepared query seam directly. Successful active cases run continuously through:

1. `28BAB0` point-world calculation, including authentic dirty-parentless `scene+C0` to `scene+100` refresh and flag update when required.
2. Detector bounds and paired endpoints, optional original `296490` spring reset, and original `11CA80` staging.
3. Original `122660` supplied-list query, original `11DD00` type-3 preparation, filtering and original `126F40` dispatch.
4. Original tree/leaf mesh code, static `294BB0` callback, and `2963E0` record appends where reached.
5. Natural return from the query and the enclosing detector.

No original instruction is replaced or patched. No helper call or return is intercepted. Expected values come only from executing original words, with passive observations. The native runtime, native expected values, saved RAM, inferred live ownership, and full synthetic RAM images are not used. Four-byte and vector reads all go through the existing strict initialized-byte interpreter. Its backing is `data=b''`, its allowed source-address set is empty, and every case has zero backing-source reads.

The normal replay verifies every loaded instruction and dispatch-table word against the authorized ELF `work/native/project/disc/SLUS_202.68`, SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Address translation is file offset `VA - 100000 + 80`. The detector ranges are individually pinned against the unchanged 48O map; the full query and callback words use the unchanged, hash-pinned 48P/48N frozen sources. No prior fixture or shared interpreter is modified.

The eleven explicit geometry input meshes are reused from pinned 48P fixture SHA-256 `a87159b1b858f6351b17de655152ad4d6fb69aed228132e1f5f27ea37b1c7f3f`. Only its `meshes` input field is consumed. Its case outputs do not drive generation, execution, or expected results. Object/filter/state/record/scene/stale-array values are explicit synthetic inputs, not reconstructions of a live guest snapshot.

## Outcome and numerical scope

The final corpus contains 170 cases: 161 natural detector returns, 142 full supplied-list queries (114 hits and 28 misses), 197 callbacks, and 948,607 original instructions. Nine probes stop at seven distinct boundaries. Four natural zero-shape-count queries are separately labeled nonfinite IEEE probes. The fixture SHA-256 is `f349aeef2a7260e485e1cd30120b897e676a505b562d37d003846f8535fa1edb`.

See `evidence/springDetectorPipeline48Q/report.json` for executed addresses/branches and source hashes. `run.log`, `frozen-run.log`, and `nonfrozen-hashes.txt` establish that both ELF-verified and frozen replay produce exactly the same fixture, executed-word manifest, and report.

Both return values are represented separately:

- `expected.query_returned` captures register V0 at `295A10`, immediately after the actual `122660` return. It is null when that query never returns or never runs.
- `expected.detector_returned` captures V0 only at natural detector return. Original `295A34` unconditionally assigns 1, so a query miss still gives detector return 1.

The normal fixture domain is individually rounded host IEEE binary32 finite geometry, including corrected scalar and VU signed-word MIN/MAX selection already established by 48O/48P. This is not proof of all physical EE/VU exceptional modes.

There are four explicit `numeric_scope: "nonfinite-IEEE-probe"` cases: positive point counts 1–4 with independent shape count zero. Original `11DD00` generates an infinite query radius under the existing interpreter, then naturally returns a query miss. These are diagnostic original-word executions, not finite-domain native compatibility cases. They must remain explicit scope rejections in an ordinary-finite native API. The no-pairs, disabled-query cases with signed point counts 0, -1, and INT_MIN do not run numeric preparation and are separately finite-scope gating cases.

## Input schema and original address bindings

`native-fixtures.json` has schema 1, four array slots, eleven meshes, and self-contained input/expected case objects. Its inherited 48P fields retain candidate order, filters, mesh IDs, candidate transforms, callback switch, normal mode, initial spring state/records, and initialized lengths/hit flags/fractions.

Additional `case.detector` fields:

- Gates: `spring_present`, `pairs_present`, `provider_present`, `provider_enabled`, `query_present`, `query_enabled`, `reset`, `explicit_list`, `shape_present`.
- Independent `shape_count`. Spring point count is `initial_state[7]`, representing spring `+7C`; these counts must not be conflated.
- `initial_query_type`, initial detector `bounds` eight raw words, detector `pairs` 32 words, query `staged` endpoints 32 words, and endpoint-scene `metadata` eight words.
- Four `point_records`, each with a fully initialized 24-word record, local point, cached scene world matrix, scene-local matrix, flag byte and opaque parent pointer. The initializer inserts that record's explicit scene pointer into record `+40`.
- `body_scene_binding`: -1 means the independent `body_world` scene; 0–3 means that exact detector point-scene slot. The binding is a caller input, not inferred ownership.
- Two explicit static-initializer flag bytes and their corresponding data words.

The query/shape/result/spring/record addresses retain the 48P deterministic map. Additional detector addresses are `44000` (detector), `45000` (point records), `46000` (paired buffer), `47000` (candidate-count cell), and `48000 + 400*i` (four point scenes). Point records and appended collision records are different buffers, representing spring `+80` and `+8C`, respectively.

Every input buffer has initialized inactive tails. This includes words that the original may preserve, copy opaquely, or consume through its unusual callback count/length behavior. An uninitialized read is an error; initialization never occurs in response to a missing read.

## Intermediate and final observations

`expected.initial` preserves pre-execution detector, scene, spring, record, ray and result state. `expected.detector` preserves the corresponding final detector/point-record/scene/bounds/pair/staging/static state. Final query fields and candidate effects use the inherited 48P schema: query flags/current candidate/copied vectors, candidate flags/copied vectors, full result selection, fractions/hit flags/normals/surfaces, all prepared ray arrays, full spring state and appended record storage.

`expected.stages` observes these original PCs without changing execution:

| PC | Stage | Position |
|---|---|---|
| `295944` | after_numeric | Point refresh/transforms and detector bounds finished |
| `295978` | after_reset | Optional authentic spring reset finished |
| `2959D4` | before_stage | Common pair-start assignment finished |
| `2959E4` | after_stage | Authentic `11CA80` returned |
| `122660` | query_entry | Original supplied-list query entered |
| `295A10` | query_return | Original supplied-list query returned |

Each stage contains detector, pair-result, and preparation snapshots. `after_prepare` observes the actual `11DD00` return inside the query. Inherited `calls`, `callback_calls`, `trace`, `leaf_updates`, `selections`, and `append_events` establish candidate order, tree/face order, strict tie behavior, callback before/after state and full records, and individual append/saturation inputs.

`body_scene_reads` captures the explicitly bound cached scene address and world matrix on entry to each actual callback. Snapshot reads use the same initialized-byte checks. Byte-audit read counts include passive observations and must not be described as a minimal guest-only data footprint.

## Coupling findings

### Preparation uses two independently bounded counts

Original detector point iteration and common pair-start writes use signed spring `+7C`. Original `11CA80` uses the independent shape `+00` count and writes two endpoint vectors and two zero metadata words for each shape pair. It sets query type 3 even if the shape pointer is absent, then returns without staging in that absent-shape case.

Thus:

- Shape count greater than point count stages explicitly initialized stale pair tails.
- Shape count less than point count leaves staged endpoint and metadata tails unchanged.
- Active endpoint-scene metadata is zero because detector passes literal A2=0 to `11CA80`; no type-2 adapter or invented scene metadata is needed.
- The callback still loops spring `+7C`, not shape count or result hit count. If point count exceeds shape count, initialized nonzero stale hit flags can append stale ray/normal/surface slots beyond the active query prefix.
- The callback reads metadata word `i`, not `2*i`. For point count 4 / shape count 1, pairs 2 and 3 copy untouched metadata words `12340002` and `12340003` into collision records. These are opaque copied words; no scene object is dereferenced for them. Zero-metadata preparation restrictions must distinguish the consumed active prefix from callback metadata payload tails.
- End reconstruction retains the 48P componentwise first-three-length-word behavior. Full detector execution does not make that formula a per-pair scalar multiply.

### Callback cached scene is bound after detector refresh

A dirty parentless point scene is refreshed before the query begins. If the body cached-scene pointer aliases that scene, the callback reads the newly refreshed world matrix. Passing the pre-refresh matrix into a separately invoked callback is observably incorrect.

`binding-refresh-distinguishing-0` and `-1` start their aliased point scenes with a rotated/translated cached world matrix and identity scene-local matrix. Authentic point refresh replaces the active cached matrix. Their records equal the independent-identity body-scene case. `binding-independent-old-matrix` instead binds an independent unchanged old matrix; its records differ. This contrasts explicit input bindings and does not imply an unproven game ownership relation.

The `scene-flags0-body-binding3` case binds a dirty scene outside the active two-point loop. That scene is not refreshed; the callback reads its old cached world. Thus neither unconditional refresh nor unconditional pre-refresh capture is correct. Prepared scenes preserve their cached matrix; prepared scenes with opaque parents do not traverse the parent.

### Ordered effects, capacity and ties

The corpus covers near/far order in both directions, equal candidates, repeated candidates, a hit followed by a geometric miss, filter/tag/sphere skips, disabled callback, skipped categories, absent material scene, prior spring/query candidate identities, reset on/off, record capacity 0/1/2/4/12, and initial counts 0/1/4.

Prepared and dirty-parentless scenes combine with rotations, a binary32 .8/.6 rotation, reflection, nonuniform scale, and shear. All final transform cases hit, so these are transform-plus-callback tests rather than merely rejection cases. Matrix behavior remains the original transpose-style arithmetic, not an invented mathematical inverse for nonrigid transforms.

The two single-candidate `distinct-normal-tie-8/9` cases use coincident faces with different first-face uniform vertex normals. Both faces hit, but the retained normal is observably determined by the first face. Separate multi-candidate mesh cases cover per-candidate reset and later overwrites. No handwritten output normal drives the execution.

## Honest stop boundaries

Provider probes cover paired and no-pair paths, including a disabled query: provider work occurs before the query-enabled gate. All fifteen selected detector gating, scene, and staging branches have both outcomes, asserted by the generator.

All probes begin at `295630`. They stop before executing the unsupported operation, without forcing its return or claiming enclosing completion:

| Stop PC | Boundary |
|---|---|
| `295740` | Provider call `2E83D0` |
| `28BAF4` | Dirty-parent scene refresh call `2897B0` |
| `2959EC` | No-pair world-registration call `11FF60` |
| `295A1C` | Unprovided-list/world query call `121920` |
| `1273A8` | Compound candidate handler after dispatch reset effects |
| `10EBF0` | Alternate normal helper call `10F580` |
| `122660` | Query entry with absent shape, before its invalid shape dereference |

The alternate-normal miss and filter-reject cases complete naturally without reaching the helper. Presence of a higher mode alone is not evidence that the unsupported helper is reached. `incomplete_dispatch` retains observed effects for compound and alternate-normal probes; it is not a successful candidate result.

## Reproduction

From the project root:

```sh
python tools/gameplay/spring_detector_pipeline_fixtures48q.py
sha256sum evidence/springDetectorPipeline48Q/native-fixtures.json evidence/springDetectorPipeline48Q/executed-original-words.json evidence/springDetectorPipeline48Q/report.json
python tools/gameplay/spring_detector_pipeline_fixtures48q.py --frozen
sha256sum -c evidence/springDetectorPipeline48Q/nonfrozen-hashes.txt
```

The generator writes only `evidence/springDetectorPipeline48Q`. This proof claims neither production/live registration nor a game tick, rendered race, PC access, guest-RAM retrieval, or runtime allocation behavior. Native integration and allocation/sanitizer checks are separate evidence.
