# Bounded controller action dispatch50EC

Source-only connection of persistent50CX packet sampling to the real48Z event
queue/manager and49A player-event handler. Only the new50EC header, implementation
and this note are owned by this change. No builds, tests, runtime probes, PC
access, original executable inspection, installs or shared build-file edits were
performed. This adapter is not yet compiled or runtime-validated.

## What this adds

`ControllerActions50EC` retains a one-poll lifecycle around an existing stable
`ControllerIngress50CX` and caller-owned `InputEventQueue48Z`. It does not own an
event array, copy a player/frame, run a new action implementation, choose button
labels or call a host input API. A successful poll cannot be replaced through
the adapter until its queue has been dispatched and explicitly acknowledged.

The route is:

1. Existing50CX/48Y sample produces the complete ordered numeric transition batch.
2. Existing `appendDeviceTransitions48Z` appends all transitions atomically.
3. Existing `dispatchInputEvents48Z` dispatches the whole caller queue once.
4. The optional `bindControllerPlayerActions50EC` binding uses the actual
   joystick application bridge, `playerEventChild49A`, and the caller's live
   `PlayerEventState49A`, environment, topology and synchronous effect callbacks.

It adds no deferred-effect substitute. An unavailable reached handler stops with
the original native partial-progress result; the adapter latches that failure
and refuses automatic replay or replacement sampling.

## Required stable owners and setup

Add `src/game/controller_actions50ec.cpp` to the intended target alongside
`controller_ingress50cx.cpp`, `device_held48y.cpp`, `event_dispatch48z.cpp` and
`player_events49a.cpp`, with their existing transitive input dependencies.
No build target has been changed in this patch.

Retain these caller-owned objects at stable addresses:

- One50CX ingress initialized with actual complete prior button records
- An event array and its `InputEventQueue48Z`; capacity is a caller-selected
  native bound, not a recovered original queue size
- One `ControllerActions50EC(ingress, queue, deviceIndex)` constructed before
  the next poll; `deviceIndex` must be in0..7
- Actual live event context and record spans, plus the application's root route
- A selected49A player-child binding, its live player/environment and bindings
- Real current action/tuning rows and synchronous effect/held-source providers

For a one-player root route, populate the actual
`JoystickApplicationBridge48Z` root flags, child storage/links and visit budget.
Populate `PlayerEventChildBinding49A.childIndex`, `.player`, `.environment` and
`.bindings`; its nested player child route and budget are also explicit inputs.
Then call `bindControllerPlayerActions50EC` with the same ingress/device index,
record sources, application, player binding and output manager callbacks.

The binder changes only:

- `records.devices[deviceIndex]` to the ingress's live32-record bank
- `player.environment->held.devices[deviceIndex]` to that same bank
- The application's child callback/context to `playerEventChild49A` and this
  specific caller-owned child binding
- Manager release/press callbacks to the existing48Z joystick bridges

No flags, topology, player fields, source indices, held-query context, action
codes or callback effects are defaulted. A different eligible root child is
unavailable through the one-child49A binding. Multi-player/mixed-child routing
needs a real caller-provided multiplexer/manager callback set instead; pass that
set to `dispatch` without using this one-player binder.

The event-context presence/mask, held source's current-context presence/masks,
application flags and ingress reader query context are views of caller-owned
live source state. Keep them coherent at the actual source entry points and
after callbacks that change them. The adapter cannot infer their ownership from
matching initial values and does not take stale context snapshots for dispatch.

`player.source5C`, `player.indexFFC`, the event's selected device index and the
reader's selected device remain explicit separate identities. Binding a bank
does not overwrite any of those selectors. Action rows may use the caller-owned
`playerEventPresetRows49D` projection, refreshed after preset changes, while the
frame reader uses `resolveControlPreset49D`'s current action codes.

## Per-poll lifecycle and queue handling

1. At the existing hardware-poll point call `actions.samplePacket(...)` instead
   of `ingress.samplePacket(...)`. It supplies its selected device index to50CX.
   On `Ok`, inspect `ingress.deviceSample().status`; NotReady and ReadFailed are
   valid samples with no synthetic release events. On rejection the previous
   ingress sample, queue and adapter phase are preserved.
2. Call `appendSample`. Queue rejection leaves the entire batch pending and the
   queue unchanged. Repair capacity/storage while retaining old pending events,
   then retry this append. Do not poll again, clear the old queue or truncate the
   sample to make it fit. Success enters `Queued`; repeated append is rejected.
3. At the actual event-manager dispatch point call `dispatch(context, records,
   callbacks)`. It checks that queue pointer/capacity/count still match the
   appended queue and that the selected device span is the live ingress bank.
   A preflight rejection has `dispatchAttempted=false` and leaves `Queued`.
   Fix the binding/restore the lease before retrying; no callbacks ran.
4. Once `dispatchAttempted=true`, the48Z result is authoritative. `Complete`
   enters the adapter's `Complete` phase. Every other native result enters
   `Blocked`, even with zero completed events. Inspect `lastDispatch()` before
   deciding recovery; a subsequent rejected attempt cannot overwrite it.
