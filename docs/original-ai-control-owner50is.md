# 50IS: source-mode AI speed/control owner adapter

## What the adapter closes

This source-only header connects the staged original speed/brake component to
existing native cold handling, shared input shaping/filtering, and engine/aero
control binding. It does not replace the current driver or broaden the current
vehicle-frame admission. It has not been built, syntax-checked, tested, or run.

There are two distinct owners to preserve:

1. Cold model parameters: original AI constructor16E288 sets vehicle58=1.
   The existing49N authoring binder's AI branch retains the full authored aero
   steering coefficient and actual left/right engine-position torque bases.
   The current player-preset4 branch instead halves aero steering and derives
   yaw bases from it. A runtime mode flip after cold binding is insufficient.
2. Runtime input/control state: AI writes raw steering B5C, throttle B7C,
   brake B80 and flags B9C; common217DE0 bypasses the player cosine shaping
   when vehicle58!=0, but still computes D28 smoothing, quantization and rate
   filtering. Common2181C4 then selects its action/steering branch, consuming
   B7C/B80 directly. The current HOST preset4 route consumes analog channels.

`ai_speed_controls50is.h` composes those already-native operations without
copying their formulas into another physics model.

## Native APIs and ownership

`bindAiColdControls50IS` requires an already-established vehicle58=1 and
matching source-resolved selection.mode58Nonzero. It binds this racer's real
PHY, source class/profile choices and actual previous parameter owners through
49N, then projects them into the existing coupled-body parameter structure.
This is a cold parameter result, not permission to convert a live player into
an AI racer. Install it only in a genuine AI cold setup, alongside its original
cooling, repair, model, navigation and remaining constructor owners.

`prepareAiSpeedControls50IS`:

- Requires a genuine AI owner and ordinary dispatcher state0,2 or3.
- Requires canonical and supplied B9C bit7 to show first-base217440
  initialization already completed; this adapter does not execute that stage.
- Consumes already-completed original navigation, state, packing, repair and
  direction outputs. It does not invent those outputs or produce AI navigation.
- Uses the exact target selector from50IR. Canonical applicationByte58 and
  impactFlagsCD8 override duplicated input flags.
- Reads cachedSpeed34, vehicleState40, damage7D0/7D4 and persistent raw/control,
  D28, filter and model-binding storage from the native frame's actual owners.
- Saves prepared49I elapsed factor supplied at the source AI scheduling point.
  It does not derive simulation time from wall clock or assume .016 seconds.
- Stops at `AwaitingBoost`, without modifying the frame or the source AI state.

`resolveAiSpeedBoost50IS` requires a real native173180 provider. A missing
provider returns `MissingBoostOwner`; it never substitutes "boost declined".
The provider must update its real cooldown109C and the shared308030 random
owner if their original branches run, and return actual action/flag changes.
A concrete source-only provider is now staged as50IT. It requires the actual
cross-racer shared RNG explicitly. Current HOST frame.noise is snapshotted and
XOR-seeded per racer, so it is not sufficient proof of that original owner;
a new per-AI seed is not allowed. Heat is already represented by BoostState50FK
heat7C, but that fact alone does not complete173180 or the AI thermal update.
The provider must preserve the canonical body/cache/control/damage records.

A callback returning false means incomplete execution, not a declined boost.
The continuation becomes `BoostIncomplete` before invocation and cannot replay
it automatically. Resolve that exact native owner's boundary before further
work. A successful callback records its result before the control binder runs,
so a later numeric binding failure cannot cause a second random draw.

`commitAiSpeedControls50IS`:

- Refuses an owner whose cached speed, state, damage, control histories or
  model bindings changed while the decision was pending.
- Calls173770 only when the completed source173180 returned zero.
- Calls existing `stepAndBindControlInputs48W` with true AI mode derived from
  actual vehicle58=1, never by writing vehicle58.
- Retains D28 and filtered history, uses actual cold rates/control parameters,
  then commits action/raw storage and the genuine engine/aero binding result.
