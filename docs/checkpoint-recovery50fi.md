# 50FI: bounded host checkpoint recovery

`tests/native/checkpoint_recovery50fi_fixture.h` implements an explicit **host
policy** for the interactive development client. At the retained native
`ExternalCut / 0x228BBC`, it can prepare a source-selected checkpoint and create
a fresh native session there. It is **not** the original complete `2255B0`
reset, and it does not remove that guard, change a native frame phase, run a
partial frame again, or claim the remaining reset/camera callbacks are joined.
The separate damage-failure policy below creates a fresh zero-damage vehicle
after an authentic selected `50C` destructive transition, without executing or
claiming the original wreck/detachment handlers.
The separate portal-failure policy below creates a fresh session after the
actual `50EN::UnsupportedLinkRemoval` boundary, preserving current navigation
and damage without completing or classifying the failed query.
The separate manual-checkpoint policy accepts an explicit development-host
request only at a normal `Ready` boundary, preserving the same navigation and
damage without inventing a native failure.

The adapter does not modify `50EN`, `49W`, `50ET`, `50EU`, or `50CP`. The `50DU`
host owns session replacement and pause/restart behavior. Normal
authored-grid startup and F5 whole-session restart remain the caller's existing
path. Frozen binaries and delivered ZIPs are outside this change.

## Source inputs and bounded work

- The live `50EU` map's actual `C8C` anchor and `C88` next node feed the existing
  `selectUndertrackReposition50ET`. The selector retains the original ordered
  branch choice and computes anchor plus authored lift and heading. No nearest
  point, inferred centerline, or fallback grid position is introduced.
- The actual main `190` graph is copied into 53 stack-owned nodes. Its copied
  world cache is refreshed with `refreshSceneWorld49P` before supplying world
  translation to the selector. Local translation comes from the canonical
  source node. The paused live graph is never mutated.
- `prepareUndertrackResetPose50ET` uses the actual current lifecycle shared
  `up434650`; there is no substitute up vector or host trigonometric heading.
- The anchor's already prepared `map.nodeIndex78` supplies the initial collision
  zone. It must be less than the actual world's zone count. `primaryZone` is
  that byte plus one, the established `50EN` native ordinal-to-reference
  translation. There is no position-based zone inference. World count is
  bounded to 1..256 and a real portal-head owner is required.
- Ground resolution uses the existing cold scene's actual COL owners, authored
  masks, prepared world matrices, spheres, and original order. It repeats the
  `50CP` `50AG/50AM` probe algorithm with the checkpoint pose substituted before
  the probe begins. Every successful candidate shortens the endpoint just as in
  `50CP`; no hit height is fabricated. At most 20 probe requests and 64 actual
  owners are accepted. Existing caller-owned stack scratch is reused.
- A missing hit, unsupported query, invalid pose, insufficient storage, missing
  index, or source-selector rejection returns `false` with an explicit
  diagnostic. `50AG::Exhausted` is never accepted as ground. The output plan or
  pose remains unchanged on failure. No recovery helper calls `exit`, allocates
  memory, loads a file, or calls a fixture/oracle callback.

All helper inputs, outputs and scratch spans must have distinct backed storage
and serial ownership. Strict binary32 compilation, `FE_TONEAREST`, no fast math
and no fused contraction remain required. The plan owns values only; it keeps no
pointers into the old session.

## Caller API

Namespace: `checkpoint_recovery50fi_fixture`.

```cpp
Plan next;
Diagnostic diagnostic;
prepare(*navigationMap, navigationProgress, s, x.bindings, x.diagnostic,
        ground.lift, liveWorld, next, diagnostic);

BodyMatrix48G grounded;
resolveGround(ground, next, 16, 0,
              groundStack.data(), groundStack.size(), grounded, diagnostic);

restoreContinuity(next, s.fields, navigationProgress, navigationCA0,
                  grounded, diagnostic);
```

`prepare` requires the genuine `ExternalCut / 0x228BBC`, phase
`PostPackedBytes50S`, player `vehicle58 == vehicle3C == 0`, attached parentless
main scene, and a negative actual main local Y. It rejects use as a generic
teleport API. `resolveGround` also verifies the plan lift is bit-identical to
the newly loaded authored lift and that the scene's actual zone count agrees.

The `Plan` fields needed by the host are:

- `sourcePose`: the selected source basis and pre-probe translation
- `selection.anchorNode` and `selection.selectedNextNode`: source receipt
- `nodeIndex78`, `primaryZone`, `zoneCount`: source-derived initial zone binding
- `progress`: exact saved `C80/C84/C88/C8C`
- `damage`: exact named `7D0/7D4/7D8/7DC` lanes
- `lift` and `prepared`: admission metadata
- `policy`: `Undertrack`, `DamageFreshVehicle`, `PortalLinkRemoval`, or
  `ManualCheckpoint`
