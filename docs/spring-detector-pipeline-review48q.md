# Independent composition review: 48Q

## Review boundary

This review covers `spring_detector_pipeline48q.{h,cpp}` and the additive borrowed-context entry in `type3_query48p.{h,cpp}`. It also reads their retained 48O/48P callees, original detector instruction map, callback map documentation, current original-execution fixture generator, and native test sources. It does not replace the original-instruction proof, execute a game, use RAM, or establish live ownership. The reviewer did not run parallel builds; final coordinated build/run results were independently reconciled against current sources and their recorded artifact hashes.

The original instruction evidence is tied to ELF SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

## Findings

No blocking runtime composition defect was found in the reviewed source. Two contract clarifications were raised:

1. `bodySceneIndex` chooses the cached matrix backing; `query.bodyScenePresent` remains the independent logical presence gate. A valid binding does not turn a logically absent body/scene into a present one. This interpretation was confirmed by the integration owner; the header now states it explicitly, and native tests cover consumed-hit rejection plus unused empty-list/skip behavior.
2. Registration and world-query boundaries are committed prefix outcomes, not transactional rejections. The tests document initially grouped them with unsupported errors. The current tests document has been corrected to describe those categories separately.

Both clarifications are resolved. Neither was evidence of a numerical runtime mismatch. Final source and verification evidence reconcile cleanly as recorded below.

## Composition and control flow

- Detector preparation runs once. Only `LimitedQueryBoundary` continues into the supplied-list type-3 query.
- Prepared spring state, independently supplied `shapePairCount`, staged endpoints, and staged metadata feed the query. The query's borrowed template is not copied or modified.
- The execution context forces `resetSpring=false` and `entry=LimitedList`, preventing a second reset and preventing the template's dispatcher flag from bypassing required preparation/filtering/callback work.
- All six shadowed 48P fields come exclusively from context: spring state, body matrix, pair count, reset flag, body-present flag, and entry. The retained 48P entry constructs that context from its original input fields and forwards to the same implementation.
- The detector's return value is separate from `query.hasAnyHit`. Complete hits and misses, `NoSpring`, and `QueryDisabled` mark a natural detector return of 1. Registration/world boundaries leave `detectorReturned=false` and do not invent a return value from an unexecuted continuation.
- Reached providers, dirty parented scene refresh, absent query shape on the supplied-list continuation, compound shape, dynamic/response callbacks, unsupported recursion, and unsupported normal/interpolation work retain explicit error boundaries. Unreached callback and geometry branches retain their lazy behavior.
- Positive shape count is required by retained 48P's ordinary finite domain. Zero-shape original IEEE probes do not establish supported finite native completion.

## Scene and count semantics

Original callback 294BB0 reads the cached scene+100 matrix and does not refresh it. Thus the 48Q scene binding correctly reads `detectorPrepared.scenes[index].world`: it observes detector-triggered refresh when that scene was visited, and preserves a stale cache when the detector never visited it. A second refresh would be incorrect.

The detector's signed spring point count and the shape's pair count remain independent. Only active detector points change pair endpoints/AABB; staging consumes the independent shape count and may legitimately include initialized stale pair tails. Query preparation and narrowphase use shape count, while the callback uses the spring point count. Consequently callback hit masks beyond the active shape count can be stale source-backed values, and the first three length words remain required for any consumed callback hit. The implementation does not normalize these unusual original semantics.

## Storage, aliases, and transactions

The 48Q guard enumerates exactly 47 writable spans: three detector sets of four spans, three query sets of eleven spans, the traversal stack, and the whole public result. It rejects pairwise overlap before scratch writes.

Each writable span uses full declared capacity. Every such span is also checked against input/source/workspace/destination descriptors; point, scene, pair, staging, metadata, ray and record source spans; candidate descriptors; filter-vector backing; mesh descriptors and vector backing; and scene-name backing. The 48Q guard deliberately uses nested vector capacities, protecting unused allocated tails across both stages. Read/read aliasing remains permitted.

The context-aware 48P implementation adds the complete context object to its immutable descriptor guard. Its own nested-vector checks retain existing 48P size-based behavior; 48Q's stronger outer capacity guard supplies the cross-stage full-backing guarantee.

Null, alignment, multiplication overflow and address-wrap checks occur before span arithmetic or writes. A zero count ignores its pointer. Caller-backed storage and valid owning-vector objects remain preconditions; a numeric span check cannot prove allocation ownership.

The detector commits only into private prepared workspace. The query likewise commits only into prepared workspace. No fallible operation follows the final destination copies. Errors can alter scratch/prepared storage but cannot partially commit public detector/query arrays or any byte of the public result. Skipped query execution copies the supplied rays/records unchanged and leaves candidate effects untouched. Source-backed stale tails are copied; extra destination capacity remains untouched.

## Runtime cost properties

Source review finds no dynamic allocation, owning-filter copy, vector growth, recursion, thread, lock, or mutable global cache in the new composition or borrowed-context seam. The guard has fixed stack size independent of input counts. Runtime traversal, filtering and copy work remain bounded by caller counts/capacities and inherited visit limits. These observations do not substitute for allocation-hook, sanitizer, or timing results.

Strict binary32 arithmetic is inherited from the existing components; finite consumed XYZ/intermediate checks do not impose invented restrictions on ignored matrix W lanes or copied record payloads. Stable rounding environment and strict compiler flags remain necessary. No target-frame-rate, total console memory, or physical EE exceptional-arithmetic claim follows from host microbenchmarks.

