# RA retained cold inputs and actual pod-live context

Isolated source-only candidate against abbf73d. No central edits or execution checks. No roster-loop factoring, Course/SessionAssets admission, clock/manager construction, physics or renderer changes.

## Immutable navigation/tuning lease

Aggregate::borrowColdNavigation(diagnostic) returns shared_ptr<const ra_cold_sources50LA::ColdNavigationLease>. Only the aggregate constructs it after its actual source decode; repeated requests return the same lease. It strongly retains that exact source Owner. No alternate parser, source registry or mutable world escapes.

Accessors expose const references to actual map/AI navigation, map/AI metadata, original M-scratch lap markers, fresh create selection, AI tuning and impact authoring. All these specific record types contain inline scalar/array data; none exposes LiveZoneWorld or writable pointer-backed topology. bindColdTracks delegates the genuine existing function and verifies already-initialized caller progress; it does not initialize an AI or change C80/C88.

Retain the lease for every use of a returned reference or AiColdTrackBindings pointer. Aggregate close rejects while any external lease owner remains, then drops its own lease before source destruction. This protects immutable backing but does not establish runtime admission or allow borrowed references to outlive their lease.

## Genuine pod-live query binding

Aggregate::bindRuntimePodLiveQuerySlot(actualBank, slot, actualBindings, actualParameters, diagnostic) runs synchronously through the same private runtime bridge and actual Bank::visit. It is called after that slot's actual AI/source/recovery constructor binding, before final grid placement. It requires the existing recovery route, actual Bank pin, eight-slot sealed idle Bank, registered frame/bindings/parameters and694 membership lanes, sourcePrepared selected collision identity, actual Ready/original-AI state and frameFactor0. It cannot create a Ready body or spoof a source phase.

The bridge owns one const PodLiveQueryContext50IZ. Its references are the actual Storage lifecycle/world/workspace, actual recovery membership694, actual reserved pod reference, and the exact registered parameters.shape address. This is the correct shared arena context: Bank restores each slot's real parameter/member state at those stable addresses. Each slot receives the same stable context pointer in its genuinely registered bindings. No public context, Storage or query getter exists.

Addresses cannot be replaced. Each slot is one-shot; an unrelated nonnull binding rejects. Grid/fresh operations now require the actual prior binding and verify its restored pointer. Source shape comparisons use the same selected COL owner's local40/rootCenter/rootHalf checks as the existing recovery constructor. No query membership attachment occurs in this new operation.

## Partial failure and destruction

Context ownership is established before any saved-slot pointer publication. If allocation or validation fails, actual preceding state is retained; if a Bank visit throws after publication, its actual slot is saved by existing Bank behavior and the context remains. Forward failure is terminal, with existing membership cleanup still available.

Neither grid retirement nor fresh retirement clears podLiveQuery pointers: saved Bank slots could restore them. Runtime/aggregate close continues to reject while the real Bank pin is live. Only after actual Bank destruction has eliminated every saved slot may terminal close clear the remaining actual arena pointer, and only when it is exactly this context. A conflicting nonnull pointer after publication rejects close. If no pointer was ever published, rejection of an unrelated existing context does not claim or clear it.

The actual bindings/parameters/frame/recovery arena must stay alive through Bank destruction and subsequent checked bridge close. Destroying those borrows first is not retirement evidence. Existing fresh/grid/camera/Follow/wreck cleanup obligations remain unchanged. No new public retirement token, caller-drained flag or callback is introduced.

## File/build visibility and call order

- New tests/native/ra_cold_navigation_lease50la.h, included by existing aggregate header.
- Existing ra_cold_aggregate50la.h and ra_source_camera_ownership50la.h only otherwise.
- Header-only definitions enter the existing main TU through aggregate inclusion; no source/build-list addition is required.

Future actual caller sequence: retain navigation lease; use source selection/tuning in the existing per-slot constructor; after real bindSelectedCold call bindRuntimePodLiveQuerySlot; then actual grid and fresh membership operations; stop before unresolved trigger-dependent neutral physics. The full cold-roster caller itself remains to be factored.