- `damageDecision`: the actual unsupported `50C` receipt for damage recovery
- `portalDecision`: the actual unsupported `50EN` receipt for portal recovery

`Diagnostic` identifies the host error, the original `50ET` error/source PC,
ground query status, ground phase, attempt count, candidate count and last owner.
`errorText` provides a short console description. Distinguish a prepared plan
from a successful terrain hit and from a successfully started new session.

## Exact 50DU join points

1. Add an optional plan to the host session boundary, owned outside
   `runSession50DU`. Keep a distinct host checkpoint-recovery exit reason rather
   than overloading F5's normal grid restart.
2. In `advanceFrame`, only within the existing
   `!baseAdvanced && ExternalCut && sourceStage == 0x228BBC` branch, prepare a
   **local** plan and preflight `resolveGround` against the still-owned ground
   scene. Only publish that plan and leave the old session after both succeed.
   On rejection, retain the paused last committed image and keep F5/restart and
   quit available. Do not continue the failed native frame or call it complete.
3. In the next cold session, the insertion is immediately after
   `ground = authored_runtime_ground50cp_fixture::load(...)`, lift verification,
   and ground-stack allocation, and before the existing `resolve(...)` and
   first `setLocalMatrix49P(...main190, pose)` call. For a supplied recovery plan,
   use `resolveGround(..., pose, diagnostic)` there. With no plan, preserve the
   original grid `resolve`, `Hit` check and pose construction unchanged.
4. Use that accepted `pose` throughout the existing initial main/secondary,
   displayed model, quaternion/body, geometry, query-cache, and history setup.
   Do not repair live ready flags or insert an unrelated pose later in setup.
5. After fresh `50EU` map and AI decoding and zone binding, create
   `navigationProgress` and `navigationCA0` as before. With a recovery plan,
   verify its saved `C80/C84` fit the new AI track and `C88/C8C` fit the new map,
   that `selection.anchorNode` still maps to `nodeIndex78`, and that the fresh
   world count matches `zoneCount`. Then call `restoreContinuity`. The root may
   use its normal explicit setup-error path for a changed source asset set;
   there must be no silent grid fallback presented as a recovery success.
6. At both initial `attachLiveZoneMembership50EN` calls, pass
   `plan.primaryZone` for recovery, otherwise the existing grid value `1`.
   Passing `1` unconditionally would register later checkpoints in the wrong
   source zone. Preserve normal overlap-radius and membership ordering.
7. F5/restart clears the recovery request and uses a clean grid session. A
   recovery preparation/ground failure remains a visible paused state with that
   manual escape available. Count/report a host recovery only after the fresh
   checkpoint session is actually ready to present and advance.

## Continuity and deliberate limits

The four navigation words survive exactly. Undertrack, portal, and manual
checkpoint recovery preserve all four canonical damage lanes exactly, without
clamping or healing. The
separate damage-failure policy deliberately sets those four lanes to zero in
the fresh vehicle. Actual source damage outside the existing damage-consumer
finite/non-subnormal domain rejects for every policy. `restoreContinuity` only touches those
fields and `CA0`; it preserves other freshly assembled fields. The caller must
invoke it only after a successful ground resolution and before the first native
frame. Its pose validation checks that grounding changed only translation Y.

`CA0` is deliberately rebased to the new main local checkpoint pose so the host
teleport is not misreported as driving across navigation gates. This is a host
restart discontinuity, not a claim of original reset semantics. The fresh
session reconstructs physics, controls, clocks, noise, effects, visual owners,
query storage and membership; effect/timer/environmental-damage bookkeeping and
other original reset callbacks are not preserved by this policy. Complete
original `2255B0` behavior remains open.

## Separate manual-checkpoint host policy

Backspace requests a development-host checkpoint reset so a pod wedged against
terrain can return to the genuine current checkpoint instead of F5's authored
grid start. Headless-live input accepts the case-insensitive, newline-delimited
command `checkpoint`. This is an explicit host convenience, not an implementation
of an original gameplay action, the remaining `2255B0` reset, or a new native
button mapping. All existing full-reset and native failure guards remain intact.

```cpp
prepareManualCheckpoint(*navigationMap, navigationProgress, s, x.bindings,
                        ground.lift, liveWorld, next, diagnostic);
```

