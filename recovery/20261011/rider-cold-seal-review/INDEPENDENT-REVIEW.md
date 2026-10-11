# Independent source-only review: Rider cold candidate and seal

Reviewed 2026-10-11 against baseline `9108f2851dd6c639437d7bf92610dc19193e4a2b`.

## Disposition

**No remaining blocking defect found in the final six-file staged slice by source inspection.** The slice is a bounded immutable-resource preparation/seal refactor. Public session binding remains Rider19-only, no current frontend call opts into Rider19+20, and directional movement and Confirm stay rejected. This is not evidence that the optional union decodes, fits all capacity/budget limits, uploads, draws, or supports movement.

An initially inspected version exposed a bound-but-unsealed backend with null-bank dereferences in `staticImage` and `binderImage`. The author added guards while this review was in progress. I reread the corrected current bytes: all three mapping accessors now require `storage_->bank` (staged backend `.cpp:199-207`). The original callback generation-overflow rejection is also present (`:121`). Neither issue remains open in the reviewed hashes below.

I did not edit implementation. Review used text reads/searches/diffs, retained original text members, repository identity reads, and SHA256 checks only. No build, test, syntax check, project parser, importer, texture/geometry decoder, native or guest launch, GL probe, sanitizer, or emulation was run.

## Exact reviewed files

Paths below are relative to `staged/`; SHA256 identifies the final reviewed bytes.

- `src/game/original_ui_rider_backend50kg.cpp`: `63ce0e8f63bfbfe79a397bf6f4b2b876b9dbf801d990c6546956b8ff87735c95`
- `src/game/original_ui_rider_backend50kg.h`: `20e3dc522af61fb957f907af89379ac56a8f39623673c21914f33ac1cf4a8f23`
- `src/game/original_ui_rider_dynamic_owner50kg.cpp`: `63fe42fd909ddd4ccc13315b7171ae527bd363dd8c1efaa56b7fdf80c5a2caee`
- `src/game/original_ui_rider_dynamic_owner50kg.h`: `64153326a1ca689ec547b4f9afa6766fe6eadd38a838b392118784c5584ce2f6`
- `platform/linux/native/original_ui_rider_session50kg.cpp`: `9ed8fc0a77ee5046c09a4cbcb872b12506fc68e4969b1d79d97d2433cb8025f6`
- `platform/linux/native/original_ui_rider_session50kg.h`: `3b1f13a9735f19e96c08f06b6e23954f1d75b6f62742fa9b275c3a8746e7da0e`

All six `preimages/` SHA256 values matched `SOURCE_PREIMAGES.json`. Supporting source files were read in the retained repository at the stated baseline; a read-only diff-name query reported no working-tree changes for the supporting files used below. The prior `../resource-union-integration/REPORT.md` hazards were reviewed first.

## 1. Candidate ownership, publication, and cleanup

The new preparation calls execute on the local `std::unique_ptr<Storage> s` inside session `bindCold`, before assignment to session `storage_` (`session.cpp:320,371-389`). The dynamic owner and backend belong to that same private storage. The only preparer friend of both is the session class. The session's union-selecting `bindCold` is itself private, with the existing frontend class friendship; public `bind` supplies `selected19` directly (`:295-302`). No active caller selects the union enum.

The frontend still allocates distinct pending-manager Rider resources, stats, binder assets, transition, and session before GPU creation (`original_ui_frontend50kg.cpp:310-375`). Its current session call is public `bind`, so it cannot accidentally opt into the union. Its build path requires sampler policy, GPU `create`, publication binding, and `Manager::prepared` before reporting success. Failed destination preparation invokes the existing pending-manager cleanup (`:417-428`); this slice does not replace or mutate the active manager or active backend bank. This is the verified construction envelope. A future explicit union caller must remain inside it; the CPU seal alone is not GPU acceptance.

Dynamic `prepareCold` builds all requested assets in local arrays and checks their authored identity before any entry pointer is changed (`dynamic_owner.cpp:71-113`). Any loader failure or exception discards those locals. The eventual commit consists of shared-pointer moves into previously empty entries, followed by the one-way flag. Backend preparation similarly constructs all material owners, owner-index mapping, and bank locally; publication into its private storage occurs only after `bank->load` succeeds (`backend.cpp:64-114`). There is no entry-by-entry live resource publication on the callback path.

