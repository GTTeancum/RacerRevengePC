# Ryloth's two original triggers: bounded native collision contract

This is a source-only implementation of the user's 2026-10-16 approval to replace the undefined original sphere/type2 entry input with deterministic native collision using original geometry. It is a deliberate compatibility difference, not recovered original108E90 parity. No build, test, syntax check, decoder, emulator, guest instruction, sanitizer or gameplay ran in this implementation worktree.

## Exact scope and geometry

Only the two authenticated RA zone4 spheres are constructed: rightSkull targets AI node29; leftskull targets node104. Their centers/radii come unchanged from `ra_trigger_authoring50la.h`, which authenticates the retained original TOB. Reset remains false, PLAYER remains excluded by actual source roster prefix, start-enabled/repeating/100-percent flags remain intact. The actual all-eight original-AI route has human prefix0, so no racer is suppressed as an invented player.

`Type2StaticCandidate49X::raOriginalTriggerSegments` is set only by the RA candidate adapter. It is false by default and invalid on other shape types, Gate owners, missing groups, or indices outside these two originals. The indexed and live entry guards reject such misuse. All existing GC/GA/MA/SA behavior is unchanged.

For an admitted type2 query, take each original COL endpoint pair's XYZ coordinates in resource order. Transform both by the current actual prepared query world matrix, with the existing separately-rounded binary32 multiply/add sequence. W words are deliberately not read. Treat each pair as a closed zero-thickness segment, not a capsule with an invented radius. In binary64, project the authored sphere center onto that segment, clamp parameter to [0,1], and compare squared distance <= authored radius squared. Degenerate segments reduce to their actual endpoint. There is no tolerance, padding, synthetic geometry, prior matrix, swept surface, triangle reinterpretation, or retained external vertex descriptor. Every endpoint must be finite; invalid inputs fail rather than producing a collision verdict. The existing native sphere kernel is reused with a null previous matrix.

The ordinary current query-sphere broadphase, original candidate enumeration, type/category/identity filters and source PLAYER/contact exclusions precede this narrowphase in their existing order. RA does not use the old swept-policy exception to broadphase rejection. Existing candidate effects and synchronous E8/action order remain unchanged. This does not generalize to unrelated original-physics branches, and does not initialize or reinterpret original opaque W input.

## Lifecycle and destinations

`ra_trigger_actions50LA::Owner` reuses the established SA/MA `NativeTriggerGroup50GA`, `TriggerEntryState50IN` and `triggerLostAi50IN` machinery. Exactly two triggers and sixteen genuine main/secondary collider identities are retained. Duplicate entry suppression, CC/D0 holes/high-water, action-before-reciprocal-add ordering, reciprocal exit removal, and re-entry behavior remain the established source lifecycle. Chance100 consumes no RNG word.

Only an entering actual main690 writes its own live C80, to29 or104. Secondary contact owns lifecycle membership but does not write another racer's C80. No C84/C88, speed, steering, pose, progress correction, safety reset or route driver is supplied. The original122BA0 root overlap remains the exit test, with actual last observed prepared root geometry; it is not used as entry geometry.

The same aggregate owns authenticated authoring, candidates and actions. `prepareNativeTriggers` appends both candidates after the178 static owners before any registry borrow/capacity reservation. The retained roster bridge resolves the actual sixteen body identities, prepares actions, seals complete180-owner storage, and cold-attaches both original sphere memberships using the same source-world lease before Bank registration/sealing. Static registry backing and candidate mutable lanes follow the existing Bank registration. Trigger action/contact history and fixed trigger memberships are shared track state, deliberately not per-racer snapshots. Bank lifetime pins retain their source/candidate backing.

## Actual runtime integration contract

1. After source authoring and static candidates, call `prepareNativeTriggers(d)` before `prepareRuntimeRosterQueryStorage`.
2. Existing actual main/secondary queries need the explicitly native trigger admission (`nativeGatePolicy=true`, callback enabled, original skipMask8). This flag does not admit a previous-pose RA sweep.
3. Inside each actual Bank racer visit, bracket each main/secondary query with `beginRuntimeTriggerQuery(bank,slot,d)` and `endRuntimeTriggerQuery(bank,d)`. The begin method derives C80 directly from the retained original roster operation's registered navigation arena; callers cannot substitute a pointer. End must run even after query failure to discharge the synchronous borrow.
4. After actual prepared query poses commit, call `observeRuntimeTriggerPoses(bank,slot,d)`. Cold grid placement already records the initial actual geometry. Do not feed invented poses.
5. At the existing source world trigger-service cadence, call `serviceRuntimeTriggers(bank,d)` once while the same Bank is idle. The integration owns cadence; this helper does not synthesize or advance a frame.

The original-AI physical frame loop and source world cadence must perform these live calls; this packet alone does not claim they ran. Root owns integration and all validation. Any unjoined live frame/render/other query route remains an independent dependency, not silently certified by this trigger policy. Partial callback/membership effects are retained on failure rather than rolled back or retried as fabricated success.
