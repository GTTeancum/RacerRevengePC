# Current Rider19 dynamic-material lifetime wiring

Baseline: `b11246118e9859f83bd75a86f5f453288be4e749` in `/workspace/shared/rr-recovery-git`.
Prepared 2026-10-11. All implementation remains outside the checkout in `candidates/`; `preimages/` contains the exact baseline bytes. The parent owns integration. Integrate the coordinated candidate set together, not selected owner/population portions.

## Scope and disposition

This is an **active-path candidate** for the already admitted one-player Rider5/1 initial Rider19 attachment and whole-manager teardown. It replaces sticky material-registration flags with actual graph/helper-domain tokens, native per-row mutable pass instances, ordered population/group events, epoch-aware collector history, and explicit renderer-first source teardown. It is not another dormant helper or a full global registry conversion.

Public bind still prepares selected19. Optional19+20 private cold descriptors remain possible; actual graph loads and callback admission are restricted to pod27/character53. Directional movement, Confirm, page2, foreign owners, profile/audio/deferred extensions and20 live admission remain rejected. Static histories retain their established manager-lifetime treatment and cannot be retired by a dynamic token.

Whole-source review is recorded separately in `WHOLE-SOURCE-REVIEW.md` against its exact hash snapshot. This report claims source implementation and source inspection only, not build, test, runtime, renderer equivalence, or gameplay success.

## Exact ownership and construction changes

- The backend's stable ledger is declared before all token/instance holders. Canonical graph footprints are prepared from actual authenticated model assets. Every actual named binder helper has its own footprint and token; neither cold descriptor presence nor `shared_ptr` count acquires ownership.
- The session derives canonical static inventories from actual retained Rider widget records, stat/portrait records, and its actual Main predecessor material records, including that predecessor's authenticated particle record. The Main resource pointer is used synchronously, then cleared from the stored cold input; this does not extend Main widget ownership. Every dynamic canonical row must be disjoint. A direct bind without the required predecessor resource owner fails with resource status before dereferencing it.
- Scene adoption happens immediately after complete native graph construction and before character binding/entry sampling. Failed unpublished candidates and failed in-place reloads invalidate graph/IK state and release their exact adopted token, even when dynamic registration was never published.
- Binder adoption follows successful actual helper mutable-state initialization. The local backend candidate array performs explicit reverse-pair cleanup if a later helper or visibility membership fails; an unsuccessful release transfers the exact remaining helper to retained cleanup state.
- Dynamic detach now reports failure. A completed detach phase is remembered so graph-release retries never replay membership/helper removal. Failed completion or a live CPU lease leaves source owners untouched. Hide/show and membership alone do not create or destroy graph owners.

## Initial callback chronology and the one-shot guard

The one-shot `everAttached`/`graphFootprint` restriction preserves the authenticated admitted initial path; it is not an assumption that character `clearSelection` is harmless.

- Retained original `../members/rider/ui/uirider/rider.dat:260–294` has Group1 `KeepEnabled=F`,23 controls, and zero associations. Its first slot is Control16. Group2 at296–300 has `KeepEnabled=T`, but its sole Control25 at226–258 contains text/prompt/icon widgets, no pod or character. The retained file's SHA256 is `fc02bd935bcc540f69137a403f11751f71d9f9b87d764d03cc0df0117cff7cb0`.
- Baseline `original_ui_navigation50kg.cpp:39–50,62–79` first deselects unowned groups, uses `clearSelection` only for KeepEnabled groups, then deselects the selected group's controls and emits one final SELECT. Thus Group1 contributes two deselect sweeps and only one final Control16 select; no dynamic character clearSelection precedes that select.
- `original_ui_rider_selection50kg.cpp:110–138` plans exactly this group entry. The session's requested90 path only emits reset callbacks; dynamic reset updates shown metadata without loading a model. All graphs are uncreated at cold bind.
- `original_ui_rider_widgets50kg.cpp:139–154` expands each command in the actual control's authored widget order. Session cold admission verifies Control16 as27/79/109/53. The sole final SELECT therefore loads pod27, visits two texts, and loads character53. Each graph attaches once; the early deselect sweeps have no registered graph to unload.
- Character clearSelection retains its real loading behavior. An unexpected descriptor-requiring callback outside27/53 or a second actual lifetime is rejected rather than made safe by pretending it is just selection bookkeeping. Neither unexpected path occurs in the inspected original initial sequence.

These statements are a source/data read-through, not an executed navigation expansion.

## Population, capacity and grouping

The initial sealed batch is closed to two graph domains and at most two actual binder-helper domains. Its summed distinct-row occurrences are9. Before source construction, the cold seal proves a40-event upper bound, four owner slots, bounded canonical-row storage, and preallocates all live-handle vector capacity. This conservative bound covers9 acquires, at most8 distinct row populations, at most18 cleanup events, and four population/group boundaries. No reattachment, live20 extension, foreign factory or post-submission source acquisition is admitted.

Every acquisition additionally reserves two cleanup events per distinct footprint row, in both journal capacity and sequence capacity. Other mutations cannot consume that guarantee. The private population issuer fixes the exact table-sorted missing-row plan and reserves each row commit plus separate population/group boundaries before constructing a pass. Acquire/release and another population cannot interleave with its scan or awaited grouping.

