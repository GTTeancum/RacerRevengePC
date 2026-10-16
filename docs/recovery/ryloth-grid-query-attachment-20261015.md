# RA same-source cold grid and query attachment candidate

Base: c468b6dea54fe7058b9ccdf368afd3db73f3d66c. Source-authored only; no build, syntax check, test, converter or gameplay invocation. Central checkout unchanged.

## Callable boundary

`Owner::placeRuntimeGridSlot(actualBank, slot, GridArenaOwners, diagnostic)`

`GridArenaOwners` borrows the actual registered bindings, parameters, original AI runtime and pose-history arena plus the stable eight-Rig owner array. Frame, main/secondary queries and filters, terrain authoring/live owner, Storage, immutable capacity, collision world, M spline and collision meshes come from the already retained RA runtime bridge. No caller pose, rank, world, membership, scratch, completion boolean or callback is accepted.

The Bank must be the exact sealed, idle eight-slot Bank carrying this runtime's private destruction-bound pin. Registered extents must cover the actual frame (excluding its genuinely shared noise lane), parameters/bindings/AI/history, body queries, both per-slot filter identity lanes, terrain source fields and live terrain owner. Existing current-course registration is unchanged. A future RA caller must register the two actual main/secondary identity scalars before sealing, alongside its genuine arena registration; no registration is performed after seal.

The supplied Rig array and arena objects are stable typed borrows, not copied mutable owners. The bridge records their addresses and rejects replacement. They must remain alive through all slot operations and cleanup; the already pinned bridge retains the actual RA source, candidates, exclusive world lease, Storage and query backing. The caller must not destroy the unique Rig owners early. Camera construction, if it follows grid placement, checks that it receives the same bindings/AI/history/Rig array.

## Order and effects

1. Validate the pin, Bank identity and registered actual fields. Preflight all eight nonzero, pairwise-distinct body identities against static, camera, terrain, recovery and reserved wreck identities. Allocate private reset scratch sized from the actual 178 collision meshes before the first arena mutation.
2. Enter exactly one real `Bank::visit(slot)`. Require actual Ready, frameFactor zero, class58=1, initialized original AI and matching manager index. Match selected immutable main/secondary resources to the retained collision roster and selected Rig's original archive path. Match actual Rig scene roots, terrain authoring receipt/lift and original normal modes 0/0/1.
3. Derive position from the actual RA M spline using existing `source_grid50IG::prepare` (2A2880). Preserve the existing explicit native roster rank policy `{4,0,1,2,3,5,6,7}`. This is a host rank assignment, not a recovered retail roster order. Preserve the installed source basis and original PHY lift.
4. Run the unchanged reset-probe loop over the same source-order RA collision owners, category16, with the existing query/accept helpers. No interpolated route elevation substitutes for a real hit.
5. Remove main, secondary and terrain memberships in existing order. Invoke existing `placeColdGrid`, using the actual Rig's own source hierarchy/root/rebase views and actual pose history.
6. Rebuild both real query sphere/world caches from their actual type2 resource root and corresponding main190/physical19C matrices. Attach main then secondary through actual Storage lifecycle/world, method2, radius0.
7. Run actual `prepareTerrainSinglePair48M`, derive its live bounds and attach terrain, again method2/radius0.

Initial collision reference1 is source-backed by 1AE578/1AE57C loading the two namespace arrays, 1AE588/1AE58C choosing their first entries, 1AE590 calling2169C0, and216A2C/216A38 storing C30/C34. Collision and visibility namespaces remain distinct. This patch attaches collision queries only; it does not synthesize a visibility owner.

The original public `groundGridPose(ColdScene, ...)` overloads remain unchanged at call sites. Their exact loop is factored into a private helper accessible only to that overload and the concrete RA bridge. There is no public arbitrary source-span injection.

## Failure and retirement

Each slot is attempted once. A rejection is terminal for further placement and preserves real partial effects; the Bank's existing visit scope saves an opponent's partial state and restores slot0. No rollback/retry of construction is invented.

`Owner::retireRuntimeGridSlot(actualBank, slot, diagnostic)` is the narrow cleanup operation. It remains callable after a terminal placement failure. Under the same real Bank visit it removes all three actual memberships and clears an unbanked retirement obligation only after all removals succeed and actual attached fields are false. It does not clear world pointers, reset source state, restore a pose, or publish gameplay readiness. Failed cleanup retains its obligation and backing for retry.

An obligation is established immediately before the first membership mutation. Bank destruction checks these unbanked obligations after its snapshot/span storage has died, without dereferencing the destroyed Bank. Thus deleting snapshots cannot hide successful or partially constructed attachments. Before Bank destruction:

- Successfully close/abandon the actual SourceCamera and Follow through their existing bank0 authority.
- Retire every reached grid slot through the fixed cleanup operation, including any failed slot.
- Retire other actual query/visibility borrowers by their own genuine owners.
- Destroy Bank, then close the aggregate.

Prepared wreck world bindings remain a separate existing close blocker. This patch does not clear them or bypass saved Bank snapshots. Aggregate close continues to reject a live Bank pin. No new public retirement token, drained flag or mutable query getter exists.

## Remaining caller and admission boundaries

The actual RA eight-racer resource/constructor caller is still missing. It must construct the real rigs/models/AI/terrain/recovery state in existing per-slot order, supply immutable capacities before Storage mutation, register actual arena lanes before seal, and call this operation only after the original cold constructors. No neutral setup tick is run here. Countdown, final race initialization, inter-racer contacts, additive LostAI registry/history and live audio/effect destinations remain independent requirements. The current static-plus-reserved-query registry is not the complete RA gameplay extent and cannot be widened in place later.

Successful geometry attachment does not certify camera preparation, a completed neutral frame, rendering, or Course/SessionAssets admission. No default course path or renderer activation changes are included.

## Files / compile visibility

- `native_racer_grid50hn_fixture.h`: private loop factoring; existing overloads delegate.
- `ra_source_camera_ownership50la.h`: actual grid operation, private scratch/borrow identities, per-slot cleanup obligations and fixed removal operation.
- `ra_cold_aggregate50la.h`: bounded synchronous entry/cleanup forwarding.
- `ra_source_camera_factory50la.h`: same-grid-arena identity check if a grid stage already exists.

All headers are already included by the main native translation unit. No out-of-line object or new build-list entry is required. A centralized build remains necessary; this deliverable makes no execution-based correctness claim.
