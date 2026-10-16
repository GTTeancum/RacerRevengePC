# Native opponent-preparation copy reduction: source-only review

## Recommendation

Use the existing `native_racer50HN::Bank::visitMany` for the all-active opponent preparation sweep only. Keep the existing single-slot path as the fallback when any opponent has stopped. This is a bounded host ownership/copy optimization, not an AI, handling, physics, projection, or draw-bind change. Following the parent’s explicit follow-up authorization, the minimal substitution was implemented only in `tests/native/terrain_interactive_drive50du.cpp`, uncommitted, for independent review. No project code, tests, builds, syntax checks, benchmarks, emulation, or remote PC work was executed.

## Evidence and limits

Reviewed initially clean source at `a67bcaf` in `/workspace/shared/rr-recovery-git` and retained Windows evidence at `/workspace/scratch/74cd1d3d7a45/windows-a67bcaf-pass/packet/windows-capture-a67bcaf-attempt1/stderr.log`. Source line references below refer to that pre-change revision.

The verified baseline is 35.72401502005235 FPS (179 completed swaps / 5.010635 seconds), MA, eight original-AI racers, zero host driver calls, zero controller samples, zero keyboard packets. The log reports 183,753 bytes of native state per racer. The supplied full-log aggregate estimates are physics 6.8801 ms/call, render_prepare_submit 10.387 ms/call, opponent_preparation 4.9494 ms/call, driver_submit 4.7674 ms/call. Those timing aggregates include countdown and are not the controlled gameplay-only FPS interval; do not add nested subphases to their parent or infer speedup from their averages. The latest 180-call window itself reports opponent_preparation 906,123,400 ns and driver_submit 862,726,100 ns; this is further evidence of where to inspect, not a before/after result.

## Exact callsite and ownership invariant

- `tests/native/terrain_interactive_drive50du.cpp:6894–6987`: opponent render preparation scope. The ascending loop at 6902 visits every active nonzero racer independently at 6907, completes draw/light/cable/Binder/flame preparation, exits the bank visit at 6953, and publishes per-racer descriptors through 6980.
- `tests/native/native_racer_bank50hn_fixture.h:106–124`: each nonzero `visit` saves player0, restores the opponent, runs its callback, saves that opponent, then restores player0. Each save/restore walks every registered span using memcpy (lines 201–206).
- Same file, lines 134–157: existing `visitMany` holds one serial bank lease, saves the previous current slot and restores the next, saves the final current slot and restores player0 on normal return, callback false, or exception. It does not increment completedSteps or alter active flags.
- `tests/native/native_racer_owners50hn.inc`: registered span ownership includes the actual frame/graph, x.bindings, x.parameters, baseTail and coldFlame. Stable execution-arena addresses and permanent immutable racer asset owners are preserved. The original AI shared-noise owner is intentionally not copied; do not change registration.

Between opponent callbacks, no operation in this loop needs the player0 arena. Post-preparation statements use race.racers, opponent-owned batch/cable/UV buffers, per-racer model owners and gcDrawLights. They neither reference `s`/`x` nor call Bank scalar reads, visits, or steps. `DrawOwner` retains its own vertex and normal buffers, while meshes/indices/spans point to permanent racer assets (`native_racer_draw50hn_fixture.h`). Binder stage output is copied into `meshes_[slot]`, not a pointer to arena state (`gameplay_binder_window_candidate_fixture.h:55–71`).

The inner light lease must keep its existing lexical extent. `beginNativeLightDraw` binds the actual current `s` and refreshes its roots; `Owner::beginDraw` retains `&s` only in that lease. `DrawLease::~DrawLease` clears it using `endDraw` before the next bank switch (`original_gc_gameplay_lighting_candidate.cpp:684,1099–1120,1206–1212`). `publishNativeDraw` copies matrices and identity/publication scalars into the per-racer output. Do not lift a light lease or sourceColors reference outside the existing inner scope.

## Bounded implementation (applied after follow-up authorization)

1. Extract the existing active-racer preparation block (6908–6952) without changing its statements, arithmetic, diagnostics, or lexical lifetime of local light/color owners.
2. Factor the existing whole loop iteration into a local helper, preserving the exact order: clear flame-prepared flag; count finished opponent; prepare active racer; count stopped retention; validate batch; publish geometry/cables/UV/model/light descriptor.
3. Before the sweep, inspect only Bank::active for slots 1 through count-1. If every opponent is active, construct a bounded stack order array containing exactly those slots in ascending order and call `visitMany(order.data(), count-1, callback)` once. The callback runs that same whole iteration and invokes the preparation block directly in the already-loaded arena; return true after completing the iteration.
4. If any opponent is stopped, run the original ascending loop and original individual `visit` calls, using the shared helpers. This keeps stopped/partial-state retention behavior and its performance unchanged and avoids loading stopped snapshots just to batch the loop.
5. Keep `OpponentPreparation` timing around the same work, and keep the final submittedOpponents assertion / renderFrame publication after the batch has restored player0. No heap allocation, new cache, Bank implementation change, arithmetic rewrite, altered shader/bind work, eligibility change, or batching across physics cuts.

For seven active opponents, the current path performs 28 full owner-span sweeps. One batch performs 16: initial player save and opponent restore, six peer transitions (12 sweeps), final opponent save and player restore. It removes 12 sweeps, or 2,205,036 logical memcpy bytes per rendered frame for this capture's owner size (43% of this loop's owner-copy volume). This is not a prediction of a 43% phase or FPS gain: vertex/normal/light/material processing remains unchanged, and actual copy time is not separately instrumented. The candidate is safe and concrete but may produce a modest gain.

## Preservation / acceptance criteria for a later authorized implementation

Source review should ensure identical ascending racer callback and publication order, unchanged all-eight original AI execution and two-phase scheduler, no RNG/noise changes, identical current graph and authored handling inputs, exact arithmetic and existing strict floating-point settings, unchanged failure propagation, and player0 restored on every exit. A failure within preparation must retain that slot's same partial bytes; a failure in descriptor publication must restore player0 and propagate as before. Successful output bytes and per-racer light/model/flame publications should match the old path. Inactive-opponent cases must take the unchanged legacy path.

The applied diff uses `prepareOpponent(racer, arenaLoaded)` for the whole iteration and `prepareActiveOpponent` for the former visit callback. The latter remains nested so light destruction precedes any bank transition. A fixed-capacity stack order array holds slots 1 through count-1. All existing checks/messages remain; a defensive batch-return assertion was added. Diff inspection found no arithmetic or underlying preparation statement changes.

Any eventual build/test/runtime verification requires separate authorization consistent with the user's current restrictions. Nothing here establishes a measured improvement or full equivalence result; this is a source-grounded implementation awaiting independent review and authorized verification.