- Writes only main.controls and main.components.bindings, matching the
  existing shared-control layout. Engine force integration remains separate.
- Does not change frame phases, body velocity, pose, physics parameters,
  navigation, race progress, scene state or clocks.

Under ordinary vehicle state0, the existing48W brake branch now receives
B7C=0/B80=1. It produces engine drive0/0, global aero brakeEC=1 and source
steering-dependent side aero drives. It does not simulate braking by lowering
a target or weakening the propulsion coefficient.

## Minimal genuine owner set still required

Cold:
- A real AI-subtype owner, vehicle58=1 from16E260 constructor, with shared base
  vehicle initialization complete. Its current node/track must come from the
  actual AI construction/assignment; native player route indices are not an
  interchangeable initializer.
- Its real PHY and source class/profile selection, bound in AI mode before use.
- Shared raw/action/flags, D28, filter, model-binding and body-cache histories
  from common initialization; no zero-filled replacement at each tick.
- AI profile MyMinSpeed108C loaded via16E7F4..16E804 (constructor default0 only
  where the original key lookup really preserves it), stateFE8 and packing
  referenceFF8. Constructor FFC=1 is only its initial value.
- Cooldown109C starts at6 in16E350; subsequent original scheduling owns it.

Per source AI update:
- Native navigation16FFE0, state16EB00, packing171C60, repair1734C0, and the
  current state's original direction planner. These remain missing joins.
- Same actual FF4 track and C80 node speed/flag, live packing scaleFFC and AI
  stateFE8. For the gated RacePace branch only, actual CCC/CB0/CC4, track3C,
  loaded RacePace7C and settings query188F60 result.
- 173180's live heat7C, signed cached speed34, raw steering B5C,1038/103C
  owners, FE8/target1008 state, probe348+10C, cooldown109C, current node's
  binding, and the already-shared308030 RNG. The pure2A2580 accessor is called
  by173180, but its BoostScale return does not enter the shown probability
  comparison. Do not invent a BoostScale multiplier. A native arithmetic
  implementation still needs these genuine owner links and source ordering.
- Actual prepared49I frame factor and a valid source base-control phase.

Some fields already exist in the native renderer/physics prototype. Their
existence does not prove that all AI-specific setters or call ordering exist.

## Remaining frame-level blocker

Active `vehicle_frame49w.cpp` has an explicit player-only admission:
- `admit` rejects any nonzero fields.vehicle58 before every frame phase.
- `readVehicleFrame49W` always invokes DualShock conditioning/player input.
- `bindVehicleFrame49W` rejects mode58Nonzero and requests Nonlinear shaping.
- The client calls player-specific navigation and player-subtype boost logic.
- `updateChariotLifecycle49V` separately rejects input.vehicle58!=0
  (chariot_lifecycle49v.cpp:310). `static_impact49y.cpp:42` independently
  rejects AI mode. These are additional audited mode boundaries, not permission
  to remove their guards.

This adapter intentionally changes none of those restrictions. A genuine AI
frame entry and complete source-specific continuations must be admitted after
reviewing each affected mode-dependent subsystem. Removing the one vehicle58
guard, calling the player reader first, forcing phases, or copying prepared
control output into a player racer would conceal the missing source path.

Therefore the stage closes a concrete cold/control-output connection, but is
not yet a usable full-race AI integration. The active GC candidate remains
separate and is not modified by this stage. The next useful source work is the genuine AI lifecycle
and scheduling boundary, especially navigation/state/packing and genuine
173180 owner links (the native decision/provider now exists in50IT); more
HOST speed caps do not close that gap.

## Verification boundary

Static source only. Same authenticated SLUS executable as50IR. Exact constructor,
shaping and binder PCs are referenced above and in the staged evidence. Strict
binary32, normal-or-zero arithmetic and source-state lifetime requirements from
50IR apply. Keep cold descriptors immutable and all storage disjoint/serial.
No tests, builds, syntax checks, runtime probes, importer execution, original
instruction execution, emulation or active checkout edits were performed.