If backend, navigation preflight, aperture, panel, or collector admission subsequently fails, the local session storage is destroyed. Its explicit destructor drains collector before dynamic, and member declaration order keeps backend and cold resource inputs alive through dynamic cleanup (`session.cpp:75-101`). No dynamic model/helper has been registered during cold preparation, so dynamic drain has no actual model detach to perform at this stage. On later frontend GPU failure, existing cleanup closes the candidate GPU and releases its session; the review does not claim an observed GPU-cleanup result.

`coldModel` is a native descriptor identity, not an original source-lifetime token. The pointer is retained by dynamic shared ownership and also by `OriginalUiRiderModelMaterials50KG::Storage::model` (`original_ui_rider_model_materials50kg.cpp:13-18,47`). Comparing it after sealing therefore does not introduce a dangling descriptor pointer in this owner arrangement. No `shared_ptr` count is used as an original source reference count.

## 2. Default population and the exact optional union

The default path loads only widgets27 and53. The union additionally loads widgets28 and54. The hardcoded bounded set is cross-checked against genuine source membership before construction:

- Control16, Group1 slot1, PodIndex19, widgets `[27,79,109,53]`.
- Control17, Group1 slot2, PodIndex20, widgets `[28,80,110,54]`.

The session verifies the23-control roster and these actual memberships, while the descriptor loader verifies exact widget provenance and pod model or character MCF identity (`session.cpp:321-340`; `dynamic_owner.cpp:83-105`). The underlying character loader derives PSG, VUT, and all six motions from the authenticated MCF and requires the expected six names. Retained `20c.mcf` explicitly names `20c.slt`, `20c.vut`, and the six20c motions. No alternate/generated model is introduced.

Original member text/hash checks used here:

- `rider.dat`: `fc02bd935bcc540f69137a403f11751f71d9f9b87d764d03cc0df0117cff7cb0`
- `widgets.dat`: `6e5fab48528286a04a6782362457cc66ef0ae478470a4cb77302b871db0f2bd5`
- `20c.mcf`: `4a0af81832b8bf21d298afaf0ecd99b5625d8740a84d0d2aee46ef13ff7b3303`

These match the retained original-member identity evidence. The source identities are checked by the program's existing authenticated archive/asset loaders, not by admitting basenames alone.

The bank still retains the original static owner0 and stat owner1. Binder images are appended once as owner2 using the existing aliasing shared pointer. Model image owners then follow ascending widget ID: default `[27,53]`, optional union `[27,28,53,54]` (`backend.cpp:94-104`). Thus default owner population/order matches the former initial19 preflight bank; optional preparation does not double the selected-frame geometry capacity. Bank generation advances once from1 to2 at seal, as the former initial dynamic population did. Unchanged detach behavior can still advance backend generation; sealing does not promise that every later generation number is frozen.

Model-material enumeration remains driven by actual scene material slots, preserving repeated19_2/19c_2 references and20c's authored slot ordering. The prior retained fixed-record evidence identifies20 pod materials as20_1,20_2,cable,20_shdw and character materials as20c_1,20c_2,20c_4,20c_3,20c_5. The staged slice does not invent20_4 or19_3. Original20_1/20_2 PSM text still selects their own body image and shared specmap1 second pass;20_shdw selects its own shadow image. This review does not re-run the scene or image decoder.

## 3. Shared sources and the IPUM path

Existing static/background/font/stat owners are retained rather than replaced. The optional20 background and portrait are checked for availability but were already supplied by the original static/stat loaders. Binder immutable assets remain shared; actual binder helper allocation and initialization still occur only in backend attach for each real named pair (`backend.cpp:145-160`). No binder animation instance is warmed by the union.

The exact `cold.ipumCache` pointer is forwarded into `backend.staticImages` and then into every model-material load and the complete bank load (`session.cpp:355-358`; `backend.cpp:85-89,94-106`). The existing19c_1 authenticated cache path and kind2 policy are unchanged. No fake readiness, replacement decode, or allowlist expansion was introduced.

`OriginalUiImageBank50KG` still deduplicates by authenticated member identity, PST byte offset/length, and CEL identity; it retains the original provenance and full decoded pixel/mip checks (`original_ui_image_bank50kg.cpp:19-29,41-95`). Shared cable/specmap1/binder sources therefore use the established bank remap, not widget IDs, owner-local image IDs, matching pixels, or filenames. Seven owners fit the owner-count bound; the candidate must still independently pass canonical-image capacity, decoding, material/sampler policy, and GPU budget checks.