5. On a complete attempt call `acknowledgeComplete`. This explicit caller action
   resets only queue count and permits another sample. It rejects changed queue
   metadata instead of clearing newly introduced work. Event bytes are retained.
   It is bookkeeping at the caller's chosen boundary, not a recovered scheduler
   prepass or a claim that original input runs once per simulation frame.

The queue may contain preceding undispatched events on entry. Append preserves
their order and dispatch includes them. The owner must not pass a queue whose
prefix already ran, or add another device poll while this adapter holds it.
This is deliberately a one-device lease, not a new global multi-device manager.
Do not bypass it by sampling50CX directly, replacing event bytes or destroying
owners; the API cannot detect arbitrary out-of-band mutations to borrowed memory.
Events, banks, topology, configuration and adapter bookkeeping must use distinct
valid typed storage. All calls and callbacks are synchronous and serial.

During48Z dispatch, its existing callback contract still applies: callbacks may
retarget codes/devices, change records/context, append within the fixed capacity,
or shrink count. Dispatch re-reads those values as before. It selects category
once per event and may run press after release if release changed transition.
The adapter does not freeze these values or undo a caller callback's deliberate
count change. Native `Complete` therefore means the actual live48Z loop finished,
not that every initially appended slot necessarily invoked an action. Queue
pointer/capacity replacements remain48Z errors with partial effects preserved.

After an unavailable/partial dispatch there is no safe generic resume cursor:
the incomplete event can already have invoked release, press, queries, direct
stores and synchronous external effects. `eventsCompleted` does not authorize
blindly replaying that event or the whole queue. Recovery must retain pending
work and resolve produced effects explicitly outside this adapter. Only after
that recovery owner has intentionally set `queue.count=0` may it call
`acceptExternalRecovery`. That method resets no data and runs no effects. It
also accepts a terminal `Complete` phase when external metadata changes require
explicit recovery instead of ordinary acknowledgement. Unattempted batches
cannot be released through this method.

## Action availability and the frame49W boundary

49A's actions are selected by three ordered held queries on every player press,
not by comparing the incoming edge code with a boost/repair/reset label. In
particular, holding the configured action while another button produces an edge
can reach that action again. This source behavior is not converted to invented
one-shot or per-button semantics.

- The configured category2 query reaches `Function2255B0` only under its existing
  player/application gates. Its larger reposition/path/reset dependencies are
  not implemented here. A null effect provider remains `MissingEffectBinding`;
  a supplied provider returning false remains `EffectUnavailable`. The two
  following49A timer stores are not a replacement for that helper.
- The code1A category0 query can reach one of Virtual94/90/8C/88. The code12
  category0 query can reach22B170,22B370,22B570,22B760 and22BA60. Their existing
  synchronous bindings remain mandatory when reached. No boost or reset meaning
  is inferred from their names, signatures or source-proven call order.
- Present category0/1 held sources require actual source query implementations.
  An absent context is a supplied source state, never a convenience fallback to
  avoid an unavailable query. A handler may legitimately miss all action gates.
- Diagnostics remain available in the native manager result, application's
  `lastRoute`, and the reached player binding's `lastResult`. Nested results are
  meaningful only if that route/child was invoked in this attempt; an untouched
  older nested result is not evidence of a new action. Handled/original callback
  return values do not stop the48Z manager's remaining events.

Repair already has a different source-supported route. The current48X reader
queries the selected `flag69C`, gates it with application state and canonical
damage, and writes B9C bit1. The staged49W owner then supplies that bit, actual
frame factor, authored `repairCF4`, component presence and damage/timer owners to
`repairVehicleImpacts50A` at `PostImpactConsumers`.50EC does not call that repair
stage from an edge callback, toggle the bit itself or subtract guessed damage.

Continue `ingress.prepareFrameReader(...)` and `readVehicleFrame49W` exactly at
their actual Ready49I/frame entry, consuming the borrowed reader before another
poll/preparation.49W continues to derive its canonical damage/gate/prior-flag
context. It still requires its actual post-player and base-stage completion.
49A event state is not a substitute49W frame or reset snapshot. A real effect
provider must operate on the correct player/frame owners and preserve their
source scheduling constraints. Dispatch cannot roll back an accepted poll or
external effects when a subsequent vehicle stage rejects.

## Bounds and verification limits

The adapter adds no allocation, worker, recursion or hidden queue storage.
Appending handles at most the existing32 transitions.48Z dispatch is bounded by
the fixed caller capacity while callbacks return; its routers retain explicit
visit budgets. The supplied callbacks' costs, allocations and correctness remain
their owners' responsibility. No whole-frame cost or target memory claim was
measured. A callback cannot safely destroy/rebind borrowed storage merely because
the adapter guards some metadata or rejects reentrant lifecycle calls.

Only source inspection was performed. The new methods still require build and
targeted normal/sanitized lifecycle coverage under the root owner's permitted
test cadence, including capacity failure, repeated append/dispatch, unavailable
effects, zero transitions, live record auxiliaries, callback queue mutations and
recovery acknowledgement. No test was run by this change. Physical button
mapping, live platform wiring, frame/event scheduling, complete actions and
playability are not established. Existing50DU,49C and48U sources are untouched.
