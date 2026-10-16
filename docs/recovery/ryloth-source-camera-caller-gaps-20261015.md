# Smallest genuine RA caller path to the private camera

Source-only review of ed6e686 and the isolated camera wrapper. No code changes or execution checks.

## Immediate next implementation slice

Factor the numeric `native_racer50HN::groundGridPose` collision loop in `native_racer_grid50hn_fixture.h:15–43` over a stable authenticated collision-owner span. Preserve the public ColdScene overload as a delegating wrapper. Keep the new RA entry private to the actual runtime ownership bridge, consuming its already-retained 178 collision owners; no public arbitrary World/Storage pair or dummy ColdScene.

Then add a private typed aggregate grid/attachment operation. Bind actual arena bindings/parameters, rebase-model storage/count, pose history and ground scratch while their addresses are stable; these are real registered arena owners, not a caller pose or completion flag. The operation takes the same pinned Bank and actual slot, enters its own fixed `Bank::visit(slot)` body, and derives every position/cache from that slot's installed source state and the retained RA M spline. It should not accept an arbitrary callback, grounded matrix, zone or claimed Ready receipt.

This is a legitimate cold ownership step independent of trigger-history resolution. It must not run the neutral physics tick or publish a camera/whole-course readiness claim.

## Source-backed grid and attachment operations

- `source_grid50IG::prepare(map, rank, GridCenterResult50AC&)` already implements original2A2880 geometry over actual M gates. RA owns map115 nodes, predecessor103 and prepared gates/zones. Existing explicit native rank assignment `{4,0,1,2,3,5,6,7}` can remain a documented host roster policy; do not call it authored RA ranking.
- Ground height uses `beginResetProbe50AG`, source-order `queryResetCandidate50AM` on the actual category16-bearing collision owners, and `acceptResetProbe50AG`, exactly as the current groundGridPose loop. Selected lift is the actual PHY-derived `x.parameters.visual.verticalLimit7EC`. No route-elevation approximation.
- `native_racer50HN::placeColdGrid(state, bindings, grounded, rebaseModels, count, poseHistory)` checks actual Ready/frameFactor0, zero body velocities and unchanged basis; it updates real scene/body/model bounds/history. No direct phase/pose fabrication.
- Reuse `prepareQuerySphere48O` and the current `prepareQueryCache` sequence at terrain_interactive_drive50du.cpp:1578–1595 for both actual type2 resources and their current main190/physical19C matrices.
- Use actual `removeLiveZoneMembership50EN` before placement and `attachLiveZoneMembership50EN` afterward for main/secondary, with the Storage's real refs/backing and `ZoneMethod50CL::Second`, radius0. Terrain uses `prepareTerrainSinglePair48M` and `live_terrain_probe50fr::bounds` before its real attachment.
- Fresh initial collision reference1 is source-proven, not copied from another course: original1AE578–1AE590 loads first actual4C0/4C4 entries;216A2C/216A38 stores C30/C34. See ryloth-runtime-binding-20261014.md:274. Use reference1 only with this actual nonempty RA lifecycle; visibility uses its distinct same-RA light owner.

## Real eight-racer constructor sequence

Reuse the working original-AI route, not an alternate-course SessionAssets object. Existing source locations are terrain_interactive_drive50du.cpp:3739–3910 and native_racer_owners50hn.inc:

1. Own the genuine eight immutable `native_racer50HW::Rig` and `native_racer_collision50HW::Owner` objects, selected original PHY/PSG/MOT/MCF/AI resources and concrete model-owner arrays. Keep all backing stable through Bank and camera cleanup.
2. Construct the existing actual arena/query/filter/terrain owners. Reserve runtime capacity from those real collision/recovery/wreck owners before the exclusive RA world lease. Call the existing aggregate Storage and Follow binding stages. Recovery uses `undertrack_owners50ew_fixture::prepareStorage` with actual world membership capacity before source selected bindings.
3. Register the existing frame/handling/bindings/AI/lap/history/query/probe/recovery lanes using native_racer_owners50hn.inc semantics. Replace only its candidate loop and Storage registration with `aggregate.registerRuntimeQueryWorldOwners(actualBank,d)`; do not double-register them. Register any real additional membership backing and main/secondary identity scalars before seal. The current caller's `gcTriggerActions || isGaTrack || isMaSaTrack` gate does not cover RA and must not be used accidentally as RA identity banking.
4. `Bank::seal(8)` then each actual `visit(slot)` performs `native_racer_model50LA::Owner::prepareEarly` using the actual Application/domain/motion library/generation, sibling binds, and `Rig::install` into the real arena. Use `native_racer50HW::bindColdHandling`, selected source AI profile parsing, `applyAiCreateClass50IU`, `bindAiCreateIdentity50IU`, `initializeAiRuntime50IV` and `bindAiColdTracks50IU`. RA's retained `freshSelection` already comes from `bindFreshAiCreateSelection50IU("RA",2,...)`; do not borrow SA class/record selections or keep inherited POD01 state.
5. Preserve `placeColdHandlingRoots`, source model rebase, effect name/owner construction, per-slot `query.resource`/`secondaryQuery.resource` switching, actual `terrain_probe_authoring50fq_fixture::prepare` and `installProbeCold`, `bindSelectedCold` recovery, reciprocal identity exclusions, and actual terrain enable. The banked POD views refer to the real permanent resource owners; no pointer into a temporary ownProbe may survive installation.
6. Perform final source grid/real query attachment for every slot using the narrow operation above. Require actual AI class1 and initialized original AI for every slot, including followed slot0. No HOST driver branch or private RNG seed.
7. Use the private same-RA light owner `attachFreshVehicle(slot,state,bindings,parameters,d)` for true fresh7F0 visibility, and existing `undertrack_owners50ew_fixture::attachFreshQueries` plus actual query publication for694/7A4. The caller currently does this at terrain_interactive_drive50du.cpp:3996–4007. These need typed aggregate operations because current RA light/Storage owners are intentionally private.
8. Construct real inter-racer contact/dynamic owner bindings from the actual immutable collision resources and eight Bank slots, as the current caller does after grid placement. Keep shared reciprocal contact/list/global state outside the Bank; register only the actual per-racer mutable lanes.