This entry point requires `s.phase == VehicleFramePhase49W::Ready`; any partial
frame, including a paused native failure, rejects with `Error::ManualBoundary`.
It takes no cut or failure receipt, has no negative-Y condition, and does not
rewrite the native phase to obtain admission. The normal finite positive lift,
rounding mode, player ownership, attached parentless scene, map/zone, copied
scene refresh, source selector, basis and damage-domain checks still apply.
Failure preserves the destination plan and leaves the original state unchanged.

A prepared plan has `policy == ManualCheckpoint`. It selects the actual current
`C8C` anchor and `C88` next node, copies all four navigation words and damage
lanes exactly, and derives the initial zone from the anchor's real `nodeIndex78`.
It neither advances a gate nor chooses another anchor to escape a bad result.
The caller must preflight `resolveGround` against the still-owned scene before
publishing the reset request or discarding the old session. A rejected selection
or missing real ground is a reported failure, never permission to fabricate a
pose or silently fall back to the grid.

`GameHostInput50ER::checkpointRequested` is a one-poll request. In window mode it
comes from the existing `WindowHost50DQ` non-repeated Backspace press edge (SDL
scancode 42), never from held-key state. In headless mode each complete
`checkpoint` command releases held raw keys and sets the flag; multiple commands
within one poll coalesce. Poll initialization clears the flag next time. Quit or
Escape suppresses a simultaneous checkpoint request, and the client retains quit
priority before considering any reset. F5/`restart` keeps the separate full-grid
restart path. The host facade reports input only; it never teleports a body or
changes navigation itself.

The client should handle this request before beginning a new native frame,
prepare a local plan, and complete the same actual-ground preflight used by the
failure policies. Only then may its existing release-all/teardown path replace
the session. Fresh setup uses the same grounded pose and primary-zone binding
described above; `restoreContinuity` rebases `CA0` so the teleport cannot appear
as a gate-crossing segment. The old session's pending native events, query
pointers, physics owners and timers are not copied. Duplicate-request handling
belongs to the caller, which can distinguish an explicit new manual press from
automatic recovery at the same anchor. Report a successful manual reset only
once the fresh checkpoint session is ready, identifying retained damage rather
than claiming a native action or whole-grid restart.

## Separate damage-failure host policy

Live overheating can authentically select `225D9C -> 226750`. The existing
native frame must still reject because the destructive handler is not joined.
`prepareDamageRecovery` accepts that reached failure as a separate host restart
reason. It does not substitute an undertrack cut, require a negative Y, mutate
the damaged native frame, clear its damage gate, advance its phase, or interpret
the unsupported transition receipt as an executable native event.

```cpp
prepareDamageRecovery(*navigationMap, navigationProgress, s, x.bindings,
                      decision, x.diagnostic, thresholdBits,
                      ground.lift, liveWorld, next, diagnostic);
```

`thresholdBits` must be exactly the argument supplied to the failed
`decideVehicleImpacts50P` call; the current client uses `0x3f800000u`. Both
operations read B9D bit 0 from the same unchanged `state.fields.flagB9D0` owner.
The integer passed to the decision leaf is only a bit-0 representative, not a
recovered raw B9D byte. The adapter requires all of:

- Frame phase `PostOptionalProbeEffects50P`, the actual phase preserved by that
  failed native call
- Frame error `ExternalCut`, with its `sourceStage` matching the actual decision
- Decision status `unsupportedTransition`
- One of the source-proven `(sourcePC, vtableSlot, originalTarget)` tuples:
  `(225D10, 88, 2261B0)`, `(225D9C, 8C, 226750)`,
  `(225E28, 8C, 226750)`, `(225E7C, 90, 227370)`, or
  `(225EC0, 94, 227A40)`; all numbers here are hexadecimal
- Reinspection by the unchanged pure `inspectImpactTransition50C`, using the
  canonical four damage lanes and `BA0.Z` with those exact threshold/flags,
  reproduces all four fields of the receipt
- Existing player, scene, source map/zone, source selector and pose admission

Arithmetic failures, unrelated cuts, mismatched/stale decisions, other phases,
and no-transition results reject. Failure leaves the destination plan untouched.
The common source geometry routine is used directly after this separate
admission; it never calls the undertrack entry with fabricated inputs.

A successful plan has `policy == DamageFreshVehicle`, keeps the exact actual
`damageDecision` for reporting, retains navigation and the genuine anchor zone,
and sets its four canonical damage lanes to positive zero. The unchanged actual
ground query still must succeed before discarding the old session. The fresh
vehicle uses the same ground/continuity and initial-zone APIs as undertrack
recovery. Plan validation rejects a damage-recovery plan with nonzero damage.