For each row, the backend creates fresh native `OriginalUiMaterialBindingState50KG` objects in pass order from prevalidated immutable keys. Only after every pass succeeds does the private row commit issue a nonzero epoch. An incomplete candidate is destroyed without an epoch. Earlier successful rows remain retained and retire normally if a later row fails. Abort poisons new acquisition/population, unlocks exact reserved cleanup, and emits no fabricated success boundary.

Full population emits its own boundary. The concrete collector synchronously rebuilds its source groups at that parent+A4 call before character capture/IK. Only successful grouping permits the separate `groupRebuilt` event and coarse registry-generation advance. Thus the initial pod and character attachments produce two genuine ordered rebuilds even if no draw occurs between them. A group failure preserves populated epochs for cleanup.

The old bulk ledger populate API is now backend-private; there are no baseline runtime callers. New integration uses only the row-at-a-time issuer. Cold model/image admission never constructs a live epoch. Duplicate cold aliases undergo source/template/origin/archive/fallback checks plus canonical image-range and prepared-key compatibility checks. Live identity and state fields are private to the backend and collector.

## Collector and accepted-source state

Dynamic registration identity is `(ledger lifetime, canonical row, pass ordinal, epoch)`. Same-row aliases use the same current native row handle regardless of which alias supplied ownership. The collector validates actual pass provenance and the current canonical epoch; geometry still requires the actual attached model. Cold-only bindings retain upload/sampler policy and receive no live handle.

Lifecycle acceptance is transactional and separate from source submission. The consumer verifies owner-count/epoch chronology and exact retained retirement tokens. A population event imports the already-constructed native state, rather than lazily initializing replacement dynamic state during packet scheduling. Only an authenticated retirement removes that old dynamic epoch. Static histories and continuously positive shared rows survive.

Retirement-only consumption compacts registration history, remaps retained bindings, and invalidates the entire redirect/representative graph, preventing stale indices from addressing a different compacted record. It does not fabricate14D440. No subsequent rendering is possible in this bounded teardown path. A genuine later rebuild would be required by a future admitted extension.

Proposed draw state and per-row state vectors are prepared before source submission. `finishSourceSubmission` remains the sole accepted-publication commit; it validates the whole set and performs nonallocating swaps only for actual acceptance. Failed started submissions poison drawing while allowing authenticated teardown consumption. The backend removes retired native-instance retention and acknowledges a prefix only after the collector accepted it. Failed consumption leaves its cursor and the journal unacknowledged.

## Renderer retirement and cleanup

The actual renderer now supplies its own identity at the existing private `beginSourceSubmission` call. The collector records that exact renderer and its attempted source generation; it rejects another renderer. This one call-site signature change in `vehicle_window_gl50ds.cpp` adds no GL operation.

The frontend calls private session `teardownSource` before resetting a Rider session. It captures the exact collector watermark and ledger frontier, requires the same recorded renderer, invokes the existing real `retireOriginalUiSourceOwner`/`gl.finish` path, and requires its successful completion result. Only then does it drain CPU borrows and transfer the private same-owner/generation/frontier entitlement to the backend. Graph/helper cleanup, collector retirement consumption, native-row finalization and journal acknowledgement occur while all consumers and the ledger still exist.

Never-submitted cleanup is derived from the actual renderer submission entry watermark being zero. It is not inferred from a frame pointer, cold bank, selection Boolean or source-update count. A renderer failure releases no material owner. A later source-cleanup failure retains its accessible Manager/session/backend and reports failure; retry repeats genuine renderer retirement and then only unfinished cleanup phases. Successful source select/populate/draw effects are never replayed.

Destructors are not the first fallible cleanup boundary. If explicit cleanup is omitted or still fails and the caller nevertheless destroys the owner, the source-owning storage is intentionally retained. The frontend retains its whole storage too, preserving Manager callback contexts, session, collector, graph/helper tokens, ledger and cold/GPU/application pins together. **This is a deliberate terminal leak for contract misuse, not successful cleanup and not an accessible retry mechanism.** Normal `release`/`close` failures return without resetting those accessible owners. The borrowed external renderer must still obey its existing lifetime contract; the assistant does not claim ownership of or recoverability after destruction of that external object.

## Files and verification boundary

The coordinated set contains16 existing files: ledger, backend, scenes, helpers and dynamic owner header/source pairs; collector and session header/source pairs; frontend source; and the renderer's single submission call-site source. No new compilation unit, CMake entry or dependency change is needed. `integration.patch` uses repository-relative paths. `PREIMAGE-SHA256SUMS` and `CANDIDATE-SHA256SUMS` identify the bytes.

Source review covered cold failure, graph build/adoption/post-adoption failure, helper candidate failure, partial row population, grouping failure, post-attach character failure, failed/accepted source submission, renderer retirement failure, partial teardown retry, row/static separation, and the19-only gates. These are inspected paths, not executed scenarios.

No builds, tests, syntax checks, sanitizers, project parsers/importers/decoders, native/GL/gameplay execution, guest instruction processing or emulation were run. No Git publication or active-checkout edit was performed. The central test and Git publication remain held for explicit user approval about the legacy build dependency; no retry, workaround, or dependency change is authorized by this stage.

Unfinished broader work remains unchanged: full static/cross-manager lifetime conversion, intra-Rider mutation completion and batch reservation, genuine20 attachment/capture, corrected movement chronology, orientation/audio/RNG/profile/presentation/deferred joins, and execution/equivalence evidence. None is enabled by this bounded candidate.
