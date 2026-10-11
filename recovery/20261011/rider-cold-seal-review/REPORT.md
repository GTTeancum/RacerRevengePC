# Private Rider cold candidate and immutable bank seal

Baseline:9108f2851dd6c639437d7bf92610dc19193e4a2b. Source-only staged slice,2026-10-11. Root integrates; active checkout was not edited.

## Result

Plan A can be isolated from source-lifetime refactoring. This six-file patch moves all model/material/image population out of callback preflight and into the existing private session construction envelope, then seals the bank once. Public `OriginalUiRiderSession50KG::bind` still prepares only genuine19 widgets27/53. A private frontend-friend `bindCold` overload permits an explicit bounded19+20 immutable population; **no caller requests that mode in this patch**.

The seal proves immutable resource population only. It does not establish a source PSM lifetime, selected model, renderer retirement, movement destination or successful GPU upload. Actual registration flags, attachment/destruction, collector history, selected packet logic and all movement/Confirm/Back gates retain their previous behavior. The separate epoch/retirement/drain findings in `../resource-union-integration/REPORT.md` remain blocking before direction activation.

## Changed files

1. `src/game/original_ui_rider_dynamic_owner50kg.h/.cpp`
   - Adds the two-value immutable population enum and a private session-only `prepareCold` method.
   - Locally loads genuine immutable pod/character descriptors for27/53, or27/28/53/54 when explicitly requested.
   - Checks actual widget kinds, widget source identities, exact pod PSG identities, and exact character basename/MCF identities through the same authenticated archive object.
   - Commits descriptor pointers together only after all requested descriptor loads succeed; rejects repeat preparation, runtime-created models and non-cold widget state.
   - Callback preflight now validates already prepared descriptors and invokes the backend read-only validator. It does not load any descriptor. Only diagnostics and the existing synchronous reentrancy guard change during validation.

2. `src/game/original_ui_rider_backend50kg.h/.cpp`
   - `bind` establishes the archive/visibility owner but intentionally publishes no partial static-only image bank.
   - Adds private session-only `prepareColdAndSeal`, which builds all model material/image owners and one complete bank in local candidates. The exact19 pair is mandatory; the20 pair must be complete; all other cold dynamic descriptors reject.
   - Preserves original static and stat owners, the genuine binder image alias, same material runtime and same authenticated IPUM-cache pointer. Bank ordering remains static,stat,binder,then ascending actual model widget IDs.
   - Publishes the immutable candidate pointers/map/bank only after bank construction succeeds, then seals irreversibly. This happens while the local session is unobservable. Collector policy and GPU checks still occur before the frontend may expose the manager.
   - After sealing, callback preflight never constructs a bank or changes resource pointers, image mappings, bank generation or source registration. It validates exact descriptor object identity against the material-owner pin and rejects missing owners before callbacks.
   - Keeps initial completed-bank generation2, as before. Existing detach generation invalidation and source registry-generation behavior are untouched.
   - Protects bank mapping getters in the newly explicit bound-but-unsealed state by returning null. No frame begins without a bank.
   - Corrects the stale header comment that claimed source PSM lifetime survived arbitrary detach. This patch does not implement the missing lifetime ledger.

3. `platform/linux/native/original_ui_rider_session50kg.h/.cpp`
   - Public bind delegates to private bindCold(selected19).
   - Only the existing frontend friend can explicitly request the optional19+20 cold mode. There is no config/CLI/input toggle and no new active caller.
   - Verifies genuine original Control16/17 mappings and widgets from the already-authenticated selection data; keeps the23-control roster intact.
   - Retains entry plan19 and source profile/route guards. Prepares descriptors and seals the complete bank before the existing navigation preflight; that preflight is now resource-read-only.
   - Retains aperture/panel/composer binding and policy admission before moving the local session into `storage_`.

No material, collector, frontend, user_rotate or model_scene implementation file is changed. No build-system change or new compiled file is needed.