The root hook belongs immediately around `decideVehicleImpacts50P`, before
plain-effects/packet/base advancement. On an admitted real damage cut, preflight
the plan and ground, request a new host session and return from `advanceFrame`.
If either fails, keep the paused last committed image and F5/restart escape.
Other native failures must retain their ordinary diagnostics. Report that this
policy replaces the failed vehicle with a fresh one at the checkpoint; do not
reuse undertrack's message that damage was retained.

Original `2261B0/226750/227370/227A40` wreck/detachment gameplay remains unjoined.
No `49W`, `50C` or undertrack guard has been changed by this adapter extension.

## Separate portal-link failure host policy

`50EN` explicitly returns `UnsupportedLinkRemoval` at source `281320` when a
searched secondary membership is not the list head. Its original head update
would orphan other live registrations; the native adapter therefore retains
this unsupported boundary and rolls back the failed membership transaction.
`preparePortalRecovery` is a development-host restart policy for that exact
returned boundary. It neither implements a generic list unlink nor modifies
the native membership algorithm, its list ordering, or its guard.

```cpp
preparePortalRecovery(*navigationMap, navigationProgress, s, x.bindings,
                      actualPrefixResult, ground.lift, liveWorld,
                      next, diagnostic);
```

The caller passes the actual returned `LiveZoneResult50EN` immediately after
the failed query prefix, not a status invented from another error. The helper
requires:

- Current phase `PostWorld`, shared by the client's main/secondary `50EQ` queries
  and `50FR` terrain-probe query
- Receipt status exactly `LiveZoneStatus50EN::UnsupportedLinkRemoval`
- Receipt bounds, geometry, and registry substatuses all `Complete`, as at the
  real source guard; every other status combination rejects
- The same player, scene, navigation, anchor-zone, source-selector, basis and
  damage-domain checks used by the existing checkpoint policy

There is no negative-Y condition or fabricated `ExternalCut / 228BBC` receipt.
The plan has `policy == PortalLinkRemoval` and keeps every field of the actual
receipt in `portalDecision`, including `crossedPortals`. That count describes
the abandoned transaction; it must not be added to committed crossing totals.
The helper cannot prove a caller-created value's provenance, so forwarding the
actual native result is part of the caller contract. It does not rerun the
failed membership operation. Invalid phase returns `PortalCut`; invalid receipt
returns `PortalDecision`. All failed preparations preserve the output plan.

The geometry path directly reuses the existing source checkpoint selection:
current `C8C`, current `C88`, actual refreshed scene, authored lift and up vector,
and the anchor's genuine zone byte. It never advances or shifts the checkpoint
to avoid the boundary. All four navigation words and current damage lanes
survive exactly. `resolveGround` must then find real ground with the same
bounded owners, masks, candidate order and probe algorithm before any restart
request is published. A missing hit or failed selection is a visible paused
state with F5 available, never an invented support surface or process exit.

The host hook must be outside the fatal assertion path, directly around the
actual main, secondary or terrain query result. Admit only the matching
`PrefixRejected` result carrying `UnsupportedLinkRemoval`; preserve ordinary
diagnostics for other failures. Return from `advanceFrame` immediately after
handling this boundary. Do not copy stale query outputs, commit a contact,
record a hit/miss, apply force/support, mark the failed frame complete, or retry
its remaining stages. Previously completed source stages are not retroactively
called transactional.

The next session uses the existing recovery ground, continuity and initial-zone
setup. Count/report a successful portal checkpoint recovery only after that
fresh session is ready. Its host message must identify the portal-link boundary
and retained damage, rather than claiming an original undertrack reset. Any
repeat-recovery guard belongs to the host session owner; if it suppresses a
repeat, pause and offer F5 rather than selecting another anchor or changing
native results. F5 clears the request and returns through the ordinary grid
restart path.

## Validation recorded here

Before the portal-policy extension, the standalone header passed C++20
`-Wall -Wextra -Wpedantic -Werror
-fsyntax-only` with strict floating-point flags, including the separate damage
entry point. That prior result does not validate the new portal entry point.
No tests or builds were run by the portal-policy or manual-checkpoint worker;
focused validation is owned by the root integrator. The manual-checkpoint change
received source review only; the earlier syntax result does not validate its
entry point or input plumbing. Its preimages are under
`solo-manual-reset50ga/preimage/`. Integration, actual-ground
smokes, failure-path checks, rendered gameplay and session restart checks belong
to the root integrator. No broad regression, guest execution, PC/device access,
excluded `49C/48U`, or unapproved `ee.bin` access was performed for this change.