## Final evidence reconciliation

Final review completed on 2026-10-06. No blocking runtime defect, unresolved contract issue, or input/artifact hash mismatch remains in this review's scope.

The following final reports all have `status=passed`, empty `changed_inputs`, successful test return codes, and input fingerprints matching current checkout bytes:

- `detectorpipeline48q-final/report.json`: 690,699 pipeline checks plus 364,189 poisoned-context checks, 1,054,888 total, zero failures.
- `detectorpipeline48q-final-asan-ubsan/report.json`: the same 1,054,888 checks, zero failures, AddressSanitizer/UndefinedBehaviorSanitizer enabled and LeakSanitizer explicitly disabled.
- `type3query48q-regression/report.json` and `type3query48q-regression-asan-ubsan/report.json`: retained callback, leaf, node, preparation, and old-entry query suites each pass. Their check counts are 105,407; 22,604; 8,968; 35,169; and 341,118 respectively, 513,266 per configuration. The old entry retains all 190 full, 5 dispatcher, and 50 transactional-rejection fixture classifications.
- `detector48q-regression/report.json` and `detector48q-regression-asan-ubsan/report.json`: retained staging/sphere and detector suites pass 20,446 and 62,885 checks respectively.

The reviewer also rehashed all report-addressed binaries and logs: 54 artifacts per final pipeline report, 60 per retained type-3 report, and 26 per retained detector report. All match. The separate final context reports and six compiled one-field wrong-source sensitivity controls also match their current input fingerprints; all six controls are detected by normal failed assertions rather than build errors or crashes.

Pipeline native coverage contains 159 committed original projections and 11 explicit original-only rejections. The committed set includes 8 `NoSpring`, 11 `QueryDisabled`, and the two named prefix boundaries. There are 3,863 safety cases, including 3,510 alias cases, plus 14 wiring cases. All instrumented runtime calls report zero allocation. Each configuration includes 28,000 serial reuse calls; this is a component observation, not a target-hardware or frame-performance claim.

The current original full-call fixture and all 11 proof-report source fingerprints match. Its 170 cases comprise 166 ordinary-finite cases and four explicitly labeled zero-shape IEEE probes. Of the 161 natural returns, every detector return is 1; all 28 query misses independently retain detector return 1. The reviewer independently rehashed the authorized ELF and compared every manifest entry: all 2,619 executed instruction words and all 15 dispatch-table entries match the ELF bytes exactly. No game/RAM execution was used in this verification.

### Exact reviewed fingerprints

- `src/game/spring_detector_pipeline48q.h`: `30d4b26487e438fd1bdd542aeea05cd7b4b81fef629b0f34ea0cdc850a05263f`
- `src/game/spring_detector_pipeline48q.cpp`: `0bb6664bbaf3cf326592080b09ad6fec6cfb3835e10eb4d88f7a7e59d8963702`
- `src/game/type3_query48p.h`: `d6dedf99041035e9ca3acc2110254fc7eb0055a021fd55eceeb6bd8020b7474b`
- `src/game/type3_query48p.cpp`: `a78d72482853fa3f6dd07295574ef9a6322814c35448fdb75cb5cc5afb473483`
- `tests/native/spring_detector_pipeline48q.cpp`: `6b8b33ceb983a2d4e5a59a04b5cd440927069a67e989a1e2225655125e88e96d`
- `tests/native/type3_context48q.cpp`: `86eb6047e4260b6b3d9f014acf29d618c7737125fe479fd8d5c51365ec2483e4`
- `tests/native/type3_query48p.cpp`: `019503b55f84fa870f90f1c98510cff3c8de564832b28bdd07b96a2ee6429f55`
- `evidence/springDetectorPipeline48Q/native-fixtures.json`: `f349aeef2a7260e485e1cd30120b897e676a505b562d37d003846f8535fa1edb`
- `evidence/springDetectorPipeline48Q/report.json`: `b461d075cd25aac359362d5e143ede4252caf47703651a8b712e6290f521897e`
- `evidence/springDetectorPipeline48Q/executed-original-words.json`: `a290f70a65b3146de2e359496554ab4b8745a922f323eca707f68b1741c236ca`

Final report fingerprints, before packaging:

- `detectorpipeline48q-final/report.json`: `ea46a281c2a5e896774cb060f9f39c7d3f6546b80f3ea8166bad81ea37695598`
- `detectorpipeline48q-final-asan-ubsan/report.json`: `85a036fc9d0562551c66a451a0c9cbe299c766932bcbc3982869a21f4c2440ca`
- `type3query48q-regression/report.json`: `74fa89bfb72331a50f615e0f6822431cd98b0920d63e7db1e3903521ba5f0cbf`
- `type3query48q-regression-asan-ubsan/report.json`: `d25fe00e0e14fa547a81e21f521bd31546a13346a368b9814754ec1c30f0cd8c`
- `detector48q-regression/report.json`: `108aaa7c17abe6c1096f06d7f91f84d01d28d8ef1580bca86eb782844e86db5a`
- `detector48q-regression-asan-ubsan/report.json`: `ccd984db653f96131e55722d5ae9c342f2cc03d16eeaa858c5c1267f2430b5fb`

This review is frozen for packaging. A later change to reviewed runtime, fixture, or test bytes requires reconciliation or rerunning the affected checks; source review alone cannot extend the recorded evidence.