Ready is the result of these existing constructors, not a flag the RA adapter sets. The camera's actual ReadHandles recheck all eight slots and their real AI/manager/pod identities.

## Neutral setup is real physics and is not yet a safe shortcut

The current `activateColdRace` block at terrain_interactive_drive50du.cpp:4059–4175 deliberately starts only after actual Course/UI ownership ends. It initializes the real original AI manager from actual MAP metadata/nodes with `initializeOriginalAiManager`, publishes its real flags into the slots, then calls the genuine `advanceFrame` for slots1..7 and0. This performs physics, graphics, query, trigger and audio/effect work. It is not an asset-only readiness step.

After those actual steps it reinitializes source lap timing as the existing cold policy specifies, ranks the real original AI fleet, calls `race_session50HM::initialize` with the same RA map/mapLap owner, seeds the explicit native grid order and samples/publishes actual observations. Only then does the actual host race satisfy the camera's Countdown/racerCount8/raceSeconds0/finishCount0 checks. Do not set Countdown or call a synthetic zero-work frame to satisfy them.

The original setup tick requires real material/contact/audio/recovery/trigger destinations and the same shared RNG/clock owners. RA's inactive audio source closure alone does not establish live D44/groups/voice ownership; muted output does not waive it. Actual runtime binding of these existing owners remains pending.

## Trigger and registry blockers remain precise

RA has two authored sphere instances (LostAI source placements29/104), not four particle colliders. Both action records and exact targets are retained by ra_trigger_authoring50LA; the actual AI track has113 nodes. A real adapter must preserve PLAYER exclusion from actual manager+34/main690 identities, C80 writes only for matching main690, Reset=false, genuine CC/D0 prior-contact bookkeeping and percent100 without RNG consumption. Existing `triggerLostAi50IN` and `TriggerEntryState50IN` provide action/lifecycle primitives.

The boolean source shape5/type2 branch still lacks a trustworthy input contract:1218AC calls108E90 directly with type2+4C/+50, whose endpoint allocation has32-byte pairs while the leaf reads20-byte normal/index records;106ED0/109F10 also consume unestablished external vertex descriptor3DFE60. No previous-pose argument exists on this branch. Do not borrow SA endpoint/sweep policy, invent triangle/vertex data, fabricate prior history or assert no grid overlap without proof.

Current RA runtime Storage reserves178 static owners plus real queries, not the complete additive trigger extent. Adding two live trigger owners requires a coherent **fresh** binding/capacity selection before Storage allocation/Bank registration. It cannot resize the already-surviving static/query route or claim capacity means registration. Cold grid attachment may advance independently; the full neutral tick/gameplay path remains blocked until the actual trigger semantics/admitted policy and live destinations are supplied.

## Lifetime / camera boundary

The bound Bank alone owns its private pin, retaining RA Storage/candidates/source through partial registration and every saved slot. External arena/query/recovery/rig/model objects must outlive the Bank and checked aggregate cleanup. The camera wrapper retains same light/particle owners and authored bytes, borrows the same Bank, and requires actual Session cleanup before Bank destruction, including failed pre-Follow construction.

Actual SourceCamera close/abandon must run while Bank, display/renderer and models are live. Then detach per-slot real memberships under their slots, destroy the Bank, and finally allow aggregate cleanup. Prepared wreck queries still retain world aliases after membership removal; no existing terminal alias retirement join is implemented. Destroying a borrowed wreck or clearing live pointers while snapshots survive is not a valid workaround.

The camera factory can be supplied after the above real caller path exists. It continues preserving RA raw cut count7 and the actual prelude decision; any unsupported positive effective cut rejects. No RA invocation/admission, new driver, camera pose or manager is proposed by this review.