## 4. Sealed callback behavior and default runtime coverage

Dynamic preflight now checks `coldPrepared`, validates the complete callback batch, and rejects any required absent pod/character descriptor. Backend preflight requires the seal and bank and checks each needed model against its exact retained descriptor pointer, material owner, owner index, and mapping (`dynamic_owner.cpp:115-131`; `backend.cpp:116-134`). Neither preflight decodes, allocates model/material owners, appends bank owners, changes the bank pointer, publishes a resource generation, attaches a graph, or performs source registration. Their diagnostic/reentrancy writes do not change resource publication. The restored generation limit and frame-lease checks still reject invalid conditions.

The current initial19 lifecycle does not secretly require other model descriptors:

- Requested90 resets the entire roster, which affects widget/backend shown metadata without calling `loadModel`.
- Authenticated Rider Group1 has `KeepEnabled=F` (`rider.dat:260-294`), so group-entry sweeps issue deselect, not character clearSelection.
- Group2 is `KeepEnabled=T`, but its sole Control25 contains text/prompt/icon widgets, no characters (`rider.dat:226-258,296-300`). Group0 also has no models.
- Group-entry selection therefore loads only the actual Control16 pod27 and character53. Deselect callbacks for other unloaded models do not require descriptors or invoke backend detach.
- Appearance/group enable, normal update, closing/Back, and current teardown add no new model-load population. Updates return early for unregistered models. Directional input still rejects before callback preflight.

The distinction is important: character clearSelection really does load its model and must reject if that model is outside the seal. The staged guard preserves that fail-closed behavior rather than broadening the seal to satisfy an unexpected callback.

## 5. Source ownership, history, and navigation gates

Cold preparation leaves every dynamic `registered`, `everLoaded`, backend `materialsRegistered`, binder-registration flag, attachment, visibility membership, and helper pointer untouched. Backend preparation explicitly rejects an already-used owner and does not invoke the actual graph/material synchronization path. Loaders prepare immutable descriptor data; the live graph, entry sample, IK capture, source synchronization, and binder initialization remain in their original load/attach sequence.

The actual dynamic load/unload/apply/drain implementation is unchanged. Backend attach only adds a seal prerequisite; its existing material-registration event remains at the real successful attachment boundary. Detach and its sticky historical flags remain unchanged. The misleading header assertion that PSMs survive detach was replaced by an explicit warning that the existing registration bookkeeping is not a row-lifetime ledger.

The collector is unchanged. It still receives dynamic registration flags when building model policies, keeps cold-only bindings out of source scheduling, and promotes an exact alias only from an already registered row with matching pass provenance. Cold20-only rows do not become live merely because their images are resident. Shared cable can become a policy alias of an actual live19 cable row; that is not a new20 graph or source lifetime. The prior epoch/zero-reference gap hazards remain relevant to future detach/reselect work. The retained-history disappearance safeguard and accepted-source-submission history update remain intact (`original_ui_rider_draw_contribution50kg.cpp:213-275,570-645,736-741`).

Session movement rejection, Confirm rejection, and the existing19-only Back proposal/acceptance checks are byte-unchanged outside the shifted bind block (`session.cpp:470-505` and subsequent route methods). No selected-owner traversal, packet owner, membership, source registration/detach/history, pose/motion, profile, audio, or deferred-work code is relaxed by this slice.

## Limits and remaining work

This review supports the staged cold-resource boundary only. It does not resolve the earlier report's canonical row epochs, zero-gap retirement, failed actual attachment accounting, real renderer completion before model destruction, movement chronology, selected20 admission, orientation transfer, audio/deferred consumers, or profile semantics. None is supplied by a sealed image bank. Those gates must remain closed until separately implemented and verified.

The new standalone backend bound/unsealed state is deliberate and its public mapping accessors are now safe. Only the session has authority to complete preparation; repository source search found no other concrete backend/dynamic-owner construction caller needing adaptation. Future standalone users must not expect `bind` alone to supply a bank.

C++ access/structure was inspected only: the dynamic header forward-declares the session in `rr::platform`, the backend includes that declaration, friendship covers the session's private preparation calls, and the new enum is visible through existing includes. This is not compilation or syntax-check evidence. The optional union remains uncalled and all runtime success claims remain unverified.