## Existing private GPU envelope, unchanged

The active frontend's `Storage::build` creates a private pending manager, calls session.bind, prepares its collector bank/samplers, creates the real GPU texture owner, binds that actual publication and only later admits the destination. Failure releases that private candidate using the existing cleanup path. This patch does not add an alternate early publication path and does not patch around collector/GPU rejection.

The optional union mode is deliberately unreachable through current public bind. A future authorized private caller can request it within the same candidate-manager/GPU envelope. Merely enabling that private call would warm images and policies; it would still leave source movement unavailable. Future row lifetime and renderer-drain integration must precede any movement activation.

## Original19 preservation by source inspection

The original Group1 has KeepEnabled=F. Existing initial entry planning deselects the roster, then selects only Control16; Group2 contains no dynamic character/pod widgets. Therefore descriptor-requiring callbacks on the admitted initial path need only widgets27/53, while show/hide/reset/deselect on other uncreated widgets still work without loading them.

Default cold load order stays pod27 then character53. Material image bank order remains static0,stat1,binder2,pod27,character53. No20 geometry/material/image owner is prepared in public bind. The removed preliminary static-only bank was previously private and replaced during cold preflight; the final default bank retains the same source owners/order and canonical identity scheme. This is a source equivalence argument, not an executed decode or render comparison.

When optional cold20 descriptors exist, the collector's existing sourceRegistered=false behavior leaves20-only rows outside source grouping until actual owners exist. A cold cable alias may refer to the same row populated by a real19 owner; that does not attach or register the20 graph. Shared images do not manufacture source references.

## Failure and ownership properties

- Invalid population, missing/mismatched member, asset/material/image-bank failure: rejects inside the local candidate. No active manager or live bank is touched.
- Descriptor preparation may finish before material preparation; both remain inside the same unobservable session candidate, whose destruction releases only cold descriptors. No dynamic graph was created, so no source detach/row retirement is invented.
- Backend material/bank candidates are local until the complete bank succeeds. Pointer commits afterward are shared-pointer moves and scalar assignments.
- Failed collector policy or actual GPU creation uses existing private candidate cleanup. No rollback of an already active publication is needed or claimed.
- Successful seal cannot be reopened by callbacks, including a callback selecting an unprepared rider. Missing descriptor/identity/mapping fails before callback execution.
- The material owner pins the cold descriptor referenced by the backend's exact `coldModel` identity; this raw identity pointer is not a source lifetime token.
- No character entry sample, runtime model graph, attachment, membership, binder helper/clock, IK, selected state, profile write, sound, RNG or deferred video work is executed by either cold method.

## Deliberately unresolved

The existing sticky source registration flags, source-row zero gaps, shared cable/binder retirement, row/pass/epoch history, actual GPU completion before source destruction, movement callback chronology and side-effect consumers are not changed. The current19 lifetime remains the only active selected population. Do not interpret the seal as permission to enable19↔20, clear Confirm readiness or reuse destroyed source material histories.

This slice also does not reserve every allocation needed by actual runtime attachment. It is an immutable resource-bank boundary, not a complete movement transaction. The existing source selection rejection remains before that work.

## Verification and artifacts

No build, test, syntax check, project parser/importer/decoder, native/guest launch, GL probe, sanitizer or emulation ran. Inspection and editing were source-only. Calls to existing loaders are staged source for a later authorized build/run; they were not executed here.

- `preimages/` and `SOURCE_PREIMAGES.json`: exact six-file baseline bytes and SHA256 values.
- `staged/` and `STAGED_FILES.json`: final source files and corresponding hashes.
- `cold-union-seal.patch`: unified patch against those preimages.
- `INDEPENDENT-REVIEW.md`: separate source review; consult its exact file hashes before integration.
- `DELIVERABLE-MANIFEST.json` and `SHA256SUMS`: final package identities and validation limits.

Root should apply only after reconciling any newer overlapping source edits. The report does not authorize executing any validation command.
