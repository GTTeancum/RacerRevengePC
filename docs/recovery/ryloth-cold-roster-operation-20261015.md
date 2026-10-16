# Inactive RA retained cold roster operation

Base copies: central source as read immediately after ca13884 / unrelated baseline 5c0fbae. The production per-slot loop is unchanged. This patch adds only an inactive compile inclusion to its translation unit.

## Callable sequence and real owners

1. Construct the existing eight Rig and collision Owner instances through the existing authenticated roster loader. Preserve the explicit endpoint-W policy: this patch neither supplies nor recovers those words.
2. `Resources::adopt(actualRigs, actualCollisions, error)` authenticates each actual pod archive using compiled `original_player_identity50IK` pins; compares retained PHY/PSG/MCF/motion and all four private COL byte arrays against those originals; retains the original AI member and the six fixed-pin existing effect members. Only after successful validation does it move the existing unique pointers into a stable private array. Object addresses do not change. The caller must relinquish mutable access, and must not have published the old array itself to grid/camera owners.
3. Capture `FreshProbe::capture(actualFrame)` at the actual pre-enable constructor point, requiring Ready, frameFactor zero, disabled terrain probe and zero probe100. This captures real numeric fields, rather than accepting replacement snapshots.
4. Aggregate `prepareRuntimeRosterQueryStorage(resources, actualRecovery, actualRecoveryGlobals, actualTerrain, actualMovement, actualWreckReservations, diagnostic)` must precede every Storage attempt/static-registry route. It strongly retains the actual adopted roster before Storage mutation. It keeps the actual terrain/movement identities and checks them against subsequent real query binding. Recovery SourceGlobals are the actual record subsequently passed to existing recovery prepareStorage, not a readiness receipt.
5. Existing actual arena/recovery construction and `bindRuntimeQueryOwners`, existing registration plus `registerRuntimeQueryWorldOwners`, then Bank seal8. There is no alternate Fixture/Bank/world allocation in this operation. All borrowed arena objects and vector capacities remain fixed.
6. Stage the genuine same-Application track-particle manager before final world image population. `Operation::bind(actualAggregate, actualApplication, actualBank, Arena, freshProbe, diagnostic)` requires that actual manager generation, Application identity, entities, domain, motion library, fresh domain collector state, registered lanes and same private roster. It acquires the aggregate's original immutable navigation lease. Application's stack shell/directory storage and actual arena must outlive checked close; its concrete shared entity/material/motion owners are strongly retained.
7. `constructSlot(actualBank, slot, diagnostic)` runs strictly slot0 through7, once, on actual Ready/frameFactor0 and zero actual Bank completedSteps. It never sets Ready. The operation remains inactive in the main program.

## Exact per-slot order

The source is the current `terrain_interactive_drive50du.cpp` per-slot Bank visit, from rig installation through contact binding, preceding HUD-name copy/grid.

- First actual Bank visit: right/left/optional bottom right/bottom left/chariot prepareEarly; sibling joins; Rig install; source handling bind; parse retained actual AI; apply actual RA class; actual all-AI creation identity/runtime; manager packing reference; same-source navigation bind; original class58/appByte/rate/lap timing stores; handling-root placement; model rebase; effect names; effect owner construction/Stop sequence; actual main/secondary resource and private native identity assignment; terrain authoring/install; selected recovery source/exclusion/model bind.
- After that visit returns, actual Bank scalar ReadHandles verify the saved main/secondary identity lanes. Then the existing genuine aggregate pod-context binder owns its own Bank visit. No nested visit is introduced.
- Finishing actual Bank visit: restore the captured pre-enable probe fields; original enableVehicleTerrain; existing Visual, Cables and cold-flame construction; real contact binding.
- Only the real finishing return records private slot completion; the retained-roster grid route rejects its absence. Failures invalidate all forward roster completion and retain partial model/material/effect state.

No HUD names, finish tuning, Ground.lift workaround, logging, grid operation, neutral physics tick, Countdown, camera prepare, renderer publication or gameplay admission is included. Exact `{0,0}` opaque effect inputs are the existing native control-projection policy; this extraction does not upgrade them to recovered original opaque fields. Visual construction is the existing cold decoder/cache path, not GPU publication.

## Native identity policy

The existing private RA runtime bridge reserves 16 nonzero uint32 native collider tokens before exclusive world lease/Storage mutation. It scans for free identities against actual source static filters, all fixed auxiliary identities, actual terrain/recovery/movement identity inputs and planned wreck identities. Overflow/exhaustion rejects. Each pair retains its actual collision roster association. These are bounded native object identities, not original pointers, source model IDs, collision shape numbers, visibility IDs or collection references.

The real constructor installs the reserved pair only at its original query-resource assignment point. Actual saved registered slot lanes are checked afterward. No public allocator, mutable getter, token receipt or arbitrary callback is introduced. The prior raw-borrow runtime entry is retained; fresh static-only ColdStage is untouched.

## Lifetime, failure, and cleanup

- Before any constructor mutation the existing concrete BankPin strongly owns the Operation; Operation strongly owns the same aggregate, actual resource roster/navigation lease and Application component owners. Runtime holds only its existing weak Bank pin. No strong cycle returns to Bank/pin.
- A private unbanked retirement obligation starts at successful binding and survives every partial constructor failure. Bank destruction checks it without reading the destroyed Bank.
- `Operation::close()` requires the actual still-live idle Bank and already-retired actual camera/grid/fresh memberships. It calls existing model `close()` in reverse source role order C, BL, BR, L, R (and reverse slot order); those methods perform their own real Bank visits. It retains incomplete cleanup for retry.
- Model objects, visuals, cables, adopted source resources and arena bindings stay retained through actual Bank destruction, including after successful model close, so saved slots cannot restore dangling host views. Only the original model owners' real close releases their original material/Entity effects. No caller drained flag or snapshot clearing is used.
- Runtime/aggregate close rejects the live Bank pin or outstanding navigation lease. After Bank destruction and release of the closed Operation handle, the existing aggregate teardown can proceed. Borrowed arena storage must remain alive through that checked teardown, including the existing pod-context unbind. Destroying a borrowed object first is not valid cleanup.
- The source-proven wreck retirement blocker remains unchanged. No preparation/cleanup here clears saved wreck world pointers or invents trigger history.

## Remaining integration boundary

This is a retained per-slot constructor extraction, not the complete caller. Actual Course/UI admission, cold arena registration/construction, selected all-AI roster creation and recovery capacity setup remain the real caller's work. The private stable Rig/model arrays deliberately have no mutable getter; a follow-on aggregate typed grid/camera handoff must consume them internally. Whole-domain population must follow all actual model material-reference completion; this operation does not claim that prepareEarly alone completed those later references or refresh a collector. Original neutral setup remains blocked by the separately documented reachable trigger/type2 heap-history dependency.

## Changed files / coverage

- New `tests/native/ra_cold_roster_resources50la.h`
- New `tests/native/ra_cold_roster_operation50la.h`
- `tests/native/ra_cold_aggregate50la.h`: concrete retained-roster reservation declaration/friend
- `tests/native/ra_source_camera_ownership50la.h`: private identity/reservation/pin/completion and retirement joins
- `tests/native/native_racer_collision50hw_fixture.h`: forward declaration and one concrete private friend only
- `tests/native/terrain_interactive_drive50du.cpp`: compile-only include; existing loop unchanged

No tests, builds, syntax checks, converters or project/game execution performed. Root must reconcile overlapping aggregate/bridge changes centrally and perform any future verification through its centralized Windows gate.
